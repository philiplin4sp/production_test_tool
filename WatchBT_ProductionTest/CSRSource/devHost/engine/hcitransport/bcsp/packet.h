/****************************************************************************

	Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
	Packet.h  -  packet functionality

DESCRIPTION
	Packets are the fundamental unit of transfer for BCSP.

	Packets can be reliable (acknowledged) or unreliable. In practice, these 
	attributes are inherited from the channel the packet is travelling on.
	The BCSPChannelType provides an enum to encapsulate this.

MODIFICATION HISTORY
	1.3  27-apr-00 npm  - added boilerplating

*/

#ifndef __PACKET_H_
#define __PACKET_H_

#ifdef	RCS_STRINGS
static  char    packet_h_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/packet.h#1 $";
#endif

#include "queue.h"
#include "common/bool.h"


struct _BCTransferRequest ;

typedef enum 
{
	BCSPChannelReliable =1,
	BCSPChannelUnreliable=0
} BCSPChannelType ;


/*
STRUCTURE 
		Packet
FIELDS
		node - packet is based on a node so that it can be held in a queue
		request - packets have an associated transfer request 
		header - the 32 bit bcsp header
		data - pointer to packet data
		dataNeedsFreeing - true if data was dynamically allocated by stack.
*/

typedef struct _Packet
{
	struct _QueueNode node ;				 
	struct _BCTransferRequest * request ;
	uint32 header ;
	uint8 * data ;
	bool dataNeedsFreeing ;
} Packet ;


extern uint8 getAck(Packet* pkt) ;
extern void setAck(Packet* pkt,uint8 n) ;
extern uint8 getSeq(Packet* pkt) ;
extern void setSeq(Packet* pkt,uint8 n) ;
extern uint8 getProtocolId(Packet * pkt) ;
extern bool isUnreliable(Packet* pkt) ;
extern uint16 getLength(Packet * pkt) ;
extern void setLength(Packet * pkt,uint32 len) ;
extern void setProtocolId(Packet * pkt,uint8 id) ;
extern void setProtocolType(Packet * pkt,BCSPChannelType type) ;
extern bool packetHasCRC(Packet * pkt) ;
extern uint8 calculateChecksum(uint32 header) ;
extern uint8 getChecksum(uint32 header) ;
extern void setPacketChecksum(Packet *pkt) ;
extern uint16 pktLen(uint32 header) ;
extern void setPacketHasCRC(Packet * pkt) ;


#endif
