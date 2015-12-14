// Copyright (C) 2000-2006 Cambridge Silicon Radio Ltd.; all rights reserved.
// Decoding of received debug channel data.
// $Id: //depot/bc/bluesuite_2_4/devHost/btcli/decodedebug.c#1 $


#include <stdio.h>

#if !defined(_WINCE) && !defined(_WIN32_WCE)
#include <assert.h>
#define ASSERT assert
#else
#include "wince/wincefuncs.h"
#endif

#include "print.h"
#include "constants.h"


void decodeDebug (const u8 *buf, u32 len)
{
	printTimestamp ();
	printlf ("debug");
	if (len != 0 && buf[len - 1] == NUL)
	{
		printlf (" \"");
		while (--len) /* Skip trailing NUL by predecrementing */
		{
			printChar (*buf++);
		}
		printlf ("\"");
	}
	else
	{
		for (; len != 0; --len)
		{
			printlf (" 0x%02x", *buf++);
		}
	}
	printlf ("\n");
}
