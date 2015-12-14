////////////////////////////////////////////////////////////////////////////////
//
//  File:       xtrabccmdvmdebug.cpp
//
//  Class:      BCCMD_VM_DEBUG_SIMPLE_PDU
//              BCCMD_VM_DEBUG_READMEM_PDU
//              BCCMD_VM_DEBUG_CHANGEDMEM_PDU
//              BCCMD_VM_DEBUG_GETMEMMAP_PDU
//
//  Purpose:    These need to be hand coded because some of them use arrays.
//
////////////////////////////////////////////////////////////////////////////////

#include "xtrabccmdvmdebugpdu.h"
#include "bcflayout.h"

enum {
    OFFSET_START(BCCMD_VM_DEBUG_SIMPLE)
	OFFSET_uint16(BCCMD_VM_DEBUG_SIMPLE_req_type),
	OFFSET_uint16(BCCMD_VM_DEBUG_SIMPLE_length),
	OFFSET_uint16(BCCMD_VM_DEBUG_SIMPLE_seq_no),
	OFFSET_uint16(BCCMD_VM_DEBUG_SIMPLE_varid),
	OFFSET_uint16(BCCMD_VM_DEBUG_SIMPLE_status),
    OFFSET_uint16(BCCMD_VM_DEBUG_SIMPLE_function),
    OFFSET_uint16(BCCMD_VM_DEBUG_SIMPLE_u16),
    BCCMD_VM_DEBUG_SIMPLE_pdusize
};

enum {
    OFFSET_START(BCCMD_VM_DEBUG_READMEM)
	OFFSET_uint16(BCCMD_VM_DEBUG_READMEM_req_type),
	OFFSET_uint16(BCCMD_VM_DEBUG_READMEM_length),
	OFFSET_uint16(BCCMD_VM_DEBUG_READMEM_seq_no),
	OFFSET_uint16(BCCMD_VM_DEBUG_READMEM_varid),
	OFFSET_uint16(BCCMD_VM_DEBUG_READMEM_status),
    OFFSET_uint16(BCCMD_VM_DEBUG_READMEM_function),
    OFFSET_uint16(BCCMD_VM_DEBUG_READMEM_start),
    OFFSET_uint16(BCCMD_VM_DEBUG_READMEM_len),
    OFFSET_uint16(BCCMD_VM_DEBUG_READMEM_mem),
    BCCMD_VM_DEBUG_READMEM_pdusize
};

enum {
    OFFSET_START(BCCMD_VM_DEBUG_CHANGEDMEM)
	OFFSET_uint16(BCCMD_VM_DEBUG_CHANGEDMEM_req_type),
	OFFSET_uint16(BCCMD_VM_DEBUG_CHANGEDMEM_length),
	OFFSET_uint16(BCCMD_VM_DEBUG_CHANGEDMEM_seq_no),
	OFFSET_uint16(BCCMD_VM_DEBUG_CHANGEDMEM_varid),
	OFFSET_uint16(BCCMD_VM_DEBUG_CHANGEDMEM_status),
    OFFSET_uint16(BCCMD_VM_DEBUG_CHANGEDMEM_function),
    OFFSET_uint16(BCCMD_VM_DEBUG_CHANGEDMEM_len),
    OFFSET_uint16(BCCMD_VM_DEBUG_CHANGEDMEM_dummy),
    OFFSET_uint16(BCCMD_VM_DEBUG_CHANGEDMEM_regions),
    BCCMD_VM_DEBUG_CHANGEDMEM_pdusize
};

enum {
    OFFSET_START(BCCMD_VM_DEBUG_GETMEMMAP)
	OFFSET_uint16(BCCMD_VM_DEBUG_GETMEMMAP_req_type),
	OFFSET_uint16(BCCMD_VM_DEBUG_GETMEMMAP_length),
	OFFSET_uint16(BCCMD_VM_DEBUG_GETMEMMAP_seq_no),
	OFFSET_uint16(BCCMD_VM_DEBUG_GETMEMMAP_varid),
	OFFSET_uint16(BCCMD_VM_DEBUG_GETMEMMAP_status),
    OFFSET_uint16(BCCMD_VM_DEBUG_GETMEMMAP_function),
    OFFSET_uint16(BCCMD_VM_DEBUG_GETMEMMAP_len),
    OFFSET_uint16(BCCMD_VM_DEBUG_GETMEMMAP_dummy),
    OFFSET_uint16(BCCMD_VM_DEBUG_GETMEMMAP_starts),
    BCCMD_VM_DEBUG_GETMEMMAP_pdusize
};

enum {
    OFFSET_START(BCCMD_VM_DEBUG_GETREGIONSIZES)
	OFFSET_uint16(BCCMD_VM_DEBUG_GETREGIONSIZES_req_type),
	OFFSET_uint16(BCCMD_VM_DEBUG_GETREGIONSIZES_length),
	OFFSET_uint16(BCCMD_VM_DEBUG_GETREGIONSIZES_seq_no),
	OFFSET_uint16(BCCMD_VM_DEBUG_GETREGIONSIZES_varid),
	OFFSET_uint16(BCCMD_VM_DEBUG_GETREGIONSIZES_status),
    OFFSET_uint16(BCCMD_VM_DEBUG_GETREGIONSIZES_function),
    OFFSET_uint16(BCCMD_VM_DEBUG_GETREGIONSIZES_len),
    OFFSET_uint16(BCCMD_VM_DEBUG_GETREGIONSIZES_dummy),
    OFFSET_uint16(BCCMD_VM_DEBUG_GETREGIONSIZES_sizes),
    BCCMD_VM_DEBUG_GETREGIONSIZES_pdusize
};

enum {
    OFFSET_START(BCCMD_VM_DEBUG_SETBREAKPOINTS)
	OFFSET_uint16(BCCMD_VM_DEBUG_SETBREAKPOINTS_req_type),
	OFFSET_uint16(BCCMD_VM_DEBUG_SETBREAKPOINTS_length),
	OFFSET_uint16(BCCMD_VM_DEBUG_SETBREAKPOINTS_seq_no),
	OFFSET_uint16(BCCMD_VM_DEBUG_SETBREAKPOINTS_varid),
	OFFSET_uint16(BCCMD_VM_DEBUG_SETBREAKPOINTS_status),
    OFFSET_uint16(BCCMD_VM_DEBUG_SETBREAKPOINTS_function),
    OFFSET_uint16(BCCMD_VM_DEBUG_SETBREAKPOINTS_ok),
    OFFSET_uint16(BCCMD_VM_DEBUG_SETBREAKPOINTS_len),
    OFFSET_uint16(BCCMD_VM_DEBUG_SETBREAKPOINTS_breakpoints),
    BCCMD_VM_DEBUG_SETBREAKPOINTS_pdusize
};

enum {
    OFFSET_START(BCCMD_VM_DEBUG_CHECKAPP)
	OFFSET_uint16(BCCMD_VM_DEBUG_CHECKAPP_req_type),
	OFFSET_uint16(BCCMD_VM_DEBUG_CHECKAPP_length),
	OFFSET_uint16(BCCMD_VM_DEBUG_CHECKAPP_seq_no),
	OFFSET_uint16(BCCMD_VM_DEBUG_CHECKAPP_varid),
	OFFSET_uint16(BCCMD_VM_DEBUG_CHECKAPP_status),
    OFFSET_uint16(BCCMD_VM_DEBUG_CHECKAPP_function),
    OFFSET_uint16(BCCMD_VM_DEBUG_CHECKAPP_size),
    OFFSET_uint16(BCCMD_VM_DEBUG_CHECKAPP_checksum),
    BCCMD_VM_DEBUG_CHECKAPP_pdusize
};

enum {
    OFFSET_START(BCCMD_VM_DEBUG_GETCONTEXT)
	OFFSET_uint16(BCCMD_VM_DEBUG_GETCONTEXT_req_type),
	OFFSET_uint16(BCCMD_VM_DEBUG_GETCONTEXT_length),
	OFFSET_uint16(BCCMD_VM_DEBUG_GETCONTEXT_seq_no),
	OFFSET_uint16(BCCMD_VM_DEBUG_GETCONTEXT_varid),
	OFFSET_uint16(BCCMD_VM_DEBUG_GETCONTEXT_status),
    OFFSET_uint16(BCCMD_VM_DEBUG_GETCONTEXT_function),
    OFFSET_uint16(BCCMD_VM_DEBUG_GETCONTEXT_stat),
    OFFSET_uint16(BCCMD_VM_DEBUG_GETCONTEXT_pc),
    OFFSET_uint16(BCCMD_VM_DEBUG_GETCONTEXT_registers),
    BCCMD_VM_DEBUG_GETCONTEXT_pdusize
};

enum {
    OFFSET_START(BCCMD_VM_DEBUG_POLLTRACE)
	OFFSET_uint16(BCCMD_VM_DEBUG_POLLTRACE_req_type),
	OFFSET_uint16(BCCMD_VM_DEBUG_POLLTRACE_length),
	OFFSET_uint16(BCCMD_VM_DEBUG_POLLTRACE_seq_no),
	OFFSET_uint16(BCCMD_VM_DEBUG_POLLTRACE_varid),
	OFFSET_uint16(BCCMD_VM_DEBUG_POLLTRACE_status),
    OFFSET_uint16(BCCMD_VM_DEBUG_POLLTRACE_function),
    OFFSET_uint16(BCCMD_VM_DEBUG_POLLTRACE_len),
    OFFSET_uint16(BCCMD_VM_DEBUG_POLLTRACE_dummy),
    OFFSET_uint16(BCCMD_VM_DEBUG_POLLTRACE_data),
    BCCMD_VM_DEBUG_POLLTRACE_pdusize
};

enum {
    OFFSET_START(BCCMD_VM_DEBUG_STEPOVER)
	OFFSET_uint16(BCCMD_VM_DEBUG_STEPOVER_req_type),
	OFFSET_uint16(BCCMD_VM_DEBUG_STEPOVER_length),
	OFFSET_uint16(BCCMD_VM_DEBUG_STEPOVER_seq_no),
	OFFSET_uint16(BCCMD_VM_DEBUG_STEPOVER_varid),
	OFFSET_uint16(BCCMD_VM_DEBUG_STEPOVER_status),
    OFFSET_uint16(BCCMD_VM_DEBUG_STEPOVER_function),
    OFFSET_uint16(BCCMD_VM_DEBUG_STEPOVER_len),
    OFFSET_uint16(BCCMD_VM_DEBUG_STEPOVER_ok),
    OFFSET_uint16(BCCMD_VM_DEBUG_STEPOVER_ranges),
    BCCMD_VM_DEBUG_STEPOVER_pdusize
};

enum {
    OFFSET_START(BCCMD_VM_DEBUG_GETPANICREASON)
    OFFSET_uint16(BCCMD_VM_DEBUG_GETPANICREASON_req_type),
    OFFSET_uint16(BCCMD_VM_DEBUG_GETPANICREASON_length),
    OFFSET_uint16(BCCMD_VM_DEBUG_GETPANICREASON_seq_no),
    OFFSET_uint16(BCCMD_VM_DEBUG_GETPANICREASON_varid),
    OFFSET_uint16(BCCMD_VM_DEBUG_GETPANICREASON_status),
    OFFSET_uint16(BCCMD_VM_DEBUG_GETPANICREASON_function),
    OFFSET_uint16(BCCMD_VM_DEBUG_GETPANICREASON_panic_reason),
    OFFSET_uint16(BCCMD_VM_DEBUG_GETPANICREASON_extra_context),
    BCCMD_VM_DEBUG_GETPANICREASON_pdusize
};

enum {
    OFFSET_START(BCCMD_VM_DEBUG_GETAPPSTART)
    OFFSET_uint16(BCCMD_VM_DEBUG_GETAPPSTART_req_type),
    OFFSET_uint16(BCCMD_VM_DEBUG_GETAPPSTART_length),
    OFFSET_uint16(BCCMD_VM_DEBUG_GETAPPSTART_seq_no),
    OFFSET_uint16(BCCMD_VM_DEBUG_GETAPPSTART_varid),
    OFFSET_uint16(BCCMD_VM_DEBUG_GETAPPSTART_status),
    OFFSET_uint16(BCCMD_VM_DEBUG_GETAPPSTART_function),
    OFFSET_uint32(BCCMD_VM_DEBUG_GETAPPSTART_app_start_addr),
    BCCMD_VM_DEBUG_GETAPPSTART_pdusize
};

enum {
    OFFSET_START(BCCMD_VM_DEBUG_GETAPPFLAGS)
	OFFSET_uint16(BCCMD_VM_DEBUG_GETAPPFLAGS_req_type),
	OFFSET_uint16(BCCMD_VM_DEBUG_GETAPPFLAGS_length),
	OFFSET_uint16(BCCMD_VM_DEBUG_GETAPPFLAGS_seq_no),
	OFFSET_uint16(BCCMD_VM_DEBUG_GETAPPFLAGS_varid),
	OFFSET_uint16(BCCMD_VM_DEBUG_GETAPPFLAGS_status),
    OFFSET_uint16(BCCMD_VM_DEBUG_GETAPPFLAGS_function),
    OFFSET_uint16(BCCMD_VM_DEBUG_GETAPPFLAGS_flags),
    BCCMD_VM_DEBUG_GETAPPFLAGS_pdusize
};

enum {
  OFFSET_START(BCCMD_VM_DEBUG_SETFEATURESTATE)
  OFFSET_uint16(BCCMD_VM_DEBUG_SETFEATURESTATE_req_type),
  OFFSET_uint16(BCCMD_VM_DEBUG_SETFEATURESTATE_length),
  OFFSET_uint16(BCCMD_VM_DEBUG_SETFEATURESTATE_seq_no),
  OFFSET_uint16(BCCMD_VM_DEBUG_SETFEATURESTATE_varid),
  OFFSET_uint16(BCCMD_VM_DEBUG_SETFEATURESTATE_status),
  OFFSET_uint16(BCCMD_VM_DEBUG_SETFEATURESTATE_function),
  OFFSET_uint16(BCCMD_VM_DEBUG_SETFEATURESTATE_features),
  OFFSET_uint16(BCCMD_VM_DEBUG_SETFEATURESTATE_rejected),
  BCCMD_VM_DEBUG_SETFEATURESTATE_pdusize
};


/******************************************************************************/

BCCMD_VM_DEBUG_SIMPLE_PDU::BCCMD_VM_DEBUG_SIMPLE_PDU()
    : BCCMD_VM_DEBUG_PDU( BCCMDVARID_VM_DEBUG )
{
}

BCCMD_VM_DEBUG_SIMPLE_PDU::BCCMD_VM_DEBUG_SIMPLE_PDU(const PDU& from)
    : BCCMD_VM_DEBUG_PDU( from )
{
}

BCCMD_VM_DEBUG_SIMPLE_PDU::BCCMD_VM_DEBUG_SIMPLE_PDU(const uint8 *buffer, uint32 len)
    : BCCMD_VM_DEBUG_PDU(buffer, len)
{
}

uint16 BCCMD_VM_DEBUG_SIMPLE_PDU::get_u16(void) const
{
    return get_uint16(BCCMD_VM_DEBUG_SIMPLE_u16);
}

void BCCMD_VM_DEBUG_SIMPLE_PDU::set_u16(uint16 value)
{
    set_uint16(BCCMD_VM_DEBUG_SIMPLE_u16,value);
}


/******************************************************************************/


BCCMD_VM_DEBUG_READMEM_PDU::BCCMD_VM_DEBUG_READMEM_PDU()
    : BCCMD_VM_DEBUG_PDU( BCCMDVARID_VM_DEBUG )
{
    set_function(BCCMDPDU_VM_DEBUG_READMEMORY);
}

BCCMD_VM_DEBUG_READMEM_PDU::BCCMD_VM_DEBUG_READMEM_PDU(const PDU& from)
    : BCCMD_VM_DEBUG_PDU( from )
{
    set_function(BCCMDPDU_VM_DEBUG_READMEMORY);
}

BCCMD_VM_DEBUG_READMEM_PDU::BCCMD_VM_DEBUG_READMEM_PDU(const uint8 *buffer, uint32 len)
    : BCCMD_VM_DEBUG_PDU(buffer, len)
{
    set_function(BCCMDPDU_VM_DEBUG_READMEMORY);
}

uint16 BCCMD_VM_DEBUG_READMEM_PDU::get_start(void) const
{
    return get_uint16(BCCMD_VM_DEBUG_READMEM_start);
}

void BCCMD_VM_DEBUG_READMEM_PDU::set_start(uint16 value)
{
    set_uint16(BCCMD_VM_DEBUG_READMEM_start,value);
}

uint16 BCCMD_VM_DEBUG_READMEM_PDU::get_len(void) const
{
    return get_uint16(BCCMD_VM_DEBUG_READMEM_len);
}

void BCCMD_VM_DEBUG_READMEM_PDU::set_len(uint16 value)
{
    resize (value * 2 + BCCMD_VM_DEBUG_READMEM_mem);
    set_uint16(BCCMD_VM_DEBUG_READMEM_len,value);
    set_length();
}

void BCCMD_VM_DEBUG_READMEM_PDU::get_mem(uint16 *copyto, uint16 len) const
{
    get_uint16Array(copyto,BCCMD_VM_DEBUG_READMEM_mem,len);
}

void BCCMD_VM_DEBUG_READMEM_PDU::set_mem(const uint16 *copyfrom, uint16 len)
{
    set_uint16Array(copyfrom,BCCMD_VM_DEBUG_READMEM_mem,len);
    set_length();
}


/******************************************************************************/


BCCMD_VM_DEBUG_CHANGEDMEM_PDU::BCCMD_VM_DEBUG_CHANGEDMEM_PDU()
    : BCCMD_VM_DEBUG_PDU( BCCMDVARID_VM_DEBUG )
{
    set_function(BCCMDPDU_VM_DEBUG_MEMCHANGED);
}

BCCMD_VM_DEBUG_CHANGEDMEM_PDU::BCCMD_VM_DEBUG_CHANGEDMEM_PDU(const PDU& from)
    : BCCMD_VM_DEBUG_PDU( from )
{
    set_function(BCCMDPDU_VM_DEBUG_MEMCHANGED);
}

BCCMD_VM_DEBUG_CHANGEDMEM_PDU::BCCMD_VM_DEBUG_CHANGEDMEM_PDU(const uint8 *buffer, uint32 len)
    : BCCMD_VM_DEBUG_PDU(buffer, len)
{
    set_function(BCCMDPDU_VM_DEBUG_MEMCHANGED);
}

uint16 BCCMD_VM_DEBUG_CHANGEDMEM_PDU::get_len(void) const
{
    return get_uint16(BCCMD_VM_DEBUG_CHANGEDMEM_len);
}

void BCCMD_VM_DEBUG_CHANGEDMEM_PDU::set_len(uint16 value)
{
    resize (value * 2 + BCCMD_VM_DEBUG_CHANGEDMEM_regions);
    set_uint16(BCCMD_VM_DEBUG_CHANGEDMEM_len,value);
    set_length();
}

void BCCMD_VM_DEBUG_CHANGEDMEM_PDU::get_regions(uint16 *copyto, uint16 len) const
{
    get_uint16Array(copyto,BCCMD_VM_DEBUG_CHANGEDMEM_regions,len);
}

void BCCMD_VM_DEBUG_CHANGEDMEM_PDU::set_regions(const uint16 *copyfrom, uint16 len)
{
    set_uint16Array(copyfrom,BCCMD_VM_DEBUG_CHANGEDMEM_regions,len);
    set_length();
}


/******************************************************************************/


BCCMD_VM_DEBUG_GETMEMMAP_PDU::BCCMD_VM_DEBUG_GETMEMMAP_PDU()
    : BCCMD_VM_DEBUG_PDU( BCCMDVARID_VM_DEBUG )
{
    set_function(BCCMDPDU_VM_DEBUG_GETMEMMAP);
}

BCCMD_VM_DEBUG_GETMEMMAP_PDU::BCCMD_VM_DEBUG_GETMEMMAP_PDU(const PDU& from)
    : BCCMD_VM_DEBUG_PDU( from )
{
    set_function(BCCMDPDU_VM_DEBUG_GETMEMMAP);
}

BCCMD_VM_DEBUG_GETMEMMAP_PDU::BCCMD_VM_DEBUG_GETMEMMAP_PDU(const uint8 *buffer, uint32 len)
    : BCCMD_VM_DEBUG_PDU(buffer, len)
{
    set_function(BCCMDPDU_VM_DEBUG_GETMEMMAP);
}

uint16 BCCMD_VM_DEBUG_GETMEMMAP_PDU::get_len(void) const
{
    return get_uint16(BCCMD_VM_DEBUG_GETMEMMAP_len);
}

void BCCMD_VM_DEBUG_GETMEMMAP_PDU::set_len(uint16 value)
{
    resize (value * 2 + BCCMD_VM_DEBUG_GETMEMMAP_starts);
    set_uint16(BCCMD_VM_DEBUG_GETMEMMAP_len,value);
    set_length();
}

void BCCMD_VM_DEBUG_GETMEMMAP_PDU::get_starts(uint16 *copyto, uint16 len) const
{
    get_uint16Array(copyto,BCCMD_VM_DEBUG_GETMEMMAP_starts,len);
}

void BCCMD_VM_DEBUG_GETMEMMAP_PDU::set_starts(const uint16 *copyfrom,uint16 len)
{
    set_uint16Array(copyfrom,BCCMD_VM_DEBUG_GETMEMMAP_starts,len);
    set_length();
}


/******************************************************************************/

BCCMD_VM_DEBUG_GETREGIONSIZES_PDU::BCCMD_VM_DEBUG_GETREGIONSIZES_PDU()
    : BCCMD_VM_DEBUG_PDU( BCCMDVARID_VM_DEBUG )
{
    set_function(BCCMDPDU_VM_DEBUG_GETREGIONSIZES);
}

BCCMD_VM_DEBUG_GETREGIONSIZES_PDU::BCCMD_VM_DEBUG_GETREGIONSIZES_PDU(
                                                                const PDU& from
                                                                    )
    : BCCMD_VM_DEBUG_PDU( from )
{
    set_function(BCCMDPDU_VM_DEBUG_GETREGIONSIZES);
}

BCCMD_VM_DEBUG_GETREGIONSIZES_PDU::BCCMD_VM_DEBUG_GETREGIONSIZES_PDU(
                                                            const uint8 *buffer,
                                                            uint32 len
                                                                    )
    : BCCMD_VM_DEBUG_PDU(buffer, len)
{
    set_function(BCCMDPDU_VM_DEBUG_GETREGIONSIZES);
}

uint16 BCCMD_VM_DEBUG_GETREGIONSIZES_PDU::get_len(void) const
{
    return get_uint16(BCCMD_VM_DEBUG_GETREGIONSIZES_len);
}

void BCCMD_VM_DEBUG_GETREGIONSIZES_PDU::set_len(uint16 value)
{
    resize (value * 2 + BCCMD_VM_DEBUG_GETREGIONSIZES_sizes);
    set_uint16(BCCMD_VM_DEBUG_GETREGIONSIZES_len,value);
    set_length();
}

void BCCMD_VM_DEBUG_GETREGIONSIZES_PDU::get_sizes(uint16 *copyto, uint16 len) const
{
    get_uint16Array(copyto,BCCMD_VM_DEBUG_GETREGIONSIZES_sizes,len);
}

void BCCMD_VM_DEBUG_GETREGIONSIZES_PDU::set_sizes(const uint16 *copyfrom, uint16 len)
{
    set_uint16Array(copyfrom,BCCMD_VM_DEBUG_GETREGIONSIZES_sizes,len);
    set_length();
}


/******************************************************************************/

BCCMD_VM_DEBUG_SETBREAKPOINTS_PDU::BCCMD_VM_DEBUG_SETBREAKPOINTS_PDU()
    : BCCMD_VM_DEBUG_PDU( BCCMDVARID_VM_DEBUG )
{
    set_function(BCCMDPDU_VM_DEBUG_SETBREAKPOINTS);
}

BCCMD_VM_DEBUG_SETBREAKPOINTS_PDU::BCCMD_VM_DEBUG_SETBREAKPOINTS_PDU(const PDU& from)
    : BCCMD_VM_DEBUG_PDU( from )
{
    set_function(BCCMDPDU_VM_DEBUG_SETBREAKPOINTS);
}

BCCMD_VM_DEBUG_SETBREAKPOINTS_PDU::BCCMD_VM_DEBUG_SETBREAKPOINTS_PDU(const uint8 *buffer,uint32 len)
    : BCCMD_VM_DEBUG_PDU(buffer, len)
{
    set_function(BCCMDPDU_VM_DEBUG_SETBREAKPOINTS);
}

uint16 BCCMD_VM_DEBUG_SETBREAKPOINTS_PDU::get_ok(void) const
{
    return get_uint16(BCCMD_VM_DEBUG_SETBREAKPOINTS_ok);
}

void BCCMD_VM_DEBUG_SETBREAKPOINTS_PDU::set_ok(uint16 value)
{
    set_uint16(BCCMD_VM_DEBUG_SETBREAKPOINTS_ok,value);
}

uint16 BCCMD_VM_DEBUG_SETBREAKPOINTS_PDU::get_len(void) const
{
    return get_uint16(BCCMD_VM_DEBUG_SETBREAKPOINTS_len);
}

void BCCMD_VM_DEBUG_SETBREAKPOINTS_PDU::set_len(uint16 value)
{
    set_uint16(BCCMD_VM_DEBUG_SETBREAKPOINTS_len,value);
}

void BCCMD_VM_DEBUG_SETBREAKPOINTS_PDU::get_breakpoints(uint16 *copyto, uint16 len) const
{
    get_uint16Array(copyto,BCCMD_VM_DEBUG_SETBREAKPOINTS_breakpoints,len);
}

void BCCMD_VM_DEBUG_SETBREAKPOINTS_PDU::set_breakpoints(const uint16 *copyfrom, uint16 len)
{
    set_uint16Array(copyfrom,BCCMD_VM_DEBUG_SETBREAKPOINTS_breakpoints,len);
    set_length();
}


/******************************************************************************/


BCCMD_VM_DEBUG_CHECKAPP_PDU::BCCMD_VM_DEBUG_CHECKAPP_PDU()
    : BCCMD_VM_DEBUG_PDU( BCCMDVARID_VM_DEBUG )
{
    set_function(BCCMDPDU_VM_DEBUG_CHECKAPP);
}

BCCMD_VM_DEBUG_CHECKAPP_PDU::BCCMD_VM_DEBUG_CHECKAPP_PDU(const PDU& from)
    : BCCMD_VM_DEBUG_PDU( from )
{
    set_function(BCCMDPDU_VM_DEBUG_CHECKAPP);
}

BCCMD_VM_DEBUG_CHECKAPP_PDU::BCCMD_VM_DEBUG_CHECKAPP_PDU(const uint8 *buffer, uint32 len)
    : BCCMD_VM_DEBUG_PDU(buffer, len)
{
    set_function(BCCMDPDU_VM_DEBUG_CHECKAPP);
}

uint16 BCCMD_VM_DEBUG_CHECKAPP_PDU::get_size(void) const
{
    return get_uint16(BCCMD_VM_DEBUG_CHECKAPP_size);
}

void BCCMD_VM_DEBUG_CHECKAPP_PDU::set_size(uint16 value)
{
    set_uint16(BCCMD_VM_DEBUG_CHECKAPP_size,value);
}

uint16 BCCMD_VM_DEBUG_CHECKAPP_PDU::get_checksum(void) const
{
    return get_uint16(BCCMD_VM_DEBUG_CHECKAPP_checksum);
}

void BCCMD_VM_DEBUG_CHECKAPP_PDU::set_checksum(uint16 value)
{
    set_uint16(BCCMD_VM_DEBUG_CHECKAPP_checksum,value);
}



/******************************************************************************/


BCCMD_VM_DEBUG_GETCONTEXT_PDU::BCCMD_VM_DEBUG_GETCONTEXT_PDU()
    : BCCMD_VM_DEBUG_PDU( BCCMDVARID_VM_DEBUG )
{
    resize(12+BCCMD_VM_DEBUG_GETCONTEXT_registers);
    set_function(BCCMDPDU_VM_DEBUG_GETCONTEXT);
    set_length();
}

BCCMD_VM_DEBUG_GETCONTEXT_PDU::BCCMD_VM_DEBUG_GETCONTEXT_PDU(const PDU& from)
    : BCCMD_VM_DEBUG_PDU( from )
{
    set_function(BCCMDPDU_VM_DEBUG_GETCONTEXT);
}

BCCMD_VM_DEBUG_GETCONTEXT_PDU::BCCMD_VM_DEBUG_GETCONTEXT_PDU(const uint8 *buffer, uint32 len)
    : BCCMD_VM_DEBUG_PDU(buffer, len)
{
    set_function(BCCMDPDU_VM_DEBUG_GETCONTEXT);
}

uint16 BCCMD_VM_DEBUG_GETCONTEXT_PDU::get_stat(void) const
{
    return get_uint16(BCCMD_VM_DEBUG_GETCONTEXT_stat);
}

void BCCMD_VM_DEBUG_GETCONTEXT_PDU::set_stat(uint16 value)
{
    set_uint16(BCCMD_VM_DEBUG_GETCONTEXT_stat,value);
}

uint16 BCCMD_VM_DEBUG_GETCONTEXT_PDU::get_pc(void) const
{
    return get_uint16(BCCMD_VM_DEBUG_GETCONTEXT_pc);
}

void BCCMD_VM_DEBUG_GETCONTEXT_PDU::set_pc(uint16 value)
{
    set_uint16(BCCMD_VM_DEBUG_GETCONTEXT_pc,value);
}

void BCCMD_VM_DEBUG_GETCONTEXT_PDU::get_registers(uint16 *copyto) const
{
    get_uint16Array(copyto,BCCMD_VM_DEBUG_GETCONTEXT_registers,6);
}

void BCCMD_VM_DEBUG_GETCONTEXT_PDU::set_registers(const uint16 *copyfrom)
{
    set_uint16Array(copyfrom,BCCMD_VM_DEBUG_GETCONTEXT_registers,6);
    set_length();
}


/******************************************************************************/


BCCMD_VM_DEBUG_POLLTRACE_PDU::BCCMD_VM_DEBUG_POLLTRACE_PDU()
    : BCCMD_VM_DEBUG_PDU( BCCMDVARID_VM_DEBUG )
{
    set_function(BCCMDPDU_VM_DEBUG_POLLTRACE);
}

BCCMD_VM_DEBUG_POLLTRACE_PDU::BCCMD_VM_DEBUG_POLLTRACE_PDU(const PDU& from)
    : BCCMD_VM_DEBUG_PDU( from )
{
    set_function(BCCMDPDU_VM_DEBUG_POLLTRACE);
}

BCCMD_VM_DEBUG_POLLTRACE_PDU::BCCMD_VM_DEBUG_POLLTRACE_PDU(const uint8 *buffer, uint32 len)
    : BCCMD_VM_DEBUG_PDU(buffer, len)
{
    set_function(BCCMDPDU_VM_DEBUG_POLLTRACE);
}

uint16 BCCMD_VM_DEBUG_POLLTRACE_PDU::get_len(void) const
{
    return get_uint16(BCCMD_VM_DEBUG_POLLTRACE_len);
}

void BCCMD_VM_DEBUG_POLLTRACE_PDU::set_len(uint16 value)
{
    resize (value + BCCMD_VM_DEBUG_POLLTRACE_data);
    set_uint16(BCCMD_VM_DEBUG_POLLTRACE_len,value);
    set_length();
}

void BCCMD_VM_DEBUG_POLLTRACE_PDU::get_data(uint16 *copyto, uint16 len) const
{
    get_uint16Array(copyto,BCCMD_VM_DEBUG_POLLTRACE_data,len);
}

void BCCMD_VM_DEBUG_POLLTRACE_PDU::set_data(const uint16 *copyfrom, uint16 len)
{
    set_uint16Array(copyfrom,BCCMD_VM_DEBUG_POLLTRACE_data,len);
    set_length();
}


/******************************************************************************/


BCCMD_VM_DEBUG_STEPOVER_PDU::BCCMD_VM_DEBUG_STEPOVER_PDU()
    : BCCMD_VM_DEBUG_PDU( BCCMDVARID_VM_DEBUG )
{
    set_function(BCCMDPDU_VM_DEBUG_STEPOVER);
}

BCCMD_VM_DEBUG_STEPOVER_PDU::BCCMD_VM_DEBUG_STEPOVER_PDU(const PDU& from)
    : BCCMD_VM_DEBUG_PDU( from )
{
    set_function(BCCMDPDU_VM_DEBUG_STEPOVER);
}

BCCMD_VM_DEBUG_STEPOVER_PDU::BCCMD_VM_DEBUG_STEPOVER_PDU(const uint8 *buffer,uint32 len)
    : BCCMD_VM_DEBUG_PDU(buffer, len)
{
    set_function(BCCMDPDU_VM_DEBUG_STEPOVER);
}

uint16 BCCMD_VM_DEBUG_STEPOVER_PDU::get_len(void) const
{
    return get_uint16(BCCMD_VM_DEBUG_STEPOVER_len);
}

void BCCMD_VM_DEBUG_STEPOVER_PDU::set_len(uint16 value)
{
    set_uint16(BCCMD_VM_DEBUG_STEPOVER_len,value);
}

uint16 BCCMD_VM_DEBUG_STEPOVER_PDU::get_ok(void) const
{
    return get_uint16(BCCMD_VM_DEBUG_STEPOVER_ok);
}

void BCCMD_VM_DEBUG_STEPOVER_PDU::set_ok(uint16 value)
{
    set_uint16(BCCMD_VM_DEBUG_STEPOVER_ok,value);
}

void BCCMD_VM_DEBUG_STEPOVER_PDU::get_ranges(uint16 *copyto, uint16 len) const
{
    get_uint16Array(copyto,BCCMD_VM_DEBUG_STEPOVER_ranges,len);
}

void BCCMD_VM_DEBUG_STEPOVER_PDU::set_ranges(const uint16 *copyfrom, uint16 len)
{
    set_uint16Array(copyfrom,BCCMD_VM_DEBUG_STEPOVER_ranges,len);
    set_length();
}


/******************************************************************************/


BCCMD_VM_DEBUG_GETPANICREASON_PDU::BCCMD_VM_DEBUG_GETPANICREASON_PDU()
    : BCCMD_VM_DEBUG_PDU( BCCMDVARID_VM_DEBUG )
{
    set_function(BCCMDPDU_VM_DEBUG_GETPANICREASON);
}

BCCMD_VM_DEBUG_GETPANICREASON_PDU::BCCMD_VM_DEBUG_GETPANICREASON_PDU(const PDU& from)
    : BCCMD_VM_DEBUG_PDU( from )
{
    set_function(BCCMDPDU_VM_DEBUG_GETPANICREASON);
}

BCCMD_VM_DEBUG_GETPANICREASON_PDU::BCCMD_VM_DEBUG_GETPANICREASON_PDU(const uint8 *buffer, uint32 len)
    : BCCMD_VM_DEBUG_PDU(buffer, len)
{
    set_function(BCCMDPDU_VM_DEBUG_GETPANICREASON);
}

uint16 BCCMD_VM_DEBUG_GETPANICREASON_PDU::get_panic_reason(void) const
{
    return get_uint16(BCCMD_VM_DEBUG_GETPANICREASON_panic_reason);
}

uint16 BCCMD_VM_DEBUG_GETPANICREASON_PDU::get_extra_context(void) const
{
    return get_uint16(BCCMD_VM_DEBUG_GETPANICREASON_extra_context);
}

BCCMD_VM_DEBUG_GETAPPSTART_PDU::BCCMD_VM_DEBUG_GETAPPSTART_PDU()
    : BCCMD_VM_DEBUG_PDU( BCCMDVARID_VM_DEBUG )
{
    set_function(BCCMDPDU_VM_DEBUG_GETAPPSTART);
}

BCCMD_VM_DEBUG_GETAPPSTART_PDU::BCCMD_VM_DEBUG_GETAPPSTART_PDU(const PDU& from)
    : BCCMD_VM_DEBUG_PDU( from )
{
    set_function(BCCMDPDU_VM_DEBUG_GETAPPSTART);
}

BCCMD_VM_DEBUG_GETAPPSTART_PDU::BCCMD_VM_DEBUG_GETAPPSTART_PDU(const uint8 *buffer, uint32 len)
    : BCCMD_VM_DEBUG_PDU(buffer, len)
{
    set_function(BCCMDPDU_VM_DEBUG_GETAPPSTART);
}

uint32 BCCMD_VM_DEBUG_GETAPPSTART_PDU::get_app_start_addr(void) const
{
    return get_uint32(BCCMD_VM_DEBUG_GETAPPSTART_app_start_addr);
}

/**********************************************************************/

BCCMD_VM_DEBUG_GETAPPFLAGS_PDU::BCCMD_VM_DEBUG_GETAPPFLAGS_PDU()
    : BCCMD_VM_DEBUG_PDU( BCCMDVARID_VM_DEBUG )
{
    set_function(BCCMDPDU_VM_DEBUG_GETAPPFLAGS);
}

BCCMD_VM_DEBUG_GETAPPFLAGS_PDU::BCCMD_VM_DEBUG_GETAPPFLAGS_PDU(const PDU& from)
    : BCCMD_VM_DEBUG_PDU( from )
{
    set_function(BCCMDPDU_VM_DEBUG_GETAPPFLAGS);
}

BCCMD_VM_DEBUG_GETAPPFLAGS_PDU::BCCMD_VM_DEBUG_GETAPPFLAGS_PDU(const uint8 *buffer, uint32 len)
    : BCCMD_VM_DEBUG_PDU(buffer, len)
{
    set_function(BCCMDPDU_VM_DEBUG_GETAPPFLAGS);
}

uint16 BCCMD_VM_DEBUG_GETAPPFLAGS_PDU::get_flags(void) const
{
    return get_uint16(BCCMD_VM_DEBUG_GETAPPFLAGS_flags);
}

/**********************************************************************/

BCCMD_VM_DEBUG_SETFEATURESTATE_PDU::BCCMD_VM_DEBUG_SETFEATURESTATE_PDU()
    : BCCMD_VM_DEBUG_PDU( BCCMDVARID_VM_DEBUG )
{
    set_function(BCCMDPDU_VM_DEBUG_SETFEATURESTATE);
}

BCCMD_VM_DEBUG_SETFEATURESTATE_PDU::BCCMD_VM_DEBUG_SETFEATURESTATE_PDU(const PDU& from)
    : BCCMD_VM_DEBUG_PDU( from )
{
    set_function(BCCMDPDU_VM_DEBUG_SETFEATURESTATE);
}

BCCMD_VM_DEBUG_SETFEATURESTATE_PDU::BCCMD_VM_DEBUG_SETFEATURESTATE_PDU(const uint8 *buffer, uint32 len)
    : BCCMD_VM_DEBUG_PDU(buffer, len)
{
    set_function(BCCMDPDU_VM_DEBUG_SETFEATURESTATE);
}

void BCCMD_VM_DEBUG_SETFEATURESTATE_PDU::set_features(uint16 value)
{
    set_uint16(BCCMD_VM_DEBUG_SETFEATURESTATE_features, value);
}

uint16 BCCMD_VM_DEBUG_SETFEATURESTATE_PDU::get_rejected(void) const
{
    return get_uint16(BCCMD_VM_DEBUG_SETFEATURESTATE_rejected);
}




