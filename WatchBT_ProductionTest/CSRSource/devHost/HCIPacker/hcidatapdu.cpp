///////////////////////////////////////////////////////////////////////////////
//
//  FILE:       hcidatapdu.h
//
//  CLASSES:    HCIDataPDU, HCIACLPDU, HCISCOPDU
//
//  PURPOSE:    bluetooth data packets.
//
///////////////////////////////////////////////////////////////////////////////

#include "hcidatapdu.h"

const size_t HCIACLPDU::header_size = 4;
const size_t HCISCOPDU::header_size = 3;

uint16 HCIDataPDU::get_handle() const
{
    return ( get_uint16 ( 0 ) & 0x0FFF );
}

void HCIDataPDU::set_handle ( uint16 handle )
{
    uint16 top = ( uint16(get_uint8 ( 1 ) & 0xF0) ) << 8;
    set_uint16 ( 0 , (handle & 0x0FFF) | top );
}

HCIACLPDU::HCIACLPDU ( const uint8 *payload, uint32 length, uint16 handle, 
		       HCIACLPDU::packet_boundary_flag pbf, 
		       HCIACLPDU::broadcast_type bct ) :
    HCIDataPDU ( PDU::hciACL , size_t(length + header_size) )
{
   set_header(handle, pbf, bct);
   set_data(payload, length);
   set_length();
}
 
void HCIACLPDU::set_header ( uint16 handle ,
                             HCIACLPDU::packet_boundary_flag pbf ,
                             HCIACLPDU::broadcast_type bct )
{
    set_uint16 ( 0 , ( handle & 0x0FFF ) | ( pbf & 0x3000 ) | ( bct & 0xC000 ) ); 
}

void HCIACLPDU::set_pbf ( packet_boundary_flag pbf )
{
    uint8 top = get_uint8 ( 1 ) & 0xCF;
    set_uint8 ( 1 , top | ( ( pbf & 0x3000 ) >> 8 ) );
}

void HCIACLPDU::set_bct ( broadcast_type bct )
{
    uint8 top = get_uint8 ( 1 ) & 0x3F;
    set_uint8 ( 1 , top | ( ( bct & 0xC000 ) >> 8 ) );
}

HCIACLPDU::packet_boundary_flag HCIACLPDU::get_pbf () const
{
    return HCIACLPDU::packet_boundary_flag( get_uint16 ( 0 ) & 0x3000 );
}

HCIACLPDU::broadcast_type HCIACLPDU::get_bct () const
{
    return HCIACLPDU::broadcast_type( get_uint16 ( 0 ) & 0xC000 );
}

void HCIACLPDU::set_length ()
{
    set_uint16 ( 2 , size() - 4 );
}

uint16 HCIACLPDU::get_length() const
{
    return get_uint16 ( 2 );
}

void HCIACLPDU::set_data ( const uint8* copyfrom , uint16 length )
{
    set_uint8Array ( copyfrom , 4 , length );
}

void HCIACLPDU::get_data ( uint8* copyto , uint16 length ) const
{
    get_uint8Array ( copyto , 4 , length );
}

const uint8 * HCIACLPDU::get_dataPtr () const
{
    return get_uint8ArrayPtr ( 4 );
}

void HCISCOPDU::set_length()
{
    set_uint8 ( 2 , size() - 3 );
}

uint8 HCISCOPDU::get_length() const
{
    return get_uint8 ( 2 );
}

void HCISCOPDU::set_data ( const uint8* copyfrom , uint16 length )
{
    set_uint8Array ( copyfrom , 3 , length );
}

void HCISCOPDU::get_data ( uint8* copyto , uint16 length ) const
{
    get_uint8Array ( copyto , 3 , length );
}

const uint8 * HCISCOPDU::get_dataPtr () const
{
    return get_uint8ArrayPtr ( 3 );
}

