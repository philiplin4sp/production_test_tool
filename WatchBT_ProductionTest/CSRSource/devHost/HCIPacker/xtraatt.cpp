//  FILE DEFINING A BLUECORE-FRIENDY PROTOCOL

#include "bcflayout.h"

enum {
	OFFSET_START(ATT_READ_CFM_T)
	OFFSET_uint16(ATT_READ_CFM_T_type),
	OFFSET_uint16(ATT_READ_CFM_T_phandle),
	OFFSET_uint16(ATT_READ_CFM_T_cid),
	OFFSET_uint16(ATT_READ_CFM_T_result),
	OFFSET_uint16(ATT_READ_CFM_T_size_value),
	OFFSET_uint16(ATT_READ_CFM_T_value),
	ATT_READ_CFM_T_pduSize
} ;

enum {
	OFFSET_START(ATT_READ_BY_TYPE_CFM_T)
	OFFSET_uint16(ATT_READ_BY_TYPE_CFM_T_type),
	OFFSET_uint16(ATT_READ_BY_TYPE_CFM_T_phandle),
	OFFSET_uint16(ATT_READ_BY_TYPE_CFM_T_cid),
	OFFSET_uint16(ATT_READ_BY_TYPE_CFM_T_handle),
	OFFSET_uint16(ATT_READ_BY_TYPE_CFM_T_result),
	OFFSET_uint16(ATT_READ_BY_TYPE_CFM_T_size_value),
	OFFSET_uint16(ATT_READ_BY_TYPE_CFM_T_value),
	ATT_READ_BY_TYPE_CFM_T_pduSize
} ;

enum {
	OFFSET_START(ATT_HANDLE_VALUE_REQ_T)
	OFFSET_uint16(ATT_HANDLE_VALUE_REQ_T_type),
	OFFSET_uint16(ATT_HANDLE_VALUE_REQ_T_phandle),
	OFFSET_uint16(ATT_HANDLE_VALUE_REQ_T_cid),
	OFFSET_uint16(ATT_HANDLE_VALUE_REQ_T_handle),
	OFFSET_uint16(ATT_HANDLE_VALUE_REQ_T_flags),
	OFFSET_uint16(ATT_HANDLE_VALUE_REQ_T_size_value),
	OFFSET_uint16(ATT_HANDLE_VALUE_REQ_T_value),
	ATT_HANDLE_VALUE_REQ_T_pduSize
} ;

enum {
	OFFSET_START(ATT_READ_BY_GROUP_TYPE_CFM_T)
	OFFSET_uint16(ATT_READ_BY_GROUP_TYPE_CFM_T_type),
	OFFSET_uint16(ATT_READ_BY_GROUP_TYPE_CFM_T_phandle),
	OFFSET_uint16(ATT_READ_BY_GROUP_TYPE_CFM_T_cid),
	OFFSET_uint16(ATT_READ_BY_GROUP_TYPE_CFM_T_result),
	OFFSET_uint16(ATT_READ_BY_GROUP_TYPE_CFM_T_handle),
	OFFSET_uint16(ATT_READ_BY_GROUP_TYPE_CFM_T_end),
	OFFSET_uint16(ATT_READ_BY_GROUP_TYPE_CFM_T_size_value),
	OFFSET_uint16(ATT_READ_BY_GROUP_TYPE_CFM_T_value),
	ATT_READ_BY_GROUP_TYPE_CFM_T_pduSize
} ;

enum {
	OFFSET_START(ATT_FIND_BY_TYPE_VALUE_REQ_T)
	OFFSET_uint16(ATT_FIND_BY_TYPE_VALUE_REQ_T_type),
	OFFSET_uint16(ATT_FIND_BY_TYPE_VALUE_REQ_T_phandle),
	OFFSET_uint16(ATT_FIND_BY_TYPE_VALUE_REQ_T_cid),
	OFFSET_uint16(ATT_FIND_BY_TYPE_VALUE_REQ_T_start),
	OFFSET_uint16(ATT_FIND_BY_TYPE_VALUE_REQ_T_end),
	OFFSET_uint16(ATT_FIND_BY_TYPE_VALUE_REQ_T_uuid),
	OFFSET_uint16(ATT_FIND_BY_TYPE_VALUE_REQ_T_size_value),
	OFFSET_uint16(ATT_FIND_BY_TYPE_VALUE_REQ_T_value),
	ATT_FIND_BY_TYPE_VALUE_REQ_T_pduSize
} ;

enum {
	OFFSET_START(ATT_READ_MULTI_CFM_T)
	OFFSET_uint16(ATT_READ_MULTI_CFM_T_type),
	OFFSET_uint16(ATT_READ_MULTI_CFM_T_phandle),
	OFFSET_uint16(ATT_READ_MULTI_CFM_T_cid),
	OFFSET_uint16(ATT_READ_MULTI_CFM_T_result),
	OFFSET_uint16(ATT_READ_MULTI_CFM_T_size_value),
	OFFSET_uint16(ATT_READ_MULTI_CFM_T_value),
	ATT_READ_MULTI_CFM_T_pduSize
} ;

enum {
	OFFSET_START(ATT_WRITE_REQ_T)
	OFFSET_uint16(ATT_WRITE_REQ_T_type),
	OFFSET_uint16(ATT_WRITE_REQ_T_phandle),
	OFFSET_uint16(ATT_WRITE_REQ_T_cid),
	OFFSET_uint16(ATT_WRITE_REQ_T_handle),
	OFFSET_uint16(ATT_WRITE_REQ_T_flags),
	OFFSET_uint16(ATT_WRITE_REQ_T_size_value),
	OFFSET_uint16(ATT_WRITE_REQ_T_value),
	ATT_WRITE_REQ_T_pduSize
} ;

enum {
	OFFSET_START(ATT_PREPARE_WRITE_REQ_T)
	OFFSET_uint16(ATT_PREPARE_WRITE_REQ_T_type),
	OFFSET_uint16(ATT_PREPARE_WRITE_REQ_T_phandle),
	OFFSET_uint16(ATT_PREPARE_WRITE_REQ_T_cid),
	OFFSET_uint16(ATT_PREPARE_WRITE_REQ_T_handle),
	OFFSET_uint16(ATT_PREPARE_WRITE_REQ_T_offset),
	OFFSET_uint16(ATT_PREPARE_WRITE_REQ_T_size_value),
	OFFSET_uint16(ATT_PREPARE_WRITE_REQ_T_value),
	ATT_PREPARE_WRITE_REQ_T_pduSize
} ;

enum {
	OFFSET_START(ATT_PREPARE_WRITE_CFM_T)
	OFFSET_uint16(ATT_PREPARE_WRITE_CFM_T_type),
	OFFSET_uint16(ATT_PREPARE_WRITE_CFM_T_phandle),
	OFFSET_uint16(ATT_PREPARE_WRITE_CFM_T_cid),
	OFFSET_uint16(ATT_PREPARE_WRITE_CFM_T_handle),
	OFFSET_uint16(ATT_PREPARE_WRITE_CFM_T_offset),
	OFFSET_uint16(ATT_PREPARE_WRITE_CFM_T_result),
	OFFSET_uint16(ATT_PREPARE_WRITE_CFM_T_size_value),
	OFFSET_uint16(ATT_PREPARE_WRITE_CFM_T_value),
	ATT_PREPARE_WRITE_CFM_T_pduSize
} ;

enum {
	OFFSET_START(ATT_HANDLE_VALUE_IND_T)
	OFFSET_uint16(ATT_HANDLE_VALUE_IND_T_type),
	OFFSET_uint16(ATT_HANDLE_VALUE_IND_T_phandle),
	OFFSET_uint16(ATT_HANDLE_VALUE_IND_T_cid),
	OFFSET_uint16(ATT_HANDLE_VALUE_IND_T_handle),
	OFFSET_uint16(ATT_HANDLE_VALUE_IND_T_flags),
	OFFSET_uint16(ATT_HANDLE_VALUE_IND_T_size_value),
	OFFSET_uint16(ATT_HANDLE_VALUE_IND_T_value),
	ATT_HANDLE_VALUE_IND_T_pduSize
} ;

enum {
	OFFSET_START(ATT_FIND_INFO_CFM_T)
	OFFSET_uint16(ATT_FIND_INFO_CFM_T_type),
	OFFSET_uint16(ATT_FIND_INFO_CFM_T_phandle),
	OFFSET_uint16(ATT_FIND_INFO_CFM_T_cid),
	OFFSET_uint16(ATT_FIND_INFO_CFM_T_handle),
	OFFSET_uint16(ATT_FIND_INFO_CFM_T_uuid_type),
	OFFSET_uint32_array(ATT_FIND_INFO_CFM_T_uuid,4),
	OFFSET_uint16(ATT_FIND_INFO_CFM_T_result),
	ATT_FIND_INFO_CFM_T_pduSize
} ;

enum {
	OFFSET_START(ATT_ADD_DB_REQ_T)
	OFFSET_uint16(ATT_ADD_DB_REQ_T_type),
	OFFSET_uint16(ATT_ADD_DB_REQ_T_phandle),
	OFFSET_uint16(ATT_ADD_DB_REQ_T_size_db),
	OFFSET_uint16(ATT_ADD_DB_REQ_T_db),
	ATT_ADD_DB_REQ_T_pduSize
} ;

enum {
	OFFSET_START(ATT_READ_BY_TYPE_REQ_T)
	OFFSET_uint16(ATT_READ_BY_TYPE_REQ_T_type),
	OFFSET_uint16(ATT_READ_BY_TYPE_REQ_T_phandle),
	OFFSET_uint16(ATT_READ_BY_TYPE_REQ_T_cid),
	OFFSET_uint16(ATT_READ_BY_TYPE_REQ_T_start),
	OFFSET_uint16(ATT_READ_BY_TYPE_REQ_T_end),
	OFFSET_uint16(ATT_READ_BY_TYPE_REQ_T_uuid_type),
	OFFSET_uint32_array(ATT_READ_BY_TYPE_REQ_T_uuid,4),
	ATT_READ_BY_TYPE_REQ_T_pduSize
} ;

enum {
	OFFSET_START(ATT_READ_BY_GROUP_TYPE_REQ_T)
	OFFSET_uint16(ATT_READ_BY_GROUP_TYPE_REQ_T_type),
	OFFSET_uint16(ATT_READ_BY_GROUP_TYPE_REQ_T_phandle),
	OFFSET_uint16(ATT_READ_BY_GROUP_TYPE_REQ_T_cid),
	OFFSET_uint16(ATT_READ_BY_GROUP_TYPE_REQ_T_start),
	OFFSET_uint16(ATT_READ_BY_GROUP_TYPE_REQ_T_end),
	OFFSET_uint16(ATT_READ_BY_GROUP_TYPE_REQ_T_group_type),
	OFFSET_uint32_array(ATT_READ_BY_GROUP_TYPE_REQ_T_group,4),
	ATT_READ_BY_GROUP_TYPE_REQ_T_pduSize
} ;

enum {
	OFFSET_START(ATT_DEBUG_IND_T)
	OFFSET_uint16(ATT_DEBUG_IND_T_type),
	OFFSET_uint16(ATT_DEBUG_IND_T_size_debug),
	OFFSET_uint16(ATT_DEBUG_IND_T_debug),
	ATT_DEBUG_IND_T_pduSize
} ;

enum {
	OFFSET_START(ATT_READ_BLOB_CFM_T)
	OFFSET_uint16(ATT_READ_BLOB_CFM_T_type),
	OFFSET_uint16(ATT_READ_BLOB_CFM_T_phandle),
	OFFSET_uint16(ATT_READ_BLOB_CFM_T_cid),
	OFFSET_uint16(ATT_READ_BLOB_CFM_T_result),
	OFFSET_uint16(ATT_READ_BLOB_CFM_T_size_value),
	OFFSET_uint16(ATT_READ_BLOB_CFM_T_value),
	ATT_READ_BLOB_CFM_T_pduSize
} ;

enum {
	OFFSET_START(ATT_READ_MULTI_REQ_T)
	OFFSET_uint16(ATT_READ_MULTI_REQ_T_type),
	OFFSET_uint16(ATT_READ_MULTI_REQ_T_phandle),
	OFFSET_uint16(ATT_READ_MULTI_REQ_T_cid),
	OFFSET_uint16(ATT_READ_MULTI_REQ_T_size_handles),
	OFFSET_uint16(ATT_READ_MULTI_REQ_T_handles),
	ATT_READ_MULTI_REQ_T_pduSize
} ;

enum {
	OFFSET_START(ATT_ACCESS_IND_T)
	OFFSET_uint16(ATT_ACCESS_IND_T_type),
	OFFSET_uint16(ATT_ACCESS_IND_T_phandle),
	OFFSET_uint16(ATT_ACCESS_IND_T_cid),
	OFFSET_uint16(ATT_ACCESS_IND_T_handle),
	OFFSET_uint16(ATT_ACCESS_IND_T_flags),
	OFFSET_uint16(ATT_ACCESS_IND_T_offset),
	OFFSET_uint16(ATT_ACCESS_IND_T_size_value),
	OFFSET_uint16(ATT_ACCESS_IND_T_value),
	ATT_ACCESS_IND_T_pduSize
} ;

enum {
	OFFSET_START(ATT_ACCESS_RSP_T)
	OFFSET_uint16(ATT_ACCESS_RSP_T_type),
	OFFSET_uint16(ATT_ACCESS_RSP_T_phandle),
	OFFSET_uint16(ATT_ACCESS_RSP_T_cid),
	OFFSET_uint16(ATT_ACCESS_RSP_T_handle),
	OFFSET_uint16(ATT_ACCESS_RSP_T_result),
	OFFSET_uint16(ATT_ACCESS_RSP_T_size_value),
	OFFSET_uint16(ATT_ACCESS_RSP_T_value),
	ATT_ACCESS_RSP_T_pduSize
} ;

#include "hcipacker/gen_att.h"
 
///////////////////////////////////////////////////////////////////////////////
//
//  Class: ATT_READ_CFM_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

ATT_READ_CFM_T_PDU::ATT_READ_CFM_T_PDU()
: ATT_PDU ( ATT_READ_CFM , ATT_READ_CFM_T_pduSize )
{
}

ATT_READ_CFM_T_PDU::ATT_READ_CFM_T_PDU( const PDU& from )
: ATT_PDU ( from )
{
}

ATT_READ_CFM_T_PDU::ATT_READ_CFM_T_PDU( const uint8 * buffer , uint32 len )
: ATT_PDU ( buffer , len )
{
}

uint16 ATT_READ_CFM_T_PDU::get_phandle() const
{
    return get_uint16 ( ATT_READ_CFM_T_phandle );
}

void ATT_READ_CFM_T_PDU::set_phandle( uint16 value )
{
    set_uint16 ( ATT_READ_CFM_T_phandle , value );
}

uint16 ATT_READ_CFM_T_PDU::get_cid() const
{
    return get_uint16 ( ATT_READ_CFM_T_cid );
}

void ATT_READ_CFM_T_PDU::set_cid( uint16 value )
{
    set_uint16 ( ATT_READ_CFM_T_cid , value );
}

uint16 ATT_READ_CFM_T_PDU::get_result() const
{
    return get_uint16 ( ATT_READ_CFM_T_result );
}

void ATT_READ_CFM_T_PDU::set_result( uint16 value )
{
    set_uint16 ( ATT_READ_CFM_T_result , value );
}

uint16 ATT_READ_CFM_T_PDU::get_size_value() const
{
    return get_uint16 ( ATT_READ_CFM_T_size_value );
}

void ATT_READ_CFM_T_PDU::set_size_value( uint16 value )
{
    set_uint16 ( ATT_READ_CFM_T_size_value , value );
}

const uint8 *ATT_READ_CFM_T_PDU::get_valuePtr() const
{
    return get_uint8ArrayPtr ( ATT_READ_CFM_T_pduSize );
}

void ATT_READ_CFM_T_PDU::set_value( const uint8 *value, uint16 len )
{
    set_final_uint8Ptr (ATT_READ_CFM_T_size_value , ATT_READ_CFM_T_pduSize , value , len);
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: ATT_READ_BY_TYPE_CFM_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

ATT_READ_BY_TYPE_CFM_T_PDU::ATT_READ_BY_TYPE_CFM_T_PDU()
: ATT_PDU ( ATT_READ_BY_TYPE_CFM , ATT_READ_BY_TYPE_CFM_T_pduSize )
{
}

ATT_READ_BY_TYPE_CFM_T_PDU::ATT_READ_BY_TYPE_CFM_T_PDU( const PDU& from )
: ATT_PDU ( from )
{
}

ATT_READ_BY_TYPE_CFM_T_PDU::ATT_READ_BY_TYPE_CFM_T_PDU( const uint8 * buffer , uint32 len )
: ATT_PDU ( buffer , len )
{
}

uint16 ATT_READ_BY_TYPE_CFM_T_PDU::get_phandle() const
{
    return get_uint16 ( ATT_READ_BY_TYPE_CFM_T_phandle );
}

void ATT_READ_BY_TYPE_CFM_T_PDU::set_phandle( uint16 value )
{
    set_uint16 ( ATT_READ_BY_TYPE_CFM_T_phandle , value );
}

uint16 ATT_READ_BY_TYPE_CFM_T_PDU::get_cid() const
{
    return get_uint16 ( ATT_READ_BY_TYPE_CFM_T_cid );
}

void ATT_READ_BY_TYPE_CFM_T_PDU::set_cid( uint16 value )
{
    set_uint16 ( ATT_READ_BY_TYPE_CFM_T_cid , value );
}

uint16 ATT_READ_BY_TYPE_CFM_T_PDU::get_handle() const
{
    return get_uint16 ( ATT_READ_BY_TYPE_CFM_T_handle );
}

void ATT_READ_BY_TYPE_CFM_T_PDU::set_handle( uint16 value )
{
    set_uint16 ( ATT_READ_BY_TYPE_CFM_T_handle , value );
}

uint16 ATT_READ_BY_TYPE_CFM_T_PDU::get_result() const
{
    return get_uint16 ( ATT_READ_BY_TYPE_CFM_T_result );
}

void ATT_READ_BY_TYPE_CFM_T_PDU::set_result( uint16 value )
{
    set_uint16 ( ATT_READ_BY_TYPE_CFM_T_result , value );
}

uint16 ATT_READ_BY_TYPE_CFM_T_PDU::get_size_value() const
{
    return get_uint16 ( ATT_READ_BY_TYPE_CFM_T_size_value );
}

void ATT_READ_BY_TYPE_CFM_T_PDU::set_size_value( uint16 value )
{
    set_uint16 ( ATT_READ_BY_TYPE_CFM_T_size_value , value );
}

const uint8 *ATT_READ_BY_TYPE_CFM_T_PDU::get_valuePtr() const
{
    return get_uint8ArrayPtr ( ATT_READ_BY_TYPE_CFM_T_pduSize );
}

void ATT_READ_BY_TYPE_CFM_T_PDU::set_value( const uint8 *value, uint16 len )
{
    set_final_uint8Ptr (ATT_READ_BY_TYPE_CFM_T_size_value , ATT_READ_BY_TYPE_CFM_T_pduSize , value , len );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: ATT_HANDLE_VALUE_REQ_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

ATT_HANDLE_VALUE_REQ_T_PDU::ATT_HANDLE_VALUE_REQ_T_PDU()
: ATT_PDU ( ATT_HANDLE_VALUE_REQ , ATT_HANDLE_VALUE_REQ_T_pduSize )
{
}

ATT_HANDLE_VALUE_REQ_T_PDU::ATT_HANDLE_VALUE_REQ_T_PDU( const PDU& from )
: ATT_PDU ( from )
{
}

ATT_HANDLE_VALUE_REQ_T_PDU::ATT_HANDLE_VALUE_REQ_T_PDU( const uint8 * buffer , uint32 len )
: ATT_PDU ( buffer , len )
{
}

uint16 ATT_HANDLE_VALUE_REQ_T_PDU::get_phandle() const
{
    return get_uint16 ( ATT_HANDLE_VALUE_REQ_T_phandle );
}

void ATT_HANDLE_VALUE_REQ_T_PDU::set_phandle( uint16 value )
{
    set_uint16 ( ATT_HANDLE_VALUE_REQ_T_phandle , value );
}

uint16 ATT_HANDLE_VALUE_REQ_T_PDU::get_cid() const
{
    return get_uint16 ( ATT_HANDLE_VALUE_REQ_T_cid );
}

void ATT_HANDLE_VALUE_REQ_T_PDU::set_cid( uint16 value )
{
    set_uint16 ( ATT_HANDLE_VALUE_REQ_T_cid , value );
}

uint16 ATT_HANDLE_VALUE_REQ_T_PDU::get_handle() const
{
    return get_uint16 ( ATT_HANDLE_VALUE_REQ_T_handle );
}

void ATT_HANDLE_VALUE_REQ_T_PDU::set_handle( uint16 value )
{
    set_uint16 ( ATT_HANDLE_VALUE_REQ_T_handle , value );
}

uint16 ATT_HANDLE_VALUE_REQ_T_PDU::get_flags() const
{
    return get_uint16 ( ATT_HANDLE_VALUE_REQ_T_flags );
}

void ATT_HANDLE_VALUE_REQ_T_PDU::set_flags( uint16 value )
{
    set_uint16 ( ATT_HANDLE_VALUE_REQ_T_flags , value );
}

uint16 ATT_HANDLE_VALUE_REQ_T_PDU::get_size_value() const
{
    return get_uint16 ( ATT_HANDLE_VALUE_REQ_T_size_value );
}

void ATT_HANDLE_VALUE_REQ_T_PDU::set_size_value( uint16 value )
{
    set_uint16 ( ATT_HANDLE_VALUE_REQ_T_size_value , value );
}

const uint8 *ATT_HANDLE_VALUE_REQ_T_PDU::get_valuePtr() const
{
    return get_uint8ArrayPtr ( ATT_HANDLE_VALUE_REQ_T_pduSize );
}

void ATT_HANDLE_VALUE_REQ_T_PDU::set_value( const uint8 *value, uint16 len )
{
    set_final_uint8Ptr (ATT_HANDLE_VALUE_REQ_T_size_value , ATT_HANDLE_VALUE_REQ_T_pduSize , value , len );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: ATT_READ_BY_GROUP_TYPE_CFM_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

ATT_READ_BY_GROUP_TYPE_CFM_T_PDU::ATT_READ_BY_GROUP_TYPE_CFM_T_PDU()
: ATT_PDU ( ATT_READ_BY_GROUP_TYPE_CFM , ATT_READ_BY_GROUP_TYPE_CFM_T_pduSize )
{
}

ATT_READ_BY_GROUP_TYPE_CFM_T_PDU::ATT_READ_BY_GROUP_TYPE_CFM_T_PDU( const PDU& from )
: ATT_PDU ( from )
{
}

ATT_READ_BY_GROUP_TYPE_CFM_T_PDU::ATT_READ_BY_GROUP_TYPE_CFM_T_PDU( const uint8 * buffer , uint32 len )
: ATT_PDU ( buffer , len )
{
}

uint16 ATT_READ_BY_GROUP_TYPE_CFM_T_PDU::get_phandle() const
{
    return get_uint16 ( ATT_READ_BY_GROUP_TYPE_CFM_T_phandle );
}

void ATT_READ_BY_GROUP_TYPE_CFM_T_PDU::set_phandle( uint16 value )
{
    set_uint16 ( ATT_READ_BY_GROUP_TYPE_CFM_T_phandle , value );
}

uint16 ATT_READ_BY_GROUP_TYPE_CFM_T_PDU::get_cid() const
{
    return get_uint16 ( ATT_READ_BY_GROUP_TYPE_CFM_T_cid );
}

void ATT_READ_BY_GROUP_TYPE_CFM_T_PDU::set_cid( uint16 value )
{
    set_uint16 ( ATT_READ_BY_GROUP_TYPE_CFM_T_cid , value );
}

uint16 ATT_READ_BY_GROUP_TYPE_CFM_T_PDU::get_result() const
{
    return get_uint16 ( ATT_READ_BY_GROUP_TYPE_CFM_T_result );
}

void ATT_READ_BY_GROUP_TYPE_CFM_T_PDU::set_result( uint16 value )
{
    set_uint16 ( ATT_READ_BY_GROUP_TYPE_CFM_T_result , value );
}

uint16 ATT_READ_BY_GROUP_TYPE_CFM_T_PDU::get_handle() const
{
    return get_uint16 ( ATT_READ_BY_GROUP_TYPE_CFM_T_handle );
}

void ATT_READ_BY_GROUP_TYPE_CFM_T_PDU::set_handle( uint16 value )
{
    set_uint16 ( ATT_READ_BY_GROUP_TYPE_CFM_T_handle , value );
}

uint16 ATT_READ_BY_GROUP_TYPE_CFM_T_PDU::get_end() const
{
    return get_uint16 ( ATT_READ_BY_GROUP_TYPE_CFM_T_end );
}

void ATT_READ_BY_GROUP_TYPE_CFM_T_PDU::set_end( uint16 value )
{
    set_uint16 ( ATT_READ_BY_GROUP_TYPE_CFM_T_end , value );
}

uint16 ATT_READ_BY_GROUP_TYPE_CFM_T_PDU::get_size_value() const
{
    return get_uint16 ( ATT_READ_BY_GROUP_TYPE_CFM_T_size_value );
}

void ATT_READ_BY_GROUP_TYPE_CFM_T_PDU::set_size_value( uint16 value )
{
    set_uint16 ( ATT_READ_BY_GROUP_TYPE_CFM_T_size_value , value );
}

const uint8 *ATT_READ_BY_GROUP_TYPE_CFM_T_PDU::get_valuePtr() const
{
    return get_uint8ArrayPtr ( ATT_READ_BY_GROUP_TYPE_CFM_T_pduSize );
}

void ATT_READ_BY_GROUP_TYPE_CFM_T_PDU::set_value( const uint8 *value, uint16 len )
{
    set_final_uint8Ptr (ATT_READ_BY_GROUP_TYPE_CFM_T_size_value , ATT_READ_BY_GROUP_TYPE_CFM_T_pduSize , value , len );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: ATT_FIND_BY_TYPE_VALUE_REQ_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

ATT_FIND_BY_TYPE_VALUE_REQ_T_PDU::ATT_FIND_BY_TYPE_VALUE_REQ_T_PDU()
: ATT_PDU ( ATT_FIND_BY_TYPE_VALUE_REQ , ATT_FIND_BY_TYPE_VALUE_REQ_T_pduSize )
{
}

ATT_FIND_BY_TYPE_VALUE_REQ_T_PDU::ATT_FIND_BY_TYPE_VALUE_REQ_T_PDU( const PDU& from )
: ATT_PDU ( from )
{
}

ATT_FIND_BY_TYPE_VALUE_REQ_T_PDU::ATT_FIND_BY_TYPE_VALUE_REQ_T_PDU( const uint8 * buffer , uint32 len )
: ATT_PDU ( buffer , len )
{
}

uint16 ATT_FIND_BY_TYPE_VALUE_REQ_T_PDU::get_phandle() const
{
    return get_uint16 ( ATT_FIND_BY_TYPE_VALUE_REQ_T_phandle );
}

void ATT_FIND_BY_TYPE_VALUE_REQ_T_PDU::set_phandle( uint16 value )
{
    set_uint16 ( ATT_FIND_BY_TYPE_VALUE_REQ_T_phandle , value );
}

uint16 ATT_FIND_BY_TYPE_VALUE_REQ_T_PDU::get_cid() const
{
    return get_uint16 ( ATT_FIND_BY_TYPE_VALUE_REQ_T_cid );
}

void ATT_FIND_BY_TYPE_VALUE_REQ_T_PDU::set_cid( uint16 value )
{
    set_uint16 ( ATT_FIND_BY_TYPE_VALUE_REQ_T_cid , value );
}

uint16 ATT_FIND_BY_TYPE_VALUE_REQ_T_PDU::get_start() const
{
    return get_uint16 ( ATT_FIND_BY_TYPE_VALUE_REQ_T_start );
}

void ATT_FIND_BY_TYPE_VALUE_REQ_T_PDU::set_start( uint16 value )
{
    set_uint16 ( ATT_FIND_BY_TYPE_VALUE_REQ_T_start , value );
}

uint16 ATT_FIND_BY_TYPE_VALUE_REQ_T_PDU::get_end() const
{
    return get_uint16 ( ATT_FIND_BY_TYPE_VALUE_REQ_T_end );
}

void ATT_FIND_BY_TYPE_VALUE_REQ_T_PDU::set_end( uint16 value )
{
    set_uint16 ( ATT_FIND_BY_TYPE_VALUE_REQ_T_end , value );
}

uint16 ATT_FIND_BY_TYPE_VALUE_REQ_T_PDU::get_uuid() const
{
    return get_uint16 ( ATT_FIND_BY_TYPE_VALUE_REQ_T_uuid );
}

void ATT_FIND_BY_TYPE_VALUE_REQ_T_PDU::set_uuid( uint16 value )
{
    set_uint16 ( ATT_FIND_BY_TYPE_VALUE_REQ_T_uuid , value );
}

uint16 ATT_FIND_BY_TYPE_VALUE_REQ_T_PDU::get_size_value() const
{
    return get_uint16 ( ATT_FIND_BY_TYPE_VALUE_REQ_T_size_value );
}

void ATT_FIND_BY_TYPE_VALUE_REQ_T_PDU::set_size_value( uint16 value )
{
    set_uint16 ( ATT_FIND_BY_TYPE_VALUE_REQ_T_size_value , value );
}

const uint8 *ATT_FIND_BY_TYPE_VALUE_REQ_T_PDU::get_valuePtr() const
{
    return get_uint8ArrayPtr ( ATT_FIND_BY_TYPE_VALUE_REQ_T_pduSize );
}

void ATT_FIND_BY_TYPE_VALUE_REQ_T_PDU::set_value( const uint8 *value, uint16 len )
{
    set_final_uint8Ptr (ATT_FIND_BY_TYPE_VALUE_REQ_T_size_value , ATT_FIND_BY_TYPE_VALUE_REQ_T_pduSize , value , len);
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: ATT_READ_MULTI_CFM_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

ATT_READ_MULTI_CFM_T_PDU::ATT_READ_MULTI_CFM_T_PDU()
: ATT_PDU ( ATT_READ_MULTI_CFM , ATT_READ_MULTI_CFM_T_pduSize )
{
}

ATT_READ_MULTI_CFM_T_PDU::ATT_READ_MULTI_CFM_T_PDU( const PDU& from )
: ATT_PDU ( from )
{
}

ATT_READ_MULTI_CFM_T_PDU::ATT_READ_MULTI_CFM_T_PDU( const uint8 * buffer , uint32 len )
: ATT_PDU ( buffer , len )
{
}

uint16 ATT_READ_MULTI_CFM_T_PDU::get_phandle() const
{
    return get_uint16 ( ATT_READ_MULTI_CFM_T_phandle );
}

void ATT_READ_MULTI_CFM_T_PDU::set_phandle( uint16 value )
{
    set_uint16 ( ATT_READ_MULTI_CFM_T_phandle , value );
}

uint16 ATT_READ_MULTI_CFM_T_PDU::get_cid() const
{
    return get_uint16 ( ATT_READ_MULTI_CFM_T_cid );
}

void ATT_READ_MULTI_CFM_T_PDU::set_cid( uint16 value )
{
    set_uint16 ( ATT_READ_MULTI_CFM_T_cid , value );
}

uint16 ATT_READ_MULTI_CFM_T_PDU::get_result() const
{
    return get_uint16 ( ATT_READ_MULTI_CFM_T_result );
}

void ATT_READ_MULTI_CFM_T_PDU::set_result( uint16 value )
{
    set_uint16 ( ATT_READ_MULTI_CFM_T_result , value );
}

uint16 ATT_READ_MULTI_CFM_T_PDU::get_size_value() const
{
    return get_uint16 ( ATT_READ_MULTI_CFM_T_size_value );
}

void ATT_READ_MULTI_CFM_T_PDU::set_size_value( uint16 value )
{
    set_uint16 ( ATT_READ_MULTI_CFM_T_size_value , value );
}

const uint8 *ATT_READ_MULTI_CFM_T_PDU::get_valuePtr() const
{
    return get_uint8ArrayPtr ( ATT_READ_MULTI_CFM_T_pduSize );
}

void ATT_READ_MULTI_CFM_T_PDU::set_value( const uint8 *value, uint16 len )
{
    set_final_uint8Ptr (ATT_READ_MULTI_CFM_T_size_value , ATT_READ_MULTI_CFM_T_pduSize , value , len);
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: ATT_WRITE_REQ_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

ATT_WRITE_REQ_T_PDU::ATT_WRITE_REQ_T_PDU()
: ATT_PDU ( ATT_WRITE_REQ , ATT_WRITE_REQ_T_pduSize )
{
}

ATT_WRITE_REQ_T_PDU::ATT_WRITE_REQ_T_PDU( const PDU& from )
: ATT_PDU ( from )
{
}

ATT_WRITE_REQ_T_PDU::ATT_WRITE_REQ_T_PDU( const uint8 * buffer , uint32 len )
: ATT_PDU ( buffer , len )
{
}

uint16 ATT_WRITE_REQ_T_PDU::get_phandle() const
{
    return get_uint16 ( ATT_WRITE_REQ_T_phandle );
}

void ATT_WRITE_REQ_T_PDU::set_phandle( uint16 value )
{
    set_uint16 ( ATT_WRITE_REQ_T_phandle , value );
}

uint16 ATT_WRITE_REQ_T_PDU::get_cid() const
{
    return get_uint16 ( ATT_WRITE_REQ_T_cid );
}

void ATT_WRITE_REQ_T_PDU::set_cid( uint16 value )
{
    set_uint16 ( ATT_WRITE_REQ_T_cid , value );
}

uint16 ATT_WRITE_REQ_T_PDU::get_handle() const
{
    return get_uint16 ( ATT_WRITE_REQ_T_handle );
}

void ATT_WRITE_REQ_T_PDU::set_handle( uint16 value )
{
    set_uint16 ( ATT_WRITE_REQ_T_handle , value );
}

uint16 ATT_WRITE_REQ_T_PDU::get_flags() const
{
    return get_uint16 ( ATT_WRITE_REQ_T_flags );
}

void ATT_WRITE_REQ_T_PDU::set_flags( uint16 value )
{
    set_uint16 ( ATT_WRITE_REQ_T_flags , value );
}

uint16 ATT_WRITE_REQ_T_PDU::get_size_value() const
{
    return get_uint16 ( ATT_WRITE_REQ_T_size_value );
}

void ATT_WRITE_REQ_T_PDU::set_size_value( uint16 value )
{
    set_uint16 ( ATT_WRITE_REQ_T_size_value , value );
}

const uint8 *ATT_WRITE_REQ_T_PDU::get_valuePtr() const
{
    return get_uint8ArrayPtr ( ATT_WRITE_REQ_T_pduSize );
}

void ATT_WRITE_REQ_T_PDU::set_value( const uint8 *value, uint16 len )
{
    set_final_uint8Ptr (ATT_WRITE_REQ_T_size_value , ATT_WRITE_REQ_T_pduSize , value , len );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: ATT_PREPARE_WRITE_REQ_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

ATT_PREPARE_WRITE_REQ_T_PDU::ATT_PREPARE_WRITE_REQ_T_PDU()
: ATT_PDU ( ATT_PREPARE_WRITE_REQ , ATT_PREPARE_WRITE_REQ_T_pduSize )
{
}

ATT_PREPARE_WRITE_REQ_T_PDU::ATT_PREPARE_WRITE_REQ_T_PDU( const PDU& from )
: ATT_PDU ( from )
{
}

ATT_PREPARE_WRITE_REQ_T_PDU::ATT_PREPARE_WRITE_REQ_T_PDU( const uint8 * buffer , uint32 len )
: ATT_PDU ( buffer , len )
{
}

uint16 ATT_PREPARE_WRITE_REQ_T_PDU::get_phandle() const
{
    return get_uint16 ( ATT_PREPARE_WRITE_REQ_T_phandle );
}

void ATT_PREPARE_WRITE_REQ_T_PDU::set_phandle( uint16 value )
{
    set_uint16 ( ATT_PREPARE_WRITE_REQ_T_phandle , value );
}

uint16 ATT_PREPARE_WRITE_REQ_T_PDU::get_cid() const
{
    return get_uint16 ( ATT_PREPARE_WRITE_REQ_T_cid );
}

void ATT_PREPARE_WRITE_REQ_T_PDU::set_cid( uint16 value )
{
    set_uint16 ( ATT_PREPARE_WRITE_REQ_T_cid , value );
}

uint16 ATT_PREPARE_WRITE_REQ_T_PDU::get_handle() const
{
    return get_uint16 ( ATT_PREPARE_WRITE_REQ_T_handle );
}

void ATT_PREPARE_WRITE_REQ_T_PDU::set_handle( uint16 value )
{
    set_uint16 ( ATT_PREPARE_WRITE_REQ_T_handle , value );
}

uint16 ATT_PREPARE_WRITE_REQ_T_PDU::get_offset() const
{
    return get_uint16 ( ATT_PREPARE_WRITE_REQ_T_offset );
}

void ATT_PREPARE_WRITE_REQ_T_PDU::set_offset( uint16 value )
{
    set_uint16 ( ATT_PREPARE_WRITE_REQ_T_offset , value );
}

uint16 ATT_PREPARE_WRITE_REQ_T_PDU::get_size_value() const
{
    return get_uint16 ( ATT_PREPARE_WRITE_REQ_T_size_value );
}

void ATT_PREPARE_WRITE_REQ_T_PDU::set_size_value( uint16 value )
{
    set_uint16 ( ATT_PREPARE_WRITE_REQ_T_size_value , value );
}

const uint8 *ATT_PREPARE_WRITE_REQ_T_PDU::get_valuePtr() const
{
    return get_uint8ArrayPtr ( ATT_PREPARE_WRITE_REQ_T_pduSize );
}

void ATT_PREPARE_WRITE_REQ_T_PDU::set_value( const uint8 *value, uint16 len )
{
    set_final_uint8Ptr (ATT_PREPARE_WRITE_REQ_T_size_value , ATT_PREPARE_WRITE_REQ_T_pduSize , value , len );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: ATT_PREPARE_WRITE_CFM_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

ATT_PREPARE_WRITE_CFM_T_PDU::ATT_PREPARE_WRITE_CFM_T_PDU()
: ATT_PDU ( ATT_PREPARE_WRITE_CFM , ATT_PREPARE_WRITE_CFM_T_pduSize )
{
}

ATT_PREPARE_WRITE_CFM_T_PDU::ATT_PREPARE_WRITE_CFM_T_PDU( const PDU& from )
: ATT_PDU ( from )
{
}

ATT_PREPARE_WRITE_CFM_T_PDU::ATT_PREPARE_WRITE_CFM_T_PDU( const uint8 * buffer , uint32 len )
: ATT_PDU ( buffer , len )
{
}

uint16 ATT_PREPARE_WRITE_CFM_T_PDU::get_phandle() const
{
    return get_uint16 ( ATT_PREPARE_WRITE_CFM_T_phandle );
}

void ATT_PREPARE_WRITE_CFM_T_PDU::set_phandle( uint16 value )
{
    set_uint16 ( ATT_PREPARE_WRITE_CFM_T_phandle , value );
}

uint16 ATT_PREPARE_WRITE_CFM_T_PDU::get_cid() const
{
    return get_uint16 ( ATT_PREPARE_WRITE_CFM_T_cid );
}

void ATT_PREPARE_WRITE_CFM_T_PDU::set_cid( uint16 value )
{
    set_uint16 ( ATT_PREPARE_WRITE_CFM_T_cid , value );
}

uint16 ATT_PREPARE_WRITE_CFM_T_PDU::get_handle() const
{
    return get_uint16 ( ATT_PREPARE_WRITE_CFM_T_handle );
}

void ATT_PREPARE_WRITE_CFM_T_PDU::set_handle( uint16 value )
{
    set_uint16 ( ATT_PREPARE_WRITE_CFM_T_handle , value );
}

uint16 ATT_PREPARE_WRITE_CFM_T_PDU::get_offset() const
{
    return get_uint16 ( ATT_PREPARE_WRITE_CFM_T_offset );
}

void ATT_PREPARE_WRITE_CFM_T_PDU::set_offset( uint16 value )
{
    set_uint16 ( ATT_PREPARE_WRITE_CFM_T_offset , value );
}

uint16 ATT_PREPARE_WRITE_CFM_T_PDU::get_result() const
{
    return get_uint16 ( ATT_PREPARE_WRITE_CFM_T_result );
}

void ATT_PREPARE_WRITE_CFM_T_PDU::set_result( uint16 value )
{
    set_uint16 ( ATT_PREPARE_WRITE_CFM_T_result , value );
}

uint16 ATT_PREPARE_WRITE_CFM_T_PDU::get_size_value() const
{
    return get_uint16 ( ATT_PREPARE_WRITE_CFM_T_size_value );
}

void ATT_PREPARE_WRITE_CFM_T_PDU::set_size_value( uint16 value )
{
    set_uint16 ( ATT_PREPARE_WRITE_CFM_T_size_value , value );
}

const uint8 *ATT_PREPARE_WRITE_CFM_T_PDU::get_valuePtr() const
{
    return get_uint8ArrayPtr ( ATT_PREPARE_WRITE_CFM_T_pduSize );
}

void ATT_PREPARE_WRITE_CFM_T_PDU::set_value( const uint8 *value, uint16 len )
{
    set_final_uint8Ptr (ATT_PREPARE_WRITE_CFM_T_size_value , ATT_PREPARE_WRITE_CFM_T_pduSize , value , len );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: ATT_HANDLE_VALUE_IND_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

ATT_HANDLE_VALUE_IND_T_PDU::ATT_HANDLE_VALUE_IND_T_PDU()
: ATT_PDU ( ATT_HANDLE_VALUE_IND , ATT_HANDLE_VALUE_IND_T_pduSize )
{
}

ATT_HANDLE_VALUE_IND_T_PDU::ATT_HANDLE_VALUE_IND_T_PDU( const PDU& from )
: ATT_PDU ( from )
{
}

ATT_HANDLE_VALUE_IND_T_PDU::ATT_HANDLE_VALUE_IND_T_PDU( const uint8 * buffer , uint32 len )
: ATT_PDU ( buffer , len )
{
}

uint16 ATT_HANDLE_VALUE_IND_T_PDU::get_phandle() const
{
    return get_uint16 ( ATT_HANDLE_VALUE_IND_T_phandle );
}

void ATT_HANDLE_VALUE_IND_T_PDU::set_phandle( uint16 value )
{
    set_uint16 ( ATT_HANDLE_VALUE_IND_T_phandle , value );
}

uint16 ATT_HANDLE_VALUE_IND_T_PDU::get_cid() const
{
    return get_uint16 ( ATT_HANDLE_VALUE_IND_T_cid );
}

void ATT_HANDLE_VALUE_IND_T_PDU::set_cid( uint16 value )
{
    set_uint16 ( ATT_HANDLE_VALUE_IND_T_cid , value );
}

uint16 ATT_HANDLE_VALUE_IND_T_PDU::get_handle() const
{
    return get_uint16 ( ATT_HANDLE_VALUE_IND_T_handle );
}

void ATT_HANDLE_VALUE_IND_T_PDU::set_handle( uint16 value )
{
    set_uint16 ( ATT_HANDLE_VALUE_IND_T_handle , value );
}

uint16 ATT_HANDLE_VALUE_IND_T_PDU::get_flags() const
{
    return get_uint16 ( ATT_HANDLE_VALUE_IND_T_flags );
}

void ATT_HANDLE_VALUE_IND_T_PDU::set_flags( uint16 value )
{
    set_uint16 ( ATT_HANDLE_VALUE_IND_T_flags , value );
}

uint16 ATT_HANDLE_VALUE_IND_T_PDU::get_size_value() const
{
    return get_uint16 ( ATT_HANDLE_VALUE_IND_T_size_value );
}

void ATT_HANDLE_VALUE_IND_T_PDU::set_size_value( uint16 value )
{
    set_uint16 ( ATT_HANDLE_VALUE_IND_T_size_value , value );
}

const uint8 *ATT_HANDLE_VALUE_IND_T_PDU::get_valuePtr() const
{
    return get_uint8ArrayPtr ( ATT_HANDLE_VALUE_IND_T_pduSize );
}

void ATT_HANDLE_VALUE_IND_T_PDU::set_value( const uint8 *value, uint16 len )
{
    set_final_uint8Ptr (ATT_HANDLE_VALUE_IND_T_size_value , ATT_HANDLE_VALUE_IND_T_pduSize , value , len );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: ATT_FIND_INFO_CFM_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

ATT_FIND_INFO_CFM_T_PDU::ATT_FIND_INFO_CFM_T_PDU()
: ATT_PDU ( ATT_FIND_INFO_CFM , ATT_FIND_INFO_CFM_T_pduSize )
{
}

ATT_FIND_INFO_CFM_T_PDU::ATT_FIND_INFO_CFM_T_PDU( const PDU& from )
: ATT_PDU ( from )
{
}

ATT_FIND_INFO_CFM_T_PDU::ATT_FIND_INFO_CFM_T_PDU( const uint8 * buffer , uint32 len )
: ATT_PDU ( buffer , len )
{
}

uint16 ATT_FIND_INFO_CFM_T_PDU::get_phandle() const
{
    return get_uint16 ( ATT_FIND_INFO_CFM_T_phandle );
}

void ATT_FIND_INFO_CFM_T_PDU::set_phandle( uint16 value )
{
    set_uint16 ( ATT_FIND_INFO_CFM_T_phandle , value );
}

uint16 ATT_FIND_INFO_CFM_T_PDU::get_cid() const
{
    return get_uint16 ( ATT_FIND_INFO_CFM_T_cid );
}

void ATT_FIND_INFO_CFM_T_PDU::set_cid( uint16 value )
{
    set_uint16 ( ATT_FIND_INFO_CFM_T_cid , value );
}

uint16 ATT_FIND_INFO_CFM_T_PDU::get_handle() const
{
    return get_uint16 ( ATT_FIND_INFO_CFM_T_handle );
}

void ATT_FIND_INFO_CFM_T_PDU::set_handle( uint16 value )
{
    set_uint16 ( ATT_FIND_INFO_CFM_T_handle , value );
}

uint16 ATT_FIND_INFO_CFM_T_PDU::get_uuid_type() const
{
    return get_uint16 ( ATT_FIND_INFO_CFM_T_uuid_type );
}

void ATT_FIND_INFO_CFM_T_PDU::set_uuid_type( uint16 value )
{
    set_uint16 ( ATT_FIND_INFO_CFM_T_uuid_type , value );
}

void ATT_FIND_INFO_CFM_T_PDU::get_uuid(uint32 *copyTo) const
{
    get_uint32Array ( copyTo, ATT_FIND_INFO_CFM_T_uuid, 4);
}

void ATT_FIND_INFO_CFM_T_PDU::set_uuid(const uint32 *copyFrom)
{
    set_uint32Array ( copyFrom, ATT_FIND_INFO_CFM_T_uuid, 4);
}

uint16 ATT_FIND_INFO_CFM_T_PDU::get_result() const
{
    return get_uint16 ( ATT_FIND_INFO_CFM_T_result );
}

void ATT_FIND_INFO_CFM_T_PDU::set_result( uint16 value )
{
    set_uint16 ( ATT_FIND_INFO_CFM_T_result , value );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: ATT_ADD_DB_REQ_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

ATT_ADD_DB_REQ_T_PDU::ATT_ADD_DB_REQ_T_PDU()
: ATT_PDU ( ATT_ADD_DB_REQ , ATT_ADD_DB_REQ_T_pduSize )
{
}

ATT_ADD_DB_REQ_T_PDU::ATT_ADD_DB_REQ_T_PDU( const PDU& from )
: ATT_PDU ( from )
{
}

ATT_ADD_DB_REQ_T_PDU::ATT_ADD_DB_REQ_T_PDU( const uint8 * buffer , uint32 len )
: ATT_PDU ( buffer , len )
{
}

uint16 ATT_ADD_DB_REQ_T_PDU::get_phandle() const
{
    return get_uint16 ( ATT_ADD_DB_REQ_T_phandle );
}

void ATT_ADD_DB_REQ_T_PDU::set_phandle( uint16 value )
{
    set_uint16 ( ATT_ADD_DB_REQ_T_phandle , value );
}

uint16 ATT_ADD_DB_REQ_T_PDU::get_size_db() const
{
    return get_uint16 ( ATT_ADD_DB_REQ_T_size_db );
}

void ATT_ADD_DB_REQ_T_PDU::set_size_db( uint16 value )
{
    set_uint16 ( ATT_ADD_DB_REQ_T_size_db , value );
}

const uint16 * ATT_ADD_DB_REQ_T_PDU::get_dbPtr() const
{
    return get_uint16ArrayPtr ( ATT_ADD_DB_REQ_T_pduSize );
}

void ATT_ADD_DB_REQ_T_PDU::set_db(const uint16 *value, uint16 len)
{
    resize ( ATT_ADD_DB_REQ_T_pduSize + len*2 );
    set_uint16 ( ATT_ADD_DB_REQ_T_size_db , len );
    set_uint16Array ( value, ATT_ADD_DB_REQ_T_pduSize, len );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: ATT_READ_BY_TYPE_REQ_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

ATT_READ_BY_TYPE_REQ_T_PDU::ATT_READ_BY_TYPE_REQ_T_PDU()
: ATT_PDU ( ATT_READ_BY_TYPE_REQ , ATT_READ_BY_TYPE_REQ_T_pduSize )
{
}

ATT_READ_BY_TYPE_REQ_T_PDU::ATT_READ_BY_TYPE_REQ_T_PDU( const PDU& from )
: ATT_PDU ( from )
{
}

ATT_READ_BY_TYPE_REQ_T_PDU::ATT_READ_BY_TYPE_REQ_T_PDU( const uint8 * buffer , uint32 len )
: ATT_PDU ( buffer , len )
{
}

uint16 ATT_READ_BY_TYPE_REQ_T_PDU::get_phandle() const
{
    return get_uint16 ( ATT_READ_BY_TYPE_REQ_T_phandle );
}

void ATT_READ_BY_TYPE_REQ_T_PDU::set_phandle( uint16 value )
{
    set_uint16 ( ATT_READ_BY_TYPE_REQ_T_phandle , value );
}

uint16 ATT_READ_BY_TYPE_REQ_T_PDU::get_cid() const
{
    return get_uint16 ( ATT_READ_BY_TYPE_REQ_T_cid );
}

void ATT_READ_BY_TYPE_REQ_T_PDU::set_cid( uint16 value )
{
    set_uint16 ( ATT_READ_BY_TYPE_REQ_T_cid , value );
}

uint16 ATT_READ_BY_TYPE_REQ_T_PDU::get_start() const
{
    return get_uint16 ( ATT_READ_BY_TYPE_REQ_T_start );
}

void ATT_READ_BY_TYPE_REQ_T_PDU::set_start( uint16 value )
{
    set_uint16 ( ATT_READ_BY_TYPE_REQ_T_start , value );
}

uint16 ATT_READ_BY_TYPE_REQ_T_PDU::get_end() const
{
    return get_uint16 ( ATT_READ_BY_TYPE_REQ_T_end );
}

void ATT_READ_BY_TYPE_REQ_T_PDU::set_end( uint16 value )
{
    set_uint16 ( ATT_READ_BY_TYPE_REQ_T_end , value );
}

uint16 ATT_READ_BY_TYPE_REQ_T_PDU::get_uuid_type() const
{
    return get_uint16 ( ATT_READ_BY_TYPE_REQ_T_uuid_type );
}

void ATT_READ_BY_TYPE_REQ_T_PDU::set_uuid_type( uint16 value )
{
    set_uint16 ( ATT_READ_BY_TYPE_REQ_T_uuid_type , value );
}

void ATT_READ_BY_TYPE_REQ_T_PDU::get_uuid(uint32 *copyTo) const
{
    get_uint32Array ( copyTo, ATT_READ_BY_TYPE_REQ_T_uuid, 4);
}

void ATT_READ_BY_TYPE_REQ_T_PDU::set_uuid(const uint32 *copyFrom)
{
    set_uint32Array ( copyFrom, ATT_READ_BY_TYPE_REQ_T_uuid, 4);
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: ATT_READ_BY_GROUP_TYPE_REQ_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

ATT_READ_BY_GROUP_TYPE_REQ_T_PDU::ATT_READ_BY_GROUP_TYPE_REQ_T_PDU()
: ATT_PDU ( ATT_READ_BY_GROUP_TYPE_REQ , ATT_READ_BY_GROUP_TYPE_REQ_T_pduSize )
{
}

ATT_READ_BY_GROUP_TYPE_REQ_T_PDU::ATT_READ_BY_GROUP_TYPE_REQ_T_PDU( const PDU& from )
: ATT_PDU ( from )
{
}

ATT_READ_BY_GROUP_TYPE_REQ_T_PDU::ATT_READ_BY_GROUP_TYPE_REQ_T_PDU( const uint8 * buffer , uint32 len )
: ATT_PDU ( buffer , len )
{
}

uint16 ATT_READ_BY_GROUP_TYPE_REQ_T_PDU::get_phandle() const
{
    return get_uint16 ( ATT_READ_BY_GROUP_TYPE_REQ_T_phandle );
}

void ATT_READ_BY_GROUP_TYPE_REQ_T_PDU::set_phandle( uint16 value )
{
    set_uint16 ( ATT_READ_BY_GROUP_TYPE_REQ_T_phandle , value );
}

uint16 ATT_READ_BY_GROUP_TYPE_REQ_T_PDU::get_cid() const
{
    return get_uint16 ( ATT_READ_BY_GROUP_TYPE_REQ_T_cid );
}

void ATT_READ_BY_GROUP_TYPE_REQ_T_PDU::set_cid( uint16 value )
{
    set_uint16 ( ATT_READ_BY_GROUP_TYPE_REQ_T_cid , value );
}

uint16 ATT_READ_BY_GROUP_TYPE_REQ_T_PDU::get_start() const
{
    return get_uint16 ( ATT_READ_BY_GROUP_TYPE_REQ_T_start );
}

void ATT_READ_BY_GROUP_TYPE_REQ_T_PDU::set_start( uint16 value )
{
    set_uint16 ( ATT_READ_BY_GROUP_TYPE_REQ_T_start , value );
}

uint16 ATT_READ_BY_GROUP_TYPE_REQ_T_PDU::get_end() const
{
    return get_uint16 ( ATT_READ_BY_GROUP_TYPE_REQ_T_end );
}

void ATT_READ_BY_GROUP_TYPE_REQ_T_PDU::set_end( uint16 value )
{
    set_uint16 ( ATT_READ_BY_GROUP_TYPE_REQ_T_end , value );
}

uint16 ATT_READ_BY_GROUP_TYPE_REQ_T_PDU::get_group_type() const
{
    return get_uint16 ( ATT_READ_BY_GROUP_TYPE_REQ_T_group_type );
}

void ATT_READ_BY_GROUP_TYPE_REQ_T_PDU::set_group_type( uint16 value )
{
    set_uint16 ( ATT_READ_BY_GROUP_TYPE_REQ_T_group_type , value );
}

void ATT_READ_BY_GROUP_TYPE_REQ_T_PDU::get_group(uint32 *copyTo) const
{
    get_uint32Array ( copyTo, ATT_READ_BY_GROUP_TYPE_REQ_T_group, 4);
}

void ATT_READ_BY_GROUP_TYPE_REQ_T_PDU::set_group(const uint32 *copyFrom)
{
    set_uint32Array ( copyFrom, ATT_READ_BY_GROUP_TYPE_REQ_T_group, 4);
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: ATT_DEBUG_IND_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

ATT_DEBUG_IND_T_PDU::ATT_DEBUG_IND_T_PDU()
: ATT_PDU ( ATT_DEBUG_IND , ATT_DEBUG_IND_T_pduSize )
{
}

ATT_DEBUG_IND_T_PDU::ATT_DEBUG_IND_T_PDU( const PDU& from )
: ATT_PDU ( from )
{
}

ATT_DEBUG_IND_T_PDU::ATT_DEBUG_IND_T_PDU( const uint8 * buffer , uint32 len )
: ATT_PDU ( buffer , len )
{
}

uint16 ATT_DEBUG_IND_T_PDU::get_size_debug() const
{
    return get_uint16 ( ATT_DEBUG_IND_T_size_debug );
}

void ATT_DEBUG_IND_T_PDU::set_size_debug( uint16 value )
{
    set_uint16 ( ATT_DEBUG_IND_T_size_debug , value );
}

const uint8 * ATT_DEBUG_IND_T_PDU::get_debugPtr() const
{
    return get_uint8ArrayPtr ( ATT_DEBUG_IND_T_pduSize );
}

void ATT_DEBUG_IND_T_PDU::set_debug( const uint8 *debug, uint16 len )
{
    set_final_uint8Ptr ( ATT_DEBUG_IND_T_size_debug , ATT_DEBUG_IND_T_pduSize , debug , len );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: ATT_READ_BLOB_CFM_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

ATT_READ_BLOB_CFM_T_PDU::ATT_READ_BLOB_CFM_T_PDU()
: ATT_PDU ( ATT_READ_BLOB_CFM , ATT_READ_BLOB_CFM_T_pduSize )
{
}

ATT_READ_BLOB_CFM_T_PDU::ATT_READ_BLOB_CFM_T_PDU( const PDU& from )
: ATT_PDU ( from )
{
}

ATT_READ_BLOB_CFM_T_PDU::ATT_READ_BLOB_CFM_T_PDU( const uint8 * buffer , uint32 len )
: ATT_PDU ( buffer , len )
{
}

uint16 ATT_READ_BLOB_CFM_T_PDU::get_phandle() const
{
    return get_uint16 ( ATT_READ_BLOB_CFM_T_phandle );
}

void ATT_READ_BLOB_CFM_T_PDU::set_phandle( uint16 value )
{
    set_uint16 ( ATT_READ_BLOB_CFM_T_phandle , value );
}

uint16 ATT_READ_BLOB_CFM_T_PDU::get_cid() const
{
    return get_uint16 ( ATT_READ_BLOB_CFM_T_cid );
}

void ATT_READ_BLOB_CFM_T_PDU::set_cid( uint16 value )
{
    set_uint16 ( ATT_READ_BLOB_CFM_T_cid , value );
}

uint16 ATT_READ_BLOB_CFM_T_PDU::get_result() const
{
    return get_uint16 ( ATT_READ_BLOB_CFM_T_result );
}

void ATT_READ_BLOB_CFM_T_PDU::set_result( uint16 value )
{
    set_uint16 ( ATT_READ_BLOB_CFM_T_result , value );
}

uint16 ATT_READ_BLOB_CFM_T_PDU::get_size_value() const
{
    return get_uint16 ( ATT_READ_BLOB_CFM_T_size_value );
}

void ATT_READ_BLOB_CFM_T_PDU::set_size_value( uint16 value )
{
    set_uint16 ( ATT_READ_BLOB_CFM_T_size_value , value );
}

const uint8 *ATT_READ_BLOB_CFM_T_PDU::get_valuePtr() const
{
    return get_uint8ArrayPtr ( ATT_READ_BLOB_CFM_T_pduSize );
}

void ATT_READ_BLOB_CFM_T_PDU::set_value( const uint8 *value, uint16 len )
{
    set_final_uint8Ptr (ATT_READ_BLOB_CFM_T_size_value , ATT_READ_BLOB_CFM_T_pduSize , value , len );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: ATT_READ_MULTI_REQ_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

ATT_READ_MULTI_REQ_T_PDU::ATT_READ_MULTI_REQ_T_PDU()
: ATT_PDU ( ATT_READ_MULTI_REQ , ATT_READ_MULTI_REQ_T_pduSize )
{
}

ATT_READ_MULTI_REQ_T_PDU::ATT_READ_MULTI_REQ_T_PDU( const PDU& from )
: ATT_PDU ( from )
{
}

ATT_READ_MULTI_REQ_T_PDU::ATT_READ_MULTI_REQ_T_PDU( const uint8 * buffer , uint32 len )
: ATT_PDU ( buffer , len )
{
}

uint16 ATT_READ_MULTI_REQ_T_PDU::get_phandle() const
{
    return get_uint16 ( ATT_READ_MULTI_REQ_T_phandle );
}

void ATT_READ_MULTI_REQ_T_PDU::set_phandle( uint16 value )
{
    set_uint16 ( ATT_READ_MULTI_REQ_T_phandle , value );
}

uint16 ATT_READ_MULTI_REQ_T_PDU::get_cid() const
{
    return get_uint16 ( ATT_READ_MULTI_REQ_T_cid );
}

void ATT_READ_MULTI_REQ_T_PDU::set_cid( uint16 value )
{
    set_uint16 ( ATT_READ_MULTI_REQ_T_cid , value );
}

uint16 ATT_READ_MULTI_REQ_T_PDU::get_size_handles() const
{
    return get_uint16 ( ATT_READ_MULTI_REQ_T_size_handles );
}

void ATT_READ_MULTI_REQ_T_PDU::set_size_handles( uint16 value )
{
    set_uint16 ( ATT_READ_MULTI_REQ_T_size_handles , value );
}

const uint16 *ATT_READ_MULTI_REQ_T_PDU::get_handlesPtr() const
{
    return get_uint16ArrayPtr ( ATT_READ_MULTI_REQ_T_pduSize );
}

void ATT_READ_MULTI_REQ_T_PDU::set_handles( const uint16 *value, uint16 len )
{
    resize ( ATT_READ_MULTI_REQ_T_pduSize + len*2 );
    set_uint16 ( ATT_READ_MULTI_REQ_T_size_handles , len );
    set_uint16Array ( value, ATT_READ_MULTI_REQ_T_pduSize, len );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: ATT_ACCESS_IND_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

ATT_ACCESS_IND_T_PDU::ATT_ACCESS_IND_T_PDU()
: ATT_PDU ( ATT_ACCESS_IND , ATT_ACCESS_IND_T_pduSize )
{
}

ATT_ACCESS_IND_T_PDU::ATT_ACCESS_IND_T_PDU( const PDU& from )
: ATT_PDU ( from )
{
}

ATT_ACCESS_IND_T_PDU::ATT_ACCESS_IND_T_PDU( const uint8 * buffer , uint32 len )
: ATT_PDU ( buffer , len )
{
}

uint16 ATT_ACCESS_IND_T_PDU::get_phandle() const
{
    return get_uint16 ( ATT_ACCESS_IND_T_phandle );
}

void ATT_ACCESS_IND_T_PDU::set_phandle( uint16 value )
{
    set_uint16 ( ATT_ACCESS_IND_T_phandle , value );
}

uint16 ATT_ACCESS_IND_T_PDU::get_cid() const
{
    return get_uint16 ( ATT_ACCESS_IND_T_cid );
}

void ATT_ACCESS_IND_T_PDU::set_cid( uint16 value )
{
    set_uint16 ( ATT_ACCESS_IND_T_cid , value );
}

uint16 ATT_ACCESS_IND_T_PDU::get_handle() const
{
    return get_uint16 ( ATT_ACCESS_IND_T_handle );
}

void ATT_ACCESS_IND_T_PDU::set_handle( uint16 value )
{
    set_uint16 ( ATT_ACCESS_IND_T_handle , value );
}

uint16 ATT_ACCESS_IND_T_PDU::get_flags() const
{
    return get_uint16 ( ATT_ACCESS_IND_T_flags );
}

void ATT_ACCESS_IND_T_PDU::set_flags( uint16 value )
{
    set_uint16 ( ATT_ACCESS_IND_T_flags , value );
}

uint16 ATT_ACCESS_IND_T_PDU::get_offset() const
{
    return get_uint16 ( ATT_ACCESS_IND_T_offset );
}

void ATT_ACCESS_IND_T_PDU::set_offset( uint16 value )
{
    set_uint16 ( ATT_ACCESS_IND_T_offset , value );
}

uint16 ATT_ACCESS_IND_T_PDU::get_size_value() const
{
    return get_uint16 ( ATT_ACCESS_IND_T_size_value );
}

void ATT_ACCESS_IND_T_PDU::set_size_value( uint16 value )
{
    set_uint16 ( ATT_ACCESS_IND_T_size_value , value );
}

const uint8 *ATT_ACCESS_IND_T_PDU::get_valuePtr() const
{
    return get_uint8ArrayPtr ( ATT_ACCESS_IND_T_pduSize );
}

void ATT_ACCESS_IND_T_PDU::set_value( const uint8 *value, uint16 len )
{
    set_final_uint8Ptr (ATT_ACCESS_IND_T_size_value , ATT_ACCESS_IND_T_pduSize , value , len);
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: ATT_ACCESS_RSP_T_PDU
//
///////////////////////////////////////////////////////////////////////////////

ATT_ACCESS_RSP_T_PDU::ATT_ACCESS_RSP_T_PDU()
: ATT_PDU ( ATT_ACCESS_RSP , ATT_ACCESS_RSP_T_pduSize )
{
}

ATT_ACCESS_RSP_T_PDU::ATT_ACCESS_RSP_T_PDU( const PDU& from )
: ATT_PDU ( from )
{
}

ATT_ACCESS_RSP_T_PDU::ATT_ACCESS_RSP_T_PDU( const uint8 * buffer , uint32 len )
: ATT_PDU ( buffer , len )
{
}

uint16 ATT_ACCESS_RSP_T_PDU::get_phandle() const
{
    return get_uint16 ( ATT_ACCESS_RSP_T_phandle );
}

void ATT_ACCESS_RSP_T_PDU::set_phandle( uint16 value )
{
    set_uint16 ( ATT_ACCESS_RSP_T_phandle , value );
}

uint16 ATT_ACCESS_RSP_T_PDU::get_cid() const
{
    return get_uint16 ( ATT_ACCESS_RSP_T_cid );
}

void ATT_ACCESS_RSP_T_PDU::set_cid( uint16 value )
{
    set_uint16 ( ATT_ACCESS_RSP_T_cid , value );
}

uint16 ATT_ACCESS_RSP_T_PDU::get_handle() const
{
    return get_uint16 ( ATT_ACCESS_RSP_T_handle );
}

void ATT_ACCESS_RSP_T_PDU::set_handle( uint16 value )
{
    set_uint16 ( ATT_ACCESS_RSP_T_handle , value );
}

uint16 ATT_ACCESS_RSP_T_PDU::get_result() const
{
    return get_uint16 ( ATT_ACCESS_RSP_T_result );
}

void ATT_ACCESS_RSP_T_PDU::set_result( uint16 value )
{
    set_uint16 ( ATT_ACCESS_RSP_T_result , value );
}

uint16 ATT_ACCESS_RSP_T_PDU::get_size_value() const
{
    return get_uint16 ( ATT_ACCESS_RSP_T_size_value );
}

void ATT_ACCESS_RSP_T_PDU::set_size_value( uint16 value )
{
    set_uint16 ( ATT_ACCESS_RSP_T_size_value , value );
}

const uint8 *ATT_ACCESS_RSP_T_PDU::get_valuePtr() const
{
    return get_uint8ArrayPtr ( ATT_ACCESS_RSP_T_pduSize );
}

void ATT_ACCESS_RSP_T_PDU::set_value( const uint8 *value, uint16 len )
{
    set_final_uint8Ptr (ATT_ACCESS_RSP_T_size_value , ATT_ACCESS_RSP_T_pduSize , value , len);
}


