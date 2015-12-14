////////////////////////////////////////////////////////////////////////////////
//
//  File:       xtrabccmdkalextaddrdebugpdu.cpp
//
//  Class:      ...
//
//  Purpose:    PDUs for debugging the Kalimba over BCCMD.
//
////////////////////////////////////////////////////////////////////////////////

#include "xtrabccmdkalextaddrdebugpdu.h"
#include "bcflayout.h"
#include "hcipacker/gen_bccmd.h"

enum {
    OFFSET_START(BCCMD_KALEXTADDRDEBUG_SIMPLE)
    OFFSET_uint16(BCCMD_KALEXTADDRDEBUG_SIMPLE_req_type),
    OFFSET_uint16(BCCMD_KALEXTADDRDEBUG_SIMPLE_length),
    OFFSET_uint16(BCCMD_KALEXTADDRDEBUG_SIMPLE_seq_no),
    OFFSET_uint16(BCCMD_KALEXTADDRDEBUG_SIMPLE_varid),
    OFFSET_uint16(BCCMD_KALEXTADDRDEBUG_SIMPLE_status),
    OFFSET_uint16(BCCMD_KALEXTADDRDEBUG_SIMPLE_function),
    OFFSET_uint16(BCCMD_KALEXTADDRDEBUG_SIMPLE_u16),
	OFFSET_uint16(BCCMD_KALEXTADDRDEBUG_SIMPLE_dummy0),
    OFFSET_uint16(BCCMD_KALEXTADDRDEBUG_SIMPLE_dummy1),
	OFFSET_uint16(BCCMD_KALEXTADDRDEBUG_SIMPLE_dummy2),
BCCMD_KAEXTADDRLDEBUG_SIMPLE_pdusize
};

enum {
    OFFSET_START(BCCMD_KALEXTADDRDEBUG_READDM)
    OFFSET_uint16(BCCMD_KALEXTADDRDEBUG_READDM_req_type),
    OFFSET_uint16(BCCMD_KALEXTADDRDEBUG_READDM_length),
    OFFSET_uint16(BCCMD_KALEXTADDRDEBUG_READDM_seq_no),
    OFFSET_uint16(BCCMD_KALEXTADDRDEBUG_READDM_varid),
    OFFSET_uint16(BCCMD_KALEXTADDRDEBUG_READDM_status),
    OFFSET_uint16(BCCMD_KALEXTADDRDEBUG_READDM_function),
    OFFSET_uint32(BCCMD_KALEXTADDRDEBUG_READDM_start),
    OFFSET_uint16(BCCMD_KALEXTADDRDEBUG_READDM_num_u16),
    OFFSET_uint16(BCCMD_KALEXTADDRDEBUG_READDM_data),
    BCCMD_KALEXTADDRDEBUG_READDM_pdusize
};

enum {
    OFFSET_START(BCCMD_KALEXTADDRDEBUG_WRITEDM)
    OFFSET_uint16(BCCMD_KALEXTADDRDEBUG_WRITEDM_req_type),
    OFFSET_uint16(BCCMD_KALEXTADDRDEBUG_WRITEDM_length),
    OFFSET_uint16(BCCMD_KALEXTADDRDEBUG_WRITEDM_seq_no),
    OFFSET_uint16(BCCMD_KALEXTADDRDEBUG_WRITEDM_varid),
    OFFSET_uint16(BCCMD_KALEXTADDRDEBUG_WRITEDM_status),
    OFFSET_uint16(BCCMD_KALEXTADDRDEBUG_WRITEDM_function),
    OFFSET_uint32(BCCMD_KALEXTADDRDEBUG_WRITEDM_start),
    OFFSET_uint16(BCCMD_KALEXTADDRDEBUG_WRITEDM_num_u16),
    OFFSET_uint16(BCCMD_KALEXTADDRDEBUG_WRITEDM_data),
    BCCMD_KALEXTADDRDEBUG_WRITEDM_pdusize
};

/******************************************************************************/

BCCMD_KALEXTADDRDEBUG_SIMPLE_PDU::BCCMD_KALEXTADDRDEBUG_SIMPLE_PDU()
        : BCCMD_KALEXTADDRDEBUG_PDU( BCCMDVARID_KALEXTADDRDEBUG )
{
    resize(20);
    set_length();
}

uint16 BCCMD_KALEXTADDRDEBUG_SIMPLE_PDU::get_u16(void) const
{
    return get_uint16(BCCMD_KALEXTADDRDEBUG_SIMPLE_u16);
}

void BCCMD_KALEXTADDRDEBUG_SIMPLE_PDU::set_u16(uint16 value)
{
    set_uint16(BCCMD_KALEXTADDRDEBUG_SIMPLE_u16,value);
}

/******************************************************************************/

BCCMD_KALEXTADDRDEBUG_READDM_PDU::BCCMD_KALEXTADDRDEBUG_READDM_PDU()
        : BCCMD_KALEXTADDRDEBUG_PDU( BCCMDVARID_KALEXTADDRDEBUG )
{
    set_function(BCCMDPDU_KALEXTADDRDEBUG_READDM);
}

void BCCMD_KALEXTADDRDEBUG_READDM_PDU::set_start(uint32 value)
{
    set_uint32(BCCMD_KALEXTADDRDEBUG_READDM_start, value);
}

uint16 BCCMD_KALEXTADDRDEBUG_READDM_PDU::get_num_u16(void) const
{
    return get_uint16(BCCMD_KALEXTADDRDEBUG_READDM_num_u16);
}

void BCCMD_KALEXTADDRDEBUG_READDM_PDU::set_num_u16(uint16 value)
{
    resize (value * 2 + BCCMD_KALEXTADDRDEBUG_READDM_data);
    set_uint16(BCCMD_KALEXTADDRDEBUG_READDM_num_u16, value);
    set_length();
}

void BCCMD_KALEXTADDRDEBUG_READDM_PDU::get_data(uint16 *copyto, uint16 len) const
{
    get_uint16Array(copyto, BCCMD_KALEXTADDRDEBUG_READDM_data, len);
}

/******************************************************************************/


BCCMD_KALEXTADDRDEBUG_WRITEDM_PDU::BCCMD_KALEXTADDRDEBUG_WRITEDM_PDU()
        : BCCMD_KALEXTADDRDEBUG_PDU( BCCMDVARID_KALEXTADDRDEBUG )
{
    set_function(BCCMDPDU_KALEXTADDRDEBUG_WRITEDM);
}

void BCCMD_KALEXTADDRDEBUG_WRITEDM_PDU::set_start(uint32 value)
{
    set_uint32(BCCMD_KALEXTADDRDEBUG_WRITEDM_start,value);
}

uint16 BCCMD_KALEXTADDRDEBUG_WRITEDM_PDU::get_num_u16(void) const
{
    return get_uint16(BCCMD_KALEXTADDRDEBUG_WRITEDM_num_u16);
}

void BCCMD_KALEXTADDRDEBUG_WRITEDM_PDU::set_num_u16(uint16 value)
{
    resize(value * 2 + BCCMD_KALEXTADDRDEBUG_WRITEDM_data);
    set_uint16(BCCMD_KALEXTADDRDEBUG_WRITEDM_num_u16, value);
    set_length();
}

void BCCMD_KALEXTADDRDEBUG_WRITEDM_PDU::set_data(const uint16 *copyfrom, uint16 len)
{
    set_uint16Array(copyfrom,BCCMD_KALEXTADDRDEBUG_WRITEDM_data, len);
    set_length();
}

//eof xtrabccmdkaldebugpdu.cpp
