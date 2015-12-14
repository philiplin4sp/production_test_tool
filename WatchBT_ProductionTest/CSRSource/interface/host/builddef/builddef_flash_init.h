/****************************************************************************
FILE
        builddef_flash_init.h  -  initialise a flash geometries bitfield

CONTAINS
        BUILDDEF_FLASH_INIT  -  macro giving bitfield intialisation value

DESCRIPTION
        This file should be included after setting the appropriate
        BUILDDEF_FLASH_OPT_* macros to set BUILDDEF_FLASH_INIT to the
        corresponding bitfield initialisation value. This result is
        suitable for assigning during a constant or variable definition.

        Note that the result is only valid until this is included again.
        This is because macro expansion is lazy, i.e. the macros are only
        expanded when used and BUILDDEF_FLASH_INIT depends on the values
        of other macros that change each time this file is included.

MODIFICATION HISTORY
        1.1   19:dec:01         at        Created.
        1.2   25:sep:03         ajh       B-1084: Added zero wait variants
        1.3   26:sep:03         ajh       oops - completed above.
        1.4   26:sep:03         ajh       fix to above - or 4 to 8.
        #6    13:apr:04         pw04      B-2130: support DFU on Paddywack
        pce-20
        #2    11:jan:05         pw04      B-4093: 16 Mbit flash support
        main
        #8    12:apr:06         pw04      B-11960: 32 Mbit flash support.
        ------------------------------------------------------------------
        --- This modification history is now closed. Do not update it. ---
        ------------------------------------------------------------------
*/


#include "builddef_flash.h"


/* Undefine any previous intermediate macros */
#undef BUILDDEF_FLASH_USE_BIT_0
#undef BUILDDEF_FLASH_USE_BIT_1
#undef BUILDDEF_FLASH_USE_BIT_2
#undef BUILDDEF_FLASH_USE_BIT_3
#undef BUILDDEF_FLASH_USE_BIT_4
#undef BUILDDEF_FLASH_USE_BIT_5
#undef BUILDDEF_FLASH_USE_BIT_6
#undef BUILDDEF_FLASH_USE_BIT_7
#undef BUILDDEF_FLASH_USE_BIT_8
#undef BUILDDEF_FLASH_USE_BIT_9
#undef BUILDDEF_FLASH_USE_BIT_10
#undef BUILDDEF_FLASH_USE_BIT_11
#undef BUILDDEF_FLASH_USE_BIT_12
#undef BUILDDEF_FLASH_USE_BIT_13

/* Map the options to bits */
#ifdef BUILDDEF_FLASH_OPT_4M_ORIGINAL
#define BUILDDEF_FLASH_USE_BIT_0 BITS64_BIT(BUILDDEF_FLASH_BIT_4M_ORIGINAL)
#else
#define BUILDDEF_FLASH_USE_BIT_0 BITS64_NONE
#endif

#ifdef BUILDDEF_FLASH_OPT_8M_ORIGINAL
#define BUILDDEF_FLASH_USE_BIT_1 BITS64_BIT(BUILDDEF_FLASH_BIT_8M_ORIGINAL)
#else
#define BUILDDEF_FLASH_USE_BIT_1 BITS64_NONE
#endif

#ifdef BUILDDEF_FLASH_OPT_4M_2KWORD
#define BUILDDEF_FLASH_USE_BIT_2 BITS64_BIT(BUILDDEF_FLASH_BIT_4M_2KWORD)
#else
#define BUILDDEF_FLASH_USE_BIT_2 BITS64_NONE
#endif

#ifdef BUILDDEF_FLASH_OPT_8M_2KWORD
#define BUILDDEF_FLASH_USE_BIT_3 BITS64_BIT(BUILDDEF_FLASH_BIT_8M_2KWORD)
#else
#define BUILDDEF_FLASH_USE_BIT_3 BITS64_NONE
#endif

#ifdef BUILDDEF_FLASH_OPT_4M_ORIGINAL_0WAIT
#define BUILDDEF_FLASH_USE_BIT_4 \
                          BITS64_BIT(BUILDDEF_FLASH_BIT_4M_ORIGINAL_0WAIT)
#else
#define BUILDDEF_FLASH_USE_BIT_4 BITS64_NONE
#endif

#ifdef BUILDDEF_FLASH_OPT_8M_ORIGINAL_0WAIT
#define BUILDDEF_FLASH_USE_BIT_5\
                          BITS64_BIT(BUILDDEF_FLASH_BIT_8M_ORIGINAL_0WAIT)
#else
#define BUILDDEF_FLASH_USE_BIT_5 BITS64_NONE
#endif

#ifdef BUILDDEF_FLASH_OPT_4M_2KWORD_0WAIT
#define BUILDDEF_FLASH_USE_BIT_6 BITS64_BIT(BUILDDEF_FLASH_BIT_4M_2KWORD_0WAIT)
#else
#define BUILDDEF_FLASH_USE_BIT_6 BITS64_NONE
#endif

#ifdef BUILDDEF_FLASH_OPT_8M_2KWORD_0WAIT
#define BUILDDEF_FLASH_USE_BIT_7 BITS64_BIT(BUILDDEF_FLASH_BIT_8M_2KWORD_0WAIT)
#else
#define BUILDDEF_FLASH_USE_BIT_7 BITS64_NONE
#endif

#ifdef BUILDDEF_FLASH_OPT_6M_2KWORD_0WAIT
#define BUILDDEF_FLASH_USE_BIT_8 BITS64_BIT(BUILDDEF_FLASH_BIT_6M_2KWORD_0WAIT)
#else
#define BUILDDEF_FLASH_USE_BIT_8 BITS64_NONE
#endif

#ifdef BUILDDEF_FLASH_OPT_6M_2KWORD
#define BUILDDEF_FLASH_USE_BIT_9 BITS64_BIT(BUILDDEF_FLASH_BIT_6M_2KWORD)
#else
#define BUILDDEF_FLASH_USE_BIT_9 BITS64_NONE
#endif

#ifdef BUILDDEF_FLASH_OPT_16M_ORIGINAL
#define BUILDDEF_FLASH_USE_BIT_10 BITS64_BIT(BUILDDEF_FLASH_BIT_16M_ORIGINAL)
#else
#define BUILDDEF_FLASH_USE_BIT_10 BITS64_NONE
#endif

#ifdef BUILDDEF_FLASH_OPT_16M_2KWORD
#define BUILDDEF_FLASH_USE_BIT_11 BITS64_BIT(BUILDDEF_FLASH_BIT_16M_2KWORD)
#else
#define BUILDDEF_FLASH_USE_BIT_11 BITS64_NONE
#endif

#ifdef BUILDDEF_FLASH_OPT_16M_ORIGINAL_0WAIT
#define BUILDDEF_FLASH_USE_BIT_12 BITS64_BIT(BUILDDEF_FLASH_BIT_16M_ORIGINAL_0WAIT)
#else
#define BUILDDEF_FLASH_USE_BIT_12 BITS64_NONE
#endif

#ifdef BUILDDEF_FLASH_OPT_16M_2KWORD_0WAIT
#define BUILDDEF_FLASH_USE_BIT_13 BITS64_BIT(BUILDDEF_FLASH_BIT_16M_2KWORD_0WAIT)
#else
#define BUILDDEF_FLASH_USE_BIT_13 BITS64_NONE
#endif

#ifdef BUILDDEF_FLASH_OPT_32M_ORIGINAL
#define BUILDDEF_FLASH_USE_BIT_14 BITS64_BIT(BUILDDEF_FLASH_BIT_32M_ORIGINAL)
#else
#define BUILDDEF_FLASH_USE_BIT_14 BITS64_NONE
#endif

#ifdef BUILDDEF_FLASH_OPT_32M_2KWORD
#define BUILDDEF_FLASH_USE_BIT_15 BITS64_BIT(BUILDDEF_FLASH_BIT_32M_2KWORD)
#else
#define BUILDDEF_FLASH_USE_BIT_15 BITS64_NONE
#endif

#ifdef BUILDDEF_FLASH_OPT_32M_ORIGINAL_0WAIT
#define BUILDDEF_FLASH_USE_BIT_16 BITS64_BIT(BUILDDEF_FLASH_BIT_32M_ORIGINAL_0WAIT)
#else
#define BUILDDEF_FLASH_USE_BIT_16 BITS64_NONE
#endif

#ifdef BUILDDEF_FLASH_OPT_32M_2KWORD_0WAIT
#define BUILDDEF_FLASH_USE_BIT_17 BITS64_BIT(BUILDDEF_FLASH_BIT_32M_2KWORD_0WAIT)
#else
#define BUILDDEF_FLASH_USE_BIT_17 BITS64_NONE
#endif


/* Combine the bits to form the initialisation value */
#undef BUILDDEF_FLASH_INIT
#define BUILDDEF_FLASH_INIT \
    BITS64_INIT(BITS64_OR_N(BUILDDEF_FLASH_USE_BIT_, 18))

/* Undefine the input macros */
#undef BUILDDEF_FLASH_OPT_4M_ORIGINAL
#undef BUILDDEF_FLASH_OPT_8M_ORIGINAL
#undef BUILDDEF_FLASH_OPT_4M_2KWORD
#undef BUILDDEF_FLASH_OPT_8M_2KWORD
#undef BUILDDEF_FLASH_OPT_4M_ORIGINAL_0WAIT
#undef BUILDDEF_FLASH_OPT_8M_ORIGINAL_0WAIT
#undef BUILDDEF_FLASH_OPT_4M_2KWORD_0WAIT
#undef BUILDDEF_FLASH_OPT_8M_2KWORD_0WAIT
#undef BUILDDEF_FLASH_OPT_6M_2KWORD_0WAIT
#undef BUILDDEF_FLASH_OPT_6M_2KWORD
#undef BUILDDEF_FLASH_OPT_16M_ORIGINAL
#undef BUILDDEF_FLASH_OPT_16M_2KWORD
#undef BUILDDEF_FLASH_OPT_16M_ORIGINAL_0WAIT
#undef BUILDDEF_FLASH_OPT_16M_2KWORD_0WAIT
#undef BUILDDEF_FLASH_OPT_32M_ORIGINAL
#undef BUILDDEF_FLASH_OPT_32M_2KWORD
#undef BUILDDEF_FLASH_OPT_32M_ORIGINAL_0WAIT
#undef BUILDDEF_FLASH_OPT_32M_2KWORD_0WAIT

