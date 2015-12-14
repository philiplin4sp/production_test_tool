///////////////////////////////////////////////////////////////////////
//
//  FILE   :  H5StackBase.cpp
//
//  Copyright CSR 2002-2006
//
//  AUTHOR :  Mark Marshall
//
//  CLASS  :  H5StackBase
//
//  PURPOSE:  Generic portion of the H5 implementation of HCITransport
//            The class H5StackBase is the basis for either an H5
//            stack or a BCSP stack.  The two classes H5Stack and
//            BCSPStack are both derived from this class.
//            The only functions that they need to add are a few simple
//            things to deal with the Link Establishment.
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/phys/h5stackbase.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

#include <climits>
#include <cstring>
#include <cassert>
#include "common/algorithm.h"
#include <list>

#include "h5stackbase.h"
#include "time/hi_res_clock.h"
#include "thread/error_msg.h"


///////////////////////////////////////////////////////////////////////
// CRC code
///////////////////////////////////////////////////////////////////////

// Table for calculating CRC for polynomial 0x1021, LSB processed first,
// initial value 0xffff, bits shifted in reverse order.

static const uint16 crc_table[256] = {
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf, 
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7, 
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e, 
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876, 
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd, 
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5, 
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c, 
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974, 
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb, 
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3, 
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a, 
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72, 
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9, 
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1, 
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738, 
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70, 
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7, 
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff, 
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036, 
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e, 
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5, 
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd, 
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134, 
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c, 
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3, 
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb, 
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232, 
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a, 
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1, 
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9, 
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330, 
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};


/////////////////////////////////////////////////////////////////////////////
// NAME
//  crc_init  -  initialise the crc calculator

static inline void crc_init(uint16 &crc)
{
    crc = 0xffff; 
}


/////////////////////////////////////////////////////////////////////////////
// NAME
//  crc_update  -  update crc with next data byte

static inline void crc_update(uint16 &crc, const uint8 *block, int len)
{
	uint16 reg;

	if (len == 0)
		return;

	reg = crc;

	do {
		reg = (reg >> 8) ^ crc_table[(reg & 0xFF) ^ *block++];
	} while(--len);

	crc = reg;
}


/////////////////////////////////////////////////////////////////////////////
// NAME
//  crc_reverse  -  get reverse of generated crc
//
// NOTE
//  The crc generator (crc_init() and crc_update())
//  creates a reversed crc, so it needs to be swapped back before
//  being passed on.

static inline uint16 crc_reverse(uint16 crc)
{
	crc = ((crc & 0x00ff) << 8) | (crc >> 8);
	crc = ((crc & 0x0f0f) << 4) | ((crc & 0xf0f0) >> 4);
	crc = ((crc & 0x3333) << 2) | ((crc & 0xcccc) >> 2);
	crc = ((crc & 0x5555) << 1) | ((crc & 0xaaaa) >> 1);
	
	return crc;
}


/////////////////////////////////////////////////////////////////////////////
// NAME
//  crc_block  -  calculate a CRC over a block of data

static inline uint16 crc_block(const uint8 *block, int len)
{
	uint16 crc;

	crc_init(crc);
	crc_update(crc, block, len);
	return crc_reverse(crc);
}


/////////////////////////////////////////////////////////////////////////////
// NAME
//  range_contains  -  is num >= min && num < max (mod 8)?
//
// FUNCTION
//  This 'macro' returns true if ((num >= min) && (num < max))
//  It properly deals with the fact that the inputs are all MOD 8.

static inline int range_contains(uint32 num, uint32 min, uint32 max)
{
	uint32 x_a = (num - min) & 0x7;
	uint32 b_a = (max - min) & 0x7;
	
	return x_a < b_a;
}


/////////////////////////////////////////////////////////////////////////////
// NAME
//	slip_byte  -  slip encode a single byte
//
// FUNCTION
//	Slip encodes a single byte.  The encoded byte is stored in the
//	buffer 'ubuf', which is assumed to be big enough.  The pointer
//	is modified to point to the first byte after the encoded byte.

static inline void slip_byte(uint8 *&ubuf, uint8 c)
{
	if (c == 0xc0)
	{
		*ubuf++ = 0xdb;
		*ubuf++ = 0xdc;
	}
	else if (c == 0xdb)
	{
		*ubuf++ = 0xdb;
		*ubuf++ = 0xdd;
	}
	else
	{
		*ubuf++ = c;
	}
}


/////////////////////////////////////////////////////////////////////////////
// NAME
//	slip_buf  -  slip encode a number of bytes
//
// FUNCTION
//	Slip encodes a number of bytes.  The encoded bytes are stored
//	in the buffer 'ubuf', which is assumed to be big enough.  The
//	pointer is modified to point to the first byte after the encoded
//  bytes.

static inline void slip_buf(uint8 *&ubuf, const uint8 *ibuf, uint32 len)
{
	while (len != 0)
	{
		slip_byte(ubuf, *ibuf++);
		--len;
	}
}


///////////////////////////////////////////////////////////////////////
// NAME
//	H5StackBase::H5StackBase  -  ctor

H5StackBase::H5StackBase(UARTPtr aFile, CallBackInterface &callBacks,
		uint32 Tretx /*= 250*/, uint8 winsize /*= 7*/)
 :	mFile(aFile),
	mTxInProgress(false),
	mCallBacks(callBacks),
	mThreadDead(mThreadSignal), mLEDone(mLESignal),
	m_timerTretx(Tretx),
	m_windowSize(winsize),
	m_msgq_unackd_txseq(0),
	m_msgq_unackd_count(0),
	m_msgq_next_txseq(0),
	m_choke(true),
	m_txack_req(false),
	m_rxseq_txack(0), m_rxack(0),
	m_rxslip_state(rxslipstate_nosync)
{
	// Bind the object & function to the three timers
	m_timerTretx.Bind(this, &H5StackBase::RetransmitPackets);

	// Add the timers to the list of timers
	m_timerList.push_back(&m_timerTretx);

	// resolution better than 125 msec.  This should probably not assert,
	// but what else can it do?
	HiResClock res;
	InitHiResClock(&res);
	assert(res.tv_sec == 0 && res.tv_nsec < 125000);
}


/////////////////////////////////////////////////////////////////////////////
// NAME
//	H5StackBase::~H5StackBase  -  dtor

H5StackBase::~H5StackBase ()
{
    Stop();
	m_msgQueue.clear();
	m_relQueue.clear();
    assert (!IsActive());
}


/////////////////////////////////////////////////////////////////////////////
// NAME
//	H5StackBase::Start  -  Start the stack running.

bool H5StackBase::Start ()
{
    if (!mFile->open())
	{
        ERROR_MSG (("H5: cannot open serial port\n"));
		return false;
	}

	if (!Threadable::Start())
	{
        ERROR_MSG (("H5: cannot start worker thread\n"));
		return false;
	}

	Threadable::HighPriority();

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// NAME
//	H5StackBase::Ready  -  Is the stack ready yet

bool H5StackBase::Ready (uint32 timeout)
{
    if (!IsActive() || !mLESignal.wait(timeout))
		return false;

	if (!m_choke)
		return true;

	ERROR_MSG(("'LE-Done' signal set, but the choke is still on?\n"));

	return false;
}


/////////////////////////////////////////////////////////////////////////////
// NAME
//	H5StackBase::Stop  -  Stop the Stack

void H5StackBase::Stop ()
{
    if (!IsActive())
		return;

	Threadable::Stop();

	ERROR_MSG (("H5: asking serial daemon to die\n"));
	mFile->abortWait();

	mThreadDead.wait();
}


/////////////////////////////////////////////////////////////////////////////
// NAME
//	H5StackBase::SendPDU  -  Send a PDU

void H5StackBase::SendPDU(const PDU &aPacket)
{
    if (!IsActive() || !mLESignal.wait(0))
		return;

	if (aPacket.size() == 0 || aPacket.size() > MAX_PAYLOAD_LEN)
		return;

	CriticalSection::Lock lock(m_queueLock);

	if (m_choke)
		return;

	switch (aPacket.channel())
	{
		// The higer layers cannot send data on these protocols,
		// so we reject the packets here.
	case PDU::zero:
	case PDU::le:
		return;

		// SCO is the only sort of data that the upper layer can send unreliably
	case PDU::hciSCO:
		m_unrelMsgQueue.push_back(aPacket);
		break;

		// All protocols apart from SCO are reliable
	case PDU::hciACL:
	case PDU::hciCommand:
	default:
		m_msgQueue.push_back(aPacket);
		break;
	}

    mFile->reqXmit();
}


/////////////////////////////////////////////////////////////////////////////
// NAME
//	H5StackBase::setDataWatch  -  Set the UART DataWatcher

void H5StackBase::setDataWatch(DataWatcher *w)
{
    mFile->setDataWatch(w);
}


/////////////////////////////////////////////////////////////////////////////
// NAME
//	H5StackBase::ThreadFunc  -  Worker thread for the H5Stack
//
// FUNCTION
//  The ThreadFunc drives both the TX and RX sections

/*virtual*/ int H5StackBase::ThreadFunc()
{
    ERROR_MSG (("H5: serial daemon started\n"));

	le_start();

    /* Prime RX by reading from UART until the read request stalls */
	while (mFile->read(m_rxBuffer, sizeof(m_rxBuffer), &m_rxLength))
		/*EMPTY*/;

#ifdef MM01_DUMP_STATS
	mFile->DumpStats();
#endif /* def MM01_DUMP_STATS */

	// This is an estimate - to C0's and a four byte BCSP header
	m_rxNeededLength = 6;
	m_rxPacketLength = ~0;

	for (;;)
    {
		HiResClockMilliSec delta = CheckPassiveTimers(m_timerList, 1000);

        UARTAbstraction::Event event = mFile->wait(delta, &m_rxLength);

        switch (event)
        {
        case UARTAbstraction::UART_RCVDONE:
#ifdef MM01_DUMP_STATS
			mFile->DumpStats();
#endif /* def MM01_DUMP_STATS */
			// We keep reading from the UART until the read stalls.  This 
			// should completely drain the UART.
			do
			{
				ReadBytesFromUART();
			}
			while (mFile->read(m_rxBuffer, std::min(sizeof(m_rxBuffer), (size_t)m_rxNeededLength), &m_rxLength));
            break;

        case UARTAbstraction::UART_XMITDONE:
			//ASSERT(mTxInProgress);
			mTxInProgress = false;
        case UARTAbstraction::CLIENT_TXREQ:
			if (!mTxInProgress)
				if (PumpTxPackets())
					mFile->reqXmit();
            break;

        case UARTAbstraction::CLIENT_DIEREQ:
			ERROR_MSG(("H5: serial daemon terminated\n"));
			mThreadDead.set();
			return 0;

        case UARTAbstraction::UART_ERROR:
			mCallBacks.linkFailed(uart_failure);
                        mThreadDead.set();
 			return 0;
			break;

        case UARTAbstraction::timedOut:
        default:
			break;
        }
    }
}


/////////////////////////////////////////////////////////////////////////////
// NAME
//  H5StackBase::CullAckedPackets  -  discard transmit messages no longer needed
//
// FUNCTION
//	Determines which reliable packets have been received (acked)
//	by the peer and discards the local copy of these if they are in
// 	the reliable transmit queue.

inline void H5StackBase::CullAckedPackets()
{
	uint32 rxack, seq;
	
	// Give up immediately if there's nothing to cull.
	if (m_msgq_unackd_count == 0)
		return;

    // The received ack value is always one more than the seq of
	// the packet being acknowledged.
	rxack = (m_rxack - 1) & 0x7;

    // 'rxack' carries the ack value from the last packet
    // received from the peer.  It marks the high water mark of
    // packets accepted by the peer.  We only use it if it refers
    // to a message in the transmit window, i.e., it should refer
    // to a message in relq.  (It's not always an error if it
    // refers to a message outside the window - it is initialised
    // that way - but it would be possible to extend this code to
    // detect absurd rxack values.)
	if (!range_contains(rxack, m_msgq_unackd_txseq, m_msgq_next_txseq))
		return;

	// Finally, we walk relq, discarding its contents,
	// until after we discard the packet with sequence
	// number rxack.
	//
	// seq counts through all of the packets that we have
	// sent but not yet got an ack for.  If at some point
	// seq == rxack we have ack'd enough packets.
	while ((seq = m_msgq_unackd_txseq) != m_msgq_next_txseq)
	{
		CriticalSection::Lock lock(m_queueLock);

		m_relQueue.pop_front();

		m_msgq_unackd_txseq = (seq + 1) & 0x7;
		m_msgq_unackd_count--;

		if (seq == rxack)
			break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// NAME
//  H5StackBase::PumpTxPackets  -  get a message to send to the uart
//
// FUNCTION
//  This function is called whenever the UART can accept more data to send
//  or whenever we recieve a new ACK.  It checks the reliable and unreliable
//  queues and decides what totransmit next (if anything).
//
// RETURNS
//  True if it is worth calling this function again imeadiately, else false.

bool H5StackBase::PumpTxPackets()
{
    uint32 wrote;
	uint8 *slip_ptr;
	uint8 header[4];
	uint8 ledata[4];
    size_t curr_len;
    const uint8 *curr_buf;
	PDU current(PDU::zero, 0);

	// Buffer where we store the packet after we have slip encoded it
	//  (before we send it to the UART)
	uint8 txbuf[(MAX_PAYLOAD_LEN + 4 + 2) * 2 + 2];

	// Discard any acknowledged reliable messages.
	CullAckedPackets();

	// Start with an empty header
	memset(header, 0, 4);

	// Choose pkt to send.  Default settings (chan = 0, rel = FALSE)
	//  - an ack packet.
    curr_len = 0;
    curr_buf = 0;

    // H5-LE messages have the highest priority.  (Always unrel.)
	if ((curr_len = le_message_avail(ledata)) != 0)
	{
		curr_buf = ledata;

		// Channel
		header[1] = MapProtocolToWire(PDU::le);

		if (SendLECRCs())
			header[0] |= 0x40;

		ERROR_MSG(("H5base: TX LE packet %02X %02X %02X %02X\n", ledata[0], ledata[1], ledata[2], ledata[3]));
	}
	else
	{
		CriticalSection::Lock lock(m_queueLock);

		/* Any unreliable TXMSG has the next highest priority */
		if (!m_unrelMsgQueue.empty())
		{
			// We need the peer to be awake if we are sending a packet 
			// that is not an LE packet.
			if (!ForceAwake())
				return true;

			/* We remove the message from the *queue* and destroy
			   it.  (This is different from the way in which relq
			   is used, where messages remain in the queue until
			   their reception is acknowledged by the peer.) 
			*/

			current = m_unrelMsgQueue.front();

			curr_len = current.size();
			curr_buf = current.data();
			header[1] = MapProtocolToWire(current.channel());

			m_unrelMsgQueue.pop_front();

			//ERROR_MSG(("H5base: TX UnReliable Packet %02x\n", header[1]));
		}
		/* Any reliable data is next in the pecking order. */
		else if (m_msgq_unackd_count < m_windowSize && !m_msgQueue.empty())
		{
			// We need the peer to be awake if we are sending a packet 
			// that is not an LE packet.
			if (!ForceAwake())
				return true;

			header[0] |= 0x80 + m_msgq_next_txseq;
			m_msgq_next_txseq = (m_msgq_next_txseq + 1) & 0x7;
			m_msgq_unackd_count++;

			m_timerTretx.SetTimer();

			current = m_msgQueue.front();

			curr_len = current.size();
			curr_buf = current.data();
			header[1] = MapProtocolToWire(current.channel());

			// Add this istem to the 'sent' Q and remove from the pending Q.
			m_relQueue.push_back(current);
			m_msgQueue.pop_front();

			//ERROR_MSG(("H5base: TX Reliable Packet %02x\n", header[1]));
		}
		/* Finally, send an ack packet if needed. */
		else if (m_txack_req)
		{
			// We need the peer to be awake if we are sending a packet 
			// that is not an LE packet.
			if (!ForceAwake())
				return true;

			header[1] = MapProtocolToWire(PDU::zero);

			//ERROR_MSG(("H5base: TX ACK Packet %02x\n", header[1]));
		}
		else
		{
	        //ERROR_MSG (("nothing to TX\n"));
			return false;
		}

		if (SendCRCs())
			header[0] |= 0x40;
	}

	// The txack value is 1 more than the seq of the received
	// packet being acknowledged.  However, the rxseq_txack value
	// was incremented by the receive path, so we use the value of
	// rxseq_txack directly.
	header[0] |= m_rxseq_txack << 3;
	header[1] |= curr_len << 4;
	header[2] = curr_len >> 4;
	header[3] = ~(header[0] + header[1] + header[2]);

	ERROR_MSG(("Tx Seq:%d Ack:%d Chan:%x\n",
		header[0] & 7, (header[0] >> 3) & 7, header[1] & 15));

	/* Only clear this now, when we are certain that we will send
	 * a packet. */
	m_txack_req = false;

	// Get a pointer to where we store the slipped packet
	slip_ptr = txbuf;

	// Message START
	*slip_ptr++ = 0xc0;
	
	slip_buf(slip_ptr, header, 4);			// Message Header
	slip_buf(slip_ptr, curr_buf, curr_len);	// Message Data

	// Do we need to calculate a CRC for this packet
	if ((header[0] & 0x40) != 0)
	{
		uint16 crc;

		// Calculate the CRC
		crc_init(crc);
		crc_update(crc, header, 4);			// Message Header
		crc_update(crc, curr_buf, curr_len);	// Message Data

		// Reverse CRC
		crc = crc_reverse(crc);

		// Store CRC
		slip_byte(slip_ptr, (uint8)((crc >> 8) & 0xff));
		slip_byte(slip_ptr, (uint8)(crc & 0xff));
	}

	// Message END
	*slip_ptr++ = 0xc0;

	// Write the data to the UART
	bool write = mFile->write(txbuf, slip_ptr - txbuf, &wrote);
	mTxInProgress = !write;
	return write;
}


/////////////////////////////////////////////////////////////////////////////
// NAME
//  H5StackBase::RetransmitPackets  -  transmit path's timed event
//
// FUNCTION
//  Checks to see if we need to retransmit some packets.  If we do it rewinds
//  the pointer to the list and resets the TX-SEQ number accordingly.  It then
//  requests to the UART that we want to perform a transmission.

void H5StackBase::RetransmitPackets()
{
	uint32 new_txseq;

	if (m_msgq_unackd_count == 0)
	{
		//ERROR_MSG(("Nothing to re-transmit\n"));
		return;
	}

	/* Arrange to retransmit all messages in the relq. */
	if ((new_txseq = m_msgq_unackd_txseq) != m_msgq_next_txseq)
	{
		//ERROR_MSG(("Re-transmitting data\n"));

		CriticalSection::Lock lock(m_queueLock);
		while (!m_relQueue.empty())
		{
			m_msgQueue.push_front(m_relQueue.back());
			m_relQueue.pop_back();
		}
		m_msgq_next_txseq = new_txseq;
		m_msgq_unackd_count = 0;

	    mFile->reqXmit();
	}
	else
	{
		//ERROR_MSG(("Not re-transmitting data\n"));
	}
}


/////////////////////////////////////////////////////////////////////////////
// NAME
//  H5StackBase::DespatchRxPacket  -  process an entire H5 message
//
// FUNCTION
//  Take a message and either pass it to the LE engine or pass it
//  on up the stack.

bool H5StackBase::DespatchRxPacket(PDU::bc_channel chan, bool rel, uint32 len)
{
    /* Is this a Link Establishment message? */
    switch (chan)
	{
	case PDU::le:
		if (rel)
			ERROR_MSG(("LE message on reliable channel?\n"));
		ERROR_MSG(("H5base: RX LE packet %02x %02X %02X %02X\n",
			m_rxbuf[4+0], m_rxbuf[4+1], m_rxbuf[4+2], m_rxbuf[4+3]));
		le_putmsg(len);
		break;

	case PDU::hciACL:
		mCallBacks.recvACLData(PDU(PDU::hciACL, m_rxbuf+4, len));
		break;

	case PDU::hciSCO:
		mCallBacks.recvSCOData(PDU(PDU::hciSCO, m_rxbuf+4, len));
		break;

	case PDU::hciCommand:
		mCallBacks.recvEvent(PDU(PDU::hciCommand, m_rxbuf+4, len));
		break;

	default:
		//ERROR_MSG(("Unhadled PDU?\n"));
		mCallBacks.recvOther(PDU(chan, m_rxbuf+4, len));
		break;
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// NAME
//  H5StackBase::DecodeRxPacket  -  decode and entire bcsp message
//
// FUNCTION
//	Take an array of bytes and check that it is a complete BCSP
//	message.  We check the BCSP header and the CRC.  This function
//	also deals with accounting for ACK/SEQ numbers and arranging
//	for ACK packets to be sent.

void H5StackBase::DecodeRxPacket(size_t len)
{
    uint32 hdr_len, hdr_chan, hdr_ack, hdr_seq, temp_len;
    bool hdr_rel, hdr_crc_present;
	PDU::bc_channel proto_num;

	// We need four bytes for a BCSP/H5 packet header
    if (len < 4)
	{
		stats.mRXErrVeryShort++;
		return;
	}

	// Check the header checksum
    if (((m_rxbuf[0] + m_rxbuf[1] + m_rxbuf[2] + m_rxbuf[3]) & 0xff) != 0xff)
	{
		stats.mRXErrBadHeader++;
		return;
	}

	// Get the channel number and the reliability flag
    hdr_rel = (m_rxbuf[0] & 0x80) != 0;
    hdr_chan = m_rxbuf[1] & 0x0f;

	// Convert the wire channel number to a protocol
	proto_num = MapWireToProtocol(hdr_chan);

	// Check to see if the packet should be blocked by the choke
    if (m_choke && !(proto_num == PDU::le && !hdr_rel))
	{
		stats.mRXErrChokeDiscard++;
		return;
	}

	// Get the length and CRC info from the header
    hdr_len = ((m_rxbuf[1] >> 4) & 0x0f) | ((uint32)m_rxbuf[2] << 4);
    hdr_crc_present = (m_rxbuf[0] & 0x40) != 0;

	// Check the the length matches what the header says
    temp_len = hdr_len + 4 + (hdr_crc_present ? 2 : 0);
    if (temp_len != len)
	{
		stats.mRXErrLenMismatch++;
		return;
	}

	// Check the CRC
    if (hdr_crc_present &&
		crc_block(m_rxbuf, len-2) != (((uint16)m_rxbuf[len-2] << 8) | m_rxbuf[len-1]))
	{
		stats.mRXErrCRCBad++;
		return;
	}

	// Get the rest of the information from the header
    hdr_seq = m_rxbuf[0] & 0x07;
    hdr_ack = (m_rxbuf[0] >> 3) & 0x07;

	ERROR_MSG(("Rx Seq:%d Ack:%d Chan:%x\n",
		hdr_seq, hdr_ack, hdr_chan));

	// Check to see if the packet is the one that we expect next
    if (hdr_rel && hdr_seq != m_rxseq_txack)
    {
		// If the packet is not the one that we expect next this is not
		// necesarily a BUG.  It probably means that someone somewhere
		// has dropped a packet, and someone has done some retransmissions.
		// This has performance issues, though the link is still fine.
		stats.mRXErrMissSeqDiscard++;
		
		// BCSP/H5 must acknowledge all reliable packets to avoid deadlock.
		m_txack_req = true;
		
		// Wake the tx path so that it can discard the acknowledged
		// message(s).
	    mFile->reqXmit();
    }
    else
    {
		// If the message is reliable we need to note the next rel
		// rxseq we will accept.  This is numerically identical to the
		// value that we send back to the peer in outbound packets'
		// ack fields to tell the host that we've got this message,
		// i.e., the ack value sent to the peer is one more than the
		// packet being acknowledged, modulo 8.
		if (hdr_rel)
		{
			m_rxseq_txack = (m_rxseq_txack + 1) & 0x7;
			m_txack_req = 1;

			// Wake the tx path to send the new ack val back to the peer.
		    mFile->reqXmit();
		}

		// We don't deliver ack msgs
		if (proto_num != PDU::zero)
		{
			// Return early if we fail to deliver the packet.  If we
			// could not deliver the packet to a higher layer, we
			// should not acknoledge it so that it is resent.
			if (!DespatchRxPacket(proto_num, hdr_rel, hdr_len))
			{
				stats.mRXErrDespatchFailed++;
				return;
			}
		}
    }

    // We accept rxack acknowledgement info from any intact packet,
    // reliable or unreliable.  This includes reliable messages with
    // the wrong seq number.
    if (hdr_ack != m_rxack)
    {
		m_rxack = hdr_ack;

		// Wake the tx path so that it can discard the acknowledged
		// message(s).
		mFile->reqXmit();
    }
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  H5StackBase::CheckRxPacketLength  -  How many more bytes are needed
//
// FUNCTION

void H5StackBase::CheckRxPacketLength()
{
	// We need four bytes for a BCSP/H5 packet header
	if (m_rxslip_ptr >= m_rxbuf+4u)
	{
		if (m_rxPacketLength == ~0u)
		{
			// Check the header checksum
			if (((m_rxbuf[0] + m_rxbuf[1] + m_rxbuf[2] + m_rxbuf[3]) & 0xff) != 0xff)
			{
				stats.mRXErrBadHeader++;

				// Reset the reciever deslipper state to junk this packet
				m_rxslip_state = rxslipstate_nosync;
				m_rxNeededLength = 6;

				m_rxPacketLength = ~0;

				return;
			}

			// Length of the packt payload
			m_rxPacketLength = ((m_rxbuf[1] >> 4) & 0x0f) | ((uint32)m_rxbuf[2] << 4);
			// add in the BCSP header and the CRC if there is one (and the last C0)
			m_rxPacketLength += 4 + (((m_rxbuf[0] & 0x40) != 0) ? 2 : 0) + 1;
		}

		m_rxNeededLength = m_rxPacketLength - (m_rxslip_ptr - m_rxbuf);
	}
	else
		m_rxNeededLength = 4 - (m_rxslip_ptr - m_rxbuf);
}

/////////////////////////////////////////////////////////////////////////////
// NAME
//  H5StackBase::ReadBytesFromUART  -  Deal with bytes read from the UART
//
// FUNCTION
//  Simple de-slipping routine.  It takes all of the data from 'm_rxBuffer'
//  and deslips it.  When it gets a complete packet (an 0xc0 followed by one
//  or more non-0xc0 byte followed by another 0xc0 byte) it sends it to the
//  next part of the RX pipeline.
//
//  This function has been fairly viciously ''optimised''.  Most of the
//  time the bytes are not escapped, and there is plenty of room in the
//  destination buffer.  Hence the ''inner loop''.

void H5StackBase::ReadBytesFromUART()
{
	// The input is in the m_rxBuffer and is of length m_rxLength.
	// The data is being put into the buffer m_rxbuf, and m_rxslip_ptr
	// points to the place in this buffer where we want to put the data.
    const uint8 *const src_end = m_rxBuffer + m_rxLength;
    const uint8 *src_ptr = m_rxBuffer;
    uint8 *dest_ptr;
    uint8 c;

	// Nothing to do.
    if (m_rxLength == 0)
		return;

	// Get the destination pointer.  This value is only correct if we are 
	// going into the 'body' or 'body_esc' states.  If we are not then it 
	// is overwritten with the correct value when we make the transition.
	dest_ptr = m_rxslip_ptr;

	while (src_ptr < src_end)
	{
		c = *src_ptr++;

		switch(m_rxslip_state)
		{
		default:
		case rxslipstate_nosync:
			if (c != 0xc0)
				break;

			if (src_ptr == src_end)
			{
				m_rxslip_state = rxslipstate_start;
				// Reset the needed length
				m_rxNeededLength = 5;
				return;
			}
			c = *src_ptr++;

			/* FALLTHROUGH */

		case rxslipstate_start:
			while (c == 0xc0)
				if (src_ptr == src_end)
				{
					m_rxslip_state = rxslipstate_start;
					// Reset the needed length
					m_rxNeededLength = 5;
					return;
				}
				else
					c = *src_ptr++;

			m_rxslip_state = rxslipstate_body;
			m_rxPacketLength = ~0;
			dest_ptr = m_rxslip_ptr = m_rxbuf;

			/* FALLTHROUGH. */
		case rxslipstate_body:

			for (;;)
			{
				// If the entire input block will fit into 'rxbuf' we dont
				// need to check the length for each byte.
				if (dest_ptr + (src_end - src_ptr) < (m_rxbuf + MAX_PAYLOAD_LEN + 4 + 2))
				{
					while (c != 0xdb && c != 0xc0)
					{
						*dest_ptr++ = c;
						if (src_ptr >= src_end)
						{
							m_rxslip_ptr = dest_ptr;
							// Reset the needed length
							CheckRxPacketLength();
							return;
						}
						c = *src_ptr++;
					}
				}

				if (c == 0xc0)
				{
					DecodeRxPacket(dest_ptr - m_rxbuf);
					m_rxslip_state = rxslipstate_nosync;
					// Reset the needed length
					m_rxNeededLength = 6;
					break;
				}

				if (c == 0xdb)
				{
					// Get the second byte of the two byte escape
					// sequence.
					if (src_ptr >= src_end)
					{
						m_rxslip_state = rxslipstate_body_esc;
						m_rxslip_ptr = dest_ptr;
						// Reset the needed length
						CheckRxPacketLength();
						return;
					}
					c = *src_ptr++;

					// FALLTHROUGH
				case rxslipstate_body_esc:
					m_rxslip_state = rxslipstate_body;

					if (c == 0xdc)
					{
						c = 0xc0;
					}
					else if (c == 0xdd)
					{
						c = 0xdb;
					}
					else
					{
						// Byte sequence error.  Abandon current message.
						stats.mRXErrSlipError++;
						m_rxslip_state = rxslipstate_nosync;
						// Reset the needed length
						m_rxNeededLength = 6;
						break;
					}
				}

				// Is there room in the RX buffer for the unslipped byte.
				if (dest_ptr >= (m_rxbuf + MAX_PAYLOAD_LEN + 4 + 2))
				{
					// The packet was too long
					stats.mRXErrSlipTooLong++;
					m_rxslip_state = rxslipstate_nosync;
					// Reset the needed length
					m_rxNeededLength = 6;
					return;
				}

				*dest_ptr++ = c;

				if (src_ptr >= src_end)
				{
					m_rxslip_ptr = dest_ptr;
					// Reset the needed length
					CheckRxPacketLength();
					return;
				}

				c = *src_ptr++;
			}
			break;
		} /*switch*/
	}
}


/////////////////////////////////////////////////////////////////////////////
// NAME
//  H5StackBase::CallBackInterface::sent*  -  Packet has been sent

/*virtual*/ void H5StackBase::CallBackInterface::sentCommand(bool) {}
/*virtual*/ void H5StackBase::CallBackInterface::sentACLData(bool) {}
/*virtual*/ void H5StackBase::CallBackInterface::sentSCOData(bool) {}
/*virtual*/ void H5StackBase::CallBackInterface::sentEvent  (bool) {}
/*virtual*/ void H5StackBase::CallBackInterface::sentOther  (bool) {}


/////////////////////////////////////////////////////////////////////////////
// NAME
//  H5StackBase::CallBackInterface::recv*  -  Packet was recieved

/*virtual*/ void H5StackBase::CallBackInterface::recvCommand(const PDU &aPacket) {}
/*virtual*/ void H5StackBase::CallBackInterface::recvACLData(const PDU &aPacket) {}
/*virtual*/ void H5StackBase::CallBackInterface::recvSCOData(const PDU &aPacket) {}
/*virtual*/ void H5StackBase::CallBackInterface::recvEvent  (const PDU &aPacket) {}
/*virtual*/ void H5StackBase::CallBackInterface::recvOther  (const PDU &aPacket) {}

/////////////////////////////////////////////////////////////////////
// NAME
//  H5StackBase::CallBackInterface::linkFailed  -  The link has failed
/*virtual*/ void H5StackBase::CallBackInterface::linkFailed(FailureMode mode) {}
