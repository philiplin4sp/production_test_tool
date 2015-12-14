/****************************************************************************
FILE
        builddef_bluecore_init.h  -  initialise a hardware types bitfield

CONTAINS
        BUILDDEF_BLUECORE_INIT  -  macro giving bitfield initialisation value

DESCRIPTION
        This file should be included after setting the appropriate
        BUILDDEF_BLUECORE_OPT_* macros to set BUILDDEF_BLUECORE_INIT to
        the corresponding bitfield initialisation value. This result is
        suitable for assigning during a constant or variable definition.

        Note that the result is only valid until this is included again.
        This is because macro expansion is lazy, i.e. the macros are only
        expanded when used and BUILDDEF_BLUECORE_INIT depends on the values
        of other macros that change each time this file is included.

MODIFICATION HISTORY
        1.1   19:dec:01  at     Created.
        1.2   23:jul:02  at     H16.48: Added builddef for kato.
        1.2   25:sep:03  ajh    B-1083: Added builddef for kalimba and nicknack.
        #5    13:apr:04  pw04   B-2130: support DFU on paddywack.
        #6    07:may:04  pws    B-2706: Support DFU loader on Coyote.
        #7    29:apr:05  pw04   B-6653: Add builddefs for Pugwash.
        #8    03:may:05  pw04   B-6653: Duh.
        #10   04:jan:06  pw04   B-10600: Add Elvis builddef.
        #11-13 16:jan:06 bs01   B-11421: Add jumpinjack builddef.
        #15   02:mar:06  sk03   B-12413: Integrate BC5 support onto main.
        ------------------------------------------------------------------
        --- This modification history is now closed. Do not update it. ---
        ------------------------------------------------------------------
*/


#include "builddef_bluecore.h"


/* Undefine any previous intermediate macros */
#undef BUILDDEF_BLUECORE_USE_BIT_0
#undef BUILDDEF_BLUECORE_USE_BIT_1
#undef BUILDDEF_BLUECORE_USE_BIT_2
#undef BUILDDEF_BLUECORE_USE_BIT_3
#undef BUILDDEF_BLUECORE_USE_BIT_4
#undef BUILDDEF_BLUECORE_USE_BIT_5
#undef BUILDDEF_BLUECORE_USE_BIT_6
#undef BUILDDEF_BLUECORE_USE_BIT_7
#undef BUILDDEF_BLUECORE_USE_BIT_8
#undef BUILDDEF_BLUECORE_USE_BIT_9
#undef BUILDDEF_BLUECORE_USE_BIT_10
#undef BUILDDEF_BLUECORE_USE_BIT_11
#undef BUILDDEF_BLUECORE_USE_BIT_12

/* Map the options to bits */
#ifdef BUILDDEF_BLUECORE_OPT_BC01
#define BUILDDEF_BLUECORE_USE_BIT_0 BITS64_BIT(BUILDDEF_BLUECORE_BIT_BC01)
#else
#define BUILDDEF_BLUECORE_USE_BIT_0 BITS64_NONE
#endif

#ifdef BUILDDEF_BLUECORE_OPT_BC02_EXTERNAL
#define BUILDDEF_BLUECORE_USE_BIT_1 \
    BITS64_BIT(BUILDDEF_BLUECORE_BIT_BC02_EXTERNAL)
#else
#define BUILDDEF_BLUECORE_USE_BIT_1 BITS64_NONE
#endif

#ifdef BUILDDEF_BLUECORE_OPT_BC02_ROM
#define BUILDDEF_BLUECORE_USE_BIT_2 \
    BITS64_BIT(BUILDDEF_BLUECORE_BIT_BC02_ROM)
#else
#define BUILDDEF_BLUECORE_USE_BIT_2 BITS64_NONE
#endif

#ifdef BUILDDEF_BLUECORE_OPT_BC3_KALIMBA
#define BUILDDEF_BLUECORE_USE_BIT_3 \
    BITS64_BIT(BUILDDEF_BLUECORE_BIT_BC3_KALIMBA)
#else
#define BUILDDEF_BLUECORE_USE_BIT_3 BITS64_NONE
#endif

#ifdef BUILDDEF_BLUECORE_OPT_BC3_NICKNACK
#define BUILDDEF_BLUECORE_USE_BIT_4 \
    BITS64_BIT(BUILDDEF_BLUECORE_BIT_BC3_NICKNACK)
#else
#define BUILDDEF_BLUECORE_USE_BIT_4 BITS64_NONE
#endif

#ifdef BUILDDEF_BLUECORE_OPT_BC3_PADDYWACK
#define BUILDDEF_BLUECORE_USE_BIT_5 \
    BITS64_BIT(BUILDDEF_BLUECORE_BIT_BC3_PADDYWACK)
#else
#define BUILDDEF_BLUECORE_USE_BIT_5 BITS64_NONE
#endif

#ifdef BUILDDEF_BLUECORE_OPT_BC3_COYOTE
#define BUILDDEF_BLUECORE_USE_BIT_6 \
    BITS64_BIT(BUILDDEF_BLUECORE_BIT_BC3_COYOTE)
#else
#define BUILDDEF_BLUECORE_USE_BIT_6 BITS64_NONE
#endif

#ifdef BUILDDEF_BLUECORE_OPT_BC4_PUGWASH
#define BUILDDEF_BLUECORE_USE_BIT_7 \
    BITS64_BIT(BUILDDEF_BLUECORE_BIT_BC4_PUGWASH)
#else
#define BUILDDEF_BLUECORE_USE_BIT_7 BITS64_NONE
#endif


#ifdef BUILDDEF_BLUECORE_OPT_BC5_ELVIS
#define BUILDDEF_BLUECORE_USE_BIT_9 \
    BITS64_BIT(BUILDDEF_BLUECORE_BIT_BC5_ELVIS)
#else
#define BUILDDEF_BLUECORE_USE_BIT_9 BITS64_NONE
#endif

#ifdef BUILDDEF_BLUECORE_OPT_BC4_JUMPINJACK
#define BUILDDEF_BLUECORE_USE_BIT_10 \
    BITS64_BIT(BUILDDEF_BLUECORE_BIT_BC4_JUMPINJACK)
#else
#define BUILDDEF_BLUECORE_USE_BIT_10 BITS64_NONE
#endif

#ifdef BUILDDEF_BLUECORE_OPT_BC5_PRISCILLA
#define BUILDDEF_BLUECORE_USE_BIT_11 \
    BITS64_BIT(BUILDDEF_BLUECORE_BIT_BC5_PRISCILLA)
#else
#define BUILDDEF_BLUECORE_USE_BIT_11 BITS64_NONE
#endif

#ifdef BUILDDEF_BLUECORE_OPT_BC7_GORDON
#define BUILDDEF_BLUECORE_USE_BIT_12 \
    BITS64_BIT(BUILDDEF_BLUECORE_BIT_BC7_GORDON)
#else
#define BUILDDEF_BLUECORE_USE_BIT_12 BITS64_NONE
#endif


/* Combine the bits to form the initialisation value */
#undef BUILDDEF_BLUECORE_INIT
#define BUILDDEF_BLUECORE_INIT \
    BITS64_INIT(BITS64_OR_N(BUILDDEF_BLUECORE_USE_BIT_, 13))

/* Undefine the input macros */
#undef BUILDDEF_BLUECORE_OPT_BC01
#undef BUILDDEF_BLUECORE_OPT_BC02_EXTERNAL
#undef BUILDDEF_BLUECORE_OPT_BC02_ROM
#undef BUILDDEF_BLUECORE_OPT_BC3_KALIMBA
#undef BUILDDEF_BLUECORE_OPT_BC3_NICKNACK
#undef BUILDDEF_BLUECORE_OPT_BC3_PADDYWACK
#undef BUILDDEF_BLUECORE_OPT_BC3_COYOTE
#undef BUILDDEF_BLUECORE_OPT_BC4_PUGWASH
#undef BUILDDEF_BLUECORE_OPT_BC5_ELVIS
#undef BUILDDEF_BLUECORE_OPT_BC4_JUMPINJACK
#undef BUILDDEF_BLUECORE_OPT_BC5_PRISCILLA
#undef BUILDDEF_BLUECORE_OPT_BC7_GORDON
