
/****************************************************************************

	Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
	PacketBuffer.h  -  defines queue to hold packets

DESCRIPTION
	A PacketBuffer is simple a queue of packets.

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating

*/

#ifndef __PACKETBUFFER_H_
#define __PACKETBUFFER_H_

#ifdef	RCS_STRINGS
static  char    packetbuffer_h_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/packetbuffer.h#1 $";
#endif






#include "packet.h" 
#include "queue.h"



struct _BCSPStack ;

typedef struct _PacketBuffer
{
	Queue queue ;
} PacketBuffer ;


extern void addPacketToBuffer(struct _BCSPStack * stack,PacketBuffer * buf,Packet * pkt) ;
extern Packet *  SYNCBLKgetPacketFromBuffer(struct _BCSPStack * stack,PacketBuffer * buf) ;
extern Packet *  ASYNCBLKgetPacketFromBuffer(struct _BCSPStack * stack,PacketBuffer * buf) ;
extern Packet *  SYNCBLKpeekAtPacketInBuffer(struct _BCSPStack * stack,PacketBuffer * buf) ;
extern Packet * peekAtPacketInBuffer(PacketBuffer * buf) ;

#endif
