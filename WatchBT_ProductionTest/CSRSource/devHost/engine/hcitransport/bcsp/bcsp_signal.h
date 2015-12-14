/****************************************************************************

	Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
	bcsp_Signal.h  -  simple signal functionality

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating

*/

#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#ifdef	RCS_STRINGS
static  char    bcsp_signal_h_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/bcsp_signal.h#1 $";
#endif




/*
STRUCTURE
	Signal
FUNCTION
	Provides a way of waiting for some condition to become true.
	The semantics on 'wait' and 'set' are that it is assumed that
	wait will only be called if the condition is false and set will
	only be called when the condition becomes true.  Therefore, 
	waiting on a signal causes the 'set' flag to be cleared, ie 
	waiting on a set signal will _not_ result in an immediate return
	but will require the signal to be set again.  (Ie, signals 
	are edge-triggered)

FIELDS
	waitMask - ids of tasks waiting on the signal.
*/

#include "common/types.h"

struct _BCSPStack ;

typedef struct _Signal
{
	uint16 waitMask ;
} Signal ;

extern void setSignal(struct _BCSPStack * stack,Signal * sig);
extern void SYNCBLKwaitSignal(struct _BCSPStack * stack,Signal * sig) ;
extern void ASYNCBLKwaitSignal(struct _BCSPStack * stack,Signal * sig) ;
#endif
