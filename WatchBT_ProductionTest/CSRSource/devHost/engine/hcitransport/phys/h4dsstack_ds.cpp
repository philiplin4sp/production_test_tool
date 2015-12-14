///////////////////////////////////////////////////////////////////////
//
//  FILE   :  H4DSStack_ds,cpp
//
//  AUTHOR :  Mark Marshall
//				(based on code by Carl Orsborn)
//
//  CLASS  :  H4DSStack (DS sections)
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
        ds_timer_wakeup  -  wake-up timed event fired
*/

void H4DSStack::ds_timer_wakeup()
{
	ds_sm(dsevtid_twu_timeout);
}

/****************************************************************************
NAME
        ds_timer_flush  -  flush timed event fired
*/

void H4DSStack::ds_timer_flush()
{
	ds_sm(dsevtid_tflush_timeout);
}

/****************************************************************************
NAME
        ds_timer_uart_idle  -  uart idle timed event fired
*/

void H4DSStack::ds_timer_uart_idle()
{
	DB_PRINTF(("Idle timer fired %d\n", mDsUartActive));
	ds_sm(dsevtid_uart_idle_timeout);
}


/****************************************************************************
NAME
        ds_alert_tx_buffer_empty  -  The transmit FIFO's are empty
*/

void H4DSStack::ds_alert_tx_buffer_empty()
{
	if (dstx_path_clear())
		ds_sm(dsevtid_tx_buffer_empty);
	else
		mDsTxBufferEmptyCheckPending = true;
}


/****************************************************************************
NAME
        ds_tx_pump_called  -  Pump has been called
*/

void H4DSStack::ds_tx_pump_called()
{
    if (mDsTxBufferEmptyCheckPending)
	{
        mDsTxBufferEmptyCheckPending = false;
        ds_arm_tx_empty_alarm();
    }
}


/****************************************************************************
NAME
        ds_sm  -  the h4ds deep sleep state machine

FUNCTION
        Feeds the message "msg" into the h4ds transmit state machine.
*/

void H4DSStack::ds_sm(H4DS_DSEVTID msg)
{
    switch(mDsState)
	{
	case state_init:          /* Initialising local state machine. */
		/* Kill the state machine's timer, in case it's running. */
		mTimerWakeup.SetPeriod(TWU_PERIOD);
		mTimerFlush.SetPeriod(TFLUSH_PERIOD);
		mTimerUartIdle.SetPeriod(TUART_IDLE_PERIOD);

		/* Lose interest in being told when any UART receive activity
		is detected, and in when the UART's transmit buffers are
        empty. */
		mDsUartRxAlarm = false;
		ds_cancel_tx_empty_alarm();

		/* Initially assume UART is idle. */
		mDsUartActive = false;

        /* Remove any pending req to check for empty UART tx buf. */
        mDsTxBufferEmptyCheckPending = false;

		/* Allow the local device to sleep until either higher level
		traffic is ready to be transmitted (e.g., a link
		establishment message) or until we receive traffic from the
		peer. */
		ds_state_torpid_prelude();
		break;

	case state_flush_tx: /* Flush transmit uart. */
		switch(msg)
		{
		case dsevtid_tx_buffer_empty:
			mTimerFlush.Cancel();
			ds_state_torpid_prelude();
			break;

        case dsevtid_tflush_timeout:
			ds_cancel_tx_empty_alarm();
            ds_state_torpid_prelude();
            break;

		case dsevtid_hl_tx_msg_avail:
			ds_cancel_tx_empty_alarm();
			mTimerFlush.Cancel();
            ds_state_drowsy_prelude();
			break;

		case dsevtid_wu_rxed:
			ds_cancel_tx_empty_alarm();
			mTimerFlush.Cancel();
            ds_state_drowsy_prelude();

			/* Send an I'm-Awake message to the peer. */
			dstx_req_ds_ia();
			break;

		default:
			break;
		}
		break;

	case state_torpid:        /* Local device may sleep. */
		switch(msg)
		{
		case dsevtid_hl_tx_msg_avail:
			H4DS_EVENT(H4DS_EVT_LOCAL_DEVICE_MAY_NOT_SLEEP);
            ds_state_drowsy_prelude();
			break;
		case dsevtid_wu_rxed:
			H4DS_EVENT(H4DS_EVT_LOCAL_DEVICE_MAY_NOT_SLEEP);
			ds_state_drowsy_prelude();
			dstx_req_ds_ia();
			break;
		default:
			break;
		}
		break;

	case state_drowsy:          /* Waking up. */
		switch(msg)
		{
		case dsevtid_wu_rxed:
			/* Send an I'm-Awake message to the peer. */
			dstx_req_ds_ia();
			break;

		case dsevtid_twu_timeout:
			if (mDsNWakeup >= H4DS_MAX_WU_MSGS)
			{
				/* Give up waiting for the peer to respond to
				our Wake-Up messages, and go idle.  In the
				idle state we'll only start to do things
				again if the UART receives traffic. */
				mDsUartRxAlarm = true;

				/* The link appears to be dead, so there's no
				point in allowing the local link
				establishment state machine to continue to
				emit its messages every 250ms.  Pause le, or
				we'll never get any sleep. */
				le_pause();

				/* We let the local machine sleep if the
				peer is unresponsive - no point in wasting
				power. */
				H4DS_EVENT(H4DS_EVT_LOCAL_DEVICE_MAY_SLEEP);

				mDsState = state_peer_idle;
			}
			else
			{
				/* Send a Wake-Up message to the peer. */
				dstx_req_ds_wu();

				/* Continue train of Wake-Up msgs. */
				mTimerWakeup.SetTimer();

				/* Bump the counter of wake-up messages. */
				++mDsNWakeup;
			}
			break;

		case dsevtid_ia_rxed:
			/* We've received an I'm-Awake message.  Take this
			to mean that the peer is awake. */
			H4DS_EVENT(H4DS_EVT_PEER_AWAKE);

			/* Cancel the train of Wake-Up messages. */
			mTimerWakeup.Cancel();

			ds_state_awake_prelude();
			break;

		default:
			break;
		}
		break;

	case state_peer_idle:       /* Peer is not responding. */
		switch(msg)
		{
		case dsevtid_rx_uart_activity:
			le_unpause();
			ds_state_drowsy_prelude();
			break;
		default:
			break;
		}
		break;

	case state_awake:           /* Send and receive traffic. */
		switch(msg)
		{
		case dsevtid_wu_rxed:
			/* Send an I'm-Awake message to the peer. */
			dstx_req_ds_ia();
			mTimerUartIdle.SetTimer();
			break;

		case dsevtid_yms_rxed:
			/* Don't call h4ds_reset_uart_idle_timer() for yms. */
			mDsState = state_awake_pass_tx;
			break;

		case dsevtid_uart_idle_timeout:
			if (!mDsUartActive /*&& HERE H4DS_UART_BUFFERS_EMPTY()*/)
			{
				DB_PRINTF(("YMS Q'd\n"));
				/* Send a You-May-Sleep message to the peer. */
				dstx_req_ds_yms();
				mDsState = state_awake_pass_rx;
			}
			else
			{
				mDsUartActive = false;
				mTimerUartIdle.SetTimer();
			}
			break;
		default:
			break;
		}
		break;

	case state_awake_pass_rx:           /* Only receive traffic. */
		switch(msg)
		{
		case dsevtid_hl_tx_msg_avail:
            ds_state_drowsy_prelude();
			break;

		case dsevtid_wu_rxed:
            ds_state_drowsy_prelude();
			dstx_req_ds_ia();				// Send an I'm-Awake message to the peer.
			break;

		case dsevtid_yms_rxed:
            ds_state_flush_tx_prelude();
			break;
		default:
			break;
		}
		break;

	case state_awake_pass_tx:           /* Only transmit traffic. */
		switch(msg)
		{
		case dsevtid_wu_rxed:
			dstx_req_ds_ia();				// Send an I'm-Awake message to the peer.
			mTimerUartIdle.SetTimer();
			mDsState = state_awake;
			break;

		case dsevtid_uart_idle_timeout:
			if (!mDsUartActive /*&& HERE H4DS_UART_BUFFERS_EMPTY()*/)
			{
				DB_PRINTF(("YMS Q'd\n"));
                ds_state_flush_tx_prelude();
				dstx_req_ds_yms();		// Send a You-May-Sleep message to the peer.
			}
			else
			{
				mDsUartActive = false;
				mTimerUartIdle.SetTimer();
			}
			break;
		default:
			break;
		}
		break;

	default:
		H4DS_PANIC(H4DS_PANIC_INT_CORRUPTION);
		break;
	}
}

/* State preludes - snipped out to save code. */

void H4DSStack::ds_state_flush_tx_prelude()
{
    mDsTxBufferEmptyCheckPending = false;
    ds_arm_tx_empty_alarm();
	mTimerFlush.SetTimer();
    mDsState = state_flush_tx;
}

void H4DSStack::ds_state_torpid_prelude()
{
    H4DS_EVENT(H4DS_EVT_LOCAL_DEVICE_MAY_SLEEP);
    mDsState = state_torpid;
}

void H4DSStack::ds_state_awake_prelude()
{
    /* In the awake state we'll be interested in the UART
    becoming idle. */

    mDsUartActive = false;
	mTimerUartIdle.SetTimer();
    mDsState = state_awake;
}

void H4DSStack::ds_state_drowsy_prelude()
{
	dstx_req_ds_wu();		// Send a Wake-Up message to the peer.

	/* Start timer to send train of Wake-Up msgs. */
	mTimerWakeup.SetTimer();

	/* Init counter of number of txed Wake-Up msgs. */
	mDsNWakeup = 1;

	mDsState = state_drowsy;
}


/****************************************************************************
NAME
        ds_can_transmit  -  does the h4ds deep sleep engine allows tx
*/

bool H4DSStack::ds_can_transmit()
{
	return (mDsState == state_awake || mDsState == state_awake_pass_tx);
}


/****************************************************************************
NAME
        ds_can_receive  -  does the h4ds deep sleep engine allows rx
*/

bool H4DSStack::ds_can_receive()
{
	return (mDsState == state_awake || mDsState == state_awake_pass_rx || mDsState == state_drowsy);
}

/****************************************************************************
NAME
        ds_arm_tx_empty_alarm  -  setup for an alarm when the TX path is idle

FUNCTION
		This function requests that the event 'dsevtid_tx_buffer_empty' is
		pushed into the DS state machine when the TX path becomes idle.

		It is not clear how best to perform this action.  The UART on a PC
		lies.  I setup a fast timer (about 20ms).  If this timer fires a
		certain number of times when we think that the TX is empty and the
		peer is not asserting flow control then we assume that the transmit
		FIFO is empty.
*/

void H4DSStack::ds_arm_tx_empty_alarm()
{
	mDsUartTxEmptyAlarm = true;
	mDsUartTxEmptyAlarmCount = 0;
	mTimerTxEmptyAlarm.SetTimer();
}


/****************************************************************************
NAME
        ds_cancel_tx_empty_alarm  -  cancel the above alert

FUNCTION
		This is called to canel the above alert.
*/

void H4DSStack::ds_cancel_tx_empty_alarm()
{
	mDsUartTxEmptyAlarm = false;
	mDsUartTxEmptyAlarmCount = 0;
	mTimerTxEmptyAlarm.SetPeriod(TTXEMPTY_PERIOD);
}


/****************************************************************************
NAME
        ds_timer_tx_empty_alarm  -  TX FIFO empty timer function

FUNCTION
		This is the action function for the tx empty alert timer.  It is 
		called repeatedly;  if the PC tells us that the FIFO is empty and
		the peer is not asserting flow control then we assume that the FIFO
		is empty, and push the correct event into the state machine.
*/

void H4DSStack::ds_timer_tx_empty_alarm()
{
	if (!mDsUartTxEmptyAlarm)
		return;

	// If we are TX'ing or flow control is set
	if (mTxInProgress || !mFile->readCTS())
	{
		// Reset counter
		mDsUartTxEmptyAlarmCount = 0;
		mTimerTxEmptyAlarm.SetTimer();
		return;
	}

	// Check the counter
	if (++mDsUartTxEmptyAlarmCount >= TXEMPTY_COUNT)
	{
		// We are now fairly confident that the TX fifo is empty?
		mDsUartTxEmptyAlarm = false;

		ds_alert_tx_buffer_empty();
	}
	else
		mTimerTxEmptyAlarm.SetTimer();
}
