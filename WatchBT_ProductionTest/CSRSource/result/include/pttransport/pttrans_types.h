#ifndef PTTRANS_TYPES_H
#define PTTRANS_TYPES_H

/**********************************************************************
*
*  FILE   :  pttrans_types.h
*
*            Copyright (C) Cambridge Silicon Radio Ltd 2007-2009
*
*  PURPOSE:  Types used by pttransport.
*
*  $Id: $
*
***********************************************************************/

#define PTTRANS_SUCCESS (0)
#define PTTRANS_FAILURE (1)
    
typedef enum pttrans_xap_state
{
    PTTRANS_XAP_STOPPED = 1,
    PTTRANS_XAP_RUNNING = 0,
    PTTRANS_XAP_NO_REPLY = -1,
    PTTRANS_XAP_SPI_LOCKED = -2
} pttrans_xap_state;

typedef enum pttrans_core_type_t
{
    pttrans_core_type_unknown,
    pttrans_core_type_autodetect,
    pttrans_core_type_bluecore,
    pttrans_core_type_unifi
} pttrans_core_type_t;

typedef enum pttrans_uniproc_t
{
    pttrans_uniproc_mac,
    pttrans_uniproc_phy,
    pttrans_uniproc_bt,
    pttrans_uniproc_none,
    pttrans_uniproc_both
} pttrans_uniproc_t;

enum pttrans_spi_errors
{   
    PTTRANS_ERROR_NO_ERROR        = PTTRANS_SUCCESS,
    PTTRANS_ERROR_GENERAL_FAILURE = PTTRANS_FAILURE,
    PTTRANS_ERROR_MEMORY,           // Out of memory.
    PTTRANS_ERROR_INVALID_LIBRARY,  // Failed to load the specified plug-in library.
    PTTRANS_ERROR_NO_DEVICE,        // Unable to find a device.
    PTTRANS_ERROR_ALREADY_OPEN,     // Caused by calling pttrans_open with library already open.
    PTTRANS_ERROR_PLUGIN_FAIL,      // A plug-in function failed but plug-in didn't give error.
    PTTRANS_ERROR_PLUGIN_FN,        // A plug-in function is missing.
    PTTRANS_ERROR_READ_VERIFY,      // We failed to read back two matching values.
    PTTRANS_ERROR_WRITE_VERIFY,     // We failed to read back the same value we wrote.
    PTTRANS_ERROR_VERSION,          // Version check failed.
    PTTRANS_ERROR_DETECT_CORE_TYPE_FAILED,  // Can be due to transport being unavailable or hardware not understanding reads to 0xFF
    PTTRANS_ERROR_TOO_MANY_STREAMS, // MAX_STREAMS streams already open.
    PTTRANS_ERROR_INVALID_STREAM,   // stream id not recognised.
    PTTRANS_ERROR_OS_ERROR,         // operating system error.
};

enum pttrans_flags
{
    PTTRANS_DONT_DESELECT_LPC =   0x1,  /* Don't de-select LPC when detecting core type */
    PTTRANS_ALLOW_UNKNOWN_CHIPS = 0x2,   /* If set, Xap helper won't assert on an unknown chip */
    PTTRANS_ALLOW_SPI_LOCKED_GDN_OVERRIDE = 0x4   /* If set, and spi is locked, it allows the true value from the chip to be returned */
};

/* 
    The mode of delays is now programmable.  Use spifns_set_shift_period_mode().

    DELAYS_SLOW_TIL_RELIABLE
            Slows down the SPI bus until reliable communication is achieved. 
            Operations may be erroneous until this is achieved.  
            The spifns_xap_reset_and_stop functions attempt to get reliable operaiton
    DELAYS_NO_INC
            Always runs the bus at maximum speed.  Slowdown is not performed.  This may 
            lead to very unreliable operation with chips in shallow sleep.
*/
typedef enum spifns_delays_mode_t 
{
    DELAYS_SLOW_TIL_RELIABLE = 0,
    DELAYS_NO_INC = 1
} spifns_delays_mode_t;

typedef struct pttrans_device
{
    char *trans;        // String to pass to pttrans_open()
    char *transport;    // Value for SPITRANS
    uint32 port;        // Value for SPIPORT
    char *name;         // Displayable string
} pttrans_device;

#endif // PTTRANS_TYPES_H
