/****************************************************************************
FILE
        bccmdradiotestpdu.h  -  bccmd radio test commands 

DESCRIPTION
	James has defined a set of radio test commands in document <bother -
	no document number>.  OK - it's a draft memo, 15th Mar called "RF
	test facilities offered by BC01 on-chip code".

	This describes a set of commands in the style of a set of function
	descriptions.  All of the functions are void, all take a set of
	arguments, all place the chip's radio in a test mode and all of the
	test modes are exited by rebooting the chip.

	The commands map to the pdus defined in this file.  These work
	exactly as the existing "special" pdus defined in bccmdpdu.h - the
	wire carries a standard pdu (BCCMDPDU) which holds one of the
	structures described below.  Each command is invoked by writing to
	BCCMDVARID_RADIOTEST, i.e. the invoking pdu is of type
	BCCMDPDU_SETREQ.

	Example Transactions
	--------------------

	1) To run the "txstart()" command.

	  host->bccmd BCCMDPDU_SETREQ

	    uint16 field 		value 
	    1	   BCCMDPDU.type	2      (BCCMDPDU_SETREQ)
	    2	   BCCMDPDU.pdulen	9      (number of uint16s in PDU)
	    3	   BCCMDPDU.seqno	1235   (value chosen by host)
	    4	   BCCMDPDU.varid	20484  (BCCMDVARID_RADIOTEST)
	    5	   BCCMDPDU.status 	0      (BCCMDPDU_STAT_OK)
	    6      BCCMDPDU.d.test      1      (RADIOTEST_TXSTART)
	    7	   BCCMDPDU.d.radiotest.txsi.lo_freq  0x1234  (lo_freq)
	    8	   BCCMDPDU.d.radiotest.txsi.level    0x1234  (level)
	    9	   BCCMDPDU.d.radiotest.txsi.mod_freq 0x1234  (mod_freq)

	  bccmd->host BCCMDPDU_GETRESP

	    uint16 field 		value 
	    1	   BCCMDPDU.type	1      (BCCMDPDU_GETRESP)
	    2	   BCCMDPDU.pdulen	9      (number of uint16s in PDU)
	    3	   BCCMDPDU.seqno	1235
	    4	   BCCMDPDU.varid	20484  (BCCMDVARID_RADIOTEST)
	    5	   BCCMDPDU.status 	0      (BCCMDPDU_STAT_OK)
	    6      BCCMDPDU.d.test      1      (RADIOTEST_TXSTART)
	    7	   BCCMDPDU.d.radiotest.txsi.lo_freq  0x1234  (lo_freq)
	    8	   BCCMDPDU.d.radiotest.txsi.level    0x1234  (level)
	    9	   BCCMDPDU.d.radiotest.txsi.mod_freq 0x1234  (mod_freq)

MODIFICATION HISTORY
	1.1  20:apr:01	ajh	Moved from dev/src/csr/bccmd.  This reset
				the version number to 1.1, hence the "OLD"
				section below.
	1.2  26:apr:01	cjo	Replace C++ comments as they break lint.
	1.3  06:jun:01  pws     Add RADIOTEST_SETTLE_RPT.
	1.4  02:oct:01  pws     Added RADIOTEST_RADIO_STATUS_ARRAY.
	1.5  06:nov:01  pws     Add RADIOTEST_CFG_TX_RF_TRIM (not yet written).
	1.6  10:dec:01  pws     Now using RADIOTEST_CFG_TX_RF_TRIM.
	1.7  07:jan:02  pws     Added RADIOTEST_PCM_TONE.
	1.8  09:jan:02  pws     hci-13 #168: add CTS/RTS and PCM CLK/SYNC tests
	1.9  01:nov:02  pws     H17.29: Placeholder RADIOTEST_DEEP_SLEEP_SELECT
	1.10 20:jan:02  ajh     Added comments to support parsing by perl.
	1.11 12:sep:03  pws     B-983: Note removal of radiotest
				commands (left definitions here).
	#12  12:dec:03  pws     B-1564: Add RADIOTEST_CFG_HOPPING_SEQUENCE.
	#14  18:may:04  pws     B-1717: Add RADIOTEST_CFG_PIO_*_MASK.
	#16  01:jun:04  pws     B-2918: EDR radiotest improvements.
	#17  20:dec:04  sv01    B-3971: Added RADIOTEST_CODEC_STEREO_LB.  
	#18  02:aug:05  jn01    B-3091: Added sentinel value RADIOTEST_END.

OLD MODIFICATION HISTORY
	1.1  10:apr:00	cjo	Created.
	1.3  3:may:00	cjo	Corrected example in comments.
        1.4  17:may:00  pws     Corrected example in comments.
	1.5  19:may:00  pws     mod_freq is a signed integer.
	1.6  11:jul:00  pws     New radiotest functions and their arguments.
	1.7  19:jul:00  pws     radiotest_config_tx_if infrastructure.
	1.8  25:jul:00  pws     Note problem with 32-bit arg to config_bit_err.
	1.10 24:aug:00  pws     Added pcm_ext_loop_back, config_xtal_ftrim.
	1.11 29:aug:00  pws     Added config_uap_lap.
	1.12 13:sep:00  pws     Added config_access_errs.
	1.13 18:sep:00  pws     Added config_iq_trim.
	1.14  2:nov:00  pws     Added config_am_addr.
	1.15 14:nov:00  cjo     Added the RCS "Revision" keyword to support
				BCCMDVARID_RADIOTEST_INTERFACE_VERSION.
	1.16 21:nov:00  pws     Renamed config_am_addr to config_tx_trim.
	1.17 28:nov:00  pws     Added RADIOTEST_PAUSE.
	1.18 15:dec:00  pws     Added RADIOTEST_RADIO_REPORT.
	1.19 19:dec:00  pws     Added RADIOTEST_LAST_COMMAND.
	1.20 22:jan:01  pws     Added RADIOTEST_CFG_LO_LVL.
	1.21 12:feb:01  pws     Added RADIOTEST_CFG_TX_COMP.
	1.22 07:mar:01  pws     Added RADIOTEST_CFG_SETTLE.
	#19  06:feb:06  pw04    B-11912: Added RADIOTEST_CFG_TX_PA_ATTEN.
        #20  08:jun:07  ls02    B-19858: Added new sentinel value
        #21  08:jun:07  ls02    B-19858: Cleared up comment
        ------------------------------------------------------------------
        --- This modification history is now closed. Do not update it. ---
        ------------------------------------------------------------------

*/

#ifndef	__BCCMDRADIOTESTPDU_H__
#define	__BCCMDRADIOTESTPDU_H__
 
#ifdef	RCS_STRINGS
static  char    bccmdradiotestpdu_h_id[]
  = "$Id: //depot/bc/bluesuite_2_4/interface/host/bccmd/bccmdradiotestpdu.h#1 $";
#endif

/* #include	"bt.h" */


/* The version number of the bccmdradiotest interface, as defined by the RCS
version number of the file that defines the interface (bccmdradiotestpdu.h).
This will change when commands are added/changed/removed.  This will not
change if the implementation of a command changes, e.g. a bug fix.

Value 0 means the information is unavailable.

The code that reports the version of this file needs this RCS keyword:
$Revision: #1 $ */


/* Turn the transmitter using local oscillator frequency "lo_freq", transmit
power "level" and modulation frequency "mod_freq". */

typedef struct {
	uint16		lo_freq;	/* Local oscillator frequency. */
	uint16		level;		/* Transmit level. */
	int16		mod_freq;	/* Modulation frequency. */
	} RADIOTEST_TXSTARTINFO;


/* Enable the receiver using local oscillator frequency "lo_freq" with at
attenuation level "attn".  If "highside" is FALSE use low side modulation,
else use high side modulation.  The final IF is routed to test pin TEST_A. */

typedef struct {
	uint16		lo_freq;	/* Local oscillator frequency. */
	bool		highside;	/* High or low side modulation. */
	uint16		attn;		/* Receiver attenuation. */
	} RADIOTEST_RXSTART1INFO;


/* Enable the receiver using local oscillator frequency "lo_freq" with at
attenuation level "attn".  If "highside" is FALSE use low side modulation,
else use high side modulation.  RSSI values are regularly sent back to the
host over a private channel. */

typedef struct {
	uint16		lo_freq;	/* Local oscillator frequency. */
	bool		highside;	/* High or low side modulation. */
	uint16		attn;		/* Receiver attenuation. */
	} RADIOTEST_RXSTART2INFO;


/* Enable the transmitter using local oscillator frequency "lo_freq" and
transmit power "level".  Use a PSBS9 payload over 5 slot packets at a
rate defaulting to 1 every 12500 microseconds, configurable via
radiotest_config_freq. */

typedef struct {
	uint16		lo_freq;	/* Local oscillator frequency. */
	uint16		level;		/* Transmit level. */
	} RADIOTEST_TXDATA1INFO;


/* Enable the transmitter using the hop sequence for transmit power "level".
Use a PSBS9 payload over 5 slot packets at a rate defaulting to 1 every 
12500 microseconds, configurable via radiotest_config_freq. */

typedef struct {
	uint16		cc;		/* bdcountrycode */
	uint16		level;		/* Transmit level. */
	} RADIOTEST_TXDATA2INFO;


/* Enable the transmitter using local oscillator frequency "lo_freq" and
transmit power "level".  Payload is 101010101...  over 5 slot packets at a
rate defaulting to 1 every 12500 microseconds, configurable via
radiotest_config_freq. */

typedef struct {
	uint16		lo_freq;	/* Local oscillator frequency. */
	uint16		level;		/* Transmit level. */
	} RADIOTEST_TXDATA3INFO;


/* Enable the transmitter using local oscillator frequency "lo_freq" and
transmit power "level".  Payload is 1111000011110000...  over 5 slot packets
at a rate defaulting to 1 every 12500 microseconds, configurable via
radiotest_config_freq. */

typedef struct {
	uint16		lo_freq;	/* Local oscillator frequency. */
	uint16		level;		/* Transmit level. */
	} RADIOTEST_TXDATA4INFO;


/* Enable the receiver using local oscillator frequency "lo_freq" and
attenuation level "attn".  If "highside" is FALSE use low side modulation,
else use high side modulation.  Receive only DM5 packets (5 slot with 2/3
FEC).  The data packets are expected to be 50 bytes of data.  The data is
discarded.  Send regular reports to the host over a private channel of the
number of packets received and the number of payloads with correctable
errors. */

typedef struct {
	uint16		lo_freq;	/* Local oscillator frequency. */
	bool		highside;	/* High or low side modulation. */
	uint16		attn;		/* Receiver attenuation. */
	} RADIOTEST_RXDATA1INFO;


/* Enable the receiver using the hopping sequence attenuation level
"attn".  If "highside" is FALSE use low side modulation, else use high side
modulation.   Receive only DM5 packets (5 slot with 2/3 FEC).  The data
packets are expected to be 50 bytes of data.  The data is discarded.  Send
regular reports to the host over a private channel of the number of packets
received and the number of payloads with correctable errors. */

typedef struct {
	uint16		cc;		/* bdcountrycode. */
	bool		highside;	/* High or low side modulation. */
	uint16		attn;		/* Receiver attenuation. */
	} RADIOTEST_RXDATA2INFO;


/* lo_freq, highside, attn as for rxdata1:  used for radiotest_bit_err1. */

typedef struct {
	uint16		lo_freq;	/* local oscillator frequency */
	bool		highside;	/* High or low side modulation. */
	uint16		attn;		/* Receiver attenuation. */
	} RADIOTEST_BER1INFO;


/* cc, highside, attn as for rxdata2:  used for radiotest_bit_err2. */

typedef struct {
	uint16		cc;		/* country code */
	bool		highside;	/* High or low side modulation. */
	uint16		attn;		/* Receiver attenuation. */
	} RADIOTEST_BER2INFO;


/* Turn on hardware loopback for the PCM port. "pcm_mode" can be
   0  Slave in normal 4-wire configuration
   1  Master in normal 4-wire configuration
   2  Master in Alcatel-specific 2-wire configuration
If this means nothing to you, 1 is a safe bet.   
   There are two versions of this, the second version also contains the
   pcm_interface over which the test will be run. It is thought that at
   somepoint we will go to just using the second version.
*/ 

typedef struct {
	uint16		pcm_mode;
	} RADIOTEST_PCMINFO;

typedef struct {
	uint16		pcm_mode;
    uint16      pcm_interface;
	} RADIOTEST_PCMINFO_IF;

/* Turn on codec hardware for stereo loopback */
typedef struct {
        uint16          samp_rate;
        uint16          reroute_optn;
	} RADIOTEST_CODEC_STEREO_LOOP_BACKINFO;

/* Turn on the synthesiser for channel "chan1" (0 to 78 inclusive), wait
for it to stabilise, then make a sudden jump to channel "chan2" and return
a one-off set of voltages for the tuner sample over the next 200
microseconds. */

typedef struct {
	uint16		chan1;		/* Channel to start test from */
	uint16		chan2;		/* Channel to finish test on */
	} RADIOTEST_SETTLEINFO;


/* Take "n_samples" samples of the IF response via internal loopback
between frequency "lo_offset" and "hi_offset", and return the corresponding
RSSI values to calibrate the behaviour of the IF filter.  The
frequency units are such that 1MHz is 0x1000; the range is 0 to
3MHz with lo_offset < hi_offset.  The samples are regularly spaced, so
the final offset used may not be exactly "hi_offset". */

typedef struct {
	uint16		n_samples;	/* Number of RSSI samples */
	int16		lo_offset;	/* Start freq, 1MHz = 0x1000 */
	int16		hi_offset;	/* End freq, same units, max 3MHz */
	} RADIOTEST_RESPINFO;


/* Calibrate the trim lookup table for country code "cc".  Return the
complete resulting table of 79 (cc = 0) or 23 (cc = 1..3) elements. */

typedef struct {
	uint16		cc;		/* bdcountrycode. */
	} RADIOTEST_TRIMINFO;


/* Receive and retransmit packets at frequency "lo_freq".  Retransmission
uses internal tx level "level".  Highside and attenuation are set to zero. */

typedef struct {
	uint16		lo_freq;	/* Local oscillator frequency. */
	uint16		level;		/* Transmit level. */
	} RADIOTEST_LOOPBACKINFO;


/* Set configuration data for some frequencies and offsets used by
the transmitter and receiver test.  "txrx_freq" is the interval in
microseconds between transmissions and expected receptions. "lb_offs"
is the interval in microseconds between receiving a packet and looping
it back.  "report_freq" is the interval in seconds (RADIOTEST_CFG_FREQ)
or microseconds (RADIOTEST_CFG_FREQ_MS) between reports sent to the host
for the rxdata and bit_err report types; it does not apply to the rxstart2
RSSI reports.  If any of the three is passed as zero, the current value
is not altered. */

typedef struct {
	uint16		txrx_freq;	/* How often to tx/rx, microsecs */
	uint16		lb_offs;	/* Offset for loopback, microsecs */
	uint16		report_freq;	/* How often to report to host, secs */
	} RADIOTEST_CFGFREQINFO;


/* Set the type and payload size for packets transmitted by the txdata tests.
"pkt_type" is a standard Bluetooth packet type, 0..11,14,15, although
only types with a data or voice payload should be used.  "pkt_size" is
the length of the data, excluding any FEC or CRC.  An invalid value
causes an error. */

typedef struct {
	uint16		pkt_type;	/* Type of packet to transmit */
	uint16		pkt_size;	/* Data size of packet to transmit */
	} RADIOTEST_CFGPKTINFO;


/* Set the bits count target for the bit_err tests.  "bits_count" gives
the total number of bits to receive before the counter are reset to zero;
If "reset" is non-zero, the counters are immediately reset.  Neither
affect the other set of counters which report the values only since the
previous report. 

IMPLEMENTATION NOTE:
The 32-bit argument here is in XAP (high word, low word) ordering.  As
the interface for the BCCMDVARID_RADIOTEST request assumes all arguments
are 16-bit words, the calling function on the host side is responsible
for dividing this number into two words, high word first.  Thus this
function effectively has three arguments: (bits_count / 65536,
bits_count % 65536, reset).
*/

typedef struct {
	uint32		bits_count;	/* Target of total counters */
	bool		reset;		/* Reset the total counters now? */
	} RADIOTEST_CFGBERINFO;

/* Configure the IF offset used in the transmitter tests.  "offset_half_mhz"
gives the value of tx_if_half_mhz to set as the intermediate frequency
transmitter offset in units of 0.5MHz.  The range is -5 to 5 inclusive.
This is for people in white coats. */

typedef struct {
	int16		offset_half_mhz;	/* Transmitter IF offset */
	} RADIOTEST_CFGTXIFINFO;

/* Configure the crystal trim value which controls, er, the trimming of
the crystal.  This determines all timing information for bc01.  The value
passed here takes effect immediately and typical survives until the
next reset (`typically', because re-initialising the synthesiser outside
the radiotest module will reset it; however this is unlikely to happen).
It does not affect the standard value which is kept in persistent store. */

typedef struct {
	uint16		xtal_ftrim;
	} RADIOTEST_CFGXTALFTRIM;

/* Configure the UAP and LAP used in test transmissions.  "uap" is
the UAP and "lap" the LAP to use; NAP is not necessary.  LAP will be in XAP
order, i.e. the upper word will be first in the uint32.
Effectively this is three words: UAP, upper-16-bits-of-lap,
lower-16-bits-of-lap. */

typedef struct {
	uint16		uap;
	uint32		lap;
	} RADIOTEST_CFGUAPLAP;

/* When testing for a transmission, the receiver uses a sliding correlator
to determine a match.  This allows up to "n_errs" errors before the match
is rejected.  This value can be configured for the radiotest modes only
by setting "n_errs" to the appropriate value, 0 to 15 inclusive.  The value
will be reset to the default (currently 10) at the next reset.
*/

typedef struct {
	uint16		n_errs;
	} RADIOTEST_CFGACCERRS;

/* The IQ block can be trimmed by a register ANA_IQ_TRIM.  This sets a
temporary value for the register, which takes effect immediately and
lasts until reset.  This is only useful if you know the behaviour of the
register; consult a hardware engineer.
*/

typedef struct {
	uint16		trim;
	} RADIOTEST_CFGIQTRIM;

/* Set the active member address used in the header of test transmissions,
and exepcted in receiving test transmissions.  It is a number between 0
and 7, inclusive.  The default value is 7.  This takes effect immediately and
lasts until reset. */

typedef struct {
	uint16		am_addr;
	} RADIOTEST_CFGTXTRIM;

/* The register ANA_LO_LVL sets a value for the output of the local
oscillator.  This sets a temporary value for that register, which takes
effect immediately and lasts until reset.  The values allowed are 0 to 15. */

typedef struct {
	uint16		lvl;
	} RADIOTEST_CFGLOLVL;

/* Values passed as "tx_offset" and "lo_offset" values to hal_update_tx_lvl,
for use in configuring the algorithm which sets a maximum tx power level.
This is only useful if you understand how hal_update_tx_lvl works.
*/

typedef struct {
	uint16		tx_offset;
	uint16		lo_offset;
	} RADIOTEST_CFGTXCOMP;

/* radio_on_offset is the time in microseconds before a transmission or
reception at which the radio is turned on by the hardware.  Supplying this
overrides PSKEY_RADIOTEST_RADIO_ON_OFFSET.
*/

typedef struct {
	uint16		radio_on_offset;
	} RADIOTEST_CFGSETTLE;

/* Set the trims used to minimise RF feedback in the transmitter.  The default
values of each trim are 0.  This takes effect immediately and lasts until 
a new test is started. */

typedef struct {
        int16           trim1;
        int16           trim2;
        } RADIOTEST_CFGTXRFTRIM;

/* Play a constant tone on the PCM port, or for bc02 with
PSKEY_HOSTIO_MAP_SCO_CODEC set, the codec.  The frequency is
(250 Hz << freq).  The amplitude is maximum for ampl == 8, half that for
ampl == 7, and so on; ampl 0 is valid.  The value dc specifies a constant
offset to add to the audio data. Interface defines which digital audio 
interface the tone is played out through. On chips with only one digital 
audio inteface this parameter is ignored. 
 
We have two versions as it would be nice to get rid of the version without 
the interface, however we are going to have both until the point where we can 
get rid of the none interface version. 
*/

typedef struct {
	uint16		freq;
	uint16		ampl;
	uint16		dc;
	} RADIOTEST_PCMTONE;

typedef struct {
	uint16		params[4];
	} RADIOTEST_PCMTONE_IF;


/* Test loopback using external wire from PIO line pio_out (0 to 7)
to PCM input pcm_in: 0 is PCM_SYNC and 1 is PCM_CLK.  Return TRUE if
wiggling pio_out causes the same transitions on pcm_in, FALSE otherwise 
 
The IF version also takes the interface to test. It is planned that the non 
interface version will disapear with time. 
*/ 

typedef struct {
	uint16		pio_out;
	uint16		pcm_in;
	} RADIOTEST_PCMTIMINGIN;

typedef struct {
	uint16		pio_out;
	uint16		pcm_in;
    uint16      pcm_interface;
	} RADIOTEST_PCMTIMINGIN_IF;

/* Command the chip to go into deep sleep.  This version provides masks
in which bits can be set to indicate that a certain line will not wake
up the chip, even if it would in the standard deep sleep configuration.
An absence of that bit does not, however, indicate that the
corresponding line is necessarily configured as a wakeup source.
It is harmless to set a bit if the line is not configured as a wakeup source.
"pio_mask" indicates PIO lines which will not wake up the chip.
"serial_mask" indicates certain serial port lines which will not
wake up the chip if the bit is set:
  bit 0:  UART rx
  bit 1:  UART cts
  bit 2:  USB resume

This feature is currently only present in special builds of the
firmware and is not available in standard releases.
*/

typedef struct {
	uint16		pio_mask;
	uint16		serial_mask;
	} RADIOTEST_DEEPSLEEPSELECT;

/* Provide the channel map for radiotest transmitter and receiver
tests which perform simplified hopping.  Each bit should be 1 to
indicate a channel is used or 0 to indicate it is not used.
Channel 0 correspond to bit 0 of word 0.  Channel 78 corresponds
to bit 14 of word 4.
*/

typedef struct {
	uint16		channels[5];
	} RADIOTEST_CFGHOPPING;

/* Provide the set of channels for which transmissions are to be
indicated on PIOs set by RADIOTEST_CFGPIOENABLEMASK.  Each bit should be 1 to
indicate a channel causes the PIOs to be high or 0 to indicate the
PIOs are to be kept low.  Channel 0 correspond to bit 0 of word 0.
Channel 78 corresponds to bit 14 of word 4.
*/

typedef struct {
	uint16		channels[5];
	} RADIOTEST_CFGPIOCHANNELMASK;

/* A mask of PIOs which will be used to indicate transmissions on
channels configured by RADIOTEST_CFGPIOCHANNELMASK.  For each bit
that is high, the corresponding PIO will be raised when a channel
set to 1 in RADIOTEST_CFGPIOCHANNELMASK is used for a transmission.
*/

typedef struct {
	uint16		mask;
	} RADIOTEST_CFGPIOENABLEMASK;

/* Set the TX PA atten value used on transmit. This really belongs with the 
 * individual power settings for the TX tests, but the interface for those 
 * is already published.
 */
typedef struct {
        uint16          atten_value;
        } RADIOTEST_CFGTXPATTEN;

/* Specify the power for transmit in db.  This is to be matched against the
 * power table to find the relevant value. 
 */

typedef struct {
        int16           db;
        } RADIOTEST_CFGTXPOWERINFO;


/* Play a constant tone on the stereo codec. Requires PSKEY_HOSTIO_MAP_SCO_CODEC
set to TRUE.  The frequency is (250 Hz << freq).  The amplitude is maximum for
ampl == 8, half that for ampl == 7, and so on; ampl 0 is valid.  The value dc
specifies a constant offset to add to the audio data.
params[0] --> Frequency
params[1] --> Amplitude
params[2] --> DC
params[3] --> Channel (0: Both, 1: Left, 2: Right)
*/
typedef struct {
	uint16		params[4];
	} RADIOTEST_PCMTONESTEREO;

/***************************************************************************/

typedef struct {
	uint16		test;		/* Test mode to enter. */

/* Acceptable values for "test". */
#define	RADIOTEST_PAUSE		(0)
#define	RADIOTEST_TXSTART	(1)
#define	RADIOTEST_RXSTART1	(2)
#define	RADIOTEST_RXSTART2	(3)
#define	RADIOTEST_TXDATA1	(4)
#define	RADIOTEST_TXDATA2	(5)
#define	RADIOTEST_TXDATA3	(6)
#define	RADIOTEST_TXDATA4	(7)
#define	RADIOTEST_RXDATA1	(8)
#define	RADIOTEST_RXDATA2	(9)

/* Built-in self tests */

#define RADIOTEST_DEEP_SLEEP	(10)
#define RADIOTEST_PCM_LB	(11)
#define RADIOTEST_SETTLE	(12) /* Removed from version 18 */
#define RADIOTEST_IF_RESP	(13) /* Removed from version 18 */
#define RADIOTEST_VCOTRIM	(14) /* Removed from version 18 */
#define RADIOTEST_RF_IQ_MATCH	(15) /* Removed from version 18 */
#define RADIOTEST_IF_IQ_MATCH	(16) /* Removed from version 18 */
#define RADIOTEST_BUILD_LUT	(17)
#define RADIOTEST_READ_LUT	(18)

/* Bit error rate, configuration and on-air loopback functions */

#define	RADIOTEST_BER1		(19)
#define RADIOTEST_BER2		(20)
#define RADIOTEST_LOOP_BACK	(21)
#define RADIOTEST_CFG_FREQ	(22)
#define RADIOTEST_CFG_PKT	(23)
#define RADIOTEST_CFG_BIT_ERR	(24)
#define RADIOTEST_RX_LOOP_BACK  (25)
#define RADIOTEST_BER_LOOP_BACK (26)
#define RADIOTEST_CONFIG_TX_IF  (27)

/* Other things that got added as an afterthought.  Expect this to grow. */
#define RADIOTEST_PCM_EXT_LB	(28)
#define RADIOTEST_CFG_XTAL_FTRIM  (29)
#define RADIOTEST_CFG_UAP_LAP	(30)
#define RADIOTEST_CFG_ACC_ERRS	(31) /* Removed from version 18 */
#define RADIOTEST_CFG_IQ_TRIM   (32)
#define RADIOTEST_CFG_TX_TRIM   (33) /* Missing from 18 but back in 19. */
#define RADIOTEST_RADIO_STATUS	(34)
#define RADIOTEST_CFG_LO_LVL	(35)
#define RADIOTEST_CFG_TX_COMP	(36)
#define RADIOTEST_CFG_SETTLE	(37) /* Removed from version 18 */
#define RADIOTEST_SETTLE_RPT	(38)
/* RADIOTEST_RADIO_STATUS_ARRAY calls the same command as 
 * RADIOTEST_RADIO_STATUS, but with a different argument to indicate
 * a more flexible format for returning values. */
#define RADIOTEST_RADIO_STATUS_ARRAY (39)
#define RADIOTEST_CFG_TX_RF_TRIM (40)
#define RADIOTEST_PCM_TONE	(41)
#define RADIOTEST_CTS_RTS_LB	(42)
#define RADIOTEST_PCM_TIMING_IN (43)
#define RADIOTEST_DEEP_SLEEP_SELECT (44)
#define RADIOTEST_CFG_HOPPING_SEQUENCE (45)
#define RADIOTEST_CFG_PIO_CHANNEL_MASK (46)
#define RADIOTEST_CFG_PIO_ENABLE_MASK (47)
#define RADIOTEST_CODEC_STEREO_LB (48)
#define RADIOTEST_CFG_TX_PA_ATTEN (49)
#define RADIOTEST_CFG_FREQ_MS	(50) 
#define RADIOTEST_PCM_TONE_STEREO	(51) 
#define	RADIOTEST_CFG_TXPOWER	(52)

#define RADIOTEST_PCM_LB_INTERFACE  (53)
#define RADIOTEST_PCM_TONE_INTERFACE (54)
#define RADIOTEST_PCM_EXT_LB_INTERFACE (55)
#define RADIOTEST_PCM_TIMING_IN_INTERFACE (56)


/* Not used as a command, just marks the last valid number. */ 
#define RADIOTEST_LAST_COMMAND	(56)

/* The following are not used as commands, just as sentinels in the
 * rt_fns[] table.  RADIOTEST_COMPLETE_LIST has replaced RADIOTEST_END to
 * indicate that the rt_fns[] table is a complete list of radiotest functions
 * Hopefully RADIOTEST_END will never again appear in new firmware... */
#define RADIOTEST_COMPLETE_LIST     (0xFFEu)
#define RADIOTEST_END           (0xFFFu)        /* Legacy sentinel */

	/* Each command has its own set of parameters. */
	union {
		RADIOTEST_TXSTARTINFO txsi;   /* test == RADIOTEST_TXSTART */
		RADIOTEST_RXSTART1INFO rxs1;  /* test == RADIOTEST_RXSTART1 */
		RADIOTEST_RXSTART2INFO rxs2;  /* test == RADIOTEST_RXSTART2 */
		RADIOTEST_TXDATA1INFO txd1;   /* test == RADIOTEST_TXDATA1 */
		RADIOTEST_TXDATA2INFO txd2;   /* test == RADIOTEST_TXDATA2 */
		RADIOTEST_TXDATA3INFO txd3;   /* test == RADIOTEST_TXDATA3 */
		RADIOTEST_TXDATA4INFO txd4;   /* test == RADIOTEST_TXDATA4 */
		RADIOTEST_RXDATA1INFO rxd1;   /* test == RADIOTEST_RXDATA1 */
		RADIOTEST_RXDATA2INFO rxd2;   /* test == RADIOTEST_RXDATA2 */
		RADIOTEST_BER1INFO ber1;      /* test == RADIOTEST_BER1 */
		RADIOTEST_BER2INFO ber2;      /* test == RADIOTEST_BER2 */
		RADIOTEST_PCMINFO pcm;	      /* test == RADIOTEST_PCM_LB
					         and RADIOTEST_PCM_EXT_LB */
		RADIOTEST_PCMINFO_IF pcm_if;	      /* test == RADIOTEST_PCM_LB_IF
					         and RADIOTEST_PCM_EXT_LB_IF
                                                 and RADIOTEST_PCM_LB_INTERFACE
                                                 and RADIOTEST_PCM_EXT_LB_INTERFACE */
	        RADIOTEST_SETTLEINFO settle;  /* test == RADIOTEST_SETTLE
					       * and RADIOTEST_SETTLE_RPT */
		RADIOTEST_RESPINFO resp;      /* test == RADIOTEST_IF_RESP */
		RADIOTEST_TRIMINFO trim;      /* test == RADIOTEST_VCOTRIM */
		RADIOTEST_LOOPBACKINFO lb;    /* test == RADIOTEST_LOOP_BACK 
						 and RADIOTEST_RX_LOOP_BACK 
						 and RADIOTEST_BER_LOOP_BACK */
		RADIOTEST_CFGFREQINFO cfreq;  /* RADIOTEST_CFG_FREQ
                                                 and RADIOTEST_CFG_FREQ_MS */
		RADIOTEST_CFGPKTINFO cpkt;    /* RADIOTEST_CFG_PKT */
	        RADIOTEST_CFGBERINFO cber;    /* RADIOTEST_CFG_BIT_ERR */
		RADIOTEST_CFGTXIFINFO ctxif;  /* RADIOTEST_CONFIG_TX_IF */
		RADIOTEST_CFGXTALFTRIM cxtft; /* RADIOTEST_CFG_XTAL_FTRIM */
	        RADIOTEST_CFGUAPLAP cuaplap;  /* RADIOTEST_CFG_UAP_LAP */
		RADIOTEST_CFGACCERRS cfgaccerrs; /* RADIOTEST_CFG_ACC_ERRS */
		RADIOTEST_CFGIQTRIM cfgiqtrim; /* RADIOTEST_CFG_IQ_TRIM */
		RADIOTEST_CFGTXTRIM cfgtxtrim; /* RADIOTEST_CFG_TX_TRIM */
	        RADIOTEST_CFGLOLVL cfglolvl;    /* RADIOTEST_CFG_LO_LVL */
		RADIOTEST_CFGTXCOMP cfgtxcomp;	/* RADIOTEST_CFG_TX_COMP */
		RADIOTEST_CFGSETTLE cfgsettle;  /* RADIOTEST_CFG_SETTLE */
                RADIOTEST_CFGTXRFTRIM cfgtxrftrim; /* RADIOTEST_CFG_TX_RF_TRIM */
        RADIOTEST_PCMTONE pcmtone; /* RADIOTEST_PCM_TONE */
	        RADIOTEST_PCMTIMINGIN pcmtimingin;
					/* RADIOTEST_PCM_TIMING_IN */
            RADIOTEST_PCMTIMINGIN_IF pcmtimingin_if;
                    /* RADIOTEST_PCM_TIMING_IN_IF
                    and RADIOTEST_PCM_TIMING_IN_INTERFACE */
		RADIOTEST_DEEPSLEEPSELECT deepsleepselect;
					/* RADIOTEST_DEEP_SLEEP_SELECT */
#if 0
		/*
		 * We deliberately do not include this since it
		 * can confuse the host code by making the PDU two
		 * bytes longer.  Instead, we check the length
		 * internally.
		 */
		RADIOTEST_CFGHOPPING cfghopping;
					/* RADIOTEST_CFG_HOPPING_SEQUENCE */
		RADIOTEST_CFGPIOCHANNELMASK cfgchannelmask;
					/* RADIOTEST_CFG_PIO_CHANNEL_MASK */
		RADIOTEST_PCMTONESTEREO pcmtonestereo; 
                                        /* RADIOTEST_PCM_TONE_STEREO */
        RADIOTEST_PCMTONE_IF pcmtone_if; /* RADIOTEST_PCM_TONE_INTERFACE */
#endif
		RADIOTEST_CFGPIOENABLEMASK cfgpioenablemask;
					/* RADIOTEST_CFG_PIO_ENABLE_MASK */
		RADIOTEST_CODEC_STEREO_LOOP_BACKINFO codec_stereo_loop_back;
	                        /* RADIOTEST_CODEC_STEREO_LB */
                RADIOTEST_CFGTXPATTEN   cfgtxpaatten; /* RADIOTEST_CFG_TX_PA_ATTEN */
                RADIOTEST_CFGTXPOWERINFO   txpwri;    /* RADIOTEST_CFG_TXPOWER */

		/* The following have no arguments and hence no corresponding
		 * PDU format:
		 * RADIOTEST_PAUSE
		 * RADIOTEST_DEEP_SLEEP
		 * RADIOTEST_RADIO_STATUS
		 * RADIOTEST_RADIO_STATUS_ARRAY
		 * RADIOTEST_CTS_RTS_LB
		 * RADIOTEST_RF_IQ_MATCH
		 * RADIOTEST_IF_IQ_MATCH
		 * RADIOTEST_BUILD_LUT
		 * RADIOTEST_READ_LUT
		 */
		} d;
	} BCCMDPDU_RADIOTEST;


#endif	/* __BCCMDRADIOTESTPDU_H__ */
