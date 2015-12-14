///////////////////////////////////////////////////////////////////////
//
//  FILE   :  H4DSStack,cpp
//
//  AUTHOR :  Mark Marshall
//				(based on code by Mark RISON and Carl Orsborn)
//
//  CLASS  :  H4DSStack
//
//  PURPOSE:  Implementation of H4DS for HCITransport
//
//            This code is based quite strongly on H4DS (carls example
//            implimentation),  I have not used the code directly (sorry
//            carl) but have cut and pasted most of the functions
//            accross.  The architecture of the host code means that
//            it makes moresense if this is one class that does
//            everything that the H4DS stack needs.
//
//            The UART model used buy the host code is quite unusual.
//            Reads and Writes are assumed to never fail.  We can write
//            an arbitatarily large block in one go, but we can only
//            write one block at a time.
//
///////////////////////////////////////////////////////////////////////

#include <cstring>
#include "common/algorithm.h"
#include <cassert>
#define ASSERT assert
#include <stdio.h>

#include "h4dsstack.h"
#include "transportapi.h"
#include "time/passive_timer.h"


/* h4ds messages' fixed "sync" preamble. */
const uint8 H4DSStack::h4ds_lock_msgfrag[H4DS_MSGFRAG_LOCK_LEN] =
    {0x40, 0x0a, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0x50};

/* h4ds "You May Sleep" message, equivalent to {'y', 'm', 's', '\0'}. */
const uint8 H4DSStack::h4ds_yms_msgfrag[H4DS_MSGFRAG_BODY_LEN] =
    {0x79, 0x6d, 0x73, 0x00};

/* h4ds "Wake Up (tick)" message, equivalent to {'t', 'i', 'c', 'k'}. */
const uint8 H4DSStack::h4ds_pre_wu_msgfrag[H4DS_MSGFRAG_BODY_LEN] = 
    {0x74, 0x69, 0x63, 0x6b};

/* h4ds "Wake Up (train)" message, equivalent to {'\0', '\0', '\0', '\0'}. */
const uint8 H4DSStack::h4ds_prez_wu_msgfrag[H4DS_MSGFRAG_BODY_LEN] = 
    {0x00, 0x00, 0x00, 0x00};

/* h4ds "Wake Up" message, equivalent to {'w', 'u', '\0', '\0'}. */
const uint8 H4DSStack::h4ds_wu_msgfrag[H4DS_MSGFRAG_BODY_LEN] = 
    {0x77, 0x75, 0x00, 0x00};

/* h4ds "I'm Awake" message, equivalent to {'i', 'a', '\0', '\0'}. */
const uint8 H4DSStack::h4ds_ia_msgfrag[H4DS_MSGFRAG_BODY_LEN] = 
    {0x69, 0x61, 0x00, 0x00};

/* h4ds link establishment "Sync" message, equiv to {'s', 'y', 'n', 'c'}. */
const uint8 H4DSStack::h4ds_sync_msgfrag[H4DS_MSGFRAG_BODY_LEN] = 
    {0x73, 0x79, 0x6e, 0x63};

/* h4ds link establishment "Sync Ack" msg, equiv to {'S', 'a', 'c', 'k'}. */
const uint8 H4DSStack::h4ds_sack_msgfrag[H4DS_MSGFRAG_BODY_LEN] = 
    {0x53, 0x61, 0x63, 0x6b};

/* h4ds link establishment "Conf" message, equiv to {'c', 'o', 'n', 'f'}. */
const uint8 H4DSStack::h4ds_conf_msgfrag[H4DS_MSGFRAG_BODY_LEN] = 
    {0x63, 0x6f, 0x6e, 0x66};

/* h4ds link establishment "Conf Ack" msg, equiv to {'C', 'a', 'c', 'k'}. */
const uint8 H4DSStack::h4ds_cack_msgfrag[H4DS_MSGFRAG_BODY_LEN] = 
    {0x43, 0x61, 0x63, 0x6b};

/* The complete set of h4ds messages. */
const H4DSStack::MSGINFO H4DSStack::h4ds_msgs[] =
{
    { h4ds_pre_wu_msgfrag,  msgid_pre_wu  },
    { h4ds_prez_wu_msgfrag, msgid_prez_wu },
    { h4ds_wu_msgfrag,      msgid_wu      },
    { h4ds_ia_msgfrag,      msgid_ia      },
    { h4ds_yms_msgfrag,     msgid_yms     },
    { h4ds_sync_msgfrag,    msgid_sync    },
    { h4ds_sack_msgfrag,    msgid_sack    },
    { h4ds_conf_msgfrag,    msgid_conf    },
	{ h4ds_cack_msgfrag,    msgid_cack    },
    { NULL,					msgid_none    }
};

// This is the maximum time that we wait for some UART activity (it can
// be as large as we want, but sometimes we get funny behaviour from
// windows if it is too large).
static const int MAX_WAIT_TIME = 5000;

/////////////////////////////////////////////////////////////

H4DSStack::H4DSStack(UARTPtr aFile, CallBackInterface &callBacks, uint32 wakeupLength)
 :	mFile(aFile),
	mDirection(WeAreHost),				// default to being the host
	mCallBacks(callBacks),
	// DS
	mDsState(state_init),
	mDsNWakeup(0),
	mDsUartActive(false),
	mDsTxBufferEmptyCheckPending(false),
	mDsUartRxAlarm(false),
	// SCANNER
	mScannerState(scst_lock),
	mScannerIndex(0),
	// LE
	mLeState(state_shy),
	mLePaused(false),
	mLePausedTsync(false),
	mLePausedTconf(false),
	// DS RX
	mDsRxState(st_scanning),
	// DS TX
	mReqTxSync(false),
	mReqTxSack(false),
	mReqTxConf(false),
	mReqTxCack(false),
	mPumpReq(false),
	// H4 RX
	mRxState(st_idle),
	mRxPktType(0),
	mRxTag(0),
	mRxPktLength(0),
	mRxH4InSync(true),
	// H4 TX
	// UART RX
	mRxNeededLength(1),
	// UART TX
	mTxInProgress(false),
	//
	mThreadDead(mThreadSignal)
{
	// We need to make sure that the Hi Res clock is working
	HiResClock res;
	InitHiResClock(&res);

	// Setup the DS timers
	mTimerTxEmptyAlarm.Bind(this, &H4DSStack::ds_timer_tx_empty_alarm);
	mTimerWakeup.Bind(this, &H4DSStack::ds_timer_wakeup);
	mTimerFlush.Bind(this, &H4DSStack::ds_timer_flush);
	mTimerUartIdle.Bind(this, &H4DSStack::ds_timer_uart_idle);
	m_timerList.push_back(&mTimerTxEmptyAlarm);
	m_timerList.push_back(&mTimerWakeup);
	m_timerList.push_back(&mTimerFlush);
	m_timerList.push_back(&mTimerUartIdle);

	// Setup the LE timers
	mTimerSync.Bind(this, &H4DSStack::le_timer_tsync);
	mTimerConf.Bind(this, &H4DSStack::le_timer_tconf);
	m_timerList.push_back(&mTimerSync);
	m_timerList.push_back(&mTimerConf);

	// Work out how many preamble chunks to send
	// Each chunk is H4DS_WU_PREAMBLE_MSG_LEN bytes long
	// There are ten bits to a byte

	// This is the number of its per chunk (times 1000 to account 
	// for millisecond->second conversion.
	const int bits_per_chunk = 10 * H4DS_MSG_LEN;

	// use rounding instead of truncation (which happens if all variables are ints)
	int WakePreambleSize =
		( ((float)(mFile->getBaudRate() * wakeupLength) / 
		   (float)(bits_per_chunk * 1000)) + 0.5);

	// Make the wakeup message.
	mWakeupMsg.reserve(H4DS_MSG_LEN * (2 + WakePreambleSize));
	mWakeupMsg.clear();

	uint8 temp[H4DS_MSG_LEN];
	memcpy(temp, h4ds_lock_msgfrag, H4DS_MSGFRAG_LOCK_LEN);

	memcpy(temp + H4DS_MSGFRAG_LOCK_LEN, h4ds_pre_wu_msgfrag, H4DS_MSGFRAG_BODY_LEN);
	for (int i=0; i<WakePreambleSize; i++)
		mWakeupMsg.insert(mWakeupMsg.end(), temp, temp + H4DS_MSG_LEN);

	memcpy(temp + H4DS_MSGFRAG_LOCK_LEN, h4ds_prez_wu_msgfrag, H4DS_MSGFRAG_BODY_LEN);
	mWakeupMsg.insert(mWakeupMsg.end(), temp, temp + H4DS_MSG_LEN);

	memcpy(temp + H4DS_MSGFRAG_LOCK_LEN, h4ds_wu_msgfrag, H4DS_MSGFRAG_BODY_LEN);
	mWakeupMsg.insert(mWakeupMsg.end(), temp, temp + H4DS_MSG_LEN);

	// Try to reduce reallocations
	mRxPacketBuffer.reserve(260),

	// Init DS
	ds_sm(dsevtid_none);

	// Init LE
	mTimerSync.SetPeriod(TSYNC_PERIOD);
	mTimerConf.SetPeriod(TCONF_PERIOD);
}

H4DSStack::~H4DSStack ()
{
    Stop();
    HiResClockSleepMilliSec(0);
    ASSERT (!IsActive());
}

void H4DSStack::setDataWatch(DataWatcher *w)
{
    mFile->setDataWatch(w);
}

void H4DSStack::setUartDirection(bool aUartGoesToChip)
{
	// UartGoesToChip = We are the host
    mDirection = aUartGoesToChip ? WeAreHost : WeAreController;
}

bool H4DSStack::Start ()
{
	if (!mFile->open())
	{
		DB_PRINTF(("H4DS: Cannot open UART!\n"));
		return false;
	}

	if (!Threadable::Start())
	{
		DB_PRINTF(("H4DS: Cannot start worker thread!\n"));
		return false;
	}

    //Threadable::HighPriority();
	
	return true;
}

void H4DSStack::Stop()
{
    if (IsActive())
    {
		Threadable::Stop();

        DB_PRINTF (("H4DS: asking serial daemon to die\n"));
        mFile->abortWait();
        mThreadDead.wait();
    }
}

void H4DSStack::sendCommand(const PDU &aPacket)
{
	ASSERT(aPacket.channel() == PDU::hciCommand);
    ASSERT(mDirection == WeAreHost);
    if (mDirection == WeAreHost)
        sendPDU(aPacket, H4_PKTTYPE_CMD);
}

void H4DSStack::sendACLData (const PDU &aPacket)
{
	ASSERT(aPacket.channel() == PDU::hciACL);
    sendPDU(aPacket, H4_PKTTYPE_ACL);
}

void H4DSStack::sendSCOData (const PDU &aPacket)
{
	ASSERT(aPacket.channel() == PDU::hciSCO);
    sendPDU(aPacket, H4_PKTTYPE_SCO);
}

void H4DSStack::sendRawData (const PDU &aPacket)
{
    ASSERT(aPacket.channel() == PDU::raw);
    sendPDU(aPacket, H4_PKTTYPE_RAW);
}

void H4DSStack::sendEvent (const PDU &aPacket)
{
	ASSERT(aPacket.channel() == PDU::hciCommand);
    ASSERT(mDirection == WeAreController);
    if (mDirection == WeAreController)
        sendPDU(aPacket, H4_PKTTYPE_EVT);
}

void H4DSStack::sendPDU (const PDU &aPacket, uint8 header)
{
	// Check to see if we are running, if we are not then dont
	// send this packet.
	if (mThreadSignal.wait(0))
	{
		// Call the callback though - stop the higher layer
		// from locking up
		mCallBacks.sentPacket(false);
		return;
	}
    ASSERT(IsActive());
	mTxQueue.addNewPacket(aPacket, header, true);
	mFile->reqXmit();
}

/////////////////////////////////////////////////////////////////////////////
// NAME
//  H4DSStack::CallBackInterface::sentPacket  -  Packet has been sent

/*virtual*/ void H4DSStack::CallBackInterface::sentPacket(bool sentOk)
{
}

/////////////////////////////////////////////////////////////////////////////
// NAME
//  H4DSStack::CallBackInterface::recvPacket  -  Packet was recieved

/*virtual*/ void H4DSStack::CallBackInterface::recvPacket(const PDU &aPacket)
{
}

/////////////////////////////////////////////////////////////////////
// NAME
//  H4DSStack::CallBackInterface::syncLost  -  The transport has crashed
//
// FUNCTION
//  This callback gets called when the stack has recieved a
//  bad packet (or the DS engine has detected errors).
//
/*virtual*/ void H4DSStack::CallBackInterface::syncLost(FailureMode mode)
{
}

#include "time/stop_watch.h"
#include "thread/signal_box.h"

bool H4DSStack::ready(uint32 timeout)
{
    StopWatch stopWatch;

    // Poll every 100 ms for timeout ms, or until the transport is ready.
    while (stopWatch.duration() <= timeout)
    {
        if (IsActive() && !le_rx_choked())
        {
            // Transport is ready
            return true;
        }
        else
        {
            SignalBox().wait(100);
        }
    }

    // Time out expired.
    return false;
}

// ==== DEBUG ====

// This is a debug routine that outputs an array of bytes.  Used for
// dumping out packets / packet fragments.
/*static*/ void H4DSStack::DumpPacket(const char *desc, const uint8 *contents, uint32 length)
{
#ifndef NDEBUG
	DB_PRINTF (("%s", desc));
	if (length != 0)
	{
		for (uint32 u = 0; u < length; ++u)
		{
			DB_PRINTF ((" %02x", contents[u]));
		}
		DB_PRINTF ((" [len %u]\n", length));
	}
	else
	{
		DB_PRINTF ((" none\n"));
	}
#endif
}


void H4DSStack::H4DS_EVENT(H4DS_EVENTS e)
{
#define EVT(e) case (e): DB_PRINTF(("H4DS: Event %s (%d)\n", #e, (e))); break;
	switch (e)
	{
		EVT(H4DS_EVT_START)
		EVT(H4DS_EVT_INITED)
		EVT(H4DS_EVT_PEER_MAY_SLEEP)
		EVT(H4DS_EVT_PEER_AWAKE)
		EVT(H4DS_EVT_LOCAL_DEVICE_MAY_SLEEP)
		EVT(H4DS_EVT_LOCAL_DEVICE_MAY_NOT_SLEEP)
		EVT(H4DS_EVT_LE_SYNC)
		EVT(H4DS_EVT_LE_CONF)
		EVT(H4DS_H4_EVT_DUFF_TX_PARAMS)
		EVT(H4DS_H4_EVT_OVERSIZE_RX_PAYLOAD)

		//EVT(H4DS_EVT_SYNC_LOSS)
		case H4DS_EVT_SYNC_LOSS:
			DB_PRINTF(("H4DS: Event %s (%d)\n", "H4DS_EVT_SYNC_LOSS", H4DS_EVT_SYNC_LOSS));
			mCallBacks.syncLost(h4_sync_loss);
			break;

		//EVT(H4DS_EVT_PEER_RESTART)
		case H4DS_EVT_PEER_RESTART:
			DB_PRINTF(("H4DS: Event %s (%d)\n", "H4DS_EVT_PEER_RESTART", H4DS_EVT_PEER_RESTART));
			mCallBacks.syncLost(bcsp_sync_recvd);
			break;

		//EVT(H4DS_H4_EVT_H4_SYNC_LOST)
		case H4DS_H4_EVT_H4_SYNC_LOST:
			DB_PRINTF(("H4DS: Event %s (%d)\n", "H4DS_H4_EVT_H4_SYNC_LOST", H4DS_H4_EVT_H4_SYNC_LOST));
			mCallBacks.syncLost(h4_sync_loss);
			break;
	}
#undef EVT
}

void H4DSStack::H4DS_PANIC(H4DS_PANICS p)
{
	DB_PRINTF(("H4DS: PANIC %d!\n", p));
	// It's not clear what we should do here
	// - hopefully this will never happen
	mCallBacks.syncLost(no_failure);
}

// ==== H4 RX ====

/****************************************************************************
NAME
	h4rx_delivermsg  -  push a complete H4 message up the stack

FUNCTION
		This function pushes the completed H4 message up the stack.
*/

inline void H4DSStack::h4rx_delivermsg()
{
	ASSERT(mRxPacketBuffer.size() == mRxPktLength);

	DB_DUMPPKT(("H4DS: Got Packet", &mRxPacketBuffer[0], mRxPacketBuffer.size()));

    switch (mRxPktType)
    {
    case H4_PKTTYPE_CMD:
		mCallBacks.recvPacket(PDU(PDU::hciCommand, &mRxPacketBuffer[0], mRxPacketBuffer.size()));
        break;

    case H4_PKTTYPE_ACL:
        mCallBacks.recvPacket(PDU(PDU::hciACL, &mRxPacketBuffer[0], mRxPacketBuffer.size()));
        break;

    case H4_PKTTYPE_SCO:
        mCallBacks.recvPacket(PDU(PDU::hciSCO, &mRxPacketBuffer[0], mRxPacketBuffer.size()));
        break;

    case H4_PKTTYPE_EVT:
        mCallBacks.recvPacket(PDU(PDU::hciCommand, &mRxPacketBuffer[0], mRxPacketBuffer.size()));
        break;

	default:
		ASSERT(0);
		break;
	}

	mRxPacketBuffer.clear();
}

/****************************************************************************
NAME
	h4rx_deliverbytes  -  push bytes up into the h4 message decoder

FUNCTION
	Pushes the "bufsiz" bytes of UART data at "buf" up into the H4
	message decoder.  If the bytes contain an entire H4 message, or if
	they form the end of a message being constructed, then the resulting
	HCI message is pushed up into higher layer code.

	This function sets the Boolean "msg_start" flag on return to indicate
	to the caller that on the next call to this function it will be
	seeking the start of a fresh H4 message.

	If this function spots an error in the H4 message's structure then it
	signals H4 sync loss, expecting external code to invoke H4's pathetic
	error recovery mechanism.

RETURNS
	The number of bytes consumed from buf.
*/

uint32 H4DSStack::h4rx_deliverbytes(const uint8 *buf, uint32 bufsiz, bool &msg_start)
{
    uint32 i, copylen;

    for(i = 0; i < bufsiz; i++, buf++)
    {
		switch (mRxState)
		{
		case st_idle:	/* Start of H4 message. */
				/* First byte is channel id. */
			switch(mRxPktType = *buf)
			{
			case H4_PKTTYPE_CMD:  /* HCI command. */
				mRxState = st_cmdhdr1;
				mRxNeededLength = 3;
				break;
			case H4_PKTTYPE_ACL:  /* HCI ACL. */
				mRxState = st_datahdr1;
				mRxNeededLength = 4;
				break;
			case H4_PKTTYPE_SCO:  /* HCI SCO. */
				mRxState = st_scohdr1;
				mRxNeededLength = 3;
				break;
			case H4_PKTTYPE_EVT:  /* HCI event. */
				mRxState = st_evthdr1;
				mRxNeededLength = 3;
				break;
			default:
				/* Illegal H4 channel. */
				if(mRxH4InSync)
				{
					mRxH4InSync = false;
					H4DS_EVENT(H4DS_H4_EVT_H4_SYNC_LOST);
				}
				msg_start = true;
				return i + 1;
			}
			break;

		case st_cmdhdr1:    /* HCI command opcode 1. */
			mRxPacketBuffer.push_back(*buf);
			mRxNeededLength--;
			mRxState = st_cmdhdr2;
			break;
		case st_cmdhdr2:    /* HCI command opcode 2. */
			mRxPacketBuffer.push_back(*buf);
			mRxNeededLength--;
			mRxState = st_cmdhdr3;
			break;
		case st_cmdhdr3:    /* HCI command length. */
			mRxPacketBuffer.push_back(*buf);
			mRxPktLength = *buf;
			mRxNeededLength = mRxPktLength;
			mRxPktLength += 3;
			if (mRxPktLength == 3)
			{
				h4rx_delivermsg();
				mRxNeededLength = 1;
				mRxState = st_idle;
				msg_start = true;
				return i + 1;
			}
			else
			{
				mRxState = st_body;
			}
			break;

		case st_datahdr1:   /* HCI data handle low. */
			mRxPacketBuffer.push_back(*buf);
			mRxNeededLength--;
			mRxState = st_datahdr2;
			break;
		case st_datahdr2:   /* HCI data handle high. */
			mRxPacketBuffer.push_back(*buf);
			mRxNeededLength--;
			mRxState = st_datahdr3;
			break;
		case st_datahdr3:   /* HCI length low. */
			mRxPacketBuffer.push_back(*buf);
			mRxPktLength = *buf;
			mRxNeededLength--;
			mRxState = st_datahdr4;
			break;
		case st_datahdr4:   /* HCI length high. */
			mRxPacketBuffer.push_back(*buf);
			mRxPktLength |= ((uint16)*buf) << 8;
			mRxNeededLength = mRxPktLength;
			mRxPktLength += 4;
			if (mRxPktLength > H4DS_H4_MAX_RX_DATA_PAYLOAD + 4)
			{
				H4DS_EVENT(H4DS_H4_EVT_OVERSIZE_RX_PAYLOAD);
				if (mRxH4InSync)
				{
					mRxH4InSync = false;
					H4DS_EVENT(H4DS_H4_EVT_H4_SYNC_LOST);
				}
				mRxNeededLength = 1;
				mRxState = st_idle;
				msg_start = true;
				return i + 1;
			}
			else if (mRxPktLength == 4)
			{
				h4rx_delivermsg();
				mRxNeededLength = 1;
				mRxState = st_idle;
				msg_start = true;
				return i + 1;
			}
			else
			{
				mRxState = st_body;
			}
			break;

		case st_scohdr1:   /* SCO data handle low. */
			mRxPacketBuffer.push_back(*buf);
			mRxNeededLength--;
			mRxState = st_scohdr2;
			break;
		case st_scohdr2:   /* SCO data handle high. */
			mRxPacketBuffer.push_back(*buf);
			mRxNeededLength--;
			mRxState = st_scohdr3;
			break;
		case st_scohdr3:   /* SCO length. */
			mRxPacketBuffer.push_back(*buf);
			mRxPktLength = *buf;
			mRxNeededLength = mRxPktLength;
            mRxPktLength += 3;
			if (mRxPktLength > H4DS_H4_MAX_RX_DATA_PAYLOAD + 3)
			{
				H4DS_EVENT(H4DS_H4_EVT_OVERSIZE_RX_PAYLOAD);
				if (mRxH4InSync)
				{
					mRxH4InSync = false;
					H4DS_EVENT(H4DS_H4_EVT_H4_SYNC_LOST);
				}
				mRxNeededLength = 1;
				mRxState = st_idle;
				msg_start = true;
				return i + 1;
			}
			else if (mRxPktLength == 3)
			{
				h4rx_delivermsg();
				mRxNeededLength = 1;
				mRxState = st_idle;
				msg_start = true;
				return i + 1;
			}
			else
			{
				mRxState = st_body;
			}
			break;

		case st_evthdr1:    /* HCI event opcode. */
			mRxPacketBuffer.push_back(*buf);
			mRxNeededLength--;
			mRxState = st_evthdr2;
			break;
		case st_evthdr2:    /* HCI event length. */
			mRxPacketBuffer.push_back(*buf);
			mRxPktLength = *buf;
			mRxNeededLength = mRxPktLength;
			mRxPktLength += 2;
			if (mRxPktLength == 2)
			{
				h4rx_delivermsg();
				mRxNeededLength = 1;
				mRxState = st_idle;
				msg_start = true;
				return i + 1;
			}
			else
			{
				mRxState = st_body;
			}
			break;

		case st_body:       /* Receiving message body. */
			copylen = std::min((unsigned long)(bufsiz - i), (unsigned long)(mRxPktLength - mRxPacketBuffer.size()));
			mRxNeededLength -= copylen;
			ASSERT(copylen >= 1);
			mRxPacketBuffer.insert(mRxPacketBuffer.end(), buf, buf + copylen);
			buf += copylen - 1;
			i += copylen - 1;

			/* If the message is complete, deliver it */
			if (mRxPacketBuffer.size() >= mRxPktLength)
			{
				h4rx_delivermsg();
				mRxNeededLength = 1;
				mRxState = st_idle;
				msg_start = true;
				return i + 1;
			}
			break;

		default:
			H4DS_PANIC(H4DS_PANIC_INT_CORRUPTION);
			break;
		}
    }

    msg_start = (mRxState == st_idle);

    return i;
}

// ==== H4 TX ====

void H4DSStack::h4tx_send_bytes(const uint8 *data, size_t len)
{
	ASSERT(!mTxInProgress);
	mTxInProgress = !mFile->write(data, len, &mTxWritten);
	mDsUartActive = true;
}

/****************************************************************************
NAME
	h4tx_send_packet  -  send a message over h4ds

FUNCTION
	This function checks to see if it has any H4 packets to send to the
	peer, and if it can it then sends one packet.  If it cannot send a
	packet it attempts to wake the peer and get itself into a state where
	it can talk to the peer.
*/

void H4DSStack::h4tx_send_packet()
{
	ASSERT(!mTxInProgress);

	// Is there anything to send
	if (!mTxQueue.getNextTxPacket())
		return;

	// Can we send anything.  If we cant this will wake up the stack
	if (!dstx_can_send_h4_packet())
		return;

#if 0
	std::vector<uint8> TxPacketBuffer;

	// Setup the packet buffer.
	TxPacketBuffer.clear();
	// don't send the packet type byte if it's a raw send.
	if (mTxQueue.nextTxPacketHeaderByte() != H4_PKTTYPE_RAW)
	{
		TxPacketBuffer.reserve(mTxQueue.nextTxPacket().size() + 1);
		TxPacketBuffer.insert(
			TxPacketBuffer.end(),
			mTxQueue.nextTxPacketHeaderByte());
	}
	else
	{
		TxPacketBuffer.reserve(mTxQueue.nextTxPacket().size());
	}
	TxPacketBuffer.insert(
		TxPacketBuffer.end(),
		mTxQueue.nextTxPacket().data(),
		mTxQueue.nextTxPacket().data() + mTxQueue.nextTxPacket().size());

	// Inform the higher layer that something has been sent.
	if (mTxQueue.nextTxPacketNeedsCallback())
		mCallBacks.sentPacket(true);

	// Unlink the PDU from the Q
	mTxQueue.unlinkNextTxPacket();

	DB_DUMPPKT(("Tx: sending pkt", &TxPacketBuffer[0], TxPacketBuffer.size()));

	h4tx_send_bytes(&TxPacketBuffer[0], TxPacketBuffer.size());
#else
	#define MAX_BUF_SIZE 3100 //Based on MAX ACL NUM * MAX ACL SIZE
	uint8 pBuf[MAX_BUF_SIZE];
	uint8 *pTmp = pBuf;
	size_t size = mTxQueue.nextTxPacket().size();

	while ((size_t) (MAX_BUF_SIZE - (pTmp - pBuf)) > size)
	{
		// don't send the packet type byte if it's a raw send.
		if (mTxQueue.nextTxPacketHeaderByte() != H4_PKTTYPE_RAW)
		{
			*pTmp++ = mTxQueue.nextTxPacketHeaderByte();
		}
		memcpy(pTmp, (const void*)mTxQueue.nextTxPacket().data(), size);
	
		pTmp += size;
		// Inform the higher layer that something has been sent. Q? Should we make callback
		// after unlinking the packet from the queue? 
		if (mTxQueue.nextTxPacketNeedsCallback())
			mCallBacks.sentPacket(true);
		// Unlink the PDU from the Q
		mTxQueue.unlinkNextTxPacket();
		if (mTxQueue.getNextTxPacket())
		{
			size = mTxQueue.nextTxPacket().size();
		}
		else
		{
			size = MAX_BUF_SIZE;
		}
	}
	
	h4tx_send_bytes((const uint8*)pBuf, (pTmp - pBuf));
#endif
}

// ==== UART TX+RX ====

/*virtual*/ int H4DSStack::ThreadFunc(void)
{
    bool dead = false;

    DB_PRINTF (("H4DS: serial daemon started\n"));

    // Prime RX by reading from UART until the read request stalls
    // This is mainly so that we get a clean start if everything has 
    // gone a bit pear shaped.  I expect the first read to block as
    // there will be no data on the wire.  H4DS is semi-reliable at
    // this point (because the transport is assumed to be unreliable).
    while (mFile->read(mRxBuffer, sizeof(mRxBuffer), &mRxLength))
		/*EMPTY*/;

    // Start the Tshy timer running
	mReqTxSync = true;
	mPumpReq = true;

	mTimerSync.SetTimer();

    while (!dead)
    {
		HiResClockMilliSec delta;

		// Try to transmit some stuff.
		// Should we do this as often as we do?
		if (!mTxInProgress)
		{
			h4tx_send_packet();

            if (mPumpReq)
            {
                mPumpReq = false;
                dstx_pump();
            }
        }

        // Check any of my timed events.
        delta = CheckPassiveTimers(m_timerList, MAX_WAIT_TIME);

        if (!mTxInProgress && mPumpReq)
            mFile->reqXmit();

        // Wait until something happens
        UARTAbstraction::Event event = mFile->wait(delta, &mRxLength);

        switch (event)
        {
        case UARTAbstraction::UART_RCVDONE:

			// If the RX alarm has been set then fire it.
			if (mDsUartRxAlarm)
			{
				mDsUartRxAlarm = false;

				// We choose to always kick the DS state machine
				// (It does no harm if it's not needed).
				ds_sm(dsevtid_rx_uart_activity);
			}

			// Read all of the bytes from the UART
			do
			{
				dsrx_uart_deliverbytes(mRxBuffer, mRxLength);

				// If this is zero then the RX path might stall
				assert(mRxNeededLength != 0);
			}
			while (mFile->read(mRxBuffer,
					   std::min(mRxNeededLength,
						uint32(sizeof(mRxBuffer))),
					   &mRxLength));
			break;

        case UARTAbstraction::UART_XMITDONE:
			ASSERT(mTxInProgress);
			mTxInProgress = false;
			break;

        case UARTAbstraction::CLIENT_TXREQ:
            break;

        case UARTAbstraction::CLIENT_DIEREQ:
            DB_PRINTF(("Requested to die.\n"));
            dead = true;
            break;

        case UARTAbstraction::UART_ERROR:
            mCallBacks.syncLost(uart_failure);
            dead = true;
            break;

        case UARTAbstraction::timedOut:
        default:
            break;
        }
    }

    // Tell everyone that we are dead
    mThreadDead.set();

    // Remove all of the unsent packets from the Q.
    // This is mainly so that we can call the callback.
    while (mTxQueue.getNextTxPacket())
    {
		// Inform the higher layer that something has been sent.
		if (mTxQueue.nextTxPacketNeedsCallback())
			mCallBacks.sentPacket(false);

		// Unlink the PDU from the Q
		mTxQueue.unlinkNextTxPacket();
    }

    return 0;
}
