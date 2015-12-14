/****************************************************************************
FILE
        slut_mem.h

DESCRIPTION
        This file contains an enumeration representing possible BlueCore
        firmware memory layouts. Tools such as host SPI tools
        interrogating the BlueCore address map need this information in
        order to interpret pointers, such as those in the SLUT.

*/

#ifndef __SLUT_MEM_H__
#define __SLUT_MEM_H__

#ifdef RCS_STRINGS
static const char slut_mem_h_id[]
  = "$Id: //depot/bc/bluesuite_2_4/interface/host/slut/slut_mem.h#1 $";
#endif

enum {

    /* All addresses below are physical (Flash/ROM) addresses in hex. */

    /* LAYOUTS FOR CHIPS PRIOR TO JEMIMA (BC7) */

    /* Loader in a DFU-supporting system. */
    /* Constants 0000-1FFF, code 010000- */
    /* (stack will be a STACK_PRE_JEMIMA) */
    LOADER_PRE_JEMIMA               = 0,

    /* Monolithic build with low stack constants. */
    /* Constants 0000-3FFF, code 010000- */
    MONOLITH_STRAPPED_PRE_JEMIMA    = 1,

    /* Monolithic build with high stack constants. */
    /* Constants 4000-7FFF, code 010000- */
    MONOLITH_FLUSH_PRE_JEMIMA       = 2,

    /* Stack in a DFU-supporting system. */
    /* Constants 4000-7FFF, code 018000- */
    /* (loader will be a LOADER_PRE_JEMIMA) */
    STACK_PRE_JEMIMA                = 3,


    /* LAYOUTS FOR JEMIMA (BC7) AND UP */
    /* (for the foreseeable future) */

    /* Loader in a DFU-supporting system. */
    /* Constants 0000-1FFF, code 08000-0FFFF */
    /* (stack will be a STACK_POST_JEMIMA) */
    LOADER_POST_JEMIMA              = 4,

    /* Monolithic build with low stack constants. */
    /* Constants 0000-3FFF, code 004000- */
    MONOLITH_STRAPPED_POST_JEMIMA   = 5,

    /* Monolithic build with high stack constants. */
    /* Constants are SPLIT into two blocks that are
     * non-contiguous in physical address space but will
     * be contiguous in data address space:
     * 0000-1FFF, 4000-5FFF
     * Code 006000- */
    MONOLITH_FLUSH_POST_JEMIMA      = 6,

    /* Stack in a DFU-supporting system. */
    /* Constants 4000-7FFF, code 010000- */
    /* (loader will be a LOADER_POST_JEMIMA) */
    STACK_POST_JEMIMA               = 7

};


#endif  /* __SLUT_MEM_H__ */
