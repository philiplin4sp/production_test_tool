/*
 * bccmdvmdebugpdu.h
 *
 * Commands for debugging VM apps.
 */

#ifndef BCCMDVMDEBUGPDU_H
#define BCCMDVMDEBUGPDU_H

/* Different things this BCCMD command can do */
typedef enum {
    BCCMDPDU_VM_DEBUG_CLAIM,
    BCCMDPDU_VM_DEBUG_SINGLESTEP,
    BCCMDPDU_VM_DEBUG_READMEMORY,
    BCCMDPDU_VM_DEBUG_GETMAXBREAKPOINTS,
    BCCMDPDU_VM_DEBUG_SETBREAKPOINTS,
    BCCMDPDU_VM_DEBUG_CLEARBREAKPOINTS,
    BCCMDPDU_VM_DEBUG_BREAKIN,
    BCCMDPDU_VM_DEBUG_EXECUTE,
    BCCMDPDU_VM_DEBUG_GETMEMMAP,
    BCCMDPDU_VM_DEBUG_MEMCHANGED,
    BCCMDPDU_VM_DEBUG_MAPCHANGED,
    BCCMDPDU_VM_DEBUG_CHECKAPP,
    BCCMDPDU_VM_DEBUG_GETCONTEXT,
    BCCMDPDU_VM_DEBUG_POLLTRACE,
    BCCMDPDU_VM_DEBUG_STEPOVER,
    BCCMDPDU_VM_DEBUG_GETREGIONSIZES,
    BCCMDPDU_VM_DEBUG_SETDEBUGMODE,
    BCCMDPDU_VM_DEBUG_GETPANICREASON,
    BCCMDPDU_VM_DEBUG_GETAPPSTART,
    BCCMDPDU_VM_DEBUG_GETAPPFLAGS,
    BCCMDPDU_VM_DEBUG_GETVERSIONNUMBER,
    BCCMDPDU_VM_DEBUG_SETFEATURESTATE,
    BCCMDPDU_VM_DEBUG_GETFEATURESAVAILABLE,
    BCCMDPDU_VM_DEBUG_BREAKPOINTONRESET
} BCCMDPDU_VM_DEBUG_FUNCTION;

typedef struct {
    uint16 function;            /* Corresponds to BCCMDPDU_VM_DEBUG_FUNCTION */
    union {
        uint16 u16;             /* Arg/Result for simple functions */
        struct {
            uint16 start;       /* Argument. Start address of memory to read */
            uint16 len;         /* Arg/Result. memory to read/words in mem[] */
            uint16 mem[1];      /* Result. data */
        } readmemory;
        struct {
            uint16 len;         /* Result. words in pairs[] */
            uint16 dummy;
            uint16 regions[1];  /* Result. start addrs of changed regions */
        } changedmem;
        struct {
            uint16 len;         /* Result. words in pairs[] */
            uint16 dummy;
            uint16 starts[1];   /* Result. start addresses of regions */
        } getmemmap;
        struct {
            uint16 len;         /* Result. words in sizes[] */
            uint16 dummy;
            uint16 sizes[1];    /* Result. sizes of regions */
        } getregionsizes;
        struct {
            uint16 ok;          /* Result. 0 for failure, non-zero for success*/
            uint16 len;         /* Arg. number of breakpoints to set */
            uint16 breakpoints[1]; /* Arg. Breakpoints to set. */
            /* Note that we assume that all the breakpoints fit in one pdu */
            /* Breakpoints are 32 bit big-endian for large mode apps */
        } setbreakpoints;
        struct {
            uint16 size;       /* Result. Size of app. */
            uint16 checksum;   /* Result. Checksum of app */
        } checkapp;
        struct {
            uint16 stat;        /* Res. Debug status */
            uint16 pc;          /* Res. PC */
            uint16 registers[1];/* Res. registers */
        } getcontext;
        struct {
            uint16 len;           /* Arg/Res. Number of _bytes_ in data */
            uint16 dummy;         /* dummy padding word */
            uint16 data[1];       /* Res. stream */
        } polltrace;
        struct {
            uint16 len;           /* Arg. Size of ranges */
            uint16 ok;            /* Res. step sucessful */
            uint16 ranges[1];        /* Arg. PC ranges */
            /* Ranges are 32 bit big-endian for large mode apps */
        } stepover;
        struct {
            uint16 panic_reason;  /* Corresponds to enum in host/vm/panicdefs.h */
            uint16 extra_context; /* Any other info about the panic */
        } getpanicreason;
        struct {
            uint32 app_start_addr;
        } getappstart;
        struct {
            uint16 flags;
        } getappflags;
        struct {
            uint16 features;  /* Bit array of feature states */
            uint16 rejected;  /* Bit array of unavailable features */
        } setfeaturestate;
    } data;
} BCCMDPDU_VM_DEBUG;

/*
 * The ouput stream from the vm is tunnelled through polltrace.data as a series
 * of packets. More info is in devHost/vmdebug/vmdebug.cpp
 */

/* Special region size indicating the region is not mapped in. Assumes regions
 * will always be smaller than 64k - a pretty safe assumption. */
#define VM_UNMAPPED_REGION ((uint16)-1)

/* Index register array tacked on to GETCONTEXT pdus */
typedef enum
{
    vm_ah,
    vm_al,
    vm_x,
    vm_y,
    vm_xh,
    vm_pch,
    n_vmdebug_regs
} vmdebug_regs;

#endif /* #ifndef BCCMDVMDEBUGPDU_H */
