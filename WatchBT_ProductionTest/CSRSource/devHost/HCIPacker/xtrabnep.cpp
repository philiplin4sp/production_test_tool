////////////////////////////////////////////////////////////////////////////////
//
//  WARNING THIS IS AN HAND CRAFTED FILE DEFINING A BLUECORE-FRIENDY PROTOCOL
//
////////////////////////////////////////////////////////////////////////////////

#include "bcflayout.h"

#define OFFSET_eighteen_uint16s(name) \
OFFSET_uint16(name),OFFSET_uint16(name##_1),\
OFFSET_uint16(name##_2),OFFSET_uint16(name##_3),\
OFFSET_uint16(name##_4),OFFSET_uint16(name##_5),\
OFFSET_uint16(name##_6),OFFSET_uint16(name##_7),\
OFFSET_uint16(name##_8),OFFSET_uint16(name##_9),\
OFFSET_uint16(name##_10),OFFSET_uint16(name##_11),\
OFFSET_uint16(name##_12),OFFSET_uint16(name##_13),\
OFFSET_uint16(name##_14),OFFSET_uint16(name##_15),\
OFFSET_uint16(name##_16),OFFSET_uint16(name##_17)


enum {
	OFFSET_START(BNEP_DATA_REQ_T)
	OFFSET_uint16(BNEP_DATA_REQ_T_type),
	OFFSET_uint16(BNEP_DATA_REQ_T_ether_type),
	OFFSET_EtherNetAddress(BNEP_DATA_REQ_T_rem_addr),
	OFFSET_uint16(BNEP_DATA_REQ_T_mblk),
	OFFSET_uint16(BNEP_DATA_REQ_T_len),
	BNEP_DATA_REQ_T_pduSize
} ;

enum {
	OFFSET_START(BNEP_DEBUG_IND_T)
	OFFSET_uint16(BNEP_DEBUG_IND_T_type),
	OFFSET_uint16(BNEP_DEBUG_IND_T_id),
	OFFSET_uint16(BNEP_DEBUG_IND_T_test),
	OFFSET_eighteen_uint16s(BNEP_DEBUG_IND_T_args),
	BNEP_DEBUG_IND_T_pduSize
} ;

enum {
	OFFSET_START(BNEP_DEBUG_REQ_T)
	OFFSET_uint16(BNEP_DEBUG_REQ_T_type),
	OFFSET_uint16(BNEP_DEBUG_REQ_T_id),
	OFFSET_uint16(BNEP_DEBUG_REQ_T_test),
	OFFSET_eighteen_uint16s(BNEP_DEBUG_REQ_T_args),
	BNEP_DEBUG_REQ_T_pduSize
} ;

enum {
	OFFSET_START(BNEP_EXTENDED_MULTICAST_DATA_REQ_T)
	OFFSET_uint16(BNEP_EXTENDED_MULTICAST_DATA_REQ_T_type),
	OFFSET_uint16(BNEP_EXTENDED_MULTICAST_DATA_REQ_T_id_not),
	OFFSET_uint16(BNEP_EXTENDED_MULTICAST_DATA_REQ_T_ether_type),
	OFFSET_EtherNetAddress(BNEP_EXTENDED_MULTICAST_DATA_REQ_T_dst_addr),
	OFFSET_EtherNetAddress(BNEP_EXTENDED_MULTICAST_DATA_REQ_T_src_addr),
	OFFSET_uint16(BNEP_EXTENDED_MULTICAST_DATA_REQ_T_mblk),
	OFFSET_uint16(BNEP_EXTENDED_MULTICAST_DATA_REQ_T_len),
	BNEP_EXTENDED_MULTICAST_DATA_REQ_T_pduSize
} ;

enum {
	OFFSET_START(BNEP_LOOPBACK_DATA_IND_T)
	OFFSET_uint16(BNEP_LOOPBACK_DATA_IND_T_type),
	OFFSET_uint16(BNEP_LOOPBACK_DATA_IND_T_phandle),
	OFFSET_uint16(BNEP_LOOPBACK_DATA_IND_T_mblk),
	OFFSET_uint16(BNEP_LOOPBACK_DATA_IND_T_len),
	BNEP_LOOPBACK_DATA_IND_T_pduSize
} ;

enum {
	OFFSET_START(BNEP_EXTENDED_DATA_IND_T)
	OFFSET_uint16(BNEP_EXTENDED_DATA_IND_T_type),
	OFFSET_uint16(BNEP_EXTENDED_DATA_IND_T_id),
	OFFSET_uint16(BNEP_EXTENDED_DATA_IND_T_ether_type),
	OFFSET_EtherNetAddress(BNEP_EXTENDED_DATA_IND_T_dst_addr),
	OFFSET_EtherNetAddress(BNEP_EXTENDED_DATA_IND_T_src_addr),
	OFFSET_uint16(BNEP_EXTENDED_DATA_IND_T_mblk),
	OFFSET_uint16(BNEP_EXTENDED_DATA_IND_T_len),
	BNEP_EXTENDED_DATA_IND_T_pduSize
} ;

enum {
	OFFSET_START(BNEP_DATA_IND_T)
	OFFSET_uint16(BNEP_DATA_IND_T_type),
	OFFSET_uint16(BNEP_DATA_IND_T_phandle),
	OFFSET_uint16(BNEP_DATA_IND_T_mblk),
	OFFSET_uint16(BNEP_DATA_IND_T_len),
	BNEP_DATA_IND_T_pduSize
} ;

enum {
	OFFSET_START(BNEP_MULTICAST_DATA_IND_T)
	OFFSET_uint16(BNEP_MULTICAST_DATA_IND_T_type),
	OFFSET_uint16(BNEP_MULTICAST_DATA_IND_T_phandle),
	OFFSET_uint16(BNEP_MULTICAST_DATA_IND_T_mblk),
	OFFSET_uint16(BNEP_MULTICAST_DATA_IND_T_len),
	BNEP_MULTICAST_DATA_IND_T_pduSize
} ;

enum {
	OFFSET_START(BNEP_EXTENDED_DATA_REQ_T)
	OFFSET_uint16(BNEP_EXTENDED_DATA_REQ_T_type),
	OFFSET_uint16(BNEP_EXTENDED_DATA_REQ_T_id),
	OFFSET_uint16(BNEP_EXTENDED_DATA_REQ_T_ether_type),
	OFFSET_EtherNetAddress(BNEP_EXTENDED_DATA_REQ_T_dst_addr),
	OFFSET_EtherNetAddress(BNEP_EXTENDED_DATA_REQ_T_src_addr),
	OFFSET_uint16(BNEP_EXTENDED_DATA_REQ_T_mblk),
	OFFSET_uint16(BNEP_EXTENDED_DATA_REQ_T_len),
	BNEP_EXTENDED_DATA_REQ_T_pduSize
} ;

#include "hcipacker/gen_bnep.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Class: BNEP_DATA_REQ_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

BNEP_DATA_REQ_T_PDU::BNEP_DATA_REQ_T_PDU()
: BNEP_PDU ( BNEP_DATA_REQ , BNEP_DATA_REQ_T_pduSize )
{
}

BNEP_DATA_REQ_T_PDU::BNEP_DATA_REQ_T_PDU( const PDU& from )
: BNEP_PDU ( from )
{
}

BNEP_DATA_REQ_T_PDU::BNEP_DATA_REQ_T_PDU( const uint8 * buffer , uint32 len )
: BNEP_PDU ( buffer , len )
{
}

uint16 BNEP_DATA_REQ_T_PDU::get_ether_type() const
{
    return get_uint16 ( BNEP_DATA_REQ_T_ether_type );
}

void BNEP_DATA_REQ_T_PDU::set_ether_type( uint16 value )
{
    set_uint16 ( BNEP_DATA_REQ_T_ether_type , value );
}

EtherNetAddress BNEP_DATA_REQ_T_PDU::get_rem_addr() const
{
    return get_EtherNetAddress ( BNEP_DATA_REQ_T_rem_addr );
}

void BNEP_DATA_REQ_T_PDU::set_rem_addr( EtherNetAddress value )
{
    set_EtherNetAddress ( BNEP_DATA_REQ_T_rem_addr , value );
}

uint16 BNEP_DATA_REQ_T_PDU::get_len() const
{
    return get_uint16 ( BNEP_DATA_REQ_T_len );
}

void BNEP_DATA_REQ_T_PDU::set_len(uint16 len)
{
    set_uint16( BNEP_DATA_REQ_T_len, len);
}

void BNEP_DATA_REQ_T_PDU::get_data( uint8 * copyTo ) const
{
    get_uint8Array ( copyTo , BNEP_DATA_REQ_T_pduSize , get_len() );
}

const uint8 * BNEP_DATA_REQ_T_PDU::get_dataPtr() const
{
    return get_uint8ArrayPtr ( BNEP_DATA_REQ_T_pduSize );
}

void BNEP_DATA_REQ_T_PDU::set_data( const uint8* copyFrom , uint16 length )
{
    set_final_uint8Ptr ( BNEP_DATA_REQ_T_len , BNEP_DATA_REQ_T_pduSize , copyFrom , length );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: BNEP_DEBUG_IND_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

BNEP_DEBUG_IND_T_PDU::BNEP_DEBUG_IND_T_PDU()
: BNEP_PDU ( BNEP_DEBUG_IND , BNEP_DEBUG_IND_T_pduSize )
{
}

BNEP_DEBUG_IND_T_PDU::BNEP_DEBUG_IND_T_PDU( const PDU& from )
: BNEP_PDU ( from )
{
}

BNEP_DEBUG_IND_T_PDU::BNEP_DEBUG_IND_T_PDU( const uint8 * buffer , uint32 len )
: BNEP_PDU ( buffer , len )
{
}

uint16 BNEP_DEBUG_IND_T_PDU::get_id() const
{
    return get_uint16 ( BNEP_DEBUG_IND_T_id );
}

void BNEP_DEBUG_IND_T_PDU::set_id( uint16 value )
{
    set_uint16 ( BNEP_DEBUG_IND_T_id , value );
}

uint16 BNEP_DEBUG_IND_T_PDU::get_test() const
{
    return get_uint16 ( BNEP_DEBUG_IND_T_test );
}

void BNEP_DEBUG_IND_T_PDU::set_test( uint16 value )
{
    set_uint16 ( BNEP_DEBUG_IND_T_test , value );
}

void BNEP_DEBUG_IND_T_PDU::get_args( uint16 * copyTo ) const
{
    get_uint16Array ( copyTo , BNEP_DEBUG_IND_T_args , 18 );
}

void BNEP_DEBUG_IND_T_PDU::set_args( const uint16 * copyFrom )
{
    set_uint16Array ( copyFrom , BNEP_DEBUG_IND_T_args , 18 );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: BNEP_DEBUG_REQ_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

BNEP_DEBUG_REQ_T_PDU::BNEP_DEBUG_REQ_T_PDU()
: BNEP_PDU ( BNEP_DEBUG_REQ , BNEP_DEBUG_REQ_T_pduSize )
{
}

BNEP_DEBUG_REQ_T_PDU::BNEP_DEBUG_REQ_T_PDU( const PDU& from )
: BNEP_PDU ( from )
{
}

BNEP_DEBUG_REQ_T_PDU::BNEP_DEBUG_REQ_T_PDU( const uint8 * buffer , uint32 len )
: BNEP_PDU ( buffer , len )
{
}

uint16 BNEP_DEBUG_REQ_T_PDU::get_id() const
{
    return get_uint16 ( BNEP_DEBUG_REQ_T_id );
}

void BNEP_DEBUG_REQ_T_PDU::set_id( uint16 value )
{
    set_uint16 ( BNEP_DEBUG_REQ_T_id , value );
}

uint16 BNEP_DEBUG_REQ_T_PDU::get_test() const
{
    return get_uint16 ( BNEP_DEBUG_REQ_T_test );
}

void BNEP_DEBUG_REQ_T_PDU::set_test( uint16 value )
{
    set_uint16 ( BNEP_DEBUG_REQ_T_test , value );
}

void BNEP_DEBUG_REQ_T_PDU::get_args( uint16 * copyTo ) const
{
    get_uint16Array ( copyTo , BNEP_DEBUG_REQ_T_args , 18 );
}

void BNEP_DEBUG_REQ_T_PDU::set_args( const uint16 * copyFrom )
{
    set_uint16Array ( copyFrom , BNEP_DEBUG_REQ_T_args , 18 );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: BNEP_EXTENDED_MULTICAST_DATA_REQ_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

BNEP_EXTENDED_MULTICAST_DATA_REQ_T_PDU::BNEP_EXTENDED_MULTICAST_DATA_REQ_T_PDU()
: BNEP_PDU ( BNEP_EXTENDED_MULTICAST_DATA_REQ , BNEP_EXTENDED_MULTICAST_DATA_REQ_T_pduSize )
{
}

BNEP_EXTENDED_MULTICAST_DATA_REQ_T_PDU::BNEP_EXTENDED_MULTICAST_DATA_REQ_T_PDU( const PDU& from )
: BNEP_PDU ( from )
{
}

BNEP_EXTENDED_MULTICAST_DATA_REQ_T_PDU::BNEP_EXTENDED_MULTICAST_DATA_REQ_T_PDU( const uint8 * buffer , uint32 len )
: BNEP_PDU ( buffer , len )
{
}

uint16 BNEP_EXTENDED_MULTICAST_DATA_REQ_T_PDU::get_id_not() const
{
    return get_uint16 ( BNEP_EXTENDED_MULTICAST_DATA_REQ_T_id_not );
}

void BNEP_EXTENDED_MULTICAST_DATA_REQ_T_PDU::set_id_not( uint16 value )
{
    set_uint16 ( BNEP_EXTENDED_MULTICAST_DATA_REQ_T_id_not , value );
}

uint16 BNEP_EXTENDED_MULTICAST_DATA_REQ_T_PDU::get_ether_type() const
{
    return get_uint16 ( BNEP_EXTENDED_MULTICAST_DATA_REQ_T_ether_type );
}

void BNEP_EXTENDED_MULTICAST_DATA_REQ_T_PDU::set_ether_type( uint16 value )
{
    set_uint16 ( BNEP_EXTENDED_MULTICAST_DATA_REQ_T_ether_type , value );
}

EtherNetAddress BNEP_EXTENDED_MULTICAST_DATA_REQ_T_PDU::get_dst_addr() const
{
    return get_EtherNetAddress ( BNEP_EXTENDED_MULTICAST_DATA_REQ_T_dst_addr );
}

void BNEP_EXTENDED_MULTICAST_DATA_REQ_T_PDU::set_dst_addr( EtherNetAddress value )
{
    set_EtherNetAddress ( BNEP_EXTENDED_MULTICAST_DATA_REQ_T_dst_addr , value );
}

EtherNetAddress BNEP_EXTENDED_MULTICAST_DATA_REQ_T_PDU::get_src_addr() const
{
    return get_EtherNetAddress ( BNEP_EXTENDED_MULTICAST_DATA_REQ_T_src_addr );
}

void BNEP_EXTENDED_MULTICAST_DATA_REQ_T_PDU::set_src_addr( EtherNetAddress value )
{
    set_EtherNetAddress ( BNEP_EXTENDED_MULTICAST_DATA_REQ_T_src_addr , value );
}

uint16 BNEP_EXTENDED_MULTICAST_DATA_REQ_T_PDU::get_len() const
{
    return get_uint16 ( BNEP_EXTENDED_MULTICAST_DATA_REQ_T_len );
}

void BNEP_EXTENDED_MULTICAST_DATA_REQ_T_PDU::set_len(uint16 len)
{
    set_uint16 ( BNEP_EXTENDED_MULTICAST_DATA_REQ_T_len , len );
}

void BNEP_EXTENDED_MULTICAST_DATA_REQ_T_PDU::get_data( uint8 * copyTo ) const
{
    get_uint8Array ( copyTo , BNEP_EXTENDED_MULTICAST_DATA_REQ_T_pduSize , get_len() );
}

const uint8 * BNEP_EXTENDED_MULTICAST_DATA_REQ_T_PDU::get_dataPtr() const
{
    return get_uint8ArrayPtr ( BNEP_EXTENDED_MULTICAST_DATA_REQ_T_pduSize );
}

void BNEP_EXTENDED_MULTICAST_DATA_REQ_T_PDU::set_data( const uint8* copyFrom , uint16 length )
{
    set_final_uint8Ptr ( BNEP_EXTENDED_MULTICAST_DATA_REQ_T_len , BNEP_EXTENDED_MULTICAST_DATA_REQ_T_pduSize , copyFrom , length );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: BNEP_LOOPBACK_DATA_IND_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

BNEP_LOOPBACK_DATA_IND_T_PDU::BNEP_LOOPBACK_DATA_IND_T_PDU()
: BNEP_PDU ( BNEP_LOOPBACK_DATA_IND , BNEP_LOOPBACK_DATA_IND_T_pduSize )
{
}

BNEP_LOOPBACK_DATA_IND_T_PDU::BNEP_LOOPBACK_DATA_IND_T_PDU( const PDU& from )
: BNEP_PDU ( from )
{
}

BNEP_LOOPBACK_DATA_IND_T_PDU::BNEP_LOOPBACK_DATA_IND_T_PDU( const uint8 * buffer , uint32 len )
: BNEP_PDU ( buffer , len )
{
}

uint16 BNEP_LOOPBACK_DATA_IND_T_PDU::get_phandle() const
{
    return get_uint16 ( BNEP_LOOPBACK_DATA_IND_T_phandle );
}

void BNEP_LOOPBACK_DATA_IND_T_PDU::set_phandle( uint16 value )
{
    set_uint16 ( BNEP_LOOPBACK_DATA_IND_T_phandle , value );
}

uint16 BNEP_LOOPBACK_DATA_IND_T_PDU::get_len() const
{
    return get_uint16 ( BNEP_LOOPBACK_DATA_IND_T_len );
}

void BNEP_LOOPBACK_DATA_IND_T_PDU::get_data( uint8 * copyTo ) const
{
    get_uint8Array ( copyTo , BNEP_LOOPBACK_DATA_IND_T_pduSize , get_len() );
}

const uint8 * BNEP_LOOPBACK_DATA_IND_T_PDU::get_dataPtr() const
{
    return get_uint8ArrayPtr ( BNEP_LOOPBACK_DATA_IND_T_pduSize );
}

void BNEP_LOOPBACK_DATA_IND_T_PDU::set_data( const uint8* copyFrom , uint16 length )
{
    set_final_uint8Ptr ( BNEP_LOOPBACK_DATA_IND_T_len , BNEP_LOOPBACK_DATA_IND_T_pduSize , copyFrom , length );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: BNEP_EXTENDED_DATA_IND_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

BNEP_EXTENDED_DATA_IND_T_PDU::BNEP_EXTENDED_DATA_IND_T_PDU()
: BNEP_PDU ( BNEP_EXTENDED_DATA_IND , BNEP_EXTENDED_DATA_IND_T_pduSize )
{
}

BNEP_EXTENDED_DATA_IND_T_PDU::BNEP_EXTENDED_DATA_IND_T_PDU( const PDU& from )
: BNEP_PDU ( from )
{
}

BNEP_EXTENDED_DATA_IND_T_PDU::BNEP_EXTENDED_DATA_IND_T_PDU( const uint8 * buffer , uint32 len )
: BNEP_PDU ( buffer , len )
{
}

uint16 BNEP_EXTENDED_DATA_IND_T_PDU::get_id() const
{
    return get_uint16 ( BNEP_EXTENDED_DATA_IND_T_id );
}

void BNEP_EXTENDED_DATA_IND_T_PDU::set_id( uint16 value )
{
    set_uint16 ( BNEP_EXTENDED_DATA_IND_T_id , value );
}

uint16 BNEP_EXTENDED_DATA_IND_T_PDU::get_ether_type() const
{
    return get_uint16 ( BNEP_EXTENDED_DATA_IND_T_ether_type );
}

void BNEP_EXTENDED_DATA_IND_T_PDU::set_ether_type( uint16 value )
{
    set_uint16 ( BNEP_EXTENDED_DATA_IND_T_ether_type , value );
}

EtherNetAddress BNEP_EXTENDED_DATA_IND_T_PDU::get_dst_addr() const
{
    return get_EtherNetAddress ( BNEP_EXTENDED_DATA_IND_T_dst_addr );
}

void BNEP_EXTENDED_DATA_IND_T_PDU::set_dst_addr( EtherNetAddress value )
{
    set_EtherNetAddress ( BNEP_EXTENDED_DATA_IND_T_dst_addr , value );
}

EtherNetAddress BNEP_EXTENDED_DATA_IND_T_PDU::get_src_addr() const
{
    return get_EtherNetAddress ( BNEP_EXTENDED_DATA_IND_T_src_addr );
}

void BNEP_EXTENDED_DATA_IND_T_PDU::set_src_addr( EtherNetAddress value )
{
    set_EtherNetAddress ( BNEP_EXTENDED_DATA_IND_T_src_addr , value );
}

uint16 BNEP_EXTENDED_DATA_IND_T_PDU::get_len() const
{
    return get_uint16 ( BNEP_EXTENDED_DATA_IND_T_len );
}

void BNEP_EXTENDED_DATA_IND_T_PDU::get_data( uint8 * copyTo ) const
{
    get_uint8Array ( copyTo , BNEP_EXTENDED_DATA_IND_T_pduSize , get_len() );
}

const uint8 * BNEP_EXTENDED_DATA_IND_T_PDU::get_dataPtr() const
{
    return get_uint8ArrayPtr ( BNEP_EXTENDED_DATA_IND_T_pduSize );
}

void BNEP_EXTENDED_DATA_IND_T_PDU::set_data( const uint8* copyFrom , uint16 length )
{
    set_final_uint8Ptr ( BNEP_EXTENDED_DATA_IND_T_len , BNEP_EXTENDED_DATA_IND_T_pduSize , copyFrom , length );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: BNEP_DATA_IND_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

BNEP_DATA_IND_T_PDU::BNEP_DATA_IND_T_PDU()
: BNEP_PDU ( BNEP_DATA_IND , BNEP_DATA_IND_T_pduSize )
{
}

BNEP_DATA_IND_T_PDU::BNEP_DATA_IND_T_PDU( const PDU& from )
: BNEP_PDU ( from )
{
}

BNEP_DATA_IND_T_PDU::BNEP_DATA_IND_T_PDU( const uint8 * buffer , uint32 len )
: BNEP_PDU ( buffer , len )
{
}

uint16 BNEP_DATA_IND_T_PDU::get_phandle() const
{
    return get_uint16 ( BNEP_DATA_IND_T_phandle );
}

void BNEP_DATA_IND_T_PDU::set_phandle( uint16 value )
{
    set_uint16 ( BNEP_DATA_IND_T_phandle , value );
}

uint16 BNEP_DATA_IND_T_PDU::get_len() const
{
    return get_uint16 ( BNEP_DATA_IND_T_len );
}

void BNEP_DATA_IND_T_PDU::get_data( uint8 * copyTo ) const
{
    get_uint8Array ( copyTo , BNEP_DATA_IND_T_pduSize , get_len() );
}

const uint8 * BNEP_DATA_IND_T_PDU::get_dataPtr() const
{
    return get_uint8ArrayPtr ( BNEP_DATA_IND_T_pduSize );
}

void BNEP_DATA_IND_T_PDU::set_data( const uint8* copyFrom , uint16 length )
{
    set_final_uint8Ptr ( BNEP_DATA_IND_T_len , BNEP_DATA_IND_T_pduSize , copyFrom , length );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: BNEP_MULTICAST_DATA_IND_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

BNEP_MULTICAST_DATA_IND_T_PDU::BNEP_MULTICAST_DATA_IND_T_PDU()
: BNEP_PDU ( BNEP_MULTICAST_DATA_IND , BNEP_MULTICAST_DATA_IND_T_pduSize )
{
}

BNEP_MULTICAST_DATA_IND_T_PDU::BNEP_MULTICAST_DATA_IND_T_PDU( const PDU& from )
: BNEP_PDU ( from )
{
}

BNEP_MULTICAST_DATA_IND_T_PDU::BNEP_MULTICAST_DATA_IND_T_PDU( const uint8 * buffer , uint32 len )
: BNEP_PDU ( buffer , len )
{
}

uint16 BNEP_MULTICAST_DATA_IND_T_PDU::get_phandle() const
{
    return get_uint16 ( BNEP_MULTICAST_DATA_IND_T_phandle );
}

void BNEP_MULTICAST_DATA_IND_T_PDU::set_phandle( uint16 value )
{
    set_uint16 ( BNEP_MULTICAST_DATA_IND_T_phandle , value );
}

uint16 BNEP_MULTICAST_DATA_IND_T_PDU::get_len() const
{
    return get_uint16 ( BNEP_MULTICAST_DATA_IND_T_len );
}

void BNEP_MULTICAST_DATA_IND_T_PDU::get_data( uint8 * copyTo ) const
{
    get_uint8Array ( copyTo , BNEP_MULTICAST_DATA_IND_T_pduSize , get_len() );
}

const uint8 * BNEP_MULTICAST_DATA_IND_T_PDU::get_dataPtr() const
{
    return get_uint8ArrayPtr ( BNEP_MULTICAST_DATA_IND_T_pduSize );
}

void BNEP_MULTICAST_DATA_IND_T_PDU::set_data( const uint8* copyFrom , uint16 length )
{
    set_final_uint8Ptr ( BNEP_MULTICAST_DATA_IND_T_len , BNEP_MULTICAST_DATA_IND_T_pduSize , copyFrom , length );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: BNEP_EXTENDED_DATA_REQ_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

BNEP_EXTENDED_DATA_REQ_T_PDU::BNEP_EXTENDED_DATA_REQ_T_PDU()
: BNEP_PDU ( BNEP_EXTENDED_DATA_REQ , BNEP_EXTENDED_DATA_REQ_T_pduSize )
{
}

BNEP_EXTENDED_DATA_REQ_T_PDU::BNEP_EXTENDED_DATA_REQ_T_PDU( const PDU& from )
: BNEP_PDU ( from )
{
}

BNEP_EXTENDED_DATA_REQ_T_PDU::BNEP_EXTENDED_DATA_REQ_T_PDU( const uint8 * buffer , uint32 len )
: BNEP_PDU ( buffer , len )
{
}

uint16 BNEP_EXTENDED_DATA_REQ_T_PDU::get_id() const
{
    return get_uint16 ( BNEP_EXTENDED_DATA_REQ_T_id );
}

void BNEP_EXTENDED_DATA_REQ_T_PDU::set_id( uint16 value )
{
    set_uint16 ( BNEP_EXTENDED_DATA_REQ_T_id , value );
}

uint16 BNEP_EXTENDED_DATA_REQ_T_PDU::get_ether_type() const
{
    return get_uint16 ( BNEP_EXTENDED_DATA_REQ_T_ether_type );
}

void BNEP_EXTENDED_DATA_REQ_T_PDU::set_ether_type( uint16 value )
{
    set_uint16 ( BNEP_EXTENDED_DATA_REQ_T_ether_type , value );
}

EtherNetAddress BNEP_EXTENDED_DATA_REQ_T_PDU::get_dst_addr() const
{
    return get_EtherNetAddress ( BNEP_EXTENDED_DATA_REQ_T_dst_addr );
}

void BNEP_EXTENDED_DATA_REQ_T_PDU::set_dst_addr( EtherNetAddress value )
{
    set_EtherNetAddress ( BNEP_EXTENDED_DATA_REQ_T_dst_addr , value );
}

EtherNetAddress BNEP_EXTENDED_DATA_REQ_T_PDU::get_src_addr() const
{
    return get_EtherNetAddress ( BNEP_EXTENDED_DATA_REQ_T_src_addr );
}

void BNEP_EXTENDED_DATA_REQ_T_PDU::set_src_addr( EtherNetAddress value )
{
    set_EtherNetAddress ( BNEP_EXTENDED_DATA_REQ_T_src_addr , value );
}

uint16 BNEP_EXTENDED_DATA_REQ_T_PDU::get_len() const
{
    return get_uint16 ( BNEP_EXTENDED_DATA_REQ_T_len );
}

void BNEP_EXTENDED_DATA_REQ_T_PDU::set_len(uint16 len)
{
    set_uint16 ( BNEP_EXTENDED_DATA_REQ_T_len , len);
}

void BNEP_EXTENDED_DATA_REQ_T_PDU::get_data( uint8 * copyTo ) const
{
    get_uint8Array ( copyTo , BNEP_EXTENDED_DATA_REQ_T_pduSize , get_len() );
}

const uint8 * BNEP_EXTENDED_DATA_REQ_T_PDU::get_dataPtr() const
{
    return get_uint8ArrayPtr ( BNEP_EXTENDED_DATA_REQ_T_pduSize );
}

void BNEP_EXTENDED_DATA_REQ_T_PDU::set_data( const uint8* copyFrom , uint16 length )
{
    set_final_uint8Ptr ( BNEP_EXTENDED_DATA_REQ_T_len , BNEP_EXTENDED_DATA_REQ_T_pduSize , copyFrom , length );
}


