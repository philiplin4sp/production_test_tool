////////////////////////////////////////////////////////////////////////////////
//
//  FILE:       xtrahqpdu.cpp
//
//  CLASS:      None
//
//  PURPOSE:    This file is included in CVS for completeness.
//
////////////////////////////////////////////////////////////////////////////////

#include "xtrahqpdu.h"
#include "bcflayout.h"

enum {
	OFFSET_START(HQ_FM_RDS_DATA)
	OFFSET_uint16(HQ_FM_RDS_DATA_req_type),
	OFFSET_uint16(HQ_FM_RDS_DATA_wordlength),
	OFFSET_uint16(HQ_FM_RDS_DATA_seq_no),
	OFFSET_uint16(HQ_FM_RDS_DATA_varid),
	OFFSET_uint16(HQ_FM_RDS_DATA_status),
	OFFSET_uint16(HQ_FM_RDS_DATA_rds_data_len),
	OFFSET_uint16(HQ_FM_RDS_DATA_rds_data),
	HQ_FM_RDS_DATA_pduSize
} ;

enum {
	OFFSET_START(HQ_FM_EVENT)
	OFFSET_uint16(HQ_FM_EVENT_req_type),
	OFFSET_uint16(HQ_FM_EVENT_wordlength),
	OFFSET_uint16(HQ_FM_EVENT_seq_no),
	OFFSET_uint16(HQ_FM_EVENT_varid),
	OFFSET_uint16(HQ_FM_EVENT_status),
	OFFSET_uint16(HQ_FM_EVENT_flags),
	OFFSET_uint16(HQ_FM_EVENT_reg_vals),
	OFFSET_uint16(HQ_FM_EVENT_reg_vals1),
	OFFSET_uint16(HQ_FM_EVENT_reg_vals2),
	OFFSET_uint16(HQ_FM_EVENT_reg_vals3),
	OFFSET_uint16(HQ_FM_EVENT_reg_vals4),
	OFFSET_uint16(HQ_FM_EVENT_reg_vals5),
	HQ_FM_EVENT_pduSize
} ;
 
enum {
	OFFSET_START(HQ_FMTX_EVENT)
	OFFSET_uint16(HQ_FMTX_EVENT_req_type),
	OFFSET_uint16(HQ_FMTX_EVENT_wordlength),
	OFFSET_uint16(HQ_FMTX_EVENT_seq_no),
	OFFSET_uint16(HQ_FMTX_EVENT_varid),
	OFFSET_uint16(HQ_FMTX_EVENT_status),
	OFFSET_uint16(HQ_FMTX_EVENT_flags),
	OFFSET_uint16(HQ_FMTX_EVENT_reg_vals),
	OFFSET_uint16(HQ_FMTX_EVENT_reg_vals1),
	OFFSET_uint16(HQ_FMTX_EVENT_reg_vals2),
	OFFSET_uint16(HQ_FMTX_EVENT_reg_vals3),
	OFFSET_uint16(HQ_FMTX_EVENT_reg_vals4),
	OFFSET_uint16(HQ_FMTX_EVENT_reg_vals5),
	HQ_FMTX_EVENT_pduSize
} ;

enum {
	OFFSET_START(HQ_COEX_UART_MESSAGE)
	OFFSET_uint16(HQ_COEX_UART_MESSAGE_req_type),
	OFFSET_uint16(HQ_COEX_UART_MESSAGE_wordlength),
	OFFSET_uint16(HQ_COEX_UART_MESSAGE_seq_no),
	OFFSET_uint16(HQ_COEX_UART_MESSAGE_varid),
	OFFSET_uint16(HQ_COEX_UART_MESSAGE_status),
        /* Our data */
	OFFSET_uint16(HQ_COEX_UART_MESSAGE_info),
	OFFSET_uint16(HQ_COEX_UART_MESSAGE_info1),
	OFFSET_uint16(HQ_COEX_UART_MESSAGE_info2),
	OFFSET_uint16(HQ_COEX_UART_MESSAGE_info3),
	OFFSET_uint16(HQ_COEX_UART_MESSAGE_info4),
	OFFSET_uint16(HQ_COEX_UART_MESSAGE_info5),
	OFFSET_uint16(HQ_COEX_UART_MESSAGE_info6),
	OFFSET_uint16(HQ_COEX_UART_MESSAGE_info7),
	HQ_COEX_UART_MESSAGE_pduSize
} ;

enum {
	OFFSET_START(HQ_DSPMANAGER_DEBUG_INFO_B)
	OFFSET_uint16(HQ_DSPMANAGER_DEBUG_INFO_B_req_type),
	OFFSET_uint16(HQ_DSPMANAGER_DEBUG_INFO_B_wordlength),
	OFFSET_uint16(HQ_DSPMANAGER_DEBUG_INFO_B_seq_no),
	OFFSET_uint16(HQ_DSPMANAGER_DEBUG_INFO_B_varid),
	OFFSET_uint16(HQ_DSPMANAGER_DEBUG_INFO_B_status),
	OFFSET_uint32(HQ_DSPMANAGER_DEBUG_INFO_B_time_us),
	OFFSET_uint16(HQ_DSPMANAGER_DEBUG_INFO_B_msgid),
	OFFSET_uint16(HQ_DSPMANAGER_DEBUG_INFO_B_msglen),
	OFFSET_uint16(HQ_DSPMANAGER_DEBUG_INFO_B_msg),
	HQ_DSPMANAGER_DEBUG_INFO_B_pduSize
} ;

enum {
	OFFSET_START(HQ_MESSAGE_FROM_OPERATOR)
	OFFSET_uint16(HQ_MESSAGE_FROM_OPERATOR_req_type),
	OFFSET_uint16(HQ_MESSAGE_FROM_OPERATOR_wordlength),
	OFFSET_uint16(HQ_MESSAGE_FROM_OPERATOR_seq_no),
	OFFSET_uint16(HQ_MESSAGE_FROM_OPERATOR_varid),
	OFFSET_uint16(HQ_MESSAGE_FROM_OPERATOR_status),
	OFFSET_uint16(HQ_MESSAGE_FROM_OPERATOR_opid),
	OFFSET_uint16(HQ_MESSAGE_FROM_OPERATOR_message),
	HQ_MESSAGE_FROM_OPERATOR_pduSize
} ;

///////////////////////////////////////////////////////////////////////////////
//
//  Class: HQ_FM_RDS_DATA_PDU
//
///////////////////////////////////////////////////////////////////////////////

HQ_FM_RDS_DATA_PDU::HQ_FM_RDS_DATA_PDU()
: HQ_PDU ( HQVARID_FM_RDS_DATA , HQ_FM_RDS_DATA_pduSize )
{
}

HQ_FM_RDS_DATA_PDU::HQ_FM_RDS_DATA_PDU( const PDU& from )
: HQ_PDU ( from )
{
}

HQ_FM_RDS_DATA_PDU::HQ_FM_RDS_DATA_PDU( const uint8 * buffer , uint32 len )
: HQ_PDU ( buffer , len )
{
}

uint16 HQ_FM_RDS_DATA_PDU::get_req_type() const
{
    return get_uint16 ( HQ_FM_RDS_DATA_req_type );
}

void HQ_FM_RDS_DATA_PDU::set_req_type( uint16 value )
{
    set_uint16 ( HQ_FM_RDS_DATA_req_type , value );
}

uint16 HQ_FM_RDS_DATA_PDU::get_wordlength() const
{
    return get_uint16 ( HQ_FM_RDS_DATA_wordlength );
}

void HQ_FM_RDS_DATA_PDU::set_wordlength( uint16 value )
{
    set_uint16 ( HQ_FM_RDS_DATA_wordlength , value );
}

uint16 HQ_FM_RDS_DATA_PDU::get_seq_no() const
{
    return get_uint16 ( HQ_FM_RDS_DATA_seq_no );
}

void HQ_FM_RDS_DATA_PDU::set_seq_no( uint16 value )
{
    set_uint16 ( HQ_FM_RDS_DATA_seq_no , value );
}

uint16 HQ_FM_RDS_DATA_PDU::get_varid() const
{
    return get_uint16 ( HQ_FM_RDS_DATA_varid );
}

void HQ_FM_RDS_DATA_PDU::set_varid( uint16 value )
{
    set_uint16 ( HQ_FM_RDS_DATA_varid , value );
}

uint16 HQ_FM_RDS_DATA_PDU::get_status() const
{
    return get_uint16 ( HQ_FM_RDS_DATA_status );
}

void HQ_FM_RDS_DATA_PDU::set_status( uint16 value )
{
    set_uint16 ( HQ_FM_RDS_DATA_status , value );
}

uint16 HQ_FM_RDS_DATA_PDU::get_rds_data_len() const
{
    return get_uint16 ( HQ_FM_RDS_DATA_rds_data_len );
}

const int HQ_FM_RDS_DATA_PDU::words_per_block = 2;
const int HQ_FM_RDS_DATA_PDU::bytes_per_block = 4;

void HQ_FM_RDS_DATA_PDU::set_rds_data_len( uint16 value )
{
    set_uint16 ( HQ_FM_RDS_DATA_rds_data_len , value );
    resize(value*bytes_per_block + HQ_FM_RDS_DATA_rds_data);
    set_length();
}

void HQ_FM_RDS_DATA_PDU::get_rds_data( uint16 * copyTo , uint16 len ) const
{
    get_uint16Array ( copyTo , HQ_FM_RDS_DATA_rds_data , len );
}


void HQ_FM_RDS_DATA_PDU::set_rds_data( const uint16 * copyFrom , uint16 len )
{
    set_uint16Array ( copyFrom , HQ_FM_RDS_DATA_rds_data , len );
    set_rds_data_len(len / words_per_block);
    set_length();
}






///////////////////////////////////////////////////////////////////////////////
//
//  Class: HQ_FM_EVENT_PDU
//
///////////////////////////////////////////////////////////////////////////////

HQ_FM_EVENT_PDU::HQ_FM_EVENT_PDU()
: HQ_PDU ( HQVARID_FM_EVENT , HQ_FM_EVENT_pduSize )
{
}

HQ_FM_EVENT_PDU::HQ_FM_EVENT_PDU( const PDU& from )
: HQ_PDU ( from )
{
}

HQ_FM_EVENT_PDU::HQ_FM_EVENT_PDU( const uint8 * buffer , uint32 len )
: HQ_PDU ( buffer , len )
{
}

uint16 HQ_FM_EVENT_PDU::get_req_type() const
{
    return get_uint16 ( HQ_FM_EVENT_req_type );
}

void HQ_FM_EVENT_PDU::set_req_type( uint16 value )
{
    set_uint16 ( HQ_FM_EVENT_req_type , value );
}

uint16 HQ_FM_EVENT_PDU::get_wordlength() const
{
    return get_uint16 ( HQ_FM_EVENT_wordlength );
}

void HQ_FM_EVENT_PDU::set_wordlength( uint16 value )
{
    set_uint16 ( HQ_FM_EVENT_wordlength , value );
}

uint16 HQ_FM_EVENT_PDU::get_seq_no() const
{
    return get_uint16 ( HQ_FM_EVENT_seq_no );
}

void HQ_FM_EVENT_PDU::set_seq_no( uint16 value )
{
    set_uint16 ( HQ_FM_EVENT_seq_no , value );
}

uint16 HQ_FM_EVENT_PDU::get_varid() const
{
    return get_uint16 ( HQ_FM_EVENT_varid );
}

void HQ_FM_EVENT_PDU::set_varid( uint16 value )
{
    set_uint16 ( HQ_FM_EVENT_varid , value );
}

uint16 HQ_FM_EVENT_PDU::get_status() const
{
    return get_uint16 ( HQ_FM_EVENT_status );
}

void HQ_FM_EVENT_PDU::set_status( uint16 value )
{
    set_uint16 ( HQ_FM_EVENT_status , value );
}

uint16 HQ_FM_EVENT_PDU::get_flags() const
{
    return get_uint16 ( HQ_FM_EVENT_flags );
}

void HQ_FM_EVENT_PDU::set_flags( uint16 value )
{
    set_uint16 ( HQ_FM_EVENT_flags , value );
}

void HQ_FM_EVENT_PDU::get_reg_vals( uint16 * copyTo ) const
{
    get_uint16Array ( copyTo , HQ_FM_EVENT_reg_vals , 6 );
}

void HQ_FM_EVENT_PDU::set_reg_vals( const uint16 * copyFrom )
{
    set_uint16Array ( copyFrom , HQ_FM_EVENT_reg_vals , 6 );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: HQ_FMTX_EVENT_PDU
//
///////////////////////////////////////////////////////////////////////////////

HQ_FMTX_EVENT_PDU::HQ_FMTX_EVENT_PDU()
: HQ_PDU ( HQVARID_FMTX_EVENT , HQ_FMTX_EVENT_pduSize )
{
}

HQ_FMTX_EVENT_PDU::HQ_FMTX_EVENT_PDU( const PDU& from )
: HQ_PDU ( from )
{
}

HQ_FMTX_EVENT_PDU::HQ_FMTX_EVENT_PDU( const uint8 * buffer , uint32 len )
: HQ_PDU ( buffer , len )
{
}

uint16 HQ_FMTX_EVENT_PDU::get_req_type() const
{
    return get_uint16 ( HQ_FMTX_EVENT_req_type );
}

void HQ_FMTX_EVENT_PDU::set_req_type( uint16 value )
{
    set_uint16 ( HQ_FMTX_EVENT_req_type , value );
}

uint16 HQ_FMTX_EVENT_PDU::get_wordlength() const
{
    return get_uint16 ( HQ_FMTX_EVENT_wordlength );
}

void HQ_FMTX_EVENT_PDU::set_wordlength( uint16 value )
{
    set_uint16 ( HQ_FMTX_EVENT_wordlength , value );
}

uint16 HQ_FMTX_EVENT_PDU::get_seq_no() const
{
    return get_uint16 ( HQ_FMTX_EVENT_seq_no );
}

void HQ_FMTX_EVENT_PDU::set_seq_no( uint16 value )
{
    set_uint16 ( HQ_FMTX_EVENT_seq_no , value );
}

uint16 HQ_FMTX_EVENT_PDU::get_varid() const
{
    return get_uint16 ( HQ_FMTX_EVENT_varid );
}

void HQ_FMTX_EVENT_PDU::set_varid( uint16 value )
{
    set_uint16 ( HQ_FMTX_EVENT_varid , value );
}

uint16 HQ_FMTX_EVENT_PDU::get_status() const
{
    return get_uint16 ( HQ_FMTX_EVENT_status );
}

void HQ_FMTX_EVENT_PDU::set_status( uint16 value )
{
    set_uint16 ( HQ_FMTX_EVENT_status , value );
}

uint16 HQ_FMTX_EVENT_PDU::get_flags() const
{
    return get_uint16 ( HQ_FMTX_EVENT_flags );
}

void HQ_FMTX_EVENT_PDU::set_flags( uint16 value )
{
    set_uint16 ( HQ_FMTX_EVENT_flags , value );
}

void HQ_FMTX_EVENT_PDU::get_reg_vals( uint16 * copyTo ) const
{
    get_uint16Array ( copyTo , HQ_FMTX_EVENT_reg_vals , 6 );
}

void HQ_FMTX_EVENT_PDU::set_reg_vals( const uint16 * copyFrom )
{
    set_uint16Array ( copyFrom , HQ_FMTX_EVENT_reg_vals , 6 );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: HQ_COEX_UART_MESSAGE_PDU
//
///////////////////////////////////////////////////////////////////////////////

HQ_COEX_UART_MESSAGE_PDU::HQ_COEX_UART_MESSAGE_PDU()
: HQ_PDU ( HQVARID_COEX_UART_MESSAGE , HQ_COEX_UART_MESSAGE_pduSize )
{
}

HQ_COEX_UART_MESSAGE_PDU::HQ_COEX_UART_MESSAGE_PDU( const PDU& from )
: HQ_PDU ( from )
{
}

HQ_COEX_UART_MESSAGE_PDU::HQ_COEX_UART_MESSAGE_PDU( const uint8 * buffer , uint32 len )
: HQ_PDU ( buffer , len )
{
}

uint16 HQ_COEX_UART_MESSAGE_PDU::get_req_type() const
{
    return get_uint16 ( HQ_COEX_UART_MESSAGE_req_type );
}

void HQ_COEX_UART_MESSAGE_PDU::set_req_type( uint16 value )
{
    set_uint16 ( HQ_COEX_UART_MESSAGE_req_type , value );
}

uint16 HQ_COEX_UART_MESSAGE_PDU::get_wordlength() const
{
    return get_uint16 ( HQ_COEX_UART_MESSAGE_wordlength );
}

void HQ_COEX_UART_MESSAGE_PDU::set_wordlength( uint16 value )
{
    set_uint16 ( HQ_COEX_UART_MESSAGE_wordlength , value );
}

uint16 HQ_COEX_UART_MESSAGE_PDU::get_seq_no() const
{
    return get_uint16 ( HQ_COEX_UART_MESSAGE_seq_no );
}

void HQ_COEX_UART_MESSAGE_PDU::set_seq_no( uint16 value )
{
    set_uint16 ( HQ_COEX_UART_MESSAGE_seq_no , value );
}

uint16 HQ_COEX_UART_MESSAGE_PDU::get_varid() const
{
    return get_uint16 ( HQ_COEX_UART_MESSAGE_varid );
}

void HQ_COEX_UART_MESSAGE_PDU::set_varid( uint16 value )
{
    set_uint16 ( HQ_COEX_UART_MESSAGE_varid , value );
}

uint16 HQ_COEX_UART_MESSAGE_PDU::get_status() const
{
    return get_uint16 ( HQ_COEX_UART_MESSAGE_status );
}

void HQ_COEX_UART_MESSAGE_PDU::set_status( uint16 value )
{
    set_uint16 ( HQ_COEX_UART_MESSAGE_status , value );
}

void HQ_COEX_UART_MESSAGE_PDU::get_info( uint16 * copyTo ) const
{
    get_uint16Array ( copyTo , HQ_COEX_UART_MESSAGE_info , 8 );
}

void HQ_COEX_UART_MESSAGE_PDU::set_info( const uint16 * copyFrom )
{
    set_uint16Array ( copyFrom , HQ_COEX_UART_MESSAGE_info , 8 );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: HQ_DSPMANAGER_DEBUG_INFO_B_PDU
//
///////////////////////////////////////////////////////////////////////////////

HQ_DSPMANAGER_DEBUG_INFO_B_PDU::HQ_DSPMANAGER_DEBUG_INFO_B_PDU()
: HQ_PDU ( HQVARID_DSPMANAGER_DEBUG_INFO_B , HQ_DSPMANAGER_DEBUG_INFO_B_pduSize )
{
}

HQ_DSPMANAGER_DEBUG_INFO_B_PDU::HQ_DSPMANAGER_DEBUG_INFO_B_PDU( const PDU& from )
: HQ_PDU ( from )
{
}

HQ_DSPMANAGER_DEBUG_INFO_B_PDU::HQ_DSPMANAGER_DEBUG_INFO_B_PDU( const uint8 * buffer , uint32 len )
: HQ_PDU ( buffer , len )
{
}

uint16 HQ_DSPMANAGER_DEBUG_INFO_B_PDU::get_req_type() const
{
    return get_uint16 ( HQ_DSPMANAGER_DEBUG_INFO_B_req_type );
}

void HQ_DSPMANAGER_DEBUG_INFO_B_PDU::set_req_type( uint16 value )
{
    set_uint16 ( HQ_DSPMANAGER_DEBUG_INFO_B_req_type , value );
}

uint16 HQ_DSPMANAGER_DEBUG_INFO_B_PDU::get_wordlength() const
{
    return get_uint16 ( HQ_DSPMANAGER_DEBUG_INFO_B_wordlength );
}

void HQ_DSPMANAGER_DEBUG_INFO_B_PDU::set_wordlength( uint16 value )
{
    set_uint16 ( HQ_DSPMANAGER_DEBUG_INFO_B_wordlength , value );
}

uint16 HQ_DSPMANAGER_DEBUG_INFO_B_PDU::get_seq_no() const
{
    return get_uint16 ( HQ_DSPMANAGER_DEBUG_INFO_B_seq_no );
}

void HQ_DSPMANAGER_DEBUG_INFO_B_PDU::set_seq_no( uint16 value )
{
    set_uint16 ( HQ_DSPMANAGER_DEBUG_INFO_B_seq_no , value );
}

uint16 HQ_DSPMANAGER_DEBUG_INFO_B_PDU::get_varid() const
{
    return get_uint16 ( HQ_DSPMANAGER_DEBUG_INFO_B_varid );
}

void HQ_DSPMANAGER_DEBUG_INFO_B_PDU::set_varid( uint16 value )
{
    set_uint16 ( HQ_DSPMANAGER_DEBUG_INFO_B_varid , value );
}

uint16 HQ_DSPMANAGER_DEBUG_INFO_B_PDU::get_status() const
{
    return get_uint16 ( HQ_DSPMANAGER_DEBUG_INFO_B_status );
}

void HQ_DSPMANAGER_DEBUG_INFO_B_PDU::set_status( uint16 value )
{
    set_uint16 ( HQ_DSPMANAGER_DEBUG_INFO_B_status , value );
}

uint32 HQ_DSPMANAGER_DEBUG_INFO_B_PDU::get_time_us() const
{
    return get_uint32 ( HQ_DSPMANAGER_DEBUG_INFO_B_time_us );
}

void HQ_DSPMANAGER_DEBUG_INFO_B_PDU::set_time_us( uint32 value )
{
    set_uint32 ( HQ_DSPMANAGER_DEBUG_INFO_B_time_us , value );
}

uint16 HQ_DSPMANAGER_DEBUG_INFO_B_PDU::get_msgid() const
{
    return get_uint16 ( HQ_DSPMANAGER_DEBUG_INFO_B_msgid );
}

void HQ_DSPMANAGER_DEBUG_INFO_B_PDU::set_msgid( uint16 value )
{
    set_uint16 ( HQ_DSPMANAGER_DEBUG_INFO_B_msgid , value );
}

uint16 HQ_DSPMANAGER_DEBUG_INFO_B_PDU::get_msglen() const
{
    return get_uint16 ( HQ_DSPMANAGER_DEBUG_INFO_B_msglen );
}

void HQ_DSPMANAGER_DEBUG_INFO_B_PDU::set_msglen( uint16 value )
{
    set_uint16 ( HQ_DSPMANAGER_DEBUG_INFO_B_msglen , value );
    resize(value*sizeof(uint16) + HQ_DSPMANAGER_DEBUG_INFO_B_msg);
    set_length();
}

void HQ_DSPMANAGER_DEBUG_INFO_B_PDU::get_msg( uint16 * copyTo , uint16 len ) const
{
    get_uint16Array ( copyTo , HQ_DSPMANAGER_DEBUG_INFO_B_msg , len );
}


void HQ_DSPMANAGER_DEBUG_INFO_B_PDU::set_msg( const uint16 * copyFrom , uint16 len )
{
    set_uint16Array ( copyFrom , HQ_DSPMANAGER_DEBUG_INFO_B_msg , len );
    set_msglen(len);
    set_length();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: HQ_MESSAGE_FROM_OPERATOR_PDU
//
///////////////////////////////////////////////////////////////////////////////

HQ_MESSAGE_FROM_OPERATOR_PDU::HQ_MESSAGE_FROM_OPERATOR_PDU()
: HQ_PDU ( HQVARID_MESSAGE_FROM_OPERATOR , HQ_MESSAGE_FROM_OPERATOR_pduSize )
{
}

HQ_MESSAGE_FROM_OPERATOR_PDU::HQ_MESSAGE_FROM_OPERATOR_PDU( const PDU& from )
: HQ_PDU ( from )
{
}

HQ_MESSAGE_FROM_OPERATOR_PDU::HQ_MESSAGE_FROM_OPERATOR_PDU( const uint8 * buffer , uint32 len )
: HQ_PDU ( buffer , len )
{
}

uint16 HQ_MESSAGE_FROM_OPERATOR_PDU::get_req_type() const
{
    return get_uint16 ( HQ_MESSAGE_FROM_OPERATOR_req_type );
}

void HQ_MESSAGE_FROM_OPERATOR_PDU::set_req_type( uint16 value )
{
    set_uint16 ( HQ_MESSAGE_FROM_OPERATOR_req_type , value );
}

uint16 HQ_MESSAGE_FROM_OPERATOR_PDU::get_wordlength() const
{
    return get_uint16 ( HQ_MESSAGE_FROM_OPERATOR_wordlength );
}

void HQ_MESSAGE_FROM_OPERATOR_PDU::set_wordlength( uint16 value )
{
    set_uint16 ( HQ_MESSAGE_FROM_OPERATOR_wordlength , value );
}

uint16 HQ_MESSAGE_FROM_OPERATOR_PDU::get_seq_no() const
{
    return get_uint16 ( HQ_MESSAGE_FROM_OPERATOR_seq_no );
}

void HQ_MESSAGE_FROM_OPERATOR_PDU::set_seq_no( uint16 value )
{
    set_uint16 ( HQ_MESSAGE_FROM_OPERATOR_seq_no , value );
}

uint16 HQ_MESSAGE_FROM_OPERATOR_PDU::get_varid() const
{
    return get_uint16 ( HQ_MESSAGE_FROM_OPERATOR_varid );
}

void HQ_MESSAGE_FROM_OPERATOR_PDU::set_varid( uint16 value )
{
    set_uint16 ( HQ_MESSAGE_FROM_OPERATOR_varid , value );
}

uint16 HQ_MESSAGE_FROM_OPERATOR_PDU::get_status() const
{
    return get_uint16 ( HQ_MESSAGE_FROM_OPERATOR_status );
}

void HQ_MESSAGE_FROM_OPERATOR_PDU::set_status( uint16 value )
{
    set_uint16 ( HQ_MESSAGE_FROM_OPERATOR_status , value );
}

uint16 HQ_MESSAGE_FROM_OPERATOR_PDU::get_opid() const
{
    return get_uint16 ( HQ_MESSAGE_FROM_OPERATOR_opid );
}

void HQ_MESSAGE_FROM_OPERATOR_PDU::set_opid( uint16 value )
{
    set_uint16 ( HQ_MESSAGE_FROM_OPERATOR_opid , value );
}

uint16 HQ_MESSAGE_FROM_OPERATOR_PDU::get_msglen() const
{
    return get_wordlength() - 6;
}

void HQ_MESSAGE_FROM_OPERATOR_PDU::set_msglen( uint16 value )
{
    resize(value*sizeof(uint16) + HQ_MESSAGE_FROM_OPERATOR_message);
    set_length();
}

void HQ_MESSAGE_FROM_OPERATOR_PDU::get_message( uint16 * copyTo , uint16 len ) const
{
    get_uint16Array ( copyTo , HQ_MESSAGE_FROM_OPERATOR_message , len );
}


void HQ_MESSAGE_FROM_OPERATOR_PDU::set_message( const uint16 * copyFrom , uint16 len )
{
    set_uint16Array ( copyFrom , HQ_MESSAGE_FROM_OPERATOR_message , len );
    set_msglen(len);
    set_length();
}
