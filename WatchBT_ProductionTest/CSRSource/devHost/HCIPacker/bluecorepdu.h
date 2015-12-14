///////////////////////////////////////////////////////////////////////////////
//
//  FILE:       bluecorepdu.h
//
//  CLASSES:    PCPDU, BCCMDPDU_PDU, HQPDU_PDU, VMPDU
//
//  PURPOSE:    Declare BlueCore specific pdus.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __BLUECOREPDU_H__
#define __BLUECOREPDU_H__

#include "hcipacker/pdu.h"
#include "bluetoothclasses/bluetoothclasses.h"

#ifdef WIN32
#ifdef __BCCMDPDU_H__
#error bccmdpdu.h already included. structs/unions may not be word aligned
#endif
#pragma pack(push, 2)
#endif

#include "host/bccmd/bccmdpdu.h"
#include "host/hq/hqpdu.h"

#ifdef WIN32
#pragma pack(pop)
#endif

class PCPDU : public PDU
{
public:
	static const uint16 byte_word_size_ratio;
protected:
    PCPDU ( PDU::bc_channel channel , uint32 size );
    explicit PCPDU ( const PDU & );
    PCPDU ( PDU::bc_channel channel , const uint8 * , uint32 );

    bool get_bool ( uint32 offset ) const
    { return PDU::get_uint16 ( offset ) != 0 ; }
    void set_bool ( uint32 offset , bool data )
    { PDU::set_uint16 ( offset , data ); }

    uint8 get_uint8 ( uint32 offset ) const
    { return (uint8) PDU::get_uint16 ( offset ); }
    void set_uint8 ( uint32 offset , uint8 data )
    { PDU::set_uint16 ( offset , data ); }

    int8 get_int8 ( uint32 offset ) const
    { return int8 ( PDU::get_uint16 ( offset ) ); }
    void set_int8 ( uint32 offset , int8 data )
    { PDU::set_uint16 ( offset , uint16(data) ); }

    int16 get_int16 ( uint32 offset ) const
    { return int16 ( PDU::get_uint16 ( offset ) ); }
    void set_int16 ( uint32 offset , uint16 data )
    { PDU::set_uint16 ( offset , uint16(data) ); }

    int32 get_int32 (uint32 offset ) const
    { return int32 ( PDU::get_uint32 ( offset ) ); }
    void set_int32 (uint32 offset , uint32 data )
    { PDU::set_uint32 ( offset , uint32(data) ); }

    //  return the actual bnumber of words/bytes copied
    uint32 get_uint8Array ( uint8* copy_to, uint32 offset , uint32 size ) const;
    uint32 get_uint16Array ( uint16* copy_to, uint32 offset , uint32 size ) const;
    uint32 get_uint32Array ( uint32* copy_to, uint32 offset , uint32 size ) const;
    void set_uint8Array ( const uint8* copy_from, uint32 offset , uint32 size );
    void set_uint16Array ( const uint16* copy_from, uint32 offset , uint32 size );
    void set_uint32Array ( const uint32* copy_from, uint32 offset , uint32 size );
};

class BCCMD_PDU :public PCPDU
{
public:
    explicit BCCMD_PDU ( const PDU & );
    BCCMD_PDU ( const uint8* , uint32 );
    explicit BCCMD_PDU ( uint16 varid , uint16 len = minimum_bytes );
    uint16 get_req_type  () const;
    uint16 get_length() const;
    uint16 get_seq_no() const;
    uint16 get_varid () const;
    uint16 get_status() const;
    // return how much was written 
    uint16 get_payload(uint16* toFill, uint16 words) const;
    uint16 get_payload_length() const;
    void set_req_type  (uint16);
    void set_length();
    void set_seq_no(uint16);
    void set_varid (uint16);
    void set_status(uint16);
    void set_payload(const uint16* toCopy, uint16 words); // resize if needed
    void resize( uint32 size ); // check minumum size.

    static const size_t header_words;
    static const size_t minimum_bytes;
    static const size_t minimum_words;
    static const size_t maximum_words;
    static const size_t implicit_words;
    static const size_t type_word_index;
    static const size_t length_word_index;
    static const size_t sequence_word_index;
    static const size_t varid_word_index;
    static const size_t status_word_index;
    static const size_t payload_word_index;
};

class HQ_PDU : public PCPDU
{
public:
    explicit HQ_PDU ( const PDU & );
    HQ_PDU ( const uint8* , uint32 );
    HQ_PDU ( uint16 varid , uint16 size ); // size in bytes

    uint16 get_req_type  () const;
    uint16 get_length() const;
    uint16 get_seq_no() const;
    uint16 get_varid () const;
    uint16 get_status() const;
    // return how much was written 
    uint16 get_payload(uint16* toFill, uint16 words) const;
    uint16 get_payload_length() const;
    void set_req_type  (uint16);
    void set_length();
    void set_seq_no(uint16);
    void set_varid (uint16);
    void set_status(uint16);
    void set_payload(const uint16* toCopy, uint16 words); // resize if needed
    void resize( uint32 size ); // check minumum size.

    static const size_t header_words;
    static const size_t minimum_bytes;
    static const size_t minimum_words;
    static const size_t maximum_words;
    static const size_t implicit_words;
    static const size_t type_word_index;
    static const size_t length_word_index;
    static const size_t sequence_word_index;
    static const size_t varid_word_index;
    static const size_t status_word_index;
    static const size_t payload_word_index;
};

class VM_PDU : public PCPDU
{
public:
    VM_PDU ( const uint16 * array = 0 ); // to chip
    VM_PDU ( const uint8* buffer , uint32 length ); // from chip
    explicit VM_PDU ( const PDU & ); // recast

    bool is_stream() const;
    uint16 operator[] ( uint32 offset ) const;
    void get_packet ( uint16 * toFill ) const; // size is (*this)[0].

    uint8 get_stream_id() const;
    size_t get_stream_size() const;
    size_t get_message_size() const;
    size_t get_stream_data( uint8* copyTo, size_t length ) const;
    size_t get_stream_data16( uint16* copyTo, size_t length ) const;
    size_t get_message_data( uint16* copyTo, size_t length ) const;
    void set_stream_id( uint8 id );
    void set_stream_data( const uint8 * copyFrom, size_t length );
    void set_stream_data16( const uint16 * copyFrom, size_t length );
    void set_message_data( const uint16 * copyFrom, size_t length );
    //  emergent protocols
    uint16 get_words() const;
    uint16 get_type() const;
	uint16 get_subtype() const;
    void set_words();
    void set_type( uint16 type );
	void set_subtype( uint16 subtype );
    const BluetoothDeviceAddress get_BluetoothDeviceAddress( uint32 offset ) const;
    void set_BluetoothDeviceAddress( uint32 offset , const BluetoothDeviceAddress& );
protected:
    VM_PDU ( uint16 type , uint32 bytesize );
};

#include "hcipacker/gen_bccmd.h"
#include "hcipacker/gen_hq.h"

#endif /*__BLUECOREPDU_H__*/
