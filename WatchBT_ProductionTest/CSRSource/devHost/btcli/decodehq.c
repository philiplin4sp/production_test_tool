// Copyright (C) 2000-2006 Cambridge Silicon Radio Ltd.; all rights reserved.
// Decoding of received hq data.
// $Id: //depot/bc/bluesuite_2_4/devHost/btcli/decodehq.c#1 $


#include <stdio.h>

#if !defined(_WINCE) && !defined(_WIN32_WCE)
#include <stddef.h>
#include <assert.h>
#define ASSERT assert
#else
#include "wince/wincefuncs.h"
#endif

#include "print.h"
#include "dictionaries.h"
#include "host/hq/hqpdu.h"

#include "globals.h"
#include "fmdecode.h"


// This function with decode and print an HQ event.
// Most events are just printed out with the bytes they contain:
// 
//     hq sn:0x00bf fm_event ok
//     0x8000 FM HQ Event: flags  : FLRST
//     0x0 0x0 0x0 0x0 0x0 0x0
//
// RSSI scan events are printed out in a 'pretty format' as:
//
//     0x400 FM HQ Event(RSSI): 95.4MHz -101dBm ----+
//

void decodeHQ (const PA pa)
{

	size_t i = 4;
	bool ppr;

	ASSERT (pa[0] >= 4);

	/* Test if this an FM RSSI event and we should pretty print it */
	ppr = (fmPrettyRSSI && pa[1] == HQVARID_FM_EVENT && pa[4] == 0x400);

	if (!ppr)
	{
		/* This is suppressed for pretty RSSI scan */
		printTimestamp ();
		printlf ("hq ");
		printByValue (NULL, pa[2], 16, "sn");
		printlf (" ");
		printByValue (hqVarID_d, pa[1], 16, "vi");
		printlf (" ");
		printByValue (hqStatus_d, pa[3], 16, "s");
	}

	if (pa[1] == HQVARID_FAULT && pa[0] >= 5)
	{
		printlf (" ");
		printByValue (faultID_d, pa[4], 16, "fi");
		++i;
	}
	else if (pa[1] == HQVARID_DRAIN_CALIBRATE && pa[0] >= 5)
	{
		printlf (" ");
		printByValue (NULL, pa[4], 32, "awake");
		printlf (" ");
		printByValue (NULL, pa[5], 32, "asleep");
		printlf (" ");
		printByValue (NULL, pa[6], 32, "half_slots");
		i += 3;
		if (pa[0] >= 6)
		{
		    printlf (" ");
		    printByValue (NULL, pa[7], 32, "half_slots_pa");
		    i++;
		    if (pa[0] >= 7)
		    {		    
				printlf (" ");
				printByValue (NULL, pa[8], 32, "fm_radio");
				i++;
		    }
		    if (pa[0] >= 8)
		    {		    
				printlf (" ");
				printByValue (NULL, pa[9], 32, "fm_radio_tx");
				i++;
		    }		    
		    if (pa[0] >= 9)
		    {		    
				printlf (" ");
				printByValue (NULL, pa[10], 32, "fm_lo");
				i++;
		    }		    
		}
	}
	else if (pa[1] == HQVARID_FM_EVENT && pa[0] >= 4)
	{
		/* FM Radio HQ Event */
		/* pa[4] contains a copy of the flag register */
		/* Test if this is an RSSI event or not. */
		if(!ppr)
		{
			/* Normal event decode */
			printlf("\n0x%lx FM HQ Event: flags ", (ul) pa[4]);
			if (pa[4] != 0)
			{

				int index = 0;
				while (fm_flag_vals[index].value != 0)
				{
					if (fm_flag_vals[index].value & pa[4])
						printlf(" : %s", fm_flag_vals[index].name);
					index++;
				}
			}
			else
			{
				printlf("all cleared");
			}
			
			i=5;

			/* There may be additional registers/value pairs in the message. A register value 
			of 0, or end of file, indicates no further registers */
			while(i < (pa[0] -1) && pa[i] != 0)
			{
				decodeFMReg(pa[i], pa[i+1]);
				i += 2;
			}
		}
		else
		{
			/* Pretty RSSI */
			double freq = ((pa[6] + 60000.0)/1000.0);
			signed char rssi = (signed char)pa[8];

			printlf("0x%lx FM HQ Event(RSSI):  ", (ul) pa[4]);

			if(freq < 100)
				printlf(" ");

			printlf("%.1fMHz ", freq);

			if(rssi > -100)
				printlf(" ");

			printlf(" %ddBm ", rssi);

			if ((signed char)pa[8] > -20)
				printlf("-");
			if ((signed char)pa[8] > -25)
				printlf("-");
			if ((signed char)pa[8] > -30)
				printlf("-");
			if ((signed char)pa[8] > -35)
				printlf("-");
			if ((signed char)pa[8] > -40)
				printlf("-");
			if ((signed char)pa[8] > -45)
				printlf("-");
			if ((signed char)pa[8] > -50)
				printlf("-");
			if ((signed char)pa[8] > -55)
				printlf("-");
			if ((signed char)pa[8] > -60)
				printlf("-");
			if ((signed char)pa[8] > -65)
				printlf("-");
			if ((signed char)pa[8] > -70)
				printlf("-");
			if ((signed char)pa[8] > -75)
				printlf("-");
			if ((signed char)pa[8] > -80)
				printlf("-");
			if ((signed char)pa[8] > -85)
				printlf("-");
			if ((signed char)pa[8] > -90)
				printlf("-");
			if ((signed char)pa[8] > -95)
				printlf("-");
			if ((signed char)pa[8] > -100)
				printlf("-");
			if ((signed char)pa[8] > -105)
				printlf("-");
			if ((signed char)pa[8] > -110)
				printlf("-");
			if ((signed char)pa[8] > -115)
				printlf("-");
			if ((signed char)pa[8] > -120)
				printlf("-");
			printlf("+\n");
		}
	}
	else if (pa[1] == HQVARID_FMTX_EVENT && pa[0] >= 4)
	{
		/* FMTX Radio HQ Event */
		/* pa[4] contains a copy of the flag register */

		printlf("\n0x%lx FMTX HQ Event: flags ", (ul) pa[4]);
		if (pa[4] != 0)
		{

			int index = 0;
			while (fmtx_flag_vals[index].value != 0)
			{
				if (fmtx_flag_vals[index].value & pa[4])
					printlf(" : %s", fmtx_flag_vals[index].name);
				index++;
			}
		}
		else
		{
			printlf("all cleared");
		}
		i=5;

		/* There may be additional registers/value pairs in the message. A register value 
		of 0, or end of file, indicates no further registers */
		while(i < (pa[0] -1) && pa[i] != 0)
		{
			decodeFMTXReg(pa[i], pa[i+1]);
			i += 2;
		}

	}
	else if (pa[1] == HQVARID_FM_RDS_DATA && pa[0] >= 5)
	{
		/* FM Radio RDS Data event */
		uint16 blockCount;

		printlf("\n");

		for (blockCount = 0, i = 6; blockCount < pa[4]; blockCount++, i += 2)
		{
			if (blockCount != 0 && blockCount%4 == 0)
				printlf("\n");
			decodeRDSBlock(pa[i-1], pa[i]);
		}

		i = pa[0];
	}
        else if (pa [1] == HQVARID_DSPMANAGER_DEBUG_INFO_B)
        {
                printlf (" time:0x%lx ", (ul) pa [4]);
                printByValue (baton_messages_d, pa[5], 16, "id");
                printlf (" len:0x%lx", (ul) pa [6]);
                i = 7; /* Everything after length printed by the remainder loop at the end */
        }

    /* Just print out the remainder of the payload.. should be 'nothing' */
	if (!ppr)
	{
		/* This is suppressed for pretty RSSI scan */
		for (; i < pa[0]; ++i)
		{
			printlf (" 0x%lx", (ul) pa[i]);

		}
		printlf ("\n");
	}

}
