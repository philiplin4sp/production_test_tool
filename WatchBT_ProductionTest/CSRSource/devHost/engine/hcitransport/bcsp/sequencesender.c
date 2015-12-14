/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        SequenceSender.c  -  sends reliable packets 

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating
	1.3  16-nov-00 at   - resend timeout reset when packet transmitted with
	                      no outstanding acknowledgements

*/


#ifdef	RCS_STRINGS
static  char    sequencesender_c_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/sequencesender.c#1 $";
#endif

#include <stdio.h>

#include "packetbuffer.h"
#include "xmitwindow.h"
#include "bcspstack.h"
#include "scheduler.h"
#include "timer.h"

/****************************************************************************
NAME
	Resender

FUNCTION
	this is the entity responsible for resending the contents of the 
	transmit window if the resend-timeout expires

IMPLEMENTATION NOTE
	This runs as a task.

****************************************************************************/



void Resender(BCSPStack * stack)
{

	XMITWindow * win = &stack->theXMITWindow ;

	while(1)
	{

		//there's no point in doing a resend if there aren't actually any
		//unacknowledged packets so check here
		if (doesWindowContainUnacknowledgedPackets(win))
		{
			BTRACE2(SEQ,"Resender rolling window back - time %d retries %d\n",stack->timeNow,stack->XMITretries);
			rollbackXMITWindow(stack,win) ;		//this may unblock the mux-sender
			stack->XMITretries++ ;				//indicate another retry
			
			//if the maximum number of retries has been exceeded, signal an error
			//by calling the environment callback responsible for doing something
			//about this
			if (stack->configuration->retryLimit &&
			(stack->XMITretries >= stack->configuration->retryLimit))
			{
				stack->environment->onLinkFailureRetries(stack->environment->envState) ;
				stack->XMITretries = 0 ;
			}
		}
		//the wait MUST come at the end since otherwise we never get to execute 
		//anything below it
		SYNCBLKwaitForTimer(stack,&stack->resendTimer,stack->configuration->resendTimeout) ;
	
	}
}

/****************************************************************************
NAME
	SequenceSender

FUNCTION
	this is the entity responsible for sending reliable packets.  It attaches 
	a sequence number to each one.

IMPLEMENTATION NOTE
	This runs as a task.

****************************************************************************/


void SequenceSender(BCSPStack * stack)
{
	Packet * pkt =null;
	XMITWindow * win = &stack->theXMITWindow ;

	//we can only queue up a packet if there is space in the transmit window and 
	//(obviously) there is a packet to send

	if (ASYNCBLKwaitForXMITWindowSpace(stack,win))
	{
		//ok - there is space but do we have a packet ?
		pkt = ASYNCBLKgetPacketFromBuffer(stack,&stack->SEQInput) ;

		if (pkt) 
		{
			//YES - we do have a packet and there is space so send it 
			if (!doesWindowContainUnacknowledgedPackets(win))
			{
				// Reset the resent timeout if no acknowledgements already pending
				resetTimer(stack,&stack->resendTimer,stack->configuration->resendTimeout) ;
			}

			setSeq(pkt,stack->seq) ;			//set the sequence number
			stack->seq = (stack->seq+1) & 7 ;	//and bump it up for the next packet
			addPacketToWindow(stack,win,pkt) ;	//add packet to xmit window
			BTRACE2(SEQ,"SEQ sending pkt %p seq %d\n",pkt,getSeq(pkt));
			return ;
		}
	}
	//we got here because either there was no window slot or else there was 
	//no packet so block on whichever combination of those conditions applied
	block(stack) ;
}
