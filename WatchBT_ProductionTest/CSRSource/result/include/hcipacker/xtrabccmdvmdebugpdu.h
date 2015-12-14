////////////////////////////////////////////////////////////////////////////////
//
//  File:       xtrabccmdvmdebugpdu.h
//
//  Class:      BCCMD_VM_DEBUG_SIMPLE_PDU
//              BCCMD_VM_DEBUG_READMEM_PDU
//              BCCMD_VM_DEBUG_CHANGEDMEM_PDU
//              BCCMD_VM_DEBUG_GETMEMMAP_PDU
//
//  Purpose:    These need to be hand coded because some of them use arrays.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __XTRABCCMDVMDEBUGPDU_H__
#define __XTRABCCMDVMDEBUGPDU_H__

#include "hcipacker/bluecorepdu.h"

class BCCMD_VM_DEBUG_SIMPLE_PDU : public BCCMD_VM_DEBUG_PDU
{
    public:
        BCCMD_VM_DEBUG_SIMPLE_PDU();
        BCCMD_VM_DEBUG_SIMPLE_PDU(const PDU&);
        BCCMD_VM_DEBUG_SIMPLE_PDU(const uint8 *buffer, uint32 len);
        uint16 get_u16(void) const;
        void set_u16(uint16 value);
};

class BCCMD_VM_DEBUG_READMEM_PDU : public BCCMD_VM_DEBUG_PDU
{
    public:
        BCCMD_VM_DEBUG_READMEM_PDU();
        BCCMD_VM_DEBUG_READMEM_PDU(const PDU&);
        BCCMD_VM_DEBUG_READMEM_PDU(const uint8 *buffer, uint32 len);
        uint16 get_start(void) const;
        void set_start(uint16 value);
        uint16 get_len(void) const;
        void set_len(uint16 value);
        void get_mem(uint16 *copyto, uint16 len) const;
        void set_mem(const uint16 *copyfrom, uint16 len);
};

class BCCMD_VM_DEBUG_CHANGEDMEM_PDU : public BCCMD_VM_DEBUG_PDU
{
    public:
        BCCMD_VM_DEBUG_CHANGEDMEM_PDU();
        BCCMD_VM_DEBUG_CHANGEDMEM_PDU(const PDU&);
        BCCMD_VM_DEBUG_CHANGEDMEM_PDU(const uint8 *buffer, uint32 len);
        uint16 get_len(void) const;
        void set_len(uint16 value);
        void get_regions(uint16 *copyto, uint16 len) const;
        void set_regions(const uint16 *copyfrom, uint16 len);
};

class BCCMD_VM_DEBUG_GETMEMMAP_PDU : public BCCMD_VM_DEBUG_PDU
{
    public:
        BCCMD_VM_DEBUG_GETMEMMAP_PDU();
        BCCMD_VM_DEBUG_GETMEMMAP_PDU(const PDU&);
        BCCMD_VM_DEBUG_GETMEMMAP_PDU(const uint8 *buffer, uint32 len);
        uint16 get_len(void) const;
        void set_len(uint16 value);
        void get_starts(uint16 *copyto, uint16 len) const;
        void set_starts(const uint16 *copyfrom, uint16 len);
};

class BCCMD_VM_DEBUG_GETREGIONSIZES_PDU : public BCCMD_VM_DEBUG_PDU
{
    public:
        BCCMD_VM_DEBUG_GETREGIONSIZES_PDU();
        BCCMD_VM_DEBUG_GETREGIONSIZES_PDU(const PDU&);
        BCCMD_VM_DEBUG_GETREGIONSIZES_PDU(const uint8 *buffer, uint32 len);
        uint16 get_len(void) const;
        void set_len(uint16 value);
        void get_sizes(uint16 *copyto, uint16 len) const;
        void set_sizes(const uint16 *copyfrom, uint16 len);
};

class BCCMD_VM_DEBUG_SETBREAKPOINTS_PDU : public BCCMD_VM_DEBUG_PDU
{
    public:
        BCCMD_VM_DEBUG_SETBREAKPOINTS_PDU();
        BCCMD_VM_DEBUG_SETBREAKPOINTS_PDU(const PDU&);
        BCCMD_VM_DEBUG_SETBREAKPOINTS_PDU(const uint8 *buffer, uint32 len);
        uint16 get_ok(void) const;
        void set_ok(uint16 value);
        uint16 get_len(void) const;
        void set_len(uint16 value);
        void get_breakpoints(uint16 *copyto, uint16 len) const;
        void set_breakpoints(const uint16 *copyfrom, uint16 len);
};

class BCCMD_VM_DEBUG_CHECKAPP_PDU : public BCCMD_VM_DEBUG_PDU
{
    public:
        BCCMD_VM_DEBUG_CHECKAPP_PDU();
        BCCMD_VM_DEBUG_CHECKAPP_PDU(const PDU&);
        BCCMD_VM_DEBUG_CHECKAPP_PDU(const uint8 *buffer, uint32 len);
        uint16 get_size(void) const;
        void set_size(uint16 value);
        uint16 get_checksum(void) const;
        void set_checksum(uint16 value);
};

class BCCMD_VM_DEBUG_GETCONTEXT_PDU : public BCCMD_VM_DEBUG_PDU
{
    public:
        BCCMD_VM_DEBUG_GETCONTEXT_PDU();
        BCCMD_VM_DEBUG_GETCONTEXT_PDU(const PDU&);
        BCCMD_VM_DEBUG_GETCONTEXT_PDU(const uint8 *buffer, uint32 len);
        uint16 get_stat(void) const;
        void set_stat(uint16 value);
        uint16 get_pc(void) const;
        void set_pc(uint16 value);
        void get_registers(uint16 *copyto) const;
        void set_registers(const uint16 *copyfrom);
};

class BCCMD_VM_DEBUG_POLLTRACE_PDU : public BCCMD_VM_DEBUG_PDU
{
    public:
        BCCMD_VM_DEBUG_POLLTRACE_PDU();
        BCCMD_VM_DEBUG_POLLTRACE_PDU(const PDU&);
        BCCMD_VM_DEBUG_POLLTRACE_PDU(const uint8 *buffer, uint32 len);
        uint16 get_len(void) const;
        void set_len(uint16 value);
        void get_data(uint16 *copyto, uint16 len) const;
        void set_data(const uint16 *copyfrom, uint16 len);
};

class BCCMD_VM_DEBUG_STEPOVER_PDU : public BCCMD_VM_DEBUG_PDU
{
    public:
        BCCMD_VM_DEBUG_STEPOVER_PDU();
        BCCMD_VM_DEBUG_STEPOVER_PDU(const PDU&);
        BCCMD_VM_DEBUG_STEPOVER_PDU(const uint8 *buffer, uint32 len);
        uint16 get_len(void) const;
        void set_len(uint16 value);
        uint16 get_ok(void) const;
        void set_ok(uint16 value);
        void get_ranges(uint16 *copyto, uint16 len) const;
        void set_ranges(const uint16 *copyfrom, uint16 len);
};

class BCCMD_VM_DEBUG_GETPANICREASON_PDU : public BCCMD_VM_DEBUG_PDU
{
    public:
        BCCMD_VM_DEBUG_GETPANICREASON_PDU();
        BCCMD_VM_DEBUG_GETPANICREASON_PDU(const PDU&);
        BCCMD_VM_DEBUG_GETPANICREASON_PDU(const uint8 *buffer, uint32 len);
        /* No set routines: this is purely reading info from the firmware */
        uint16 get_panic_reason(void) const;
        uint16 get_extra_context(void) const;
};

class BCCMD_VM_DEBUG_GETAPPSTART_PDU : public BCCMD_VM_DEBUG_PDU
{
    public:
        BCCMD_VM_DEBUG_GETAPPSTART_PDU();
        BCCMD_VM_DEBUG_GETAPPSTART_PDU(const PDU&);
        BCCMD_VM_DEBUG_GETAPPSTART_PDU(const uint8 *buffer, uint32 len);
        /* No set routines: this is purely reading info from the firmware */
        uint32 get_app_start_addr(void) const;
};


class BCCMD_VM_DEBUG_GETAPPFLAGS_PDU : public BCCMD_VM_DEBUG_PDU
{
    public:
        BCCMD_VM_DEBUG_GETAPPFLAGS_PDU();
        BCCMD_VM_DEBUG_GETAPPFLAGS_PDU(const PDU&);
        BCCMD_VM_DEBUG_GETAPPFLAGS_PDU(const uint8 *buffer, uint32 len);
        /* No set routines: this is purely reading info from the firmware */
        uint16 get_flags(void) const;
};

class BCCMD_VM_DEBUG_SETFEATURESTATE_PDU : public BCCMD_VM_DEBUG_PDU
{
 public:
  BCCMD_VM_DEBUG_SETFEATURESTATE_PDU();
  BCCMD_VM_DEBUG_SETFEATURESTATE_PDU(const PDU&);
  BCCMD_VM_DEBUG_SETFEATURESTATE_PDU(const uint8 *buffer, uint32 len);
  void set_features(uint16 value);
  uint16 get_rejected(void) const;
};

#endif /* #ifndef __XTRABCCMDVMDEBUGPDU_H__ */
