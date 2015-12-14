/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        scheduler.c  -  the scheduler

MODIFICATION HISTORY
	1.3  27-apr-00 npm  - added boilerplating

*/


#ifdef	RCS_STRINGS
static  char    scheduler_c_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/scheduler.c#1 $";
#endif

#include <setjmp.h>
#include <stdlib.h>
#include "bcspstack.h"
/*

  Import the tasks and assemble them into an array

*/

extern void MUXSender(BCSPStack* ) ;
extern void SequenceSender(BCSPStack*) ;
extern void Receiver(BCSPStack*) ;
extern void SlipSender(BCSPStack * stack) ;
extern void SLIPReceiver(BCSPStack * stack) ;
extern void RequestRouter(BCSPStack * stack) ;
extern void PacketDeliverer(BCSPStack * stack);
extern void Resender(BCSPStack * stack) ;
extern void LinkEstablishmentSender(BCSPStack * stack) ;
extern void LinkEstablishmentReceive(BCSPStack * stack) ;


static void (*taskList[])(BCSPStack*)  = 
{
	Resender,
	MUXSender,
	SequenceSender,
	Receiver,
	SlipSender,
	SLIPReceiver,
	RequestRouter,
	PacketDeliverer,
	LinkEstablishmentSender,
	LinkEstablishmentReceive,
} ;

/****************************************************************************
NAME
	grabUserTransferRequests

FUNCTION
	utility function to move user transfer requests from their queues
	to the scheduler queued.

IMPLEMENTATION NOTE
	this needs to be done inside a critical section because the user can
	mess around with the transfer-request queues at any moment

****************************************************************************/

void grabUserTransferRequests(BCSPStack * stack)
{
	BCTransferRequest * req ;

	stack->environment->enterCS(stack->environment->envState) ;

	while ((req = (BCTransferRequest*) schedGetNodeFromQueue(&stack->incomingReadRequests)) != NULL)
	{
		BTRACE2(SCHEDULER,"Scheduler adding read-request %p chan %d\n",req,req->channel) ;
		addTransferRequestToQueue(stack,&stack->readRequests[req->channel],req) ;
	}

	while ((req = (BCTransferRequest*) schedGetNodeFromQueue(&stack->incomingWriteRequests)) != NULL) 
	{
		BTRACE2(SCHEDULER,"Scheduler adding write-request %p chan %d\n",req,req->channel) ;
		addTransferRequestToQueue(stack,&stack->requestRouterInput,req) ;
	}
	stack->environment->leaveCS(stack->environment->envState) ;
}


/****************************************************************************
NAME
	scheduler

FUNCTION
	The heart of the stack - calls all available tasks until there are 
	none left then returns an indication of the time it expects to be 
	woken up.

IMPLEMENTATION NOTE
	This is a simple cooperative STREAMS scheduler 
	where tasks parasitically use the scheduler stack.  
	This of course means that they must run to completion.
	The scheduler expects to be woken up by the environment to handle 
	incoming bytes and timer things.
	Task blocking is handled by setting up a longjmp back to the scheduler
	routine and then moving on to the next task.  Blocked tasks are 
	identified by setting their id bit in the stacks waitmask field.

****************************************************************************/

uint32 scheduler(BCSPStack * stack,uint32 timeNow)
{
	int numtasks ;
	uint16 allidle ;
	numtasks = sizeof(taskList) / sizeof(bool (*)(BCSPStack*)) ;
	allidle = (1<<numtasks) -1 ;
	stack->timeNow = timeNow ;
	stack->wakeupTime = wrapTime(stack,stack->timeNow + 10000) ; //wait 10 seconds by default - watch out for wraparound
	
	//check to see whether any of the tasks waiting on a timer event
	//might be free to run now
	checkAlarm(stack,&stack->resendTimer,timeNow) ;
	checkAlarm(stack,&stack->LinkEstablishmentTimer,timeNow) ;


	//now add any transfer requests that have come in....
	grabUserTransferRequests(stack) ;
	
	// Now actually run the stack

	BTRACE2(SCHEDULER,"---->>>>Scheduler starting with waitmask %x time %d\n",stack->waitMask,stack->timeNow) ;
	
	while (stack->waitMask != allidle) 
	{
		int t ;
		uint32 id = 1 ;
		for (t = 0 ; t<numtasks ; t++) 
		{
			//this runs only the tasks which are not idle
			if (!(stack->waitMask & id))
			{
				//setup the return context
				if (!setjmp(stack->schedulerState))
				{
					//call the current task 
					stack->currentTaskId = id ;
					taskList[t](stack) ;
				}
			}
			id = id <<1 ;
		}
	
	}	
	BTRACE2(SCHEDULER,"<<<<----Scheduler completed loop - waitmask %x sleeping until time %d\n",stack->waitMask,stack->wakeupTime) ;
	return stack->wakeupTime ;
}

/****************************************************************************
NAME
	immediateBlock

FUNCTION
	Blocks the current task after setting both the schedulers waitmask
	and the waitmask of the queue/signal that wants the block

IMPLEMENTATION NOTE

****************************************************************************/

void immediateBlock(BCSPStack * stack,uint16 * waitMask)
{
	*waitMask |= stack->currentTaskId ;
	stack->waitMask |= stack->currentTaskId ;

	//now do longjmp
	longjmp(stack->schedulerState,stack->currentTaskId) ;
}

/****************************************************************************
NAME
	deferredBlock

FUNCTION
	Performs a 'deferred' block.  Ie, the task is marked as blocked but
	allowed to continue execution so it can try other things.

IMPLEMENTATION NOTE

****************************************************************************/

void deferredBlock(BCSPStack * stack,uint16 * waitMask)
{
	//just don't bother returning 
	*waitMask |= stack->currentTaskId ;
	stack->waitMask |= stack->currentTaskId ;
}

/****************************************************************************
NAME
	unblock

FUNCTION
	unblocks all tasks waiting on the supplied waitmask.  This is 
	accomplished by clearing the appropriate bits in the stack waitmask

IMPLEMENTATION NOTE

****************************************************************************/

void unblock(BCSPStack * stack,uint16 *waitMask) 
{
	//clear wait bits for the tasks that have registered a block on this
	//mask
	stack->waitMask |= *waitMask ; 
	stack->waitMask ^= *waitMask ; 
	*waitMask = 0 ;
}

/****************************************************************************
NAME
	block

FUNCTION
	causes a forced block for the current task - typically called after
	a deferred block.

IMPLEMENTATION NOTE

****************************************************************************/

void block(BCSPStack * stack)
{
	stack->waitMask |= stack->currentTaskId ;
	//now do longjmp
	longjmp(stack->schedulerState,stack->currentTaskId) ;
}


/****************************************************************************
NAME
	isStackIdle

FUNCTION
	called by environment to detect whether the stack is idle or can 
	do some more work.

IMPLEMENTATION NOTE

****************************************************************************/


bool isStackIdle(BCSPStack * stack) 
{
	uint16 allidle,numtasks ;
	numtasks = sizeof(taskList) / sizeof(bool (*)(BCSPStack*)) ;
	allidle = (1<<numtasks) -1 ;
	return  (stack->waitMask == allidle) ; 
}
