/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        Packet.c  -  packet services

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating
$Log: packet.c,v $
Revision 1.1  2002/03/27 18:35:11  ajh
Moved and renamed from
devHost/HostStack/singledll/

Revision 1.1  2001/06/29 12:56:52  ajh
First adding, temporarily, of a single dll version of the hoststack,
including bluetest as the test app.

Revision 1.3  2001/05/22 15:12:51  cl01
Reworking for CE/Win32 merge


*/


#ifdef	RCS_STRINGS
static  char    packet_c_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/packet.c#1 $";
#endif

#include <stddef.h>
#include "packet.h" 




#define GET_BITS(var,pos,size) (((var) >> (pos)) & ((((uint32)1)<<(size))-1))

#define CLR_BITS(var,pos,size) (var) ^= (GET_BITS((var),(pos),(size)) << (pos)) 

#define SET_BITS(var,data,pos,size) CLR_BITS((var),(pos),(size)) ;\
								(var) |= ((((uint32)data) & ((1<<(size))-1)) << (pos))


/****************************************************************************
NAME
	getAck

FUNCTION
	unpacks the acknowledgement number from the packet header

IMPLEMENTATION NOTE

****************************************************************************/

uint8 getAck(Packet * pkt)
{
	return (uint8) (GET_BITS(pkt->header,3,3)) ;
}

/****************************************************************************
NAME
	setAck

FUNCTION
	packs the acknowledgement number into the  packet header

IMPLEMENTATION NOTE

****************************************************************************/

void setAck(Packet * pkt,uint8 n )
{
	SET_BITS(pkt->header,n,3,3) ;
}
/****************************************************************************
NAME
	getSeq

FUNCTION
	unpacks the sequence number from the packet header

IMPLEMENTATION NOTE

****************************************************************************/

uint8 getSeq(Packet * pkt)
{
	return (uint8)(GET_BITS(pkt->header,0,3)) ;
}

/****************************************************************************
NAME
	setSeq

FUNCTION
	packs the sequence number into the  packet header

IMPLEMENTATION NOTE

****************************************************************************/

void setSeq(Packet * pkt,uint8 n )
{
	SET_BITS(pkt->header,n,0,3) ;
}

/****************************************************************************
NAME
	getProtocolId

FUNCTION
	unpacks the protocol id from the packet header

IMPLEMENTATION NOTE

****************************************************************************/

uint8 getProtocolId(Packet * pkt)
{
	return (uint8)(GET_BITS(pkt->header,8,4)) ;
}

/****************************************************************************
NAME
	setProtocolId

FUNCTION
	packs the protocol id into the  packet header

IMPLEMENTATION NOTE

****************************************************************************/

void setProtocolId(Packet * pkt,uint8 n)
{
	SET_BITS(pkt->header,n,8,4) ;
}

/****************************************************************************
NAME
	isUnreliable

FUNCTION
	unpacks the protocol type bit from the packet header and interprets
	it as reliable or unreliable

IMPLEMENTATION NOTE

****************************************************************************/

bool isUnreliable(Packet *pkt)
{
	uint8 protocolType = (uint8)(GET_BITS(pkt->header,7,1)) ;
	return (protocolType == 0) ;
}

/****************************************************************************
NAME
	setProtocolType

FUNCTION
	packs the protocol type into the packet header

IMPLEMENTATION NOTE

****************************************************************************/

void setProtocolType(Packet * pkt,BCSPChannelType type)
{
	SET_BITS(pkt->header,type,7,1) ;
}

/****************************************************************************
NAME
	pktLen

FUNCTION
	unpacks the packet length from a uint32 claiming to be a packet header

IMPLEMENTATION NOTE

****************************************************************************/

uint16 pktLen(uint32 header)
{
	return (uint16)(GET_BITS(header,12,12)) ;
}

/****************************************************************************
NAME
	getLength

FUNCTION
	unpacks the length field from the packet header

IMPLEMENTATION NOTE

****************************************************************************/

uint16 getLength(Packet * pkt)
{
	return (uint16)(GET_BITS(pkt->header,12,12)) ;
}

/****************************************************************************
NAME
	setLength

FUNCTION
	packs the length field into the packet header

IMPLEMENTATION NOTE

****************************************************************************/

void setLength(Packet * pkt, uint32 len) 
{
	SET_BITS(pkt->header,len,12,12) ;
}

/****************************************************************************
NAME
	packetHasCRC

FUNCTION
	unpacks the CRC-present bit from a packet header and interprets it 
	as true or false

IMPLEMENTATION NOTE

****************************************************************************/

bool packetHasCRC(Packet * pkt)
{
	return (bool)(GET_BITS(pkt->header,6,1)) ;
}


/****************************************************************************
NAME
	setPacketHasCRC

FUNCTION
	sets the CRC-present field in the packet header

IMPLEMENTATION NOTE

****************************************************************************/

void setPacketHasCRC(Packet * pkt)
{
	SET_BITS(pkt->header,1,6,1) ;
}


/****************************************************************************
NAME
	calculateChecksum

FUNCTION
	calculates the 8 bit checksum for a uint32 presenting itself as a packet
	header

IMPLEMENTATION NOTE

****************************************************************************/


uint8 calculateChecksum(uint32 header) 
{
	uint8 hdr0,hdr1,hdr2 ;
	hdr0 = (uint8) (header & 0xff) ;
	hdr1 = (uint8) ((header>>8) & 0xff) ;
	hdr2 = (uint8) ((header>>16) & 0xff) ;

	return (uint8) (~(hdr0+hdr1+hdr2)) ;
}

/****************************************************************************
NAME
	getChecksum

FUNCTION
	unpacks the checksum byte from a uint32 presenting itself as a packet header

IMPLEMENTATION NOTE

****************************************************************************/

uint8 getChecksum(uint32 header) 
{
	return (uint8) ((header >>24) &0xff) ;
}

/****************************************************************************
NAME
	setPacketChecksum

FUNCTION
	calculates and packs the checksum field in a packet header.

IMPLEMENTATION NOTE

****************************************************************************/

void setPacketChecksum(Packet *pkt)
{
	uint8 chk = calculateChecksum(pkt->header) ;
	pkt->header &= 0x00ffffff ;   //clear out top bits
	pkt->header |= (((uint32) chk) <<24) ;
}

