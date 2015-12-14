/****************************************************************************

	Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
	StackConfiguration.h  -  defines configuration structure

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating
	1.3  27-apr-00 npm  - fixed mismatched endif
	1.4  17-jan-01 mr01 - added detection of remote reset (BC01-S-010-e)
	                      commented out unused LinkEstablishmentTimeout

*/

#ifndef _STACKCONFIG_H_
#define _STACKCONFIG_H_

#ifdef	RCS_STRINGS
static  char    stackconfiguration_h_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/stackconfiguration.h#1 $";
#endif


#include "common/types.h"
#include "common/bool.h"
#include "bcsp_linkage.h"

/*
STRUCTURE
	StackConfiguration

FUNCTION
	Used to marshall all the information required to configure
	a BCSP stack
*/

struct _BCSPStack ;
typedef struct _StackConfiguration
{
//	uint32 LinkEstablishmentTimeout ;
	uint32 resendTimeout ;
	uint32 retryLimit ;
	uint32 TShy ;
	uint32 TConf ;
	uint16 confLimit ;
	uint8 windowSize ;
	bool useCRC ;
	bool useLinkEstablishmentProtocol ;


} StackConfiguration ;

BCSP_API StackConfiguration * getDefaultStackConfiguration() ;
BCSP_API StackConfiguration * getStackConfiguration(struct _BCSPStack * stack) ;
BCSP_API void setStackConfiguration(struct _BCSPStack * stack,StackConfiguration * cfg) ;

#endif
