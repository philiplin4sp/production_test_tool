///////////////////////////////////////////////////////////////////////
//
//  FILE   :  H4DSStack_dsrx,cpp
//
//  AUTHOR :  Mark Marshall
//				(based on code by Mark RISON and Carl Orsborn)
//
//  CLASS  :  H4DSStack (H4DS RX)
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
///////////////////////////////////////////////////////////////////////

#include <cstring>
#include "common/algorithm.h"
#include <cassert>
#define ASSERT assert
#include <stdio.h>

#include "h4dsstack.h"
#include "transportapi.h"
#include "time/passive_timer.h"


/****************************************************************************
NAME
        dsrx_uart_deliverbytes  -  push uart bytes up into h4ds stack

IMPLEMENTATION NOTES
        This function receives all bytes received from the UART.  The bulk of
        traffic is passed up through the h4ds stack layers to form received
        h4/HCI messages.  However, h4ds layer messages are consumed from the
        received byte stream.

        dsrx_uart_deliverbytes() is a wrapper for the core function
        dsrx_x_uart_deliverbytes(), which does the real work.  The wrapper
        keeps pushing bytes into the core function until it reports
        that it has has passed all of the bytes up to the h4 layer.

        The wrapper function cannot call the core function if "len" is zero.
        This simplifies use of dsrx_x_uart_deliverbytes()'s calls to
        dsrx_msg_start() and dsrx_msg_start_scan().

        While we are receiving h4 traffic, the main state machine remains in
        st_awake_h4_msg, where the h4ds code does not inspect each byte
        flowing past.  This is important for performance.
*/

/*inline*/ uint32 H4DSStack::dsrx_x_uart_deliverbytes(const uint8 *b, uint32 n)
{
    uint32 ntaken = 0;
    bool message_start;

    // We always reset this to one here.  If we want to be efficent later
    // we can then set it to the correct value.
    mRxNeededLength = 1;

    switch (mDsRxState)
    {
    case st_scanning:
		/* In the Groggy state, we're only interested in
		   Wake-Up and I'm-Awake messages.  These are h4ds layer
		   messages, so we scan the byte stream for the start of
		   such messages.  We may have just woken up, so we may
		   be receiving junk from the UART.  We scan the inbound
		   byte stream, discarding bytes, until we match a
		   Wake-Up message.  This case also discards the stream
		   of 0x00 bytes expected at the start of a Wake-Up
		   message. */
		switch(dsrx_msg_start(*b))
		{
		case msglayer_h4:
			/* Only pass h4 traffic up out of the
			   h4ds stack if the deep sleep state
			   machine allows it, and if we've
			   completed link establishment.
			   Otherwise, just assume we're reading
			   junk, and so discard the h4 msg. */
			if (ds_can_receive() && !le_rx_choked())
				mDsRxState = st_h4_msg;
			else
				ntaken = 1;
			break;

		case msglayer_h4ds:
			/* We've matched the initial byte of
			   an h4ds msg, but we haven't consumed
			   it.  We've consumed any rubbish
			   before the initial msg byte. */
			scan_init_match_msg();
			mDsRxState = st_h4ds_msg;
			break;

		case msglayer_none:
			/* ntaken should equal n.  Wake-Up
			   messages' leading 0x00 bytes are
			   discarded here. */
			if (ds_can_receive() && !le_tx_choked())
				H4DS_EVENT(H4DS_EVT_SYNC_LOSS);
			ntaken = 1;
			break;

		default:
			H4DS_PANIC(H4DS_PANIC_INT_CORRUPTION);
			ntaken = 1;
			break;
		}
		break;

    case st_h4_msg:
		/* The byte stream is ducted up into the h4 layer to
		   form an h4 message.  The h4ds library is required to
		   make a maximum of one call to H4DS_DELIVERBYTES() per
		   call to h4ds_uart_deliverbytes(); this is signalled
		   by setting h4ds_deliverbytes_called to TRUE, which
		   will shortly cause h4ds_uart_deliverbytes() to
		   return.

		   The h4 layer signals that it is will be expecting the
		   start of a fresh h4 message on the next call to
		   h4ds_h4rx_deliverbytes() via the message_start flag.
		   We use this to return to the st_awake_scanning state,
		   where we search for the start of the next h4 or h4ds
		   message. */

		ntaken = h4rx_deliverbytes(b, n, message_start);
		if (message_start)
			mDsRxState = st_scanning;
		break;

    case st_h4ds_msg:
		switch (scan_match_msg(b, n, ntaken))
		{
		case msgid_none:
			/* Message incomplete. */
			break;
		case msgid_pre_wu:
		case msgid_prez_wu:
			mDsRxState = st_scanning;
			break;
		case msgid_wu:
			DB_PRINTF(("H4DS: DS-awake: Got WU!\n"));
			ds_sm(dsevtid_wu_rxed);
			mDsUartActive = true;
			mDsRxState = st_scanning;
			break;
		case msgid_ia:
			DB_PRINTF(("H4DS: DS-awake: Got IA!\n"));
			ds_sm(dsevtid_ia_rxed);
			mDsUartActive = true;
			mDsRxState = st_scanning;
			break;
		case msgid_yms:
			DB_PRINTF(("H4DS: DS-awake: Got YMS!\n"));
			ds_sm(dsevtid_yms_rxed);
			// NB: The Uart is not active now!
			mDsRxState = st_scanning;
			break;
		case msgid_sync:
			DB_PRINTF(("H4DS: DS-awake: Got SYNC!\n"));
			le_fsm(lemsgid_sync);
			mDsUartActive = true;
			mDsRxState = st_scanning;
			break;
		case msgid_sack:
			DB_PRINTF(("H4DS: DS-awake: Got SACK!\n"));
			le_fsm(lemsgid_sack);
			mDsUartActive = true;
			mDsRxState = st_scanning;
			break;
		case msgid_conf:
			DB_PRINTF(("H4DS: DS-awake: Got CONF!\n"));
			le_fsm(lemsgid_conf);
			mDsUartActive = true;
			mDsRxState = st_scanning;
			break;
		case msgid_cack:
			DB_PRINTF(("H4DS: DS-awake: Got CACK!\n"));
			le_fsm(lemsgid_cack);
			mDsUartActive = true;
			mDsRxState = st_scanning;
			break;
		case msgid_duff:
			DB_PRINTF(("H4DS: DS-awake: Got DUFF!\n"));
			/* Sync loss in the awake state means
			   something has gone wrong.  External
			   code should treat this as seriously
			   as an h4 sync loss. */

			if (ds_can_receive() && !le_tx_choked())
				H4DS_EVENT(H4DS_EVT_SYNC_LOSS);

			mDsUartActive = true;
			mDsRxState = st_scanning;
			break;

		default:
			DB_PRINTF(("H4DS: DS-awake: Got panic!\n"));
			H4DS_PANIC(H4DS_PANIC_INT_CORRUPTION);
			break;
		}
		break;

    default:
		H4DS_PANIC(H4DS_PANIC_INT_CORRUPTION);
		break;
    }

    return ntaken;
}

void H4DSStack::dsrx_uart_deliverbytes(const uint8 *buf, uint32 len)
{
    uint32 ntaken;

	for (ntaken = 0; ntaken < len;)
		ntaken += dsrx_x_uart_deliverbytes(buf + ntaken, len - ntaken);

    if (mRxNeededLength == 0)
		mRxNeededLength = 1;
}


/****************************************************************************
NAME
        dsrx_msg_start  -  Is this the start of a H4(DS) message

FUNCTION
*/

/*static*/ /*inline*/ H4DSStack::H4DS_MSGLAYER H4DSStack::dsrx_msg_start(uint8 b)
{
	switch(b)
	{
	/* Start of an h4 message? */
//HERE	case 0x01:      /* HCI command. */
	case 0x02:      /* HCI ACL data. */
	case 0x03:      /* HCI SCO data. */
	case 0x04:      /* HCI event. */
		return msglayer_h4;

	/* Start of the "sync" part of a h4ds message? */
	case 0x40:
		return msglayer_h4ds;

	default:
		break;
	}

	return msglayer_none;
}
