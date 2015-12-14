/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        PacketBuffer.c  -  buffer for packets

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating
$Log: packetbuffer.c,v $
Revision 1.1  2002/03/27 18:35:12  ajh
Moved and renamed from
devHost/HostStack/singledll/

Revision 1.1  2001/06/29 12:56:52  ajh
First adding, temporarily, of a single dll version of the hoststack,
including bluetest as the test app.

Revision 1.3  2001/05/22 15:12:00  cl01
Reworking for CE/Win32 merge


*/


#ifdef	RCS_STRINGS
static  char    packetbuffer_c_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/packetbuffer.c#1 $";
#endif

#include <stddef.h>
#include "packetbuffer.h"
#include "bcspstack.h"

/****************************************************************************
NAME
	addPacketToBuffer

FUNCTION
	wrapper function to add a packet to the buffer

IMPLEMENTATION NOTE
	uses underlying queue functionality

****************************************************************************/


void addPacketToBuffer(struct _BCSPStack * stack,PacketBuffer * buf,Packet * pkt) 
{
	addNodeToQueue(stack,&buf->queue,&pkt->node) ;
}

/****************************************************************************
NAME
	SYNCBLKgetPacketFromBuffer

FUNCTION
	attempts to obtain a packet from the buffer.  If none is available, block
	immediately.

IMPLEMENTATION NOTE
	uses underlying queue functionality

****************************************************************************/


Packet *  SYNCBLKgetPacketFromBuffer(struct _BCSPStack * stack,PacketBuffer * buf) 
{
	Packet * pkt ;
	pkt = (Packet*) SYNCBLKgetNodeFromQueue(stack,&buf->queue) ;
	return pkt ; //this will block rather than return NULL
}

/****************************************************************************
NAME
	ASYNCBLKgetPacketFromBuffer

FUNCTION
	attempts to obtain a packet from the buffer.  If none is available, return
	null and block later.

IMPLEMENTATION NOTE
	uses underlying queue functionality

****************************************************************************/


Packet *  ASYNCBLKgetPacketFromBuffer(struct _BCSPStack * stack,PacketBuffer * buf) 
{
	Packet * pkt ;
	pkt = (Packet*) ASYNCBLKgetNodeFromQueue(stack,&buf->queue) ;
	return pkt ; 
}

/****************************************************************************
NAME
	peekAtPacketInBuffer

FUNCTION
	return pointer to first packet in buffer without actually removing it or
	null if no packet present

IMPLEMENTATION NOTE
	uses underlying queue functionality

****************************************************************************/


Packet * peekAtPacketInBuffer(PacketBuffer * buf) 
{
	return (Packet*) peekAtNodeInQueue(&buf->queue) ;
}

/****************************************************************************
NAME
	SYNCBLKpeekAtPacketInBuffer

FUNCTION
	return pointer to first packet in buffer without actually removing it.
	If no packet present, block immediately

IMPLEMENTATION NOTE
	uses underlying queue functionality

****************************************************************************/

Packet * SYNCBLKpeekAtPacketInBuffer(struct _BCSPStack * stack,PacketBuffer * buf) 
{
	return (Packet*) SYNCBLKpeekAtNodeInQueue(stack,&buf->queue) ;
}

