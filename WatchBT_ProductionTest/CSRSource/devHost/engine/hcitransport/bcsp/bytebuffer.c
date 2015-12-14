/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        ByteBuffer.c  -  byte-oriented fifo

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating
	1.3  22-jun-00 npm	- minor optimisation in buffer wraparound code
	1.4  22-jun-00 npm  - optimisation fix
*/


#ifdef	RCS_STRINGS
static  char    bytebuffer_c_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/bytebuffer.c#1 $";
#endif

#include "bytebuffer.h"

/****************************************************************************
NAME
	numFreeSlotsInByteBuffer

FUNCTION
	returns the amount of available space left in the byte buffer

IMPLEMENTATION NOTE

****************************************************************************/


uint32 numFreeSlotsInByteBuffer(ByteBuffer * buf) 
{
	return sizeof(buf->buffer) - buf->numEntries ;
}

/****************************************************************************
NAME
	numFreeSlotsInByteBuffer

FUNCTION
	returns the number of bytes in the buffer

IMPLEMENTATION NOTE

****************************************************************************/


uint32 numEntriesInByteBuffer(ByteBuffer * buf) 
{
	return buf->numEntries ;
}

/****************************************************************************
NAME
	writeToByteBuffer

FUNCTION
	writes len bytes from src into the buffer

IMPLEMENTATION NOTE
	It is assumed that the user has already checked that the buffer will not
	be overflowed by this write.  Also, the write may release another task
	that is waiting for signalTrigger characters to be present.

****************************************************************************/

void writeToByteBuffer(struct _BCSPStack *stack,ByteBuffer * buf,uint8 * src,uint32 n) 
{
	uint32 c ;
	buf->numEntries += n ;
	for (c = 0 ; c < n ; c++) 
	{
		//add the chars one at a time, wrapping round the end of the buffer
		//as necessary.
		buf->buffer[buf->firstFreeSlot] = src[c] ;
		buf->firstFreeSlot = (buf->firstFreeSlot+1) & BYTE_BUFFERSIZE_MASK ;
	}
	//now signal the waiting task if necessary.
	if ((buf->signalTrigger) && (buf->numEntries >= buf->signalTrigger )) 
	{
		buf->signalTrigger =0 ; //reset it
		setSignal(stack,&buf->signal) ;
	}
}

/****************************************************************************
NAME
	writeByteToByteBuffer

FUNCTION
	writes a single byte into the buffer

IMPLEMENTATION NOTE
	It is assumed that the user has already checked that the buffer will not
	be overflowed by this write.  Also, the write may release another task
	that is waiting for signalTrigger characters to be present.

****************************************************************************/

void writeByteToByteBuffer(struct _BCSPStack *stack,ByteBuffer * buf,uint8 data) 
{
	buf->numEntries ++  ;
	buf->buffer[buf->firstFreeSlot] = data ;
	buf->firstFreeSlot = (buf->firstFreeSlot+1) & BYTE_BUFFERSIZE_MASK ;
	
	//check the trigger and signal the waiting task if required.
	if ((buf->signalTrigger) && (buf->numEntries >= buf->signalTrigger )) 
	{
		buf->signalTrigger =0 ; //reset it
		setSignal(stack,&buf->signal) ;
	}
}

/****************************************************************************
NAME
	readFromByteBuffer

FUNCTION
	writes len bytes from the buffer into dest

IMPLEMENTATION NOTE
	It is assumed that the user has already checked that the buffer has 
	enought bytes to satisfy this read.  Also, the read may release another task
	that is waiting for signalTrigger spaces to be available.

****************************************************************************/


void readFromByteBuffer(struct _BCSPStack *stack,ByteBuffer * buf,uint8 * dest,uint32 n) 
{
	uint32 c ;
	uint32 numFreeSlots ;
	//figure out where we're going to start reading from...
	uint32 firstTakenSlot ;
	
	if (!n) return ; //short-circuit zero-length requests
	
	//if the request is for non-zero length, the number of entries must be non-zero and
	//the following code is ok
	firstTakenSlot = (buf->firstFreeSlot - buf->numEntries) & BYTE_BUFFERSIZE_MASK ;
	
	buf->numEntries -= n ;
	for (c = 0 ; c < n ; c++) 
	{
		//simple copy loop, wrap when we hit the end of the buffer
		dest[c] = buf->buffer[firstTakenSlot]  ;
		firstTakenSlot = (firstTakenSlot+1) & BYTE_BUFFERSIZE_MASK ;
	}
	numFreeSlots = sizeof(buf->buffer) - buf->numEntries ;

	//now check whether we need to signal a waiting task.
	if ((buf->signalTrigger) && (numFreeSlots >= buf->signalTrigger )) 
	{
		buf->signalTrigger =0 ; //reset it
		setSignal(stack,&buf->signal) ;
	}
}

/****************************************************************************
NAME
	readByteFromByteBuffer

FUNCTION
	reads a single byte from the buffer

IMPLEMENTATION NOTE
	It is assumed that the user has already checked that the buffer has 
	enought bytes to satisfy this read.  Also, the read may release another task
	that is waiting for signalTrigger spaces to be available.

****************************************************************************/

uint8 readByteFromByteBuffer(struct _BCSPStack *stack,ByteBuffer * buf) 
{
	uint32 numFreeSlots ;
	//figure out where the first byte is - obviously numEntries is non-zero if we're 
	//reading a byte
	
	uint32 firstTakenSlot = (buf->firstFreeSlot - buf->numEntries) & BYTE_BUFFERSIZE_MASK; 
	buf->numEntries -- ;

	numFreeSlots = sizeof(buf->buffer) - buf->numEntries ;
	
	//check whether we need to wake up a waiting task
	if ((buf->signalTrigger) && (numFreeSlots >= buf->signalTrigger )) 
	{
		buf->signalTrigger =0 ; //reset it
		setSignal(stack,&buf->signal) ;
	}

	return buf->buffer[firstTakenSlot] ;
}


/****************************************************************************
NAME
	peekAtByteInByteBuffer

FUNCTION
	returns the byte at the front of the buffer.  

IMPLEMENTATION NOTE
	This function is always called after the prog has waited for at least one
	byte to be present

****************************************************************************/


uint8 peekAtByteInByteBuffer(ByteBuffer * buf) 
{
	uint32 firstTakenSlot = (buf->firstFreeSlot - buf->numEntries) &  BYTE_BUFFERSIZE_MASK  ; 
	return buf->buffer[firstTakenSlot] ;
}

/****************************************************************************
NAME
	SYNCBLKwaitForFreeSlotsInByteBuffer

FUNCTION
	waits for a certain amount of free space to be available in the buffer

IMPLEMENTATION NOTE
	

****************************************************************************/

void SYNCBLKwaitForFreeSlotsInByteBuffer(struct _BCSPStack *stack,ByteBuffer * buf,uint32 n)
{

	uint32 freeSlots = 	sizeof(buf->buffer) - buf->numEntries ;
	if (freeSlots >= n) return ;
	buf->signalTrigger = n ; 
	SYNCBLKwaitSignal(stack,&buf->signal) ;
}

/****************************************************************************
NAME
	SYNCBLKwaitForEntriesInByteBuffer

FUNCTION
	waits for a certain number of bytes to be available in the buffer

IMPLEMENTATION NOTE
	

****************************************************************************/

void SYNCBLKwaitForEntriesInByteBuffer(struct _BCSPStack *stack,ByteBuffer * buf,uint32 n)
{

	if (buf->numEntries>= n) return ;
	buf->signalTrigger = n ; 
	SYNCBLKwaitSignal(stack,&buf->signal) ;
}




