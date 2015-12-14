////////////////////////////////////////////////////////////////////////////////
//
//  FILE:       xtrabccmd.h
//
//  CLASS:      BCCMD_PS_PDU,
//              BCCMD_MEM_PDU,
//              BCCMD_BUF_PDU,
//              BCCMD_E2_APP_DATA_PDU

//              BCCMD_LC_RX_FRACS
//
//  PURPOSE:    the autogenerator is too dumb to cope with arrays, so
//              this file sorts them out with some hand written code.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __XTRABCCMD_H__
#define __XTRABCCMD_H__

#include "hcipacker/bluecorepdu.h"

class BCCMD_MEMORY_PDU : public BCCMD_PDU
{
public:
    BCCMD_MEMORY_PDU() ;
    BCCMD_MEMORY_PDU( const PDU& ) ;
    BCCMD_MEMORY_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_base_addr(void) const ;
    void set_base_addr(uint16 value) ;
    uint16 get_len(void) const ;
    void set_len(uint16 value) ;
    void get_mem( uint16 * copyto , uint32 len ) const ;
    void set_mem( const uint16 * copyfrom , uint32 len ) ;
};

class BCCMD_RANDOM_PDU : public BCCMD_PDU
{
public:
    BCCMD_RANDOM_PDU() ;
    BCCMD_RANDOM_PDU( const PDU& ) ;
    BCCMD_RANDOM_PDU( const uint8 * buffer , uint32 len ) ;
    void set_len(uint16 value) ;
    uint16 get_len(void) const ;
    uint16 get_type(void) const ;
    void set_type(uint16 value) ;
    void get_data( uint16 * copyto , uint32 len ) const ;
};

class BCCMD_PS_PDU : public BCCMD_PDU
{
public:
    BCCMD_PS_PDU() ;
    BCCMD_PS_PDU( const PDU& ) ;
    BCCMD_PS_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_id(void) const ;
    void set_id(uint16 value) ;
    uint16 get_len(void) const ;
    void set_len(uint16 value) ;
    uint16 get_stores(void) const ;
    void set_stores(uint16 value) ;
    void get_psmem( uint16 * copyto , uint32 len ) const ;
    void set_psmem( const uint16 * copyfrom , uint32 len ) ;
};

class BCCMD_BUFFER_PDU : public BCCMD_PDU
{
public:
    BCCMD_BUFFER_PDU() ;
    BCCMD_BUFFER_PDU( const PDU& ) ;
    BCCMD_BUFFER_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_handle(void) const ;
    void set_handle(uint16 value) ;
    uint16 get_start(void) const ;
    void set_start(uint16 value) ;
    uint16 get_len(void) const ;
    void set_len(uint16 value) ;
    void get_bufmem( uint8 * copyto , uint32 len ) const ;
    void set_bufmem( const uint8 * copyfrom , uint32 len ) ;
};

class BCCMD_E2_APP_DATA_PDU : public BCCMD_PDU
{
public:
    BCCMD_E2_APP_DATA_PDU() ;
    BCCMD_E2_APP_DATA_PDU( const PDU& ) ;
    BCCMD_E2_APP_DATA_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_offset(void) const ;
    void set_offset(uint16 value) ;
    uint16 get_words(void) const ;
    void set_words(uint16 value) ;
    void get_data( uint16 * copyto , uint32 len ) const ;
    void set_data( const uint16 * copyfrom , uint32 len ) ;
};

class BCCMD_I2C_TRANSFER_PDU : public BCCMD_PDU
{
public:
    BCCMD_I2C_TRANSFER_PDU() ;
    BCCMD_I2C_TRANSFER_PDU( const PDU& ) ;
    BCCMD_I2C_TRANSFER_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_address(void) const ;
    void set_address(uint16 value) ;
    uint16 get_tx_octets(void) const ;
    void set_tx_octets(uint16 value) ;
    uint16 get_rx_octets(void) const ;
    void set_rx_octets(uint16 value) ;
    bool get_restart(void) const ;
    void set_restart(bool value) ;
    uint16 get_octets(void) const ;
    void get_data( uint8 * copyto , uint32 len ) const ;
    void set_data( const uint8 * copyfrom , uint32 len ) ;
    void get_tx_data( uint8 * copyto , uint32 len ) const ;
    void get_rx_data( uint8 * copyto , uint32 len ) const ;
};

class BCCMD_BUILD_NAME_PDU : public BCCMD_PDU
{
public:
    BCCMD_BUILD_NAME_PDU() ;
    BCCMD_BUILD_NAME_PDU( const PDU& ) ;
    BCCMD_BUILD_NAME_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_start(void) const ;
    void set_start(uint16 value) ;
    uint16 get_len(void) const ;
    void set_len(uint16 value) ;
    void get_bufmem( uint8 * copyto , uint32 len ) const ;
    void set_bufmem( const uint8 * copyfrom , uint32 len ) ;
};

class BCCMD_LC_RX_FRACS_PDU : public BCCMD_PDU
{
public:
    BCCMD_LC_RX_FRACS_PDU() ;
    BCCMD_LC_RX_FRACS_PDU( const PDU& ) ;
    BCCMD_LC_RX_FRACS_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_value_count(void) const ;
    void set_value_count(uint16 value) ;
    void get_rx_frac_vals( uint16 * copyto , uint32 len ) const ;
    void set_rx_frac_vals( const uint16 * copyfrom , uint32 len ) ;
};

class BCCMD_FM_RDS_PDU : public BCCMD_PDU
{
public:
    BCCMD_FM_RDS_PDU() ;
    BCCMD_FM_RDS_PDU( const PDU& );
    BCCMD_FM_RDS_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_req_type(void) const ;
    void set_req_type(uint16 value) ;
    uint16 get_wordlength(void) const ;
    void set_wordlength(uint16 value) ;
    uint16 get_seq_no(void) const ;
    void set_seq_no(uint16 value) ;
    uint16 get_varid(void) const ;
    void set_varid(uint16 value) ;
    uint16 get_status(void) const ;
    void set_status(uint16 value) ;
    uint16 get_rds_len(void) const ;
    void set_rds_len(uint16 value) ;
    void get_rds_data( uint16 * copyTo , uint16 len ) const ;
    void set_rds_data( const uint16 * copyFrom , uint16 len ) ;
    static const int words_per_block;
    static const int bytes_per_block;
};

class BCCMD_FMTX_TEXT_PDU : public BCCMD_PDU
{
public:
    BCCMD_FMTX_TEXT_PDU() ;
    BCCMD_FMTX_TEXT_PDU( const PDU& );
    BCCMD_FMTX_TEXT_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_req_type(void) const ;
    void set_req_type(uint16 value) ;
    uint16 get_wordlength(void) const ;
    void set_wordlength(uint16 value) ;
    uint16 get_seq_no(void) const ;
    void set_seq_no(uint16 value) ;
    uint16 get_varid(void) const ;
    void set_varid(uint16 value) ;
    uint16 get_status(void) const ;
    void set_status(uint16 value) ;
	//Payload
    uint16 get_text_type(void) const ;
    void set_text_type(uint16 value) ;
    void get_string( uint16 * copyTo , uint16 len ) const ;
    void set_string( const uint16 * copyFrom , uint16 len ) ;
};




class BCCMD_SHA256_DATA_PDU : public BCCMD_PDU
{
public:
    BCCMD_SHA256_DATA_PDU() ;
    BCCMD_SHA256_DATA_PDU( const PDU& ) ;
    BCCMD_SHA256_DATA_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_msg_len(void) const ;
    void set_msg_len(uint16 value) ;
    void get_msg( uint8 * copyto , uint16 len ) const ;
    void set_msg( const uint8 * copyfrom , uint16 len ) ;
};

class BCCMD_SHA256_RES_PDU : public BCCMD_PDU
{
public:
    BCCMD_SHA256_RES_PDU() ;
    BCCMD_SHA256_RES_PDU( const PDU& ) ;
    BCCMD_SHA256_RES_PDU( const uint8 * buffer , uint32 len ) ;
    uint32 get_time(void) const ;
    void set_time(uint32 value) ;
    void get_hash( uint8 * copyto ) const ; /* 32 uint8s */
    void set_hash( const uint8 * copyfrom ) ;
};

class BCCMD_HQ_SCRAPING_PDU : public BCCMD_PDU
{
public:
    BCCMD_HQ_SCRAPING_PDU() ;
    BCCMD_HQ_SCRAPING_PDU( const PDU& ) ;
    BCCMD_HQ_SCRAPING_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_hq_len(void) const ;
	void set_hq_len(uint16 value) ;
    void get_hq_data( uint16 * copyto , uint32 len ) const ;
	void set_hq_data( const uint16 * copyfrom , uint32 len ) ;
    HQ_PDU get_hq() const;
};

class BCCMD_RDF_BLE_PACKET_MATCH_PDU : public BCCMD_PDU
{
public:
    BCCMD_RDF_BLE_PACKET_MATCH_PDU() ;
    BCCMD_RDF_BLE_PACKET_MATCH_PDU( const PDU& ) ;
    BCCMD_RDF_BLE_PACKET_MATCH_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_type(void) const ;
	void set_type(uint16 value) ;
    void get_tplate( uint16 * copyto , uint16 len ) const ;
	void set_tplate( const uint16 * copyfrom , uint16 len ) ;
    void get_match( uint16 * copyto , uint16 len ) const ;
	void set_match( const uint16 * copyfrom , uint16 len ) ;
};

class BCCMD_RDF_BLE_ADVERTISING_PDU : public BCCMD_PDU
{
public:
    BCCMD_RDF_BLE_ADVERTISING_PDU() ;
    BCCMD_RDF_BLE_ADVERTISING_PDU( const PDU& ) ;
    BCCMD_RDF_BLE_ADVERTISING_PDU( const uint8 * buffer , uint32 len ) ;  
    uint8 get_mode(void) const ;
	void set_mode(uint8 value) ;
    uint16 get_adv_interval(void) const ;
	void set_adv_interval(uint16 value) ;
    uint8 get_adv_type(void) const ;
	void set_adv_type(uint8 value) ;
    uint8 get_addr_type(void) const ;
	void set_addr_type(uint8 value) ;
    uint8 get_channels(void) const ;
	void set_channels(uint8 value) ;
    uint8 get_data_len(void) const ;
	void set_data_len(uint8 value) ;
    uint8 get_anti_whiten_begin(void) const ;
    void set_anti_whiten_begin(uint8 value) ;
    uint8 get_anti_whiten_end(void) const ;
	void set_anti_whiten_end(uint8 value) ;
    void get_data( uint8 * copyto , uint8 len ) const ;
	void set_data( const uint8 * copyfrom , uint8 len ) ;
};

class BCCMD_RDF_BLE_RAND_ADDR_PDU : public BCCMD_PDU
{
public:
    BCCMD_RDF_BLE_RAND_ADDR_PDU() ;
    BCCMD_RDF_BLE_RAND_ADDR_PDU( const PDU& ) ;
    BCCMD_RDF_BLE_RAND_ADDR_PDU( const uint8 * buffer , uint32 len ) ;
    void get_addr( uint8 * copyto , uint16 len ) const ;
	void set_addr( const uint8 * copyfrom , uint16 len ) ;
};

class BCCMD_OTP_STATS_PDU : public BCCMD_PDU
{
public:
    BCCMD_OTP_STATS_PDU() ;
    BCCMD_OTP_STATS_PDU( const PDU& ) ;
    BCCMD_OTP_STATS_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_len(void) const ;
    void set_len(uint16 value) ;
    void get_stats( uint16 * copyto , uint32 len ) const ;
    void set_stats( const uint16 * copyfrom , uint32 len ) ;
};

class BCCMD_BLE_ADV_FILT_ADD_PDU : public BCCMD_PDU
{
public:
    BCCMD_BLE_ADV_FILT_ADD_PDU();
    BCCMD_BLE_ADV_FILT_ADD_PDU( const PDU& );
    BCCMD_BLE_ADV_FILT_ADD_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_operation(void) const ;
    void set_operation(uint16 value) ;
    uint16 get_ad_type(void) const ;
    void set_ad_type(uint16 value) ;
    uint16 get_interval(void) const ;
    void set_interval(uint16 value) ;
    uint16 get_pattern_length(void) const ;
    void set_pattern_length(uint16 value) ;
    void get_pattern( uint16 * copyto , uint32 len ) const ;
    void set_pattern( const uint16 * copyfrom , uint32 len ) ;
};

class BCCMD_SIFLASH_PDU : public BCCMD_PDU
{
public:
    BCCMD_SIFLASH_PDU();
    BCCMD_SIFLASH_PDU( const PDU& );
    BCCMD_SIFLASH_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_command(void) const ;
    void set_command(uint16 value) ;
    uint32 get_word_address(void) const ;
    void set_word_address(uint32 value) ;
    uint16 get_data_length(void) const ;
    void set_data_length(uint16 value) ;
    void get_data( uint16 * copyto , uint32 len ) const ;
    void set_data( const uint16 * copyfrom , uint32 len ) ;
};

class BCCMD_CREATE_OPERATOR_P_PDU : public BCCMD_PDU
{
public:
    BCCMD_CREATE_OPERATOR_P_PDU();
    BCCMD_CREATE_OPERATOR_P_PDU( const PDU& );
    BCCMD_CREATE_OPERATOR_P_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_len(void) const ;
    void set_len(uint16 value) ;
    void get_patch( uint16 * copyto , uint32 len ) const ;
    void set_patch( const uint16 * copyfrom , uint32 len ) ;
};

class BCCMD_OPERATOR_MESSAGE_PDU : public BCCMD_PDU
{
public:
    BCCMD_OPERATOR_MESSAGE_PDU();
    BCCMD_OPERATOR_MESSAGE_PDU( const PDU& );
    BCCMD_OPERATOR_MESSAGE_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_opid(void) const ;
    void set_opid(uint16 value) ;
    uint16 get_arg_count(void) const ;
    void get_arguments( uint16 * copyto , uint32 len ) const ;
    void set_arguments( const uint16 * copyfrom , uint32 len ) ;
};

class BCCMD_START_OPERATOR_PDU : public BCCMD_PDU
{
public:
    BCCMD_START_OPERATOR_PDU();
    BCCMD_START_OPERATOR_PDU( const PDU& );
    BCCMD_START_OPERATOR_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_count(void) const ;
    void set_count(uint16 value) ;
    void get_list( uint16 * copyto , uint32 len ) const ;
    void set_list( const uint16 * copyfrom , uint32 len ) ;
};

class BCCMD_STOP_OPERATOR_PDU : public BCCMD_PDU
{
public:
    BCCMD_STOP_OPERATOR_PDU();
    BCCMD_STOP_OPERATOR_PDU( const PDU& );
    BCCMD_STOP_OPERATOR_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_count(void) const ;
    void set_count(uint16 value) ;
    void get_list( uint16 * copyto , uint32 len ) const ;
    void set_list( const uint16 * copyfrom , uint32 len ) ;
};

class BCCMD_RESET_OPERATOR_PDU : public BCCMD_PDU
{
public:
    BCCMD_RESET_OPERATOR_PDU();
    BCCMD_RESET_OPERATOR_PDU( const PDU& );
    BCCMD_RESET_OPERATOR_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_count(void) const ;
    void set_count(uint16 value) ;
    void get_list( uint16 * copyto , uint32 len ) const ;
    void set_list( const uint16 * copyfrom , uint32 len ) ;
};

class BCCMD_DESTROY_OPERATOR_PDU : public BCCMD_PDU
{
public:
    BCCMD_DESTROY_OPERATOR_PDU();
    BCCMD_DESTROY_OPERATOR_PDU( const PDU& );
    BCCMD_DESTROY_OPERATOR_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_count(void) const ;
    void set_count(uint16 value) ;
    void get_list( uint16 * copyto , uint32 len ) const ;
    void set_list( const uint16 * copyfrom , uint32 len ) ;
};

#endif

