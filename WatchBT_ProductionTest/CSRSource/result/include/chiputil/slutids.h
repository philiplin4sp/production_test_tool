#ifndef SLUTIDS_H
#define SLUTIDS_H

/**********************************************************************
*
*  FILE   :  slutids.h
*
*            Copyright (C) Cambridge Silicon Radio Ltd 2005-2009
*
*  PURPOSE:  SLUT ids.
*
*  $Id: //depot/bc/bluesuite_2_4/devHost/util/chiputil/slutids.h#1 $
*
***********************************************************************/

namespace bluecore
{

/* Build identifiers. */
typedef enum SYMBOL_ID
{
    SLUTID_NONE             = 0,
    SLUTID_ID_STRING        = 7,    /* Build identifier string. */
    SLUTID_ID_INTEGER       = 8,    /* Build identifier integer. */
    SLUTID_BCCMD_INTERFACE  = 9,    /* BCCMD interface available */
    SLUTID_ID_STRING_FULL   = 13    /* Long build identifier string. */
} SYMBOL_ID;

}

namespace unifi
{

/* This a munged version of the output from dot11/main/common/hostio/hip_hh_gen.pl hip_signals.xml.
 * It's been fiddled to match the names used by BlueCore rather than changing the code
 * common to both chips. */

typedef enum SYMBOL_ID
{
    SLUTID_NONE                                      = 0x0000,
    SLUTID_PCI_SLOT_CONFIG                           = 0x0001,
    SLUTID_SDIO_SLOT_CONFIG                          = 0x0002,
    SLUTID_ID_INTEGER                                = 0x0003,
    SLUTID_ID_STRING                                 = 0x0004,
    SLUTID_ID_STRING_FULL                            = 0x0004,
    SLUTID_PERSISTENT_STORE_DB                       = 0x0005,
    SLUTID_RESET_VECTOR_PHY                          = 0x0006,
    SLUTID_RESET_VECTOR_MAC                          = 0x0007,
    SLUTID_SDIO_LOADER_CONTROL                       = 0x0008,
    SLUTID_TEST_COMMAND                              = 0x0009,
    SLUTID_TEST_ALIVE                                = 0x000A,
    SLUTID_TEST_PARAMETERS                           = 0x000B,
    SLUTID_TEST_RESULTS                              = 0x000C,
    SLUTID_TEST_FIRMWARE_VERSION                     = 0x000D,
    SLUTID_MIB_PSID_RANGES                           = 0x000E,
    SLUTID_KIP_TABLE                                 = 0x000F,
    SLUTID_PANIC_DATA_PHY                            = 0x0010,
    SLUTID_PANIC_DATA_MAC                            = 0x0011,
    SLUTID_BOOT_LOADER_CONTROL                       = 0x0012,
    SLUTID_SOFT_MAC                                  = 0x0013
} SYMBOL_ID;

}

#endif // SLUTIDS_H
