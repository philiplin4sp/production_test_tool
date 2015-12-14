
/****************************************************************************

	Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
	Environment.h  -  defines environment structure for stack

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating

*/

#ifndef _ENVIRONMENT_H
#define _ENVIRONMENT_H

#ifdef	RCS_STRINGS
static  char    environment_h_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/environment.h#1 $";
#endif

#include "common/types.h"
#include "packet.h"
#include "bcsp_linkage.h"

struct _BCSPStack ;

/*
STRUCTURE
	BCSPEnvironment

FUNCTION
	The 'Environment' is a way to provide helper functions for
	the generic stack so that it can actually do useful things

*/

typedef struct _BCSPEnvironment
{
	void * (*allocMem)(void * envState,uint32 size) ;
	void (*freeMem)(void * envState,void*) ;
	void (*enterCS)(void * envState) ;
	void (*leaveCS)(void * envState) ;
	void (*signal)(void * envState) ;
	void (*onLinkFailureSync)(void * envState) ;
	void (*onLinkFailureRetries)(void * envState) ;
	void (*releasePacket)(void * envState,Packet * pkt) ;
	uint32 clockMask ;	//used to compare times
	void * envState ;	//this is for users to tack on state for the functions

} BCSPEnvironment ;

BCSP_API BCSPEnvironment * getDefaultEnvironment() ;
BCSP_API BCSPEnvironment * getEnvironment(struct _BCSPStack * stack) ;
BCSP_API void setEnvironment(struct _BCSPStack * stack,BCSPEnvironment * env) ;


#endif
