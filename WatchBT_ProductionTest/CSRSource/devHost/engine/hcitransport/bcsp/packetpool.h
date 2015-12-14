/****************************************************************************

	Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
	PacketPool.h  -  defines pool for holding packets

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating

*/


#ifndef _PACKETPOOL_H_
#define _PACKETPOOL_H_

#ifdef	RCS_STRINGS
static  char    packetpool_h_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/packetpool.h#1 $";
#endif




#include "packetbuffer.h"
#include "bcsp_signal.h"

struct _BCSPStack ;


/*
STRUCTURE
	PacketPool

FUNCTION
	The PacketPool provides an accounting service for the packets
	in the system. It is important that not all packets are allocated
	to either send or receive since deadlock might occur.  Therefore 
	all functions wishing to obtain a packet are required to say whether
	they intend to use if for xmit or rcv.and the pool keeps track
	accordingly.  The PacketPool is also responsible for freeing 
	packets data sections when necessary.
	
FIELDS
	XMITSignal -used to signal tasks waiting for a packet for transmission
	RCVSignal - used to signal tasks waiting for a packet for receiving
	buffer - used to hold packets when not in use
	numXMITpacketsOut - keeps track of how many packets are out for transmission
	numRCVpacketsOut - keeps track of how many packets are out for receive
	totalPackets - the total number of packets in the pool


IMPLEMENTATION NOTES
	There are a maximum of 256 packets in the pool since
	the numXXXpacketsOut fields are uint8s.
	There are a minimum of MIN_PACKETS_PER_MODE *2 packets required 
	for the packet pool 

*/

typedef struct 
{
	Signal XMITSignal ;
	Signal RCVSignal ;
	PacketBuffer buffer ;
	uint8 numXMITpacketsOut ;
	uint8 numRCVpacketsOut ;
	uint8 totalPackets ;

} PacketPool ;

typedef enum
{
	modeRCV,
	modeXMIT
} PoolMode ;

extern void returnPacketToPool(struct _BCSPStack *stack,Packet *pkt,PoolMode mode) ;
extern Packet * SYNCBLKgetPacketFromPool(struct _BCSPStack *stack,PoolMode mode,uint16 length) ;
extern void providePacketForPool(struct _BCSPStack *stack,Packet * pkt) ;

#endif
