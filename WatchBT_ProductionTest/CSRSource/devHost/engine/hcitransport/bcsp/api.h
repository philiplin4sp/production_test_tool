
/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        api.h  -  make bcsp-level packet services public

CONTAINS

BCSPwritePacket - write a bcsp packet to a channel
BCSPreadPacket	- read a bcsp packet from a channel
BCSPopenChannel - open a bcsp channel
BCinitTransferRequest - initialise a transfer request
BCreloadTransferRequest - reload a transfer request 

DESCRIPTION

COMMENTS

The transfer-request functions are deliverately prefixed with BC since they are 
actually BCSP independent

MODIFICATION HISTORY
	1.5  27-apr-00 npm  - added boilerplating

*/



#ifndef _API_H_
#define _API_H_

#ifdef	RCS_STRINGS
static  char    api_h_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/api.h#1 $";
#endif



#include "transferrequest.h"



struct _BCSPStack ;

/*
STRUCTURE
	BCSPChannel

FUNCTION
	Encapsulate all required information to identify a channel uniquely

FIELDS
	stack - the stack to which this channel belongs
	channel - the channel index 
	
*/
typedef struct 
{
	struct _BCSPStack * stack ;
	uint8 channel ;

}  BCSPChannel ;




BCSP_API  void BCSPwritePacket(BCSPChannel * chan,BCTransferRequest * req) ;
BCSP_API  void BCSPreadPacket(BCSPChannel * chan,BCTransferRequest * req) ;
BCSP_API bool BCSPopenChannel(BCSPChannel * chan,struct _BCSPStack * stack,uint8 chanNum,BCSPChannelType type,BCSPCallbackType callbackType);
BCSP_API void BCinitTransferRequest(struct _BCTransferRequest * req,uint8 * data,uint16 len,void (*callback)(BCTransferRequest*,BCTransferStatus,void*),void * callbackParameter) ;
BCSP_API void BCreloadTransferRequest(struct _BCTransferRequest * req,uint8 * data,uint16 len) ;



#endif
