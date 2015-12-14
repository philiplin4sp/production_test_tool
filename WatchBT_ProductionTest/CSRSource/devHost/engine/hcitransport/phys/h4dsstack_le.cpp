///////////////////////////////////////////////////////////////////////
//
//  FILE   :  H4DSStack_le,cpp
//
//  AUTHOR :  Mark Marshall
//				(based on code by Mark RISON and Carl Orsborn)
//
//  CLASS  :  H4DSStack (LE sections)
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
        le_fsm  -  the h4ds link establishment entity state machine

FUNCTION
        Feeds the message "msg" into the h4ds link establishment state
        machine.

IMPLEMENTATION NOTE
        The code doesn't generally bother to cancel timers - they time
        out harmlessly, so there's no point.
*/

void H4DSStack::le_fsm(H4DS_LEMSGID msg)
{
	switch (mLeState)
	{
	case state_shy:
		switch (msg)
		{
		case lemsgid_tsync_timeout:
		case lemsgid_rx_uart_activity:
			dstx_req_le_sync();
			mTimerSync.SetTimer();
			break;

		case lemsgid_sync:
			dstx_req_le_sack();
			break;

		case lemsgid_sack:
			H4DS_EVENT(H4DS_EVT_LE_SYNC);
			dstx_req_le_conf();
			mTimerConf.SetTimer();
			mLeState = state_curious;
			break;

		default:
			break;
		}
		break;

	case state_curious:
		switch (msg)
		{
		case lemsgid_tconf_timeout:
			dstx_req_le_conf();
			mTimerConf.SetTimer();
			break;

		case lemsgid_sync:
			dstx_req_le_sack();
			break;

		case lemsgid_conf:
			dstx_req_le_cack();
			break;

		case lemsgid_cack:
			H4DS_EVENT(H4DS_EVT_LE_CONF);
			mLeState = state_garrulous;
			break;

		default:
			break;
		}
		break;

	case state_garrulous:
		switch (msg)
		{
		case lemsgid_conf:
			dstx_req_le_cack();
			break;

		case lemsgid_sync:
			/* Peer has apparently restarted. */
			H4DS_EVENT(H4DS_EVT_PEER_RESTART);
			mLeState = state_exit;
			mCallBacks.syncLost(h4_sync_loss);
			break;
		default:
			break;
		}
		break;

	case state_exit:
        /* Block all h4 traffic (until reinitialised). */
		break;

	default:
        H4DS_PANIC(H4DS_PANIC_INT_CORRUPTION);
		break;
	}
}


/****************************************************************************
NAME
        h4ds_le_tx_choked  -  is the h4ds transmit path choked?
*/

bool H4DSStack::le_tx_choked()
{
	return mLeState != state_garrulous;
}


/****************************************************************************
NAME
        h4ds_le_rx_choked  -  is the h4ds receive path choked?
*/

bool H4DSStack::le_rx_choked()
{
	return (mLeState != state_garrulous) && (mLeState != state_curious);
}


/****************************************************************************
NAME
        le_pause  -  pause the link establishment engine

FUNCTION
        Pause the link establishment state machine until it is unpaused by a
        call to le_unpause().

        This function exists to allow the deep sleep state machine to shut
        down the link establishment state machine when the peer cannot be
        contacted (e.g., UART wire drop out or peer crash).  In this state it
        is pointless to continue to run local code to try to rouse the peer
        as it just wastes power.  Hence the pause function to hold the link
        establishment state machine.

IMPLEMENTATION NOTES
        We know that the pause function will only be called from the ds code,
        and that in that state it will not be able to receive any messages
        from there peer.  Hence, the le engine only has to worry about timed
        events in its paused state.

        The code in the pause and unpause functions, and related code in the
        two timed event catcher functions, combines to record timed events in
        the paused state and pass any recorded timed events into the state
        machine when it is unpaused.  Integrating the pause/unpause logic
        into the state machine would create a horrid mess - of code and of
        the design.
*/

void H4DSStack::le_pause()
{
	mLePaused = true;
}


/****************************************************************************
NAME
        le_unpause  -  unpause the link establishment engine

IMPLEMENTATION NOTES
        See the comments for le_pause().
*/

void H4DSStack::le_unpause()
{
	mLePaused = false;

	if (mLePausedTsync)
	{
		mLePausedTsync = false;
		le_fsm(lemsgid_tsync_timeout);
	}

	if (mLePausedTconf)
	{
		mLePausedTconf = false;
		le_fsm(lemsgid_tconf_timeout);
	}
}


/****************************************************************************
NAME
        le_timer_tsync  -  report a tsync timeout event to the fsm

FUNCTION
		Called when the Tsync timer expires.  It pushed the correct
		message into the le_fsm.
*/

void H4DSStack::le_timer_tsync()
{
	if (mLePaused)
		mLePausedTsync = true;
	else
		le_fsm(lemsgid_tsync_timeout);
}

/****************************************************************************
NAME
        le_timer_tconf  -  report a tconf timeout event to the fsm

FUNCTION
		Called when the Tconf timer expires.  It pushed the correct
		message into the le_fsm.
*/

void H4DSStack::le_timer_tconf()
{
	if (mLePaused)
		mLePausedTconf = true;
	else
		le_fsm(lemsgid_tconf_timeout);
}


