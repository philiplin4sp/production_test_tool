///////////////////////////////////////////////////////////////////////
//
//  FILE   :  H5Stack.h
//
//  Copyright CSR 2002-2006
//
//  AUTHOR :  Mark Marshall
//
//  CLASS  :  H5Stack
//
//  PURPOSE:  H5 specific portion of H5 implementation of HCITransport
//
//            The class defined in this file provides a complete
//            implimentation of an H5 Stack.  It is derived from
//            H5StackBase.  It impliemts the complete H5 Link
//            Establishment engine.
//
//  MODIFICATION HISTORY:
//
///////////////////////////////////////////////////////////////////////

#ifndef H5STACK_H__
#define H5STACK_H__

#include "h5stackbase.h"

class H5Stack : public H5StackBase
{
 public:
    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  H5Stack  -  ctor
    //
    H5Stack(UARTPtr aFile, CallBackInterface &callBacks,
	    uint32 Tretx = 250, uint32 Tshy = 250, uint32 Tconf = 250,
	    bool useCRCs = true, bool useOOF = false, uint8 winsize = 4,
	    uint32 Twakeup = 50, bool muzzleAtStart = false);

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  ~H5Stack  -  dtor
    //
    virtual ~H5Stack ();

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  HostSleep  -  The host is going to sleep
    //
    // FUNCTION
    //  Tell the peer (Host Controller) that we are going to sleep.  In H5
    //  that means we are going to send a sleep message.
    //
    void HostSleep();

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  HostWakeup  -  The host is awake (and wants to talk to the peer)
    //
    // FUNCTION
    //  Tell the peer (Host Controller) that we are waking up.  In H5
    //  that means we are going to send a wakeup message.
    //
    void HostWakeup();

 protected:

    // These are the H5 protocol numbers.  These are the only channel
    // numbers that have been allocated so far in the H5 spec.  We do
    // not remap the other BCSP channels to new H5 channels.  All CSR
    // specific data must be tunneled at the moment.
    enum
    {
	IO_DATA_H5_PROTOCOL_ACK = 0,
	IO_DATA_H5_PROTOCOL_HCI_CMD	= 1,
	IO_DATA_H5_PROTOCOL_HCI_ACL = 2,
	IO_DATA_H5_PROTOCOL_HCI_SCO = 3,
	IO_DATA_H5_PROTOCOL_HCI_EVT = 4,
	IO_DATA_H5_PROTOCOL_LINK_MGT = 15
    };

    // This is the length of the LE message header (conf/conf resp
    // mesages might be longer)
    enum { LEMSG_HEADER_LEN = 2 };

    // lemsgid's are internally passed around by the LE engine.
    enum lemsgid
    {
	lemsgid_none,		/* no message */
	lemsgid_sync,		/* sync message */
	lemsgid_sync_resp,	/* sync-resp message */
	lemsgid_config,		/* config message */
	lemsgid_config_resp,	/* config-resp message */
	lemsgid_wakeup,		/* wakeup message */
	lemsgid_woken,		/* woken message (wakeup_resp) */
	lemsgid_sleep,		/* sleep message */
	lemsgid_tshy_timeout,	/* message Tshy timeout */
	lemsgid_tconf_timeout,	/* message Tconf timeout */
	lemsgid_twakeup_timeout,/* message Twakeup timeout */
	lemsgid_send_wakeup,	/* send a wakeup message to the peer */
	lemsgid_send_sleep	/* send a sleep message to the peer */
    };

    // These are the actual LE messages
    static const uint8 sync_payload[2];
    static const uint8 sync_resp_payload[2];
    static const uint8 config_payload[2];
    static const uint8 config_resp_payload[2];
    static const uint8 wakeup_payload[2];
    static const uint8 woken_payload[2];
    static const uint8 sleep_payload[2];

    // This payload table maps the LE messages to the message id's above
    struct payload_table_s
    {
	lemsgid id;
	const uint8 *data;
    };

    static const payload_table_s payload_table[];

    // Timers for LE
    PassiveTimer	m_timerTshy;
    PassiveTimer	m_timerTconf;

    // Timer that is used for the Wakeup state machine
    PassiveTimer	m_timerTwakeup;

    // Do we send CRCs
    bool	m_useCRCs;

    // Can we do Out Of Band Flow Control XON/XOFF (no!)
    bool	m_useOOf;

    enum h5_le_state
    {
	le_state_uninit,
	le_state_init,
	le_state_active
    };

    h5_le_state	m_le_state;		// The stat of the h5-le-fsm

    bool	m_le_muzzled;		// h5-le passive start.

    bool	m_txsync_req;		// h5-le sync requested.
    bool    	m_txsyncresp_req;	// h5-le syncresp requested.
    bool    	m_txconf_req;		// h5-le conf requested.
    bool    	m_txconfresp_req;	// h5-le confresp requested.
    bool    	m_txwakeup_req;		// h5-le wake requested.
    bool    	m_txwoken_req;		// h5-le woken requested.
    bool    	m_txsleep_req;		// h5-le sleep requested.

    enum h5_peer_asleep
    {
	h5sleep_awake,
	h5sleep_asleep,
	h5sleep_waking
    };

    // is the peer device asleep
    h5_peer_asleep		m_peer_sleep_state;

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  MapProtocolToWire  -  Map packet type to wire channel number
    //
    virtual uint8 MapProtocolToWire(PDU::bc_channel proto);

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  MapWireToProtocol  -  Map wire channel number to packet type
    //
    virtual PDU::bc_channel MapWireToProtocol(uint8 wire);

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  SendCRCs  -  Should we add a CRC to a general packet
    //
    virtual bool SendCRCs();

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  SendLECRCs  -  Should we add a CRC to a LE packet
    //
    virtual bool SendLECRCs();

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  h5le_fsm  -  The H5 Link Establishment engine
    //
    void h5le_fsm(lemsgid msg);

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  le_start  -  Start the LE engine
    //
    virtual void le_start();

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  le_message_avail  -  is there a new LE transmit message available
    //
    virtual uint32 le_message_avail(uint8 *data);

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  le_putmsg  -  we have recieved a new LE message.
    //
    virtual void le_putmsg(uint32 len);

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  ForceAwake  -  Force the peer into life

    virtual bool ForceAwake();

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  le_tconf  -  The T-Conf timer has expired, maybe send a conf message
    //
    void le_tconf();

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  le_tshy  -  The T-Shy timer has expired, maybe send a sync message
    //
    void le_tshy();

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  le_twakeup  -  The T-Wakeup timer has expired, maybe ?
    //
    void le_twakeup();
};

#endif // H5STACK_H__
