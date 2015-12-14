/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        api.c  -  bcsp-level packet services

MODIFICATION HISTORY
	1.5  27-apr-00 npm  - added boilerplating
	1.6  27-apr-00 npm  - revised modification history

*/


#ifdef	RCS_STRINGS
static  char    api_c_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/api.c#1 $";
#endif



#include "bcspstack.h"
#include "api.h"
#include "channelcontrol.h"



/****************************************************************************
NAME
	BCSPopenChannel

FUNCTION
	opens a bcsp channel on a particular stack.

IMPLEMENTATION NOTE
	channels have a type (reliable/unreliable) and a callback type 
	(immediate/deferred) associated with them.  All packets on the channel
	will inherit these attributes.
	This function will be called in the users context so we protect the 
	channel control word by using the users critical section.

RETURNS
	true if the channel could be opened
	false if the channel is already open

****************************************************************************/



bool BCSPopenChannel(BCSPChannel * chan,struct _BCSPStack * stack,uint8 chanNum,
					 BCSPChannelType type,BCSPCallbackType callbackType)
{
	bool success ;
	BCSPEnvironment * env = stack->environment ;
	//open the channel in a critical section in case someone else tries
	//to open the same channel at the same time.
	env->enterCS(env->envState) ;
	success = openChannel(stack,chanNum,type,callbackType);
	env->leaveCS(env->envState) ;

	//check success and fill in appropriate fields
	if (success) 
	{
		chan->stack = stack ;
		chan->channel = chanNum ;
		return true ;
	}
	return false ;
}

/****************************************************************************
NAME
	userAddTranserRequest

FUNCTION
	assists BCSPread/writepacket by actually adding the transfer request to the 
	desired transfer request queue.

IMPLEMENTATION NOTE
	This function is called in the user's context so it's important to 
	place the queue-manipulation routine inside a critical-section.  The stack
	does likewise when manipulating the transfer queues so we're covered.
****************************************************************************/

void userAddTransferRequest(Queue * queue,BCSPChannel * hChan,BCTransferRequest * req)
{
	BCSPStack * stack ;
	BCSPEnvironment * env ;
	
	stack =hChan->stack ;
	env = stack->environment ;

	//set up the request
	req->status = transferWaiting ;
	req->channel = hChan->channel ;

	//add the request to the input queue; this needs to be in a 
	//critical section because it is called by the
	//user and can happen at any time
	env->enterCS(env->envState) ;
	userAddNodeToQueue(queue,(QueueNode*)req) ;
	env->leaveCS(env->envState) ;

	//we need to prod the stack now in case it's asleep
	env->signal(env->envState) ;

}

/****************************************************************************
NAME
	BCSPwritePacket

FUNCTION
	performs a write operation (described by a transfer request)
	on the given channel.  

****************************************************************************/


void BCSPwritePacket(BCSPChannel * hChan,BCTransferRequest * req)
{
	userAddTransferRequest( & hChan->stack->incomingWriteRequests,hChan,req) ;
}

/****************************************************************************
NAME
	BCSPreadPacket

FUNCTION
	performs a read operation (described by a transfer request)
	on the given channel.  

IMPLEMENTATION NOTE
	This function is called in the user's context so it's important to 
	place the queue-manipulation routine inside a critical-section.  The stack
	does likewise when manipulating the transfer queues so we're covered.
****************************************************************************/


void BCSPreadPacket(BCSPChannel * hChan,BCTransferRequest * req)
{
	userAddTransferRequest( & hChan->stack->incomingReadRequests,hChan,req) ;
}



/****************************************************************************
NAME
	BCinitTransferRequest

FUNCTION
	Initialises a transfer-request.  
	The idea is that the callback info is unlikely to change over the course
	of a long transfer so this should be a one-time	operation whilst 
	reloadTransferRequest should be called for each packet. 

****************************************************************************/

void BCinitTransferRequest(BCTransferRequest * req,uint8 * data,uint16 len,
							 void (*callback)(BCTransferRequest*,BCTransferStatus,void*),
							 void * callbackParameter) 
{
	//set up all the user parameters in one go
	req->data = data ;
	req->length = len ;
	req->callback = callback ;
	req->callbackParam = callbackParameter ;
}

/****************************************************************************
NAME
	BCreloadTransferRequest

FUNCTION
	Initialises just enought of a transfer-request to allow a new packet-transfer.  
	The idea is that the callback info is unlikely to change over the course
	of a long transfer so initialise should be a one-time	operation whilst 
	reloadTransferRequest should be called for each packet. 

****************************************************************************/

void BCreloadTransferRequest(BCTransferRequest * req,uint8 * data,uint16 len)
{
	req->data = data ;
	req->length = len ;
}



