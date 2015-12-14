/* *****************************************************************************
   %%fullcopyright(2005)        http://www.csr.com
   %%version

   $Revision: #1 $  $Date: 2011/01/19 $
   *****************************************************************************

   *****************************************************************************
   NAME:
      kalimba_message.h

   DESCRIPTION:
      Messages passed between BlueCore firmware and the Kalimba DSP.

      Short messages consist of a 16-bit ID and up to four 16-bit arguments.

      Long messages are built up from a number of short messages, see the
      description below under KAL_MSG_PART_BASE.

      If the top-bit of the ID is set the message is routed to the firmware,
      otherwise it is given to the VM application.

      These are simple #define's (converted to .CONSTS for kalimba) since they
      need to be used by the kalimba assembler for the DSP side of things.

   NOTES:
      Port numbers are handled in a rather inconsistent manner. In this file:
      * "read port" is a port sending data to the Kalimba. They are numbered
        from 0 upwards (0..3 on BC3-MM, 0..7 on BC5-MM onwards).
      * "write port" is a port taking data from the Kalimba. They are also
        numbered from 0 upwards (0..3 on BC3-MM, 0..7 on BC5-MM onwards).
      * "either type of port" means either a read port or a write port.
        + Read port numbers are used unchanged.
        + Write port numbers are offset by NUM_PORTS (the number of read ports),
          (so they are 4..7 on BC3-MM, 8..15 on BC5-MM onwards).
   *****************************************************************************
*/

#ifndef _KALIMBA_MESSAGES_H
#define _KALIMBA_MESSAGES_H

/*
  Kalimba Ready

  DSP -> FW

  Indicates that the message library in the DSP has successfully
  started and is ready to accept messages from the XAP.
*/
#define KAL_MSG_KALIMBA_READY                0x8000



/*
  Configure port

  FW -> DSP

  Indicate that a DSP port has been configured. Sent from the firmware
  to tell the DSP that a port has just been associated with a buffer
  with the given read and write handles.

  Arguments

    port       Number of either type of port.
    offset     pointer in DSP space to the offset for access
               (owned by the DSP), zero if the port is being
               disconnected.
               (16 bits; to be sign-extended by DSP on devices
               with 24-bit addresses.)
    limit      pointer in DSP space to the limit on the offset
               (updated by Xap), zero if the port is being
               disconnected.
               (16 bits; to be sign-extended by DSP on devices
               with 24-bit addresses.)
    metadata   0: no metadata attached
               1: metadata attached
               All other values are reserved at present.
*/
#define KAL_MSG_CONFIGURE_PORT               0x8001



/*
  Data consumed

  DSP -> FW

  Indicate that data has been consumed through the specified ports and
  the firmware may want to try refilling it.

  Arguments

    ports      bit N set if data consumed on read port N
*/
#define KAL_MSG_DATA_CONSUMED                0x8002



/*
  Data produced

  DSP -> FW

  Indicate that data has been written through the specified ports and
  the firmware may want to try processing it.

  Arguments

    port       bit N set if data produced on write port N
*/
#define KAL_MSG_DATA_PRODUCED                0x8003



/*
  DAC warp

  DSP -> FW

  Adjust warp for one or both DACs.

  Arguments

    which     1 for A, 2 for B, 3 for A&B
    warp      warp to use
*/
#define KAL_MSG_WARP_DAC                     0x8004



/*
  ADC warp

  DSP -> FW

  Adjust warp for one or both ADCs.

  Arguments

    which     1 for A, 2 for B, 3 for A&B
    warp      warp to use
*/
#define KAL_MSG_WARP_ADC                     0x8005



/*
  Read persistent store

  DSP->FW

  Request the value of a persistent store key

  Arguments

    key      the key to read, as passed to PsFullRetrieve
*/
#define KAL_MSG_PS_READ                      0x8006



/*
  Result of a read from persistent store

  FW->DSP

  Returns the value of a persistent store key (as a long message)

  The maximum size of key which can be read is limited to 64 words
  (63 in older firmware; see B-60183).

  A key which does not exist is read as if it had zero length.

  Arguments
    len      length of this message, including key and value
    key      the key which was read
    value    the words of the key's value
*/
#define KAL_LONG_MSG_PS_RESULT               0x8007



/*
  Indication of a failed read from persistent store

  FW->DSP

  Indicates that the result of a read could not be returned
  because resources on BlueCore were limited. It may be
  possible to try the read again later.

  Arguments
    key      the key which failed to be read
*/
#define KAL_MSG_PS_FAIL                      0x8008



/*
  Find physical flash address for a file

  DSP <-> FW

  Asks the firmware for the physical flash address for a given
  file specified by a FILE_INDEX. Any name to index lookup must
  be performed by the VM application.

  The same message number will be sent back, with the address
  supplied (or zero as the address if an error occurred.)

  Arguments

    index      the index of the file whose address we want
    low_addr   the lower 16 bits of the address (response only)
    high_addr  the upper 16 bits of the address (response only)
*/
#define KAL_MSG_FILE_ADDRESS                 0x8009



/*
  Request wall clock

  DSP->FW

  Request a wall clock from the firmware. This clock will be the
  same on both ends of a link (the link associated with the
  supplied Bluetooth address.) The clock will also have a
  timestamp from the chip's microsecond timer indicating when
  it was valid.

  Arguments
    word 0 - bits 47:32 of BDADDR
    word 1 - bits 31:16 of BDADDR
    word 2 - bits 15:0 of BDADDR
*/
#define KAL_MSG_GET_WALL_CLOCK               0x800A



/*
  Response to KAL_MSG_GET_WALL_CLOCK

  FW->DSP

  Note: this must be sent as a long message

  Arguments
    word 0 - bits 47:32 of BDADDR
    word 1 - bits 31:16 of BDADDR
    word 2 - bits 15:0 of BDADDR
    word 3 - MS 16bits of clock
    word 4 - LS 16bits of clock
    word 5 - MS 16bits of TIMER_TIME when the clock value was valid
    word 6 - LS 16bits of TIMER_TIME when the clock value was valid
*/
#define KAL_LONG_MSG_WALL_CLOCK_RESPONSE          0x800B



/*
  Response to KAL_MSG_GET_WALL_CLOCK indicating an error

  FW->DSP

  Arguments
    word 0 - bits 47:32 of BDADDR
    word 1 - bits 31:16 of BDADDR
    word 2 - bits 15:0 of BDADDR
*/
#define KAL_MSG_WALL_CLOCK_FAILED            0x800C



/*
  Read Random numbers

  DSP->FW

  Request the random numbers from firmware

  Arguments

    word0    Size of random numbers to read (in bits)
    word1    Request ID of the random number request
*/
#define KAL_MSG_RAND_REQ                      0x800D



/*
  Result of a random number request

  FW->DSP

  Returns the random numbers (as a long message)

  The maximum size of random numbers that can be returned is
  firmware's internal constant (e.g., 128 bits). If requested size is
  greater than this maximum limit then returned size is trimmed to
  this limit.

  Arguments
    len      length of this message, includes :-
                                Returned size
                                Req ID
                                Value:Random numbers
    Size     Number of random numbers returned (in bits)
    Req Id   Returns the same Request ID that was received
    Value    Sequence of random numbers
*/
#define KAL_LONG_MSG_RAND_RESULT               0x800E



/*
  Indication of a failed read for random numbers

  FW->DSP

  Indicates that the result of a request could not be returned
  because an error in BlueCore. It may be possible to try the read 
  again later.

  Arguments
    Req ID      Request ID as received in the request
*/
#define KAL_MSG_RAND_FAIL                      0x800F


/*
  Message to set Kalimba software watchdog

  DSP->FW

  Receipt of this by the firmware starts a one-shot
  timer; if another KAL_MSG_WATCHDOG_SET is not sent
  within that time, the firmware assumes that the DSP
  application has crashed. Currently this triggers a
  VM message MESSAGE_KALIMBA_WATCHDOG_EVENT to any
  registered Kalimba task, if the VM is present.

  The DSP can send this message with a delay of 0 to
  cancel any existing timer.

  Arguments
    Delay      Timeout in milliseconds (0 = disable)
*/
#define KAL_MSG_WATCHDOG_SET                     0x8010

/*
  Request the time of the next sniff anchor point 

  DSP->FW


  Arguments
    word 0 - bits 47:32 of BDADDR
    word 1 - bits 31:16 of BDADDR
    word 2 - bits 15:0 of BDADDR
*/
#define KAL_MSG_GET_NEXT_SNIFF_TIME            0x8011

/*
  Response to KAL_MSG_GET_NEXT_SNIFF_TIME

  FW->DSP

  Note: this must be sent as a long message

  Arguments
    word 0 - bits 47:32 of BDADDR
    word 1 - bits 31:16 of BDADDR
    word 2 - bits 15:0 of BDADDR
    word 3 - MS 16bits of clock
    word 4 - LS 16bits of clock
*/
#define KAL_LONG_MSG_GET_NEXT_SNIFF_TIME_RESPONSE   0x8012


/*
  Response to KAL_MSG_GET_NEXT_SNIFF_TIME indicating an error

  FW->DSP

  Arguments
    word 0 - bits 47:32 of BDADDR
    word 1 - bits 31:16 of BDADDR
    word 2 - bits 15:0 of BDADDR
*/
#define KAL_MSG_GET_NEXT_SNIFF_TIME_FAILED     0x8013


/*
  Request SCO parameters for the specified port

  DSP->FW

  Arguments
    word 0 - either type of port number
*/
#define KAL_MSG_GET_SCO_PARAMS          0x8014


/*
  SCO parameters for a port
  This will be sent when triggered by a KAL_MSG_GET_SCO_PARAMS
  but is also sent asynchronously whenever the SCO connection
  is renegotiated.

  FW->DSP

  Note: this must be sent as a long message

  Arguments
    word 0 - either type of port number
    word 1 - Tesco, in slots
    word 2 - Wesco, in slots
    word 3 - to-air packet length, in bytes
    word 4 - from-air packet length, in bytes
    word 5/6 - wall clock value for the start of the first
               reserved slot (MSW in 5, LSW in 6)
    word 7 - to-air processing time required by firmware, in us
    word 8 - from-air processing time required by firmware, in us
  (Words 7 and 8 indicate how much margin, relative to the start
   of the reserved slot, that the DSP needs to allow when sending
   or reading the data.)
*/
#define KAL_LONG_MSG_SCO_PARAMS_RESULT 0x8015


/*
  Response to KAL_MSG_GET_SCO_PARAMS indicating an error

  FW->DSP

  Arguments
    word 0 - either type of port number
*/
#define KAL_MSG_GET_SCO_PARAMS_FAILED   0x8016


/*
  Indicate that a port requires frame-based semantics

  DSP->FW

  Arguments
    word 0 - write port number
    word 1 - frame length (0 = use sample-based logic)
*/
#define KAL_MSG_SET_FRAME_LENGTH 0x8017


/*
  Get the BT address corresponding to a port

  DSP->FW

  Arguments
    word 0 - either type of port number
*/
#define KAL_MSG_GET_BT_ADDRESS 0x8018


/*
  Response to KAL_MSG_GET_BT_ADDRESS

  FW->DSP

  Arguments
    word 0 - 
        bits 0:6  - either type of port number
        bit 7     - set on failure
        bits 8:15 - type field of typed BDADDR
    word 1 - bits 47:32 of BDADDR
    word 2 - bits 31:16 of BDADDR
    word 3 - bits 15:0 of BDADDR
*/
#define KAL_MSG_PORT_BT_ADDRESS 0x8019
#define KAL_GET_BT_ADDRESS_FAILED (1 << 7)

/*
  Response to KAL_MSG_SET_FRAME_LENGTH

  DSP->FW

  Arguments
    word 0 - write port number
    word 1 - non-zero for success, zero for failure
*/
#define KAL_MSG_FRAME_LENGTH_RESPONSE 0x801A

/*
  The following messages are used by Baton.
*/

#define KAL_MSG_BATON_BASE      0x9000
#define KAL_MSG_BATON_LIMIT     0x91FF
/* Everything between these two inclusive is reserved for Baton */
/* We'll use 90xx for Kalimba->XAP and 91xx for XAP->Kalimba */

#define BATON_COMMAND_REPLY     0x9000
#define BATON_MSG_FROM_OPERATOR             0x9001
#define BATON_DSP_PANIC                     0x9002

/* Not in use:                              0x9100 */
#define BATON_CONNECT                       0x9101
#define BATON_START_OPERATOR                0x9102
#define BATON_STOP_OPERATOR                 0x9103
#define BATON_RESET_OPERATOR                0x9104
#define BATON_DESTROY_OPERATOR              0x9105
#define BATON_DISCONNECT                    0x9106
#define BATON_OPERATOR_MESSAGE              0x9107
/* Not in use:                              0x9108 */
#define BATON_CREATE_OPERATOR_C             0x9109
#define BATON_CREATE_OPERATOR_P             0x910a
#define BATON_DSPMANAGER_DEBUG              0x910b
#define BATON_SYNC_CONNECTIONS              0x910c
#define BATON_UNSYNC_CONNECTIONS            0x910d


/*
  The following messages are used internally by the firmware and DSP
  libraries to allow passing longer messages. They will never be seen
  by application code in the VM or the DSP.

  DSP <-> FW

  MSG_PART_BASE..MSG_PART_BASE+3 have a 4 word payload.

  If bit 1 is set then this is the initial packet and starts with the
  id and length of the long message

  If bit 2 is set then this is the final packet of the long message
*/

#define KAL_MSG_PART_BASE                    0xFFF0

#endif

