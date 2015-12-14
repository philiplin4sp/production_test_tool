/****************************************************************************
FILE
	hqpdu.h  -  bluecore host querier packet structure

DESCRIPTION
	There are two bidirectional "private channels" that carry messages
	between the host and the host controller (aka the bc01 chip) over the
	BCSP link:
	
	1) One carries commands from a control entity on the host (possibly
	   called bcmgr) to the bccmd task on the host.  The bccmd task is a
	   command interpreter, carrying out local actions in response to
	   the host's commands.  The channel's reverse path carries command
	   responses back to the control entity on the host.
	
	   This channel will allow the host to configure, control and
	   monitor the CSR/bluecore Bluetooth implementation.

	2) The second channel carries commands from the host querier (hq)
	   task on the host controller to a command interpreter on the
	   host.  The reverse channel carries back the host's command
	   responses.
	   
	   This channel has found only very limited use in the past, with the
           main messages being from fault(), delayed_panic() and a "booted"
           message from onchipbcmgr. DSP manager will generate messages in a
           number of situations.
	
	This file describes the protocol pdus carried on the second channel.
	The first channel's protocol is described in bccmdpdu.h.

	These pdus are structured to fit CCL's "bluecore friendly format",
	i.e. the pdus' wire structures match the xap's memory structure.  The
	lower byte of each uint16 travels over the wire first (confirmed by
	jrrk).  Thus, the pdus can be defined as C structures on the
	understanding that the wire format will be a (byte) serialisation of
	the way in which the types will be laid out by the xap C compiler.
	The pdus defined here are an even number of bytes as implied by use
	of the "bluecore friendly format".

	The hq protocol is similar in style to that of the command
	interpreter, bccmd and is similarly influenced by snmp.  The host
	controller (client) views the services presented by the host (server)
	as a set of values to be read and/or written.  Only two types of
	transaction are defined:

	    To get a value, the client sends a GET-REQUEST pdu to the
	    server.  The server reads the value and returns it in a
	    GET-RESPONSE pdu to the host.

	    To set a value, the client sends a SET-REQUEST pdu to the
	    server.  The server sets the value and returns the new value in a
	    GET-RESPONSE pdu to the host.

	There are only three basic types of PDU:

            GET-REQUEST - (called HQPDU_GETREQ below) asks for the value
               of a variable to be returned to the host.

            GET-RESPONSE - (called HQPDU_GETRESP below) returns the value
                of a variable to the host.

            SET-REQUEST - (called HQPDU_SETREQ below) asks to set the
                value of a variable. 

	The host controller commands the host to perform an action or gives a 
	status report by writing to a particular variable.  Abstracting the
	bccmd's functionality as reading and writing variables promotes a
	simple protocol.

	No attempt is made to support snmp's SMI - the way in which snmp
	names variables.  This protocol uses a single uint16 to identify each
	variable.

	Every REQUEST received by the host causes it to (attempt to) return
	one RESPONSE.  However, in exceptional circumstances a response may
	not be sent or received.  Thus, the host controller must use a
	timeout.

	The GET-REQUEST PDU is always the same size as the corresponding
	GET-RESPONSE PDU.  Similarly, the SET-REQUEST PDU is always the same
	size as the corresponding GET-RESPONSE PDU.

	The type HQPDU is of fixed size.  However, it is acceptable for
	payload part of a message to be longer than this.  The actual length
	of the PDU is given in HQPDU.pdulen.  Awkward payloads may overhang
	the sizeof(HQPDU) in the manner of bccmdpdu.h, however for all
	current messages there is no overhang - all messages fit within the
	HQPDU.  The result is that all HQPDU messages are currently the
	same size.

	HQ responses can be passed over SPI through HQ Scraping.  If the 
	length of a new pdu type is longer than the request passed from the 
	host - it will not be returned through this mechanism.  This may be 
	acceptable as not all content is required in production - but worth
	reviewing messages if they are longer.

        Example Transactions
        --------------------  

        1) To signal that the bc01 has just been booted.

          hq->host HQPDU_SETREQ

            uint16 field             value
            1      HQPDU.type        2      (HQPDU_SETREQ)
            2      HQPDU.pdulen      12     (number of uint16s in PDU)
            3      HQPDU.seqno       1235   (value chosen by bc01)
            4      HQPDU.varid       1      (HQVARID_BOOTED)
            5      HQPDU.status      0      (HQPDU_STAT_OK)
            6      HQPDU.d.dummy     0      (no payload)
            7->12  HQPDU.d  (unused) 0      (unused space - set to zero)

          host->hq HQPDU_GETRESP

            uint16 field             value
            1      HQPDU.type        1      (HQPDU_GETRESP)
            2      HQPDU.pdulen      12     (number of uint16s in PDU)
            3      HQPDU.seqno       1235
            4      HQPDU.varid       1      (HQVARID_BOOTED)
            5      HQPDU.status      0      (HQPDU_STAT_OK)
            6      HQPDU.d.dummy     0      (no payload)
            7->12  HQPDU.d  (unused) 0      (unused space - set to zero)

        2) To signal that the bc01 is about to reboot itself following
	   an irrecoverable error.  The response may not reach the bc01
	   in time, coz' it may be rebooting.

          hq->host HQPDU_SETREQ

            uint16 field             value
            1      HQPDU.type        2      (HQPDU_SETREQ)
            2      HQPDU.pdulen      12     (number of uint16s in PDU)
            3      HQPDU.seqno       1236   (value chosen by bc01)
            4      HQPDU.varid       2049   (HQVARID_DELAYED_PANIC)
            5      HQPDU.status      0      (HQPDU_STAT_OK)
            6      HQPDU.d.u16       31     (panicid - deathbed confession)
	    7-12   HQPDU.d (unused)  0	    (unused space - set to zero)

          host->hq HQPDU_GETRESP

            uint16 field             value
            1      HQPDU.type        1      (HQPDU_GETRESP)
            2      HQPDU.pdulen      12     (number of uint16s in PDU)
            3      HQPDU.seqno       1236
            4      HQPDU.varid       2049   (HQVARID_DELAYED_PANIC)
            5      HQPDU.status      0      (HQPDU_STAT_OK)
            6      HQPDU.d.u16       31     (panicid - deathbed confession)
	    7-12   HQPDU.d (unused)  0	    (unused space - set to zero)

        3) To pass a fault report to the host.

          hq->host HQPDU_SETREQ

            uint16 field             value
            1      HQPDU.type        2      (HQPDU_SETREQ)
            2      HQPDU.pdulen      12     (number of uint16s in PDU)
            3      HQPDU.seqno       1237   (value chosen by bc01)
            4      HQPDU.varid       4096   (HQVARID_FAULT)
            5      HQPDU.status      0      (HQPDU_STAT_OK)
            6      HQPDU.d.fault.f   10     (faultid - failure code)
            7-8    HQPDU.d.fault.t   999999 (TIME is uint32, low word 1st)
            9      HQPDU.d.fault.n   3      (Number of times fault occurred)
            10-12  HQPDU.d  (unused) 0      (unused space - set to zero)

          host->hq HQPDU_GETRESP

            uint16 field             value
            1      HQPDU.type        1      (HQPDU_GETRESP)
            2      HQPDU.pdulen      12     (number of uint16s in PDU)
            3      HQPDU.seqno       1237
            4      HQPDU.varid       4096   (HQVARID_FAULT)
            5      HQPDU.status      0      (HQPDU_STAT_OK)
            6      HQPDU.d.fault.f   10     (faultid - failure code)
            7-8    HQPDU.d.fault.t   999999 (TIME is uint32, low word 1st)
            9      HQPDU.d.fault.n   3      (Number of times fault occurred)
            10-12  HQPDU.d  (unused) 0      (unused space - set to zero)

        4) Host refuses request to read unknown value.

          hq->host HQPDU_GETREQ

            uint16 field             value
            1      HQPDU.type        0      (HQPDU_GETREQ)
            2      HQPDU.pdulen      12     (number of uint16s in PDU)
            3      HQPDU.seqno       1238   (value chosen by bc01)
            4      HQPDU.varid       12345  (HQVARID_TWADDLE)
            5      HQPDU.status      0      (HQPDU_STAT_OK)
            6      HQPDU.d.dummy     0      (no payload)
            7->12  HQPDU.d  (unused) 0      (unused space - set to zero)

          host->hq HQPDU_GETRESP

            uint16 field             value
            1      HQPDU.type        1      (HQPDU_GETRESP)
            2      HQPDU.pdulen      12     (number of uint16s in PDU)
            3      HQPDU.seqno       1238
            4      HQPDU.varid       12345  (HQVARID_TWADDLE)
            5      HQPDU.status      1      (HQPDU_STAT_NO_SUCH_VARID)
            6      HQPDU.d.dummy     0      (no payload)
            7->12  HQPDU.d  (unused) 0      (unused space - set to zero)

MODIFICATION HISTORY
	1.1  20:apr:01  ajh	Moved to interface/host/hq.
				Removed dependency on bt.h.
	1.2  13:jun:01  cjo	Corrected comment - bcmon task removed.
	1.3  02:oct:01  pws     Added HQPDU_RADIOTEST_STATUS_ARRAY.
	1.4.branched    ajh     Changed HQPDU_ARRAYn to more meaningfully
				named types.  Corrected a few comments.
	1.6  21:nov:02  pws     H17.71: HQVARID_BER_TRIGGER placeholder.
	1.7  22:nov:02  pws     H17.71: Rationalise names.
	#10  10:feb:04  mm      B-1494: Added HQPDU_DRAIN_CALIBRATE.
	#11  29:sep:04  mm      B-3367: Update drain to use info about Ext-PA
        #12  23:feb:05  ay02    B-5865: Added HQPDU_SNIFF_REQ
        #13  29:mar:05  ay02    B-6126: Added HQPDU_ESCO_REQ
        #20  30:jan:06  ay02    B-11538 FM radio events sent over HQ
        #21  08:feb:06  jn01    B-11809: make HQ FM_RDS_DATA match spec
        #22  02:mar:06  sk03    B-12413: Integrate BC5 support onto main.
        #24  18:may:06  sv01    B-12174: Update drain pdu with FM radio.

OLD MODIFICATION HISTORY
	1.1  10:dec:99	sm	Created.
	1.5  6:apr:00	cjo	Reworked.
	1.6  10:apr:00	cjo	Restructured.
	1.8  11:apr:00	cjo	Added radiotest reports.
	1.9  19:apr:00  cjo     Additions agreed between James and jyb's
				client.  Extra vals in HQPDU_RXPKTSTATS.
	1.10 11:jul:00  pws     New PDU's for returning radiotest data.
	1.11 29:aug:00  pws     Note ordering of 32-bit integers in bit err.
	1.12 18:oct:00  cjo     Corrected comment.
	1.13 18:oct:00  cjo     Corrected more comments - pws increased the
				size of HQPDU, so all of the above example
				transactions were broken.
	1.14 15:dec:00  pws     Add HQVARID_RADIO_STATUS.
	1.15 19:jan:01  cjo     Made comments correcter.
				Aligned HQPDU_STAT* codes with bccmdpdu.h.
	1.16 22:jan:01  pws     Say `(0 to 8)'.
	1.17 01:mar:01  pws     New argument for hq_radio_status().
	1.18 01:mar:01  pws     Change PDU length in examples to 12.
	1.19 12:may:02  npm     Add HQ_PDU_PROTOCOL_ERROR .
        ------------------------------------------------------------------
        --- This modification history is now closed. Do not update it. ---
        ------------------------------------------------------------------
*/

#ifndef	__HQPDU_H__
#define	__HQPDU_H__
 
#ifdef RCS_STRINGS
static char hqpdu_h_id[]
  = "$Id: //depot/bc/bluesuite_2_4/interface/host/hq/hqpdu.h#1 $";
#endif


/***************************************************************************
HQPDU_FAULT
===========

A report of a set of fault() calls on the bc01. */

typedef struct {
	uint16		f;	/* A fault identifier. */
	uint32		t;	/* TIME of first report. */
	uint16		n;	/* Number of f reports. */
	} HQPDU_FAULT;

/***************************************************************************
HQPDU_PROTOCOL_ERROR
====================

A report of an error in an air protocol 1. */

typedef struct {
     uint16		layer;	/* Layer which detected problem */
     uint16		entity;	/* remote or local entity*/
     uint16		error;	/* error code*/
     uint16             info0 ; /* optional additional information */
     uint16             info1 ; /* optional additional information */
     uint16             info2 ; /* optional additional information */
	} HQPDU_PROTOCOL_ERROR;


/**************************************************************************
HQPDU_RXPKTSTATS
================

A report of a set of received packets and correctable received packets from
one of the bc01-specific radio tests in the radiotest subsystem. */

typedef struct {
	uint16		n_pkts;			/* Total no. pkts rxed. */
	uint16		n_good_pkts;		/* HERE wtf is a "good" pkt? */
	uint16		n_correctable_pkts;	/* No. correctable pkts rxed */
	uint16		rssi;			/* avg rssi value.  */
	bool		rssi_valid;		/* Is "rssi" valid? */
	} HQPDU_RXPKTSTATS;

/**************************************************************************
HQPDU_LUT_ENTRY
===============

A part of an array report with a single uint16 value. */

typedef struct {
	uint16		index;			/* Index of array element */
	uint16		trim;			/* Value to return */
	} HQPDU_LUT_ENTRY;

/**************************************************************************
HQPDU_SYNTH_SETTLE
==================

A part of an array report with a single uint16 value. */

typedef struct {
	uint16		index;			/* Index of array element */
	uint16		voltage;		/* Value to return */
	} HQPDU_SYNTH_SETTLE;

/**************************************************************************
HQPDU_RADIO_STATUS_ARRAY
========================

A part of an array report with a single uint16 value. */

typedef struct {
	uint16		index;			/* Index of array element */
	uint16		value;			/* Value to return */
	} HQPDU_RADIO_STATUS_ARRAY;

/**************************************************************************
HQPDU_FILTER_RESP
=================

A part of an array report with two uint16 values. */

typedef struct {
	uint16		index;			/* Index of array element */
	uint16		freq;			/* First value to return */
	uint16		rssi;			/* Second value to return */
	} HQPDU_FILTER_RESP;

/**************************************************************************
HQPDU_IQ_MATCH
==============

A part of an array report with three uint16 values. */

typedef struct {
	uint16		index;			/* Index of array element */
	uint16		trim;			/* First value to return */
	uint16		rssi_chan;		/* Second value to return */
	uint16		rssi_image;		/* Third value to return */
	} HQPDU_IQ_MATCH;

/**************************************************************************
HQPDU_BIT_ERR
=============

One measurement for a bit error report.  The type of measurement is given
by the index.  These are as listed for radiotest_bit_err1 in radiotest.h.
The two values are a count over the last report period (default 1 second),
and a cumulative amount to date (until the target count is reached, when
it is reset). 

Note that 32-bit integers have their words in XAP order.  This means
that the data is effectively a series of 16-bit words as follows:
  index (0 to 8)
  high word of value_last
  low word of value_last
  high word of value_tot
  low word of value_tot
*/

typedef struct {
	uint16		index;			/* Type of measurement */
	uint32		value_last;		/* Value over last period */
	uint32		value_tot;		/* Value since reset */
	} HQPDU_BIT_ERR;

/**************************************************************************
HQPDU_RADIO_STATUS
==================

A set of six numbers returned by radiotest_get_radio_report.  They are the
values of the registers ANA_TX_LVL, ANA_TX_EXT_LVL, ANA_RX_LVL, ANA_LO_LVL
an ANA_IQ_TRIM, if the IQ trim was updated, a value of the signal
to image ratio, and the register RX_ATTEN.  If you don't already know what
those are, you aren't interested.
*/

typedef struct {
	uint16		tx_lvl;		/* Internal transmission level */
	uint16		tx_ext_lvl;	/* External transmission level */
	uint16		rx_lvl;		/* Reception level */
	uint16		lo_lvl;		/* Local oscillator level */
	uint16		iq_trim;	/* IQ trim */
	uint16		sir;		/* Signal/image ratio for IQ trim */
	uint16		rx_atten;	/* Receiver attentuation */
} HQPDU_RADIO_STATUS;


/* Red-M Change Starts */

/**************************************************************************
HQPDU_BER_TRIGGER
===================

Sent when a configured bit error rate has been hit. Used to trigger
handoff operations.
*/

typedef struct {
	uint16 hci_handle;
	uint16 avg_ber;
	uint16 raw_ber;
	uint16 ber_threshold;		/* Only 8 bits used */
} HQPDU_BER_TRIGGER;

/* Red-M Change Ends */


/**************************************************************************
HQPDU_DRAIN_CALIBRATE
=====================

This is sent when we are configuring the drain sub-system.
*/

typedef struct {
    uint32	awake_time;
    uint32      sleep_time;
    uint32      half_slots;
    uint32      half_slots_pa;
    uint32      fm_radio;
    uint32      fm_tx_radio;
    uint32      fm_lo;
} HQPDU_DRAIN_CALIBRATE;


/**************************************************************************
HQPDU_SNIFF_REQ
===============

Sniff parameter configuration
*/

typedef struct {
     uint16 id; /* only 3 bits used */
     uint16 t;
     uint16 d;
     uint16 attempts;
     uint16 timeout;
} HQPDU_SNIFF_REQ;

/**************************************************************************
HQPDU_ACCEPT_TDI
================

Sniff parameters received from peer
The host's reply (RESP) will just say
NO, YES, or COUNT_EXCEEDED instead of t
*/

typedef struct {
     uint16 id;
     uint16 t;
     uint16 d;
     uint16 attempts;
     uint16 timeout;
     uint16 event;
} HQPDU_ACCEPT_TDI;

/**************************************************************************
HQPDU_ESCO_REQ
================

The fields from the LMP_esco_req PDU so that the host can decide what
value to use for DeSCO. Only this field is used when the RESP is received
by the controller.
*/

typedef struct {
     uint16 sco_handle;
     uint16 hci_handle;
     uint16 pt_s2m; /* 8 bits*/
     uint16 pt_m2s;
     uint16 tesco;
     uint16 wesco;
     uint16 dsco; /* filled in by host */
} HQPDU_ESCO_REQ;


/**************************************************************************
HQPDU_FM_EVENT
===============

*/

typedef struct {
     uint16 flags;
     uint16 reg_vals[6]; /* Register + Value pairs */
} HQPDU_FM_EVENT;


/**************************************************************************
HQPDU_FMTX_EVENT
===============

*/

typedef struct {
     uint16 flags;
     uint16 reg_vals[6]; /* Register + Value pairs */
} HQPDU_FMTX_EVENT;

/**************************************************************************
HQPDU_FM_RDS_DATA
=================

*/

typedef struct {
     uint16 rds_data_len; /* number of blocks */
     uint16 rds_data[1];
} HQPDU_FM_RDS_DATA;


/**************************************************************************
HQPDU_ADC_RESULT 
================

Returns the (unsigned) result of an adc reading requested by BCCMD_ADC_READ 

The host should accept this 10 bit value only if "channel" matches
the value written to BCCMDVARID_ADC_READ.

Note: the result is always 10 bit. On chips with 8 bit ADC's, it will
have the bottom two bits zero, as a result of hal_adc_convert converting
8 bit to 10 bit via result << 2.
*/

typedef struct {
     uint16 result; 
     uint16 channel;
} HQPDU_ADC_RESULT; 


/**************************************************************************
HQPDU_GPS_STATUS
================

Returns the GPS status.
*/

typedef struct {
        uint16 flags;
} HQPDU_GPS_STATUS; 

/**************************************************************************
HQPDU_EGPS_TIME_STAMP
================

Returns the eGPS Time Stamp for the INPUT scenario
*/

typedef struct {
        uint32 time;
        uint8  time_fine;
        uint8  flags;
} HQPDU_EGPS_TIME_STAMP; 

/**************************************************************************
HQPDU_SCO_STREAM_HANDLES 
========================

Returns the SCO stream's handles
*/

typedef struct {
    uint16 sco_handle;
    uint16 source_id; 
    uint16 sink_id;
} HQPDU_SCO_STREAM_HANDLES; 

/**************************************************************************
HQPDU_COEX_UART_MESSAGE
=======================

Used for debugging the Coex UART interface on Leo. The data will be
(in general) the 7 bytes of data received over the interface. The
extra words can be used for other information in debugging modes.
*/
typedef struct {
    uint16 info[8];
} HQPDU_COEX_UART_MESSAGE;

/**************************************************************************
HQPDU_DSPMANAGER_DEBUG_INFO_B
=============================

Used for debugging the Baton interface within DSP manager.
*/
typedef struct {
    uint32 time_us;
    uint16 msgid;
    uint16 msglen;
    uint16 msg[1];  /* Struct hack */
} HQPDU_DSPMANAGER_DEBUG_INFO_B;

/**************************************************************************
HQPDU_MESSAGE_FROM_OPERATOR
=============================

Used for sending message from DSP operator to the host.
*/
typedef struct {
    uint16 opid;
    uint16 message[1];    /* Struct hack */
} HQPDU_MESSAGE_FROM_OPERATOR;

/**************************************************************************
HQPDU
=====

The protocol's pdu structure. */

typedef struct {
	/* PDU type. */
	uint16		type;		

/* Acceptable values for "type". */
#define	HQPDU_GETREQ		(0)
#define	HQPDU_GETRESP		(1)
#define	HQPDU_SETREQ		(2)

	/* Length of PDU in uint16s. */
	uint16		pdulen;			

	/* Sequence number. */
	uint16		seqno;		

	/* Id of variable to be accessed. */
	uint16		varid;		

	/* How host managed to perform operation. */
	uint16		status;		

/* Acceptable values for "status". */
#define	HQPDU_STAT_OK		(0)	/* Default.  No problem found. */
#define	HQPDU_STAT_NO_SUCH_VARID (1)	/* varid not recognised. */
#define	HQPDU_STAT_TOO_BIG	(2)	/* Data exceeded pdu/mem capacity.*/
#define	HQPDU_STAT_NO_VALUE	(3)	/* Variable has no value. */
#define	HQPDU_STAT_BAD_PDU	(4)	/* Bad value(s) in PDU. */
#define	HQPDU_STAT_NO_ACCESS	(5)	/* Bad value(s) in PDU. */
#define	HQPDU_STAT_READ_ONLY	(6)	/* Value of varid inaccessible. */
#define	HQPDU_STAT_WRITE_ONLY	(7)	/* Bad value(s) in PDU. */
#define	HQPDU_STAT_ERROR	(8)	/* The useless catch-all error. */
#define HQPDU_STAT_PERMISSION_DENIED (9) /* Request not allowed. */


/**************************************************************************
That's the end of the common header of an hqpdu.  Now follows stuff that
varies according to the settings of the "type" and "varid" fields.

Not all of the types in the following union occupy all of the union's space.
All unused space must be set to zero.

*** IF ADDING INFORMATION

Check whether HQ Scraping continues to work as this can be broken if the
size of the structure extends.  The Host Tools will need to change, but 
useful to discover in advance.
  Test with BlueTest3.
  Connect using SPI.
  Provoke a Fault (non 0 fault code)
  Response should appear
**************************************************************************/

	union {
		/* Useless placeholder.  Nothing is written into HQPDU.d if
		"type" is HQPDU_GETREQ or if "varid" is between
		HQVARID_COMMANDS_START and (HQVARID_UINT8_START-1). */

		uint16	dummy;

		/* For use if "varid" is between HQVARID_UINT8_START and
		(HQVARID_UINT16_START-1). */

		uint8	u8;

		/* For use if "varid" is between HQVARID_UINT16_START and
		(HQVARID_UINT32_START-1). */

		uint16	u16;

		/* For use if "varid" is between HQVARID_UINT32_START and
		(HQVARID_SPECIAL_START-1). */

		uint32	u32;

		/* If "varid" is HQVARID_SPECIAL_START or above then
		one of a set of data structures follows. */

		HQPDU_FAULT              fault;	   /* HQVARID_FAULT          */
		HQPDU_PROTOCOL_ERROR     protocol_error;
                                                   /* HQVARID_PROTOCOL_ERROR */
		HQPDU_RXPKTSTATS         rxpkt_stats;
                                                   /* HQVARID_RXPKTSTATS     */
		HQPDU_LUT_ENTRY          lut_entry;/* HQVARID_LUT_ENTRY      */
		HQPDU_SYNTH_SETTLE       synth_settle;
                                                   /* HQVARID_LUT_ENTRY      */
		HQPDU_RADIO_STATUS_ARRAY radio_status_array;
                                                   /* HQVARID_LUT_ENTRY      */
		HQPDU_FILTER_RESP        filter_resp;
                                                   /* HQVARID_FILTER_RESP    */
		HQPDU_IQ_MATCH           iq_match; /* HQVARID_IQ_MATCH       */
		HQPDU_BIT_ERR            bit_err;  /* HQVARID_BIT_ERR        */
	        HQPDU_RADIO_STATUS       radio_status;
                                                   /* HQVARID_RADIO_STATUS   */
		HQPDU_BER_TRIGGER	 ber_trigger; /* HQVARID_BER_TRIGGER */
	        HQPDU_DRAIN_CALIBRATE    drain; /* HQVARID_DRAIN_CALIBRATE */
                HQPDU_SNIFF_REQ          sniff; /* HQVARID_SNIFF_REQ */
                HQPDU_ACCEPT_TDI         accept_tdi; /* HQVARID_ACCEPT_TDI */
                HQPDU_ESCO_REQ           esco;       /* HQPDU_ESCO_REQ */
                HQPDU_FM_EVENT           fm_event; /* HQVARID_FM_EVENT */
                HQPDU_FM_RDS_DATA        fm_rds_data; /* HQVARID_FM_RDS_DATA */
                HQPDU_ADC_RESULT         adc_result; /* HQVARID_ADC_RESULT */
                HQPDU_GPS_STATUS         gps_status;
                HQPDU_EGPS_TIME_STAMP    egps_time_stamp;
                HQPDU_SCO_STREAM_HANDLES sco_stream_handles;
                HQPDU_COEX_UART_MESSAGE  coex_uart_message;
                HQPDU_DSPMANAGER_DEBUG_INFO_B   dspmanager_debug_info_b;
                HQPDU_MESSAGE_FROM_OPERATOR  message_from_operator;
		} d;

	} HQPDU;


/***************************************************************************
The varids are listed in blocks.  Each block holds variables of a given
type.  This allows the host to determine a variable's type by seeing which
range it falls in. */


/***************************************************************************
COMMAND VARIDS (variables which invoke commands when written to). */

#define	HQVARID_COMMANDS_START 	(0)

/* HQVARID_NO_VARIABLE harmlessly and silently does nothing. */
#define	HQVARID_NO_VARIABLE	(HQVARID_COMMANDS_START+0)

/* Message to the host that the bc01 has just booted itself. */
#define	HQVARID_BOOTED		(HQVARID_COMMANDS_START+1)

/* HQs for Stibbons (DSPmanager).
 * It's not yet known exactly what commands there will be, so for
 * now a block of 16 values are reserved and will be filled in as
 * the code is integrated back to mainline. When the work is
 * finished the remaining values will be relinquished.
 */

#define HQVARID_STIBBONS_C01 (HQVARID_COMMANDS_START+2)
#define HQVARID_STIBBONS_C02 (HQVARID_COMMANDS_START+3)
#define HQVARID_STIBBONS_C03 (HQVARID_COMMANDS_START+4)
#define HQVARID_STIBBONS_C04 (HQVARID_COMMANDS_START+5)
#define HQVARID_STIBBONS_C05 (HQVARID_COMMANDS_START+6)
#define HQVARID_STIBBONS_C06 (HQVARID_COMMANDS_START+7)
#define HQVARID_STIBBONS_C07 (HQVARID_COMMANDS_START+8)
#define HQVARID_STIBBONS_C08 (HQVARID_COMMANDS_START+9)
#define HQVARID_STIBBONS_C09 (HQVARID_COMMANDS_START+10)
#define HQVARID_STIBBONS_C10 (HQVARID_COMMANDS_START+11)
#define HQVARID_STIBBONS_C11 (HQVARID_COMMANDS_START+12)
#define HQVARID_STIBBONS_C12 (HQVARID_COMMANDS_START+13)
#define HQVARID_STIBBONS_C13 (HQVARID_COMMANDS_START+14)
#define HQVARID_STIBBONS_C14 (HQVARID_COMMANDS_START+15)
#define HQVARID_STIBBONS_C15 (HQVARID_COMMANDS_START+16)
#define HQVARID_STIBBONS_C16 (HQVARID_COMMANDS_START+17)


/***************************************************************************
Variables with type [u]int8.  The variable's value is held in HQPDU.d.u8. */

#define	HQVARID_UINT8_START 	(1024)


/****************************************************************************
Variables with type [u]int16.  The variable's value is held in HQPDU.d.u16. */

#define	HQVARID_UINT16_START 	(2048)

/* Msg to the host that the bc01 is about to reboot.  Reason in the u16. */
#define	HQVARID_DELAYED_PANIC	(HQVARID_UINT16_START+0)

/* An rssi report from one of the bc01's radio's test modes. */
#define	HQVARID_RSSI		(HQVARID_UINT16_START+1)


/* HQs for Stibbons (DSPmanager).
 * It's not yet known exactly what commands there will be, so for
 * now a block of 16 values are reserved and will be filled in as
 * the code is integrated back to mainline. When the work is
 * finished the remaining values will be relinquished.
 */

/* Downloading a capability succeeded. Capability ID in the u16. */
#define HQVARID_CAPABILITY_DOWNLOAD_COMPLETE (HQVARID_UINT16_START+3)

#define HQVARID_STIBBONS_U02 (HQVARID_UINT16_START+4)
#define HQVARID_STIBBONS_U03 (HQVARID_UINT16_START+5)
#define HQVARID_STIBBONS_U04 (HQVARID_UINT16_START+6)
#define HQVARID_STIBBONS_U05 (HQVARID_UINT16_START+7)
#define HQVARID_STIBBONS_U06 (HQVARID_UINT16_START+8)
#define HQVARID_STIBBONS_U07 (HQVARID_UINT16_START+9)
#define HQVARID_STIBBONS_U08 (HQVARID_UINT16_START+10)
#define HQVARID_STIBBONS_U09 (HQVARID_UINT16_START+11)
#define HQVARID_STIBBONS_U10 (HQVARID_UINT16_START+12)
#define HQVARID_STIBBONS_U11 (HQVARID_UINT16_START+13)
#define HQVARID_STIBBONS_U12 (HQVARID_UINT16_START+14)
#define HQVARID_STIBBONS_U13 (HQVARID_UINT16_START+15)
#define HQVARID_STIBBONS_U14 (HQVARID_UINT16_START+16)
#define HQVARID_STIBBONS_U15 (HQVARID_UINT16_START+17)
#define HQVARID_STIBBONS_U16 (HQVARID_UINT16_START+18)

/* Message for the number of packets counted during a BLE radio test */
#define HQVARID_BLE_RADIO_TEST (HQVARID_UINT16_START+19)


/****************************************************************************
Variables with type [u]int32.  The variable's value is held in HQPDU.d.u32. */

#define	HQVARID_UINT32_START 	(3072)


/***************************************************************************
Variables of special (i.e., awkward) types. */

#define	HQVARID_SPECIAL_START 	(4096)

/* Report in HQPDU.d.fault. */
#define	HQVARID_FAULT		(HQVARID_SPECIAL_START+0)

/* Report in HQPDU.d.rxpkt_stats. */
#define	HQVARID_RXPKTSTATS	(HQVARID_SPECIAL_START+1)

/* Report in HQPDU.d.lut_entry */
#define HQVARID_LUT_ENTRY	(HQVARID_SPECIAL_START+2)

/* Report in HQPDU.d.synth_settle */
#define HQVARID_SYNTH_SETTLE	(HQVARID_SPECIAL_START+3)

/* Report in HQPDU.d.filter_resp */
#define HQVARID_FILTER_RESP	(HQVARID_SPECIAL_START+4)

/* Report in HQPDU.d.iq_match */
#define HQVARID_IQ_MATCH	(HQVARID_SPECIAL_START+5)

/* Report in HQPDU.d.bit_err */
#define HQVARID_BIT_ERR		(HQVARID_SPECIAL_START+6)

/* Report in HQPDU.d.radio_status */
#define HQVARID_RADIO_STATUS	(HQVARID_SPECIAL_START+7)

/* Report in HQPDU.d.radio_status_array */
#define HQVARID_RADIO_STATUS_ARRAY	(HQVARID_SPECIAL_START+8)

/* Report in HQPDU.d.protocol_error. */
#define	HQVARID_PROTOCOL_ERROR		(HQVARID_SPECIAL_START+9)

/* Red-M Change Starts */
/* A handoff trigger event: report in HQPDU.d.bert. */
#define	HQVARID_BER_TRIGGER	        (HQVARID_SPECIAL_START+10)
/* Red-M Change Ends */

/* Report in HQPDU.d.drain */
#define HQVARID_DRAIN_CALIBRATE         (HQVARID_SPECIAL_START+11)

/* Sniff calculations via host rather than LC */
#define HQVARID_SNIFF_REQ               (HQVARID_SPECIAL_START+12)

/* Sniff parameters received from remore link. Host must check them */
#define HQVARID_ACCEPT_TDI              (HQVARID_SPECIAL_START+13)

/* ESCO parameters to be sent to host so dsco can be chosed */
#define HQVARID_ESCO_REQ                (HQVARID_SPECIAL_START+14)

/* FM event */
#define HQVARID_FM_EVENT                (HQVARID_SPECIAL_START+15)

/* FM RDS DATA event (Bluecore Push) */
#define HQVARID_FM_RDS_DATA             (HQVARID_SPECIAL_START+16)

/* ADC result */
#define HQVARID_ADC_RESULT              (HQVARID_SPECIAL_START+17)

/* GPS result */
#define HQVARID_GPS_STATUS              (HQVARID_SPECIAL_START+18)

/* FMTX event */
#define HQVARID_FMTX_EVENT              (HQVARID_SPECIAL_START+19)

/* eGPS pulse result */
#define HQVARID_EGPS_TIME_STAMP         (HQVARID_SPECIAL_START+20)

/* RDF data send : dead, see B-85901 */
/*#define HQVARID_RDF_ADV_PACKET          (HQVARID_SPECIAL_START+21)*/

/* RDF info send : dead, see B-85901 */
/*#define HQVARID_RDF_INFO_SEND           (HQVARID_SPECIAL_START+22)*/

/* SCO stream handles */
#define HQVARID_SCO_STREAM_HANDLES      (HQVARID_SPECIAL_START+23)

/* coex uart info send */
#define HQVARID_COEX_UART_MESSAGE        (HQVARID_SPECIAL_START+24)

/* HQs for Stibbons (DSPmanager).
 * It's not yet known exactly what commands there will be, so for
 * now a block of 16 values are reserved and will be filled in as
 * the code is integrated back to mainline. When the work is
 * finished the remaining values will be relinquished.
 */

/* Baton debugging messages. */
#define HQVARID_DSPMANAGER_DEBUG_INFO_B  (HQVARID_SPECIAL_START+25)

/* Unsolicited message from an operator. */
#define HQVARID_MESSAGE_FROM_OPERATOR   (HQVARID_SPECIAL_START+26)

/* Downloading a capability failed. */
#define HQVARID_CAPABILITY_DOWNLOAD_FAILED   (HQVARID_SPECIAL_START+27)

#define HQVARID_STIBBONS_S04 (HQVARID_SPECIAL_START+28)
#define HQVARID_STIBBONS_S05 (HQVARID_SPECIAL_START+29)
#define HQVARID_STIBBONS_S06 (HQVARID_SPECIAL_START+30)
#define HQVARID_STIBBONS_S07 (HQVARID_SPECIAL_START+31)
#define HQVARID_STIBBONS_S08 (HQVARID_SPECIAL_START+32)
#define HQVARID_STIBBONS_S09 (HQVARID_SPECIAL_START+33)
#define HQVARID_STIBBONS_S10 (HQVARID_SPECIAL_START+34)
#define HQVARID_STIBBONS_S11 (HQVARID_SPECIAL_START+35)
#define HQVARID_STIBBONS_S12 (HQVARID_SPECIAL_START+36)
#define HQVARID_STIBBONS_S13 (HQVARID_SPECIAL_START+37)
#define HQVARID_STIBBONS_S14 (HQVARID_SPECIAL_START+38)
#define HQVARID_STIBBONS_S15 (HQVARID_SPECIAL_START+39)
#define HQVARID_STIBBONS_S16 (HQVARID_SPECIAL_START+40)


#endif	/* __HQPDU_H__ */
