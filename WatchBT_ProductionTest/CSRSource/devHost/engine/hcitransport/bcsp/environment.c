/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        Environment.c  -  external environment services for stack

MODIFICATION HISTORY
	1.3  27-apr-00 npm  - added boilerplating

*/


#ifdef	RCS_STRINGS
static  char    environment_c_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/environment.c#1 $";
#endif

#include "bcspstack.h"
#include <stdlib.h>

//the stack calls allocMem and freeMem to allocate data-sections for
//packets.  There can never be more data-sections allocated than 
//packets are supplied, so one improvement to the default scheme of
//just calling malloc and free would be to provide a fixed-block 
//allocator.  The default scheme also assumes that we'll never fail to 
//allocate the required memory.  If it is expected that the alloc _can_
//fail, we would need to block on the alloc until a free allowed us
//(hopefully) to continue.

static void * defaultAllocMem(void * envState,uint32 size)
{
	void * mem = malloc(size) ;
	return mem ;
}

static void defaultFreeMem(void * envState,void * ptr)
{
	free(ptr) ;
}

//the stack uses the critical section to guard the transfer-request 
//queues - these are the only objects which are subject to thread-
//contention.  The default implementation does nothing and will almost
//certainly need to be replaced in anything other than a toy version.

static void defaultEnterCS(void * envState)
{
}

static void defaultLeaveCS(void * envState)
{
}

//the environments signal is raised to alert it that a transfer
//request has been received and that it should wake up the stack
//the default signalling mechanism is to do nothing

static void defaultSignal(void * envState)
{
}

static void defaultOnLinkFailureSync(void * envState)
{
	//this is pretty serious so the default behaviour is to at least 
	//print out something about it!
	BTRACE0(0xffffffff,"LINK FAILURE(sync packet)\n") ;
}

static void defaultOnLinkFailureRetries(void * envState)
{
	//this is pretty serious so the default behaviour is to at least 
	//print out something about it!
	BTRACE0(0xffffffff,"LINK FAILURE(retries exceeded)\n") ;
}

static void defaultReleasePacket(void * envState,Packet * ptr)
{
	defaultFreeMem(envState,ptr) ;
}

//now set up the default environment

const BCSPEnvironment defaultEnvironment =
{
	defaultAllocMem,
	defaultFreeMem,
	defaultEnterCS,
	defaultLeaveCS,
	defaultSignal,
	defaultOnLinkFailureSync,
	defaultOnLinkFailureRetries,
	defaultReleasePacket,

	0xffffffff, //all bits significant in clock
	null,		//user-defined state	
} ;


BCSPEnvironment * getDefaultEnvironment() 
{
	return (BCSPEnvironment *) &defaultEnvironment ;
}
BCSPEnvironment * getEnvironment(BCSPStack * stack) 
{
	return stack->environment ;
}

void setEnvironment(BCSPStack * stack,BCSPEnvironment * env)
{
	stack->environment = env ;

}
