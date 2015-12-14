/****************************************************************************

	Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
	Shutdown.h  -  exports shutdown/cleanup functionality

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating

*/

#ifndef _SHUTDOWN_H
#define _SHUTDOWN_H

#ifdef	RCS_STRINGS
static  char    shutdown_h_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/shutdown.h#1 $";
#endif

#include "common/linkage.h"

BCSP_API void BCSPshutdownStack(BCSPStack * stack) ;


#endif
