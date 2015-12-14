/*
 * bccmdkaldebugpdu.h
 *
 * The definitions in this file pertain to the KALDEBUG BCCMD PDU, which was
 * used for debugging the Kalimba DSP up to and including 23.x firmware,
 * but is no longer supported.
 * These definitions are for tool backward compatibility and should never
 * be changed.
 * Not to be confused with the similar KALEXTADDRDEBUG BCCMD, which replaces
 * KALDEBUG from 25.x firmware onwards.
 */

#ifndef BCCMDKALDEBUGPDU_H
#define BCCMDKALDEBUGPDU_H

typedef enum {
    BCCMDPDU_KALDEBUG_VERSION,
    BCCMDPDU_KALDEBUG_STEP,
    BCCMDPDU_KALDEBUG_GETCONTEXT,
    BCCMDPDU_KALDEBUG_WRITEREG,
    BCCMDPDU_KALDEBUG_READDM,
    BCCMDPDU_KALDEBUG_READPM,
    BCCMDPDU_KALDEBUG_WRITEDM,
    BCCMDPDU_KALDEBUG_WRITEPM,
    BCCMDPDU_KALDEBUG_SETDATABREAKPOINT,
    BCCMDPDU_KALDEBUG_CLEARDATABREAKPOINT,
    BCCMDPDU_KALDEBUG_STOP,
    BCCMDPDU_KALDEBUG_WAKEUP,
    BCCMDPDU_KALDEBUG_INTERRUPT,
    BCCMDPDU_KALDEBUG_RESET,
    BCCMDPDU_KALDEBUG_RUN,
    BCCMDPDU_KALDEBUG_CLAIM,
    BCCMDPDU_KALDEBUG_RELEASE,
    BCCMDPDU_KALDEBUG_GETSTATE,
    BCCMDPDU_KALDEBUG_SETBREAKPOINT,
    BCCMDPDU_KALDEBUG_CLEARBREAKPOINT,
    BCCMDPDU_KALDEBUG_SETNUMBEREDPMBREAKPOINT,
    BCCMDPDU_KALDEBUG_CLEARNUMBEREDPMBREAKPOINT
} BCCMDPDU_KALDEBUG_FUNCTION;

typedef struct {
    uint16 function;
    union {
        uint16 u16;
        struct {
            uint16 state;  /* (Res) */
            uint16 dummy;  /* Padding */
            uint16 reg[1]; /* (Res) Registers */
        } context;
        struct {
            uint16 reg; /* (Arg) Which register */
            uint16 lsw; /* (Arg) LSW of value to write */
            uint16 msb; /* (Arg) MSB of value to write */
        } writereg;
        struct {
            uint16 start_u24;   /* (Arg) Start address */
            uint16 num_u16;  /* (Arg) number of 16 bit words */
            uint16 data[1]; /* (Res) Packed into bytes little endian wise */
        } readdm;
        struct {
            uint16 start_u32;   /* (Arg) Start address */
            uint16 num_u16;  /* (Arg) number of 16 bit words */
            uint16 data[1]; /* (Res) Packed into bytes little endian wise */
        } readpm;
        struct {
            uint16 start_u24;   /* (Arg) Start address */
            uint16 num_u16;     /* (Arg) number of 16 bit words */
            uint16 data[1]; /* (Arg) Data to write, packed little endian wise */
        } writedm;
        struct {
            uint16 start_u32;   /* (Arg) Start address */
            uint16 num_u16;     /* (Arg) number of 16 bit words */
            uint16 data[1]; /* (Arg) Data to write, packed little endian wise */
        } writepm;
        struct {
            uint16 addr;  /* (Arg) Address to set a data breakpoint */
            uint16 read;  /* (Arg) bool - break on read */
            uint16 write; /* (Arg) bool - break on write */
        } setdatabreakpoint;
        struct {
            uint16 addr;              /* (Arg) Address to set a program memory breakpoint */
            uint16 brk_point_number;  /* (Arg) Which numbered hardware breakpoint to set*/
        } setpmbreakpoint;
    } data;
} BCCMDPDU_KALDEBUG;

#endif
