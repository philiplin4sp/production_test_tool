/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        Timer.c  -  timer services

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating

*/


#ifdef	RCS_STRINGS
static  char    timer_c_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/timer.c#1 $";
#endif

#include "bcspstack.h"
#include "timer.h"


/****************************************************************************
NAME
	isTimeAfter

FUNCTION
	utility function to tell us whether time 1 is after time2

IMPLEMENTATION NOTE
	time1 > time2 is determined by whether the difference is 
	less than half the total clock range (supplied by environment).

****************************************************************************/


bool isTimeAfter(BCSPStack * stack,uint32 time1,uint32 time2)
{
	uint32 halfRange ;
	uint32 diff = time1 -time2 ;
	halfRange = (stack->environment->clockMask >>1) + 1 ; //do shift first
	diff &= stack->environment->clockMask ; //make sure we get rid of any sign-extended bits
	return (diff < halfRange) ;
}

/****************************************************************************
NAME
	isTimeAfter

FUNCTION
	utility function to keep time within bounds set by environent

IMPLEMENTATION NOTE
	it's assumed that times always occupy a whole number of bit positions

****************************************************************************/


uint32 wrapTime(BCSPStack * stack,uint32 time)
{
	return time & stack->environment->clockMask ;
}


/****************************************************************************
NAME
	resetTimer

FUNCTION
	resets the wakeup time for a timer that is already waiting

IMPLEMENTATION NOTE
	if the desired wakeup time is closer than the current stack wakeup time,
	we need to adjust that also

****************************************************************************/

void resetTimer(struct _BCSPStack * stack,Timer * timer,uint32 delay) 
{
	timer->wakeupTime = wrapTime(stack,stack->timeNow +delay) ;
	if (isTimeAfter(stack,stack->wakeupTime,timer->wakeupTime)) 
	{
			//move the stacks wakeup time closer to now
			stack->wakeupTime = timer->wakeupTime ;
	}
}

/****************************************************************************
NAME
	SYNCBLKwaitForTimer

FUNCTION
	waits for 'delay' ms by setting the timer and waiting on it

IMPLEMENTATION NOTE
	if the desired wakeup time is closer than the current stack wakeup time,
	we need to adjust that also

****************************************************************************/

void SYNCBLKwaitForTimer(struct _BCSPStack * stack,Timer * timer,uint32 delay) 
{
	timer->wakeupTime = wrapTime(stack,stack->timeNow +delay) ;
	if (isTimeAfter(stack,stack->wakeupTime,timer->wakeupTime)) 
	{
			//move the stacks wakeup time closer to now
			stack->wakeupTime = timer->wakeupTime ;
	}

	//wait for timer to go off if the timer wakup time is in the future
	if (isTimeAfter(stack,timer->wakeupTime,stack->timeNow)) 
	{
		SYNCBLKwaitSignal(stack,&timer->sig) ;
	}
}


/****************************************************************************
NAME
	checkAlarm

FUNCTION
	checks to see whether a timer has gone off

IMPLEMENTATION NOTE
	this is called from scheduler to check whether the timer has expired and 
	the waiting thread needs to be signalled

****************************************************************************/

void checkAlarm(struct _BCSPStack * stack,Timer * timer,uint32 timeNow)
{
	if (isTimeAfter(stack,timeNow,timer->wakeupTime) ||
		(timeNow == timer->wakeupTime))
	{
		setSignal(stack,&timer->sig) ;
	}
	else 
	{
		if (isTimeAfter(stack,stack->wakeupTime,timer->wakeupTime))
		{
			//move stacks wakeup time closer to now 
			stack->wakeupTime = timer->wakeupTime ;
		}
	}
}
