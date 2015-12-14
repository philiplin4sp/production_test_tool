/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        BCSPStack.h  -  make BCSPStack public

MODIFICATION HISTORY
	1.3  27-apr-00 npm  - added boilerplating
	1.4  17-jan-01 mr01 - added detection of remote reset (BC01-S-010-e)

*/

#ifndef _BCSPSTACK_H_
#define _BCSPSTACK_H_

#include <setjmp.h>

#include "bcsp_linkage.h"
#include "timer.h"
#include "packetbuffer.h"
#include "xmitwindow.h"
#include "slip.h"
#include "bcsp_signal.h"
#include "transferrequest.h"
#include "bytebuffer.h"
#include "packetpool.h"
#include "channelcontrol.h"
#include "stackconfiguration.h"
#include "api.h"
#include "btrace.h"
#include "scheduler.h"
#include "environment.h"

typedef struct _BCSPStack
{
  jmp_buf schedulerState ;
  BCSPEnvironment * environment ;
  Queue incomingReadRequests ;
  Queue incomingWriteRequests ;
  PacketBuffer MUXUnreliableInput  ;
  PacketBuffer SEQInput  ;
  PacketBuffer RCVInputBuffer ;
  PacketBuffer PacketDelivererInput ;
  PacketBuffer LinkEstablishmentInput ;
  Packet *  SLIPInputPacket ;
  Signal SLIPSendAvailableSignal ;
  Signal MUXHasPacketForSLIPSignal ;
  PacketPool thePacketPool ;
  ByteBuffer SLIPByteOutput ;
  ByteBuffer SLIPByteInput ;
  StackConfiguration * configuration ;
  uint32 lastPacketSentAt ;
  uint32 lastSyncSentAt ;
  uint32 timeNow ;
  uint32 wakeupTime ;
  Timer resendTimer ;
  Timer LinkEstablishmentTimer ;

  XMITWindow theXMITWindow ;
  BCTransferRequestQueue  requestRouterInput ;
  BCTransferRequestQueue  unreliableWriteRequests ;
  BCTransferRequestQueue  readRequests[16] ;
  ChannelControl channels[16] ;
  SLIPState theSLIPState ;
  Signal txackSignal ;
  Signal unchokeSignal ;
  uint32 currentTaskId ;
  Packet theAckPacket ;
  uint16 confCnt ;
  uint16 waitMask ;
  uint8 XMITretries ;
  uint8 txack ;
  uint8 rxack ;
  uint8 seq ;
  enum { bcsp_shy , bcsp_curious , bcsp_garrulous } loquacity;
  bool isUnchoked ;
  bool ackRequired ;

} BCSPStack ;

#include "shutdown.h"

BCSP_API void  initialiseStack(struct _BCSPStack * stack) ;
BCSP_API uint16 numBytesInTransmitBuffer(BCSPStack * stack) ;
BCSP_API uint16 numFreeSlotsInReceiveBuffer(BCSPStack * stack) ;
BCSP_API uint8 readByteFromTransmitBuffer(BCSPStack * stack) ;
BCSP_API void writeByteToReceiveBuffer(BCSPStack * stack,uint8 data) ;
BCSP_API void readFromTransmitBuffer(BCSPStack * stack,uint8* dest,uint32 len) ;
BCSP_API void writeToReceiveBuffer(BCSPStack * stack,uint8 *src,uint32 len) ;
BCSP_API bool BCSPLinkEstablished(BCSPStack * stack) ;
BCSP_API void BCSPaddPacket(struct _BCSPStack * theStack,Packet * pkt) ;

extern void ReleaseXMITWindowAcknowledgedSlots(BCSPStack * theStack) ;

#endif
