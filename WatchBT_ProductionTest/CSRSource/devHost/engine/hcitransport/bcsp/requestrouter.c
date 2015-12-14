/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        RequestRouter.c  -  request router task

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating
	1.3  28-apr-00 npm  - added data integrity check
	1.4  13-mar-01 at   - added length check to tracing code
*/


#ifdef	RCS_STRINGS
static  char    requestrouter_c_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/requestrouter.c#1 $";
#endif

#include "memory.h"
#include "bcspstack.h"
#include "scheduler.h"
#include "api.h"

/****************************************************************************
NAME
	RequestRouter

FUNCTION
	this task takes requests from the input queue and sends them to either
	the reliable or unreliable queues.

IMPLEMENTATION NOTE
	this is a task.

****************************************************************************/


void RequestRouter(BCSPStack * stack)
{
	BCTransferRequestQueue * reqQueue ;
	BCTransferRequest * req ;

	//wait for the stack to be unchoked 
	if (!stack->isUnchoked) SYNCBLKwaitSignal(stack,&stack->unchokeSignal) ; 
	
	//now wait for a request on any channel
	//if we find one, try to allocate a packet and assign the request to it
	reqQueue = &stack->requestRouterInput ;

	//we can only peek for the request because we might fail on getting the  packet
	req = SYNCBLKpeekAtHeadOfTransferRequestQueue(stack,reqQueue) ;
	if (req)
	{
		Packet * pkt ;
		//now try to get a packet - if we fail, we block and come back later
		if (getCallbackType(stack,req->channel) == BCSPCallbackImmediate)
		{
			pkt = SYNCBLKgetPacketFromPool(stack,modeXMIT,req->length) ;
			memcpy(pkt->data,req->data,req->length) ;
			doCallback(req,transferComplete) ;		
		}
		else 
		{
			pkt = SYNCBLKgetPacketFromPool(stack,modeXMIT,0) ;
			pkt->data = req->data ;			//just point to data from client
			if (pkt->data)
			{
				if (getLength(pkt) < 4) BTRACE1(DATAINTEGRITY,"RR pkt len %d\n",getLength(pkt)) ;
				else if (getLength(pkt) < 8) BTRACE2(DATAINTEGRITY,"RR pkt len %d data %x\n",getLength(pkt),*(uint32*)(pkt->data)) ;
				else BTRACE3(DATAINTEGRITY,"RR pkt len %d data %x %x\n",getLength(pkt),*(uint32*)(pkt->data),*(uint32*)(pkt->data+4)) ;
			}
			setLength(pkt,req->length) ;	//set length
			pkt->request = req ;			//attach the transfer request
		}

		//if we've got a packet, we need to consume the transfer request
		SYNCBLKgetTransferRequestFromQueue(stack,reqQueue) ;
		//now set up the packet and transfer details...
		setProtocolId(pkt,req->channel) ;		//choose the channel 
		
		BTRACE2(REQROUTER,"RR - dispatching request chan %d len %d\n",req->channel,req->length) ;
		setProtocolType(pkt,getProtocolType(stack,req->channel)) ; 
	
		if (getProtocolType(stack,req->channel) ==BCSPChannelReliable)
		{
			//pass the (reliable) packet on to the SEQUENCE sender 
			BTRACE2(REQROUTER,"RR - dispatching reliable pkt chan %d len %d\n",req->channel,req->length) ;
			addPacketToBuffer(stack,&stack->SEQInput,pkt) ;
		}
		else
		{
			//pass the (unreliable) packet on to the MUX sender 
			BTRACE2(REQROUTER,"RR - dispatching unreliable pkt chan %d len %d\n",req->channel,req->length) ;
			addPacketToBuffer(stack,&stack->MUXUnreliableInput,pkt) ;

		}
		return  ;
	}

	//if we get down here, we failed to find a single request  (or failed 
	//to obtain a packet) so block on any conditions that caused us to wait
	block(stack) ;
}
