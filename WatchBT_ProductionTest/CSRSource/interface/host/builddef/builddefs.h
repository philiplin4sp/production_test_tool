/****************************************************************************
FILE
        builddefs.h  -  firmware build identifiers

DESCRIPTION
        A set of identifiers that describe some of the major build (make)
        options used to compile bc0x firmware builds.

        These are used by the BCCMD command BCCMDVARID_GET_NEXT_BUILDDEF and
        by the builddef subsystem.

MODIFICATION HISTORY
        1.1   07:dec:01  cjo    Created.
        1.2   10:dec:01  pws    Arbitrarily renamed BC02X to fit my build defs.
        1.3   11:dec:01  cjo    Typo.
        1.4   10:jan:02  pws    hci-13 #172: introduce TRANSPORT_USER def.
        1.5   12:mar:02  pws    H15.72: Add BUILDDEF_CHIP_BC02_KATO.
        1.6   09:apr:02  sm     H15.107: introduce TRANSPORT_NONE def.
        1.8   30:apr:02  pws    H15.143: add BUILDDEF_REQUIRE_8MBIT.
        1.9   22:may:02  pws    H15.177: add BUILDDEF_INSTALL_RADIOTEST_MODULE,
                                BUILDDEF_RADIOTEST_LITE
        1.10  10:jun:02  pws    H15.203: BUILDDEF_TRANSPORT_ALL comment.
        1.11  17:jul:02  at     H16.16: Added BUILDDEF_INSTALL_FLASH and
                                BUILDDEF_INSTALL_EEPROM.
        1.12  23:aug:02  at     H16.96: Added BUILDDEF_INSTALL_COMBO_DOT11.
        1.13  05:sep:02  pws    H16.117: Add BUILDDEF_LOWPOWER_TX.
        1.14  25:nov:02  pws    H17.70: Add BUILDDEF_TRANSPORT_H5.
        1.15  20:mar:03  pws    H18.72: Added BUILDDEF_COMPILER_GCC.
        1.16  26:mar:03  cjo    Added BUILDDEF_TRANSPORT_TWUTL as a
                                (#define) alias of BUILDDEF_TRANSPORT_H5.
        1.17  14:apr:03  pws    B-394: Definitions for new bc02/BC3 variants.
        1.19  21:may:03  pws    B-365: Add BUILDDEF_BT_VER_1_2.
        1.20  21:may:03  pws    B-540: Rename BUILDDEF_TRANSPORT_ALL{,_HCI}.
        1.21  20:aug:03  cjo    Comment typos.
        1.22  20:aug:03  cjo    Added BUILDDEF_INSTALL_UDP_MODULE.
                                Recommented the IPV4 and IPV6 entries.
        #25   09:jul:04  pws    B-3293: Naming and releasing from 19 on.
        #26   22:jul:04  sm     B-3432: Add BUILDDEF_TRANSPORT_H4DS.
        #27   03:aug:04  pws    B-3550: Add CHIP_BASE_BC4 and builddef.
        ------------------------------------------------------------------
        --- This modification history is now closed. Do not update it. ---
        ------------------------------------------------------------------
*/

#ifndef __BUILDDEFS_H__
#define __BUILDDEFS_H__


/****************************************************************************
A set of #defines mapping to major #defines that direct bc0x firmware
build time options.

The #defines must be in numerical order.

These must be #defines - enums might get changed by weird compilers.

These values must not be changed or removed. */


/* BUILDDEF_NONE is a special value - used to indicate "not a build
definition".  Typically used for list termination. */
#define BUILDDEF_NONE                           (0)

/* Firmware to run on one of the bc01 chips (bc01a, bc01b, etc.). */
#define BUILDDEF_CHIP_BASE_BC01                 (1)

/* Firmware to run on one of the bc02 chips (bc02x, bc02rom, etc.). */
#define BUILDDEF_CHIP_BASE_BC02                 (2)

/* Firmware to run on bc01b. */
#define BUILDDEF_CHIP_BC01B                     (3)

/* Firmware to run on bc02x (aka bc02ext). */
#define BUILDDEF_CHIP_BC02_EXTERNAL             (4)

/* ***DEPRECATED*** Vanilla HCI firmware build. */
#define BUILDDEF_BUILD_HCI                      (5)

/* ***DEPRECATED*** Vanilla RFCOMM firmware build. */
#define BUILDDEF_BUILD_RFCOMM                   (6)

/* Bluetooth specification version 1.1. */
#define BUILDDEF_BT_VER_1_1                     (7)

/*
 * Deprecated define. The individual transport defines should be used instead.
 *
 * All HCI host transports supported.  (Currently bcsp, usb and h4.)
 * Note that this does not include physical transports used by non-HCI
 * firmware or SDIO.
 */
#define BUILDDEF_TRANSPORT_ALL_HCI              (8)
/* Deprecated less specific name for the same thing. */
#define BUILDDEF_TRANSPORT_ALL                  (BUILDDEF_TRANSPORT_ALL_HCI)

/* BCSP host transport supported. */
#define BUILDDEF_TRANSPORT_BCSP                 (9)

/* H4 host transport supported. */
#define BUILDDEF_TRANSPORT_H4                   (10)

/* (BT device) USB host transport supported. */
#define BUILDDEF_TRANSPORT_USB                  (11)

/* Maximum effective encryption key length 56 bits. */
#define BUILDDEF_MAX_CRYPT_KEY_LEN_56           (12)

/* Maximum effective encryption key length 128 bits. */
#define BUILDDEF_MAX_CRYPT_KEY_LEN_128          (13)

/* Host transport with user access to UART supported. */
#define BUILDDEF_TRANSPORT_USER                 (14)

/* Firmware to run on the first BC02 ROM version, Kato (TSMC process). */
#define BUILDDEF_CHIP_BC02_KATO                 (15)

/* No host transport supported. */
#define BUILDDEF_TRANSPORT_NONE                 (16)


/* Requires 8 Mbit flash (requires 4 Mbit if not present). */
#define BUILDDEF_REQUIRE_8MBIT                  (18)

/* Radiotest module is present. */
#define BUILDDEF_INSTALL_RADIOTEST_MODULE       (19)

/* Radiotest module, if present, has reduced functionality. */
#define BUILDDEF_RADIOTEST_LITE                 (20)

/* Flash support is present. */
#define BUILDDEF_INSTALL_FLASH                  (21)

/* EEPROM support is present. */
#define BUILDDEF_INSTALL_EEPROM                 (22)

/* Support for IEEE 802.11b combo designs is present. */
#define BUILDDEF_INSTALL_COMBO_DOT11            (23)

/* Support for low power transmissions down to -90 dBm is present. */
#define BUILDDEF_LOWPOWER_TX                    (24)

/* Support for the H5 reliable UART transport defined by Bluetooth 1.2.
(Likely to be known as the "Three Wire UART Transport Layer", not H5. */
#define BUILDDEF_TRANSPORT_TWUTL                (25)
#define BUILDDEF_TRANSPORT_H5                   (BUILDDEF_TRANSPORT_TWUTL)

/* Stack was compiled using gcc instead of the Norcroft compiler. */
#define BUILDDEF_COMPILER_GCC                   (26)

/* Firmware for the second source BC2-ROM chip, Clouseau.
 * Note that BUILDDEF_CHIP_BC02_KATO may also be present. */
#define BUILDDEF_CHIP_BC02_CLOUSEAU             (27)

/* Firmare for the stripped down verions of Clouseau, Toulouse.
 * Note that Toulouse firmware may be reported as Clouseau firmware. */
#define BUILDDEF_CHIP_BC02_TOULOUSE             (28)

/* Firmware to run on the BC3 family of chips.
 * Note that BUILDDEF_CHIP_BASE_BC02 may also be present. */
#define BUILDDEF_CHIP_BASE_BC3                  (29)

/* Firmware for the basic BC3 ROM chip, Nicknack.
 * Note that BUILDDEF_CHIP_BC02_KATO may also be present. */
#define BUILDDEF_CHIP_BC3_NICKNACK              (30)

/* Firmware for the BC3 multimedia chip, Kalimba. */
#define BUILDDEF_CHIP_BC3_KALIMBA               (31)

/* The HCI interface is available. */
#define BUILDDEF_INSTALL_HCI_MODULE             (32)

/* The L2CAP interface is available. */
#define BUILDDEF_INSTALL_L2CAP_MODULE           (33)

/* The DM interface is available. */
#define BUILDDEF_INSTALL_DM_MODULE              (34)

/* The SDP interface is available. */
#define BUILDDEF_INSTALL_SDP_MODULE             (35)

/* The RFCOMM interface is available. */
#define BUILDDEF_INSTALL_RFCOMM_MODULE          (36)

/* The HID functionality is available. */
#define BUILDDEF_INSTALL_HIDIO_MODULE           (37)

/* The PAN interface is available. */
#define BUILDDEF_INSTALL_PAN_MODULE             (38)

/* The IPV4 functionality is available. */
#define BUILDDEF_INSTALL_IPV4_MODULE            (39)

/* The IPV6 functionality is available. */
#define BUILDDEF_INSTALL_IPV6_MODULE            (40)

/* The TCP interface is available. */
#define BUILDDEF_INSTALL_TCP_MODULE             (41)

/* Bluetooth specification version 1.2. */
#define BUILDDEF_BT_VER_1_2                     (42)

/* The UDP interface is available. */
#define BUILDDEF_INSTALL_UDP_MODULE             (43)

/* Require flash which doesn't need wait states */
#define BUILDDEF_REQUIRE_0_WAIT_STATES          (44)

/* Firmware for Paddywack, now known as BC3-flash. */
#define BUILDDEF_CHIP_BC3_PADDYWACK             (45)

/* Firmware for Coyote, now known as BC4-External. */
#define BUILDDEF_CHIP_BC3_COYOTE                (46)
#define BUILDDEF_CHIP_BC4_COYOTE                (BUILDDEF_CHIP_BC3_COYOTE)

/* Firmware for Oddjob, now known as BC4-ROM. */
#define BUILDDEF_CHIP_BC3_ODDJOB                (47)
#define BUILDDEF_CHIP_BC4_ODDJOB                (BUILDDEF_CHIP_BC3_ODDJOB)

/* H4DS host transport supported. */
#define BUILDDEF_TRANSPORT_H4DS                 (48)

/* Firmware for chips from Coyote and Oddjob with EDR */
#define BUILDDEF_CHIP_BASE_BC4                  (49)

#endif  /* __BUILDDEFS_H__ */
