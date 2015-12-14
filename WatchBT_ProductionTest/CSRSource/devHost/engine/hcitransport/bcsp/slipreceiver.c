/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        SLIPReceiver.c  -  SLIP recepetion and packet verification task

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating
	1.3  28-apr-00 npm  - added data integrity check
	1.4  28-apr-00 npm  - data integrity check only reliable packets

*/


#ifdef	RCS_STRINGS
static  char    slipreceiver_c_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/slipreceiver.c#1 $";
#endif

#include "bcspstack.h"
#include "slip.h"
#include "bytebuffer.h"
#include "crc.h"

#include "common/order.h"

/****************************************************************************
NAME
	bailOut

FUNCTION
	utility function to allow receive to abort when it detects an error

IMPLEMENTATION NOTE
	Despite the name, this function does not actually jump all the way back 
	to the scheduler and must be followed by a return.

****************************************************************************/


void bailOut(BCSPStack * stack,SLIPState * state)
{
	Packet * pkt = state->RCVpkt ;	

	if (pkt)
	{
		BTRACE4(SLIPRCV,"SLIP dumping bad packet %p len %d seq %d ack %d\n",pkt,getLength(pkt),getSeq(pkt),getAck(pkt)) ; 
		BTRACE3(SLIPRCV,"read crc = %x calculated %x (raw %x)\n",state->RCVCRC,getBigEndianCRC(state->RCVcalcCRC),state->RCVcalcCRC ) ; 
		returnPacketToPool(stack,pkt,modeRCV) ;
		state->RCVpkt = null ;
	}
	state->RCVState = SLIPunsynchronised ;
}

/****************************************************************************
NAME
	bailOut

FUNCTION
	utility function to setup the next receive block

IMPLEMENTATION NOTE

****************************************************************************/

void setupRcv(SLIPState * state,uint8 * dest,uint16 len,SLIPmachineState nextMachineState)
{
	state->RCVdata = dest ;
	state->RCVbytesRemaining = len ;
	state->RCVState=nextMachineState ;
}


/****************************************************************************
NAME
	SYNCBLKunstuffByte

FUNCTION
	decodes SLIP-encoded characters in the receive buffer. Blocks immediately
	if there isn't a character (or if there's only half an encoded char) in the
	buffer

IMPLEMENTATION NOTE
	sets the endOfData flag if a raw c0 (slip frame marker) is found

****************************************************************************/

uint8 SYNCBLKunstuffByte(BCSPStack * stack,ByteBuffer * buf,bool * endOfData)
{
	uint8 dataByte ;
	//wait for one char initially - most chars won't be encoded 
	SYNCBLKwaitForEntriesInByteBuffer(stack,buf,1) ;
	*endOfData = false ;
	//just peek at the data - we may need to get another one if this char is encoded
	dataByte = peekAtByteInByteBuffer(buf) ;
	if (dataByte == 0xdb)
	{
		//the char is encoded so wait for two chars to be present
		SYNCBLKwaitForEntriesInByteBuffer(stack,buf,2) ;
		readByteFromByteBuffer(stack,buf) ; //this is just the escape char so dump it
		dataByte = readByteFromByteBuffer(stack,buf) ; 
		//do the decode
		switch (dataByte)
		{
		case 0xdc : return 0xc0 ; break ;
		case 0xdd : return 0xdb ; break ;
		default: *endOfData = true ; //we shouldn't be able to see any other value
				return dataByte ;
				break ;
		}
	}
	else 
	if (dataByte == 0xc0) 
	{
		//this is most unexpected - we should never see an naked c0 in this routine!
		*endOfData = true ;
	}
	//just an ordinary character
	dataByte = readByteFromByteBuffer(stack,buf) ; 
	return dataByte ;
}

/****************************************************************************
NAME
	SYNCBLKreadAsManyBytesAsPossible

FUNCTION
	reads as many characters as possible from the receive buffer and keep trying
	until the current receive is satisfied.  Block immediately if we run out
	of data.

IMPLEMENTATION NOTE

****************************************************************************/

bool SYNCBLKreadAsManyBytesAsPossible(BCSPStack * stack,SLIPState * state,bool doCRC)
{
	//return true if ok - false if error 
	uint8 dataByte ;
	bool endOfData ;

	//keep reading bytes until we're done
	while (state->RCVbytesRemaining)
	{
		//block until we can read a byte
		dataByte= SYNCBLKunstuffByte(stack,&stack->SLIPByteInput,&endOfData) ;
		if (endOfData) 
		{
			bailOut(stack,state) ; //oops - this was a bit unexpected - start again
			return false;
		}
		*state->RCVdata = dataByte ;
		state->RCVdata ++ ;
		state->RCVbytesRemaining-- ;
		//update the crc as we go
		if (doCRC) updateCRC(&state->RCVcalcCRC,dataByte) ;
	}
	return true ;
}


/****************************************************************************
NAME
	SLIPReceiver

FUNCTION
	performs slip-decoding and integrity checking (slipreceiver is a bit of a
	misnomer)  Chucks away corrupt packets and only passes good ones to the 
	receiver task.

IMPLEMENTATION NOTE

****************************************************************************/

void SLIPReceiver(BCSPStack * stack)
{
    Packet * pkt;
    SLIPState * state ;

    while(1) 
    {
        state = &stack->theSLIPState ;

        pkt = state->RCVpkt ;
        switch (state->RCVState)
        {
        case SLIPunsynchronised :
            //when we're unsynchronised we need to wait for a c0
            while (state->RCVState == SLIPunsynchronised)
            {
                //we wait for _2_ bytes because it's possible we've synchronised in the 
                //middle of a packet and will see the tail c0 followed by the head c0
                uint8 dataByte ;
                SYNCBLKwaitForEntriesInByteBuffer(stack,&stack->SLIPByteInput,2) ;
                dataByte = readByteFromByteBuffer(stack,&stack->SLIPByteInput) ;
                if (dataByte == 0xc0) 
                {
                    //we need to take a peek at the next byte in case it's c0 too in
                    //which case we need to discard it.
                    BTRACE0(SLIPRCV,"SLIP - synchronising on c0\n") ; 

                    if (peekAtByteInByteBuffer(&stack->SLIPByteInput) == 0xc0)
                        readByteFromByteBuffer(stack,&stack->SLIPByteInput) ;

                    //now start reading the header
                    setupRcv(state,(uint8*)&state->RCVheader,4,SLIPRCVHeader) ;
                    initCRC(&state->RCVcalcCRC) ; //initialise CRC
                }
                else
                    BTRACE1(SLIPRCV,"SLIP attempting to sync  - discarding %x\n",dataByte) ;
            }
            break ;

        case SLIPRCVHeader :
            {
                uint32 header ;
                if (!SYNCBLKreadAsManyBytesAsPossible(stack,state,true))
                    return  ;
                NETWORK_REORDER32(header,state->RCVheader) ;
                if (calculateChecksum(header) != getChecksum(header))
                {
                    //the header's corrupt - we might as well give up now
                    bailOut(stack,state) ;
                    return ;
                }
                //we got a good header so allocate a packet for it
                pkt=state->RCVpkt = SYNCBLKgetPacketFromPool(stack,modeRCV,pktLen(header)) ;
                pkt->header = header ;
                //read the data
                setupRcv(state,pkt->data,getLength(pkt),SLIPRCVData) ;
            }
            break ;

        case SLIPRCVData :
            {
                if (!SYNCBLKreadAsManyBytesAsPossible(stack,state,true))
                    return  ;
                //after the data comes the CRC (maybe)
                if (packetHasCRC(pkt))
                    setupRcv(state,(uint8*) &state->RCVCRC,2,SLIPRCVCRC) ;
                else
                    state->RCVState = SLIPexpectEndOfData ;
                if (pkt->data && !isUnreliable(pkt))
                {
                    if (getLength(pkt) < 4) BTRACE1(DATAINTEGRITY,"SR pkt len %d\n",getLength(pkt)) ;
                    else if (getLength(pkt) < 8) BTRACE2(DATAINTEGRITY,"SR pkt len %d data %x\n",getLength(pkt),*(uint32*)(pkt->data)) ;
                    else BTRACE3(DATAINTEGRITY,"SR pkt len %d data %x %x\n",getLength(pkt),*(uint32*)(pkt->data),*(uint32*)(pkt->data+4)) ;
                }
            }
            break ;

        case SLIPRCVCRC :
            {
                uint16 crc ;
                if (!SYNCBLKreadAsManyBytesAsPossible(stack,state,false))
                    return  ;
                //at this point the received crc will be msb first
                crc = getBigEndianCRC(state->RCVcalcCRC) ;
                if (crc != state->RCVCRC)
                {
                    bailOut(stack,state);
                    return ;
                }
                //crc is ok so expect a c0 next...
                state->RCVState = SLIPexpectEndOfData ;
            }
            break ;


        case SLIPexpectEndOfData :
            {
                uint8 dataByte ;
                BTRACE4(SLIPRCV,"SLIP received packet - waiting for end %p len %d seq %d ack %d\n",pkt,getLength(pkt),getSeq(pkt),getAck(pkt)) ; 
                //wait for a byte - it had better be a c0 !
                SYNCBLKwaitForEntriesInByteBuffer(stack,&stack->SLIPByteInput,1) ;
                dataByte = readByteFromByteBuffer(stack,&stack->SLIPByteInput) ;
                if (dataByte != 0xc0) 
                {
                    bailOut(stack,state)  ;  //another wretched corrupt packet
                    return ;
                }
                state->RCVState = SLIPSendPacket ;
            }
            break ;

        case SLIPSendPacket :
            {
                //wow, we've finally got a good packet - we've already checked the header
                //checksum and CRC and we know the length is what we expected so pass
                //it on to the receiver task.
                BTRACE4(SLIPRCV,"SLIP received good packet %p len %d seq %d ack %d\n",pkt,getLength(pkt),getSeq(pkt),getAck(pkt)) ; 
                addPacketToBuffer(stack,&stack->RCVInputBuffer,state->RCVpkt) ;
                state->RCVState = SLIPunsynchronised ;
                state->RCVpkt = null ;
            }
            break ;
        } //switch
    } //while
}
