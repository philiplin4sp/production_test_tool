///////////////////////////////////////////////////////////////////////
//
//  FILE   :  H4DSStack_dstx,cpp
//
//  AUTHOR :  Mark Marshall
//				(based on code by Mark RISON and Carl Orsborn)
//
//  CLASS  :  H4DSStack (H4DS TX)
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
        dstx_can_send_h4_packet  -  can we send an H4 packet

FUNCTION
	If it is possible to send an H4 message at the moment this
	function will return true.  It is the callers job to actually
	send the message.  If we cannot send a mesage at the moment this
	function will inform the h4ds state machine that there is a higher
	layer message available.
*/

bool H4DSStack::dstx_can_send_h4_packet()
{
	/* To get here the caller must be requesting to send a
	complete h4 message.

	Are we allowed to send the h4 message?

	- Is the link establishment engine refusing to pass traffic?
	- Are there any pending requests to send link establishment
			messages (which have higher priority than h4)?
	- Is the deep sleep engine in a state to allow messages to
			be transmitted? 
	- Has the deep sleep engine any pending requests to send
			deep sleep msgs (which have higher priority than h4)? */
	if (le_tx_choked() || dstx_le_msg_pending() || !ds_can_transmit() || dstx_ds_msg_pending())
	{
		/* Permission refused.  Signal that there's an
		attempt has been made to send an h4 message.  This
		may try to rouse the peer. */
		if (!le_tx_choked())
			ds_sm(dsevtid_hl_tx_msg_avail);

		return false;
	}

	/* Defences passed.  Can send the bytes. */
	return true;
}


/****************************************************************************
NAME
        dstx_pump  -  send h4ds messages to the uart
*/

void H4DSStack::dstx_pump()
{
	ASSERT(!mTxInProgress);

	/* We're only allowed to send one message per call to the
	pump function, so only call the link establishment engine if
	the deep sleep engine has not tried to send. */

	if (dstx_pump_ds())
	{
		/* If we have an LE message to send then we need to be woken up 
		again.  If we still have outstanding DS messages then this will
		already be set. */
		if (dstx_le_msg_pending())
			mPumpReq = true;
	}
	else
		dstx_pump_le();

	ds_tx_pump_called();
}


/****************************************************************************
NAME
        dstx_pump_le  -  transmit pending h4ds link establishment messages

FUNCTION
        Attempt to send a single pending h4ds link establishment message to
        the peer.
*/

void H4DSStack::dstx_pump_le()
{
	if (dstx_le_msg_pending())
	{
		if (ds_can_transmit() && !dstx_ds_msg_pending())
		{
			/* We have (at least) one pending request to send a
			link establishment message; try to honour it. */
			if (mReqTxSync)
			{
				dstx_h4ds_msg(h4ds_sync_msgfrag);
				mReqTxSync = false;
			}
			else if (mReqTxSack)
			{
				dstx_h4ds_msg(h4ds_sack_msgfrag);
				mReqTxSack = false;
			}
			else if (mReqTxConf)
			{
				dstx_h4ds_msg(h4ds_conf_msgfrag);
				mReqTxConf = false;
			}
			else if (mReqTxCack)
			{
				dstx_h4ds_msg(h4ds_cack_msgfrag);
				mReqTxCack = false;
			}
			else
			{
				H4DS_PANIC(H4DS_PANIC_INT_CORRUPTION);
			}
		}
		else
		{
			/* Kick the deep sleep engine if no link establishment
			message has been sent.  (It could have failed because the
			deep sleep engine thinks the peer is asleep, so this call
			should attempt to rouse the peer.  It could also have failed
			because the UART refused the bytes.  In this case the signal
			to the deep sleep engine should be harmlessly ignored. */
			ds_sm(dsevtid_hl_tx_msg_avail);
		}

		/* Arrange to be called again if there's still work to do. */
		if (dstx_le_msg_pending())
			mPumpReq = true;
	}
}


/****************************************************************************
NAME
        dstx_pump_ds  -  transmit pending h4ds deep sleep messages

FUNCTION
        Attempt to send a single pending h4ds deep sleep message to the
        peer.

RETURNS
        TRUE if an attempt has been made to send a h4ds deep sleep message
        to the peer, else FALSE.

        (A return value of TRUE does not imply that a message *has* been
        sent to the peer.)
*/

bool H4DSStack::dstx_pump_ds()
{
	/* Have we a message to send to the UART? */
	if (!dstx_ds_msg_pending())
		return false;

	/* If mDsMsgQ is not empty then we will be attempting to send 
	bytes to the UART, which is what we need to report to our
	caller. */

	/* Try to send the deep sleep msg to the UART. */
	switch (mDsMsgQ.front())
	{
	case dsmsgid_wu:
		ASSERT(!mTxInProgress);
		h4tx_send_bytes(&mWakeupMsg[0], mWakeupMsg.size());
		break;

	case dsmsgid_ia:
		dstx_h4ds_msg(h4ds_ia_msgfrag);
		break;

	case dsmsgid_yms:
		dstx_h4ds_msg(h4ds_yms_msgfrag);
		break;

	default:
		H4DS_PANIC(H4DS_PANIC_INT_CORRUPTION);
		break;
	}

	mDsMsgQ.pop_front();

	/* If there are still more messages waiting to be sent then
	arrange to be called again. */
	if (dstx_ds_msg_pending())
		mPumpReq = true;

	return true;
}


/****************************************************************************
NAME
        dstx_req_*  -  request to transmit an LE or DS packet
*/

void H4DSStack::dstx_req_le_sync()
{
	mReqTxSync = true;
	mPumpReq = true;
}

void H4DSStack::dstx_req_le_sack()
{
	mReqTxSack = true;
	mPumpReq = true;
}

void H4DSStack::dstx_req_le_conf()
{
	mReqTxConf = true;
	mPumpReq = true;
}

void H4DSStack::dstx_req_le_cack()
{
	mReqTxCack = true;
	mPumpReq = true;
}

void H4DSStack::dstx_req_ds_ia()
{
	mDsMsgQ.push_back(dsmsgid_ia);
	mPumpReq = true;
}

void H4DSStack::dstx_req_ds_wu()
{
	mDsMsgQ.push_back(dsmsgid_wu);
	mPumpReq = true;
}

void H4DSStack::dstx_req_ds_yms()
{
	mDsMsgQ.push_back(dsmsgid_yms);
	mPumpReq = true;
}


/****************************************************************************
NAME
    dstx_ds_msg_pending  -  Is there a DS message pending to transmit?
*/

bool H4DSStack::dstx_ds_msg_pending()
{
	return !mDsMsgQ.empty();
}


/****************************************************************************
NAME
    dstx_le_msg_pending  -  Is there an LE message pending to transmit?
*/

bool H4DSStack::dstx_le_msg_pending()
{
	return (!mLePaused) && (mReqTxSync || mReqTxSack || mReqTxConf || mReqTxCack);
}


/****************************************************************************
NAME
        dstx_path_clear  -  are any DS or LE messages pending to transmit?
*/

bool H4DSStack::dstx_path_clear()
{
    return !dstx_le_msg_pending() && !dstx_ds_msg_pending();
}


/****************************************************************************
NAME
        dstx_h4ds_msg  -  send an h4ds message to the uart

FUNCTION
        Builds an h4ds message and sends it to the UART.  "msgbody" must
        point to a byte buffer of size H4DS_MSGFRAG_BODY_LEN, holding the
        h4ds's message body.
*/

void H4DSStack::dstx_h4ds_msg(const uint8 *msgbody)
{
	uint8 msg[H4DS_MSG_LEN];

	ASSERT(!mTxInProgress);

	/* Assemble the h4ds message. */
	memcpy(msg, h4ds_lock_msgfrag, H4DS_MSGFRAG_LOCK_LEN);
	memcpy(msg + H4DS_MSGFRAG_LOCK_LEN, msgbody, H4DS_MSGFRAG_BODY_LEN);

	h4tx_send_bytes(msg, H4DS_MSG_LEN);
}
