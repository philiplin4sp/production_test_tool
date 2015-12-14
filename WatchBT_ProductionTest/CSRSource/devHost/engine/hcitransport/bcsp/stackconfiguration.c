/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        StackConfiguration.c  -  configuration defaults

MODIFICATION HISTORY
	1.3  27-apr-00 npm  - added boilerplating
	1.4  17-jan-01 mr01 - added detection of remote reset (BC01-S-010-e)
	                      commented out unused LinkEstablishmentTimeout

*/


#ifdef	RCS_STRINGS
static  char    stackconfiguration_c_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/stackconfiguration.c#1 $";
#endif

#include "bcspstack.h"
#include "stackconfiguration.h"

/*
The default configuration with which the stack boots
*/

const StackConfiguration defaultStackConfiguration =
{
//	10000,	//LinkEstablishmentTimeout
	250,	//resendTimeout 
	20,		//retryLimit
	300,	//TShy
	250,	//TConf
	2,		//confLimit
	4,		//windowSize
	true,	//do use CRC 
	true,	//do use linkEstablishment Protocol
} ;


/****************************************************************************
NAME
	getDefaultStackConfiguration

FUNCTION
	returns a pointer to the default stack configuration

IMPLEMENTATION NOTE

****************************************************************************/



StackConfiguration * getDefaultStackConfiguration() 
{
	return (StackConfiguration *) & defaultStackConfiguration ;
}


/****************************************************************************
NAME
	getDefaultStackConfiguration

FUNCTION
	returns a pointer to the current stack configuration

IMPLEMENTATION NOTE

****************************************************************************/

StackConfiguration * getStackConfiguration(BCSPStack * stack) 
{
	return stack->configuration ;
}

/****************************************************************************
NAME
	getDefaultStackConfiguration

FUNCTION
	sets the stack configuration

IMPLEMENTATION NOTE
	The stack does not make a copy of the configuration - it 
	just keeps hold of the pointer.

****************************************************************************/

void setStackConfiguration(BCSPStack * stack, StackConfiguration * config)
{
	stack->configuration = config ;
}
