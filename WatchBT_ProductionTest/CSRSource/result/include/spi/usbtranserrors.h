#ifndef USBTRANS_ERRORS_H
#define USBTRANS_ERRORS_H

/**********************************************************************
*
*  FILE   :  usbtranserrors.h
*
*            Copyright (C) Cambridge Silicon Radio Ltd 2006-2009
*
*  PURPOSE:  Error codes used for USB SPI
*
*  $Id: $
*
***********************************************************************/

enum usbspi_errors
{
        // usbspi errors are in the range 0x200-0x2FF
    USBSPI_ERROR_MEMORY      = 0x201,
    USBSPI_ERROR_CONNECT     = 0x202,   // Couldn't find a Babel to talk to.
    USBSPI_ERROR_DRIVER_OPEN = 0x203,   // Couldn't open the driver.
    USBSPI_ERROR_DRIVER_IO   = 0x204,   // Driver IO operation failed.
    USBSPI_ERROR_DSP         = 0x205,   // Error reported from the DSP.
    USBSPI_ERROR_LARGEDATA   = 0x206,   // 'bitcount' for reading/writing JTAG register too large.
    USBSPI_ERROR_MUTEX       = 0x207,
    USBSPI_ERROR_VERSION     = 0x208,   // The Babel firmware doesn't support the given action.
    USBSPI_ERROR_NOT_INIT    = 0x209    // The library was not properly initialised before use.
};

#endif // USBTRANS_ERRORS_H
