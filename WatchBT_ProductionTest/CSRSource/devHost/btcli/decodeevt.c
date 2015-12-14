// Copyright (C) 2000-2006 Cambridge Silicon Radio Ltd.; all rights reserved.
// Decoding of received HCI events (and symbolic connection handle support).
// $Id: //depot/bc/bluesuite_2_4/devHost/btcli/decodeevt.c#1 $


#include <stdio.h>

#if !defined(_WINCE) && !defined(_WIN32_WCE)
#include <stddef.h>
#include <assert.h>
#define ASSERT assert
#else
#include "wince/wincefuncs.h"
#endif

#include "btcli.h"
#include "print.h"
#include "constants.h"
#include "globals.h"
#include "dictionaries.h"
#include "commandtable.h"
#include "assertn.h"
#include "common/types_t.h"
#include "app/bluestack/bluetooth.h"
#include "app/bluestack/hci.h"

static void ic_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 1 || pa[0] == 2 + 2);

	printStatus (pa[2]);
	if (pa[0] == 2 + 2)
	{
		printlf (" ");
		printByValue (NULL, pa[3], 8, "nr");
	}
}


static void ir_e (const PA pa)
{
	size_t i;

	ASSERT (pa[0] >= 2 + 1);
	ASSERT (pa[0] == 2 + 1 + pa[2] * 8);

	printByValue (NULL, pa[2], 8, "nr");
	for (i = 0; i < pa[2]; ++i)
	{
		lastlap = pa[3 + i * 8 + 0];
		lastuap = pa[3 + i * 8 + 1];
		lastnap = pa[3 + i * 8 + 2];
		printlf (" ");
		printba (pa[3 + i * 8 + 0], pa[3 + i * 8 + 1], pa[3 + i * 8 + 2]);
		printlf (" ");
		printByValue (psrm_d, pa[3 + i * 8 + 3], 8, "psrm");
		printlf (" ");
		printByValue (pspm_d, pa[3 + i * 8 + 4], 8, "pspm");
		printlf (" ");
		printByValue (psm_d, pa[3 + i * 8 + 5], 8, "psm");
		printlf (" ");
		printByValue (NULL, pa[3 + i * 8 + 6], 24, "cod");
		printlf (" ");
		printByValue (NULL, pa[3 + i * 8 + 7], 16, "co");
	}
}


static void irwr_e (const PA pa)
{
	size_t i;

	ASSERT (pa[0] >= 2 + 1);
	ASSERT (pa[0] == 2 + 1 + pa[2] * 8);

	printByValue (NULL, pa[2], 8, "nr");
	for (i = 0; i < pa[2]; ++i)
	{
		lastlap = pa[3 + i * 8 + 0];
		lastuap = pa[3 + i * 8 + 1];
		lastnap = pa[3 + i * 8 + 2];
		printlf (" ");
		printba (pa[3 + i * 8 + 0], pa[3 + i * 8 + 1], pa[3 + i * 8 + 2]);
		printlf (" ");
		printByValue (psrm_d, pa[3 + i * 8 + 3], 8, "psrm");
		printlf (" ");
		printByValue (pspm_d, pa[3 + i * 8 + 4], 8, "pspm");
		printlf (" ");
		printByValue (NULL, pa[3 + i * 8 + 5], 24, "cod");
		printlf (" ");
		printByValue (NULL, pa[3 + i * 8 + 6], 16, "co");
		printlf (" rssi:%i", (signed char) pa[3 + i * 8 + 7]);
	}
}


static void eir_e (const PA pa)
{
	ASSERT (pa[0] >= 2 + 1);
	//ASSERT (pa[0] == 2 + 1 + pa[2] * 8);

	printByValue (NULL, pa[2], 8, "nr");

	lastlap = pa[3];
	lastuap = pa[4];
	lastnap = pa[5];
	printlf (" ");
	printba (pa[3], pa[4], pa[5]);
	printlf (" ");
	printByValue (psrm_d, pa[6], 8, "psrm");
	printlf (" ");
	printByValue (pspm_d, pa[7], 8, "pspm");
	printlf (" ");
	printByValue (NULL, pa[8], 24, "cod");
	printlf (" ");
	printByValue (NULL, pa[9], 16, "co");
	printlf (" ");
	printByValue (NULL, pa[10], 16, "rssi");
        printEIRD(&pa[11],pa[0]-11);
#if 0
	for (i = pa[0] - 11, ptr = &pa[11]; (i > 0 && *ptr > 0 && *ptr <= i); i -= *ptr, ptr += *ptr)
	{
		j = ptr[1]*8 - 8;
		ptr2 = &ptr[2];
		printlf ("\n\t");
		printByValue (eirdt_d, ptr[1], 8, "eirdt");
		printlf (":\t");
		switch (ptr[1])
		{
		case 0x00: /* End of Inquiry Response Data */
			break;

		case 0x01: /* Flags word */
			printByValue (eirdtf_d, ptr[2], j, "eirdtf");
			break;

		case 0x02: /* 16-bit Service Class UUIDs - incomplete list */
		case 0x03: /* 16-bit Service Class UUIDs - complete list */
		case 0x04: /* 32-bit Service Class UUIDs - incomplete list */
		case 0x05: /* 32-bit Service Class UUIDs - complete list */
			printByValue (NULL, ptr[2], j, "data");
			break;

		case 0xff: /* Manufacturer-specific */
			printByValue (manufacturerName_d, *(ptr2++), 16, "mn");
			printlf (":\t");
			j-=16;
			/* Intentional fall-through */

		case 0x06: /* 128-bit Service Class UUIDs - incomplete list */
		case 0x07: /* 128-bit Service Class UUIDs - complete list */
		default:
			for (; j >= 32; j -= 32)
			    printByValue (NULL, *(ptr2++), 32, "data");
/* HERE */
			break;

		case 0x08: /* Local name - abbreviated */
		case 0x09: /* Local name - complete */
			printString (&ptr[2], ptr[1]-1);
			break;
		}
	}
#endif
	if (pa[2] > 1)
		printlf ("\n*** Error: eir response may only contain one result but received event has an nr of greater than one!\n");
}


static int addAcl (u32 ch)
{
	int i;

	for (i = 0; i < 18; ++i)
	{
		if (aclch[i] == NOHANDLE)
		{
            openAclChannel(ch);
			aclch[i] = ch;
			return i;
		}
	}

	return -1;
}


static int addSco (u32 ch)
{
	int i;

	for (i = 0; i < 10; ++i)
	{
		if (scoch[i] == NOHANDLE)
		{
            openScoChannel(i, ch);
			return i;
		}
	}

	return -1;
}


static void delAcl (u32 ch)
{
	int i;

	for (i = 0; i < 18; ++i)
	{
		if (aclch[i] == ch)
		{
            closeAclChannel(ch);
			aclch[i] = NOHANDLE;
			return;
		}
	}
}


static void delSco (u32 ch)
{
	int i;

	for (i = 0; i < 10; ++i)
	{
		if (scoch[i] == ch)
		{
            closeScoChannel(i, ch);
			return;
		}
	}
}


static void cnc_e (const PA pa)
{
	bool wlps = false;
	int i;

	ASSERT (pa[0] == 2 + 7);

	printStatus (pa[2]);
	if (pa[2] == 0) /* success */
	{
		if (pa[7] == LT_SCO)
		{
			addSco (pa[3]);
			lastscoch = pa[3];
		}
		else if (pa[7] == LT_ACL)
		{
			i = addAcl (pa[3]);
			ulplink[i] = false;
			lastaclch = pa[3];
			if (autoWlps)
			{
				wlps = true;
			}
		}
		lastlap = pa[4];
		lastuap = pa[5];
		lastnap = pa[6];
	}
	printlf (" ");
	printch (pa[3]);
	printlf (" ");
	printba (pa[4], pa[5], pa[6]);
	printlf (" ");
	printByValue (linkType_d, pa[7], 8, "lt");
	printlf (" ");
	printByValue (encryptionMode_d, pa[8], 8, "em");
	printlf ("\n");
	if (wlps)
	{
		parseCmd ("wlps l eall", true);
	}
}


static void cr_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 5);

	lastlap = pa[2];
	lastuap = pa[3];
	lastnap = pa[4];
	printba (pa[2], pa[3], pa[4]);
	printlf (" ");
	printByValue (NULL, pa[5], 24, "cod");
	printlf (" ");
	printByValue (linkType_d, pa[6], 8, "lt");
}


static void dc_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 3);

    printTimestamp();
    printlf("disconnection_complete ");
    printStatus(pa[2]);
    printlf(" ");
    printch(pa[3]);
    printlf(" ");
    printByValue(errorCode_d, pa[4], 8, "r");
    if (pa[2] == 0) /* success */
    {
        delAcl(pa[3]);
        delSco(pa[3]);
        if (pa[3] == lastscoch)
        {
            lastscoch = NOHANDLE;
        }
        else if (pa[3] == lastaclch)
        {
            lastaclch = NOHANDLE;
        }
    }
}


static void ac_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 2);

	printStatus (pa[2]);
	printlf (" ");
	printch (pa[3]);
}


static void rnrc_e (const PA pa)
{
//	ASSERT (pa[0] == 2 + 4 + 248);

	printStatus (pa[2]);
	printlf (" ");
	printba (pa[3], pa[4], pa[5]);
	printlf (" ");
	printString (&pa[6], 248);

	if (pa[0] != 2 + 4 + 248)
	{
		printlf (" (short)");
	}
}


static void ec_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 3);

	printStatus (pa[2]);
	printlf (" ");
	printch (pa[3]);
	printlf (" ");
	printByValue (offOn_d, pa[4], 8, "ee");
}


static void cclkc_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 2);

	printStatus (pa[2]);
	printlf (" ");
	printch (pa[3]);
}


static void mlkc_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 3);

	printStatus (pa[2]);
	printlf (" ");
	printch (pa[3]);
	printlf (" ");
	printByValue (keyFlag_d, pa[4], 8, "kf");
}


static void rrsfc_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 10);

	printStatus (pa[2]);
	printlf (" ");
	printch (pa[3]);
	printlf (" ");
	printFeaturesPageX (0, &pa[4], "lf");
}


static void rrvic_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 5);

	printStatus (pa[2]);
	printlf (" ");
	printch (pa[3]);
	printlf (" ");
	printByValue (lmpVersion_d, pa[4], 8, "lv");
	printlf (" ");
	printByValue (manufacturerName_d, pa[5], 16, "mn");
	printlf (" ");
	printByValue (NULL, pa[6], 16, "ls");
	if (pa[5] == 0xa /* CSR */ &&
	    lookupByValue (id_d, pa[6]))
	{
		printlf ("(");
		printByValue (id_d, pa[6], 16, NULL);
		printlf (")");
	}
}


static void qsc_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 8);

	printStatus (pa[2]);
	printlf (" ");
	printch (pa[3]);
	printlf (" ");
	printByValue (NULL, pa[4], 8, "f");
	printlf (" ");
	printByValue (serviceType_d, pa[5], 8, "st");
	printlf (" ");
	printByValue (NULL, pa[6], 32, "tr");
	printlf (" ");
	printByValue (NULL, pa[7], 32, "pb");
	printlf (" ");
	printByValue (NULL, pa[8], 32, "l");
	printlf (" ");
	printByValue (NULL, pa[9], 32, "dv");
}


static void fsc_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 9);

	printStatus (pa[2]);
	printlf (" ");
	printch (pa[3]);
	printlf (" ");
	printByValue (NULL, pa[4], 8, "f");
	printlf (" ");
	printByValue (fd_d, pa[5], 8, "fd");
	printlf (" ");
	printByValue (serviceType_d, pa[6], 8, "st");
	printlf (" ");
	printByValue (NULL, pa[7], 32, "tr");
	printlf (" ");
	printByValue (NULL, pa[8], 32, "tbs");
	printlf (" ");
	printByValue (NULL, pa[9], 32, "pb");
	printlf (" ");
	printByValue (NULL, pa[10], 32, "al");
}


#define BADCOMMAND ((size_t) -1)

static size_t findHCIcommandByOpcode (unsigned opcode)
{
	size_t i, n = getNumCommands ();

	if (opcode == NOTHCICOMMAND)
	{
		return BADCOMMAND;
	}

	for (i = 0; i < n; ++i)
	{
		if (getCommandOpcode (i) == opcode)
		{
			break;
		}
	}

	if (i == n)
	{
		return BADCOMMAND;
	}

	return i;
}


static void cc_e (const PA pa)
{
	size_t index;

	ASSERT (pa[0] >= 2 + 2); /* There is not necessarily a status */

	printByValue (NULL, pa[2], 8, "nhcp");

	index = findHCIcommandByOpcode (pa[3]);
	if (index == BADCOMMAND)
	{
		size_t i;

		if (csr_extensions && pa[3] == TUNNEL)
		{
			printlf (" manufacturer_extension");
		}
		else if (!csr_extensions && pa[3] >= TUNNEL)
		{
			printlf (" manufacturer_extension_0x%03x", pa[3] - TUNNEL);
		}
		else
		{
			printlf (" ");
			printByValue (NULL, pa[3], 16, "co");
		}
		if (pa[0] > 2 + 2)
		{
			printlf (" rp:");
		}
		for (i = 2 + 2; i < pa[0]; ++i)
		{
			printlf (" 0x%lx", (ul) pa[i]);
		}
		return;
	}

	ASSERT (pa[0] >= 2 + 2 + 1); /* There is now! */

	printlf (" %s ", getCommandFullName (index));
	
	printStatus (pa[4]);

	if (getCommandCompleteDecoder (index) != NULL)
	{
		if (pa[3] != RESET)
		{
			printlf (" ");
		}
		(getCommandCompleteDecoder (index)) (pa);
	}
	else
	{
		ASSERT (pa[0] == 2 + 2 + 1);
	}
}


static void cs_e (const PA pa)
{
	size_t index;

	ASSERT (pa[0] == 2 + 3);

	if (pa[2] == 0) /* pending */
	{
		printlf ("pending");
	}
	else
	{
		printStatus (pa[2]);
	}
	printlf (" ");
	printByValue (NULL, pa[3], 8, "nhcp");
	if (csr_extensions && pa[4] == TUNNEL)
	{
		printlf (" manufacturer_extension");
	}
	else if (!csr_extensions && pa[4] >= TUNNEL)
	{
		printlf (" manufacturer_extension_0x%03x", pa[4] - TUNNEL);
	}
	else if ((index = findHCIcommandByOpcode (pa[4])) == BADCOMMAND)
	{
		printlf (" ");
		printByValue (NULL, pa[4], 16, "co");
	}
	else
	{
		printlf (" %s", getCommandFullName (index));
	}
}


static void he_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 1);

	printByValue (faultID_d, pa[2], 8, "hc");
}


static void fo_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 1);

	printch (pa[2]);
}


static void rc_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 5);

	printStatus (pa[2]);
	printlf (" ");
	printba (pa[3], pa[4], pa[5]);
	printlf (" ");
	printByValue (ms_d, pa[6], 8, "nr");
}

static void nocp_e (const PA pa)
{
    uint8 i;
    bool first_display = true;
    bool displayed = false;

	ASSERT (pa[0] >= 2 + 1);
	ASSERT (pa[0] == 2 + 1 + pa[2] * 2);

    for (i = 0; i < pa[2]; ++i)
    {
        if (!isSendFileActive(pa[3 + i * 2 + 0]) &&
            !isRecvFileActive(pa[3 + i * 2 + 0]))
        {
            /* don't display nocps for channels with file transfers */

            displayed = true;
            if (first_display)
            {
		        printTimestamp ();
		        printlf ("number_of_completed_packets ");
		        printByValue (NULL, pa[2], 8, "noh");
                first_display = false;
            }

			printlf (" ");
			printch (pa[3 + i * 2 + 0]);
			printlf (" ");
			printByValue (NULL, pa[3 + i * 2 + 1], 16, "hnocp");
        }
    }

    if (displayed)
    {
        printlf("\n");
    }
}


static void mc_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 4);

	printStatus (pa[2]);
	printlf (" ");
	printch (pa[3]);
	printlf (" ");
	printByValue (mode_d, pa[4], 8, "cm");
	printlf (" ");
	printByValue (NULL, pa[5], 16, "i");
}


static void rlk_e (const PA pa)
{
	size_t i;
	int j;

	ASSERT (pa[0] >= 2 + 1);
	ASSERT (pa[0] == 2 + 1 + pa[2] * 19);

	printByValue (NULL, pa[2], 8, "nk");
	for (i = 0; i < pa[2]; ++i)
	{
		printlf (" ");
		printba (pa[3 + i * 19 + 0], pa[3 + i * 19 + 1], pa[3 + i * 19 + 2]);
		ASSERT8 (pa[3 + i * 19 + 3 + 15]);
		printlf (" lk:0x%02x", (unsigned) pa[3 + i * 19 + 3 + 15]);
		for (j = 14; j >= 0; --j)
		{
			ASSERT8 (pa[3 + i * 19 + 3 + j]);
			printlf ("%02x", (unsigned) pa[3 + i * 19 + 3 + j]);
		}
	}
}


static void pcr_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 3);

	lastlap = pa[2];
	lastuap = pa[3];
	lastnap = pa[4];
	printba (pa[2], pa[3], pa[4]);
}


static void lkr_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 3);

	lastlap = pa[2];
	lastuap = pa[3];
	lastnap = pa[4];
	printba (pa[2], pa[3], pa[4]);
}


static void lkn_e (const PA pa)
{
	int j;

	ASSERT (pa[0] == 2 + 20 || pa[0] == 2 + 19);

	printba (pa[2], pa[3], pa[4]);
	ASSERT8 (pa[2 + 3 + 15]);
	printlf (" lk:0x%02x", (unsigned) pa[2 + 3 + 15]);
	for (j = 14; j >= 0; --j)
	{
		ASSERT8 (pa[2 + 3 + j]);
		printlf ("%02x", (unsigned) pa[2 + 3 + j]);
	}
	if (pa[0] == 2 + 20)
	{
		printlf (" ");
		printByValue (keyType_d, pa[2 + 3 + 16], 8, "kt");
	}
}


static void lc_e (const PA pa)
{
	size_t i;

	ASSERT (pa[0] >= 2);

	printlf ("hcp:");
	for (i = 2; i < pa[0]; ++i)
	{
		if (i % 5 == 2 && i != 2)
		{
			printlf ("-");
		}
		ASSERT8 (pa[i]);
		printlf ("%02x", (unsigned) pa[i]);
	}
}


static void dbo_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 1);

	printByValue (linkType_d, pa[2], 8, "lt");
}


static void msc_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 2);

	printch (pa[2]);
	printlf (" ");
	printByValue (NULL, pa[3], 8, "lms");
}


static void rcoc_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 3);

	printStatus (pa[2]);
	printlf (" ");
	printch (pa[3]);
	printlf (" ");
	printByValue (NULL, pa[4], 16, "co");
}


static void cptc_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 3);

	printStatus (pa[2]);
	printlf (" ");
	printch (pa[3]);
	printlf (" ");
	printByValue (packetType_d, pa[4], 16, "pt");
}


static void qv_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 1);

	printch (pa[2]);
}


static void psmc_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 4);

	printba (pa[2], pa[3], pa[4]);
	printlf (" ");
	printByValue (psm_d, pa[5], 8, "psm");
}


static void psrmc_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 4);

	printba (pa[2], pa[3], pa[4]);
	printlf (" ");
	printByValue (psrm_d, pa[5], 8, "psrm");
}


static void rrefc_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 12);

	printStatus (pa[2]);
	printlf (" ");
	printch (pa[3]);
	printlf (" ");
	printByValue (NULL, pa[4], 8, "pn");
	printlf (" ");
	printByValue (NULL, pa[5], 8, "mpn");
	printlf (" ");

	printFeaturesPageX(pa[4], &pa[6], "elf");
}


static void scc_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 11);

	printStatus (pa[2]);
	if (pa[2] == 0) /* success */
	{
		addSco (pa[3]);
		lastscoch = pa[3];
		lastlap = pa[4];
		lastuap = pa[5];
		lastnap = pa[6];
	}
	printlf (" ");
	printch (pa[3]);
	printlf (" ");
	printba (pa[4], pa[5], pa[6]);
	printlf (" ");
	printByValue (linkType_d, pa[7], 8, "lt");
	printlf (" ");
	printByValue (NULL, pa[8], 8, "ti");
	printlf (" ");
	printByValue (NULL, pa[9], 8, "rw");
	printlf (" ");
	printByValue (NULL, pa[10], 16, "rpl");
	printlf (" ");
	printByValue (NULL, pa[11], 16, "tpl");
	printlf (" ");
	printByValue (airMode_d, pa[12], 8, "am");
}


static void scch_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 6);

	printStatus (pa[2]);
	printlf (" ");
	printch (pa[3]);
	printlf (" ");
	printByValue (NULL, pa[4], 8, "ti");
	printlf (" ");
	printByValue (NULL, pa[5], 8, "rw");
	printlf (" ");
	printByValue (NULL, pa[6], 16, "rpl");
	printlf (" ");
	printByValue (NULL, pa[7], 16, "tpl");
}

static void ssr_e (const PA pa)
{
    ASSERT (pa[0] == 2 + 6);
    printStatus(pa[2]);
    printlf (" ");
    printch (pa[3]);
    printlf (" ");
    printByValue (NULL, pa[4], 16, "tl");
    printlf (" ");
    printByValue (NULL, pa[5], 16, "rl");
    printlf (" ");
    printByValue (NULL, pa[6], 16, "rto");
    printlf (" ");
    printByValue (NULL, pa[7], 16, "lto");
}

/* Encryption Key Refresh Complete (0x30) */
static void ekr_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 2);
	printStatus (pa[2]);
	printlf (" ");
	printch (pa[3]);
}

/* Simple Pairing IO Capability Request Event (0x31) */
static void iocreq_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 3);
	lastlap = pa[2];
	lastuap = pa[3];
	lastnap = pa[4];
	printba (pa[2], pa[3], pa[4]);
}

/* Simple Pairing IO Capability Response Event (0x32) */
static void iocres_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 3 + 3);

	printba (pa[2], pa[3], pa[4]);
	printlf (" ");
	printByValue (iocap_d, pa[5], 8, "io");
	printlf (" ");
	printByValue (oobpresent_d, pa[6], 8, "oob");
	printlf (" ");
	printByValue (authenticationrequirements_d, pa[7], 8, "ar");
}

/* Simple Pairing User Confirmation Request Event (0x33) */
static void ucr_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 3 + 1);
	lastlap = pa[2];
	lastuap = pa[3];
	lastnap = pa[4];
	printba (pa[2], pa[3], pa[4]);
	printlf (" ");
	printHexAndDec(pa[5], 32, 6, "numeric_value");
/*	printByValue (NULL, pa[5], 32, "numeric_value"); */
}

/* Simple Pairing User Passkey Request Event (0x34) */
static void upr_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 3);
	lastlap = pa[2];
	lastuap = pa[3];
	lastnap = pa[4];
	printba (pa[2], pa[3], pa[4]);
}

/* Simple Pairing Remote OOB Data Request Event (0x35) */
static void rodreq_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 3);
	lastlap = pa[2];
	lastuap = pa[3];
	lastnap = pa[4];
	printba (pa[2], pa[3], pa[4]);
}

/* Simple Pairing Complete Event (0x36) */
static void spc_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 1 + 3);
	printStatus (pa[2]);
	printlf (" ");
	printba (pa[3], pa[4], pa[5]);
}

/* Link Supervision Timeout Changed (0x38) */
static void lsto_e(const PA pa)

{
    ASSERT (pa[0] == 2 + 2);
	printch (pa[2]);
	printlf (" ");
	printByValue (NULL, pa[3], 16, "lst");
}

/* Enhanced Flush Complete (0x39) */
static void efc_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 1);
	printch (pa[2]);
}

/* Simple Pairing User Passkey Notification Event (0x3B) */
static void upn_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 3 + 1);
	printba (pa[2], pa[3], pa[4]);
	printlf (" ");
	printHexAndDec(pa[5], 32, 6, "passkey");
}

/* Simple Pairing Keypress Notification Event (0x3C) */
static void kn_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 3 + 1);
	printba (pa[2], pa[3], pa[4]);
	printlf (" ");
	printByValue (notificationtype_d, pa[5], 8, "nt");
}

/* Remote Host Supported Features Notification Event (0x3D) */
static void rhsfn_e (const PA pa)
{
	ASSERT (pa[0] == 2 + 3 + 8);
	printba (pa[2], pa[3], pa[4]);
	printlf (" ");
	printFeaturesPageX(1, &pa[5], "hsf");
}

static void ulp_e (const PA pa)
{
	int i;
	int j;

	switch (pa[2])
	{
	case HCI_EV_ULP_ADVERTISING_REPORT:
		printlf ("advertising_report");
		printlf (" num_reports: %d", pa[3]);
		printlf (" event_type: ");
		printEventType (pa[4]);
		printlf (" address_type: ");
		printAddressType (pa[5]);
		printlf (" address: ");
		printAddressUlp(pa[6], pa[7], pa[8], pa[9], pa[10], pa[11]);
		printlf (" data_len: %d", pa[12]);
		printlf (" data: ");
		printlf ("\"");
		for (j=0;j<pa[12];j++){
			printChar(pa[13+j]);
		}
		printlf ("\"");
		printlf (" rssi: %d", (int8)pa[13+pa[12]]);
		printlf ("\n");
		break;

	case HCI_EV_ULP_CONNECTION_COMPLETE:
		printlf ("connection_complete ");
		printStatus (pa[3]);
		printlf (" ");
		if (pa[3] == 0) /* status: success */
		{
			i = addAcl (pa[4]);
			ulplink[i] = true;
			lastaclch = pa[4];
			lastlap = pa[8] | pa[9]<<8;
			lastuap = pa[10] | pa[11]<<8;
			lastnap = pa[12] | pa[13]<<8;
		}
                printch (pa[4]);
		printlf (" ");
		printByValue (ms_d, pa[6], 8, NULL);
		printlf (" peer_addr:");
		printAddressType (pa[7]);
		printlf (":");
		printAddressUlp (pa[8], pa[9], pa[10], pa[11], pa[12], pa[13]);
		printlf (" conn_interval:%d", pa[14]|pa[15]<<8);
		printlf (" conn_latency:%d", pa[16]|pa[17]<<8);
		printlf (" conn_timeout:%d", pa[18]|pa[19]<<8);
		printlf (" clk_accuracy:");
		if (pa[6] == 0x01)
			printByValue (clkacc_d, pa[20], 8, NULL);
		else
			printlf ("0x%02X",pa[20]);
		printlf ("\n");
		break;

	case HCI_EV_ULP_CONNECTION_UPDATE_COMPLETE:
		printlf ("connection_update_complete ");
		printStatus (pa[3]);
		printlf (" ");
		printch (pa[4]);
		printlf (" interval: 0x%04x latency: 0x%04x timeout: 0x%04x\n", pa[6]|(pa[7]<<8), pa[8]|(pa[9]<<8), pa[10]|(pa[11]<<8));
		break;

	case HCI_EV_ULP_READ_REMOTE_USED_FEATURES_COMPLETE:
		printlf ("read_remote_used_features_complete ");
		printStatus (pa[3]);
		printlf (" ");
		printch (pa[4]);
		printLEFeatures(&pa[6], " le_features");
		printlf("\n");
		break;

	case HCI_EV_ULP_LONG_TERM_KEY_REQUESTED:
		printlf ("ltk_requested ");
		printch (pa[3]);
		printlf (" rand:0x");
		for (i = 12; i >= 5; i--)
			printlf ("%02x", pa[i]);
		printlf (" ediv:0x%04x\n", pa[13]|(pa[14]<<8));
		break;
	}
}

typedef struct HCIeventTableElement
{
	void (*const decoder_e) (const PA);
	const char *name;
} HCIeventTableElement;


typedef HCIeventTableElement HCIeventTable[];


static HCIeventTable hciEventTable =
{
#ifdef MGRABBREVEVT
	ic_e,	"ic",
	ir_e,	"ir",
	cnc_e,	"cnc",
	cr_e,	"cr",
	dc_e,	"dc",
	ac_e,	"ac",
	rnrc_e,	"rrnc",
	ec_e,	"ec",
	cclkc_e,"cclkc",
	mlkc_e,	"mlkc",
	rrsfc_e,"rrsfc",
	rrvic_e,"rrvic",
	NULL,	"qsc",
	cc_e,	"cc",
	cs_e,	"cs",
	he_e,	"he",
	NULL,	"fo",
	rc_e,	"rc",
	nocp_e,	"nocp",
	mc_e,	"mc",
	rlk_e,	"rlk",
	pcr_e,	"pcr",
	lkr_e,	"lkr",
	lkn_e,	"lkn",
	lc_e,	"lc",
	dbo_e,	"dbo",
	msc_e,	"msc",
	rcoc_e,	"rcoc",
	cptc_e,	"cptc",
	NULL,	"qv",
	psmc_e,	"psmc",
	psrmc_e,"psrmc"
#else
	ic_e,	"inquiry_complete",
	ir_e,	"inquiry_result",
#define CNCE	0x03
	cnc_e,	"connection_complete",
	cr_e,	"connection_request",
#define DCE	0x05
	dc_e,	"disconnection_complete",
	ac_e,	"authentication_complete",
	rnrc_e,	"remote_name_request_complete",
	ec_e,	"encryption_change",
	cclkc_e,"change_connection_link_key_complete",
	mlkc_e,	"master_link_key_complete",
	rrsfc_e,"read_remote_supported_features_complete",
	rrvic_e,"read_remote_version_information_complete",
	qsc_e,	"qos_setup_complete",
	cc_e,	"command_complete",
	cs_e,	"command_status",
	he_e,	"hardware_error",
	fo_e,	"flush_occurred",
	rc_e,	"role_change",
#define NOCPE	0x13
	nocp_e,	"number_of_completed_packets",			// 0x13
	mc_e,	"mode_change",					// 0x14
	rlk_e,	"return_link_keys",				// 0x15
	pcr_e,	"pin_code_request",				// 0x16
	lkr_e,	"link_key_request",				// 0x17
	lkn_e,	"link_key_notification",			// 0x18
	lc_e,	"loopback_command",				// 0x19
	dbo_e,	"data_buffer_overflow",				// 0x1a
	msc_e,	"max_slots_change",				// 0x1b
	rcoc_e,	"read_clock_offset_complete",			// 0x1c
	cptc_e,	"connection_packet_type_changed",		// 0x1d
	qv_e,	"qos_violation",				// 0x1e
	psmc_e,	"page_scan_mode_change",			// 0x1f
	psrmc_e,"page_scan_repetition_mode_change",		// 0x20
	fsc_e,	"flow_specification_complete",			// 0x21
	irwr_e,	"inquiry_result_with_rssi",			// 0x22
	rrefc_e,"read_remote_extended_features_complete",	// 0x23
	NULL,	"fixed_address",				// 0x24
	NULL,	"alias_address",				// 0x25
	NULL,	"generate_alias_request",			// 0x26
	NULL,	"active_address",				// 0x27
	NULL,	"allow_private_pairing",			// 0x28
	NULL,	"alias_address_request",			// 0x29
	NULL,	"alias_not_recognized",				// 0x2a
	NULL,	"fixed_address_attempt",			// 0x2b
	scc_e,	"synchronous_connection_complete",		// 0x2c
	scch_e,	"synchronous_connection_changed",		// 0x2d
	ssr_e,	"sniff_subrate_complete",			// 0x2e
	eir_e,	"extended_inquiry_result",			// 0x2f
	ekr_e,	"encryption_key_refresh_complete",	// 0x30
	iocreq_e, "io_capability_request",			// 0x31
	iocres_e, "io_capability_response",			// 0x32
	ucr_e,	"user_confirmation_request",		// 0x33
	upr_e,	"user_passkey_request",				// 0x34
	rodreq_e,"remote_oob_data_request",			// 0x35
	spc_e,	"simple_pairing_complete",			// 0x36
	NULL,	"unknown_0x37",						// 0x37
	lsto_e, "link_supervision_timeout_changed", // 0x38
	efc_e,  "enhanced_flush_complete",			// 0x39
	NULL,	"unknown_0x3A",						// 0x3A
	upn_e,	"user_passkey_notification",		// 0x3B
	kn_e,   "keypress_notification",			// 0x3C
	rhsfn_e,"remote_host_supported_features_notification" // 0x3D

#define ULPE    0x3e
#define VSDE	0xff
#endif
};


void decodeEvt (const PA pa)
{
	ASSERT (pa[0] >= 2);
	ASSERT8 (pa[1]);

	if (pa[1] != 0 && pa[1] <= sizeof (hciEventTable) / sizeof (hciEventTable[0]) && hciEventTable[pa[1] - 1].decoder_e != NULL)
	{
		if (pa[1] != NOCPE && pa[1] != DCE)
		{
			printTimestamp ();
			printlf ("%s ", hciEventTable[pa[1] - 1].name);
		}
		hciEventTable[pa[1] - 1].decoder_e (pa);
	}
	else if (pa[1] != 0 && pa[1] <= sizeof (hciEventTable) / sizeof (hciEventTable[0]))
	{
		size_t i;

		printTimestamp ();
		printlf ("%s ", hciEventTable[pa[1] - 1].name);

		for (i = 2; i < pa[0]; ++i)
		{
			printlf (" 0x%02x", (unsigned) pa[i]);
		}
	}
	else
	{
		size_t i;

		printTimestamp ();
		if (pa[1] == VSDE)
		{
			printlf ("manufacturer_extension ");
		}
		else if (pa[1] == ULPE)
		{
			printlf ("LE_");
			ulp_e (pa);
			return;
		}
		else
		{
			printlf ("evt_0x%02x ", (unsigned) pa[1]);
		}

		for (i = 2; i < pa[0]; ++i)
		{
			if (i % 5 == 2 && i != 2)
			{
				printlf ("-");
			}
			ASSERT8 (pa[i]);
			printlf ("%02x", (unsigned) pa[i]);
		}
	}

	if (pa[1] != NOCPE && pa[1] != CNCE)
	{
		printlf ("\n");
	}
}
