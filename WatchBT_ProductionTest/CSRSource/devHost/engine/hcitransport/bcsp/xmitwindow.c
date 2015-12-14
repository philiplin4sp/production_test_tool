/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        XMITWindow.c  -  transmit window for bcsp stack

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating
	1.3  16-oct-00 npm  - fixed bug in debug version of rollback trying to report empty window

$Log: xmitwindow.c,v $
Revision 1.1  2002/03/27 18:35:19  ajh
Moved and renamed from
devHost/HostStack/singledll/

Revision 1.1  2001/06/29 12:56:55  ajh
First adding, temporarily, of a single dll version of the hoststack,
including bluetest as the test app.

Revision 1.6  2001/05/31 17:56:19  mr01
Delinting.

Revision 1.5  2001/05/22 15:10:51  cl01
Reworking for CE/Win32 merge


*/


#ifdef	RCS_STRINGS
static  char    xmitwindow_c_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/xmitwindow.c#1 $";
#endif
#include <stddef.h>
#include "packet.h"
#include "bcspstack.h"
#include "api.h"
#include "slipsender.h"

/****************************************************************************
NAME
	setXMITWindowSize

FUNCTION
	sets the window size 

IMPLEMENTATION NOTE

****************************************************************************/


void setXMITWindowSize(XMITWindow * win,uint8 n)
{
	win->windowSize = n ;
}

/****************************************************************************
NAME
	addPacketToWindow

FUNCTION
	adds a packet to the xmit window - assumes at least one free slot. 

IMPLEMENTATION NOTE
	This is called by the sequence sender and must
	signal the MUXSender that there is an packet to send

****************************************************************************/


void addPacketToWindow(struct _BCSPStack * stack,XMITWindow * win,Packet * pkt)
{
	//add the packet
	win->Window[win->firstFreeSlot] = pkt ;
	win->firstFreeSlot++ ;
	if (win->firstFreeSlot == win->windowSize) win->firstFreeSlot = 0 ; //wrap it
	win->numUnacknowledgedEntries ++ ;
	win->numUnsentEntries++ ;
	//signal the mux sender
	setSignal(stack,&win->windowHasUnsentEntriesSignal) ;
}

/****************************************************************************
NAME
	ASYNCBLKwaitForXMITWindowSpace

FUNCTION
	waits for there to be space in the xmitwindow.  Blocks if there isn't
	but returns anyway so the task can try something else.

IMPLEMENTATION NOTE
	This is called by the sequence sender to see when it can add a packet
	if there is no space, we need to wait for the receiver to remove a packet
	which will occur when the peer sends an ack for a packet(s) which have been
	received

****************************************************************************/


bool ASYNCBLKwaitForXMITWindowSpace(struct _BCSPStack * stack,XMITWindow * win)
{
	if ((win->numUnsentEntries == win->windowSize) || 
		(win->numUnacknowledgedEntries == win->windowSize))
	{
		ASYNCBLKwaitSignal(stack,&win->notFullSignal) ;
		return false ;
	}
	return true ;
}

/****************************************************************************
NAME
	ReleaseXMITWindowAcknowledgedSlots

FUNCTION
	frees all slots for packets that have just been acked.

IMPLEMENTATION NOTE
	this advances through the window, freeing up packets that have been
	acknowledged.  This should cause some slots to free up which will
	unblock the SequenceSender.  There's a slight complication in that 
	a previous window rollback can result in us getting an ack for a packet
	that the slipsender is in the middle of sending.  Obviously it would be 
	a 'bad thing' to disappear this from under the slip-senders nose so if we
	find that we're about to do this, we force the slip-sender to jump to 
	its end state.This should result in the peer receivig a bad packet 
	and silently dropping it.

****************************************************************************/


void ReleaseXMITWindowAcknowledgedSlots(BCSPStack * stack) 
{
	XMITWindow * win ;
	win= & stack->theXMITWindow ;

	//walk through the window discarding acknowledged slots
	while ((win->numUnacknowledgedEntries) &&
		  (getSeq(win->Window[win->firstUnacknowledgedSlot]) != stack->rxack))
		  
	{
		Packet * pkt = win->Window[win->firstUnacknowledgedSlot] ;
		
		//check whether this is the packet that the slip-sender is sending
		if (pkt == stack->SLIPInputPacket) 
		{
			BTRACE0(RCV,"Aborting slip send\n") ;
			abortSLIPSend(stack) ;
		}

		//call the callback - we have to do it here rather than 
		//the slip sender because of retransmissions
		doCallback(pkt->request,transferComplete) ;

		returnPacketToPool(stack,pkt,modeXMIT) ;
		win->numUnacknowledgedEntries -- ;
		win->firstUnacknowledgedSlot ++ ;
		if (win->firstUnacknowledgedSlot == win->windowSize)  win->firstUnacknowledgedSlot = 0; 
	}

	//There's a scenario where the number of unacknowledged entries can be less than the 
	//number of unsent entries !  (Another way of thinking of this is that there can be 
	//packets which have already been acknowledged despite apparently being unsent!)
	//This can occur if we time out and reset the xmit window then an
	//ack comes in after all. Eg we send 0,1,2,3, timeout and resend 0 then a delayed
	//ack from the peer for 0,1,2,3 arrives.  The code above will take care of removing
	//the acknowledged packets from the window but we need also to adjust the number of 
	//unsent entries accordingly.
	
	if (win->numUnacknowledgedEntries < win->numUnsentEntries)
	{
		win->firstUnsentSlot = win->firstUnacknowledgedSlot ;
		win->numUnsentEntries = win->numUnacknowledgedEntries ;
		setSignal(stack,&win->notFullSignal) ;
	}

	//now clear the retries flag and put the resender back to sleep
	//since we know beyond doubt that we've done
	//a successful send
	resetTimer(stack,&stack->resendTimer,stack->configuration->resendTimeout) ;
	stack->XMITretries = 0 ;

	//Now signal the sequence sender so that it can swamp us with yet more packets
	setSignal(stack,&win->notFullSignal) ;
}

/****************************************************************************
NAME
	ASYNCBLKremoveFirstUnsentPacketFromWindow

FUNCTION
	gets a packet from the window for transmission

IMPLEMENTATION NOTE
	Called by the MUXSender to obtain a candidate packet for transmission
	Obviously, removing a packet from the window will free up space and allow the 
	sequence sender to give us another one so we give it a prod.

****************************************************************************/

Packet * ASYNCBLKremoveFirstUnsentPacketFromWindow(struct _BCSPStack * stack,XMITWindow * win)
{
	Packet * pkt = null ;

	if (win->numUnsentEntries!=0) 
	{
		//all's well - there are packets a'plenty
		pkt = win->Window[win->firstUnsentSlot] ;
		win->firstUnsentSlot ++ ;
		if (win->firstUnsentSlot == win->windowSize) win->firstUnsentSlot = 0 ;
		win->numUnsentEntries-- ;
		//signal seq-sender that it can give us another packet
		setSignal(stack,&win->notFullSignal) ;
	}
	else 
	{
		//unfortunately there aren't any packets for transmission today so 
		//we'll wait for one to appear.  This can happen either through the normal
		//means of a packet being added by the Sequence Sender or by a timeout causing
		//the window to be rolled back.
		ASYNCBLKwaitSignal(stack,&win->windowHasUnsentEntriesSignal) ;
	}
	return pkt ;
}

/****************************************************************************
NAME
	doesWindowContainUnacknowledgedPackets

FUNCTION
	returns true if there are unacknowledged packets in the window (ie packets
	which have been transmitted but not yet acked)

IMPLEMENTATION NOTE

****************************************************************************/

bool doesWindowContainUnacknowledgedPackets(XMITWindow * win)
{
	return (win->numUnacknowledgedEntries != 0) ;
}

/****************************************************************************
NAME
	rollbackXMITWindow

FUNCTION
	in the event of a transmission timeout, this function is called to  
	cause any previously-sent-but-unacknowledged packets to be retransmitted

IMPLEMENTATION NOTE
	This will result in the mux sender being woken up to send the packets

****************************************************************************/


void rollbackXMITWindow(struct _BCSPStack * stack,XMITWindow * win) 
{

	win->firstUnsentSlot = win->firstUnacknowledgedSlot ;
	win->numUnsentEntries = win->numUnacknowledgedEntries ;
	//now signal the mux-sender but only if there are actually some unsent entries
	if (win->numUnsentEntries) setSignal(stack,&win->windowHasUnsentEntriesSignal) ;
  
    //NB - the window can now be empty on rollback due to this function being called 
    //from the shutdown code
    BTRACE2(SEQ,"Window rolled back to first unsent %d at pos %d\n",
        win->Window[win->firstUnsentSlot] ?  getSeq(win->Window[win->firstUnsentSlot]) : 0,win->firstUnsentSlot) ;
}
