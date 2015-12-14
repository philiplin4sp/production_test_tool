/*
 * fault_codes.h - THIS FILE IS A SINGLE X-MACRO TABLE _NOT_ A STANDARD HEADER
 * 
 * PLEASE NOTE THIS IS NOT A STANDARD HEADER FILE - you likely want fault_if.h
 *
 *
 * Header guards are not appropriate, this file should be '#include'd within
 * the body of the source file, after definition of specified macros. 
 * See below for details.
 * 
 */
 
/*
To use the data in fault_codes.h, use the following:

#define ...RELEVANT_FEATURES...
...
#define FAULT_IFACE_X(enum, hexv, sdesc, ldesc) case hexv:  return ldesc;
#define FAULT_IFACE_SEP 
 switch(fault)
   {
#include "fault_codes.h"
    default:
       return "";
   }
#undef FAULT_IFACE_X
#undef 
FAULT_IFACE_SEP

Definitions: 
RELEVANT_FEATURES: various #defines such as ON_CHIP_DEBUG
                   
These all have their usual meanings.

FAULT_IFACE_REMOVE_FOSSILS: 
                    Define this to remove any FOSSILized fault codes.
                    This is used to remove firmware definitions of
                    faults that can no longer be emitted, while
                    including them in host software so that it can
                    recognise faults in older chips.

FAULT_IFACE_X(enum, hexv, sdesc, ldesc): Standard X-Macro. Takes 4 arguments:
  enum - the name of the enum ie FAULT_MYSTERY
  hexv - The fault code value, supplied in hex ie 0x01
  sdesc - The short description of the error. Typically the enum minus 
          FAULT_ prefix, in lower case ie "mystery"
  ldesc - The long description, used in vm_debug. ie "General fault"
This should produce code being replaced by X-Macro


FAULT_IFACE_SEP: Separator between each FAULT_IFACE_X. Will usually be " " or ","



*/

/*  
    If you add public fault codes (below 0x80) the following will be updated: 
    
    The switch statement in ./devHost/vmdebug/vmdebug.cpp function  
    get_fault_message() accesses the long description
    string for the new fault code. 
    
    
    The Dictionary structure faultID_d  in ./devHost/btcli/dictionaries.c
    accesses the short description.
   
*/ 

/* Reasons to be cheerless. */


/* Do not use - it is used in the code to indicate "not a fault". */

FAULT_IFACE_X(NO_FAULT, 0x00,  
             "no_fault", "")   
FAULT_IFACE_SEP 
 
/* The useless catch-all.  Used to indicate a fault where no other 
appropriate fault code exists. */ 
 
FAULT_IFACE_X(FAULT_MYSTERY, 0x01, 
                   "mystery", 
                   "General fault")   
FAULT_IFACE_SEP 
 
/* No Bluetooth address.  An attempt is being made to run the firmware with 
no Bluetooth address.  (In practice this fault code should never be emitted 
as the default ("psrom") address will be used if no address has been put into 
the module's persistent store.  However, each module should be configured 
with a unique address.) */ 
 
FAULT_IFACE_X(FAULT_NO_BDADDR, 0x02, 
                   "no_bdaddr", 
                   "No Bluetooth address")   
FAULT_IFACE_SEP 
 
/* No Bluetooth country code.  An attempt is being made to run the firmware 
with no Bluetooth country code.  (In practice this fault code should never be 
emitted as the default ("psrom") country code will be used if no country code 
has been put into the module's persistent store.  In most cases the default 
will be the appropriate choice.) */ 
 
FAULT_IFACE_X(FAULT_NO_BDCOUNTRYCODE, 0x03, 
                   "no_bdcountrycode", 
                   "No Bluetooth country code")   
FAULT_IFACE_SEP 
 
/* No Bluetooth class-of-device code.  An attempt is being made to run the 
firmware with no Bluetooth class-of-device code.  (In practice this will 
never happen as the default ("psrom") class-of-device code will be used if no 
class-of-device code has been put into the module's persistent store.  In 
most cases the default will be the appropriate choice.) */ 
 
FAULT_IFACE_X(FAULT_NO_BDCOD, 0x04, 
                   "no_bdcod", 
                   "No Bluetooth class-of-device code")   
FAULT_IFACE_SEP 
 
/* An attempt to write to the ps has failed because there was insufficient 
free space. */ 
 
FAULT_IFACE_X(FAULT_PS_FULL, 0x05, 
                   "ps_full", 
                   "Attempt to write to the PS has failed due to insufficient free space")   
FAULT_IFACE_SEP 
 
/* An error has been detected in the radio state machine (part of an lc 
interrupt). */ 
 
FAULT_IFACE_X(FAULT_RSM_EVENT, 0x06, 
                   "rsm_event", 
                   "Error detected in the radio state machine")   
FAULT_IFACE_SEP 
                         
/* The code that blocks and unblocks interrupts has lost state.  This 
probably means that code has called unblock_interrupts() without first 
calling block_interrupts(). */ 
 
/* Unused as of v1.52 - upgraded to a panic */ 
#ifndef FAULT_IFACE_REMOVE_FOSSILS 
    FAULT_IFACE_X(FAULT_INTERRUPT_BLOCK, 0x07, 
                              "interrupt_block", 
                "The code that blocks and unblocks interrupts has lost state.")
    FAULT_IFACE_SEP 
#endif
/* An attempt has been made to unget data from one of the mmu buffers 
that was not previously taken from the buffer. */ 
 
FAULT_IFACE_X(FAULT_BUF_UNGET, 0x08, 
                   "buf_unget", 
 "An attempt was made to unget data that was not previously taken from the MMU buffer")   
FAULT_IFACE_SEP 
 
/* The lc (background) has lost state on an acl connection.  This should 
only be emitted where a connection's state data has been corrupted. */ 
 
FAULT_IFACE_X(FAULT_DUFF_ACLID, 0x09, 
                   "duff_aclid", 
                   "The LC has lost state on an ACL connection")   
FAULT_IFACE_SEP 
 
/* The host interface support code has been told by the hardware that a 
message has been received from the host but the code has found no 
corresponding information (in the protocol header buffer). */ 
 
FAULT_IFACE_X(FAULT_HOSTIO_HDR_NONE, 0x0a, 
                   "hostio_hdr_none", 
 "No information found in protocol header buffer to match message supposedly sent by host")   
FAULT_IFACE_SEP 
 
/* The host interface support code has detected a checksum error in a 
received BCSP packet.  This fault code currently isn't emitted. */ 
 
FAULT_IFACE_X(FAULT_HOSTIO_CHKSUM_ERROR, 0x0b, 
                   "hostio_chksum_error", 
                   "Checksum error detected in a received BCSP packet")   
FAULT_IFACE_SEP 
 
/* The host interface support code has detected an internal error. */ 
 
FAULT_IFACE_X(FAULT_HOSTIO_INTERNAL_ERROR, 0x0c, 
                   "hostio_internal_error", 
                   "Internal error detected")   
FAULT_IFACE_SEP 
 
/* The chip has received an BCSP ack for a message it hasn't sent. */ 
 
FAULT_IFACE_X(FAULT_HOSTIO_INVALID_ACK, 0x0d, 
                   "hostio_invalid_ack", 
                   "The chip received a BCSP ack for a message it did not send")   
FAULT_IFACE_SEP 
 
/* BCSP link failure.  This is not used in 12.X and later builds as link 
failure provokes a panic() which will normally reboot the chip. */ 
 
FAULT_IFACE_X(FAULT_HOSTIO_LINK_FAILED, 0x0e, 
                   "hostio_link_failed", 
                   "BCSP link failure")   
FAULT_IFACE_SEP 
 
/* The host interface support code has discarded an unexpected message 
(fragment) from a buffer.  This generally indicates coding error or an 
abuse of the chip's communications channels. */ 
 
FAULT_IFACE_X(FAULT_HOSTIO_DISCARDED_HCI_DATA, 0x0f, 
                   "hostio_discarded_hci_data", 
                   "An unexpected message has been discarded")   
FAULT_IFACE_SEP 
 
/* The host interface support code that converts between messages' internal 
and wire formats has detected an error.  At the time of writing this 
comment this fault code is used only to indicate that a message generated 
by the chip is incorrectly formatted, so this implies an internal error. */ 
 
FAULT_IFACE_X(FAULT_HOSTIO_CONVERT_FAILURE, 0x10, 
                   "hostio_convert_failure", 
                   "Error detected in conversion from message to wire format")   
FAULT_IFACE_SEP 
 
/* An attempt to discard a message fragment sent over USB has failed.  This 
should never occur, so the code indicates a gross internal error. */ 
 
FAULT_IFACE_X(FAULT_USB_BUFFER_ERROR, 0x11, 
                   "usb_buffer_error", 
 "An attempt to discard message fragment sent over USB has failed")   
FAULT_IFACE_SEP     
 
/* An attempt to calibrate the radio synthesiser has failed.  Calibration is 
normally during system initialisation.  It is sometimes acceptable to 
reattempt calibration - see BCCMD_RECAL. */ 
 
FAULT_IFACE_X(FAULT_SYNTH, 0x12, 
                   "synth", 
                   "Attempt to calibrate the radio synthesiser has failed")   
FAULT_IFACE_SEP 
 
/* The firmware has attempted to send a message to the host on a (BCSP or 
hci-extn) channel for which it has no data conversion/transfer handler. 
There are no handlers for unallocated channels (e.g., BCSP channel 15).  Also 
there are no handlers for HCI SCO and ACL data as these flows are dealt with 
separately for system performance.  This fault should never be emitted as 
it indicates a gross internal error. */ 
 
FAULT_IFACE_X(FAULT_HOSTIO_WRITE_UNHANDLED, 0x13, 
                   "hostio_write_unhandled", 
"Attempted to send a message to the host on a channel for which it has no data conversion")   
FAULT_IFACE_SEP 
 
/* An attempt to send a message to the host has failed.  This fault should 
never be emitted as it indicates a gross internal error. */ 
 
FAULT_IFACE_X(FAULT_HOSTIO_WRITE_FAILED, 0x14, 
                   "hostio_write_failed", 
                   "Attempt to send message to host has failed")   
FAULT_IFACE_SEP 
 
/* An attempt to send a message to the host has failed because the 
corresponding (BCSP or hci-extn) channel has been disabled.  This fault 
normally indicates a misconfiguration of the ps. */ 
 
FAULT_IFACE_X(FAULT_HOSTIO_WRITE_DISABLED, 0x15, 
                   "hostio_write_disabled", 
  "Attempt to send message to host has failed due to channel being disabled")   
FAULT_IFACE_SEP 
 
/* The host has sent a message to the chip on a (BCSP or hci-extn) channel 
for which there is no data conversion/transfer handler.  There are no 
handlers for unallocated channels (e.g., BCSP channel 15). */ 
 
FAULT_IFACE_X(FAULT_HOSTIO_READ_UNHANDLED, 0x16, 
                   "hostio_read_unhandled", 
 "Attempt to sned message on channel for which there is no data conversion/transfer handler")   
FAULT_IFACE_SEP 
 
/* An attempt to send a message from the host has failed because the 
corresponding (BCSP or hci-extn) channel has been disabled.  This fault 
normally indicates a misconfiguration of the ps or inappropriate traffic from 
the host. */ 
 
FAULT_IFACE_X(FAULT_HOSTIO_READ_DISCARDED, 0x17, 
                   "hostio_read_discarded", 
 "Attempt to send message from host has failed due to channel being disabled")   
FAULT_IFACE_SEP 
 
/* Either the chip received an unrecognised BCSP Link Establishment message 
from the host or the chip ran out of pool memory when processing a BCSP-LE 
message.  The latter is unlikely as BCSP-LE is used during system 
initialisation.  (BCSP-LE used to be called BCSP LM - serial link 
management.  The name was changed to avoid confusion with the 
Bluetooth LM.) */ 
 
FAULT_IFACE_X(FAULT_HOSTIO_READ_CONV_BCSPLE, 0x18, 
                   "hostio_read_conv_bcsple", 
                   "Unrecognised or discarded BCSP-LE message")   
FAULT_IFACE_SEP 
 
/* Either the chip received an unrecognised BCCMD command from the host or 
the chip ran out of pool memory when processing the message.  In either case 
the message has been discarded. */ 
 
FAULT_IFACE_X(FAULT_HOSTIO_READ_CONV_BCCMD, 0x19, 
                   "hostio_read_conv_bccmd", 
                   "Unrecognised or discarded BCCMD")   
FAULT_IFACE_SEP 
 
/* Either the chip received an unrecognised HQ response from the host or the 
chip ran out of pool memory when processing the message.  In either case the 
message has been discarded. */ 
 
FAULT_IFACE_X(FAULT_HOSTIO_READ_CONV_HQ, 0x1a, 
                   "hostio_read_conv_hq", 
                   "Unrecognised or discarded HQ response")   
FAULT_IFACE_SEP 
 
/* Either the chip received an unrecognised DM (Device Manager) message from 
the host or the chip ran out of pool memory when processing the message.  In 
either case the message has been discarded. */ 
 
FAULT_IFACE_X(FAULT_HOSTIO_READ_CONV_DM, 0x1b, 
                   "hostio_read_conv_dm", 
                   "Unrecognised or discarded DM message")   
FAULT_IFACE_SEP    /* Read convert fail DM. */ 
/* Either the chip received an unrecognised HCI command message from the host 
or the chip ran out of pool memory when processing the message.  In either 
case the message has been discarded. */ 
 
FAULT_IFACE_X(FAULT_HOSTIO_READ_CONV_HCI, 0x1c, 
                   "hostio_read_conv_hci",  
                   "Unrecognised or discarded HCI message")   
                   
FAULT_IFACE_SEP 
 
/* Either the chip received an unrecognised L2CAP (upper interface) command 
message from the host or the chip ran out of pool memory when processing the 
message.  In either case the message has been discarded. */ 
 
FAULT_IFACE_X(FAULT_HOSTIO_READ_CONV_L2CAP, 0x1d, 
                   "hostio_read_conv_l2cap", 
                   "Unrecognised or discarded L2CAP message")   
FAULT_IFACE_SEP 
 
/* Either the chip received an unrecognised RFCOMM (upper interface) command 
message from the host or the chip ran out of pool memory when processing the 
message.  In either case the message has been discarded. */ 
 
FAULT_IFACE_X(FAULT_HOSTIO_READ_CONV_RFCOMM, 0x1e, 
                   "hostio_read_conv_rfcomm", 
                   "Unrecognised or discarded RFCOMM message")   
FAULT_IFACE_SEP 
 
/* Either the chip received an unrecognised SDD/SDP (upper interface) command 
message from the host or the chip ran out of pool memory when processing the 
message.  In either case the message has been discarded. */ 
 
FAULT_IFACE_X(FAULT_HOSTIO_READ_CONV_SDD, 0x1f, 
                   "hostio_read_conv_sdd", 
                   "Unrecognised or discarded SDD message")   
FAULT_IFACE_SEP 
 
/* Either the chip received an unrecognised DEBUG channel from the host or 
the chip ran out of pool memory when processing the message.  In either case 
the message has been discarded.  The details of the debug channel's commands 
and responses will not be published. */ 
 
FAULT_IFACE_X(FAULT_HOSTIO_READ_CONV_DEBUG, 0x20, 
                   "hostio_read_conv_debug", 
                   "Unrecognised or discarded debug message")   
FAULT_IFACE_SEP 
 
/* Either the chip received an unrecognised DFU command message from the host 
or the chip ran out of pool memory when processing the message.  In either 
case the message has been discarded. */ 
 
FAULT_IFACE_X(FAULT_HOSTIO_READ_CONV_DFU, 0x21, 
                   "hostio_read_conv_dfu", 
                   "Unrecognised or discarded DFU message")   
FAULT_IFACE_SEP 
 
/* Either the chip received an unrecognised VM command message from the host 
or the chip ran out of pool memory when processing the message.  In either 
case the message has been discarded. */ 
 
FAULT_IFACE_X(FAULT_HOSTIO_READ_CONV_VM, 0x22, 
                   "hostio_read_conv_vm", 
                   "Unrecognised or discarded VM message")   
FAULT_IFACE_SEP 
 
/* The chip has received an H4 packet from the host that is either corrupt or 
contains unacceptable data.  For example, the packet may contain ACL data for 
an unrecognised HCI handle.  The packet has been discarded. */ 
 
FAULT_IFACE_X(FAULT_H4_RX_BAD_PDU, 0x23, 
                   "h4_rx_bad_pdu", 
 "Received an H4 packet that is corrupt or contains unacceptable data")   
FAULT_IFACE_SEP 
 
/* Battery state codes removed as battery monitor removed. */ 
 
#ifndef FAULT_IFACE_REMOVE_FOSSILS 
    FAULT_IFACE_X(FAULT_VBATT_LOW, 0x24, 
                       "vbatt_low", 
                       "Battery state low.")   
    FAULT_IFACE_SEP         /* Saggy battery. */
#endif
#ifndef FAULT_IFACE_REMOVE_FOSSILS 
    FAULT_IFACE_X(FAULT_VBATT_HIGH, 0x25, 
                       "vbatt_high", 
                       "Battery state high.")   
    FAULT_IFACE_SEP           /* Perky battery. */
#endif
/* Removed to prevent confusing hosts when a unexpected disconnect occurs  
during data transfer */ 
#ifndef FAULT_IFACE_REMOVE_FOSSILS 
    FAULT_IFACE_X(FAULT_HOSTIO_INVALID_HCI_HANDLE, 0x26, 
                       "hostio_invalid_hci_handle", 
                       "Invalid HCI handle.")   
    FAULT_IFACE_SEP  /*Invalid hci handle. */ 
#endif
/* The LC has received and acknowledged a point-to-point ACL packet from a 
(radio) peer.  It has then discarded the message because it has run out of 
memory.  This fault message indicates a gross error in the firmware resulting 
from data corruption or failing to perform flow control correctly.  The host 
should probably reboot the chip. */ 
 
FAULT_IFACE_X(FAULT_LC_INBOUND_PP_DATA_LOST, 0x27, 
                   "lc_inbound_pp_data_lost", 
 "The LC has discarded a point-to-point ACL message due to lack of memory")   
FAULT_IFACE_SEP 
 
/* As FAULT_LC_INBOUND_PP_DATA_LOST, but for point-to-point LMP traffic.  */ 
 
FAULT_IFACE_X(FAULT_LC_INBOUND_PP_LMP_LOST, 0x28, 
                   "lc_inbound_pp_lmp_lost", 
 "The LC has discarded a point-to-point LMP message due to lack of memory")   
FAULT_IFACE_SEP 
 
/* As FAULT_LC_INBOUND_PP_DATA_LOST, but for broadcast ACL traffic.  */ 
 
FAULT_IFACE_X(FAULT_LC_INBOUND_BC_DATA_LOST, 0x29, 
                   "lc_inbound_bc_data_lost", 
 "The LC has discarded a broadcast ACL message due to lack of memory")   
FAULT_IFACE_SEP 
 
/* As FAULT_LC_INBOUND_PP_DATA_LOST, but for broadcast LMP traffic.  */ 
 
FAULT_IFACE_X(FAULT_LC_INBOUND_BC_LMP_LOST, 0x2a, 
                   "lc_inbound_bc_lmp_lost", 
 "The LC has discarded a broadcast LMP message due to lack of memory")   
FAULT_IFACE_SEP 
 
/* An attempt to reset the lc has failed. */ 
 
FAULT_IFACE_X(FAULT_LC_RESET_FAIL, 0x2b, 
                   "lc_reset_fail", 
                   "Attempt to reset the LC has failed")   
FAULT_IFACE_SEP 
 
/* An attempt to send an LMP message has failed because the resources 
required could not be located. */ 
 
FAULT_IFACE_X(FAULT_LC_NO_LMP_BUFFER, 0x2c, 
                   "lc_no_lmp_buffer", 
 "Attempt to send a LMP message failed because resources could not be located")   
FAULT_IFACE_SEP 
 
/* An attempt to send an LMP message has failed because there was not 
enough resource to hold this message. */ 
 
FAULT_IFACE_X(FAULT_LC_NO_LMP_BUFFER_SPACE, 0x2d, 
                   "lc_no_lmp_buffer_space", 
 "Attempt to send a LMP message failed due to lack of resources")   
FAULT_IFACE_SEP 
 
/* A message received by the LC background had a bogus ID, beyond the 
range of lcbgmsgid_enum_length. */ 
 
FAULT_IFACE_X(FAULT_LC_BG_MSG_ID, 0x2e, 
                   "lc_bg_msg_id", 
                   "LC received message with out-of-range ID")   
FAULT_IFACE_SEP 
 
/* The "radioio" code (interface between LC and HCI for a firmware build with 
on-chip L2CAP) has been told that it has an ACL message (fragment), but 
when it went to pick up the message it had vanished.  This indicates a 
gross internal coding error, and should never be emitted.  The fault is not 
used on an HCI build. */ 
 
FAULT_IFACE_X(FAULT_RADIOIO_ACL_BUFFER_CORRUPT, 0x2f, 
                   "radioio_acl_buffer_corrupt", 
                   "Alerted to ACL message but then not found")   
FAULT_IFACE_SEP 
 
/* The value of a PSKEY was unusable.  The chip may have used 
a default value instead. */ 
 
FAULT_IFACE_X(FAULT_DUFF_PSBC_DATA, 0x30, 
                   "duff_psbc_data", 
                   "Value of a PSKEY was unusable")   
FAULT_IFACE_SEP 
 
/* An I2C-bus EEPROM has been detected but not configured. A BCCMD set of 
varid BCCMDVARID_E2_DEVICE is required before persistent store or VM 
application data can be written. */ 
 
FAULT_IFACE_X(FAULT_I2C_EEPROM_UNCONFIGURED, 0x31, 
                   "i2c_eeprom_unconfigured", 
                   "An I2C-bus EEPROM has been detected but not configured")   
FAULT_IFACE_SEP 
 
/* A maths error, such as a divide by zero, has occurred */ 
 
FAULT_IFACE_X(FAULT_MATHS_ERROR, 0x32, 
                   "maths_error", 
                   "A maths error has occurred")   
FAULT_IFACE_SEP 
 
/* No flash device was recognised and the firmware did not boot from ROM. 
This usually indicates that there is a hardware problem preventing correct 
identification of a connected flash, but it could also mean that an 
unsupported flash device is being used. */ 
 
FAULT_IFACE_X(FAULT_FLASH_NOT_RECOGNISED, 0x33, 
                   "flash_not_recognised", 
                   "No flash device was recognised and firmware didn't boot from ROM")   
FAULT_IFACE_SEP 
 
/* The LC has unexpectedly been asked to shutdown an ACL link 
(e.g. the link is already in the process of being shut down). */ 
 
FAULT_IFACE_X(FAULT_UNEXPECTED_ACL_SHUTDOWN, 0x34, 
                   "unexpected_acl_shutdown", 
                   "The LC has unexpectedly been asked to shutdown an ACL link")   
FAULT_IFACE_SEP 
 
/* The contents of a patch PS key are incorrect (wrong data, wrong key or 
wrong firmware version). */ 
 
FAULT_IFACE_X(FAULT_INVALID_PATCH, 0x35, 
                   "invalid_patch", 
                   "Contents of patch PSKEYs are incorrect")   
FAULT_IFACE_SEP 
 
/* The coexistence software has found an error in its configuration 
   settings or it has failed to inititalise the hardware for some 
   reason.  (Maybe the application is missing if we are using the 
   Kalimba). */ 
 
FAULT_IFACE_X(FAULT_COEX_MISSCONFIGURED, 0x36, 
                   "coex_misconfigured", 
 "Coexistence software found error in config or failed to initialise hardware")   
FAULT_IFACE_SEP 
 
/*  
  A call to init_usrpio() has attempted to bagsy a PIO line which has 
  already been bagsied. This will happen if two subsystems both try 
  to use the same PIO. The result will be confusion for anyone using 
  the PIO status to determine system beaviour. */ 
FAULT_IFACE_X(FAULT_ALREADY_BAGSIED, 0x37, 
                   "already_bagsied", 
 "Two subsystems have attempted to used the same PIO")   
FAULT_IFACE_SEP 
 
/* A memory address of 0x8000 or less was used as the destination for a 
   block copy command (such as memset or memcpy). This is dangerous as the 
   buffer subsystem cannot keep up with bc writes, leading to data corruption 
   or worse (see B-5521 for instance).  
   NOTE: this value must match that defined in memset.xap */ 
FAULT_IFACE_X(FAULT_BAD_BC_DEST_ADDRESS, 0x38, 
                   "bad_bc_dest_address", 
 "Dangerous memory address used for destination of block copy")   
FAULT_IFACE_SEP 
     
/* An attempt was made to destroy a buffer while it was still attached to the 
   PCM hardware. */ 
FAULT_IFACE_X(FAULT_IN_USE_PCM_BUFFER_DESTROYED, 0x39, 
                   "in_use_pcm_buffer_destroyed", 
 "Attempt made to destroy buffer while it was still attached to the PCM hardware")   
FAULT_IFACE_SEP 
 
/* Someone attempted to configure an odd size pmalloc pool. This is forbidden. 
   The firmware will round up the pool to the next even size. This may mean 
   that the pools no longer fit. */ 
FAULT_IFACE_X(FAULT_ODD_SIZE_PMALLOC_POOL, 0x3a, 
                   "odd_size_pmalloc_pool", 
 "Odd size pmalloc pool rounded up to next even size, may no longer fit")   
FAULT_IFACE_SEP 
 
/* An error occurred while attempting to route the FM audio. 
   It could be that invalid routing parameters were specified, or that the 
   audio resources requested were already in use. 
   (This fault number was called FAULT_FM_RDS_I2C for a while, but was only 
   ever used for audio routing failures.) */ 
FAULT_IFACE_X(FAULT_FM_AUDIO_ROUTING, 0x3b, 
                   "fm_audio_routing", 
                   "Error while attempting to route the FM audio")   
FAULT_IFACE_SEP 
 
/* Boot-time calibration of the external slow clock frequency failed; a 
   default frequency value has been assumed. This can indicate that the 
   external slow clock is broken or not present. 
   Functions that rely on the external slow clock (which, depending on 
   configuration, could include the FM radio or the entire chip on BC5-FM) 
   may not function correctly if the default frequency is incorrect (or 
   if the clock signal is broken). */ 
FAULT_IFACE_X(FAULT_EXT_SLOW_CLOCK_CAL_FAILED, 0x3c, 
                   "ext_slow_clock_cal_failed", 
 "Calibration of external slow clock frequency failed, default value has been assumed")   
FAULT_IFACE_SEP 
 
/* LC foreground watchdog triggered. 
   The LC watchdog is a fail-safe mechanism that should never trigger. 
   If it does trigger, it indicates that the Bluetooth digits have 
   locked up, or something similarly horrific. 
   NOTE: this fault will only ever be emitted if 
   PSKEY_LC_ENABLE_WATCHDOG_FAULT is set to true, which (at the time of 
   writing) it isn't by default. */ 
FAULT_IFACE_X(FAULT_LC_FG_WATCHDOG_BARKED, 0x3d, 
                   "lc_fg_watchdog_barked", 
 "Internal error: LC foreground watchdog triggered")   
FAULT_IFACE_SEP 
 
/* Flash copy of PSkeys were corrupt - this can be detected either 
   by the lack of a NULL key at the start of either key block, or 
   by a checksum error when walking the key list when doing a 
   free-space check. */ 
FAULT_IFACE_X(FAULT_PSFL_CORRUPT_OR_MISSING, 0x3e, 
                   "psfl_corrupt_or_missing", 
                   "Flash copy of PSKEYs were corrupt")   
FAULT_IFACE_SEP 
 
/* We timed out in (at least) one of the 8/10 bit ADC conversion spin loops. 
   This would generally be caused by the ADC not being clocked or the  
   internal state machine in it being jammed up in some way.  Post Elvis r04 
   A& JP r03 the ADC clocks are gated. */ 
FAULT_IFACE_X(FAULT_ADC_TENBIT_TIMEOUT, 0x3f, 
                   "adc_tenbit_timeout", 
                   "Internal error: timed out ADC conversion loop")   
FAULT_IFACE_SEP 
 
          /* We have received either an hci event from the 
             host, or we have received an unknown servide id 
             in the SDIO packet header. We will discard the packet 
             by emit a fault. */ 
FAULT_IFACE_X(FAULT_INVALID_SDIO_SERVICE_ID, 0x40, 
                   "invalid_sdio_service_id", 
                   "SDIO service ID unknown")   
FAULT_IFACE_SEP 
           
/* When using HCI_Disconnect, HCI_Reject_Synchronous_Connection_Request or  
HCI_Reject_Connection_Request it is possible to send an invalid reason code  
over the link.  We will let this happen and we emit this fault code to aid  
debugging a host stack. */ 
 
FAULT_IFACE_X(FAULT_HCI_INVALID_REASON_CODE, 0x41, 
                   "hci_invalid_reason_code", 
                   "Invalid reason code sent across link")   
FAULT_IFACE_SEP 
 
 
/* When calling hal_fm_relinquish_adc2 we have to guarantee that the calls are  
  balanced. The first call should be with TRUE and all subsequent calls should  
  be balanced. The only valid sequence is {TRUE FALSE}_n 
  where n is the number of times it is repeated  */ 
FAULT_IFACE_X(FAULT_HAL_FM_RELINQUISH_ADC2_UNBALANCED, 0x42, 
                   "hal_fm_relinquish_adc2_unbalanced", 
 "Internal error: unbalanced call to the HAL FM Relinquish ADC2 function")   
FAULT_IFACE_SEP 
 
 
/* When registering string descriptors specified in 
 * PSKEY_USB_STRING_DESCRIPTORS_MAP we have been passed an 
 * invalid string descriptor index (one that is greater than 255). 
 */ 
FAULT_IFACE_X(FAULT_USB_STRING_DESCRIPTORS_MAP, 0x43, 
                   "usb_string_descriptors_map", 
 "Invalid index (>255) when registering USB Descriptors from PSKEY_USB_STRING_DESCRIPTORS_MAP")   
FAULT_IFACE_SEP 
 
/* Fault code 0x44 was briefly allocated as follows during the 
 * development of B-32594, but was un-allocated before that 
 * work was finished. It was possible to emit it when reading 
 * reading the extended USB property feature descriptors. It 
 * is not believed that any firmware has been or will be released 
 * that can emit this fault code with this meaning. */ 
/*        FAULT_USB_PROP_DESCRIPTORS_MAP = 0x44,     */ 
 
/* An LMP message is received in packet type other than DM1 and DV */ 
FAULT_IFACE_X(FAULT_LC_LMP_IN_INVALID_PACKET, 0x44, 
                   "lc_lmp_in_invalid_packet", 
 "An LMP message packet type other than DM1 or DV has been received")   
FAULT_IFACE_SEP 
 
          /* An attempt to use the frequency counter hardware was made while it 
             was already in use by another part of the firmware. The second 
             attempt was denied access. */ 
FAULT_IFACE_X(FAULT_FREQ_COUNTER_IN_USE, 0x45, 
                   "freq_counter_in_use", 
 "Internal error: an attempt to use the frequency counter hardware was made whole it was already in use by another part of the firmware")   
FAULT_IFACE_SEP 
 
/* While attempting to calibrate the slow clock, a large value for the period 
 * was obtained, indicating that the clock is slower than the minimum allowed 
 * frequency (25 MHz on BC7) 
 */ 
FAULT_IFACE_X(FAULT_SLOW_CLOCK_TOO_SLOW, 0x46, 
                   "slow_clock_too_slow", 
 "Calibration of the Slow Clock indicates that it is slower then minimum allowed frequency")   
FAULT_IFACE_SEP 
 
        /* There was a hardware fault in the FM LO leading to a 
         * misconfuration */ 
FAULT_IFACE_X(FAULT_HAL_FM_LO, 0x47, 
                   "hal_fm_lo", 
 "Internal error: there was a hardware fault in the FM LO leading to a misconfiguration")   
FAULT_IFACE_SEP 
 
        /* An attempt has been made to turn on BOTH the FM rx and tx*/ 
FAULT_IFACE_X(FAULT_FM_API_COMMITTED, 0x48, 
                   "fm_api_committed", 
 "An attempt has been made to turn on both FM receiver and transmitter simultaneously")   
FAULT_IFACE_SEP 
 
        /* There was a non-fatal hardware fault when attempting to configure 
         * the FM transmitter. */ 
FAULT_IFACE_X(FAULT_HAL_FM_TX, 0x49, 
                   "hal_fm_tx", 
 "Internal error: there was a non-fatal hardware fault when attempting to configure the FM transmitter")   
FAULT_IFACE_SEP 
 
/* 
 * Firmware is the wrong version for the chip revision. 
 * If you continue after seeing this fault, any weirdness you see is 
 * your own problem. 
 * (This is a less-drastic version of PANIC_WRONG_ANA_HARDWARE_VERSION; 
 * see the description of that for how this mechanism is configured 
 * and when it should be used.) 
 */ 
FAULT_IFACE_X(FAULT_WRONG_ANA_HARDWARE_VERSION, 0x4a, 
                   "wrong_ana_hardware_version", 
 "Firmware is the wrong version for this chip revision")   
FAULT_IFACE_SEP 
 
    /* Someone has tried to claim or release a running ADC. */ 
FAULT_IFACE_X(FAULT_ADC_RUNNING, 0x4b, 
                   "adc_running", 
 "Internal error: a claim or release on a running ADC has been made")   
FAULT_IFACE_SEP 
 
/* An LMP timeout has fired for a dead link or for a transaction that has 
   already terminated. */ 
 
FAULT_IFACE_X(FAULT_LM_SPURIOUS_TIMEOUT, 0x4c, 
                   "lm_spurious_timeout", 
 "An LMP timeout has occurred for an inactive link or transaction that has already terminated")   
FAULT_IFACE_SEP 
 
    /* No system reference clock was detected. BlueCore will have 
     * significantly degraded performance, i.e. Bluetooth will not work. 
     * This should only occur with an incomplete application design, or if 
     * BlueCore is not soldered to the PCB correctly. */ 
FAULT_IFACE_X(FAULT_NO_REFERENCE_CLOCK, 0x4d, 
                   "no_reference_clock", 
 "No system reference clock was detected, Bluetooth performance will be significantly degraded")   
FAULT_IFACE_SEP 
 
    /* Unbalanced calls to suspend_resolve_enables() detected */
    /* Removed, no longer used after audio refactor */ 
#ifndef FAULT_IFACE_REMOVE_FOSSILS 
FAULT_IFACE_X(FAULT_SUSPEND_RESOLVE_ENABLES_REF_CNT, 0x4e, 
                   "suspend_resolve_enables_ref_cnt", 
 "Internal error (audio): unbalanced calls to suspend_resolve_enables() detected")   
FAULT_IFACE_SEP 
#endif
 
    /* LPC program memory is writable. */ 
FAULT_IFACE_X(FAULT_LPC_PMEM_WRITABLE, 0x4f, 
                   "lpc_pmem_writable", 
                   "LPC program memory is writable")   
FAULT_IFACE_SEP 
 
    /* FastPipe subsystem has received a flow control token with an invalid 
     * pipe ID from the host. */ 
FAULT_IFACE_X(FAULT_FASTPIPE_INVALID_ID, 0x50, 
                   "fastpipe_invalid_id", 
 "FastPipe subsystem has received a flow control token with an invalid pipe ID from the host")   
FAULT_IFACE_SEP 
 
    /* Attempted simultaneous use of one of the "BIST" ADCs by firmware 
     * and hardware was detected. The result of the firmware's ADC 
     * use is indeterminate and probably incorrect. This is indicative 
     * of a firmware design fault. */ 
FAULT_IFACE_X(FAULT_ADC_STOLEN, 0x51, 
                   "adc_stolen", 
 "Internal error: attempted simultaneous use of one of the BIST ADCs by firmware and hardware was detected")   
FAULT_IFACE_SEP 
 
    /* As an advertiser or slave we have received a channel map from the 
     * master containing less than a certain minimum channels enabled 
     */ 
FAULT_IFACE_X(FAULT_BLE_INVALID_CHANNEL_MAP_RECEIVED, 0x52, 
                   "ble_invalid_channel_map_received", 
 "Channel map from master received with less then minimum of channels enabled")   
FAULT_IFACE_SEP 
 
    /* We are attempting to install ULP radio parameters more than once 
     */ 
FAULT_IFACE_X(FAULT_BLE_UNBALANCED_RADIO_SETUP, 0x53, 
                   "ble_unbalanced_radio_setup", 
                   "Attempt to install ULP radio parameters more than once")   
FAULT_IFACE_SEP 
 
    /* We are trying to run a ULP operation with BT radio settings 
     * or vice versa */ 
FAULT_IFACE_X(FAULT_BLE_INCORRECT_RADIO_PARAMS, 0x54, 
                   "ble_incorrect_radio_params", 
 "Attempt to configure ULP with BT radio setting or visa versa")   
FAULT_IFACE_SEP 
 
        /* There was a request to use the FMTX  
        antenna when it is uncalibrated*/ 
FAULT_IFACE_X(FAULT_HAL_FMTX_ANT_UNCALIBRATED, 0x55, 
                   "hal_fmtx_ant_uncalibrated", 
 "Attempt to use FM tx antenna before it has been calibrated")   
FAULT_IFACE_SEP 
 
    /* There's a BT power table in the PSKEY tif space which isn't the 
     * one that this firmware wants. Warns the customer that they need 
     * to generate a correct-format power table. Meantime, we'll use 
     * the ROM default (rather than try munging theirs) */ 
FAULT_IFACE_X(FAULT_WRONG_BT_POWERTABLE_FOUND, 0x56, 
                   "wrong_bt_powertable_found", 
 "PSKEY BT Power table format is incorrect, using ROM default instead")   
FAULT_IFACE_SEP 
 
    /* Refless: esco count should always be ge 0, oddly enough. If this 
     * occurs our logic has gone wrong, but the chances are that the 
     * device will still continue to function. */ 
FAULT_IFACE_X(FAULT_REFLESS_ESCO_COUNT_NEGATIVE, 0x57, 
                   "refless_esco_count_negative", 
                   "Refless ESCO count is negative")   
FAULT_IFACE_SEP 
 
    /* Hostio redirects HCI buffers to dummy ('freezes' them) when links 
     * are disconnected to avoid issues with the buffers being used by 
     * the host after they had been destroyed.  Only expected to be seen 
     * in test scenarios, but that includes tests by customers - and not  
     * otherwise easy to find. 
     */ 
FAULT_IFACE_X(FAULT_HOSTIO_FROZEN_HANDLES, 0x58, 
                   "hostio_frozen_handles", 
                   "HCI Buffer access to frozen links after disconnection")   
FAULT_IFACE_SEP 
 
    /* An inconsistent set of default routings have been specified for the 
     * audio interfaces */ 
FAULT_IFACE_X(FAULT_PCM_DEFAULT_AUDIO_ROUTING, 0x59, 
                   "pcm_default_audio_routing", 
 "Inconsistent default audio routing parameters have been configured")   
FAULT_IFACE_SEP 
 
    /* Someone tried to use clock switching without configuring it properly */ 
FAULT_IFACE_X(FAULT_CLOCK_SWITCHING_MISCONFIGURED, 0x5a, 
                   "clock_switching_misconfigured", 
                   "Clock switching has been misconfigured")   
FAULT_IFACE_SEP 
 
    /*IQ trim has overrun*/ 
FAULT_IFACE_X(FAULT_PERIODIC_TRIM_OVERRUN, 0x5b, 
                   "periodic_trim_overrun", 
                   "Internal error: IQ trim has overrun")   
FAULT_IFACE_SEP 
 
/*  Removed, no longer do a check on the MMU clock for baud rate detect */ 
#ifndef FAULT_IFACE_REMOVE_FOSSILS 
FAULT_IFACE_X(FAULT_AUTOBAUD_MMU_CLOCK, 0x5c, 
                   "autobaud_mmu_clock", 
 "Internal error: MMU clock not as expected during baud rate detection")    
FAULT_IFACE_SEP  
#endif
 
    
 
    /* Couldn't reserve space for PSKEY_PMALLOC_BASE */ 
FAULT_IFACE_X(FAULT_PMALLOC_BASE_INSUFFICIENT_SPACE, 0x5e, 
                   "pmalloc_base_insufficient_space", 
                   "Could not reserve space for PSKEY_PMALLOC_BASE")   
FAULT_IFACE_SEP 
    /* Couldn't reserve space for PSKEY_PMALLOC_PER_ACL */ 
FAULT_IFACE_X(FAULT_PMALLOC_ACL_INSUFFICIENT_SPACE, 0x5f, 
                   "pmalloc_acl_insufficient_space", 
                   "Could not reserve space for PSKEY_PMALLOC_PER_ACL")   
FAULT_IFACE_SEP 
    /* Couldn't reserve space for PSKEY_PMALLOC_PER_SCO */ 
FAULT_IFACE_X(FAULT_PMALLOC_SCO_INSUFFICIENT_SPACE, 0x60, 
                   "pmalloc_sco_insufficient_space", 
                   "Could not reserve space for PSKEY_PMALLOC_PER_SCO")   
FAULT_IFACE_SEP 
    /* Couldn't reserve space for PSKEY_PMALLOC_APP */ 
FAULT_IFACE_X(FAULT_PMALLOC_APP_INSUFFICIENT_SPACE, 0x61, 
                   "pmalloc_app_insufficient_space", 
                   "Could not reserve space for PSKEY_PMALLOC_APP")   
FAULT_IFACE_SEP 
 
    /* A PSKEY_PMALLOC_* key has a zero or odd length */ 
FAULT_IFACE_X(FAULT_PMALLOC_INVALID_PSKEY_LENGTH, 0x62, 
                   "pmalloc_invalid_pskey_length", 
                   "A PSKEY_PMALLOC_* key has a zero or odd length")   
FAULT_IFACE_SEP 
    /* A pool entry in one of the PSKEY_PMALLOC_* keys is incorrect: 
     * e.g. zero or very large. 
     */  
FAULT_IFACE_X(FAULT_PMALLOC_INVALID_POOL_ENTRY, 0x63, 
                   "pmalloc_invalid_pool_entry", 
                   "A pool entry in one of the PSKEY_PMALLOC_* keys is incorrect")   
FAULT_IFACE_SEP 
    /* A pool entry in one of the PSKEY_PMALLOC_* keys is requesting 
     * an odd pool size. Pool sizes must be even. 
     */ 
FAULT_IFACE_X(FAULT_PMALLOC_ODD_POOL_SIZE, 0x64, 
                   "pmalloc_odd_pool_size", 
 "A pool entry in one of the PSKEY_PMALLOC_* keys is requesting an odd pool size. Pool sizes must be even")   
FAULT_IFACE_SEP 
 
    /* Something has gone terribly wrong with the thermometer. */ 
FAULT_IFACE_X(FAULT_INVALID_TEMPERATURE, 0x65, 
                   "invalid_temperature", 
                   "Hardware temperature sensor outside valid range")   
FAULT_IFACE_SEP 
 
    /* The ANA regulator voltage is screwy, probably caused by a badly soldered 
     * chip */ 
FAULT_IFACE_X(FAULT_ANA_REGULATOR_VOLTAGE, 0x66, 
                   "ana_regulator_voltage", 
                   "ANA regulator voltage is out of range")   
FAULT_IFACE_SEP 
 
    /* gps buffer initialisation failure. */ 
FAULT_IFACE_X(FAULT_GPS_BUFFER, 0x67, 
                   "gps_buffer", 
                   "Creating GPS buffer's failed")   
FAULT_IFACE_SEP 
 
    /* GPS VCO calibration failure. */ 
FAULT_IFACE_X(FAULT_GPS_VCO_CALIBRATION, 0x68, 
                   "gps_vco_calibration", 
                   "GPS VCO calibration failed")   
FAULT_IFACE_SEP 
 
    /* Firmware received a Kalimba DSP message it doesn't support. */ 
FAULT_IFACE_X(FAULT_KALIMBA_UNSUPPORTED_MSG, 0x69, 
                   "kalimba_unsupported_msg", 
 "An unsupported message was sent from a DSP application to the firmware")   
FAULT_IFACE_SEP 
 
    /* PCM can't figure out the PCM master clock rate while trying to enable 
     * the interface. 
     */ 
FAULT_IFACE_X(FAULT_PCM_INVALID_CLOCK_RATE, 0x6a, 
                   "pcm_invalid_clock_rate", 
                   "pcm invalid clock rate")   
FAULT_IFACE_SEP 
 
    /* Patch bundle checksum failed, or something else went wrong during 
     * checksum. 
     */ 
FAULT_IFACE_X(FAULT_INVALID_PATCH_BUNDLE, 0x6b, 
                   "invalid_patch_bundle", 
                   "The checksum on a patch bundle failed")   
FAULT_IFACE_SEP 
     
    /* As an advertiser we have received a CONNECT_REQ with wrong values */ 
FAULT_IFACE_X(FAULT_BLE_INVALID_CONN_REQ_PARAM_RECEIVED, 0x6c, 
                   "ble_invalid_conn_req_param_received", 
                   "Connection request contains invalid parameters")   
FAULT_IFACE_SEP 
 
    /* The host has sent an ACL (includes FastPipe) packet with no payload */ 
FAULT_IFACE_X(FAULT_HCI_ACL_PACKET_NO_PAYLOAD, 0x6d, 
                   "hci_acl_packet_no_payload", 
                   "Host has sent an ACL (includes FastPipe) packet with no payload")   
FAULT_IFACE_SEP 
     
    /* Trying to deep sleep using the internally divided fast reference clock 
     * as the timing source but we don't think we are using a quartz crystal 
     */ 
FAULT_IFACE_X(FAULT_SLOW_CLOCK_INT_DIV_FREF_NO_XTAL, 0x6e, 
                   "int_div_fref_slow_clock_no_xtal", 
 "Deep sleep timing configured using internally divided fast reference but no quartz crystal present")   
FAULT_IFACE_SEP 
 
    /* Detected a problem with audio hardware configuration parameters 
     * (eg invalid codec sample rate) 
     */ 
FAULT_IFACE_X(FAULT_AUDIO_INVALID_CONFIG, 0x6f, 
                   "audio_invalid_config", 
                   "The audio hardware configuration parameters are invalid")   
FAULT_IFACE_SEP 
 
    /* In the buffer subsystem, an attempt to map in an area greater than 
     * 4k words was detected, which cannot reliably be done.  This suggests 
     * a firmware bug (not checking length) or corrupt data (giving an 
     * illegally oversized message).  Only applicable to BC7 or later. */ 
FAULT_IFACE_X(FAULT_BUF_MAP_REQ_TOO_LARGE, 0x70, 
                   "buf_map_req_too_large", 
                   "An invalid (too large) buffer mapping request was detected")   
FAULT_IFACE_SEP 
 
    /* In the ULP/BLE code, we have received a CONNECTION_UPDATE linklayer message 
     * with parameters that are invalid */ 
FAULT_IFACE_X(FAULT_BLE_INVALID_CONN_UPDATE_PARAM_RECEIVED, 0x71, 
                   "ble_invalid_conn_update_received", 
                   "BLE Connection update request contained invalid parameters")   
FAULT_IFACE_SEP 
 
    /* In the audio driver, a change was requested which requires the interface to
     * be in active, but at the time of the call the interface was already active */
FAULT_IFACE_X(FAULT_AUDIO_INTERFACE_NOT_INACTIVE, 0x72, 
                   "audio_interface_not_inactive", 
                   "The audio interface was not inactive while trying to make a change")   
FAULT_IFACE_SEP 

/* Previn failed to do a disconnection when requested by Stibbons.
 * For a disconnect request from host, the failure indication is sent in the
 * bccmd response and a fault is not raised. */
FAULT_IFACE_X(FAULT_DISCONNECT_FAILED_WITHIN_DSPMANAGER, 0x73, 
                   "disconnect_failed_within_dspmanager", 
                   "The DSP component of DSPmanager failed to do a disconnect when requested by another part of the firmware")
FAULT_IFACE_SEP

/* Stibbons failed to send a message to Previn.
 * Note that when a bccmd is involved, the failure indication is sent in the
 * response and a fault is not raised. */
FAULT_IFACE_X(FAULT_SEND_TO_DSPMANAGER_COMPONENT_IN_DSP, 0x74, 
                   "send_to_dspmanager_component_in_dsp", 
                   "The firmware component of DSPmanager was unable to send a message to the DSP component")
FAULT_IFACE_SEP

/* Previn sent an unexpected message to Stibbons. */
FAULT_IFACE_X(FAULT_DSPMANAGER_COMPONENT_IN_DSP_BAD_MESSAGE, 0x75, 
                   "dspmanager_component_in_dsp_bad_message", 
                   "The DSP component of DSPmanager sent an invalid message to the firmware component.")
FAULT_IFACE_SEP

/* Unexpected call sequence in the kalimba_loader subsystem.
 *
 * This could be the result of a broken DSP application (particularly one that
 * emits a KALIMBA_READY message at the wrong time), a misbehaving DSP
 * debugger on the host, or, as always, a failure internal to the BlueCore
 * firmware.
 *
 * (This is a fault rather than a panic to avoid a broken user-written
 * DSP program locking the device in a cycle of continuous resets.) */
FAULT_IFACE_X(FAULT_DSP_LOAD_SEQUENCING_ERROR, 0x76,
                   "dsp_load_sequencing_error",
                   "Sequencing error in DSP loading process")
FAULT_IFACE_SEP

/* Invalid file system address access 
 *
 * The file system access is gaurded with an address range sanity check 
 * to protect against any possible illegal access from one file system 
 * (like an external SQIF) to another (like an internal file system)*/
FAULT_IFACE_X(FAULT_FILE_SYSTEM_ADDR_RANGE_CHK_FAIL, 0x77,
                   "invalid_fs_address_access",
                   "Sanity check on the valid address range in the file system access failed ")
FAULT_IFACE_SEP
 
/* When calibrationg the audio ADC (on Gordon), the calibration complete
   status bit should be set within 5 usec. This fault is generated if
   the bit is not set after 10 usec. */
FAULT_IFACE_X(FAULT_AUDIO_ADC_CALIBRATION_TIMEOUT, 0x78, 
                   "audio_adc_calibration_timeout", 
                   "Calibration complete status bit not set after audio ADC calibration")
FAULT_IFACE_SEP 

/* Deep sleep code exceeded the configured buffer size. */
FAULT_IFACE_X(FAULT_SLEEP_CODE_SIZE_EXCEEDED, 0x80, 
                   "sleep_code_size_exceeded", 
                   "Deep Sleep code has exceeded it's allocated buffer size.")
FAULT_IFACE_SEP


    /*****************************************************************  
     * WARNING: FAULT CODE 0xEB WAS USED BY MISTAKE. PLEASE FILL IN 
     *          THE GAP BEFORE THIS WARNING.
     *
     *          WHOEVER ADDS FAULTCODE 0xEA PLEASE DELETE THIS WARNING 
     *****************************************************************/ 

 
#ifdef CHIP_JEMIMA 
 /* CHIP_JEMIMA is being used as it will prevent inclusion in host tools.
    This is a hack - a better conditional ought to be used. 
*/

/* An operation is being attempted that may not succeed because the audio 
   codec is currently in FM bypass mode */ 
    FAULT_IFACE_X(FAULT_FM_BYPASS_ENABLED, 0xeb, "0xeb", "Fault 0xeb")   
    FAULT_IFACE_SEP 
#endif


    /*****************************************************************  
     * WARNING: FAULT CODE 0xFE WAS USED BY MISTAKE. PLEASE FILL IN 
     *          THE GAP BEFORE THIS WARNING.
     *
     *          WHOEVER ADDS FAULTCODE 0xFD PLEASE DELETE THIS WARNING 
     *****************************************************************/ 

/* The code for the HCI Hardware Error Event used to signal H4 link 
synchronisation failure, as described in bc01-s-030. */ 
       
FAULT_IFACE_X(FAULT_H4_LINK_SYNC_FAILURE, 0xfe, 
                   "h4_link_sync_failure", 
                   "H4 link synchronisation has failed")
FAULT_IFACE_SEP 

    /*****************************************************************  
     * BELOW THIS POINT ARE DEBUG FAULT CODES THAT SHOULD NOT APPEAR
     * IN ANY BUILD.
     *
     * DEBUG FAULT CODES COUNT UP FROM 0X4000, PLEASE APEND NEW 
     * DEBUG FAULT CODES TO THE BOTTOM OF THE LIST.
     *****************************************************************/ 
    
#if defined(ON_CHIP_DEBUG) && defined(DEBUG_RADIO_BUFFER_ERRORS)
    FAULT_IFACE_X(FAULT_IGNORED_BUFFER_ERROR, 0x4000, "", "")   
    FAULT_IFACE_SEP 
#endif /*defined(ON_CHIP_DEBUG) && defined(DEBUG_RADIO_BUFFER_ERRORS) */ 


#ifdef ON_CHIP_DEBUG
/* Used when debugging is turned on.  Note that we have been known 
to release builds such as this to selected customers. */ 
 
/* An attempt to send a debugging message as an HCI manufacturer extension 
failed because of the lack of memory.  It's confusing if these simply 
disappear, so we report a fault to warn the poor sap debugging. */ 
    FAULT_IFACE_X(FAULT_DEBUG_NO_MEMORY, 0x4001, "", "")   
    FAULT_IFACE_SEP 
 
/* An attempt to send a debugging message as an HCI manufacturer extension 
   failed because the xput_message or fp_put_message reported a failure. */ 
    FAULT_IFACE_X(FAULT_DEBUG_PUT_MESSAGE_FAILED, 0x4002, "", "")   
    FAULT_IFACE_SEP 
#endif /* ON_CHIP_DEBUG */ 
 
#ifdef FAULT_ON_INSOMNIA
/* An attempt to deep sleep failed twice. This is likely indicative of a 
   hardware error. */ 
    FAULT_IFACE_X(FAULT_INSOMNIA, 0x4003, "", "")   
    FAULT_IFACE_SEP 
#endif /*FAULT_ON_INSOMNIA */ 
 
/* Temporary faults for debugging feel free to reuse don't bother to comment */ 
#ifdef LC_ERROR_RECOVERY_DEBUG_FAULT_CODES
    FAULT_IFACE_X(FAULT_LC_UNEXPECTED_AUX1_PACKET, 0x4004, "", "")   
    FAULT_IFACE_SEP /* What's that doing here */ 
    FAULT_IFACE_X(FAULT_LC_NOISY_PACKET_HEADER, 0x4005, "", "")   
    FAULT_IFACE_SEP  /* Could be corrupt - drop */ 
    FAULT_IFACE_X(FAULT_LC_DUBIOUS_PACKET_HEADER, 0x4006, "", "")   
    FAULT_IFACE_SEP/* Hmmm - drop to be safe */ 
    FAULT_IFACE_X(FAULT_LC_PACKET_HEADER_BURST_NOISE, 0x4007, "", "")   
    FAULT_IFACE_SEP /* Dynamic threshold */ 
    FAULT_IFACE_X(FAULT_SYNTH_TRIM_RECOVERY, 0x4008, "", "")   
    FAULT_IFACE_SEP /* Fell back to trim all */ 
#endif
 
#ifdef DEBUG_SWITCH 
    FAULT_IFACE_X(FAULT_SWITCH_ACL_PENDING, 0x4009, "", "")   
    FAULT_IFACE_SEP 
    FAULT_IFACE_X(FAULT_SWITCH_TIMEOUT, 0x400a, "", "")   
    FAULT_IFACE_SEP 
    FAULT_IFACE_X(FAULT_SWITCH_SHUTDOWN, 0x400b, "", "")   
    FAULT_IFACE_SEP 
    FAULT_IFACE_X(FAULT_SWITCH_LMP_PENDING, 0x400c, "", "")   
    FAULT_IFACE_SEP 
#endif 
       
#ifdef INSTALL_MCDSP_SHUNT 
    /* Recoverable faults in the hostio shunt runtime code. */ 
    FAULT_IFACE_X(FAULT_HOSTIO_SHUNT_UNEXPECTED_START_PKT, 0x400d, "", "")   
    FAULT_IFACE_SEP 
    FAULT_IFACE_X(FAULT_HOSTIO_SHUNT_UNEXPECTED_CONT_PKT, 0x400e, "", "")   
    FAULT_IFACE_SEP 
    FAULT_IFACE_X(FAULT_HOSTIO_SHUNT_INVALID_START_PKT, 0x400f, "", "")   
    FAULT_IFACE_SEP 
    FAULT_IFACE_X(FAULT_HOSTIO_SHUNT_INVALID_CONT_PKT, 0x4010, "", "")   
    FAULT_IFACE_SEP 
#endif
 
/* The next fault code is just for the end of the list, it shouldn't be
used as a real fault code. */ 
       
FAULT_IFACE_X(FAULT_MAX_FAULT_CODE, 0x7fff, "", "")

 /*Please note last item in list must not have FAULT_IFACE_SEP macro appended */

