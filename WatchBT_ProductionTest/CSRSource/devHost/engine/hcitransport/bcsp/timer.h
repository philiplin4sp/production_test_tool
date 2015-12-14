
/****************************************************************************

	Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
	Timer.h  -  defines timer functionality

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating

*/

#ifndef _TIMER_H_
#define _TIMER_H_

#ifdef	RCS_STRINGS
static  char    timer_h_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/timer.h#1 $";
#endif


#include "bcsp_signal.h"

struct _BCSPStack ;

/*
STRUCTURE
	Timer

FUNCTION
	Provides a 'alarm clock' functionality for tasks.  A task can 
	set a timer to expire in 'n' milliseconds then wait for it to occur

FIELDS
	sig - used to signal waiting task 
	wakeupTime - the time when this timer should wake up the waiting task(s)
*/

typedef struct _Timer
{
	Signal sig ;
	uint32 wakeupTime ;
} Timer ;


extern void SYNCBLKwaitForTimer(struct _BCSPStack * stack,Timer * timer,uint32 delay) ;
extern void checkAlarm(struct _BCSPStack * stack,Timer * timer,uint32 timeNow) ;
extern uint32 wrapTime(struct _BCSPStack * stack,uint32 time) ;
extern void resetTimer(struct _BCSPStack * stack,Timer * timer,uint32 delay) ;

#endif


