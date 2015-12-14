/****************************************************************************
FILE
        builddef_bluecore.h  -  a bitfield type of hardware types

CONTAINS
        builddef_bluecore_bits  -  a bitfield type of hardware types

DESCRIPTION
        This file defines the individual bits of a bitfield type that can
        be used to list a combination of hardware types (BlueCore variants).

MODIFICATION HISTORY
        1.1   19:dec:01  at     Created.
        1.2   23:jul:02  at     H16.48: Added builddef for kato.
        1.3   25:sep:03  ajh    B-1083: Added builddef for kalimba and nicknack
        #5    13:apr:04  pw04   B-2130: support DFU on paddywack.
        #6    07:may:04  pws    B-2706: Support DFU loader on Coyote.
        #7    29:apr:05  pw04   B-6653: Add builddefs for Pugwash.
        #9    04:jan:06  pw04   B-10600: Add Elvis builddef.
        #13   02:mar:06  sk03   B-12413: Integrate BC5 support onto main.
        ------------------------------------------------------------------
        --- This modification history is now closed. Do not update it. ---
        ------------------------------------------------------------------
*/

#ifndef __BUILDDEF_BLUECORE_H__
#define __BUILDDEF_BLUECORE_H__


#include "bits64.h"


/* Type to use for a bitfield of supported hardware types */
typedef bits64 builddef_bluecore_bits;

/* Name the individual bits (these must be simple decimal literals) */
#define BUILDDEF_BLUECORE_BIT_BC01              0
#define BUILDDEF_BLUECORE_BIT_BC02_EXTERNAL     1
#define BUILDDEF_BLUECORE_BIT_BC02_ROM          2
#define BUILDDEF_BLUECORE_BIT_BC3_KALIMBA       3
#define BUILDDEF_BLUECORE_BIT_BC3_NICKNACK      4
#define BUILDDEF_BLUECORE_BIT_BC3_PADDYWACK     5
#define BUILDDEF_BLUECORE_BIT_BC3_COYOTE        6
#define BUILDDEF_BLUECORE_BIT_BC4_PUGWASH       7
#define BUILDDEF_BLUECORE_BIT_BC5_ELVIS         9
#define BUILDDEF_BLUECORE_BIT_BC4_JUMPINJACK    10
#define BUILDDEF_BLUECORE_BIT_BC5_PRISCILLA     11
#define BUILDDEF_BLUECORE_BIT_BC7_GORDON        12

#endif
