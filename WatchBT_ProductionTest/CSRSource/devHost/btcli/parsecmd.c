/*
 * Copyright (C) 2000-2006 Cambridge Silicon Radio Ltd.; all rights reserved.
 * Command line parsing, and command_complete event decoding.
 *
 * MODIFICATION HISTORY
 *	#153   01:mar:04 pws    B-2096: Support BCCMDVARID_REMOTE_SNIFF_LIMITS
 *	#159   18:may:04 pws    B-1717: Add RADIOTEST_CFG_PIO_*_MASK.
 *	#164   13:jul:04 pw04   B-3200: BCCMD to get RSSI.
 *	#165   21:jul:04 sk03   B-3349: Added eSCO test code.
 *	#166   23:jul:04 sk03   B-3349: Improvements to eSCO test code.
 *	#172   12:oct:04 ay02   B-4126: Added LC_RX_FRACS command for PER info
 *	#201   26:jan:06 pw04   B-11152: New HCI i/f for sniff sub-rating.
 *	#204   24:feb:06 pw04   B-12334: HCI i/f for SSR changed again.
 *	#213   12:apr:06 sv01   B-13427: Add support for 3 PCM and 2 I2S BCCMDs
 *	#214   17:oct:06 ab12   B-17599: Added support for enhanced flush 
 *
 * $Id: //depot/bc/bluesuite_2_4/devHost/btcli/parsecmd.c#3 $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !defined(_WINCE) && !defined(_WIN32_WCE)
#include <errno.h>
#include <assert.h>
#define ASSERT assert
#include <time.h> /* This pulls in the Posix.1b realtime stuff, if available */
#if defined (POSIX_NO_CLOCK_GETTIME) && !defined (BSD_NO_GETTIMEOFDAY)
#include <sys/time.h>	/* This pulls in the BSD stuff needed for timestamping with subsecond precision */
#include <unistd.h>	/* Appears to be needed under Linux; can't do much harm under other Un*xes */
#endif
#else
#include "wince/wincefuncs.h"
#endif

#include "print.h"
#include "btcli.h"
#include "vars.h"
#include "parselib.h"
#include "dictionaries.h"
#include "commandtable.h"
#include "constants.h"
#include "put.h"
#include "globals.h"
#include "host/bccmd/bccmdpdu.h"
#include "assertn.h"
#include "psbcallkeys.h"
#include "struct.h"
#include "psop_state.h"

#ifndef _WIN32
#include <unistd.h> /* For sleep () */
#else
#if !defined(_WINCE) && !defined(_WIN32_WCE)
#include "windows.h" /* For Sleep () */
#include <math.h> /* For pow () */
#include "mmsystem.h" /* For time*Period () */
#include <sys/timeb.h> /* For _ftime () */
#endif
#endif

#ifdef USE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

#ifdef MGRWIN32ALLOCDEBUG
#include "crtdbg.h"
#endif

#include "unicode/ichar.h"

#include "log_file_handler.h"


#define LMP_DEBUG_CMD (CHANNEL_ID_LMP_DEBUG | FRAGMENT_START | FRAGMENT_END)


/*
 * This file also contains the decoding functions for Command_Complete.
 * Moving them, though, would involve either making them all non-static,
 * or an extra table of some sort, because of the way the hciCommandTable
 * works.  Leave for a rainy day...  HERE
 */


u32 lastaclch, aclch[18], lastscoch, scoch[10], abch = 0xbac, pbch = 0xbec;
u8 lastlk[16] = { 0x38, 0x37, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31,
		  0x38, 0x37, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31 };
u8 lastpc[16] = { 0x31, 0x32, 0x33, 0x34, 0xff, 0xff, 0xff, 0xff,
		  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
   lastpclen = 4;
u32 lastlap, lastuap, lastnap;
bool ulplink[10];
int showLMPcqddr = false, showLMPcqddr2 = false, showLMPpower = false, showAcl = true, showSco = true, autoWlps = false, showAscii = false, slurping = false, bdslurping = false, showRaw = false, csr_extensions = true, decode_eir = true, showGps = false;
int showAfh = true;
int showExpansions = true;
int fmPrettyRSSI = false;
bool auto_rawlog = false;



RESTARTMODE restart_after_commands = RESTART_NONE;
char		restart_filename[MAX_RESTART_FILENAME_LEN+1];


#ifdef DONTFTPWITHL2CAPHEADERS
u32 aclsize = 0x80;	/* The payload size for sends */
#endif
static u16 voiceSetting;	/* The last wvs value */
static u16 manufacturerName = 0xffff;	/* The last local manufacturer name (sic) */
u32 hcthfce;			/* The last shcthfc fce value */
static bool quitFlag;
static size_t opindex;		/* The HCI opcode being parsed */
u16 bccmdseqno = 0;
u8 lastc[16];

u8 lastr[16];


/* SHM */

int DecodeACL = true, showRawData = false, showHeader = false;
int showRetransmit = false, showl2cap = false, ignoreIPMulticasts = false, ignoreMiscMulticasts = false, ignoreSpam = true, bnepVerbose = false;
bool gBccmdResetFlowControlled = false;
bool gRestartFlowControlled = false;
int DecodeSCO = false;


/* Sleep command for use on POSIX and WIN32 platforms
 */
void sleep_ms(int ms)
{
#ifndef _WIN32
#ifndef POSIX_NO_NANOSLEEP
	struct timespec ts;
	ts.tv_sec = ms / 1000;
	ts.tv_nsec = ms % 1000 * 1000000;
	nanosleep (&ts, NULL);
#else
	sleep ((ms + 999) / 1000);
#endif
#else
	Sleep (ms);
#endif
}

/*
 * Parsing of HCI commands and
 * decoding of corresponding Command_Complete events.
 */


static void nop (void)
{
	printTimestamp ();
	printlf ("nop: cannot be sent!\n");
}


static void i (void)
{
	u32 l = GIAC, il = 0x30, nr = 0;
	if (	getDone () ||
		(get (24, &l, iac_d) &&
		 get (8, &il, NULL) &&
		 get (8, &nr, NULL) &&
		 getDone ()))
	{
		putCmd (3);
		put (l);
		put (il);
		put (nr);
		putDone ();
		return;
	}
	printSyntaxStart ();
	printSyntaxContin ("(defaults: giac il:0x30 nr:0x00)");
	printSyntaxContin ("l:giac|liac il nr");
}


static void pim (void)
{
	u32 maxpl, minpl, l, il, nr;
	if (	get (16, &maxpl, NULL) &&
		get (16, &minpl, NULL) &&
		get (24, &l, iac_d) &&
		get (8, &il, NULL) &&
		get (8, &nr, NULL) &&
		getDone ())
	{
		putCmd (5);
		put (maxpl);
		put (minpl);
		put (l);
		put (il);
		put (nr);
		putDone ();
		return;
	}
	printSyntax ("maxpl minpl l:giac|liac il nr");
}


static void cc_common (u32 ars)
{
	u32 lap = lastlap, uap = lastuap, nap = lastnap, pt = ACL_ALL, psrm = PSRM_R1, psm = MANDATORY, co = 0, ars0 = ars;
	bool ok = false;
	if (getDone ())
	{
		ok = true;
	}
	else if (getba (&lap, &uap, &nap))
	{
		if (getDone ())
		{
			ok = true;
		}
		else if (get (16, &pt, packetType_d))
		{
			if (getDone ())
			{
				ok = true;
			}
			else if (get (8, &psrm, psrm_d))
			{
				if (getDone ())
				{
					ok = true;
				}
				else if (	get (8, &psm, psm_d) &&
						get (16, &co, NULL) &&
						get (8, &ars, ms_d) &&
						getDone ())
				{
					ok = true;
				}
			}
		}
	}
	if (ok)
	{
		lastlap = lap;
		lastuap = uap;
		lastnap = nap;
		putCmd (8);
		put (lap);
		put (uap);
		put (nap);
		put (pt);
		put (psrm);
		put (psm);
		put (co);
		put (ars);
		putDone ();
		return;
	}

	printSyntaxStart ();
	printSyntaxContin (ars0 == MASTER ? "(defaults: last d135 r1 mandatory co:0x0000 master)" : "(defaults: last d135 r1 mandatory co:0x0000 slave)");
	printSyntaxContin ("ba");
	printSyntaxContin ("ba pt:dm1|dh1|dm3|dh3|dm5|dh5|dm|dh|d1|d3|d5|d13|d35|d15|d135|dm1-|dh1-|dm3-|dh3-|dm5-|dh5-|dm-|dh-|d1-|d3-|d5-|d13-|d35-|d15-|d135-|2dh1|2dh3|2dh5|3dh1|3dh3|3dh5|mr1|mr3|mr5|mr");
	printSyntaxContin ("ba pt psrm:r0|r1|r2");
	printSyntaxContin ("ba pt psrm psm:m|o1|o2|o3 co ars:m|s");
}


static void cc (void)
{
	cc_common (MASTER);
}


static void ccs (void)
{
	cc_common (SLAVE);
}


static void d (void)
{
	u32 ch, r = UEC;
	bool ok = false;
	if (getDone ())
	{
		if (lastscoch != NOHANDLE)
		{
			ch = lastscoch;
			ok = true;
		}
		else if (lastaclch != NOHANDLE)
		{
			ch = lastaclch;
			ok = true;
		}
	}
	else if (	getsch (&ch) &&
			(getDone () ||
			 (get (8, &r, disconnectReason_d) &&
			  getDone ())))
	{
		ok = true;
	}
	if (ok)
	{
		putCmd (2);
		put (ch);
		put (r);
		putDone ();
		return;
	}

	printSyntaxStart ();
	printSyntaxContin ("(defaults: last user_ended_connection)");
	printSyntaxContin ("ch");
	printSyntaxContin ("ch r:uec|lr|atpo|af|urf|ukns");
}


static void asc (void)
{
	u32 ch, pt;
	if (	getch (&ch) &&
		get (16, &pt, packetType_d) &&
		getDone ())
	{
		putCmd (2);
		put (ch);
		put (pt);
		putDone ();
		return;
	}
	printSyntax ("ch pt:hv1|hv2|hv3|hv12|hv13|hv23|hv123");
}


static void ssc (void)
{
	u32 ch, tb, rb, ml, cf, re, pt;
	if (	getsch (&ch) &&
		get (32, &tb, NULL) &&
		get (32, &rb, NULL) &&
		get (16, &ml, NULL) &&
		get (16, &cf, NULL) &&
		get (8, &re, re_d) &&
		get (16, &pt, epacketType_d) &&
		getDone ())
	{
		putCmd (7);
		put (ch);
		put (tb);
		put (rb);
		put (ml);
		put (cf);
		put (re);
		put (pt);
		putDone ();
		return;
	}
	printSyntax ("ch tb rb ml cf re:n|pc|lq pt:hv1|hv2|hv3|ev3|ev4|ev5|hv|ev|all");
}


static void acr_common (u32 r)
{
	u32 lap = lastlap, uap = lastuap, nap = lastnap, r0 = r;
	if (	getDone () ||
		(getba (&lap, &uap, &nap) &&
		 get (8, &r, ms_d) &&
		 getDone ()))
	{
		putCmd (4);
		put (lap);
		put (uap);
		put (nap);
		put (r);
		putDone ();
		return;
	}
	printSyntaxStart ();
	printSyntaxContin (r0 == MASTER ? "(defaults: last master)" : "(defaults: last slave)");
	printSyntaxContin ("ba r:m|s");
}


static void acr (void)
{
	acr_common (SLAVE);
}


static void acrs (void)
{
	acr_common (MASTER);
}


static void ascr (void)
{
	u32 lap = lastlap, uap = lastuap, nap = lastnap, tb, rb, ml, cf, re, pt;
	if (	getba (&lap, &uap, &nap) &&
		get (32, &tb, NULL) && /* HERE dictionary? */
		get (32, &rb, NULL) && /* HERE dictionary? */
		get (16, &ml, NULL) && /* HERE dictionary? */
		get (16, &cf, NULL) &&
		get (8, &re, re_d) &&
		get (16, &pt, epacketType_d) &&
		getDone ())
	{
		putCmd (9);
		put (lap);
		put (uap);
		put (nap);
		put (tb);
		put (rb);
		put (ml);
		put (cf);
		put (re);
		put (pt);
		putDone ();
		return;
	}
	printSyntax ("ba tb rb ml cf re:n|pc|lq pt:hv1|hv2|hv3|ev3|ev4|ev5|hv|ev|all");
}


static void rcr (void)
{
	u32 lap = lastlap, uap = lastuap, nap = lastnap, r = SR;
	if (	getDone () ||
		(getba (&lap, &uap, &nap) &&
		 get (8, &r, rejectConnectionReason_d) &&
		 getDone ()))
	{
		putCmd (4);
		put (lap);
		put (uap);
		put (nap);
		put (r);
		putDone ();
		return;
	}
	printSyntaxStart ();
	printSyntaxContin ("(defaults: last security_reasons)");
	printSyntaxContin ("ba r:lr|sr|pd");
}


static void rscr (void)
{
	rcr ();
}


static void lkrr (void)
{
	u32 lap = lastlap, uap = lastuap, nap = lastnap;
	u8 lk[16];

	memcpy (lk, lastlk, sizeof (lk));
	if (	getDone () ||
		(getba (&lap, &uap, &nap) &&
		 getLongHex (lk, sizeof (lk)) &&
		 getDone ()))
	{
		memcpy (lastlk, lk, sizeof (lk));
		putCmd (3 + sizeof (lk));
		put (lap);
		put (uap);
		put (nap);
		putArray (lk, sizeof (lk));
		putDone ();
		return;
	}
	printSyntaxStart ();
	printSyntaxContin ("(defaults: last lastlk)");
	printSyntaxContin ("ba lk");
}


static void lkrnr (void)
{
	u32 lap = lastlap, uap = lastuap, nap = lastnap;
	if (	getDone () ||
		(getba (&lap, &uap, &nap) &&
		 getDone ()))
	{
		putCmd (3);
		put (lap);
		put (uap);
		put (nap);
		putDone ();
		return;
	}
	printSyntaxStart ();
	printSyntaxContin ("(default: last)");
	printSyntaxContin ("ba");
}


static void pcrr (void)
{
	u32 lap = lastlap, uap = lastuap, nap = lastnap, pinlen = lastpclen;
	u8 pin[16] = { 0x31, 0x32, 0x33, 0x34 };

	memcpy (pin, lastpc, sizeof (pin));
	if (	getDone () ||
		(getba (&lap, &uap, &nap) &&
		 ((pinlen = getString ((char *) pin, sizeof (pin))) != BADSIZE ||
		  (get (8, &pinlen, NULL) &&
		   getLongHex (pin, sizeof (pin)))) &&
		 getDone ()))
	{
		memcpy (lastpc, pin, sizeof (pin));
		lastpclen = (u8)pinlen;
		putCmd (4 + sizeof (pin));
		put (lap);
		put (uap);
		put (nap);
		put (pinlen);
		putArray (pin, sizeof (pin));
		putDone ();
		return;
	}
	printSyntaxStart ();
	printSyntaxContin ("(default: last lastpc)");
	printSyntaxContin ("ba \"pc\"");
	printSyntaxContin ("ba pcl pc");
}


static void pcrnr (void)
{
	u32 lap = lastlap, uap = lastuap, nap = lastnap;
	if (	getDone () ||
		(getba (&lap, &uap, &nap) &&
		 getDone ()))
	{
		putCmd (3);
		put (lap);
		put (uap);
		put (nap);
		putDone ();
		return;
	}
	printSyntaxStart ();
	printSyntaxContin ("(default: last)");
	printSyntaxContin ("ba");
}


static void ccpt (void)
{
	u32 ch, pt;
	if (	getsch (&ch) &&
		get (16, &pt, packetType_d) &&
		getDone ())
	{
		putCmd (2);
		put (ch);
		put (pt);
		putDone ();
		return;
	}
	printSyntax ("ch pt:dm1|dh1|dm3|dh3|dm5|dh5|dm|dh|d1|d3|d5|d13|d35|d15|d135|hv1|hv2|hv3|hv12|hv13|hv23|hv123");
}


static void sce (void)
{
	u32 ch, ee;
	if (	getch (&ch) &&
		get (8, &ee, offOn_d) &&
		getDone ())
	{
		putCmd (2);
		put (ch);
		put (ee);
		putDone ();
		return;
	}
	printSyntax ("ch ee:off|on");
}


static void mlk (void)
{
	u32 kf;
	if (	get (8, &kf, keyFlag_d) &&
		getDone ())
	{
		putCmd (1);
		put (kf);
		putDone ();
		return;
	}
	printSyntax ("kf:sp|t");
}


static void rnr (void)
{
	u32 lap = lastlap, uap = lastuap, nap = lastnap, psrm = PSRM_R1, psm = MANDATORY, co = 0;
	if (	getDone () ||
		(getba (&lap, &uap, &nap) &&
		 (getDone () ||
		  (get (8, &psrm, psrm_d) &&
		   get (8, &psm, psm_d) &&
		   get (16, &co, NULL) &&
		   getDone ()))))
	{
		putCmd (6);
		put (lap);
		put (uap);
		put (nap);
		put (psrm);
		put (psm);
		put (co);
		putDone ();
		return;
	}
	printSyntaxStart ();
	printSyntaxContin ("(defaults: last r1 mandatory co:0x0000)");
	printSyntaxContin ("ba");
	printSyntaxContin ("ba psrm:r0|r1|r2 psm:m|o1|o2|o3 co");
}


static void rref (void)
{
	u32 ch, pn;
	if (	getch (&ch) &&
		get (8, &pn, NULL) &&
		getDone ())
	{
		putCmd (2);
		put (ch);
		put (pn);
		putDone ();
		return;
	}
	printSyntax ("ch pn");
}


static void rlh (void)
{
	u32 ch;
	if (	getsch (&ch) &&
		getDone ())
	{
		putCmd (1);
		put (ch);
		putDone ();
		return;
	}
	printSyntax ("ch");
}


static void rlh_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 3);

	printch (pa[5]);
	printlf (" ");
	printByValue (NULL, pa[6], 8, "lh");
	printlf (" ");
	printByValue (NULL, pa[7], 32, "r");
}


static void rc (void)
{
	u32 ch, wc;
	if (	getch (&ch) &&
		get (8, &wc, wc_d) &&
		getDone ())
	{
		putCmd (2);
		put (ch);
		put (wc);
		putDone ();
		return;
	}
	printSyntax ("ch wc:l|r");
}


static void rc_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 3);

	printch (pa[5]);
	printlf (" ");
	printByValue (NULL, pa[6], 32, "c");
	printlf (" ");
	if (pa[7] == 0xffff) /* HERE dictionary? */
	{
		printlf ("unknown");
	}
	else
	{
		printByValue (NULL, pa[7], 16, "a");
	}
}


static void hm (void)
{
	u32 ch, hmmaxi, hmmini = (u32) -1;
	if (	getch (&ch) &&
		get (16, &hmmaxi, NULL) &&
		(getDone () ||
		 (get (16, &hmmini, NULL) &&
		  getDone ())))
	{
		putCmd (3);
		put (ch);
		put (hmmaxi);
		if (hmmini == (u32) -1)
		{
			hmmini = hmmaxi;
		}
		put (hmmini);
		putDone ();
		return;
	}
	printSyntax ("ch hmmaxi [hmmini]");
}


static void sm (void)
{
	u32 ch, smaxi, smini, sa, st = (u32) -1;
	if (	getch (&ch) &&
		get (16, &smaxi, NULL) &&
		get (16, &smini, NULL) &&
		get (16, &sa, NULL) &&
		(getDone () ||
		 (get (16, &st, NULL) &&
		  getDone ())))
	{
		putCmd (5);
		put (ch);
		put (smaxi);
		if (st == (u32) -1) /* Fortunately they're all 16-bit quantities! */
		{
			st = sa;
			sa = smini;
			smini = smaxi;
		}
		put (smini);
		put (sa);
		put (st);
		putDone ();
		return;
	}
	printSyntax ("ch smaxi [smini] sa st");
}


static void chpar (void)
{
	u32 ch = lastaclch;
	if (	(ch != NOHANDLE &&
		 getDone ()) ||
		(getch (&ch) &&
		 getDone ()))
	{
		putCmd (1);
		put (ch);
		putDone ();
		return;
	}
	printSyntax ("[ch]");
}


static void pm (void)
{
	u32 ch, bmaxi, bmini = (u32) -1;
	if (	getch (&ch) &&
		get (16, &bmaxi, NULL) &&
		(getDone () ||
		 (get (16, &bmini, NULL) &&
		  getDone ())))
	{
		putCmd (3);
		put (ch);
		put (bmaxi);
		if (bmini == (u32) -1)
		{
			bmini = bmaxi;
		}
		put (bmini);
		putDone ();
		return;
	}
	printSyntax ("ch bmaxi [bmini]");
}


static void qs (void)
{
	u32 ch, f, st, tr, pb, l, dv;
	if (	getch (&ch) &&
		get (8, &f, NULL) &&
		get (8, &st, serviceType_d) &&
		get (32, &tr, NULL) &&
		get (32, &pb, NULL) &&
		get (32, &l, NULL) &&
		get (32, &dv, NULL) &&
		getDone ())
	{
		putCmd (7);
		put (ch);
		put (f);
		put (st);
		put (tr);
		put (pb);
		put (l);
		put (dv);
		putDone ();
		return;
	}
	printSyntax ("ch f st:nt|be|g tr pb l dv");
}


static void fs (void)
{
	u32 ch, f, fd, st, tr, tbs, pb, al;
	if (	getch (&ch) &&
		get (8, &f, NULL) &&
		get (8, &fd, fd_d) &&
		get (8, &st, serviceType_d) &&
		get (32, &tr, NULL) &&
		get (32, &tbs, NULL) &&
		get (32, &pb, NULL) &&
		get (32, &al, NULL) &&
		getDone ())
	{
		putCmd (8);
		put (ch);
		put (f);
		put (fd);
		put (st);
		put (tr);
		put (tbs);
		put (pb);
		put (al);
		putDone ();
		return;
	}
	printSyntax ("ch f fd:o|i st:nt|be|g tr tbs pb al");
}

static void sssr (void)
{
	u32 ch, max_rl, min_rto, min_lto;
	if (getch (&ch) &&
		get (16, &max_rl, NULL) &&
		get (16, &min_rto, NULL) &&
		get (16, &min_lto, NULL) &&
		getDone ())
	{
		putCmd (4);
		put (ch);
		put (max_rl);
		put (min_rto);
		put (min_lto);
		putDone();
		return;
	}
	printSyntax ("ch max_rl min_rto min_lto");
}

static void sssr_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printch (pa[5]);
}

static void rd_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 2);

	printch (pa[5]);
	printlf (" ");
	printByValue (ms_d, pa[6], 8, "cr");
}


static void sr (void)
{
	u32 lap, uap, nap, r;
	if (	getba (&lap, &uap, &nap) &&
		get (8, &r, ms_d) &&
		getDone ())
	{
		putCmd (4);
		put (lap);
		put (uap);
		put (nap);
		put (r);
		putDone ();
		return;
	}
	printSyntax ("ba r:m|s");
}


static void rlps_cc (const PA pa)
{
	bool first = true;
	unsigned reserved;
#define SHOWFEATURE(is,txt)		\
	if (!first)			\
	{				\
		printlf ("|");		\
	}				\
	if (!(is))			\
	{				\
		printlf ("~");		\
	}				\
	printlf (txt);			\
	first = false;

	ASSERT (pa[0] == 2 + 2 + 1 + 2);

	printch (pa[5]);
	printlf (" ");
	printByValue (NULL, pa[6], 16, "lps");
	printlf ("(");
	SHOWFEATURE (pa[6] & 0x01, "mss")
	SHOWFEATURE (pa[6] & 0x02, "hm")
	SHOWFEATURE (pa[6] & 0x04, "sm")
	SHOWFEATURE (pa[6] & 0x08, "pm")
	SHOWFEATURE (pa[6] & 0x10, "scm")
	ASSERT16 (pa[6]);
	reserved = (unsigned) pa[6] & ~0x001fU;
	if (reserved != 0)
	{
		printlf ("|");
		printlf ("0x%04x", (unsigned) reserved);
	}
	printlf (")");
#undef SHOWFEATURE
}


static void wlps (void)
{
	u32 ch, lps;
	if (	getch (&ch) &&
		get (16, &lps, linkPolicySetting_d) &&
		getDone ())
	{
		putCmd (2);
		put (ch);
		put (lps);
		putDone ();
		return;
	}
	printSyntax ("ch lps:dall|emss|ehm|esm|epm|escm|eall|eall12");
}


static void wlps_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printch (pa[5]);
}


static void rdlps_cc (const PA pa)
{
	bool first = true;
	unsigned reserved;
#define SHOWFEATURE(is,txt)		\
	if (!first)			\
	{				\
		printlf ("|");		\
	}				\
	if (!(is))			\
	{				\
		printlf ("~");		\
	}				\
	printlf (txt);			\
	first = false;

	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printByValue (NULL, pa[5], 16, "dlps");
	printlf ("(");
	SHOWFEATURE (pa[5] & 0x01, "mss")
	SHOWFEATURE (pa[5] & 0x02, "hm")
	SHOWFEATURE (pa[5] & 0x04, "sm")
	SHOWFEATURE (pa[5] & 0x08, "pm")
	SHOWFEATURE (pa[5] & 0x10, "scm")
	ASSERT16 (pa[5]);
	reserved = (unsigned) pa[5] & ~0x001fU;
	if (reserved != 0)
	{
		printlf ("|");
		printlf ("0x%04x", (unsigned) reserved);
	}
	printlf (")");
#undef SHOWFEATURE
}


static void wdlps (void)
{
	u32 lps;
	if (	get (16, &lps, linkPolicySetting_d) &&
		getDone ())
	{
		putCmd (1);
		put (lps);
		putDone ();
		return;
	}
	printSyntax ("dlps:dall|emss|ehm|esm|epm|escm|eall|eall12");
}


static void ccc (void)
{
	u32 lap = lastlap, uap = lastuap, nap = lastnap;
	if (	getDone () ||
		(getba (&lap, &uap, &nap) &&
		 getDone ()))
	{
		putCmd (3);
		put (lap);
		put (uap);
		put (nap);
		putDone ();
		return;
	}
	printSyntaxStart ();
	printSyntaxContin ("(default: last)");
	printSyntaxContin ("ba");
}


static void rnrc (void)
{
	u32 lap = lastlap, uap = lastuap, nap = lastnap;
	if (	getDone () ||
		(getba (&lap, &uap, &nap) &&
		 getDone ()))
	{
		putCmd (3);
		put (lap);
		put (uap);
		put (nap);
		putDone ();
		return;
	}
	printSyntaxStart ();
	printSyntaxContin ("(default: last)");
	printSyntaxContin ("ba");
}


static void sem (void)
{
	u8 m[8];
	if (getLongHex (m, sizeof (m)))
	{
		putCmd (sizeof (m));
		putArray (m, sizeof (m));
		putDone ();
		return;
	}
	printSyntax ("m");
}


static void reinit (void)
{
    aclEngineReInit();
    fastpipeEngineReInit();
    scoEngineReInit();
    
	hcthfce = OFFON_OFF;
#ifdef DONTFTPWITHL2CAPHEADERS
	aclsize = 0x80;
#endif
	voiceSetting = 0x60;
	psop_clear_all();
}


void parseInit (void)
{
	quitFlag = false;
	reinit ();
}


static void r_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1);

	reinit ();
}


static void sef (void)
{
	u32 ft, fct, cod, codm, lap, uap, nap, aa;

	if (get (8, &ft, filterType_d))
	{
		switch (ft)
		{
		case 0:
			if (getDone ())
			{
				putCmd (1);
				put (ft);
				putDone ();
				return;
			}
			break;
		case 1:
			if (get (8, &fct, filterConditionType_d))
			{
				switch (fct)
				{
				case 0:
					if (getDone ())
					{
						putCmd (2);
						put (ft);
						put (fct);
						putDone ();
						return;
					}
					break;
				case 1:
					if (	get (24, &cod, NULL) &&
						get (24, &codm, NULL) &&
						getDone ())
					{
						putCmd (4);
						put (ft);
						put (fct);
						put (cod);
						put (codm);
						putDone ();
						return;
					}
					break;
				case 2:
					if (	getba (&lap, &uap, &nap) &&
						getDone ())
					{
						putCmd (5);
						put (ft);
						put (fct);
						put (lap);
						put (uap);
						put (nap);
						putDone ();
						return;
					}
					break;
				}
			}
			break;
		case 2:
			if (get (8, &fct, filterConditionType_d))
			{
				switch (fct)
				{
				case 0:
					if (	get (8, &aa, autoAccept_d) &&
						getDone ())
					{
						putCmd (3);
						put (ft);
						put (fct);
						put (aa);
						putDone ();
						return;
					}
					break;
				case 1:
					if (	get (24, &cod, NULL) &&
						get (24, &codm, NULL) &&
						get (8, &aa, autoAccept_d) &&
						getDone ())
					{
						putCmd (5);
						put (ft);
						put (fct);
						put (cod);
						put (codm);
						put (aa);
						putDone ();
						return;
					}
					break;
				case 2:
					if (	getba (&lap, &uap, &nap) &&
						get (8, &aa, autoAccept_d) &&
						getDone ())
					{
						putCmd (6);
						put (ft);
						put (fct);
						put (lap);
						put (uap);
						put (nap);
						put (aa);
						putDone ();
						return;
					}
					break;
				}
			}
			break;
		}
	}

	printSyntaxStart ();
	printSyntaxContin ("ft:clear");
	printSyntaxContin ("ft:ir fct:all");
	printSyntaxContin ("ft:ir fct:cod cod codm");
	printSyntaxContin ("ft:ir fct:ba ba");
	printSyntaxContin ("ft:cs fct:all aaf:m|a|s");
	printSyntaxContin ("ft:cs fct:cod cod codm aaf:m|a|s");
	printSyntaxContin ("ft:cs fct:ba ba aaf:m|a|s");
}


static void f_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printch (pa[5]);
}

static void ef (void)
{
	u32 ch, pkt_type;
	if (getch (&ch) &&
		get (8, &pkt_type, NULL) &&
		getDone ())
	{
		putCmd (2);
		put (ch);
		put (pkt_type);
		putDone();
		return;
	}
	printSyntax ("ch pkt_type");
}

static void ef_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printch (pa[5]);
}

static void rpint_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printByValue (pinType_d, pa[5], 8, "pt");
}


static void wpint (void)
{
	u32 pt;
	if (	get (8, &pt, pinType_d) &&
		getDone ())
	{
		putCmd (1);
		put (pt);
		putDone ();
		return;
	}
	printSyntax ("pt:v|f");
}


static void rslk (void)
{
	u32 lap = 0xffffff, uap = 0xff, nap = 0xffff, af = ONEORALL_ONE;
	bool ok = false;
	if (getDone ())
	{
		af = ONEORALL_ALL;
		ok = true;
	}
	else if (get (0, &af, all_d))
	{
		if (getDone ())
		{
			ok = true;
		}
	}
	else if (	getba (&lap, &uap, &nap) &&
			(getDone () ||
			 (get (8, &af, oneOrAll_d) &&
			  getDone ())))
	{
		ok = true;
	}
	if (ok)
	{
		putCmd (4);
		put (lap);
		put (uap);
		put (nap);
		put (af);
		putDone ();
		return;
	}
	printSyntaxStart ();
	printSyntaxContin ("[all]");
	printSyntaxContin ("ba (default: one)");
	printSyntaxContin ("ba raf:one|all");
}


static void rslk_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 2);

	printByValue (NULL, pa[5], 16, "mnk");
	printlf (" ");
	printByValue (NULL, pa[6], 16, "nkr");
}


static void wslk (void)
{
#define MAXWSLK ((255 - 1) / (6 + 16))
	u32 n, i, lap[MAXWSLK], uap[MAXWSLK], nap[MAXWSLK];
	u8 lk[MAXWSLK][16];
	if (get (8, &n, NULL))
	{
		if (n > MAXWSLK)
		{
			printTimestamp ();
			printlf ("wslk: nktw<=%i!\n", MAXWSLK); /* Can't be more or else won't fit in command packet */
			return;
		}
		for (i = 0; i < n; ++i)
		{
			if (!(	getba (&lap[i], &uap[i], &nap[i]) &&
				getLongHex (lk[i], sizeof (lk[i]))))
			{
				break;
			}
		}
		if (i == n && getDone ())
		{
			putCmd (1 + (3 + sizeof (lk[0])) * n);
			put (n);
			for (i = 0; i < n; ++i)
			{
				put (lap[i]);
				put (uap[i]);
				put (nap[i]);
				putArray (lk[i], sizeof (lk[i]));
			}
			putDone ();
			return;
		}
	}
	printSyntax ("nktw [ba lk]*");
}


static void wslk_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printByValue (NULL, pa[5], 8, "nkw");
}


static void dslk (void)
{
	u32 lap = 0xffffff, uap = 0xff, nap = 0xffff, af = ONEORALL_ONE;
	bool ok = false;
	if (get (0, &af, all_d))
	{
		if (getDone ())
		{
			ok = true;
		}
	}
	else if (	getba (&lap, &uap, &nap) &&
			(getDone () ||
			 (get (8, &af, oneOrAll_d) &&
			  getDone ())))
	{
		ok = true;
	}
	if (ok)
	{
		putCmd (4);
		put (lap);
		put (uap);
		put (nap);
		put (af);
		putDone ();
		return;
	}
	printSyntaxStart ();
	printSyntaxContin ("all");
	printSyntaxContin ("ba (default: one)");
	printSyntaxContin ("ba daf:one|all");
}


static void dslk_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printByValue (NULL, pa[5], 16, "nkd");
}


static void cln (void)
{
	size_t len;
	u8 name[248];
	memset (name, 0xff, sizeof (name));
	if (	(len = getString ((char *) name, sizeof (name))) != BADSIZE &&
		getDone ())
	{
		if (len != sizeof (name))
		{
			name[len] = NUL;
		}
		putCmd (sizeof (name));
		putArray (name, sizeof (name));
		putDone ();
		return;
	}
	printSyntax ("\"n\"");
}


static void rln_cc (const PA pa)
{
//	ASSERT (pa[0] == 2 + 2 + 1 + 248);

	printString (&pa[5], pa[0] - 2 - 2 - 1);

	if (pa[0] != 2 + 2 + 1 + 248)
	{
		printlf (" (short)");
	}
}


static void rcat_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printByValue (NULL, pa[5], 16, "cat");
}


static void wcat (void)
{
	u32 cat;
	if (	get (16, &cat, NULL) &&
		getDone ())
	{
		putCmd (1);
		put (cat);
		putDone ();
		return;
	}
	printSyntax ("cat");
}


static void rpt_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printByValue (NULL, pa[5], 16, "pt");
}


static void wpt (void)
{
	u32 pt;
	if (	get (16, &pt, NULL) &&
		getDone ())
	{
		putCmd (1);
		put (pt);
		putDone ();
		return;
	}
	printSyntax ("pt");
}


static void rse_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printByValue (scanEnable_d, pa[5], 8, "se");
}


static void wse (void)
{
	u32 se;
	if (	get (8, &se, scanEnable_d) &&
		getDone ())
	{
		putCmd (1);
		put (se);
		putDone ();
		return;
	}
	printSyntax ("se:n|i|p|ip");
}


static void rpsa_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 2);

	printByValue (NULL, pa[5], 16, "psi");
	printlf (" ");
	printByValue (NULL, pa[6], 16, "psw");
}


static void wpsa (void)
{
	u32 psi, psw;
	if (	get (16, &psi, NULL) &&
		get (16, &psw, NULL) &&
		getDone ())
	{
		putCmd (2);
		put (psi);
		put (psw);
		putDone ();
		return;
	}
	printSyntax ("psi psw");
}


static void risa_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 2);

	printByValue (NULL, pa[5], 16, "isi");
	printlf (" ");
	printByValue (NULL, pa[6], 16, "isw");
}


static void wisa (void)
{
	u32 isi, isw;
	if (	get (16, &isi, NULL) &&
		get (16, &isw, NULL) &&
		getDone ())
	{
		putCmd (2);
		put (isi);
		put (isw);
		putDone ();
		return;
	}
	printSyntax ("isi isw");
}


static void rae_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printByValue (disabledEnabled_d, pa[5], 8, "ae");
}


static void wae (void)
{
	u32 ae;
	if (	get (8, &ae, disabledEnabled_d) &&
		getDone ())
	{
		putCmd (1);
		put (ae);
		putDone ();
		return;
	}
	printSyntax ("ae:d|e");
}


static void rem_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printByValue (encryptionMode_d, pa[5], 8, "em");
}


static void wem (void)
{
	u32 em;
	if (	get (8, &em, encryptionMode_d) &&
		getDone ())
	{
		putCmd (1);
		put (em);
		putDone ();
		return;
	}
	printSyntax ("em:d|eptp|eboth");
}


static void rcod_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printByValue (NULL, pa[5], 24, "cod");
}


static void wcod (void)
{
	u32 cod;
	if (	get (24, &cod, NULL) &&
		getDone ())
	{
		putCmd (1);
		put (cod);
		putDone ();
		return;
	}
	printSyntax ("cod");
}


static void rvs_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printByValue (NULL, pa[5], 16, "vs");
}


static void wvs (void)
{
	u32 vs;
	if (	get (16, &vs, NULL) &&
		getDone ())
	{
		voiceSetting = (u16) vs;
		putCmd (1);
		put (vs);
		putDone ();
		return;
	}
	printSyntax ("vs");
}


static void raft_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 2);

	printch (pa[5]);
	printlf (" ");
	printByValue (NULL, pa[6], 16, "ft");
}


static void waft (void)
{
	u32 ch, ft;
	if (	getch (&ch) &&
		get (16, &ft, NULL) &&
		getDone ())
	{
		putCmd (2);
		put (ch);
		put (ft);
		putDone ();
		return;
	}
	printSyntax ("ch ft");
}


static void waft_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printch (pa[5]);
}


static void rnbr_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printByValue (NULL, pa[5], 8, "nbr");
}


static void wnbr (void)
{
	u32 nbr;
	if (	get (8, &nbr, NULL) &&
		getDone ())
	{
		putCmd (1);
		put (nbr);
		putDone ();
		return;
	}
	printSyntax ("nbr");
}


static void rhma_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printByValue (NULL, pa[5], 8, "hma");
}


static void whma (void)
{
	u32 hma;
	if (	get (8, &hma, NULL) &&
		getDone ())
	{
		putCmd (1);
		put (hma);
		putDone ();
		return;
	}
	printSyntax ("hma");
}


static void rtpl (void)
{
	u32 ch, t;
	if (	getch (&ch) &&
		get (8, &t, powerLevelType_d) &&
		getDone ())
	{
		putCmd (2);
		put (ch);
		put (t);
		putDone ();
		return;
	}
	printSyntax ("ch t:c|m");
}


static void rtpl_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 2);

	printch (pa[5]);
	printlf (" tpl:%i", (signed char) pa[6]);
}

static void retpl (void)
{
	u32 ch, t;
	if (	getch (&ch) &&
		get (8, &t, powerLevelType_d) &&
		getDone ())
	{
		putCmd (2);
		put (ch);
		put (t);
		putDone ();
		return;
	}
	printSyntax ("ch t:c|m");
}


static void retpl_cc (const PA pa)
{
        /* Verify length of the response (2+2+1 is header) */
        ASSERT (pa[0] == 2 + 2 + 1 + 4);

        printch (pa[5]);
        printlf (" etpl:%i,%i,%i", (signed char) pa[6], (signed char) pa[7], (signed char) pa[8]);
}



static void rsfce_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printByValue (disabledEnabled_d, pa[5], 8, "sfce");
}


static void wsfce (void)
{
	u32 sfce;
	if (	get (8, &sfce, disabledEnabled_d) &&
		getDone ())
	{
		putCmd (1);
		put (sfce);
		putDone ();
		return;
	}
	printSyntax ("sfce:d|e");
}


static void shcthfc (void)
{
	u32 fce;
	if (	get (8, &fce, offOn_d) &&
		getDone ())
	{
		if (fce == OFFON_OFF || fce == OFFON_ON)
		{
			hcthfce = fce;
		}
		putCmd (1);
		put (fce);
		putDone ();
		return;
	}
	printSyntax ("fce:off|on");
}


static void hbs (void)
{
	u32 al, sl, na, ns;
	if (	get (16, &al, NULL) &&
		get (8, &sl, NULL) &&
		get (16, &na, NULL) &&
		get (16, &ns, NULL) &&
		getDone ())
	{
		putCmd (4);
		put (al);
		put (sl);
		put (na);
		put (ns);
		putDone ();
		return;
	}
	printSyntax ("hadpl hsdpl htnadp htnsdp");
}


static void hnocp (void)
{
#define MAXHNOCP ((255 - 1) / (2 + 2))
	u32 n, i, ch[MAXHNOCP], hnocp[MAXHNOCP];
	if (get (8, &n, NULL))
	{
		if (n > MAXHNOCP)
		{
			printTimestamp ();
			printlf ("hnocp: noh<=%i!\n", MAXHNOCP); /* Can't be more or else won't fit in command packet */
			return;
		}
		for (i = 0; i < n; ++i)
		{
			if (!(	getch (&ch[i]) &&
				get (16, &hnocp[i], NULL)))
			{
				break;
			}
		}
		if (i == n && getDone ())
		{
			putCmd (1 + 2 * n);
			put (n);
			for (i = 0; i < n; ++i)
			{
				put (ch[i]);
				put (hnocp[i]);
			}
			putDone ();
			return;
		}
	}
	printSyntax ("noh [ch hnocp]*");
}


static void rlst_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 2);

	printch (pa[5]);
	printlf (" ");
	printByValue (NULL, pa[6], 16, "lst");
}


static void wlst (void)
{
	u32 ch, lst;
	if (	getch (&ch) &&
		get (16, &lst, NULL) &&
		getDone ())
	{
		putCmd (2);
		put (ch);
		put (lst);
		putDone ();
		return;
	}
	printSyntax ("ch lst");
}


static void wlst_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printch (pa[5]);
}


static void rnosi_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printByValue (NULL, pa[5], 8, "nsi");
}


static void rcil_cc (const PA pa)
{
	size_t i;

	ASSERT (pa[0] >= 2 + 2 + 1 + 1);
	ASSERT (pa[0] == 2 + 2 + 1 + 1 + pa[5]);

	printByValue (NULL, pa[5], 8, "nci");
	for (i = 0; i < pa[5]; ++i)
	{
		printlf (" ");
		printByValue (iac_d, pa[6 + i], 24, "il");
	}
}


static void wcil (void)
{
#define MAXWCIL ((255 - 1) / 3)
	u32 n, i, iac[MAXWCIL];
	if (get (8, &n, NULL))
	{
		if (n > MAXWCIL)
		{
			printTimestamp ();
			printlf ("wcil: nci<=%i!\n", MAXWCIL); /* Can't be more or else won't fit in command packet */
			return;
		}
		for (i = 0; i < n; ++i)
		{
			if (!get (24, &iac[i], iac_d))
			{
				break;
			}
		}
		if (i == n && getDone ())
		{
			putCmd (1 + n);
			put (n);
			for (i = 0; i < n; ++i)
			{
				put (iac[i]);
			}
			putDone ();
			return;
		}
	}
	printSyntax ("nci [il:giac|liac]*");
}


static void rpspm_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printByValue (pspm_d, pa[5], 8, "pspm");
}


static void wpspm (void)
{
	u32 pspm;
	if (	get (8, &pspm, pspm_d) &&
		getDone ())
	{
		putCmd (1);
		put (pspm);
		putDone ();
		return;
	}
	printSyntax ("pspm:p0|p1|p2");
}


static void rpsm_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printByValue (psm_d, pa[5], 8, "psm");
}


static void wpsm (void)
{
	u32 psm;
	if (	get (8, &psm, psm_d) &&
		getDone ())
	{
		putCmd (1);
		put (psm);
		putDone ();
		return;
	}
	printSyntax ("psm:m|o1|o2|o3");
}


static void sacc (void)
{
	static const u8 cm_all[10] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f };
	static const u8 cm_even[10] = { 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55 };
	static const u8 cm_odd[10] = { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x2a };
	u32 cmp;
	u8 cm[10];
	bool ok = false;
	if (	get (0, &cmp, afh_channel_classification_d) )
	{
		switch (cmp)
		{
		case SACC_ALL:
			memcpy (cm, cm_all, sizeof (cm));
			break;
		case SACC_EVEN:
			memcpy (cm, cm_even, sizeof (cm));
			break;
		case SACC_ODD:
			memcpy (cm, cm_odd, sizeof (cm));
			break;
		default:
			{
				const char *chansVar = findVar ("DOT11CHANNELS");
				int chans = chansVar ? (int) strtol (chansVar, NULL, 0) : 11;
				int i;
				memset (cm, 0, sizeof (cm));
				for (i = 0; i < 79; i++)
				{
					if (	( i < (i32) ((cmp - 2402) - chans )) ||
						( (i32) ((cmp - 2402) + chans) < i ))
						cm[i / 8] |= 1 << (i % 8);
				}
			}
		}
		ok = true;
	}
	else if (	getLongHex (cm, sizeof (cm)) )
	{
		ok = true;
	}
	if ( ok && getDone () )
	{
		putCmd (sizeof (cm));
		putArray (cm, sizeof (cm));
		putDone ();
		return;
	}
	printSyntax ("cm");
	printSyntaxStart ();
	printSyntaxContin ("cm:0x...");
	printSyntaxContin ("cm:all|even|odd");
	printSyntaxContin ("cm:c1|c2|...|c14");
	printlf ("(block DOT11CHANNELS channels either side of IEEE 802.11b centre frequency)\n");
}


static void rpst_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printByValue (st_d, pa[5], 8, "pst");
}


static void wpst (void)
{
	u32 pst;
	if (	get (8, &pst, st_d) &&
		getDone ())
	{
		putCmd (1);
		put (pst);
		putDone ();
		return;
	}
	printSyntax ("pst:s|i");
}


static void rist_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printByValue (st_d, pa[5], 8, "ist");
}


static void wist (void)
{
	u32 ist;
	if (	get (8, &ist, st_d) &&
		getDone ())
	{
		putCmd (1);
		put (ist);
		putDone ();
		return;
	}
	printSyntax ("ist:s|i");
}


static void rim_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printByValue (im_d, pa[5], 8, "im");
}


static void wim (void)
{
	u32 im;
	if (	get (8, &im, im_d) &&
		getDone ())
	{
		putCmd (1);
		put (im);
		putDone ();
		return;
	}
	printSyntax ("wim:s|r|e");
}


static void raccm_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printByValue (disabledEnabled_d, pa[5], 8, "ccm");
}


static void waccm (void)
{
	u32 ccm;
	if (	get (8, &ccm, disabledEnabled_d) &&
		getDone ())
	{
		putCmd (1);
		put (ccm);
		putDone ();
		return;
	}
	printSyntax ("ccm:d|e");
}


static void weird (void)
{
	size_t len = 0, datalen, len_rem;
	u32 fr, arg;
	u8 data[240];
	if (get(8, &fr, fecr_d))
	{
		memset (data, 0, sizeof (data));
		while ((len_rem = sizeof (data) - len) > 0)
		{
			if ((datalen = getLongHex (&data[len], len_rem /*sizeof (data) - len*/)))
			{
				len += datalen;
			}
			else if (get (8, &arg, NULL))
			{
				data[len++] = (u8) arg;
			}
			else
			{
				break;
			}
		}
		if (len != 0 && getDone ())
		{
			putCmd (sizeof (data) + 1);
			put (fr);
			putArray (data, sizeof (data));
			putDone ();
			return;
		}
	}
	printSyntax ("fr:r|n [octet|hexstring]+");
}


static void reird_cc (const PA pa)
{
	ASSERT (pa[0] > 2 + 2 + 1 + 1);
	printByValue (fecr_d, pa[5], 8, "fr");
        printEIRD(&pa[6],pa[0]-6);

	if (pa[0] != 2 + 2 + 1 + 1 + 240)
	{
		printlf (" (short)");
	}
}


static void rlvi_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 5);

	printByValue (hciVersion_d, pa[5], 8, "hv");
	printlf (" ");
	printByValue (NULL, pa[6], 16, "hr");
	printlf (" ");
	printByValue (lmpVersion_d, pa[7], 8, "lv");
	printlf (" ");
	printByValue (manufacturerName_d, pa[8], 16, "mn");
	manufacturerName = (u16)pa[8];
	printlf (" ");
	printByValue (NULL, pa[9], 16, "ls");
	if (pa[8] == 0xa /* CSR */ &&
	    lookupByValue (id_d, pa[9]))
	{
		printlf ("(");
		printByValue (id_d, pa[9], 16, NULL);
		printlf (")");
	}
}


static void rlsc_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 64);

	printCommands (&pa[5]);
}


static void rlsf_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 8);

	printFeaturesPageX (0, &pa[5], "lf");
}


static void rlef (void)
{
	u32 pn;
	if (	get (8, &pn, NULL) &&
		getDone ())
	{
		putCmd (1);
		put (pn);
		putDone ();
		return;
	}
	printSyntax ("pn");
}


static void rlef_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 2 + 8);

	printByValue (NULL, pa[5], 8, "pn");
	printlf (" ");
	printByValue (NULL, pa[6], 8, "mpn");
	printlf (" ");
	printFeaturesPageX (pa[5], &pa[7], "elf");
}

uint16 aclPacketLength;
uint8 syncDataPacketLength;
uint16 aclPacketNumber;
uint16 syncDataPacketNumber;

static void rbs_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 4);

#ifdef DONTFTPWITHL2CAPHEADERS
	aclsize = pa[5];
#endif
	printByValue (NULL, pa[5], 16, "hadpl");
    aclPacketLength = (uint16) pa[5];
	printlf (" ");
	printByValue (NULL, pa[6], 8, "hsdpl");
    syncDataPacketLength = (uint8) pa[6];
	printlf (" ");
	printByValue (NULL, pa[7], 16, "htnadp");
    aclPacketNumber = (uint16) pa[7];
	printlf (" ");
	printByValue (NULL, pa[8], 16, "htnsdp");
    syncDataPacketNumber = (uint16) pa[8];
}


static void rcc_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printByValue (countryCode_d, pa[5], 8, "cc");
}


static void ba_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 3);

	printba (pa[5], pa[6], pa[7]);
}


static void rfcc_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 2);

	printch (pa[5]);
	printlf (" ");
	printByValue (NULL, pa[6], 16, "fcc");
}


static void rstfcc_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printch (pa[5]);
}


static void glq_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 2);

	printch (pa[5]);
	printlf (" ");
	printByValue (NULL, pa[6], 8, "lq");
}


static void rr_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 2);

	printch (pa[5]);
	printlf (" r:%i", (signed char) pa[6]);
}


static void racm_cc (const PA pa)
{
	size_t i;
	ASSERT (pa[0] == 2 + 2 + 1 + 1 + 1 + 10);

	printch (pa[5]);
	printlf (" ");
	printByValue (disabledEnabled_d, pa[6], 8, "am");
	printlf (" ");
	printlf ("acm:0x");
	for (i = 10; i != 0; --i)
	{
		ASSERT8 (pa[i + 6]);
		printlf ("%02x", (unsigned) pa[i + 6]);
	}
	if (pa[4] == 0 /* success */ && pa[6] == 1 /* AFH_enabled */)
	{
		printlf ("(");
		for (i = 0; i < 79; ++i)
		{
			if (i % 8 == 0 && i != 0)
			{
				printlf ("-");
			}
			printlf ("%u", (pa[7 + i / 8] >> (i % 8)) & 1);
		}
		printlf (")");
	}
}

static void reks_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 2);

	printch (pa[5]);
	printlf (" ks:%u", pa[6]);
}

static void rlm_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printByValue (loopbackMode_d, pa[5], 8, "lm");
}


static void wlm (void)
{
	u32 lm;
	if (	get (8, &lm, loopbackMode_d) &&
		getDone ())
	{
		putCmd (1);
		put (lm);
		putDone ();
		return;
	}
	printSyntax ("lm:n|l|r");
}




static void raw (void)
{
	u32 arg;
	size_t datalen;
	u16 len = 0;
	u8 *pkt = malloc (3 + 255);

	ASSERT (pkt != NULL);
	while (len < 3 + 255)
	{
		if ((datalen = getLongHex (&pkt[len], 3 + 255 - len)))
		{
			len += datalen;
		}
		else if (get (8, &arg, NULL))
		{
			pkt[len++] = (u8) arg;
		}
		else
		{
			break;
		}
	}
	if (len != 0 && getDone ())
	{
		sendCmdRaw (pkt, len);
		return;
	}

	free (pkt);
	printSyntax ("[octet|hexstring]+");
}


static void nopar (void)
{
	if (getDone ())
	{
		putCmd (0);
		putDone ();
		return;
	}
	printSyntax ("");
}


static void vectorin (const char *name, void (*vector) (void))
{
	size_t i, n = getNumCommands ();

	/*
	 * This is non-trivial because the right opindex needs to be
	 * set for the syntax help to work.
	 */
	for (i = 0; i < n; ++i)
	{
		if (strcmp (getCommandAbbNam (i), name) == 0)
		{
			break;
		}
	}
	ASSERT (i != n);
	opindex = i;
	vector ();
}

static void psgett (void);
static void pssizee (void);
static void psnextt (void);
static void psnextall (void);
static void psmtt (void);
static void memget (void);
static void randget (void);
static void bufget (void);
static void bufset (void);
static void i2c (void);
static void e2gett (void);
static void appgett (void);
static void rbn (void);
static void get_l2cap_crch (void);
static void siflash (void);

static void bcget (void)
{
	u32 vi, ch, val;
	if (get (16, &vi, bcCmdVarID_d))
	{
		switch (vi)
		{
		default:
			if (getDone ())
			{
				PA pa = { 4, BCCMDPDU_GETREQ, 0, 0 };
				pa[2] = ++bccmdseqno;
				pa[3] = vi;
				sendBCCMD (pa);
				return;
			}
			break;

		case BCCMDVARID_LC_RX_FRACS:
			if (getDone())
			{
				PA pa = { 6, BCCMDPDU_GETREQ, 0, 0 };
				pa[2] = ++bccmdseqno;		// should this not be count?
				pa[3] = vi;
				pa[4] = 18;			// Max table size
				sendBCCMD(pa);
				return;
			}
			break;

		case BCCMDVARID_CRYPTKEYLEN:
		case BCCMDVARID_PICONET_INSTANT:
		case BCCMDVARID_ENABLE_AFH:
        case BCCMDVARID_RSSI_ACL:
            if (getch (&ch) && getDone ())
            {
                PA pa = { 5, BCCMDPDU_GETREQ, 0, 0, 0 };
                pa[2] = ++bccmdseqno;
                pa[3] = vi;
                pa[4] = ch;
                sendBCCMD (pa);
                return;
            }
            break;

		case BCCMDVARID_GET_CLR_EVT:
			if (get (16, &ch, evtCntID_d) && getDone ())
			{
				PA pa = { 5, BCCMDPDU_GETREQ, 0, 0, 0 };
				pa[2] = ++bccmdseqno;
				pa[3] = vi;
				pa[4] = ch;
				sendBCCMD (pa);
				return;
			}
			break;

		case BCCMDVARID_GET_NEXT_BUILDDEF:
			if (get (16, &ch, NULL) && getDone ())
			{
				PA pa = { 5, BCCMDPDU_GETREQ, 0, 0, 0 };
				pa[2] = ++bccmdseqno;
				pa[3] = vi;
				pa[4] = ch;
				sendBCCMD (pa);
				return;
			}
			break;

		case BCCMDVARID_REMOTE_SNIFF_LIMITS:
			if (getDone ())
			{
				PA pa = { 10, BCCMDPDU_GETREQ, 0, 0, 0, 0, 0, 0, 0, 0 };
				pa[2] = ++bccmdseqno;
				pa[3] = vi;
				sendBCCMD (pa);
				return;
			}
			break;

		case BCCMDVARID_LIMIT_EDR_POWER:
			if (getch (&ch) && getDone ())
			{
				PA pa = { 6, BCCMDPDU_GETREQ, 0, 0, 0, 0 };
				pa[2] = ++bccmdseqno;
				pa[3] = vi;
				pa[4] = ch;
				sendBCCMD(pa);
				return;
			}
			break;

		/* FM/RDS radio register read (except RDS register)*/
		case BCCMDVARID_FM_REG:
			if (get (8, &ch, fm_reg_d) && getDone ())
			{
				PA pa = { 9, BCCMDPDU_GETREQ, 0, 0, 0, 0, 0, 0, 0};
				pa[2] = ++bccmdseqno;
				pa[3] = vi;
				pa[4] = ch; /*Register sub-address*/
				/* Remaining fields are zero */
				sendBCCMD (pa);
				return;
			}
			break;

		/* FM/RDS radio read RDS data 
			NOTE - The calculation for length is provisional. 
			It may change when the final data packing is defined.*/
		case BCCMDVARID_FM_RDS:
			if (get (16, &ch, NULL) && getDone ())
			{
				u32 *pa; /* Relies on the type of PA being u32[], not nice */
				int length, count;
				length = 2 * ch + 5;
				if (length < 9)
					length = 9;
				pa = malloc(length * sizeof(*pa));
				/* PA pa = { 9, BCCMDPDU_GETREQ, 0, 0, 0, 0, 0, 0, 0}; */
				pa[0] = length;
				pa[1] = BCCMDPDU_GETREQ;
				pa[2] = ++bccmdseqno;
				pa[3] = vi;
				pa[4] = ch; /* Number of blocks to return */
				for (count = 5; count < length; count++)
					pa[count] = 0;
				sendBCCMD (pa);
				free (pa);
				return;
			}
			break;
            
		/* RDS radio register read */
		case BCCMDVARID_FMTX_REG:
			if (get (8, &ch, fmtx_reg_d) && getDone ())
			{
				PA pa = { 9, BCCMDPDU_GETREQ, 0, 0, 0, 0, 0, 0, 0};
				pa[2] = ++bccmdseqno;
				pa[3] = vi;
				pa[4] = ch; /*Register sub-address*/
				/* Remaining fields are zero */
				sendBCCMD (pa);
				return;
			}
			break;
            
		case BCCMDVARID_HQ_SCRAPING:
			if (get (16, &ch, NULL) && getDone ())
			{
				u32 *pa; /* Relies on the type of PA being u32[], not nice */
				int length, count;

				length = ch + 5;
				if (length < 9)
					length = 9;
				pa = malloc(length * sizeof(*pa));
				//PA pa = { 9, BCCMDPDU_GETREQ, 0, 0, 4, 1, 2, 3, 4 };
				pa[0] = length;
				pa[1] = BCCMDPDU_GETREQ;
				pa[2] = ++bccmdseqno;
				pa[3] = vi;
				pa[4] = ch;
				for (count = 5; count < length; count++)
					pa[count] = 0;
				sendBCCMD (pa);
				free (pa);
				return;
			}
			break;

		case BCCMDVARID_PS:
			vectorin ("psget", psgett);
			return;
			break;

		case BCCMDVARID_PSSIZE:
			vectorin ("pssize", pssizee);
			return;
			break;

		case BCCMDVARID_PSNEXT:
			vectorin ("psnext", psnextt);
			return;
			break;

		case BCCMDVARID_PSNEXT_ALL:
			vectorin ("psnextall", psnextall);
			return;
			break;

		case BCCMDVARID_MEMORY:
			vectorin ("memget", memget);
			return;
			break;

		case BCCMDVARID_RANDOM:
			vectorin ("randget", randget);
			return;
			break;

		case BCCMDVARID_BUFFER:
			vectorin ("bufget", bufget);
			return;
			break;

		case BCCMDVARID_I2C_TRANSFER:
			vectorin ("i2c", i2c);
			return;
			break;

		case BCCMDVARID_E2_DEVICE:
			vectorin ("e2get", e2gett);
			return;
			break;

		case BCCMDVARID_E2_APP_DATA:
			appgett();
			return;
			break;
		case BCCMDVARID_PS_MEMORY_TYPE:
			vectorin ("psmt", psmtt);
			return;
			break;

		case BCCMDVARID_L2CAP_CRC:
			vectorin ("get_l2cap_crc", get_l2cap_crch);
			return;
			break;

		case BCCMDVARID_BUILD_NAME:
			vectorin ("rbn", rbn);
			return;
			break;
		case BCCMDVARID_RECLAIM_RAM_RESERVE: 
                {
                    u32 type, value;
                    if (get (8, &type, NULL) &&
                        get (16, &value, NULL) &&
                        getDone ())
                    {
                        PA pa = { 6, BCCMDPDU_SETREQ, 0, 0, 0, 0};
                        pa[2] = ++bccmdseqno;
                        pa[3] = vi;
                        pa[4] = type;
                        pa[5] = value;
                        sendBCCMD (pa);
                        return;
                    }
                    printSyntax ("type(1:tokens) value");
                    return;
                }
                break;	
                case BCCMDVARID_COEX_INT_TEST:
			if (get (8, &ch, LwctMode_d) &&
                                get (8, &val, LwctValue_d) && getDone ())
			{
				PA pa = { 6, BCCMDPDU_GETREQ, 0, 0, 0, 0};
				pa[2] = ++bccmdseqno;
				pa[3] = vi;
				pa[4] = ch;
                                pa[5] = val;
				sendBCCMD(pa);
				return;
			}
			break;

                case BCCMDVARID_COEX_DUMP:
                        if (getDone ())
			{
			    PA pa = { 6, BCCMDPDU_GETREQ, 0, 0, 0, 0};
			    pa[2] = ++bccmdseqno;
			    pa[3] = vi;
			    sendBCCMD(pa);
			    return;
			}
			break;

                    case BCCMDVARID_FMTX_TEXT:
                        printlf("bcget fmtx_text not supported\n");
                        return;
                        break;
                    case BCCMDVARID_SCO_PARAMETERS:
			if (getsch (&ch) && getDone ())
			{
				PA pa = { 5, BCCMDPDU_GETREQ, 0, 0, 0 };
				pa[2] = ++bccmdseqno;
				pa[3] = vi;
				pa[4] = ch;
				sendBCCMD (pa);
				return;
			}
			break;


                    case  BCCMDVARID_COASTER_SV_STATE:
#define ONE_ENTRY_BCCMDVARID_COASTER_SV_STATE   0,0,0, 0,0,0, 0,0,0, 0  /* 10 entries, NOT words in msg */
#define SIZE_ENTRY_BCCMDVARID_COASTER_SV_STATE (10)
                        {
                            PA pa = { 4, BCCMDPDU_GETREQ, 0, 0,     /* max 4 entries: */
                                      ONE_ENTRY_BCCMDVARID_COASTER_SV_STATE, ONE_ENTRY_BCCMDVARID_COASTER_SV_STATE,
                                      ONE_ENTRY_BCCMDVARID_COASTER_SV_STATE, ONE_ENTRY_BCCMDVARID_COASTER_SV_STATE };
                            u32 startIndex, numEntries;

                            if (!get (8, &startIndex, NULL) ) {
                        BCCMDVARID_COASTER_SV_STATE_usage:
                                printSyntax("coaster_sv_state entriesToSkip(0-n) entriesToRead (1-4)");
                                break;
                            }
                            if (!get (8, &numEntries, NULL) )
                                goto BCCMDVARID_COASTER_SV_STATE_usage;

                            if ( numEntries <= 0 || numEntries > 4 || startIndex > 20)
                                goto BCCMDVARID_COASTER_SV_STATE_usage;

                            if ( !getDone() )
                                goto BCCMDVARID_COASTER_SV_STATE_usage;

                            pa[0] += numEntries * SIZE_ENTRY_BCCMDVARID_COASTER_SV_STATE;
                            pa[2] = ++bccmdseqno;
                            pa[3] = vi;
                            pa[4] = (startIndex << 8) | numEntries;
                            
                            sendBCCMD(pa);
                            return;
                        }
                        break;


		case BCCMDVARID_CHARGER_TRIMS:
			if (getDone())
			{
				PA pa = { 12, BCCMDPDU_GETREQ, 0, BCCMDVARID_CHARGER_TRIMS, 0, 0, 0, 0, 0, 0, 0, 0};
				pa[2] = ++bccmdseqno;		// Sequence Number
				sendBCCMD(pa);
				return;
			}
			break;

		case BCCMDVARID_CAPSENSE_RT_PADS:
			{
				u32 pad_sel;

				if(get(8, &pad_sel, NULL) && getDone())
				{
					uint32 * pa;
					int index;
					u16 pads = (u16)pad_sel;

					/* Now grab enough memory for the results 
						= sizeof data structure + 5 for PDU data */
					index = (sizeof(BCCMDPDU_CAPSENSE_RT_PADS)/sizeof(u16)) + 5;
					pa = malloc(index * sizeof(*pa));
					memset(pa, 0, (index * sizeof(*pa)));	/* Just in case */
					pa[0] = index;
					pa[1] = BCCMDPDU_GETREQ;
					pa[2] = ++bccmdseqno;
					pa[3] = BCCMDVARID_CAPSENSE_RT_PADS;
					pa[CAPSENSE_PADSEL_OFFSET]  = pad_sel;	/* Bit pattern */

					sendBCCMD(pa);
					free(pa);
					return;
				}
				printSyntax("pad bit pattern");
				return;
			}

        case BCCMDVARID_SIFLASH:
            vectorin ("siflash", siflash);
            return;
            break;

		}
	}
	printSyntaxStart ();
	printSyntaxContin ("vi:... (see bcfind)");
	printSyntaxContin ("vi:cryptkeylen|piconet_instant|enable_afh|rssi_acl ch");
	printSyntaxContin ("vi:get_clr_evt eci:...");
	printSyntaxContin ("vi:get_next_builddef id:...");
	printlf ("(also see psget/pssize/psnext/psnextall/psmt/memget/randget/bufget/i2c/e2get/get_l2cap_crc/rbn)\n");
}


static int bcvarnbits (u16 vi)
{
	vi &= ~BCCMDVARID_READWRITE;
	if (vi < BCCMDVARID_UINT8_START)
		return 0;
	if (vi < BCCMDVARID_UINT16_START)
		return 8;
	if (vi < BCCMDVARID_UINT32_START)
		return 16;
	if (vi < BCCMDVARID_SPECIAL_START)
		return 32;
	return 256;
}


static void pssett (void);
static void psclrr (void);
static void psclrsr (void);
static void psclralll (void);
static void memsett (void);
static void rt (void);
static void i2c (void);
static void e2sett (void);
static void appsett (void);
static void eafhh (void);
static void berthresht (void);
static void l2cap_crch (void);
static void emrh (void);
static void lapth (void);
static void rsl (void);
static void suaeset(void);
static void pc32 (void);
static void plj  (void);
static void prar (void);
static void dar  (void);
static void dac  (void);
static void epos (void);
static void wcit(void);
static void sgso (void );
static void sgsi (void );
static void scso (void );
static void scsi (void );
static void scfg (void );
static void sas (void );
static void scon (void );
static void sdis (void );
static void sts (void );
static void stc (void );
static void stca (void );
static void msa (void );
static void ess (void );
static void pcr (void );
static void psr (void );
static void pspf (void );
static void rawbcset (void );
static void rawbcget (void );
static void micbiasctrl (void);
static void micbiascfg (void);
static void ledcfg (void);

static void startop (void);
static void stopop (void);
static void rstop (void);
static void dop (void);
static void cop_c (void);
static void cop_p (void);
static void opmsg (void);


static void bcset (void)
{
	u32 vi, ch, arg;
	int nbits;

	if (get (16, &vi, bcCmdVarID_d))
	{
		switch (vi)
		{
		default:
			nbits = bcvarnbits ((u16) vi);
			if (	(nbits == 0 ||
				 get (nbits < 32 ? nbits : 32, &arg, NULL)) &&
				getDone ())
			{
				PA pa = { 5, BCCMDPDU_SETREQ, 0, 0, 0 };
				pa[2] = ++bccmdseqno;
				pa[3] = vi;
				if (nbits != 0)
				{
					pa[4] = arg;
				}
				else
				{
					pa[0] = 4;
				}
				sendBCCMD (pa);
				return;
			}
			break;
		case BCCMDVARID_WLAN_COEX_PRIORITY:
                        if (getch (&ch) && get (16, &arg, NULL) && getDone ())
                        {
                                PA pa = { 6, BCCMDPDU_SETREQ, 0, 0, 0, 0 };
                                pa[2] = ++bccmdseqno;
                                pa[3] = vi;
                                pa[4] = ch;
                                pa[5] = arg;
                                sendBCCMD (pa);
                                return;
                        }
                        break;                        	
		case BCCMDVARID_SNIFF_SUB_RATE:
			if (getch (&ch))
			{
				u32 args[4], i;
				for (i = 0; i < 4; i++)
				{
					if (!get(16, args+i, NULL))
						break;
				}
				if (i == 4 && getDone())
				{
					PA pa = { 9, BCCMDPDU_SETREQ, 0, 0, 0, 0, 0, 0, 0};
					pa[2] = ++bccmdseqno;
					pa[3] = vi;
					pa[4] = ch;
					pa[5] = args[0];
					pa[6] = args[1];
					pa[7] = args[2];
					pa[8] = args[3];
					sendBCCMD(pa);
					return;
				}
			}
			break;
			
		case BCCMDVARID_LIMIT_EDR_POWER:
			if (getch (&ch) && get (16, &arg, NULL) &&
				getDone ())
			{
				PA pa = { 6, BCCMDPDU_SETREQ, 0, 0, 0, 0 };
				pa[2] = ++bccmdseqno;
				pa[3] = vi;
				pa[4] = ch;
				pa[5] = arg;
				sendBCCMD (pa);
				return;
			}
			break;


		/* FM/RDS radio register write */
		case BCCMDVARID_FM_REG:
			if ( get (8, &ch, fm_reg_d))
			{
				if( get (16, &arg, NULL) && getDone())
				{
					PA pa = { 9, BCCMDPDU_SETREQ, 0, 0, 0, 0, 0, 0, 0 };
					pa[2] = ++bccmdseqno;
					pa[3] = vi;
					pa[4] = ch; /* Register sub-address */
					pa[5] = arg;
					/* Remaining fields are zero */
					sendBCCMD (pa);
					return;
				}
			}
			break;

		/* FMTX/RDS radio register write */
		case BCCMDVARID_FMTX_REG:
			if ( get (8, &ch, fmtx_reg_d))
			{
				if( get (16, &arg, NULL) && getDone())
				{
					PA pa = { 9, BCCMDPDU_SETREQ, 0, 0, 0, 0, 0, 0, 0 };
					pa[2] = ++bccmdseqno;
					pa[3] = vi;
					pa[4] = ch; /* Register sub-address */
					pa[5] = arg;
					/* Remaining fields are zero */
					sendBCCMD (pa);
					return;
				}
			}
			break;            
		case BCCMDVARID_FMTX_TEXT:
			{
			/* 
			 * This is going to get nasty.. The basic requirement 
			 * is to parse 'number number string number array' 
			 */
			uint16 type,strLen,realStrLen,repLen,realRepLen;
			size_t dynstrlen;

			/* Declare storage for strings */
			uint8 bytes[134];
			const uint8 * byteptr = &bytes[0];

			/* Declare temporary storage for strings */
			struct dynstr str;

			new_dynstr(&str, 0);

			/* Get the message type uint16*/
			get (8, &arg, NULL);
			type = (uint16) arg;

			/* Get string length as character pairs */
			get (8, &arg, NULL);
			strLen = (u16) arg;  

			realStrLen = 0;
			/* Get the string */ 
			if ((dynstrlen = getStringDynamic (&str)) != BADSIZE)
			{
				realStrLen = (uint16) dynstrlen;
				/* Handle "String" as input */
				memcpy(&bytes[0], str.data, realStrLen);
			}
			else
			{
				/* Handle 0x12 0x24 0x36 as input */
				/* note that with this method of input it's quite difficult to
				   give it an incorrect length */
				while (realStrLen < (strLen*2) )
				{
					size_t len;
					if ((len = getLongHex (&bytes[realStrLen], sizeof(bytes)-realStrLen)))
						realStrLen += len;
					else if (get (8, &arg, NULL))
						bytes[realStrLen++] = (u8) arg;
					else
						break;
				}
			}
			/* pad to even length */
			if ((realStrLen & 1) == 1)
			{
				bytes[realStrLen++] = 0xD;
			}

			/* Get repertroir length */
			get (8, &arg, NULL);
			repLen = (u16) arg;

			/* Clear the dynamic string */
			delete_dynstr(&str);
			new_dynstr(&str, 0);

			realRepLen = 0;
			/* Handle 0x12 0x24 0x36 as input */
			while (realRepLen < sizeof(bytes)-realStrLen)
			{
				size_t len;
				if ((len = getLongHex (&bytes[realStrLen+realRepLen], sizeof(bytes)-realStrLen-realRepLen)))
					realRepLen += len;
				else if (get (8, &arg, NULL))
					bytes[realStrLen + realRepLen++] = (u8) arg;
				else
					break;
			}
			/* We don't seem to be able to have an odd length for this. */
			if ((realRepLen & 1) == 1)
			{
				bytes[realStrLen + realRepLen++] = 0;
			}

			if (getDone())
			{
				size_t i,j,totalLen = 4 + 6 + realStrLen + realRepLen;
				uint32 *pa = malloc(sizeof(uint32)*totalLen);
				pa[0] = totalLen; /* In bytes! */
				pa[1] = BCCMDPDU_SETREQ;
				pa[2] = ++bccmdseqno;
				pa[3] = vi;
				pa[4] = (uint32) type;
				pa[5] = (uint32) strLen;
				pa[6] = (uint32) realStrLen;
				for (i=0;i<realStrLen;++i)
				{
					pa[7+i] = byteptr[i];
				}
				pa[7+i] = (uint32) repLen;
				pa[8+i] = (uint32) realRepLen;
				j = 9+i;
				for (i=0;i<realRepLen;++i)
				{
					pa[j+i] = byteptr[realStrLen + i];
				}
				sendBCCMD(pa);
				free(pa);
				delete_dynstr(&str);
				return;
			}
			else
			{
				delete_dynstr(&str);
			}

			}
			break;            
		case BCCMDVARID_PS:
			vectorin ("psset", pssett);
			return;
			break;

		case BCCMDVARID_PSCLR:
			vectorin ("psclr", psclrr);
			return;
			break;

		case BCCMDVARID_PSCLRS:
			vectorin ("psclrs", psclrsr);
			return;
			break;

		case BCCMDVARID_PSCLRALLS:
			vectorin ("psclrall", psclralll);
			return;
			break;

        case BCCMDVARID_PSCLRALL:
          	if (getDone ())
	        {
		        PA pa = { 5, BCCMDPDU_SETREQ, 0, 0, 0};
		        pa[2] = ++bccmdseqno;
		        pa[3] = BCCMDVARID_PSCLRALL;
		        sendBCCMD (pa);
		        return;
	        }
            break;
		case BCCMDVARID_MEMORY:
			vectorin ("memset", memsett);
			return;
			break;

		case BCCMDVARID_RADIOTEST:
			vectorin ("rt", rt);
			return;
			break;

		case BCCMDVARID_I2C_TRANSFER:
			vectorin ("i2c", i2c);
			return;
			break;

		case BCCMDVARID_E2_DEVICE:
			vectorin ("e2set", e2sett);
			return;
			break;

		case BCCMDVARID_E2_APP_DATA:
            appsett();
			return;
			break;

		case BCCMDVARID_ENABLE_AFH:
			vectorin ("eafh", eafhh);
			return;
			break;

		case BCCMDVARID_BER_THRESHOLD:
			vectorin ("berthresh", berthresht);
			return;
			break;

		case BCCMDVARID_L2CAP_CRC:
			vectorin ("l2cap_crc", l2cap_crch);
			return;
			break;

		case BCCMDVARID_ENABLE_MEDIUM_RATE:
			vectorin ("emr", emrh);
			return;
			break;

		case BCCMDVARID_LOCK_ACL_PACKET_TYPE:
			vectorin ("lapt", lapth);
			return;
			break;

		case BCCMDVARID_REMOTE_SNIFF_LIMITS:
			vectorin ("rsl", rsl);
			return;
			break;


		case BCCMDVARID_BUFFER:
			vectorin ("bufset", bufset);
			return; 
			break;
		case BCCMDVARID_PCM_CONFIG32:
			vectorin ("pc32", pc32);
			return;
			break;
		case BCCMDVARID_PCM_LOW_JITTER:
			vectorin ("plj", plj);
			return;
			break;
		case BCCMDVARID_PCM_RATE_AND_ROUTE:
			vectorin ("prar", prar);
			return;
			break;
		case BCCMDVARID_DIGITAL_AUDIO_RATE:
			vectorin ("dar", dar);
			return;
			break;
		case BCCMDVARID_DIGITAL_AUDIO_CONFIG:
			vectorin ("dac", dac);
			return;
			break;
		case BCCMDVARID_SHA256_DATA:
                     {
                        size_t dataLen;
                        uint8 bytes[64];
                        const uint8 * data = &bytes[0];
                        struct dynstr str;
                        new_dynstr(&str, 0);
                        if ((dataLen = getStringDynamic (&str)) != BADSIZE )
                        {
                            data = (u8 *) str.data;
                        }
                        else
                        {
                           dataLen = 0;
                           while (dataLen < 64)
                           {
                              size_t len;
                              if ((len = getLongHex (&bytes[dataLen], sizeof(bytes)-dataLen)))
                                 dataLen += len;
                              else if (get (8, &arg, NULL))
                                 bytes[dataLen++] = (u8) arg;
                              else
                                 break;
                           }
                        }
                        if (getDone())
                        {
                           size_t i,totalLen = 5 + dataLen;
                           uint32 *pa = malloc(sizeof(uint32)*totalLen);
                           pa[0] = totalLen;
                           pa[1] = BCCMDPDU_SETREQ;
                           pa[2] = ++bccmdseqno;
                           pa[3] = vi;
                           pa[4] = dataLen;
                           for (i=0;i<dataLen;++i)
                           {
                              pa[5+i] = data[i];
                           }
                           sendBCCMD(pa);
                           free(pa);
                           delete_dynstr(&str);
                           return;
                        }
                        else
                           delete_dynstr(&str);

                     }
                     break;
        case BCCMDVARID_ALLOCATE_RAM_RESERVE: 
                {
                    u32 type, value;
  
                    if (get (8, &type, NULL) &&
                        get (16, &value, NULL) &&
                        getDone ())
                    {
                        PA pa = { 6, BCCMDPDU_SETREQ, 0, 0, 0, 0};
                        pa[2] = ++bccmdseqno;
                        pa[3] = vi;
                        pa[4] = type;
                        pa[5] = value;
                        sendBCCMD (pa);
                        return;
                    }
                    printSyntax ("type(1:tokens) value");
                }
                break;
        case BCCMDVARID_FASTPIPE_ENABLE:
            {
                uint32 limit;
                if(get(32, &limit, NULL) && getDone())
                {
                    PA pa = {6, BCCMDPDU_SETREQ, 0, 0, 0, 0};
                    pa[2] = ++bccmdseqno;
                    pa[3] = vi;
                    pa[4] = limit;
                    sendBCCMD(pa);
                    return;
                }  
            }   
            break;

        case BCCMDVARID_BLE_ADV_FILT_ADD:
            {
                u32 arg, operation, ad_type, interval, pattern;
                u32 length = 0; /* generated from input */
                u32 len = 8;
                u32 pa[BCCMDPDU_MAXLEN - 2];
                char* rest = NULL;

                if (get (16, &operation, ulp_adv_filt_operations) &&
                    get (16, &ad_type, ulp_adv_filt_ad_type) &&
                    get (16, &interval, NULL))
                {
                    /*  get the rest of the line, we will parse it ourselves. */
                    rest = getRestOfLine();

                    /* check we have somthing otherwise print the command syntax */
                    if (rest != NULL)
                    {
                        /* if it starts with a quote treat as a string */
                        if (*rest == '\"')
                        {
                            for (length=1;length<strlen(rest)-1;length++,len++)
                            {
                                if (length < (BCCMDPDU_MAXLEN - 2))
                                {
                                    pa[len] = *(rest+length);
                                }
                                else 
                                {
                                    printf("Max PDU limit reached, limit is %d.\n",BCCMDPDU_MAXLEN - 2);
                                    break;
                                }
                            }
                            length--;
                        }
                        else if (0 < strlen(rest))
                        {
                            /*Assume we have been given raw data */
                            char* tok = strtok (rest, " ");

                            /* tokenise on spaces */
                            while (NULL != tok) {
                                /* if this isn't a number we will send 0 */
                                pa[len] = strtol (tok, NULL, 0);
                                length++;
                                len++;
                                
                                if (len >= BCCMDPDU_MAXLEN - 2)
                                {
                                    printf("Max PDU limit reached, limit is %d.\n",BCCMDPDU_MAXLEN - 2);
                                    break;
                                }

                                /* Get next token */
                                tok = strtok(NULL, " ");
                            } 
                        }

                        free(rest);

                        if (len == length + 8 && getDone ())
                        {
                            pa[0] = len;
                            pa[1] = BCCMDPDU_SETREQ;
                            pa[2] = ++bccmdseqno;
                            pa[3] = vi;
                            pa[4] = operation;
                            pa[5] = ad_type;
                            pa[6] = interval;
                            pa[7] = length;
                            sendBCCMD (pa);
                            return;
                        }
                    }
                }
                printSyntax ("ble_adv_filt_add type:or ad_type:flags|incomplete_16bit_uuids|complete_16bit_uuids|" 
                             "incomplete_32bit_uuids|complete_32bit_uuids|incomplete_128bit_uuids|"
                             "complete_128bit_uuids|incomplete_local_name|complete_local_name|"
                             "tx_power_level|class_of_device|simple_pairing_hash_c|simple_pairing_randomizer|"
                             "tk_value|security_manager_oob_flags|slave_connection_interval_range|"
                             "service_solicitation_uuids_16bit|service_solicitation_uuids_128bit|"
                             "service_data interval [pattern:<quoted string>|"
                             "<space separated list of integers>]");
            }
            return;
        case BCCMDVARID_FASTPIPE_CREATE:
	  {
                uint32 pipe_id, req_rx_c, des_rx_c, req_tx_c, des_tx_c, overhead_h, cap_rx_h;

                if(get(16, &pipe_id, NULL) &&
                   get(32, &overhead_h, NULL) &&
                   get(32, &cap_rx_h, NULL) &&
                   get(32, &req_tx_c, NULL) &&
                   get(32, &des_tx_c, NULL) &&
                   get(32, &req_rx_c, NULL) &&
                   get(32, &des_rx_c, NULL) &&
                   getDone())
                {
                    PA pa = {16, BCCMDPDU_SETREQ, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0};
                    pa[2] = ++bccmdseqno;
                    pa[3] = vi;
                    pa[4] = pipe_id;
                    pa[5] = overhead_h;
                    pa[6] = cap_rx_h;
                    pa[7] = req_tx_c;
                    pa[8] = des_tx_c;
                    pa[9] = req_rx_c;
                    pa[10] = des_rx_c;
                    sendBCCMD(pa);
                    return;
                }
            }   
            break;
        case BCCMDVARID_FASTPIPE_DESTROY:
            {
                uint32 pipe_id;
                if(get(16, &pipe_id, NULL) &&
                   getDone())
                {
                    PA pa = {6, BCCMDPDU_SETREQ, 0, 0, 0, 0};
                    pa[2] = ++bccmdseqno;
                    pa[3] = vi;
                    pa[4] = pipe_id;
                    btcli_enter_critical_section();
                    fastpipeEngineDestroyChannelSender(pipe_id);
                    btcli_leave_critical_section();
                    sendBCCMD(pa);
                    return;
                }  
            }  
			break;
        case BCCMDVARID_FASTPIPE_RESIZE:
            {
                uint32 delta;
                if (get(32, &delta, NULL) &&
                    getDone())
                {
                    PA pa = {7, BCCMDPDU_SETREQ, 0, 0, 0, 0, 0};
                    pa[2] = ++bccmdseqno;
                    pa[3] = vi;
                    pa[4] = delta;
                    sendBCCMD(pa);
                    return;
                }
            }
            break;
        case BCCMDVARID_COEX_INT_TEST:
            {
                uint32 mod, val;
                if(get(16, &mod, NULL) &&
                   get(16, &val, NULL) &&
                   getDone())
                {
                    PA pa = {6, BCCMDPDU_SETREQ, 0, 0, 0, 0};
                    pa[2] = ++bccmdseqno;
                    pa[3] = vi;
                    pa[4] = mod;
                    pa[5] = val;
                    sendBCCMD(pa);
                    return;
                }  
            }  
			break;

        case BCCMDVARID_GPS_START:
	    {
		PA pa = { 6, BCCMDPDU_SETREQ, 0, 0, 0, 0};
		u32 flags, delay;
		
		if (!get (16, &flags, NULL))
			flags = 0x50; /* 2.048MHz 1 bit */

		if (!get (16, &delay, NULL))
			delay = 1000;

		pa[2] = ++bccmdseqno;
		pa[3] = vi;
		pa[4] = flags;
		pa[5] = delay;
		sendBCCMD(pa);
		return;
	    }
	    
        case BCCMDVARID_GPS_STOP:
	    {
		PA pa = { 5, BCCMDPDU_SETREQ, 0, 0, 0};

		u32 flags;
		
		if (!get (16, &flags, NULL))
			flags = 0;

		pa[2] = ++bccmdseqno;
		pa[3] = vi;
		pa[4] = flags;
		sendBCCMD(pa);
		return;
	    }

        case BCCMDVARID_COASTER_START:
	    {
		PA pa = { 6, BCCMDPDU_SETREQ, 0, 0, 0, 0};
		u32 flags, delay;
		
		if (!get (16, &flags, NULL))
			flags = 0x51; /* 2.048MHz 2 bit */

		if (!get (16, &delay, NULL))
			delay = 1000;

		pa[2] = ++bccmdseqno;
		pa[3] = vi;
		pa[4] = flags;
		pa[5] = delay;
		sendBCCMD(pa);
		return;
	    }
	    
        case BCCMDVARID_COASTER_STOP:
	    {
		PA pa = { 5, BCCMDPDU_SETREQ, 0, 0, 0};

		u32 flags;
		
		if (!get (16, &flags, NULL))
			flags = 0;

		pa[2] = ++bccmdseqno;
		pa[3] = vi;
		pa[4] = flags;
		sendBCCMD(pa);
		return;
	    }

        case BCCMDVARID_COASTER_DEL_SV:
	    {
		PA pa = { 8, BCCMDPDU_SETREQ, 0, 0, 0, 0, 0, 0};

		u32 d0,d1,d2,d3,d4,d5,d6,d7;
		
		if (!get (16, &d0, NULL)) {
                    printSyntax("coaster_del_sv [up to 8 SVids (not PRNs)]");
                    break;
                }
		if (!get (8, &d1, NULL)) d1 = 0;
		if (!get (8, &d2, NULL)) d2 = 0;
		if (!get (8, &d3, NULL)) d3 = 0;
		if (!get (8, &d4, NULL)) d4 = 0;
		if (!get (8, &d5, NULL)) d5 = 0;
		if (!get (8, &d6, NULL)) d6 = 0;
		if (!get (8, &d7, NULL)) d7 = 0;

		pa[2] = ++bccmdseqno;
		pa[3] = vi;
		pa[4] = d0 | (d1 << 8);
		pa[5] = d2 | (d3 << 8);
		pa[6] = d4 | (d5 << 8);
		pa[7] = d6 | (d7 << 8);
		sendBCCMD(pa);
		return;
	    }
            
        case  BCCMDVARID_COASTER_INDICATIONS:
	    {
		PA pa = { 7, BCCMDPDU_SETREQ, 0, 0, 0, 0, 0};

		u32 d0,d1,d2;
		
                /* Bits 0,1 mask, set one of them only:
                 * bit 0: Periodic indications
                 * bit 1: Single indication close as we can after a specific timestamp.
                 * bit 2-4:
                 *       all zero: Just a time synch event, with a
                 *       timestamp in it at the earliest opportunity after
                 *       the requested time(s).
                 *       bit 2 set: packed SV info on up to 12 SVs.
                 *       bit 3 set: decoded databits+quality on up to 12 SVs.
                 *       bit 4 set: optflags is a bitset of SVids; only collect databits
                 *                  for those set.  Bit 4 clear and bit 3 set => collect
                 *                  databits for all SVs.
                 */
		if (!get (16, &d0, NULL) ) {
                    printSyntax("coaster_indications <flags> <timespec> [<opt>]");
                    printSyntax("flags: 5: periodic SV info; 6: one-shot SVs; 13: periodic SV+databits; 14: 1-shot SV+bits");
                    printSyntax("    bit 0: Periodic indications");
                    printSyntax("    bit 1: Single indication at specific timestamp.");
                    printSyntax("    bit 2-4:");
                    printSyntax("          bit 2 set: packed SV info on up to 12 SVs.");
                    printSyntax("          bit 3 set: decoded databits+quality on up to 12 SVs.");
                    printSyntax("          bit 4 set: optflags is a bitset of SVids to collect databits");
                    printSyntax("timespec: timestamp/us for indication, or period/us, depending on flags");
                    printSyntax("opt: bitset of SVids for which to collect databits - if so requested");
                    printSyntax("(to stop periodic indications use coaster_indications 2 0)");
                    break;
                }
		if (!get (32, &d1, NULL))
			d1 = 0;

		if (!get (16, &d2, NULL))
			d2 = 0;

                if ( d0 & 1 ) {
                    d1 <<= 16;      /* Shift count to first halfword */
                    d1 |= 0xffffu;  /* set 65535 repeats of the periodic indication */
                }

		pa[2] = ++bccmdseqno;
		pa[3] = vi;
		pa[4] = d0;
		pa[5] = d1;
		pa[6] = d2;
		sendBCCMD(pa);
		return;
	    }

        case  BCCMDVARID_COASTER_ADD_SV:
#define ONE_ENTRY_BCCMDVARID_COASTER_ADD_SV   0,0,0, 0,0,0, 0,0 /* 8 entries, NOT words in msg */
#define SIZE_ENTRY_BCCMDVARID_COASTER_ADD_SV (8)
	    {
		PA pa = { 4, BCCMDPDU_SETREQ, 0, 0,     /* max 4 entries: */
                          ONE_ENTRY_BCCMDVARID_COASTER_ADD_SV, ONE_ENTRY_BCCMDVARID_COASTER_ADD_SV,
                          ONE_ENTRY_BCCMDVARID_COASTER_ADD_SV, ONE_ENTRY_BCCMDVARID_COASTER_ADD_SV };
                u32 numEntries, i;


		if (!get (8, &numEntries, NULL) ) {
            BCCMDVARID_COASTER_ADD_SV_usage:
                    printSyntax("coaster_add_sv numSats(1-4) <satellite details> * 1-4");
                    printSyntax("<satellite details> is uSecs 80ths codeDopp codeShift carrDopp dataTransition satId PRN");
                    break;
                }
                if ( numEntries <= 0 || numEntries > 4 )
                    goto BCCMDVARID_COASTER_ADD_SV_usage;

                pa[0] += numEntries * SIZE_ENTRY_BCCMDVARID_COASTER_ADD_SV;

		pa[2] = ++bccmdseqno;
		pa[3] = vi;

                for ( i = 0; i < numEntries; i++ ) {
                    /* Timestamp */
                    u32         uSecs;       /* 0-max */
                    u32         uSec80ths;   /* 0-80 */
                    /* Info pertaining to that timestamp */
                    u32         codeDoppler;       /* S15.16, up to +/-5Hz */
                    u32         codeShift;         /* 16.16, 0 <= x < 1023.0 */
                    u32         carrierDoppler;    /* S15.16, up to +/-15kHz */
                    u32         dataTransition;    /* 0-20 ms to next dataBit boundary */
                    u32         satidPrn;          /* 8:8 handle to manage it:SV PRN signal id */
                    u32 tmp;

                    if ( !get ( 32, &uSecs,          NULL ) ) goto BCCMDVARID_COASTER_ADD_SV_usage;
                    if ( !get (  8, &uSec80ths,      NULL ) ) goto BCCMDVARID_COASTER_ADD_SV_usage;
                    if ( !get ( 32, &codeDoppler,    NULL ) ) goto BCCMDVARID_COASTER_ADD_SV_usage;
                    if ( !get ( 32, &codeShift,      NULL ) ) goto BCCMDVARID_COASTER_ADD_SV_usage;
                    if ( !get ( 32, &carrierDoppler, NULL ) ) goto BCCMDVARID_COASTER_ADD_SV_usage;
                    if ( !get (  8, &dataTransition, NULL ) ) goto BCCMDVARID_COASTER_ADD_SV_usage;
                    if ( !get (  8, &satidPrn,       NULL ) ) goto BCCMDVARID_COASTER_ADD_SV_usage;
                    if ( !get (  8, &tmp,            NULL ) ) goto BCCMDVARID_COASTER_ADD_SV_usage;

                    satidPrn <<= 8;
                    satidPrn |= tmp;

                    pa[ 4 + 0 + i * SIZE_ENTRY_BCCMDVARID_COASTER_ADD_SV ] = numEntries;
                    pa[ 4 + 1 + i * SIZE_ENTRY_BCCMDVARID_COASTER_ADD_SV ] = uSecs;
                    pa[ 4 + 2 + i * SIZE_ENTRY_BCCMDVARID_COASTER_ADD_SV ] = uSec80ths;
                    pa[ 4 + 3 + i * SIZE_ENTRY_BCCMDVARID_COASTER_ADD_SV ] = codeDoppler;   
                    pa[ 4 + 4 + i * SIZE_ENTRY_BCCMDVARID_COASTER_ADD_SV ] = codeShift;   
                    pa[ 4 + 5 + i * SIZE_ENTRY_BCCMDVARID_COASTER_ADD_SV ] = carrierDoppler;   
                    pa[ 4 + 6 + i * SIZE_ENTRY_BCCMDVARID_COASTER_ADD_SV ] = dataTransition;
                    pa[ 4 + 7 + i * SIZE_ENTRY_BCCMDVARID_COASTER_ADD_SV ] = satidPrn;
                }
                if ( !getDone() )
                    goto BCCMDVARID_COASTER_ADD_SV_usage;

		sendBCCMD(pa);
		return;
	    }

        case BCCMDVARID_EGPS_PULSE_CONFIG:
	    {
                PA pa = { 5, BCCMDPDU_SETREQ, 0, 0, 0 };
            u32 pio;
		
            if (!get (16, &pio, NULL))
                pio = 0;

            pa[2] = ++bccmdseqno;
            pa[3] = vi;
            pa[4] = pio;
            sendBCCMD(pa);
            return;
	    }
        
        case BCCMDVARID_EGPS_PULSE_INPUT_START:
	    {
                PA pa = { 5, BCCMDPDU_SETREQ, 0, 0, 0 };
            u32 pio, edge_mode;
		
            if (!get (8, &pio, NULL))
                pio = 0;
                
            if (!get (8, &edge_mode, NULL))
                edge_mode = 0;

            pa[2] = ++bccmdseqno;
            pa[3] = vi;
            pa[4] = pio;
            pa[5] = edge_mode;
            sendBCCMD(pa);
            return;
        }
        
        case BCCMDVARID_EGPS_PULSE_OUTPUT_START:
	    {
			vectorin ("epos", epos);
			return;
			break;
	    }
        
        case BCCMDVARID_EGPS_PULSE_STOP:
	    {
            PA pa = { 5, BCCMDPDU_SETREQ, 0, 0, 0 };
            u32 pio_stop, flags;
		
            if (!get (8, &pio_stop, NULL))
                pio_stop = 0xFF;
                
            if (!get (16, &flags, NULL))
                flags = 0;

            pa[2] = ++bccmdseqno;
            pa[3] = vi;
            pa[4] = pio_stop;
            pa[5] = flags;
            sendBCCMD(pa);
            return;
	    }

        case BCCMDVARID_CODEC_INPUT_GAIN:
        {
            uint32 gain_a, gain_b;
            if(get(16, &gain_a, NULL) &&
               get(16, &gain_b, NULL) &&
               getDone())
            {
                PA pa = {6, BCCMDPDU_SETREQ, 0, 0, 0, 0};
                pa[2] = ++bccmdseqno;
                pa[3] = vi;
                pa[4] = gain_a;
                pa[5] = gain_b;
                sendBCCMD(pa);
                return;
            }
            printSyntax ("codec_input_gain [gain_a] [gain_b]");
            break;
        }
        case BCCMDVARID_CODEC_OUTPUT_GAIN:
        {
            uint32 gain_a, gain_b;
            if(get(16, &gain_a, NULL) &&
               get(16, &gain_b, NULL) &&
               getDone())
            {
                PA pa = {6, BCCMDPDU_SETREQ, 0, 0, 0, 0};
                pa[2] = ++bccmdseqno;
                pa[3] = vi;
                pa[4] = gain_a;
                pa[5] = gain_b;
                sendBCCMD(pa);
                return;
            }
            printSyntax ("codec_output_gain [gain_a] [gain_b]");
            break;
        } 

        case BCCMDVARID_STREAM_GET_SOURCE:
            vectorin ("sgso", sgso);
            return;
        case BCCMDVARID_STREAM_GET_SINK:
            vectorin ("sgsi", sgsi);
            return;
        case BCCMDVARID_STREAM_CLOSE_SOURCE:
            vectorin ("scso", scso);
            return;
        case BCCMDVARID_STREAM_CLOSE_SINK:
            vectorin ("scsi", scsi);
            return;
        case BCCMDVARID_STREAM_CONFIGURE:
            vectorin ("scfg", scfg);
            return;
        case BCCMDVARID_STREAM_ALIAS_SINK:
            vectorin ("sas", sas);
            return;
        case BCCMDVARID_STREAM_CONNECT:
            vectorin ("scon", scon);
            return;
        case BCCMDVARID_STREAM_TRANSFORM_DISCONNECT:
            vectorin ("sdis", sdis);
            return;
        case BCCMDVARID_STREAM_TRANSACTION_START:
            vectorin ("sts", sts);
            return;
        case BCCMDVARID_STREAM_TRANSACTION_COMPLETE:
            vectorin ("stc", stc);
            return;
        case BCCMDVARID_STREAM_TRANSACTION_CANCEL:
            vectorin ("stca", stca);
            return;

	    case BCCMDVARID_MAP_SCO_AUDIO:
            vectorin ("msa", msa);
            return;
        case BCCMDVARID_ENABLE_SCO_STREAMS:
            vectorin ("ess", ess);
            return;

	    case BCCMDVARID_PCM_CLOCK_RATE:
            vectorin ("pcr", pcr);
            return;

	    case BCCMDVARID_PCM_SYNC_RATE:
            vectorin ("psr", psr);
            return;

	    case BCCMDVARID_PCM_SLOTS_PER_FRAME:
            vectorin ("pspf", pspf);
            return;

		case BCCMDVARID_MIC_BIAS_CTRL:
			vectorin ("micbiasctrl", micbiasctrl);
			return;

		case BCCMDVARID_MIC_BIAS_CONFIGURE:
			vectorin ("micbiascfg", micbiascfg);
			return;

		case BCCMDVARID_LED_CONFIG:
			vectorin ("ledcfg", ledcfg);
			return;

		case BCCMDVARID_PIO32_DIRECTION_MASK:
		case BCCMDVARID_PIO32:
		case BCCMDVARID_PIO32_PROTECT_MASK:
		case BCCMDVARID_PIO32_STRONG_BIAS:
		case BCCMDVARID_PIO32_MAP_PINS:  
			{
				u32 mask, bits;

				if (get (32, &mask, NULL) &&
					get (32, &bits, NULL) &&
					getDone ())
				{
					PA pa = { 7, BCCMDPDU_SETREQ, 0, vi,
							  mask, bits, 0 };
					pa[2] = ++bccmdseqno;
					sendBCCMD (pa);
					return;
				}
				printSyntax ("mask bits");
				return;
			}

        case BCCMDVARID_CLOCK_MODULATION_CONFIG:
        {
            u32 amount, rate, wibbling;
                                                                                                                
            if(get(16, &amount, rdfscan_d) 
               && get(16, &rate, NULL) 
               && get(16, &wibbling, NULL)
               && getDone())
            {
                                PA pa = {7, BCCMDPDU_SETREQ, 0, 0, 0, 0, 0};
                                pa[2] = ++bccmdseqno;
                                pa[3] = vi;
                                pa[4] = amount;
                                pa[5] = rate;
                                pa[6] = wibbling;
                                sendBCCMD(pa);
                                return;
            }
            printSyntax("clock_modulation_config amount rate wibbling");
            return;
        }
        case BCCMDVARID_BLE_RADIO_TEST :  
        {
            u32 command = 0; 
            u32 channel = 0;
            u32 length = 0;
            u32 payload = 0;
            PA pa = {8, BCCMDPDU_SETREQ, 0, 0, 0, 0, 0, 0};
            const char syntax[] = "ble_radio_test command:s|t|r channel length pktpayload:prbs9|prbs15|all0|all1|altnib0|altnib1|altbit0|altbit1";
            P0,	
            
            /*check if this is the stop command */
            get (16, &command, ulp_radio_test_d);
            switch (command) 
            {
                case BCCMDPDU_BLE_RADIO_TEST_STOP:
                {
                    /*this is the stop command */
                    if (!getDone ())
                    {
                        /*there is a problem here*/
                        printSyntax (syntax);
                        return;  
                    }
                    break;
                }
                case BCCMDPDU_BLE_RADIO_TEST_RECEIVER:
                {
                    /*this is a receive command so we only need the channel*/
                    if (!(get (8, &channel, NULL) && getDone ()))
                    {
                        /*there is a problem here*/
                        printSyntax (syntax);
                        return;
                    }
                    break;
                }
                case BCCMDPDU_BLE_RADIO_TEST_TRANSMITTER:
                {
                    /*this is the transmitter command so get the other parameters*/
                    if (!(get (8, &channel, NULL) &&
                    get (8, &length, NULL) &&
                    get (8, &payload, pktpayload_d) &&
                    getDone ()))
                    {
                        /*there is a problem here*/
                        printSyntax (syntax);
                        return;  
                    }
                    break;
                }
                default:
                    printSyntax (syntax);
                    return; 
            }
            /*pack up the message and send it on it's way*/
            pa[2] = ++bccmdseqno;
            pa[3] = vi;
            pa[4] = command;
            pa[5] = channel;
            pa[6] = length;
            pa[7] = payload;
            sendBCCMD (pa);
            return;
        }

        case BCCMDVARID_SIFLASH:
            vectorin ("siflash", siflash);
            return;

        case BCCMDVARID_START_OPERATOR:
            vectorin ("startop", startop);
            return;

        case BCCMDVARID_STOP_OPERATOR:
            vectorin ("stopop", stopop);
            return;

        case BCCMDVARID_RESET_OPERATOR:
            vectorin ("rstop", rstop);
            return;

        case BCCMDVARID_DESTROY_OPERATOR:
            vectorin ("dop", dop);
            return;

        case BCCMDVARID_CREATE_OPERATOR_C:
            vectorin ("cop_c", cop_c);
            return;

        case BCCMDVARID_CREATE_OPERATOR_P:
            vectorin ("cop_p", cop_p);
            return;

        case BCCMDVARID_OPERATOR_MESSAGE:
            vectorin ("opmsg", opmsg);
            return;

        case BCCMDVARID_SPI_LOCK_CUSTOMER_KEY:
			{
			u32 key1,key2,key3,key4;

                if(get(32, &key1, NULL) &&
                   get(32, &key2, NULL) &&
                   get(32, &key3, NULL) &&
                   get(32, &key4, NULL) &&
                   getDone())
                {
                    PA pa = {8, BCCMDPDU_SETREQ, 0, 0, 0, 0, 0, 0};
                    pa[2] = ++bccmdseqno;
                    pa[3] = vi;
                    pa[4] = key1;
                    pa[5] = key2;
                    pa[6] = key3;
                    pa[7] = key4;
                    sendBCCMD(pa);
                    return;
                }
  
            break;
			}
	} /* switch (vi) */
	} 
	printSyntax ("vi:... [arg] (see bcfind)");
	printlf ("(also see psset/pssets/psclr/psclrs/psclrall/memset/rt/i2c/e2set/eafh/berthresh/l2cap_crc/emr/ass/lapt/rsl/sha256_data)\n");
}

static void rawbccmd (uint16 req_type)
{
	u32 varid;
	uint16 *buf = NULL;
	size_t len = 0;
	bool syntax_error = false;

	if (get(16, &varid, bcCmdVarID_d))
	{
		while(!syntax_error && !getDone())
		{
			u32 val;
			u32 multiplicity;
			if(getNumericMultiple(16, &val, &multiplicity))
			{
				u32 i;
				uint16 *new_buf;

				new_buf = realloc(buf, (len + multiplicity)*sizeof(uint16));
				if(!new_buf)
				{
					free(buf);
					printlf("out of memory\n");
					return;
				}
				buf = new_buf;

				for(i = 0; i < multiplicity; ++i) buf[len+i] = (uint16) val;
				len += multiplicity;
			}
			else
			{
				syntax_error = true;
			}
		}
	}
	else
	{
		syntax_error = true;
	}

	if(!syntax_error)
	{
		sendRawBCCMD(req_type, (uint16) varid, ++bccmdseqno, buf, len);
	}
	else
	{
		printSyntax("varid [ { uint16 | uint16 * N } ... ]");
	}
	free(buf);
}

static void rawbcget (void)
{
	rawbccmd(BCCMDPDU_GETREQ);
}

static void rawbcset (void)
{
	rawbccmd(BCCMDPDU_SETREQ);
}

static void bcfind (void)
{
	struct dynstr buf;
	new_dynstr(&buf, 0);

	if (getDone() ||
		(getStringDynamicZ (&buf) != BADSIZE && 
		 getDone()))
	{
		if (!printMatchByName(bcCmdVarID_d, buf.data))
		{
			printlf ("not found\n");
		}
	} 
	else
	{
		printSyntaxStart ();
		printSyntaxContin ("(to list all bccmds)");
		printSyntaxContin ("\"substring\"");
	}

	delete_dynstr(&buf);
}


static void rt (void)
{
	u32 vi = BCCMDVARID_RADIOTEST, rt, a1 = 0, a2 = 0, a3 = 0, a4, a5;

	if (	get (16, &rt, radiotest_d) &&
		(getDone () ||
		 (get (16, &a1, NULL) &&
		  (getDone () ||
		   (get (16, &a2, NULL) &&
		    (getDone () ||
		     (get (16, &a3, NULL) &&
		      getDone ())))))))
	{
		PA pa = { 8, BCCMDPDU_SETREQ, 0, 0, 0, 0, 0, 0 };
		pa[2] = ++bccmdseqno;
		pa[3] = vi;
		pa[4] = rt;
		pa[5] = a1;
		pa[6] = a2;
		pa[7] = a3;
		sendBCCMD (pa);
		return;
	}
	else if (get (16, &a4, NULL) &&
		 getDone ())
	{
		/*
		 * required by RADIOTEST_PCM_TONE_STEREO
		 */
		PA pa = { 9, BCCMDPDU_SETREQ, 0, 0, 0, 0, 0, 0, 0 };
		pa[2] = ++bccmdseqno;
		pa[3] = vi;
		pa[4] = rt;
		pa[5] = a1;
		pa[6] = a2;
		pa[7] = a3;
		pa[8] = a4;
		sendBCCMD (pa);
		return;
	}
	else if (get (16, &a5, NULL) &&
		 getDone ())
	{
		/*
		 * required by RADIOTEST_CFG_HOPPING_SEQUENCE
		 * and RADIOTEST_CFG_PIO_CHANNEL_MASK.
		 */
		PA pa = { 10, BCCMDPDU_SETREQ, 0, 0, 0, 0, 0, 0, 0, 0 };
		pa[2] = ++bccmdseqno;
		pa[3] = vi;
		pa[4] = rt;
		pa[5] = a1;
		pa[6] = a2;
		pa[7] = a3;
		pa[8] = a4;
		pa[9] = a5;
		sendBCCMD (pa);
		return;
	}
	
	printSyntax ("rt:... [a1 [a2 [a3]]] (see rtfind)");
}


static void rtfind (void)
{
	struct dynstr buf;
	new_dynstr(&buf, 0);

	if (getDone() || 
		(getStringDynamicZ (&buf) != BADSIZE && 
		 getDone()))
	{
		if (!printMatchByName(radiotest_d, buf.data))
		{
			printlf ("not found\n");

		}
	}
	else
	{
		printSyntaxStart ();
		printSyntaxContin ("(to list all radiotests)");
		printSyntaxContin ("\"substring\"");
	}

	delete_dynstr(&buf);
}


static void saip(void)
{
	if (getDone ())
	{
		PA pa = { 26, TUNNEL, LMP_DEBUG_CMD, 0x18, 0x72, /* HERE demagicify 
								Seems unsupported now */
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		sendCmd (pa);
		return;
	}

	printSyntax ("saip");
}
static void ssip(void)
{
	if (getDone ())
	{
		PA pa = { 26, TUNNEL, LMP_DEBUG_CMD, 0x18, 0x73, /* HERE demagicify */
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		sendCmd (pa);
		return;
	}

	printSyntax ("ssip");
}
static void saop(void)
{
	if (getDone ())
	{
		PA pa = { 26, TUNNEL, LMP_DEBUG_CMD, 0x18, 0x74, /* HERE demagicify */
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		sendCmd (pa);
		return;
	}

	printSyntax ("saop");
}
static void ssop(void)
{
	if (getDone ())
	{
		PA pa = { 26, TUNNEL, LMP_DEBUG_CMD, 0x18, 0x75, /* HERE demagicify */
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		sendCmd (pa);
		return;
	}

	printSyntax ("ssop");
}

#include "presentation_data.c"

static const PresentationKey* findPresentationKey (pskey presentation)
{
	size_t i;

	for (i = 0; i < sizeof (presentationKeys) / sizeof (presentationKeys[0]); ++i)
	{
		if (presentationKeys[i].presentation == presentation)
		{
			return &presentationKeys[i];
		}
	}

	return NULL;
}

static void psgett (void)
{
	u32 vi = BCCMDVARID_PS, id, s = PS_STORES_DEFAULT, len = BCCMDPDU_MAXLEN - 8;
    
    bool ok = false;
    bool user_length_specified = false;
    bool user_length_ignored = false;

#define OFFSETOF_PS_DATA 7
	PA pa = { OFFSETOF_PS_DATA, BCCMDPDU_GETREQ, 0, 0, 0, 0, 0 };
	const PresentationKey *pk;

    PSOPDATA psopdata;

    if (get (16, &id, psKey_d))
    {
    	psopdata.state = NONPRESENTATIONSIZE;
    	pa[2] = psopdata.seqno = ++bccmdseqno;
	    pa[3] = vi;
	    pa[4] = psopdata.pskeyreal = (u16) id;
	    pa[6] = s;

        pk = findPresentationKey ((pskey) id);
		if (pk != NULL)
		{
			psopdata.state = PRESENTATIONREADSIZE;
			psopdata.pskeypresentation = psopdata.pskeyreal;
			psopdata.offset = pk->offset;
			psopdata.len = pk->len;
			pa[4] = psopdata.pskeyreal = pk->real;
            user_length_ignored = true;
			ASSERT (psopdata.offset / 16 == (psopdata.offset + psopdata.len - 1) / 16); // Presentation keys should not span u16s 
		}

        ok = true;
    }

    if (ok && 
        !getDone ())
    {
        if (get (16, &s, stores_d))
        {
            pa[6] = s;
        } 
        else
        {
            ok = false;
        }
    }

    if (ok &&
        !getDone ())
    {
        if (get (16, &len, NULL))
        {
            if (!user_length_ignored)
            {
                pa[5] = len;
                user_length_specified = true;
            }
        }
        else
        {
            ok = false;
        }
    }

    if (ok && getDone ())
    {
        if (user_length_specified)
        {
            pa[3] = BCCMDVARID_PS;
            pa[5] = len;
        }
        else
        {
            pa[3] = BCCMDVARID_PSSIZE;
            // only need to save state if it's multi-part.
            psop_add(&psopdata);
        }

   		sendBCCMD (pa);
    }
    else
    {
       	printSyntax ("id:... [s:d|t|i|f|r|if|ifr|ti|tif|tifr [len]] (see psfind)");
    }
}


static void pssett (void)
{
	u32 vi = BCCMDVARID_PS, arg, id, len;

	if (get (16, &id, psKey_d))
	{
		u32 len_alloc = BCCMDPDU_MAXLEN - 2; /* - 2 because DeviceController automatically inserts two extra words */
		u32 *pa = (u32 *) malloc (len_alloc * sizeof (u32));
		ASSERT (pa != NULL);
		len = OFFSETOF_PS_DATA;
		while (get (16, &arg, NULL))
		{
			if (len_alloc <= len)
			{
				len_alloc *= 2;
				pa = (u32 *) realloc (pa, len_alloc * sizeof (u32));
				ASSERT (pa != NULL);
			}
			pa[len++] = arg;
		}
		if (getDone ())
		{
			const PresentationKey *pk;

			pa[0] = len;
			pa[1] = BCCMDPDU_SETREQ;
			pa[2] = ++bccmdseqno;
			pa[3] = vi;
			pa[4] = (u16) id;
			pa[5] = len - OFFSETOF_PS_DATA;
			pa[6] = PS_STORES_DEFAULT;

			pk = findPresentationKey ((pskey) id);
			if (pk != NULL)
			{
				PSOPDATA psopdata;
				psopdata.seqno = bccmdseqno;
				psopdata.pskeyreal = (u16) id;
				psopdata.pskeypresentation = psopdata.pskeyreal;
				psopdata.offset = pk->offset;
				psopdata.len = pk->len;
				pa[1] = BCCMDPDU_GETREQ;
				pa[4] = psopdata.pskeyreal = pk->real;
				pa[5] = BCCMDPDU_MAXLEN - 8;
				psopdata.value = (u16) pa[7];
				ASSERT (psopdata.offset / 16 == (psopdata.offset + psopdata.len - 1) / 16); /* Presentation keys should not span u16s */
				if (len - OFFSETOF_PS_DATA < 1)
				{
					printTimestamp ();
					printlf ("psset: value required!\n");
					free (pa);
					return;
				}
				else if (len - OFFSETOF_PS_DATA > 1 || (psopdata.value >> psopdata.len) != 0)
				{
					printTimestamp ();
					printlf ("psset: too big!\n");
					free (pa);
					return;
				}
				psopdata.state = PRESENTATIONWRITEGET;
				psop_add(&psopdata);
			}

			sendBCCMD (pa);

			free (pa);
			return;
		}
		free (pa);
	}
	printSyntax ("id:... [word]* (see psfind)");
}


static void pssetss (void)
{
	u32 vi = BCCMDVARID_PS, arg, id, s = PS_STORES_DEFAULT, len;

	if (get (16, &id, psKey_d) &&
		(getDone () ||
		 get (16, &s, storesSet_d)))
	{
		u32 len_alloc = BCCMDPDU_MAXLEN - 2; /* - 2 because DeviceController automatically inserts two extra words */
		u32 *pa = (u32 *) malloc (len_alloc * sizeof (u32));
		ASSERT (pa != NULL);
		len = OFFSETOF_PS_DATA;
		while (get (16, &arg, NULL))
		{
			if (len_alloc <= len)
			{
				len_alloc *= 2;
				pa = (u32 *) realloc (pa, len_alloc * sizeof (u32));
				ASSERT (pa != NULL);
			}
			pa[len++] = arg;
		}
		if (getDone ())
		{
			const PresentationKey *pk;

			pa[0] = len;
			pa[1] = BCCMDPDU_SETREQ;
			pa[2] = ++bccmdseqno;
			pa[3] = vi;
			pa[4] = (u16) id;
			pa[5] = len - OFFSETOF_PS_DATA;
			pa[6] = s;

			pk = findPresentationKey ((pskey) id);
			if (pk != NULL)
			{
				PSOPDATA psopdata;
				psopdata.seqno = bccmdseqno;
				psopdata.pskeyreal = (u16) id;
				psopdata.pskeypresentation = psopdata.pskeyreal;
				psopdata.offset = pk->offset;
				psopdata.len = pk->len;
				pa[1] = BCCMDPDU_GETREQ;
				pa[4] = psopdata.pskeyreal = pk->real;
				pa[5] = BCCMDPDU_MAXLEN - 8;
				psopdata.value = (u16) pa[7];
				ASSERT (psopdata.offset / 16 == (psopdata.offset + psopdata.len - 1) / 16); /* Presentation keys should not span u16s */
				if (len - OFFSETOF_PS_DATA < 1)
				{
					printTimestamp ();
					printlf ("pssets: value required!\n");
					free (pa);
					return;
				}
				else if (len - OFFSETOF_PS_DATA > 1 || (psopdata.value >> psopdata.len) != 0)
				{
					printTimestamp ();
					printlf ("pssets: too big!\n");
					free (pa);
					return;
				}
				psopdata.state = PRESENTATIONWRITEGET;
				psop_add(&psopdata);
			}

			sendBCCMD (pa);

			free (pa);
			return;
		}
		free (pa);
	}
	printSyntax ("id:... [s:d|t|i|f [word]*] (see psfind)");
}


static void psclrr (void)
{
	u32 id, s = PS_STORES_DEFAULT;

	if (	get (16, &id, psKey_d))
	{
		if (getDone ())
		{
			PA pa = { 5, BCCMDPDU_SETREQ, 0, 0, 0 };
			pa[2] = ++bccmdseqno;
			pa[3] = BCCMDVARID_PSCLR;
			pa[4] = id;
			sendBCCMD (pa);
			return;
		}
		else if (	get (16, &s, storesClr_d) &&
				getDone ())
		{
			PA pa = { 6, BCCMDPDU_SETREQ, 0, 0, 0, 0 };
			pa[2] = ++bccmdseqno;
			pa[3] = BCCMDVARID_PSCLRS;
			pa[4] = id;
			pa[5] = s;
			sendBCCMD (pa);
			return;
		}
	}
	printSyntax ("id:... [s:d|t|i|f|if|ti|tif] (see psfind)");
}


static void psclrsr (void)
{
	u32 id, s;

	if ( get (16, &id, psKey_d) &&
		 get (16, &s, storesClr_d) &&
		 getDone ())
	{
		PA pa = { 6, BCCMDPDU_SETREQ, 0, 0, 0, 0 };
		pa[2] = ++bccmdseqno;
		pa[3] = BCCMDVARID_PSCLRS;
		pa[4] = id;
		pa[5] = s;
		sendBCCMD (pa);
		return;
	}
	printSyntax ("id:... s:d|t|i|f|if|ti|tif (see psfind)");
}


static void psclralll (void)
{
	u32 s = PS_STORES_DEFAULT;

	if (getDone ())
	{
		PA pa = { 5, BCCMDPDU_SETREQ, 0, 0, 0};
		pa[2] = ++bccmdseqno;
		pa[3] = BCCMDVARID_PSCLRALLS;
		sendBCCMD (pa);
		return;
	}
	else if (get (16, &s, storesClr_d) &&
		 getDone ())
	{
		PA pa = { 5, BCCMDPDU_SETREQ, 0, 0 , 0};
		pa[2] = ++bccmdseqno;
		pa[3] = BCCMDVARID_PSCLRALLS;
		pa[4] = s;
		sendBCCMD (pa);
		return;
	}
	printSyntax ("[s:d|t|i|f|if|ti|tif]");
}


static void pssizee (void)
{
	u32 vi = BCCMDVARID_PSSIZE, id, s = PS_STORES_DEFAULT;

	if (	get (16, &id, psKey_d) &&
		(getDone () ||
		 (get (16, &s, stores_d) &&
		  getDone ())))
	{
		PA pa = { 7, BCCMDPDU_GETREQ, 0, 0, 0, 0, 0 };
		pa[2] = ++bccmdseqno;
		pa[3] = vi;
		pa[4] = id;
		pa[6] = s;
		sendBCCMD (pa);
		return;
	}

	printSyntax ("id:... [s:d|t|i|f|r|if|ifr|ti|tif|tifr] (see psfind)");
}


static void psnextt (void)
{
	u32 vi = BCCMDVARID_PSNEXT, id, s = PS_STORES_DEFAULT;

	if (	get (16, &id, psKey_d) &&
		(getDone () ||
		 (get (16, &s, stores_d) &&
		  getDone ())))
	{
		PA pa = { 6, BCCMDPDU_GETREQ, 0, 0, 0, 0 };
		pa[2] = ++bccmdseqno;
		pa[3] = vi;
		pa[4] = id;
		pa[5] = s;
		sendBCCMD (pa);
		return;
	}

	printSyntax ("id:... [s:d|t|i|f|r|if|ifr|ti|tif|tifr] (see psfind)");
}


static void psnextall (void)
{
	u32 vi = BCCMDVARID_PSNEXT_ALL, id;

	if (	get (16, &id, psKey_d) &&
		getDone ())
	{
		PA pa = { 5, BCCMDPDU_GETREQ, 0, 0, 0 };
		pa[2] = ++bccmdseqno;
		pa[3] = vi;
		pa[4] = id;
		sendBCCMD (pa);
		return;
	}

	printSyntax ("id:... (see psfind)");
}


static void psmtt (void)
{
	u32 vi = BCCMDVARID_PS_MEMORY_TYPE, s = PS_STORES_DEFAULT;

	if (	getDone () ||
		(get (16, &s, storesSet_d) &&
		 getDone ()))
	{
		PA pa = { 7, BCCMDPDU_GETREQ, 0, 0, 0, 0, 0 };
		pa[2] = ++bccmdseqno;
		pa[3] = vi;
		pa[4] = s;
		sendBCCMD (pa);
		return;
	}

	printSyntax ("[s:d|t|i|f]");
}


static void psslurp (void)
{
	u32 vi = BCCMDVARID_PSNEXT, s = PS_STORES_I;

	if (	getDone () ||
		(get (16, &s, stores_d) &&
		 getDone ()))
	{
		PA pa = { 6, BCCMDPDU_GETREQ, 0, 0, 0, 0 };
		slurping = true;
		pa[2] = ++bccmdseqno;
		pa[3] = vi;
		pa[4] = 0;
		pa[5] = s;
		sendBCCMD (pa);
		return;
	}

	printSyntaxStart ();
	printSyntaxContin ("(default: i)");
	printSyntaxContin ("[s:d|t|i|f|r|if|ifr|ti|tif|tifr]");
}


static void bdslurp (void)
{
	u32 vi = BCCMDVARID_GET_NEXT_BUILDDEF;

	if (getDone ())
	{
		PA pa = { 5, BCCMDPDU_GETREQ, 0, 0, 0, 0 };
		bdslurping = true;
		pa[2] = ++bccmdseqno;
		pa[3] = vi;
		pa[4] = 0;
		sendBCCMD (pa);
		return;
	}

	printSyntax ("");
}


static void psfind (void)
{
	struct dynstr buf;
	new_dynstr(&buf, 0);

	if (getDone() || 
		(getStringDynamicZ (&buf) != BADSIZE && 
		 getDone()))
	{
		if (!printMatchByName(psKey_d, buf.data))
		{
			printlf ("not found\n");

		}
	}
	else
	{
		printSyntaxStart ();
		printSyntaxContin ("(to list all pskeys)");
		printSyntaxContin ("\"substring\"");
	}

	delete_dynstr(&buf);
}


/* needed in struct.c */
DictionaryPair *sym_d = NULL;
/* Allocated size of symbol table */
size_t sym_d_alloc;
/* Size of symbol table actually in use */
size_t sym_d_count;
/* Initial size for the symbol table */
#define SYM_D_ALLOC_INIT (20000)
/* Size by which to increment when full */
#define SYM_D_ALLOC_INC (5000)

/*
 * The two arguments are because I don't have the slightest idea
 * about this wretched ichar nonsense.  Feel free to fix.
 */
bool loadSymbolData (const ichar *sympath, const char *csympath)
{
	FILE *f;
	char line[256], *addr;
	bool indata = false;
	size_t i;

	if (csympath != NULL)
	{
		f = fopen (csympath, "r");
	}
	else
	{
		if (sympath == NULL)
		{
			sympath = igetenv (II ("SYMPATH"));
		}

		if (sympath == NULL)
		{
			return false;
		}

		f = ifopen (sympath, II ("r"));
	}
	if (f == NULL)
	{
		return false;
	}

	if (sym_d != NULL)
	{
		/* rereading: free entries but preserve table */
		for (i = 0; i < sym_d_count; i++)
			free (/* yuk */ (char *) sym_d[i].name);
		sym_d_count = 0;
	}
	else
	{
		ASSERT (sym_d_alloc == 0);
		ASSERT (sym_d_count == 0);
		sym_d_alloc = SYM_D_ALLOC_INIT;
		sym_d = (DictionaryPair *) malloc (sym_d_alloc * sizeof (*sym_d));
		ASSERT (sym_d != NULL);
	}

	for (;;)
	{
		if (fgets (line, sizeof (line), f) == NULL)
		{
			iprintlf (II("%s: corrupt!\n"), sympath);
			break;
		}
		ASSERT (line[strlen (line) - 1] == '\n' || line[strlen (line) - 1] == '\r');

		if (!indata)
		{
			if (*line == '.' && strncmp (line, ".DATA", strlen (".DATA")) == 0)
			{
				indata = true;
			}
			continue;
		}
		if (*line == '.' && strncmp (line, ".END", strlen (".END")) == 0)
		{
			break;
		}

		addr = strstr (line, ", ");
		ASSERT (addr != NULL);
		*addr = NUL;
		addr += strlen (", ");

		/* leave space for null entry */
		if (sym_d_count >= sym_d_alloc-1)
		{
			sym_d_alloc += SYM_D_ALLOC_INC;
			sym_d = realloc (sym_d, sym_d_alloc * sizeof (*sym_d));
			ASSERT (sym_d != NULL);
		}
		sym_d[sym_d_count].name = malloc (strlen (line) + 1);
		ASSERT (sym_d[sym_d_count].name != NULL);
		strcpy ((char *) sym_d[sym_d_count].name,
			*line == '$' ? line + 1 : line);
		* (u32 *) &sym_d[sym_d_count].value =
			strtoul (addr, &addr, 16);
		ASSERT (*addr == '\n' || *addr == '\r');
		++sym_d_count;
	}

	/* note last entry is not included in sym_d_count */
	sym_d[sym_d_count].name = NULL;
	* (u32 *) &sym_d[sym_d_count].value = 0;

	fclose (f);

	return true;
}


static void memget (void)
{
	u32 vi = BCCMDVARID_MEMORY, addr, len = 1;
	if (	get (16, &addr, sym_d) &&
		(getDone () ||
		 (get (16, &len, NULL) &&
		  getDone ())))
	{
		if (len > BCCMDPDU_MAXLEN - 8)
		{
			printTimestamp ();
			printlf ("memget: len<=%i!\n", BCCMDPDU_MAXLEN - 8); /* Can't be more or else won't fit in bccmd pdu */
		}
		else
		{
			PA pa = { 6, BCCMDPDU_GETREQ, 0, 0, 0, 0 };
			pa[2] = ++bccmdseqno;
			pa[3] = vi;
			pa[4] = addr;
			pa[5] = len;
			sendBCCMD (pa);
		}
		return;
	}
	printSyntax ("addr:... [len] (see memfind)");
}


static void randget (void)
{
	u32 vi = BCCMDVARID_RANDOM, len = 1;
	u32 type = 0 ;

	if (	get (16, &len, NULL) &&
		(getDone () ||
		 (get (16, &type, NULL) &&
		  getDone ())))
	{
		if (len > BCCMDPDU_MAXLEN - 8)
		{
			printTimestamp ();
			printlf ("randget: len<=%i!\n", BCCMDPDU_MAXLEN - 8); /* Can't be more or else won't fit in bccmd pdu */
		}
		else
		{
			PA pa = { 6, BCCMDPDU_GETREQ, 0, 0, 0, 0 };
			pa[2] = ++bccmdseqno;
			pa[3] = vi;
			pa[4] = len;
			pa[5] = type;
			sendBCCMD (pa);
		}
		return;
	}
	printSyntax ("len type.... ");  // output if error on input command line
}

static void memsett (void)
{
	u32 vi = BCCMDVARID_MEMORY, arg, addr, len;

	if (get (16, &addr, sym_d))
	{
		u32 pa[BCCMDPDU_MAXLEN - 2] = { 0, BCCMDPDU_SETREQ }; /* - 2 because DeviceController automatically inserts two extra words */
		len = 6;
		while (len != sizeof (pa) / sizeof (pa[0]) && get (16, &arg, NULL))
		{
			pa[len++] = arg;
		}
		if (getDone ())
		{
			pa[0] = len;
			pa[2] = ++bccmdseqno;
			pa[3] = vi;
			pa[4] = addr;
			pa[5] = len - 6;
			sendBCCMD (pa);
			return;
		}
	}
	printSyntax ("addr:... [word]* (see memfind)");
}


static void memsett32 (void)
{
	u32 vi = BCCMDVARID_MEMORY, arg, addr, len;

	if (get (16, &addr, sym_d))
	{
		u32 pa[BCCMDPDU_MAXLEN - 2] = { 0, BCCMDPDU_SETREQ }; /* - 2 because DeviceController automatically inserts two extra words */
		len = 6;
		while (len + 1 < sizeof (pa) / sizeof (pa[0]) && get (32, &arg, NULL))
		{
			pa[len++] = arg >> 16;
			pa[len++] = arg & 0xffff;
		}
		if (getDone ())
		{
			pa[0] = len;
			pa[2] = ++bccmdseqno;
			pa[3] = vi;
			pa[4] = addr;
			pa[5] = len - 6;
			sendBCCMD (pa);
			return;
		}
	}
	printSyntax ("addr:... [dword]* (see memfind)");
}


static void memfind (void)
{
	struct dynstr buf;
	new_dynstr(&buf, 0);

	if (sym_d == NULL)
	{
		printlf ("symbol table not loaded\n");
	}
	else if (getDone() ||
			 (getStringDynamicZ (&buf) != BADSIZE && 
			  getDone()))
	{
		if (!printMatchByName(sym_d, buf.data))
		{
			printlf ("not found\n");

		}
	}
	else
	{
	free(buf.data);
	buf.data = NULL;	

		printSyntaxStart ();
		printSyntaxContin ("(to list all symbols)");
		printSyntaxContin ("\"substring\"");
	}

	delete_dynstr(&buf);
}


static void bufset (void) 
{
    u32 var_id = BCCMDVARID_BUFFER;
    u32 handle, start, data_len; 
    struct dynstr data_str;

    if( get(16, &handle, NULL) &&
        get(16, &start, NULL) ) 
    {
        /* Now get the data to we want to fill the buffer with */
        new_dynstr(&data_str, 0);
        data_len = getStringDynamic (&data_str);

        if( getDone() && data_len != BADSIZE ) 
        {
            size_t i;
            size_t total_len = 7 + data_len;
            uint32 *pa=malloc(sizeof(uint32)*total_len);
            
            if(data_len > BCCMDPDU_MAXLEN-7) {
		total_len = BCCMDPDU_MAXLEN;
		data_len  = BCCMDPDU_MAXLEN-7;
		printlf("Will truncate string to a length of %d\n",BCCMDPDU_MAXLEN-7);
            }
            pa[0] = total_len;
            pa[1] = BCCMDPDU_SETREQ;
            pa[2] = ++bccmdseqno;
            pa[3] = var_id;
            pa[4] = handle;
            pa[5] = start;
            pa[6] = data_len;

            for (i=0;(i<data_len);i++) pa[i+7]=data_str.data[i];
            sendBCCMD(pa);
            free(pa);
        } 
        else 
        {
            printlf ("bufset wrong length\n");
        }
        delete_dynstr(&data_str);
        return;
    } 
    printSyntax("handle start data\n");

}


static void bufget (void)
{
	u32 vi = BCCMDVARID_BUFFER, handle, start, len;
	if (	get (16, &handle, NULL) &&
		get (16, &start, NULL) &&
		get (16, &len, NULL) &&
		getDone ())
	{
		if (len > BCCMDPDU_MAXLEN - 8)
		{
			printTimestamp ();
			printlf ("bufget: len<=%i!\n", BCCMDPDU_MAXLEN - 8); /* Can't be more or else won't fit in bccmd pdu */
		}
		else
		{
			PA pa = { 7, BCCMDPDU_GETREQ, 0, 0, 0, 0, 0 };
			pa[2] = ++bccmdseqno;
			pa[3] = vi;
			pa[4] = handle;
			pa[5] = start;
			pa[6] = len;
			sendBCCMD (pa);
		}
		return;
	}
	printSyntax ("handle start len");
}


static void loadsym (void)
{
	struct dynstr buf;
	new_dynstr(&buf, 0);
	
	if (getStringDynamicZ (&buf) != BADSIZE && 
		getDone ())
	{
		if (!loadSymbolData (NULL, buf.data))
		{
			printlf ("symbol table not found\n");
		}
		else
		{
			printlf ("symbol table loaded OK\n");
		}
	}
	else
	{
		printSyntax ("\"path_to_symbol_table\"");
	}

	delete_dynstr(&buf);
}

static void quit (void)
{
	if (getDone ())
	{
		quitFlag = true;
		return;
	}

	printSyntax ("");
}


static void rawlog (void)
{
	if (getDone ())
	{
		showWatchedData ();
		return;
	}

	printSyntax ("");
}

#ifdef LATENCY_TRACKING
static void ipclog (void)
{
	if (getDone ())
	{
		showEventTracking ();
		return;
	}

	printSyntax ("");
}
#endif

static void rbn (void)
{
	u32 start = 0;
	get (16, &start, NULL);
	if (getDone ())
	{
		PA pa = { 15, BCCMDPDU_GETREQ, 13, BCCMDVARID_BUILD_NAME, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		pa[2] = ++bccmdseqno;
		pa[4] = start;
		sendBCCMD (pa);
		return;
	}
	printSyntax ("[start]");
}

static void et (void)
{
	PA pa = { 7, BCCMDPDU_GETREQ, 13, BCCMDVARID_ESCOTEST, 0, 0, 0 };
	u32 fn, val;

	if (getDone ())
	{
		DictionaryPair *dp;
		struct dynstr buf;
		new_dynstr(&buf, 0);
		strcpy(buf.data, "et ");

		for (dp = et_fns_d; dp->name != 0; dp += 2)
		{
			/* Really needs to be altered to handle dynamic length strings properly */
			strcpy (buf.data + 3, dp->name);
			parseCmd (buf.data, false);
		}

		delete_dynstr(&buf);

		return;
	}
	else if (get (0, &fn, et_fns_d))
	{
		/* Check to see if we are reading or writing the value */
		if (get (16, &val, NULL))
		{
			/* We were passed a value to write */
			pa[1] = BCCMDPDU_SETREQ;
			pa[5] = val;
		}
		if (getDone ())
		{
			pa[2] = ++bccmdseqno;
			pa[4] = fn;
			sendBCCMD (pa);
			return;
		}
	}

	printSyntax ("[fn:... [val]]");
}

#ifdef MGRDIAG
extern void getSomeBCSPdiagnostics (void);
static void diag (void)
{
	if (getDone ())
	{
		getSomeBCSPdiagnostics ();
		return;
	}

	printSyntax ("");
}
#endif

/* restart command, which takes optional script name */

static void restartt (void)
{
        int restart_action = RESTART_NONE;
        /* Just end of line, no script */
	if (getDone())
	{
		restart_action = RESTART_NO_SCRIPT;
	}
        else
        {
        struct dynstr buf;
        
                new_dynstr(&buf, 0);
                
                /* Anything else should only be a valid string
                 * so it is okay to use the library routine. 
                 * The getdone() checks there was nothing else on the line
                 */
                if (getStringDynamicZ (&buf) != BADSIZE && 
                    getDone ())
                {
                    strncpy(restart_filename,buf.data,MAX_RESTART_FILENAME_LEN);
                    restart_action = RESTART_NEW_SCRIPT;
                }

                delete_dynstr(&buf);
        }

        if (restart_action == RESTART_NONE)
        {
                printSyntaxStart();
	printSyntaxContin ("(default: no file)");
	printSyntaxContin ("\"filename\"");
        }
        else
        {
                restart_after_commands = restart_action;
        }
}



static void sym (void)
{
	int i;

	if (getDone ())
	{
		printlf ("last(ba):0x%04lx%02lx%06lx\n", (ul) lastnap, (ul) lastuap, (ul) lastlap);

		if (lastaclch != NOHANDLE)
		{
			ASSERT16 (lastaclch);
			printlf ("last(aclch):0x%04x\n", (unsigned) lastaclch);
		}
		if (lastscoch != NOHANDLE)
		{
			ASSERT16 (lastscoch);
			printlf ("last(scoch):0x%04x\n", (unsigned) lastscoch);
		}
		printlf ("last(lk):0x");
		for (i = 15; i >= 0; --i)
		{
			printlf ("%02x", lastlk[i]);
		}
		printlf ("\n");
		printlf ("last(pc):\"");
		for (i = 0; i < lastpclen; ++i)
		{
			printChar (lastpc[i]);
		}
		printlf ("\"\n");

		for (i = 0; i < 18; ++i)
		{
			if (aclch[i] != NOHANDLE)
			{
				ASSERT16 (aclch[i]);
				printlf ("acl%i:0x%04x\n", i, (unsigned) aclch[i]);
			}
		}
		for (i = 0; i < 10; ++i)
		{
			if (scoch[i] != NOHANDLE)
			{
				ASSERT16 (scoch[i]);
				printlf ("sco%i:0x%04x\n", i, (unsigned) scoch[i]);
			}
		}
		ASSERT16 (abch);
		printlf ("ab:0x%04x\n", (unsigned) abch);
		ASSERT16 (pbch);
		printlf ("pb:0x%04x\n", (unsigned) pbch);

        aclFstatAll();
		return;
	}

	printSyntax ("");
}


static void sleepp (void)
{
	u32 ms = 1000;

	if (getDone () ||
		(get (32, &ms, NULL) &&
		 getDone ()))
	{
		sleep_ms(ms);
		return;
	}

	printSyntaxStart ();
	printSyntaxContin ("(default: 1000)");
	printSyntaxContin ("[ms]");
}


static void sys (void)
{

	struct dynstr buf;
	new_dynstr(&buf, 0);
	
	if (getStringDynamicZ (&buf) != BADSIZE && getDone ())
	{
		system (buf.data);
	}
	else
	{
		printSyntax ("\"command\"");
	}

	delete_dynstr(&buf);
}

static char *extractFromQuotedString(const char *string)
{
    char *stringStart, *stringEnd;
    char *stringCopy;
    size_t length;

    char *retVal = NULL;

    if (NULL != (stringStart = strchr(string, '"')))
    {
        ++stringStart;
        if (NULL != (stringEnd = strrchr(stringStart, '"')))
        {
            length = stringEnd - stringStart;
            if (NULL != (stringCopy = malloc(length + 1)))
            {
                if (NULL != strncpy(stringCopy, stringStart, length))
                {
                    *(stringCopy + length) = '\0';
                    retVal = stringCopy;
                }
                else
                {
                    free(stringCopy);
                }
            }
        }
    }

    return retVal;
}

static int optLog (const char *opts)
{
    char *fnameStart,*fnameEnd;
    ichar *ilfname;
    char *lfname;
    int nameLen;
    int OK=1, FAIL=0;

    /* Is there a log file open?*/
    if (printLog ()) 
        return(FAIL);

    /* Lets extract the filename */
    if ( (fnameStart = strchr(opts, '"') ) == NULL )
        return(FAIL);

    /* Lets move past the " char*/
    fnameStart++;

    /* Where does it end?*/
    if ((fnameEnd = strrchr(fnameStart,'"')) == NULL)    
        return(FAIL); 

    /*Lets back off a little to remove the " */
    --fnameEnd;    
    nameLen = 1+(fnameEnd - fnameStart); 
    if (nameLen <= 0 )
        return(FAIL);  

    /*lets get a clean copy of the file name */
    lfname = malloc(nameLen + 1);
    ASSERT (NULL != lfname);

    if (strncpy(lfname,fnameStart,nameLen) ==NULL) 
        return(FAIL) ;
    
    /* Lets make sure we terminate it*/
    *(lfname+nameLen) ='\0';

    ilfname = iccoercedup (lfname);
    ASSERT (NULL != ilfname);

    if ((logname != NULL && istrcmp (ilfname, logname) == 0) ||
        (sconame[0] != NULL && strcmp (lfname, sconame[0]) == 0) ||
        (sconame[1] != NULL && strcmp (lfname, sconame[1]) == 0) ||
        (sconame[2] != NULL && strcmp (lfname, sconame[2]) == 0))
    {
        return FAIL;
    }

    /* convert to ichar (may be unicode) for printlOpen*/
    logname = ilfname;

    /* Lets try and open the log file*/
    if (!printlOpen(logname))
        return(FAIL);

    /*Let the user know we opened the log file */
    printlf("Log file \"%s\" created\n", lfname);

    free (lfname);
    lfname = NULL;

    return(OK); 
}

static bool isLogFileNameClash(const char *name)
{
    ichar *iname = iccoercedup(name);
    bool clashStatus = (logname != NULL && istrcmp(iname, logname) == 0) ||
                       (sconame[0] != NULL && strcmp(name, sconame[0]) == 0) ||
                       (sconame[1] != NULL && strcmp(name, sconame[1]) == 0) ||
                       (sconame[2] != NULL && strcmp(name, sconame[2]) == 0);
    free(iname);
    return clashStatus;
}

static void enableOptScoLog(int sconum, const char *opts)
{
    char *name = extractFromQuotedString(opts);

    if (NULL != name &&
        NULL == sconame[sconum] &&
        !isLogFileNameClash(name) &&
        enableScoLogging(sconum, name))
    {
        sconame[sconum] = name;
        printlf("sco log file \"%s\" created\n",sconame[sconum]);
    }
    else
    {
        free(name);
        printlf("ERROR: Could not create sco log file\n");
    }
}

static void disableOptScoLog(int sconum)
{
    if (NULL != sconame[sconum])
    {
        char *name = sconame[sconum];
        printlf ("Closing sco log file: \"%s\"\n", sconame[sconum]);
        disableScoLogging(sconum);
        sconame[sconum] = NULL; 
        free(name);
    }
    else
    {
        printlf("The sco log file could not be closed\n");
    }
}

static void opt (void)
{
    if (getDone ())
    {
        int i;
        printTimestamp ();
        printlf ("opt %cawlps\n", autoWlps ? '+' : '-');
        printTimestamp ();
        printlf ("opt %ccqddr\n", showLMPcqddr ? '+' : '-');
        if (!showLMPcqddr && showLMPcqddr2)
        {
            printTimestamp ();
            printlf ("opt %cc2\n", showLMPcqddr2 ? '+' : '-');
        }
        printTimestamp ();
        printlf ("opt %cpower\n", showLMPpower ? '+' : '-');
        printTimestamp ();
        printlf ("opt %cacl\n", showAcl ? '+' : '-');
        printTimestamp ();
        printlf ("opt %clog\n", printLog() ? '+' : '-');
        printTimestamp ();
        printlf ("opt %csco\n", showSco ? '+' : '-');
        printTimestamp ();
        printlf ("opt %cafh\n", showAfh ? '+' : '-');
        printTimestamp ();
        printlf ("opt %cfmrssi\n", fmPrettyRSSI ? '+' : '-');
        printTimestamp ();
        for (i = 0; i < SCO_COUNT; ++i)
        {
            printlf ("opt %cscolog%d\n", NULL != sconame[i]? '+' : '-', i);
            printTimestamp ();
        }
        printlf ("opt %cascii\n", showAscii ? '+' : '-');
        printTimestamp ();
        printlf ("opt %craw\n", showRaw ? '+' : '-');
        printTimestamp ();
        printlf ("opt %ceir\n", decode_eir ? '+' : '-');
        printTimestamp ();
        printlf ("opt %cauto_rawlog\n", auto_rawlog ? '+' : '-');
        printTimestamp ();
        printlf ("opt %cgps\n", showGps ? '+' : '-');
		printTimestamp ();
		printlf ("opt %cblockresetrestart\n", (gBccmdResetFlowControlled && gRestartFlowControlled) ? '+' : '-');
    }
    else
    {
        const char *opts;

        opts = getRestOfLine ();

        if (strstr (opts, "+awlps") != NULL)
        {
            autoWlps = true;
        }
        else if (strstr (opts, "-awlps") != NULL)
        {
            autoWlps = false;
        }
        else if (strstr (opts, "+cqddr") != NULL)
        {
            showLMPcqddr = true;
        }
        else if (strstr (opts, "-cqddr") != NULL)
        {
            showLMPcqddr = false;
        }
        else if (strstr (opts, "+c2") != NULL)
        {
            showLMPcqddr2 = true;
        }
        else if (strstr (opts, "-c2") != NULL)
        {
            showLMPcqddr2 = false;
        }
        else if (strstr (opts, "+power") != NULL)
        {
            showLMPpower = true;
        }
        else if (strstr (opts, "-power") != NULL)
        {
            showLMPpower = false;
        }
        else if (strstr (opts, "+expand") != NULL)
        {
            showExpansions = true;
        }
        else if (strstr (opts, "-expand") != NULL)
        {
            showExpansions = false;
        }
        else if (strstr (opts, "+afh") != NULL)
        {
            showAfh = true;
        }
        else if (strstr (opts, "-afh") != NULL)
        {
            showAfh = false;
        }
        else if (strstr (opts, "+fmrssi") != NULL)
        {
            fmPrettyRSSI = true;
        }
        else if (strstr (opts, "-fmrssi") != NULL)
        {
            fmPrettyRSSI = false;
        }
        else if (strstr (opts, "+acl") != NULL)
        {
            showAcl = true;
        }
        else if (strstr (opts, "-acl") != NULL)
        {
            showAcl = false;
        }
        else if (strstr (opts, "+fp") != NULL)
        {
            showFp = true;
        }
        else if (strstr (opts, "-fp") != NULL)
        {
            showFp = false;
        }
        else if (strstr (opts, "+scolog0") != NULL)
        {
            enableOptScoLog(0, opts);
        }
        else if (strstr (opts, "+scolog1") != NULL)
        {
            enableOptScoLog(1, opts);
        }
        else if (strstr (opts, "+scolog2") != NULL)
        {
            enableOptScoLog(2, opts);
        }
        else if (strstr (opts, "+scolog") != NULL)
        {
            enableOptScoLog(0, opts); 
        }
        else if (strstr (opts, "-scolog0") != NULL)
        {
            disableOptScoLog(0);
        }
        else if (strstr (opts, "-scolog1") != NULL)
        {
            disableOptScoLog(1);
        }
        else if (strstr (opts, "-scolog2") != NULL)
        {
            disableOptScoLog(2);
        }
        else if (strstr (opts, "-scologall") != NULL)
        {
            int i;
            
            for (i = 0; i < SCO_COUNT; ++i)
            {
                disableOptScoLog(i);
            }
        }
        else if (strstr (opts, "-scolog") != NULL)
        {
            disableOptScoLog(0);
        }
        else if (strstr (opts, "+sco") != NULL)
        {
                showSco = true;
        }
        else if (strstr (opts, "-sco") != NULL)
        {
                showSco = false;
        }
        else if (strstr (opts, "-time") != NULL)
        {
                printTimestamps (false);
        }
        else if (strstr (opts, "+time") != NULL)
        {
                printTimestamps (true);
        }
        else if (strstr (opts, "-tag") != NULL)
        {
                printTags (NULL);
        }
        else if (strstr (opts, "+tag") != NULL)
        {
                printTags (strstr (opts, "+tag") + strlen ("+tag"));
        }
        else if (strstr (opts, "+ascii") != NULL)
        {
                showAscii = true;
        }
        else if (strstr (opts, "-ascii") != NULL)
        {
                showAscii = false;
        }
        else if  (strstr (opts, "+log") != NULL)
        {
            if(!optLog(opts))
            { 
                printlf("ERROR: Could not create log file\n");
            } 
        }
        else if (strstr (opts, "-log") != NULL)
        {
            if (printLog())
            {
                char *lfname = iinarrowdup(logname);
                ASSERT (lfname != NULL);
                printlf("Closing log file \"%s\"\n", lfname);
                free(logname);
                logname = NULL;
                free(lfname);
                lfname = NULL;
                printlClose();
            }
        }
        else if (strstr (opts, "+raw") != NULL)
        {
            showRaw = true;
        }
        else if (strstr (opts, "-raw") != NULL)
        {
            showRaw = false;
        }
        else if (strstr (opts, "+gps") != NULL)
        {
            showGps = true;
        }
        else if (strstr (opts, "-gps") != NULL)
        {
            showGps = false;
        }
        else if (strstr (opts, "+eir") != NULL)
        {
            decode_eir = true;
        }
        else if (strstr (opts, "-eir") != NULL)
        {
            decode_eir = false;
        }
        else if (strstr (opts, "+rl") != NULL)
        {
            char *file_name = extractFromQuotedString(opts);
            if (file_name)
            {
                switch (enable_rawlog_log(file_name))
                {
                case FILE_OPEN_SUCCESSFUL:
                    printlf("raw log file \"%s\" created\n", file_name);
                    break;
                case FILE_OPEN_UNSUCCESSFUL:
                    printlf("ERROR: raw log file \"%s\" could not be created\n", file_name);
                    break;
                case REOPENED_SAME_FILE_SAME_LOG:
                    printlf("ERROR: raw log already open with that file\n");
                    break;
                case ALREADY_LOGGING:
                    printlf("ERROR: already logging raw data\n");
                    break;
                case CLASH_EXISTS:
                    printlf("ERROR: file already being used for logging\n");
                    break;
                default: /* should be impossible */
                    printlf("ERROR: mystery\n");
                    break;
                }
            }
            else
            {
                printlf("ERROR: Could not create raw log file\n");
            }
            free(file_name);
        }
        else if (strstr (opts, "-rl") != NULL)
        {
            switch (disable_rawlog_log())
            {
            case FILE_CLOSE_SUCCESSFUL:
                printlf("Raw log closed successfully\n");
                break;
            case FILE_CLOSE_UNSUCCESSFUL:
                printlf("ERROR: unable to close file\n");
                break;
            case NOT_LOGGING:
                printlf("ERROR: not logging raw data\n");
                break;
            default: /* should be impossible */
                printlf("ERROR: mystery\n");
                break;
            }
        }
        else if (strstr (opts, "+auto_rawlog"))
        {
            auto_rawlog = true;
        }
        else if (strstr (opts, "-auto_rawlog"))
        {
            auto_rawlog = false;
        }
        else if (strstr (opts, "-blockresetrestart") != NULL)
        {
            gBccmdResetFlowControlled = false;
            gRestartFlowControlled = false;
        }
        else if (strstr (opts, "+blockresetrestart") != NULL)
        {
            gBccmdResetFlowControlled = true;
            gRestartFlowControlled = true;
        }
        else
        {
            printSyntaxStart ();
            printSyntaxContin ("(+|-)(awlps|cqddr|power|afh|acl|sco|time|expand|ascii|raw|eir|blockresetrestart|tag[...])");
            printSyntaxContin ("+scolog[n] \"filename\"|+log \"filename\"|-scologall|-log|-scolog[n]");
            printSyntaxContin ("(+|-)auto_rawlog");
            printSyntaxContin ("(to display)");
        }

        free ((void *) opts);
    }
}


static void echo (void)
{
        const char *text = getRestOfLine ();

	printTimestamp ();
        printlf ("%s\n", text);
        free (text);
}


#ifdef MGR

static bool waitdo (bool not)
{
	u32 i;
	volatile u32 *ch = NULL;
	extern Dictionary scoch_d, aclch_d;

#define TIMEOUT 45 /* s */

	if (get (0, &i, scoch_d))
	{
		if (getDone ())
		{
			ch = &scoch[i];
		}
	}
	else if (get (0, &i, aclch_d))
	{
		if (getDone ())
		{
			ch = &aclch[i];
		}
	}

	if (ch)
	{
		time_t t0 = time (NULL);
		while ((not ? *ch != NOHANDLE : *ch == NOHANDLE) && difftime (time (NULL), t0) <= TIMEOUT)
		{
#ifdef _WIN32
			Sleep (1000);
#else
			sleep (1);
#endif
		}
		if (difftime (time (NULL), t0) > TIMEOUT)
		{
			printlf ("timeout\n");
		}
		return true;
	}

	return false;
}


static void wait (void)
{
	if (!waitdo (false))
	{
		printSyntax ("ch");
	}
}


static void waitn (void)
{
	if (!waitdo (true))
	{
		printSyntax ("ch");
	}
}

#endif /* MGR */


static void hcconn (void)
{
	if (getDone ())
	{
		PA pa = { 26, TUNNEL, LMP_DEBUG_CMD, HCI_DEBUG_STATS, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		sendCmd (pa);
		return;
	}

	printSyntax ("");
}


static void hcmem (void)
{
	if (getDone ())
	{
		PA pa = { 26, TUNNEL, LMP_DEBUG_CMD, HCI_DEBUG_MEM, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		sendCmd (pa);
		return;
	}

	printSyntax ("");
}


static void smr (void)
{
	u32 ch = lastaclch;
	u32 nce = 4;
	u32 npa = 2;
	u32 tsp = 160;
	bool ok = false;

	if (getDone ())
	{
		ok = true;
	}
	else if (getch (&ch) &&
		 get (8, &nce, NULL) &&
		 get (8, &npa, NULL) &&
		 get (16, &tsp, NULL) &&
		 getDone ())
	{
		ok = true;
	}

	if (ok)
	{
		PA pa = { 26, TUNNEL, LMP_DEBUG_CMD, HCI_DEBUG_SCATTER_REQ,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		pa[8]  = ch;
		pa[9]  = ch>>8;
		pa[10]  = nce;
		pa[11]  = 0;
		pa[12]  = npa;
		pa[13]  = 0;
		pa[14]  = tsp;
		pa[15]  = tsp>>8;

		sendCmd (pa);
		return;
	}

	printSyntax ("[ch nce npa tsp] (defaults: last 4 2 160)");
}


static void usr (void)
{
	u32 ch = lastaclch;
	bool ok = false;

	if (getDone ())
	{
		ok = true;
	}
	else if (getch (&ch) && getDone ())
	{
		ok = true;
	}

	if (ok)
	{
		PA pa = { 26, TUNNEL, LMP_DEBUG_CMD, HCI_DEBUG_UNSCATTER_REQ,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		pa[8]  = ch;
		pa[9]  = ch>>8;
		sendCmd (pa);
		return;
	}

	printSyntax ("[ch]");
}


static void ssr (void)
{
	u32 ch = lastaclch;
	u32 sr;

	if (getch (&ch) &&
		get (8, &sr, NULL) &&
		getDone ())
	{
		PA pa = { 26, TUNNEL, LMP_DEBUG_CMD, HCI_DEBUG_SET_SUBRATE,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		pa[8]  = ch;
		pa[9]  = ch>>8;
		pa[10] = sr;
		sendCmd (pa);
		return;
	}

	printSyntax ("ch sr");
}


static void ppe (void)
{
	u32 ch = lastaclch;
	u32 ppe;

	if (getch (&ch) &&
		get (8, &ppe, NULL) &&
		getDone ())
	{
		PA pa = { 26, TUNNEL, LMP_DEBUG_CMD, HCI_DEBUG_PP_REQ,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		pa[8]  = ch;
		pa[9]  = ch>>8;
		pa[10] = ppe;
		sendCmd (pa);
		return;
	}

	printSyntax ("ch npa");
}

static void aa (void)
{
	u32 ch = lastaclch;
	u32 awin, aint;

	if (getch (&ch) &&
		get (16, &awin, NULL) &&
		get (16, &aint, NULL) &&
		getDone ())
	{
		PA pa = { 26, TUNNEL, LMP_DEBUG_CMD, HCI_DEBUG_SET_ABSENCE,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		pa[8]  = ch;
		pa[9]  = ch>>8;
		pa[10] = awin;
		pa[11] = awin>>8;
		pa[12] = aint;
		pa[13] = aint>>8;
		sendCmd (pa);
		return;
	}

	printSyntax ("ch window interval");
}

static void da (void)
{
	u32 ch = lastaclch;

	if (getch (&ch) &&
		getDone ())
	{
		PA pa = { 26, TUNNEL, LMP_DEBUG_CMD, HCI_DEBUG_CLEAR_ABSENCE,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		pa[8]  = ch;
		pa[9]  = ch>>8;
		sendCmd (pa);
		return;
	}

	printSyntax ("ch");
}

static void ep (void)
{
	if (getDone ())
	{
		PA pa = { 26, TUNNEL, LMP_DEBUG_CMD, HCI_DEBUG_ESCO_PING,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		sendCmd (pa);
		return;
	}

	printSyntax ("");
}


static void sopt (void)
{
	if (getDone ())
	{
		printTimestamp ();
		printlf ("sopt %csco\n", DecodeSCO ? '+' : '-');
		printTimestamp ();
		printlf ("sopt %cacl\n", DecodeACL ? '+' : '-');
		printTimestamp ();
		printlf ("sopt %crd\n", showRawData ? '+' : '-');
		printTimestamp ();
		printlf ("sopt %ch\n", showHeader ? '+' : '-');
		printTimestamp ();
		printlf ("sopt %crt\n", showRetransmit ? '+' : '-');
		printTimestamp ();
		printlf ("sopt %cl2cap\n", showl2cap ? '+' : '-');
		printTimestamp ();
		printlf ("sopt %cbnepipmc\n", ignoreIPMulticasts ? '-' : '+');
		printTimestamp ();
		printlf ("sopt %cbnepmiscmc\n", ignoreMiscMulticasts ? '-' : '+');
		printTimestamp ();
		printlf ("sopt %cbnepspam\n", ignoreSpam ? '-' : '+');
		printTimestamp ();
		printlf ("sopt %cbnepverbose\n", bnepVerbose ? '+' : '-');
	}
	else
	{
		const char *opts;
		opts = getRestOfLine ();

		if (strstr (opts, "+sco") != NULL)
		{
			DecodeSCO = true;
		}
		else if (strstr (opts, "-sco") != NULL)
		{
			DecodeSCO = false;
		}
		else if (strstr (opts, "+acl") != NULL)
		{
			DecodeACL = true;
		}
		else if (strstr (opts, "-acl") != NULL)
		{
			DecodeACL = false;
		}
		else if (strstr (opts, "+rd") != NULL)
		{
			showRawData = true;
		}
		else if (strstr (opts, "-rd") != NULL)
		{
			showRawData = false;
		}
		else if (strstr (opts, "+h") != NULL)
		{
			showHeader = true;
		}
		else if (strstr (opts, "-h") != NULL)
		{
			showHeader = false;
		}
		else if (strstr (opts, "+rt") != NULL)
		{
			showRetransmit = true;
		}
		else if (strstr (opts, "-rt") != NULL)
		{
			showRetransmit = false;
		}
		else if (strstr (opts, "+l2cap") != NULL)
		{
			showl2cap = true;
		}
		else if (strstr (opts, "-l2cap") != NULL)
		{
			showl2cap = false;
		}
		else if (strstr (opts, "+bnepipmc") != NULL)
		{
			ignoreIPMulticasts = false;
		}
		else if (strstr (opts, "-bnepipmc") != NULL)
		{
			ignoreIPMulticasts = true;
		}
		else if (strstr (opts, "+bnepmiscmc") != NULL)
		{
			ignoreMiscMulticasts = false;
		}
		else if (strstr (opts, "-bnepmiscmc") != NULL)
		{
			ignoreMiscMulticasts = true;
		}
		else if (strstr (opts, "+bnepspam") != NULL)
		{
			ignoreSpam = false;
		}
		else if (strstr (opts, "-bnepspam") != NULL)
		{
			ignoreSpam = true;
		}
        else if (strstr (opts, "+bnepverbose") != NULL)
        {
            bnepVerbose = true;
        }
        else if (strstr (opts, "-bnepverbose") != NULL)
        {
            bnepVerbose = false;
        }
		else
		{
			printSyntaxStart ();
			printSyntaxContin ("(+|-)(sco|acl|rd|h|rt|l2cap|bnepmc|bnepspam|bnepverbose)");
			printSyntaxContin ("(to display)");
		}

		free ((void *) opts);
	}
}

static void sb (void)
{
	u32 clk;

	if (get (32, &clk, NULL) && getDone ())
	{
		PA pa = { 26, TUNNEL, LMP_DEBUG_CMD, HCI_DEBUG_SET_BTCLOCK, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		pa[8]  = clk;
		pa[9]  = clk >> 8;
		pa[10] = clk >> 16;
		pa[11] = clk >> 24;
		sendCmd (pa);
		return;
	}

	printSyntax ("clk");
}

static void br (void)
{
	u32 type, state;

	if (get (16, &type, block_rx_d) &&
		get (16, &state, disabledEnabled_d) &&
		getDone ())
	{
		PA pa = { 26, TUNNEL, LMP_DEBUG_CMD, HCI_DEBUG_BLOCK_RX, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		pa[8] = type;
		pa[9] = type >> 8;
		pa[10] = state;
		pa[11] = state >> 8;
		sendCmd (pa);
		return;
	}

	printSyntax ("t:lsa|mssp s:d|e|...");
}


static void trs (void)
{
	if (getDone ())
	{
        printTimestamp();
		if (trans_sleep_now ())
        {
            printlf("trs succeeded\n");
        }
        else
        {
            printlf("trs failed\n");
        }
	}
    else
    {
	    printSyntaxStart ();
    }
}

static void trw (void)
{
	if (getDone ())
	{
        printTimestamp();
		if (trans_sleep_never ())
        {
            printlf("trw succeeded\n");
        }
        else
        {
            printlf("trw failed\n");
        }
	}
    else
    {
	    printSyntaxStart ();
    }
}

static void lt (void)
{
	u32 ch, arg, len, tid, o;
	u8 buf[16];
	size_t datalen, i;
	if (getch (&ch))
	{
		PA pa = { 26, TUNNEL, LMP_DEBUG_CMD, HCI_DEBUG_TX_LMP, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		len = 8;
		if (get (0, &tid, ms_d))
		{
			if (get (15, &o, lmp_d))
			{

				/* Is it an escape opcode - well anything bigger than 8 bits */

				if (o > 0xff)

				{

					pa[len++] = ((o >> 7) & 0xfe) | tid;

					pa[len++] = (o & 0xff);

				}

				else

				{

					pa[len++] = (o << 1) | tid;

				}
			}
			while (len != sizeof (pa) / sizeof (pa[0]))
			{
				if ((datalen = getLongHex (buf, sizeof (buf))))
				{
					for (i = 0; i < datalen && len != sizeof (pa) / sizeof (pa[0]); ++i)
					{
						pa[len++] = buf[i];
					}
				}
				else if (get (8, &arg, NULL))
				{
					pa[len++] = arg;
				}
				else
				{
					break;
				}
			}
			if (getDone ())
			{
				pa[4] = len - 8;
				pa[5] = (len - 8) >> 8;
				pa[6] = ch;
				pa[7] = ch >> 8;
				sendCmd (pa);
				return;
			}
		}
		else
		{
			while (len != sizeof (pa) / sizeof (pa[0]) && get (8, &arg, NULL))
			{
				pa[len++] = arg;
			}
			if (len != 8 && getDone ())
			{
				pa[4] = len - 8;
				pa[5] = (len - 8) >> 8;
				pa[6] = ch;
				pa[7] = ch >> 8;
				sendCmd (pa);
				return;
			}
		}
	}

	printSyntaxStart ();
	printSyntaxContin ("ch tid:m|s o:... [n]*");
	printSyntaxContin ("ch [octet]+");
}


static void ls (void)
{
	if (getDone ())
	{
		PA pa = { 26, TUNNEL, LMP_DEBUG_CMD, HCI_DEBUG_STOP_LM, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		sendCmd (pa);
		return;
	}

	printSyntax ("");
}


static void lg (void)
{
	if (getDone ())
	{
		PA pa = { 26, TUNNEL, LMP_DEBUG_CMD, HCI_DEBUG_START_LM, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		sendCmd (pa);
		return;
	}

	printSyntax ("");
}


static void ltc (void)
{
	u32 ts, hm, tf, rf, pcm, pp, pt, lotd;
	if (	get (8, &ts, testScenario_d) &&
		(((ts == PAUSE_TEST_MODE ||
		   ts == EXIT_TEST_MODE) &&
		  getDone ()) ||
		 (get (8, &hm, hoppingMode_d) &&
		  get (8, &tf, NULL) &&
		  get (8, &rf, NULL) &&
		  get (8, &pcm, powerControlMode_d) &&
		  get (8, &pp, NULL) &&
		  get (8, &pt, testPacketType_d) &&
		  get (16, &lotd, NULL) &&
		  getDone ())))
	{
		PA pa = { 26, TUNNEL, LMP_DEBUG_CMD, HCI_DEBUG_LMP_TEST_CONTROL, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		pa[8] = ts;
		pa[10] = hm;
		pa[12] = tf;
		pa[14] = rf;
		pa[16] = pcm;
		pa[18] = pp;
		pa[20] = pt;
		pa[22] = lotd;
		pa[23] = lotd >> 8;
		sendCmd (pa);
		return;
	}

	printSyntaxStart ();
	printSyntaxContin ("ts:p|tt0|tt1|tt1010|prbs|clba|clbs|aww|sww|tt11110000|etm hm:sf|eu|j|f|s|r tf rf pcm:f|a pp pt:... lotd");
	printSyntaxContin ("ts:p|etm");
}


static void lta (void)
{
	if (getDone ())
	{
		PA pa = { 26, TUNNEL, LMP_DEBUG_CMD, HCI_DEBUG_LMP_TEST_ACTIVATE, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		sendCmd (pa);
		return;
	}

	printSyntax ("");
}


static void smj (void)
{
	/*
	 * Note this name is now illogical since it now controls jitter on the
	 * slave as well.
	 */
	u32 onoff;
	if (get (8, &onoff, offOn_d))
	{
		PA pa = { 26, TUNNEL, LMP_DEBUG_CMD, HCI_DEBUG_MASTER_JITTER, 
			  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		pa[8] = onoff;
		sendCmd (pa);
		return;
	}

	printSyntax ("off|on");
}


void exec_do (const char *filename)
{
	FILE *f;
	
	f = fopen (filename, "r");
	if (f == NULL)
	{
		printTimestamp ();
		printlf ("exec: %s!\n", strerror (errno));
	} 
	else 
	{
		struct dynstr buf;
		new_dynstr(&buf, 0);

		while (fgetsDynamic (&buf, f) != NULL)
		{
			parseCmd (buf.data, true);
			if (quitFlag)
			{
				break;
			}
			buf.length = 0;
		}
		delete_dynstr(&buf);
		fclose (f);
	}
	
}

static void exec (void)
{
	struct dynstr buf;
	new_dynstr(&buf, 0);
	
	if (getStringDynamicZ (&buf) != BADSIZE && 
		getDone ())
	{
		exec_do (buf.data);
	}
	else
	{
		printSyntax ("\"filename\"");
	}

	delete_dynstr(&buf);
}


static void slave (void)
{
	if (getDone ())
	{
		parseCmd ("sef connection_setup all auto_accept", true);
		parseCmd ("wse inquiry_page", true);
		parseCmd ("rba", true);
		return;
	}

	printSyntax ("");
}


static void msp (void)
{
	if (getDone ())
	{
		parseCmd ("bcset map_sco_pcm 1", true);
		return;
	}

	printSyntax ("");
}

static void settran (void)
{
	u32 transport, bps;
#define NOSETTRAN ((u32) -1)

	if (!get (8, &transport, transport_d))
	{
		transport = NOSETTRAN;
	}

	if (transport == TRANSPORT_USB ||
		transport == TRANSPORT_NONE ||
		!get (32, &bps, NULL))
	{
		bps = NOSETTRAN;
	}

	if (	getDone () &&
		(transport != NOSETTRAN || bps != NOSETTRAN) &&
		(bps <= 15999633  /* Else get overflow in calculation below */
		 /* || bps >= 123 /@ Else get zero in calculation below */
		 || bps == NOSETTRAN))
	{
		u16 uart_config = 0;
		u32 uart_baud_rate = 0;

		if (transport != NOSETTRAN)
		{
			PA pa = { 8, BCCMDPDU_SETREQ, 0, BCCMDVARID_PS, 0, 1, 0, 0 };

			pa[2] = ++bccmdseqno;
			pa[4] = PSKEY_HOST_INTERFACE;
			pa[6] = PS_STORES_DEFAULT;
			pa[7] = transport;

			sendBCCMD (pa);

			if (transport == TRANSPORT_BCSP)
			{
				uart_config = 0x06; /* HERE magic */
			}
			else if (transport == TRANSPORT_H4)
			{
				uart_config = 0xa8; /* HERE magic */
			}
		}
		if (bps != NOSETTRAN)
		{
			PA pa = { 8, BCCMDPDU_SETREQ, 0, BCCMDVARID_PS, 0, 1, 0, 0 };

			uart_baud_rate = (bps * 128 + 15625) / 31250;

			pa[2] = ++bccmdseqno;
			pa[4] = PSKEY_UART_BAUDRATE;
			pa[6] = PS_STORES_DEFAULT;
			ASSERT (uart_baud_rate <= 0xffffUL);
			pa[7] = (u16) uart_baud_rate;

			sendBCCMD (pa);

			++uart_baud_rate;
		}
		if (uart_config != 0 || uart_baud_rate != 0)
		{
			PA pa = { 17, BCCMDPDU_GETREQ, 0, BCCMDVARID_PS, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			PSOPDATA psopdata;
			psopdata.state = PRESENTATIONWRITEGET;
			pa[2] = psopdata.seqno = ++bccmdseqno;
			pa[4] = psopdata.pskeyreal = PSKEY_HOSTIO_UART_PS_BLOCK;
			pa[6] = PS_STORES_DEFAULT;
			/*
			 * if doing a PRESENTATIONWRITEREAD and
			 * psopdata.pskeypresentation == psopdata.pskeyreal
			 * then this is doing a write of the UART config
			 * and/or baud rate.  These are stored in
			 * in psopdata.offset and psopdata.len
			 * respectively, and the value in psopdata.len
			 * is incremented.  If the value in either
			 * of these is zero then the corresponding bit
			 * in the PS is not to be changed.
			 */
			psopdata.pskeypresentation = psopdata.pskeyreal;
			psopdata.offset = uart_config;
			ASSERT (uart_baud_rate <= 0xffffUL);
			psopdata.len = (u16) uart_baud_rate;
			psop_add(&psopdata);

			sendBCCMD (pa);
		}

		return;
	}

	printSyntaxStart ();
	printSyntaxContin ("bps");
	printSyntaxContin ("t:bcsp|h5|h4|h4ds|raw [bps]");
	printSyntaxContin ("t:usb|none");
}


static void help (void)
{
	printlf ("'find' to list all commands\n"
		"'find \"substring\"' to find a command by name fragment\n"
		"'<cmd> ?' for command syntax ('[]' for optionals, '|' for alternates)\n"
		"Read the fine manual for more help\n");
}


static void find (void)
{
	struct dynstr buf;
	new_dynstr(&buf, 0);

	if (getDone () || 
		(getStringDynamicZ (&buf) != BADSIZE && 
		 getDone ()))
	{
		size_t i, n = getNumCommands ();
		bool found = false;

		for (i = 0; i < n; ++i)
		{
			if ((strstr (getCommandAbbNam (i), buf.data) != NULL ||
				strstr (getCommandFullName (i), buf.data) != NULL) &&
                                strchr (getCommandAbbNam (i), ' ') == NULL)
			{
				found = true;
				printlf ("%s\t%s\n", getCommandAbbNam (i), getCommandFullName (i));
			}
		}
		if (!found)
		{
			printlf ("not found\n");
		}
	} 
	else 
	{
		printSyntaxStart ();
		printSyntaxContin ("(to list all commands)");
		printSyntaxContin ("\"substring\"");
	}

	delete_dynstr(&buf);
}


static void set (void)
{
	const char *name, *value;

	if (getDone ())
	{
		printVar (NULL);
		return;
	}

	if ((name = getVar ()) != NULL)
	{
		if (getDone ())
		{
			printVar (name);
			free ((void *) name);
			return;
		}
		if (getEquals ())
		{
			if (getDone ())
			{
				deleteVar (name);
			}
			else
			{
				value = getRestOfLine ();
				addVar (name, value);
				free ((void *) value);
			}
			free ((void *) name);
			return;
		}
		free ((void *) name);
	}

	printSyntaxStart ();
	printSyntaxContin ("var=value");
	printSyntaxContin ("var= (to delete the variable)");
	printSyntaxContin ("var (to display the variable's value)");
	printSyntaxContin ("(to display all variables)");
}


static void vm_data (void)
{
	size_t len;
	u8 buf[256+16];
	u32 pos;
	u32 c;

	memset (buf, 0, sizeof (buf));

#ifdef MMUSE8BITHOST
	/* Use 8 bit version (deprecated) */
	if (get (8, &c, NULL))
	{
		buf[1] = (u8) (c & 0xff);
		pos = 2;

		while (!getDone () && pos < 100)
		{
			if (get (8, &c, NULL))
			{
				buf[pos++] = (u8) (c & 0xff);
			}
			else if ((len = getStringZ ((char *) &buf[pos], sizeof (buf) - pos)) != BADSIZE)
			{
				/* len does not include the NUL */
				pos += len + 1;
			}
			else
			{
				printSyntax ("sc [\"string\"|byte]*");
				return;
			}
		}

		buf[0] = (u8) (pos & 0xff);

		sendVMdata (buf, pos);
		return;
	}

	printSyntax ("sc [\"string\"|byte]*");
#else
	/* Use 16 bit version */
	if (get (16, &c, NULL))
	{
		buf[2] = (u8) (c & 0xff);
		buf[3] = (u8) (c >> 8);
		pos = 4;

		while (!getDone () && pos < 100)
		{
			if (get (16, &c, NULL))
			{
				buf[pos++] = (u8) (c & 0xff);
				buf[pos++] = (u8) (c >> 8);
			}
			else if ((len = getStringZ ((char *) &buf[pos], sizeof (buf) - pos)) != BADSIZE)
			{
				/* len does not include the NUL */
				// +2 because +1 for NUL and +1 for padding if odd, then round down
				pos += (len + 2) & ~1;
				/* make sure that the pad character is 'nul' */
				buf[pos-1] = 0;
			}
			else
			{
				printSyntax ("sc [\"string\"|word]*");
				return;
			}
		}

		buf[0] = (u8) (pos >> 1);
		buf[1] = (u8) (pos >> 9);

		sendVMdata (buf, pos);
		return;
	}

	printSyntax ("sc [\"string\"|word]*");
#endif
}

static char aclmsgpad[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
#define ACLMSGPADBYTES (sizeof(aclmsgpad)-1)
static void acl (void)
{
	u32 ch, pbf = START, bf = PTP, cid = 0x0040, l2caplen;
	size_t datalen;
	struct dynstr data;
	bool ok = false, l2capheader = true;
	
	new_dynstr(&data, 0);

	if (lastaclch != NOHANDLE && 
		(l2caplen = datalen = getStringDynamic (&data)) != BADSIZE)
	{
		ch = lastaclch;
		ok = true;
	}
	else if (lastaclch != NOHANDLE && 
            get (16, &l2caplen, NULL) &&
            getDone() )
	{
                ch = lastaclch;
                datalen = 0;
                while ((l2caplen - datalen) > ACLMSGPADBYTES)
                {
                    /* appendToDynamic returns new string length */
                    datalen = appendToDynamic( &data, aclmsgpad, ACLMSGPADBYTES);
                }
                datalen = appendToDynamic( &data, aclmsgpad, l2caplen - datalen);
                ASSERT( l2caplen == datalen );
                datalen = l2caplen;
		ok = true;
	}
	else if (getch (&ch))
	{
            if ((l2caplen = datalen = getStringDynamic (&data)) != BADSIZE)
            {
                ok = true;
            }
            else if (get (2, &pbf, pbf_d) && 
                     get (2, &bf, bf_d))
            {
                if (pbf == START || pbf == START_NAF)
                {
                    if ((l2caplen = datalen = getStringDynamic (&data)) != BADSIZE)
                    {
                        l2capheader = false;
                        ok = true;
                    }
                    else if (get (16, &l2caplen, NULL) &&
                             get (16, &cid, cid_d))
                    {
                        if ((datalen = getLongHex ((u8 *) data.data, data.capacity)) != 0) /* should be fine treated as normal string */
                        {
                            ok = true;
                        }
                        else if ((datalen = getStringDynamic (&data)) != BADSIZE)
                        {
                            ok = true;
                        }
                    }
                }
                else
                {
                    if ((datalen = getLongHex ((u8 *) data.data, data.capacity)) != 0) /* should be fine treated as normal string */
                    {
                        ok = true;
                    }
                    else if ((datalen = getStringDynamic (&data)) != BADSIZE)
                    {
                        ok = true;
                    }
                }
            }
        }
        
	if (ok && getDone ())
	{
#if 0
		size_t u;
		printlf ("-> ch 0x%04lx pbf %lu bf %lu", (ul) ch, (ul) pbf, (ul) bf);
		if (pbf == START || pbf == START_NAF)
		{
			printlf (" l2len %u cid 0x%04x", l2caplen, cid);
		}
		for (u = 0; u < datalen; ++u)
		{
			printlf (" %02x", data[u + 4]);
		}
		printlf (" [%lu]\n", (ul) datalen);
#else
                sendAcl ((uint16) ch, (uint16)cid, (uint8) pbf, (uint8) bf, (u8 *) data.data, datalen, l2capheader, (u16)l2caplen);
#endif
		if ( ok )
		{
			if ( bf == ACTIVE_BC )
				abch = ch;
			else if ( bf == PICONET_BC )
				pbch = ch;
			if (ch != abch && ch != pbch)
			{
				lastaclch = ch;
			}
		}
	} 
	else 
	{
		printSyntaxStart ();
		printSyntaxContin ("[ch] \"string\" (defaults: start point-to-point cid:0x0040)");
		printSyntaxContin ("[count/bytes] (defaults: start point-to-point cid:0x0040)");
		printSyntaxContin ("ch pbf:s|snaf bf:ptp|ab|pb \"string\"");
		printSyntaxContin ("ch pbf:s|snaf bf:ptp|ab|pb l2caplen cid:n|s|cl|d40 data|\"string\"");
		printSyntaxContin ("ch pbf:c bf:ptp|ab|pb data|\"string\"");
	}

	delete_dynstr(&data);
}

static void aclab (void)
{
	u16 cid = 0x0040;
	size_t datalen;
	struct dynstr data;
	new_dynstr(&data, 0);

	if ((datalen = getStringDynamic (&data)) != BADSIZE && 
		getDone ())
	{
		sendAcl ((u16) abch, cid, START, 1, (u8 *) data.data, datalen, true, datalen);
	} 
	else
	{
		printSyntax ("\"string\"");
	}

	delete_dynstr(&data);
}


static void aclpb (void)
{
	u32 cid = 0x0040;
	size_t datalen;
	
	struct dynstr data;
	new_dynstr(&data, 0);

	if ((datalen = getStringDynamic (&data)) != BADSIZE && 
		getDone ())
	{
		sendAcl ((u16) pbch, (u16)cid, START, 2, (u8 *) data.data, (u16)datalen, true, (u16)datalen);
	}
	else
	{
		printSyntax ("\"string\"");
	}

	delete_dynstr(&data);
}

static void ping (void)
{
	char *rol = getRestOfLine ();
	char *req = malloc (strlen ("acl \"ping\" ") + strlen (rol) + 10);
	char time[16];

	if (*rol != '?') 
	{
		ASSERT (req != NULL);
		strcpy (req, "acl ");
		strcat (req, rol);
		if (req[strlen (req) - 1] != ' ')
		{
			strcat (req, " ");
		}
		strcat (req, "\"ping");
		getTimestamp (time);
		strcat (req, time + 3);
		strcat (req, "\"");
		parseCmd (req, true);
	}
	else
	{
		printSyntax ("[ch]");
	}

	free (rol);
	free (req);
}


static void aclr (void)
{
	u32 ch, pbf = START, bf = PTP, l2caplen, cid = 0x0040;
	size_t datalen;
	bool ok = false;

	struct dynstr data;
	new_dynstr(&data, 0);

	if (lastaclch != NOHANDLE && 
		(l2caplen = datalen = getStringDynamic (&data)) != BADSIZE)
	{
		ch = lastaclch;
		ok = true;
	}
	else if (getch (&ch))
	{
		if ((l2caplen = datalen = getStringDynamic (&data)) != BADSIZE)
		{
			cid = 0x0040;
			ok = true;
		}
		else if (get (2, &pbf, pbf_d) && 
				 get (2, &bf, bf_d))
		{
			if ((pbf != START && pbf != START_NAF) ||
				((pbf == START || pbf == START_NAF) &&
				 get (16, &l2caplen, NULL) &&
				 get (16, &cid, cid_d)))
			{
				if ((datalen = getLongHex ((u8 *) data.data, data.capacity)))
				{
					ok = true;
				}
				else if ((datalen = getStringDynamic (&data)) != BADSIZE)
				{
					ok = true;
				}
			}
		}
	}

	if (ok && getDone())
	{
#if 0
		size_t u;

			printlf ("-> ch 0x%04lx pbf %lu bf %lu", (ul) ch, (ul) pbf, (ul) bf);
			if (pbf == START || pbf == START_NAF)
			{
				printlf (" l2len %u cid 0x%04x", l2caplen, cid);
			}
			for (u = 0; u < datalen; ++u)
			{
				printlf (" %02x", data.data[u + 4]);
			}
			printlf (" [%lu]\n", (ul) datalen);
#else
		sendAclRaw((u16) ch, (u16) cid, (u8) pbf, (u8) bf, (u8 *) data.data, datalen, (u16)l2caplen);
#endif
		if (ch != abch && ch != pbch)
		{
			lastaclch = ch;
        }
	}
	else
	{
		printSyntaxStart ();
		printSyntaxContin ("[ch] \"string\" (defaults: start point-to-point cid:0x0040)");
		printSyntaxContin ("ch pbf:s|snaf bf:ptp|ab|pb l2caplen cid:n|s|cl|d40 data|\"string\"");
		printSyntaxContin ("ch pbf:c bf:ptp|ab|pb data|\"string\"");
	}

	delete_dynstr(&data);
}

static void sco (void)
{
	u32 ch;
	size_t datalen;
	u8 data[255]; /* Maximum possible in SCO packet */
	bool ok = false;

	if (lastscoch != NOHANDLE && (datalen = getString ((char *) data, sizeof (data))) != BADSIZE)
	{
		ch = lastscoch;
		ok = true;
	}
	else if (getsch (&ch))
	{
		if ((datalen = getString ((char *) data, sizeof (data))) != BADSIZE)
		{
			ok = true;
		}
	}

	if (ok)
	{
		if (getDone ())
		{
			sendSco ((u16) ch, data, (u8) datalen);
			lastscoch = ch;
			return;
		}
	}

	printSyntax ("[ch] \"string\"");
}

#if !defined(_WINCE) && !defined(_WIN32_WCE) /* HERE Move this into the test below! */

#undef BTCLI_SENDSCO_SUPPORTABLE
#if defined (_WIN32) || (!defined (POSIX_NO_NANOSLEEP) && !(defined (POSIX_NO_CLOCK_GETTIME) && defined (BSD_NO_GETTIMEOFDAY)))
#define BTCLI_SENDSCO_SUPPORTABLE
#endif

#ifdef BTCLI_SENDSCO_SUPPORTABLE

/* Samples per second */
#define SCO_RATE 8000 
/* Size of a sample in bits */
#define SAMPLE_SIZE 8 

static void sends (void)
{
    uint32 ch;
    uint32 rate       = SCO_RATE, 
           sampleSize = SAMPLE_SIZE;

    bool override_packet_size = false;
    uint16 packet_size = 0;
    bool override_num_packets = false;
    uint16 num_packets = 0;

    struct dynstr filename;

    if ((voiceSetting & 0x0320) == 0x0020) /* 16-bit linear */
    {
        sampleSize <<= 1;
    }

    if (!getsch(&ch))
    {
        ch = lastscoch;
    }

    new_dynstr(&filename, 0);
    if (ch != NOHANDLE &&
        getStringDynamicZ (&filename) != BADSIZE)
    {
        uint32 num;
        if (get(16, &num, NULL))
        {
            override_packet_size = true;
            packet_size = (uint16) num;

            if (get(16, &num, NULL))
            {
                override_num_packets = true;
                num_packets = (uint16) num;
            }
        }
    }

    if (getDone())
    {
        enableScoFileSend((uint16)ch, 
                          filename.data, 
                          rate, 
                          sampleSize,
                          override_packet_size,
                          packet_size,
                          override_num_packets,
                          num_packets);
    }
    else
    {
        printSyntax ("[ch] \"filename\" [packet_size] [num_packets]");
    }

    delete_dynstr(&filename);
}

#endif /* BTCLI_SENDSCO_SUPPORTABLE */
#endif

static void sendd (bool flushable)
{
    u32 ch;

    struct dynstr filename;
    new_dynstr(&filename, 0);

    if (!getch (&ch))
    {
        ch = lastaclch;
    }

    if (ch != NOHANDLE &&
        !isAclChannelOpen((u16)ch))
    {
        btcli_enter_critical_section();
        printTimestamp();
        printlf("send: ");
        printch(ch);
        printlf(" invalid acl ch\n");
        btcli_leave_critical_section();
    }
    else if (ch == abch || ch == pbch)
    {
        btcli_enter_critical_section();
        printTimestamp ();
        printlf ("send: can't broadcast a file!\n");
        btcli_leave_critical_section();
    }
    else if (ch != NOHANDLE &&
             getDone())
    {
        uint32 fileSize = 0, bytesSent = 0;

        if (cancelFileSend((uint16)ch, &fileSize, &bytesSent))
        {
            btcli_enter_critical_section();
            printTimestamp();
            if (0 == fileSize)
            {
                printlf ("send: aborted on ");
            }
            else
            {
                printlf ("send: aborted after %lu of %lu octets sent on ", (ul) bytesSent, (ul) fileSize);
            }
            printch(ch);
            printlf("\n");
            btcli_leave_critical_section();
        }
        else
        {
            btcli_enter_critical_section();    
            printlf ("send: not sending on ");
            printch (ch);
            printlf ("!\n");
            btcli_leave_critical_section();    
        }

    }
    else if (getStringDynamicZ (&filename) != BADSIZE &&
             getDone())
    {
        if (isSendFileActive((uint16)ch))
        {
            btcli_enter_critical_section();
            printTimestamp ();
            printlf ("send: still sending on ");
			printch (ch);
			printlf ("!\n");
            btcli_leave_critical_section();
        }
        else if (!sendFile((uint16)ch, filename.data, flushable))
        {
            btcli_enter_critical_section();
            printTimestamp ();
            printlf ("send: %s!\n", strerror (errno));
            btcli_leave_critical_section();
        }
    }
    else
    {
        printSyntaxStart ();
        printSyntaxContin ("[ch] \"filename\"");
        printSyntaxContin ("[ch] (to abort)");
    }
    unblock_callbacks();
    delete_dynstr(&filename);
}

static void send_f (void)
{
    sendd(true);
}

static void send_nf (void)
{
    sendd(false);
}

static void recvv (bool flushable)
{
    u32 ch;

    struct dynstr filename;
    new_dynstr(&filename, 0);

    if (!getch (&ch))
    {
        ch = lastaclch;
    }

    if (ch != NOHANDLE &&
        !isAclChannelOpen((uint16) ch))
    {
        btcli_enter_critical_section();
        printTimestamp();
        printlf("recv: ");
        printch(ch);
        printlf(" invalid acl ch\n");
        btcli_leave_critical_section();
    }
    else if (ch != NOHANDLE &&
             getDone ())
    {
        uint32 fileSize = 0, bytesReceived = 0;
        
        if (cancelFileRecv((uint16)ch, &fileSize, &bytesReceived))
        {
            btcli_enter_critical_section();
            printTimestamp();
            if (0 == fileSize)
            {
                printlf ("recv: aborted on ");
            }
            else
            {
                printlf ("recv: aborted after %lu of %lu octets received on ", (ul) bytesReceived, (ul) fileSize);
            }
            printch(ch);
            printlf("\n");
	    btcli_leave_critical_section();
        }
        else
        {
            btcli_enter_critical_section();
	    printTimestamp();
            printlf ("recv: not receiving on ");
            printch (ch);
            printlf ("!\n");
	    btcli_leave_critical_section();
        }
    }
    else if (ch != NOHANDLE &&
             getStringDynamicZ (&filename) != BADSIZE &&
             getDone ())
    {
        if (isRecvFileActive((uint16)ch))
        {
            btcli_enter_critical_section();
            printTimestamp();
            printlf ("recv: still receiving on ");
			printch (ch);
			printlf ("!\n");
            btcli_leave_critical_section();
        }
        else if (recvFile((uint16)ch, filename.data, flushable))
        {
            btcli_enter_critical_section();
	    printTimestamp();
            printlf ("recv: ready to receive on ");
            printch (ch);
            printlf ("\n");
            btcli_leave_critical_section();
        }
        else
        {
            btcli_enter_critical_section();
	    printTimestamp();
            printlf ("recv: %s!\n", strerror (errno));
            btcli_leave_critical_section();
        }
    }
    else
    {
        btcli_enter_critical_section();
        printSyntaxStart ();
        printSyntaxContin ("[ch] \"filename\"");
        printSyntaxContin ("[ch] (to abort)");
        btcli_leave_critical_section();
    }
    delete_dynstr(&filename);
}

static void recv_f(void)
{
    recvv(true);
}

static void recv_nf(void)
{
    recvv(false);
}

static void fstatt (void)
{
	u32 ch;
	
    if (getch(&ch) &&
        getDone())
    {
        if (isAclChannelOpen(ch))
        {
            /* ch given. Display specific acl channel.*/
            aclFstat(ch);
        }
        else if (fastpipeEngineIsConnected(ch))
        {
            fastpipeFstat(ch);
        }
        else
        {
            printlf("Unrecognised handle: ");
            printch(ch);
            printlf("\n");
        }
    } 
    else if (getDone())
    {
        /* No ch given. Display all channels. */
        aclFstatAll();
        fastpipeFstatAll();
    }
    else
    {
		printSyntax ("[ch]");
    }
}

static void i2c (void)
{
	u32 arg, addr, txlen = 0, rxlen = 0, restart = 1;

	if (	get (16, &addr, NULL) &&
		(getDone () ||
		 (get (16, &rxlen, NULL) &&
		  (getDone () ||
		   get (1, &restart, NULL)))))
	{
		if (rxlen > BCCMDPDU_MAXLEN - 10)
		{
			printTimestamp ();
			printlf ("i2c: len<=%i!\n", BCCMDPDU_MAXLEN - 10); /* Can't be more or else won't fit in bccmd pdu */
			return;
		}
		else
		{
			u32 pa[BCCMDPDU_MAXLEN - 1] = { 0, BCCMDPDU_SETREQ }; /* - 1 because DeviceController automatically inserts two extra words */
			txlen = 9;
			while (txlen + rxlen != sizeof (pa) / sizeof (pa[0]) && get (8, &arg, NULL))
			{
				pa[txlen++] = arg;
			}
			if (getDone ())
			{
				pa[0] = txlen + rxlen;
				pa[2] = ++bccmdseqno;
				pa[3] = BCCMDVARID_I2C_TRANSFER;
				pa[4] = addr;
				pa[5] = txlen - 9;
				pa[6] = rxlen;
				pa[7] = restart;
				pa[8] = 0;
				memset (pa + txlen, 0, rxlen);
				sendBCCMD (pa);
				return;
			}
		}
		return;
	}
	printSyntax ("addr:... [rxlen [restart:0|1 [txbyte]*]]");
}


static void e2sett (void)
{
	u32 log2_bytes, addr_mask;

	if (get (16, &log2_bytes, NULL) &&
		get (16, &addr_mask, NULL) &&
		getDone ())
	{
		PA pa = { 6, BCCMDPDU_SETREQ, 0, 0, 0, 0 };
		pa[2] = ++bccmdseqno;
		pa[3] = BCCMDVARID_E2_DEVICE;
		pa[4] = log2_bytes;
		pa[5] = addr_mask;
		sendBCCMD (pa);
		return;
	}
	printSyntax ("log2_bytes:... addr_mask:...");
}


static void e2gett (void)
{
	if (getDone ())
	{
		PA pa = { 6, BCCMDPDU_GETREQ, 0, 0, 0, 0 };
		pa[2] = ++bccmdseqno;
		pa[3] = BCCMDVARID_E2_DEVICE;
		sendBCCMD (pa);
		return;
	}
	printSyntax ("");
}


static void appsett (void)
{
	u32 arg, offset, len;

	if (get (16, &offset, NULL))
	{
		u32 pa[BCCMDPDU_MAXLEN - 2] = { 0, BCCMDPDU_SETREQ }; /* - 2 because DeviceController automatically inserts two extra words */
		len = 6;
		while (len != sizeof (pa) / sizeof (pa[0]) && get (16, &arg, NULL))
		{
			pa[len++] = arg;
		}
		if (getDone ())
		{
			pa[0] = len;
			pa[2] = ++bccmdseqno;
			pa[3] = BCCMDVARID_E2_APP_DATA;
			pa[4] = offset;
			pa[5] = len - 6;
			sendBCCMD (pa);
			return;
		}
	}
	printSyntax ("offset:... [word]*");
}

static void appgett (void)
{
	u32 offset, len = 1;

	if (get (16, &offset, NULL) &&
		(getDone () ||
		 (get (16, &len, NULL) &&
		  getDone ())))
	{
		if (len > BCCMDPDU_MAXLEN - 8)
		{
			printTimestamp ();
			printlf ("e2_app_data: len<=%i!\n", BCCMDPDU_MAXLEN - 8); /* Can't be more or else won't fit in bccmd pdu */
		}
		else
		{
			PA pa = { 6, BCCMDPDU_GETREQ, 0, 0, 0, 0 };
			pa[2] = ++bccmdseqno;
			pa[3] = BCCMDVARID_E2_APP_DATA;
			pa[4] = offset;
			pa[5] = len;
			sendBCCMD (pa);
		}
		return;
	}
	printSyntax ("offset:... [len]");
}

static void berthresht (void)
{
	u32 ch, threshold;

	if (getch (&ch) &&
		get (16, &threshold, NULL) &&
		getDone ())
	{
		PA pa = { 6, BCCMDPDU_SETREQ, 0, 0, 0, 0 };
		pa[2] = ++bccmdseqno;
		pa[3] = BCCMDVARID_BER_THRESHOLD;
		pa[4] = ch;
		pa[5] = threshold;
		sendBCCMD (pa);
		return;
	}
	printSyntax ("ch ber_threshold");
}

static void eafhh (void)
{
	u32 ch, enable;

	if (getch (&ch) &&
		get (8, &enable, disabledEnabled_d) &&
		getDone ())
	{
		PA pa = { 6, BCCMDPDU_SETREQ, 0, 0, 0, 0 };
		pa[2] = ++bccmdseqno;
		pa[3] = BCCMDVARID_ENABLE_AFH;
		pa[4] = ch;
		pa[5] = enable;
		sendBCCMD (pa);
		return;
	}
	printSyntax ("ch e:d|e");
}

static void l2cap_crch (void)
{
	u32 ch, enable, is_tx, cid;

	if (getch (&ch) &&
		get (16, &is_tx, l2capCrcRxTx_d) &&
		get (16, &cid, NULL) &&
		get (8, &enable, disabledEnabled_d) &&
		getDone ())
	{
		PA pa = { 8, BCCMDPDU_SETREQ, 0, 0, 0, 0, 0, 0 };
		pa[2] = ++bccmdseqno;
		pa[3] = BCCMDVARID_L2CAP_CRC;
		pa[4] = ch;
		pa[5] = is_tx;
		pa[6] = cid;
		pa[7] = enable;
		sendBCCMD (pa);
		return;
	}
	printSyntax ("ch tc:tx|rx cid e:d|e");
}

static void emrh (void)
{
	u32 ch, enable;

	if (getch (&ch) &&
		get (8, &enable, disabledEnabled_d) &&
		getDone ())
	{
		PA pa = { 6, BCCMDPDU_SETREQ, 0, 0, 0, 0 };
		pa[2] = ++bccmdseqno;
		pa[3] = BCCMDVARID_ENABLE_MEDIUM_RATE;
		pa[4] = ch;
		pa[5] = enable;
		sendBCCMD (pa);
		return;
	}
	printSyntax ("ch e:d|e");
}

static void lapth (void)
{
	u32 pkt;

	if (get (8, &pkt, lockedAclPacketTypes_d) &&
		getDone ())
	{
		PA pa = { 5, BCCMDPDU_SETREQ, 0, 0, 0 };
		pa[2] = ++bccmdseqno;
		pa[3] = BCCMDVARID_LOCK_ACL_PACKET_TYPE;
		pa[4] = pkt;
		sendBCCMD (pa);
		return;
	}
	printSyntax ("pt:off|dm1|dh1|dm3|dh3|dm5|dh5|2dh1|3dh1|2dh3|3dh3|2dh5|3dh5");
}

static void get_l2cap_crch (void)
{
	u32 ch, is_tx, cid;

	if (getch (&ch) &&
		get (16, &is_tx, l2capCrcRxTx_d) &&
		get (16, &cid, NULL) &&
		getDone ())
	{
		PA pa = { 8, BCCMDPDU_GETREQ, 0, 0, 0, 0, 0, 0 };
		pa[2] = ++bccmdseqno;
		pa[3] = BCCMDVARID_L2CAP_CRC;
		pa[4] = ch;
		pa[5] = is_tx;
		pa[6] = cid;
		pa[7] = 0;
		sendBCCMD (pa);
		return;
	}
	printSyntax ("ch tc:tx|rx cid");
}

static void rsl (void)
{
	PA pa = { 10, BCCMDPDU_SETREQ, 0, BCCMDVARID_REMOTE_SNIFF_LIMITS,
		  0, 0, 0, 0, 0, 0 };
	int i;
	bool ok = true;

	pa[2] = ++bccmdseqno;

	for (i = 0; i < 6; ++i)
	{
		if (!get (16, pa + 4 + i, NULL))
		{
			ok = false;
			break;
		}
	}
	if (!getDone ())
	{
		ok = false;
	}

	if (ok)
	{
		sendBCCMD (pa);
		return;
	}

	printSyntax ("maxi mini maxa mina maxt mint");
}



static void pc32 (void)
{
	u32 if_id, config32;

	if (get (16, &if_id, NULL) &&
	    get (32, &config32, NULL) &&
	    getDone ())
	{
		PA pa = { 6, BCCMDPDU_SETREQ, 0, BCCMDVARID_PCM_CONFIG32, if_id, config32 };
		pa[2] = ++bccmdseqno;
		sendBCCMD (pa);
		return;
	}
	printSyntax ("pcm_if_id  config32");
}

static void plj (void)
{
	u32 if_id, low_jitter;

	if (get (16, &if_id, NULL) &&
	    get (32, &low_jitter, NULL) &&
	    getDone ())
	{
		PA pa = { 6, BCCMDPDU_SETREQ, 0, BCCMDVARID_PCM_LOW_JITTER, if_id, low_jitter };
		pa[2] = ++bccmdseqno;
		sendBCCMD (pa);
		return;
	}
	printSyntax ("pcm_if_id  low_jitter");
}

static void prar (void)
{
    u32 port_id, sync_port, io_type, read_rate, write_rate;

	if (get (16, &port_id, NULL) &&
	    get (16, &sync_port, NULL) &&
	    get (16, &io_type, NULL) &&
	    get (32, &read_rate, NULL) &&
	    get (32, &write_rate, NULL) &&
	    getDone ())
	{
		PA pa = { 9, BCCMDPDU_SETREQ, 0, BCCMDVARID_PCM_RATE_AND_ROUTE, port_id, sync_port, io_type, read_rate, write_rate };
		pa[2] = ++bccmdseqno;
		sendBCCMD (pa);
		return;
	}
	printSyntax ("port_id  sync_port  io_type  read_rate  write_rate");
}

static void dar (void)
{
        u32 if_id, freq, samp_size;

	if ( get (16, &if_id, NULL) &&
	     get (32, &freq, NULL) &&
	     get (16, &samp_size, NULL) &&
	     getDone ())
	{
		PA pa = { 7, BCCMDPDU_SETREQ, 0, BCCMDVARID_DIGITAL_AUDIO_RATE, if_id, freq, samp_size };
		pa[2] = ++bccmdseqno;
		sendBCCMD (pa);
		return;
	}
	printSyntax ("pcm_if_id  frequency  samp_size");
}

static void dac (void)
{
	u32 if_id, options;

	if ( get (16, &if_id, NULL) &&
	     get (16, &options, NULL) &&
	     getDone ())
	{
		PA pa = { 6, BCCMDPDU_SETREQ, 0, BCCMDVARID_DIGITAL_AUDIO_CONFIG, if_id, options };
		pa[2] = ++bccmdseqno;
		sendBCCMD (pa);
		return;
	}
	printSyntax ("pcm_if_id  options");
}

static void epos (void)
{
    u32 pio, start_time, duration_time, repeat_time, start_time_fine, duration_time_fine, repeat_time_fine;

    if (get (8, & pio, NULL) &&
        get (32, & start_time, NULL) &&
        get (32, & duration_time, NULL) &&
        get (32, & repeat_time, NULL) &&
        get (16, & start_time_fine, NULL) &&
        get (16, & duration_time_fine, NULL) &&
        get (16, & repeat_time_fine, NULL) &&
        getDone ())
    {
        PA pa = { 11, BCCMDPDU_SETREQ, 0, BCCMDVARID_EGPS_PULSE_OUTPUT_START, pio, start_time, duration_time, repeat_time, start_time_fine, duration_time_fine, repeat_time_fine };

        pa[2] = ++bccmdseqno;
        sendBCCMD (pa);
        return;
    }
    printSyntax ("pio start_time duration_time repeat_time start_time_fine duration_time_fine repeat_time_fine");
}

/* Simple Pairing: Write Simple Pairing Mode */
static void wspm (void)
{
	u32 spm;
	if (get (8, &spm, spm_d) &&
		getDone ())
	{
		putCmd (1);
		put (spm);
		putDone ();
		return;
	}
	printSyntax ("spm:e|ns");
}

/* Simple Pairing: Command Complete (Read Simple Pairing Mode) */
static void rspm_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);
	printByValue (spm_d, pa[5], 8, "spm");
}

/* Simple Pairing: Command Complete (Read Local OOB Data) */
static void rlod_cc (const PA pa)
{
	int i;
	ASSERT (pa[0] == 2 + 2 + 1 + 16 + 16);

	printlf("c:0x");
	for (i = 15; i >=0; --i)
		printlf("%02x", (unsigned) pa[5 + i]);

	printlf(" r:0x");
	for (i = 15; i >=0; --i)
		printlf("%02x", (unsigned) pa[5 + 16 + i]);
}

/* Simple Pairing: Command Complete (Read Inquiry Response Transmit Power Level) */
static void rirtpl_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printlf("txp:%i", (signed char) pa[5]);
}

/* Simple Pairing: Write Inquiry Transmit Power Level */
static void witpl (void)
{
	i32 tx_power;
	if (get(32, (u32 *) &tx_power, NULL) &&
		getDone ())
	{
		if ((tx_power >= -128) && (tx_power <= 127))
		{
			putCmd (1);
			put (tx_power);
			putDone ();
			return;
		}
	}
	printSyntax ("txp: dBm");
}

/* Simple Pairing: Write Simple Pairing Debug Mode */
static void wspdm (void)
{
	u32 spdm;

	if (get (8, &spdm, spdm_d) &&
		getDone ())
	{
		putCmd (1);
		put (spdm);
		putDone ();
		return;
	}
	printSyntax ("spdm:e|d");
}

/* Simple Pairing: IO Capabilities Response */
static void iocr (void)
{
	u32 lap = lastlap, uap = lastuap, nap = lastnap;
	u32 iocap, oobp;
	u32 ar;

	if (getba (&lap, &uap, &nap) &&
		get (8, &iocap, iocap_d) &&
		get (8, &oobp, oobpresent_d) && 
		get (8, &ar, authenticationrequirements_d) && 
		getDone())
	{
		putCmd (6);
		put (lap);
		put (uap);
		put (nap);
		put (iocap);
		put (oobp);
		put (ar);
		putDone ();
		return;
	}
	printSyntax ("ba io:d|yn|kb|n oob:p|a ar:mpnrnb|mprnb|mpnrdb|mprdb|mpnrgb|mprgb");
}

/* Simple Pairing: User Confirmation Request Reply */
static void ucrr(void)
{
	u32 lap = lastlap, uap = lastuap, nap = lastnap;
	if (	getDone () ||
		(getba (&lap, &uap, &nap) &&
		 getDone ()))
	{
		putCmd (3);
		put (lap);
		put (uap);
		put (nap);
		putDone ();
		return;
	}
	printSyntaxStart ();
	printSyntaxContin ("(default: last)");
	printSyntaxContin ("ba");
}

/* Simple Pairing: User Confirmation Request Negative Reply */
static void ucrnr(void)
{
	u32 lap = lastlap, uap = lastuap, nap = lastnap;
	if (	getDone () ||
		(getba (&lap, &uap, &nap) &&
		 getDone ()))
	{
		putCmd (3);
		put (lap);
		put (uap);
		put (nap);
		putDone ();
		return;
	}
	printSyntaxStart ();
	printSyntaxContin ("(default: last)");
	printSyntaxContin ("ba");
}

/* Simple Pairing: User Passkey Request Reply */
static void uprr(void)
{
	u32 lap = lastlap, uap = lastuap, nap = lastnap;
	u32 numeric_value = 0;

	if (	getDone () ||
		(getba (&lap, &uap, &nap) &&
		 get(32, &numeric_value, NULL) &&
		 getDone ()))
	{
		putCmd (4);
		put (lap);
		put (uap);
		put (nap);
		put (numeric_value);
		putDone ();
		return;
	}
	printSyntaxStart ();
	printSyntaxContin ("(default: last 0)");
	printSyntaxContin ("ba numeric_value");
}

/* Simple Pairing: User Passkey Request Negative Reply */
static void uprnr(void)
{
	u32 lap = lastlap, uap = lastuap, nap = lastnap;

	if (	getDone () ||
		(getba (&lap, &uap, &nap) &&
		 getDone ()))
	{
		putCmd (3);
		put (lap);
		put (uap);
		put (nap);
		putDone ();
		return;
	}

	printSyntaxStart ();
	printSyntaxContin ("(default: last)");
	printSyntaxContin ("ba");
}

/* Simple Pairing: Remote OOB Data Request Reply */
static void rodrr(void)
{
	u32 lap = lastlap, uap = lastuap, nap = lastnap;
	u8 c[16];
	u8 r[16];

	memcpy (c, lastc, sizeof (c));
	memcpy (r, lastr, sizeof (r));

	if (getDone () ||
		(getba (&lap, &uap, &nap) &&
		 getLongHex (c, sizeof (c)) &&
		 getLongHex (r, sizeof (r)) &&
		 getDone ()))
	{
		memcpy (lastc, c, sizeof (c));
		memcpy (lastr, r, sizeof (r));

		putCmd (3 + sizeof (c) + sizeof (r));
		put (lap);
		put (uap);
		put (nap);
		putArray (c, sizeof (c));
		putArray (r, sizeof (r));
		putDone ();
		return;
	}

	printSyntaxStart ();
	printSyntaxContin ("(defaults: last lastc lastr)");
	printSyntaxContin ("ba c r");
}

/* Simple Pairing: Remote OOB Request Negative Reply */
static void rodrnr(void)
{
	u32 lap = lastlap, uap = lastuap, nap = lastnap;
	if (	getDone () ||
		(getba (&lap, &uap, &nap) &&
		 getDone ()))
	{
		putCmd (3);
		put (lap);
		put (uap);
		put (nap);
		putDone ();
		return;
	}

	printSyntaxStart ();
	printSyntaxContin ("(default: last)");
	printSyntaxContin ("ba");
}

/* Simple Pairing: IO Capability Request Negative Reply */
static void iocrnr(void)
{
	u32 lap = lastlap, uap = lastuap, nap = lastnap, r = 0x38;
	if (	getDone () ||
		(getba (&lap, &uap, &nap) &&
		 get (8, &r, errorCode_d) &&
		 getDone ()))
	{
		putCmd (4);
		put (lap);
		put (uap);
		put (nap);
		put (r);
		putDone ();
		return;
	}
	printSyntaxStart ();
	printSyntaxContin ("(defaults: last host_busy_pairing)");
	printSyntaxContin ("ba r:...");
}

/* Simple Pairing: Send Keypress Notification */
static void skn(void)
{
	u32 lap = lastlap, uap = lastuap, nap = lastnap;
	u32 nt;

	if (getba (&lap, &uap, &nap) &&
 		get (8, &nt, notificationtype_d) &&
		getDone ())
	{
		putCmd (4);
		put (lap);
		put (uap);
		put (nap);
		put (nt);
		putDone ();
		return;
	}

	printSyntax ("ba nt:pes|pde|pdd|pc|pec");
}

/* LE: Write Le Host Support */
static void wlehs (void)
{
	u32 lesh;
	u32 sleh;
    
	if (get (8, &lesh, lesh_d) &&
        get (8, &sleh, sleh_d) &&
		getDone ())
	{
		putCmd (2);
		put (lesh);
		put (sleh);
		putDone ();
		return;
	}
	printSyntax ("lesh:e|d sled:e|d");
}

/* LE: Command Complete (Read LE Host Support) */
static void rlehs_cc (const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1 + 1);
	printByValue (lesh_d, pa[5], 8, "lesh");
	printlf (" ");
	printByValue (sleh_d, pa[6], 8, "sleh");
}

/****/
/* BLE commands */
/****/
/* Set advertising enable */
static void lesae(void)
{
	u32 am;
	if (get (8, &am, am_d) &&
		getDone ())
	{
		putCmd (1);
		put (am);
		putDone ();
		return;
	}
	printSyntax ("e|d");
}

/* Set scan enable */
static void lesse(void)
{
	u32 sm;
        u32 filtdup;
	if (get (8, &sm, sm_d) &&
                get (8, &filtdup, filtdup_d) &&
		getDone ())
	{
		putCmd (2);
		put (sm);
                put (filtdup);
		putDone ();
		return;
	}
	printSyntax ("sm:e|d filtdup:e|d");
}


/* Receiver test */
static void lert(void)
{
	u32 rxchan;
	if( get(8, &rxchan, NULL) &&
		getDone ())
	{
		putCmd(1);
		put(rxchan);
		putDone();
		return;
	}
	printSyntax ("rxchan");
}

/* Transmitter test */
static void lett(void)
{
	u32 txchan, lentestdata, pktpayload;
	if( get(8, &txchan, NULL) &&
		get(8, &lentestdata, NULL) &&
		get(8, &pktpayload, pktpayload_d) &&
		getDone())
	{
		putCmd(3);
		put(txchan);
		put(lentestdata);
		put(pktpayload);
		putDone();
		return;
	}
	printSyntax ("txchan lengthtestdata pktpayload:prbs9|prbs15|all0|all1|altnib0|altnib1|altbit0|altbit1");
}

static void lete_cc(const PA pa)
{
	ASSERT (pa[0] == 2 + 2 + 1 + 1);

	printByValue (NULL, pa[5], 16, "num_packets");
}

/* LE Read local Supported Features */
static void lerlsf_cc (const PA pa)
{
	printLEFeatures(&pa[5], "le_features");
	printlf("\n");
}

/* Rand */
static void ler_cc (const PA pa)
{
    int i;

    printlf (" random_number:0x");

    for (i = 8; i > 0; i--)
    {
        printlf ("%.2x", pa[i+4]);
    }

}

/* Set scan parameters */
static void lessp(void)
{
	u32 st, at, si, sw, fp;
	if (get (8, &st, stype_d) &&
		get (16, &si, NULL) &&
		get (16, &sw, NULL) &&
		get (8, &at, at_d) &&
		get (8, &fp, sfp_d) &&
		getDone ())
	{
		putCmd (5);
		put (st);
		put (si);
		put (sw);
		put (at);
		put (fp);
		putDone ();
		return;
	}
	printSyntax ("st:p|a si sw at:pub|rand s_fp:all|whitelist");
}

/* Set advertising parameters */
static void lesap(void)
{
	u32 ai_min, ai_max, adv_t, at_own, at_init, lap_i, uap_i, nap_i, adv_chm, adv_fp;
	if (get (16, &ai_min, NULL) &&
		get (16, &ai_max, NULL) &&
		get (8, &adv_t, aet_d) &&
		get (8, &at_own, at_d) &&
		get (8, &at_init, at_d) &&
		getba (&lap_i, &uap_i, &nap_i) &&
		get (8, &adv_chm, sac_d) &&
		get (8, &adv_fp, afp_d) &&
		getDone ())
	{
		putCmd (10);
		put (ai_min);
		put (ai_max);
		put (adv_t);
		put (at_own);
		put (at_init);
		put (lap_i);
		put (uap_i);
		put (nap_i);
		put (adv_chm);
		put (adv_fp);
		putDone ();
		return;
	}
	printSyntax ("ai_min ai_max adv_t:cu|cd|nc|du own_at:pub|rand dir_at:pub|rand dir_addr adv_chm adv_fp:cs|c|s|w");
}

/* Create connection */
static void lecc(void)
{
	u32 si, sw, i_fp, atp, lap_pa, uap_pa, nap_pa, ato, ci_min, ci_max, cl, ct, mil, mal;

	if (get (16, &si, NULL) &&
		get (16, &sw, NULL) &&
		get (8, &i_fp, ifp_d) &&
		get (8, &atp, at_d) &&
		getba (&lap_pa, &uap_pa, &nap_pa) &&
		get (8, &ato, at_d) &&
		get (16, &ci_min, NULL) &&
		get (16, &ci_max, NULL) &&
		get (16, &cl, NULL) &&
		get (16, &ct, NULL) &&
		get (16, &mil, NULL) &&
		get (16, &mal, NULL) &&
		getDone ())
	{
		putCmd (14);
		put (si);
		put (sw);
		put (i_fp);
		put (atp);
		put (lap_pa);
		put (uap_pa);
		put (nap_pa);
		put (ato);
		put (ci_min);
		put (ci_max);
		put (cl);
		put (ct);
		put (mil);
		put (mal);
		putDone ();
		return;
	}
	printSyntax ("si sw i_fp:p|w pat pa oat ci_min ci_max cl sto mil mal");
}

/* LE_Read_White_List_Size */
/* No Command, just a command completed event handler */
static void lerwls_cc (const PA pa)
{
	printByValue (NULL, pa[5], 8, "wls");
}

/* LE_Add_Device_To_White_List */
static void leadtwl(void)
{
	u32 at;
	u32 lap, nap, uap;
	if ( get(8, &at, at_d) &&
		getba (&lap, &uap, &nap) &&
		getDone ())
	{
		putCmd (4);
		put(at);
		put (lap);
		put (uap);
		put (nap);
		putDone ();
		return;
	}
	printSyntax ("at:pub|rand ba");
}

/* LE_Remove_Device_From_White_List */
static void lerdfwl(void)
{
	u32 at;
	u32 lap, nap, uap;
	if ( get(8, &at, at_d) &&
		getba (&lap, &uap, &nap) &&
		getDone ())
	{
		putCmd (4);
		put(at);
		put (lap);
		put (uap);
		put (nap);
		putDone ();
		return;
	}
	printSyntax ("at:pub|rand ba");
}

/* Connection update */
static void lecu(void)
{
	u32 ch, interval_min, interval_max, latency, timeout, min_len, max_len;
	
	if (getsch (&ch) &&
		get (16, &interval_min, NULL) &&
		get (16, &interval_max, NULL) &&
		get (16, &latency, NULL) &&
		get (16, &timeout, NULL) &&
		get (16, &min_len, NULL) &&
		get (16, &max_len, NULL) &&
		getDone ())
	{
		putCmd (7);
		put (ch);
		put (interval_min);
		put (interval_max);
		put (latency);
		put (timeout);
		put (min_len);
		put (max_len);
		putDone ();
		return;
	}

	printSyntax ("ch interval_min interval_max latency timeout min_len max_len");
}

static void leshcc(void)
{
	u8 cm[5];
	if (getLongHex (cm, sizeof (cm)) &&
		getDone ())
	{
		putCmd (sizeof(cm));
		putArray (cm, sizeof (cm));
		putDone ();
		return;
	}
	printSyntax ("channel_map");
}

/* Set advertising data */
static void lesad(void)
{
	size_t len;
	u32 data_len;
	u8 data[31];
	u32 i;
	memset (data, 0x00, sizeof (data));
	if (!get (8, &data_len, NULL))
	{
		data_len = 0xff;
	}

	if( ( ((len = getString ((char *) data, sizeof (data))) != BADSIZE) ||
	      ((len = getLongHex (data, sizeof (data))) != 0) ) &&
	    getDone()
	  )
	{
		putCmd (1 + sizeof(data));
		put (data_len==0xff?len:data_len);
		for (i = 0; i < sizeof(data); i++)
		{
			put (data[i]);
		}
		putDone ();
		return;
	}
	printSyntax ("<length> \"data\"|hexdata");
}

/* Long term key requested reply */
static void leltkrr(void)
{
	u8 key[16];
	u32 connection_id;
	if (getsch (&connection_id) &&
		getLongHex (key, sizeof (key)) &&
		getDone ())
	{
		putCmd (1 + sizeof(key));
		put (connection_id);
		putArray (key, sizeof (key));
		putDone ();
		return;
	}
	printSyntax ("ch ltk");
}

static void leltkrr_cc (const PA pa)
{
    printch (pa[5]);
}


static void leltkrnr_cc (const PA pa)
{
    printch (pa[5]);
}

/* Start encryption */
static void lese(void)
{
    u32 connection_id;
    u8  random_number[8];
    u32  ediv;
    u8  ltk[16];

    if ( 
         getsch (&connection_id) &&
         getLongHex (random_number, 8) &&
         get (16, &ediv, NULL) &&
         getLongHex (ltk, 16) &&
         getDone())
    {
        putCmd( 26 );
        put(connection_id);
        putArray(random_number, 8);
        put(ediv);
        putArray(ltk, 16);
        putDone();
        return;
    }
        printSyntax ("ch randnum ediv ltk");
}

/* Encrypt */
static void lee(void)
{
    u8  key[16];
    u8  plain_text[16];

    if ( 
         getLongHex (key, 16) &&
         getLongHex (plain_text, 16) &&
         getDone())
    {
        putCmd( 32 );
        putArray(key, 16);
        putArray(plain_text, 16);
        putDone();
        return;
    }
        printSyntax ("key plain_text");
}

static void lee_cc (const PA pa)
{
	int i;
	//ASSERT (pa[0] == 2 + 2 + 1 + 16);
        printlf("e:0x");
	for (i = 15; i >=0; --i)
		printlf("%02x", (unsigned) pa[5 + i]);
}

/* Set scan response data */
static void lessrd(void)
{
	size_t len;
	u32 data_len;
	u8 data[31];
	u32 i;
	
	memset (data, 0x00, sizeof (data));
	if (!get (8, &data_len, NULL))
	{
		data_len = 0xff;
	}

	if( ( ((len = getString ((char *) data, sizeof (data))) != BADSIZE) ||
	      ((len = getLongHex (data, sizeof (data))) != 0) ) &&
	    getDone()
	  )
	{
		putCmd (1 + sizeof(data));
		put (data_len==0xff?len:data_len);
		for (i = 0; i < sizeof(data); i++)
		{
			put (data[i]);
		}
		putDone ();
		return;
	}
	printSyntax ("<length> \"data\"|hexdata");
}

/* Set random address */
static void lesra(void)
{
	u32 lap, nap, uap;
	if (getba (&lap, &uap, &nap) &&
		getDone ())
	{
		putCmd (3);
		put (lap);
		put (uap);
		put (nap);
		putDone ();
		return;
	}
	printSyntax ("ba");
}

static void lerbs_cc (const PA pa)
{
	printByValue (NULL, pa[5], 16, "dpl");
	printlf (" ");
	printByValue (NULL, pa[6], 8, "ndp");
}

static void lesem(void)
{
    /* sem() will not modify the opcode.  Right now, the event masks are
       the same size so this should be ok. */
    sem();
}

static void leractp_cc(const PA pa)
{
    printlf("txp:%i", (signed char) pa[5]);
}

static void lercm_cc(const PA pa)
{
    size_t i;

    printch (pa[5]);
    printlf (" map:0x");
    for (i = 5; i != 0; --i)
    {
        printlf ("%02x", (unsigned) pa[i + 5]);
    }
}

static void lerss_cc(const PA pa)
{
    ASSERT (pa[0] == 2 + 2 + 1 + 8);

    printLESupportedStates(&pa[5], "le_states");   
}

/****/
/* End BLE commands */
/****/


static void cbrr(void)
{
    /* This command pssets to the correct baudrate, warm resets, changes the
       host side uart baudrate, then restarts. */

    extern bool reconfigured_baudrate;
    extern uint32 baudrate;

    uint32 cbrr_bps;

    if (get(32, &cbrr_bps, NULL) &&
        getDone())
    {        
        reconfigured_baudrate = true;
        baudrate = cbrr_bps;
        if (!warm_reset())
        {
            printlf("Could not warm reset");
        }
        if (!reconfigure_uart_baudrate(cbrr_bps))
        {
            printlf("Reconfiguring uart baud rate not supported\n");
        }


        parseCmd("restart", false);
    }
    else
    {
        printSyntax("bps");
    }
}

static void sfp(void)
{
    uint32 ch;
    struct dynstr data;

    new_dynstr(&data, 0);

    if (get(16, &ch, NULL) &&
        (getStringDynamic (&data) != BADSIZE || 
         get_uint8_list_Dynamic (&data)) &&
        getDone())
    {
        sendFp((uint16)ch, data.length, (u8 *) data.data);
    }
    else
    {
        printSyntax("ch data");
    }
    delete_dynstr(&data);
}

static void fpi(void)
{
    if (getDone())
    {
        btcli_enter_critical_section();
        fastpipeEngineDisplayInfo();
        btcli_leave_critical_section();
    }
    else
    {
        printSyntax("");
    }
}

static void print_sendfp_syntax(void)
{
    printTimestamp();
    printSyntaxStart();
    printSyntaxContin("ch filename (to start send)");
    printSyntaxContin("ch (to cancel send)");
}

static void sendfp(void)
{
    uint32 ch;
    long file_size;
    struct dynstr file_name;
    new_dynstr(&file_name, 0);
    btcli_enter_critical_section();

    if (get(16, &ch, NULL))
    {
        if (getStringDynamicZ (&file_name) != BADSIZE &&
            getDone())
        {
            if (fastpipeFileSendEnable((u16)ch, file_name.data, &file_size))
            {
                printTimestamp();
                printlf("Sending %ld octets on ", file_size);
                printch(ch);
                printlf("\n");
                fastpipeFileSendStart((u16)ch);
            }
            else
            {
                printTimestamp();
                printlf("Unable to send file\n");
            }
        }
        else if (getDone())
        {
            if (fastpipeFileSendDisable((u16)ch))
            {
                printTimestamp();
                printlf("Cancelled file send\n");
            }
            else
            {
                printTimestamp();
                printlf("Unable to cancel file send\n");
            }
        }
        else
        {
            print_sendfp_syntax();
        }
    }
    else
    {
        print_sendfp_syntax();
    }
    btcli_leave_critical_section();
}

static void print_recvfp_syntax(void)
{
    printTimestamp();
    printSyntaxStart();
    printSyntaxContin("ch filename (to start receive)");
    printSyntaxContin("ch (to cancel receive)");
}

static void recvfp(void)
{
    uint32 ch;
    struct dynstr file_name;
    new_dynstr(&file_name, 0);

    btcli_enter_critical_section();
    if (get(16, &ch, NULL))
    {
        if (getStringDynamicZ (&file_name) != BADSIZE &&
            getDone())
        {
            printlf("Enabling file receive: %s\n",
                    lookupByValue(fastpipe_engine_result_d, 
                                  fastpipeEngineFileReceiveEnable(ch, file_name.data)));
        }
        else if (getDone())
        {
            printlf("Disabling file receive: %s\n",
                    lookupByValue(fastpipe_engine_result_d,
                                  fastpipeEngineFileReceiveDisable(ch)));
        }
        else
        {
            print_recvfp_syntax();
        }
    }
    else
    {
        print_recvfp_syntax();
    }

    btcli_leave_critical_section();
}

static void rfpl(void)
{
    uint32 reduction; // The amount to reduce limitc by.

    btcli_enter_critical_section();
    if (get(32, &reduction, NULL) &&
        getDone())
    {
        printlf("Reducing limitc: %s\n",
                lookupByValue(fastpipe_engine_result_d,
                              fastpipeEngineReduceLimitc(reduction)));
    }
    else
    {
        printTimestamp();
        printSyntax("reduction");
    }
    btcli_leave_critical_section();
}

static void wcit(void)
{
	u32 m, v;

	if (get (8, &m, LwctMode_d) &&
	    get (8, &v, LwctValue_d) &&
	    getDone ())
	{
		PA pa = { 6, BCCMDPDU_SETREQ, 0, BCCMDVARID_COEX_INT_TEST, m, v };
		pa[2] = ++bccmdseqno;
		sendBCCMD (pa);
		return;
	}
	printSyntax ("mode:START|WRITE_DIR|WRITE_LVL|READ|STOP value:0x00|0x01|0x02|0x03|0x04|0x05|0x06|0x07");
}

static void fpc (void)
{
	u32 handle;
	u32 pwr_cmd;

	if (   getch(&handle)
	    && get (16, &pwr_cmd, force_power_control_request_d) 
	    && getDone ()
	    && (pwr_cmd <= getMax(force_power_control_request_d)))
	{
		PA pa = { 26, TUNNEL, LMP_DEBUG_CMD, HCI_DEBUG_FORCE_POWER_CONTROL, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		/* Can't see it documented elsewhere, so add it here...
		 * Although PA is u32, it is sent as u8 on the link (go figure)
		 * so need to populate as below */
		pa[8]  = handle;        /* Populate channel (num0) */
		pa[9]  = handle>>8;
		pa[10]  = pwr_cmd;      /* and num1 */
		pa[11]  = 0;

		sendCmd (pa);
		return;
	}
	printSyntax("ch pwr:automatic|manual|up|down|max|reset");
}

static void sgso (void )
{
    u32 source, opt1, opt2;

    if (get (16, &source, stream_source_sink_d) &&
        get (16, &opt1, NULL) &&
        get (16, &opt2, NULL) &&
        getDone ())
    {
        PA pa = { 7, BCCMDPDU_SETREQ, 0, BCCMDVARID_STREAM_GET_SOURCE,
                  source, opt1, opt2 };
        pa[2] = ++bccmdseqno;
        sendBCCMD (pa);
        return;
    }
    printSyntax ("source:pcm|i2s|codec|fm|spdif|dig_mic|fastpipe opt1 opt2");
}

static void sgsi (void )
{
    u32 sink, opt1, opt2;

    if (get (16, &sink, stream_source_sink_d) &&
        get (16, &opt1, NULL) &&
        get (16, &opt2, NULL) &&
        getDone ())
    {
        PA pa = { 7, BCCMDPDU_SETREQ, 0, BCCMDVARID_STREAM_GET_SINK,
                  sink, opt1, opt2 };
        pa[2] = ++bccmdseqno;
        sendBCCMD (pa);
        return;
    }
    printSyntax ("sink:pcm|i2s|codec|fm|spdif|a2dp|fastpipe opt1 opt2");
}

static void scso (void )
{
    u32 sid;

    if (get (16, &sid, NULL) &&
        getDone ())
    {
        PA pa = { 5, BCCMDPDU_SETREQ, 0, BCCMDVARID_STREAM_CLOSE_SOURCE, sid };
        pa[2] = ++bccmdseqno;
        sendBCCMD (pa);
        return;
    }
    printSyntax ("sid");
}

static void scsi (void )
{
    u32 sid;

    if (get (16, &sid, NULL) &&
        getDone ())
    {
        PA pa = { 5, BCCMDPDU_SETREQ, 0, BCCMDVARID_STREAM_CLOSE_SINK, sid };
        pa[2] = ++bccmdseqno;
        sendBCCMD (pa);
        return;
    }
    printSyntax ("sid");
}

static void scfg (void )
{
    u32 sid, key, value;

    if (get (16, &sid, NULL) &&
        get (16, &key, stream_config_d) &&
        get (32, &value, NULL) &&
        getDone ())
    {
        PA pa = { 7, BCCMDPDU_SETREQ, 0, BCCMDVARID_STREAM_CONFIGURE,
                  sid, key, value };
        pa[2] = ++bccmdseqno;
        sendBCCMD (pa);
        return;
    }
    printSyntax ("sid key value");
}

static void sas (void )
{
    u32 sid1, sid2;

    if (get (16, &sid1, NULL) &&
        get (16, &sid2, NULL) &&
        getDone ())
    {
        PA pa = { 6, BCCMDPDU_SETREQ, 0, BCCMDVARID_STREAM_ALIAS_SINK,
                  sid1, sid2 };
        pa[2] = ++bccmdseqno;
        sendBCCMD (pa);
        return;
    }
    printSyntax ("sid sid");
}

static void scon (void )
{
    u32 source_sid, sink_sid;

    if (get (16, &source_sid, NULL) &&
        get (16, &sink_sid, NULL) &&
        getDone ())
    {
        PA pa = { 6, BCCMDPDU_SETREQ, 0, BCCMDVARID_STREAM_CONNECT,
                  source_sid, sink_sid };
        pa[2] = ++bccmdseqno;
        sendBCCMD (pa);
        return;
    }
    printSyntax ("source_sid sink_sid");
}

static void ssync (void )
{
    u32 sid1, sid2;

    if (get (16, &sid1, NULL) &&
        get (16, &sid2, NULL) &&
        getDone ())
    {
        PA pa = { 6, BCCMDPDU_SETREQ, 0, BCCMDVARID_STREAM_SYNC_SID,
                  sid1, sid2 };
        pa[2] = ++bccmdseqno;
        sendBCCMD (pa);
        return;
    }
    printSyntax ("sync_sid1 sync_sid2");
}


static void sdis (void )
{
    u32 transform;

    if (get (16, &transform, NULL) &&
        getDone ())
    {
        PA pa = { 5, BCCMDPDU_SETREQ, 0, BCCMDVARID_STREAM_TRANSFORM_DISCONNECT,
                  transform };
        pa[2] = ++bccmdseqno;
        sendBCCMD (pa);
        return;
    }
    printSyntax ("transform_id");
}

static void sts (void )
{
    if (getDone ())
    {
        PA pa = { 4, BCCMDPDU_SETREQ, 0, BCCMDVARID_STREAM_TRANSACTION_START };
        pa[2] = ++bccmdseqno;
        sendBCCMD (pa);
    }
}

static void stc (void )
{
    if (getDone ())
    {
        PA pa = { 4, BCCMDPDU_SETREQ, 0, BCCMDVARID_STREAM_TRANSACTION_COMPLETE };
        pa[2] = ++bccmdseqno;
        sendBCCMD (pa);
    }
}

static void stca (void )
{
    if (getDone ())
    {
        PA pa = { 4, BCCMDPDU_SETREQ, 0, BCCMDVARID_STREAM_TRANSACTION_CANCEL };
        pa[2] = ++bccmdseqno;
        sendBCCMD (pa);
    }
}

static void msa (void )
{
    u32 source_sid, sink_sid;

    if (get (16, &source_sid, NULL) &&
        get (16, &sink_sid, NULL) &&
        getDone ())
    {
        PA pa = { 6, BCCMDPDU_SETREQ, 0, BCCMDVARID_MAP_SCO_AUDIO,
                  source_sid, sink_sid };
        pa[2] = ++bccmdseqno;
        sendBCCMD (pa);
        return;
    }
    printSyntax ("source_sid sink_sid");
}

static void ess (void )
{
    u32 enable;

    if (get (16, &enable, NULL) &&
        getDone ())
    {
        PA pa = { 5, BCCMDPDU_SETREQ, 0, BCCMDVARID_ENABLE_SCO_STREAMS,
                  enable };
        pa[2] = ++bccmdseqno;
        sendBCCMD (pa);
        return;
    }
    printSyntax ("1(enable) or 0(disable)");
}

static void pcr (void )
{
    u32 pcm_interface, clock_rate;

    if (get (16, &pcm_interface, NULL) &&
        get (32, &clock_rate, NULL) &&
        getDone ())
    {
        PA pa = { 6, BCCMDPDU_SETREQ, 0, BCCMDVARID_PCM_CLOCK_RATE,
                  pcm_interface, clock_rate };
        pa[2] = ++bccmdseqno;
        sendBCCMD (pa);
        return;
    }
    printSyntax ("interface clock_rate");
}

static void pspf (void )
{
    u32 pcm_interface, slots;

    if (get (16, &pcm_interface, NULL) &&
        get (16, &slots, NULL) &&
        getDone ())
    {
        PA pa = { 6, BCCMDPDU_SETREQ, 0, BCCMDVARID_PCM_SLOTS_PER_FRAME,
                  pcm_interface, slots };
        pa[2] = ++bccmdseqno;
        sendBCCMD (pa);
        return;
    }
    printSyntax ("interface slots");
}

static void psr (void )
{
    u32 pcm_interface, sync_rate;

    if (get (16, &pcm_interface, NULL) &&
        get (32, &sync_rate, NULL) &&
        getDone ())
    {
        PA pa = { 6, BCCMDPDU_SETREQ, 0, BCCMDVARID_PCM_SYNC_RATE,
                  pcm_interface, sync_rate };
        pa[2] = ++bccmdseqno;
        sendBCCMD (pa);
        return;
    }
    printSyntax ("interface sync_rate");
}


static void h4raw (void)
{
	u32 varid;
	uint8 *buf = NULL;
	size_t len = 0;
	bool syntax_error = false;

	while(!syntax_error && !getDone())
	{
		u32 val;
		u32 multiplicity;
		if(getNumericMultiple(8, &val, &multiplicity))
		{
			u32 i;
			uint8 *new_buf;

			new_buf = realloc(buf, (len + multiplicity)*sizeof(uint8));
			if(!new_buf)
			{
				free(buf);
				printlf("out of memory\n");
				return;
			}
			buf = new_buf;

			for(i = 0; i < multiplicity; ++i) buf[len+i] = (uint8) val;
			len += multiplicity;
		}
		else
		{
			syntax_error = true;
		}
	}

	if(len > 0 && !syntax_error)
	{
		sendH4Raw(buf, len);
	}
	else
	{
		printSyntax("{ uint8 | uint8 * N } ...");
	}
	free(buf);
}

/* Set Pin Function (spf): Function to Handle 
 * spf command
 */
static void spf(void)
{
    u32 pin_number, function;
    if(get(16, &pin_number, NULL) 
       && get(16, &function, SetPinFunction_d) 
       && getDone())
    {
        PA pa = {6, BCCMDPDU_SETREQ, 0, 
                    BCCMDVARID_PIO32_SET_PIN_FUNCTION, pin_number, function};
        pa[2] = ++bccmdseqno;
        sendBCCMD(pa);
        return;
    }
    printSyntax("pin function");
}

static void micbiasctrl (void)
{
    u32 enable, current, voltage, low_power;

    if (get (16, &enable, NULL) &&
        get (16, &current, mic_bias_configure_d) &&
        get (16, &voltage, NULL) &&
		get (16, &low_power, NULL) &&
        getDone ())
    {
        PA pa = { 8, BCCMDPDU_SETREQ, 0, BCCMDVARID_MIC_BIAS_CTRL,
                  enable, current, voltage, low_power };
        pa[2] = ++bccmdseqno;
        sendBCCMD (pa);
        return;
    }
    printSyntax ("enable current voltage low_power");
}

static void micbiascfg (void)
{
    u32 instance, key, value;

    if (get (16, &instance, NULL) &&
        get (16, &key, mic_bias_configure_d) &&
        get (16, &value, NULL) &&
        getDone ())
    {
        PA pa = { 7, BCCMDPDU_SETREQ, 0, BCCMDVARID_MIC_BIAS_CONFIGURE,
                  instance, key, value };
        pa[2] = ++bccmdseqno;
        sendBCCMD (pa);
        return;
    }
    printSyntax ("instance key value");
}

static void ledcfg (void)
{
    u32 led, led_key, value;

    if (get (16, &led, NULL) &&
        get (16, &led_key, led_config_d) &&
        get (16, &value, NULL) &&
        getDone ())
    {
        PA pa = { 7, BCCMDPDU_SETREQ, 0, BCCMDVARID_LED_CONFIG,
                  led, led_key, value };
        pa[2] = ++bccmdseqno;
        sendBCCMD (pa);
        return;
    }
    printSyntax ("led led_key value");
}

static void siflash (void)
{
    u32 cmd,  addr=0, length=0, pdu_len = 0;
    u32 pa[BCCMDPDU_MAXLEN - 2] = {0, BCCMDPDU_SETREQ, 0};
    if (get (16, &cmd, siflash_d))
    {
        if (BCCMDPDU_SIFLASH_CHIP_ERASE_CMD == cmd)
        {
            pdu_len = 7;
        }
        else
        {
            if( get (32, &addr, NULL) && get (16, &length, NULL))
            {
                switch (cmd)
                {
                    case BCCMDPDU_SIFLASH_WRITE_CMD:
                        {
                            u32 arg, txlen = 7;
                            /* check for data length for read or write data */
                            if (length > BCCMDPDU_MAXLEN - 9)
                            {
                                printTimestamp ();
                                /* Can't be more or else won't fit in bccmd pdu */
                                printlf ("siflash: len<=%i!\n", BCCMDPDU_MAXLEN - 9);
                                return;
                            }
                            /* copy the data from the command line */
                            while (length  != 
                                sizeof (pa) / sizeof (pa[0]) && get (16, &arg, NULL))
                            {
                                pa[txlen++] = arg;
                            }
                            pdu_len = txlen;
                        }
                        break;
                    case BCCMDPDU_SIFLASH_READ_CMD:
                        {
                            if (length > BCCMDPDU_MAXLEN - 9)
                            {
                                printTimestamp ();
                                /* Can't be more or else won't fit in bccmd pdu */
                                printlf ("siflash: len<=%i!\n", BCCMDPDU_MAXLEN - 9);
                                return;
                            }
                            /* assign the length such that these many words are reserved to read */
                            pdu_len = length + 7;
                        }
                        break;
                    case BCCMDPDU_SIFLASH_SECTOR_ERASE_CMD:
                    case BCCMDPDU_SIFLASH_BLOCK64_ERASE_CMD:
                        pdu_len = 7;
                        break;
                    default:
                        printlf ("siflash: unknown command, for help just enter siflash ");
                        return;
                        break;
                }
            }
        }
    }
    if (getDone () && pdu_len)
    {
        pa[0] = pdu_len;
        pa[2] = ++bccmdseqno;
        pa[3] = BCCMDVARID_SIFLASH;
        pa[4] = cmd;
        pa[5] = addr;
        pa[6] = length;
        sendBCCMD (pa);
        return;
    }
    else
    {
        printSyntax (" cmd [word_address word_length word_data] \n" 
                  "Following are the commands to use \n"
                  "*Read data: siflash [read | 0] word_addr word_len \n"
                  "*Write data: siflash [write | 1] word_addr word_len word_data[0]...word_data[word_len-1]\n"
                  "*Erase complete chip: siflash [chip_erase | 2] \n"
                  "*Sector erase: siflash [sector_erase | 3] word_addr number_of_sectors \n"
                  "*Block64 erase: siflash [block64_erase | 4] word_addr number_of_block64 \n ");
    }
}

static bool stibbons_list (u32 varid)
{
    u32 pa [BCCMDPDU_MAXLEN] = { 0, BCCMDPDU_SETREQ, 0, varid };
    u32 opid;
    size_t n = 5;

    while (get (16, &opid, NULL) && n < BCCMDPDU_MAXLEN)
        pa [n++] = opid;
    if (getDone ())
    {
        pa [0] = n;
        pa [2] = ++bccmdseqno;
        pa [4] = n - 5;
        sendBCCMD (pa);
        return true;
    }
    return false;
}

static void stibbons_vector (u32 varid)
{
    if (!stibbons_list (varid))
        printSyntax ("opid ...");
}

static void startop (void)
{
    stibbons_vector (BCCMDVARID_START_OPERATOR);
}

static void stopop (void)
{
    stibbons_vector (BCCMDVARID_STOP_OPERATOR);
}

static void rstop (void)
{
    stibbons_vector (BCCMDVARID_RESET_OPERATOR);
}

static void dop (void)
{
    stibbons_vector (BCCMDVARID_DESTROY_OPERATOR);
}

static void cop_c (void)
{
    u32 capid, n_patches, skip_count;

    if (get (16, &capid, NULL) &&
        get (16, &n_patches, NULL) &&
        get (16, &skip_count, NULL) &&
        getDone ())
    {
        PA pa = { 8, BCCMDPDU_SETREQ, 0, BCCMDVARID_CREATE_OPERATOR_C,
                  capid, n_patches, skip_count };
        pa[2] = ++bccmdseqno;
        sendBCCMD (pa);
        return;
    }
    printSyntax ("capid n_patches skip_count");
}

static void cop_p (void)
{
    if (!stibbons_list (BCCMDVARID_CREATE_OPERATOR_P))
        printSyntax ("patch ...");
}

static void opmsg (void)
{
    u32 opid;

    if (get (16, &opid, NULL))
    {
        u32 pa [BCCMDPDU_MAXLEN] = { 0, BCCMDPDU_SETREQ, 0,
                                     BCCMDVARID_OPERATOR_MESSAGE, opid };
        size_t n = 5;
        u32 msg;

        while (get (16, &msg, NULL) && n < BCCMDPDU_MAXLEN)
            pa [n++] = msg;
        if (getDone ())
        {
            pa [0] = n;
            pa [2] = ++bccmdseqno;
            sendBCCMD (pa);
            return;
        }
    }
    printSyntax ("opid message...");
}


#define hcir(x) x, x ## _cc, #x		/* Command with parameters and result */
#define hcirba(x) x, ba_cc, #x		/* Command with parameters and BD_ADDR result */
#define hci(x) x, NULL, #x		/* Command with parameters but no result (or just status) */
#define hcicr(x) chpar, x ## _cc, #x	/* Command with connection handle parameter only and result */
#define hcic(x) chpar, NULL, #x		/* Command with connection handle parameter only but no result (or just status) */
#define hci0r(x) nopar, x ## _cc, #x	/* Command without parameters but with result */
#define hci0rba(x) nopar, ba_cc, #x	/* Command without parameters but with BD_ADDR result */
#define hci0(x) nopar, NULL, #x		/* Command without parameters or result (or just status) */
#define cmd(x) NOTHCICOMMAND, x, NULL, #x	/* Non-HCI command */
#define marker(x) NOTHCICOMMAND, NULL, NULL, x, x    /* Special marker (the name MUST contain a space) */


typedef struct HCIcommandTableElement
{
	const unsigned opcode;
	void (*const parser) (void);
	void (*const decoder_cc) (const PA);
	const char *abbnam;
	const char *fullname;
} HCIcommandTableElement;

typedef HCIcommandTableElement HCIcommandTable[];

static HCIcommandTable hciCommandTable =
{
	/* A bit of a misnomer since it also contains non-HCI commands! */
	0x0000, hci	(nop),	"nop", /* Keep this first */
	0x0401, hci	(i),	"inquiry",
	0x0402, hci0	(ic),	"inquiry_cancel",
	0x0403, hci	(pim),	"periodic_inquiry_mode",
	0x0404, hci0	(epim),	"exit_periodic_inquiry_mode",
	0x0405, hci	(cc),	"create_connection",
	0x0405, hci	(ccs),	"create_connection_and_switch",
	0x0406, hci	(d),	"disconnect",
	0x0407, hci	(asc),	"add_sco_connection",
	0x0408, hcirba	(ccc),	"create_connection_cancel",
	0x0409, hci	(acr),	"accept_connection_request",
	0x0409, hci	(acrs),	"accept_connection_request_and_switch",
	0x040a, hci	(rcr),	"reject_connection_request",
	0x040b, hcirba	(lkrr),	"link_key_request_reply",
	0x040c, hcirba	(lkrnr),"link_key_request_negative_reply",
	0x040d, hcirba	(pcrr),	"pin_code_request_reply",
	0x040e, hcirba	(pcrnr),"pin_code_request_negative_reply",
	0x040f, hci	(ccpt),	"change_connection_packet_type",
	0x0411, hcic	(ar),	"authentication_requested",
	0x0413, hci	(sce),	"set_connection_encryption",
	0x0415, hcic	(cclk),	"change_connection_link_key",
	0x0417, hci	(mlk),	"master_link_key",
	0x0419, hci	(rnr),	"remote_name_request",
	0x041a, hcirba	(rnrc),	"remote_name_request_cancel",
	0x041b, hcic	(rrsf),	"read_remote_supported_features",
	0x041c, hci	(rref),	"read_remote_extended_features",
	0x041d, hcic	(rrvi),	"read_remote_version_information",
	0x041f, hcic	(rco),	"read_clock_offset",
	0x0420, hcir	(rlh),	"read_lmp_handle",
	0x0428, hci	(ssc),	"setup_synchronous_connection",
	0x0429, hci	(ascr),	"accept_synchronous_connection_request",
	0x042a, hci	(rscr),	"reject_synchronous_connection_request",
	0x042b, hcirba  (iocr), "io_capability_response", 
	0x042c, hcirba	(ucrr),	"user_confirmation_request_reply",
	0x042d, hcirba	(ucrnr),"user_confirmation_request_negative_reply",
	0x042e, hcirba	(uprr),	"user_passkey_request_reply",
	0x042f, hcirba	(uprnr),"user_passkey_request_negative_reply",
	0x0430, hcirba  (rodrr),"remote_oob_data_request_reply",
	0x0433, hcirba  (rodrnr),"remote_oob_data_request_negative_reply",
	0x0434, hcirba  (iocrnr),"io_capability_request_negative_reply",

	0x0801, hci	(hm),	"hold_mode",
	0x0803, hci	(sm),	"sniff_mode",
	0x0804, hcic	(esm),	"exit_sniff_mode",
	0x0805, hci	(pm),	"park_mode",
	0x0806, hcic	(epm),	"exit_park_mode",
	0x0807, hci	(qs),	"qos_setup",
	0x0809, hcicr	(rd),	"role_discovery",
	0x080b, hci	(sr),	"switch_role",
	0x080c, hcicr	(rlps),	"read_link_policy_settings",
	0x080d, hcir	(wlps),	"write_link_policy_settings",
	0x080e, hci0r	(rdlps),"read_default_link_policy_settings",
	0x080f, hci	(wdlps),"write_default_link_policy_settings",
	0x0810, hci	(fs),	"flow_specification",
	0x0811, hcir	(sssr),	"set_sniff_sub_rate",

	0x0c01, hci	(sem),	"set_event_mask",
	RESET,	hci0r	(r),	"reset",
	0x0c05, hci	(sef),	"set_event_filter",
	0x0c08, hcicr	(f),	"flush",
	0x0c09, hci0r	(rpint),"read_pin_type",
	0x0c0a, hci	(wpint),"write_pin_type",
	0x0c0b, hci0	(cnuk),	"create_new_unit_key",
	0x0c0d, hcir	(rslk),	"read_stored_link_key",
	0x0c11, hcir	(wslk),	"write_stored_link_key",
	0x0c12,	hcir	(dslk),	"delete_stored_link_key",
	0x0c13, hci	(cln),	"change_local_name",
	0x0c14, hci0r	(rln),	"read_local_name",
	0x0c15, hci0r	(rcat),	"read_connection_accept_timeout",
	0x0c16, hci	(wcat),	"write_connection_accept_timeout",
	0x0c17, hci0r	(rpt),	"read_page_timeout",
	0x0c18, hci	(wpt),	"write_page_timeout",
	0x0c19, hci0r	(rse),	"read_scan_enable",
	0x0c1a, hci	(wse),	"write_scan_enable",
	0x0c1b, hci0r	(rpsa),	"read_page_scan_activity",
	0x0c1c, hci	(wpsa),	"write_page_scan_activity",
	0x0c1d, hci0r	(risa),	"read_inquiry_scan_activity",
	0x0c1e, hci	(wisa),	"write_inquiry_scan_activity",
	0x0c1f, hci0r	(rae),	"read_authentication_enable",
	0x0c20, hci	(wae),	"write_authentication_enable",
	0x0c21, hci0r	(rem),	"read_encryption_mode",
	0x0c22, hci	(wem),	"write_encryption_mode",
	0x0c23, hci0r	(rcod),	"read_class_of_device",
	0x0c24, hci	(wcod),	"write_class_of_device",
	0x0c25, hci0r	(rvs),	"read_voice_setting",
	0x0c26, hci	(wvs),	"write_voice_setting",
	0x0c27, hcicr	(raft),	"read_automatic_flush_timeout",
	0x0c28, hcir	(waft),	"write_automatic_flush_timeout",
	0x0c29, hci0r	(rnbr),	"read_num_broadcast_retransmissions",
	0x0c2a, hci	(wnbr),	"write_num_broadcast_retransmissions",
	0x0c2b, hci0r	(rhma),	"read_hold_mode_activity",
	0x0c2c, hci	(whma),	"write_hold_mode_activity",
	0x0c2d, hcir	(rtpl),	"read_transmit_power_level",
	0x0c2e, hci0r	(rsfce),"read_sco_flow_control_enable",
	0x0c2f, hci	(wsfce),"write_sco_flow_control_enable",
	0x0c31, hci	(shcthfc), "set_host_controller_to_host_flow_control",
	0x0c33, hci	(hbs),	"host_buffer_size",
	0x0c35, hci	(hnocp),"host_number_of_completed_packets",
	0x0c36, hcicr	(rlst),	"read_link_supervision_timeout",
	0x0c37, hcir	(wlst),	"write_link_supervision_timeout",
	0x0c38, hci0r	(rnosi),"read_number_of_supported_iac",
	0x0c39, hci0r	(rcil),	"read_current_iac_lap",
	0x0c3a, hci	(wcil),	"write_current_iac_lap",
	0x0c3b, hci0r	(rpspm),"read_page_scan_period_mode",
	0x0c3c, hci	(wpspm),"write_page_scan_period_mode",
	0x0c3d, hci0r	(rpsm),	"read_page_scan_mode",
	0x0c3e, hci	(wpsm),	"write_page_scan_mode",
	0x0c3f, hci	(sacc),	"set_afh_channel_classification",
	0x0c42, hci0r	(rist),	"read_inquiry_scan_type",
	0x0c43, hci	(wist),	"write_inquiry_scan_type",
	0x0c44, hci0r	(rim),	"read_inquiry_mode",
	0x0c45, hci	(wim),	"write_inquiry_mode",
	0x0c46, hci0r	(rpst),	"read_page_scan_type",
	0x0c47, hci	(wpst),	"write_page_scan_type",
	0x0c48, hci0r	(raccm),"read_afh_channel_classification_mode",
	0x0c49, hci	(waccm),"write_afh_channel_classification_mode",
	0x0c51, hci0r	(reird),"read_extended_inquiry_response_data",
	0x0c52, hci	(weird),	"write_extended_inquiry_response_data",
	0x0c53, hcic (rek),		"refresh_encryption_key",
	0x0c55, hci0r (rspm),	"read_simple_pairing_mode", 
	0x0c56, hci (wspm),		"write_simple_pairing_mode", 
	0x0c57, hci0r (rlod),	"read_local_oob_data",
	0x0c58, hci0r (rirtpl),	"read_inquiry_response_transmit_power_level",
	0x0c59, hci (witpl),	"write_inquiry_transmit_power_level",
	0x0c5f, hcir	(ef),	"enhanced_flush",
	0x0c60, hcirba (skn),   "send_keypress_notification",
	0x0c68, hcir   (retpl),	"read_enhanced_transmit_power_level",
	0x0c6c, hci0r (rlehs),  "read_le_host_support", 
	0x0c6d, hci (wlehs),    "write_le_host_support", 

	0x1001, hci0r	(rlvi),	"read_local_version_information",
	0x1002, hci0r	(rlsc),	"read_local_supported_commands",
	0x1003, hci0r	(rlsf),	"read_local_supported_features",
	0x1004, hcir	(rlef),	"read_local_extended_features",
	0x1005, hci0r	(rbs),	"read_buffer_size",
	0x1007, hci0r	(rcc),	"read_country_code",
	0x1009, hci0rba	(rba),	"read_bd_addr",

	0x1401, hcicr	(rfcc),	"read_failed_contact_counter",
	0x1402, hcicr	(rstfcc), "reset_failed_contact_counter",
	0x1403, hcicr	(glq),	"get_link_quality",
	0x1405, hcicr	(rr),	"read_rssi",
	0x1406, hcicr	(racm),	"read_afh_channel_map",
	0x1407, hcir	(rc),	"read_clock",
	0x1408, hcicr   (reks), "read_encryption_key_size",

	0x1801, hci0r	(rlm),  "read_loopback_mode",
	0x1802, hci	(wlm),	"write_loopback_mode",
	0x1803, hci0	(edutm),"enable_device_under_test_mode",
	0x1804, hci	(wspdm),"write_simple_pairing_debug_mode",

    0x2001, hci (lesem), "le_set_event_mask",
	0x2002, hci0r (lerbs), "le_read_buffer_size",
	0x2003, hci0r (lerlsf), "le_read_local_supported_features",
	0x2005, hci (lesra), "le_set_random_address",
	0x2006, hci (lesap), "le_set_advertising_parameters",
    0x2007, hci0r (leractp), "le_read_advertising_channel_tx_power",
	0x2008, hci (lesad), "le_set_advertising_data",
	0x2009, hci (lessrd), "le_set_scan_response_data",
	0x200a, hci (lesae), "le_set_advertise_enable",
	0x200b, hci (lessp), "le_set_scan_parameters",
	0x200c, hci (lesse), "le_set_scan_enable",
	0x200d, hci (lecc), "le_create_connection",
	0x200e, hci0 (leccc), "le_create_connection_cancel",
	0x200f, hci0r   (lerwls),  "le_read_white_list_size",
	0x2010, hci0    (lecwl),   "le_clear_white_list",
	0x2011, hci     (leadtwl), "le_add_device_to_white_list",
	0x2012, hci     (lerdfwl), "le_remove_device_from_white_list",
	0x2013, hci (lecu), "le_connection_update",
	0x2014, hci (leshcc), "le_set_host_channel_classification",
    0x2015, hcicr (lercm), "le_read_channel_map",
	0x2016, hcic (lerruf), "le_read_remote_used_features",
	0x2017, hcir (lee), "le_encrypt",
	0x2018, hci0r(ler), "le_rand",
	0x2019, hci (lese), "le_start_encryption",
	0x201a, hcir (leltkrr), "le_long_term_key_requested_reply",
	0x201b, hcicr (leltkrnr), "le_long_term_key_requested_negative_reply",
	0x201c, hci0r (lerss), "le_read_supported_states",
	0x201d, hci (lert), "le_receiver_test",
	0x201e, hci (lett), "le_transmitter_test",
	0x201f, hci0r (lete), "le_test_end",
	
		/* neilp */
		cmd (saip), "switch_to_alt_in_pipe",
		cmd (ssip), "switch_to_std_in_pipe",
		cmd (saop), "switch_to_alt_out_pipe",
		cmd (ssop), "switch_to_std_out_pipe",
		cmd	(raw),	"send_command_raw",
		cmd	(help),	"help",
		cmd	(find),	"find_command",
		cmd	(acl),	"send_acl_data",
		cmd	(aclab),"send_acl_active_broadcast_data",
		cmd	(aclpb),"send_acl_piconet_broadcast_data",
		cmd	(aclr),	"send_acl_data_raw",
		cmd	(ping),	"send_acl_ping",
		NOTHCICOMMAND, send_f, NULL, "send", "send_file",
        NOTHCICOMMAND, send_nf, NULL, "sendnf", "send_file_non_flushable",
		NOTHCICOMMAND, recv_f, NULL, "recv", "receive_file",
        NOTHCICOMMAND, recv_nf, NULL, "recvnf", "receive_file_not_flushable",
		NOTHCICOMMAND, fstatt, NULL, "fstat", "show_file_transfer_status",
		cmd	(sco),	"send_sco_data",
#ifdef BTCLI_SENDSCO_SUPPORTABLE
		cmd	(sends),"send_sco_file",
#endif
		cmd	(set),	"set_variable",
		cmd	(sym),	"show_symbols",
		cmd	(slave),"prepare_slave",
		cmd	(msp),	"bcset_map_sco_pcm_1",
		cmd	(settran), "psset_transport",
		NOTHCICOMMAND, sleepp, NULL, "sleep", "pause",
		cmd	(sys),	"execute_system_command",
		cmd	(opt),	"set_option",
		cmd	(echo),	"echo",
#ifdef MGR
		cmd	(wait),	"wait_connection_handle",
		cmd	(waitn),"wait_not_connection_handle",
#endif
		cmd	(hcconn), "host_controller_connection_information",
		cmd	(hcmem),  "host_controller_memory_information",

		cmd	(smr),	"scatter_request",
		cmd	(usr),	"unscatter_request",

		cmd	(ssr),	"set_subrate",
		cmd	(ppe),	"presence_point_extension",

		cmd	(aa),	"add_absence",
		cmd	(da),	"delete_absence",

		cmd	(ep),	"esco_ping",


		cmd	(sb),	"set_btclock",
		cmd	(br),	"block_rx",

		cmd	(lt),	"lmp_tx",
		cmd	(ls),	"lm_stop",
		cmd	(lg),	"lm_start",
		cmd	(ltc),	"lmp_test_control",
		cmd	(lta),	"lmp_test_activate",
		cmd	(smj),	"set_master_jitter",
		cmd	(exec),	"execute_script",
		cmd	(vm_data), "send_vm_data",
		cmd	(bcget),"bccmd_getreq",
		cmd	(bcset),"bccmd_setreq",
		cmd	(bcfind), "bccmd_find_varid",
		cmd	(rt),	"bccmd_setreq_radiotest",
		cmd	(rtfind), "bccmd_find_radiotest",
		NOTHCICOMMAND, psgett, NULL, "psget", "ps_getreq",
		NOTHCICOMMAND, pssett, NULL, "psset", "ps_setreq",
		NOTHCICOMMAND, pssetss, NULL, "pssets", "ps_setsreq",
		NOTHCICOMMAND, psclrr, NULL, "psclr", "ps_clearreq",
		NOTHCICOMMAND, psclrsr, NULL, "psclrs", "ps_clearsreq",
		NOTHCICOMMAND, psclralll, NULL, "psclrall", "ps_clearallreq",
		NOTHCICOMMAND, pssizee, NULL, "pssize", "ps_getsizereq",
		NOTHCICOMMAND, psnextt, NULL, "psnext", "ps_getnextreq",
		NOTHCICOMMAND, psnextall, NULL, "psnextall", "ps_getnextallreq",
		NOTHCICOMMAND, psslurp, NULL, "psslurp", "ps_slurp_keys",
		NOTHCICOMMAND, psmtt, NULL, "psmt", "ps_memory_type",
		cmd	(psfind), "ps_find_keyid",
		NOTHCICOMMAND, bdslurp, NULL, "bdslurp", "bd_slurp_builddefs",
		cmd	(memget), "host_controller_read_memory",
		cmd (randget), "get_random",   // long form of command
		NOTHCICOMMAND, memsett, NULL, "memset", "host_controller_write_memory",
		NOTHCICOMMAND, memsett32, NULL, "memset32", "host_controller_write_memory_32bit",
		cmd	(memfind), "host_controller_find_symbol",
		cmd	(bufget), "host_controller_read_buffer",
        cmd (bufset), "host_controller_write_buffer",
		cmd	(loadsym), "symbol_table_load",
		NOTHCICOMMAND, restartt, NULL, "restart", "restart_transport",
		cmd	(i2c),	"i2c_transfer",
		NOTHCICOMMAND, e2sett, NULL, "e2set", "e2_device_setreq",
		NOTHCICOMMAND, e2gett, NULL, "e2get", "e2_device_getreq",
		NOTHCICOMMAND, eafhh, NULL, "eafh", "enable_afh",
		NOTHCICOMMAND, l2cap_crch, NULL, "l2cap_crc", "l2cap_crc",
		NOTHCICOMMAND, get_l2cap_crch, NULL, "get_l2cap_crc", "get_l2cap_crc",
		/* pws: order seems to be random so I have respected this... */
		NOTHCICOMMAND, berthresht, NULL, "berthresh", "ber_threshold",
		NOTHCICOMMAND, emrh, NULL, "emr", "enable_medium_rate",
		NOTHCICOMMAND, lapth, NULL, "lapt", "lock_acl_packet_type",
		NOTHCICOMMAND, struct_disp, NULL, "strdisp", "struct_disp",
		NOTHCICOMMAND, struct_load, NULL, "strload", "struct_load",
		NOTHCICOMMAND, struct_match, NULL, "strmatch", "struct_match",
		NOTHCICOMMAND, struct_whatis, NULL, "strwhat", "struct_whatis",
		NOTHCICOMMAND, struct_depth, NULL, "strdepth", "struct_depth",
		NOTHCICOMMAND, rsl, NULL, "rsl", "remote_sniff_limits",

		cmd	(rawlog), "show_logged_raw_transport",
#ifdef LATENCY_TRACKING
		cmd	(ipclog), "show_logged_IPC_timings",
#endif
		cmd	(rbn),	"read_build_name",
		cmd	(et),	"escotest",
		/* H4+/H5 Sleep */
		cmd	(trs),	"trans_sleep",
		cmd	(trw),	"trans_wakeup",
#ifdef MGRDIAG
		cmd	(diag),	"mysterious_diagnostics",
#endif
		cmd	(quit),	"quit",
		cmd	(pc32),	"pcm_config32",
		cmd	(plj),	"pcm_low_jitter",
		cmd	(prar),	"pcm_rate_and_route",
		cmd	(dar),	"digital_audio_rate",
		cmd	(dac),	"digital_audio_config",
        cmd	(epos),	"egps_pulse_output_start",
        cmd (cbrr), "change_baudrate_reset_restart",
        cmd (sfp), "send_fastpipe",
        cmd (fpi), "fastpipe_info",
        /* WLAN coex interface test */
        cmd (wcit), "wlan_coex_int_test",
        cmd (sendfp), "send_file_fastpipe",
        cmd (recvfp), "receive_file_fastpipe",
        cmd (rfpl), "reduce_fastpipe_limitc",

        cmd (fpc), "force_power_control",

        cmd (sgso), "stream_get_source",
        cmd (sgsi), "stream_get_sink",
        cmd (scso), "stream_close_source",
        cmd (scsi), "stream_close_sink",
        cmd (scfg), "stream_configure",
        cmd (sas),  "stream_alias_sink",
        cmd (scon), "stream_connect",
        cmd (ssync), "stream_sync_sid",
        cmd (sdis), "stream_transform_disconnect",
        cmd (sts),  "stream_transaction_start",
        cmd (stc),  "stream_transaction_complete",
        cmd (stca), "stream_transaction_cancel",
		cmd (msa), "map_sco_audio",
		cmd (ess), "enable_sco_streams",
		cmd (pcr), "pcm_clock_rate",
		cmd (pspf), "pcm_slots_per_frame",
		cmd (psr), "pcm_sync_rate",
        cmd (rawbcget), "rawbcget",
        cmd (rawbcset), "rawbcset",
        cmd (h4raw), "h4raw",
        cmd (spf), "pio32_set_pin_function",
		cmd (micbiasctrl), "mic_bias_ctrl",
		cmd (micbiascfg), "mic_bias_configure",
		cmd (ledcfg), "led_configure",
		cmd (siflash), "siflash",

        cmd (startop), "start_operator",
        cmd (stopop), "stop_operator",
        cmd (rstop), "reset_operator",
        cmd (dop), "destroy_operator",
        cmd (cop_c), "create_operator_c",
        cmd (cop_p), "create_operator_p",
        cmd (opmsg), "operator_message",

};
#define N_HCI_COMMANDS (sizeof hciCommandTable / sizeof hciCommandTable [0])


size_t getNumCommands (void)
{
    return N_HCI_COMMANDS;
}

const char *getCommandAbbNam (size_t i)
{
	ASSERT (i < getNumCommands ());
	return hciCommandTable[i].abbnam;
}

const char *getCommandFullName (size_t i)
{
	ASSERT (i < getNumCommands ());
	return hciCommandTable[i].fullname;
}

static const char *getCommandName (size_t i)
{
	ASSERT (i < getNumCommands ());
	return	hciCommandTable[i].opcode == NOTHCICOMMAND ?
		hciCommandTable[i].abbnam : /* Return short name for non-HCI */
		hciCommandTable[i].fullname;
}

const char *getCurrentCommandName (void)
{
	return getCommandName (opindex);
}

DECLWORKAROUND getCommandCompleteDecoder (size_t i)
{
	ASSERT (i < getNumCommands ());
	return hciCommandTable[i].decoder_cc;
}

unsigned getCommandOpcode (size_t i)
{
	ASSERT (i < getNumCommands ());
	return hciCommandTable[i].opcode;
}

unsigned getCurrentCommandOpcode (void)
{
	return getCommandOpcode (opindex);
}


/* Here we go! */
void parseCmd (const char *s, bool force)
{
	const char *ps = s;
        bool expanded;
	
	struct dynstr s2;
	new_dynstr(&s2, 0);

        while (isspace (*s))
                s++;
	if (*s == '#')
	{
		printlfOnly (s);
		if (s[strlen (s) - 1] != '\n')
		{
			printlfOnly ("\n");
		}
		delete_dynstr(&s2);
		return;
	}

	expanded = strchr (s, '$');
	if (expanded)
	{
		if (!substituteVarsDynamic (&s2, s))
		{
			printlf ("line too long after variable substitution\n");

			delete_dynstr(&s2);
			return;
		}
		ps = s2.data;
	}
	else
	{
		ps = s;
	}
	startParse (ps);

	if (getDone ())
	{
		printlfOnly ("\n");

		delete_dynstr(&s2);
		return; /* Blank line */
	}
	
	if (force || printPrefix () || (expanded && showExpansions))
	{
		if (printLog () && (expanded && showExpansions))
		{
			printlfOnly (s);
		}
#ifdef THREADING_WORKAROUND
		btcli_enter_critical_section();
#endif/*THREADING_WORKAROUND*/
		printTimestamp ();
		printlf ("%s", ps);
		if (strchr (ps, '\n') == NULL)
		{
			printlf ("\n");
		}
#ifdef THREADING_WORKAROUND
		btcli_leave_critical_section();
#endif/*THREADING_WORKAROUND*/
	}
	else if (printLog ())
	{
		printlfOnly (ps);
		if (strchr (ps, '\n') == NULL)
		{
			printlfOnly ("\n");
		}
	}

	if (!getCommandIndex (&opindex))
	{
		printlf ("unknown command\n");

		delete_dynstr(&s2);
		return;
	}

	hciCommandTable[opindex].parser ();

	delete_dynstr(&s2);
}


#if 0
void testevt (void)
{
	u32 pa[256];

	pa[0] = 2 + 2;
	pa[1] = 0x01;
	pa[2] = 0x00;
	pa[3] = 6;
	decode (pa); /* ic success 0x06 */

	pa[0] = 2 + 3;
	pa[1] = 0x0f;
	pa[2] = 0;
	pa[3] = 42;
	pa[4] = 0;
	decode (pa); /* cs pending 0x2a nop */

	pa[0] = 2 + 2 + 1;
	pa[1] = 0x0e;
	pa[2] = 1;	/* Num_HCI_Command_Packets */
	pa[3] = 0x0402;
	pa[4] = 0x0d; /* cc 0x01 ic host_rejected_due_to_limited_resources */
	decode (pa);

	pa[4] = 0x32;
	decode (pa); /* cc 0x01 ic 0x32 */

	pa[0] = 2 + 2 + 1 + 3;
	pa[3] = 0x040b;
	pa[4] = 0;
	pa[5] = 0x789abc;
	pa[6] = 0x56;
	pa[7] = 0x1234;
	decode (pa); /* cc 0x01 lkrr success 0x123456789abc */

	pa[0] = 2 + 2 + 1 + 2;
	pa[3] = 0x0809;
	pa[4] = 0x14;
	pa[5] = 0x25;
	pa[6] = 1;
	decode (pa); /* cc 0x01 rd other_end_terminated_connection_low_resources 0x0025 slave */

	pa[0] = 2 + 2 + 1 + 1;
	pa[3] = 0x0c19;
	pa[4] = 0;
	pa[5] = 3;
	decode (pa); /* cc 0x01 rse success inquiry_page */

	pa[0] = 2 + 2 + 1 + 3;
	pa[3] = 0x1009;
	pa[4] = 1;
	pa[5] = 0x789abc;
	pa[6] = 0x56;
	pa[7] = 0x1234;
	decode (pa); /* cc 0x01 rba unknown_hci_command 0x123456789abc */

	pa[1] = 0;
	decode (pa); /* evt_0x00 ... */

	pa[1] = 0x21;
	decode (pa); /* evt_0x00 ... */

	pa[0] = 2 + 4;
	pa[1] = 0x20;
	pa[2] = 0x78;
	pa[3] = 0;
	pa[4] = 0xab;
	pa[5] = 1;
	decode (pa); /* psrmc 0x00ab00000078 r1 */
}
#endif


#ifdef MGRWIN32ALLOCDEBUG

_CrtMemState cms;

int MyAllocHook(int nAllocType, void *pvData,
				size_t nSize, int nBlockUse, long lRequest,
				const unsigned char * szFileName, int nLine )
{
	if ( nBlockUse == _CRT_BLOCK )
		return( 1 );

	if (nAllocType == _HOOK_ALLOC)
		fprintf (stderr, "alloc: ");
	else if (nAllocType == _HOOK_REALLOC)
		fprintf (stderr, "realloc: ");
	else if (nAllocType == _HOOK_FREE)
		fprintf (stderr, "free: ");
	else
		fprintf (stderr, "%i: ", nAllocType);

	fprintf (stderr, "p %p, size %lu, bu %i, req# %li, %s/%i\n",
		pvData, (unsigned long) nSize, nBlockUse, lRequest,
		szFileName != NULL ? szFileName : "<unknown>", nLine);

	return 1;
}

int MyReportHook(
				 int   nRptType,
				 char *szMsg,
				 int  *retVal
				 )
{
	char *RptTypes[] = { "Warning", "Error", "Assert" };
	
	//   if ( ( nRptType > 0 ) || ( strstr( szMsg, "DAMAGE" ) ) )
	fprintf( stderr, "%i: %s\n", nRptType, szMsg );
	
	retVal = 0;
	
	return( false );
}

#endif


static char *getcmdline(void);

void btcli (void)
{
    char *inptr;

#ifndef _WIN32 /* A desperate attempt to get non-full buffering for btclid */
//#include <unistd.h>
#ifdef MGRPIPEDEBUG
	fprintf (stderr, "{stdout cnt %i ptr %p base %p flag %02x file %u}\n", stdout->_cnt, stdout->_ptr, stdout->_base, stdout->_flag, stdout->_file);
	fprintf (stderr, "{stdout cnt %i ptr %p base %p flag %02x file %u}\n", stdout->_cnt, stdout->_ptr, stdout->_base, stdout->_flag, stdout->_file);
	fprintf (stderr, "{stdin cnt %i ptr %p base %p flag %02x file %u}\n", stdin->_cnt, stdin->_ptr, stdin->_base, stdin->_flag, stdin->_file);
	fprintf (stderr, "{stderr cnt %i ptr %p base %p flag %02x file %u}\n", stderr->_cnt, stderr->_ptr, stderr->_base, stderr->_flag, stderr->_file);
#endif
#ifdef MGRPIPEDEBUG
	fprintf (stderr, "{stdout cnt %i ptr %p base %p flag %02x file %u}\n", stdout->_cnt, stdout->_ptr, stdout->_base, stdout->_flag, stdout->_file);
	fprintf (stderr, "Is%s a tty\n", isatty (1) ? "" : "n't");
#endif
#endif

	//	testevt();

#ifdef MGRWIN32ALLOCDEBUG
	//	_CrtSetReportHook( MyReportHook );
	_CrtSetAllocHook( MyAllocHook );
	_CrtMemCheckpoint (&cms);
	//	_CrtSetBreakAlloc (159L);
#endif
#ifdef _WIN32_WCE
	Sleep(1000);
#endif
	while (!quitFlag && (inptr = getcmdline()))
	{
#ifdef MGRWIN32ALLOCDEBUG
		//		printf ("====\n");
		//		_CrtMemDumpAllObjectsSince (&cms);
		//		_CrtMemCheckpoint (&cms);
		_CrtMemDumpStatistics (&cms);
		//		printf ("lTotalCount: %li %li\n", (long) cms.lTotalCount, (long) cms.lHighWaterCount);
		//		printf ("====\n");
#endif
		parseCmd (inptr, false);
		if (restart_after_commands != RESTART_NONE)
		{
            // optionally we need to wait for any outstanding commands to finish. (B-81501)
            if(gRestartFlowControlled)
            {
                //we should check for pending commands on the queue
                int timeout = 20;
                size_t queued = 0;
                while (((queued = pendingItemsOnRegulator()) > 0) && timeout)
                {
                    printf("pending queued size = %d\n", queued);
                    sleep_ms(200);
                    timeout--;
                }
                printf("pending final queued size = %d\n", queued);
            }
			break;
		}
#ifdef _WIN32_WCE
		Sleep(1000);
#endif
	}
}


#if !defined(_WINCE) && !defined(_WIN32_WCE)
#ifdef _WIN32

u32 mtime (void)
{
	struct _timeb tb;

	_ftime (&tb);

	return (u32) tb.time * 1000 + tb.millitm;
}


#else
#ifndef POSIX_NO_CLOCK_GETTIME

u32 mtime (void)
{
	struct timespec ts;
	int rc = clock_gettime (CLOCK_REALTIME, &ts);
	ASSERT (rc == 0);
	return ts.tv_nsec / 1000000 + (u32) ts.tv_sec * 1000;
}

#else
#ifndef BSD_NO_GETTIMEOFDAY

u32 mtime (void)
{
	struct timeval tv;
#ifndef NDEBUG
	int rc =
#endif
#ifdef linux
	gettimeofday (&tv, NULL);
#else
	struct timezone tz;
	int rc = gettimeofday (&tv, &tz);
#endif
	ASSERT (rc == 0);
	return tv.tv_usec / 1000 + (u32) tv.tv_sec * 1000;
}

#else

u32 mtime (void)
{
	return (u32) time (NULL) * 1000;
}

#endif
#endif
#endif
#endif


#ifdef MGR
/* Stuff to try to catch memory leaks; a bit thread-unsafe... */
#ifdef malloc
void *mallocs[256]; /* Hope this inits to NULL... */
unsigned nallocs = 0;
#undef malloc
void *mallocc (size_t n)
{
	extern void *malloc (size_t n);
	size_t i;
	bool stored = false;
	void *p;

#ifndef MGRQUIETMALLOC
	printlf ("malloc:");
#endif
	p = malloc (n);

	if (nallocs != ~0)
	{
		++nallocs;
		for (i = 0; i < sizeof (mallocs) / sizeof (mallocs[0]); ++i)
		{
			if (!stored && mallocs[i] == NULL)
			{
				mallocs[i] = p;
				stored = true;
			}
		}
		if (!stored)
		{
			printlf ("Ran out of malloc-checking space; not checking anymore!\n");
			nallocs = ~0;
		}
	}

#ifndef MGRQUIETMALLOC
	printlf ("%p(%lu)\n", p, (ul) n);
#endif
	return p;
}
#endif

#ifdef free
#undef free
void freee (void *p)
{
	extern void free (void *p);
	size_t i;
	bool stored = false;

#ifndef MGRQUIETMALLOC
	printlf ("free:::");
#endif
	free (p);

	if (nallocs != ~0)
	{
		--nallocs;
		for (i = 0; i < sizeof (mallocs) / sizeof (mallocs[0]); ++i)
		{
			if (!stored && mallocs[i] == p)
			{
				mallocs[i] = NULL;
				stored = true;
				break;
			}
		}
		if (!stored)
		{
			printlf ("*** free (notalloc) ***\n");
			abort ();
		}
	}
	
#ifndef MGRQUIETMALLOC
	printlf ("%p[%u]\n", p, nallocs);
#endif
}
#endif
#endif

/*ARGSUSED*/
char *getcmdline(void)
{
#ifdef USE_READLINE
	if (isatty(0)) {
		static char *line_read = (char *)NULL;
		char *new_line;
		
		new_line = readline("");
		
		if (new_line && *new_line)
		{
			/* Non empty, see if it's the same as the previous one */
			if (!line_read || strcmp(new_line, line_read))
				add_history(new_line);
			
			if (line_read)
				free(line_read);
			line_read = new_line;
			
			return new_line;
		}
		else if (new_line)
		{
			/* Empty but not NULL: forget it. */
			free(new_line);
			return "";
		}
		else
		{
		/*
		* NULL, so we are giving up and going home.
		* tidy mind, tidy... er... something
			*/
			free(line_read);
			line_read = NULL;
			return NULL;
		}
	}
	else
	{
#endif
		static struct dynstr str = {BUFFERSIZE, 0, NULL};
		struct dynstr *retval;
		if (NULL == str.data) {
			str.data = malloc(BUFFERSIZE);
		}
		str.length = 0;

		ASSERT(NULL != str.data);
		
		retval = fgetsDynamic(&str, stdin);

		if (NULL == retval)
		{
			return NULL;
		}
		else
		{
			return retval->data;
		}
		/*
		return fgets (buf, sizeof (buf), stdin);
		*/
#ifdef USE_READLINE
	}
#endif
}
