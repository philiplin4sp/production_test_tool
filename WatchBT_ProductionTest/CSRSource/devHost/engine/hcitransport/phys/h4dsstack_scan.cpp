///////////////////////////////////////////////////////////////////////
//
//  FILE   :  H4DSStack_scan,cpp
//
//  AUTHOR :  Mark Marshall
//				(based on code by Mark RISON and Carl Orsborn)
//
//  CLASS  :  H4DSStack (Scanner sections)
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
        scan_match_msg_start  -  identify an h4ds message from its first byte

FUNCTION
        Each h4ds message body starts with a different byte.  This function
        obtains information for the h4ds message body which starts with "b".

RETURNS
        A MSGINFO for which the first byte of the corresponding message is
        "b", or NULL if there is no such message.
*/

/*static*/ const H4DSStack::MSGINFO *H4DSStack::scan_match_msg_start(uint8 b)
{
    const MSGINFO *mi;

    for (mi = h4ds_msgs; mi->msg; mi++)
        if (b == mi->msg[0])
            return mi;

    return 0;
}

/****************************************************************************
NAME
        scan_init_match_msg  -  initialise h4ds message scanner

FUNCTION
        Prepare scan_match_msg() to match a fresh message.
*/

void H4DSStack::scan_init_match_msg()
{
	mScannerState = scst_lock;
	mScannerIndex = 0;
}

/****************************************************************************
NAME
        scan_match_msg  -  h4ds message scanner

FUNCTION
        This function matches the contents of the "len" byte buffer at "buf"
        with the set of h4ds messages, and reports when it matches a complete
        message.

        The initialisation function scan_init_match_msg() must be called
        before making a first call to scan_match_msg().  After this, it
        is expected that a sequence of calls will be made to
        scan_match_msg(), each offering a block of bytes received, in
        sequence, from the h4ds peer, until scan_match_msg() has matched
        a complete h4ds message.  The function then reports the message
        matched.

RETURNS
        When a sequence of calls to this function has provided all of the
        bytes of an h4ds message, the function returns an identifier for the
        message matched.

        If this function has been given insufficient bytes to match a
        complete message, it will return msgid_none.

        If this function cannot match "buf" to (part of) an h4ds message it
        returns msgid_duff.

        ntaken is set to the number of bytes consumed by this call to
        scan_match_msg().  If the function returns msgid_duff, it does
        not consume the byte that could not be matched to an h4ds message.
        (For example, if the correct message is "abcd" and the caller offers
        "abde" then only "ab" will be consumed.)

        The function does not know about the sequence of 0x00 bytes expected
        at the start of a Wake-Up message, so such bytes will be rejected
        with msgid_duff.
*/

H4DSStack::H4DS_MSGID H4DSStack::scan_match_msg(const uint8 *buf, const uint32 len, uint32 &ntaken)
{
	uint32 i;

	for(i = 0; i < len; i++, buf++)
	{
		switch (mScannerState)
		{
		case scst_lock:
			/* Match the "lock" - the common byte sequence that
			starts all h4ds layer messages. */
			if (*buf == h4ds_lock_msgfrag[mScannerIndex++])
			{
				if (mScannerIndex >= H4DS_MSGFRAG_LOCK_LEN)
					mScannerState = scst_body_start;
			}
			else
			{
				/* The ill-matching byte may be the start
				of a good message, so don't consume it. */
				scan_init_match_msg();
				ntaken = i;
				return msgid_duff;
			}
			break;

		case scst_body_start:
			if ((mScannerMsg = scan_match_msg_start(*buf)) != 0)
			{
				mScannerIndex = 1;
				mScannerState = scst_in_body;
			}
			else
			{
				/* The ill-matching byte may be the start
				of a good message, so don't consume it. */
				scan_init_match_msg();
				ntaken = i;
				return msgid_duff;
			}
			break;

		case scst_in_body:
			if (*buf == mScannerMsg->msg[mScannerIndex++])
			{
				if (mScannerIndex >= H4DS_MSGFRAG_BODY_LEN)
				{
					/* Must consume final msg byte. */
					ntaken = i + 1;
					scan_init_match_msg();
					return mScannerMsg->id;
				}
			}
			else
			{
				/* The ill-matching byte may be the start
				of a good message, so don't consume it. */
				scan_init_match_msg();
				ntaken = i;
				return msgid_duff;
			}
			break;

		default:
			H4DS_PANIC(H4DS_PANIC_INT_CORRUPTION);
			break;
		}
	}

	/* We've run out of input bytes. */
	ntaken = len;
	return msgid_none;
}
