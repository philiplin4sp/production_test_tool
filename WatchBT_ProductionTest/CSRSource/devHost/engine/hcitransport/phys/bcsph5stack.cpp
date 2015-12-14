///////////////////////////////////////////////////////////////////////
//
//  FILE   :  BCSPH5Stack.cpp
//
//  Copyright CSR 2002-2006
//
//  AUTHOR :  Mark Marshall
//
//  CLASS  :  BCSPH5Stack
//
//  PURPOSE:  Generic portion of BCSP implementation of HCITransport
//			  This is based on H5StackBase.
//
///////////////////////////////////////////////////////////////////////

#include "bcsph5stack.h"

#include <climits>
#include <cstring>
#include <cassert>
#include "common/algorithm.h"

#include "time/hi_res_clock.h"
#include "thread/error_msg.h"


const uint8 BCSPH5Stack::sync_payload[LEMSG_HEADER_LEN]      = { 0xda, 0xdc, 0xed, 0xed };
const uint8 BCSPH5Stack::sync_resp_payload[LEMSG_HEADER_LEN] = { 0xac, 0xaf, 0xef, 0xee };
const uint8 BCSPH5Stack::conf_payload[LEMSG_HEADER_LEN]      = { 0xad, 0xef, 0xac, 0xed };
const uint8 BCSPH5Stack::conf_resp_payload[LEMSG_HEADER_LEN] = { 0xde, 0xad, 0xd0, 0xd0 };

/* This is a table of the two byte payloads that we expect to recieve from the peer */
const BCSPH5Stack::payload_table_s BCSPH5Stack::payload_table[] =
{
	{ lemsgid_sync,			sync_payload },
	{ lemsgid_sync_resp,	sync_resp_payload },
	{ lemsgid_conf,			conf_payload },
	{ lemsgid_conf_resp,	conf_resp_payload },
	{ lemsgid_none,			0 }
};


/////////////////////////////////////////////////////////////////////////
// NAME
//  BCSPH5Stack  -  ctor

BCSPH5Stack::BCSPH5Stack(UARTPtr aFile, CallBackInterface &callBacks,
		uint32 Tretx /*= 250*/, uint32 Tshy /*= 250*/, uint32 Tconf /*= 250*/,
		bool useCRCs /*= true*/, uint8 winsize /*= 4*/,
		bool muzzleAtStart /*= false*/)
	: H5StackBase(aFile, callBacks, Tretx, winsize),
	m_timerTshy(Tshy),
	m_timerTconf(Tconf),
	m_useCRCs(useCRCs),
	m_le_state(le_state_uninit), m_le_muzzled(muzzleAtStart),
	m_txsync_req(true), m_txsyncresp_req(false),
	m_txconf_req(false), m_txconfresp_req(false)
{
	// Simple sanity checking
	if (m_windowSize < 1)
		m_windowSize = 1;
	else if (m_windowSize > 7)
		m_windowSize = 7;

	// Bind the object & function to the three timers
	m_timerTshy.Bind(this, &BCSPH5Stack::le_tshy);
	m_timerTconf.Bind(this, &BCSPH5Stack::le_tconf);

	// Add the timers to the list of timers
	m_timerList.push_back(&m_timerTshy);
	m_timerList.push_back(&m_timerTconf);
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  ~BCSPH5Stack  -  dtor

/*virtual*/ BCSPH5Stack::~BCSPH5Stack ()
{
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  MapProtocolToWire  -  Map packet type to wire channel number

/*virtual*/ uint8 BCSPH5Stack::MapProtocolToWire(PDU::bc_channel proto)
{
	return uint8(proto);
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  MapWireToProtocol  -  Map wire channel number to packet type

/*virtual*/ PDU::bc_channel BCSPH5Stack::MapWireToProtocol(uint8 wire)
{
	return PDU::bc_channel(wire);
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  SendCRCs  -  Should we add a CRC to a general packet

/*virtual*/ bool BCSPH5Stack::SendCRCs()
{
	return m_useCRCs;
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  SendLECRCs  -  Should we add a CRC to a LE packet

/*virtual*/ bool BCSPH5Stack::SendLECRCs()
{
	return m_useCRCs;
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  bcsple_fsm  -  The BCSP Link Establishment engine

void BCSPH5Stack::bcsple_fsm(lemsgid msg)
{
	bool send_msg = false;

    switch (m_le_state)
    {
    default:
    case le_state_uninit:
		switch (msg)
		{
		case lemsgid_tshy_timeout:
			if (!m_le_muzzled)
			{
				m_txsync_req = true;
				send_msg = true;
				m_timerTshy.SetTimer();
			}
			break;

		case lemsgid_sync:
			if (m_le_muzzled)
			{
				// If we have been muzzled until now we need to send some sync messages
				m_le_muzzled = false;
				m_txsync_req = true;
				m_timerTshy.SetTimer();
			}
			m_txsyncresp_req = true;
			send_msg = true;
			break;

		case lemsgid_sync_resp:
			ERROR_MSG(("LE-State -> INIT\n"));
			m_le_state = le_state_init;
			m_txconf_req = true;
			send_msg = true;
			m_timerTconf.SetTimer();
			break;

		default:
			break;
		}
		break;

    case le_state_init:
		switch (msg)
		{
		case lemsgid_tconf_timeout:
			m_txconf_req = true;
			send_msg = true;
			m_timerTconf.SetTimer();
			break;

		case lemsgid_sync:
			m_txsyncresp_req = true;
			send_msg = true;
			break;

		case lemsgid_conf:
			m_txconfresp_req = true;
			send_msg = true;
			break;

		case lemsgid_conf_resp:
			ERROR_MSG(("LE-State -> ACTIVE\n"));
			mLEDone.set();
			m_le_state = le_state_active;
			m_choke = false;
			break;

		default:
			break;
		}
		break;

    case le_state_active:
		switch (msg) 
		{
		case lemsgid_sync:
			ERROR_MSG(("LE-State -> UNINIT (Got sync - peer reset?!)\n"));
			mCallBacks.linkFailed(bcsp_sync_recvd);
			break;

		case lemsgid_conf:
			m_txconfresp_req = true;
			send_msg = true;
			break;

		default:
			break;
		}
		break;
	}

	if (send_msg)
	    mFile->reqXmit();
}


/////////////////////////////////////////////////////////////////////////////
// NAME
//  le_start  -  Start the LE engine

/*virtual*/ void BCSPH5Stack::le_start()
{
	if (!m_le_muzzled)
	{
		m_txsync_req = true;
		m_timerTshy.SetTimer();
		mFile->reqXmit();
	}
}


/////////////////////////////////////////////////////////////////////////////
// NAME
//  le_message_avail  -  is there a new LE transmit message available

/*virtual*/ uint32 BCSPH5Stack::le_message_avail(uint8 *data)
{
	if (m_txsync_req || m_txsyncresp_req || m_txconf_req || m_txconfresp_req)
	{
        if (m_txsync_req)
		{
			m_txsync_req = false;
			memcpy(data, sync_payload, LEMSG_HEADER_LEN);
			return LEMSG_HEADER_LEN;
		}
		else if (m_txsyncresp_req)
		{
            m_txsyncresp_req = false;
			memcpy(data, sync_resp_payload, LEMSG_HEADER_LEN);
			return LEMSG_HEADER_LEN;
		}
        else if (m_txconf_req)
		{
            m_txconf_req = false;
			memcpy(data, conf_payload, LEMSG_HEADER_LEN);
			return LEMSG_HEADER_LEN;
		}
        else if (m_txconfresp_req)
		{
			m_txconfresp_req = false;
			memcpy(data, conf_resp_payload, LEMSG_HEADER_LEN);
			return LEMSG_HEADER_LEN;
		}
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  le_putmsg  -  we have recieved a new LE message.

/*virtual*/ void BCSPH5Stack::le_putmsg(uint32 len)
{
	lemsgid msgid = lemsgid_none;

	if (len == LEMSG_HEADER_LEN)
	{
		for (int i = 0; payload_table[i].id != lemsgid_none; ++i)
			if (memcmp(&m_rxbuf[4], payload_table[i].data, LEMSG_HEADER_LEN) == 0)
				msgid = payload_table[i].id;
	}

	bcsple_fsm(msgid);
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  le_tconf  -  The T-Conf timer has expired, maybe send a conf message

void BCSPH5Stack::le_tconf()
{
	bcsple_fsm(lemsgid_tconf_timeout);
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  le_tshy  -  The T-Shy timer has expired, maybe send a sync message

void BCSPH5Stack::le_tshy()
{
    bcsple_fsm(lemsgid_tshy_timeout);
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  ForceAwake  -  Force the peer into life

/*virtual*/ bool BCSPH5Stack::ForceAwake()
{
	return true;
}
