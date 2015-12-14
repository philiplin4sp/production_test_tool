/****************************************************************************

	Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
	XMITWindow.h  -  defines bcsp xmit window

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating

*/

#ifndef _XMITWINDOW_H_
#define _XMITWINDOW_H_

#ifdef	RCS_STRINGS
static  char    xmitwindow_h_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/xmitwindow.h#1 $";
#endif



#include "bcsp_signal.h"

struct _BCSPStack ;

/****************************************************************************
STRUCTURE
	XMITWindow

FUNCTION
	the xmitwindow keeps track of packets which have been transmitted but
	not acknowledged.  It has a limited size and can signal to tasks when 
	it has space or entries.

FIELDS
	Window - the packets in the xmitwindow
	windowHasUnsentEntriesSignal - used to signal when entries available for transmision
	notFullSignal - used to signal when more packets can be added
	windowSize - max number of entries in window
	firstUnacknowedgedSlot - first sent but unacknowledged packet
	firstFreeSlot - first empty slot
	numUnacknowledgedEntries - the number of packets that have been sent but not acked
	firstUnsentSlot - pointer to first packet which has not yet been transmitted
  
IMPLEMENTATION NOTE
	The window is implememted as a FIFO

****************************************************************************/

typedef struct
{

	
	Packet * Window[8] ;
	Signal windowHasUnsentEntriesSignal ;
	Signal notFullSignal ;
	uint8 windowSize ;
	uint8 firstUnacknowledgedSlot ;
	uint8 firstFreeSlot  ;
	uint8 numUnsentEntries  ;
	uint8 numUnacknowledgedEntries  ;
	uint8 firstUnsentSlot  ;
	
} XMITWindow ;

extern void setXMITWindowSize(XMITWindow * win,uint8 n) ;
extern void addPacketToWindow(struct _BCSPStack * stack,XMITWindow * win,Packet * pkt);
extern bool ASYNCBLKwaitForXMITWindowSpace(struct _BCSPStack * stack,XMITWindow * win);
extern Packet * ASYNCBLKremoveFirstUnsentPacketFromWindow(struct _BCSPStack * stack,XMITWindow * win) ;
extern bool doesWindowContainUnacknowledgedPackets(XMITWindow * win) ;

extern void rollbackXMITWindow(struct _BCSPStack * stack,XMITWindow * win) ;

#endif
