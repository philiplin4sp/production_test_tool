///////////////////////////////////////////////////////////////////////
//
//  FILE   :  BCSPH5Stack.h
//
//  Copyright CSR 2002-2006
//
//  AUTHOR :  Mark Marshall
//
//  CLASS  :  BCSPH5Stack
//
//  PURPOSE:  BCSP specific portion of H5/BCSP implementation of
//            HCITransport
//
//            The class defined in this file provides a complete 
//            implimentation of an BCSP Stack.  It is derived from 
//            H5StackBase.  It impliemts the complete BCSP Link 
//            Establishment engine.  It is an alternative to the
//            old BCSP engine (which is C using longjmp).
//
//  MODIFICATION HISTORY:
//
///////////////////////////////////////////////////////////////////////

#ifndef BCSPH5Stack_H__
#define BCSPH5Stack_H__

#include "h5stackbase.h"

class BCSPH5Stack : public H5StackBase
{
public:

    BCSPH5Stack(UARTPtr aFile, CallBackInterface &callBacks,
		uint32 Tretx = 250, uint32 Tshy = 250, uint32 Tconf = 250,
		bool useCRCs = true, uint8 winsize = 4,
		bool muzzleAtStart = false);

    virtual ~BCSPH5Stack ();

protected:

	// This is the length of the LE message header (conf/conf resp mesages might be longer)
	enum { LEMSG_HEADER_LEN = 4 };

	// lemsgid's are internally passed around by the LE engine.
	enum lemsgid
	{
		lemsgid_none,			/* no message */
		lemsgid_sync,			/* sync message */
		lemsgid_sync_resp,		/* sync-resp message */
		lemsgid_conf,			/* conf message */
		lemsgid_conf_resp,		/* conf-resp message */
		lemsgid_tshy_timeout,	/* message indicating Tshy timeout */
		lemsgid_tconf_timeout,	/* message indicating Tconf timeout */
	};

	// These are the actual LE messages
	static const uint8 sync_payload[LEMSG_HEADER_LEN];
	static const uint8 sync_resp_payload[LEMSG_HEADER_LEN];
	static const uint8 conf_payload[LEMSG_HEADER_LEN];
	static const uint8 conf_resp_payload[LEMSG_HEADER_LEN];

	// This payload table maps the LE messages to the message id's above
	struct payload_table_s
	{
		lemsgid id;
		const uint8 *data;
	};

	static const payload_table_s payload_table[];

	// Timers for LE
	PassiveTimer		m_timerTshy;
	PassiveTimer		m_timerTconf;

	// Configuration data for H5
	bool				m_useCRCs;	// Do we send CRCs

	enum bcsp_le_state
	{
        le_state_uninit,
        le_state_init,
        le_state_active
	};

	bcsp_le_state		m_le_state;				// The stat of the h5-le-fsm

    bool				m_le_muzzled;			// h5-le passive start.

    bool				m_txsync_req;			// h5-le sync requested.
    bool    			m_txsyncresp_req;		// h5-le syncresp requested.
    bool    			m_txconf_req;			// h5-le conf requested.
    bool    			m_txconfresp_req;		// h5-le confresp requested.

	// The LE engine.
	void bcsple_fsm(lemsgid msg);

	// Functions used to map between the wire channel number and
	// the protocol id used in the rest of the stack.
	virtual uint8 MapProtocolToWire(PDU::bc_channel proto);
	virtual PDU::bc_channel MapWireToProtocol(uint8 wire);

	// Should we add CRCs to the packets that we are sending
	virtual bool SendCRCs();
	virtual bool SendLECRCs();

	// The LE engine plumbing.
	virtual void le_start();
	virtual uint32 le_message_avail(uint8 *data);
	virtual void le_putmsg(uint32 len);
	void le_tconf();
	void le_tshy();

	// Make sure that the chip is awake
	virtual bool ForceAwake();
};

#endif // BCSPH5Stack_H__
