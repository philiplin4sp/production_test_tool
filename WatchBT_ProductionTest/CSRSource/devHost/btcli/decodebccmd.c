/*
 * Copyright (C) 2000-2006 Cambridge Silicon Radio Ltd.; all rights reserved.
 * Decoding of received bccmd data (including presentation key support).
 *
 * MODIFICATION HISTORY
 *	#38     01:mar:04 pws    B-2096: Support BCCMDVARID_REMOTE_SNIFF_LIMITS
 *	#40     05:jul:04 pw04   B-3200: add get RSSI bccmd.
 *	#42     21:jul:04 sk03   B-3349: Added eSCO test code.
 *	#43     23:jul:04 sk03   B-3349: Improvements to eSCO test code.
 *	#44     12:oct:04 ay02   B-4216: Added LC_RX_FRACS command for PER info
 *      #48     25:nov:04 sk03   B-4672: Replace pause_user_data flag with
 *                               semaphore.
 *
 * $Id: //depot/bc/bluesuite_2_4/devHost/btcli/decodebccmd.c#3 $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#if !defined(_WINCE) && !defined(_WIN32_WCE)
#include <stddef.h>
#include <assert.h>
#define ASSERT assert
#else
#include "wince/wincefuncs.h"
#endif

#include "btcli.h"
#include "print.h"
#include "dictionaries.h"
#include "assertn.h"
#include "host/bccmd/bccmdpdu.h"
#include "host/fastpipe/fastpipe_types.h"
#include "globals.h"
#include "psop_state.h"

#include "fmdecode.h"

#include "fastpipe_engine_iface.h"

static char* rx_fracs_defs[] = {
	"SUCCESSFUL RECEPTION  ", 	"SYNC TIMEOUT          ",
	"BUFFER ERROR (RECEIVE)", 	"MEMBER ERROR (INCORRECT SLAVE)",
	"HEADER ERROR (BAD CRC)", 	"PAYLOAD LENGTH ERROR  ",
	"PAYLOAD ERROR (BAD CRC)",	"DEMODULATION ERROR    ",
	"MULTI SLOT PACKET     ", 	"UNSUPPORTED PACKET    ",
	"SYNC TIMEOUT FOR EDR  ", 	"EDR DEMODULATION ERROR",
};
static size_t rx_fracs_def_count = sizeof(rx_fracs_defs) / sizeof(rx_fracs_defs[0]);

void print_bccmdgetrespfields(uint16 sn, uint16 vi, uint16 s)
{
    printTimestamp ();
    printlf ("bccmd_getresp ");
    printByValue (NULL, sn, 16, "sn");
    printlf (" ");
    printByValue (bcCmdVarID_d, vi, 16, "vi");
    printlf (" ");
    printByValue (bcCmdStatus_d, s, 16, "s");
}

static size_t print_stibbons_error(const PA pa)
{
    printByValue (stibbons_error_codes_d, pa [4], 16, "error");
    return 5;
}

static size_t print_stibbons_counted_error(const PA pa)
{
    printByValue (NULL, pa [4], 16, "succeeded");
    printlf (" ");
    printByValue (stibbons_error_codes_d, pa [5], 16, "error");
    return 6;
}

static void print_bccmdgetresp(const PA pa)
{
    print_bccmdgetrespfields(pa[2], pa[1], pa[3]);
}

void decodeBCCMD (const PA pa)
{
	size_t i = 4, istart, strend;
#define IMAX ((size_t) -1)
	int minwid = 0, printable;

	ASSERT (pa[0] >= 4);

#define OFFSETOF_PS_DATA 7

	if (pa[1] == BCCMDVARID_PS && pa[0] >= OFFSETOF_PS_DATA)
	{
		PSOPDATA psopdata;
		psop_get(pa[2], &psopdata);

		switch (psopdata.state)
		{
		default:
			if(psopdata.state != NONE)
			{
				printlf("unexpected event in PS state machine: received psget in state %d\n", psopdata.state);
				psopdata.state = NONE;
			}

			if (!slurping)
			{
				print_bccmdgetresp(pa);
			}
			break;

		case PRESENTATIONREADGET:
			if(!slurping)
			{
				print_bccmdgetresp(pa);
			}

			if (pa[4] == psopdata.pskeyreal)
			{
				if (pa[3] == BCCMD_OK &&
				    pa[5] * 16 >= (unsigned) (psopdata.offset + psopdata.len) &&
				    pa[5] + OFFSETOF_PS_DATA <= pa[0])
				{
					printlf (" ");
					printByValue (psKey_d, psopdata.pskeypresentation, 16, "id");
					printlf (" ");
					printByValue (stores_d, pa[6], 16, "s");
					printlf (" ");
					printByValue (NULL, 1, 16, "len");
					printlf (" 0x%0*x", (psopdata.len + 3) / 4, ((unsigned) pa[OFFSETOF_PS_DATA + psopdata.offset / 16] >> (psopdata.offset % 16)) & ((1 << psopdata.len) - 1));
					i = IMAX;
				}
				psopdata.state = NONE;
			}
			break;

		case PRESENTATIONWRITEGET:
			if (pa[4] == psopdata.pskeyreal)
			{
				if (pa[3] == BCCMD_OK &&
				    ((psopdata.pskeyreal == psopdata.pskeypresentation && pa[5] >= 2) || pa[5] * 16 >= (unsigned) (psopdata.offset + psopdata.len)) &&
				    pa[5] + OFFSETOF_PS_DATA <= pa[0])
				{
					u32 *newpa = malloc (pa[0] * sizeof (pa[0]));
					ASSERT (newpa != NULL);
					memcpy (newpa, pa, pa[0] * sizeof (pa[0]));
					newpa[1] = BCCMDPDU_SETREQ;
					newpa[2] = psopdata.seqno = ++bccmdseqno;
					newpa[3] = BCCMDVARID_PS;
					if (psopdata.pskeyreal == psopdata.pskeypresentation)
					{
						/* See description in parsecmd.c */
						if (psopdata.len != 0)
						{
							newpa[OFFSETOF_PS_DATA] = psopdata.len - 1;
						}
						if (psopdata.offset != 0)
						{
							newpa[OFFSETOF_PS_DATA + 1] &= 0xff00;
							newpa[OFFSETOF_PS_DATA + 1] |= psopdata.offset;
						}
					}
					else
					{
						newpa[OFFSETOF_PS_DATA + psopdata.offset / 16] &= ~(((1 << psopdata.len) - 1) << (psopdata.offset % 16));
						newpa[OFFSETOF_PS_DATA + psopdata.offset / 16] |= psopdata.value << (psopdata.offset % 16);
					}

					psopdata.state = PRESENTATIONWRITESET;
					psop_update(pa[2], &psopdata);

					sendBCCMD (newpa);
					free (newpa);
					return;
				}
			}
			else if (!slurping)
			{
				print_bccmdgetresp(pa);
			}
			psopdata.state = NONE;

			break;

		case PRESENTATIONWRITESET:
			if (!slurping)
			{
				print_bccmdgetresp(pa);
			}

			if (pa[4] == psopdata.pskeyreal)
			{
				if (pa[3] == BCCMD_OK &&
				    (psopdata.pskeyreal != psopdata.pskeypresentation && pa[5] * 16 >= (unsigned) (psopdata.offset + psopdata.len)) &&
				    pa[5] + OFFSETOF_PS_DATA <= pa[0])
				{
					printlf (" ");
					printByValue (psKey_d, psopdata.pskeypresentation, 16, "id");
					printlf (" ");
					printByValue (storesSet_d, pa[6], 16, "s");
					printlf (" ");
					printByValue (NULL, 1, 16, "len");
					printlf (" 0x%0*x", (psopdata.len + 3) / 4, (unsigned) psopdata.value);
					i = IMAX;
				}
			}
			psopdata.state = NONE;
			break;
		}
		psop_update(pa[2], &psopdata);

		if (slurping)
		{
			printTimestamp ();
			if (pa[6] != PS_STORES_T &&
			    pa[6] != PS_STORES_I &&
			    pa[6] != PS_STORES_F)
			{
				printlf ("psset ");
				printByValue (psKey_d, pa[4], 16, NULL);
			}
			else
			{
				printlf ("pssets ");
				printByValue (psKey_d, pa[4], 16, NULL);
				printlf (" ");
				printByValue (stores_d, pa[6], 16, NULL);
			}
			if (pa[3] != BCCMD_OK)
			{
				printlf (" ");
				printByValue (bcCmdStatus_d, pa[3], 16, "s");
				i = IMAX;
			}
		}

		if (i != IMAX)
		{
			if (!slurping)
			{
				printlf (" ");
				printByValue (psKey_d, pa[4], 16, "id");
				printlf (" ");
				printByValue (stores_d, pa[6], 16, "s");
			}
			if (pa[3] == BCCMD_OK)
			{
				if (!slurping)
				{
					printlf (" ");
					printByValue (NULL, pa[5], 16, "len");
				}
				i = OFFSETOF_PS_DATA;
				minwid = 4;
			}
			else
			{
				i = IMAX;
			}
		}

		if (slurping)
		{
			PA pa2 = { 6, BCCMDPDU_GETREQ, 0, 0, 0, 0 };
			pa2[2] = ++bccmdseqno;
			pa2[3] = BCCMDVARID_PSNEXT;
			pa2[4] = pa[4];
			pa2[5] = pa[6];
			sendBCCMD (pa2);
		}
	}
	else if (pa[1] == BCCMDVARID_PSNEXT)
	{
		ASSERT (pa[0] == 7);
		if (pa[3] != BCCMD_OK)
		{
			slurping = false;
		}

		if (slurping)
		{
			if (pa[6] == 0)
			{
				printTimestamp ();
				if (pa[4] == 0)
				{
					printlf ("psslurp: nothing there!\n");
				}
				else
				{
					printlf ("psslurp: done\n");
				}
				slurping = false;
			}
			else
			{
#define OFFSETOF_PS_DATA 7
				u32 vi = BCCMDVARID_PSSIZE;
				PSOPDATA psopdata;
				PA pa2 = { OFFSETOF_PS_DATA, BCCMDPDU_GETREQ, 0, 0, 0, 0, 0 };

				psopdata.state = NONPRESENTATIONSIZE;
				pa2[2] = psopdata.seqno = ++bccmdseqno;
				pa2[3] = vi;
				pa2[4] = psopdata.pskeyreal = pa[6];
				pa2[6] = pa[5];

				psop_add(&psopdata);

				sendBCCMD (pa2);
			}
			return;
		}
		print_bccmdgetresp(pa);
		printlf (" ");
		printByValue (psKey_d, pa[4], 16, "id");
		printlf (" ");
		printByValue (stores_d, pa[5], 16, "s");
		if (pa[3] == BCCMD_OK)
		{
			printlf (" ");
			printByValue (psKey_d, pa[6], 16, "id");
		}
		i = IMAX;
	}
	else if (pa[1] == BCCMDVARID_PSSIZE)
	{
		PSOPDATA psopdata;
		psop_get(pa[2], &psopdata);

		ASSERT (pa[0] == 7);

		switch(psopdata.state)
		{
		default:
			if(psopdata.state != NONE)
			{
				printlf("unexpected event in PS state machine: received pssize in state %d\n", psopdata.state);
				psopdata.state = NONE;
			}
			print_bccmdgetresp(pa);
			printlf (" ");
			printByValue (psKey_d, pa[4], 16, "id");
			printlf (" ");
			printByValue (stores_d, pa[6], 16, "s");
			if (pa[3] == BCCMD_OK)
			{
				printlf (" ");
				printByValue (NULL, pa[5], 16, "size");
			}
			psop_update(pa[2], &psopdata);
			break;
		case NONPRESENTATIONSIZE:
		case PRESENTATIONREADSIZE:
		case PRESENTATIONWRITESIZE:
			{
				PA pa2 = { 7, BCCMDPDU_GETREQ, 0, 0, 0, 0, 0 };

				pa2[2] = psopdata.seqno = ++bccmdseqno;
				pa2[3] = BCCMDVARID_PS;
				pa2[4] = psopdata.pskeyreal;
				pa2[5] = pa[5];
				pa2[6] = pa[6];

				switch(psopdata.state)
				{
				case NONPRESENTATIONSIZE:
					psopdata.state = NONE;
					break;
				case PRESENTATIONREADSIZE:
					psopdata.state = PRESENTATIONREADGET;
					break;
				case PRESENTATIONWRITESIZE:
					psopdata.state = PRESENTATIONWRITEGET;
					break;
				}
				psop_update(pa[2], &psopdata);

				sendBCCMD (pa2);

				return;
			}
		}
		i = IMAX;
	}
	else if (pa[1] == BCCMDVARID_GET_NEXT_BUILDDEF)
	{
		ASSERT (pa[0] == 6);
		if (pa[3] != BCCMD_OK)
		{
			print_bccmdgetresp(pa);
			bdslurping = false;
		}
		else if (!bdslurping)
		{
			print_bccmdgetresp(pa);
		}
		if (bdslurping)
		{
			printTimestamp ();
			if (pa[5] == 0)
			{
				if (pa[4] == 0)
				{
					printlf ("bdslurp: nothing there!\n");
				}
				else
				{
					printlf ("bdslurp: done\n");
				}
				bdslurping = false;
				return;
			}
			else
			{
				PA pa2 = { 5, BCCMDPDU_GETREQ, 0, 0, 0 };
				pa2[2] = ++bccmdseqno;
				pa2[3] = BCCMDVARID_GET_NEXT_BUILDDEF;
				pa2[4] = pa[5];
				sendBCCMD (pa2);
			}
			printlf ("builddef ");
			printByValue (builddef_d, pa[5], 16, NULL);
			i = IMAX;
		}
	}
	else
	{
		print_bccmdgetresp(pa);

		if (pa[1] == BCCMDVARID_PSCLR)
		{
			ASSERT (pa[0] == 5);
			printlf (" ");
			printByValue (psKey_d, pa[4], 16, "id");
			i = 5;
		}
		else if (pa[1] == BCCMDVARID_PSCLRS)
		{
			ASSERT (pa[0] == 6);
			printlf (" ");
			printByValue (psKey_d, pa[4], 16, "id");
			printlf (" ");
			printByValue (storesClr_d, pa[5], 16, "s");
			i = 6;
		}
		else if (pa[1] == BCCMDVARID_PSCLRALLS)
		{
			ASSERT (pa[0] == 5);
			printlf (" ");
			printByValue (storesClr_d, pa[4], 16, "s");
			i = 5;
		}
		else if (pa[1] == BCCMDVARID_CRYPTKEYLEN || pa[1] == BCCMDVARID_PICONET_INSTANT
			 )
		{
			ASSERT (pa[0] == 6);
			printlf (" ");
			printch (pa[4]);
			i = 5;
		}
		else if (pa[1] == BCCMDVARID_ENABLE_AFH
				)
		{
			ASSERT (pa[0] == 6);
			printlf (" ");
			printch (pa[4]);
			printlf (" ");
			printByValue (disabledEnabled_d, pa[5], 8, NULL);
			i = IMAX;
		}

		else if (pa[1] == BCCMDVARID_MEMORY)
		{
			ASSERT (pa[0] >= 6);
			printlf (" ");
			printByValue (NULL, pa[4], 16, "addr");
			printlf (" ");
			printByValue (NULL, pa[5], 16, "len");
			if (pa[3] == BCCMD_OK)
			{
				i = 6;
				minwid = 4;
			}
			else
			{
				i = IMAX;
			}
		}
		
		else if (pa[1] == BCCMDVARID_RANDOM)
		{
			ASSERT (pa[0] >= 6);
			printlf (" ");
			printByValue (NULL, pa[4], 16, "len");
			printlf (" ");
			printByValue (NULL, pa[5], 16, "type");
			if (pa[3] == BCCMD_OK)
			{
				i = 6;
				minwid = 4;
			}
			else
			{
				i = IMAX;
			}
		}
		else if (pa[1] == BCCMDVARID_HQ_SCRAPING)
		{
			ASSERT (pa[0] >= 9);
			printlf (" ");
			printByValue (NULL, pa[4], 16, "length");
			if (pa[3] == BCCMD_OK)
			{
				i = 5;
				minwid = 4;
			}
			else
			{
				i = IMAX;
			}

		}
		else if (pa[1] == BCCMDVARID_BUFFER)
		{
			ASSERT (pa[0] >= 7);
			printlf (" ");
			printByValue (NULL, pa[4], 16, "handle");
			printlf (" ");
			printByValue (NULL, pa[5], 16, "start");
			printlf (" ");
			printByValue (NULL, pa[6], 16, "len");
			if (pa[3] == BCCMD_OK)
			{
				i = 7;
				minwid = 2;
			}
			else
			{
				i = IMAX;
			}
		}
		else if (pa[1] == BCCMDVARID_I2C_TRANSFER)
		{
			ASSERT (pa[0] >= 9);
			printlf (" ");
			printByValue (NULL, pa[4], 8, "addr");
			printlf (" ");
			printByValue (NULL, pa[5], 16, "txlen");
			printlf (" ");
			printByValue (NULL, pa[6], 16, "rxlen");
			printlf (" ");
			printByValue (NULL, pa[7], 1, "restart");
			printlf (" ");
			printByValue (NULL, pa[8], 16, "done");
			if (pa[3] == BCCMD_OK)
			{
				i = 9;
				minwid = 2;
			}
			else
			{
				i = IMAX;
			}
		}
		else if (pa[1] == BCCMDVARID_SIFLASH)
		{
			ASSERT (pa[0] >= 7);
			printlf (" ");
			printByValue (NULL, pa[4], 16, "cmd");
			printlf (" ");
			printByValue (NULL, pa[5], 32, "word_addr");
			printlf (" ");
			printByValue (NULL, pa[6], 16, "length");
			if (pa[3] == BCCMD_OK)
			{
				i = 7;
				minwid = 2;
			}
			else
			{
				i = IMAX;
			}
		}
		else if (pa[1] == BCCMDVARID_E2_DEVICE)
		{
			ASSERT (pa[0] == 6);
			printlf (" ");
			printByValue (NULL, pa[4], 5, "log2_bytes");
			printlf (" ");
			printByValue (NULL, pa[5], 3, "addr_mask");
			i = 6;
		}
		else if (pa[1] == BCCMDVARID_E2_APP_DATA)
		{
			ASSERT (pa[0] >= 6);
			printlf (" ");
			printByValue (NULL, pa[4], 16, "offset");
			printlf (" ");
			printByValue (NULL, pa[5], 16, "len");
			if (pa[3] == BCCMD_OK)
			{
				i = 6;
				minwid = 4;
			}
			else
			{
				i = IMAX;
			}
		}
		else if (pa[1] == BCCMDVARID_PS_MEMORY_TYPE)
		{
			ASSERT (pa[0] == 6);
			printlf (" ");
			printByValue (storesSet_d, pa[4], 16, "s");
			printlf (" ");
			printByValue (psmt_d, pa[5], 16, "mt");
			i = 6;
		}
		else if (pa[1] == BCCMDVARID_L2CAP_CRC)
		{
			ASSERT (pa[0] == 8);
			printlf (" ");
			printch (pa[4]);
			printlf (" ");
			printByValue (l2capCrcRxTx_d, pa[5], 16, "tc");
			printlf (" ");
			printByValue (NULL, pa[6], 16, "cid");
			printlf (" ");
			printByValue (disabledEnabled_d, pa[7], 8, "e");
			i = IMAX;
		}
		else if (pa[1] == BCCMDVARID_REMOTE_SNIFF_LIMITS)
		{
			ASSERT (pa[0] == 10);
			printlf (" ");
			printByValue (NULL, pa[4], 16, "maxi");
			printlf (" ");
			printByValue (NULL, pa[5], 16, "mini");
			printlf (" ");
			printByValue (NULL, pa[6], 16, "maxa");
			printlf (" ");
			printByValue (NULL, pa[7], 16, "mina");
			printlf (" ");
			printByValue (NULL, pa[8], 16, "maxt");
			printlf (" ");
			printByValue (NULL, pa[9], 16, "mint");
			i = 10;
		}
		else if (pa[1] == BCCMDVARID_BUILD_NAME)
		{
			unsigned int next_offset;
			ASSERT (pa[0] >= 9);
			printlf (" ");
			printByValue (NULL, pa[4], 16, "start");
			printlf (" ");
			printByValue (NULL, pa[5], 16, "len");
			if (pa[3] == BCCMD_OK)
			{
				printlf (" ");
				printString (&pa[6], pa[0] - 6);
			}
			i = IMAX;

			/* Check to see if we have reached the end of the build name */
			next_offset = pa[4] + pa[0] - 6;
			if (next_offset < pa[5])
			{
				/* Request the next portion of the build name */
				PA pa = { 15, BCCMDPDU_GETREQ, 13, BCCMDVARID_BUILD_NAME, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
				pa[2] = ++bccmdseqno;
				pa[4] = next_offset;
				sendBCCMD (pa);
			}
		}
		else if (pa[1] == BCCMDVARID_RSSI_ACL)
		{
			ASSERT (pa[0] == 6);
			printlf (" ");
			printch (pa[4]);
			printlf (" r:%i", (signed char) pa[5]);
			i = IMAX;
		}
		else if (pa[1] == BCCMDVARID_ESCOTEST)
		{
			ASSERT (pa[0] >= 6);
			printlf (" ");
			switch (pa[4])
			{
			case BCCMDPDU_ESCOTEST_ACTIVE_MODES:
			{
				/* Display the selected test modes */
				DictionaryPair *dp;
				printByValue (et_fns_d, BCCMDPDU_ESCOTEST_ACTIVE_MODES, 16, NULL);
							for (dp = et_modes_d; dp->name != 0; dp++)
							{
					printlf (dp == et_modes_d ? ":" : "|");
									if ((pa[5] & dp->value) == 0)
						printlf ("~");
									printlf (dp->name);
							}
				break;
					}
			case BCCMDPDU_ESCOTEST_SCO_TX_ERR_RATE:
			case BCCMDPDU_ESCOTEST_SCO_RX_ERR_RATE:
			case BCCMDPDU_ESCOTEST_SCO_RTX_ERR_RATE:
			case BCCMDPDU_ESCOTEST_SCO_RRX_ERR_RATE:
			case BCCMDPDU_ESCOTEST_SCO_TX_FRAME_RATE:
			case BCCMDPDU_ESCOTEST_PAUSE_STATE:
				/* Display the (e)SCO (re)tx/rx error insertion rate */
				printByValue (NULL, pa[5], 16, lookupByValue (et_fns_d, pa[4]));
				break;
			default:
				/* Unrecognised reason code */
				printlf ("error");
				break;
			}
			i = IMAX;
		}
		else if (pa[1] == BCCMDVARID_LC_FRAC_COUNT)
		{
			/* check success value otherwise Firmware is not compatible */
			if (pa[3] != BCCMDPDU_STAT_OK)
				printlf("\nCommand not compatible with firmware\n");
			else
				printlf("\nPacket Error Results table contains %lu elements\n", (ul) pa[4]);
			i = IMAX;
		}
		else if (pa[1] == BCCMDVARID_LC_RX_FRACS)
		{
			/* check success value otherwise Firmware is not compatible */
			if (pa[3] != BCCMDPDU_STAT_OK)
			{
				printlf("\nCommand not compatible with firmware - no PER table available\n");
			}
			else
			{
				printlf("\nError table received for BCCMDVARID_LC_RX_FRACS command:\n");

				for (i=0; i<pa[4]; i++)
				{
					if (i < rx_fracs_def_count )
						printlf("[%s] %lx", rx_fracs_defs[i], (ul) pa[i + 5]);
					else
						printlf("[unknown] %lx", (ul) pa[i + 5]);

					if (pa[i+5] != 0)
						printlf(", %2.2f%%", ((float)pa[i+5]*100.0) / (float)10000.0);	/* really 10000 not 0x10000 */
					printlf("\n");
				}
			}	
			i = IMAX;
		}
		else if (pa[1] == BCCMDVARID_FM_REG)
		{
			/* FM Register Variable. Decode the data in selected variables. */
			if(decodeFMReg(pa[4], pa[5]))
				i = 6;
		}
		else if (pa[1] == BCCMDVARID_FMTX_REG)
		{
			/* FMTX Register Variable. Decode the data in selected variables. */
			if(decodeFMTXReg(pa[4], pa[5]))
				i = 6;
		}
		else if (pa[1] == BCCMDVARID_FM_RDS)
		{
			/* RDS Recieve data is in data/status pairs.
			The first data is at position 5 with status byte at 6*/
			printlf("\n");
			i = 5;
			for(i=5; i + 1 < pa[0]; i+=2)
			{
				if (i != 5 && (i-5)%8 == 0)
					printlf("\n");
				decodeRDSBlock(pa[i], pa[i+1]);
			}
		}
		else if (pa[1] == BCCMDVARID_FMTX_TEXT)
		{
			/* RDS Transmit data, this is a lengthy string*/
			printlf("\n");
			decodeRDSTEXT(pa);
		}
		else if (pa[1] == BCCMDVARID_FASTPIPE_ENABLE)
		{
			ASSERT (pa[0] == 6);
			printlf (" ");
			printByValue (NULL, pa[4], 32, "lh");
			printlf (" ");
			printByValue (fastpipe_result_d, pa[5], 16, "r");

			if (pa[3] == BCCMD_OK && pa[5] == FASTPIPE_SUCCESS)
			{
				fastpipeEngineEnable (pa[4]);
			}

			i = 6;
		}
		else if (pa[1] == BCCMDVARID_FASTPIPE_CREATE)
		{
			ASSERT (pa[0] == 16);
			printlf (" ");
			printByValue (NULL, pa[4], 16, "pi");
			printlf (" ");
			printByValue (NULL, pa[5], 32, "oh");
			printlf (" ");
			printByValue (NULL, pa[6], 32, "crh");
			printlf (" ");
			printByValue (NULL, pa[7], 32, "rtc");
			printlf (" ");
			printByValue (NULL, pa[8], 32, "dtc");
			printlf (" ");
			printByValue (NULL, pa[9], 32, "rrc");
			printlf (" ");
			printByValue (NULL, pa[10], 32, "drc");
			printlf (" ");
			printByValue (NULL, pa[11], 32, "oc");
			printlf (" ");
			printByValue (NULL, pa[12], 32, "ctc");
			printlf (" ");
			printByValue (NULL, pa[13], 32, "crc");
			printlf (" ");
			printByValue (NULL, pa[14], 16, "h");
			printlf (" ");
			printByValue (fastpipe_result_d, pa[15], 16, "r");

			if (pa[3] == BCCMD_OK && pa[15] == FASTPIPE_SUCCESS)
			{
				/* Only attempt to create a pipe on the host if the controller
				   reports no problems. */
				enum FastPipeEngineResult r = fastpipeEngineConnect (pa[4],
				                                                     pa[5],
				                                                     pa[6],
				                                                     pa[11],
				                                                     pa[12],
				                                                     pa[13],
				                                                     pa[14]);
				if (r != FASTPIPE_ENGINE_SUCCESS)
				{
					printlf ("\nfastpipeEngineConnect ");
					printByValue (fastpipe_engine_result_d, r, 16, "result");
				}
			}

			i = 16;
		}
		else if (pa[1] == BCCMDVARID_FASTPIPE_DESTROY)
		{
			ASSERT (pa[0] == 6);
			printlf (" ");
			printByValue (NULL, pa[4], 16, "pi");
			printlf (" ");
			printByValue (fastpipe_result_d, pa[5], 16, "r");

			if (pa[3] == BCCMD_OK && pa[5] == FASTPIPE_SUCCESS)
			{
				enum FastPipeEngineResult r = fastpipeEngineDisconnect (pa[4]);
				if (r != FASTPIPE_ENGINE_SUCCESS)
				{
					printlf ("\nfastpipeEngineDisconnect ");
					printByValue (fastpipe_engine_result_d, r, 16, "result");
				}
			}

			i = 6;
		}
		else if (pa[1] == BCCMDVARID_FASTPIPE_RESIZE)
		{
			ASSERT (pa[0] == 7);
			printlf (" dc:%li", pa[4]);
			printlf (" ");
			printByValue (NULL, pa[5], 32, "lc");
			printlf (" ");
			printByValue (fastpipe_result_d, pa[6], 16, "r");

			if (pa[3] == BCCMD_OK && pa[6] == FASTPIPE_SUCCESS)
			{
				enum FastPipeEngineResult r = fastpipeEngineResize (pa[5]);
				if (r != FASTPIPE_ENGINE_SUCCESS)
				{
					printlf ("\nfastpipeEngineResize ");
					printByValue (fastpipe_engine_result_d, r, 16, "result");
				}
			}

			i = 7;
		}
		else if (pa[1] == BCCMDVARID_ALLOCATE_RAM_RESERVE ||
				 pa[1] == BCCMDVARID_RECLAIM_RAM_RESERVE)
		{
			ASSERT (pa[0] == 6);
			printByValue (NULL, pa[4], 8, "t");
			printlf (" ");
			printByValue (NULL, pa[5], 16, "v");

			i = 6;
		}
		else if (pa[1] == BCCMDVARID_COEX_INT_TEST)
		{
			if (pa[4] == BCCMDPDU_COEX_INT_TEST_STATUS_SUCCESS)
				printlf("\nstatus: success value: 0x%02x", pa[5]);
			else if (pa[4] == BCCMDPDU_COEX_INT_TEST_STATUS_UNSUPPORTED_MODE_ATTEMPTED)
				printlf("\nstatus: unsupported mode attempted");
			else if (pa[4] == BCCMDPDU_COEX_INT_TEST_STATUS_UNABLE_PERFORM_OP)
				printlf("\nstatus: unable to perform operation requested");
			else
				printlf("\nstatus: unable to decode bccmd output");
			i = 6;
		}

                else if (pa[1] == BCCMDVARID_COEX_DUMP)
                {
                        if (pa[3] == BCCMD_OK)
                        {
                                uint16 scheme;
                                uint16 idx;

                                printlf("\n\nCoexistence System Dump\n\n");

                                scheme = pa[4] | pa[5];
                                switch(scheme)
                                {
                                        case BCCMDPDU_COEX_DUMP_SCHEME_NONE           :
                                                printlf("  Coexistence Scheme:  Disabled\n");
                                                break;
                                        case BCCMDPDU_COEX_DUMP_SCHEME_UNITY_2        :
                                                printlf("  Coexistence Scheme:  Unity 2\n");
                                                break;
                                        case BCCMDPDU_COEX_DUMP_SCHEME_UNITY_3        :
                                                printlf("  Coexistence Scheme:  Unity 3\n");
                                                break;
                                        case BCCMDPDU_COEX_DUMP_SCHEME_UNITY_4        :
                                                printlf("  Coexistence Scheme:  Unity 4\n");
                                                break;
                                        case BCCMDPDU_COEX_DUMP_SCHEME_UNITY_3_PLUS   :
                                                printlf("  Coexistence Scheme:  Unity 3+\n");
                                                break;
                                        case BCCMDPDU_COEX_DUMP_SCHEME_UNITY_4_PLUS   :
                                                printlf("  Coexistence Scheme:  Unity 4+\n");
                                                break;
                                        case BCCMDPDU_COEX_DUMP_SCHEME_UNITY_3E       :
                                                printlf("  Coexistence Scheme:  Unity 3e\n");
                                                break;
                                        case BCCMDPDU_COEX_DUMP_SCHEME_UNITY_4E       :
                                                printlf("  Coexistence Scheme:  Unity 4e\n");
                                                break;
                                        case BCCMDPDU_COEX_DUMP_SCHEME_UNITY_3E_PLUS  :
                                                printlf("  Coexistence Scheme:  Unity 3e+\n");
                                                break;
                                        case BCCMDPDU_COEX_DUMP_SCHEME_UNITY_4E_PLUS  :
                                                printlf("  Coexistence Scheme:  Unity 4e+\n");
                                                break;
                                        case BCCMDPDU_COEX_DUMP_SCHEME_UNITY_PRO  :
                                                printlf("  Coexistence Scheme:  Unity-Pro\n");
                                                break;
                                        default:
                                                printlf("  Coexistence Scheme:  %4.4x\n",scheme);
                                                break;
                                }

                                printlf("\n");
                                switch(pa[4])
                                {
                                        case BCCMDPDU_COEX_DUMP_SCM_UNITY_2:
                                                if (pa[6] & BCCMDPDU_COEX_DUMP_SIGNAL_ACTIVE)
                                                {
                                                        if ((pa[6] & BCCMDPDU_COEX_DUMP_SIGNAL_ACTIVE)
                                                                 == BCCMDPDU_COEX_DUMP_SIGNAL_ACTIVE_HIGH)
                                                        {
                                                                printlf("  BT_PRIORITY: Pin %d  Active High\n",(pa[6]>>8));
                                                        }
                                                        else
                                                        {
                                                                printlf("  BT_PRIORITY: Pin %d  Active Low\n",(pa[6]>>8));
                                                        }
                                                        if ((pa[7] & BCCMDPDU_COEX_DUMP_SIGNAL_ACTIVE)
                                                                 == BCCMDPDU_COEX_DUMP_SIGNAL_ACTIVE_HIGH)
                                                        {
                                                                printlf("  WLAN_ACTIVE: Pin %d  Active High\n",(pa[7]>>8));
                                                        }
                                                        else
                                                        {
                                                                printlf("  WLAN_ACTIVE: Pin %d  Active Low\n",(pa[7]>>8));
                                                        }
                                                }
                                                break;
                                        case BCCMDPDU_COEX_DUMP_SCM_UNITY_3:
                                                if (pa[8] & BCCMDPDU_COEX_DUMP_SIGNAL_ACTIVE)
                                                {
                                                        if ((pa[8] & BCCMDPDU_COEX_DUMP_SIGNAL_ACTIVE)
                                                                 == BCCMDPDU_COEX_DUMP_SIGNAL_ACTIVE_HIGH)
                                                        {
                                                                printlf("  BT_ACTIVE  : Pin %d  Active High\n",(pa[8]>>8));
                                                        }
                                                        else
                                                        {
                                                                printlf("  BT_ACTIVE  : Pin %d  Active Low\n",(pa[8]>>8));
                                                        }
                                                }

                                                if (pa[9] & BCCMDPDU_COEX_DUMP_SIGNAL_ACTIVE)
                                                {
                                                        if ((pa[9] & BCCMDPDU_COEX_DUMP_SIGNAL_ACTIVE)
                                                                 == BCCMDPDU_COEX_DUMP_SIGNAL_ACTIVE_HIGH)
                                                        {
                                                                printlf("  BT_STATUS  : Pin %d  Active High\n",(pa[9]>>8));
                                                        }
                                                        else
                                                        {
                                                                printlf("  BT_STATUS  : Pin %d  Active Low\n",(pa[9]>>8));
                                                        }
                                                }

                                                if (pa[10] & BCCMDPDU_COEX_DUMP_SIGNAL_ACTIVE)
                                                {
                                                        if ((pa[10] & BCCMDPDU_COEX_DUMP_SIGNAL_ACTIVE)
                                                                 == BCCMDPDU_COEX_DUMP_SIGNAL_ACTIVE_HIGH)
                                                        {
                                                                printlf("  WLAN_DENY  : Pin %d  Active High\n",(pa[10]>>8));
                                                        }
                                                        else
                                                        {
                                                                printlf("  WLAN_DENY  : Pin %d  Active Low\n",(pa[10]>>8));
                                                        }
                                                }

                                                if ((pa[5] & BCCMDPDU_COEX_DUMP_OPT_PLUS)
                                                    && (pa[11] & BCCMDPDU_COEX_DUMP_SIGNAL_ACTIVE))
                                                {
                                                        if ((pa[11] & BCCMDPDU_COEX_DUMP_SIGNAL_ACTIVE)
                                                             == BCCMDPDU_COEX_DUMP_SIGNAL_ACTIVE_HIGH)
                                                        {
                                                                printlf("  BT_PERIODIC: Pin %d  Active High\n",(pa[11]>>8));
                                                        }
                                                        else
                                                        {
                                                                printlf("  BT_PERIODIC: Pin %d  Active Low\n",(pa[11]>>8));
                                                        }
                                                }

                                                if ((pa[5] & BCCMDPDU_COEX_DUMP_OPT_INBAND)
                                                    && (pa[12] & BCCMDPDU_COEX_DUMP_SIGNAL_ACTIVE))
                                                {
                                                        if ((pa[12] & BCCMDPDU_COEX_DUMP_SIGNAL_ACTIVE)
                                                             == BCCMDPDU_COEX_DUMP_SIGNAL_ACTIVE_HIGH)
                                                        {
                                                                printlf("  BT_INBAND  : Pin %d  Active High\n",(pa[12]>>8));
                                                        }
                                                        else
                                                        {
                                                                printlf("  BT_INBAND  : Pin %d  Active Low\n",(pa[12]>>8));
                                                        }
                                                }

                                                printlf("\n");
                                                if (!(pa[5] & BCCMDPDU_COEX_DUMP_OPT_EXPRESS))
                                                {
                                                        printlf("  T1 Time : %3d us\n",pa[16]);
                                                        printlf("  T2 Time : %3d us\n",pa[17]);
                                                }
                                                else
                                                {
                                                        printlf("  BTActiveLead : %3d us\n",pa[18]);
                                                        printlf("  BTStatusLead : %3d us\n",pa[19]);
                                                }
                                                break;
                                        default:
                                                break;
                                }

                                if (pa[13])
                                {
                                    printlf("\n");
                                    printlf("  Coexistence UART TX  : Pin %d\n",pa[14]);
                                    printlf("  Coexistence UART RX  : Pin %d\n",pa[15]);
                                }

                                if (pa[4] != BCCMDPDU_COEX_DUMP_SCM_NONE)
                                {
                                        idx = 20;
                                        printlf("\n  Priority Table\n\n");
                                        printlf("    Page             %3d   Page Scan        %3d\n",pa[idx+ 0],pa[idx+ 1]);
                                        printlf("    Inquiry          %3d   Inquiry Scan     %3d\n",pa[idx+ 2],pa[idx+ 3]);
                                        printlf("    Role Switch      %3d   LMP to Master    %3d\n",pa[idx+ 4],pa[idx+ 5]);
                                        printlf("    LMP from Master  %3d   SCO / eSCO       %3d\n",pa[idx+ 6],pa[idx+ 7]);
                                        printlf("    ESCO Retrans.    %3d   Poll             %3d\n",pa[idx+ 8],pa[idx+ 9]);
                                        printlf("    Sniff            %3d   Bulk ACL         %3d\n",pa[idx+10],pa[idx+11]);
                                        printlf("    Broadcast        %3d   Park             %3d\n",pa[idx+12],pa[idx+13]);
                                        printlf("    Band Scan        %3d   Conditional Scan %3d\n",pa[idx+14],pa[idx+15]);
                                        printlf("    Radio Trim       %3d\n",pa[idx+16]);

                                        idx = 37;
                                        printlf("\n  Ble Priority Table\n\n");
                                        printlf("    Non-Connect Adv  %3d   Discoverable Adv %3d\n",pa[idx+ 0],pa[idx+ 1]);
                                        printlf("    Con Undir Adv    %3d   Con Direct Adv   %3d\n",pa[idx+ 2],pa[idx+ 3]);
                                        printlf("    Adv (Scan Resp)  %3d   Passive Scan     %3d\n",pa[idx+ 4],pa[idx+ 5]);
                                        printlf("    Active Scan      %3d   AS (Scan Resp)   %3d\n",pa[idx+ 6],pa[idx+ 7]);
                                        printlf("    Initiator   .    %3d   Con Est (Master) %3d\n",pa[idx+ 8],pa[idx+ 9]);
                                        printlf("    Con Est (Slave)  %3d   Anchor Point (M) %3d\n",pa[idx+10],pa[idx+11]);
                                        printlf("    Anchor Point (S) %3d   Data (Master)    %3d\n",pa[idx+12],pa[idx+13]);
                                        printlf("    Data (Slave)     %3d   Link Layer Cntrl %3d\n",pa[idx+14],pa[idx+15]);
                                }
                                idx = 53;
                                                         
                                if (pa[idx])
                                {
                                        printlf("\n  Debug\n\n");
                                        printlf("    %4.4x %4.4x %4.4x %4.4x\n",pa[idx+1],pa[idx+2],pa[idx+3],pa[idx+4]);
                                        printlf("    %4.4x %4.4x %4.4x %4.4x\n",pa[idx+5],pa[idx+6],pa[idx+7],pa[idx+8]);
                                        printlf("    %4.4x %4.4x %4.4x %4.4x\n",pa[idx+9],pa[idx+10],pa[idx+11],pa[idx+12]);
                                        printlf("    %4.4x %4.4x %4.4x %4.4x\n",pa[idx+13],pa[idx+14],pa[idx+15],pa[idx+16]);
                                }

                                i = 70;
                        }
                        else
                        {
                                printlf("\n\n  Firmware does not support Coexistence Dump\n\n");
                                i = pa[0];
                        }
                }                
		else if (pa[1] == BCCMDVARID_GPS_START)
		{
			printByValue (NULL, pa[4], 16, " flags");
			printByValue (NULL, pa[5], 16, " delay");
		}
		else if (pa[1] == BCCMDVARID_GPS_STOP)
		{
			printByValue (NULL, pa[4], 16, " flags");
		}
		else if (pa[1] == BCCMDVARID_COASTER_START)
		{
			printByValue (NULL, pa[4], 16, " flags");
			printByValue (NULL, pa[5], 16, " delay");
		}
		else if (pa[1] == BCCMDVARID_COASTER_STOP)
		{
			printByValue (NULL, pa[4], 16, " flags");
		}
		else if (pa[1] == BCCMDVARID_COASTER_DEL_SV)
		{
			printlf (" SVlist");
			printByValue (NULL, pa[4] & 255, 8, " 0");
			printByValue (NULL, pa[4] >> 8 , 8, " 1");
			printByValue (NULL, pa[5] & 255, 8, " 2");
			printByValue (NULL, pa[5] >> 8 , 8, " 3");
			printByValue (NULL, pa[6] & 255, 8, " 4");
			printByValue (NULL, pa[6] >> 8 , 8, " 5");
			printByValue (NULL, pa[7] & 255, 8, " 6");
			printByValue (NULL, pa[7] >> 8 , 8, " 7");
		}
		else if (pa[1] == BCCMDVARID_COASTER_INDICATIONS)
		{
			printByValue (NULL, pa[4], 16, " mode");
			printByValue (NULL, pa[5], 32, " timestamp/us");
		}
		else if (pa[1] == BCCMDVARID_COASTER_ADD_SV)
		{
			printByValue (NULL, pa[4], 8, " entries");
		}
		else if (pa[1] == BCCMDVARID_COASTER_SV_STATE)
		{
			int i;
			printByValue (NULL, (pa[4]>>8), 8, " startIndex");
			printByValue (NULL, (pa[4]&0xff), 8, " numEntries filled");

#define SIZE_ENTRY_BCCMDVARID_COASTER_SV_STATE (10)

			if ( 4 >= (pa[4]&0xff) ) {
				printlf("\n");
				for ( i = 0; i < (pa[4]&0xff) ; i++ ) {
					printlf(
					        "%2d: ts %12d:%2d codeDopp 0x%08x, codeShift 0x%07x, carrDopp 0x%08x, lock 0x%04x, SNR %5d, dataTxn %2d, satid %2d, PRN %2d\n",
					        i,
					        pa[ SIZE_ENTRY_BCCMDVARID_COASTER_SV_STATE * i + 4 + 1 ],
					        pa[ SIZE_ENTRY_BCCMDVARID_COASTER_SV_STATE * i + 4 + 2 ],
					        pa[ SIZE_ENTRY_BCCMDVARID_COASTER_SV_STATE * i + 4 + 3 ],
					        pa[ SIZE_ENTRY_BCCMDVARID_COASTER_SV_STATE * i + 4 + 4 ],
					        pa[ SIZE_ENTRY_BCCMDVARID_COASTER_SV_STATE * i + 4 + 5 ],
					        pa[ SIZE_ENTRY_BCCMDVARID_COASTER_SV_STATE * i + 4 + 6 ],
					        pa[ SIZE_ENTRY_BCCMDVARID_COASTER_SV_STATE * i + 4 + 7 ],
					        pa[ SIZE_ENTRY_BCCMDVARID_COASTER_SV_STATE * i + 4 + 8 ],
					        pa[ SIZE_ENTRY_BCCMDVARID_COASTER_SV_STATE * i + 4 + 9 ] >> 8,
					        pa[ SIZE_ENTRY_BCCMDVARID_COASTER_SV_STATE * i + 4 + 9 ] & 0xff );
				}
			}
			else {
				printlf( "\nEntry count %u out of range!", (unsigned) pa[4]&0xff );
			}
		}
		else if (pa[1] == BCCMDVARID_EGPS_PULSE_CONFIG)
		{
			printlf (" ");
			printByValue (NULL, pa[4], 16, "pio");
		}
		else if (pa[1] == BCCMDVARID_EGPS_PULSE_INPUT_START)
		{
			printlf (" ");
			printByValue (NULL, pa[4], 8, "pio");
			printlf (" ");
			printByValue (NULL, pa[5], 8, "edge_mode");
		}
		else if (pa[1] == BCCMDVARID_EGPS_PULSE_STOP)
		{
			printlf (" ");
			printByValue (NULL, pa[4], 8, "pio_stop");
			printlf (" ");
			printByValue (NULL, pa[5], 16, "flags");
		}
		else if (pa[1] == BCCMDVARID_SCO_PARAMETERS)
		{
			printlf (" ");
			printch (pa[4]);
			if(pa[3] == BCCMD_OK)
			{
				printlf (" T(e)sco:%lu", (ul) pa[5]);
				printlf (" Wesco:%lu", (ul) pa[6]);
				printlf (" Reserved Slots:%lu", (ul) pa[7]);
			}
			i = 8;
		}
		else if (pa[1] == BCCMDVARID_CODEC_INPUT_GAIN)
		{
			printlf (" ");
			printByValue (NULL, pa[4], 16, "gain_a");
			printlf (" ");
			printByValue (NULL, pa[5], 16, "gain_b");

		}
		else if (pa[1] == BCCMDVARID_CODEC_OUTPUT_GAIN)
		{
			printlf (" ");
			printByValue (NULL, pa[4], 16, "gain_a");
			printlf (" ");
			printByValue (NULL, pa[5], 16, "gain_b");

		}
		else if ((pa[1] == BCCMDVARID_STREAM_GET_SOURCE) ||
		         (pa[1] == BCCMDVARID_STREAM_GET_SINK))
		{
			printlf (" ");
			if (pa[4] == 0)
				printByValue (NULL, pa[4], 16, "failed");
			else
				printByValue (NULL, pa[4], 16, "sid");

			i = 5;
		}
		else if (pa[1] == BCCMDVARID_STREAM_CONNECT)
		{
			printlf (" ");
			if (pa[4] == 0)
				printByValue (NULL, pa[4], 16, "failed");
			else
				printByValue (NULL, pa[4], 16, "transform_id");

			i = 5;
		}
		else if(pa[1] == BCCMDVARID_CHARGER_TRIMS)
		{
			if(pa[3] == BCCMD_OK)
            {
				printlf ("\nchgref: %d", pa[4]);
                printlf (" hvref: %d", pa[5]);
                printlf (" rtrim: %d", pa[6]);
                printlf (" itrim: %d", pa[7]);
                printlf (" iext: %d", pa[8]);
				printlf (" iterm: %d", pa[9]);
                printlf (" vfast: %d", pa[10]);
                printlf (" hyst: %d", pa[11]);
            }
			else
            {
				printlf("\nCharger Trims not found.");
            }
			i = IMAX; /* Set to max whole message has been printed */
		}
		else if(pa[1] == BCCMDVARID_CAPSENSE_RT_PADS)
		{
			if(pa[3] == BCCMD_OK)
			{
				/* Preserve values for use in the read command */
				u16 pad_sel = (u16)pa[CAPSENSE_PADSEL_OFFSET];
				u16 num_pads_board = (u16)pa[CAPSENSE_PADBORD_OFFSET];
				bool shield = (bool)pa[CAPSENSE_SHIELD_OFFSET];
				u32 * pad_cap = &pa[CAPSENSE_PADVAL_OFFSET];
				u16 mask;
				u32 pad;

				/* Has pad0 been selected when it's shielded? */
				if(shield)
				{
					printlf("\nPad 0 is shielded: measurement was not possible");
				}
				for(pad = 0, mask = 1; pad < num_pads_board; pad++, mask <<= 1)
				{
					if(mask & pad_sel)
					{
						printlf("\nPad%d capacitance: %d", pad, (uint16)pad_cap[pad]);
					}
				}
				printlf("\n");
			}
			else
			{
				char * reason = lookupByValue(bcCmdStatus_d, pa[3]);
				if(reason == NULL)
				{
					printlf("\nCapsense RT failed, reason %lu is undefined", pa[3]);
				}
				else
				{
					printlf("\nCapsense RT failed, reason - %lu:%s", pa[3], reason);
				}
            }
			i = IMAX; /* Set to max whole message has been printed */
		}
		else if(pa[1] == BCCMDVARID_PIO32_DIRECTION_MASK ||  \
				pa[1] == BCCMDVARID_PIO32 || \
				pa[1] == BCCMDVARID_PIO32_PROTECT_MASK || \
				pa[1] == BCCMDVARID_PIO32_STRONG_BIAS || \
				pa[1] == BCCMDVARID_PIO32_MAP_PINS )
		{
			printlf ("\nResults: 0x%08x", pa[6]);
			i = IMAX; /* Set to max whole message has been printed */
		}
                else if (pa [3] != BCCMDPDU_STAT_OK &&
                         (
                          pa [1] == BCCMDVARID_CREATE_OPERATOR_C      ||
                          pa [1] == BCCMDVARID_CREATE_OPERATOR_P      ||
                          pa [1] == BCCMDVARID_OPERATOR_MESSAGE       ||
                          pa [1] == BCCMDVARID_DOWNLOAD_CAPABILITY    ||
                          pa [1] == BCCMDVARID_REMOVE_DOWNLOADED_CAPABILITY
                        ))
                {
                    printlf (" ");
                    i = print_stibbons_error (pa);
                }
                else if (pa [1] == BCCMDVARID_CREATE_OPERATOR_C)
                {
                    printlf (" ");
                    printByValue (NULL, pa [4], 16, "opid");
                    if (pa [4] == 0)
                    {
                        printlf (" ");
                        printByValue (NULL, pa [5], 16, "np");
                        printlf (" ");
                        printByValue (NULL, pa [6], 16, "skip");
                        printlf (" ");
                        printByValue (create_operator_skip_flag_d, pa [7], 16, "sf");
                        i = 8;
                    }
                    else
                        i = 5;
                }
                else if (pa [1] == BCCMDVARID_CREATE_OPERATOR_P ||
                         pa [1] == BCCMDVARID_OPERATOR_MESSAGE)
                {
                    printlf (" ");
                    printByValue (NULL, pa [4], 16, "opid");
                    i = 5;
                }
                else if (pa [1] == BCCMDVARID_START_OPERATOR   ||
                         pa [1] == BCCMDVARID_STOP_OPERATOR    ||
                         pa [1] == BCCMDVARID_RESET_OPERATOR   ||
                         pa [1] == BCCMDVARID_DESTROY_OPERATOR   )
                {
                    printlf (" ");
                    if (pa[3] == BCCMDPDU_STAT_OK)
                    {
                        printByValue (NULL, pa[4], 16, "succeeded");
                        i = pa [0];
                    }
                    else
                        i = print_stibbons_counted_error (pa);
                }
				else if (pa[1] == BCCMDVARID_SPI_LOCK_STATUS)
				{
					printlf (" ");
					printByValue (spilock_d, pa[4], 16, NULL);
                    i = IMAX;
				}

	}

	strend = 0;
	istart = i;
	printable = 1;
	for (; i < pa[0]; ++i)
	{
		ASSERT (pa[1] != BCCMDVARID_PS || pa[i] <= 0xffffUL);


		printlf (" 0x%0*lx", minwid, (ul) pa[i]);
		/*
		 * Try to guess if this is a printable string.
		 * Don't attempt to handle multiple bytes packed into a word.
		 * If a word is 0, and it was printable so far, assume
		 * this is the end of the string.
		 * If everything was printable to the end, assume it
		 * is a chunk of a printable string.
		 */
		if (showAscii && printable && !strend)
		{
			if (!pa[i])
			strend = i;
			else if (pa[i] > 127 || !isprint (pa[i]))
			printable = 0;
		}
	}
	if (showAscii && printable)
	{
		if (!strend)
		strend = pa[0];
		if (strend - istart > 2)
		{
		/*
		 * It looks like we can print it and it's at least 3
		 * characters.
		 */
		printlf (" (\"");
		for (i = istart; i < strend; i++)
			printChar ((char) pa[i]);
		printlf ("\")");
		}
	}
	printlf ("\n");
}
