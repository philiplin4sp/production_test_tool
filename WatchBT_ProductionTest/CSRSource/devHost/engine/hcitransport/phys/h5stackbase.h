///////////////////////////////////////////////////////////////////////
//
//  FILE   :  H5StackBase.h
//
//  Copyright CSR 2002-2006
//
//  AUTHOR :  Mark Marshall
//
//  CLASS  :  H5StackBase
//
//  PURPOSE:  Generic portion of H5/BCSP implementation of HCITransport
//
///////////////////////////////////////////////////////////////////////

#ifndef H5STACKBASE_H__
#define H5STACKBASE_H__

#include <list>

#include "common/nocopy.h"
#include "thread/thread.h"
#include "thread/signal_box.h"
#include "thread/critical_section.h"
#include "time/passive_timer.h"

#include "uart.h"
#include "transportapi.h"

class H5StackBase : public Threadable
{
 public:
    // Forward Definition
    class CallBackInterface;

 protected:
    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  H5StackBase  -  H5StackBase ctor
    //
    // FUNCTION
    //  This is the constructor for an H5StackBase.  You cannot make these 
    //  on there own, you can only construct classes that are derived from 
    //  this one.
    //
    H5StackBase(UARTPtr aFile, CallBackInterface &callBacks,
		uint32 Tretx = 250, uint8 winsize = 4);

 public:
    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  ~H5StackBase  -  H5StackBase dtor
    //
    // FUNCTION
    //  Destructor.  Performs all cleanup,
    //
    virtual ~H5StackBase();

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  Start  -  Start the stack running
    //
    // FUNCTION
    //  Start the stack.  This will create a new worker thread that will 
    //  perform all of the interesting functions of the H5Stack.
    //
    bool Start();

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //	H5StackBase::Ready  -  Is the stack ready yet
    //
    // FUNCTION
    //  return true if the stack is running and has completetd LE.
    //
    bool Ready(uint32 timeout);

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  Stop  -  Stop the stack
    //
    // FUNCTION
    //  This function is called to stop the stack.  It closes the COM port, 
    //  and frees all memory that it is no longer using.  The worker thread 
    //  is also killed.
    //
    void Stop();

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  SendPDU  -  Send a Protocol Data Unit to the Peer
    //
    // FUNCTION
    //  This function is used to send data ot the peer.  The PDU structure 
    //  now contains all of the information about the PDU, specifically its
    //  length and which channel it should be sent on.
    //
    //  If this function is called but the stack is not ready the PDU is 
    //  silently discarded.
    //
    void SendPDU(const PDU &aPacket);

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  setDataWatch  -  Magic debug function, used by BtCli
    //
    // FUNCTION
    //  This function is passed a DataWatcher pointer.  It then informs the 
    //  Data Watcher of all of the data bytes that flow through it.
    //
    void setDataWatch(DataWatcher *w);

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  class CallBackInterface  -  A class of callback functions
    //
    // FUNCTION
    // This is the base class for all of the H5 callbacks.
    // Classes that use an H5StackBase should derive there own
    // class from this, and pass a reference to it to H5Stack.
    // H5StackBase will then use the virtual functions as callbacks.
    //
    class CallBackInterface : public ::NoCopy
    {
    public:
	CallBackInterface() {}
	virtual ~CallBackInterface() {}

	/////////////////////////////////////////////////////////////////////
	// NAME
	//  H5StackBase::CallBackInterface::sent*  -  Packet has been sent
	//
	// FUNCTION
	//  These callbacks get called when the stack has sent a packet.  The 
	//  boolean indicates if the packet was sent correctly or not.  They do
	//  nothing by default.
	//
	virtual void sentCommand(bool);
	virtual void sentACLData(bool);
	virtual void sentSCOData(bool);
	virtual void sentEvent  (bool);
	virtual void sentOther  (bool);

	/////////////////////////////////////////////////////////////////////
	// NAME
	//  H5StackBase::CallBackInterface::recv*  -  Packet was recieved
	//
	// FUNCTION
	//  These callbacks get called when the stack has recieved a
	//  packet.  They do nothing by default.
	//
	virtual void recvCommand(const PDU &aPacket);
	virtual void recvACLData(const PDU &aPacket);
	virtual void recvSCOData(const PDU &aPacket);
	virtual void recvEvent  (const PDU &aPacket);
	virtual void recvOther  (const PDU &aPacket);

	/////////////////////////////////////////////////////////////////////
	// NAME
	//  H5StackBase::CallBackInterface::linkFailed  -  The link has failed
	//
	// FUNCTION
	//  This callback is used whne the stack detects that the link has
	//  failed,
	//
	virtual void linkFailed(FailureMode mode);
    };

 protected:

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  MapProtocolToWire  -  Map packet type to wire channel number
    //
    // FUNCTION
    //   Used to map between the protocol id used in the rest of the 
    //   stack and the wire channel number.
    //
    virtual uint8 MapProtocolToWire(PDU::bc_channel proto) = 0;

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  MapWireToProtocol  -  Map wire channel number to packet type
    //
    // FUNCTION
    //   Used to map between the wire channel number and
    //   the protocol id used in the rest of the stack.
    //
    virtual PDU::bc_channel MapWireToProtocol(uint8 wire) = 0;

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  SendCRCs  -  Should we add a CRC to a general packet
    //
    // FUNCTION
    //  Should we add CRCs to the packets that we are sending.
    //
    virtual bool SendCRCs() = 0;

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  SendLECRCs  -  Should we add a CRC to a LE packet
    //
    // FUNCTION
    //  Should we add CRCs to the LE packets that we are sending.  This
    //  function is used for LE messages.  These are treated differently 
    //  because they must be sent before negotiation of parameters with H5.
    //
    virtual bool SendLECRCs() = 0;

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  le_start  -  Start the LE engine
    //
    // FUNCTION
    //  This function starts of the LE engine sending the initial messages
    //  if we are not muzzled  (muzzeling is handled by the derived class).
    //
    virtual void le_start() = 0;

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  le_message_avail  -  is there a new LE transmit message available
    //
    // FUNCTION
    //  This determines if there is an LE message available to send.  If a 
    //  mesage is available it is copied to the buffer pointed to by data
    //  (which is assumed to be big enough >= 4 bytes).  The length of the
    //  message is returned.  If there is no message to send then 0 is 
    //  returned.  At the moment all LE messages must have a size in the
    //  range [1,4].
    //
    virtual uint32 le_message_avail(uint8 *data) = 0;

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  le_putmsg  -  we have recieved a new LE message.
    //
    // FUNCTION
    //	A new LE message has been recieved.  The body of the message is in 
    //  the buffer ''m_rxbuf''.  The message is ''len'' bytes long.
    //
    virtual void le_putmsg(uint32 len) = 0;

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  ForceAwake  -  Force the peer into life
    //
    // FUNCTION
    //  This returns true if the peer is awake.  If the peer is not awake
    //  it should prime the LE engine so that we can attempt to wake the
    //  peer and then return false.  PumpTxPackets should then get called
    //  again, and hopefully the peer will wake.
    // 
    //  This is not used for BCSP, because there is no concept of a
    //  device / peer being asleep.
    //
    virtual bool ForceAwake() = 0;

 protected:
    enum { MAX_PAYLOAD_LEN = 4095 };

    // This is the UART object that we use to talk to the hardware
    UARTPtr		mFile;

	bool mTxInProgress;

    // The CallBackInterface contains all of the callbacks that we might need
    CallBackInterface	&mCallBacks;

    // This signal is used to indicate when the thread has died
    SignalBox		mThreadSignal;
    SignalBox::Item	mThreadDead;

    // This signal is used to indicate when we have finished LE
    SignalBox		mLESignal;
    SignalBox::Item	mLEDone;

    // This lock must be held if either m_msgQueue or m_unrelMsg are
    // being changed!
    CriticalSection	m_queueLock;

    // Queues of PDU's
    typedef std::list<PDU> PDUlist;

    // The Q of reliable messages.
    PDUlist		m_msgQueue;
    // The Q of reliable messages that have been sent.
    PDUlist		m_relQueue;

    // The Q of unreliable messages
    PDUlist		m_unrelMsgQueue;

    // This is the list of all timers in the H5/BCSP engine.  The
    // individual timers below should not be used.  Instead only the
    // list of timers should be used for all time update operations.
    // Classes derived from this one can add new timers to the list to
    // register timed events,
    PassiveTimerList	m_timerList;

    // Retransmission timer
    PassiveTimer	m_timerTretx;

    // This is the size of window that we are using.  It is setup by
    // the H5 LE or is specified as a paramter to the ctor.
    uint32		m_windowSize;

    // This is the sequence number of the oldest packet that we sent
    // that has not been ackd yet.  We have packets in the range
    // [m_msgq_unackd_txseq, m_msgq_next_txseq) (though not including
    // m_msgq_next_txseq) in the retransmit Q.
    uint32		m_msgq_unackd_txseq;

    // This ia a count of the unacked packets in the reliable Q that
    // have already been sent at least once.
    uint32		m_msgq_unackd_count;

    // Reliable packet sequence number - used by fsm to emit packets.
    // This is the sequence number that will be used for the next
    // packet that we send.
    uint32		m_msgq_next_txseq;

    // This is where the data goes from the UART.
    uint8		m_rxBuffer[1024];
    uint32		m_rxLength;
    uint32		m_rxPacketLength;
    size_t		m_rxNeededLength;

    // Variables that communicate between the TX and RX halves
    bool		m_choke;		// Is the choke applied?
    bool		m_txack_req;		// Request tx rxseq_txack.
    uint32		m_rxseq_txack;  	// rxseq == txack.
    uint32		m_rxack;        	// Received acknowledgement.

    // Stuff to deal with deslipping data from the UART.
    enum rxslipstate
    {
	rxslipstate_nosync,         /* No SLIP synchronisation. */
	rxslipstate_start,          /* Sending initial c0. */
	rxslipstate_body,           /* Sending message body. */
	rxslipstate_body_esc        /* Received escape character. */
    };

    rxslipstate		m_rxslip_state;
    uint8		*m_rxslip_ptr;
    uint8		m_rxbuf[MAX_PAYLOAD_LEN + 4 + 2];

    // We generate some stats here about what is going on.  How many
    // packets have we sent, how many bytes have we sent and how many
    // times we have retransmited a packet.  Stuff like that.
    //
    // Perhaps this is of no use, and perhaps it should be optional
    // (cotrolled by the preprocessor?
    struct Stats
    {
	uint32	mBytesSent;	// Usefull data (dont count LE,
				// header, CRC or retransmit)
	uint32	mRawBytesSent;	// Data to UART
	uint32	mPacketsSent;	// Packets that were good

	uint32	mBytesRead;	// Usefull data (dont count LE,
				// header, CRC or retransmit)
	uint32	mRawBytesRead;	// Data from UART
	uint32	mPacketsRead;	// Packets that were good (went to
				// higher layer)

	uint32	mRXErrSlipError;	// We got an invalid slip
					// sequence
	uint32	mRXErrSlipTooLong;	// The message was too long
					// (while deslipping)
	uint32	mRXErrVeryShort;	// Less than four bytes
					// between C0's
	uint32	mRXErrBadHeader;	// Header checksum was bad
	uint32	mRXErrChokeDiscard;	// Got a packet while the
					// choke was enabled
	uint32	mRXErrLenMismatch;	// The header length did not
					// match the length between
					// the C0's
	uint32	mRXErrCRCBad;		// The CRC did not match
	uint32	mRXErrMissSeqDiscard;	// The sequence number was not
					// what we expected
	uint32	mRXErrDespatchFailed;	// The higher layers failed to
					// accept the packet (cant
					// happen)

	Stats() :
	    mBytesSent(0), mRawBytesSent(0), mPacketsSent(0),
	    mBytesRead(0), mRawBytesRead(0), mPacketsRead(0),
	    mRXErrSlipError(0), mRXErrSlipTooLong(0), mRXErrVeryShort(0),
	    mRXErrBadHeader(0), mRXErrChokeDiscard(0), mRXErrLenMismatch(0),
	    mRXErrCRCBad(0), mRXErrMissSeqDiscard(0), mRXErrDespatchFailed(0)
	{}
    } stats;

 private:
    /////////////////////////////////////////////////////////////////////////
    // NAME
    //	H5StackBase::ThreadFunc  -  Worker thread for the H5Stack
    //
    virtual int ThreadFunc();

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  H5StackBase::CullAckedPackets - discard transmit messages
    //							no longer needed
    //
    void CullAckedPackets();

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  H5StackBase::PumpTxPackets  -  get a message to send to the uart
    //
    bool PumpTxPackets();

    //////////////////////////////////////////////////////////////////////////
    // NAME
    //  H5StackBase::RetransmitPackets  -  transmit path's timed event
    //
    void RetransmitPackets();

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  H5StackBase::DespatchRxPacket  -  process an entire H5 message
    //
    bool DespatchRxPacket(PDU::bc_channel chan, bool rel, uint32 len);

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  H5StackBase::DecodeRxPacket  -  decode and entire bcsp message
    //
    void DecodeRxPacket(size_t len);

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  H5StackBase::CheckRxPacketLength  -  How many more bytes are needed
    //
    void CheckRxPacketLength();

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  H5StackBase::ReadBytesFromUART  -  Deal with bytes read from the UART
    //
    void ReadBytesFromUART();
};

#endif // H5STACKBASE_H__
