/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        PacketDeliverer.c  -  packet deliverer task

MODIFICATION HISTORY
	1.4  27-apr-00 npm  - added boilerplating
	1.5  28-apr-00 npm  - added data integrity check
	1.6  12-mar-01 at   - safer tracing code
*/


#ifdef	RCS_STRINGS
static  char    packetdeliverer_c_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/packetdeliverer.c#1 $";
#endif

#include <stdlib.h>
#include <memory.h>
#include "bcspstack.h"
#include "api.h"


/****************************************************************************
NAME
	PacketDeliverer

FUNCTION
	Task that waits for packets to rise from the lower levels then delivers them 
	to waiting transfer requests.

IMPLEMENTATION NOTE
	this runs as a task
	The assumption is that higher-level protocols should always provide a large
	enough buffer to receive a whole packet.

****************************************************************************/


void PacketDeliverer(BCSPStack * theStack)
{
	BCTransferRequestQueue * reqQueue ;
	BCTransferRequest * req ;
	uint32 chan ;
	BCTransferStatus status ;
	uint16 copylen ;

	//just peek at the packet since we might need to come back later if there is no
	//transfer request available.
	Packet * pkt = SYNCBLKpeekAtPacketInBuffer(theStack,&theStack->PacketDelivererInput);
	chan = getProtocolId(pkt) ;

	BTRACE3(PKTDLVR,"Packet deliverer - rcvd packet %p len %d chan %d\n",pkt,getLength(pkt),chan) ;
	//try and get a matching transfer request for the packet
	reqQueue = &theStack->readRequests[chan] ;
	req = SYNCBLKgetTransferRequestFromQueue(theStack,reqQueue) ;
	
	BTRACE4(PKTDLVR,"Packet deliverer - delivering packet %p len %d chan %d req len %d\n",pkt,getLength(pkt),chan,req->length) ;

	status = transferComplete ;
	copylen = getLength(pkt) ;
	if (copylen > req->length)
	{
		//oops - the recipient didn't reserve enough space - we'll copy 
		//what we can but signal an error
		copylen = req->length ;
		status = transferBufferTooSmall ;
	}

	BTRACE3(PKTDLVR,"Packet deliverer - copying data to %p from %p len %d\n",req->data,pkt->data,copylen) ;
	memcpy(req->data,pkt->data,copylen) ;

	if (pkt->data)
	{
		if (getLength(pkt) < 4) BTRACE1(DATAINTEGRITY,"PD pkt len %d\n",getLength(pkt)) ;
		else if (getLength(pkt) < 8) BTRACE2(DATAINTEGRITY,"PD pkt len %d data %x\n",getLength(pkt),*(uint32*)(pkt->data)) ;
		else BTRACE3(DATAINTEGRITY,"PD pkt len %d data %x %x\n",getLength(pkt),*(uint32*)(pkt->data),*(uint32*)(pkt->data+4)) ;
	}

	//this won't actually block
	SYNCBLKgetPacketFromBuffer(theStack,&theStack->PacketDelivererInput) ;

	req->length= copylen ;//indicate to client how much data was copied
	doCallback(req,status) ; //call client

	//free packet and data 
	returnPacketToPool(theStack,pkt,modeRCV) ;
	return  ;
}


