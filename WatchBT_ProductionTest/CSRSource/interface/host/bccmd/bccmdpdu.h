/****************************************************************************
FILE
        bccmdpdu.h  -  bluecore command interpreter packet structure

DESCRIPTION
	There are two bidirectional "private channels" that carry messages
	between the host and the host controller (aka the bc01 chip) over the
	BCSP (or USB) link:

	 1) One carries commands from a control entity on the host (possibly
	    called bcmgr) to the bccmd task on the host.  The bccmd task is a
	    command interpreter, carrying out local actions in response to
	    the host's commands.  The channel's reverse path carries command
	    responses back to the control entity on the host.

	    This channel will allow the host to configure, control and
	    monitor the CSR/bluecore Bluetooth implementation.

	 2) The second channel carries commands from the hq (host querier)
	    task on the host controller to a command interpreter on the
	    host.  The reverse channel carries back the host's command
	    responses.

	    This channel is expected to find only very limited use.  At
	    present, the only messages are from fault() and delayed_panic().

	This file describes the protocol pdus carried on the first channel.
	The second channel's protocol is described in hqpdu.h.

	In addition to the host<->host_controller path, it is anticipated
	that one or more alternative means of passing commands into the bccmd
	will be provided, e.g., from the chip's SPI interface.  It is
	expected that the same protocol/pdus will be used on all interfaces
	to bccmd, with command responses being returned to the corresponding
	command sources.

	It is also anticipated that the set of commands held in the pdus may
	have different behaviours depending on the interface on which the
	commands are submitted.  Notably, for security reasons, some commands
	will be rejected if they reach bccmd via the BCSP channel.

	As these pdus are to pass through the BCSP, they are structured to
	fit CCL's "bluecore friendly format", i.e. the pdus' wire structures
	will match the xap's memory structure.  The lower byte of each uint16
	travels over the wire first (confirmed by jrrk).  Thus, the pdus can
	be defined as C structures on the understanding that the wire format
	will be a (byte) serialisation of the way in which the types will be
	laid out by the xap C compiler.

	To allow a possible future optimisation in the way in which hostio
	copies pdus to/from BUFFERS, all pdus defined here are an even number
	of bytes.  (This is also implied by use of the "bluecore friendly
	format".)

	The protocol is influenced by snmp.  The host (client) views the
	services presented by the bccmd (server) as a set of values to be
	read and/or written.  There are only two types of transaction:

	    To read a value, the host sends a GET-REQUEST pdu to the bccmd.
	    The bccmd reads the value and returns it in a GET-RESPONSE pdu to
	    the host.

	    To set a value, the host sends a SET-REQUEST pdu to the bccmd.
	    The bccmd sets the value and returns the new value in a
	    GET-RESPONSE pdu to the host.

	There are only three basic types of PDU:

	    GET-REQUEST - (called BCCMDPDU_GETREQ below) asks for the value
	        of a variable to be returned to the host.

	    GET-RESPONSE - (called BCCMDPDU_GETRESP below) returns the value
	        of a variable to the host.

	    SET-REQUEST - (called BCCMDPDU_SETREQ below) asks to set the
	        value of a variable.

	The protocol may be extended to provide a GET-NEXT-REQUEST in the
	future.  This would work in a manner similar to snmp.  There is no
	current need for this functionality.

	The host commands the bccmd Bluetooth chip to perform an action by
	writing to a particular variable.  For example, the chip might be
	rebooted by writing to a variable called "reboot".  Abstracting the
	bccmd's functionality as reading and writing variables promotes a
	simple protocol.

	Alas, not all of the protocol's transactions fit this simple
	read/write pattern cleanly - some variables are special in that they
	need extra parameters, e.g., accessing blocks of RAM.

	No attempt is made to support snmp's SMI - the way in which snmp
	names variables.  This protocol uses a single uint16 to identify each
	variable.

	Every GET-REQUEST and SET-REQUEST received by the bccmd causes it to
	(attempt to) return one GET-RESPONSE.  However, in exceptional
	circumstances, e.g. provoking a reboot, the host cannot guarantee to
	receive the GET_RESPONSE.  Thus, the host must use a timeout.

	The GET-REQUEST PDU is of fixed minimum size; it consists of an
	instance of the typedef BCCMDPDU, described below.  This effectively
	asks "return me the value of the variable with identifier ID".  Even
	though this PDU carries no data to the server it still provides
	data storage sized to allow the server's response.  This peculiarity
	is purely to simplify implementation.

	The two other PDU types consist of an instance of BCCMDPDU,
	optionally followed by extra data:

	 1) The SET_REQUEST BCCMDPDU effectively says "set the value of the
	    variable with identifier ID to be ..."  The PDU carries new value
	    in a union at the end of the BCCMDPDU.

	    However, some variables don't have values, notably "variables"
	    which cause an action when written, e.g., reboot.

	 2) The BCCMDPDU part of the GET_RESPONSE PDU effectively says
	    "the value of the variable with identifier ID is ...".  The
	    PDU carries the item's value in the union at the end of the
	    BCCMDPDU.

	    However, if the data cannot be returned to the host then the data
	    part of the PDU is still returned - it is just not initialised by
	    the server.  The status code in the header indicates the cause of
	    the caller's disappointment.

	Most variables' values fit within the union at the end of the
	BCCMDPDU struct.  This is wasteful in that the largest data struct
	sets the size of all BCCMDPDUs, but it is simple to work with, and
	this traffic is rare, so we don't care too much about wasting
	bandwidth or RAM.

	However, a handful of data types have variable sized data - values
	for RAM, ps and buffers.  Here life becomes complicated.  The union at the
	end of the BCCMDPDU has a (small) fixed size array for carrying these
	data, but it will be acceptable for the array to be bigger than
	advertised.  The actual size of the data can be determined by the
	pdulen field by subtracting BCCMDPDU_OVERHEAD_LEN.

	Example Transactions
	--------------------

	1) To read the value of variable BCCMDVARID_BC01_STATUS

	  host->bccmd BCCMDPDU_GETREQ

	    uint16 field 		value
	    1	   BCCMDPDU.type	0      (BCCMDPDU_GETREQ)
	    2	   BCCMDPDU.pdulen	9      (number of uint16s in PDU)
	    3	   BCCMDPDU.seqno	1234   (value chosen by host)
	    4	   BCCMDPDU.varid	10241  (BCCMDVARID_BC01_STATUS)
	    5	   BCCMDPDU.status 	0      (BCCMDPDU_STAT_OK)
	    6	   BCCMDPDU.d.u16 	0      (empty)
	    7-9	   ignored              0      (zero padding)

	  bccmd->host BCCMDPDU_GETRESP

	    uint16 field 		value
	    1	   BCCMDPDU.type	1      (BCCMDPDU_GETRESP)
	    2	   BCCMDPDU.pdulen	9      (number of uint16s in PDU)
	    3	   BCCMDPDU.seqno	1234
	    4	   BCCMDPDU.varid	10241  (BCCMDVARID_BC01_STATUS)
	    5	   BCCMDPDU.status 	0      (BCCMDPDU_STAT_OK)
	    6	   BCCMDPDU.d.u16 	0x8642 (value of bc01_status)
	    7-9	   ignored              0      (zero padding)

	2) To set the value of variable BCCMDVARID_CONFIG_UART to the
	   value 0x1234:

	  host->bccmd BCCMDPDU_SETREQ

	    uint16 field 		value
	    1	   BCCMDPDU.type	2      (BCCMDPDU_SETREQ)
	    2	   BCCMDPDU.pdulen	9      (number of uint16s in PDU)
	    3	   BCCMDPDU.seqno	1235   (value chosen by host)
	    4	   BCCMDPDU.varid	26626  (BCCMDVARID_CONFIG_UART)
	    5	   BCCMDPDU.status 	0      (BCCMDPDU_STAT_OK)
	    6	   BCCMDPDU.d.u16 	0x1234 (data)
	    7-9	   ignored              0      (zero padding)

	  bccmd->host BCCMDPDU_GETRESP

	    uint16 field 		value
	    1	   BCCMDPDU.type	1      (BCCMDPDU_GETRESP)
	    2	   BCCMDPDU.pdulen	9      (number of uint16s in PDU)
	    3	   BCCMDPDU.seqno	1235
	    4	   BCCMDPDU.varid	26626  (BCCMDVARID_CONFIG_UART)
	    5	   BCCMDPDU.status 	0      (BCCMDPDU_STAT_OK)
	    6	   BCCMDPDU.d.u16 	0x1234 (data)
	    7-9	   ignored              0      (zero padding)

	3) Failing to set the value of non-existent uint8 variable
	   BCCMDVARID_UTTERCRUD to the value 0x23:

	  host->bccmd BCCMDPDU_SETREQ

	    uint16 field 		value
	    1	   BCCMDPDU.type	2      (BCCMDPDU_SETREQ)
	    2	   BCCMDPDU.pdulen	9      (number of uint16s in PDU)
	    3	   BCCMDPDU.seqno	1236   (value chosen by host)
	    4	   BCCMDPDU.varid	9066   (BCCMDVARID_UTTERCRUD)
	    5	   BCCMDPDU.status 	0      (BCCMDPDU_STAT_OK)
	    6	   BCCMDPDU.d.u16 	0x0023 (dummy data)
	    7-9	   ignored              0      (zero padding)

	  bccmd->host BCCMDPDU_GETRESP

	    uint16 field 		value
	    1	   BCCMDPDU.type	1      (BCCMDPDU_GETRESP)
	    2	   BCCMDPDU.pdulen	9      (number of uint16s in PDU)
	    3	   BCCMDPDU.seqno	1236
	    4	   BCCMDPDU.varid	9066   (BCCMDVARID_UTTERCRUD)
	    5	   BCCMDPDU.status 	1      (BCCMDPDU_STAT_NO_SUCH_VARID)
	    6	   BCCMDPDU.d.u16 	0x0000 (dummy data - no match needed.)
	    7-9	   ignored              0      (zero padding)

	4) To read the 3 words of memory from data address 0xb374.

	  host->bccmd BCCMDPDU_GETREQ

	    uint16 field 		value
	    1	   BCCMDPDU.type	0      (BCCMDPDU_GETREQ)
	    2	   BCCMDPDU.pdulen	11     (number of uint16s in PDU)
	    3	   BCCMDPDU.seqno	1237   (value chosen by host)
	    4	   BCCMDPDU.varid	4097   (BCCMDVARID_MEMORY)
	    5	   BCCMDPDU.status 	0      (BCCMDPDU_STAT_OK)
	    6	   BCCMDPDU.d.mem.base_addr 0xb374  (memory address)
	    7	   BCCMDPDU.d.mem.len   3      (n words to be read)
	    8	   BCCMDPDU.d.mem.dummy 0      (padding)
	    9	   BCCMDPDU.d.mem.mem[0] 0     (dummy data)
	    10	   BCCMDPDU.d.mem.mem[1] 0     (dummy data)
	    11	   BCCMDPDU.d.mem.mem[2] 0     (dummy data)

	  bccmd->host BCCMDPDU_GETRESP

	    uint16 field 		value
	    1	   BCCMDPDU.type	1      (BCCMDPDU_GETRESP)
	    2	   BCCMDPDU.pdulen	11     (number of uint16s in PDU)
	    3	   BCCMDPDU.seqno	1237   (value chosen by host)
	    4	   BCCMDPDU.varid	4097   (BCCMDVARID_MEMORY)
	    5	   BCCMDPDU.status 	0      (BCCMDPDU_STAT_OK)
	    6	   BCCMDPDU.d.mem.base_addr 0xb374
	    7	   BCCMDPDU.d.mem.len   3      (n words read)
	    8	   BCCMDPDU.d.mem.dummy 0      (padding)
	    9	   BCCMDPDU.d.mem.mem[0] 0x1357 (contents of 0xb374)
	    10	   BCCMDPDU.d.mem.mem[1] 0x9ace (contents of 0xb375)
	    11	   BCCMDPDU.d.mem.mem[2] 0x2468 (contents of 0xb376)

	5) To set the value of the persistent store variable PSKEY_BDADDR,
	   the local device's Bluetooth address, to {lap=0x123456,
	   uap=0x78, nap=0x9abc}.  The address is held in the ps as type
	   bdaddr, which is typedefed to be {uint32 lap; uint8 uap;
	   uint16 nap;}, so two bytes are wasted in memory - one because only
	   24 bits of the uint32 are used, the other because of the XAP
	   treating the uint8 as a uint16.

	  host->bccmd BCCMDPDU_SETREQ

	    uint16 field 		value
	    1	   BCCMDPDU.type	2      (BCCMDPDU_SETREQ)
	    2	   BCCMDPDU.pdulen	12     (number of uint16s in PDU)
	    3	   BCCMDPDU.seqno	1238   (value chosen by host)
	    4	   BCCMDPDU.varid	0x7003 (BCCMDVARID_PS)
	    5	   BCCMDPDU.status 	0      (BCCMDPDU_STAT_OK)
	    6	   BCCMDPDU.d.ps.id     1      (PSKEY_BDADDR)
	    7	   BCCMDPDU.d.ps.len    4      (sizeof(bdaddr))
	    8	   BCCMDPDU.d.ps.stores 0      (PS_STORES_DEFAULT)
	    9	   BCCMDPDU.d.ps.psmem[0] 0x0056  (lap - high word of uint32)
	    10	   BCCMDPDU.d.ps.psmem[1] 0x1234  (lap - low word of uint32)
	    11	   BCCMDPDU.d.ps.psmem[2] 0x0078  (uap - in low byte of word)
	    12	   BCCMDPDU.d.ps.psmem[3] 0x9abc  (nap)

	  <Delay, while ps value is written.>

	  bccmd->host BCCMDPDU_GETRESP

	    uint16 field 		value
	    1	   BCCMDPDU.type	1      (BCCMDPDU_GETRESP)
	    2	   BCCMDPDU.pdulen	12     (number of uint16s in PDU)
	    3	   BCCMDPDU.seqno	1238   (value chosen by host)
	    4	   BCCMDPDU.varid	0x7003 (BCCMDVARID_PS)
	    5	   BCCMDPDU.status 	0      (BCCMDPDU_STAT_OK)
	    6	   BCCMDPDU.d.ps.id     1      (PSKEY_BDADDR)
	    7	   BCCMDPDU.d.ps.len    4      (sizeof(bdaddr))
	    8	   BCCMDPDU.d.ps.stores 0      (PS_STORES_DEFAULT)
	    9	   BCCMDPDU.d.ps.psmem[0] 0x0056  (lap - high word of uint32)
	    10	   BCCMDPDU.d.ps.psmem[1] 0x1234  (lap - low word of uint32)
	    11	   BCCMDPDU.d.ps.psmem[2] 0x0078  (uap - in low byte of word)
	    12	   BCCMDPDU.d.ps.psmem[3] 0x9abc  (nap)

ADDENDUM
	This file is also used by the host tools.  Part of that
	process is parsing by devTools/bin/pdu/bccmd_parse.pl.
	This tool reads all the VARIDs, and special structures, and
	outputs a list of PDU's understood by the host tools.
	This list contains:
	 * all PDUs with VARIDs < BCCMDVARID_SPECIAL_START
	 * all PDUs with VARIDs >= BCCMDVARID_SPECIAL_START, with
	   struct defined in this file matching names.
           BCCMDVARID_HYPOTHETICAL matches BCCMDPDU_HYPOTHETICAL.


REFERENCES
	bc01-s-024   -  bccmd protocol.
	bc01-s-026   -  bccmd basic command set.
	bc01-s-027   -  bccmd basic test commands.
	bc01-s-028   -  bccmd security.
	bc01-sp-040  -  bccmd ps commands.
	bc01-sp-045  -  bccmd radiotest commands.
	bc01-sp-047  -  spi bccmd protocol.

MODIFICATION HISTORY
	1.1   20:apr:01   ajh	Moved file from src/csr/bccmd to
				interface/host/bccmd, and sets the version
				number back to 1.1, hence the "OLD" section
				below.
	1.2   25:apr:01   cjo	Corrected comment.
	1.3   25:apr:01   cjo	Added support for commands
				BCCMDVARID_CRYPTKEYLEN and
				BCCMDVARID_PICONET_INSTANT.
				Replaced bloody C++ comment with C style
				as former breaks lint.
	1.4   18:may:01   cjo	Added support for commands
				BCCMDVARID_DEFRAG_RESET.
	1.5   8:jun:01    cjo   Added support for commands BCCMDVARID_RAND
				and BCCMDVARID_DEFAULT_TX_POWER.
	1.6   11:jun:01   cjo	Comments.
	1.7   11:jun:01   cjo	Comments.
	1.8   13:jun:01   cjo	Comments.
	1.9   29:jun:01   cjo	Extended BCCMDVARID_PS and BCCMDVARID_PSSIZE
				to allow stores to be specified.
	1.10  14:sep:01   cjo	Added BCCMDVARID_GET_CLR_EVT.
	1.11  14:sep:01   cjo	Changed BCCMDVARID_TESTA_AMUX and
				BCCMDVARID_TESTB_AMUX from read-only to
				read-write.  This changes the varids.
	1.12  20:sep:01   cjo	Added BCCMDVARID_MAXCRYPTKEYLEN.
	1.13  03:oct:01   pws   Add BCCMDVARID_DISABLE_VM.
	1.14  03:oct:01   cjo   Renamed BCCMDVARID_DISABLE_VM to be
				BCCMDVARID_KILL_VM_APPLICATION.
	1.15  03:oct:01   pws   Really changed it.
	1.16  07:dec:01   cjo   Added BCCMDVARID_GET_NEXT_BUILDDEF.
	1.17  17:jan:02   at    Added PS_STORES_T*, BCCMDVARID_PSCLRS and
				BCCMDVARID_PSCLRALLS to support psram.
	1.18  29:jan:02   pws   hci-15 #10: BCCMDVARID_SINGLE_CHAN,
	                        BCCMDVARID_HOPPING_ON.
	1.19  25:feb:02   pws   H15.47: add BCCMDVARID_CANCEL_PAGE.
	1.20  08:apr:02   pws   H15.106: add BCCMDVARID_CHECK_ROM,
				BCCMDVARID_CHECK_RAM,
				BCCMDVARID_CHECK_RAM_RESULT.
	1.21  26:jun:02   pws   H16.12: add BCCMDVARID_PCM_GAIN (uint16, r/w).
	1.22  26:jun:02   pws   H16.12: Changed to *_ATTENUATION.
	1.23  26:jun:02   pws   H16.12: Fix comment.
	1.24  01:jul:02   at    H16.16: Add I2C/EEPROM access commands.
	1.25  27:jul:02   at    H16.16: Removed i2c field from union.
	1.26  27:jul:02   at    H16.16: Renumbered BCCMDVARID_E2_APP_SIZE
				to avoid clash with BCCMDVARID_PCM_ATTENUATION.
	1.27  01:aug:02   cjo   H16.58: Add varids BCCMDVARID_STACK_CHECK and
				BCCMDVARID_STACK_CHECK_CFM to support
				rom/flash integrity check.
	1.28  09:aug:02   pws   H16.66: Add BCCMDVARID_BYPASS_UART placeholder.
	1.29  09:aug:02   pws   H16.67: Add BCCMDVARID_CHIPANAVER.
	1.30  29:aug:02   pws   T.579: Added BCCMDVARID_SYNC_CLOCK,
	                        BCCMDVARID_ANA_FTRIM.
	1.31  18:sep:02   at    H16.130: Added BCCMDVARID_BUILDID_LOADER.
	1.34  08:nov:02   at    Updated BCCMDVARID_STACK_CECK* description.
	1.35  08:nov:02   cjo   H17.46: Changed BCCMD command names.
				STACK_CHECK becomes FIRMWARE_CHECK, and
				STACK_CHECK_CFM becomes FIRMWARE_CHECK_MASK.
	1.36  08:nov:02   cjo   H17.46: Undo screwed up change control
				comments.
	1.37  13:nov:02   at    H17.52: Added BCCMDVARID_ENABLE_AFH.
	1.38  21:nov:02   pws   H17.71: BCCMDVARID_BER_THRESHOLD placeholder.
	1.39  06:dec:02   pws   H17.98: Add BCCMDVARID_PIO_STRONG_BIAS.
				Also correct comments on PIO register width.
	1.40  24:mar:03   cjo   Following advice from Simon Harrison,
				corrected "* 64" comments for the Red-M call
				handoff command.
	1.41  20:may:03   npm   B-528 - bcccmd access to bootmode
	1.42  06:jun:03   at    B-598: Add BCCMDVARID_PS_MEMORY_TYPE.
	1.43  03:sep:03   cjo   B-941: Commented that varids for RECAL,
				INIT_BT_STACK, ACTIVATE_BT_STACK, PSCLR,
				PSCLRALL, BC01_STATUS and CANCEL_PAGE are
				redundant.
	#44   18:sep:03   cjo   B-1031: Add BCCMDVARID_PSNEXT_ALL.
	#45   23:sep:03   pws   B-1055: scatternet/SCO data workaround.
	#46   01:oct:03   mm	B-1135: Added L2CAP CRC code.
	#47   20:nov:03   sms   B-1423: Reserve PDUs for medium rate testing.
	#48   08:jan:04   tg    B-1497: Added BCCMDVARID_KALDEBUG.
	#49   22:jan:04   doc   B-1809: No change to this file.
	#50   26:jan:04   tg    B-1497: Commented BCCMDVARID_KALDEBUG.
	#51   26:jan:04   sms   B-1799: Add medium rate debugging PDUs.
	#53   01:mar:04   pws   B-1716: Remote limits for sniff.
	#54   01:mar:04   ckl   B-1943: Babel / Casirack BCCMD VARIDs.
	#58   20:apr:04   pws   B-2411: Add BCCMDVARID_ANA_FTRIM_READWRITE.
	#61   20:may:04   pws   B-2820: Yet more scatternet/SCO.
	#62   29:jun:04   pws   B-3175: Add BCCMDVARID_SNIFF_MULTISLOT_ENABLE
	#63   30:jun:04   pws   B-3201: Add BCCMDVARID_DEEP_SLEEP_PERIOD
        #64   13:jul:04   pw04  B-3200: BCCMD to get RSSI.
	#65   21:jul:04   sk03  B-3349: Added eSCO test code.
	#66   28:jul:04   pws   B-3479: BCCMDVARID_CHECK_ROM_CRC32.
	#68   19:aug:04   sk03  B-3349: Added eSCO test code to drop eSCO frames.
	#69   15:sep:04   pws   B-3976: Extend !sniff_multislot_enable.
	#70   23:sep:04   pws   B-4028: Add BCCMDVARID_LIMIT_EDR_POWER.
	#71   27:sep:04   pws   B-4028: Remove bogus integration diff.
	#72   12:oct:04   ay02  B-4216: Add BCCMDVARID_LC_RX_FRAC_COUNT
	                                 and BCCMDVARID_LC_RX_FRACS to get PER
	#73   02:nov:04   ay02  B-4533: Add BCCMDVARID_ROUTE_CLOCK
        #74   25:nov:04   sk03  B-4672: Replace pause_user_data flag with
                                semaphore.
        #76   03:feb:05   ay02  B-4551: Add EXT_CLOCK_PERIOD
        #83   19:apr:05   ay02  B-6429: Add BCCMDVARID_HOST_DECIDES_TIMING
	#84   19:apr:05   sw04  B-6406: Add BCCMDVARID_HOSTIO_ENABLE_DEBUG
        #85   26:apr:05   sv01  B-5247: Add BCCMDPDU_ENABLE_ANTENNA_DIVERSITY
	#86   28:apr:05   sv01  B-5247: Add above bccmdpdu to union d
	#86   03:may:05   sv01  B-5247: Add BCCMDPDU_ANTENNA_SELECTION_PATTERN
	#87   03:may:05   sv01  B-5247: Correct definition above.
        #88   06:may:05   sw04  B-6877: Add BCCMDVARID_SAVE_CONNECTION_EVENT_FILTERS
        #91   17:may:05   rgb   B-5204: Made charger bccmds uint16 NOT special.
        #92   18:may:05   rgb   B-5204: Put bc3psu back in union for legacy support only.
        #95   24:jun:05   ay02  B-7382: Add support for PIO2 (pins 15-23)
        #96   01:jul:05   ay02  B-7822: HOST_DECIDES_TIMING was read-only
	#98   04:jul:05   sv01  B-7842: Add BCCMDVARID_SET_AD_HYSTERISIS_LIMIT.
	#99   04:jul:05   sv01  B-7842: Correct spelling of HYSTER*E*SIS.
	#100  04:jul:05   sv01  B-7842: Remove "set" from bccmd name.
	#101  04:jul:05   sv01  B-7842: Rename bccmd (again).
	#102  04:jul:05   sv01  B-7842: Correct typo.
	#103  07:jul:05   sv01  B-7842: Add pdu definition for hysteresis limit
	#104  08:jul:05   sv01  B-7842: Comment fix.
        #106  12:jul:05   sv01  B-5247: Integrate addition of bccmdpdu to union
        #108  12:jul:05   ajh   B-8012: Add comments about host tools.
	#109  12:jul:05   sv01  B-7842: Add more antenna bccmdpdus to union d
	#111  18:jul:05   sv01  B-7842: Integrate change from antenna-diversity
	#112  19:jul:05   sv01  B-7842: Comment fix
	#113  21:jul:05   pw04  B-5641: Grab the varid for sniff sub-rating.
	#116  25:jul:05   sv01  B-8282: Add BCCMDs antenna_diversity_{rssi,ber}
	#117  25:jul:05   sv01  B-8282: Fix typo in comment.
	#118  20:jul:05   ay02  B-8196: Add BCCMDVARID_TCD_LATENCY_TEST
	#119  27:jul:05   ay02  B-8196: Modify BCCMDVARID_TCD_LATENCY_TEST
	#122  28:jul:05   sk03  CommentChange: Remove C++ style comments.
	#123  28:jul:05   sk03  Remove accidentally-added #include.
	#124  03:aug:05   sv01  B-8424: Add #define for antenna array size
	...
	#128  15:aug:05   sv01  B-8602: Add BCCMDVARID_BROADCAST_ANTENNA
	#129  15:aug:05   sv01  B-8602: Move sniff-sr pdu out of antenna ifndef
	#130  19:aug:05   sv01  B-8602: Rename broadcast to default antenna
	#131  01:sep:05   ay02  B-9019: Add BCCMDVARID_GET_PS_FREE_COUNT
	#132  28:sep:05   ay02  B-9229: Add BCCMDVARID_SUPPRESS_TCD_DATA
	...
        #136  30:jan:06   ay02  B-11538: Implement FM API over BCCMD
        #137  08:feb:06   jn01  B-11809: Rename FM_RDS_DATA to FM_RDS per spec
        #138  08:feb:06   jn01  B-11809: FM_RDS: rename more & correct type
        #139  02:mar:06   sk03  B-12413: Integrate BC5 support onto main.
        #140  03:mar:06   ajh   B-12413: Remove "#ifdef INSTALL_FM"'s.
        #141  10:mar:06   sk03  B-12349: Implement FM audio routing BCCMD.
        #142  10:apr:06   sk03  B-13365: Add dynamic configuration of PCM.
        #143  11:apr:06   sw04  CommentChange: Tidy comments
        #144  11:apr:06   sk03  B-13365: Add BCCMD to change PCM_CONFIGn.
        #145  21:jul:06   sk03  B-15564: Rename i2s_if_* bccmds.
        #146  09:aug:06   ld05  B-15882: Acl away time, external timeout
        #147  15:aug:06   ab12  B-9449: Added bccmds for sha256 testing
        #148
        #149
        #150  06:sep:06   sr06  B-16644: Added bccmd for low_sco_priority
        #151  06:sep:06   sv01  B-14976: Added 3 BCCMDs for ADPCM config.
        #152  07:sep:06   sv01  B-14976: Oops, missed the #defines.
        #153  07:sep:06   sv01  B-14976: Wrap structs in #ifndef NO_ADPCM.
        #154  07:sep:06   sv01  B-16644: Add low_pri_sco struct & union member.
        #154  07:sep:06   sv01  B-14976: Rename BCCMD adpcm_local_enabled.
        #156  18:sep:06   sr06  B-16644: updated names for enable low pri sco
        #157  25:sep:06   sw04  B-15762: BCCMD to set inquiry priority
        #158  04:oct:06   ab12  B-17359: BCCMD to allow enable/disable of ACL
                                         acks.
        #159  18:oct:06   ab12  B-17359: Revert the change which added BCCMD to 
                                         allow enable/disable of ACL acks.
        #160  19:oct:06   rb01  B-17974: BCCMD to force charger trim set.
        #161  19:oct:06   rb01  B-17974: Don't delete / from comment start!
        #162  03:nov:06   sv01  B-14976: Rename esco_codec to voice_codec.
        #163  03:nov:06   sv01  B-14976: Rename BCCMD adpcm_bitrate.
        #164  07:nov:06   sv01  B-14976: Add voice_codec enum.
        #165  10:nov:06   sv01  B-14976: Move some ADPCM cmds to special start.
        #166  05:dec:06   sv01  B-19671: Remove voice_codec enum.
        #166  06:dec:06   jd07  B-18702: Add BCCMD_SET_PANIC_ON_FAULT
        #169  13:dec:06   sv01  B-20018: Re-number 2 varids back. 
        #170  19:dec:06   sv01  B-20247: Remove BCCMD adpcm_bits_per_sample.
        #172  21:dec:06   sv01  B-20314: Make VOICE_CODEC write only.
        #173  29:DEC:06   jdo7  B-18676: Add BCCMD_AUX_DAC,BCCMD_AUX_DAC_ENABLE
        #174  22:jan:07   ld05  B-20567: Added Mic bias BCCMD,PDU structure 
        #175  22:jan:07   ld05  B-20676: Harmonise bccmdpdu in mainline and 22 
        #176  22:jan:07   ld05  B-20675: More.
        #177  23:jan:07   ld05  B-20567: More Added Mic bias BCCMD,PDU structure
        #178  02:mar:07   ay02  B-20195: Add WLAN Coex prioritisation
        #179  25:apr:07   sw04  B-20657: Inquiry transmit power level

OLD MODIFICATION HISTORY

	1.1   29:nov:99	  cjo	Created.
	1.2   30:nov:99	  cjo	Corrected error in comment showing example
				ps transaction.
				Removed "vartype" and "pdulen" fields.
				Added memory block read example.
	1.3   30:nov:99	  cjo	Added extra varids, mostly for the adc.
	1.12  4:apr:00    cjo	BCCMDVARID_PS had the same value as another
				bccmdvarid.
	1.13  5:apr:00    cjo	Updated comments.
				Returned the pdulen field to BCCMDPDU_HDR.
				Added BCCMDVARID_RADIOTEST.
				Almost #included bccmdradiotestpdu.h.
	1.15  10:apr:00   cjo	Added serial number.
	1.16  11:apr:00   cjo	Converted to use BCCMDPDU.
				Removed BCCMDVARID_MODULE_ID.
	1.17  16:apr:00   cjo	Added BCCMDVARID_PSCLR.
	1.21  17:apr:00   cjo	Added BCCMDVARID_PSCLRMOST and
				BCCMDVARID_PSCLRALL.
				Overhauled read/write permissions.
	1.22  3:may:00    cjo	Corrected BCCMDVARID_BC01_STATUS example.
	1.23  3:may:00    cjo	Corrected more examples.
	1.24  3:may:00    cjo	Added BCCMDVARID_BUILDID,
				BCCMDVARID_CHIPVER and BCCMDVARID_CHIPREV.
	1.25  4:may:00    cjo	BCCMDVARID_CONFIG_UART to read/write.
	1.26  5:may:00    cjo	Added BCCMDVARID_MAP_SCO_PCM and
				BCCMDVARID_SPI_BUFSIZE.
	1.27  6:may:00    cjo	Added BCCMDVARID_COLD_HALT and
				BCCMDVARID_WARM_HALT.
				Added BCCMDPDU_STAT_NO_ACCESS.
	1.29  22:may:00   cjo	Added padding uint16 to BCCMDPDU_MEMORY
				and BCCMDPDU_PS to even up sizes of
				commands that use extensible arrays.
	1.30  22:may:00   cjo   Corrected ps and mem examples in comments.
	1.31  23:may:00   cjo   Improved commenting of the RECAL command.
	1.32  30:may:00   cjo   Added pio port access commands.
	1.33  31:may:00   cjo   Added BCCMDVARID_BT_CLOCK.
	1.35  6:jun:00    cjo   Corrected comments.
	1.36  12:jul:00   cjo   Added calls to provoke (test) calls to
				panic(), delayed_panic() and fault().
	1.37  19:jul:00   cjo   Changed BCCMDVARID_PIO from RO to RW.
	1.38  21:jul:00   cjo   BCCMDVARID_RECAL from WO ot RW.
	1.39  21:jul:00   cjo   Changed BCCMDVARID_RECAL back again.
	1.42  3:aug:00    cjo   Added BCCMDVARID_PIO_PROTECT_MASK.
	1.43  9:aug:00    cjo   Added 57k6 baud.
	1.45  21:sep:00   cjo   BCCMDVARID_PSSIZE.
	1.47  25:sep:00   cjo   Corrected value of UART_BAUD_19200.
	1.48  25:sep:00   cjo   Removed obsolete #define BCCMDPDU_HDR.
	1.52  16:oct:00   cjo   Corrected comment.
	1.53  27:oct:00   pws   Comment out PSCLRMOST differently so as not
				not to confuse PSCLRMOST.
	1.54  31:oct:00   cjo   Corrected comments.
	1.59  29:nov:00   cjo   Added BCCMDVARID_MAX_TX_POWER.
	1.60  23:jan:01   cjo   BCCMDVARID_PSNEXT.
	1.62  29:jan:01   cjo   Moved BCCMDVARID_PSSIZE BCCMDVARID_PSNEXT
				to the specials block.
	1.62  5:mar:01    cjo   Changed interface to adc.
        ------------------------------------------------------------------
        --- This modification history is now closed. Do not update it. ---
        ------------------------------------------------------------------
*/

#ifndef	__BCCMDPDU_H__
#define	__BCCMDPDU_H__

#ifdef RCS_STRINGS
static const char bccmdpdu_h_id[]
  = "$Id: //depot/bc/bluesuite_2_4/interface/host/bccmd/bccmdpdu.h#3 $";
#endif

/****************************************************************************
BCCMDPDU_MEMORY
===============

Data used to control a read from, or write to, the bc01's memory space.

The bccmd treats the value of "base_addr" as a uint16*.  It specifies the
base address of the block to be written or read.

When used to specify a "write" operation or used to report on a "read"
operation, the "len" field sets the number of uint16s to be written or read.
This block of uint16s is held in mem[].

The mem[] array is declared as being of length 1, but the length is actually
arbitrary.  The actual length can be derived from the "pdulen" field by
subtracting BCCMDPDU_OVERHEAD_LEN.  The limit implied by BCCMDPDU_MAXLEN must
be respected.  We can pull this nasty trick because the mem[] is the last
element of the BCCMDPDU struct.

The bccmd performs some basic filtering of the address range.  The machine
can't be hung by reading/writing arbitrary locations, but all sorts of
unpleasant effects can be provoked - e.g., buffer page exception interrupts,
mysterious flash memory effects, etc.   Actual restrictions are TBD. HERE */

typedef struct {
	uint16		base_addr;	/* Start address. */
	uint16		len;		/* Number of uint16s. */
	uint16		dummy;		/* Padding word - unused. */
	uint16		mem[1];		/* Data. */
	} BCCMDPDU_MEMORY;


/****************************************************************************
BCCMDPDU_PS
===========

Data used to control a read or write to the bc01's persistent store.

The "id" is the same identifier used in the psbc_data, where it identifies an
item of data.

When used to specify an item of data to be read, the "len" field specifies
the (maximum) length of the data to be returned.  If the bccmd is able to
determine that the data stored under the identifier "id" is larger than "len"
then a BCCMDPDU_STAT_TOO_BIG value is given in the reply's "status" field.
The "stores" field describes which of the four ps stores to examine and
the order in which to search them.

When used to specify an item of data to be written, the "len" field specifies
the data's length in uint16s.  The data is passed to the bccmd in the "len"
element array psmem[]. The "stores" field describes which of the three
writable ps stores to modify.

Comments for the mem[] array in BCCMDPDU_MEMORY also apply to psmem[].

Writing to the bc01's ps hangs the machine for the (significant) duration of
the operation.  This should avoided while the processor is supporting a
Bluetooth link.

The requirement that the data is passed as a number of uint16s presumes that
the ps interface handles data in uint16s.  If we ever use uint8-wide
persistent storage this is likely to be problematic. */

typedef struct {
	uint16		id;		/* PS identifier (pskey). */
	uint16		len;		/* Length of data. */

/* The "stores" parameter is a bitmask.  Only these entries work. */
#define	PS_STORES_DEFAULT (0x0000)	/* default stores for command. */
#define	PS_STORES_I       (0x0001)	/* implementation (normal psi.h). */
#define	PS_STORES_F       (0x0002)	/* factory-set (psf.h). */
#define	PS_STORES_R       (0x0004)	/* rom (psrom.h). */
#define	PS_STORES_T       (0x0008)	/* transient (psram.h). */
#define	PS_STORES_IF      (0x0003)	/* psi.h->psf.h. */
#define	PS_STORES_IFR     (0x0007)	/* psi.h->psf.h->psrom.h. */
#define	PS_STORES_TI      (0x0009)	/* psram.h->psi.h. */
#define	PS_STORES_TIF     (0x000b)	/* psram.h->psi.h->psf.h. */
#define	PS_STORES_TIFR    (0x000f)	/* psram.h->psi.h->psf.h->psrom.h. */

	uint16		stores;		/* PS store(s) for read only. */
	uint16		psmem[1];	/* Data. */
	} BCCMDPDU_PS;

/*
BCCMDPDU_PSNEXT
===============

Support for the BCCMDVARID_PSNEXT command. */

typedef struct {
	uint16		id;		/* PS identifier (pskey). */
	uint16		stores;		/* PS store(s) to examine. */
	uint16		nextid;		/* Next PS identifier (pskey). */
	} BCCMDPDU_PSNEXT;

/*
BCCMDPDU_PSNEXT_ALL
===================

Support for the BCCMDVARID_PSNEXT_ALL command. */

typedef struct {
	uint16		id;		/* PS identifier (pskey). */
	uint16		nextid;		/* Next PS identifier (pskey). */
	} BCCMDPDU_PSNEXT_ALL;


/*
BCCMDPDU_PSSIZE
===============

Support for the BCCMDVARID_PSSIZE command.  In the getreq the "id" specifies
the pskey to be examined and the "stores" describes the stores to examine
and the order in which to search them.  In the corresponding successful
getresp the "len" gives the length of data stored under "id" measured in
uint16s. */

typedef struct {
	uint16		id;		/* PS identifier (pskey). */
	uint16		len;		/* Length of data stored under id. */
	uint16		stores;		/* PS store(s) to examine. */
	} BCCMDPDU_PSSIZE;


/*
BCCMDPDU_PSCLRS
===============

Support for the BCCMDVARID_PSCLRS command. */

typedef struct {
	uint16		id;		/* PS identifier (pskey). */
	uint16		stores;		/* PS store(s) to examine. */
	} BCCMDPDU_PSCLRS;


/*
BCCMDPDU_PS_MEMORY_TYPE
=======================
Support for the BCCMDVARID_PS_MEMORY_TYPE command. */

typedef struct {
	uint16		stores;		/* PS store(s) to examine. */

/* The "memorytype" return value will be one of the following. */
#define PSMEMORY_FLASH    (0x0000)  /* Flash memory */
#define PSMEMORY_EEPROM   (0x0001)  /* EEPROM */
#define PSMEMORY_RAM      (0x0002)  /* RAM (transient) */
#define PSMEMORY_ROM      (0x0003)  /* ROM (or read-only flash memory) */

	uint16		memorytype;	/* The underlying memory type. */
	} BCCMDPDU_PS_MEMORY_TYPE;


/****************************************************************************
BCCMDPDU_BUFFER
===============

Data used to control a read or write to one of the bc01's buffers.

The bccmd treats the "handle" field as the bufhandle of the buffer to be
accessed.

The "start" field specifies the first byte (offset) in the buffer to be
accessed.

The "length" field specifies the number of bytes to be read or written.

Data to be read or written is held in the bufmem[] array.

The bufmem[] array is declared as being of length 1, but the length is
actually arbitrary.  The actual length can be derived from the "pdulen"
field by subtracting BCCMDPDU_OVERHEAD_LEN.  The limit implied by
BCCMDPDU_MAXLEN must be respected.  We can pull this nasty trick because the
bufmem[] is the last element of the BCCMDPDU struct.

bufmem[] is declared to be uint8[], so the upper byte of each location is
wasted.  These extra bytes are passed over the wire, but must be ignored.
I.e., the wire data is *not packed* - each buffer byte lies in the lower
byte of each of the "length" unint16s.

The "length" field is the actual size of the array bufmem[]. */

typedef struct {
	uint16		handle;		/* bufhandle of buffer */
	uint16		start;		/* First byte to be accessed. */
	uint16		len;		/* Number of bytes to be accessed. */
	uint8		bufmem[1];	/* Data. */
	} BCCMDPDU_BUFFER;


/****************************************************************************
BCCMDPDU_ADC
============

[Post B-33848, this command is obsolete; use ADC_READ instead]

Writing to BCCMDVARID_ADC provokes the firmware into attempting to read from
and ADC channel.  The result may subsequently be obtained by reading from
BCCMDVARID_ADCRES, which obtains a BCCMDPDU_ADCRES.

The result of reading the ADC is held in the "result" field.  The host should
accept this 8 bit value only if "valid" is TRUE (1) and if "channel" matches
the value written to BCCMDVARID_ADC.

The ADC reading will fail if the value written to BCCMDVARID_ADC does not
describe a valid ADC input, or if the processor is extremely heavily laden.

Under heavy load conditions the ADC can take several tens of milliseconds to
make the reading, hence the value of the "valid" flag.

Only one ADC read attempt should be attempted at a time; the "channel"
field helps guard against this. */

typedef struct {
	uint16		channel;	/* ADC channel read. */
	bool		valid;		/* Is "result" valid? */
	uint16		result;		/* Value read from ADC. */
	} BCCMDPDU_ADCRES;


/****************************************************************************
BCCMDPDU_CRYPTKEYLEN
====================

When encryption is applied to an ACL connection the LMs on the local and
remote machines negotiate the (effective) encryption key length.  This
structure supports a command that allows the host to discover the key length,
measured in bytes.

The getreq must hold the hci_handle, and keylen must be zero.

If the command is successful getresp holds the (same) hci_handle and the
effective key length negotiated for the connection.  This may not be
the link's current encryption strength: it is possible that encryption
was first negotiated and applied to the link (deriving the link's "keylen"
value) then encryption was removed - but the negotiated "keylen" value
will still reflect the earlier negotiation. */

typedef struct {
	uint16		hci_handle;	/* ACL/HCI handle. */
	uint16		keylen;		/* Effective key length, in bytes. */
	} BCCMDPDU_CRYPTKEYLEN;


/****************************************************************************
BCCMDPDU_PICONET_INSTANT
========================

The local device's Bluetooth clock is available via one of the uint32
commands.  However, if the local device is a slave on a connection then that
connection's master's Bluetooth clock can be of interest.  This structure
supports a command that obtains the Bluetooth clock that applies to an
established ACL.  If the local device is the master of the ACL then the value
is (much) the same as the value returned by the uint32 command.

The getreq sets the hci_handle, and sets h_instant to zero.

If the command is successful getresp holds the (same) hci_handle and
h_instant holds HALF! the value of the ACL link's master's Bluetooth clock.

(Reporting half the link's clock is mr01's choice; it seems Byzantine to me.)
*/

typedef struct {
	uint16		hci_handle;	/* ACL/HCI handle. */
	uint32		h_instant;	/* Timestamp. */
	} BCCMDPDU_PICONET_INSTANT;


/****************************************************************************
BCCMDPDU_GET_CLR_EVT
====================

The firmware includes a set of counters that are used to monitor certain
events.  Each time the event occurs a counter is incremented.

This varid obtains the value of the counter identified by "evtcntid" and
clears the counter to zero.

The VM also has access to these same event counters, so care must be taken if
the VM and host access a single counter - this is unlikely to be a
significant problem for real applications.

The monitoring mechanisms are not precise - it is possible to overcount or
undercount.  This is because the underlying code deliberately does not bother
to block interrupts when these counters are accessed so as to avoid blizzards
of block/unblock trains, which could upset the radio's timing.  Since this
function is likely to be used just to waggle LEDs it doesn't seem to be worth
fixing - more accurately, it seems to be worth not fixing.

Each counter is held in a uint16, so the values can wrap.

Reading and clearing the BCCMDPDU_GCE_NONE counter has no effect; it accesses
no counter and the cnt value returned is always zero.

The PIODEBOUNCE, USRADC, BCSTAT and DEEPSLEEP counters should not be read by
the host - if the host (reads and) clears these counters an element of the
firmware can break. 

nb: USRADC died in u23.

*/

typedef struct {
/* Event counter identifier and acceptable values. */
	uint16			   evtcntid;
#define	BCCMDPDU_GCE_NONE	   (0)   /* No counter. */
#define	BCCMDPDU_GCE_TXACL	   (1)   /* Tx acl event started. */
#define	BCCMDPDU_GCE_RXACL	   (2)   /* Rx acl event started. */
#define	BCCMDPDU_GCE_TXSCO	   (3)   /* Tx sco event started. */
#define	BCCMDPDU_GCE_RXSCO	   (4)   /* Rx sco event started. */
#define	BCCMDPDU_GCE_TXHOST	   (5)   /* Tx host event started. */
#define	BCCMDPDU_GCE_RXHOST	   (6)   /* Rx host event started. */
#define	BCCMDPDU_GCE_RADIOACTIVE   (7)   /* Any radio activity. */
#define	BCCMDPDU_GCE_PIODEBOUNCE   (8)   /* PIO debounce value change. */
#define	BCCMDPDU_GCE_USRADC	   (9)   /* ADC converstion finished. */
#define	BCCMDPDU_GCE_BCSTAT	   (10)  /* Chip status changed. */
#define	BCCMDPDU_GCE_DEEPSLEEP	   (11)  /* Chip woken from deep sleep. */

/* Number of events.  0 in getreq, counter value in getresp. */
	uint16			   cnt;
	} BCCMDPDU_GET_CLR_EVT;


/****************************************************************************
BCCMDPDU_GET_NEXT_BUILDDEF
==========================

The elements included by each firmware build is controlled by a set of
top-level #defines, passed to the build system.  The GET_NEXT_BUILDDEF
command allows the host to determine the list of these #defines, and hence
the characteristics/elements of the build.

The values of "id" and "nextid" are from builddefs.h, a header that can be
used by firmware and host code.  This file's entries directly map to the
build system's top-level #defines.

The command is build on the common "get next" mechanism.  The normal manner
of use is that the host first calls the command with "id" set to zero.  The
command returns the BCCMDPDU_GET_NEXT_BUILDDEF with the "nextid" set to the
first (numerically sorted) identifier.  The host records this identifer, then
places it into the "id" field of the next call to the command, which obtains
the second identifier.  This process is continued until "nextid" is returned
holding BUILDDEF_NONE. */

typedef struct {
	uint16			   id;
	uint16			   nextid;
	} BCCMDPDU_GET_NEXT_BUILDDEF;


/****************************************************************************
BCCMDPDU_I2C_TRANSFER
=====================

Data used to control a composite write/read transfer on the I2C-bus.

The "address" field specifies the 7bit slave address (in bits 7 to 1).

The "tx_octets" field specifies the number of bytes to be written.

The "rx_octets" field specifies the number of bytes to be read.

The "restart" field should normally be TRUE, but may be set to FALSE to
force a change of direction without an intervening repeated start
condition (Sr).

The "octets" field must be zero for the getreq/setreq, and is set to the
number of bytes transferred and acknowledged for the getresp. This includes
both the slave address byte(s) and the final byte read, even though the
latter is followed by a NACK.

The "data" array is used for both the data to be written and the data read.
It is declared as being of length 1, but the length is actually artibtrary.
The first "tx_octets" elements contain the bytes to be written, and the
following "rx_octets" elements are overwritten by the bytes read. The limit
imposed by BCCMDPDU_MAXLEN must be respected. "data" is declared to be
uint8[], so the upper byte of each location is wasted.

See the description of i2c_transfer() in i2c.h for a more comprehensive
explanation; the "data" array corresponds to the "tx_data" and "rx_data"
parameters, and the return value is written to "octets". */

typedef struct {
	uint16			   address;
	uint16			   tx_octets;
	uint16			   rx_octets;
	bool			   restart;
	uint16			   octets;
	uint8			   data[1];
	} BCCMDPDU_I2C_TRANSFER;


/****************************************************************************
BCCMDPDU_E2_DEVICE
==================

A header is stored at the start of I2C EEPROM devices used with BlueCore to
indicate the device characteristics. This is required to enable the correct
addressing scheme to be used.

The "log2_bytes" field specifies the size of the device as (2 ^ log2_bytes)
bytes.

The "addr_mask" field specifies which, if any, bits of the slave address
are used to extend the array address byte(s). Any of the three least
significant bits may be set to 1 to indicate that the corresponding slave
address bit is used.
*/

typedef struct {
	uint16			   log2_bytes;
	uint16			   addr_mask;
	} BCCMDPDU_E2_DEVICE;


/****************************************************************************
BCCMDPDU_E2_APP_DATA
=====================

Data used to control a read or write of part of a VM application stored
in EEPROM.

The "offset" field specifies the offset in words from the start of the
region reserved for a VM application at which to start reading or writing.

The "words" field specifies the maximum number of words to read or write.
In the getresp this indicates the actual number of words read or written,
which may be less than the number requested if the reserved size is
exceeded.

The "dummy" field is not currently used; it must be set to 0.

The "data" array is used for the data to be read or written. It is declared
as being or length 1, but the length is actually arbitrary. The actual
length is given by the "words" field. The limit imposed by BCCMDPDU_MAXLEN
must be respected.
*/

typedef struct {
	uint16			   offset;
	uint16			   words;
	uint16			   dummy;
	uint16			   data[1];
	} BCCMDPDU_E2_APP_DATA;

/****************************************************************************
BCCMDPDU_ENABLE_AFH
====================

Adaptive frequency hopping (AFH) is used by default if both devices
support it (i.e. their AFH_capable feature bit is set).  This varid allows
use of AFH to be explicitly enabled or disabled, on either a global or
per-link basis.

The setreq must hold the hci_handle specifying the link to control, and the
required state.  If the hci_handle is zero then this sets the state for new
connections, and for any active links where the local device is master.  If
the hci_handle is non-zero then it must specify an active link where the
local device is master.  Attempting to enable AFH for a link where one or
both devices does not support AFH, or for a link where the local device is
a slave, results in an error.

The getreq must hold the hci_handle, and enable must be zero.  If the
command is successful then the getresp holds the (same) hci_handle and
enable indicates the state of the specified link.  If hci_handle specifies
an active link where the local device is master then the current state for
that link is returned, otherwise the state used for new connections is
returned.

Note that having use of AFH enabled for a link or future connections does
not necessarily mean that AFH will be used.  The LM may decide to operate
a link with AFH disabled for other reasons, for example to avoid use of
multiple park beacon structures.
*/

typedef struct {
	uint16		hci_handle;	/* ACL/HCI handle, or 0. */
	bool		enable;		/* Disable (0) or enable (1). */
	} BCCMDPDU_ENABLE_AFH;

/****************************************************************************
BCCMDPDU_L2CAP_CRC
==================

This BCCMD is used to turn on/off the automatic generation of CRC's on
transmit and to turn on the checking of CRC's on reception.  */

typedef struct {
    uint16          hci_handle;     /* ACL/HCI handle. */
    bool            tx_cid;         /* TRUE if CID is a transmit channel */
    uint16          cid;            /* The CID */
    bool            enable;         /* TRUE to enable this CID. */
} BCCMDPDU_L2CAP_CRC;

/****************************************************************************
BCCMDPDU_ENABLE_MEDIUM_RATE
==========================

*/

typedef struct {
	uint16		hci_handle;	/* ACL/HCI handle */
	bool		enable;		/* Disable (0) or enable (1). */
	} BCCMDPDU_ENABLE_MEDIUM_RATE;



/****************************************************************************
BCCMDPDU_SUPPRESS_TCD_DATA
==========================
*/

typedef struct {
	uint16		acl_handle;	/* ACL handle */
	bool		enable;		/* Disable (0) or enable (1). */
	} BCCMDPDU_SUPPRESS_TCD_DATA;

/****************************************************************************
BCCMDPDU_RADIOTEST
==================

Complicated and growing, so held in a separate header file. */

#include	"bccmdradiotestpdu.h"

/****************************************************************************
BCCMDPDU_KALDEBUG
=================

Commands for debugging kalimba.
*/

#include "bccmdkaldebugpdu.h"

/****************************************************************************
BCCMDPDU_KALEXTADDRDEBUG
========================
(New) BCCMD support for debugging kalimba DSP (for BC5 and above) 
*/

#include "bccmdkalextaddrdebugpdu.h"
/****************************************************************************
BCCMDPDU_VM_DEBUG
================

Commands for doing on chip debugging of VM apps.

*/

#include "bccmdvmdebugpdu.h"


/***************************************************************************
BCCMDPDU_BC3PSU
===============

Old commands for testing charger hardware. Support for this has been removed
from the firmware. This functionality is now provided via seperate BCCMDs,
rather than multiplexed through a single BCCMD.

*/
#include "bccmdbc3psupdu.h"

/* Red-M Change Starts */
/****************************************************************************
BCCMDPDU_BER_THRESHOLD
======================

Setting a non-zero threshold means that when raw_ber hits 32*threshold, an
HQ packet is emitted to indicate a fall in signal quality.
These events are one-shot: the threshold is zeroed after triggering.
Only the lower 8 bits are used in the threshold. */

typedef struct {
	uint16		hci_handle;	/* ACL/HCI handle. */
	uint16		ber_threshold;	/* point at which ber trigger fires.
					   8 bits used, premultiplied by 32. */
	} BCCMDPDU_BER_THRESHOLD;

/* Red-M Change Ends */

/****************************************************************************
BCCMDPDU_BUILD_NAME
===================

Read the name of the firmware build, as defined in the src/csr/id/id.c file.

As the build name may be too long to fit into a single message packet, an
offset field is defined that specifies how far through the build name to
start reading from. The GETRESP message also contains a length field to give
the total length of the build name; the host uses a combination of this
field, the message length and the offset to determine when the end of the
build name is reached.

The "start" field specifies the first byte (offset) in the buffer to be
accessed.

The "length" field gives the length of the build name in bytes.

The build name (sub-section) is placed in the bufmem[] array; this is
declared as being of length 1, but the length is actually arbitrary. We can
pull this nasty trick because the bufmem[] is the last element of the
BCCMDPDU struct.

bufmem[] is declared to be uint8[], so the upper byte of each location is
wasted.  These extra bytes are passed over the wire, but must be ignored.
I.e., the wire data is *not packed* - each buffer byte lies in the lower
byte of each of the "length" unint16s.
*/

typedef struct {
	uint16		start;		/* First byte to be accessed. */
	uint16		len;		/* Returned length of build name string. */
	uint8		bufmem[1];	/* Data. */
	} BCCMDPDU_BUILD_NAME;

/****************************************************************************
BCCMDPDU_RSSI_ACL
======================

Read the RSSI value in DBM on a particular HCI ACL handle.
*/

typedef struct {
	uint16		hci_handle;	/* ACL/HCI handle. */
	int16		rssi;	        /* The RSSI value */
	} BCCMDPDU_RSSI_ACL;

/****************************************************************************
BCCMDPDU_AWAY_TIME
======================

        Read the away time using the given HCI ACL handle 
*/

typedef struct {
	uint16		hci_handle;	/* ACL/HCI handle. */
	uint32		time;	        /* last rx time */
	} BCCMDPDU_AWAY_TIME;


/****************************************************************************
BCCMDPDU_ESCO_TEST
==================

Interact with the eSCO test modes.
*/

typedef enum {
    BCCMDPDU_ESCOTEST_ACTIVE_MODES,     /* The selected test modes */
    BCCMDPDU_ESCOTEST_SCO_TX_ERR_RATE,  /* (e)SCO transmit error insertion rate */
    BCCMDPDU_ESCOTEST_SCO_RX_ERR_RATE,  /* (e)SCO receive error insertion rate */
    BCCMDPDU_ESCOTEST_SCO_RTX_ERR_RATE, /* (e)SCO retransmit error insertion rate */
    BCCMDPDU_ESCOTEST_SCO_RRX_ERR_RATE, /* Error insertion rate on receptions
                                           of retransmitted (e)SCO packets*/
    BCCMDPDU_ESCOTEST_SCO_TX_FRAME_RATE,/* Rate at which eSCO frames are dropped */
    BCCMDPDU_ESCOTEST_PAUSE_STATE       /* The current value of the pause_user_data semaphore (Read Only) */
} BCCMDPDU_ESCOTEST_FUNCTION;

typedef enum {
    BCCMDPDU_ESCOTEST_MODE_SCO_TXERR = 1,   /* Insert errors into (e)SCO transmissions */
    BCCMDPDU_ESCOTEST_MODE_SCO_RXERR = 2,   /* Insert errors into (e)SCO receptions */
    BCCMDPDU_ESCOTEST_MODE_SCO_RTXERR = 4,  /* Insert errors into (e)SCO retransmissions */
    BCCMDPDU_ESCOTEST_MODE_SCO_RRXERR = 8,  /* Insert errors into receptions of
                                               retransmitted (e)SCO packets */
    BCCMDPDU_ESCOTEST_MODE_SCO_TXFRAME = 16 /* Drop entire eSCO frames when transmitting */
} BCCMDPDU_ESCOTEST_MODES;

typedef struct {
    uint16 function;
    uint16 data;
} BCCMDPDU_ESCOTEST;

/****************************************************************************
BCCMDPDU_LIMIT_EDR_POWER
========================

Get or set the boolean determining whether the transmit power is
limited for EDR when class 1 radio operation is requested.
*/

typedef struct {
	uint16		hci_handle;	/* ACL/HCI handle. */
	bool		enable;	        /* Whether to limit */
	} BCCMDPDU_LIMIT_EDR_POWER;


/***************************************************************************
BCCMDPDU_LC_RX_FRACS
====================

Read the Packet Error Rate table.
On entry words indicates the maximum number of words that the array
 rx_fracs can hold.
On exit it indicates the number of words that were actually copied.
*/

typedef struct {
	uint16		value_count;
	uint16		rx_frac_vals[1];
	} BCCMDPDU_LC_RX_FRACS;


/***************************************************************************
On chips with the FM radio, this is the BCCMD interface
*/

typedef struct {
     uint8  fm_register;
     uint16 value;
} BCCMDPDU_FM_REG;

typedef struct {
     uint16 rds_len;     /* number of RDS blocks requested or actually read */
     uint16 rds_data[1]; /* RDS Data block */
} BCCMDPDU_FM_RDS;

/***************************************************************************
On chips with the FM radio, this is the BCCMD interface
*/

typedef struct {
     uint8  fmtx_register;
     uint16 value;
} BCCMDPDU_FMTX_REG;

typedef struct {
     uint16 text_type;
     uint16 string[66];
} BCCMDPDU_FMTX_TEXT;

/****************************************************************************
BCCMDPDU_PCM_CONFIG32
=====================

This BCCMD is used to override the value of the PCM_CONFIG32 PSKey. */

typedef struct {
     uint16             pcm_if_id;  /* A valid PCM bus ID (0 or 1) */
     uint32             config32;   /* The new pcm_config32 value */
} BCCMDPDU_PCM_CONFIG32;

/****************************************************************************
BCCMDPDU_PCM_LOW_JITTER
=======================

This BCCMD is used to override the value of the PCM_LOW_JITTER PSKey. */

typedef struct {
     uint16             pcm_if_id;  /* A valid PCM bus ID (0 or 1) */
     uint32             low_jitter; /* The new pcm_low_jitter value */
} BCCMDPDU_PCM_LOW_JITTER;

/****************************************************************************
BCCMDPDU_PCM_RATE_AND_ROUTE
===========================

This BCCMD is used to control the rate and routing of a given PCM port. */

typedef struct {
     uint16             port_id;    /* A valid voice port */
     uint16             sync_port;  /* Either a valid voice port or PCM_NO_PORT */
     uint16             io_type;    /* Where this port is being routed to */
     uint32             read_rate;  /* Read rate from port in Hz */
     uint32             write_rate; /* Write rate to port in Hz */
} BCCMDPDU_PCM_RATE_AND_ROUTE;

/****************************************************************************
BCCMDPDU_DIGITAL_AUDIO_*
========================

These BCCMDs are used to configure a PCM bus when used in I2S mode. */

typedef struct {
     uint16             pcm_if_id;  /* A valid PCM bus ID (0 or 1) */
     uint32             frequency;  /* The port's clock frequency in Hz */
     uint16             samp_size;  /* The number of bits per sample */
} BCCMDPDU_DIGITAL_AUDIO_RATE;

typedef struct {
     uint16             pcm_if_id;  /* A valid PCM bus ID (0 or 1) */
     uint16             options;    /* The PCM's configuration options */
} BCCMDPDU_DIGITAL_AUDIO_CONFIG;


/****************************************************************************
BCCMDPDU_SHA256
===============

These BCCMDs are used to pass messages into the sha256 processing engine
and get the corresponding hash result in return. */

typedef struct {
    uint16  msg_len;
    uint8   msg[1]; /* Place holder for variable length input string */
} BCCMDPDU_SHA256_DATA;
    
#define BCCMDPDU_SHA256_RES_HASH_SIZE (32)
typedef struct {
    uint32  time;
    uint8  hash[BCCMDPDU_SHA256_RES_HASH_SIZE]; /* Place holder for hash results array 32x8-bit hash vals */
} BCCMDPDU_SHA256_RES;


/****************************************************************************
BCCMDPDU_COMBO_DOT11_ESCO_RTX_PRIORITY
=========================== */

typedef struct {
     bool               enabled;
} BCCMDPDU_COMBO_DOT11_ESCO_RTX_PRIORITY;



/****************************************************************************
BCCMDPDU_HQ_SCRAPING
========================
This BCCMD is used to get an HQ packet to host over SPI */

typedef struct {
     uint16             hq_len;     /* hq packet length */
     uint16             hq_data[1]; /* hq data block */
} BCCMDPDU_HQ_SCRAPING;
/****************************************************************************
BCCMDPDU_HQ_SCRAPING_ENABLE
========================
This BCCMD is used to get enable  HQ packet to host over SPI */

typedef struct {
	bool enable;/* hq scraping on 1: hq scraping off 0 */
     
} BCCMDPDU_HQ_SCRAPING_ENABLE;


/****************************************************************************
BCCMDPDU_PANIC_ON_FAULT
========================
This BCCMD is used to alter default behaviour of PSKEY_PANIC_ON_FAULT*/

typedef struct {
	bool enable;/* panic on fault on 1: panic on fault 0 */
     
} BCCMDPDU_PANIC_ON_FAULT;

/*****************************************************************************
BCCMDPDU_MIC_BIAS_CTRL

This BCCMD is used to control the MIC bias on for mic bias instance 0.
*/


typedef struct{
        uint16  mic_bias_enable;
        uint16  mic_bias_current;
        uint16  mic_bias_voltage;
        uint16  enable_mic_bias_low_power_mode;
}BCCMDPDU_MIC_BIAS_CTRL;

/****************************************************************************
BCCMDPDU_WLAN_COEX_PRIORITY

*/
typedef struct {
    uint16 handle;     /* The ACL id to prioritise */
    bool enable;
}BCCMDPDU_WLAN_COEX_PRIORITY;

/***************************************************************************
BCCMDPDU_RANDOM

This BCCMD is used for performing tests on the random number generator
*/

typedef struct {
	uint16		len;	    /* Number of 16bit words to get */
	uint16		type ;      /*  Type of data to return */
	uint16		dummy;		/* Padding word - unused. */
	uint16		data[1];	/* Data. */
}BCCMDPDU_RANDOM ;

/***************************************************************************
BCCMDPDU_FASTPIPE_ENABLE

This BCCMD is used to enable the FastPipe system.
*/

typedef struct {
    /* Data to be filled in by host in request */
    uint32 limit_host;       /* Limit on data that can be sent to the host */

    /* Data filled in by controller in response */
    uint16 result;           /* Indicates success or specific reason for failure */
}BCCMDPDU_FASTPIPE_ENABLE;

/***************************************************************************
BCCMDPDU_FASTPIPE_CREATE

This BCCMD is used to create a new FastPipe.
*/
typedef struct {
    /* Data to be filled in by host in request */
    uint16 pipe_id;                 /* ID for the pipe */
    uint32 overhead_host;           /* Pipe overhead on the host */
    uint32 capacity_rx_host;        /* Capacity of receive buffer on the host */
    uint32 required_tx_controller;  /* Required capacity of tx buffer on controller */
    uint32 desired_tx_controller;   /* Desired capacity of tx buffer on controller */
    uint32 required_rx_controller;  /* Required capacity of rx buffer on controller */
    uint32 desired_rx_controller;   /* Desired capacity of rx buffer on controller */

    /* Data filled in by controller in response */
    uint32 overhead_controller;     /* Pipe overhead on the controller */
    uint32 capacity_tx_controller;  /* Capacity of transmit buffer on controller */
    uint32 capacity_rx_controller;  /* Capacity of receive buffer on controller */
    uint16 handle;                  /* Handle for the new pipe */
    uint16 result;                  /* Indicates success or specific reason for failure */
}BCCMDPDU_FASTPIPE_CREATE;

/***************************************************************************
BCCMDPDU_FASTPIPE_DESTROY

This BCCMD is used to destroy an existing FastPipe.
*/

typedef struct {
    /* Data to be filled in by host in request */
    uint16 pipe_id;             /* ID of the pipe */

    /* Data filled in by controller in response */
    uint16 result;              /* Indicates success or specific reason for failure */
}BCCMDPDU_FASTPIPE_DESTROY;


/***************************************************************************
BCCMDPDU_FASTPIPE_RESIZE

This BCCMD is used to alter limit_controller
*/
typedef struct {
    /* Data to be filled in by host in request */
    int32 delta_controller;  /* The change in limit_controller as a signed
                              * value */

    /* Data filled in by controller in response */
    uint32 limit_controller; /* The new limit_controller */
    uint16 result;           /* Indicates success or specific reason for failure */
}BCCMDPDU_FASTPIPE_RESIZE;


/****************************************************************************
BCCMDPDU_ALLOCATE_RAM_RESERVE / BCCMDPDU_RECLAIM_RAM_RESERVE 

These BCCMDs allow a host to allocate a RAM reserve (for now the only valid type is 
tokens) to, or reclaim the RAM reserve (tokens) from, the hostio_ram_reserve subsystem.
This RAM reserve is used for shunts and FastPipe.
*/

/* Allowed values for 'type' in BCCMDPDU_ALLOCATE/RECLAIM_RAM_RESERVE */
typedef enum {
    RAM_RESERVE_ALLOC_TYPE_TOKEN = 1
} RAM_RESERVE_ALLOC_TYPES;

typedef struct {
    uint16   type;
    uint16  value;    
} BCCMDPDU_ALLOCATE_RAM_RESERVE;

typedef struct {
    uint16   type;
    uint16  value;    
} BCCMDPDU_RECLAIM_RAM_RESERVE;


/****************************************************************************
BCCMDPDU_COEX_INT_TEST

This BCCMD is used to test the coex interface
*/

typedef struct {
    uint8 mode;            /* 0x00=START, 0x01=WRITE_DIR,
                                               0x02=WRITE_LVL, 0x03=READ, 0X04=STOP */
    uint8 value;           /* Bit mask - bit 0: RF_Active
                                                                    bit 1: Status
                                                                    bit 2: TX_CONFX
                                                                    bits 3-7: reserved */
}BCCMDPDU_COEX_INT_TEST;


/****************************************************************************
BCCMDPDU_COEX_DUMP
    This BCCMD is used to dump coexistence info.

DESCRIPTION
    The following enums and structure provide an abstraction between the
    internal Coexistence Module and the external dump interface.
*/
enum COEX_DUMP_ENUM {

    BCCMDPDU_COEX_DUMP_SCM_NONE                           = 0x0000,
    BCCMDPDU_COEX_DUMP_SCM_UNITY_2                        = 0x0100,
    BCCMDPDU_COEX_DUMP_SCM_UNITY_3                        = 0x0200,

    BCCMDPDU_COEX_DUMP_OPT_NONE                           = 0x0000,
    BCCMDPDU_COEX_DUMP_OPT_INBAND                         = 0x0001,
    BCCMDPDU_COEX_DUMP_OPT_PLUS                           = 0x0002,
    BCCMDPDU_COEX_DUMP_OPT_EXPRESS                        = 0x0004,

    BCCMDPDU_COEX_DUMP_SCHEME_NONE                        = 0x0000,
    BCCMDPDU_COEX_DUMP_SCHEME_UNITY_2                     = 0x0100,
    BCCMDPDU_COEX_DUMP_SCHEME_UNITY_3                     = 0x0200,
    BCCMDPDU_COEX_DUMP_SCHEME_UNITY_4                     = 0x0201,
    BCCMDPDU_COEX_DUMP_SCHEME_UNITY_3_PLUS                = 0x0202,
    BCCMDPDU_COEX_DUMP_SCHEME_UNITY_4_PLUS                = 0x0203,
    BCCMDPDU_COEX_DUMP_SCHEME_UNITY_3E                    = 0x0204,
    BCCMDPDU_COEX_DUMP_SCHEME_UNITY_4E                    = 0x0205,
    BCCMDPDU_COEX_DUMP_SCHEME_UNITY_3E_PLUS               = 0x0206,
    BCCMDPDU_COEX_DUMP_SCHEME_UNITY_4E_PLUS               = 0x0207,
    BCCMDPDU_COEX_DUMP_SCHEME_UNITY_PRO                   = 0x0400,

    BCCMDPDU_COEX_DUMP_SIGNAL_DIABLED                     = 0x0000,
    BCCMDPDU_COEX_DUMP_SIGNAL_ACTIVE_HIGH                 = 0x0001,
    BCCMDPDU_COEX_DUMP_SIGNAL_ACTIVE_LOW                  = 0x0002,
    BCCMDPDU_COEX_DUMP_SIGNAL_ACTIVE                      = 0x0003
};


typedef struct {
    uint16 scheme;
    uint16 options;

    uint16 bt_priority;
    uint16 wlan_active;

    uint16 bt_active;
    uint16 bt_status;
    uint16 wlan_deny;

    uint16 bt_periodic;
    uint16 bt_inband;

    uint16 uart_active;
    uint16 uart_tx;
    uint16 uart_rx;

    uint16 time_t1;
    uint16 time_t2;

    uint16 btactive_lead;
    uint16 btstatus_lead;

    uint16 priority_table00;
    uint16 priority_table01;
    uint16 priority_table02;
    uint16 priority_table03;
    uint16 priority_table04;
    uint16 priority_table05;
    uint16 priority_table06;
    uint16 priority_table07;
    uint16 priority_table08;
    uint16 priority_table09;
    uint16 priority_table10;
    uint16 priority_table11;
    uint16 priority_table12;
    uint16 priority_table13;
    uint16 priority_table14;
    uint16 priority_table15;
    uint16 priority_table16;

    uint16 priority_table17;
    uint16 priority_table18;
    uint16 priority_table19;
    uint16 priority_table20;
    uint16 priority_table21;
    uint16 priority_table22;
    uint16 priority_table23;
    uint16 priority_table24;
    uint16 priority_table25;
    uint16 priority_table26;
    uint16 priority_table27;
    uint16 priority_table28;
    uint16 priority_table29;
    uint16 priority_table30;
    uint16 priority_table31;
    uint16 priority_table32;

    uint16 debug_enable;
    uint16 debug00;
    uint16 debug01;
    uint16 debug02;
    uint16 debug03;
    uint16 debug04;
    uint16 debug05;
    uint16 debug06;
    uint16 debug07;
    uint16 debug08;
    uint16 debug09;
    uint16 debug10;
    uint16 debug11;
    uint16 debug12;
    uint16 debug13;
    uint16 debug14;
    uint16 debug15;

}BCCMDPDU_COEX_DUMP;
                            


/****************************************************************************
ENUM
        coex_int_test_enum  -  constants relating to the WLAN Coexistence Interface Test

DESCRIPTION
    This enum contains definitions for constants as specified 
    in WLAN Coexistence Interface Test. These constants are used by the
    BCCMD and FBCMD written to perform this test and in
    lc_combo_dot11_test_mode_determine  in dev/src/csr/lc/combo_dot11.c.
*/

enum coex_int_test_enum {

    BCCMDPDU_COEX_INT_TEST_MODE_START                        = 0x00,
    BCCMDPDU_COEX_INT_TEST_MODE_WRITE_DIR                    = 0x01,
    BCCMDPDU_COEX_INT_TEST_MODE_WRITE_LVL                    = 0x02,
    BCCMDPDU_COEX_INT_TEST_MODE_READ                         = 0x03,
    BCCMDPDU_COEX_INT_TEST_MODE_STOP                         = 0x04,

    BCCMDPDU_COEX_INT_TEST_STATUS_SUCCESS                    = 0x00,
    BCCMDPDU_COEX_INT_TEST_STATUS_UNSUPPORTED_MODE_ATTEMPTED = 0x01,
    BCCMDPDU_COEX_INT_TEST_STATUS_UNABLE_PERFORM_OP          = 0x02,

    BCCMDPDU_COEX_INT_TEST_VALUE_RET_ZERO                    = 0x00

};

/****************************************************************************
BCCMDPDU_GPS_START

This BCCMD is used to start the GPS receiver.

Please keep COASTER_START up-to-date with any changes to this struct; they
need the same configuration info for obvious reasons.
*/

/**
 * @brief gps_start - Start Streaming Command
 * 
 * This command is used to start GPS streaming of I and Q data.
 * 
 * See Section 5.1 BC7830A16 GPS API Specification - CS-119889
 *
 * var id uint16 BCCMDVARID_GPS_START GPS Start command identifier (=0x5046)
 * 
 * @param uint16 flags:
 *   See Table 5.2 of CS-119889)
 *   A bitset containing:
 *   - bit 0: Sample size, 0: 1 bit, 1: 2 bit
 *   - bit 1: Bit order, 0: little, 1: big
 *   - bit 2: Interleave I/Q samples (N/A if coaster)
 *   - bit 3: Byte order, 0: little, 1: big
 *   - bits 4-7: Sample rate, 11: 1.024 MHz, 7: 1.536 MHz, 5: 2.048MHz
 *   - bit 8: Streaming with Coaster (Unsupported) (SBZ)
 *   - bit 9: Coaster enable (always set in BCCMDPDU_COASTER_START)
 *   - bit 10: reserved SBZ
 *   - bit 11: reserved SBZ
 *   - bit 12: Duty cycle lo: 00=>100%, 01=>75% 
 *   - bit 13: Duty cycle hi: 10=>50%, 11=>25% all approx.
 *   - bit 14: reserved SBZ
 *   - bit 15: Test mode, deliver one buffer then stop (SBZ)
 * @param uint16 pad1: zeropadding SBZ (=0x0000)
 * @param uint16 pad2: zeropadding SBZ (=0x0000)
 * @param uint16 pad3: zeropadding SBZ (=0x0000)
 * 
 * @return BCCMDPDU_STAT_OK
 * 
 * Note: Sample frequency is coded as
 * @code
 *          (12288kHz /(sample frequency - 1))
 * @endcode
 * hence the numbers 11, 7, 5 in Table 5.7 of CS-119889; these are directly
 * inserted into a hardware divider.  Duty cycle: to reduce host transport
 * load and/or BlueCore XAP processor load and/or power consumption, the GPS
 * radio can be instructed to take fewer radio slots than are available to
 * it. The reduction in duty cycle is approximately 25%, 50% or 75%. If
 * Bluetooth activity takes place interleaved with GPS radio use, these
 * proportions are necessarily inexact, but some proportional reduction of GPS
 * radio use will occur.
 *
 * This command can also be used to control Coaster, by setting bit 9 of flags.
 *
 * Stop the GPS streaming using BCCMDPDU_GPS_STOP.
 *
 */
typedef struct {
        uint16 flags;   /* Bit mask
                           bit 0: Sample size, 0: 1 bit, 1: 2 bit
                           bit 1: Bit order, 0: little, 1: big
                           bit 2: Interleave I/Q samples (N/A if coaster)
                           bit 3: Byte order, 0: little, 1: big

                           bit 4-7: Sample rate, 11: 1.024 MHz, 7: 1.536 MHz, 5: 2.048MHz

                           bit 8: Streaming with Coaster (if bit 9 set)
                           bit 9: Coaster enable (no streaming)
                           bit 10: reserved SBZ
                           bit 11: reserved SBZ

                           bit 12: Duty cycle lo: 00=>100%, 01=>75% 
                           bit 13: Duty cycle hi: 10=>50%, 11=>25% all approx.
                           bit 14: reserved SBZ
                           bit 15: Test mode, deliver one buffer then stop (SBZ)
                        */
        uint16 delay;   /* Not used. */
} BCCMDPDU_GPS_START;

/****************************************************************************
BCCMDPDU_GPS_STOP

This BCCMD is used to stop the GPS receiver.  
*/

/**
 * @brief gps_stop - Stop Streaming Command
 * 
 * This command is used to stop GPS streaming of I and Q data. 
 * 
 * See section 5.2 BC7830A16 GPS API Specification - CS-119889
 * 
 * var id uint16 BCCMDVARID_GPS_STOP GPS Stop command identifier (=0x5047)
 * 
 * @param uint16 flags: 0 == PAUSE, 1 == SHUTDOWN
 * @param uint16 pad1: zeropadding SBZ (=0x0000)
 * @param uint16 pad2: zeropadding SBZ (=0x0000)
 * @param uint16 pad3: zeropadding SBZ (=0x0000)
 * 
 * @return BCCMDPDU_STAT_OK
 * 
 * Depending on the flag, either the chip is told to go into shutdown
 * (deep sleep) or into pause mode (standby). In pause mode the TCXO is
 * running and time stamp and phase coherence is maintained. In Shutdown the
 * timestamp is not kept precise, nor is the phase coherency, as the chip is
 * powered down and running on RTC clock.  Note: If BT activity requires the
 * chip to be in a different mode (more power hungry mode) this will have
 * precedence, so if there is BT activity there is no guarantee that the chip
 * for example enters shutdown.
 *
 * This command is synonymous with coaster_stop (BCCMDPDU_COASTER_STOP)
 */
typedef struct {
    uint16 flags; /* bit 0: set: SHUTDOWN, unset:PAUSE */
} BCCMDPDU_GPS_STOP;

/****************************************************************************
BCCMDPDU_VM_STATUS

Poll the status of the VM and, if terminated normally, the exit code.
*/

typedef struct {
    uint16 vmstatus;    /* see interface/host/vm/status.h */
    uint16 exit_code; /* only meaningful if status is VM_STATUS_EXIT */
} BCCMDPDU_VM_STATUS;


/****************************************************************************
BCCMDPDU_EGPS_PULSE_CONFIG

This BCCMD is used to start the eGPS pulse
*/

/** 
 * @brief egps_pulse_config - Configure EGPS Pulse operation (NOT USED)
 *
 * var id uint16 EGPS Pulse input command identifier (= 0x5049)
 *
 * This command is not used.
 */
typedef struct {
    uint16 pio;             /*INPUT/OUTPUT mode, we support values from 0-7*/
} BCCMDPDU_EGPS_PULSE_CONFIG;

/****************************************************************************
BCCMDPDU_EGPS_PULSE_INPUT_START
*/

/** 
 * @brief egps_pulse_input_start - Start EGPS Input Pulse operation
 * 
 * See 7.2 BC7830A16 GPS API Specification - CS-119889
 * 
 * var id uint16 EGPS Pulse input command identifier (= 0x504A)
 * 
 * Enables the triggering of a pulse from the selected PIO. The BCCMD command
 * egps_pulse_config is used to select which PIO to use for triggering.  When
 * the triggering occurs, BlueCore returns back a timestamp to the host. The
 * timestamp is returned in two separate parameters: Parameter 1:
 * Microseconds(us), Parameter 2: 1/80ths of microsecond, nanoseconds in units
 * of 12.5ns. The maximum units you can have here is 79.  This is the same
 * time format and reference as the GPS streamed data and Coaster parameter
 * reference times.  Note: The last parameter is flags which are unused
 * 
 * @param uint8 pio:         PIO to be used with this functionality
 * @param uint8 edge_mode:   INPUT mode 0 = falling, 1 = rising
 * @param uint16 pad1: zeropadding SBZ (=0x0000)
 * @param uint16 pad2: zeropadding SBZ (=0x0000)
 * @param uint16 pad3: zeropadding SBZ (=0x0000)
 *
 * @return BCCMDPDU_STAT_OK
 */ 
typedef struct {
    uint8 pio;                  /*pio to be used with this funcitonality*/
    uint8 edge_mode;         /*INPUT mode 0=falling 1=rising*/
} BCCMDPDU_EGPS_PULSE_INPUT_START;

/****************************************************************************
BCCMDPDU_EGPS_PULSE_OUTPUT_START

This BCCMD is used to start the eGPS pulse
Note: the pulse_period_us & pulse_period_ns are added together to make the final pulse period. 
E.g. pulse_period_us=1 pulse_period_ns=2 => result = 1us + (2x12.5ns) = 1.025us or 1025ns
*/

/**
 * @brief egps_pulse_output_start - Start EGPS Output Pulse operation
 * 
 * see 7.1 BC7830A16 GPS API Specification - CS-119889
 * 
 * var id uint16 EGPS Pulse output command identifier (= 0x504B)
 * 
 * The EGPS Pulse features provide the option to timestamp an input pulse
 * received on a programmable I/O pin, and to output a pulse on a PIO at a
 * given time. In both cases the resolution is in units of the 80MHz internal
 * clock, i.e. 12.5ns.
 * 
 * This command has two sets of functionality: a) Single pulse generation:
 * repeat_time is set to zero; b) Continuous pulse generation: Signal is
 * repeated.
 * 
 * The times have been split up in two parts: Microseconds: The range is
 * 4294967296 (a 32-bit register for unsigned integers) Nanoseconds expressed
 * in 1/80ths microseconds: The fine time is units of 1/80th microsecond, one
 * unit is 12.5ns. The maximum valid value is 79 (a 7 bit register) This is
 * the same format and time reference as Coaster parameter timestamps and
 * streamed GPS data message timestamps.
 * 
 * @param uint8  pio            : PIO to be used with this functionality
 * @param uint32 start_time     : When the pulse should start (us)
 * @param uint32 duration_time  : Duration for the pulse (us )
 * @param uint32 repeat_time    : Time until the pulse will repeat (us)
 * @param int16  start_time_fine: Fraction of start time in 1/80ths of us
 * @param int16  duration_fine  : Fraction of duration time in 1/80ths of us
 * @param int16  repeat_fine    : Fraction of repeat time
 * @param int16  flags          : Unused (SBZ)
 *
 * @return BCCMDPDU_STAT_OK
 */ 
typedef struct {
    uint8 pio;                  /*pio to be used with this funcitonality*/
    uint32 start_time; /* OUTPUT, when the pulse should start (us) */
    uint32 duration_time; /* OUTPUT, the duration for the pulse (us) */
    uint32 repeat_time; /* OUTPUT, time until the pulse will repeat (us) */
    uint16 start_time_fine;      /* OUTPUT, fraction of start time */
    uint16 duration_time_fine;      /* OUTPUT, fraction of duration time */
    uint16 repeat_time_fine;      /* OUTPUT, fraction of repeat time */
    uint16 flags;                /* unused for the moment */
} BCCMDPDU_EGPS_PULSE_OUTPUT_START;

/****************************************************************************
BCCMDPDU_EGPS_PULSE_STOP

This BCCMD is used to stop the eGPS pulse.  
*/

/** 
 * @brief egps_stop_pulse - Stop EGPS Pulse operation
 *
 * See 7.4 BC7830A16 GPS API Specification - CS-119889
 * 
 * var id uint16 EGPS Pulse stop command identifier (= 0x504C)
 * 
 * Stops all the pulse activities on the selected PIO or if no parameter is
 * given then all PIOs are restored to their original state.
 *
 * @param uint8 pio_stop:          Which PIO to stop/restore
 * @param uint8 flags:             Unused (SBZ)
 * @param uint16 pad1: zeropadding SBZ (=0x0000)
 * @param uint16 pad2: zeropadding SBZ (=0x0000)
 * @param uint16 pad3: zeropadding SBZ (=0x0000)
 *
 * @return BCCMDPDU_STAT_OK
 */
typedef struct {
        uint8 pio_stop;     /* which pio to stop/restore */
        uint16 flags;   /* Unused for the moment */
} BCCMDPDU_EGPS_PULSE_STOP;

/****************************************************************************
BCCMDPDU_SCO_PARAMETERS
=======================

This BCCMD is used to read SCO/eSCO parameters
*/
    
typedef struct {
    uint16 sco_handle; /* A valid SCO handle */
    uint16 tsco; /* Slots between SCO/eSCO windows */
    uint16 wesco; /* Slots in eSCO retx window */
    uint16 reserved_slots; /* Slots in SCO/eSCO reserved window */
} BCCMDPDU_SCO_PARAMETERS;
	
/****************************************************************************

--------- COASTER control commands (Jemima LC7 GPS Coaster)

BCCMDPDU_COASTER_START

This BCCMD is used to start the coaster, analogous to GPS_START which
starts delivery of GPS antenna data to the host.

Structure must actually exist with primitive members only, rather than
being just typedef'd, for some parser in btcli build.  Hence this is a copy
of what's in GPS_START.
*/
/**
 * @brief coaster_start - Start Coaster Command
 * 
 * See 6.1 BC7830A16 GPS API Specification - CS-119889
 * 
 * var id uint16 Coaster start command identifier (= 0x504e)
 * 
 * Coaster can relieve the host CPU of the load of tracking satellites that
 * have been acquired successfully. When coaster is operating, there is no
 * streaming of I/Q samples required to the host. Tracking is entirely done on
 * BlueCore using a Delay Lock Loop (DLL) and Phase Lock Loop (PLL). Coaster
 * can track at least 9 satellites and possibly up to 12 depending on other
 * load (e.g. Bluetooth traffic). Coaster is capable of tracking and decoding
 * databits down to 25dBHz.  For the Coaster to track satellites, the host
 * software must send down the tracking parameters for each satellite, using
 * BCCMDPDU_COASTER_ADD_SV.  Once
 * the Coaster has taken over the tracking it updates the host periodically,
 * at a particular time, or synchronously depending on what the host requests
 * using BCCMDPDU_COASTER_INDICATIONS.
 * 
 * This BCCMD command is used to start the Coaster. This command has the same
 * format as gps_start, but the coaster enable bit is always set to 1 by the
 * firmware (should be set as 0 by the host).
 * 
 * @param uint16 flags:
 *   See Table 5.2 of CS-119889)
 *   A bitset containing:
 *   - bit 0: Sample size, 0: 1 bit, 1: 2 bit
 *   - bit 1: Bit order, 0: little, 1: big
 *   - bit 2: Interleave I/Q samples (N/A if coaster)
 *   - bit 3: Byte order, 0: little, 1: big
 *   - bits 4-7: Sample rate, 11: 1.024 MHz, 7: 1.536 MHz, 5: 2.048MHz
 *   - bit 8: Streaming with Coaster (Unsupported) (SBZ)
 *   - bit 9: Coaster enable (set by the firmware)
 *   - bit 10: reserved SBZ
 *   - bit 11: reserved SBZ
 *   - bit 12: Duty cycle lo: 00=>100%, 01=>75% 
 *   - bit 13: Duty cycle hi: 10=>50%, 11=>25% all approx.
 *   - bit 14: reserved SBZ
 *   - bit 15: Test mode, deliver one buffer then stop (SBZ)
 * @param uint16 pad1: zeropadding SBZ (=0x0000)
 * @param uint16 pad2: zeropadding SBZ (=0x0000)
 * @param uint16 pad3: zeropadding SBZ (=0x0000)
 * 
 * @return BCCMDPDU_STAT_OK
 * 
 * Note: The coaster only supports non-interleaved and little endian bits and
 * byte order.  Sample frequency is coded as
 * @code
 *          (12288kHz /(sample frequency - 1))
 * @endcode
 * hence the numbers 11, 7, 5 in Table 5.7 of CS-119889; these are directly
 * inserted into a hardware divider.  Duty cycle: to reduce host transport
 * load and/or BlueCore XAP processor load and/or power consumption, the GPS
 * radio can be instructed to take fewer radio slots than are available to
 * it. The reduction in duty cycle is approximately 25%, 50% or 75%. If
 * Bluetooth activity takes place interleaved with GPS radio use, these
 * proportions are necessarily inexact, but some proportional reduction of GPS
 * radio use will occur.
 *
 * Stop the coaster using BCCMDPDU_COASTER_STOP.
 */
typedef struct {
        uint16 flags;   /* Bit mask
                           bit 0: Sample size, 0: 1 bit, 1: 2 bit
                           bit 1: Bit order, 0: little, 1: big
                           bit 2: Interleave I/Q samples (N/A if coaster)
                           bit 3: Byte order, 0: little, 1: big

                           bit 4-7: Sample rate, 11: 1.024 MHz, 7: 1.536 MHz, 5: 2.048MHz

                           bit 8: Streaming with Coaster (if bit 9 set)
                           bit 9: Coaster enable (always set in COASTER_START)
                           bit 10: reserved SBZ
                           bit 11: reserved SBZ

                           bit 12: Duty cycle lo: 00=>100%, 01=>75% 
                           bit 13: Duty cycle hi: 10=>50%, 11=>25% all approx.
                           bit 14: reserved SBZ
                           bit 15: Test mode, deliver one buffer then stop (SBZ)
                        */
        uint16 delay;   /* Not used. */
} BCCMDPDU_COASTER_START;

/****************************************************************************
BCCMDPDU_COASTER_STOP

This BCCMD is used to stop the coaster, analogous to GPS_STOP.
*/

/**
 * @brief coaster_stop - Stop Coaster Command
 *
 * See 6.2 BC7830A16 GPS API Specification - CS-119889
 * 
 * var id uint16 Coaster stop command identifier (= 0x504f)
 * 
 * This command is sent to the chip in order to stop the coaster. Depending on
 * the flag, either the chip is told to go into shutdown (deep sleep) or into
 * pause mode (standby). In pause mode the TCXO is running and time stamp and
 * phase coherence is maintained. In Shutdown the timestamp is not kept
 * precise (nor phase coherency) - the chip is powered down and running on
 * RTC clock. If streaming of I/Q samples is required, the host must send a
 * gps_start command. Also, when switching between coaster and streaming, the
 * standby mode must be used in order to preserve timestamp and coherency.
 * Note: If BT activity requires the chip to be in a different mode (more
 * power hungry mode) this has precedence, in which case there is no guarantee
 * that the chip for example enters shutdown.
 * 
 * @param uint16 flags: 0 == PAUSE, 1 == SHUTDOWN
 * @param uint16 pad1: zeropadding SBZ (=0x0000)
 * @param uint16 pad2: zeropadding SBZ (=0x0000)
 * @param uint16 pad3: zeropadding SBZ (=0x0000)
 * 
 * @return BCCMDPDU_STAT_OK
 * 
 * This command is synonymous with gps_stop (BCCMDPDU_GPS_STOP)
 */
typedef struct {
    uint16 flags; /* bit 0: set: SHUTDOWN, unset:PAUSE */
} BCCMDPDU_COASTER_STOP;

/****************************************************************************
BCCMDPDU_COASTER_ADD_SV

This BCCMD is used to configure coaster tracking channels before coasting.
This structure will later be "too large".
*/

/** 
 * @brief coaster_add_sv - Add or Modify Satellites to Track
 *
 * See 6.3 BC7830A16 GPS API Specification - CS-119889
 * 
 * var id uint16 Coaster_add command identifier (= 0x5050)
 * 
 * This command is limited to a description of up to 4 satellites in each
 * message. The field numEntries in the first description indicates how many
 * satellites are to follow.
 * 
 * Note: Only the first satellite description numEntries is valid.  If the
 * Coaster is idle (i.e. the firmware is in the OFF or PAUSED state), then the
 * command adds satellites to the list that the Coaster will track. The satId
 * parameter must be unique; no satellite with the same satId must exist
 * already.  If the coaster is running (i.e. the firmware is in the COASTING
 * state), then this command modifies an existing tracked satellite. Therefore
 * a satellite with the satId must already exist, the opposite case to adding
 * a new SV.  The new parameters are set into the satellite state and its
 * tracking is completely reinitialised at the next appropriate time in the
 * coaster's processing. Thus one satellite can be replaced by another, if you
 * change all the parameters including the PRN (but not the satId).
 * 
 * @param uint16[12] satellites[4]: one to four Satellite descriptions:
 *   - First  Satellite uint16[ 0:11], a structure containing:
 *     - uint16 numEntries;        Count of satellite description entries present in
 *                         message, range 1-4. Only read this value from the
 *                         first satellite.
 *     - uint32 uSecs;       Timestamp uSecs part, range 0-2^32-1
 *     - uint16 uSecs80ths;  Timestamp 1/80th uSec part 0-79
 *     - int32 codeDoppler;   Doppler on the code. Range -16Hz, +16Hz. Signed
 *                         S15.16 two's-complement fixed-point format.
 *     - uint32 codeShift;  Phase of the code. 16.16 unsigned fixed-point
 *                         format in chips, range 0 <= x < 1023.0
 *     - int32 carrierDoppler;   Doppler on the carrier. Range -16kHz,+16kHz. Signed
 *                         S15.16 two's-complement fixed-point format.  
 *     - uint16 dataTransition;  Time (in ms) until acquisition time for which a bit
 *                         transition will occur.
 *     - uint16 satidPrn;  Bits[15:8]: ID for deleting this SV; bits[7:0] PRN
 *   - Second Satellite uint16[12:23] Optional
 *   - Third  Satellite uint16[24:35] Optional
 *   - Fourth Satellite uint16[36:47] Optional
 *
 * @return BCCMDPDU_STAT_OK
 * @return BCCMDPDU_STAT_BAD_PDU if parameters are invalid
 * 
 * The satid (Satellite Id) in bits[15:8] of satidPrn in each entry must be
 * unique as it is used for selecting which satellite to delete, and to
 * identify (in addition to the PRN) satellites when reading back the tracking
 * parameters. It has no other significance.  To track 6 satellites in the
 * coaster, send this message twice each with 3 different SVs detailed, or 6
 * times each with one satellite only, or once with 4 and once with 2, &c.
 *
 * Once added, satellite descriptions persist until deletion by means of
 * BCCMDPDU_COASTER_DEL_SV.
 */
typedef struct {
    uint16         num_entries; /* How many entries actually present? */
    /* Timestamp */
    uint32         uSecs;       /* 0-max */
    uint8          uSec80ths;   /* 0-80 */
    /* Info pertaining to that timestamp */
    uint32         code_doppler;      /* S15.16, up to +/-5Hz */
    uint32         code_shift;        /* 16.16, 0 <= x < 1023.0 */
    uint32         carrier_doppler;   /* S15.16, up to +/-15kHz */
    uint8          data_transition;   /* 0-20 ms to next dataBit boundary */
    uint16         satid_prn;         /* 8:8 handle to manage it:SV PRN
                                       * signal id */

    /* 12 words.  So can fit 4 of them in one 64-word message.  The btcli
     * builder requires extreme simplicity here, so we'll just repeat this
     * whole struct if supplying n SVs in one message.  numEntries is only
     * valid for the zeroth entry.
     */
#if 0 /* This exclusion does not apply in BTCli packet builder world. */
    /* Three repeats of the above content for the packet builder magic: */

    uint16         e1_dummy;     
    uint32         e1_uSecs;           
    uint8          e1_uSec80ths;       
    uint32         e1_code_doppler;    
    uint32         e1_code_shift;      
    uint32         e1_carrier_doppler; 
    uint8          e1_data_transition; 
    uint16         e1_satid_prn;       
                                    
    uint16         e2_dummy;     
    uint32         e2_uSecs;           
    uint8          e2_uSec80ths;       
    uint32         e2_code_doppler;    
    uint32         e2_code_shift;      
    uint32         e2_carrier_doppler; 
    uint8          e2_data_transition; 
    uint16         e2_satid_prn;       
                                    
    uint16         e3_dummy;     
    uint32         e3_uSecs;           
    uint8          e3_uSec80ths;       
    uint32         e3_code_doppler;    
    uint32         e3_code_shift;      
    uint32         e3_carrier_doppler; 
    uint8          e3_data_transition; 
    uint16         e3_satid_prn;       
#endif
} BCCMDPDU_COASTER_ADD_SV;

/****************************************************************************
BCCMDPDU_COASTER_DEL_SV

This BCCMD is used to delete coaster channel configuration..
*/

/** 
 * @brief coaster_del_sv - Delete or Suspend Satellites From a Tracking Set
 * 
 * See 6.4 BC7830A16 GPS API Specification - CS-119889
 * 
 * var id uint16 Coaster add SV identifier (= 0x5051)
 * 
 * The host may delete all satellites, completely clearing the tracked set, or
 * delete one or several individual satellites by satid by setting
 * coaster_del_sv. If the coaster is running at the time of the request,
 * delete all is rejected, and individual satellites are suspended rather than
 * deleted. They may be re-activated by setting their parameters using
 * BCCMDPDU_COASTER_ADD_SV for the appropriate satellite ID, as for
 * modifying a satellite.
 * 
 * @param uint16 svPairs1: One or two satellite ids, packed
 *                  bits[15:8]:bits[7:0]. Zero means no ID.
 * @param uint16 svPairs2:	One or two satellite IDs
 * @param uint16 svPairs3:	One or two satellite IDs
 * @param uint16 svPairs4:	One or two satellite IDs
 * 
 * @return BCCMDPDU_STAT_OK
 * @return BCCMDPDU_STAT_ERROR if sat ID not known
 * @return BCCMDPDU_STAT_ERROR if delete-all when coaster is running
 * 
 * As a special case, if all eight possible satellite identities are zero,
 * then all satellites are deleted.  Important Note: This deletes all tracking
 * information, the host must read the current tracking parameters back before
 * deleting the satellites or all information will be lost. The Coaster must
 * be idle to use the delete all option.  Deleting individual entries while
 * the Coaster is running (tracking) causes them to be suspended rather than
 * deleted, which reduces the load on the Coaster, possibly increasing its
 * performance for the remaining channels.
 */
typedef struct {
    uint16 sv_pairs1; /* Each entry codes two SVs by channel id (not PRN) */
                      /* Special case: all zero means delete all, not none. */
    uint16 sv_pairs2;
    uint16 sv_pairs3;
    uint16 sv_pairs4; /* Oh man, you can't even have an array... */

} BCCMDPDU_COASTER_DEL_SV;

/****************************************************************************
BCCMDPDU_COASTER_SV_STATE

This BCCMD is used to read back the tracking state of the coaster channels.
This structure will later be "too large".
*/

/** 
 * @brief coaster_sv_state - Read Coaster satellite state (NOT SUPPORTED)
 *
 * var id uint16 Coaster SV State command identifier (= 0x3052)
 * 
 * Reads back the SV parameters in a manner similar (but not identical) to
 * the style of BCCMDPDU_COASTER_ADD_SV.  This command is used only for testing
 * and it is not supported.
 *
 * @param uint16[14] satellites[4]: one to four Satellite descriptions:
 *   - First  Satellite uint16[ 0:13], a structure containing:
 *     - uint16 numEntries;      8:8 start entry and count (up to 4).
 *       - IN:  bytes 8:8 ents to skip:ents to read
 *       - OUT:           ents to skip:ents filled
 *     - uint32 uSecs;       Timestamp uSecs part, range 0-2^32-1
 *     - uint16 uSecs80ths;  Timestamp 1/80th uSec part 0-79
 *     - int32 codeDoppler;   Doppler on the code. Range -16Hz, +16Hz. Signed
 *                         S15.16 two's-complement fixed-point format.
 *     - uint32 codeShift;  Phase of the code. 16.16 unsigned fixed-point
 *                         format in chips, range 0 <= x < 1023.0
 *     - int32 carrierDoppler;     Doppler on the carrier. Range -16kHz,+16kHz. Signed
 *                         S15.16 two's-complement fixed-point format.  
 *     - uint16 lock_status_bits; lockStatus bitset per GPS struct.
 *     - uint16 SNR;               SNR x 8, ie. 13.3 fixed-point, a ratio not dB.
 *     - uint16 dataTransition;  Time (in ms) until acquisition time for which a bit
 *                         transition will occur.
 *     - uint16 satidPrn;  Bits[15:8]: ID for deleting this SV; bits[7:0] PRN
 *   - Second Satellite uint16[14:27] Optional
 *   - Third  Satellite uint16[28:41] Optional
 *   - Fourth Satellite uint16[42:55] Optional
 *
 * @return BCCMDPDU_STAT_OK
 * @return BCCMDPDU_STAT_NO_VALUE if the requested SV is not present
 */
typedef struct {
    /* IN:  bytes 8:8 ents to skip:ents to read
     * OUT:           ents to skip:ents filled */
    uint16           start_index_num_entries;
    /* Timestamp */
    uint32           uSecs;       /* 0-max */
    uint8            uSec80ths;   /* 0-80 */
    /* Info pertaining to that timestamp */
    uint32           code_doppler;      /* S15.16, up to +/-5Hz */
    uint32           code_shift;        /* 16.16, 0 <= x < 1023.0 */
    uint32           carrier_doppler;   /* S15.16, up to +/-15kHz */
    uint16           lock_status_bits;  /* lockStatus bitset per GPS struct */
    uint16           SNR;               /* SNR x 8, ratio not dB */
    uint8            data_transition;   /* 0-20 ms to next dataBit boundary */
    uint16           satid_prn;         /* 8:8 handle to manage it:SV PRN
                                         * signal id */

    /* 14 words.  So can fit 4 of them in one 64-word message.  Same as
     * ADD_SV, metadata only valid for first entry.  */

#if 0 /* This exclusion does not apply in BTCli packet builder world. */
    /* Three repeats of the above content for the packet builder magic: */
    uint16           e1_dummy; /* start_index_num_entries only valid in
                                * initial entry */
    uint32           e1_uSecs;
    uint8            e1_uSec80ths;
    uint32           e1_code_doppler;    
    uint32           e1_code_shift;      
    uint32           e1_carrier_doppler; 
    uint16           e1_lock_status_bits;
    uint16           e1_SNR;             
    uint8            e1_data_transition; 
    uint16           e1_satid_prn;       

    uint16           e2_dummy;
    uint32           e2_uSecs;
    uint8            e2_uSec80ths;
    uint32           e2_code_doppler;    
    uint32           e2_code_shift;      
    uint32           e2_carrier_doppler; 
    uint16           e2_lock_status_bits;
    uint16           e2_SNR;             
    uint8            e2_data_transition; 
    uint16           e2_satid_prn;       

    uint16           e3_dummy;
    uint32           e3_uSecs;
    uint8            e3_uSec80ths;
    uint32           e3_code_doppler;    
    uint32           e3_code_shift;      
    uint32           e3_carrier_doppler; 
    uint16           e3_lock_status_bits;
    uint16           e3_SNR;             
    uint8            e3_data_transition; 
    uint16           e3_satid_prn;       
#endif
} BCCMDPDU_COASTER_SV_STATE;

/****************************************************************************
BCCMDPDU_COASTER_INDICATIONS

This BCCMD is used set up asynchronous HCI channel 0x713
(buf_gps_ctl_tohost) indications so that the data-driven host can readback
the sat states to make a position or switch to host tracking or whatever.
The actual timestamp is in the message for debugging or to allow host to
lock with realtime somehow.
*/

/**
 * @brief coaster_indications - Request Coaster Indications Command
 *
 * See 6.5 BC7830A16 GPS API Specification - CS-119889
 * 
 * var id uint16 Coaster indications command identifier (= 0x5055)
 * 
 * @param uint16 events: One of:
 *    - 14 = 0x0e for a One-off Coaster Indication
 * 	- bit 0 = 0: Periodic indications off
 * 	- bit 1 = 1: One-off event at time specified
 * 	- bit 2 = 1: Tracking params indication
 * 	- bit 3 = 1: Decoded databits indication (optional)
 * 	- bit [4:15] = SBZ
 *    - 13 = 0x0d for Periodic Coaster Indications
 * 	- bit 0 = 1: Periodic indications on
 * 	- bit 1 = 0: No one-off event 
 * 	- bit 2 = 1: Tracking params indication
 * 	- bit 3 = 1: Decoded databits indication (optional)
 * 	- bit 4..15 = SBZ
 * @param union timing: either:
 *    - One-off event:
 *      -  uint32 uSecs_timestamp; Timestamp after which to make the indication
 *    - Or, periodic indications:
 *       - uint16 period_ms;  Period in milliseconds. 0 means one right now.
 *       - uint16 repeats;    Count of repeats. 0 means cancel all events
 * @param uint16 optflags:  If nonzero, bitset of which SV IDs to collect
 *                           and report the databits from.
 *                           Only applicable if databits reporting is on.
 *                           Otherwise SBZ.
 * @param uint16 pad3: zeropadding SBZ (=0x0000)
 * 
 * In all cases the indication occurs at the next opportunity the coaster
 * firmware gets to perform the operation.  There is no guarantee that the
 * requested timestamp for the indication is used. Bluetooth traffic may
 * result in a delay as to when the indication can be made. In any case, the
 * reported timestamp in the indication refers to when the satellite
 * parameters are/were valid, not the request time.
 */
typedef struct {
    uint16 events;  /* Bits 0,1 mask, set one of them only:
                         bit 0: Periodic indications
                         bit 1: Single indication close as we can after a
                         specific timestamp.
                       Bits 2...:
                         all zero: Just a time synch event, with a
                           timestamp in it at the earliest opportunity
                           after the requested time(s).
                         bit 2 set: packed SV info on up to 12 SVs.
                         bit 3 set: decoded databits+quality on up to 12 SVs.
                         bit 4 set: optflags is a bitset of SVids; only
                           collect databits for those set.  Bit 4 clear and
                           bit 3 set => collect databits for all SVs.
                    */
    uint32 uu; /* Union not allowed here because tools builder fails. */
    uint16 optflags;  /* bitset of SVids, bits 0-15 represent SVids 1-16. */
} BCCMDPDU_COASTER_INDICATIONS;

#ifdef __XAP__
union bccmdpdu_coaster_indications_arg {
    uint32 uSecs_timestamp; /* events == 1 */
    struct {      /* events == 2 */
        uint16 period_ms; /* event period (approx) in milliSecs, range O(1min).
                           * Zero means just to do one event right now. */
        uint16 repeats; /* How many times to fire off; 0 => cancel any periodic
                         * processing for those event types specified in the
                         * events mask. */
    } repeat;
};
#endif /* __XAP__ to exclude in tools build */

/****************************************************************************
BCCMDPDU_RDF_DATA_CAPTURE_ENABLE

This BCCMD is used to enable/disable the functionality that allows the
IQ data from different antennas to be captured and passed up to the host
during a recieve
*/

enum {
    RDF_BT_LINK = 1,
    RDF_ULP_SCAN = 2
};

typedef struct {
    uint16 modes;
    uint16 timer_delay; /* in milliseconds */
    uint16 nsams;
    uint16 nap;
    uint8 uap;
    uint32 lap; 
} BCCMDPDU_RDF_DATA_CAPTURE_ENABLE;

/****************************************************************************
BCCMDPDU_CODEC_INPUT_GAIN

This BCCMD is used to set the CODEC A and B input gains.
*/
typedef struct {
    uint16 gain_a;
    uint16 gain_b;
} BCCMDPDU_CODEC_INPUT_GAIN;

/****************************************************************************
BCCMDPDU_CODEC_OUTPUT_GAIN

This BCCMD is used to set the CODEC A and B output gains.
*/
typedef struct {
    uint16 gain_a;
    uint16 gain_b;
} BCCMDPDU_CODEC_OUTPUT_GAIN;
/****************************************************************************
BCCMDPDU_STREAM_GET_SOURCE

This structure is used for the following command:
BCCMDVARID_STREAM_GET_SOURCE
*/
typedef struct {
    uint16 resource;
    uint16 opt1;
    uint16 opt2;
} BCCMDPDU_STREAM_GET_SOURCE;
/****************************************************************************
BCCMDPDU_STREAM_GET_SINK

This structure is used for the following command:
BCCMDVARID_STREAM_GET_SINK
*/
typedef struct {
    uint16 resource;
    uint16 opt1;
    uint16 opt2;
} BCCMDPDU_STREAM_GET_SINK;
/****************************************************************************
BCCMDPDU_STREAM_CONFIGURE

This structure is used for the following command:
BCCMDVARID_STREAM_CONFIGURE
*/
typedef struct {
    uint16 sid;
    uint16 key_config;
    uint32 value;
} BCCMDPDU_STREAM_CONFIGURE;
/****************************************************************************
BCCMDPDU_STREAM_CONNECT

This structure is used for the following command:
BCCMDVARID_STREAM_CONNECT
*/
typedef struct {
    uint16 source_sid;
    uint16 sink_sid;
} BCCMDPDU_STREAM_CONNECT;
/****************************************************************************
BCCMDPDU_STREAM_ALIAS_SINK

This structure is used for the following command:
BCCMDVARID_STREAM_ALIAS_SINK
*/
typedef struct {
    uint16 sink1_sid;
    uint16 sink2_sid;
} BCCMDPDU_STREAM_ALIAS_SINK;
/****************************************************************************
BCCMDPDU_STREAM_SYNC_SID

This structure is used for the following command:
BCCMDPDU_STREAM_SYNC_SID
*/
typedef struct {
    uint16 sid1;
    uint16 sid2;
} BCCMDPDU_STREAM_SYNC_SID;

/****************************************************************************
BCCMDPDU_MAP_SCO_AUDIO

This structure is used for the following command:
BCCMDPDU_MAP_SCO_AUDIO
*/
typedef struct {
    uint16 source_sid;
    uint16 sink_sid;
} BCCMDPDU_MAP_SCO_AUDIO;

/****************************************************************************
BCCMDPDU_MAP_SCO_WBS

This structure is used for the following command:
BCCMDPDU_MAP_SCO_WBS
It is identical to BCCMDPDU_MAP_SCO_AUDIO
*/
typedef struct {
    uint16 source_sid;
    uint16 sink_sid;
} BCCMDPDU_MAP_SCO_WBS;

/****************************************************************************
BCCMDPDU_PCM_CLOCK_RATE

This structure is used for the following command:
BCCMDPDU_PCM_CLOCK_RATE
*/
typedef struct {
    uint16 pcm_interface;
    uint32 clock_rate;
} BCCMDPDU_PCM_CLOCK_RATE;

/****************************************************************************
BCCMDPDU_PCM_SLOTS_PER_FRAME

This structure is used for the following command:
BCCMDPDU_PCM_SLOTS_PER_FRAME
*/
typedef struct {
    uint16 pcm_interface;
    uint16 slots_per_frame;
} BCCMDPDU_PCM_SLOTS_PER_FRAME;

/****************************************************************************
BCCMDPDU_PCM_SYNC_RATE

This structure is used for the following command:
BCCMDPDU_PCM_SYNC_RATE
*/
typedef struct {
    uint16 pcm_interface;
    uint32 sync_rate;
} BCCMDPDU_PCM_SYNC_RATE;

/****************************************************************************
BCCMDPDU_RDF_BLE_PACKET_MATCH

This structure is used to set the packet matching required for RDF in
BLE scanning mode.
*/

enum {
    RDF_MATCH_NONE = 0,
    RDF_MATCH_HEADER = 1,
    RDF_MATCH_ADVADDR = 2,
    RDF_MATCH_PAYLOAD = 3
};

typedef struct {
    uint16 type;
    uint16 tplate[3];
    uint16 match[3];
} BCCMDPDU_RDF_BLE_PACKET_MATCH;

/****************************************************************************
BCCMDPDU_RDF_BLE_ADVERTISING

This structure is used to enable or disable RDF advertising with the required
parameters.
*/

typedef struct {
    uint8 mode;
    uint16 adv_interval;
    uint8 adv_type;
    uint8 addr_type;
    uint8 channels;
    uint8 data_len;
    uint8 anti_whiten_begin;
    uint8 anti_whiten_end;
    uint8 data[1];
} BCCMDPDU_RDF_BLE_ADVERTISING;


/****************************************************************************
BCCMDPDU_RDF_BLE_SCAN_MODE

This structure is used to enable/disable/set the RDF scanning mode.
*/
typedef enum {
    BCCMDPDU_RDF_BLE_SCAN_MODE_DISABLE = 0,
    BCCMDPDU_RDF_BLE_SCAN_MODE_DF = 1,
    BCCMDPDU_RDF_BLE_SCAN_MODE_POS = 2
} BCCMDPDU_RDF_BLE_SCAN_MODE_MODES;

typedef struct {
    uint8 mode;
    uint8 samples;
} BCCMDPDU_RDF_BLE_SCAN_MODE;


/****************************************************************************
BCCMDPDU_RDF_BLE_RAND_ADDR

This structure is used to set the RDF random address for RDF advertising.
*/

typedef struct {
    uint8 addr[1];
} BCCMDPDU_RDF_BLE_RAND_ADDR;

/****************************************************************************
BCCMDPDU_RDF_PT

This structure is used to enable and disable RDF production test.  The
antenna parameter is used as the antenna to select when entering production
test.
*/

typedef struct {
    uint8 enable;
    uint8 antenna;
} BCCMDPDU_RDF_PT;

/****************************************************************************
BCCMDPDU_INFORM_LINK_PROFILES

This structure is used to set and retrieve Leo specific information
pertaining to which ACL links are being used with certain profiles.
CS-201710-DC has details.
*/

typedef struct {
    uint16 handle;
    uint16 profiles;
} BCCMDPDU_INFORM_LINK_PROFILES;


/****************************************************************************
BCCMDPDU_CREATE_OPERATOR_C
                                                                                                                
Stibbons: create an operator (capability)
*/
                                                                                                                
typedef struct {
    uint16 id;
    uint16 num_patches;
    uint16 skip_count;
    uint16 skip_flag;
} BCCMDPDU_CREATE_OPERATOR_C;

/****************************************************************************
BCCMDPDU_CREATE_OPERATOR_P
                                                                                                                
Stibbons: create an operator (patch)
*/
                                                                                                                
typedef struct {
    uint16 len;
    uint16 patch[1];
} BCCMDPDU_CREATE_OPERATOR_P;

/****************************************************************************
BCCMDPDU_START_OPERATOR
                                                                                                                
Stibbons: start an operator
*/
typedef struct {
    uint16 count;
    uint16 list[1];
} BCCMDPDU_START_OPERATOR;

/****************************************************************************
BCCMDPDU_STOP_OPERATOR
                                                                                                                
Stibbons: stop an operator
*/
typedef struct {
    uint16 count;
    uint16 list[1];
} BCCMDPDU_STOP_OPERATOR;

/****************************************************************************
BCCMDPDU_RESET_OPERATOR
                                                                                                                
Stibbons: reset an operator
*/
typedef struct {
    uint16 count;
    uint16 list[1];
} BCCMDPDU_RESET_OPERATOR;

/****************************************************************************
BCCMDPDU_DESTROY_OPERATOR
                                                                                                                
Stibbons: destroy an operator
*/
typedef struct {
    uint16 count;
    uint16 list[1];
} BCCMDPDU_DESTROY_OPERATOR;

/****************************************************************************
BCCMDPDU_OPERATOR_MESSAGE
                                                                                                                
Stibbons: Message for operator.
*/
typedef struct {
    uint16 opid;
    uint16 arguments[1];
} BCCMDPDU_OPERATOR_MESSAGE;

typedef struct {
    uint16 message_num;
    uint16 length;
    uint16 message [1];
} BCCMDPDU_PREVIN_TUNNEL;

typedef struct {
    uint16 subsystem;
    uint16 arguments [1];
} BCCMDPDU_DSPMANAGER_DEBUG;

/****************************************************************************
BCCMDPDU_CLOCK_MODULATION_CONFIG
                                                                                                                
Data corresponding to PSKEY_CLOCK_MODULATION_CONFIG
*/
                                                                                                                
typedef struct {
    uint16 amount;
    uint16 rate;
    uint16 wibbling;
} BCCMDPDU_CLOCK_MODULATION_CONFIG;


/****************************************************************************
BCCMDVARID_CHARGER_TRIMS
                                                                                                                
Data corresponding charger trims stored in information block on BC7+ chips 
first introduced in Gordon. 

*/
typedef struct
{
    uint16  chgref_trim;   /* Trim Vref for charger */
    int16   hvref_trim;    /* Trim Vref for high voltage parts */
    uint16  rtrim;         /* Trim float voltage */
    uint16  itrim;         /* Trim charger current internal pass transistor */
    uint16  iext_trim;     /* Trim charger current external pass transistor */
    uint16  iterm_trim;    /* Trim to adjust the termination current */
    uint16  vfast_trim;    /* Trim trickle->fast charge transition voltage */
    uint16  hyst_trim;     /* Trim hysteresis for standby->fast transition */                        
}BCCMDPDU_CHARGER_TRIMS;

/* Create same thing under a different name for use in firmware. */
typedef BCCMDPDU_CHARGER_TRIMS trim_block;

/***************************************************************************
BCCMDPDU_PIO32

This BCCMD is used to control PIO32s
*/
typedef struct 
{
    /* Data to be filled in by host in request */
    uint32 mask;  /* PIOs to change */
    uint32 bits;  /* PIO pins value */

    /* Data filled in by controller in response. */
    uint32 result; /* Indicates success or specific reason for failure */
}BCCMDPDU_PIO32;

/* Create same thing under a different name for use in firmware. */
typedef BCCMDPDU_PIO32 pio32_set;

/***************************************************************************
BCCMDPDU_PIO32_DIRECTION_MASK

This BCCMD is used to control PIO32s
*/

typedef struct 
{
    /* Data to be filled in by host in request */
    uint32 mask;  /* PIOs to change */
    uint32 bits;  /* PIO pins value */

    /* Data filled in by controller in response. */
    uint32 result; /* Indicates success or specific reason for failure */
}BCCMDPDU_PIO32_DIRECTION_MASK;

/***************************************************************************
BCCMDPDU_PIO32_PROTECT_MASK

This BCCMD is used to control PIO32s
*/

typedef struct 
{
    /* Data to be filled in by host in request */
    uint32 mask;  /* PIOs to change */
    uint32 bits;  /* PIO pins value */

    /* Data filled in by controller in response. */
    uint32 result; /* Indicates success or specific reason for failure */
}BCCMDPDU_PIO32_PROTECT_MASK;

/***************************************************************************
BCCMDPDU_PIO32_STRONG_BIAS

This BCCMD is used to control PIO32s
*/

typedef struct 
{
    /* Data to be filled in by host in request */
    uint32 mask;  /* PIOs to change */
    uint32 bits;  /* PIO pins value */

    /* Data filled in by controller in response. */
    uint32 result; /* Indicates success or specific reason for failure */
}BCCMDPDU_PIO32_STRONG_BIAS;

/***************************************************************************
BCCMDPDU_PIO32_MAP_PINS

This BCCMD is used to control PIO32s
*/

typedef struct 
{
    /* Data to be filled in by host in request */
    uint32 mask;  /* PIOs to change */
    uint32 bits;  /* PIO pins value */

    /* Data filled in by controller in response. */
    uint32 result; /* Indicates success or specific reason for failure */
}BCCMDPDU_PIO32_MAP_PINS;

/****************************************************************************
BCCMDPDU_MIC_BIAS_CONFIGURE

This structure is used for the following command:
BCCMDVARID_MIC_BIAS_CONFIGURE
*/
typedef struct {
    uint16 instance;
    uint16 key_config;
    uint16 value;
} BCCMDPDU_MIC_BIAS_CONFIGURE;

/****************************************************************************
BCCMDPDU_PIO32_SET_PIN_FUNCTION

This structure is used for mapping the pin for a supported function
where 
    pin_number is (0...31)
    function member should carry corresponding value to map a function.
        UART_RX = 0
        UART_TX = 1
        UART_RTS = 2
        UART_CTS = 3
        PCM_IN = 4
        PCM_OUT = 5
        PCM_SYNC = 6
        PCM_CLK = 7
*/
typedef struct 
{
    uint16  pin_number;
    uint16  function;
}BCCMDPDU_PIO32_SET_PIN_FUNCTION;

/****************************************************************************
BCCMDPDU_LED_CONFIG

This structure is used to configure LEDs
The structure elements are:
    led -> led number from 0-5
    key -> led function to be used. The keys are:
        LED_ENABLE = 0
        LED_DUTY_CYCLE = 1
        LED_PERIOD = 2
        LED_FLASH_ENABLE = 3
        LED_FLASH_RATE = 4
        LED_FLASH_MAX_HOLD = 5
        LED_FLASH_MIN_HOLD = 6
        LED_FLASH_SEL_INVERT = 7
        LED_FLASH_SEL_DRIVE = 8
        LED_FLASH_SEL_TRISTATE = 9
    value -> respective argument passed
*/
typedef struct
{
    uint16 led;
    uint16 led_key;
    uint16 value;
} BCCMDPDU_LED_CONFIG;

/****************************************************************************
BCCMDPDU_BLE_ADV_FILT_ADD

The structure is used to control how BlueCore filters advertising report
events by advertising data content. Each use of this command will
add an advertising filter to any existing filters. Filtering is based only
on the contents of advertising data. Since directed connectable adverts do
not contain advertising data, and they are expressly intended for the
receiving device, they are always passed to the host and are unaffected
by this filter. Otherwise, the event type, Bluetooth device address of the
sender, and other properties of the advertising reports are ignored by the
filter. It just filters on data.

With no filter present, all advertising packets received during scan are
passed to the host in LE Advertising Report Events, subject to advert flood
protection. The filter is used to select advertising reports based on the
contents of the advertising data (AD) and send to the host only the
matching reports. Advertising reports that are not directed connectable and
do not match are discarded.

The structure elements are:
    operation -> taken from BCCMDPDU_BLE_ADVERTISING_REPORT_FILTER_OPERATION.
                 operation describes the relationship between multiple filters.
                 Currently the only valid operation is OR, meaning that adverts
                 will be sent to the host if they are matched by any of the
                 filters.
    ad_type -> the AD type of the AD structure to match. The filter will only
               match adverts containing an AD structure of this type.
    interval -> the interval for repeated attempts to match the pattern in
                the data portion of the AD structure. E.g. if the interval is
                4 then we attempt to match at offsets, 0, 4, 8, 12, etc in
                the data portion of the AD structure. If interval is 0 then
                we only attempt to match at offset 0.
    pattern_length -> the length of the pattern data.
    pattern -> the pattern data to be matched.

Directed connectable adverts are always passed straight through to the host,
but all others are compared against each filter until there is a match or the
filters have been exhausted. Since BCCMDPDU_BLE_ADV_REP_FILT_OR is currently
the only valid operation, advertising report events are passed to the host if
they match any filter.

For a filter to match:

1) The advertising data must comply with the advertising and scan response data
   format specified (see Bluetooth V4.0 Vol 3 Part C (GAP) Section 11).
2) The advertising data must contain an AD structure with AD type ad_type.
3) The data part of that AD structure must match the pattern either at 0
   offset into the data part of the AD structure, or at offsets given by
   multiples of the interval.
*/
typedef enum
{
    BCCMDPDU_BLE_ADV_REP_FILT_OR
} BCCMDPDU_BLE_ADV_REP_FILT_OPERATION;

typedef struct
{
    uint16 operation;
    uint16 ad_type;
    uint16 interval;
    uint16 pattern_length;
    uint16 pattern[1];
} BCCMDPDU_BLE_ADV_FILT_ADD;

/****************************************************************************
BCCMDPDU_BLE_RADIO_TEST

This BCCMD is used to perform a transmitter test using the Low Energy radio.

*/

/**
 * @brief ble_radio_test - Control a test of the Low Energy radio
 * 
 * This command is used to start or stop a test of the Low Energy radio.
 * 
 * The parameters are as specified for the LE HCI command 
 * HCI_LE_Transmitter_Test / HCI_LE_Receiver_Test. See the Bluetooth Host 
 * Controller Interface Functional Specification section 7.8.28 & 7.8.29.
 *
 * var id uint16 BCCMDVARID_LE_RADIO_TEST Control le radio test identifier 
 * (=0x508A)
 *
 * @param uint16 command:
 *   Test command
 *     0 BCCMDPDU_BLE_RADIO_TEST_STOP        Stop any test
 *     1 BCCMDPDU_BLE_RADIO_TEST_RECEIVER    Perform receiver test
 *     2 BCCMDPDU_BLE_RADIO_TEST_TRANSMITTER Perform transmitter test
 * @param uint8  channel:
 *   Number of the Low Energy channel to test
 *   Applicable to transmitter and receiver tests only
 * @param uint8  length:
 *   Applicable to transmitter test only
 *   Limited to 0 - 0x25
 * @param uint8 payload:
 *   Applicable to transmitter test only
 *   0x00 Pseudo-Random bit sequence 9
 *   0x01 Pattern of alternating bits '11110000'
 *   0x02 Pattern of alternating bits '10101010'
 *   0x03 Pseudo-Random bit sequence 15
 *   0x04 Pattern of All '1' bits
 *   0x05 Pattern of All '0' bits
 *   0x06 Pattern of alternating bits '00001111'
 *   0x07 Pattern of alternating bits '0101'
 *
 * @return BCCMDPDU_STAT_OK
 * 
 * Note: 
 *   Conversion of frequency to Low Energy channel is defined as
 * @code
 *          (Freq - 2402) / 2
 * @endcode
 *   i.e. channel 1 is 2404 Mhz
 *
 * Stop transmitter or receiver tests using 
 * BCCMDPDU_BLE_TRANSMITTER_TEST with BCCMDPDU_BLE_RADIO_TEST_STOP as 
 * the command 
 */
typedef enum {
    BCCMDPDU_BLE_RADIO_TEST_STOP = 0,
    BCCMDPDU_BLE_RADIO_TEST_RECEIVER = 1,
    BCCMDPDU_BLE_RADIO_TEST_TRANSMITTER = 2
} BCCMDPDU_BLE_RADIO_TEST_CMD;


typedef struct {
        uint16 command;
        uint8  channel;   
        uint8  length;
        uint8  payload;
} BCCMDPDU_BLE_RADIO_TEST;

/****************************************************************************
BCCMDPDU_SIFLASH

This BCCMD is used for serial flash memory operations

*/

/**
 * @brief siflash - Read/Write/Erase operation on serial flash memory
 * 
 * This command is used to Read/Write/Erase the serial flash memory
 * 
 * The parameters are as below
 * 
 * @param uint16 command:
 *   0 ==> BCCMDPDU_SIFLASH_READ_CMD           read data from serial flash
 *   1 ==> BCCMDPDU_SIFLASH_WRITE_CMD          write data from serial flash
 *   2 ==> BCCMDPDU_SIFLASH_CHIP_ERASE_CMD     Erase complete chip
 *   3 ==> BCCMDPDU_SIFLASH_SECTOR_ERASE_CMD   Erase sector
 *   4 ==> BCCMDPDU_SIFLASH_BLOCK64_ERASE_CMD  Erase block 64Kbytes
 *
 * @addr uint32 word_address
 *   word address in serial flash memory.
 *   (2 8bit data is equivalent to 1 word address)
 *
 * @param uint16 data_length:
 *   len of the data in words to write or read OR number of sectors/blocks to erase.
 *
 * @param uint16 data:
 *   data contains the words that should be written into/read from serial
 *   flash memory
 *
 * @return BCCMDPDU_STAT_OK
 * 
 */
typedef enum {
    BCCMDPDU_SIFLASH_READ_CMD,         /* read data from serial flash */
    BCCMDPDU_SIFLASH_WRITE_CMD,        /* write data from serial flash */
    BCCMDPDU_SIFLASH_CHIP_ERASE_CMD,   /* Erase complete chip */
    BCCMDPDU_SIFLASH_SECTOR_ERASE_CMD, /* Erase sector  */
    BCCMDPDU_SIFLASH_BLOCK64_ERASE_CMD /* Erase block 64Kbytes */
} BCCMDPDU_SIFLASH_CMD;

typedef struct {
    uint16 command;
    uint32 word_address;
    uint16 data_length;
    uint16 data[1];
} BCCMDPDU_SIFLASH;

/****************************************************************************
BCCMDPDU_CAPSENSE_RT_PADS

This BCCMD is used to initialise the Cap Sense Radio Test.

The structure is composed of a number of fields, some of which are supplied
by the host and some are returned by the server -
Host supplies -
    pad_sel - The bit pattern used to select which pads are to be initialised
              for read.  If the pattern includes PAD 0 and the pad is configured
              as shielded, the bit will cleared in the pattern,
Server returns -
    num_pads_board - The actual number of pads on the board,
    shield - Flag indicates whether PAD0 is shielded.
    pad_capn - The capacitance value of pad n.
*/
typedef struct
{
    uint16 pad_sel;             /* Pad selection bit pattern  */
    uint16 num_pads_board;      /* Number of pads on the board */
    uint16 shield;              /* Set TRUE if pad0 is shielded */
    uint16 pad_cap0;            /* Pad 0 capacitance value */
    uint16 pad_cap1;            /* Pad 1 capacitance value */
    uint16 pad_cap2;            /* Pad 2 capacitance value */
    uint16 pad_cap3;            /* Pad 3 capacitance value */
    uint16 pad_cap4;            /* Pad 4 capacitance value */
    uint16 pad_cap5;            /* Pad 5 capacitance value */
} BCCMDPDU_CAPSENSE_RT_PADS;

/****************************************************************************
BCCMDPDU_SPI_LOCK_CUSTOMER_KEY

This structure contains the 128-bit customer key that can sent by host.
128 bit is transmitted in four 32-bit using big-endian ordering.
*/
typedef struct
{
    uint32 cust_key1;
    uint32 cust_key2;
    uint32 cust_key3;
    uint32 cust_key4;
}BCCMDPDU_SPI_LOCK_CUSTOMER_KEY;


/****************************************************************************
This enum defines the return type values for BCCMDVARID_SPI_LOCK_STATUS
*/

/* Represent bit 0 for locked status and bit 1 for customer key status */
#define SPI_LOCK_LOCKED_STATUS_BIT_POS     0
#define SPI_LOCK_CUST_KEY_STATUS_BIT_POS   1

enum {
    SPI_LOCK_STATUS_UNLOCKED_AND_CUST_KEY_UNSET  = 0,
    SPI_LOCK_STATUS_LOCKED_AND_CUST_KEY_UNSET    = 1,
    SPI_LOCK_STATUS_UNLOCKED_AND_CUST_KEY_SET    = 2,
    SPI_LOCK_STATUS_LOCKED_AND_CUST_KEY_SET      = 3
};

/****************************************************************************
BCCMDPDU
========

A PDU always starts with a BCCMDPDU.  If the data is larger than will fit in
the BCCMDPDU the extra data follows the BCCMDPDU.  (E.g., the BCCMDPDU could
carry a large BCCMDPDU_PS value.)

The host sends PDUs of types BCCMDPDU_GETREQ and BCCMDPDU_SETREQ to the bccmd
on the chip.  The bccmd sends PDUs of type BCCMDPDU_GETRESP back to the
host.

The PDU "type" field is first in the structure to match CCL's conventions.

The PDU "pdulen" field gives the total length of the PDU counted in uint16s.
(The use of "Bluecore Friendly Format" implies the PDU must be an even number
of bytes, so we might as well count in uint16s.)

Each PDU carries a sequence number.  The bccmd (on chip) copies the "seqno"
from each PDU it receives into the corresponding BCCMDPDU_GETRESP(s).  This
allows the host to match questions to answers.  bccmd makes no use of
"seqno".

The "varid" field defines the variable to be read or written.  The value zero
must not be used to identify a variable.

Both the host and bccmd maintain knowledge of each varid's type; this implies
the variable's size.   (Though, incidently, this information can normally be
derived from a varid's value as these values are allocated in bands matching
variables' sizes.)

The "status" field allows the bccmd to proffer pathetic excuses for not
fulfilling the host's commands.  By default the "status" field takes value
BCCMDPDU_STAT_OK.  All PDUs of type BCCMDPDU_GETREQ and BCCMDPDU_SETREQ must
have a "status" of BCCMDPDU_STAT_OK.

Not all "varid" values are defined.  If the bccmd doesn't recognise the
"varid" of a variable it will reply with a "status" of
BCCMDPDU_STAT_NO_SUCH_VARID.

The bccmd may recognise the value of a "varid", but it may have been told not
to allow the variable to be accessed (from a given command source - security
reasons).  In this case it will also reply with a "status" of
BCCMDPDU_STAT_NO_SUCH_VARID.

Other values of the "status" field should be self-evident.  The "status"
value BCCMDPDU_STAT_ERROR is a catch-all - provided when none of the other
codes fits the situation.

If this protocol is extended to provide a GETNEXT-REQ pdu then it is likely
that a "vartype" field, describing the varid's type, will be added to
BCCMDPDU.  (A "vartype" field was provided in the first draft of this
definition.)

(Although it would be natural to use enums to describes some of the elements
of BCCMDPDU, we cannot do so as they are really ints, and ints have different
sizes and representations on different machines.) */

typedef struct {
	/* PDU type. */
	uint16		type;

/* Acceptable values for "type". */
#define	BCCMDPDU_GETREQ		(0)
#define	BCCMDPDU_GETRESP	(1)
#define	BCCMDPDU_SETREQ		(2)
/* #define BCCMDPDU_GETNEXTREQ	(3)  may be added. */

	/* Total PDU length in uint16s. */
	uint16		pdulen;
        /* To get payload length, subtract BCCMDPDU_OVERHEAD_LEN (see below) */

/* Maximum total length of a PDU. */
#define	BCCMDPDU_MAXLEN		(64)

	/* Sequence number. */
	uint16		seqno;

	/* Identifier of variable to be accessed. */
	uint16		varid;

	/* How the bccmd managed to perform the operation. */
	uint16		status;

/* Acceptable values for "status". */
#define	BCCMDPDU_STAT_OK	(0)	/* Default.  No problem found. */
#define	BCCMDPDU_STAT_NO_SUCH_VARID (1)	/* varid not recognised. */
#define	BCCMDPDU_STAT_TOO_BIG	(2)	/* Data exceeded pdu/mem capacity. */
#define	BCCMDPDU_STAT_NO_VALUE  (3)	/* Variable has no value. */
#define	BCCMDPDU_STAT_BAD_PDU	(4)	/* Bad value(s) in PDU. */
#define	BCCMDPDU_STAT_NO_ACCESS	(5)	/* Value of varid inaccessible. */
#define	BCCMDPDU_STAT_READ_ONLY	(6)	/* Value of varid unwritable. */
#define	BCCMDPDU_STAT_WRITE_ONLY (7)	/* Value of varid unreadable. */
#define	BCCMDPDU_STAT_ERROR	(8)	/* The useless catch-all error. */
#define BCCMDPDU_STAT_PERMISSION_DENIED (9) /* Request not allowed. */
#define BCCMDPDU_STAT_TIMEOUT   (10)    /* Deferred command timed-out. */


/***************************************************************************
That's the end of the common header of a bccmdpdu.  This define gives its
length in the units used for pdulen.
***************************************************************************/

#define BCCMDPDU_OVERHEAD_LEN 5


/***************************************************************************
Now follows stuff that varies according to the settings of the "type" and
"varid" fields.
***************************************************************************/


        union {
		/* Useless placeholder.  Nothing is written into BCCMDPDU.d
		if "type" is BCCMDPDU_GETREQ or if "varid", stripped of its
		access control bits, is between BCCMDVARID_COMMANDS_START and
		(BCCMDVARID_UINT8_START-1). */

                uint16  	dummy;

		/* For use if "varid", stripped of its access control bits,
		is between BCCMDVARID_UINT8_START and
		(BCCMDVARID_UINT16_START-1). */

                uint8   	u8;

		/* For use if "varid", stripped of its access control bits,
		is between BCCMDVARID_UINT16_START and
		(BCCMDVARID_UINT32_START-1). */

                uint16  	u16;

		/* For use if "varid", stripped of its access control bits,
		is between BCCMDVARID_UINT32_START and
		(BCCMDVARID_SPECIAL_START-1). */

                uint32  	u32;

		/* If "varid", stripped of its access control bits, is
		BCCMDVARID_SPECIAL_START or above then one of a set of 
		data forms follows. */

		BCCMDPDU_MEMORY mem;	/* If "varid" is BCCMDVARID_MEMORY. */
		BCCMDPDU_PS	ps;	/* If "varid" is BCCMDVARID_PS. */
		BCCMDPDU_BUFFER bufmem; /* If "varid" is BCCMDVARID_BUFFER. */
		BCCMDPDU_RADIOTEST radiotest;  /* If "varid" is
					    BCCMDVARID_RADIOTEST. */
		BCCMDPDU_PSNEXT psn;	/* If "varid" is BCCMDVARID_PSNEXT. */
		BCCMDPDU_PSNEXT_ALL psna; /* If "varid" is
					    BCCMDVARID_PSNEXT_ALL. */
		BCCMDPDU_PSSIZE pss;	/* If "varid" is BCCMDVARID_PSSIZE. */
		BCCMDPDU_PSCLRS psc;	/* If "varid" is BCCMDVARID_PSCLRS. */
		BCCMDPDU_PS_MEMORY_TYPE psmt;/* If "varid" is
					    BCCMDVARID_PS_MEMORY_TYPE. */
		BCCMDPDU_ADCRES adcres; /* If "varid" is BCCMDVARID_ADCRES. */
		BCCMDPDU_CRYPTKEYLEN ckl; /* BCCMDVARID_CRYPTKEYLEN. */
		BCCMDPDU_PICONET_INSTANT pi; /* BCCMDVARID_PICONET_INSTANT. */
		BCCMDPDU_GET_CLR_EVT gce; /* BCCMDVARID_GET_CLR_EVT. */
		BCCMDPDU_GET_NEXT_BUILDDEF gnbd; /* If "varid" is
					    BCCMDVARID_GET_NEXT_BUILDDEF. */
	        /* The struct for BCCMDVARID_I2C_TRANSFER is larger than the
		minimum BCCMD PDU size, so is omitted from this union:
		    BCCMDPDU_I2C_TRANSFER i2c;
		*/
		BCCMDPDU_E2_DEVICE e2d; /* BCCMDVARID_E2_DEVICE. */
		BCCMDPDU_E2_APP_DATA e2ad; /* BCCMDVARID_E2_APP_DATA. */
		BCCMDPDU_ENABLE_AFH ea; /* BCCMDVARID_ENABLE_AFH. */
	        BCCMDPDU_L2CAP_CRC l2cc; /* BCCMDPDU_L2CAP_CRC */
                BCCMDPDU_KALDEBUG kaldebug;  /* BCCMDVARID_KALDEBUG */
                BCCMDPDU_VM_DEBUG vmdebug; /* BCCMDVARID_VM_DEBUG */
                /* The struct for BCCMDVARID_KALEXTADDRDEBUG pushes the size
                of the BCCMDPDU above 9 words, and results with the
                compile-time assertion in bccmd.c (B-13242). Since the minimum 
                length check in bccmd_process_command() is based on the
                sizeof(BCCMDPDU) rather than the 9 which the spec refers, it
                could cause the firmware to start rejecting the PDUs which is
                actually valid in specs. Hence this assert was introduced. 
                The conventional way to solve this was to not include in the 
                PDU in the union, and explicitly cast and range check on the 
                firmware side. See B-70164 for more details. 
                BCCMDPDU_KALEXTADDRDEBUG kalextaddrdebug */
/* Red-M Change Starts */
		BCCMDPDU_BER_THRESHOLD bert;	/* BCCMDVARID_BER_THRESHOLD */
/* Red-M Change Ends */
		BCCMDPDU_ENABLE_MEDIUM_RATE emr; /* BCCMDVARID_MEDIUM_RATE. */
		BCCMDPDU_BUILD_NAME bn; /* BCCMDVARID_BUILD_NAME */
                BCCMDPDU_RSSI_ACL rssi_acl;     /* BCCMDVARID_RSSI_ACL */
                BCCMDPDU_ESCOTEST esco_test;    /* BCCMDVARID_ESCOTEST */
	        BCCMDPDU_BC3PSU bc3psu;     /* Only to support legacy api */
		BCCMDPDU_LIMIT_EDR_POWER limit_edr_power; /* BCCMDVARID_LIMIT_EDR_POWER */
		BCCMDPDU_LC_RX_FRACS lc_rx_fracs; /* BCCMDVARID_LC_RX_FRACS */
                /* The structure for sniff sub-rating information is too big,
                 * so is left out of the union.
                BCCMDPDU_SNIFF_SUB_RATE ssr;
                */
                BCCMDPDU_FM_REG fm_reg; /* BCCMDVARID_FM_REG */
                BCCMDPDU_FM_RDS fm_rds; /* BCCMDVARID_FM_RDS */
                /* The structure for pcm_rate_and_route is too big,
                 * so is left out of the union.
                BCCMDPDU_PCM_RATE_AND_ROUTE pcm_rate_and_route;
                */
                BCCMDPDU_PCM_CONFIG32 pcm_config32;
                                       /* BCCMDVARID_PCM_CONFIG32 */
                BCCMDPDU_PCM_LOW_JITTER pcm_low_jitter;
                                       /* BCCMDVARID_PCM_LOW_JITTER */
                BCCMDPDU_DIGITAL_AUDIO_RATE digital_audio_rate;
                                       /* BCCMDVARID_DIGITAL_AUDIO_RATE */
                BCCMDPDU_DIGITAL_AUDIO_CONFIG digital_audio_config;
                                       /* BCCMDVARID_DIGITAL_AUDIO_CONFIG */
                BCCMDPDU_AWAY_TIME      away_time;
                                       /* away time structure for external time out check */
                BCCMDPDU_SHA256_DATA sha256_data;                      
                                       /* BCCMDVARID_SHA256_DATA */
                /* The structure for sha256_res is too big,  
                 * so is left out of the union. (BCCMDVARID_SHA256_RES)
                BCCMDPDU_SHA256_RES sha256_res;
                */
                BCCMDPDU_COMBO_DOT11_ESCO_RTX_PRIORITY combo_dot11_esco_rtx_priority;
		
		BCCMDPDU_HQ_SCRAPING hq_scraping; /* BCCMDVARID_HQ_SCRAPING*/
		BCCMDPDU_HQ_SCRAPING_ENABLE hq_scraping_enable; 
                                            /* BCCMDVARID_HQ_SCRAPI|NG_ENABLE*/
	        BCCMDPDU_PANIC_ON_FAULT panic_on_fault;
                                            /*BCCMDVARID_PANIC_ON_FAULT*/ 
                /* Mic bias control structure for BC5-MM */
                BCCMDPDU_MIC_BIAS_CTRL  mic_bias_ctrl;  	    
                BCCMDPDU_MIC_BIAS_CONFIGURE mic_bias_configure;
                BCCMDPDU_WLAN_COEX_PRIORITY wlan_coex_priority;
                
                BCCMDPDU_FMTX_REG fmtx_reg; /* BCCMDVARID_FMTX_REG */ 
                /* The structure for FM_TEXT is too big,  */
                /* BCCMDPDU_FMTX_TEXT fmtx_text;  BCCMDVARID_FMTX_TEXT */                      
                BCCMDPDU_RANDOM random;     /* BCCMDVARID_RANDOM */
                /* BCCMDPDU_FASTPIPE_ENABLE is too big */
                /* BCCMDPDU_FASTPIPE_CREATE is too big */
                BCCMDPDU_FASTPIPE_DESTROY fastpipe_destroy;
                /* BCCMDPDU_FASTPIPE_RESIZE fastpipe_resize is too big */
                BCCMDPDU_ALLOCATE_RAM_RESERVE  allocate_ram_reserve; 
                                          /* BCCMDVARID_ALLOCATE_RAM_RESERVE*/
                BCCMDPDU_RECLAIM_RAM_RESERVE  reclaim_ram_reserve; 
                                          /* BCCMDVARID_RECLAIM_RAM_RESERVE*/
                BCCMDPDU_COEX_INT_TEST coex_int_test;
                                            /*BCCMDVARID_COEX_INT_TEST*/
                BCCMDPDU_GPS_START gps_start;
                BCCMDPDU_GPS_STOP gps_stop;
                BCCMDPDU_VM_STATUS vm_status;
                BCCMDPDU_EGPS_PULSE_CONFIG egps_pulse_config;
                BCCMDPDU_EGPS_PULSE_INPUT_START egps_pulse_input_start;
                /* BCCMDPDU_EGPS_PULSE_OUTPUT_START is too big */
                BCCMDPDU_EGPS_PULSE_STOP egps_pulse_stop;
                BCCMDPDU_SCO_PARAMETERS sco_parameters;
                /* BCCMDVARID_SCO_PARAMETERS */                

                BCCMDPDU_COASTER_START        coaster_start   ;
                BCCMDPDU_COASTER_STOP         coaster_stop    ;
                /* BCCMDPDU_COASTER_ADD_SV    is too big */
                BCCMDPDU_COASTER_DEL_SV       coaster_del_sv  ;
                /* BCCMDPDU_COASTER_SV_STATE  is too big */
                BCCMDPDU_COASTER_INDICATIONS  coaster_indications;
                /* BCCMDPDU_RDF_DATA_CAPTURE_ENABLE is too big */
                /* Two separate structures are defined for the codec input
                           * and output inorder to generate the mapping 
                           * between BCCMDPDU and BCCMDVARID while 
                           * building the btcli */
                BCCMDPDU_CODEC_INPUT_GAIN    codec_input_gain;
                BCCMDPDU_CODEC_OUTPUT_GAIN   codec_output_gain;

                BCCMDPDU_STREAM_GET_SOURCE    stream_get_source;
                BCCMDPDU_STREAM_GET_SINK      stream_get_sink;
                BCCMDPDU_STREAM_CONFIGURE     stream_configure;
                BCCMDPDU_STREAM_CONNECT       stream_connect;
                BCCMDPDU_STREAM_ALIAS_SINK    stream_alias_sink;
                BCCMDPDU_STREAM_SYNC_SID      stream_sync_sid;
                /* BCCMDPDU_COEX_DUMP         coex_dump */
                /* BCCMDPDU_RDF_BLE_PACKET_MATCH is too big */
                /* BCCMDPDU_RDF_BLE_ADVERTISING is too big */
                /* BCCMDPDU_RDF_BLE_RAND_ADDR is too big */
                BCCMDPDU_RDF_BLE_SCAN_MODE    rdf_ble_scan;
                BCCMDPDU_MAP_SCO_AUDIO    map_sco_audio;
                BCCMDPDU_MAP_SCO_WBS    map_sco_wbs;
                BCCMDPDU_RDF_PT rdf_pt;
	        BCCMDPDU_INFORM_LINK_PROFILES ilp;
                BCCMDPDU_CREATE_OPERATOR_C create_operator_c;
                BCCMDPDU_CREATE_OPERATOR_P create_operator_p;
                BCCMDPDU_START_OPERATOR start_operator;
                BCCMDPDU_STOP_OPERATOR stop_operator;
                BCCMDPDU_RESET_OPERATOR reset_operator;
                BCCMDPDU_DESTROY_OPERATOR destroy_operator;
                BCCMDPDU_OPERATOR_MESSAGE operator_message;
                BCCMDPDU_PREVIN_TUNNEL previn_tunnel;
                BCCMDPDU_DSPMANAGER_DEBUG dspmanager_debug;
                BCCMDPDU_CLOCK_MODULATION_CONFIG    clock_modulation_config;
                BCCMDPDU_PIO32_SET_PIN_FUNCTION   pio32_set_pin_function;
                BCCMDPDU_LED_CONFIG led_config;
                
                /* BCCMDVARID_CHARGER_TRIMS is too long
                BCCMDPDU_CHARGER_TRIMS chrg_trims ; */
                
                /* BCCMDPDU_PIO32 pio32_setreq; is too big */

                /* BCCMDPDU_BLE_ADV_FILT_ADD ble_adv_filt_add; is too big */
                BCCMDPDU_BLE_RADIO_TEST         ble_radio_test;
                /* BCCMDPDU_SIFLASH siflash; is too big */

                /* BCCMDPDU_CAPSENSE_RT_PADS is too long
                BCCMDPDU_CAPSENSE_RT_INIT cs_init_sel_pads; */
                BCCMDPDU_PCM_SLOTS_PER_FRAME    pcm_slots_per_frame;
                BCCMDPDU_PCM_SYNC_RATE          pcm_sync_rate;
                BCCMDPDU_PCM_CLOCK_RATE         pcm_clock_rate;
                /* BCCMDPDU_SPI_LOCK_CUSTOMER_KEY is too big */
				

		} d;
	} BCCMDPDU;


/***************************************************************************/

/* The varids are listed in blocks.  Each block holds variables of a given
type.  This allows bccmd to determine a variable's type by seeing which
range it falls in.

Commands from the host, received via the bcsp/hci_extn private channel, are
subject to some restrictions.  These bitmasks/offsets apply to varids to
restrain the host's ambitions. */

#define BCCMDVARID_READABLE     0x2000
#define BCCMDVARID_WRITABLE     0x4000

#define BCCMDVARID_NOACCESS	0
#define BCCMDVARID_READONLY	BCCMDVARID_READABLE
#define BCCMDVARID_WRITEONLY	BCCMDVARID_WRITABLE
#define BCCMDVARID_READWRITE	(BCCMDVARID_READABLE | BCCMDVARID_WRITABLE)


/***************************************************************************/
/* COMMAND VARIDS (variables which invoke commands when written to. */

/* Marker of start of the command varid block. */
#define	BCCMDVARID_COMMANDS_START (0)

/* Dummy variable.  No value written or read.  No command action performed. */
#define	BCCMDVARID_NO_VARIABLE	(BCCMDVARID_COMMANDS_START+ \
					BCCMDVARID_READWRITE+0)

/* bc01's best attempt at imitating a hardware reset. */
#define	BCCMDVARID_COLD_RESET	(BCCMDVARID_COMMANDS_START+ \
					BCCMDVARID_WRITEONLY+1)

/* Restart software, but attempt to preserve some values from existing state. */
#define	BCCMDVARID_WARM_RESET	(BCCMDVARID_COMMANDS_START+ \
					BCCMDVARID_WRITEONLY+2)

/* bc01's best attempt at imitating a hardware halt.  This currently
doesn't work (insufficiently strong sleeping pills). */
#define	BCCMDVARID_COLD_HALT	(BCCMDVARID_COMMANDS_START+ \
					BCCMDVARID_WRITEONLY+3)

/* Halt software, but attempt to preserve some values from existing state.
This currently doesn't work (insufficiently strong sleeping pills). */
#define	BCCMDVARID_WARM_HALT	(BCCMDVARID_COMMANDS_START+ \
					BCCMDVARID_WRITEONLY+4)

/* Initialise the bc01's Bluetooth stack.  (Start stack's tasks.) */
/* Obsolete command. */
#define	BCCMDVARID_INIT_BT_STACK (BCCMDVARID_COMMANDS_START+ \
					BCCMDVARID_WRITEONLY+5)

/* Activate the bc01's Bluetooth stack.  (Controlled init sequencing.) */
/* Obsolete command. */
#define	BCCMDVARID_ACTIVATE_BT_STACK (BCCMDVARID_COMMANDS_START+ \
					BCCMDVARID_WRITEONLY+6)

/* Enable the radio to transmit. */
#define	BCCMDVARID_ENABLE_TX	(BCCMDVARID_COMMANDS_START+ \
					BCCMDVARID_WRITEONLY+7)

/* Prevent the radio from transmitting. */
#define	BCCMDVARID_DISABLE_TX	(BCCMDVARID_COMMANDS_START+ \
					BCCMDVARID_WRITEONLY+8)

/* Force recalibration of the synthesiser's look up table.  This may take a
significant time (2 seconds?).

If the chip's boot time calibration fails then a message will have been sent
to the host via the hq channel.  The host might then reasonably invoke this
command to force a further attempt at recalibration.  Until the synth has
been calibrated successfully the lc prevents traffic travelling over the
radio.

Once the synth has successfully been calibrated any attempt to recalibrate
will probably destroy any active radio communcation and cause the calibration
either to fail or to scramble the look up table - either way, all future
radio communication will be unlikely to succeed.  The only safe way out of
such a mess will be to reboot.

Returns status of BCCMDPDU_STAT_ERROR if the calibration fails. */
/* Obsolete command. */
#define	BCCMDVARID_RECAL	(BCCMDVARID_COMMANDS_START+ \
					BCCMDVARID_WRITEONLY+9)

/* Remove almost all values from the bc01's persistent store (ps). */
/* This command is obsolete - it is no longer supported. */
/* #define	BCCMDVARID_PSCLRMOST   	     (BCCMDVARID_COMMANDS_START+ \
					BCCMDVARID_WRITEONLY+10) */

/* Remove all values from the bc01's persistent store (ps).  This varid is
deprecated - use BCCMDVARID_PSCLRALLS instead. */
/* Obsolete command. */
#define	BCCMDVARID_PSCLRALL    	     (BCCMDVARID_COMMANDS_START+ \
					BCCMDVARID_NOACCESS+11)

/* Take a snapshot of the values in the ps. */
#define	BCCMDVARID_PS_FACTORY_SET    (BCCMDVARID_COMMANDS_START+ \
					BCCMDVARID_NOACCESS+12)

/* Restore all ps values from the last BCCMDVARID_PS_FACTORY_SET command. */
#define	BCCMDVARID_PS_FACTORY_RESTORE  (BCCMDVARID_COMMANDS_START+ \
					BCCMDVARID_WRITEONLY+13)

/* As BCCMDVARID_PS_FACTORY_RESTORE, but remove all other ps values. */
#define	BCCMDVARID_PS_FACTORY_RESTORE_ALL (BCCMDVARID_COMMANDS_START+ \
					BCCMDVARID_WRITEONLY+14)

/* Arrange that next reboot will perform ps defrag, then warm reboot. */
#define	BCCMDVARID_DEFRAG_RESET	(BCCMDVARID_COMMANDS_START+ \
					BCCMDVARID_WRITEONLY+15)

/* Stop whatever the VM is doing, if anything; it is not restartable. */
#define BCCMDVARID_KILL_VM_APPLICATION (BCCMDVARID_COMMANDS_START+ \
					BCCMDVARID_WRITEONLY+16)

/* Turn hopping back on.  This is only useful after using
BCCMDVARID_SINGLE_CHAN to enable transmit and receive on a single channel. */

#define BCCMDVARID_HOPPING_ON (BCCMDVARID_COMMANDS_START+ \
			       BCCMDVARID_WRITEONLY+17)

/* Cancel any page the lc happens to be doing.  If it isn't, don't care. */
/* Obsolete command. */
#define BCCMDVARID_CANCEL_PAGE (BCCMDVARID_COMMANDS_START+ \
				BCCMDVARID_WRITEONLY+18)

/*
 * Arrange to test system RAM.  The test is destructive and this causes
 * a system reset.  See the description for BCCMDVARID_CHECK_RAM_RESULT
 * for more details.
 *
 * May not be available on all builds; if not available, getting or setting
 * will return BCCMD_NO_SUCH_VARID.
 */
#define BCCMDVARID_CHECK_RAM (BCCMDVARID_COMMANDS_START+ \
			      BCCMDVARID_WRITEONLY+19)

/*
 * From chips starting with bc02-kato only, enable UART bypass mode.
 * This redirects the UART I/O lines to PIO pins 4 to 7 inclusive.
 * The assumption is that in fact no UART is connected to these pins
 * and the chip will enter deep sleep until a reset is forced by
 * hardware.  This takes immediate effect so no response will be received.
 */
#define BCCMDVARID_BYPASS_UART (BCCMDVARID_COMMANDS_START+ \
				BCCMDVARID_WRITEONLY+20)

/*
 * Synchronise the internal clock to correspond to a remote master.
 * Has no effect if there is no remote master.  This does not store
 * the result, which lasts until the next reset.  The value set can
 * be retrieved via BCCMDVARID_ANA_FTRIM; this is suitable for storing in
 * PSKEY_ANA_FTRIM.  Obviously, how useful the value is depends on the
 * accuracy of the remote master's clock.
 *
 * The command returns immediately with a successful status indication
 * if there is a remote master, but the operation takes several
 * seconds after that, up to a maximum of roughly 5.  No further status
 * indication is returned; examining changes to BCCMDVARID_ANA_FTRIM is
 * the only way of testing the effect of the command.
 */
#define BCCMDVARID_SYNC_CLOCK  (BCCMDVARID_COMMANDS_START+ \
				  BCCMDVARID_WRITEONLY+21)

/*
 * Enable Device Under Test Mode.
 * Do just the same as the HCI command of the same name.
 */
#define BCCMDVARID_ENABLE_DUT_MODE (BCCMDVARID_COMMANDS_START+ \
				    BCCMDVARID_WRITEONLY+22)
/*
Babel commands.

The babel hardware is a USB to SPI converter.  It's based around a BlueCore,
with PIO lines driving the SPI as Master.

In order to get the Babel's BlueCore to execute the spi transactions, we
have these BCCMDs.  They basically encapsulate the spifns and spicommon APIs,
so we have such things as block read and write, verified operations, and
some stop and go calls.

Eventually we will implement the interface between the Host and the Babel's
BlueCore in terms of a USB mass storage device.

*/


/*
The following Babel commands are executed as standard commands.
They return the success of the operation using
BCCMDPDU_STAT_ERROR or BCCMDPDU_STAT_OK.
*/
#define BCCMDVARID_BABEL_SPI_RESET_AND_GO   (BCCMDVARID_COMMANDS_START+ \
                                        BCCMDVARID_WRITEONLY + 23)

#define BCCMDVARID_BABEL_SPI_RESET_AND_STOP (BCCMDVARID_COMMANDS_START+ \
                                        BCCMDVARID_WRITEONLY + 24)

#define BCCMDVARID_BABEL_SPI_GO             (BCCMDVARID_COMMANDS_START+ \
                                        BCCMDVARID_WRITEONLY + 25)

#define BCCMDVARID_BABEL_SPI_STOP           (BCCMDVARID_COMMANDS_START+ \
                                        BCCMDVARID_WRITEONLY + 26)

/* 
   On chips prior to BC7 this write-only command is used to force
   the clock signal to be output on AIO[1] instead of AIO[0].
   Once the clock routing has been changed to AIO[1] it cannot be
   reset to AIO[0] without resetting the chip.
 */
#define BCCMDVARID_ROUTE_CLOCK           (BCCMDVARID_COMMANDS_START+ \
                                        BCCMDVARID_WRITEONLY + 27)

/*
 * Store current set event filters in PS for fake host to reissue
 * when BlueCore is rebooted.
 */
#define BCCMDVARID_SAVE_CONNECTION_EVENT_FILTERS (BCCMDVARID_COMMANDS_START+ \
                                                  BCCMDVARID_WRITEONLY + 28)

#define BCCMDVARID_SHA256_INIT          (BCCMDVARID_COMMANDS_START+ \
                                        BCCMDVARID_WRITEONLY + 29)


/* Configure analog parts and digits for GPS */
#define BCCMDVARID_GPS_CONFIG          (BCCMDVARID_COMMANDS_START+ \
                                        BCCMDVARID_WRITEONLY + 30)

/*
 * Set the device to be connectable through BCCMD for DUT mode test
 */
#define BCCMDVARID_ENABLE_DEV_CONNECT (BCCMDVARID_COMMANDS_START+ \
                                          BCCMDVARID_WRITEONLY + 31)

/* BCCMD for stream transaction start */
#define BCCMDVARID_STREAM_TRANSACTION_START  (BCCMDVARID_COMMANDS_START+ \
                                      BCCMDVARID_WRITEONLY + 32)

/* BCCMD for stream transaction complete*/
#define BCCMDVARID_STREAM_TRANSACTION_COMPLETE  (BCCMDVARID_COMMANDS_START+ \
                                      BCCMDVARID_WRITEONLY + 33)

/* BCCMD for stream transaction cancel*/
#define BCCMDVARID_STREAM_TRANSACTION_CANCEL  (BCCMDVARID_COMMANDS_START+ \
                                      BCCMDVARID_WRITEONLY + 34)

/* BCCMD for SPI DSP Debug : to set a flag in the preserve block 
   to facilitate the debugger to set breakpoints on subsequent warm_reset*/
#define BCCMDVARID_KALSPIDEBUG_SETDEBUGMODE  (BCCMDVARID_COMMANDS_START+ \
                                      BCCMDVARID_WRITEONLY + 35)

/* BCCMDs for Stibbons (DSPmanager).
 * It's not yet known exactly what commands there will be, so for
 * now a block of 16 values are reserved and will be filled in as
 * the code is integrated back to mainline. When the work is
 * finished the remaining values will be relinquished.
 */

#define BCCMDVARID_STIBBONS_C01 (BCCMDVARID_COMMANDS_START+ \
                                      BCCMDVARID_NOACCESS +  36)
#define BCCMDVARID_STIBBONS_C02 (BCCMDVARID_COMMANDS_START+ \
                                      BCCMDVARID_NOACCESS +  37)
#define BCCMDVARID_STIBBONS_C03 (BCCMDVARID_COMMANDS_START+ \
                                      BCCMDVARID_NOACCESS +  38)
#define BCCMDVARID_STIBBONS_C04 (BCCMDVARID_COMMANDS_START+ \
                                      BCCMDVARID_NOACCESS +  39)
#define BCCMDVARID_STIBBONS_C05 (BCCMDVARID_COMMANDS_START+ \
                                      BCCMDVARID_NOACCESS +  40)
#define BCCMDVARID_STIBBONS_C06 (BCCMDVARID_COMMANDS_START+ \
                                      BCCMDVARID_NOACCESS +  41)
#define BCCMDVARID_STIBBONS_C07 (BCCMDVARID_COMMANDS_START+ \
                                      BCCMDVARID_NOACCESS +  42)
#define BCCMDVARID_STIBBONS_C08 (BCCMDVARID_COMMANDS_START+ \
                                      BCCMDVARID_NOACCESS +  43)
#define BCCMDVARID_STIBBONS_C09 (BCCMDVARID_COMMANDS_START+ \
                                      BCCMDVARID_NOACCESS +  44)
#define BCCMDVARID_STIBBONS_C10 (BCCMDVARID_COMMANDS_START+ \
                                      BCCMDVARID_NOACCESS +  45)
#define BCCMDVARID_STIBBONS_C11 (BCCMDVARID_COMMANDS_START+ \
                                      BCCMDVARID_NOACCESS +  46)
#define BCCMDVARID_STIBBONS_C12 (BCCMDVARID_COMMANDS_START+ \
                                      BCCMDVARID_NOACCESS +  47)
#define BCCMDVARID_STIBBONS_C13 (BCCMDVARID_COMMANDS_START+ \
                                      BCCMDVARID_NOACCESS +  48)
#define BCCMDVARID_STIBBONS_C14 (BCCMDVARID_COMMANDS_START+ \
                                      BCCMDVARID_NOACCESS +  49)
#define BCCMDVARID_STIBBONS_C15 (BCCMDVARID_COMMANDS_START+ \
                                      BCCMDVARID_NOACCESS +  50)
#define BCCMDVARID_STIBBONS_C16 (BCCMDVARID_COMMANDS_START+ \
                                      BCCMDVARID_NOACCESS +  51)

/* BLE advertising report filter. BTSW-623
 * When an advertising report filter is set, the LM will discard
 * BLE advertising reports that do not match the filter. With no
 * filter set, the LM passes all advertising reports to the host.
 *
 * The following will clear the advertitising filter.
 */
#define BCCMDVARID_BLE_ADV_FILT_CLEAR (BCCMDVARID_COMMANDS_START+ \
                                      BCCMDVARID_NOACCESS +52)

/* BCCMD to lock SPI Port. Once set SPI is locked on next reset.
 * There is no equivalent unlock BCCMD. If for our internal purpose
 * we want to unlock SPI then that will be done by some other mean 
 * e.g., spi tools */
#define BCCMDVARID_SPI_LOCK_INITIATE_LOCK (BCCMDVARID_COMMANDS_START+ \
                                      BCCMDVARID_WRITEONLY +53)
/***************************************************************************/
/* Variables of type [u]int8.  The variable's value is in BCCMDPDU.d.u8. */

/* Marker of start of the [u]int8 varid block. */
#define	BCCMDVARID_UINT8_START (1024)

/* <empty list> */


/***************************************************************************/
/* Variables of type [u]int16.  The variable's value is in BCCMDPDU.d.u16. */

/* Marker of start of the [u]int16 varid block. */
#define	BCCMDVARID_UINT16_START (2048)


/* BC01 status. */
/* Obsolete command. */
#define	BCCMDVARID_BC01_STATUS	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+1)

/* UART configuration.  Writing this value has immediate effect and the
value is not written to the ps.  To make a permanent configuration change
write directly to the ps. */
#define BCCMDVARID_CONFIG_UART       (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READWRITE+2)

/* The value of the UART config variable is a bitfield, formed by ORing the
following values.  For example, to set one stop bit, 38.4 kbaud and even parity
the value is ( UART_BAUD_38400 | UART_ONE_STOP_BIT | UART_EVEN_PARITY ). */

#define UART_BAUD_9600               (0x0027)
#define UART_BAUD_19200              (0x004f)
#define UART_BAUD_38400              (0x009d)
#define UART_BAUD_57600              (0x00ec)
#define UART_BAUD_115200             (0x01d8)
#define UART_BAUD_921600             (0x0ebf)
#define UART_ONE_STOP_BIT    	     (0x0000)
#define UART_TWO_STOP_BITS   	     (0x2000)
#define UART_NO_PARITY               (0x0000)
#define UART_ODD_PARITY              (0x4000)
#define UART_EVEN_PARITY             (0xc000)

/* The bc01 holds a pair of analogue multiplexors (rotary switches) that
route the voltage from a point within the chip to one of a pair of external
test pins.  These varids select the switches' positions.  Acceptable
values 0->15. */

#define	BCCMDVARID_TESTA_AMUX	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READWRITE+3)
#define	BCCMDVARID_TESTB_AMUX	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READWRITE+4)

/* The argument to the last call made to panic().  Because panic() tears down
the machine in a hurry, the returned value may be suspect. */
#define BCCMDVARID_PANIC_ARG         (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READWRITE+5)

/* The argument to the last call made to fault().  This value may be
from before the current boot, so its value may be suspect. */
#define BCCMDVARID_FAULT_ARG         (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READWRITE+6)

/* The validity of the checksum over the preserve memory area as determined
by init_preserve_memory(), called during system start up.  This indicates the
integrity of BCCMDVARID_PANIC_ARG and BCCMDVARID_FAULT_ARG.  The value is
really a bool, so zero means failure. */

#define BCCMDVARID_PRESERVE_VALID    (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+7)


/* These ADC varids are no longer used - see below. */
#define	BCCMDVARID_ADC0	     	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+8)
#define	BCCMDVARID_ADC1	     	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+9)
#define	BCCMDVARID_ADC2	     	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+10)
#define	BCCMDVARID_ADC3	     	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+11)
#define	BCCMDVARID_ADC4	     	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+12)
#define	BCCMDVARID_ADC5	     	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+13)
#define	BCCMDVARID_ADC6	     	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+14)
#define	BCCMDVARID_ADC7	     	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+15)
#define	BCCMDVARID_ADC8	     	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+16)
#define	BCCMDVARID_ADC9	     	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+17)
#define	BCCMDVARID_ADC10     	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+18)
#define	BCCMDVARID_ADC11     	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+19)
#define	BCCMDVARID_ADC12     	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+20)
#define	BCCMDVARID_ADC13     	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+21)
#define	BCCMDVARID_ADC14     	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+22)
#define	BCCMDVARID_ADC15     	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+23)

/* Remove a value from the bc01's persistent store (ps).  The value is the
key (pskey) of the item to be removed, as defined in psbc_data.  This varid
is deprecated - use BCCMDVARID_PSCLRS instead. */
/* Obsolete command. */
#define	BCCMDVARID_PSCLR     	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_WRITEONLY+24)

/* Obtain the stack firmware's build identifier. */
#define	BCCMDVARID_BUILDID     	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+25)

/* BlueCore hardware's major hardware version. */
#define	BCCMDVARID_CHIPVER     	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+26)

/* BlueCore's hardware's minor hardware version. */
#define	BCCMDVARID_CHIPREV     	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+27)

/* Normal hci calls to add sco connections to acl links cause the sco data
stream to route over hci.  If this variable is set to "1" then the next hci
call to add a sco link will route the sco channel's data over the bc01's
first pcm interface rather than over hci.  bc01b has only one pcm interface,
but bc02 and later versions of the chip may have up to 3, so setting this
variable may route to pcm channel 2 or 3 in future.  Setting this variable to
zero clears a preceding request to route over a pcm channel.  */

#define	BCCMDVARID_MAP_SCO_PCM       (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_WRITEONLY+28)

/* This command is no longer supported ...  See bccmd_spi.h.

It is possible to pass commands to bccmd over spi, using a fixed-size ram
buffer in the bc01.  This variable obtains the buffer's size.   The size
limits the length of some bccmd commands' payloads. */

#define	BCCMDVARID_SPI_BUFSIZE 	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+29)

/* Sets whether the individual bits of the bc01's PIO port are used as inputs
or outputs.  The PIO is 16 bits wide.  If a bit is 1 then the
corresponding bit position of the chip's PIO port is set to be an
output, else it is set as an input.

This command must be used with care: chip code using the pio port probably
depends on knowledge of the bits' directions. */

#define	BCCMDVARID_PIO_DIRECTION_MASK 	     	(BCCMDVARID_UINT16_START+ \
						BCCMDVARID_READWRITE+30)
/* Old name. */
#define	BCCMDVARID_PIO_MASK			BCCMDVARID_PIO_DIRECTION_MASK

/* Access the value of the chip's 16-bit pio port.
Whether a pio bit is input (on read) or output (on write)
depends on the value of BCCMDVARID_PIO_MASK. */

#define	BCCMDVARID_PIO 	     		(BCCMDVARID_UINT16_START+ \
						BCCMDVARID_READWRITE+31)

/* Provoke a panic() call within the chip so causing an immediate shutdown
and reboot of the chip.  The value written to this varid is the value passed
to the panic() call.  This is provided for testing only and should not be
used as a normal mechanism for restarting the module. */

#define	BCCMDVARID_PROVOKE_PANIC 	(BCCMDVARID_UINT16_START+ \
						BCCMDVARID_WRITEONLY+32)

/* Provoke a delayed_panic() call within the chip so causing an shutdown and
reboot of the chip after a delay.  The value written to this varid is the
value passed to the delayed_panic() call.  This is provided for testing only
and should not be used as a normal mechanism for restarting the module. */

#define	BCCMDVARID_PROVOKE_DELAYED_PANIC (BCCMDVARID_UINT16_START+ \
						BCCMDVARID_WRITEONLY+33)

/* Provoke a fault() call within the chip.  This should cause the chip to
attempt to send a fault report back to the host.  The value written to this
varid is the value passed to the fault() call.  This is provided for testing
only. */

#define	BCCMDVARID_PROVOKE_FAULT 	(BCCMDVARID_UINT16_START+ \
						BCCMDVARID_WRITEONLY+34)

/* The chip's PIO port (confusingly called the "aux" io port by the hal and
below) is supposed to be a general purpose provision.  Alas, lots of bits of
hardware and software bag various pins for their own purposes, e.g. external
power amp control (hw) and usb support (firmware).  This effectively means
that for any given application/configuration only a subset of the 16 pins may
be available to an application.  This applies whether the application is on
chip or on the host.  In either case the application needs some sort
constraint on its attempts to change arbitrary pio pins.  For example, we
cannot allow the host to use bccmd commands to set and clear arbitrary pio
pins because this may interfere with, say, usb operation.  We certainly can't
block interrupts while a pair of bccmds perform a read-modify-write.

The low uint8 of this uint16 acts as a crude access control mask.  The bits
in the mask map to the corresponding bits in the pio port.  Each bit set to 1
in the mask cannot be set by bccmd/application. */

#define	BCCMDVARID_PIO_PROTECT_MASK	(BCCMDVARID_UINT16_START+ \
						BCCMDVARID_READONLY+35)

/* Old location/value for the pssize varid.  Now in the specials block.
#define	BCCMDVARID_PSSIZE     	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+36) */

/* The version number of the bccmd interface, as defined by the RCS version
number of the file that defines the interface (bccmdpdu.h).  This will change
when commands are added/changed/removed.  This will not change if the
implementation of a command changes, e.g. a bug fix.

Value 0 means the information is unavailable.

The code that reports the version of this file needs this RCS keyword:
$Revision: #3 $ */

#define	BCCMDVARID_INTERFACE_VERSION  (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+37)


/* The version number of the bccmdradiotest interface (a sub-element of the
bccmd interface), as defined by the RCS version number of the file that
defines the interface (bccmdradiotestpdu.h).  This will change when commands
are added/changed/removed.  This will not change if the implementation of a
command changes, e.g. a bug fix.

Value 0 means the information is unavailable.

The code that reports the version of this file needs the "Revision" RCS
keyword to be in file bccmdradiotestpdu.h. */

#define	BCCMDVARID_RADIOTEST_INTERFACE_VERSION  (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+38)


/* Set the maximum transmit power.  A RAM value is set from pskey
PSKEY_LC_MAX_TX_POWER when the system boots; this limits the maximum transmit
power of the radio.  This bccmd varid writes to this RAM value, allowing the
maximum transmit power to be altered after booting.

This limit is only referenced when the lc considers increasing the transmit
power, so if the transmit power on a link is currently above this level the
new value won't take effect until an attempt is made to increase the power.
Values as for the pskey - dBm. */

#define	BCCMDVARID_MAX_TX_POWER		(BCCMDVARID_UINT16_START+ \
					  BCCMDVARID_READWRITE+39)


/* Old location/value for the psnext varid.  Now in the specials block.
#define	BCCMDVARID_PSNEXT     	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+40) */


/* [Post B-33848, this VARID is obsolete; use ADC_READ instead]

The bc01 includes an ADC that can read voltages from points within the
chip.  In front of the ADC is a MUX (another rotary switch) that selects the
point to be monitored.  This varid and ADCRES are used to make ADC readings.

The interface is a little awkward - a consequence of the way the request is
scheduled within the firmware.  The ADC channel to be read is first written
to BCCMDVARID_ADC; this provokes the firmware to start to make the ADC
reading.  The result may subsequently be picked up by reading
BCCMDVARID_ADCRES.

We deliberately do not describe the ADC channels here as this reveals too
much of the innards of the radio, and this file has been known to leak
outside CSR.  The only channel that might legitimately interest host code is
channel 1 - this measures the chip's Vdd against an internal reference.  This
is used by the on-chip battery monitor. */

#define	BCCMDVARID_ADC     	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_WRITEONLY+41)


/* The bc01 uses a random number generator to support pairing, authentication
and encryption.  This varid obtains a single uint16 of random data from the
generator.

The bc01 uses a hardware mechanism to produce a slow trickle of random
bits.  These are collected in a store.  Requests for random numbers
take from this store.  If the rate of bit consumption exceeds generation
(roughly 500 bits/s) then reading fails. */

#define	BCCMDVARID_RAND     	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+42)


/* The pskey PSKEY_LC_DEFAULT_TX_POWER sets the device's default transmit
power, used for page, inquiry, and their responses, and as the initial power
for new acl links (before power control takes over).  The lc takes the value
from ps at system boot.

The value is normally constrained by the Bluetooth power class, though
under most circumstances it should be between 0 and 4 dBm.

This command provides allows the default transmit power to be adjusted.
The value is a signed int16, expressed in dBm.

The value is constrained by the unit's power table and maximum transmit
power value. */

#define	BCCMDVARID_DEFAULT_TX_POWER  (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READWRITE+43)

/* The maximum effective encryption key length is 16 bytes, limited by the
hardware and BT spec.  Some governments' anti-privacy demands have forced us
to add a smaller limit in some firmware builds.  This varid returns the
maximum effective encryption key length imposed by the firmware, measured
in bytes.   Note that the value of PSKEY_ENC_KEY_LMAX may further
undermine the user's security. */

#define	BCCMDVARID_MAXCRYPTKEYLEN    (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+44)

/* Remove all values from the bc01's persistent store (ps).  The value is
the stores to clear.  This is an extension of BCCMDVARID_PSCLRALL. */
#define	BCCMDVARID_PSCLRALLS         (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_NOACCESS+45)

/* Transmit and receive on a single channel.  Turns off hopping and
whitening. */
#define BCCMDVARID_SINGLE_CHAN	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_WRITEONLY+46)

/*
 * Return a 16-bit checksum of ROM contents.  This only works with real rom.
 * May not be available on all builds; if not available, getting or setting
 * will return BCCMD_NO_SUCH_VARID.
 */
#define BCCMDVARID_CHECK_ROM	     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+47)

/*
 * Return a 16-bit checksum of RAM contents.
 * As this trashes the entire contents of RAM it is handled in two
 * stages.  Writing to the variable BCCMDVARID_CHECK_RAM causes a
 * destructive RAM test to be made; the result of the test is then
 * saved, and a warm reset forced.  After the reset, reading from
 * the variable BCCMDVARID_CHECK_RAM_RESULT will return the result
 * of the test.
 *	0  Test succeeded
 *      1  Test failed at stage 1
 *      2  Test failed at stage 2
 *      3  Result of test was not valid.
 *
 * May not be available on all builds; if not available, getting or setting
 * will return BCCMD_NO_SUCH_VARID.
 */
#define BCCMDVARID_CHECK_RAM_RESULT     (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+49)

/*
 * Set or get the PCM gain.  This is a three bit number which uses
 * the top three bits of the PCM stream.  It is common to all PCM
 * slots.  A value outside the range 0 to 7 will be rejected.
 */
#define BCCMDVARID_PCM_ATTENUATION	(BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READWRITE+50)

/* Set or get the number of 16-bit words reserved for a VM application in
EEPROM. Setting the size erases any VM application that was present
previously. */
#define BCCMDVARID_E2_APP_SIZE       (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+51)

/* Two varids are used to perform an integrity check of the firmware:

BCCMDVARID_FIRMWARE_CHECK returns a bitfield indicating which components of
the firmware were successfully tested, i.e. for which the checksum is
correct.

BCCMDVARID_FIRMWARE_CHECK_MASK returns a bitfield indicating which components
of the firmware can be tested, i.e. that the components are present and
contain the information required to verify the checksum.

If the two varids return the same value then all of the checks that could
be performed resulted in the expected checksum, i.e. no errors were
detected.

The bitfield returned by both varids has the following bits defined:
    bit 0 - Stack
    bit 1 - Loader
    bit 2 - VM application
    bit 3 - FileSystem
*/

#define BCCMDVARID_FIRMWARE_CHECK       (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+52)

#define BCCMDVARID_FIRMWARE_CHECK_MASK   (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+53)

/* The version of the analogue hardware on the BlueCore chip */

#define BCCMDVARID_CHIPANAVER	(BCCMDVARID_UINT16_START+ \
				   BCCMDVARID_READONLY+54)

/*
 * Return the internal value corresponding to the configuration setting
 * PSKEY_ANA_FTRIM.  The value will usually be the same but may have
 * been altered by BCCMDVARID_SYNC_CLOCK.  In that case, the value
 * read is suitable for storing in PSKEY_ANA_FTRIM; the bccmd command
 * does not do this.
 *
 * For builds without radiotest, we also need to be able to set
 * this value.  Unfortunately this needs an additional PS key as
 * the permissions are part of the name.  That key is
 * BCCMDVARID_ANA_FTRIM_READWRITE.
 */
#define BCCMDVARID_ANA_FTRIM	(BCCMDVARID_UINT16_START+ \
				   BCCMDVARID_READONLY+55)

/* Obtain the loader firmware's build identifier. */
#define	BCCMDVARID_BUILDID_LOADER    (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READONLY+56)

/*
For hardware starting with Kato, it is possible for a PIO used as input
to be configured with a weak bias (the default), or a strong bias.  A
strong bias is a constant current source of a few tens of microamps.
This value sets or returns the register
*/
#define BCCMDVARID_PIO_STRONG_BIAS	(BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READWRITE+57)

/*
We desire to be able to read the current bootmode via bccmd.  More than
this, we wish to be able to boot into a new mode at the behest of the host.
Note that a bootmode SET_REQUEST may well not generate a GET_RESPONSE
since it will trigger an immediate warm reset
*/

#define BCCMDVARID_BOOTMODE	        (BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READWRITE+58)

/*
Determine how we handle a pathology in the specification with data
transfer when we are in a scatternet with one HV3 link (either as master
or slave) and a second link on which we are slave.  We need to override
the SCO link to receive an acknowledgement from the master of the second
link otherwise data transfer to the master will stall.  If this is zero,
we only do this for control packets, since allowing bulk L2CAP data
has a severe impact on the SCO link.  With this set to TRUE, we will
allow ACL links to override the SCO link in this case.

This only applies to ACL links on which we are slave, other than
the link with the HV3 connection.

The default state of this is now 1, i.e. the feature is active.
*/

#define BCCMDVARID_SCATTERNET_OVERRIDE_SCO	(BCCMDVARID_UINT16_START+ \
						BCCMDVARID_READWRITE+59)

/*
This is a debug PDU to lock all ACL transmissions to the same packet type.
It requires FIX_PACKET_TYPE to be defined to work. By default, that's not
defined. Also, the wiring is not yet present on the main line.
*/

#define BCCMDVARID_LOCK_ACL_PACKET_TYPE    (BCCMDVARID_UINT16_START+ \
                                            BCCMDVARID_WRITEONLY+60)

/* a uint16 operation
 * The shift delay is in BCCMDPDU.d.u16
 */
#define BCCMDVARID_BABEL_SPI_DELAY     (BCCMDVARID_UINT16_START+ \
                                        BCCMDVARID_READWRITE + 61)

/* a uint16 read only operation.
 * A spi_run_state_t will be returned in BCCMDPDU.d.u16
 */
#define BCCMDVARID_BABEL_SPI_RUN_STATE (BCCMDVARID_UINT16_START+ \
                                        BCCMDVARID_READONLY + 62)

/* See description of BCCMDVARID_ANA_FTRIM */

#define BCCMDVARID_ANA_FTRIM_READWRITE	(BCCMDVARID_UINT16_START+ \
				   BCCMDVARID_READWRITE+63)
/*
 * If 1 (the default), calculations of Dsniff in sniff mode on the
 * master of a piconet take account of the possibility that a remote
 * device may send multislot packets.  This uses available bandwidth
 * less efficiently if the sniff interval is small (as used typically by
 * HID devices) and in fact the devices will only be sending single slot
 * packets.  If the value is 0, Dsniff may be set so that the devices
 * can be in consecutive slot paris, even if they are configured to use
 * multislot packet.
 *
 * In detail, this has the following effect on calculations of Dsniff:
 *   - If the value is 1, devices in sniff mode will be spaced out
 *     to allow multislot exchanges.  Up to 10 slots are required.
 *     This is useful for maximizing bandwidth to the sniff links,
 *     but gives poor use of the remaining bandwidth if Tsniff is
 *     small, since traffic to other devices must be scheduled
 *     into the short spaces and hence cannot make the best use
 *     of multislot packets.
 *   - If the value is 0, the scheduler will do its best to ensure
 *     that sniff slots are scheduled adjacent to other sniff slots or
 *     SCO slots.  The value of sniff_attempts is taken into account.
 *     For example, if sniff_attempts is 2 (i.e. 4 slots) then the
 *     scheduler will try to arrange a new sniff event to start 4 slots
 *     after the start slot for this event.  If the link has guaranteed
 *     quality of service, then sniff_timeout is also taken into account
 *     and is added to sniff_attempt in calculating the optimum offset
 *     to the next sniff start slot.
 *
 * This command is not likely to be useful if sniff is only in
 * use for power saving.
 */
#define BCCMDVARID_SNIFF_MULTISLOT_ENABLE (BCCMDVARID_UINT16_START+ \
				    BCCMDVARID_READWRITE+64)

/* Command to retrieve rx_fracs Packet Error Rate information */
/* 
   This command is used in conjunction with BCCMDVARID_LC_RX_FRACS.
   It returns the size of the array used with BCCMDVARID_LC_RX_FRACS.
   Currently the only value returned is 16, stating that the
   LC_RX_FRACS table contains 16 entries. 
 

*/


#define BCCMDVARID_LC_FRAC_COUNT        (BCCMDVARID_UINT16_START+ \
                                     BCCMDVARID_READONLY + 65)

/* a uint16 read only operation.
 * returns a bitmap showing state of the SPI lines when each
 * SPI line is driven high.  The host can use this bitmap to determine if any
 * of the SPI output lines are shorted to ground, Vdd or another SPI line.
 *
 * Bit:            11   10    9    8    7    6    5    4    3    2    1    0
 * Driven Line:       <- MOSI Hi  ->      <-  CLK hi  ->      <-  CSB hi  ->
 *              +----+----+----+----+----+----+----+----+----+----+----+----+
 * State:       |    |MOSI| CSB| CLK|    |MOSI| CSB| CLK|    |MOSI| CSB| CLK|
 *              +----+----+----+----+----+----+----+----+----+----+----+----+
 */
#define BCCMDVARID_BABEL_SPI_GET_LINE_STATUS (BCCMDVARID_UINT16_START+ \
                                              BCCMDVARID_READONLY + 66)


/* Route the external 32KHz clock thorugh AIO0 and read its value.
 * The value returned is in units of 0.25us and represents the
 * clock period. Note that the clock has already been divided to
 * be a 1KHz clock, so the expected value is 4000, not 125.
 */

#define BCCMDVARID_EXT_CLOCK_PERIOD (BCCMDVARID_UINT16_START+ \
                                     BCCMDVARID_READONLY + 67)

/*
 * Set the value of host_decides_timing, which is set initially by
 * PSKEY_HOST_DECIDES_TIMING
 */
#define BCCMDVARID_HOST_DECIDES_TIMING	(BCCMDVARID_UINT16_START+ \
					BCCMDVARID_WRITEONLY+68)

/* uint16 write only operation to enable HOSTIO debug information.
 * bits assignments:
 * 	bit 0	- H4 resync debug (B-6406)
 *	All others unassigned.
 */
#define BCCMDVARID_HOSTIO_ENABLE_DEBUG (BCCMDVARID_UINT16_START+ \
                                        BCCMDVARID_WRITEONLY + 69)


/* Set the trim value for the charger voltage in 3.8..4.5V
   range in 50mV steps. */
#define BCCMDVARID_CHARGER_PSU_TRIM            (BCCMDVARID_UINT16_START+ \
                                                 BCCMDVARID_WRITEONLY + 71)

/* Enable/disable the switch mode PSU SMPS regulator. */
#define BCCMDVARID_PSU_SMPS_ENABLE               (BCCMDVARID_UINT16_START+ \
                                                 BCCMDVARID_WRITEONLY + 72)

/* Enable/disable the high voltage linear-regulator */
#define BCCMDVARID_PSU_HV_LINEAR_ENABLE          (BCCMDVARID_UINT16_START+ \
                                                 BCCMDVARID_WRITEONLY + 73)

/* Turn LED0 on/off */
#define BCCMDVARID_LED0_ENABLE                   (BCCMDVARID_UINT16_START+ \
                                                 BCCMDVARID_WRITEONLY + 74)

/* Turn LED1 on/off */
#define BCCMDVARID_LED1_ENABLE                   (BCCMDVARID_UINT16_START+ \
                                                 BCCMDVARID_WRITEONLY + 75)

/* On BC3-audio this checks if we are charging or not. On certain BlueCore
   variants with charging hardware, we obtain more detailed information
   on the charger state. */
#define BCCMDVARID_CHARGER_STATUS              (BCCMDVARID_UINT16_START+ \
                                                 BCCMDVARID_READONLY + 76)

/* Enable/disable the charging hardware (only available on certain BlueCore
   variants). We can also read the currently set value. */
#define BCCMDVARID_CHARGER_DISABLE             (BCCMDVARID_UINT16_START+ \
                                                BCCMDVARID_READWRITE + 77)

/* Suppress or allow LED0 to light when charging (only available on certain
   BlueCore variants). We can also read the currently set value. */
#define BCCMDVARID_CHARGER_SUPPRESS_LED0        (BCCMDVARID_UINT16_START+ \
                                                 BCCMDVARID_READWRITE + 78)


/* Set coexistence sync pulse period. */
#define BCCMDVARID_COEX_SYNC_PERIOD	(BCCMDVARID_UINT16_START+ \
					BCCMDVARID_READWRITE+84)


/* Set or get whether coex priority pios should be toggled */
#define BCCMDVARID_COEX_PRIORITY_PIOS_ON	(BCCMDVARID_UINT16_START+ \
						 BCCMDVARID_READWRITE+85)


/* Set or get the routing of the FM audio */
#define BCCMDVARID_FM_AUDIO_ROUTING     	(BCCMDVARID_UINT16_START+ \
						 BCCMDVARID_READWRITE+87)

#define BCCMDVARID_COMBO_DOT11_ESCO_RTX_PRIORITY (BCCMDVARID_UINT16_START+ \
						 BCCMDVARID_READWRITE+88)


/* inquiry priority level (0 = default) */
#define BCCMDVARID_INQUIRY_PRIORITY             (BCCMDVARID_UINT16_START+ \
						 BCCMDVARID_READWRITE+92)


#define BCCMDVARID_CHARGER_SET_TRIM_NOW         (BCCMDVARID_UINT16_START+ \
                                                 BCCMDVARID_WRITEONLY+93)

#define BCCMDVARID_HQ_SCRAPING_LEN              (BCCMDVARID_UINT16_START+ \
                                                 BCCMDVARID_WRITEONLY+94)

#define BCCMDVARID_HQ_SCRAPING_ENABLE           (BCCMDVARID_UINT16_START+ \
                                                 BCCMDVARID_READWRITE+95)

#define BCCMDVARID_PANIC_ON_FAULT               (BCCMDVARID_UINT16_START+ \
						   BCCMDVARID_READWRITE+96)

#define BCCMDVARID_AUX_DAC                      (BCCMDVARID_UINT16_START+ \
						   BCCMDVARID_WRITEONLY+97)

#define BCCMDVARID_AUX_DAC_ENABLE               (BCCMDVARID_UINT16_START+ \
						   BCCMDVARID_READWRITE +98)

#define BCCMDVARID_INQUIRY_TX_POWER             (BCCMDVARID_UINT16_START+ \
						   BCCMDVARID_READWRITE +99)

/* Post B-33848 replacement for ADC/ADCRES; returns result via HQ */
#define BCCMDVARID_ADC_READ                     (BCCMDVARID_UINT16_START+ \
                                                   BCCMDVARID_WRITEONLY+100)

#define BCCMDVARID_VREF_CONSTANT                (BCCMDVARID_UINT16_START+ \
                                                   BCCMDVARID_READONLY+101)

#define BCCMDVARID_CLK_SKEW_COUNT               (BCCMDVARID_UINT16_START+ \
                                                    BCCMDVARID_READONLY+102)

#define BCCMDVARID_CLKSW_CURRENT_CLOCKS         (BCCMDVARID_UINT16_START+ \
                                                    BCCMDVARID_READWRITE+103)


 /* Set  the PCM2 gain.  This will only effect the PCM volume for 13 bit
     codes where external codec uses the last 3 bits as a volume control.
     A value outside the range 0 to 7 will be rejected.
 */
#define BCCMDVARID_PCM2_ATTENUATION	(BCCMDVARID_UINT16_START+ \
					BCCMDVARID_WRITEONLY+104)

#define BCCMDVARID_AFH_CHANNEL_RELEASE_TIME   (BCCMDVARID_UINT16_START+ \
                                                   BCCMDVARID_READWRITE +105)

/* BCCMD to disable RDF */
#define BCCMDVARID_RDF_DATA_CAPTURE_DISABLE (BCCMDVARID_UINT16_START+ \
                                             BCCMDVARID_WRITEONLY+106)

/* BCCMD for stream close source */
#define BCCMDVARID_STREAM_CLOSE_SOURCE  (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_WRITEONLY + 107)

/* BCCMD for stream close sink*/
#define BCCMDVARID_STREAM_CLOSE_SINK  (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_WRITEONLY + 108)

/* BCCMD for stream transform disconnect*/
#define BCCMDVARID_STREAM_TRANSFORM_DISCONNECT  (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_WRITEONLY + 109)

/* BCCMD for GSM receive mode */
#define BCCMDVARID_GSM_RX_MODE  (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_READWRITE + 110)

/* Set the Bluetooth low energy T_IFS time */
#define BCCMDVARID_BLE_T_IFS  (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_WRITEONLY + 111)

#define BCCMDVARID_RDF_PT_SELECT_ANTENNA (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_WRITEONLY + 112)

/* Set GSM receive channel (ARFCN) */
#define BCCMDVARID_GSM_RX_ARFCN  (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_READWRITE + 113)

/* Get cached temperature */
#define BCCMDVARID_CACHED_TEMPERATURE (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_READONLY + 114)

/* Get number of free entries in the whitelist */
#define BCCMDVARID_BLE_WHITELIST_FREE_SPACE  (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_READONLY + 115)

/* NB Special Leo: Unlock the OTP
   Takes 1 uint16 argument.
   arg0: TRUE/FALSE (0001 or 0000)

   This command locks or unlocks the OTP, preventing or allowing its
   contents to be modified.
   If the OTP is locked (the default state) then attempts to write to
   it with PS set will fail with BCCMDPDU_STAT_ERROR.
 */
#define BCCMDVARID_UNLOCK_OTP  (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_READWRITE + 116)

/* Enable/disable the Bluetooth low energy scan backoff counter */
#define BCCMDVARID_BLE_SCAN_BACKOFF   (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_WRITEONLY + 117)

/* Enable/disable SCO Streams */
#define BCCMDVARID_ENABLE_SCO_STREAMS  (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_WRITEONLY + 118)

/* Special Leo: Enable coex message to host reporting */
#define BCCMDVARID_ENABLE_COEX_MESSAGE_REPORTING  (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_WRITEONLY + 119)

/* Enable/disable coexistence signalling */
#define BCCMDVARID_COEX_ENABLE  (BCCMDVARID_UINT16_START + \
                                      BCCMDVARID_WRITEONLY + 120)

/* BCCMD for GPS enable */
#define BCCMDVARID_GPS_DSP_ENABLE  (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_WRITEONLY + 121)

/* Set/get the BLE default tx power */
#define BCCMDVARID_BLE_DEFAULT_TX_POWER (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_READWRITE + 122)

/* Set FMTX cdac for antenna calibration */
#define BCCMDVARID_FMTX_SET_CDAC (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_WRITEONLY + 123)

/* BCCMDs for Stibbons (DSPmanager).
 * It's not yet known exactly what commands there will be, so for
 * now a block of 16 values are reserved and will be filled in as
 * the code is integrated back to mainline. When the work is
 * finished the remaining values will be relinquished.
 */

/* Get the version of an existing capability */
#define BCCMDVARID_STIBBONS_U01X (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_NOACCESS + 124)

/* Remove a downloaded capability. */
#define BCCMDVARID_REMOVE_DOWNLOADED_CAPABILITY (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_NOACCESS + 125)

#define BCCMDVARID_STIBBONS_U03 (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_NOACCESS + 126)
#define BCCMDVARID_STIBBONS_U04 (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_NOACCESS + 127)
#define BCCMDVARID_STIBBONS_U05 (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_NOACCESS + 128)
#define BCCMDVARID_STIBBONS_U06 (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_NOACCESS + 129)
#define BCCMDVARID_STIBBONS_U07 (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_NOACCESS + 130)
#define BCCMDVARID_STIBBONS_U08 (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_NOACCESS + 131)
#define BCCMDVARID_STIBBONS_U09 (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_NOACCESS + 132)
#define BCCMDVARID_STIBBONS_U10 (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_NOACCESS + 133)
#define BCCMDVARID_STIBBONS_U11 (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_NOACCESS + 134)
#define BCCMDVARID_STIBBONS_U12 (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_NOACCESS + 135)
#define BCCMDVARID_STIBBONS_U13 (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_NOACCESS + 136)
#define BCCMDVARID_STIBBONS_U14 (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_NOACCESS + 137)
#define BCCMDVARID_STIBBONS_U15 (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_NOACCESS + 138)
#define BCCMDVARID_STIBBONS_U16 (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_NOACCESS + 139)

/* Enable clock modulation*/
#define BCCMDVARID_CLOCK_MODULATION_ENABLE (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_WRITEONLY + 140)

/* BCCMD that simply replies after a delay (used for testing the
   deferral logic) */

#define BCCMDVARID_WAIT_FOR_N_MS (BCCMDVARID_UINT16_START+ \
                                      BCCMDVARID_WRITEONLY + 141)

/* Turn LED2 on/off */
#define BCCMDVARID_LED2_ENABLE   (BCCMDVARID_UINT16_START+ \
                                   BCCMDVARID_WRITEONLY + 142)

/* Read SPI Lock status i.e. status of SPI lock and customer key */
#define BCCMDVARID_SPI_LOCK_STATUS   (BCCMDVARID_UINT16_START+ \
                                        BCCMDVARID_READONLY + 143)

/***************************************************************************/
/* Variables of type [u]int32.  The variable's value is in BCCMDPDU.d.u32.
(jrrk confirms that the XAP stores a uint32 with the more significant uint16
in the lower address.  Thus, presuming the bcsp carries data in ascending
addr. order, the more significant uint16 will travel over the wire first.) */

#define	BCCMDVARID_UINT32_START (3072)

/* The local device's bluetooth clock, held in the low 28 bits of the
returned value.  This increments every 312.5 microseconds, so the returned
value will be a little stale. */

#define BCCMDVARID_BT_CLOCK	(BCCMDVARID_UINT32_START+ \
					BCCMDVARID_READONLY+0)

/* Force the local device into deep sleep for the given period
in microseconds (a 32-bit quantity).  The device cannot be woken
early from this mode.  It may wake briefly in the intervening period
but will not resume normal operation at those points.  It will resume
normal operation when the given period has expired.  It is unspeficied
whether the the BCCMD reply to the sleep request appears before or
after the sleep.  This BCCMD is intended for debugging.
*/

#define BCCMDVARID_DEEP_SLEEP_PERIOD	(BCCMDVARID_UINT32_START+ \
					BCCMDVARID_WRITEONLY+1)

/*
 * Return a 32-bit CRC of ROM contents.  This only produces predictable
 * results with a genuine ROM part, not a ROM build in flash.
 * May not be available on all builds; if not available, getting or
 * it setting will return BCCMD_NO_SUCH_VARID.
 * The CRC performed is the same as used during by DFU.
 */
#define BCCMDVARID_CHECK_ROM_CRC32	(BCCMDVARID_UINT32_START+ \
					  BCCMDVARID_READONLY+2)

#define BCCMDVARID_DEEP_SLEEP_TIME  (BCCMDVARID_UINT32_START+ \
                      BCCMDVARID_READONLY+3)

/* Return a bitmap representing the AROPE capabilities of the
firmware.  AROPE stands for automatic renegotiation of periodic events
and is a method for managing periodic events which may want to use
the radio at the same time.  BTSW-681 */
#define BCCMDVARID_BLE_AROPE_CAPABILITIES  (BCCMDVARID_UINT32_START+ \
                      BCCMDVARID_READONLY+4)

/* For each capability a single bit is specified which shall be set to 1 if 
the capability is supported and set to 0 otherwise. */
#define BLE_AROPE_CAPABILITIES_BLE_MASTER_ESCO_SLAVE (0x00000001)
#define BLE_AROPE_CAPABILITIES_BLE_SLAVE_ESCO_MASTER (0x00000002)
#define BLE_AROPE_CAPABILITIES_BLE_SLAVE_ESCO_SLAVE  (0x00000004)

/* This key is obsolete now, it has been replaced by 
 * BCCMDVARID_SPI_LOCK_INITIATE_LOCK */	
/* Writes the 32 bit stay locked key which locks the SPI port*/
#define BCCMDVARID_SPI_LOCK_STAY_LOCKED_KEY	(BCCMDVARID_UINT32_START+ \
			                            BCCMDVARID_WRITEONLY+5)

/***************************************************************************/
/* Variables of special (i.e. awkward) types.  PDUs with this type always
use a special struct within the the BCCMDPDU to carry the data.  The type of
struct depends on the value of "varid". */

#define	BCCMDVARID_SPECIAL_START (4096)

/* A block memory read or written.  The data is held in the BCCMDPDU_MEMORY
in the BCCMDPDU.d. */

#define	BCCMDVARID_MEMORY     	(BCCMDVARID_SPECIAL_START+ \
					BCCMDVARID_NOACCESS+1)

/* Read or write from/to a bc01 buffer.  The data is carried in the
BCCMDPDU_BUFFER struct in the BCCMDPDU.d.  Use this command only with great
care. */

#define	BCCMDVARID_BUFFER     	(BCCMDVARID_SPECIAL_START+ \
					BCCMDVARID_NOACCESS+2)

/* Read or write from/to the bc01's persistent store.  The ps data is held in
the BCCMDPDU_PS in the BCCMDPDU.d.  ps keys values have individual read/write
permissions which add to the access controls in varids. */

#define	BCCMDVARID_PS     	(BCCMDVARID_SPECIAL_START+ \
					BCCMDVARID_READWRITE+3)

/* Run one of the (jdc-defined) radio tests.  These are detailed in
bccmdradiotestspdu.h - described in a separate file because the list seems
to grow hourly. */

#define	BCCMDVARID_RADIOTEST   	(BCCMDVARID_SPECIAL_START+ \
					BCCMDVARID_WRITEONLY+4)


/* Finds the next higher key in the ps.  Notionally this command numerically
sorts all of the keys in the ps into ascending order and returns the first
key that that is greater than the pskey value in the u16.  If the u16 is
0 then the lowest valued key in the store is returned.

This function deals only with the values of keys in the persistent
store - it ignores the data stored with keys.

The command returns the next key in the store with value greater than the
id or 0 if there is no such key.

The "stores" parameter describes which of the three ps stores to examine
and the order in which to search them.

The getreq holds the initial pskey in BCCMDPDU_PSNEXT.id and the stores to be
examined in BCCMDPDU_PSNEXT.stores.  A successful getresp holds the next
pskey in BCCMDPDU_PS.nextid. */

#define	BCCMDVARID_PSNEXT     	     (BCCMDVARID_SPECIAL_START+ \
					BCCMDVARID_READONLY+5)


/* How much storage is held under a specified pskey?   The getreq holds the
pskey in BCCMDPDU_PSSIZE.id and the stores to examine in
BCCMDPDFU_PSSIZE.stores.  A successful getresp holds the size, counted in
uint16s, in BCCMDPDU_PSSIZE.len. */

#define	BCCMDVARID_PSSIZE     	     (BCCMDVARID_SPECIAL_START+ \
					BCCMDVARID_READONLY+6)


/* Result of attempting to read the ADC. */
#define	BCCMDVARID_ADCRES     	     (BCCMDVARID_SPECIAL_START+ \
					BCCMDVARID_READONLY+7)


/* How strong is the encryption on an established ACL? */
#define	BCCMDVARID_CRYPTKEYLEN 	     (BCCMDVARID_SPECIAL_START+ \
					BCCMDVARID_READONLY+8)


/* Half the BT clock on an established ACL. */
#define	BCCMDVARID_PICONET_INSTANT   (BCCMDVARID_SPECIAL_START+ \
					BCCMDVARID_READONLY+9)


/* Get and clear an evt event counter. */
#define	BCCMDVARID_GET_CLR_EVT	     (BCCMDVARID_SPECIAL_START+ \
					BCCMDVARID_READONLY+10)

/* Get the next build definition identifier. */
#define	BCCMDVARID_GET_NEXT_BUILDDEF (BCCMDVARID_SPECIAL_START+ \
					BCCMDVARID_READONLY+11)

/* Remove a value from the bc01's persistent store (ps).  The setreq holds
the key (pskey) of the item to be removed in BCCMDPDU_PSCLRS.id, and the
stores to remove it from in BCCMDPDU_PSCLRS.stores.  This is an extension
of BCCMDVARID_PSCLR. */
#define	BCCMDVARID_PSCLRS            (BCCMDVARID_SPECIAL_START+ \
					BCCMDVARID_WRITEONLY+12)

/* Perform a composite write/read transfer on the I2C-bus. */
#define BCCMDVARID_I2C_TRANSFER      (BCCMDVARID_SPECIAL_START+ \
					BCCMDVARID_NOACCESS+13)

/* Read or write the chracteristics of an I2C EEPROM device. */
#define BCCMDVARID_E2_DEVICE         (BCCMDVARID_SPECIAL_START+ \
					BCCMDVARID_READONLY+14)

/* Read or write part of a VM application stored in EEPROM. */
#define BCCMDVARID_E2_APP_DATA       (BCCMDVARID_SPECIAL_START+ \
					BCCMDVARID_READONLY+15)

/* Enable or disable use of adaptive frequency hopping (AFH) for either
existing links or new connections where the local device is master. */
#define BCCMDVARID_ENABLE_AFH        (BCCMDVARID_SPECIAL_START+ \
					BCCMDVARID_READWRITE+16)

/* Red-M Change Starts */

/* This value is used to control a "bad link" trigger, used for handoff.
The first word is an HCI handle.  The second is the BER trigger value;
setting it to zero disables the trigger. Otherwise the value is from 1->255
where the trigger is (val * 32) and compared to raw BER. */
#define	BCCMDVARID_BER_THRESHOLD     	(BCCMDVARID_SPECIAL_START+ \
					BCCMDVARID_READWRITE+17)

/* Red-M Change Ends */

/* Read the type of memory underlying the BCCMDPDU_PS_MEMORY_TYPE.stores
store. The result is returned in the BCCMDPDU_PS_MEMORY_TYPE.memorytype
field. */
#define BCCMDVARID_PS_MEMORY_TYPE	(BCCMDVARID_SPECIAL_START+ \
					BCCMDVARID_READONLY+18)

/* The firmware contains a list of pskeys that are used directly by the
firmware (e.g., PSKEY_BDADDR) or that can reasonably be set for use with/by
the firmware (e.g., PSKEY_USR0).  This command searches this list.

The command returns the first key from the list that that is numerically
greater than "id" and writes this to "nextid".  If "id" is zero then the
lowest numbered key is returned.  If "id" is the highest numbered pskey then
"nextid" is set to zero.

This should allow host tools, e.g., pstool.exe, to display only pskeys
relevent to the attached firmware build. */

#define	BCCMDVARID_PSNEXT_ALL  	     (BCCMDVARID_SPECIAL_START+ \
					BCCMDVARID_READONLY+19)

/* Enable or disable the generation or checking of a CRC on an L2CAP
   channel. */
#define BCCMDVARID_L2CAP_CRC            (BCCMDVARID_SPECIAL_START+ \
                                        BCCMDVARID_READWRITE+20)

/* The following two bccmds were used for medium rate testing on the branch
 * main-medium-rate. Now available for re-use but may be prudent to use other
 * commands in preference.
 */

#define BCCMDVARID_ENABLE_MEDIUM_RATE  (BCCMDVARID_SPECIAL_START+ \
                                        BCCMDVARID_WRITEONLY+21)
  
/*                 ALTER_SCO_SETTINGS  was 22 
 */

/* Command to debug the kalimba. See bccmdkaldebugpdu.h for more details */
#define BCCMDVARID_KALDEBUG             (BCCMDVARID_SPECIAL_START+ \
                                         BCCMDVARID_READWRITE+23)

/* Reserved for VM debugging commands */
#define BCCMDVARID_VM_DEBUG         (BCCMDVARID_SPECIAL_START+ \
                                     BCCMDVARID_READWRITE + 24)


/*
Get or set the limits which will be applied by BlueCore to sniff
requests initiated by a remote device.  The information consists of six
16-bit words in the order:

Max Tsniff	)  Even, 0 to 0xfffe
Min Tsniff	)
Max attempts	  ) 1 to 0x7fff inclusive
Min attempts	  )
Max timeout	) 0 to 0x7fff inclusive
Min timeout	)

The quantities correspond to the values in the HCI sniff specification.
(Note in particular this means Tsniff is in baseband slots while
attempts and timeout are in receive slots.)
If the initiating device requests values out of the range given,
a reply will be sent offering the nearest values inside the range.

Any value can be 0xffff to specify `don't care'.  This is an
exception to the limits shown above.

Note that the ability to negotiate the `attempts' and `timeout'
parameters is not explicit in the Bluetooth specification, hence
attempts to alter these values might be rejected by the remote side.
Typical values of attempts and timeouts are small, no more than
a dozen or so.
*/
#define BCCMDVARID_REMOTE_SNIFF_LIMITS	(BCCMDVARID_SPECIAL_START+ \
					 BCCMDVARID_READWRITE + 25)

/*
BABEL_SPI_RAW is a read / write operation using unverified spi.
BABEL_SPI_VERIFIED is a read / write operation using verified spi.
They use the BCCMDPDU_MEMORY pdu
*/
#define BCCMDVARID_BABEL_SPI_RAW       (BCCMDVARID_SPECIAL_START+ \
                                        BCCMDVARID_READWRITE + 26)
#define BCCMDVARID_BABEL_SPI_VERIFIED  (BCCMDVARID_SPECIAL_START+ \
                                        BCCMDVARID_READWRITE + 27)

/* Obtain the stack firmware's build name. */
#define BCCMDVARID_BUILD_NAME          (BCCMDVARID_SPECIAL_START+ \
                                        BCCMDVARID_READONLY + 28)

/* Get last RSSI value on previously given ACL handle.
 * Returns STAT_OK if data is available, STAT_NO_DATA if it isn't,
 * and STAT_ERROR if no valid ACL handle has been specified.
 */
#define BCCMDVARID_RSSI_ACL             (BCCMDVARID_SPECIAL_START + \
                                         BCCMDVARID_READONLY + 29)

/* Commands to control the eSCO test modes.
 */
#define BCCMDVARID_ESCOTEST             (BCCMDVARID_SPECIAL_START + \
                                         BCCMDVARID_READWRITE + 30)

/* Commands to control battery charger on paddywack/pugwash. No longer
   supported. We now have seperate bccmds for charger functionality
   rather than multiplexing through this single bccmd. */

#define BCCMDVARID_BC3PSU        (BCCMDVARID_SPECIAL_START+ \
                                     BCCMDVARID_READWRITE + 31)


/* The arguments are an ACL channel and a boolean.
 * If the boolean is TRUE, attempts to increase the power on
 * an enhanced data rate (EDR) link to class 1 levels will not be
 * honoured.  Instead, the device will limit the power at the maximum
 * possible level below and report "maximum power" to its
 * peer.  The class 1 level is recognised by the presence
 * of the appropriate bit in the EDR section of
 * PSKEY_LC_ENHANCED_POWER_TABLE.
 *
 * If the boolean is FALSE (the default setting),
 * attempts to increase power when in EDR will be honoured, but the
 * device may switch to basic data rate if the hardware does
 * not support class 1 operation in EDR.
 *
 * The former setting is appropriate if any application using
 * the given ACL channel requires the full EDR bandwidth.
 * Otherwise, the default setting is more appropriate.
 *
 * The PDU is not handled if no EDR support is present.
 */
#define BCCMDVARID_LIMIT_EDR_POWER	(BCCMDVARID_SPECIAL_START + \
					 BCCMDVARID_READWRITE + 32)


/* Request the Packet Error Rate - array size will be the value
   returned with the BCCMDVARID_LC_FRAC_COUNT command */
#define BCCMDVARID_LC_RX_FRACS        (BCCMDVARID_SPECIAL_START+ \
                                     BCCMDVARID_READONLY + 33)




/* Commands the LM to enable sniff sub-rating on a particular link */
#define BCCMDVARID_SNIFF_SUB_RATE   (BCCMDVARID_SPECIAL_START + \
                                    BCCMDVARID_WRITEONLY + 40)



/* Read/write registers on the FM chip */
#define BCCMDVARID_FM_REG  (BCCMDVARID_SPECIAL_START+ \
                                                 BCCMDVARID_READWRITE + 45)
#define BCCMDVARID_FM_RDS  (BCCMDVARID_SPECIAL_START+ \
                                                 BCCMDVARID_READONLY + 46)

/* Configure a PCM port and the routing to it. */
#define BCCMDVARID_PCM_CONFIG32             (BCCMDVARID_SPECIAL_START+ \
                                                 BCCMDVARID_WRITEONLY + 47)
#define BCCMDVARID_PCM_LOW_JITTER           (BCCMDVARID_SPECIAL_START+ \
                                                 BCCMDVARID_WRITEONLY + 48)
#define BCCMDVARID_PCM_RATE_AND_ROUTE       (BCCMDVARID_SPECIAL_START+ \
                                                 BCCMDVARID_WRITEONLY + 49)
#define BCCMDVARID_DIGITAL_AUDIO_RATE       (BCCMDVARID_SPECIAL_START+ \
                                                 BCCMDVARID_WRITEONLY + 50)
#define BCCMDVARID_DIGITAL_AUDIO_CONFIG     (BCCMDVARID_SPECIAL_START+ \
                                                 BCCMDVARID_WRITEONLY + 51)
                                                
/*
* Returns the last time this device was communicated with 
* from another device, used for an external timeout check
*/					  

#define BCCMDVARID_AWAY_TIME	(BCCMDVARID_SPECIAL_START+ \
					                BCCMDVARID_READONLY+52)

/* Sha256 debugging commands */
/********************** MOVED DUE TO DUPLICATION of VARID **************
* We are suppose to make the index increment contiguous inside the 
* specific var type,So we moved this define upto index 58 below
***********************************************************************
#define	BCCMDVARID_SHA256_DATA     	(BCCMDVARID_SPECIAL_START+ \
					 BCCMDVARID_READWRITE+52)
***********************************************************************/
#define	BCCMDVARID_SHA256_RES     	(BCCMDVARID_SPECIAL_START+ \
					 BCCMDVARID_READWRITE+53)



#define BCCMDVARID_HQ_SCRAPING            ( BCCMDVARID_SPECIAL_START+ \
        					 BCCMDVARID_READONLY+56)  

 					 

#define BCCMDVARID_MIC_BIAS_CTRL        ( BCCMDVARID_SPECIAL_START+ \
					 BCCMDVARID_READWRITE+57) 	
			 
	
	/* Sha256 debugging commands */
#define	BCCMDVARID_SHA256_DATA     	(BCCMDVARID_SPECIAL_START+ \
					 BCCMDVARID_READWRITE+58)

#define BCCMDVARID_WLAN_COEX_PRIORITY (BCCMDVARID_SPECIAL_START + \
                                       BCCMDVARID_WRITEONLY+59)

/* Bag this one for FM-TX nothing added yet, 
 *but we need a fixed number for the FM-API */                                       
#define BCCMDVARID_FMTX_REG  (BCCMDVARID_SPECIAL_START+ \
                                       BCCMDVARID_READWRITE + 62)

#define BCCMDVARID_FMTX_TEXT  (BCCMDVARID_SPECIAL_START+ \
                                       BCCMDVARID_WRITEONLY + 63)
                                       
/* The randXX function is called repeately and a block of random 
numbers is returned. */

#define	BCCMDVARID_RANDOM     	(BCCMDVARID_SPECIAL_START+ \
					BCCMDVARID_NOACCESS+64)
					
/* The FastPipe BCCMDs for enabling the system and creating and
 * destroying pipes */
#define BCCMDVARID_FASTPIPE_ENABLE  (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_WRITEONLY + 65)

#define BCCMDVARID_FASTPIPE_CREATE  (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_WRITEONLY + 66)

#define BCCMDVARID_FASTPIPE_DESTROY (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_WRITEONLY + 67)

/* Testing the WLAN coex interface */					
#define BCCMDVARID_COEX_INT_TEST (BCCMDVARID_SPECIAL_START + \
                                  BCCMDVARID_READWRITE + 69)

/* GPS testing -- Start the GPS data transfer.
*/
#define BCCMDVARID_GPS_START            (BCCMDVARID_SPECIAL_START+ \
                                        BCCMDVARID_WRITEONLY+70)

/* GPS testing -- Stop the GPS data transfer.
*/
#define BCCMDVARID_GPS_STOP             (BCCMDVARID_SPECIAL_START+ \
                                        BCCMDVARID_WRITEONLY+71)

/* VM status */
#define BCCMDVARID_VM_STATUS            (BCCMDVARID_SPECIAL_START+ \
                                        BCCMDVARID_READONLY + 72)
                                        
/* Config the eGPS pulse generation
*/
#define BCCMDVARID_EGPS_PULSE_CONFIG        (BCCMDVARID_SPECIAL_START+ \
                                            BCCMDVARID_WRITEONLY+73)
/* Start the eGPS pulse generation for input scenario
*/
#define BCCMDVARID_EGPS_PULSE_INPUT_START   (BCCMDVARID_SPECIAL_START+ \
                                            BCCMDVARID_WRITEONLY+74)
/* Start the eGPS pulse generation for output scenario
*/
#define BCCMDVARID_EGPS_PULSE_OUTPUT_START  (BCCMDVARID_SPECIAL_START+ \
                                            BCCMDVARID_WRITEONLY+75)

/* Stop the eGPS pulse generation
*/
#define BCCMDVARID_EGPS_PULSE_STOP          (BCCMDVARID_SPECIAL_START+ \
                                            BCCMDVARID_WRITEONLY+76)
/* Read SCO/eSCO parameters
*/
#define BCCMDVARID_SCO_PARAMETERS    (BCCMDVARID_SPECIAL_START+ \
                                     BCCMDVARID_READONLY + 77)
/* Start Coaster */
#define BCCMDVARID_COASTER_START     (BCCMDVARID_SPECIAL_START+	\
					BCCMDVARID_WRITEONLY + 78)
/* Stop Coaster */
#define BCCMDVARID_COASTER_STOP      (BCCMDVARID_SPECIAL_START+	\
					BCCMDVARID_WRITEONLY + 79)
/* Configure Coaster */
#define BCCMDVARID_COASTER_ADD_SV    (BCCMDVARID_SPECIAL_START+	\
					BCCMDVARID_WRITEONLY + 80)
/* Delete configured channel(s) from Coaster */
#define BCCMDVARID_COASTER_DEL_SV    (BCCMDVARID_SPECIAL_START+	\
					BCCMDVARID_WRITEONLY + 81)
/* Read back tracking state from Coaster */
#define BCCMDVARID_COASTER_SV_STATE  (BCCMDVARID_SPECIAL_START+	\
					BCCMDVARID_READONLY  + 82)

#define BCCMDVARID_ALLOCATE_RAM_RESERVE (BCCMDVARID_SPECIAL_START +\
                                       BCCMDVARID_WRITEONLY + 83)

#define BCCMDVARID_RECLAIM_RAM_RESERVE (BCCMDVARID_SPECIAL_START + \
                                         BCCMDVARID_WRITEONLY + 84)

/* Control HQ indications from coaster */
#define BCCMDVARID_COASTER_INDICATIONS (BCCMDVARID_SPECIAL_START+     \
                                        BCCMDVARID_WRITEONLY  + 85)

/* BCCMD to enable RDF */
#define BCCMDVARID_RDF_DATA_CAPTURE_ENABLE (BCCMDVARID_SPECIAL_START+ \
                                             BCCMDVARID_WRITEONLY+86)


#define BCCMDVARID_FASTPIPE_RESIZE  (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_WRITEONLY + 87)

/* BCCMD to set the CODEC Input gain (for both A&B inputs) */
#define BCCMDVARID_CODEC_INPUT_GAIN  (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_WRITEONLY + 88)

/* BCCMD to set the CODEC Output gain (for both A&B outputs) */
#define BCCMDVARID_CODEC_OUTPUT_GAIN  (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_WRITEONLY + 89)

/* BCCMD for stream get source */
#define BCCMDVARID_STREAM_GET_SOURCE  (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_WRITEONLY + 90)

/* BCCMD for stream get sink */
#define BCCMDVARID_STREAM_GET_SINK  (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_WRITEONLY + 91)

/* BCCMD for stream configure*/
#define BCCMDVARID_STREAM_CONFIGURE  (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_WRITEONLY + 92)

/* BCCMD for stream alias sink*/
#define BCCMDVARID_STREAM_ALIAS_SINK  (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_WRITEONLY + 93)

/* BCCMD for stream connect*/
#define BCCMDVARID_STREAM_CONNECT  (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_WRITEONLY + 94)

#define BCCMDVARID_RDF_PT_ANT_PIO (BCCMDVARID_SPECIAL_START+ \
                                     BCCMDVARID_READWRITE + 95)

#define BCCMDVARID_COEX_DUMP      (BCCMDVARID_SPECIAL_START+ \
                                     BCCMDVARID_READONLY + 96)

#define BCCMDVARID_RDF_BLE_PACKET_MATCH (BCCMDVARID_SPECIAL_START+ \
                                     BCCMDVARID_WRITEONLY + 97)

/* BCCMD for stream sync sids*/
#define BCCMDVARID_STREAM_SYNC_SID   (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_WRITEONLY + 98)

#define BCCMDVARID_RDF_BLE_ADVERTISING (BCCMDVARID_SPECIAL_START+ \
                                        BCCMDVARID_WRITEONLY + 99)

#define BCCMDVARID_RDF_BLE_RAND_ADDR (BCCMDVARID_SPECIAL_START+ \
                                        BCCMDVARID_WRITEONLY + 100)

/* BCCMD for enabling/disabling RDF scanning */
#define BCCMDVARID_RDF_BLE_SCAN_MODE (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_WRITEONLY + 101)

/* BCCMD to debug the kalimba (for BC5 and above)  
   See bccmdkalextaddrdebugpdu.h for more details */
#define BCCMDVARID_KALEXTADDRDEBUG   (BCCMDVARID_SPECIAL_START+ \
                                         BCCMDVARID_READWRITE+102)

#define BCCMDVARID_PCM_SLOTS_PER_FRAME (BCCMDVARID_SPECIAL_START+ \
                                         BCCMDVARID_WRITEONLY + 103)

#define BCCMDVARID_PCM_CLOCK_RATE (BCCMDVARID_SPECIAL_START+ \
                                         BCCMDVARID_WRITEONLY + 104)

#define BCCMDVARID_PCM_SYNC_RATE (BCCMDVARID_SPECIAL_START+ \
                                         BCCMDVARID_WRITEONLY + 105)

#define BCCMDVARID_MAP_SCO_AUDIO (BCCMDVARID_SPECIAL_START+ \
                                         BCCMDVARID_WRITEONLY + 106)

#define BCCMDVARID_MAP_SCO_WBS (BCCMDVARID_SPECIAL_START+ \
                                         BCCMDVARID_WRITEONLY + 107)

/* BCCMD for enabling/disabling RDF production test mode */
#define BCCMDVARID_RDF_PT (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_WRITEONLY + 108)

/***
 * NB Customer specific (Intel, for the Leo and BillieJean projects)
 * BCCMD to help Leo's coex message spec
 *
 */
#define BCCMDVARID_INFORM_LINK_PROFILES (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_READWRITE + 109)

/**
 * NB Customer specific (Intel, for the Leo and BillieJean projects)
 * BCCMD to retrieve OTP stats from the firmware.
 *
 * The return value from this command is an array containing 22 values
 * with the following format:
 *
 * [0]  "Write retry bin 1"
 * [1]  "Write retry bin 2"
 * [2]  "Write retry bin 3"
 * [3]  "Write retry bin 4"
 * [4]  "Write retry bin 5"
 * [5]  "Write retry bin 6"
 * [6]  "Correctable writes"
 * [7]  "Uncorrectable writes"
 * [8]  "Total words written"
 * [9]  "Total words read"
 * [10] "Correctable reads"
 * [11] "Uncorrectable reads"
 * [12] "Failed data [0]"
 * [13] "Failed data [1]"
 * [14] "Failed data [2]"
 * [15] "Failed data [3]"
 * [16] "Uncorrectable address"
 * [17] "Correctable address 1"
 * [18] "Correctable address 2"
 * [19] "Correctable address 3"
 * [20] "Correctable address 4"
 * [21] "Index into correctable address"
 *
 */
#define BCCMDVARID_OTP_STATS (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_READONLY + 110)

/* BCCMDs for Stibbons (DSPmanager).
 * It's not yet known exactly what commands there will be, so for
 * now a block of 16 values are reserved and will be filled in as
 * the code is integrated back to mainline. When the work is
 * finished the remaining values will be relinquished.
 */

/* Used in Stibbons development, then withdrawn. */
#define BCCMDVARID_STIBBONS_S01X (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_NOACCESS + 111)

/* BCCMDs to start, stop, reset, or destroy a list of operators. */
#define BCCMDVARID_START_OPERATOR (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_WRITEONLY + 112)
#define BCCMDVARID_STOP_OPERATOR (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_WRITEONLY + 113)
#define BCCMDVARID_RESET_OPERATOR (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_WRITEONLY + 114)
#define BCCMDVARID_DESTROY_OPERATOR (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_WRITEONLY + 115)

/* BCCMD to send a message to an operator. */
#define BCCMDVARID_OPERATOR_MESSAGE (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_WRITEONLY + 116)

/* BCCMDs to create a new operator. */
#define BCCMDVARID_CREATE_OPERATOR_C (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_WRITEONLY + 117)
#define BCCMDVARID_CREATE_OPERATOR_P (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_WRITEONLY + 118)

/* BCCMD to start downloading a capability */
#define BCCMDVARID_DOWNLOAD_CAPABILITY  (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_NOACCESS + 119)

#define BCCMDVARID_STIBBONS_S10 (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_NOACCESS + 120)
#define BCCMDVARID_STIBBONS_S11 (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_NOACCESS + 121)
#define BCCMDVARID_STIBBONS_S12 (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_NOACCESS + 122)
#define BCCMDVARID_STIBBONS_S13 (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_NOACCESS + 123)
#define BCCMDVARID_STIBBONS_S14 (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_NOACCESS + 124)
#define BCCMDVARID_DSPMANAGER_DEBUG (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_WRITEONLY + 125)
#define BCCMDVARID_PREVIN_TUNNEL (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_NOACCESS + 126)

/* Configure clock modulation */
#define BCCMDVARID_CLOCK_MODULATION_CONFIG (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_WRITEONLY + 127)

/* Get Charge Trims for BC7+ Chips with charger) */
#define BCCMDVARID_CHARGER_TRIMS (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_READONLY + 128)

/* Mic bias config BCCMD, this supersedes MIC_BIAS_CTRL */
#define BCCMDVARID_MIC_BIAS_CONFIGURE (BCCMDVARID_SPECIAL_START+ \
                                       BCCMDVARID_WRITEONLY + 129)


/*
  This Command is used to set the pin to a requested function. 
  For example if a pin can be mapped as UART_TX/UART_RX/PCM_IN etc,  
  then this BCCMD can be used to set the pin for one of the supported function

  Command takes 2 parameters
   *) First parameter: 16 bit represents the pin number which can be (0..31), 
  if the BC chip supports more than 16 PIO lines.
  
   *) Second Parameter: 16 bit value / Function represents the pin function.
      Function can be any of the following  
                       0 ==> UART_RX
                       1 ==> UART_TX
                       2 ==> UART_CTS
                       3 ==> UART_RTS
                       4 ==> PCM_IN
                       5 ==> PCM_OUT
                       6 ==> PCM_SYNC
                       7 ==> PCM_CLK
      Users are requested to go through the chip specific data sheet document 
      which is part of the release package.

  Trying to set a function which is NOT supported by the pin will return an 
  error without affecting/modifying the existing pin function.

  Usage Example:
  Ex: say PIO[3] can be mapped to UART_RX/PCM_IN/PIO and currently mapped as PIO
  then users should use the PIO32_MAP_PINS to unmap it  and then use this 
  BCCMD to map to a function.
  spf 3 UART_RX --> to map PIO[3] as UART_RX
  OR 
  spf 3 0 --> to map PIO[3] as UART_RX
  To map back the PIO[3] as a PIO, PIO32_MAP_PINS command should be used.

  If the pin supports only two functions i.e., PIO/UART_RX then PIO32_MAP_PINS
  BCCMD is sufficient to map the pin as UART_RX/PIO depending upon the mask.
  
*/
#define BCCMDVARID_PIO32_SET_PIN_FUNCTION (BCCMDVARID_SPECIAL_START + \
                                       BCCMDVARID_WRITEONLY + 130)

/* 
 * BCCMD controls for 32bit PIOs
 *
 * PIO ports are upto 32bits wide, widths of 16,24 and 32 bits are common.
 * This commands are intended to support all cases, bits wider than the 
 * hardware are ignored. 
 *
 * These 32bit BCCMDs are intended supercede the following 16bit commands:
 *     BCCMDVARID_PIO_MASK,
 *     BCCMDVARID_PIO,
 *     BCCMDVARID_PIO_PROTECT_MASK, 
 *     BCCMDVARID_PIO_STRONG_BIAS.
 *  
 * However their functionality will be retained. 
 */

/* Sets whether the individual bits of the PIO port are used as inputs
or outputs.  If a bit is 1 then the corresponding bit position of the 
chip's PIO port is set to be an output, else it is set as an input.

This command must be used with care: chip code using the pio port probably
depends on knowledge of the bits' directions. */

#define	BCCMDVARID_PIO32_DIRECTION_MASK 	     (BCCMDVARID_SPECIAL_START+ \
						BCCMDVARID_READWRITE+131)

/* Access the value of the chip's 32-bit pio port.
Whether a pio bit is input (on read) or output (on write)
depends on the value of BCCMDVARID_PIO32_DIRECTION_MASK. */

#define	BCCMDVARID_PIO32 	     		(BCCMDVARID_SPECIAL_START+ \
						BCCMDVARID_READWRITE+132)
                        
/* The chip's PIO port (confusingly called the "aux" io port by the hal and
below) is supposed to be a general purpose provision.  Alas, lots of bits of
hardware and software bag various pins for their own purposes, e.g. external
power amp control (hw) and usb support (firmware).  This effectively means
that for any given application/configuration only a subset of the pins may
be available to an application.  This applies whether the application is on
chip or on the host.  In either case the application needs some sort
constraint on its attempts to change arbitrary pio pins.  For example, we
cannot allow the host to use bccmd commands to set and clear arbitrary pio
pins because this may interfere with, say, usb operation.  We certainly can't
block interrupts while a pair of bccmds perform a read-modify-write.

This commands acts as a crude access control mask.  The bits in the mask map
to the corresponding bits in the pio port.  Each bit set to 1 in the mask
cannot be set by bccmd/application. */

#define	BCCMDVARID_PIO32_PROTECT_MASK	(BCCMDVARID_SPECIAL_START+ \
						BCCMDVARID_READONLY+133)

/* For hardware starting with Kato, it is possible for a PIO used as input
to be configured with a weak bias (the default), or a strong bias.  A
strong bias is a constant current source of a few tens of microamps.
This value sets or returns the register
*/
#define BCCMDVARID_PIO32_STRONG_BIAS	(BCCMDVARID_SPECIAL_START+ \
					BCCMDVARID_READWRITE+134)

/* Some chips (e.g. Elvis, Priscilla, Gordon) let us map pin between basic 
PIOs and multiplexed peripheral functions. 

This command allows the host the select between PIO and peripheral functions.
Each bit set to a 1 selects a PIO, and 0 the multiplexed function. 
Note: Not all Pins are multiplexed, and these will vary between packages */
 
#define BCCMDVARID_PIO32_MAP_PINS	(BCCMDVARID_SPECIAL_START+ \
					BCCMDVARID_READWRITE+135) 

/* This command allows to make changes in LED configuration. This command
   configures the following paramaters: 
        LED_ENABLE
        LED_DUTY_CYCLE
        LED_PERIOD
        LED_FLASH_ENABLE
        LED_FLASH_RATE
        LED_FLASH_MAX_HOLD
        LED_FLASH_MIN_HOLD
        LED_FLASH_SEL_INVERT
        LED_FLASH_SEL_DRIVE
        LED_FLASH_SEL_TRISTATE
*/
#define BCCMDVARID_LED_CONFIG   (BCCMDVARID_SPECIAL_START+ \
					BCCMDVARID_WRITEONLY+136)

/* BLE advertising report filter. BTSW-623
 * When an advertising report filter is set, the LM will discard
 * BLE advertising reports that do not match the filter. With no
 * filter set, the LM passes all advertising reports to the host.
 *
 * The following will add an advertising filter to the list.
 */
#define BCCMDVARID_BLE_ADV_FILT_ADD (BCCMDVARID_SPECIAL_START+ \
                                        BCCMDVARID_WRITEONLY+137)

/** BCCMD for Low Energy Radio tests
 * 
 * This command is used to start or stop a test of the Low Energy 
 * radio.
 */
#define BCCMDVARID_BLE_RADIO_TEST (BCCMDVARID_SPECIAL_START+ \
                                      BCCMDVARID_WRITEONLY + 138)

/** 
 * BCCMD for serial flash memory operations
 * This command is used to read/write/erase serial flash memory
 */
#define BCCMDVARID_SIFLASH (BCCMDVARID_SPECIAL_START + \
                                      BCCMDVARID_NOACCESS + 139)

/*
 * BCCMD for Cap Sense Radio Test
 * This is used to read the capacitance of the pads defined by
 * a provided bit pattern, hence allowing the test to determine whether
 * the chip package solder balls are connected to the cap sense pads.
 */
#define BCCMDVARID_CAPSENSE_RT_PADS (BCCMDVARID_SPECIAL_START + \
                                            BCCMDVARID_READONLY + 140)
                                      
/* Writes the 128 bit Customer key which unlocks the SPI when chip boots*/
#define BCCMDVARID_SPI_LOCK_CUSTOMER_KEY (BCCMDVARID_SPECIAL_START+ \
                                            BCCMDVARID_WRITEONLY + 142)

/* ----------------------------------------------------------------- */
#endif	/* __BCCMDPDU_H__ */
/* EOF */
