/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        PacketPool.c  -  packet pool services

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating
	1.3  12-mar-01 at  -  packet data pointer initialised if not used
*/


#ifdef	RCS_STRINGS
static  char    packetpool_c_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/packetpool.c#1 $";
#endif

#include "packetpool.h"

#include "bcspstack.h"

//change this to control how many packets are reserved for each mode
#define MIN_PACKETS_PER_MODE 2 


/****************************************************************************
NAME
	returnPacketToPool

FUNCTION
	returns a packet to the packet pool.  This may cause a waiting task to
	be signalled. The packets data section may be freed as well.

IMPLEMENTATION NOTE

****************************************************************************/

void returnPacketToPool(struct _BCSPStack *stack,Packet *pkt,PoolMode mode)
{

	PacketPool * pool = &stack->thePacketPool ;
	switch (mode)
	{
	case modeXMIT: 
			setSignal(stack,&pool->XMITSignal) ;
			pool->numXMITpacketsOut -- ;
			break ;
	case modeRCV:
			setSignal(stack,&pool->RCVSignal) ;
			pool->numRCVpacketsOut -- ;
			break ;
	}
	BTRACE8(PKTMNG,"releasing pkt %p  data %p len %d (free=%d) mode %d out = %d,%d (%d)\n",
				pkt,pkt->data,getLength(pkt),pkt->dataNeedsFreeing,mode,pool->numRCVpacketsOut,
				pool->numXMITpacketsOut,pool->totalPackets) ;
	if (pkt->dataNeedsFreeing) 
		stack->environment->freeMem(stack->environment->envState,pkt->data) ;

	addPacketToBuffer(stack,&pool->buffer,pkt) ;
							
}

/****************************************************************************
NAME
	SYNCBLKgetPacketFromPool

FUNCTION
	Attempts to obtain a packet from the pool for the given mode.  If none are
	available (or we've reached the limit for this mode) this function will
	block immediately.  This function also attempts to obtain memory for the
	packets data section.

IMPLEMENTATION NOTE
	It is currently assumed that the alloc call to get memory for the packets
	data section never fails. This should really be fixed. 
	TODO - fix this ?

****************************************************************************/


Packet * SYNCBLKgetPacketFromPool(struct _BCSPStack *stack,PoolMode mode,uint16 length)
{

	Packet * pkt ;
	PacketPool * pool = &stack->thePacketPool ;

	switch (mode)
	{
	case modeXMIT:
			if (pool->totalPackets - pool->numXMITpacketsOut <= MIN_PACKETS_PER_MODE)   
			{
				SYNCBLKwaitSignal(stack,&pool->XMITSignal) ;
			}
			else 
			{
				pkt = SYNCBLKgetPacketFromBuffer(stack,&pool->buffer) ;
				pool->numXMITpacketsOut ++ ;
			}
			break ;
	case modeRCV:
			if (pool->totalPackets - pool->numRCVpacketsOut <= MIN_PACKETS_PER_MODE)   
			{
				SYNCBLKwaitSignal(stack,&pool->RCVSignal) ;
			}
			else
			{
				pkt = SYNCBLKgetPacketFromBuffer(stack,&pool->buffer) ;
				pool->numRCVpacketsOut ++ ;
			}
			break ;
	}
	//we have a packet - now see whether we need to allocated data for it (indicated 
	//by non-zero length)
	if (length) 
	{
		pkt->dataNeedsFreeing = true ;
		pkt->data = stack->environment->allocMem(stack->environment->envState,length) ;

	}
	else
	{
		pkt->dataNeedsFreeing = false ;
		pkt->data = 0;
	}
	//fill in rest of fields
	pkt->request = null ;
	pkt->header = 0 ;
	setLength(pkt,length) ;
	
	BTRACE8(PKTMNG,"getting pkt %p  data %p len %d (free=%d) mode %d out = %d,%d (%d)\n",
				pkt,pkt->data,getLength(pkt),pkt->dataNeedsFreeing,mode,pool->numRCVpacketsOut,
				pool->numXMITpacketsOut,pool->totalPackets) ;
	
	return pkt ;
}

/****************************************************************************
NAME
	providePacketForPool

FUNCTION
	provides a packet for the packet pool

IMPLEMENTATION NOTE
	This function must be called before the stack has beed started since there
	is no thread-protection around it.
****************************************************************************/

void providePacketForPool(struct _BCSPStack *stack,Packet * pkt) 
{
	PacketPool * pool = &stack->thePacketPool ;
	addPacketToBuffer(stack,&pool->buffer,pkt) ;
	pool->totalPackets ++ ;
}
