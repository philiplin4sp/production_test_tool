/****************************************************************************

	Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
	Scheduler.h  -  exports scheduler (un)blocking functionality

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating

*/

#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#ifdef	RCS_STRINGS
static  char    scheduler_h_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/scheduler.h#1 $";
#endif

#include "common/types.h"
#include "common/linkage.h"
#include "common/bool.h"

struct _BCSPStack ;

BCSP_API uint32 scheduler(struct _BCSPStack * theStack,uint32 timeNow);
BCSP_API bool isStackIdle(struct _BCSPStack * stack) ;

extern void immediateBlock(struct _BCSPStack * stack,uint16 * waitMask);
extern void unblock(struct _BCSPStack * stack,uint16 *waitMask) ;
extern void deferredBlock(struct _BCSPStack * stack,uint16 * waitMask) ;
extern void block(struct _BCSPStack * stack) ;

#endif
