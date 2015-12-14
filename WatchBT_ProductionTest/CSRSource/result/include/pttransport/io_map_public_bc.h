#ifndef __IO_MAP_PUBLIC_H__
#define __IO_MAP_PUBLIC_H__

/****************************************************************************
Copyright (C) Cambridge Silicon Radio Ltd 2002-2009

FILE
    io_map_public.h  -  register allocations used by host tools.

DESCRIPTION
        Hardware declarations header file (lower level) for use by
        tools driving SPI (such as emulator), not the software stack.

        Lists memory mapped register addresses.

MODIFICATION HISTORY
    1.1  11:jan:02  pws     Created by curtailing old io_map_public.h.
    1.2  21:jan:02  pws     hci-14 #12++: def of TIMER_SLOW_TIMER_PERIOD.
    1.3  21:jan:02  pws     hci-14 #12++: remove `dummy' definition.
    1.4  22:jan:02  pws     hci-14 #12++: three more registers needed.
    1.5  07:jun:02  ckl     Added RSTGEN_WATCHDOG_KICK
    #6   17:sep:03  sms B-1005: Added free list registers.
    #7   17:sep:03  sms     B-1005: Added modification history for #6.

OLD MODIFICATION HISTORY
    1.1  12:apr:99  rwy     First created.
*/

#ifdef RCS_STRINGS
static const char io_map_public_h_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/spi/pttransport/io_map_public_bc.h#1 $";
#endif

enum io_map_enum {
/* -- Analogue control and status registers -- */

/* Declarations of read/write registers */
ANA_CONFIG2                        = 0xFF7E,   /* RW  16 bits */
ANA_LO_FREQ                        = 0xFF82,   /* RW  16 bits */
ANA_LO_FTRIM                       = 0xFF83,   /* RW  16 bits */

/* Addresses of read only registers */
ANA_VERSION_ID                     = 0xFF7D,   /* R   16 bits */


/* -- Global enable and status registers -- */

/* Addresses of read/write registers */
GBL_RST_ENABLES                     = 0xFF91,  /* RW   2 bits */
GBL_TIMER_ENABLES                   = 0xFF94,  /* RW   4 bits */
GBL_MISC_ENABLES                    = 0xFF97,  /* RW   8 bits */
GBL_MISC2_ENABLES                   = 0xFF52,  /* RW   4 bits */
GBL_CLK_RATE                        = 0xFFDE,  /* RW   5 bits */

/* Addresses of read only registers */
GBL_CHIP_VERSION                    = 0xFF9A,  /* R   16 bits */


/* -- Event timer control and status registers -- */

/* Declarations of read only registers */
TIMER_SLOW_TIMER_PERIOD             = 0xFFB9,  /* R   13 bits */


/* -- SPI command registers -- */

/* Addresses of read/write registers */
SPI_EMU_CMD                         = 0x006A,  /* RW   8 bits */
SPI_USER_CMD                        = 0x006B,  /* RW   8 bits */
SPI_BRK_DATA_ADDR                   = 0x006C,  /* RW  16 bits */
SPI_BRK_DATA_RW_EN                  = 0x006D,  /* RW   2 bits */


/* -- Memory management unit control and status registers -- */

/* Addresses of read/write registers */
MMU_FREE_LIST_ADDR                  = 0x0070,  /* RW  16 bits */
MMU_FLASH_BANK_SELECT               = 0x0073,  /* RW  12 bits */
MMU_CONST_BANK_SELECT               = 0x0074,  /* RW   9 bits */

/* Addresses of read only registers */
MMU_FREE_LIST_NEXT_ADDR             = 0xFFCF,  /* R   16 bits */


/* -- Reset generator registers -- */

/* Addresses of read/write registers */
RSTGEN_WATCHDOG_DELAY               = 0x0076,  /* RW  16 bits */
RSTGEN_WATCHDOG_KICK                = 0x0077,  /* RW   1 bit  */


/* -- XAP program registers -- */

/* Addresses of read/write registers */
XAP_AH                              = 0xFFE0,  /* RW  16 bits */
XAP_AL                              = 0xFFE1,  /* RW  16 bits */
XAP_UXH                             = 0xFFE2,  /* RW   8 bits */
XAP_UXL                             = 0xFFE3,  /* RW  16 bits */
XAP_UY                              = 0xFFE4,  /* RW  16 bits */
XAP_IXH                             = 0xFFE5,  /* RW   8 bits */
XAP_IXL                             = 0xFFE6,  /* RW  16 bits */
XAP_IY                              = 0xFFE7,  /* RW  16 bits */
XAP_FLAGS                           = 0xFFE8,  /* RW   8 bits */
XAP_PCH                             = 0xFFE9,  /* RW   8 bits */
XAP_PCL                             = 0xFFEA,  /* RW  16 bits */
XAP_BRK_REGH                        = 0xFFEB,  /* RW   8 bits */
XAP_BRK_REGL                        = 0xFFEC,  /* RW  16 bits */
XAP_RSVD_13                         = 0xFFED,  /* RW  16 bits */
XAP_RSVD_14                         = 0xFFEE,  /* RW  16 bits */
XAP_RSVD_15                         = 0xFFEF   /* RW  16 bits */

};

typedef enum io_map_enum io_map;

/* flash bank area */
static const int FLASH_BANK_BASE         = 0x8000;
static const int FLASH_BANK_SIZE         = 0x1000;
/* Convert address in flash into a FLASH_BANK_SELECT page number */
#define FLASH_ADDR_TO_FLASH_BANK(addr)   ((unsigned short)(addr) >> 12)

#endif /* __IO_MAP_PUBLIC_H__ */
