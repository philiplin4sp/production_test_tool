// Copyright (C) 2000-2006 Cambridge Silicon Radio Ltd.; all rights reserved.
// Various console output functions.
// $Id: //depot/bc/bluesuite_2_4/devHost/btcli/print.c#1 $


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#if !defined(_WINCE) && !defined(_WIN32_WCE)
#include <assert.h>
#define ASSERT assert
#else
#include "wince/wincefuncs.h"
#endif

#ifdef _WIN32
#if !defined(_WINCE) && !defined(_WIN32_WCE)
#include <sys/timeb.h>	/* This pulls in the Win32 stuff needed for timestamping with subsecond precision */
#include <time.h>
#else
#include <winbase.h>
#endif
#else
#ifndef POSIX_NO_CLOCK_GETTIME
#include <time.h>	/* This pulls in the Posix.1b realtime stuff needed for timestamping with subsecond precision */
#else
#ifndef BSD_NO_GETTIMEOFDAY
#include <sys/time.h>	/* This pulls in the BSD stuff needed for timestamping with subsecond precision */
#include <unistd.h>	/* Appears to be needed under Linux; can't do much harm under other Un*xes */
#endif
#endif
#endif

#include "time/hi_res_clock.h"
#include "thread/critical_section.h"

#include "print.h"
#include "globals.h"		/* for last stuff */
#include "constants.h"		/* for NOHANDLE */
#include "commandtable.h"	/* for the current command name */
#include "dictionaries.h"	/* for error code dictionary */
#include "assertn.h"

struct Messages
{
    char *msg;
    struct Messages *next;
} *async_msgs = NULL, *last_async_msg = NULL;
struct critical_section *async_msgs_cs = NULL;

static void print_async_messages();
static void add_async_message(char *msg);

static void print_backwards_timestamp(const char *prev, const char *next)
{
    static const char fmt[] = "backwards time stamp detected. prev=%s, next=%s";
    char *msg = (char *) malloc(sizeof(fmt)/sizeof(fmt[0]) - 4 + strlen(prev) + strlen(next));
    sprintf(msg, fmt, prev, next);
    add_async_message(msg);
}

static void print_cleanup()
{
    backwards_timestamp_callback = NULL;
    print_async_messages();
    destroy_critical_section(async_msgs_cs);
    async_msgs_cs = NULL;
}

void print_init()
{
#ifndef _WIN32
    if (setvbuf (stdout, NULL, _IOLBF, BUFSIZ) != 0)
    {
        perror ("setvbuf(stdout, NULL, _IOLBF, BUFSIZ) failed");
    }
#endif

    InitHiResClock(NULL);
    if(!async_msgs_cs)
    {
        async_msgs_cs = create_critical_section();
        atexit(print_cleanup);
    }

    backwards_timestamp_callback = &print_backwards_timestamp;
}

/* HERE could probably cut down on printlf (" ");s by making all these routines prepend a space */

static FILE *flog = NULL;
static bool tsenabled = false;
static char *tag = NULL;


void printByValue (Dictionary dictionary, u32 value, int size, const char *prefix)
{
	const char *name;

	if (dictionary == NULL || (name = lookupByValue (dictionary, value)) == NULL)
	{
		ASSERT (size > 0);
		ASSERTN (value, size);
		if (prefix)
		{
			printlf ("%s:0x%0*lx", prefix, (size + 3) / 4, (ul) value);
		}
		else
		{
			printlf ("0x%0*lx", (size + 3) / 4, (ul) value);
		}
	}
	else
	{
		printlf ("%s", name);
	}
}


void printba (u32 lap, u32 uap, u32 nap)
{
	ASSERT24 (lap);
	ASSERT8 (uap);
	ASSERT16 (nap);
	printlf ("ba:0x%04lx%02lx%06lx", (ul) nap, (ul) uap, (ul) lap);
}


void printch (u32 ch)
{
	int i;

	if (ch != NOHANDLE)
	{
		if (ch == abch)
		{
			printlf ("ab");
			return;
		}

		if (ch == pbch)
		{
			printlf ("pb");
			return;
		}

		for (i = 0; i < 18; ++i)
		{
			if (ch == aclch[i])
			{
				printlf ("acl%i", i);
				return;
			}
		}

		for (i = 0; i < 10; ++i)
		{
			if (ch == scoch[i])
			{
				printlf ("sco%i", i);
				return;
			}
		}
	}

	ASSERT16 (ch);
	printlf ("ch:0x%04x", (unsigned) ch);
}


void printChar (char c)
{
	/* Assumes ASCII... */
	if (c == 127)
	{
		printlf ("^?");
	}
	else if (c == '\\')
	{
		printlf ("\\\\");
	}
	else if (c == '"')
	{
		printlf ("\\\"");
	}
	else if (c == '^')
	{
		printlf ("\\^");
	}
	else if (c == 13)
	{
		printlf ("\\r");
	}
	else if (c == 10)
	{
		printlf ("\\n");
	}
	else if (c == 9)
	{
		printlf ("\\t");
	}
	else if (c == 0)
	{
		printlf ("\\0");
	}
	else if (c >= 0 && c < 32)
	{
		printlf ("^%c", c + '@');
	}
	else if ((u8) c > 126)
	{
		printlf ("\\x%02x", (u8) c);
	}
	else
	{
		printlf ("%c", c);
	}
}


void printString (const PA pa, size_t n)
{
	size_t i;
	char c;

	printlf ("\"");
	for (i = 0; i < n; ++i)
	{
		ASSERT ((pa[i] & ~0xffUL) == 0);
		c = (char) pa[i];
		if (c == NUL)
		{
			break;
		}
		printChar (c);
	}
	printlf ("\"");
}


void printStringOrHexes (const u8 *data, size_t n)
{
	size_t i;
	u8 c;

	for (i = 0; i < n; ++i)
	{
		/* Assumes ASCII... */
		c = data[i];
		if (c > 126 ||
		    (c < 32 && c != 13 && c != 10 && c != 0))
		{
			break;
		}
	}

	if (i == n)
	{
		printlf ("\"");
		while (n--)
		{
			printChar (*data++);
		}
		printlf ("\"");
	}
	else
	{
		while (n--)
		{
			printlf ("%02x", *data++);
		}
	}
}


void printQuintets (const u8 *data, size_t n)
{
	size_t i;

	for (i = 0; i < n; ++i)
	{
		if (i % 5 == 0 && i != 0)
		{
			printlf ("-");
		}
		printlf ("%02x", data[i]);
	}
}



void printHexAndDec (u32 value, int hexsize, int digits, const char *prefix)
{
	if (prefix)
	{
		printlf ("%s:0x%0*lx (%0*d)", prefix, (hexsize + 3) / 4, (ul) value, digits, (ul) value);
	}
	else
	{
		printlf ("0x%0*lx (%0*d)", (hexsize + 3) / 4, (ul) value, digits, (ul) value);
	}
}

void printEIRD (const u32 *data, u32 length)
{
	size_t i, data_len, len = 0;
	bool format_error = false, start = true;

	printlf(" ird:");
    if (decode_eir)
    {
	    if (*data == NUL)
		    printlf("0");
	    while ((len < 240) && (*data))
	    {
		    data_len = *(data++);
		    if (!start)
			    printlf(",");
		    start = false;
		    printlf ("0x%02x-0x", data_len);
		    if ((len += data_len + 1) > 240)
		    {
			    data_len -= (len - 240);
			    format_error = true;
		    }
		    for (i = data_len; i > 0;)
		    {
			    printlf("%02x", data[--i]);
		    }
		    data += data_len;
	    }
	    if (format_error)
		    printlf ("\nWarning: bad EIR data structure");
    }
    else
    {
        if (length)
            printlf("0x%02x", data[0]);
        for(i = 1; i<length;++i)
        {
            printlf(" 0x%02x", data[i]);
        }
    }
}

void printFeaturesPageX (int page, const PA pa, const char *prefix)
{
    bool first = true;
    size_t i;
    bool unknown = false;

    /* If this list of known features does not include a feature bit then 
     * it will not be decoded by this function.  Therefore when adding feature
     * bits below ensure that these array values are updated.
     */
    u32 known_features_mask[2][8] = {
        {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0x87}, // Page 0
        {0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  // Page 1
    };

#define SHOWFEATURE(is,txt)         \
    if (!first)                     \
    {                               \
        printlf ("|");              \
    }                               \
    if (!(is))                      \
    {                               \
        printlf ("~");              \
    }                               \
    printlf (txt);                  \
    first = false;

    if (prefix)
        printlf("%s:0x", prefix);
    else
        printlf("f:0x");

    for (i = 8; i != 0; --i)
    {
        ASSERT8 (pa[i - 1]);
        printlf ("%02x", (unsigned) pa[i - 1]);
    }

    /* The abbreviations displayed should be predictable.  The first letters 
     * of the names should always be used, unless there is a conflict with an
     * existing name.  In that case, follow the abbreviations displayed should 
     * be predictable rule and do something sensible.  Note: This might also 
     * include names which could conflict with well known names in Bluetooth, 
     * for example Erroneous Data Reporting should be "edr", however that is 
     * very confusing.
     */
    switch(page)
    {
    case 0:
        printlf ("(");
        SHOWFEATURE (pa[0] & 0x01, "3sp")     	/* 3 slot packets */
        SHOWFEATURE (pa[0] & 0x02, "5sp")       /* 5 slot packets */
        SHOWFEATURE (pa[0] & 0x04, "e")         /* Encryption */
        SHOWFEATURE (pa[0] & 0x08, "so")        /* Slot offset */
        SHOWFEATURE (pa[0] & 0x10, "ta")        /* Timing accuracy */
        SHOWFEATURE (pa[0] & 0x20, "mss")       /* Role switch */
        SHOWFEATURE (pa[0] & 0x40, "hm")        /* Hold mode */
        SHOWFEATURE (pa[0] & 0x80, "sm")        /* Sniff mode */
        
        SHOWFEATURE (pa[1] & 0x01, "pm")        /* Park state */
        SHOWFEATURE (pa[1] & 0x02, "rm")        /* Power control requests */
        SHOWFEATURE (pa[1] & 0x04, "cqddr")     /* Channel quality driven data rate (CQDDR) */
        SHOWFEATURE (pa[1] & 0x08, "sl")        /* SCO link */
        SHOWFEATURE (pa[1] & 0x10, "hv2p")      /* HV2 packets */
        SHOWFEATURE (pa[1] & 0x20, "hv3p")      /* HV3 packets */
        SHOWFEATURE (pa[1] & 0x40, "usd")       /* u-law log synchronous data */
        SHOWFEATURE (pa[1] & 0x80, "asd")       /* A-law log synchronous data*/
        
        SHOWFEATURE (pa[2] & 0x01, "csd")       /* CVSD synchronous data */
        SHOWFEATURE (pa[2] & 0x02, "ppn")       /* Paging parameter negotiation */
        SHOWFEATURE (pa[2] & 0x04, "pc")        /* Power control */
        SHOWFEATURE (pa[2] & 0x08, "tsd")       /* Transparent synchronous data */
        printlf ("|fcl%u", pa[2] >> 4 & 7);     /* Flow control lag */
        SHOWFEATURE (pa[2] & 0x80, "be")        /* Broadcast Encryption */
        
        SHOWFEATURE (pa[3] & 0x01, "scm")       /* Reserved (scatter mode) */
        SHOWFEATURE (pa[3] & 0x02, "2dh")       /* Enhanced Data Rate ACL 2 Mbps mode */
        SHOWFEATURE (pa[3] & 0x04, "3dh")       /* Enhanced Data Rate ACL 3 Mbps mode */
        SHOWFEATURE (pa[3] & 0x08, "eis")       /* Enhanced inquiry scan */
        SHOWFEATURE (pa[3] & 0x10, "iis")       /* Interlaced inquiry scan */
        SHOWFEATURE (pa[3] & 0x20, "ips")       /* Interlaced page scan */
        SHOWFEATURE (pa[3] & 0x40, "rwir")      /* RSSI with inquiry results */
        SHOWFEATURE (pa[3] & 0x80, "esl")       /* Extended SCO link (EV3 packets) */
        
        SHOWFEATURE (pa[4] & 0x01, "ev4p")      /* EV4 packets */
        SHOWFEATURE (pa[4] & 0x02, "ev5p")      /* EV5 packets */
        SHOWFEATURE (pa[4] & 0x04, "absm")      /* Reserved (absense masks) */
        SHOWFEATURE (pa[4] & 0x08, "acaps")     /* AFH capable slave */
        SHOWFEATURE (pa[4] & 0x10, "aclas")     /* AFH classification slave */
        SHOWFEATURE (pa[4] & 0x20, "bredrns")   /* BR/EDR Not Supported */
        SHOWFEATURE (pa[4] & 0x40, "lesc")      /* LE Supported (Controller) */
        SHOWFEATURE (pa[4] & 0x80, "mr3sp")     /* 3-slot Enhanced Data Rate ACL packets */
    
        SHOWFEATURE (pa[5] & 0x01, "mr5sp")     /* 5-slot Enhanced Data Rate ACL packets */
        SHOWFEATURE (pa[5] & 0x02, "ssr")       /* Sniff subrating */
        SHOWFEATURE (pa[5] & 0x04, "epr")       /* Pause encryption */
        SHOWFEATURE (pa[5] & 0x08, "acapm")     /* AFH capable master */
        SHOWFEATURE (pa[5] & 0x10, "aclam")     /* AFH classification master */
        SHOWFEATURE (pa[5] & 0x20, "2ev")       /* Enhanced Data Rate eSCO 2 Mbps mode */
        SHOWFEATURE (pa[5] & 0x40, "3ev")       /* Enhanced Data Rate eSCO 3 Mbps mode */
        SHOWFEATURE (pa[5] & 0x80, "mr3es")     /* 3-slot Enhanced Data Rate eSCO packets */
    
        SHOWFEATURE (pa[6] & 0x01, "eir")       /* Extended Inquiry Response */
        SHOWFEATURE (pa[6] & 0x02, "slebredrcc") /* Simultaneous LE and BR/EDR to Same Device Capable (Controller) */
        /* SHOWFEATURE (pa[6] & 0x04, "anpc") - alphanumbeic pin capable depricated */
        SHOWFEATURE (pa[6] & 0x08, "sp")        /* Secure Simple Pairing */
        SHOWFEATURE (pa[6] & 0x10, "epdu")      /* Encapsulated PDU */
        SHOWFEATURE (pa[6] & 0x20, "errdr")     /* Erroneous Data Reporting */
        SHOWFEATURE (pa[6] & 0x40, "pbf")       /* Non-flushable Packet Boundary Flag */
        SHOWFEATURE (pa[6] & 0x80, "ps")        /* Reserved (persistent sniff) */

        SHOWFEATURE (pa[7] & 0x01, "lsto")      /* Link Supervision Timeout Changed Event */
        SHOWFEATURE (pa[7] & 0x02, "irtxpl")    /* Inquiry TX Power Level */
        SHOWFEATURE (pa[7] & 0x04, "epc")       /* Enhanced Power Control */
        /* SHOWFEATURE (pa[n] & 0xnn, reserved) */
        SHOWFEATURE (pa[7] & 0x80, "ef")        /* Extended features */
        break;
                
    case 1:
        /* host features...*/
        printlf ("(");
        SHOWFEATURE (pa[0] & 0x01, "ssphs")     /* Secure Simple Pairing (Host Support) */
        SHOWFEATURE (pa[0] & 0x02, "lesh")      /* LE Supported (Host) */
        SHOWFEATURE (pa[0] & 0x04, "slebredrch") /* Simultaneous LE and BR/EDR to Same Device Capable (Host) */
        break;

    default:
        /* Don't need to display unknown features - we don't know any.. */
        return;
    }

    /* work out whether any of the features bit are unknown */
    for (i = 0; i < 8; i++)
    {
        if (pa[i] & ~known_features_mask[page][i])
        {
            unknown = true;
            break;
        }
    }

    if (unknown)
    {
        /* some are unknown display them masking out what we know */
        printlf ("|0x");
        for (i = 8; i != 0; --i)
        {
            printlf ("%02x", pa[i-1] & ~known_features_mask[page][i-1]);
        }
    }

    printlf (")");

#undef SHOWFEATURE
}

void printLEFeatures(const PA pa, const char *prefix)
{
    bool first = true;
    size_t i;
    bool unknown = false;

    /* If this list of known features does not include a feature bit then 
     * it will not be decoded by this function.  Therefore when adding feature
     * bits below ensure that these array values are updated.
     */
    u32 le_known_features_mask[8] = {
		0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};      

#define SHOWFEATURE(is,txt)         \
    if (!first)                     \
    {                               \
        printlf ("|");              \
    }                               \
    if (!(is))                      \
    {                               \
        printlf ("~");              \
    }                               \
    printlf (txt);                  \
    first = false;

    if (prefix)
        printlf("%s:0x", prefix);
    else
        printlf("lef:0x");

    for (i = 8; i != 0; --i)
    {
        ASSERT8 (pa[i - 1]);
        printlf ("%02x", (unsigned) pa[i - 1]);
    }

    /* The abbreviations displayed should be predictable.  The first letters 
     * of the names should always be used, unless there is a conflict with an
     * existing name.  In that case, follow the abbreviations displayed should 
     * be predictable rule and do something sensible.  Note: This might also 
     * include names which could conflict with well known names in Bluetooth, 
     * for example Erroneous Data Reporting should be "edr", however that is 
     * very confusing.
     */
    printlf ("(");
    SHOWFEATURE (pa[0] & 0x01, "e")     	/* LE Encryption */

    /* work out whether any of the features bit are unknown */
    for (i = 0; i < 8; i++)
    {
        if (pa[i] & ~le_known_features_mask[i])
        {
            unknown = true;
            break;
        }
    }

    if (unknown)
    {
        /* some are unknown display them masking out what we know */
        printlf ("|0x");
        for (i = 8; i != 0; --i)
        {
            printlf ("%02x", pa[i-1] & ~le_known_features_mask[i-1]);
        }
    }

    printlf (")");

#undef SHOWFEATURE
}

void printLESupportedStates(const PA pa, const char *prefix)
{
	bool first = true;
	size_t i;

#define SHOWSTATE(is,txt)		\
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


	if (prefix)
		printlf("%s:0x", prefix);
	else
		printlf("le_states:0x");

	for (i = 8; i != 0; --i)
	{
		ASSERT8 (pa[i - 1]);
		printlf ("%02x", (unsigned) pa[i - 1]);
	}

	printlf ("(");
	SHOWSTATE (pa[0] & 0x01, "nc adv")
	SHOWSTATE (pa[0] & 0x02, "disc adv")
	SHOWSTATE (pa[0] & 0x04, "c adv")
	SHOWSTATE (pa[0] & 0x08, "dir adv")
	SHOWSTATE (pa[0] & 0x10, "p scan")
	SHOWSTATE (pa[0] & 0x20, "a scan")
	SHOWSTATE (pa[0] & 0x40, "init")
	SHOWSTATE (pa[0] & 0x80, "slave")
	SHOWSTATE (pa[1] & 0x01, "nc adv,p scan")
	SHOWSTATE (pa[1] & 0x02, "disc adv,p scan")
	SHOWSTATE (pa[1] & 0x04, "c adv,p scan")
	SHOWSTATE (pa[1] & 0x08, "dir adv,p scan")
	SHOWSTATE (pa[1] & 0x10, "nc adv,a scan")
	SHOWSTATE (pa[1] & 0x20, "disc adv,a scan")
	SHOWSTATE (pa[1] & 0x40, "c adv,a scan")
	SHOWSTATE (pa[1] & 0x80, "dir adv,a scan")
	SHOWSTATE (pa[2] & 0x01, "nc adv,init")
	SHOWSTATE (pa[2] & 0x02, "disc adv,init")
	SHOWSTATE (pa[2] & 0x04, "nc adv,master")
	SHOWSTATE (pa[2] & 0x08, "disc adv,master")
	SHOWSTATE (pa[2] & 0x10, "nc adv,slave")
	SHOWSTATE (pa[2] & 0x20, "disc adv,slave")
	SHOWSTATE (pa[2] & 0x40, "p scan,init")
	SHOWSTATE (pa[2] & 0x80, "a scan,init")
	SHOWSTATE (pa[3] & 0x01, "p scan,master")
	SHOWSTATE (pa[3] & 0x02, "a scan,master")
	SHOWSTATE (pa[3] & 0x04, "p scan,slave")
	SHOWSTATE (pa[3] & 0x08, "a scan,slave")
	SHOWSTATE (pa[3] & 0x10, "init,master")
	printlf (")");

#undef SHOWSTATE
}


void printSyntax (const char *s)
{
	printlf ("syntax: %s %s\n", getCurrentCommandName (), s);
}

void printSyntaxCommand (void)
{
	printlf ("syntax: %s ", getCurrentCommandName());
}


void printSyntaxStart (void)
{
	printlf ("syntax:\n");
}


void printSyntaxContin (const char *s)
{
	printlf ("%s %s\n", getCurrentCommandName (), s);
}

void printSyntaxEnd (void)
{
	printlf ("\n");
}


void printStatus (u32 statusCode)
{
	printByValue (errorCode_d, statusCode, 8, "s");
}

void printReason (u32 reasonCode)
{
	printByValue (ulp_reasonCode_d, reasonCode, 8, "s");
}

void printAddressType (u32 addressType)
{
	printByValue (at_d, addressType, 8, "at");
}

void printAddressUlp (u32 o1, u32 o2, u32 o3, u32 o4, u32 o5, u32 o6)
{
	printlf ("0x%02x%02x%02x%02x%02x%02x", o6, o5, o4, o3, o2, o1);
}

void printMoreProfiles (u32 moreProfiles)
{
	printByValue (prof_d, moreProfiles, 8, "s");
}

void printEncRequired (u32 encRequired)
{
	printByValue (enc_d, encRequired, 8, "s");
}

void printEventType (u32 eventType)
{
	printByValue (et_d, eventType, 8, "s");
}

void printTimestamps (int enable)
{
	tsenabled = enable;
}


void printTags (const char *t)
{
	if (tag)
	{
		free (tag);
		tag = NULL;
	}
	if (t)
	{
		tag = malloc (strlen (t) + 1);
		if (tag)
		{
			strcpy (tag, t);
		}
	}
}


void getTimestamp (char *buf)
{
#ifdef _WIN32
	char *tstring;
#if !defined(_WINCE) && !defined(_WIN32_WCE)
	struct _timeb tb;

	_ftime (&tb);
	tstring = ctime (&tb.time);
	sprintf (buf, "%.8s.%03u", tstring + 11, (unsigned) tb.millitm);
#else /* _WINCE */
	SYSTEMTIME t;

	GetLocalTime(&t);
	sprintf (buf,"%02u:%02u:%02u.%03u", t.wHour, t.wMinute, t.wSecond,t.wMilliseconds);
#endif /* !_WINCE */
#else /* !_WIN32 */
	HiResClockHMS(buf);
#endif /* _WIN32 */
}


void printTimestamp (void)
{
    if ( tsenabled )
    {
        char buf[16];
	    getTimestamp (buf);    

        printlf ("%s%s ", tag ? tag : "", buf);
    }
    else 
        printTag();
}


void printTag (void)
{
    if ( tag )
	{
		printlf ("%s", tag);
	}
}


int printlOpen (const ichar *s)
{
	ASSERT (flog == NULL);

	flog = ifopen (s, II("a"));

	return flog != NULL;
}


bool printLog (void)
{
	return flog != NULL;
}


bool printPrefix (void)
{
	return tag || tsenabled;
}


void printlClose (void)
{
	if (flog != NULL)
	{
		fclose (flog);
		flog = NULL;
	}
}


void printlfOnly (const char *s)
{
	if (flog != NULL)
	{
		fprintf (flog, "%s", s);
	}
}


void iprintlfOnly (const ichar *s)
{
	if (flog != NULL)
	{
		ifprintf (flog, II("%s"), s);
	}
}


int printlf (const char *s, ...)
{
	va_list ap;
	int i;

	if (flog != NULL)
	{
		va_list lp;
		va_start (lp, s);
		vfprintf (flog, s, lp);
		va_end (lp);
	}

	va_start (ap, s);
	i = vprintf (s, ap);
	va_end (ap);

#ifdef _WIN32
	/*
	 * In some configurations, for some reason, stdout appears
	 * to be fully buffered (rather than line or not buffered).
	 * setvbuf doesn't appear to work in Windows (setting line
	 * buffering appears to behave as if full buffering has been
	 * set), so the best solution would appear to flush stdout
	 * after every (v)printf.
	 *
	 * Sigh.
	 */
	fflush (stdout);
#endif

	// if our format string ends in \n then print async messages.
	if(s && s[0] && s[strlen(s)-1]=='\n') print_async_messages();

	return i;
}


int iprintlf (const ichar *s, ...)
{
	va_list ap;
	int i;

	if (flog != NULL)
	{
		va_list lp;
		va_start (lp, s);
		ivfprintf (flog, s, lp);
		va_end (lp);
	}
	
	va_start (ap, s);
	i = ivprintf (s, ap);
	va_end (ap);

#ifdef _WIN32
	/*
	 * In some configurations, for some reason, stdout appears
	 * to be fully buffered (rather than line or not buffered).
	 * setvbuf doesn't appear to work in Windows (setting line
	 * buffering appears to behave as if full buffering has been
	 * set), so the best solution would appear to flush stdout
	 * after every (v)printf.
	 *
	 * Sigh.
	 */
	fflush (stdout);
#endif

	// if our format string ends in \n then print async messages
	if(s && s[0] && s[strlen(s)-1]=='\n') print_async_messages();

	return i;
}

static void print_async_messages()
{
    lock_critical_section(async_msgs_cs);
    while(async_msgs)
    {
        struct Messages *my_async_msg = NULL;
        my_async_msg = async_msgs;
        async_msgs = async_msgs->next;
        if(!async_msgs) last_async_msg = NULL;
        printlf("%s%c", my_async_msg->msg, '\n');
        free(my_async_msg->msg);
        free(my_async_msg);
    }
    unlock_critical_section(async_msgs_cs);
}

static void add_async_message(char *msg)
{
    struct Messages *new_async_msg = (struct Messages *) malloc(sizeof(struct Messages));
    new_async_msg->msg = msg;
    new_async_msg->next = NULL;
    lock_critical_section(async_msgs_cs);
    if(last_async_msg) last_async_msg->next = new_async_msg;
    last_async_msg = new_async_msg;
    if(!async_msgs) async_msgs = new_async_msg;
    unlock_critical_section(async_msgs_cs);
}

void printCommands (const PA pa)
{
	size_t i;
	int pos = 0;

#define SHOWCOMMAND(txt)	do		\
	{								\
		int word = pos / 8;			\
		int mask = 1 << (pos % 8);	\
		if (pos != 0)				\
			printlf ("|");			\
		if (!(pa[word] & mask))		\
			printlf ("~");			\
		printlf (txt);				\
		pos++;						\
	} while (0)

#define SKIPCOMMAND(txt)	do		\
	{								\
		pos++;						\
	} while (0)

	printlf ("lc:0x");
	for (i = 64; i != 0; --i)
	{
		ASSERT8 (pa[i - 1]);
		printlf ("%02x", (unsigned) pa[i - 1]);
	}

	printlf ("(");
/* Octet 0
0 Inquiry
1 Inquiry Cancel
2 Periodic Inquiry Mode
3 Exit Periodic Inquiry Mode
4 Create Connection
5 Disconnect
6 Add SCO Connection
7 Cancel Create Connection*/
	SHOWCOMMAND ("i");
	SHOWCOMMAND ("ic");
	SHOWCOMMAND ("pim");
	SHOWCOMMAND ("epim");
	SHOWCOMMAND ("cc");
	SHOWCOMMAND ("d");
	SHOWCOMMAND ("asc");
	SHOWCOMMAND ("ccc");

/* Octet 1
0 Accept Connection Request
1 Reject Connection Request
2 Link Key Request Reply
3 Link Key Request Negative Reply
4 PIN Code Request Reply
5 PIN Code Request Negative Reply
6 Change Connection Packet Type
7 Authentication Request*/
	SHOWCOMMAND ("acr");
	SHOWCOMMAND ("rcr");
	SHOWCOMMAND ("lkrr");
	SHOWCOMMAND ("lkrnr");
	SHOWCOMMAND ("pcrr");
	SHOWCOMMAND ("pcrnr");
	SHOWCOMMAND ("ccpt");
	SHOWCOMMAND ("ar");

/* Octet 2
0 Set Connection Encryption
1 Change Connection Link Key
2 Master Link Key
3 Remote Name Request
4 Cancel Remote Name Request
5 Read Remote Supported Features
6 Read Remote Extended Features
7 Read Remote Version Information*/
	SHOWCOMMAND ("sce");
	SHOWCOMMAND ("cclk");
	SHOWCOMMAND ("mlk");
	SHOWCOMMAND ("rnr");
	SHOWCOMMAND ("rnrc");
	SHOWCOMMAND ("rrsf");
	SHOWCOMMAND ("rref");
	SHOWCOMMAND ("rrvi");

/* Octet 3
0 Read Clock Offset
1 Read LMP Handle
2 Exchange Fixed Info
3 Exchange Alias Info
4 Private Pairing Request Reply
5 Private Pairing Request Negative Reply
6 Generated Alias
7 Alias Address Request Reply*/
	SHOWCOMMAND ("rco");
	SHOWCOMMAND ("rlh");
	printlf ("[");
	SHOWCOMMAND ("efi");	/* Anonymity Mode, removed from the spec */
	SHOWCOMMAND ("eai");	/**/
	SHOWCOMMAND ("pprr");	/**/
	SHOWCOMMAND ("pprnr");	/**/
	SHOWCOMMAND ("ga");		/**/
	SHOWCOMMAND ("aarr");	/**/

/* Octet 4
0 Alias Address Request Negative Reply
1 Hold Mode
2 Sniff Mode
3 Exit Sniff Mode
4 Park Mode
5 Exit Park Mode
6 QoS Setup
7 Role Discovery*/
	SHOWCOMMAND ("aarnr");	/**/
	printlf ("]");
	SHOWCOMMAND ("hm");
	SHOWCOMMAND ("sm");
	SHOWCOMMAND ("esm");
	SHOWCOMMAND ("pm");
	SHOWCOMMAND ("epm");
	SHOWCOMMAND ("qs");
	SHOWCOMMAND ("rd");

/* Octet 5
0 Switch Role
1 Read Link Policy Settings
2 Write Link Policy Settings
3 Read Default Link Policy Settings
4 Write Default Link Policy Settings
5 Flow Specification
6 Set Event Mark
7 Reset*/
	SHOWCOMMAND ("sr");
	SHOWCOMMAND ("rlps");
	SHOWCOMMAND ("wlps");
	SHOWCOMMAND ("rdlps");
	SHOWCOMMAND ("wdlps");
	SHOWCOMMAND ("fs");
	SHOWCOMMAND ("sem");
	SHOWCOMMAND ("r");

/* Octet 6
0 Set Event Filter
1 Flush
2 Read PIN Type
3 Write PIN Type
4 Create New Unit Key
5 Read Stored Link Key
6 Write Stored Link Key
7 Delete Stored Link Key*/
	SHOWCOMMAND ("sef");
	SHOWCOMMAND ("f");
	SHOWCOMMAND ("rpt");
	SHOWCOMMAND ("wpt");
	SHOWCOMMAND ("cnuk");
	SHOWCOMMAND ("rslk");
	SHOWCOMMAND ("wslk");
	SHOWCOMMAND ("dslk");

/* Octet 7
0 Write Local Name
1 Read Local Name
2 Read Connection Accept Timeout
3 Write Connection Accept Timeout
4 Read Page Timeout
5 Write Page Timeout
6 Read Scan Enable
7 Write Scan Enable*/
	SHOWCOMMAND ("wln");
	SHOWCOMMAND ("rln");
	SHOWCOMMAND ("rcat");
	SHOWCOMMAND ("wcat");
	SHOWCOMMAND ("rpt");
	SHOWCOMMAND ("wpt");
	SHOWCOMMAND ("rse");
	SHOWCOMMAND ("wse");

/* Octet 8
0 Read Page Scan Activity
1 Write Page Scan Activity
2 Read Inquiry Scan Activity
3 Write Inquiry Scan Activity
4 Read Authentication Enable
5 Write Authentication Enable
6 Read Encryption Mode
7 Write Encryption Mode*/
	SHOWCOMMAND ("rpsa");
	SHOWCOMMAND ("wpsa");
	SHOWCOMMAND ("risa");
	SHOWCOMMAND ("wisa");
	printlf ("[");
	SHOWCOMMAND ("rae"); /* Removed in BT2.1 */
	SHOWCOMMAND ("wae"); /* Removed in BT2.1 */
	SHOWCOMMAND ("rem"); /* Removed in BT2.1 */
	SHOWCOMMAND ("wem"); /* Removed in BT2.1 */
	printlf ("]");

/* Octet 9
0 Read Class Of Device
1 Write Class Of Device
2 Read Voice Setting
3 Write Voice Setting
4 Read Automatic Flush Timeout
5 Write Automatic Flush Timeout
6 Read Num Broadcast Retransmissions
7 Write Num Broadcast Retransmissions*/
	SHOWCOMMAND ("rcod");
	SHOWCOMMAND ("wcod");
	SHOWCOMMAND ("rvs");
	SHOWCOMMAND ("wvs");
	SHOWCOMMAND ("raft");
	SHOWCOMMAND ("waft");
	SHOWCOMMAND ("rnbr");
	SHOWCOMMAND ("wnbr");

/* Octet 10
0 Read Hold Mode Activity
1 Write Hold Mode Activity
2 Read Transmit Power Level
3 Read SCO Flow Control Enable
4 Write SCO Flow Control Enable
5 Set Host Controller To Host Flow Control
6 Host Buffer Size
7 Host Number Of Completed Packets*/
	SHOWCOMMAND ("rhma");
	SHOWCOMMAND ("whma");
	SHOWCOMMAND ("rtpl");
	SHOWCOMMAND ("rsfce");
	SHOWCOMMAND ("wsfce");
	SHOWCOMMAND ("shcthfc");
	SHOWCOMMAND ("hbs");
	SHOWCOMMAND ("hnocp");

/* Octet 11
0 Read Link Supervision Timeout
1 Write Link Supervision Timeout
2 Read Number of Supported IAC
3 Read Current IAC LAP
4 Write Current IAC LAP
5 Read Page Scan Period Mode
6 Write Page Scan Period Mode
7 Read Page Scan Mode*/
	SHOWCOMMAND ("rlst");
	SHOWCOMMAND ("wlst");
	SHOWCOMMAND ("rnosi");
	SHOWCOMMAND ("rcil");
	SHOWCOMMAND ("wcil");
	printlf ("[");
	SHOWCOMMAND ("rpspm"); /* Not in BT2.0 */
	SHOWCOMMAND ("wpspm"); /* Not in BT2.0 */
	printlf ("]");
	SHOWCOMMAND ("rpsm");

/* Octet 12
0 Write Page Scan Mode
1 Set AFH Channel Classification
2 <reserved>
3 <reserved>
4 Read Inquiry Scan Type
5 Write Inquiry Scan Type
6 Read Inquiry Mode
7 Write Inquiry Mode*/
	SHOWCOMMAND ("wpsm");
	SHOWCOMMAND ("sacc");
	/*printlf ("{");*/
	SKIPCOMMAND ("x1");
	SKIPCOMMAND ("x2");
	/*SHOWCOMMAND ("x1");*/		/* Read Extended Data Mode*/
	/*SHOWCOMMAND ("x2");*/		/* Write Extended Data Mode*/
	/*printlf ("}");*/
	SHOWCOMMAND ("rist");
	SHOWCOMMAND ("wist");
	SHOWCOMMAND ("rim");
	SHOWCOMMAND ("wim");

/* Octet 13
0 Read Page Scan Type
1 Write Page Scan Type
2 Read AFH Channel Classification Mode
3 Write AFH Channel Classification Mode
4 Read Anonymity Mode
5 Write Anonymity Mode
6 Read Alias Authentication Enable
7 Write Alias Authentication Enable*/
	SHOWCOMMAND ("rpst");
	SHOWCOMMAND ("wpst");
	SHOWCOMMAND ("raccm");
	SHOWCOMMAND ("waccm");
	printlf ("[");
	SHOWCOMMAND ("ram");	/* Anonymity Mode, removed from the spec */
	SHOWCOMMAND ("wam");	/**/
	SHOWCOMMAND ("raae");	/**/
	SHOWCOMMAND ("waae");	/**/

/* Octet 14
0 Read Anonymous Address Change Parameters
1 Read Anonymous Address Change Parameters
2 Reset Fixed Address Attempts Counter
3 Read Local Version Information
4 Read Local Supported Commands
5 Read Local Supported Features
6 Read Local Extended Features
7 Read Buffer Size*/
	SHOWCOMMAND ("raacp");	/**/
	SHOWCOMMAND ("waacp");	/**/
	SHOWCOMMAND ("rfaac");	/**/
	printlf ("]");
	SHOWCOMMAND ("rlvi");
	/*printlf ("{");*/
	SHOWCOMMAND ("rlsc");	/* Read Local Supported Commands, this was removed from the spec? */
	/*printlf ("}");*/
	SHOWCOMMAND ("rlsf");
	SHOWCOMMAND ("rlef");
	SHOWCOMMAND ("rbs");

/* Octet 15
0 Read Country Code
1 Read BD ADDR
2 Read Failed Contact Count
3 Reset Failed Contact Count
4 Get Link Quality
5 Read RSSI
6 Read AFH Channel Map
7 Read BD Clock*/
	SHOWCOMMAND ("rcc");
	SHOWCOMMAND ("rba");
	SHOWCOMMAND ("rfcc");
	SHOWCOMMAND ("rstfcc");
	SHOWCOMMAND ("glq");
	SHOWCOMMAND ("rr");
	SHOWCOMMAND ("racm");
	SHOWCOMMAND ("rc");

/* Octet 16
0 Read Loopback Mode
1 Write Loopback Mode
2 Enable Device Under Test Mode
3 Setup Synchronous Connection
4 Accept Synchronous Connection
5 Reject Synchronous Connection
6 <reserved>
7 <reserved>*/
	SHOWCOMMAND ("rlm");
	SHOWCOMMAND ("wlm");
	SHOWCOMMAND ("edutm");
	SHOWCOMMAND ("ssc");
	SHOWCOMMAND ("assc");
	SHOWCOMMAND ("rssc");
	SKIPCOMMAND ("");
	SKIPCOMMAND ("");

/* Octet 17
0 Read Extended Inquiry Response
1 Write Extended Inquiry Respone
2 Refresh Encryption Key
3 <reserved>
4 Sniff Subrating
5 Read Simple Pairing Mode
6 write Simple Pairing Mode
7 Read Local OOB Data*/
	SHOWCOMMAND ("reird");
	SHOWCOMMAND ("weird");
	SHOWCOMMAND ("rek");
	SKIPCOMMAND ("");
	SHOWCOMMAND ("sssr");
	SHOWCOMMAND ("rspm");
	SHOWCOMMAND ("wspm");
	SHOWCOMMAND ("rlod");

/* Octet 18
0 Read Inquiry Response Transmit Power
1 Write Inquiry Response Transmit Power
2 Read Default Erroneous Data Reporting
3 Write Default Erroneous Data Reporting
4 <reserved>Read Persistent Sniff Interval
5 <reserved>Write Persistent Sniff Interval
6 <reserved>Delete Persisitent sniff Interval
7 IO Capability Response*/
	SHOWCOMMAND ("ritpl");
	SHOWCOMMAND ("witpl");
	SHOWCOMMAND ("rdedr");
	SHOWCOMMAND ("wdedr");
	SKIPCOMMAND ("");
	SKIPCOMMAND ("");
	SKIPCOMMAND ("");
	SHOWCOMMAND ("iocr");

/* Octet 19
0 User Confirmation Request Reply Command
1 User Confirmation Request Negative Reply Command
2 User PassKey Request Reply command
3 User PassKey Request Negative Reply command
4 Remote OOB Data Request Reply Command
5 Write Simple Pairing Debug Mode
6 Enhanced Flush
7 Remote OOB Data Negative Request Reply Command*/
	SHOWCOMMAND ("ucrr");
	SHOWCOMMAND ("ucrnr");
	SHOWCOMMAND ("uprr");
	SHOWCOMMAND ("uprnr");
	SHOWCOMMAND ("rodrr");
	SHOWCOMMAND ("wspdm");
	SHOWCOMMAND ("ef");
	SHOWCOMMAND ("rodnrr");

/* Octet 20
0 Reserved
1 Reserved
2 Send Keypress Notification Command
3 IO Capability Request Negative Reply Command
4 Read Encryption Key Size
5 Reserved 
6 Reserved
7 Reserved*/
	SKIPCOMMAND ("");
	SKIPCOMMAND ("");
	SHOWCOMMAND ("skn");
	SHOWCOMMAND ("iocrnr");
	SHOWCOMMAND ("reks");
	SKIPCOMMAND ("");
	SKIPCOMMAND ("");
	SKIPCOMMAND ("");

/* Octet 21
0 Create Physical Link
1 Accept Physical Link
2 Disconnect Physical Link
3 Create Logical Link
4 Accept Logical Link
5 Disconnect Logical Link 
6 Logical Link Cancel
7 Flow Spec Modify*/
    SHOWCOMMAND ("cpl");
    SHOWCOMMAND ("apl");
    SHOWCOMMAND ("dpl");
    SHOWCOMMAND ("cll");
    SHOWCOMMAND ("all");
    SHOWCOMMAND ("dll");
    SHOWCOMMAND ("llc");
    SHOWCOMMAND ("fsm");

/* Octet 22
0 Read Logical Link Accept Timeout
1 Write Logical Link Accept Timeout
2 Set Event Mask Page 2
3 Read Location Data
4 Write Location Data
5 Read Local AMP Info 
6 Read Local AMP_ASSOC
7 Write Remote AMP_ASSOC*/
    SHOWCOMMAND ("rllat");
    SHOWCOMMAND ("wllat");
    SHOWCOMMAND ("semp2");
    SHOWCOMMAND ("rld");
    SHOWCOMMAND ("wld");
    SHOWCOMMAND ("rlai");
    SHOWCOMMAND ("rlaa");
    SHOWCOMMAND ("wraa");

/* Octet 23
0 Read Flow Control Maode
1 Write Flow Control Mode
2 Read Data Block Size
3 Reserved
4 Reserved
5 Enable AMP Receiver Reports 
6 AMP Test End
7 AMP Test Command*/
    SHOWCOMMAND ("rfcm");
    SHOWCOMMAND ("wfcm");
    SHOWCOMMAND ("rdbs");
    SKIPCOMMAND ("");
    SKIPCOMMAND ("");
    SHOWCOMMAND ("earr");
    SHOWCOMMAND ("ate");
    SHOWCOMMAND ("atc");

/* Octet 24
0 Read Enhanced Transmit Power Level
1 Reserved
2 Read Best Effort Flush Timeout
3 Write Best Effort Flush Timeout
4 Short Range Mode
5 Reserved 
6 Reserved
7 Reserved*/
    SHOWCOMMAND ("retpl");
    SKIPCOMMAND ("");
    SHOWCOMMAND ("rbeft");
    SHOWCOMMAND ("wbeft");
    SHOWCOMMAND ("srm");
    SHOWCOMMAND ("rlehs");
    SHOWCOMMAND ("wlehs");
    SKIPCOMMAND ("");

/* Octet 25
0 LE Set Event Mask
1 LE Read Buffer Size
2 LE Read Local Supported Features
3 LE Set Local Used Features
4 LE Set Random Address
5 LE Set Advertising Parameters 
6 LE Read Advertising Channel TX Power
7 LE Set Advertising Data*/
    SHOWCOMMAND ("lesem");
    SHOWCOMMAND ("lerbs");
    SHOWCOMMAND ("lerlsf");
    SHOWCOMMAND ("lesluf");
    SHOWCOMMAND ("lesra");
    SHOWCOMMAND ("lesap");
    SHOWCOMMAND ("leractp");
    SHOWCOMMAND ("lesad");

/* Octet 26
0 LE Set Scan Response Data
1 LE Set Advertise Enable
2 LE Set Scan Parameters
3 LE Set Scan Enable
4 LE Create Connection
5 LE Create Connection Cancel 
6 LE Read White List Size
7 LE Clear White List*/
    SHOWCOMMAND ("lessrd");
    SHOWCOMMAND ("lesae");
    SHOWCOMMAND ("lessp");
    SHOWCOMMAND ("lesse");
    SHOWCOMMAND ("lecc");
    SHOWCOMMAND ("leccc");
    SHOWCOMMAND ("lerwls");
    SHOWCOMMAND ("lecwl");

/* Octet 27
0 LE Add Device To White list
1 LE Remove Device From White List
2 LE Connection Update
3 LE Set Host Channel Classification
4 LE Read Channel Map
5 LE Read Remote Used Features 
6 LE Encrypt
7 LE Rand*/
    SHOWCOMMAND ("leadtwl");
    SHOWCOMMAND ("lerdfwl");
    SHOWCOMMAND ("lecu");
    SHOWCOMMAND ("leshcc");
    SHOWCOMMAND ("lercm");
    SHOWCOMMAND ("lerruf");
    SHOWCOMMAND ("lee");
    SHOWCOMMAND ("ler");

/* Octet 28
0 LE Start Encryption
1 LE Long Term Key Requested Reply
2 LE Long Term Key Requested Negative Reply
3 LE Read Supported States
4 LE Receiver Test
5 LE Transmitter Test 
6 LE Test End
7 Reserved*/
    SHOWCOMMAND ("lese");
    SHOWCOMMAND ("leltkrr");
    SHOWCOMMAND ("leltkrnr");
    SHOWCOMMAND ("lerss");
    SHOWCOMMAND ("lert");
    SHOWCOMMAND ("lett");
    SHOWCOMMAND ("lete");
    SKIPCOMMAND ("");
        
    printlf (")");

#undef SHOWCOMMAND
#undef SKIPCOMMAND
}
