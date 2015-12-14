/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        ByteBuffer.h  -  simple fifo for buffering bytes

DESCRIPTION
	the ByteBuffer class is intended to be the interface between the stack and the 
	environment.  The SLIP send and receive layers write to and read to 
	byte buffers and then the environment either empties or refills them 
	to/from the uart 
	Byte-buffers are simple cicular FIFOs built on an array.
	The bytebuffer has a signal which can be used to allow threads to wait for 
	either characters or free slots to appear.  
	No buffers require both kinds of events so the signal is shared between both
	modes.

		

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating

*/

#ifndef _BYTEBUFFER_H
#define _BYTEBUFFER_H

#ifdef	RCS_STRINGS
static  char    bytebuffer_h_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/bytebuffer.h#1 $";
#endif

#include "common/types.h"

#ifndef BYTE_BUFFERSIZE
#define BYTE_BUFFERSIZE 32 // this must be a power of 2
#endif
#define BYTE_BUFFERSIZE_MASK (BYTE_BUFFERSIZE-1)

//enforce the power-of-two restriction
#if  (BYTE_BUFFERSIZE & (BYTE_BUFFERSIZE-1))
#error BYTE_BUFFERSIZE must be a power of 2
#endif

struct _BCSPStack ;

#include "bcsp_signal.h"

typedef struct _ByteBuffer
{
	Signal signal ;
	uint8 buffer[BYTE_BUFFERSIZE] ;	// the data
	uint32 numEntries ;				// the number of bytes occupied
	uint32 firstFreeSlot ;			// index to the first free slot
	uint32 signalTrigger ;			// threshold for triggerering waiting task 
} ByteBuffer ;


extern uint32 numFreeSlotsInByteBuffer(ByteBuffer * buf) ;
extern uint32 numEntriesInByteBuffer(ByteBuffer * buf) ;

extern void writeToByteBuffer(struct _BCSPStack *stack,ByteBuffer * buf,uint8 * src,uint32 n) ;
extern void writeByteToByteBuffer(struct _BCSPStack *stack,ByteBuffer * buf,uint8 c) ;
extern void readFromByteBuffer(struct _BCSPStack *stack,ByteBuffer * buf,uint8 * dest,uint32 n) ;
extern uint8 readByteFromByteBuffer(struct _BCSPStack *stack,ByteBuffer * buf) ;
extern uint8 peekAtByteInByteBuffer(ByteBuffer * buf) ;

extern void SYNCBLKwaitForFreeSlotsInByteBuffer(struct _BCSPStack *stack,ByteBuffer * buf,uint32 n);
extern void SYNCBLKwaitForEntriesInByteBuffer(struct _BCSPStack *stack,ByteBuffer * buf,uint32 n);

#endif
