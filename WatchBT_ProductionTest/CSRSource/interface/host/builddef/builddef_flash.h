/****************************************************************************
FILE
        builddef_flash.h  -  a bitfield type of flash geometries

CONTAINS
        builddef_flash_bits  -  a bitfield type of flash geometries

DESCRIPTION
        This file defines the individual bits of a bitfield type that can
        be used to list a combination of flash geometries.

MODIFICATION HISTORY
        1.1   19:dec:01  at     Created.
        1.2   25:sep:03  ajh    adding zero wait flash.
        #4    13:apr:04  pw04   B-2130: support DFU on Paddywack
        pce-20
        #2    11:jan:05  pw04   B-4093: 16 Mbit flash support
        main
        #6    12:apr:06  pw04   B-11960: 32 Mbit flash support.
        ------------------------------------------------------------------
        --- This modification history is now closed. Do not update it. ---
        ------------------------------------------------------------------

*/

#ifndef __BUILDDEF_FLASH_H__
#define __BUILDDEF_FLASH_H__


#include "bits64.h"


/* Type to use for a bitfield of supported flash geometries */
typedef bits64 builddef_flash_bits;

/* Name the individual bits (these must be simple decimal literals) */
#define BUILDDEF_FLASH_BIT_4M_ORIGINAL          0
#define BUILDDEF_FLASH_BIT_8M_ORIGINAL          1
#define BUILDDEF_FLASH_BIT_4M_2KWORD            2
#define BUILDDEF_FLASH_BIT_8M_2KWORD            3
#define BUILDDEF_FLASH_BIT_4M_ORIGINAL_0WAIT    4
#define BUILDDEF_FLASH_BIT_8M_ORIGINAL_0WAIT    5
#define BUILDDEF_FLASH_BIT_4M_2KWORD_0WAIT      6
#define BUILDDEF_FLASH_BIT_8M_2KWORD_0WAIT      7
#define BUILDDEF_FLASH_BIT_6M_2KWORD_0WAIT      8
#define BUILDDEF_FLASH_BIT_6M_2KWORD            9
#define BUILDDEF_FLASH_BIT_16M_ORIGINAL         10
#define BUILDDEF_FLASH_BIT_16M_2KWORD           11
#define BUILDDEF_FLASH_BIT_16M_ORIGINAL_0WAIT   12
#define BUILDDEF_FLASH_BIT_16M_2KWORD_0WAIT     13
#define BUILDDEF_FLASH_BIT_32M_ORIGINAL         14
#define BUILDDEF_FLASH_BIT_32M_2KWORD           15
#define BUILDDEF_FLASH_BIT_32M_ORIGINAL_0WAIT   16
#define BUILDDEF_FLASH_BIT_32M_2KWORD_0WAIT     17

#endif
