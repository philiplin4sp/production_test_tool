/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        Signal.c  -  signalling services

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating

*/


#ifdef	RCS_STRINGS
static  char    signal_c_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/signal.c#1 $";
#endif


#include "bcspstack.h"
#include "bcsp_signal.h"
#include "scheduler.h"

/****************************************************************************
NAME
	setSignal

FUNCTION
	wakes up any tasks waiting on this signal

IMPLEMENTATION NOTE

****************************************************************************/


void setSignal(struct _BCSPStack * stack,Signal * sig)
{
	unblock(stack,&sig->waitMask) ;
}

/****************************************************************************
NAME
	ASYNCBLKwaitSignal

FUNCTION
	waits for a signal to be set but allows task to continue so it can
	try other things as well

IMPLEMENTATION NOTE

****************************************************************************/

void ASYNCBLKwaitSignal(struct _BCSPStack * stack,Signal * sig) 
{
	deferredBlock(stack,&sig->waitMask) ;
}

/****************************************************************************
NAME
	SYNCBLKwaitSignal

FUNCTION
	waits for a signal to be set.Blocks immediaately

IMPLEMENTATION NOTE

****************************************************************************/

void SYNCBLKwaitSignal(struct _BCSPStack * stack,Signal * sig) 
{
	immediateBlock(stack,&sig->waitMask) ;
}


