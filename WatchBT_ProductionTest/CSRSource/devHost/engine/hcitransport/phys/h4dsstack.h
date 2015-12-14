///////////////////////////////////////////////////////////////////////
//
//  FILE   :  H4DSStack.h
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
//            everything that the H4DS stack needs.  This means
//            that this file is larger than CJO would like.
//
//            The UART model used bu the host code is quite unusual.
//            Reads and Writes are assumed to never fail.  We can write
//            an arbitatarily large block in one go, but we can only
//            write one block at a time.
//
///////////////////////////////////////////////////////////////////////

#ifndef H4DSSTACK_H__
#define H4DSSTACK_H__

#ifdef WIN32
// Switch off warning about identifiers being truncated to 255 chars!
#pragma warning(disable : 4786)
#endif

#include "common/types.h"
#include "uart.h"
#include "thread/critical_section.h"
#include "thread/thread.h"
#include "thread/signal_box.h"
#include "transportapi.h"
#include "time/passive_timer.h"
#include "h4_queues.h"
#include <vector>

#undef NDEBUG
#define NDEBUG

#ifdef NDEBUG
#define DB_PRINTF(x) ((void)0)
#define DB_DUMPPKT(x) ((void)0)
#else
#include <stdio.h>
#define DB_PRINTF(x) printf x
#define DB_DUMPPKT(x) DumpPacket x
#endif


class H4DSStack : public Threadable
{
public:
    // Forward Definition
    class CallBackInterface;

    H4DSStack (UARTPtr aFile, CallBackInterface &callBacks, uint32 wakeupLength);
    virtual ~H4DSStack();

    bool ready (uint32 timeout);
    bool Start ();
    void Stop ();

    void sendCommand (const PDU &aPacket);
    void sendACLData (const PDU &aPacket);
    void sendSCOData (const PDU &aPacket);
    void sendRawData (const PDU &aPacket);
    void sendEvent   (const PDU &aPacket);

	// This is true if we are a Host (normally the case)
    void setUartDirection(bool aUartGoesToChip);

    void setDataWatch(DataWatcher *w);

	//void SleepControl(enum SleepType mode);
	//void PingPeriod(int millisecs);

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  class CallBackInterface  -  A class of callback functions
    //
    // FUNCTION
    // This is the base class for all of the H4DS callbacks.
    // Classes that use an H4DSStack should derive there own
    // class from this, and pass a reference to it to H4DSStack.
    // H4DSStack will then use the virtual functions as callbacks.
    //
    class CallBackInterface : public ::NoCopy
    {
	public:
		CallBackInterface() {}
		virtual ~CallBackInterface() {}

		/////////////////////////////////////////////////////////////////////
		// NAME
		//  H4DSStack::CallBackInterface::sentPacket  -  Packet has been sent
		//
		// FUNCTION
		//  This callback gets called when the stack has sent a packet.  The 
		//  boolean indicates if the packet was sent correctly or not.
		//
		virtual void sentPacket(bool);

		/////////////////////////////////////////////////////////////////////
		// NAME
		//  H4DSStack::CallBackInterface::recvPacket  -  Packet was recieved
		//
		// FUNCTION
		//  This callback gets called when the stack has recieved a
		//  packet.
		//
		virtual void recvPacket(const PDU &aPacket);

		/////////////////////////////////////////////////////////////////////
		// NAME
		//  H4DSStack::CallBackInterface::syncLost  -  The transport has crashed
		//
		// FUNCTION
		//  This callback gets called when the stack has recieved a
		//  bad packet (or the DS engine has detected errors).
		//
		virtual void syncLost(FailureMode mode);
    };

private:

    enum H4_PKTTYPE        /* See BT1.0B/H:4;2 */
    {
        H4_PKTTYPE_CMD = 1,
        H4_PKTTYPE_ACL = 2,
        H4_PKTTYPE_SCO = 3,
        H4_PKTTYPE_EVT = 4,
        H4_PKTTYPE_RAW = 5,
    };

	// General
    UARTPtr mFile;

	// Which role are we (the same code should work as a HostController emulation)
    enum { WeAreHost, WeAreController } mDirection;

    // The CallBackInterface contains all of the callbacks that we might need
    CallBackInterface	&mCallBacks;

	// ==== General ====

	enum
	{
		H4DS_MAX_WU_MSGS = 20
	};

    // This is the list of all timers in the H5/BCSP engine.  The
    // individual timers below should not be used.  Instead only the
    // list of timers should be used for all time update operations.
    // Classes derived from this one can add new timers to the list to
    // register timed events,
    PassiveTimerList	m_timerList;

	// ==== DEBUG ====

	static void DumpPacket(const char *desc, const uint8 *contents, uint32 length);

	enum H4DS_PANICS
	{
		H4DS_PANIC_INT_CORRUPTION
	};

	enum H4DS_EVENTS
	{
		/* Initialisation of the h4ds library has started. */
		H4DS_EVT_START,

		/* Initialisation of the h4ds library has been completed. */
		H4DS_EVT_INITED,

		/* The h4ds engine has lost sync while in its awake state.  Something has
		gone seriously wrong.  External code should treat this alert as seriously as
		an h4 sync loss. */
		H4DS_EVT_SYNC_LOSS,

		/* The h4ds code has allowed the peer to sleep. Consequently, any subsequent
		calls to h4ds_tx_msg() should be rejected until the peer is perceived to be
		awake again.  While the peer is apparently asleep all calls to h4ds_tx_msg()
		will be rejected. */
		H4DS_EVT_PEER_MAY_SLEEP,

		/* The h4ds code believes the peer is awake and able to receive
		link establishment and h4 traffic.  If the H4DS_EVT_LE_CONF event has
		also been emitted and if the H4DS_EVT_PEER_RESTART event has not been
		emitted then the h4ds code will normally pass h4 traffic. */
		H4DS_EVT_PEER_AWAKE,

		/* The h4ds library allows the local device to enter its deep sleep state, in
		which the local UART can be disabled.  See the description of
		H4DS_EVT_LOCAL_DEVICE_MAY_NOT_SLEEP. */
		H4DS_EVT_LOCAL_DEVICE_MAY_SLEEP,

		/* The h4ds library no longer allows the local device to enter its deep sleep
		state; the local UART must remain enabled.  External code may use
		H4DS_EVT_LOCAL_DEVICE_MAY_SLEEP and this event to decide whether the local
		device may enter its deep sleep state. */
		H4DS_EVT_LOCAL_DEVICE_MAY_NOT_SLEEP,

		/* The h4ds link establishment engine has established sync with its peer and
		the choke has been removed locally.  Messages may now be accepted by
		h4ds_tx_msg() and h4ds_uart_deliverbytes(). */
		H4DS_EVT_LE_SYNC,

		/* The h4ds link establishment engine has received confirmation from the peer
		that it's in its "garrulous" state, so allowing generation of the sync-lost
		event below. */
		H4DS_EVT_LE_CONF,

		/* The h4ds link establishment engine has detected that the peer h4ds-le
		engine has restarted.  This presumably means that the peer h4ds stack or
		system has been restarted (e.g., crash and reboot).  The common local
		response would be to restart (h4ds_init()) the local h4ds stack.  The h4ds
		engine will not pass any h4 traffic after emitting this event. */
		H4DS_EVT_PEER_RESTART,

		/* The function h4ds_h4_txmsg() has been called with dubious parameters. */
		H4DS_H4_EVT_DUFF_TX_PARAMS,

		/* ?? */
		H4DS_H4_EVT_H4_SYNC_LOST,

		/* ?? */
		H4DS_H4_EVT_OVERSIZE_RX_PAYLOAD
	};

	void H4DS_EVENT(H4DS_EVENTS e);
	void H4DS_PANIC(H4DS_PANICS p);

	// ==== DS ====

	enum
	{
		TWU_PERIOD = 250,
		TUART_IDLE_PERIOD = 50,
		TTXEMPTY_PERIOD = 20,
		TXEMPTY_COUNT = 3,
		TFLUSH_PERIOD = 20
	};

	enum H4DS_DSSTATE
	{
        state_init,                /* Initialise state machine. */
        state_flush_tx,			   /* Flush transmit uart. */
        state_torpid,              /* Local device may sleep. */
        state_drowsy,              /* Waking up. */
        state_peer_idle,           /* Peer is not responding. */
        state_awake,               /* Send and receive HL traffic. */
        state_awake_pass_rx,       /* Only receive HL traffic. */
        state_awake_pass_tx        /* Only transmit HL traffic. */
	};
	H4DS_DSSTATE mDsState;

	enum H4DS_DSEVTID
	{
        dsevtid_none,              /* No message. */
        dsevtid_hl_tx_msg_avail,   /* Higher layer msg available. */
        dsevtid_ia_rxed,           /* I'm-Awake msg received. */
        dsevtid_wu_rxed,           /* Wake-Up msg received. */
        dsevtid_yms_rxed,          /* You-May-Sleep msg received. */
        dsevtid_twu_timeout,       /* Wake-Up timer fired. */
        dsevtid_tflush_timeout,    /* UART flush timer fired. */
        dsevtid_uart_idle_timeout, /* UART idle timer fired. */
        dsevtid_rx_uart_activity,  /* UART rx activity detected. */
        dsevtid_tx_buffer_empty    /* Tx UART buffers empty. */
	};

	uint32 mDsNWakeup;

	bool mDsUartActive;
	bool mDsTxBufferEmptyCheckPending;
	bool mDsUartRxAlarm;

	bool mDsUartTxEmptyAlarm;
	int mDsUartTxEmptyAlarmCount;
    PassiveTimer mTimerTxEmptyAlarm;

    PassiveTimer mTimerWakeup;
    PassiveTimer mTimerFlush;
    PassiveTimer mTimerUartIdle;

	void ds_timer_wakeup();
	void ds_timer_flush();
	void ds_timer_uart_idle();
	void ds_alert_tx_buffer_empty();
	void ds_tx_pump_called();
	void ds_sm(H4DS_DSEVTID msg);
	void ds_state_flush_tx_prelude();
	void ds_state_torpid_prelude();
	void ds_state_awake_prelude();
	void ds_state_drowsy_prelude();
	bool ds_can_transmit();
	bool ds_can_receive();
	void ds_arm_tx_empty_alarm();
	void ds_cancel_tx_empty_alarm();
	void ds_timer_tx_empty_alarm();

	// ==== SCANNER ====

	enum H4DS_MSGLAYER
	{
		msglayer_none,
		msglayer_h4ds,
		msglayer_h4
	};

	enum H4DS_MSGID
	{
        msgid_none,           /* No message. */
        msgid_duff,           /* Unknown or corrupt message detected. */
        msgid_pre_wu,         /* Wake-Up Preamble. */
        msgid_prez_wu,        /* Wake-Up Last Preamble. */
        msgid_wu,             /* Wake-Up. */
        msgid_ia,             /* I'm-Awake. */
        msgid_yms,            /* You-May-Sleep. */
        msgid_sync,           /* Link establishment "Sync". */
        msgid_sack,           /* Link establishment "Sync Ack". */
        msgid_conf,           /* Link establishment "Conf". */
        msgid_cack,           /* Link establishment "Conf Ack". */
        msgid_h4              /* h4 layer: cmd/evt, ACL or SCO. */
	};

	enum
	{
		H4DS_MSGFRAG_LOCK_LEN = 8,
		H4DS_MSGFRAG_BODY_LEN = 4,
		H4DS_MSG_LEN = H4DS_MSGFRAG_LOCK_LEN + H4DS_MSGFRAG_BODY_LEN,
	};

	static const uint8 h4ds_lock_msgfrag[H4DS_MSGFRAG_LOCK_LEN];

	static const uint8 h4ds_yms_msgfrag[H4DS_MSGFRAG_BODY_LEN];
	static const uint8 h4ds_pre_wu_msgfrag[H4DS_MSGFRAG_BODY_LEN];
	static const uint8 h4ds_prez_wu_msgfrag[H4DS_MSGFRAG_BODY_LEN];
	static const uint8 h4ds_wu_msgfrag[H4DS_MSGFRAG_BODY_LEN];
	static const uint8 h4ds_ia_msgfrag[H4DS_MSGFRAG_BODY_LEN];
	static const uint8 h4ds_sync_msgfrag[H4DS_MSGFRAG_BODY_LEN];
	static const uint8 h4ds_sack_msgfrag[H4DS_MSGFRAG_BODY_LEN];
	static const uint8 h4ds_conf_msgfrag[H4DS_MSGFRAG_BODY_LEN];
	static const uint8 h4ds_cack_msgfrag[H4DS_MSGFRAG_BODY_LEN];

	struct MSGINFO
	{
        const uint8 *msg;       /* Message (body) byte sequence. */
        H4DS_MSGID id;          /* Corresponding message identifier. */
    };

	/* The complete set of h4ds messages. */
	static const MSGINFO h4ds_msgs[];

	enum H4DS_SCANNERSTATE
	{
        scst_lock = 0,             /* Matching "lock" part of a message. */
        scst_body_start = 1,       /* Seeking the start of a message body. */
        scst_in_body = 2           /* Matching "body" part of a message. */
	};

	H4DS_SCANNERSTATE mScannerState;
	uint32 mScannerIndex;
	const MSGINFO *mScannerMsg;

	void scan_init_match_msg();
	static const MSGINFO *scan_match_msg_start(uint8 b);
	H4DS_MSGID scan_match_msg(const uint8 *buf, const uint32 len, uint32 &ntaken);

	// ==== LE ====

	enum
	{
		TSYNC_PERIOD = 250,
		TCONF_PERIOD = 250
	};

	enum H4DS_LESTATE
	{
        state_shy,
        state_curious,
        state_garrulous,
		state_exit
	};
	H4DS_LESTATE mLeState;

	enum H4DS_LEMSGID
	{
        lemsgid_none,            /* No message. */
        lemsgid_sync,            /* Sync message. */
        lemsgid_sack,            /* Sync-acknowledge message. */
        lemsgid_conf,            /* Conf message. */
        lemsgid_cack,            /* Conf-acknowledge message. */
        lemsgid_tsync_timeout,   /* Message indicating Tsync timeout. */
        lemsgid_tconf_timeout,   /* Message indicating Tconf timeout. */
		lemsgid_rx_uart_activity /* Message indicating activity from the peer. */
	};

	bool mLePaused;
	bool mLePausedTsync;
	bool mLePausedTconf;

    PassiveTimer mTimerConf;
    PassiveTimer mTimerSync;

	void le_fsm(H4DS_LEMSGID msg);
	void le_timer_tconf();
	void le_timer_tsync();
	bool le_tx_choked();
	bool le_rx_choked();
	void le_pause();
	void le_unpause();

	// ==== DS RX ====

	enum H4DS_RXSTATE
	{
        st_scanning = 0,	/* Seeking Wake-Up or I'm-Awake message. */
        st_h4_msg = 1,		/* Matching h4 message. */
        st_h4ds_msg = 2		/* Matching h4ds layer message. */
	};
    H4DS_RXSTATE mDsRxState;

	/*inline*/ static H4DS_MSGLAYER dsrx_msg_start(uint8 b);
	static H4DS_MSGLAYER dsrx_msg_start_scan(const uint8 *buf, uint32 len, uint32 &offset);
	/*inline*/ uint32 dsrx_x_uart_deliverbytes(const uint8 *b, uint32 n);
	void dsrx_uart_deliverbytes(const uint8 *buf, uint32 len);

	// ==== DS TX ====

	enum H4DS_DSMSGID
	{
        dsmsgid_none = 0,       /* No message. */
        dsmsgid_wu = 1,         /* Wake-Up. */
        dsmsgid_ia = 2,         /* I'm-Awake. */
        dsmsgid_yms = 3         /* You-May-Sleep. */
	};

	bool mReqTxSync;
	bool mReqTxSack;
	bool mReqTxConf;
	bool mReqTxCack;

	// This is a bit gross.  This vector contains the correct number of
	// 0's for the wakeup preamble.  We really need to send it in one
	// chunk so there are no gaps when we are using a high baud rate.
	std::vector<uint8> mWakeupMsg;

	std::list<H4DS_DSMSGID> mDsMsgQ;

	bool mPumpReq;

	bool dstx_can_send_h4_packet();
	void dstx_pump();
	void dstx_pump_le();
	bool dstx_pump_ds();
	void dstx_req_le_sync();
	void dstx_req_le_sack();
	void dstx_req_le_conf();
	void dstx_req_le_cack();
	void dstx_req_ds_ia();
	void dstx_req_ds_wu();
	void dstx_req_ds_yms();
	bool dstx_ds_msg_pending();
	bool dstx_le_msg_pending();
	bool dstx_path_clear();
	void dstx_h4ds_msg(const uint8 *msgbody);

	// ==== H4 RX ====

	enum
	{
		H4DS_H4_MAX_RX_DATA_PAYLOAD = 65535
	};

	// These are the states as we read in a packet.
	enum H4_RXSTATE
	{
        st_idle,		/* Waiting for start of new H4 message. */
        st_cmdhdr1,    		/* HCI command opcode 1 */
        st_cmdhdr2,    		/* HCI command opcode 2 */
        st_cmdhdr3,    		/* HCI command length. */
        st_evthdr1,    		/* HCI event opcode 1 */
        st_evthdr2,    		/* HCI event length. */
        st_datahdr1,   		/* HCI data handle 1 */
        st_datahdr2,   		/* HCI data handle 2 */
        st_datahdr3,   		/* HCI length 1 */
        st_datahdr4,   		/* HCI length 2 */
        st_scohdr1,   		/* HCI SCO handle 1 */
        st_scohdr2,   		/* HCI SCO handle 2 */
        st_scohdr3,   		/* HCI length 1 */
        st_body,			/* Receiving message body. */
        st_sync_lost		/* Rubbish received. */
	};
	H4_RXSTATE mRxState;

    uint8 mRxPktType;
	uint16 mRxTag;
	uint32 mRxPktLength;
	std::vector<uint8> mRxPacketBuffer;
	bool mRxH4InSync;

	void h4rx_delivermsg();
	uint32 h4rx_deliverbytes(const uint8 *buf, uint32 bufsiz, bool &msg_start);

	// ==== H4 TX ====

	H4Queues mTxQueue;

	uint32 mTxWritten;

	void h4tx_send_bytes(const uint8 *data, size_t len);
	void h4tx_send_packet();

	// ==== UART RX ====

	uint8 mRxBuffer[1024];
	uint32 mRxLength;

	// This variable records the length of data that we need to read in
	// order to finish the packet.  In situations where we are reading
	// the header it contains the length needed to finish the header.
	uint32 mRxNeededLength;

	// ==== UART TX ====

	bool mTxInProgress;

	// ==== MISC ====

    virtual int ThreadFunc();

    SignalBox mThreadSignal;
    SignalBox::Item mThreadDead;

    void sendPDU(const PDU &pdu, uint8 header);
};

#endif // H4DSSTACK_H__
