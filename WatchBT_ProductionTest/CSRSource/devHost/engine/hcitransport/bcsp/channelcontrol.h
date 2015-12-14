
/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        ChannelControl.h  -  defines simple bit field for channel attributes

DESCRIPTION
	A channel control is an internal bit-field maintained by the stack 
	which holds the channel attributes:
	open : yes/no
	callback type: immediate/deferred
	protocol type: reliable/unreliable


MODIFICATION HISTORY
	1.3  27-apr-00 npm  - added boilerplating

*/





#ifndef _CHANNELCONTROL_H
#define _CHANNELCONTROL_H

#ifdef	RCS_STRINGS
static  char    channelcontrol_h_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/channelcontrol.h#1 $";
#endif






struct _BCSPStack ;

typedef enum 
{
	BCSPCallbackDeferred=0,	//uses client data pointer and must wait for ack
	BCSPCallbackImmediate=1 //does memcpy and can return immediately
} BCSPCallbackType ;


typedef uint8 ChannelControl ;

extern BCSPCallbackType getCallbackType(struct _BCSPStack * stack,uint8 chan) ;
extern BCSPChannelType getProtocolType(struct _BCSPStack * stack,uint8 chan) ;
extern bool openChannel(struct _BCSPStack * stack,uint8 chan,BCSPChannelType  type,BCSPCallbackType callbacktype) ;


#endif
