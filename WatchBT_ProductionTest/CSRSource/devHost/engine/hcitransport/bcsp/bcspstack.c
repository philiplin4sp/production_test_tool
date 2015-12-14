/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        BCSPStack.c  -  the bcsp stack

*/


#ifdef	RCS_STRINGS
static  char    bcspstack_c_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/bcspstack.c#1 $";
#endif

#include <stddef.h>
#include <memory.h>
#include "bcspstack.h" 

/****************************************************************************
NAME
	initialiseStack

FUNCTION
	sets up the state which the stack needs

IMPLEMENTATION NOTE
	it is assumed that the stack state is initially zero-filled when 
	obtained from the client.  This function fills in the small amount 
	non-zero state that needs to be set.

****************************************************************************/


void initialiseStack(BCSPStack * stack)
{
	memset(stack,0,sizeof(*stack)) ;
	stack->configuration = getDefaultStackConfiguration() ;
	setXMITWindowSize(&stack->theXMITWindow,stack->configuration->windowSize) ;
	stack->environment = getDefaultEnvironment() ;
}

/****************************************************************************
NAME
	numBytesInTransmitBuffer

FUNCTION
	returns the number of bytes in the stacks transmit buffer

IMPLEMENTATION NOTE
	this is just a simple user-friendly wrapper for the buffer

****************************************************************************/


uint16 numBytesInTransmitBuffer(BCSPStack * stack) 
{
	return numEntriesInByteBuffer(&stack->SLIPByteOutput) ;
}

/****************************************************************************
NAME
	numFreeSlotsInReceiveBuffer

FUNCTION
	returns the amount of free space  in the stacks receive buffer

IMPLEMENTATION NOTE
	this is just a simple user-friendly wrapper for the buffer

****************************************************************************/

uint16 numFreeSlotsInReceiveBuffer(BCSPStack * stack) 
{
	return numFreeSlotsInByteBuffer(&stack->SLIPByteInput) ;
}

/****************************************************************************
NAME
	readByteFromTransmitBuffer

FUNCTION
	reads (and removes) a single byte  from the stacks transmit buffer

IMPLEMENTATION NOTE
	this is just a simple user-friendly wrapper for the buffer

****************************************************************************/


uint8 readByteFromTransmitBuffer(BCSPStack * stack) 
{
	return readByteFromByteBuffer(stack,&stack->SLIPByteOutput) ;
}


/****************************************************************************
NAME
	writeByteToReceiveBuffer

FUNCTION
	inserts a single byte  into the stacks receive buffer

IMPLEMENTATION NOTE
	this is just a simple user-friendly wrapper for the buffer

****************************************************************************/


void writeByteToReceiveBuffer(BCSPStack * stack,uint8 data) 
{
	writeByteToByteBuffer(stack,&stack->SLIPByteInput,data) ;
}

/****************************************************************************
NAME
	readFromTransmitBuffer

FUNCTION
	reads len bytes from the transmit buffer

IMPLEMENTATION NOTE
	len must be <= the number of bytes actually present in the buffer
	It is assumed that the client has checked the number of bytes available
	before calling this.

****************************************************************************/


void readFromTransmitBuffer(BCSPStack * stack,uint8* dest,uint32 len) 
{
	readFromByteBuffer(stack,&stack->SLIPByteOutput,dest,len) ;
}
/****************************************************************************
NAME
	writeToReceiveBuffer

FUNCTION
	writes len bytes into the receive buffer

IMPLEMENTATION NOTE
	len must be <= the number of free slots in the buffer.
	It is assumed that the client has checked the number of free slots
	before calling this.

****************************************************************************/

void writeToReceiveBuffer(BCSPStack * stack,uint8 *src,uint32 len) 
{
	writeToByteBuffer(stack,&stack->SLIPByteInput,src,len) ;
}

/****************************************************************************
NAME
	BCSPLinkEstablished

FUNCTION
	returns true if the link-establishment entity has managed to make a 
	connection with a peer.

IMPLEMENTATION NOTE
	this may also return true if the link-establishment protocol is disabled

****************************************************************************/


bool BCSPLinkEstablished(BCSPStack * stack) 
{
	return stack->isUnchoked ;
}

/****************************************************************************
NAME
	BCSPaddPacket

FUNCTION
	adds a packet for use by the peer.  This is really just a packet header
	and doesn't actually contain the data section.

IMPLEMENTATION NOTE
	this may also return true if the link-establishment protocol is disabled

****************************************************************************/


void BCSPaddPacket(BCSPStack * theStack,Packet * pkt) 
{
	providePacketForPool(theStack,pkt) ;
}

