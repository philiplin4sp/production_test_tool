#ifndef _IO_MAP_PUBLIC_ROB_H_
#define _IO_MAP_PUBLIC_ROB_H_

/* *************************************************************************  *
   COMMERCIAL IN CONFIDENCE
   Copyright (C) Cambridge Silicon Radio Ltd 2001-2010

   http://www.csr.com

   $Id$
   $Name$
   $Source$

   DESCRIPTION
      Hardware declarations header file (lower level) for use by
      tools driving SPI (such as emulator), not the software stack.
      Lists memory mapped register addresses.

*  *************************************************************************  */

enum io_map_enum {

TIMER_TIME                              = 0x000f,
PROC_INSTR_QUEUE_FLUSH                  = 0x0018,  /*  RW   1 bit  */

BTLE_BLE_BANK_SELECT                    = 0xf63c,
BTLE_LINK_RANDOM_NUMBER                 = 0xf6f6,

XAP_PCH                                 = 0xFFE9,  /*  RW   8 bits */
XAP_PCL                                 = 0xFFEA,  /*  RW  16 bits */

GBL_CHIP_VERSION                        = 0xFE81,  /*  R   16 bits */

dummy

};

typedef enum io_map_enum io_map;

static const int BTLE_BLE_BANK_SELECT_VALUES  = 0x0002;
static const int BTLE_BLE_BANK_SELECT_MASK    = 0xffff;

#endif // _IO_MAP_PUBLIC_ROB_H_
