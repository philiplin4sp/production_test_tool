// Copyright (C) 2000-2006 Cambridge Silicon Radio Ltd.; all rights reserved.
// Building and sending HCI commands.
// $Id: //depot/bc/bluesuite_2_4/devHost/btcli/put.c#1 $


#include <stddef.h>	/* for NULL */
#include <stdlib.h>

#if !defined(_WINCE) && !defined(_WIN32_WCE)
#include <assert.h>
#define ASSERT assert
#else
#include "wince/wincefuncs.h"
#endif

#include "put.h"
#include "commandtable.h"
#include "btcli.h"


static u32 *paout = NULL;
static size_t pacurrent = 0;


void putCmd (size_t n)
{
	ASSERT (paout == NULL);
	paout = malloc (sizeof (u32) * (n + 2));
	ASSERT (paout != NULL);

	paout[0] = n + 2;
	paout[1] = getCurrentCommandOpcode ();
	pacurrent = 2;

//	printlf ("-> 0x%lx 0x%04x", (ul) (n + 2), hciCommandTable[opindex].opcode);
}


void put (u32 n)
{
	ASSERT (paout != NULL);
	ASSERT (pacurrent < paout[0]);

	paout[pacurrent++] = n;

//	printlf (" 0x%lx", (ul) n);
}


void putArray (u8 a[], size_t n)
{
	size_t i;
	for (i = 0; i < n; ++i)
	{
		put (a[i]);
	}
}


void putDone (void)
{
#if 0
	size_t i;

	ASSERT (paout != NULL);
	ASSERT (pacurrent == paout[0]);

	paout[0] = pacurrent;

	printlf ("->");
	for (i = 0; i < paout[0]; ++i)
	{
		printlf (" 0x%lx", (ul) paout[i]);
	}
	printlf ("\n");
#else
	ASSERT (paout != NULL);
	ASSERT (pacurrent == paout[0]);

	sendCmd (paout);

	free (paout);
	paout = NULL;
#endif
}
