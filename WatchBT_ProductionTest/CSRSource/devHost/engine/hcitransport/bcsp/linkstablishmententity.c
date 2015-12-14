/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        LinkEstablishmentEntity.c  -  link establishment entity  

MODIFICATION HISTORY
	1.3  27-apr-00 npm  - added boilerplating
	1.4  17-jan-01 mr01 - added detection of remote reset (BC01-S-010-e)

*/


#ifdef	RCS_STRINGS
static  char    linkestablishment_c_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/linkstablishmententity.c#1 $";
#endif


#include <memory.h>
#include "bcspstack.h"
#include "scheduler.h"

static const uint8 syncPacketData[] = {0xda,0xdc,0xed,0xed} ;
static const uint8 syncRespPacketData[] = {0xac,0xaf,0xef,0xee} ;
static const uint8 confPacketData[] = {0xad,0xef,0xac,0xed} ;
static const uint8 confRespPacketData[] = {0xde,0xad,0xd0,0xd0} ;

/****************************************************************************
NAME
	LinkEstablishmentSender

FUNCTION
	Periodically sends out sync packets until the stack is unchoked

IMPLEMENTATION NOTE
	This task is paired with LinkEstablishment receive which monitors incoming
	packets.

****************************************************************************/

void LinkEstablishmentSender(BCSPStack * stack)
{
	//periodically send out link establishment 'sync' packets 

	Packet * pkt ;

	if (stack->loquacity == bcsp_garrulous && stack->confCnt == stack->configuration->confLimit)
	{
		//block ourselves forever
		block(stack) ;
	}

	BTRACE0(LINKEST,"Link Establishment - waiting for packet...\n") ;

	//get a packet for the sync we're about to send
	pkt=SYNCBLKgetPacketFromPool(stack,modeXMIT,0) ;

	if (stack->loquacity == bcsp_curious )
	{
		BTRACE0(LINKEST,"Link Establishment - sending conf packet\n") ;
		pkt->data = (uint8*) confPacketData ;
		++stack->confCnt ;
	}
	else
	{
		BTRACE0(LINKEST,"Link Establishment - sending sync packet\n") ;
		pkt->data = (uint8*) syncPacketData ;
	}
	setProtocolId(pkt,1) ;
	setProtocolType(pkt,BCSPChannelUnreliable) ;
	setLength(pkt,sizeof(syncPacketData) /* == sizeof(confPacketData) */) ;
	addPacketToBuffer(stack,&stack->MUXUnreliableInput,pkt) ;

	//the wait MUST come at the end since otherwise we never get to execute 
	//anything below it

	BTRACE0(LINKEST,"Link Establishment - setting timer and waiting...\n") ;
	SYNCBLKwaitForTimer(stack,&stack->LinkEstablishmentTimer,stack->isUnchoked ? stack->configuration->TConf : stack->configuration->TShy) ;
}


/****************************************************************************
NAME
	LinkEstablishmentReceive

FUNCTION
	Responds to incoming sync packets with 'sync response' and unblocks the
	stack if a sync-response is received.

IMPLEMENTATION NOTE
	This task is paired with LinkEstablishmentSender.

****************************************************************************/

void LinkEstablishmentReceive(BCSPStack * stack)
{

	Packet * pkt = SYNCBLKgetPacketFromBuffer(stack,&stack->LinkEstablishmentInput);

	//check packet length as well to be rigorous
	if ( (getLength(pkt) == sizeof(syncPacketData)) &&
		(!memcmp(pkt->data,syncPacketData,sizeof(syncPacketData)))) 
	{
		//lose the packet 
		returnPacketToPool(stack,pkt,modeRCV) ;
		if (stack->loquacity == bcsp_garrulous )
		{
			BTRACE0(LINKEST,"Link Establishment - got sync packet again - peer has rebooted!\n") ;
			stack->environment->onLinkFailureSync(stack->environment->envState) ;
		}
		else
		{
			BTRACE0(LINKEST,"Link Establishment - got sync packet - sending response\n") ;
			//it's tempting to reuse this packet but this would confuse the 
			//packet-pool accounting system so we return it (above) and get another;
			//it's possible this could fail in which case we'd have 
			//to wait for another sync packet to appear before we could try again
			pkt = SYNCBLKgetPacketFromPool(stack,modeXMIT,0) ;
			pkt->data = (uint8*) syncRespPacketData ;
			setLength(pkt,sizeof(syncRespPacketData)) ;
			setProtocolId(pkt,1) ;
			setProtocolType(pkt,BCSPChannelUnreliable) ;
			addPacketToBuffer(stack,&stack->MUXUnreliableInput,pkt) ;
		}
	}
	else
	if ( (getLength(pkt) == sizeof(confPacketData)) &&
		(!memcmp(pkt->data,confPacketData,sizeof(confPacketData)))) 
	{
		//lose the packet 
		returnPacketToPool(stack,pkt,modeRCV) ;
		if (stack->loquacity == bcsp_shy)
		{
			BTRACE0(LINKEST,"Link Establishment - got conf packet - huh?\n") ;
		}
		else
		{
			BTRACE0(LINKEST,"Link Establishment - got conf packet - sending response\n") ;
			//it's tempting to reuse this packet but this would confuse the 
			//packet-pool accounting system so we return it (above) and get another;
			//it's possible this could fail in which case we'd have 
			//to wait for another conf packet to appear before we could try again
			pkt = SYNCBLKgetPacketFromPool(stack,modeXMIT,0) ;
			pkt->data = (uint8*) confRespPacketData ;
			setLength(pkt,sizeof(confRespPacketData)) ;
			setProtocolId(pkt,1) ;
			setProtocolType(pkt,BCSPChannelUnreliable) ;
			addPacketToBuffer(stack,&stack->MUXUnreliableInput,pkt) ;
		}
	}
	else
	if ((getLength(pkt) == sizeof(syncRespPacketData)) &&
		(!memcmp(pkt->data,syncRespPacketData,sizeof(syncRespPacketData)))) 
	{
		//lose the packet 
		returnPacketToPool(stack,pkt,modeRCV) ;
		if (stack->loquacity == bcsp_shy)
		{
			BTRACE0(LINKEST,"Link Establishment - got syncresp - now curious\n") ;
			//  set to curious
			stack->loquacity = bcsp_curious;
			//stop Tshy and start Tconf
			resetTimer(stack,&stack->LinkEstablishmentTimer,stack->configuration->TConf) ;
		}
		else
		{
			BTRACE0(LINKEST,"Link Establishment - got syncresp - huh?\n") ;
		}
	}
	else
	if ((getLength(pkt) == sizeof(confRespPacketData)) &&
		(!memcmp(pkt->data,confRespPacketData,sizeof(confRespPacketData)))) 
	{
		//lose the packet 
		returnPacketToPool(stack,pkt,modeRCV) ;
		if (stack->loquacity == bcsp_curious)
		{
			BTRACE0(LINKEST,"Link Establishment - got confresp - now garrulous\n") ;
			//block the conf sender...
			stack->confCnt = stack->configuration->confLimit ;
			//we've made it!
			stack->loquacity = bcsp_garrulous;
			stack->isUnchoked = true ;
			setSignal(stack,&stack->unchokeSignal); //wake up rest of stack
		}
		else
		{
			BTRACE0(LINKEST,"Link Establishment - got confresp - huh?\n") ;
		}
	}
	else 
	{
		BTRACE0(LINKEST,"Link Establishment - got unrecognised packet\n") ;
		returnPacketToPool(stack,pkt,modeRCV) ;
	}

	
}
