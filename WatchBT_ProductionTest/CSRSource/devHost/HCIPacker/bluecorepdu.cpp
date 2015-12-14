//////////////////////////////////////////////////////////////////////////////
//
//  FILE:  bluecorepdu.cpp
//
//  Copyright CSR 2003-4.
//
//  PURPOSE:  to define the CSR proprietary pdu types.
//
///////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "bluecorepdu.h"

const uint16 PCPDU::byte_word_size_ratio = 2;

PCPDU::PCPDU ( PDU::bc_channel channel , uint32 size_in_bytes )
: PDU ( channel , size_in_bytes )
{
}

PCPDU::PCPDU ( const PDU& from ) : PDU ( from )
{
}

PCPDU::PCPDU ( PDU::bc_channel channel , const uint8 * buffer , uint32 len )
:PDU ( channel , buffer , len )
{
}

uint32 PCPDU::get_uint8Array ( uint8* copy_to, uint32 offset , uint32 size ) const
{
    if (!copy_to)
        return 0;
    //  work in char widths.  Offset is in uint8 widths.
    if ( ( offset + ( size * byte_word_size_ratio ) ) > this->size() )
        size = ( this->size() - offset ) / byte_word_size_ratio;
    //  i is an offset within the pdu. j is an index in the array.
    uint32 j = 0;
    for ( uint32 i = 0 ; j < size ; i+=byte_word_size_ratio,++j )
        copy_to[j] = get_uint8 ( offset + i );
    return size;
}

uint32 PCPDU::get_uint16Array ( uint16* copy_to, uint32 offset , uint32 size ) const
{
    if (!copy_to)
        return 0;
    //  work in char widths.  Offset is in uint8 widths.
    //  Size of uin16 in BCF is 2 * sizeof(uint8).
    if ( ( offset + (byte_word_size_ratio * size) ) > this->size() )
        size = ( this->size() - offset ) / byte_word_size_ratio;
    //  i is an offset within the pdu. j is an index in the array.
    uint32 j = 0;
    for ( uint32 i = 0 ; j < size ; i+=byte_word_size_ratio,++j )
        copy_to[j] = get_uint16 ( offset + i );
    return size;
}


uint32 PCPDU::get_uint32Array ( uint32* copy_to, uint32 offset , uint32 size ) const
{
    static const int uint32size = 4;
	if (!copy_to)
        return 0;
    //  work in char widths.  Offset is in uint8 widths.
    //  Size of uint32 in BCF is 4 * sizeof(uint8).
    if ( ( offset + (uint32size * size) ) > this->size() )
        size = ( this->size() - offset ) / uint32size;
    //  i is an offset within the pdu. j is an index in the array.
    uint32 j = 0;
    for ( uint32 i = 0 ; j < size ; i+=uint32size,++j )
        copy_to[j] = get_uint32 ( offset + i );
    return size;
}


void PCPDU::set_uint8Array ( const uint8* copy_from, uint32 offset , uint32 size )
{
    //  size of uint8 in pdu is 2.  offset is in chars.
    if ( offset + ( size * byte_word_size_ratio ) > this->size() )
        resize ( offset + ( size * byte_word_size_ratio ) );
    for ( uint32 i = 0 ; i < size ; ++i )
        set_uint8 ( offset + (i * byte_word_size_ratio) , copy_from[i] );
}

void PCPDU::set_uint16Array ( const uint16* copy_from, uint32 offset , uint32 size )
{
    if ( offset + (size * byte_word_size_ratio) > this->size() )
        resize ( offset + ( size * byte_word_size_ratio ) );
    for ( uint32 i = 0 ; i < size ; ++i )
        set_uint16 ( offset + (i * byte_word_size_ratio) , copy_from[i] );
}

void PCPDU::set_uint32Array ( const uint32* copy_from, uint32 offset , uint32 size )

{
    static const int uint32size = 4;
    if ( offset + (size * uint32size) > this->size() )
        resize ( offset + ( size * uint32size ) );
    for ( uint32 i = 0 ; i < size ; ++i )
        set_uint32 ( offset + (i * uint32size) , copy_from[i] );
}

///////////////////////////////////////////////////////////////////////////////
//  BCCMDPDU_PDU
///////////////////////////////////////////////////////////////////////////////

const size_t BCCMD_PDU::header_words = 5;
const size_t BCCMD_PDU::minimum_bytes = 18;
const size_t BCCMD_PDU::minimum_words = minimum_bytes/byte_word_size_ratio;
const size_t BCCMD_PDU::maximum_words = BCCMDPDU_MAXLEN;
// length and status words are implicit when sending.
// type and length are implicit when receiving.
const size_t BCCMD_PDU::implicit_words = 2;
const size_t BCCMD_PDU::type_word_index = 0;
const size_t BCCMD_PDU::length_word_index = 1;
const size_t BCCMD_PDU::sequence_word_index = 2;
const size_t BCCMD_PDU::varid_word_index = 3;
const size_t BCCMD_PDU::status_word_index = 4;
const size_t BCCMD_PDU::payload_word_index = 5;

BCCMD_PDU::BCCMD_PDU ( uint16 varid , uint16 len )
: PCPDU ( PDU::bccmd , len > minimum_bytes ? len : minimum_bytes )
{
    set_varid ( varid );
    set_length ();
}

BCCMD_PDU::BCCMD_PDU ( const PDU& from ) : PCPDU ( from )
{
}

BCCMD_PDU::BCCMD_PDU ( const uint8 * buffer , uint32 len )
:PCPDU ( PDU::bccmd , buffer , len )
{
}

uint16 BCCMD_PDU::get_req_type () const
{
    return PDU::get_uint16 ( type_word_index * byte_word_size_ratio );
}

uint16 BCCMD_PDU::get_length () const
{
    return PDU::get_uint16 ( length_word_index * byte_word_size_ratio );
}

uint16 BCCMD_PDU::get_seq_no () const
{
    return PDU::get_uint16 ( sequence_word_index * byte_word_size_ratio );
}

uint16 BCCMD_PDU::get_varid () const
{
    return PDU::get_uint16 ( varid_word_index * byte_word_size_ratio );
}

uint16 BCCMD_PDU::get_status () const
{
    return PDU::get_uint16 ( status_word_index * byte_word_size_ratio );
}

uint16 BCCMD_PDU::get_payload (uint16* result, uint16 length) const
{
    return uint16( PCPDU::get_uint16Array ( result , 
                                   payload_word_index * byte_word_size_ratio,
                                   length ) );
}

uint16 BCCMD_PDU::get_payload_length () const
{
    return (size()/byte_word_size_ratio) - payload_word_index;
}

void BCCMD_PDU::set_req_type ( uint16 type )
{
    PDU::set_uint16 ( type_word_index * byte_word_size_ratio , type );
}

void BCCMD_PDU::set_length ()
{
    PDU::set_uint16 ( length_word_index * byte_word_size_ratio , size()/byte_word_size_ratio );
}

void BCCMD_PDU::set_seq_no ( uint16 seq_no )
{
    PDU::set_uint16 ( sequence_word_index * byte_word_size_ratio , seq_no );
}

void BCCMD_PDU::set_varid ( uint16 varid )
{
    PDU::set_uint16 ( varid_word_index * byte_word_size_ratio , varid );
}

void BCCMD_PDU::set_status ( uint16 status )
{
    PDU::set_uint16 ( status_word_index * byte_word_size_ratio , status );
}

void BCCMD_PDU::set_payload ( const uint16 * data , uint16 size)
{
    PCPDU::set_uint16Array ( data , payload_word_index * byte_word_size_ratio , size );
}

void BCCMD_PDU::resize ( uint32 size )
{
    PDU::resize ( size <= minimum_bytes ? minimum_bytes : size );
}

//void BCCMDPDU_PDU::stamp_header ( BCCMDPDU * s )
//{
//    s->type = get_type();
//}

///////////////////////////////////////////////////////////////////////////////
//  HQPDU_PDU
///////////////////////////////////////////////////////////////////////////////

const size_t HQ_PDU::header_words = 5;
const size_t HQ_PDU::minimum_bytes = 18;
const size_t HQ_PDU::minimum_words = minimum_bytes/byte_word_size_ratio;
const size_t HQ_PDU::maximum_words = BCCMDPDU_MAXLEN;
// length and status words are implicit when sending.
// type and length are implicit when receiving.
const size_t HQ_PDU::implicit_words = 2;
const size_t HQ_PDU::type_word_index = 0;
const size_t HQ_PDU::length_word_index = 1;
const size_t HQ_PDU::sequence_word_index = 2;
const size_t HQ_PDU::varid_word_index = 3;
const size_t HQ_PDU::status_word_index = 4;
const size_t HQ_PDU::payload_word_index = 5;

HQ_PDU::HQ_PDU ( uint16 varid , uint16 size ) // bytes
: PCPDU ( PDU::hq , size )
{
    set_varid ( varid );
    set_length ();
}

HQ_PDU::HQ_PDU ( const PDU& from ) : PCPDU ( from )
{
}

HQ_PDU::HQ_PDU ( const uint8 * buffer , uint32 len )
: PCPDU ( PDU::hq , buffer , len )
{
}

uint16 HQ_PDU::get_req_type () const
{
    return PDU::get_uint16 ( 0 );
}

uint16 HQ_PDU::get_length () const
{
    return PDU::get_uint16 ( 2 );
}

uint16 HQ_PDU::get_seq_no () const
{
    return PDU::get_uint16 ( 4 );
}

uint16 HQ_PDU::get_varid () const
{
    return PDU::get_uint16 ( 6 );
}

uint16 HQ_PDU::get_status () const
{
    return PDU::get_uint16 ( 8 );
}

uint16 HQ_PDU::get_payload (uint16* result, uint16 length) const
{
    return uint16( PCPDU::get_uint16Array ( result , 
                                   payload_word_index * byte_word_size_ratio,
                                   length ) );
}

uint16 HQ_PDU::get_payload_length () const
{
    return (size()/byte_word_size_ratio) - payload_word_index;
}

void HQ_PDU::set_req_type ( uint16 type )
{
    PDU::set_uint16 ( 0 , type );
}

void HQ_PDU::set_length ()
{
    PDU::set_uint16 ( 2 , size()/2 );
}

void HQ_PDU::set_seq_no ( uint16 seq_no )
{
    PDU::set_uint16 ( 4 , seq_no );
}

void HQ_PDU::set_varid ( uint16 varid )
{
    PDU::set_uint16 ( 6 , varid );
}

void HQ_PDU::set_status ( uint16 status )
{
    PDU::set_uint16 ( 8 , status );
}

void HQ_PDU::set_payload ( const uint16 * data , uint16 size)
{
    PCPDU::set_uint16Array ( data , payload_word_index * byte_word_size_ratio , size );
}

void HQ_PDU::resize ( uint32 size )
{
    PDU::resize ( size <= minimum_words ? minimum_words : size );
}

///////////////////////////////////////////////////////////////////////////////
//  VM_PDU
///////////////////////////////////////////////////////////////////////////////

VM_PDU::VM_PDU ( uint16 type , uint32 bytesize )
: PCPDU ( PDU::vm , (bytesize + (bytesize & 1)) )// round up
{
    set_words();
    set_subtype ( type );
}

VM_PDU::VM_PDU ( const uint8 * buffer , uint32 length )
: PCPDU ( PDU::vm , buffer , length )
{
}

VM_PDU::VM_PDU ( const PDU& from )
: PCPDU ( from )
{
}

VM_PDU::VM_PDU ( const uint16 * data )
: PCPDU ( PDU::vm , 1 )
{
    if ( data && data[0] )
        set_uint16Array ( data , 0 , data[0] );
    else
        set_uint16 ( 0 , 1 );
}

bool VM_PDU::is_stream() const
{
    return get_uint8(3) & 0x3;
}

uint16 VM_PDU::operator [] ( uint32 offset ) const
{
    return get_uint16 ( offset * 2 );
}

void VM_PDU::get_packet ( uint16 * toFill ) const
{
    get_uint16Array ( toFill , 0 , (*this)[0] );
}

void VM_PDU::set_stream_id( uint8 id )
{
    if ( size() < 4 )
        resize(4);
    set_uint8(2,id);
}

void VM_PDU::set_stream_data ( const uint8 * copyFrom, size_t length )
{
    if ( length + 4 > size() )
        resize(((length+1)/2)*2 + 4); // round up to the nearest even number;
    PDU::set_uint8Array(copyFrom,4,length);
    PDU::set_uint8(3,(length&1)?1:2);
}

void VM_PDU::set_stream_data16 ( const uint16 * copyFrom, size_t length )
{
	// the data is 16 bits so double the length
    if ( length*2 + 4 > size() )
        resize(length*2 + 4);
    PDU::set_uint16Array(copyFrom,4,length);
    PDU::set_uint8(3,2);
}

void VM_PDU::set_message_data ( const uint16 * copyFrom, size_t length )
{
	// the data is 16 bits so double the length
    if ( length*2 + 4 > size() )
        resize(length*2 + 4);
    PDU::set_uint16Array(copyFrom,4,length);
    PDU::set_uint8(3,0);
}

uint8 VM_PDU::get_stream_id() const
{
    return get_uint8(2);
}

size_t VM_PDU::get_stream_size() const
{
    // payload runs from byte 4 to the end if byte 3 == 2, or byte 4 to one from the end if byte 3 == 1.
    return size() - 6 + get_uint8(3);
}

size_t VM_PDU::get_message_size() const
{
    // knock off the first 4 bytes for count and ID, convert to bytes
    return (size() - 4)>>1;
}

size_t VM_PDU::get_stream_data ( uint8 * copyTo, size_t length ) const
{
    if ( length > get_stream_size() )
        length = get_stream_size();
    PDU::get_uint8Array(copyTo,4,length);
    return length;
}

size_t VM_PDU::get_stream_data16 ( uint16 * copyTo, size_t length ) const
{
    if ( length > (get_stream_size()>>1) )
        length = (get_stream_size()>>1);
    PDU::get_uint16Array(copyTo,4,length);
    return length;
}

size_t VM_PDU::get_message_data ( uint16 * copyTo, size_t length ) const
{
    if ( length > get_message_size() )
        length = get_message_size();
    length = PDU::get_uint16Array(copyTo,4,length);
    return length;
}

uint16 VM_PDU::get_words() const
{
    return get_uint16 ( 0 );
}

uint16 VM_PDU::get_type() const
{
    return get_uint16 ( 2 );
}

uint16 VM_PDU::get_subtype() const
{
	return get_uint16 ( 4 );
}

void VM_PDU::set_words()
{
    set_uint16 ( 0 , size() / 2 ); // round down
}

void VM_PDU::set_type( uint16 type )
{
	set_uint16 ( 2 , type );
}

void VM_PDU::set_subtype( uint16 subtype )
{
	set_type( 0 );
	set_uint16( 4, subtype );
}

const BluetoothDeviceAddress VM_PDU::get_BluetoothDeviceAddress( uint32 offset ) const
{
    uint24 lap = get_uint24 ( offset );
    uint8 uap = get_uint8 ( offset + 4 );
    uint16 nap = get_uint16 ( offset + 6 );
    return BluetoothDeviceAddress ( nap , uap , lap );
}

void VM_PDU::set_BluetoothDeviceAddress( uint32 offset , const BluetoothDeviceAddress& value )
{
    set_uint24 ( offset ,  value.get_lap() );
    set_uint8 ( offset + 4 , value.get_uap() );
    set_uint16 ( offset + 6 , value.get_nap() );
}

