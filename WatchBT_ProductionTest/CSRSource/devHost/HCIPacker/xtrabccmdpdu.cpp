////////////////////////////////////////////////////////////////////////////////
//
//  FILE:       xtrabccmdpdu.cpp
//
//  CLASS:      BCCMD_PS_PDU,
//              BCCMD_MEM_PDU,
//              BCCMD_BUF_PDU,
//              BCCMD_E2_APP_DATA_PDU
//
//  PURPOSE:    the autogenerator is too dumb to cope with arrays, so
//              this file sorts them out with some hand written code.
//
////////////////////////////////////////////////////////////////////////////////

#include "xtrabccmdpdu.h"
#include "bcflayout.h"

enum {
	OFFSET_START(BCCMD_MEMORY)
	OFFSET_uint16(BCCMD_MEMORY_req_type),
	OFFSET_uint16(BCCMD_MEMORY_length),
	OFFSET_uint16(BCCMD_MEMORY_seq_no),
	OFFSET_uint16(BCCMD_MEMORY_varid),
	OFFSET_uint16(BCCMD_MEMORY_status),
	OFFSET_uint16(BCCMD_MEMORY_base_addr),
	OFFSET_uint16(BCCMD_MEMORY_len),
	OFFSET_uint16(BCCMD_MEMORY_dummy),
	OFFSET_uint16(BCCMD_MEMORY_mem),
	BCCMD_MEMORY_pduSize
} ;

enum {
	OFFSET_START(BCCMD_RANDOM)
	OFFSET_uint16(BCCMD_RANDOM_req_type),
	OFFSET_uint16(BCCMD_RANDOM_length),
	OFFSET_uint16(BCCMD_RANDOM_seq_no),
	OFFSET_uint16(BCCMD_RANDOM_varid),
	OFFSET_uint16(BCCMD_RANDOM_status),
	OFFSET_uint16(BCCMD_RANDOM_len),
	OFFSET_uint16(BCCMD_RANDOM_type),
	OFFSET_uint16(BCCMD_RANDOM_dummy),
	OFFSET_uint16(BCCMD_RANDOM_data),
	BCCMD_RANDOM_pduSize
} ;

enum {
	OFFSET_START(BCCMD_PS)
	OFFSET_uint16(BCCMD_PS_req_type),
	OFFSET_uint16(BCCMD_PS_length),
	OFFSET_uint16(BCCMD_PS_seq_no),
	OFFSET_uint16(BCCMD_PS_varid),
	OFFSET_uint16(BCCMD_PS_status),
	OFFSET_uint16(BCCMD_PS_id),
	OFFSET_uint16(BCCMD_PS_len),
	OFFSET_uint16(BCCMD_PS_stores),
	OFFSET_uint16(BCCMD_PS_psmem),
	BCCMD_PS_pduSize
} ;

enum {
	OFFSET_START(BCCMD_BUFFER)
	OFFSET_uint16(BCCMD_BUFFER_req_type),
	OFFSET_uint16(BCCMD_BUFFER_length),
	OFFSET_uint16(BCCMD_BUFFER_seq_no),
	OFFSET_uint16(BCCMD_BUFFER_varid),
	OFFSET_uint16(BCCMD_BUFFER_status),
	OFFSET_uint16(BCCMD_BUFFER_handle),
	OFFSET_uint16(BCCMD_BUFFER_start),
	OFFSET_uint16(BCCMD_BUFFER_len),
	OFFSET_uint8(BCCMD_BUFFER_bufmem),
	BCCMD_BUFFER_pduSize
} ;

enum {
	OFFSET_START(BCCMD_E2_APP_DATA)
	OFFSET_uint16(BCCMD_E2_APP_DATA_req_type),
	OFFSET_uint16(BCCMD_E2_APP_DATA_length),
	OFFSET_uint16(BCCMD_E2_APP_DATA_seq_no),
	OFFSET_uint16(BCCMD_E2_APP_DATA_varid),
	OFFSET_uint16(BCCMD_E2_APP_DATA_status),
	OFFSET_uint16(BCCMD_E2_APP_DATA_offset),
	OFFSET_uint16(BCCMD_E2_APP_DATA_words),
	OFFSET_uint16(BCCMD_E2_APP_DATA_dummy),
	OFFSET_uint16(BCCMD_E2_APP_DATA_data),
	BCCMD_E2_APP_DATA_pduSize
} ;

//  this one has "PDU" in it, because I was feeling lazy.
enum {
	OFFSET_START(BCCMD_I2C_TRANSFER_PDU)
	OFFSET_uint16(BCCMD_I2C_TRANSFER_PDU_req_type),
	OFFSET_uint16(BCCMD_I2C_TRANSFER_PDU_length),
	OFFSET_uint16(BCCMD_I2C_TRANSFER_PDU_seq_no),
	OFFSET_uint16(BCCMD_I2C_TRANSFER_PDU_varid),
	OFFSET_uint16(BCCMD_I2C_TRANSFER_PDU_status),
	OFFSET_uint16(BCCMD_I2C_TRANSFER_PDU_address),
	OFFSET_uint16(BCCMD_I2C_TRANSFER_PDU_tx_octets),
	OFFSET_uint16(BCCMD_I2C_TRANSFER_PDU_rx_octets),
	OFFSET_uint16(BCCMD_I2C_TRANSFER_PDU_restart),
	OFFSET_uint16(BCCMD_I2C_TRANSFER_PDU_octets),
	OFFSET_uint16(BCCMD_I2C_TRANSFER_PDU_data),
} ;

enum {
	OFFSET_START(BCCMD_BUILD_NAME)
	OFFSET_uint16(BCCMD_BUILD_NAME_req_type),
	OFFSET_uint16(BCCMD_BUILD_NAME_length),
	OFFSET_uint16(BCCMD_BUILD_NAME_seq_no),
	OFFSET_uint16(BCCMD_BUILD_NAME_varid),
	OFFSET_uint16(BCCMD_BUILD_NAME_status),
	OFFSET_uint16(BCCMD_BUILD_NAME_start),
	OFFSET_uint16(BCCMD_BUILD_NAME_len),
	OFFSET_uint8(BCCMD_BUILD_NAME_bufmem),
	BCCMD_BUILD_NAME_pduSize
} ;


enum {
	OFFSET_START(BCCMD_LC_RX_FRACS)
	OFFSET_uint16(BCCMD_LC_RX_FRACS_req_type),
	OFFSET_uint16(BCCMD_LC_RX_FRACS_length),
	OFFSET_uint16(BCCMD_LC_RX_FRACS_seq_no),
	OFFSET_uint16(BCCMD_LC_RX_FRACS_varid),
	OFFSET_uint16(BCCMD_LC_RX_FRACS_status),
	OFFSET_uint16(BCCMD_LC_RX_FRACS_value_count),
	OFFSET_uint16(BCCMD_LC_RX_FRACS_rx_frac_vals),
	BCCMD_LC_RX_FRACS_pduSize
} ;


enum {
	OFFSET_START(BCCMD_FM_RDS)
	OFFSET_uint16(BCCMD_FM_RDS_req_type),
	OFFSET_uint16(BCCMD_FM_RDS_wordlength),
	OFFSET_uint16(BCCMD_FM_RDS_seq_no),
	OFFSET_uint16(BCCMD_FM_RDS_varid),
	OFFSET_uint16(BCCMD_FM_RDS_status),
	OFFSET_uint16(BCCMD_FM_RDS_rds_len),
	OFFSET_uint16(BCCMD_FM_RDS_rds_data),
	BCCMD_FM_RDS_pduSize
} ;

enum {
	OFFSET_START(BCCMD_FMTX_TEXT)
	OFFSET_uint16(BCCMD_FMTX_TEXT_req_type),
	OFFSET_uint16(BCCMD_FMTX_TEXT_wordlength),
	OFFSET_uint16(BCCMD_FMTX_TEXT_seq_no),
	OFFSET_uint16(BCCMD_FMTX_TEXT_varid),
	OFFSET_uint16(BCCMD_FMTX_TEXT_status),
	OFFSET_uint16(BCCMD_FMTX_TEXT_text_type),
	OFFSET_uint16(BCCMD_FMTX_TEXT_string),
	BCCMD_FMTX_TEXT_pduSize
} ;

enum {
	OFFSET_START(BCCMD_SHA256_DATA)
	OFFSET_uint16(BCCMD_SHA256_DATA_req_type),
	OFFSET_uint16(BCCMD_SHA256_DATA_wordlength),
	OFFSET_uint16(BCCMD_SHA256_DATA_seq_no),
	OFFSET_uint16(BCCMD_SHA256_DATA_varid),
	OFFSET_uint16(BCCMD_SHA256_DATA_status),
	OFFSET_uint16(BCCMD_SHA256_DATA_msg_len),
	OFFSET_uint16(BCCMD_SHA256_DATA_msg),
	BCCMD_SHA256_DATA_pduSize
} ;

enum {
	OFFSET_START(BCCMD_SHA256_RES)
	OFFSET_uint16(BCCMD_SHA256_RES_req_type),
	OFFSET_uint16(BCCMD_SHA256_RES_wordlength),
	OFFSET_uint16(BCCMD_SHA256_RES_seq_no),
	OFFSET_uint16(BCCMD_SHA256_RES_varid),
	OFFSET_uint16(BCCMD_SHA256_RES_status),
	OFFSET_uint32(BCCMD_SHA256_RES_time),
	OFFSET_uint8_array(BCCMD_SHA256_RES_hash,32),
	BCCMD_SHA256_RES_pduSize
} ;

enum {
	OFFSET_START(BCCMD_HQ_SCRAPING)
	OFFSET_uint16(BCCMD_HQ_SCRAPING_req_type),
	OFFSET_uint16(BCCMD_HQ_SCRAPING_length),
	OFFSET_uint16(BCCMD_HQ_SCRAPING_seq_no),
	OFFSET_uint16(BCCMD_HQ_SCRAPING_varid),
	OFFSET_uint16(BCCMD_HQ_SCRAPING_status),
	OFFSET_uint16(BCCMD_HQ_SCRAPING_len),
	OFFSET_uint16(BCCMD_HQ_SCRAPING_data),
	BCCMD_HQ_SCRAPING_pduSize
} ;

enum {
    OFFSET_START(BCCMD_RDF_BLE_PACKET_MATCH)
    OFFSET_uint16(BCCMD_RDF_BLE_PACKET_MATCH_type),
    OFFSET_uint16_array(BCCMD_RDF_BLE_PACKET_MATCH_tplate,3),
    OFFSET_uint16_array(BCCMD_RDF_BLE_PACKET_MATCH_match,3),
    BCCMD_RDF_BLE_PACKET_MATCH_pduSize
};

enum {
    OFFSET_START(BCCMD_RDF_BLE_ADVERTISING)
	OFFSET_uint16(BCCMD_RDF_BLE_ADVERTISING_req_type),
	OFFSET_uint16(BCCMD_RDF_BLE_ADVERTISING_length),
	OFFSET_uint16(BCCMD_RDF_BLE_ADVERTISING_seq_no),
	OFFSET_uint16(BCCMD_RDF_BLE_ADVERTISING_varid),
	OFFSET_uint16(BCCMD_RDF_BLE_ADVERTISING_status),
    OFFSET_uint8(BCCMD_RDF_BLE_ADVERTISING_mode),
    OFFSET_uint16(BCCMD_RDF_BLE_ADVERTISING_adv_interval),
    OFFSET_uint8(BCCMD_RDF_BLE_ADVERTISING_adv_type),
    OFFSET_uint8(BCCMD_RDF_BLE_ADVERTISING_addr_type),
    OFFSET_uint8(BCCMD_RDF_BLE_ADVERTISING_channels),
    OFFSET_uint8(BCCMD_RDF_BLE_ADVERTISING_data_len),
    OFFSET_uint8(BCCMD_RDF_BLE_ADVERTISING_anti_whiten_begin),
    OFFSET_uint8(BCCMD_RDF_BLE_ADVERTISING_anti_whiten_end),
    OFFSET_uint8(BCCMD_RDF_BLE_ADVERTISING_data),
    BCCMD_RDF_BLE_ADVERTISING_pduSize
};

enum {
    OFFSET_START(BCCMD_RDF_BLE_RAND_ADDR)
	OFFSET_uint16(BCCMD_RDF_BLE_RAND_ADDR_req_type),
	OFFSET_uint16(BCCMD_RDF_BLE_RAND_ADDR_length),
	OFFSET_uint16(BCCMD_RDF_BLE_RAND_ADDR_seq_no),
	OFFSET_uint16(BCCMD_RDF_BLE_RAND_ADDR_varid),
	OFFSET_uint16(BCCMD_RDF_BLE_RAND_ADDR_status),
    OFFSET_uint8(BCCMD_RDF_BLE_RAND_ADDR_addr),
    BCCMD_RDF_BLE_RAND_ADDR_pduSize
};

enum {
	OFFSET_START(BCCMD_OTP_STATS)
	OFFSET_uint16(BCCMD_OTP_STATS_req_type),
	OFFSET_uint16(BCCMD_OTP_STATS_length),
	OFFSET_uint16(BCCMD_OTP_STATS_seq_no),
	OFFSET_uint16(BCCMD_OTP_STATS_varid),
	OFFSET_uint16(BCCMD_OTP_STATS_status),
	OFFSET_uint16(BCCMD_OTP_STATS_len),
	OFFSET_uint16(BCCMD_OTP_STATS_stats),
	BCCMD_OTP_STATS_pduSize
} ;

enum {
        OFFSET_START(BCCMD_BLE_ADV_FILT_ADD)
        OFFSET_uint16(BCCMD_BLE_ADV_FILT_ADD_req_type),
        OFFSET_uint16(BCCMD_BLE_ADV_FILT_ADD_wordlength),
        OFFSET_uint16(BCCMD_BLE_ADV_FILT_ADD_seq_no),
        OFFSET_uint16(BCCMD_BLE_ADV_FILT_ADD_varid),
        OFFSET_uint16(BCCMD_BLE_ADV_FILT_ADD_status),
        OFFSET_uint16(BCCMD_BLE_ADV_FILT_ADD_operation),
        OFFSET_uint16(BCCMD_BLE_ADV_FILT_ADD_ad_type),
        OFFSET_uint16(BCCMD_BLE_ADV_FILT_ADD_interval),
        OFFSET_uint16(BCCMD_BLE_ADV_FILT_ADD_pattern_length),
        OFFSET_uint16(BCCMD_BLE_ADV_FILT_ADD_pattern),
        BCCMD_BLE_ADV_FILT_ADD_pduSize
} ;

enum {
        OFFSET_START(BCCMD_SIFLASH)
        OFFSET_uint16(BCCMD_SIFLASH_req_type),
        OFFSET_uint16(BCCMD_SIFLASH_wordlength),
        OFFSET_uint16(BCCMD_SIFLASH_seq_no),
        OFFSET_uint16(BCCMD_SIFLASH_varid),
        OFFSET_uint16(BCCMD_SIFLASH_status),
        OFFSET_uint16(BCCMD_SIFLASH_command),
        OFFSET_uint32(BCCMD_SIFLASH_word_address),
        OFFSET_uint16(BCCMD_SIFLASH_data_length),
        OFFSET_uint16(BCCMD_SIFLASH_data),
        BCCMD_SIFLASH_pduSize
} ;

enum {
        OFFSET_START(BCCMD_CREATE_OPERATOR_P)
        OFFSET_uint16(BCCMD_CREATE_OPERATOR_P_req_type),
        OFFSET_uint16(BCCMD_CREATE_OPERATOR_P_wordlength),
        OFFSET_uint16(BCCMD_CREATE_OPERATOR_P_seq_no),
        OFFSET_uint16(BCCMD_CREATE_OPERATOR_P_varid),
        OFFSET_uint16(BCCMD_CREATE_OPERATOR_P_status),
        OFFSET_uint16(BCCMD_CREATE_OPERATOR_P_len),
        OFFSET_uint16(BCCMD_CREATE_OPERATOR_P_patch),
        BCCMD_CREATE_OPERATOR_P_pduSize
} ;

enum {
        OFFSET_START(BCCMD_OPERATOR_MESSAGE)
        OFFSET_uint16(BCCMD_OPERATOR_MESSAGE_req_type),
        OFFSET_uint16(BCCMD_OPERATOR_MESSAGE_wordlength),
        OFFSET_uint16(BCCMD_OPERATOR_MESSAGE_seq_no),
        OFFSET_uint16(BCCMD_OPERATOR_MESSAGE_varid),
        OFFSET_uint16(BCCMD_OPERATOR_MESSAGE_status),
        OFFSET_uint16(BCCMD_OPERATOR_MESSAGE_opid),
        OFFSET_uint16(BCCMD_OPERATOR_MESSAGE_arguments),
        BCCMD_OPERATOR_MESSAGE_pduSize
} ;

enum {
        OFFSET_START(BCCMD_START_OPERATOR)
        OFFSET_uint16(BCCMD_START_OPERATOR_req_type),
        OFFSET_uint16(BCCMD_START_OPERATOR_wordlength),
        OFFSET_uint16(BCCMD_START_OPERATOR_seq_no),
        OFFSET_uint16(BCCMD_START_OPERATOR_varid),
        OFFSET_uint16(BCCMD_START_OPERATOR_status),
        OFFSET_uint16(BCCMD_START_OPERATOR_count),
        OFFSET_uint16(BCCMD_START_OPERATOR_list),
        BCCMD_START_OPERATOR_pduSize
} ;

enum {
        OFFSET_START(BCCMD_STOP_OPERATOR)
        OFFSET_uint16(BCCMD_STOP_OPERATOR_req_type),
        OFFSET_uint16(BCCMD_STOP_OPERATOR_wordlength),
        OFFSET_uint16(BCCMD_STOP_OPERATOR_seq_no),
        OFFSET_uint16(BCCMD_STOP_OPERATOR_varid),
        OFFSET_uint16(BCCMD_STOP_OPERATOR_status),
        OFFSET_uint16(BCCMD_STOP_OPERATOR_count),
        OFFSET_uint16(BCCMD_STOP_OPERATOR_list),
        BCCMD_STOP_OPERATOR_pduSize
} ;

enum {
        OFFSET_START(BCCMD_RESET_OPERATOR)
        OFFSET_uint16(BCCMD_RESET_OPERATOR_req_type),
        OFFSET_uint16(BCCMD_RESET_OPERATOR_wordlength),
        OFFSET_uint16(BCCMD_RESET_OPERATOR_seq_no),
        OFFSET_uint16(BCCMD_RESET_OPERATOR_varid),
        OFFSET_uint16(BCCMD_RESET_OPERATOR_status),
        OFFSET_uint16(BCCMD_RESET_OPERATOR_count),
        OFFSET_uint16(BCCMD_RESET_OPERATOR_list),
        BCCMD_RESET_OPERATOR_pduSize
} ;

enum {
        OFFSET_START(BCCMD_DESTROY_OPERATOR)
        OFFSET_uint16(BCCMD_DESTROY_OPERATOR_req_type),
        OFFSET_uint16(BCCMD_DESTROY_OPERATOR_wordlength),
        OFFSET_uint16(BCCMD_DESTROY_OPERATOR_seq_no),
        OFFSET_uint16(BCCMD_DESTROY_OPERATOR_varid),
        OFFSET_uint16(BCCMD_DESTROY_OPERATOR_status),
        OFFSET_uint16(BCCMD_DESTROY_OPERATOR_count),
        OFFSET_uint16(BCCMD_DESTROY_OPERATOR_list),
        BCCMD_DESTROY_OPERATOR_pduSize
} ;

/*******************************************************************/

BCCMD_MEMORY_PDU::BCCMD_MEMORY_PDU()
: BCCMD_PDU( BCCMDVARID_MEMORY )
{
}

BCCMD_MEMORY_PDU::BCCMD_MEMORY_PDU( const PDU& from )
: BCCMD_PDU( from )
{
}

BCCMD_MEMORY_PDU::BCCMD_MEMORY_PDU( const uint8* buffer , uint32 len )
: BCCMD_PDU( buffer , len )
{
}

uint16 BCCMD_MEMORY_PDU::get_base_addr(void) const
{
    return get_uint16(BCCMD_MEMORY_base_addr) ;
}

void BCCMD_MEMORY_PDU::set_base_addr(uint16 val)
{
    set_uint16(BCCMD_MEMORY_base_addr,val) ;
}

uint16 BCCMD_MEMORY_PDU::get_len(void) const
{
    return get_uint16(BCCMD_MEMORY_len) ;
}

void BCCMD_MEMORY_PDU::set_len(uint16 val)
{
    set_uint16(BCCMD_MEMORY_len,val) ;
    resize ( BCCMD_MEMORY_mem + 2 * val );
    set_length();
}

void BCCMD_MEMORY_PDU::get_mem( uint16 * copyto , uint32 len ) const
{
    get_uint16Array(copyto,BCCMD_MEMORY_mem,len) ;
}

void BCCMD_MEMORY_PDU::set_mem( const uint16 * copyfrom , uint32 len )
{
    set_uint16Array(copyfrom,BCCMD_MEMORY_mem,len) ;
    set_length();
}

/*******************************************************************/
BCCMD_OTP_STATS_PDU::BCCMD_OTP_STATS_PDU()
: BCCMD_PDU( BCCMDVARID_OTP_STATS )
{
}

BCCMD_OTP_STATS_PDU::BCCMD_OTP_STATS_PDU( const PDU& from )
: BCCMD_PDU( from )
{
}

BCCMD_OTP_STATS_PDU::BCCMD_OTP_STATS_PDU( const uint8* buffer , uint32 len )
: BCCMD_PDU( buffer , len )
{
}

uint16 BCCMD_OTP_STATS_PDU::get_len(void) const
{
    return get_uint16(BCCMD_OTP_STATS_len) ;
}

void BCCMD_OTP_STATS_PDU::set_len(uint16 val)
{
    set_uint16(BCCMD_OTP_STATS_len,val) ;
    resize ( BCCMD_OTP_STATS_stats + 2 * val );
    set_length();
}

void BCCMD_OTP_STATS_PDU::get_stats( uint16 * copyto , uint32 len ) const
{
    get_uint16Array(copyto,BCCMD_OTP_STATS_stats,len) ;
}

void BCCMD_OTP_STATS_PDU::set_stats( const uint16 * copyfrom , uint32 len )
{
    set_uint16Array(copyfrom,BCCMD_OTP_STATS_stats,len) ;
    set_length();
}

/*******************************************************************/

BCCMD_RANDOM_PDU::BCCMD_RANDOM_PDU()
: BCCMD_PDU( BCCMDVARID_RANDOM )
{
}

BCCMD_RANDOM_PDU::BCCMD_RANDOM_PDU( const PDU& from )
: BCCMD_PDU( from )
{
}

BCCMD_RANDOM_PDU::BCCMD_RANDOM_PDU( const uint8* buffer , uint32 len )
: BCCMD_PDU( buffer , len )
{
}

uint16 BCCMD_RANDOM_PDU::get_len(void) const
{
    return get_uint16(BCCMD_RANDOM_len) ;
}

void BCCMD_RANDOM_PDU::set_len(uint16 val)
{
    set_uint16(BCCMD_RANDOM_len,val) ;
    resize ( BCCMD_RANDOM_data + 2 * val );
    set_length();
}

void BCCMD_RANDOM_PDU::get_data( uint16 * copyto , uint32 len ) const
{
    get_uint16Array(copyto,BCCMD_RANDOM_data,len) ;
}


uint16 BCCMD_RANDOM_PDU::get_type(void) const
{
    return get_uint16(BCCMD_RANDOM_type) ;
}

void BCCMD_RANDOM_PDU::set_type(uint16 val)
{
	 set_uint16(BCCMD_RANDOM_type,val) ;
}

/*******************************************************************/


BCCMD_PS_PDU::BCCMD_PS_PDU()
: BCCMD_PDU(BCCMDVARID_PS)
{
}

BCCMD_PS_PDU::BCCMD_PS_PDU( const PDU& from )
: BCCMD_PDU( from )
{
}

BCCMD_PS_PDU::BCCMD_PS_PDU( const uint8* buffer , uint32 len )
: BCCMD_PDU( buffer , len )
{
}

uint16 BCCMD_PS_PDU::get_id(void) const
{
    return get_uint16(BCCMD_PS_id) ;
}

void BCCMD_PS_PDU::set_id(uint16 val)
{
    set_uint16(BCCMD_PS_id,val) ;
}

uint16 BCCMD_PS_PDU::get_len(void) const
{
    return get_uint16(BCCMD_PS_len) ;
}

void BCCMD_PS_PDU::set_len(uint16 val)
{
    resize ( val * 2 + BCCMD_PS_psmem );
    set_uint16(BCCMD_PS_len,val) ;
    set_length();
}

uint16 BCCMD_PS_PDU::get_stores(void) const
{
    return get_uint16(BCCMD_PS_stores) ;
}

void BCCMD_PS_PDU::set_stores(uint16 val)
{
    set_uint16(BCCMD_PS_stores,val) ;
}

void BCCMD_PS_PDU::get_psmem( uint16 * copyto , uint32 len ) const
{
    get_uint16Array(copyto,BCCMD_PS_psmem,len) ;
}

void BCCMD_PS_PDU::set_psmem( const uint16 * copyfrom , uint32 len )
{
    set_uint16Array(copyfrom,BCCMD_PS_psmem,len) ;
    set_len ( (uint16)len );
}

/*******************************************************************/


BCCMD_BUFFER_PDU::BCCMD_BUFFER_PDU()
: BCCMD_PDU(BCCMDVARID_BUFFER)
{
}

BCCMD_BUFFER_PDU::BCCMD_BUFFER_PDU( const PDU& from )
: BCCMD_PDU( from )
{
}

BCCMD_BUFFER_PDU::BCCMD_BUFFER_PDU( const uint8* buffer , uint32 len )
: BCCMD_PDU( buffer , len )
{
}

uint16 BCCMD_BUFFER_PDU::get_handle(void) const
{
    return get_uint16(BCCMD_BUFFER_handle) ;
}

void BCCMD_BUFFER_PDU::set_handle(uint16 val)
{
    set_uint16(BCCMD_BUFFER_handle,val) ;
}

uint16 BCCMD_BUFFER_PDU::get_start(void) const
{
    return get_uint16(BCCMD_BUFFER_start) ;
}

void BCCMD_BUFFER_PDU::set_start(uint16 val)
{
    set_uint16(BCCMD_BUFFER_start,val) ;
}

uint16 BCCMD_BUFFER_PDU::get_len(void) const
{
    return get_uint16(BCCMD_BUFFER_len) ;
}

void BCCMD_BUFFER_PDU::set_len(uint16 val)
{
    set_uint16(BCCMD_BUFFER_len,val) ;
    resize ( BCCMD_BUFFER_bufmem + 2 * val );
    set_length();
}

void BCCMD_BUFFER_PDU::get_bufmem( uint8 * copyto , uint32 len ) const
{
    get_uint8Array(copyto,BCCMD_BUFFER_bufmem,len) ;
}

void BCCMD_BUFFER_PDU::set_bufmem( const uint8 * copyfrom , uint32 len )
{
    set_uint8Array(copyfrom,BCCMD_BUFFER_bufmem,len) ;
    set_length();
}

/*******************************************************************/


BCCMD_E2_APP_DATA_PDU::BCCMD_E2_APP_DATA_PDU()
: BCCMD_PDU(BCCMDVARID_E2_APP_DATA)
{
}

BCCMD_E2_APP_DATA_PDU::BCCMD_E2_APP_DATA_PDU( const PDU& from )
: BCCMD_PDU( from )
{
}

BCCMD_E2_APP_DATA_PDU::BCCMD_E2_APP_DATA_PDU( const uint8* buffer , uint32 len )
: BCCMD_PDU( buffer , len )
{
}

uint16 BCCMD_E2_APP_DATA_PDU::get_offset(void) const
{
    return get_uint16(BCCMD_E2_APP_DATA_offset) ;
}

void BCCMD_E2_APP_DATA_PDU::set_offset(uint16 val)
{
    set_uint16(BCCMD_E2_APP_DATA_offset,val) ;
}

uint16 BCCMD_E2_APP_DATA_PDU::get_words(void) const
{
    return get_uint16(BCCMD_E2_APP_DATA_words) ;
}

void BCCMD_E2_APP_DATA_PDU::set_words(uint16 val)
{
    set_uint16(BCCMD_E2_APP_DATA_words,val) ;
    resize ( BCCMD_E2_APP_DATA_data + 2 * val );
    set_length();
}

void BCCMD_E2_APP_DATA_PDU::get_data( uint16 * copyto , uint32 len ) const
{
    get_uint16Array(copyto,BCCMD_E2_APP_DATA_data,len) ;
}

void BCCMD_E2_APP_DATA_PDU::set_data( const uint16 * copyfrom , uint32 len )
{
    set_uint16Array(copyfrom,BCCMD_E2_APP_DATA_data,len) ;
    set_length();
}

/*******************************************************************/


BCCMD_LC_RX_FRACS_PDU::BCCMD_LC_RX_FRACS_PDU()
: BCCMD_PDU(BCCMDVARID_LC_RX_FRACS)
{
}

BCCMD_LC_RX_FRACS_PDU::BCCMD_LC_RX_FRACS_PDU( const PDU& from )
: BCCMD_PDU( from )
{
}

BCCMD_LC_RX_FRACS_PDU::BCCMD_LC_RX_FRACS_PDU( const uint8* buffer , uint32 len )
: BCCMD_PDU( buffer , len )
{
}

uint16 BCCMD_LC_RX_FRACS_PDU::get_value_count(void) const
{
    return get_uint16(BCCMD_LC_RX_FRACS_value_count) ;
}

void BCCMD_LC_RX_FRACS_PDU::set_value_count(uint16 val)
{
    set_uint16(BCCMD_LC_RX_FRACS_value_count,val) ;
    resize ( BCCMD_LC_RX_FRACS_rx_frac_vals + 2 * val );
    set_length();
}

void BCCMD_LC_RX_FRACS_PDU::get_rx_frac_vals( uint16 * copyto , uint32 len ) const
{
    get_uint16Array(copyto,BCCMD_LC_RX_FRACS_rx_frac_vals,len) ;
}

void BCCMD_LC_RX_FRACS_PDU::set_rx_frac_vals( const uint16 * copyfrom , uint32 len )
{
    set_uint16Array(copyfrom,BCCMD_LC_RX_FRACS_rx_frac_vals,len) ;
    set_length();
}

/*******************************************************************/

BCCMD_I2C_TRANSFER_PDU::BCCMD_I2C_TRANSFER_PDU()
: BCCMD_PDU(BCCMDVARID_I2C_TRANSFER)
{
    set_restart( true );
}

BCCMD_I2C_TRANSFER_PDU::BCCMD_I2C_TRANSFER_PDU( const PDU& from )
: BCCMD_PDU( from )
{
}

BCCMD_I2C_TRANSFER_PDU::BCCMD_I2C_TRANSFER_PDU( const uint8 * buffer , uint32 len )
: BCCMD_PDU( buffer , len )
{
}

uint16 BCCMD_I2C_TRANSFER_PDU::get_address(void) const
{
    return get_uint16 ( BCCMD_I2C_TRANSFER_PDU_address );
}

void BCCMD_I2C_TRANSFER_PDU::set_address(uint16 value)
{
    set_uint16 ( BCCMD_I2C_TRANSFER_PDU_address , value );
}

uint16 BCCMD_I2C_TRANSFER_PDU::get_tx_octets(void) const
{
    return get_uint16 ( BCCMD_I2C_TRANSFER_PDU_tx_octets );
}

void BCCMD_I2C_TRANSFER_PDU::set_tx_octets(uint16 value)
{
    set_uint16 ( BCCMD_I2C_TRANSFER_PDU_tx_octets , value );
    resize ( BCCMD_I2C_TRANSFER_PDU_data + 2 * ( value + get_rx_octets() ) );
    set_length();
}

uint16 BCCMD_I2C_TRANSFER_PDU::get_rx_octets(void) const
{
    return get_uint16 ( BCCMD_I2C_TRANSFER_PDU_rx_octets );
}

void BCCMD_I2C_TRANSFER_PDU::set_rx_octets(uint16 value)
{
    set_uint16 ( BCCMD_I2C_TRANSFER_PDU_rx_octets , value );
    resize ( BCCMD_I2C_TRANSFER_PDU_data + 2 * ( value + get_tx_octets() ) );
    set_length();
}

bool BCCMD_I2C_TRANSFER_PDU::get_restart(void) const
{
    return get_bool ( BCCMD_I2C_TRANSFER_PDU_restart );
}

void BCCMD_I2C_TRANSFER_PDU::set_restart(bool value)
{
    set_bool ( BCCMD_I2C_TRANSFER_PDU_restart , value );
}

uint16 BCCMD_I2C_TRANSFER_PDU::get_octets(void) const
{
    return get_uint16 ( BCCMD_I2C_TRANSFER_PDU_octets );
}

void BCCMD_I2C_TRANSFER_PDU::get_data( uint8 * copyto , uint32 len ) const
{
    get_uint8Array ( copyto , BCCMD_I2C_TRANSFER_PDU_data , len );
}

void BCCMD_I2C_TRANSFER_PDU::set_data( const uint8 * copyfrom , uint32 len )
{
    uint16 data_size = get_tx_octets() + get_rx_octets();
    set_uint8Array ( copyfrom , BCCMD_I2C_TRANSFER_PDU_data ,
        ( len >  data_size ) ? data_size : len );
}

void BCCMD_I2C_TRANSFER_PDU::get_tx_data( uint8 * copyto , uint32 len ) const
{
    uint16 data_size = get_tx_octets();
    get_data ( copyto , ( len < data_size ) ? len : data_size );
}

void BCCMD_I2C_TRANSFER_PDU::get_rx_data( uint8 * copyto , uint32 len ) const
{
    uint16 data_size = get_rx_octets();
    get_uint8Array ( copyto , BCCMD_I2C_TRANSFER_PDU_data + 2*get_tx_octets() ,
        ( len < data_size ) ? len : data_size );
}

/*******************************************************************/


BCCMD_BUILD_NAME_PDU::BCCMD_BUILD_NAME_PDU()
: BCCMD_PDU(BCCMDVARID_BUILD_NAME)
{
}

BCCMD_BUILD_NAME_PDU::BCCMD_BUILD_NAME_PDU( const PDU& from )
: BCCMD_PDU( from )
{
}

BCCMD_BUILD_NAME_PDU::BCCMD_BUILD_NAME_PDU( const uint8* buffer , uint32 len )
: BCCMD_PDU( buffer , len )
{
}

uint16 BCCMD_BUILD_NAME_PDU::get_start(void) const
{
    return get_uint16(BCCMD_BUILD_NAME_start) ;
}

void BCCMD_BUILD_NAME_PDU::set_start(uint16 val)
{
    set_uint16(BCCMD_BUILD_NAME_start,val) ;
}

uint16 BCCMD_BUILD_NAME_PDU::get_len(void) const
{
    return get_uint16(BCCMD_BUILD_NAME_len) ;
}

void BCCMD_BUILD_NAME_PDU::set_len(uint16 val)
{
    set_uint16(BCCMD_BUILD_NAME_len,val) ;
    resize ( BCCMD_BUILD_NAME_bufmem + 2 * val );
    set_length();
}

void BCCMD_BUILD_NAME_PDU::get_bufmem( uint8 * copyto , uint32 len ) const
{
    get_uint8Array(copyto,BCCMD_BUILD_NAME_bufmem,len) ;
}

void BCCMD_BUILD_NAME_PDU::set_bufmem( const uint8 * copyfrom , uint32 len )
{
    set_uint8Array(copyfrom,BCCMD_BUILD_NAME_bufmem,len) ;
    set_length();
}

/*******************************************************************/
/*******************************************************************/



///////////////////////////////////////////////////////////////////////////////
//
//  Class: BCCMD_FM_RDS_PDU
//
///////////////////////////////////////////////////////////////////////////////

BCCMD_FM_RDS_PDU::BCCMD_FM_RDS_PDU()
: BCCMD_PDU ( BCCMDVARID_FM_RDS , BCCMD_FM_RDS_pduSize )
{
}

BCCMD_FM_RDS_PDU::BCCMD_FM_RDS_PDU( const PDU& from )
: BCCMD_PDU ( from )
{
}

BCCMD_FM_RDS_PDU::BCCMD_FM_RDS_PDU( const uint8 * buffer , uint32 len )
: BCCMD_PDU ( buffer , len )
{
}

uint16 BCCMD_FM_RDS_PDU::get_req_type() const
{
    return get_uint16 ( BCCMD_FM_RDS_req_type );
}

void BCCMD_FM_RDS_PDU::set_req_type( uint16 value )
{
    set_uint16 ( BCCMD_FM_RDS_req_type , value );
}

uint16 BCCMD_FM_RDS_PDU::get_wordlength() const
{
    return get_uint16 ( BCCMD_FM_RDS_wordlength );
}

void BCCMD_FM_RDS_PDU::set_wordlength( uint16 value )
{
    set_uint16 ( BCCMD_FM_RDS_wordlength , value );
}

uint16 BCCMD_FM_RDS_PDU::get_seq_no() const
{
    return get_uint16 ( BCCMD_FM_RDS_seq_no );
}

void BCCMD_FM_RDS_PDU::set_seq_no( uint16 value )
{
    set_uint16 ( BCCMD_FM_RDS_seq_no , value );
}

uint16 BCCMD_FM_RDS_PDU::get_varid() const
{
    return get_uint16 ( BCCMD_FM_RDS_varid );
}

void BCCMD_FM_RDS_PDU::set_varid( uint16 value )
{
    set_uint16 ( BCCMD_FM_RDS_varid , value );
}

uint16 BCCMD_FM_RDS_PDU::get_status() const
{
    return get_uint16 ( BCCMD_FM_RDS_status );
}

void BCCMD_FM_RDS_PDU::set_status( uint16 value )
{
    set_uint16 ( BCCMD_FM_RDS_status , value );
}

uint16 BCCMD_FM_RDS_PDU::get_rds_len() const
{
    return get_uint16 ( BCCMD_FM_RDS_rds_len );
}

const int BCCMD_FM_RDS_PDU::bytes_per_block = 4;
const int BCCMD_FM_RDS_PDU::words_per_block = 2;

void BCCMD_FM_RDS_PDU::set_rds_len( uint16 value )
{
    //  len is blocks (a block is two words = 4 bytes)
    set_uint16 ( BCCMD_FM_RDS_rds_len , value );
    resize(value*bytes_per_block + BCCMD_FM_RDS_rds_data);
    set_length();
}

void BCCMD_FM_RDS_PDU::get_rds_data( uint16 * copyTo , uint16 len ) const
{
    get_uint16Array ( copyTo , BCCMD_FM_RDS_rds_data , len );
}

void BCCMD_FM_RDS_PDU::set_rds_data( const uint16 * copyFrom , uint16 len )
{
//  len is words (a block is two words)
    set_uint16Array ( copyFrom , BCCMD_FM_RDS_rds_data , len );
    set_rds_len(len / words_per_block);
    set_length();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Class: BCCMD_FMTX_TEXT_PDU
//
///////////////////////////////////////////////////////////////////////////////

BCCMD_FMTX_TEXT_PDU::BCCMD_FMTX_TEXT_PDU()
: BCCMD_PDU ( BCCMDVARID_FMTX_TEXT , BCCMD_FMTX_TEXT_pduSize )
{
}

BCCMD_FMTX_TEXT_PDU::BCCMD_FMTX_TEXT_PDU( const PDU& from )
: BCCMD_PDU ( from )
{
}

BCCMD_FMTX_TEXT_PDU::BCCMD_FMTX_TEXT_PDU( const uint8 * buffer , uint32 len )
: BCCMD_PDU ( buffer , len )
{
}

uint16 BCCMD_FMTX_TEXT_PDU::get_req_type() const
{
    return get_uint16 ( BCCMD_FMTX_TEXT_req_type );
}

void BCCMD_FMTX_TEXT_PDU::set_req_type( uint16 value )
{
    set_uint16 ( BCCMD_FMTX_TEXT_req_type , value );
}

uint16 BCCMD_FMTX_TEXT_PDU::get_wordlength() const
{
    return get_uint16 ( BCCMD_FMTX_TEXT_wordlength );
}

void BCCMD_FMTX_TEXT_PDU::set_wordlength( uint16 value )
{
    set_uint16 ( BCCMD_FMTX_TEXT_wordlength , value );
}

uint16 BCCMD_FMTX_TEXT_PDU::get_seq_no() const
{
    return get_uint16 ( BCCMD_FMTX_TEXT_seq_no );
}

void BCCMD_FMTX_TEXT_PDU::set_seq_no( uint16 value )
{
    set_uint16 ( BCCMD_FMTX_TEXT_seq_no , value );
}

uint16 BCCMD_FMTX_TEXT_PDU::get_varid() const
{
    return get_uint16 ( BCCMD_FMTX_TEXT_varid );
}

void BCCMD_FMTX_TEXT_PDU::set_varid( uint16 value )
{
    set_uint16 ( BCCMD_FMTX_TEXT_varid , value );
}

uint16 BCCMD_FMTX_TEXT_PDU::get_status() const
{
    return get_uint16 ( BCCMD_FMTX_TEXT_status );
}

void BCCMD_FMTX_TEXT_PDU::set_status( uint16 value )
{
    set_uint16 ( BCCMD_FMTX_TEXT_status , value );
}

uint16 BCCMD_FMTX_TEXT_PDU::get_text_type() const
{
    return get_uint16 ( BCCMD_FMTX_TEXT_text_type );
}


void BCCMD_FMTX_TEXT_PDU::set_text_type( uint16 value )
{
    set_uint16 ( BCCMD_FMTX_TEXT_text_type , value );
}

void BCCMD_FMTX_TEXT_PDU::get_string( uint16 * copyTo , uint16 len ) const
{
    get_uint16Array ( copyTo , BCCMD_FMTX_TEXT_string , len );
}

void BCCMD_FMTX_TEXT_PDU::set_string( const uint16 * copyFrom , uint16 len )
{

    set_uint16Array ( copyFrom , BCCMD_FMTX_TEXT_string , len );
}



///////////////////////////////////////////////////////////////////////////////
//
//  Class: BCCMD_SHA256_DATA_PDU
//
///////////////////////////////////////////////////////////////////////////////

BCCMD_SHA256_DATA_PDU::BCCMD_SHA256_DATA_PDU()
: BCCMD_PDU ( BCCMDVARID_SHA256_DATA , BCCMD_SHA256_DATA_pduSize )
{
}

BCCMD_SHA256_DATA_PDU::BCCMD_SHA256_DATA_PDU( const PDU& from )
: BCCMD_PDU ( from )
{
}

BCCMD_SHA256_DATA_PDU::BCCMD_SHA256_DATA_PDU( const uint8 * buffer , uint32 len )
: BCCMD_PDU ( buffer , len )
{
}

uint16 BCCMD_SHA256_DATA_PDU::get_msg_len() const
{
    return get_uint16 ( BCCMD_SHA256_DATA_msg_len );
}

void BCCMD_SHA256_DATA_PDU::set_msg_len( uint16 value )
{
    //  len is uint16s
    set_uint16 ( BCCMD_SHA256_DATA_msg_len , value );
    resize(value*byte_word_size_ratio + BCCMD_SHA256_DATA_msg);
    set_length();
}

void BCCMD_SHA256_DATA_PDU::get_msg( uint8 * copyTo , uint16 len ) const
{
    get_uint8Array ( copyTo , BCCMD_SHA256_DATA_msg , len );
}

void BCCMD_SHA256_DATA_PDU::set_msg( const uint8 * copyFrom , uint16 len )
{
    //  len is uint8s
    set_msg_len(len);
    set_uint8Array ( copyFrom , BCCMD_SHA256_DATA_msg , len );
    set_length();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class: BCCMD_SHA256_RES_PDU
//
///////////////////////////////////////////////////////////////////////////////

BCCMD_SHA256_RES_PDU::BCCMD_SHA256_RES_PDU()
: BCCMD_PDU ( BCCMDVARID_SHA256_RES , BCCMD_SHA256_RES_pduSize )
{
}

BCCMD_SHA256_RES_PDU::BCCMD_SHA256_RES_PDU( const PDU& from )
: BCCMD_PDU ( from )
{
}

BCCMD_SHA256_RES_PDU::BCCMD_SHA256_RES_PDU( const uint8 * buffer , uint32 len )
: BCCMD_PDU ( buffer , len )
{
}

uint32 BCCMD_SHA256_RES_PDU::get_time() const
{
    return get_uint32 ( BCCMD_SHA256_RES_time );
}

void BCCMD_SHA256_RES_PDU::set_time( uint32 value )
{
    set_uint32 ( BCCMD_SHA256_RES_time , value );
}

void BCCMD_SHA256_RES_PDU::get_hash( uint8 * copyTo ) const
{
    get_uint8Array ( copyTo , BCCMD_SHA256_RES_hash , BCCMDPDU_SHA256_RES_HASH_SIZE );
}

void BCCMD_SHA256_RES_PDU::set_hash( const uint8 * copyFrom )
{
    set_uint8Array ( copyFrom , BCCMD_SHA256_RES_hash , BCCMDPDU_SHA256_RES_HASH_SIZE );
}

/*******************************************************************/

BCCMD_HQ_SCRAPING_PDU::BCCMD_HQ_SCRAPING_PDU()
: BCCMD_PDU( BCCMDVARID_HQ_SCRAPING )
{
}

BCCMD_HQ_SCRAPING_PDU::BCCMD_HQ_SCRAPING_PDU( const PDU& from )
: BCCMD_PDU( from )
{
}

BCCMD_HQ_SCRAPING_PDU::BCCMD_HQ_SCRAPING_PDU( const uint8* buffer , uint32 len )
: BCCMD_PDU( buffer , len )
{
}

uint16 BCCMD_HQ_SCRAPING_PDU::get_hq_len(void) const
{
    return get_uint16(BCCMD_HQ_SCRAPING_len) ;
}

void BCCMD_HQ_SCRAPING_PDU::set_hq_len(uint16 val)
{
    set_uint16(BCCMD_HQ_SCRAPING_len,val) ;
    resize ( BCCMD_HQ_SCRAPING_data + 2 * val );
    set_length();
}

void BCCMD_HQ_SCRAPING_PDU::get_hq_data( uint16 * copyto , uint32 len ) const
{
    get_uint16Array(copyto,BCCMD_HQ_SCRAPING_data,len) ;
}

void BCCMD_HQ_SCRAPING_PDU::set_hq_data( const uint16 * copyfrom , uint32 len )
{
    set_uint16Array(copyfrom,BCCMD_HQ_SCRAPING_data,len) ;
    set_length();
}

HQ_PDU BCCMD_HQ_SCRAPING_PDU::get_hq() const
{
    return HQ_PDU(get_uint8ArrayPtr(BCCMD_HQ_SCRAPING_data),size()-BCCMD_HQ_SCRAPING_data);
}

/*******************************************************************/
BCCMD_RDF_BLE_PACKET_MATCH_PDU::BCCMD_RDF_BLE_PACKET_MATCH_PDU()
: BCCMD_PDU( BCCMDVARID_RDF_BLE_PACKET_MATCH )
{
}

BCCMD_RDF_BLE_PACKET_MATCH_PDU::BCCMD_RDF_BLE_PACKET_MATCH_PDU( const PDU& from )
: BCCMD_PDU( from )
{
}

BCCMD_RDF_BLE_PACKET_MATCH_PDU::BCCMD_RDF_BLE_PACKET_MATCH_PDU( const uint8* buffer , uint32 len )
: BCCMD_PDU( buffer , len )
{
}

uint16 BCCMD_RDF_BLE_PACKET_MATCH_PDU::get_type(void) const
{
    return get_uint16(BCCMD_RDF_BLE_PACKET_MATCH_type); 
}

void BCCMD_RDF_BLE_PACKET_MATCH_PDU::set_type(uint16 val)
{
    set_uint16(BCCMD_RDF_BLE_PACKET_MATCH_type, val);
}

void BCCMD_RDF_BLE_PACKET_MATCH_PDU::get_tplate(uint16 * copyto, uint16 len) const
{
    get_uint16Array(copyto, BCCMD_RDF_BLE_PACKET_MATCH_tplate, len);
}

void BCCMD_RDF_BLE_PACKET_MATCH_PDU::set_tplate(const uint16 * copyfrom, uint16 len)
{
    set_uint16Array(copyfrom, BCCMD_RDF_BLE_PACKET_MATCH_tplate, len);
    set_length();
}

void BCCMD_RDF_BLE_PACKET_MATCH_PDU::get_match(uint16 * copyto, uint16 len) const
{
    get_uint16Array(copyto, BCCMD_RDF_BLE_PACKET_MATCH_match, len);
}

void BCCMD_RDF_BLE_PACKET_MATCH_PDU::set_match(const uint16 * copyfrom, uint16 len)
{
    set_uint16Array(copyfrom, BCCMD_RDF_BLE_PACKET_MATCH_match, len);
    set_length();
}

/*******************************************************************/
BCCMD_RDF_BLE_ADVERTISING_PDU::BCCMD_RDF_BLE_ADVERTISING_PDU()
: BCCMD_PDU( BCCMDVARID_RDF_BLE_ADVERTISING, BCCMD_RDF_BLE_ADVERTISING_pduSize )
{
}

BCCMD_RDF_BLE_ADVERTISING_PDU::BCCMD_RDF_BLE_ADVERTISING_PDU( const PDU& from )
: BCCMD_PDU( from )
{
}

BCCMD_RDF_BLE_ADVERTISING_PDU::BCCMD_RDF_BLE_ADVERTISING_PDU( const uint8* buffer , uint32 len )
: BCCMD_PDU( buffer , len )
{
}

uint8 BCCMD_RDF_BLE_ADVERTISING_PDU::get_mode(void) const
{
    return get_uint8(BCCMD_RDF_BLE_ADVERTISING_mode); 
}

void BCCMD_RDF_BLE_ADVERTISING_PDU::set_mode(uint8 val)
{
    set_uint8(BCCMD_RDF_BLE_ADVERTISING_mode, val);
}

uint16 BCCMD_RDF_BLE_ADVERTISING_PDU::get_adv_interval(void) const
{
    return get_uint16(BCCMD_RDF_BLE_ADVERTISING_adv_interval); 
}

void BCCMD_RDF_BLE_ADVERTISING_PDU::set_adv_interval(uint16 val)
{
    set_uint16(BCCMD_RDF_BLE_ADVERTISING_adv_interval, val);
}

uint8 BCCMD_RDF_BLE_ADVERTISING_PDU::get_adv_type(void) const
{
    return get_uint8(BCCMD_RDF_BLE_ADVERTISING_adv_type); 
}

void BCCMD_RDF_BLE_ADVERTISING_PDU::set_adv_type(uint8 val)
{
    set_uint8(BCCMD_RDF_BLE_ADVERTISING_adv_type, val);
}

uint8 BCCMD_RDF_BLE_ADVERTISING_PDU::get_addr_type(void) const
{
    return get_uint8(BCCMD_RDF_BLE_ADVERTISING_addr_type); 
}

void BCCMD_RDF_BLE_ADVERTISING_PDU::set_addr_type(uint8 val)
{
    set_uint8(BCCMD_RDF_BLE_ADVERTISING_addr_type, val);
}

uint8 BCCMD_RDF_BLE_ADVERTISING_PDU::get_channels(void) const
{
    return get_uint8(BCCMD_RDF_BLE_ADVERTISING_channels); 
}

void BCCMD_RDF_BLE_ADVERTISING_PDU::set_channels(uint8 val)
{
    set_uint8(BCCMD_RDF_BLE_ADVERTISING_channels, val);
}

uint8 BCCMD_RDF_BLE_ADVERTISING_PDU::get_data_len(void) const
{
    return get_uint8(BCCMD_RDF_BLE_ADVERTISING_data_len); 
}

void BCCMD_RDF_BLE_ADVERTISING_PDU::set_data_len(uint8 val)
{
    set_uint8(BCCMD_RDF_BLE_ADVERTISING_data_len, val);   
    resize(val+BCCMD_RDF_BLE_ADVERTISING_data);
    set_length();
}

uint8 BCCMD_RDF_BLE_ADVERTISING_PDU::get_anti_whiten_begin(void) const
{
    return get_uint8(BCCMD_RDF_BLE_ADVERTISING_anti_whiten_begin); 
}

void BCCMD_RDF_BLE_ADVERTISING_PDU::set_anti_whiten_begin(uint8 val)
{
    set_uint8(BCCMD_RDF_BLE_ADVERTISING_anti_whiten_begin, val);
}

uint8 BCCMD_RDF_BLE_ADVERTISING_PDU::get_anti_whiten_end(void) const
{
    return get_uint8(BCCMD_RDF_BLE_ADVERTISING_anti_whiten_end); 
}

void BCCMD_RDF_BLE_ADVERTISING_PDU::set_anti_whiten_end(uint8 val)
{
    set_uint8(BCCMD_RDF_BLE_ADVERTISING_anti_whiten_end, val);
}

void BCCMD_RDF_BLE_ADVERTISING_PDU::get_data(uint8 * copyto, uint8 len) const
{
    get_uint8Array(copyto, BCCMD_RDF_BLE_ADVERTISING_data, len);
}

void BCCMD_RDF_BLE_ADVERTISING_PDU::set_data(const uint8 * copyfrom, uint8 len)
{ 
    set_uint8Array(copyfrom, BCCMD_RDF_BLE_ADVERTISING_data, len);
    set_length();
}

/*******************************************************************/
BCCMD_RDF_BLE_RAND_ADDR_PDU::BCCMD_RDF_BLE_RAND_ADDR_PDU()
: BCCMD_PDU( BCCMDVARID_RDF_BLE_RAND_ADDR, BCCMD_RDF_BLE_RAND_ADDR_pduSize )
{
}

BCCMD_RDF_BLE_RAND_ADDR_PDU::BCCMD_RDF_BLE_RAND_ADDR_PDU( const PDU& from )
: BCCMD_PDU( from )
{
}

BCCMD_RDF_BLE_RAND_ADDR_PDU::BCCMD_RDF_BLE_RAND_ADDR_PDU( const uint8* buffer , uint32 len )
: BCCMD_PDU( buffer , len )
{
}

void BCCMD_RDF_BLE_RAND_ADDR_PDU::get_addr(uint8 * copyto, uint16 len) const
{
    get_uint8Array(copyto, BCCMD_RDF_BLE_RAND_ADDR_addr, len);
}

void BCCMD_RDF_BLE_RAND_ADDR_PDU::set_addr(const uint8 * copyfrom, uint16 len)
{
    set_uint8Array(copyfrom, BCCMD_RDF_BLE_RAND_ADDR_addr, len);
    set_length();
}

/*******************************************************************/
BCCMD_BLE_ADV_FILT_ADD_PDU::BCCMD_BLE_ADV_FILT_ADD_PDU()
: BCCMD_PDU( BCCMDVARID_BLE_ADV_FILT_ADD , BCCMD_BLE_ADV_FILT_ADD_pduSize )
{
}

BCCMD_BLE_ADV_FILT_ADD_PDU::BCCMD_BLE_ADV_FILT_ADD_PDU( const PDU& pdu )
: BCCMD_PDU(pdu)
{
}

BCCMD_BLE_ADV_FILT_ADD_PDU::BCCMD_BLE_ADV_FILT_ADD_PDU( const uint8* buffer , uint32 len )
: BCCMD_PDU( buffer , len )
{
}

uint16 BCCMD_BLE_ADV_FILT_ADD_PDU::get_operation(void) const
{
    return get_uint16(BCCMD_BLE_ADV_FILT_ADD_operation) ;
}

void BCCMD_BLE_ADV_FILT_ADD_PDU::set_operation(uint16 val)
{
    set_uint16(BCCMD_BLE_ADV_FILT_ADD_operation,val) ;
}

uint16 BCCMD_BLE_ADV_FILT_ADD_PDU::get_ad_type(void) const
{
    return get_uint16(BCCMD_BLE_ADV_FILT_ADD_ad_type) ;
}

void BCCMD_BLE_ADV_FILT_ADD_PDU::set_ad_type(uint16 val)
{
    set_uint16(BCCMD_BLE_ADV_FILT_ADD_ad_type,val) ;
}

uint16 BCCMD_BLE_ADV_FILT_ADD_PDU::get_interval(void) const
{
    return get_uint16(BCCMD_BLE_ADV_FILT_ADD_interval) ;
}

void BCCMD_BLE_ADV_FILT_ADD_PDU::set_interval(uint16 val)
{
    set_uint16(BCCMD_BLE_ADV_FILT_ADD_interval,val) ;
}

uint16 BCCMD_BLE_ADV_FILT_ADD_PDU::get_pattern_length(void) const
{
    return get_uint16(BCCMD_BLE_ADV_FILT_ADD_pattern_length) ;
}

void BCCMD_BLE_ADV_FILT_ADD_PDU::set_pattern_length(uint16 val)
{
    set_uint16(BCCMD_BLE_ADV_FILT_ADD_pattern_length,val) ;
    resize(BCCMD_BLE_ADV_FILT_ADD_pattern + 2 * val);
    set_length();
}

void BCCMD_BLE_ADV_FILT_ADD_PDU::get_pattern( uint16 * copyto , uint32 len ) const
{
    get_uint16Array(copyto, BCCMD_BLE_ADV_FILT_ADD_pattern, len);
}

void BCCMD_BLE_ADV_FILT_ADD_PDU::set_pattern( const uint16 * copyfrom , uint32 len )
{
    set_uint16Array(copyfrom, BCCMD_BLE_ADV_FILT_ADD_pattern, len);
    set_length();
}

/*******************************************************************/
BCCMD_SIFLASH_PDU::BCCMD_SIFLASH_PDU()
: BCCMD_PDU( BCCMDVARID_SIFLASH , BCCMD_SIFLASH_pduSize )
{
}

BCCMD_SIFLASH_PDU::BCCMD_SIFLASH_PDU( const PDU& pdu )
: BCCMD_PDU(pdu)
{
}

BCCMD_SIFLASH_PDU::BCCMD_SIFLASH_PDU( const uint8* buffer , uint32 len )
: BCCMD_PDU( buffer , len )
{
}

uint16 BCCMD_SIFLASH_PDU::get_command(void) const
{
    return get_uint16(BCCMD_SIFLASH_command) ;
}

void BCCMD_SIFLASH_PDU::set_command(uint16 val)
{
    set_uint16(BCCMD_SIFLASH_command,val) ;
}

uint32 BCCMD_SIFLASH_PDU::get_word_address(void) const
{
    return get_uint32(BCCMD_SIFLASH_word_address) ;
}

void BCCMD_SIFLASH_PDU::set_word_address(uint32 val)
{
    set_uint32(BCCMD_SIFLASH_word_address,val) ;
}

uint16 BCCMD_SIFLASH_PDU::get_data_length(void) const
{
    return get_uint16(BCCMD_SIFLASH_data_length) ;
}

void BCCMD_SIFLASH_PDU::set_data_length(uint16 val)
{
    set_uint16(BCCMD_SIFLASH_data_length,val);
    resize(BCCMD_SIFLASH_data + 2 * val);
    set_length();
}

void BCCMD_SIFLASH_PDU::get_data( uint16 * copyto , uint32 len ) const
{
    get_uint16Array(copyto, BCCMD_SIFLASH_data, len);
}

void BCCMD_SIFLASH_PDU::set_data (const uint16 * copyfrom , uint32 len )
{
    set_uint16Array(copyfrom, BCCMD_SIFLASH_data, len);
    set_length();
}

/*******************************************************************/
BCCMD_CREATE_OPERATOR_P_PDU::BCCMD_CREATE_OPERATOR_P_PDU()
: BCCMD_PDU( BCCMDVARID_CREATE_OPERATOR_P , BCCMD_CREATE_OPERATOR_P_pduSize )
{
}

BCCMD_CREATE_OPERATOR_P_PDU::BCCMD_CREATE_OPERATOR_P_PDU( const PDU& pdu )
: BCCMD_PDU(pdu)
{
}

BCCMD_CREATE_OPERATOR_P_PDU::BCCMD_CREATE_OPERATOR_P_PDU( const uint8* buffer , uint32 len )
: BCCMD_PDU( buffer , len )
{
}

uint16 BCCMD_CREATE_OPERATOR_P_PDU::get_len(void) const
{
    return get_uint16(BCCMD_CREATE_OPERATOR_P_len) ;
}

void BCCMD_CREATE_OPERATOR_P_PDU::set_len(uint16 val)
{
    set_uint16(BCCMD_CREATE_OPERATOR_P_len,val);
    resize(BCCMD_CREATE_OPERATOR_P_patch + 2 * val);
    set_length();
}

void BCCMD_CREATE_OPERATOR_P_PDU::get_patch( uint16 * copyto , uint32 len ) const
{
    get_uint16Array(copyto, BCCMD_CREATE_OPERATOR_P_patch, len);
}

void BCCMD_CREATE_OPERATOR_P_PDU::set_patch (const uint16 * copyfrom , uint32 len )
{
    set_len (len);
    set_uint16Array(copyfrom, BCCMD_CREATE_OPERATOR_P_patch, len);
    set_length();
}

/*******************************************************************/
BCCMD_OPERATOR_MESSAGE_PDU::BCCMD_OPERATOR_MESSAGE_PDU()
: BCCMD_PDU( BCCMDVARID_OPERATOR_MESSAGE , BCCMD_OPERATOR_MESSAGE_pduSize )
{
}

BCCMD_OPERATOR_MESSAGE_PDU::BCCMD_OPERATOR_MESSAGE_PDU( const PDU& pdu )
: BCCMD_PDU(pdu)
{
}

BCCMD_OPERATOR_MESSAGE_PDU::BCCMD_OPERATOR_MESSAGE_PDU( const uint8* buffer , uint32 len )
: BCCMD_PDU( buffer , len )
{
}

uint16 BCCMD_OPERATOR_MESSAGE_PDU::get_opid(void) const
{
    return get_uint16(BCCMD_OPERATOR_MESSAGE_opid) ;
}

void BCCMD_OPERATOR_MESSAGE_PDU::set_opid(uint16 val)
{
    set_uint16(BCCMD_OPERATOR_MESSAGE_opid,val);
}

uint16 BCCMD_OPERATOR_MESSAGE_PDU::get_arg_count(void) const
{
    return get_uint16 ( BCCMD_OPERATOR_MESSAGE_wordlength ) - 6;
}

void BCCMD_OPERATOR_MESSAGE_PDU::get_arguments( uint16 * copyto , uint32 len ) const
{
    get_uint16Array(copyto, BCCMD_OPERATOR_MESSAGE_arguments, len);
}

void BCCMD_OPERATOR_MESSAGE_PDU::set_arguments (const uint16 * copyfrom , uint32 len )
{
    resize(BCCMD_OPERATOR_MESSAGE_arguments + 2 * len);
    set_uint16Array(copyfrom, BCCMD_OPERATOR_MESSAGE_arguments, len);
    set_length();
}

/*******************************************************************/
BCCMD_START_OPERATOR_PDU::BCCMD_START_OPERATOR_PDU()
: BCCMD_PDU( BCCMDVARID_START_OPERATOR , BCCMD_START_OPERATOR_pduSize )
{
}

BCCMD_START_OPERATOR_PDU::BCCMD_START_OPERATOR_PDU( const PDU& pdu )
: BCCMD_PDU(pdu)
{
}

BCCMD_START_OPERATOR_PDU::BCCMD_START_OPERATOR_PDU( const uint8* buffer , uint32 len )
: BCCMD_PDU( buffer , len )
{
}

uint16 BCCMD_START_OPERATOR_PDU::get_count(void) const
{
    return get_uint16(BCCMD_START_OPERATOR_count) ;
}

void BCCMD_START_OPERATOR_PDU::set_count(uint16 val)
{
    set_uint16(BCCMD_START_OPERATOR_count,val);
    resize(BCCMD_START_OPERATOR_list + 2 * val);
    set_length();
}

void BCCMD_START_OPERATOR_PDU::get_list( uint16 * copyto , uint32 len ) const
{
    get_uint16Array(copyto, BCCMD_START_OPERATOR_list, len);
}

void BCCMD_START_OPERATOR_PDU::set_list (const uint16 * copyfrom , uint32 len )
{
    set_count (len);
    set_uint16Array(copyfrom, BCCMD_START_OPERATOR_list, len);
    set_length();
}

/*******************************************************************/
BCCMD_STOP_OPERATOR_PDU::BCCMD_STOP_OPERATOR_PDU()
: BCCMD_PDU( BCCMDVARID_STOP_OPERATOR , BCCMD_STOP_OPERATOR_pduSize )
{
}

BCCMD_STOP_OPERATOR_PDU::BCCMD_STOP_OPERATOR_PDU( const PDU& pdu )
: BCCMD_PDU(pdu)
{
}

BCCMD_STOP_OPERATOR_PDU::BCCMD_STOP_OPERATOR_PDU( const uint8* buffer , uint32 len )
: BCCMD_PDU( buffer , len )
{
}

uint16 BCCMD_STOP_OPERATOR_PDU::get_count(void) const
{
    return get_uint16(BCCMD_STOP_OPERATOR_count) ;
}

void BCCMD_STOP_OPERATOR_PDU::set_count(uint16 val)
{
    set_uint16(BCCMD_STOP_OPERATOR_count,val);
    resize(BCCMD_STOP_OPERATOR_list + 2 * val);
    set_length();
}

void BCCMD_STOP_OPERATOR_PDU::get_list( uint16 * copyto , uint32 len ) const
{
    get_uint16Array(copyto, BCCMD_STOP_OPERATOR_list, len);
}

void BCCMD_STOP_OPERATOR_PDU::set_list (const uint16 * copyfrom , uint32 len )
{
    set_count (len);
    set_uint16Array(copyfrom, BCCMD_STOP_OPERATOR_list, len);
    set_length();
}

/*******************************************************************/
BCCMD_RESET_OPERATOR_PDU::BCCMD_RESET_OPERATOR_PDU()
: BCCMD_PDU( BCCMDVARID_RESET_OPERATOR , BCCMD_RESET_OPERATOR_pduSize )
{
}

BCCMD_RESET_OPERATOR_PDU::BCCMD_RESET_OPERATOR_PDU( const PDU& pdu )
: BCCMD_PDU(pdu)
{
}

BCCMD_RESET_OPERATOR_PDU::BCCMD_RESET_OPERATOR_PDU( const uint8* buffer , uint32 len )
: BCCMD_PDU( buffer , len )
{
}

uint16 BCCMD_RESET_OPERATOR_PDU::get_count(void) const
{
    return get_uint16(BCCMD_RESET_OPERATOR_count) ;
}

void BCCMD_RESET_OPERATOR_PDU::set_count(uint16 val)
{
    set_uint16(BCCMD_RESET_OPERATOR_count,val);
    resize(BCCMD_RESET_OPERATOR_list + 2 * val);
    set_length();
}

void BCCMD_RESET_OPERATOR_PDU::get_list( uint16 * copyto , uint32 len ) const
{
    get_uint16Array(copyto, BCCMD_RESET_OPERATOR_list, len);
}

void BCCMD_RESET_OPERATOR_PDU::set_list (const uint16 * copyfrom , uint32 len )
{
    set_count (len);
    set_uint16Array(copyfrom, BCCMD_RESET_OPERATOR_list, len);
    set_length();
}

/*******************************************************************/
BCCMD_DESTROY_OPERATOR_PDU::BCCMD_DESTROY_OPERATOR_PDU()
: BCCMD_PDU( BCCMDVARID_DESTROY_OPERATOR , BCCMD_DESTROY_OPERATOR_pduSize )
{
}

BCCMD_DESTROY_OPERATOR_PDU::BCCMD_DESTROY_OPERATOR_PDU( const PDU& pdu )
: BCCMD_PDU(pdu)
{
}

BCCMD_DESTROY_OPERATOR_PDU::BCCMD_DESTROY_OPERATOR_PDU( const uint8* buffer , uint32 len )
: BCCMD_PDU( buffer , len )
{
}

uint16 BCCMD_DESTROY_OPERATOR_PDU::get_count(void) const
{
    return get_uint16(BCCMD_DESTROY_OPERATOR_count) ;
}

void BCCMD_DESTROY_OPERATOR_PDU::set_count(uint16 val)
{
    set_uint16(BCCMD_DESTROY_OPERATOR_count,val);
    resize(BCCMD_DESTROY_OPERATOR_list + 2 * val);
    set_length();
}

void BCCMD_DESTROY_OPERATOR_PDU::get_list( uint16 * copyto , uint32 len ) const
{
    get_uint16Array(copyto, BCCMD_DESTROY_OPERATOR_list, len);
}

void BCCMD_DESTROY_OPERATOR_PDU::set_list (const uint16 * copyfrom , uint32 len )
{
    set_count (len);
    set_uint16Array(copyfrom, BCCMD_DESTROY_OPERATOR_list, len);
    set_length();
}
