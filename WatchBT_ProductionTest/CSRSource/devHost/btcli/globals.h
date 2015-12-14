// Copyright (C) 2000-2006 Cambridge Silicon Radio Ltd.; all rights reserved.
// btcli globals.
// $Id: //depot/bc/bluesuite_2_4/devHost/btcli/globals.h#1 $


#ifndef GLOBALS_H
#define GLOBALS_H

#define SCO_COUNT 3

#include <stdio.h>	/* for FILE */
#if !defined(_WINCE) && !defined(_WIN32_WCE)
#include <time.h>	/* for time_t */
#else
#include "wince/wincefuncs.h"
#endif

#include "typedefs.h"	/* for u32 */
#include "constants.h"	/* for MAXSENDS */

extern u32 lastaclch, aclch[18], lastscoch, scoch[10], abch, pbch;
extern bool ulplink[10];
extern u32 lastlap, lastuap, lastnap;
extern u32 hcthfce;
extern u16 bccmdseqno;
extern int showLMPcqddr, showLMPcqddr2, showLMPpower, showAcl, showSco, showGps;
extern int showAfh;
extern int showExpansions;
extern int showAscii, autoWlps;
extern int slurping, bdslurping;
extern int showRaw, csr_extensions;
extern int decode_eir;
extern int fmPrettyRSSI;

typedef enum
{
        RESTART_NONE,
        RESTART_NO_SCRIPT,
        RESTART_NEW_SCRIPT
} RESTARTMODE;


#define MAX_RESTART_FILENAME_LEN    128

extern RESTARTMODE                  restart_after_commands;
extern char                         restart_filename[MAX_RESTART_FILENAME_LEN+1];


/* SHM */
extern int  DecodeSCO,
            DecodeACL,
            showRawData,
            showHeader,
            showRetransmit,
            showl2cap,
            ignoreIPMulticasts,
            ignoreMiscMulticasts,
            ignoreSpam,
            bnepVerbose;

extern bool gBccmdResetFlowControlled,
            gRestartFlowControlled;


typedef enum bitpattern
{
	NOPATTERN,
	P0,
	P1,
	P10,
	P11110000,
	PRBS
} BITPATTERN;

typedef enum psopstate
{
	NONE,

	NONPRESENTATIONSIZE,

	PRESENTATIONREADSIZE,
	PRESENTATIONREADGET,

	PRESENTATIONWRITESIZE,
	PRESENTATIONWRITEGET,
	PRESENTATIONWRITESET
} PSOPSTATE;

typedef struct psopdata
{
	PSOPSTATE state;
	u16 seqno;
	u16 pskeyreal, pskeypresentation;
	u16 offset, len;
	u16 value;
} PSOPDATA;

u32 mtime (void);


/* 
	Capsense
*/
#define CAPSENSE_PAD0_MASK 0x01
/* The following offset values are taken from the definitions
   in BCCMDPDU_CAPSENSE_RT_PADS n bccmdpdu.h */
#define CAPSENSE_PADSEL_OFFSET  4
#define CAPSENSE_PADBORD_OFFSET 5		/* No. of pads on the board */
#define CAPSENSE_SHIELD_OFFSET  6		/* Pad 0 shield flag */
#define CAPSENSE_PADVAL_OFFSET  7		/* Start of capacitance values */

#endif /* GLOBALS_H */
