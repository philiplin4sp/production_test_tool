///////////////////////////////////////////////////////////////////////
//
//  FILE   :  H5Stack.cpp
//
//  Copyright CSR 2002-2006
//
//  AUTHOR :  Mark Marshall
//
//  CLASS  :  H5Stack
//
//  PURPOSE:  H5 Stack implementation for HCITransport
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/phys/h5stack.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

#include <climits>
#include <cstring>
#include <cassert>
#include "common/algorithm.h"

#include "h5stack.h"

#include "time/hi_res_clock.h"
#include "thread/error_msg.h"


const uint8 H5Stack::sync_payload[2]        = { 0x01, 0x7e };
const uint8 H5Stack::sync_resp_payload[2]   = { 0x02, 0x7d };
const uint8 H5Stack::config_payload[2]      = { 0x03, 0xfc }; /* + config data */
const uint8 H5Stack::config_resp_payload[2] = { 0x04, 0x7b }; /* + config data */
const uint8 H5Stack::wakeup_payload[2]      = { 0x05, 0xfa };
const uint8 H5Stack::woken_payload[2]       = { 0x06, 0xf9 };
const uint8 H5Stack::sleep_payload[2]       = { 0x07, 0x78 };

/* This is a table of the two byte payloads that we expect to recieve from the peer */
const H5Stack::payload_table_s H5Stack::payload_table[] =
{
	{ lemsgid_sync,			sync_payload },
	{ lemsgid_sync_resp,	sync_resp_payload },
	{ lemsgid_config,		config_payload },
	/*{ lemsgid_config_resp,	config_resp_payload },*/
	{ lemsgid_wakeup,		wakeup_payload },
	{ lemsgid_woken,		woken_payload },
	{ lemsgid_sleep,		sleep_payload },
	{ lemsgid_none,			0 }
};


/////////////////////////////////////////////////////////////////////////
// NAME
//  H5Stack  -  ctor

H5Stack::H5Stack(UARTPtr aFile, CallBackInterface &callBacks,
		uint32 Tretx /*= 250*/, uint32 Tshy /*= 250*/, uint32 Tconf /*= 250*/,
		bool useCRCs /*= true*/, bool useOOF /*= false*/, uint8 winsize /*= 4*/,
		uint32 Twakeup /*= 50*/, bool muzzleAtStart /*= false*/)
	: H5StackBase(aFile, callBacks, Tretx, winsize),
	m_timerTshy(Tshy),
	m_timerTconf(Tconf),
	m_timerTwakeup(Twakeup),
	m_useCRCs(useCRCs), m_useOOf(useOOF),
	m_le_state(le_state_uninit), m_le_muzzled(muzzleAtStart),
	m_txsync_req(true), m_txsyncresp_req(false),
	m_txconf_req(false), m_txconfresp_req(false),
	m_txwakeup_req(false), m_txwoken_req(false),
	m_txsleep_req(false), m_peer_sleep_state(h5sleep_awake)
{
	// Simple sanity checking
	if (m_windowSize < 1)
		m_windowSize = 1;
	else if (m_windowSize > 7)
		m_windowSize = 7;

	// Bind the object & function to the three timers
	m_timerTshy.Bind(this, &H5Stack::le_tshy);
	m_timerTconf.Bind(this, &H5Stack::le_tconf);
	m_timerTwakeup.Bind(this, &H5Stack::le_twakeup);

	// Add the timers to the list of timers
	m_timerList.push_back(&m_timerTshy);
	m_timerList.push_back(&m_timerTconf);
	m_timerList.push_back(&m_timerTwakeup);
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  ~H5Stack  -  dtor

/*virtual*/ H5Stack::~H5Stack ()
{
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  HostSleep  -  The host is going to sleep

void H5Stack::HostSleep()
{
	h5le_fsm(lemsgid_send_sleep);
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  HostWakeup  -  The host is awake (and wants to talk to the peer)

void H5Stack::HostWakeup()
{
	h5le_fsm(lemsgid_send_wakeup);
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  MapProtocolToWire  -  Map packet type to wire channel number

/*virtual*/ uint8 H5Stack::MapProtocolToWire(PDU::bc_channel proto)
{
	switch (proto)
	{
	case PDU::zero:
		return IO_DATA_H5_PROTOCOL_ACK;

	case PDU::le:
		return IO_DATA_H5_PROTOCOL_LINK_MGT;

		// These three channels need to be remapped.  The H5 channel
		// numbers do not match the BCSP channel numbers.
	case PDU::hciACL:
		return IO_DATA_H5_PROTOCOL_HCI_ACL;

	case PDU::hciSCO:
		return IO_DATA_H5_PROTOCOL_HCI_SCO;

	case PDU::hciCommand:
		return IO_DATA_H5_PROTOCOL_HCI_CMD;

	default:
		ERROR_MSG(("H5: Unknown Protocol?"));
		assert(0);
		break;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  MapWireToProtocol  -  Map wire channel number to packet type

/*virtual*/ PDU::bc_channel H5Stack::MapWireToProtocol(uint8 wire)
{
	static const PDU::bc_channel wire2proto[16] =
	{
		PDU::zero,
		PDU::hciCommand,	// HCI Command
		PDU::hciACL,
		PDU::hciSCO,
		PDU::hciCommand,	// HCI Event
		PDU::h2hc,
		PDU::hc2h,
		PDU::dm,
		PDU::l2cap,
		PDU::rfcomm,
		PDU::sdp,
		PDU::debug,
		PDU::upgrade,
		PDU::vm,
		PDU::fourteen,
		PDU::le
	};
	assert(wire <= 15);
	return wire2proto[wire];
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  SendCRCs  -  Should we add a CRC to a general packet

/*virtual*/ bool H5Stack::SendCRCs()
{
	return m_useCRCs;
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  SendLECRCs  -  Should we add a CRC to a LE packet

/*virtual*/ bool H5Stack::SendLECRCs()
{
	// We never add CRCs to LE packets under H5
	return false;
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  h5le_fsm  -  The H5 Link Establishment engine
//
// FUNCTION
//  This function is the LE state machine.  It is passed lemsgid's and
//  move from state to state.  This function is the bulk of this entire 
//  class.

void H5Stack::h5le_fsm(lemsgid msg)
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

		case lemsgid_config:
			// The message from the host contains configuration data.
			// The configuration response message to the host contains
			// our reply.
			m_txconfresp_req = true;
			send_msg = true;
			break;

		case lemsgid_config_resp:
			// There is no data in the config_resp message from the
			// host.
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

		case lemsgid_config:
			m_txconfresp_req = true;
			send_msg = true;
			break;

		case lemsgid_wakeup:
			ERROR_MSG(("The peer is waking us up.\n"));
			m_peer_sleep_state = h5sleep_awake;
			m_txwoken_req = true;
			send_msg = true;
			break;

		case lemsgid_woken:
			ERROR_MSG(("The peer is awake.\n"));
			m_peer_sleep_state = h5sleep_awake;
			// We dont have an LE message to send, but we might have packets
			// queued that we did not send because the peer was asleep
			send_msg = true;
			break;

		case lemsgid_sleep:
			ERROR_MSG(("The peer is asleep.\n"));
			m_peer_sleep_state = h5sleep_asleep;
			break;

		case lemsgid_send_sleep:
			ERROR_MSG(("We are asleep.\n"));
			m_peer_sleep_state = h5sleep_asleep;
			m_txsleep_req = true;
			send_msg = true;
			break;

		case lemsgid_send_wakeup:
			if (m_peer_sleep_state == h5sleep_asleep)
			{
				ERROR_MSG(("We are trying to wake the host.\n"));
				m_peer_sleep_state = h5sleep_waking;
				m_txwakeup_req = true;
				send_msg = true;
				m_timerTwakeup.SetTimer();
				break;
			}
			break;

		case lemsgid_twakeup_timeout:
			if (m_peer_sleep_state != h5sleep_awake)
			{
				ERROR_MSG(("We are still trying to wake the host.\n"));
				m_peer_sleep_state = h5sleep_waking;
				m_txwakeup_req = true;
				send_msg = true;
				m_timerTwakeup.SetTimer();
				break;
			}
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

/*virtual*/ void H5Stack::le_start()
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

/*virtual*/ uint32 H5Stack::le_message_avail(uint8 *data)
{
	if (m_txsync_req || m_txsyncresp_req || m_txconf_req || m_txconfresp_req
		|| m_txwakeup_req || m_txwoken_req || m_txsleep_req)
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
			memcpy(data, config_payload, LEMSG_HEADER_LEN);
			data[2] = (m_useCRCs ? 0x10 : 0x00) | (m_useOOf ? 0x08 : 0x00) | (m_windowSize & 0x07);
			return LEMSG_HEADER_LEN + 1;
		}
        else if (m_txconfresp_req)
		{
			m_txconfresp_req = false;
			memcpy(data, config_resp_payload, LEMSG_HEADER_LEN);
			return LEMSG_HEADER_LEN;
		}
		else if (m_txwakeup_req)
		{
			m_txwakeup_req = false;
			memcpy(data, wakeup_payload, LEMSG_HEADER_LEN);
			return LEMSG_HEADER_LEN;
		}
		else if (m_txwoken_req)
		{
			m_txwoken_req = false;
			memcpy(data, woken_payload, LEMSG_HEADER_LEN);
			return LEMSG_HEADER_LEN;
		}
		else if (m_txsleep_req)
		{
			m_txsleep_req = false;
			memcpy(data, sleep_payload, LEMSG_HEADER_LEN);
			return LEMSG_HEADER_LEN;
		}
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  le_putmsg  -  we have recieved a new LE message.

/*virtual*/ void H5Stack::le_putmsg(uint32 len)
{
	lemsgid msgid = lemsgid_none;

	if (len == LEMSG_HEADER_LEN)
	{
		for (int i = 0; payload_table[i].id != lemsgid_none; ++i)
			if (m_rxbuf[4] == payload_table[i].data[0] && m_rxbuf[5] == payload_table[i].data[1])
				msgid = payload_table[i].id;
	}
	else if(len == LEMSG_HEADER_LEN+1)
	{
		if (m_rxbuf[4] == config_resp_payload[0] && m_rxbuf[5] == config_resp_payload[1])
		{
			msgid = lemsgid_config_resp;
			// Get the configuration data from the peer
			m_useCRCs = (m_rxbuf[6] & 0x10) != 0;
			m_useOOf = (m_rxbuf[6] & 0x08) != 0;
			m_windowSize = m_rxbuf[6] & 0x07;
		}
	}

	h5le_fsm(msgid);
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  ForceAwake  -  Force the peer into life

/*virtual*/ bool H5Stack::ForceAwake()
{
	switch (m_peer_sleep_state)
	{
	case h5sleep_asleep:
		h5le_fsm(lemsgid_send_wakeup);
		return false;

	case h5sleep_waking:
		return false;
	default:
		break;
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  le_tconf  -  The T-Conf timer has expired, maybe send a conf message

void H5Stack::le_tconf()
{
	h5le_fsm(lemsgid_tconf_timeout);
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  le_tshy  -  The T-Shy timer has expired, maybe send a sync message

void H5Stack::le_tshy()
{
    h5le_fsm(lemsgid_tshy_timeout);
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  le_twakeup  -  The T-Wakeup timer has expired, maybe ?
//
// FUNCTION
//  The Wakeup timer is fairly protocol independant.  It is only used
//  in H5.  In H5 it is used to send wakeup message tos the peer.  It is
//  quite likely that the peer will not respond to the first one that we
//  send, so we send a few.

/*virtual*/ void H5Stack::le_twakeup()
{
	h5le_fsm(lemsgid_twakeup_timeout);
}
