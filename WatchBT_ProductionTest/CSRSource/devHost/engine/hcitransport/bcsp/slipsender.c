/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        SlipSender.c  -  slip xmit layer

MODIFICATION HISTORY
	1.4  27-apr-00 npm  - added boilerplating
	1.5  28-apr-00 npm  - added data integrity check
	1.6  28-apr-00 npm  - data check only reliable packets
	1.7  07-dec-00 mr01 - do callback for unreliable packets on channel with deferred callbacks
	1.8  17-jan-01 mr01 - remove ghost definition

*/


#ifdef	RCS_STRINGS
static  char    slipsender_c_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/slipsender.c#1 $";
#endif

#include "bcspstack.h"
#include "slip.h"
#include "scheduler.h"
#include "crc.h"

#include "common/order.h"

#include <stdlib.h>

/****************************************************************************
NAME
	setupTransmit

FUNCTION
	utility function to set up a send for the slip-sender

IMPLEMENTATION NOTE

****************************************************************************/

void setupTransmit(SLIPState * state,uint8 * data,uint16 n,	
					SLIPmachineState nextMachineState)
{
	BTRACE2(SLIP,"SLIP - setting up transmit len %d next state %d\n",n,nextMachineState) ;
	state->XMITdata  =  data;
	state->XMITbytesRemaining = n ; 
	state->XMITState =nextMachineState;
}

/****************************************************************************
NAME
	SLIPstuffByte

FUNCTION
	utility function to stuff a byte into the transmit buffer

IMPLEMENTATION NOTE
	SLIP encodes control chars

****************************************************************************/


void SLIPstuffByte(BCSPStack * stack,ByteBuffer * buf,uint8 databyte)
{
	static const uint8 seq[2][2] = {{0xdb,0xdc},{0xdb,0xdd}};
	BTRACE1(SLIPSNDRAW,"SLIP stuffing byte %x\n",databyte) ;
	switch (databyte)
	{
		case 0xc0 :	//SLIP framing
				writeToByteBuffer(stack,buf,(uint8*)seq[0],2) ;
				break ;

		case 0xdb : //SLIP escape
				writeToByteBuffer(stack,buf,(uint8*)seq[1],2) ;
				break ;
		default: 	//normal character
				writeToByteBuffer(stack,buf,&databyte,1) ;
				break ;
	}

}

/****************************************************************************
NAME
	SYNCBLKsendAsManyBytesAsPossible

FUNCTION
	utility function to stuff as many bytes as possible into the transmit buffer in 
	one go.  Continue until all the bytes in the current transmit have been sent.
	If there is not enough room in the buffer, block immediately.

IMPLEMENTATION NOTE
	We always wait for at least 2 free slots in the buffer so that we are 
	guaranteed to be able to write an escaped char.

****************************************************************************/


void SYNCBLKsendAsManyBytesAsPossible(BCSPStack * stack,SLIPState * state,bool doCRC)
{
	while (state->XMITbytesRemaining)
	{
		uint8 dataByte ;
		//ensure that there are at least two bytes in the buffer...
		SYNCBLKwaitForFreeSlotsInByteBuffer(stack,&stack->SLIPByteOutput,2) ;
		dataByte = *state->XMITdata ; 
		state->XMITdata++ ;
		SLIPstuffByte(stack,&stack->SLIPByteOutput,dataByte) ;
		state->XMITbytesRemaining-- ;
		//update crc if necessary
		if (doCRC) updateCRC(&state->XMITcalcCRC,dataByte) ;
	}
}

/****************************************************************************
NAME
	SlipSender

FUNCTION
	the slip-sender layer.  This takes packets as input and turns them into 
	streams of SLIP-encoded bytes.

IMPLEMENTATION NOTE
	this runs as a task
	The SLIP-sender only accepts one packet at a time to minimise problems with
	the xmit-window rollback mechanism.  Because a window-rollback can be followed
	by a late-arriving ack, we can end up with the scenario where the slip-sender
	is in the middle of sending a packet that the xmitwindow is trying to delete.
	This means that the xmit-window needs to check all victim packets against those
	in the slip-sender queue.  Allowing SLIP to queue only one packet simplies
	this process considerably.

****************************************************************************/

void SlipSender(BCSPStack * stack)
{                                               
	//send a packet if possible
	SLIPState * state ;
	Packet * pkt ;

	//wait for a packet to appear - we don't actually remove it from the buffer
	//until we've finished sending it because we'll almost
	//certainly block at some stage trying to stuff data into the output buffer

	pkt = stack->SLIPInputPacket ;
	if (!pkt) SYNCBLKwaitSignal(stack,&stack->MUXHasPacketForSLIPSignal) ;
	while(1)
	{
		//wait for at least one free slot - this will be sufficient to send the 
		//SLIP framing marker
		SYNCBLKwaitForFreeSlotsInByteBuffer(stack,&stack->SLIPByteOutput,1) ;

		state = &stack->theSLIPState ;

		switch (state->XMITState)
		{
			case SLIPaboutToSendStart :
			{
				//send the frame-start and set up a transmission of the packet header
				BTRACE3(SLIPSND,"SLIP - sending start (pkt %p seq %d ack %d)\n",pkt,getSeq(pkt),getAck(pkt)) ;
				writeByteToByteBuffer(stack,&stack->SLIPByteOutput,0xc0) ;
				NETWORK_REORDER32(state->XMITheader,pkt->header) ; //make sure endian-ness is correct
				setupTransmit(state,(uint8*) &state->XMITheader,4,SLIPsendingHeader) ;
				initCRC(&state->XMITcalcCRC) ; //start crc
			}
			break ;

			case SLIPsendingHeader : 
			{
				SYNCBLKsendAsManyBytesAsPossible(stack,state,true) ;
				//when the header has been sent, send the data section
				setupTransmit(state,pkt->data,getLength(pkt),SLIPsendingData) ;
			}
			break ;
			
			case SLIPsendingData :	 
			{
				bool doCRC = packetHasCRC(pkt) ;
				SYNCBLKsendAsManyBytesAsPossible(stack,state,doCRC) ;
				if (pkt->data && !isUnreliable(pkt)) BTRACE3(DATAINTEGRITY,"SS pkt len %d data %x %x\n",getLength(pkt),*(uint32*)(pkt->data),*(uint32*)(pkt->data+4)) ;


				//when the header has been sent, send the crc (if required)
				if (doCRC)
				{
					state->XMITcalcCRC= getBigEndianCRC(state->XMITcalcCRC) ;
					setupTransmit(state,(uint8*) &state->XMITcalcCRC,2,SLIPsendingCRC) ;
				}
				else state->XMITState = SLIPaboutToSendEnd ;
			}
			break ;
			
			case SLIPsendingCRC :
			{
				SYNCBLKsendAsManyBytesAsPossible(stack,state,false) ;
				//when the crc has been sent, send the frame-end
				state->XMITState = SLIPaboutToSendEnd ;
			}
			break ; 

			case SLIPaboutToSendEnd :
			{
				//this state is introduced to allow the mux-sender to abort the packet
				//send in a resonably friendly way. by jumping to the sendEnd state
				//if this is an unreliable packet on a channel with deferred callbacks,
				//we want to do the callback now that the packet has been sent
				//(but have to be careful with link establishment packets,
				//which are unreliable but whose request member is NULL!)
				if (isUnreliable(pkt) && pkt->request != NULL && getCallbackType(stack,pkt->request->channel) == BCSPCallbackDeferred)
				{
					doCallback(pkt->request,transferComplete) ;
				}
				//if this is an unreliable packet it's up to us to free it 
				if (isUnreliable(pkt) && (pkt != &stack->theAckPacket))
					returnPacketToPool(stack,pkt,modeXMIT) ;
				state->XMITState = SLIPsendEnd ;
			}
			break ;

			case SLIPsendEnd:
			{
				//send the frame-end and clear the input packet, signalling that
				//we are ready to accept another packet.
				BTRACE3(SLIPSND,"SLIP - sending end (pkt %p seq %d ack %d)\n",pkt,getSeq(pkt),getAck(pkt)) ;
				writeByteToByteBuffer(stack,&stack->SLIPByteOutput,0xc0) ;
				state->XMITState = SLIPaboutToSendStart ; 
				stack->SLIPInputPacket = null ; ;
				setSignal(stack,&stack->SLIPSendAvailableSignal) ; //allow mux to send another packet
				return ; //let another task execute
			}
			break ;
		}
	}
	return   ;
}

/****************************************************************************
NAME
	abortSLIPSend

FUNCTION
	utility function to abort the slip-sender by forcing it into the end state..

IMPLEMENTATION NOTE
	This is called by the xmit-window if it detects that SLIP is in the middle
	of sending a packet that it wants to delete.
	The assumption is that SLIP is sending a reliable packet at the time.
	Since the only time it can get aborted is if the packet is reliable and the 
	xmitwindow is rolling forward, this is safe..


****************************************************************************/

void abortSLIPSend(BCSPStack * stack )
{
	BTRACE0(SLIPSND,"***Abort SLIP Send called***\n") ;
	stack->theSLIPState.XMITState = SLIPsendEnd ;
}
