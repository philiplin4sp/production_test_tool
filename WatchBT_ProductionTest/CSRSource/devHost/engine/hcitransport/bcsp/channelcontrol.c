
/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        ChannelControl.c  -  channel attributes bit-field

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating

*/


#ifdef	RCS_STRINGS
static  char    channelcontrol_c_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/channelcontrol.c#1 $";
#endif


#include "bcspstack.h"

#define LINK_TYPE_BIT 1
#define CALLBACK_TYPE_BIT 2
#define OPEN_BIT 7


#define CALLBACK_TYPE_MASK (1<<CALLBACK_TYPE_BIT)
#define LINK_TYPE_MASK (1<<LINK_TYPE_BIT)
#define OPEN_MASK (1<<OPEN_BIT)


/****************************************************************************
NAME
	openChannel

FUNCTION
	attempts to open a channel with the supplied attributes and set up a 
	channel control to record this.

IMPLEMENTATION NOTE

****************************************************************************/



bool openChannel(struct _BCSPStack * stack,uint8 chan,BCSPChannelType  type,BCSPCallbackType callbacktype)
{
	ChannelControl ctrl = stack->channels[chan] ;
	if (ctrl & OPEN_MASK) return false ;
	ctrl |= OPEN_MASK ;
	ctrl |= (type << LINK_TYPE_BIT);
	ctrl |= (callbacktype <<CALLBACK_TYPE_BIT);
	stack->channels[chan] = ctrl ;
	return true ;
}

/****************************************************************************
NAME
	getCallbackType

FUNCTION
	accessor function to unpack callback type from channel control structure

IMPLEMENTATION NOTE

****************************************************************************/


BCSPCallbackType getCallbackType(struct _BCSPStack * stack,uint8 chan)
{
	ChannelControl ctrl = stack->channels[chan] ;

	if (ctrl & CALLBACK_TYPE_MASK) return BCSPCallbackImmediate ;
	return BCSPCallbackDeferred ;
}

/****************************************************************************
NAME
	getProtocolType

FUNCTION
	accessor function to unpack protocol type from channel control structure

IMPLEMENTATION NOTE

****************************************************************************/

BCSPChannelType getProtocolType(struct _BCSPStack * stack,uint8 chan)
{
	ChannelControl ctrl = stack->channels[chan] ;

	if (ctrl & LINK_TYPE_MASK) return BCSPChannelReliable ;
	return BCSPChannelUnreliable ;
}

