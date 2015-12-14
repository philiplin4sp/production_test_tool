/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        Shutdown.c  -  shutdown and cleanup  services

MODIFICATION HISTORY
	1.3  27-apr-00 npm  - added boilerplating
	1.4   9-oct-00 npm  - fixed shutdown leak of packets in xmitwindow
	1.5   9-oct-00 npm  - fixed shutdown leak of packets in xmitwindow (really!) 
    1.6   1-nov-00 npm  - fixed incorrect callback for slip input packet
	                      during shutdown
*/


#ifdef	RCS_STRINGS
static  char    shutdown_c_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/shutdown.c#1 $";
#endif

#include "bcspstack.h"

void disposePacket(BCSPStack * stack,Packet *pkt)
{
	if (!pkt) return ;
	doCallback(pkt->request,transferCancelled) ;
	//it doesn't matter which mode we use since they're all 
	//going to be freed anyway
	returnPacketToPool(stack,pkt,modeRCV) ;
}

void disposePacketBuffer(BCSPStack * stack, PacketBuffer *buf) 
{
	Packet * pkt ;
	while (peekAtPacketInBuffer(buf))
	{
		pkt = SYNCBLKgetPacketFromBuffer(stack,buf) ;
		disposePacket(stack,pkt) ;
	}
}

void disposeTransferRequestQueue(BCSPStack * stack,BCTransferRequestQueue * q)
{
	BCTransferRequest * req ;
	while ((req = ASYNCBLKgetTransferRequestFromQueue(stack,q)))
	{
		// originally we got a packet here and used disposePacket to do the 
		// appropriate callback but that led to problems with pool accounting 
		// causing a block and thus exception.  So it's easier (and safer)
		//just to call the callback directly 
		doCallback(req,transferCancelled) ;
	}
}

void BCSPshutdownStack(BCSPStack * stack)
{
	
	Packet * pkt ;
	int  c ;
	//run the scheduler to ensure that user transfer requests queues are drained
	//it's also important to empty the output buffer.  This ensures that the 
	//SlipSender can't get blocked and so a pending abort can be processed,
	//resulting in a coherent stack state.  Without this, SLIPinputPacket can be 
	//a packet that has just been aborted and called back

	while (numEntriesInByteBuffer(&stack->SLIPByteOutput)) 
			readByteFromByteBuffer(stack,&stack->SLIPByteOutput) ;
	//also drain the input buffer for completeness
	while (numEntriesInByteBuffer(&stack->SLIPByteInput)) 
			readByteFromByteBuffer(stack,&stack->SLIPByteInput) ;
	 

	scheduler(stack,0) ;
	
	//now return all packets to the packet pool, calling callbacks as we go...
	//NB - once we start disposing of packets, pool-accounting is 
	//in an unstable state and we must not attempt to get any packets back out again

	disposePacket(stack,stack->SLIPInputPacket) ;
	
	disposePacketBuffer(stack,&stack->MUXUnreliableInput) ;
	disposePacketBuffer(stack,&stack->MUXUnreliableInput) ;
	disposePacketBuffer(stack,&stack->SEQInput) ;
	//now do the receive side ...
	disposePacketBuffer(stack,&stack->PacketDelivererInput) ;
	disposePacketBuffer(stack,&stack->LinkEstablishmentInput) ;
	disposePacketBuffer(stack,&stack->RCVInputBuffer) ;
	disposePacket(stack,stack->theSLIPState.RCVpkt) ;


	//clear transfer request that never got associated with packets...
	disposeTransferRequestQueue(stack,&stack->requestRouterInput) ;
	for (c = 0 ; c < 16 ; c++)
		disposeTransferRequestQueue(stack,&stack->readRequests[c]) ;


	//now empty out the xmit window - it's possible that someone tried to do a write 
	//and then link-establishment failed.

	rollbackXMITWindow(stack,&stack->theXMITWindow) ;
	while ((pkt = ASYNCBLKremoveFirstUnsentPacketFromWindow(stack,&stack->theXMITWindow)))
	{
	    if (pkt != stack->SLIPInputPacket) disposePacket(stack,pkt) ;
	}


	//now all the packets should be back in the packet pool with their
	//callbacks having been cancelled and memory returned.  The only thing
	//left to do is to return the packets themselves.
	while (peekAtPacketInBuffer(&stack->thePacketPool.buffer))
	{
		pkt = SYNCBLKgetPacketFromBuffer(stack,&stack->thePacketPool.buffer) ;
		stack->environment->releasePacket(stack->environment->envState,pkt) ;
	}
	
}
