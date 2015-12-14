
/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        Receiver.c  -  receiver task

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating
	1.5  18-apr-01 at   - non-link-establishment packets discarded while choked
	1.6  18-apr-01 at   - corrected link establishment channel number
*/


#ifdef	RCS_STRINGS
static  char    receiver_c_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/receiver.c#1 $";
#endif

#include <stdio.h>
#include <stdlib.h>


#include "packetbuffer.h"
#include "bcspstack.h"
#include "xmitwindow.h"


/****************************************************************************
NAME
	Receiver

FUNCTION
	this entity receives packets from the SLIP-receiver.  By the time they get 
	here, the slip-decode has been done and the packets have been integrity-checked
	There are still some additional steps to be be taken before the packet is routed
	to its destination:  the ack field needs to be interpreted and the xmit-window
	rolled forward

IMPLEMENTATION NOTE
	This runs as a task.

****************************************************************************/
void Receiver(BCSPStack * stack)
{
	//Wait until we get a packet from SLIP
	Packet * pkt = SYNCBLKgetPacketFromBuffer(stack,&stack->RCVInputBuffer) ;

	//Now remove any acknowledged packets from the transmit window
	//this will free up the transmit side to send even more
	if (stack->rxack != getAck(pkt))
	{
		stack->rxack=getAck(pkt) ;
		ReleaseXMITWindowAcknowledgedSlots(stack) ;
	}

	//check whether this is just an ack packet

	if	((getSeq(pkt) ==0) &&
		 (getProtocolId(pkt)==0) &&
		 (getLength(pkt)==0))
	{
		BTRACE2(RCV,"Rcv - ACK %p %d\n",pkt,getAck(pkt)) ;
		//if it is an ack, we can just return it to oblivion
		returnPacketToPool(stack,pkt,modeRCV) ;
	}
	else if (!stack->isUnchoked && (getProtocolId(pkt) != 1))
	{
		BTRACE4(RCV,"Rcv - dumping while choked pkt %p seq %d ack %d chan %d\n",pkt,getSeq(pkt),getAck(pkt),getProtocolId(pkt)) ;
		//Discard (without acknowledgement) any packets other than link
		//establishment received while choked
		returnPacketToPool(stack,pkt,modeRCV) ;
	}
	else
	{
		//it's a real packet - figure out whether it's reliable or unreliable
		if (isUnreliable(pkt))
		{
			BTRACE4(RCV,"Rcv - got unreliable pkt %p seq %d ack %d chan %d\n",pkt,getSeq(pkt),getAck(pkt),getProtocolId(pkt)) ;

			//it's unreliable - need to deliver it.  
			//Channel 1 is reserved for link-establishment 
			if (getProtocolId(pkt)!=1)
				 addPacketToBuffer(stack,&stack->PacketDelivererInput,pkt) ;
			else addPacketToBuffer(stack,&stack->LinkEstablishmentInput,pkt) ;
		}
		else
		{
			//We now know that the packet is reliable and a bona-fide data packet
			//but there is still the possibility that it is not the one we're expecting.
			//In that case we have to discard it.  However, regardless of whether it's 
			//'Mr Right' we need to send an ack.  Acks are required for unexpected 
			//packets because there is a possible scenario where the peer loses our ack
			//and rolls back the window so far that it never sends the right packet.
			//Eg, with a window size of 4
			//Peer sends 0,1,2,3.  We (finally) send ack 4 but it gets lost.
			//Peer resends 0,1,2,3 but we're expecting 4 so don't ack  
			//Unless we get round to transmitting some data packets, we'll be stuck.

			if (getSeq(pkt) == stack->txack) 
			{
				// JBS: no GetTickCount on POSIX
                //   BTRACE5(RCV,"Rcv - got reliable pkt %p seq %d ack %d chan %d time %d\n",pkt,getSeq(pkt),getAck(pkt),getProtocolId(pkt), GetTickCount () ) ;
				BTRACE4(RCV,"Rcv - got reliable pkt %p seq %d ack %d chan %d time %d\n",pkt,getSeq(pkt),getAck(pkt),getProtocolId(pkt)) ;

				//bump the ack number by 1 and give the packet to the postman.
				addPacketToBuffer(stack,&stack->PacketDelivererInput,pkt) ;
				stack->txack = (stack->txack+1) &7 ;
			}
			else
			{
				// JBS: no GetTickCount on POSIX
				//   BTRACE6(RCV,"Rcv - expecting %d : dumping reliable pkt %p seq %d ack %d chan %d time %d\n",stack->txack,pkt,getSeq(pkt),getAck(pkt),getProtocolId(pkt) , GetTickCount () ) ;
				BTRACE5(RCV,"Rcv - expecting %d : dumping reliable pkt %p seq %d ack %d chan %d time %d\n",stack->txack,pkt,getSeq(pkt),getAck(pkt),getProtocolId(pkt)) ;
				//if it's out of sequence, we have to discard it
				returnPacketToPool(stack,pkt,modeRCV) ;
			}
			//now wake up the muxsender to send an ack.
			stack->ackRequired = true ;
			setSignal(stack,&stack->txackSignal) ;
		}
	}
	
	return   ;

}
