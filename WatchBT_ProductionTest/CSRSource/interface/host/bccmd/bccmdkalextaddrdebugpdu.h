/****************************************************************************
FILE
        bccmdkalextaddrdebugpdu.h  -  (new) BCCMD DSP debug 

DESCRIPTION
        In addition to the SPI DSP debug support, we need the DSP debug support 
        through BCCMD (for a limited set of commands) primarily to support the 
        tools like parameter manager (See BTSW-520).
        This also covers the DSP with increased address space (BC5 and above)
        16-bit for BC5, 24-bit for BC7/8 and 32-bit in future

        The definitions in this file pertain to the KALEXTADDRDEBUG BCCMD
        supported by 25.x firmware onwards. They are not for the obsolete
        KALDEBUG BCCMD which was used up to 23.x firmware.
*/

#ifndef __BCCMDKALEXTADDRDEBUGPDU_H__
#define __BCCMDKALEXTADDRDEBUGPDU_H__

typedef enum {
    BCCMDPDU_KALEXTADDRDEBUG_VERSION,
    BCCMDPDU_KALEXTADDRDEBUG_GETSTATE,
    BCCMDPDU_KALEXTADDRDEBUG_READDM,
    BCCMDPDU_KALEXTADDRDEBUG_WRITEDM,
    BCCMDPDU_KALEXTADDRDEBUG_STOP,
    BCCMDPDU_KALEXTADDRDEBUG_RUN,
    BCCMDPDU_KALEXTADDRDEBUG_RESET
} BCCMDPDU_KALEXTADDRDEBUG_FUNCTION;

typedef struct {
    uint16 function;
    union {
        uint16 u16;
        struct {
            uint32 start_addr; /* (Arg) Start address: 
                                  16 bit for BC5: 24 bit for BC7 */
            uint16 num_u16;    /* (Arg) number of 16 bit words */
            uint16 data[1];    /* (Res) Packed into bytes little endian wise */
        } readdm;
        struct {
            uint32 start_addr; /* (Arg) Start address: 
                                  16 bit for BC5: 24 bit for BC7 */
            uint16 num_u16;    /* (Arg) number of 16 bit words */
            uint16 data[1]; /* (Arg) Data to write, packed little endian wise */
        } writedm;
    } data;
} BCCMDPDU_KALEXTADDRDEBUG;

#endif  /* __BCCMDKALEXTADDRDEBUGPDU_H__*/
