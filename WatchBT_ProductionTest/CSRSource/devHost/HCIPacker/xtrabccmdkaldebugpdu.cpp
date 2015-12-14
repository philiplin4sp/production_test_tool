////////////////////////////////////////////////////////////////////////////////
//
//  File:       xtrabccmdkaldebugpdu.cpp
//
//  Class:      ...
//
//  Purpose:    PDUs for debugging the Kalimba over BCCMD.
//
////////////////////////////////////////////////////////////////////////////////

#include "xtrabccmdkaldebugpdu.h"
#include "bcflayout.h"
#include "hcipacker/gen_bccmd.h"

enum {
    OFFSET_START(BCCMD_KALDEBUG_SIMPLE)
    OFFSET_uint16(BCCMD_KALDEBUG_SIMPLE_req_type),
    OFFSET_uint16(BCCMD_KALDEBUG_SIMPLE_length),
    OFFSET_uint16(BCCMD_KALDEBUG_SIMPLE_seq_no),
    OFFSET_uint16(BCCMD_KALDEBUG_SIMPLE_varid),
    OFFSET_uint16(BCCMD_KALDEBUG_SIMPLE_status),
    OFFSET_uint16(BCCMD_KALDEBUG_SIMPLE_function),
    OFFSET_uint16(BCCMD_KALDEBUG_SIMPLE_u16),
    BCCMD_KALDEBUG_SIMPLE_pdusize
};

enum {
    OFFSET_START(BCCMD_KALDEBUG_CONTEXT)
    OFFSET_uint16(BCCMD_KALDEBUG_CONTEXT_req_type),
    OFFSET_uint16(BCCMD_KALDEBUG_CONTEXT_length),
    OFFSET_uint16(BCCMD_KALDEBUG_CONTEXT_seq_no),
    OFFSET_uint16(BCCMD_KALDEBUG_CONTEXT_varid),
    OFFSET_uint16(BCCMD_KALDEBUG_CONTEXT_status),
    OFFSET_uint16(BCCMD_KALDEBUG_CONTEXT_function),
    OFFSET_uint16(BCCMD_KALDEBUG_CONTEXT_state),
    OFFSET_uint16(BCCMD_KALDEBUG_CONTEXT_dummy),
    OFFSET_uint16(BCCMD_KALDEBUG_CONTEXT_reg),
    BCCMD_KALDEBUG_CONTEXT_pdusize
};

enum {
    OFFSET_START(BCCMD_KALDEBUG_WRITEREG)
    OFFSET_uint16(BCCMD_KALDEBUG_WRITEREG_req_type),
    OFFSET_uint16(BCCMD_KALDEBUG_WRITEREG_length),
    OFFSET_uint16(BCCMD_KALDEBUG_WRITEREG_seq_no),
    OFFSET_uint16(BCCMD_KALDEBUG_WRITEREG_varid),
    OFFSET_uint16(BCCMD_KALDEBUG_WRITEREG_status),
    OFFSET_uint16(BCCMD_KALDEBUG_WRITEREG_function),
    OFFSET_uint16(BCCMD_KALDEBUG_WRITEREG_reg),
    OFFSET_uint16(BCCMD_KALDEBUG_WRITEREG_lsw),
    OFFSET_uint16(BCCMD_KALDEBUG_WRITEREG_msb),
    BCCMD_KALDEBUG_WRITEREG_pdusize
};

enum {
    OFFSET_START(BCCMD_KALDEBUG_READDM)
    OFFSET_uint16(BCCMD_KALDEBUG_READDM_req_type),
    OFFSET_uint16(BCCMD_KALDEBUG_READDM_length),
    OFFSET_uint16(BCCMD_KALDEBUG_READDM_seq_no),
    OFFSET_uint16(BCCMD_KALDEBUG_READDM_varid),
    OFFSET_uint16(BCCMD_KALDEBUG_READDM_status),
    OFFSET_uint16(BCCMD_KALDEBUG_READDM_function),
    OFFSET_uint16(BCCMD_KALDEBUG_READDM_start_u24),
    OFFSET_uint16(BCCMD_KALDEBUG_READDM_num_u16),
    OFFSET_uint16(BCCMD_KALDEBUG_READDM_data),
    BCCMD_KALDEBUG_READDM_pdusize
};

enum {
    OFFSET_START(BCCMD_KALDEBUG_WRITEDM)
    OFFSET_uint16(BCCMD_KALDEBUG_WRITEDM_req_type),
    OFFSET_uint16(BCCMD_KALDEBUG_WRITEDM_length),
    OFFSET_uint16(BCCMD_KALDEBUG_WRITEDM_seq_no),
    OFFSET_uint16(BCCMD_KALDEBUG_WRITEDM_varid),
    OFFSET_uint16(BCCMD_KALDEBUG_WRITEDM_status),
    OFFSET_uint16(BCCMD_KALDEBUG_WRITEDM_function),
    OFFSET_uint16(BCCMD_KALDEBUG_WRITEDM_start_u24),
    OFFSET_uint16(BCCMD_KALDEBUG_WRITEDM_num_u16),
    OFFSET_uint16(BCCMD_KALDEBUG_WRITEDM_data),
    BCCMD_KALDEBUG_WRITEDM_pdusize
};

enum {
    OFFSET_START(BCCMD_KALDEBUG_READPM)
    OFFSET_uint16(BCCMD_KALDEBUG_READPM_req_type),
    OFFSET_uint16(BCCMD_KALDEBUG_READPM_length),
    OFFSET_uint16(BCCMD_KALDEBUG_READPM_seq_no),
    OFFSET_uint16(BCCMD_KALDEBUG_READPM_varid),
    OFFSET_uint16(BCCMD_KALDEBUG_READPM_status),
    OFFSET_uint16(BCCMD_KALDEBUG_READPM_function),
    OFFSET_uint16(BCCMD_KALDEBUG_READPM_start_u32),
    OFFSET_uint16(BCCMD_KALDEBUG_READPM_num_u16),
    OFFSET_uint16(BCCMD_KALDEBUG_READPM_data),
    BCCMD_KALDEBUG_READPM_pdusize
};

enum {
    OFFSET_START(BCCMD_KALDEBUG_WRITEPM)
    OFFSET_uint16(BCCMD_KALDEBUG_WRITEPM_req_type),
    OFFSET_uint16(BCCMD_KALDEBUG_WRITEPM_length),
    OFFSET_uint16(BCCMD_KALDEBUG_WRITEPM_seq_no),
    OFFSET_uint16(BCCMD_KALDEBUG_WRITEPM_varid),
    OFFSET_uint16(BCCMD_KALDEBUG_WRITEPM_status),
    OFFSET_uint16(BCCMD_KALDEBUG_WRITEPM_function),
    OFFSET_uint16(BCCMD_KALDEBUG_WRITEPM_start_u32),
    OFFSET_uint16(BCCMD_KALDEBUG_WRITEPM_num_u16),
    OFFSET_uint16(BCCMD_KALDEBUG_WRITEPM_data),
    BCCMD_KALDEBUG_WRITEPM_pdusize
};

enum {
    OFFSET_START(BCCMD_KALDEBUG_SETDATABREAKPOINT)
    OFFSET_uint16(BCCMD_KALDEBUG_SETDATABREAKPOINT_req_type),
    OFFSET_uint16(BCCMD_KALDEBUG_SETDATABREAKPOINT_length),
    OFFSET_uint16(BCCMD_KALDEBUG_SETDATABREAKPOINT_seq_no),
    OFFSET_uint16(BCCMD_KALDEBUG_SETDATABREAKPOINT_varid),
    OFFSET_uint16(BCCMD_KALDEBUG_SETDATABREAKPOINT_status),
    OFFSET_uint16(BCCMD_KALDEBUG_SETDATABREAKPOINT_function),
    OFFSET_uint16(BCCMD_KALDEBUG_SETDATABREAKPOINT_addr),
    OFFSET_uint16(BCCMD_KALDEBUG_SETDATABREAKPOINT_read),
    OFFSET_uint16(BCCMD_KALDEBUG_SETDATABREAKPOINT_write),
    BCCMD_KALDEBUG_SETDATABREAKPOINT_pdusize
};

enum {
    OFFSET_START(BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT)
    OFFSET_uint16(BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT_req_type),
    OFFSET_uint16(BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT_length),
    OFFSET_uint16(BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT_seq_no),
    OFFSET_uint16(BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT_varid),
    OFFSET_uint16(BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT_status),
    OFFSET_uint16(BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT_function),
    OFFSET_uint16(BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT_addr),
    OFFSET_uint16(BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT_brk_point_number),
    BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT_pdusize
};

/******************************************************************************/

BCCMD_KALDEBUG_SIMPLE_PDU::BCCMD_KALDEBUG_SIMPLE_PDU()
        : BCCMD_KALDEBUG_PDU( BCCMDVARID_KALDEBUG )
{
}

BCCMD_KALDEBUG_SIMPLE_PDU::BCCMD_KALDEBUG_SIMPLE_PDU(const PDU& from)
        : BCCMD_KALDEBUG_PDU( from )
{
}

BCCMD_KALDEBUG_SIMPLE_PDU::BCCMD_KALDEBUG_SIMPLE_PDU(const uint8 *buffer, uint32 len)
        : BCCMD_KALDEBUG_PDU(buffer, len)
{
}

uint16 BCCMD_KALDEBUG_SIMPLE_PDU::get_u16(void) const
{
        return get_uint16(BCCMD_KALDEBUG_SIMPLE_u16);
}

void BCCMD_KALDEBUG_SIMPLE_PDU::set_u16(uint16 value)
{
        set_uint16(BCCMD_KALDEBUG_SIMPLE_u16,value);
}

/******************************************************************************/

/* Having fixed B-18370, adjust for M-1069 to read/write NUMSTALLS register. */
/* Add another unit16 for the ADDSUB_SATURATE_ON_OVERFLOW register B-14240   */
const int BCCMD_KALDEBUG_CONTEXT_PDU::num_words_in_regs = 53+2+1; 

BCCMD_KALDEBUG_CONTEXT_PDU::BCCMD_KALDEBUG_CONTEXT_PDU()
        : BCCMD_KALDEBUG_PDU( BCCMDVARID_KALDEBUG )
{
        resize (num_words_in_regs * 2 + BCCMD_KALDEBUG_CONTEXT_reg);
        set_length();
}

BCCMD_KALDEBUG_CONTEXT_PDU::BCCMD_KALDEBUG_CONTEXT_PDU(const PDU& from)
        : BCCMD_KALDEBUG_PDU( from )
{
        resize (num_words_in_regs * 2 + BCCMD_KALDEBUG_CONTEXT_reg);
        set_length();
}

BCCMD_KALDEBUG_CONTEXT_PDU::BCCMD_KALDEBUG_CONTEXT_PDU(const uint8 *buffer, uint32 len)
        : BCCMD_KALDEBUG_PDU(buffer, len)
{
        resize (num_words_in_regs * 2 + BCCMD_KALDEBUG_CONTEXT_reg);
        set_length();
}

uint16 BCCMD_KALDEBUG_CONTEXT_PDU::get_state(void) const
{
        return get_uint16(BCCMD_KALDEBUG_CONTEXT_state);
}

void BCCMD_KALDEBUG_CONTEXT_PDU::set_state(uint16 value)
{
        set_uint16(BCCMD_KALDEBUG_CONTEXT_state,value);
}

void BCCMD_KALDEBUG_CONTEXT_PDU::get_reg(uint16 *copyto, uint16 len) const
{
        get_uint16Array(copyto,BCCMD_KALDEBUG_CONTEXT_reg,len);
}

void BCCMD_KALDEBUG_CONTEXT_PDU::set_reg(const uint16 *copyfrom, uint16 len)
{
        set_uint16Array(copyfrom,BCCMD_KALDEBUG_CONTEXT_reg,len);
        set_length();
}


/******************************************************************************/

BCCMD_KALDEBUG_WRITEREG_PDU::BCCMD_KALDEBUG_WRITEREG_PDU()
        : BCCMD_KALDEBUG_PDU( BCCMDVARID_KALDEBUG )
{
    set_function(BCCMDPDU_KALDEBUG_WRITEREG);
}

BCCMD_KALDEBUG_WRITEREG_PDU::BCCMD_KALDEBUG_WRITEREG_PDU(const PDU& from)
        : BCCMD_KALDEBUG_PDU( from )
{
}

BCCMD_KALDEBUG_WRITEREG_PDU::BCCMD_KALDEBUG_WRITEREG_PDU(const uint8 *buffer, uint32 len)
        : BCCMD_KALDEBUG_PDU(buffer, len)
{
}

uint16 BCCMD_KALDEBUG_WRITEREG_PDU::get_reg(void) const
{
        return get_uint16(BCCMD_KALDEBUG_WRITEREG_reg);
}

void BCCMD_KALDEBUG_WRITEREG_PDU::set_reg(uint16 value)
{
       set_uint16(BCCMD_KALDEBUG_WRITEREG_reg,value);
}

uint16 BCCMD_KALDEBUG_WRITEREG_PDU::get_lsw(void) const
{
        return get_uint16(BCCMD_KALDEBUG_WRITEREG_lsw);
}

void BCCMD_KALDEBUG_WRITEREG_PDU::set_lsw(uint16 value)
{
        set_uint16(BCCMD_KALDEBUG_WRITEREG_lsw,value);
}

uint16 BCCMD_KALDEBUG_WRITEREG_PDU::get_msb(void) const
{
        return get_uint16(BCCMD_KALDEBUG_WRITEREG_msb);
}

void BCCMD_KALDEBUG_WRITEREG_PDU::set_msb(uint16 value)
{
        set_uint16(BCCMD_KALDEBUG_WRITEREG_msb,value);
}

/******************************************************************************/

BCCMD_KALDEBUG_READDM_PDU::BCCMD_KALDEBUG_READDM_PDU()
        : BCCMD_KALDEBUG_PDU( BCCMDVARID_KALDEBUG )
{
}

BCCMD_KALDEBUG_READDM_PDU::BCCMD_KALDEBUG_READDM_PDU(const PDU& from)
        : BCCMD_KALDEBUG_PDU( from )
{
}

BCCMD_KALDEBUG_READDM_PDU::BCCMD_KALDEBUG_READDM_PDU(const uint8 *buffer, uint32 len)
        : BCCMD_KALDEBUG_PDU(buffer, len)
{
}

uint16 BCCMD_KALDEBUG_READDM_PDU::get_start_u24(void) const
{
        return get_uint16(BCCMD_KALDEBUG_READDM_start_u24);
}

void BCCMD_KALDEBUG_READDM_PDU::set_start_u24(uint16 value)
{
        set_uint16(BCCMD_KALDEBUG_READDM_start_u24,value);
}

uint16 BCCMD_KALDEBUG_READDM_PDU::get_num_u16(void) const
{
        return get_uint16(BCCMD_KALDEBUG_READDM_num_u16);
}

void BCCMD_KALDEBUG_READDM_PDU::set_num_u16(uint16 value)
{
        resize (value * 2 + BCCMD_KALDEBUG_READDM_data);
        set_uint16(BCCMD_KALDEBUG_READDM_num_u16,value);
        set_length();
}

void BCCMD_KALDEBUG_READDM_PDU::get_data(uint16 *copyto, uint16 len) const
{
        get_uint16Array(copyto,BCCMD_KALDEBUG_READDM_data,len);
}

void BCCMD_KALDEBUG_READDM_PDU::set_data(const uint16 *copyfrom, uint16 len)
{
        set_uint16Array(copyfrom,BCCMD_KALDEBUG_READDM_data,len);
        set_length();
}


/******************************************************************************/
BCCMD_KALDEBUG_READPM_PDU::BCCMD_KALDEBUG_READPM_PDU()
        : BCCMD_KALDEBUG_PDU( BCCMDVARID_KALDEBUG )
{
}

BCCMD_KALDEBUG_READPM_PDU::BCCMD_KALDEBUG_READPM_PDU(const PDU& from)
        : BCCMD_KALDEBUG_PDU( from )
{
}

BCCMD_KALDEBUG_READPM_PDU::BCCMD_KALDEBUG_READPM_PDU(const uint8 *buffer, uint32 len)
        : BCCMD_KALDEBUG_PDU(buffer, len)
{
}

uint16 BCCMD_KALDEBUG_READPM_PDU::get_start_u32(void) const
{
        return get_uint16(BCCMD_KALDEBUG_READPM_start_u32);
}

void BCCMD_KALDEBUG_READPM_PDU::set_start_u32(uint16 value)
{
        set_uint16(BCCMD_KALDEBUG_READPM_start_u32,value);
}

uint16 BCCMD_KALDEBUG_READPM_PDU::get_num_u16(void) const
{
        return get_uint16(BCCMD_KALDEBUG_READPM_num_u16);
}

void BCCMD_KALDEBUG_READPM_PDU::set_num_u16(uint16 value)
{
        resize (value * 2 + BCCMD_KALDEBUG_READPM_data);
        set_uint16(BCCMD_KALDEBUG_READPM_num_u16,value);
        set_length();
}

void BCCMD_KALDEBUG_READPM_PDU::get_data(uint16 *copyto, uint16 len) const
{
        get_uint16Array(copyto,BCCMD_KALDEBUG_READPM_data,len);
}

void BCCMD_KALDEBUG_READPM_PDU::set_data(const uint16 *copyfrom, uint16 len)
{
        set_uint16Array(copyfrom,BCCMD_KALDEBUG_READPM_data,len);
        set_length();
}


/******************************************************************************/


BCCMD_KALDEBUG_WRITEDM_PDU::BCCMD_KALDEBUG_WRITEDM_PDU()
        : BCCMD_KALDEBUG_PDU( BCCMDVARID_KALDEBUG )
{
}

BCCMD_KALDEBUG_WRITEDM_PDU::BCCMD_KALDEBUG_WRITEDM_PDU(const PDU& from)
        : BCCMD_KALDEBUG_PDU( from )
{
}

BCCMD_KALDEBUG_WRITEDM_PDU::BCCMD_KALDEBUG_WRITEDM_PDU(const uint8 *buffer, uint32 len)
        : BCCMD_KALDEBUG_PDU(buffer, len)
{
}

uint16 BCCMD_KALDEBUG_WRITEDM_PDU::get_start_u24(void) const
{
        return get_uint16(BCCMD_KALDEBUG_WRITEDM_start_u24);
}

void BCCMD_KALDEBUG_WRITEDM_PDU::set_start_u24(uint16 value)
{
        set_uint16(BCCMD_KALDEBUG_WRITEDM_start_u24,value);
}

uint16 BCCMD_KALDEBUG_WRITEDM_PDU::get_num_u16(void) const
{
        return get_uint16(BCCMD_KALDEBUG_WRITEDM_num_u16);
}

void BCCMD_KALDEBUG_WRITEDM_PDU::set_num_u16(uint16 value)
{
        resize (value * 2 + BCCMD_KALDEBUG_WRITEDM_data);
        set_uint16(BCCMD_KALDEBUG_WRITEDM_num_u16,value);
        set_length();
}

void BCCMD_KALDEBUG_WRITEDM_PDU::get_data(uint16 *copyto, uint16 len) const
{
        get_uint16Array(copyto,BCCMD_KALDEBUG_WRITEDM_data,len);
}

void BCCMD_KALDEBUG_WRITEDM_PDU::set_data(const uint16 *copyfrom, uint16 len)
{
        set_uint16Array(copyfrom,BCCMD_KALDEBUG_WRITEDM_data,len);
        set_length();
}


/******************************************************************************/

BCCMD_KALDEBUG_WRITEPM_PDU::BCCMD_KALDEBUG_WRITEPM_PDU()
        : BCCMD_KALDEBUG_PDU( BCCMDVARID_KALDEBUG )
{
}

BCCMD_KALDEBUG_WRITEPM_PDU::BCCMD_KALDEBUG_WRITEPM_PDU(const PDU& from)
        : BCCMD_KALDEBUG_PDU( from )
{
}

BCCMD_KALDEBUG_WRITEPM_PDU::BCCMD_KALDEBUG_WRITEPM_PDU(const uint8 *buffer, uint32 len)
        : BCCMD_KALDEBUG_PDU(buffer, len)
{
}

uint16 BCCMD_KALDEBUG_WRITEPM_PDU::get_start_u32(void) const
{
        return get_uint16(BCCMD_KALDEBUG_WRITEPM_start_u32);
}

void BCCMD_KALDEBUG_WRITEPM_PDU::set_start_u32(uint16 value)
{
        set_uint16(BCCMD_KALDEBUG_WRITEPM_start_u32,value);
}

uint16 BCCMD_KALDEBUG_WRITEPM_PDU::get_num_u16(void) const
{
        return get_uint16(BCCMD_KALDEBUG_WRITEPM_num_u16);
}

void BCCMD_KALDEBUG_WRITEPM_PDU::set_num_u16(uint16 value)
{
        resize (value * 2 + BCCMD_KALDEBUG_WRITEPM_data);
        set_uint16(BCCMD_KALDEBUG_WRITEPM_num_u16,value);
        set_length();
}

void BCCMD_KALDEBUG_WRITEPM_PDU::get_data(uint16 *copyto, uint16 len) const
{
        get_uint16Array(copyto,BCCMD_KALDEBUG_WRITEPM_data,len);
}

void BCCMD_KALDEBUG_WRITEPM_PDU::set_data(const uint16 *copyfrom, uint16 len)
{
        set_uint16Array(copyfrom,BCCMD_KALDEBUG_WRITEPM_data,len);
        set_length();
}


/******************************************************************************/


BCCMD_KALDEBUG_SETDATABREAKPOINT_PDU::BCCMD_KALDEBUG_SETDATABREAKPOINT_PDU()
        : BCCMD_KALDEBUG_PDU( BCCMDVARID_KALDEBUG )
{
    set_function(BCCMDPDU_KALDEBUG_SETDATABREAKPOINT);
}

BCCMD_KALDEBUG_SETDATABREAKPOINT_PDU::BCCMD_KALDEBUG_SETDATABREAKPOINT_PDU(const PDU& from)
        : BCCMD_KALDEBUG_PDU( from )
{
}

BCCMD_KALDEBUG_SETDATABREAKPOINT_PDU::BCCMD_KALDEBUG_SETDATABREAKPOINT_PDU(const uint8 *buffer, uint32 len)
        : BCCMD_KALDEBUG_PDU(buffer, len)
{
}

uint16 BCCMD_KALDEBUG_SETDATABREAKPOINT_PDU::get_addr(void) const
{
        return get_uint16(BCCMD_KALDEBUG_SETDATABREAKPOINT_addr);
}

void BCCMD_KALDEBUG_SETDATABREAKPOINT_PDU::set_addr(uint16 value)
{
        set_uint16(BCCMD_KALDEBUG_SETDATABREAKPOINT_addr,value);
}

uint16 BCCMD_KALDEBUG_SETDATABREAKPOINT_PDU::get_read(void) const
{
        return get_uint16(BCCMD_KALDEBUG_SETDATABREAKPOINT_read);
}

void BCCMD_KALDEBUG_SETDATABREAKPOINT_PDU::set_read(uint16 value)
{
        set_uint16(BCCMD_KALDEBUG_SETDATABREAKPOINT_read,value);
}

uint16 BCCMD_KALDEBUG_SETDATABREAKPOINT_PDU::get_write(void) const
{
        return get_uint16(BCCMD_KALDEBUG_SETDATABREAKPOINT_write);
}

void BCCMD_KALDEBUG_SETDATABREAKPOINT_PDU::set_write(uint16 value)
{
        set_uint16(BCCMD_KALDEBUG_SETDATABREAKPOINT_write,value);
}


/******************************************************************************/

BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT_PDU::BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT_PDU()
        : BCCMD_KALDEBUG_PDU( BCCMDVARID_KALDEBUG )
{
    set_function(BCCMDPDU_KALDEBUG_SETNUMBEREDPMBREAKPOINT);
}

BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT_PDU::BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT_PDU(const PDU& from)
        : BCCMD_KALDEBUG_PDU( from )
{
}

BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT_PDU::BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT_PDU(const uint8 *buffer, uint32 len)
        : BCCMD_KALDEBUG_PDU(buffer, len)
{
}

uint16 BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT_PDU::get_addr(void) const
{
        return get_uint16(BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT_addr);
}

void BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT_PDU::set_addr(uint16 value)
{
        set_uint16(BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT_addr, value);
}

uint16 BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT_PDU::get_brk_point_number(void) const
{
        return get_uint16(BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT_brk_point_number);
}

void BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT_PDU::set_brk_point_number(uint16 value)
{
        set_uint16(BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT_brk_point_number, value);
}

//eof xtrabccmdkaldebugpdu.cpp
