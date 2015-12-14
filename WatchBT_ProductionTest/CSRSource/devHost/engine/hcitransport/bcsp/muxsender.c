/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        MUXSender.c  -  mux sender task

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating
$Log: muxsender.c,v $
Revision 1.1  2002/03/27 18:35:11  ajh
Moved and renamed from
devHost/HostStack/singledll/

Revision 1.1  2001/06/29 12:56:52  ajh
First adding, temporarily, of a single dll version of the hoststack,
including bluetest as the test app.

Revision 1.6  2001/05/31 17:56:18  mr01
Delinting.

Revision 1.5  2001/05/22 15:13:30  cl01
Reworking for CE/Win32 merge


*/


#ifdef	RCS_STRINGS
static  char    muxsender_c_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/muxsender.c#1 $";
#endif

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>


#include "packetbuffer.h"
#include "bcspstack.h" 
#include "xmitwindow.h"

/****************************************************************************
NAME
	MUXSender

FUNCTION
	this is the entity responsible for sending all packets on to the SLIP
	layer.  It can extract packets from the (reliable) xmit window or the
	unreliable queue and can also generate an ack packet if need be.

IMPLEMENTATION NOTE
	This runs as a task.
	this must be enclosed in a while loop to make the blocking sequence safe.
	Ie, if we were to return rather than loop, then when only a reliable packet was 
	available we wouldn't arm the ack-required condition.  

****************************************************************************/

void MUXSender(BCSPStack * stack)
{
	Packet * pkt = null ;

	while(1) 
	{
		//wait for the slip-sender to be available - (the slip-sender only has one packet
		//slot rather than a queue to prevent problems with window-rollbacks.

		if (stack->SLIPInputPacket) SYNCBLKwaitSignal(stack,&stack->SLIPSendAvailableSignal) ;

		//we prefer to send an unreliable packet if one is available
		pkt=ASYNCBLKgetPacketFromBuffer(stack,&stack->MUXUnreliableInput) ;


		//otherwise, we'll take a reliable one if one is available
		if (!pkt) pkt = ASYNCBLKremoveFirstUnsentPacketFromWindow(stack,&stack->theXMITWindow) ;

		//the last resort is to send a dedicated ack packet but we only need 
		//to do this if an ack is definitely required
		if (!pkt)
		{
			if (!stack->ackRequired) SYNCBLKwaitSignal(stack,&stack->txackSignal) ; 
			//blocking on the signal has the side-effect of also blocking on the
			//previous conditions so we'll be woken up by anything
			stack->ackRequired = false ;
			pkt = &stack->theAckPacket ; //use the dedicated ack packet
		}

		//ok - at this point we have a packet and we're intent on sending it

		setAck(pkt, stack->txack) ; //set the correct acknowledgement number
		
		//add the checksum to the header after setting crc
		if (stack->configuration->useCRC) setPacketHasCRC(pkt) ;
		setPacketChecksum(pkt) ;

		//pass it on to SLIP layer
        // JBS: no GetTickCount on POSIX
        //   BTRACE5(MUX,"MUX sending pkt %p seq %d ack %d len %d time %d\n",pkt,getSeq(pkt),getAck(pkt),getLength(pkt), GetTickCount() ) ;
		BTRACE4(MUX,"MUX sending pkt %p seq %d ack %d len %d\n",pkt,getSeq(pkt),getAck(pkt),getLength(pkt)) ;
		
		stack->SLIPInputPacket = pkt ;
		setSignal(stack,&stack->MUXHasPacketForSLIPSignal) ;
	}
}
