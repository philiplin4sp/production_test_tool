/****************************************************************************

	Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
	SLIP.h  -  defines state required by slip sender and receiver

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating

*/

#ifndef _SLIP_H_
#define _SLIP_H_


#ifdef	RCS_STRINGS
static  char    slip_h_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/slip.h#1 $";
#endif


typedef enum
{
	SLIPaboutToSendStart=0,
	SLIPsendingHeader,
	SLIPsendingData,
	SLIPsendingCRC,
	SLIPaboutToSendEnd,
	SLIPsendEnd,
	SLIPunsynchronised=0,
	SLIPRCVHeader,
	SLIPRCVData,
	SLIPRCVCRC,
	SLIPexpectEndOfData,
	SLIPSendPacket
} SLIPmachineState ;

typedef struct
{
	//Transmit state...
	SLIPmachineState XMITState ;
	uint8 * XMITdata ;
	uint32 XMITheader ; //this is a network-order rendition of the packet header
	uint16 XMITbytesRemaining ;
	uint16 XMITcalcCRC ;

	//receive state 
	SLIPmachineState RCVState ;
	Packet * RCVpkt ;
	uint8* RCVdata ;
	uint32 RCVheader ; 
	uint16 RCVCRC ;
	uint16 RCVbytesRemaining ;
	uint16 RCVcalcCRC ;


} SLIPState ;



#endif
