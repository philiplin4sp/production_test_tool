
#include "xtracommandcompletepdus.h"
#include "app/bluestack/bluetooth.h"
#include "app/bluestack/hci.h"
#include "hcilayout.h"

#define OFFSET_START(name) name##hdr=5

enum {
	OFFSET_START(HCI_READ_LOCAL_NAME_RET_T),
	//OFFSET_UNHANDLED_uint8(HCI_READ_LOCAL_NAME_RET_T_name_part),
	HCI_READ_LOCAL_NAME_RET_T_pduSize
} ;

enum {
	OFFSET_START(HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_RET_T),
	OFFSET_uint8(HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_RET_T_fec_required),
	//OFFSET_UNHANDLED_uint8(HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_RET_T_eir_data_part),
	HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_RET_T_pduSize
} ;

enum {
	OFFSET_START(HCI_READ_LOCAL_SUPP_COMMANDS_RET_T),
	OFFSET_uint8_array(HCI_READ_LOCAL_SUPP_COMMANDS_RET_T_supp_commands, 64),
	HCI_READ_LOCAL_SUPP_COMMANDS_RET_T_pduSize
} ;

enum {
	OFFSET_START(HCI_READ_LOCAL_SUPP_FEATURES_RET_T),
	//OFFSET_UNHANDLED_uint8(HCI_READ_LOCAL_SUPP_FEATURES_RET_T_lmp_supp_features),
	OFFSET_uint32(HCI_READ_LOCAL_SUPP_FEATURES_RET_T_lmp_supp_features_part0),
	OFFSET_uint32(HCI_READ_LOCAL_SUPP_FEATURES_RET_T_lmp_supp_features_part1),
	HCI_READ_LOCAL_SUPP_FEATURES_RET_T_pduSize
} ;

enum {
	OFFSET_START(HCI_READ_CURRENT_IAC_LAP_RET_T),
	OFFSET_uint8(HCI_READ_CURRENT_IAC_LAP_RET_T_num_current_iac),
	//OFFSET_UNHANDLED_uint24(HCI_READ_CURRENT_IAC_LAP_RET_T_iac_lap),
	HCI_READ_CURRENT_IAC_LAP_RET_T_pduSize
} ;

enum {
	OFFSET_START(HCI_READ_AFH_CHANNEL_MAP_RET_T),
	OFFSET_uint16(HCI_READ_AFH_CHANNEL_MAP_RET_T_handle),
	OFFSET_uint8(HCI_READ_AFH_CHANNEL_MAP_RET_T_mode),
	OFFSET_uint8(HCI_READ_AFH_CHANNEL_MAP_RET_T_map),
	OFFSET_uint8(HCI_READ_AFH_CHANNEL_MAP_RET_T_map1),
	OFFSET_uint8(HCI_READ_AFH_CHANNEL_MAP_RET_T_map2),
	OFFSET_uint8(HCI_READ_AFH_CHANNEL_MAP_RET_T_map3),
	OFFSET_uint8(HCI_READ_AFH_CHANNEL_MAP_RET_T_map4),
	OFFSET_uint8(HCI_READ_AFH_CHANNEL_MAP_RET_T_map5),
	OFFSET_uint8(HCI_READ_AFH_CHANNEL_MAP_RET_T_map6),
	OFFSET_uint8(HCI_READ_AFH_CHANNEL_MAP_RET_T_map7),
	OFFSET_uint8(HCI_READ_AFH_CHANNEL_MAP_RET_T_map8),
	OFFSET_uint8(HCI_READ_AFH_CHANNEL_MAP_RET_T_map9),
	HCI_READ_AFH_CHANNEL_MAP_RET_T_pduSize
} ;

enum {
	OFFSET_START(HCI_READ_LOCAL_EXT_FEATURES_RET_T),
	OFFSET_uint8(HCI_READ_LOCAL_EXT_FEATURES_RET_T_page_num),
	OFFSET_uint8(HCI_READ_LOCAL_EXT_FEATURES_RET_T_max_page_num),
	OFFSET_uint32(HCI_READ_LOCAL_EXT_FEATURES_RET_T_lmp_ext_features),
	OFFSET_uint32(HCI_READ_LOCAL_EXT_FEATURES_RET_T_lmp_ext_features_p2),
	HCI_READ_LOCAL_EXT_FEATURES_RET_T_pduSize
} ;

enum {
	OFFSET_START(HCI_READ_LOCAL_OOB_DATA_RET_T),
	OFFSET_uint32(HCI_READ_LOCAL_OOB_DATA_RET_T_c_part0),
	OFFSET_uint32(HCI_READ_LOCAL_OOB_DATA_RET_T_c_part1),
	OFFSET_uint32(HCI_READ_LOCAL_OOB_DATA_RET_T_c_part2),
	OFFSET_uint32(HCI_READ_LOCAL_OOB_DATA_RET_T_c_part3),
	OFFSET_uint32(HCI_READ_LOCAL_OOB_DATA_RET_T_r_part0),
	OFFSET_uint32(HCI_READ_LOCAL_OOB_DATA_RET_T_r_part1),
	OFFSET_uint32(HCI_READ_LOCAL_OOB_DATA_RET_T_r_part2),
	OFFSET_uint32(HCI_READ_LOCAL_OOB_DATA_RET_T_r_part3),
	HCI_READ_LOCAL_OOB_DATA_RET_T_pduSize
} ;

enum {
	OFFSET_START(HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_RET_T),
	OFFSET_uint8_array(HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_RET_T_feature_set, 8),
	HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_RET_T_pduSize
} ;

enum {
	OFFSET_START(HCI_ULP_RAND_RET_T),
	OFFSET_uint8_array(HCI_ULP_RAND_RET_T_random_number, 8),
	HCI_ULP_RAND_RET_T_pduSize
} ;

enum {
	OFFSET_START(HCI_ULP_READ_SUPPORTED_STATES_RET_T),
	OFFSET_uint8_array(HCI_ULP_READ_SUPPORTED_STATES_RET_T_supported_states, 8),
	HCI_ULP_READ_SUPPORTED_STATES_RET_T_pduSize
} ;


enum {
        OFFSET_START(HCI_ULP_ENCRYPT_RET_T),
        OFFSET_uint8_array(HCI_ULP_ENCRYPT_RET_T_encrypted_data, 16),
        HCI_ULP_ENCRYPT_RET_T_pduSize
} ;


#include "hcipdu.h"
#include <stdio.h>


HCI_READ_LOCAL_NAME_RET_T_PDU::HCI_READ_LOCAL_NAME_RET_T_PDU()
:   HCICommandCompletePDU(HCI_READ_LOCAL_NAME_RET,HCI_READ_LOCAL_NAME_RET_T_pduSize)
{
}

HCI_READ_LOCAL_NAME_RET_T_PDU::HCI_READ_LOCAL_NAME_RET_T_PDU( const PDU& from )
:   HCICommandCompletePDU( from )
{
}

const BluetoothName HCI_READ_LOCAL_NAME_RET_T_PDU::get_name_part() const
{
	return BluetoothName ( get_uint8ArrayPtr(HCI_READ_LOCAL_NAME_RET_T_pduSize) , size() - HCI_READ_LOCAL_NAME_RET_T_pduSize ) ;
}

bool HCI_READ_LOCAL_NAME_RET_T_PDU::size_expected() const
{
    return size() == HCI_READ_LOCAL_NAME_RET_T_pduSize + 248u;
}

/*******************************************************************/


HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_RET_T_PDU::HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_RET_T_PDU()
:   HCICommandCompletePDU(HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_RET,HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_RET_T_pduSize)
{
}

HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_RET_T_PDU::HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_RET_T_PDU( const PDU& from )
:   HCICommandCompletePDU( from )
{
}

bool HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_RET_T_PDU::get_fec_required() const
{
	return (bool) get_uint8(HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_RET_T_fec_required);
}

const EIRData HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_RET_T_PDU::get_eir_data_part() const
{
	return EIRData ( get_uint8ArrayPtr(HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_RET_T_pduSize) ,
                size() - HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_RET_T_pduSize ) ;
}

bool HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_RET_T_PDU::size_expected() const
{
    return size() == HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_RET_T_pduSize + 240u;
}

/*******************************************************************/

HCI_READ_LOCAL_SUPP_FEATURES_RET_T_PDU::HCI_READ_LOCAL_SUPP_FEATURES_RET_T_PDU()
:   HCICommandCompletePDU(HCI_READ_LOCAL_SUPP_FEATURES_RET,HCI_READ_LOCAL_SUPP_FEATURES_RET_T_pduSize)
{
}

HCI_READ_LOCAL_SUPP_FEATURES_RET_T_PDU::HCI_READ_LOCAL_SUPP_FEATURES_RET_T_PDU( const PDU& from )
:   HCICommandCompletePDU(from)
{
}

const LMPSupportedFeatures HCI_READ_LOCAL_SUPP_FEATURES_RET_T_PDU::get_lmp_supp_features() const
{
	uint8 a[8] ;
	get_uint8Array(a,HCI_READ_LOCAL_SUPP_FEATURES_RET_T_lmp_supp_features_part0,8) ;
	LMPSupportedFeatures f(a) ; ;
	return f ;
}

bool HCI_READ_LOCAL_SUPP_FEATURES_RET_T_PDU::size_expected() const
{
    return size() == HCI_READ_LOCAL_SUPP_FEATURES_RET_T_pduSize;
}


/*******************************************************************/


HCI_READ_LOCAL_SUPP_COMMANDS_RET_T_PDU::HCI_READ_LOCAL_SUPP_COMMANDS_RET_T_PDU()
:   HCICommandCompletePDU(HCI_READ_LOCAL_SUPP_COMMANDS_RET,HCI_READ_LOCAL_SUPP_COMMANDS_RET_T_pduSize)
{
}

HCI_READ_LOCAL_SUPP_COMMANDS_RET_T_PDU::HCI_READ_LOCAL_SUPP_COMMANDS_RET_T_PDU( const PDU& from )
:   HCICommandCompletePDU(from)
{
}

const SupportedCommands HCI_READ_LOCAL_SUPP_COMMANDS_RET_T_PDU::get_supp_commands() const
{
	uint8 a[64];
	get_uint8Array(a, HCI_READ_LOCAL_SUPP_COMMANDS_RET_T_supp_commands, 64);
	SupportedCommands f(a) ; ;
	return f ;
}

bool HCI_READ_LOCAL_SUPP_COMMANDS_RET_T_PDU::size_expected() const
{
    return size() == HCI_READ_LOCAL_SUPP_COMMANDS_RET_T_pduSize;
}


/*******************************************************************/


HCI_READ_CURRENT_IAC_LAP_RET_T_PDU::HCI_READ_CURRENT_IAC_LAP_RET_T_PDU()
:   HCICommandCompletePDU(HCI_READ_CURRENT_IAC_LAP_RET,HCI_READ_CURRENT_IAC_LAP_RET_T_pduSize)
{
}

HCI_READ_CURRENT_IAC_LAP_RET_T_PDU::HCI_READ_CURRENT_IAC_LAP_RET_T_PDU( const PDU& from )
:   HCICommandCompletePDU(from)
{
}

uint8 HCI_READ_CURRENT_IAC_LAP_RET_T_PDU::get_num_current_iac() const
{
	return get_uint8(HCI_READ_CURRENT_IAC_LAP_RET_T_num_current_iac) ;
}


uint24 HCI_READ_CURRENT_IAC_LAP_RET_T_PDU::get_iac_lap ( uint8 index ) const
{
	uint16 offset = HCI_READ_CURRENT_IAC_LAP_RET_T_pduSize ;
	offset += index *3 ;
	return get_uint24(offset) ;
}

bool HCI_READ_CURRENT_IAC_LAP_RET_T_PDU::size_expected() const
{
    return size() >  HCI_READ_CURRENT_IAC_LAP_RET_T_num_current_iac
        && size() == HCI_READ_CURRENT_IAC_LAP_RET_T_pduSize + (get_num_current_iac() * 3u);
}


/*******************************************************************/

HCI_READ_AFH_CHANNEL_MAP_RET_T_PDU::HCI_READ_AFH_CHANNEL_MAP_RET_T_PDU()
:   HCICommandCompletePDU(HCI_READ_AFH_CHANNEL_MAP_RET,HCI_READ_AFH_CHANNEL_MAP_RET_T_pduSize)
{
}


HCI_READ_AFH_CHANNEL_MAP_RET_T_PDU::HCI_READ_AFH_CHANNEL_MAP_RET_T_PDU( const PDU& from )
:   HCICommandCompletePDU(from)
{
}

HCI_READ_AFH_CHANNEL_MAP_RET_T_PDU::HCI_READ_AFH_CHANNEL_MAP_RET_T_PDU( const uint8* data , uint32 length )
:   HCICommandCompletePDU(data,length)
{
}

uint16 HCI_READ_AFH_CHANNEL_MAP_RET_T_PDU::get_handle() const
{
    return get_uint16 ( HCI_READ_AFH_CHANNEL_MAP_RET_T_handle );
}

uint8 HCI_READ_AFH_CHANNEL_MAP_RET_T_PDU::get_mode() const
{
    return get_uint8 ( HCI_READ_AFH_CHANNEL_MAP_RET_T_mode );
}

AFHmap HCI_READ_AFH_CHANNEL_MAP_RET_T_PDU::get_map() const
{
    return AFHmap ( get_uint8ArrayPtr ( HCI_READ_AFH_CHANNEL_MAP_RET_T_map ) );
}

bool HCI_READ_AFH_CHANNEL_MAP_RET_T_PDU::size_expected() const
{
    return size() == HCI_READ_AFH_CHANNEL_MAP_RET_T_pduSize;
}

/*******************************************************************/


HCI_READ_LOCAL_EXT_FEATURES_RET_T_PDU::HCI_READ_LOCAL_EXT_FEATURES_RET_T_PDU()
:   HCICommandCompletePDU ( HCI_READ_LOCAL_EXT_FEATURES , HCI_READ_LOCAL_EXT_FEATURES_RET_T_pduSize )
{	
}

HCI_READ_LOCAL_EXT_FEATURES_RET_T_PDU::HCI_READ_LOCAL_EXT_FEATURES_RET_T_PDU ( const PDU& a )
:   HCICommandCompletePDU ( a )
{	
}

HCI_READ_LOCAL_EXT_FEATURES_RET_T_PDU::HCI_READ_LOCAL_EXT_FEATURES_RET_T_PDU ( const uint8 * data , uint32 len )
:   HCICommandCompletePDU ( data , len )
{	
}

uint8 HCI_READ_LOCAL_EXT_FEATURES_RET_T_PDU::get_page_num(void) const
{
    return get_uint8(HCI_READ_LOCAL_EXT_FEATURES_RET_T_page_num) ;
}

uint8 HCI_READ_LOCAL_EXT_FEATURES_RET_T_PDU::get_max_page_num(void) const
{
    return get_uint8(HCI_READ_LOCAL_EXT_FEATURES_RET_T_max_page_num) ;
}

const LMPSupportedFeatures HCI_READ_LOCAL_EXT_FEATURES_RET_T_PDU::get_lmp_ext_features(void) const
{
	uint8 a[8] ;
	get_uint8Array(a,HCI_READ_LOCAL_EXT_FEATURES_RET_T_lmp_ext_features,8) ;
	LMPSupportedFeatures f(a) ; ;
	return f ;
}

bool HCI_READ_LOCAL_EXT_FEATURES_RET_T_PDU::size_expected() const
{
    return size() == HCI_READ_LOCAL_EXT_FEATURES_RET_T_pduSize;
}


/*******************************************************************/


HCI_READ_LOCAL_OOB_DATA_RET_T_PDU::HCI_READ_LOCAL_OOB_DATA_RET_T_PDU()
:   HCICommandCompletePDU ( HCI_READ_LOCAL_OOB_DATA , HCI_READ_LOCAL_OOB_DATA_RET_T_pduSize )
{	
}

HCI_READ_LOCAL_OOB_DATA_RET_T_PDU::HCI_READ_LOCAL_OOB_DATA_RET_T_PDU ( const PDU& a )
:   HCICommandCompletePDU ( a )
{	
}

HCI_READ_LOCAL_OOB_DATA_RET_T_PDU::HCI_READ_LOCAL_OOB_DATA_RET_T_PDU ( const uint8 * data , uint32 len )
:   HCICommandCompletePDU ( data , len )
{	
}

const SPhashC HCI_READ_LOCAL_OOB_DATA_RET_T_PDU::get_c(void) const
{
	return SPhashC(get_uint8ArrayPtr(HCI_READ_LOCAL_OOB_DATA_RET_T_c_part0)) ;
}

const SPrandomizerR HCI_READ_LOCAL_OOB_DATA_RET_T_PDU::get_r(void) const
{
	return SPrandomizerR(get_uint8ArrayPtr(HCI_READ_LOCAL_OOB_DATA_RET_T_r_part0)) ;
}

bool HCI_READ_LOCAL_OOB_DATA_RET_T_PDU::size_expected() const
{
    return size() == HCI_READ_LOCAL_OOB_DATA_RET_T_pduSize;
}


/*******************************************************************/

HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_RET_T_PDU::HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_RET_T_PDU()
:   HCICommandCompletePDU ( HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES , HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_RET_T_pduSize )
{	
}

HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_RET_T_PDU::HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_RET_T_PDU ( const PDU& a )
:   HCICommandCompletePDU ( a )
{	
}

HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_RET_T_PDU::HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_RET_T_PDU ( const uint8 * data , uint32 len )
:   HCICommandCompletePDU ( data , len )
{	
}

ULPSupportedFeatures HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_RET_T_PDU::get_feature_set(void) const
{
	uint8 a[8];
	get_uint8Array(a,HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_RET_T_feature_set,8) ;
	ULPSupportedFeatures f(a);
	return f;
}

bool HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_RET_T_PDU::size_expected() const
{
    return size() == HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_RET_T_pduSize;
}

/*******************************************************************/

HCI_ULP_RAND_RET_T_PDU::HCI_ULP_RAND_RET_T_PDU()
:   HCICommandCompletePDU ( HCI_ULP_RAND , HCI_ULP_RAND_RET_T_pduSize )
{	
}

HCI_ULP_RAND_RET_T_PDU::HCI_ULP_RAND_RET_T_PDU ( const PDU& a )
:   HCICommandCompletePDU ( a )
{	
}

HCI_ULP_RAND_RET_T_PDU::HCI_ULP_RAND_RET_T_PDU ( const uint8 * data , uint32 len )
:   HCICommandCompletePDU ( data , len )
{	
}

DATA_uint8_len_32_ret HCI_ULP_RAND_RET_T_PDU::get_random_number(void) const
{
    return DATA_uint8_len_32_ret(get_uint8ArrayPtr(HCI_ULP_ENCRYPT_RET_T_encrypted_data)) ;
}

bool HCI_ULP_RAND_RET_T_PDU::size_expected() const
{
    return size() == HCI_ULP_RAND_RET_T_pduSize;
}

/*******************************************************************/

HCI_ULP_READ_SUPPORTED_STATES_RET_T_PDU::HCI_ULP_READ_SUPPORTED_STATES_RET_T_PDU()
:   HCICommandCompletePDU ( HCI_ULP_READ_SUPPORTED_STATES , HCI_ULP_READ_SUPPORTED_STATES_RET_T_pduSize )
{	
}

HCI_ULP_READ_SUPPORTED_STATES_RET_T_PDU::HCI_ULP_READ_SUPPORTED_STATES_RET_T_PDU ( const PDU& a )
:   HCICommandCompletePDU ( a )
{	
}

HCI_ULP_READ_SUPPORTED_STATES_RET_T_PDU::HCI_ULP_READ_SUPPORTED_STATES_RET_T_PDU ( const uint8 * data , uint32 len )
:   HCICommandCompletePDU ( data , len )
{	
}

ULPSupportedStates HCI_ULP_READ_SUPPORTED_STATES_RET_T_PDU::get_supported_states(void) const
{
	uint8 a[8];
	get_uint8Array(a,HCI_ULP_READ_SUPPORTED_STATES_RET_T_supported_states,8) ;
	ULPSupportedStates f(a);
	return f;
}

bool HCI_ULP_READ_SUPPORTED_STATES_RET_T_PDU::size_expected() const
{
    return size() == HCI_ULP_READ_SUPPORTED_STATES_RET_T_pduSize;
}

/*******************************************************************/


HCI_ULP_ENCRYPT_RET_T_PDU::HCI_ULP_ENCRYPT_RET_T_PDU()
:   HCICommandCompletePDU ( HCI_ULP_ENCRYPT , HCI_ULP_ENCRYPT_RET_T_pduSize )
{	
}

HCI_ULP_ENCRYPT_RET_T_PDU::HCI_ULP_ENCRYPT_RET_T_PDU ( const PDU& a )
:   HCICommandCompletePDU ( a )
{	
}

HCI_ULP_ENCRYPT_RET_T_PDU::HCI_ULP_ENCRYPT_RET_T_PDU ( const uint8 * data , uint32 len )
:   HCICommandCompletePDU ( data , len )
{	
}

const DATA_uint8_len_16_ret HCI_ULP_ENCRYPT_RET_T_PDU::get_encrypted_data(void) const
{
	return DATA_uint8_len_16_ret(get_uint8ArrayPtr(HCI_ULP_ENCRYPT_RET_T_encrypted_data)) ;
}

bool HCI_ULP_ENCRYPT_RET_T_PDU::size_expected() const
{
    return size() == HCI_ULP_ENCRYPT_RET_T_pduSize;
}


/*******************************************************************/
enum {
    OFFSET_START(HCI_ULP_READ_CHANNEL_MAP_RET_T),
    OFFSET_uint16(HCI_ULP_READ_CHANNEL_MAP_RET_T_handle),
    OFFSET_uint8(HCI_ULP_READ_CHANNEL_MAP_RET_T_map),
    OFFSET_uint8(HCI_ULP_READ_CHANNEL_MAP_RET_T_map1),
    OFFSET_uint8(HCI_ULP_READ_CHANNEL_MAP_RET_T_map2),
    OFFSET_uint8(HCI_ULP_READ_CHANNEL_MAP_RET_T_map3),
    OFFSET_uint8(HCI_ULP_READ_CHANNEL_MAP_RET_T_map4),
    HCI_ULP_READ_CHANNEL_MAP_RET_T_pduSize
} ;

HCI_ULP_READ_CHANNEL_MAP_RET_T_PDU::HCI_ULP_READ_CHANNEL_MAP_RET_T_PDU()
:   HCICommandCompletePDU ( HCI_ULP_READ_CHANNEL_MAP , HCI_ULP_READ_CHANNEL_MAP_RET_T_pduSize )
{
}

HCI_ULP_READ_CHANNEL_MAP_RET_T_PDU::HCI_ULP_READ_CHANNEL_MAP_RET_T_PDU ( const PDU& a )
:   HCICommandCompletePDU ( a )
{
}

HCI_ULP_READ_CHANNEL_MAP_RET_T_PDU::HCI_ULP_READ_CHANNEL_MAP_RET_T_PDU ( const uint8 * data , uint32 len )
:   HCICommandCompletePDU ( data , len )
{
}

uint16 HCI_ULP_READ_CHANNEL_MAP_RET_T_PDU::get_connection_handle() const
{
    return get_uint16 ( HCI_ULP_READ_CHANNEL_MAP_RET_T_handle );
}

ULPChannelMap HCI_ULP_READ_CHANNEL_MAP_RET_T_PDU::get_ulp_channel_map() const
{
    return ULPChannelMap ( get_uint8ArrayPtr ( HCI_ULP_READ_CHANNEL_MAP_RET_T_map ) );
}

bool HCI_ULP_READ_CHANNEL_MAP_RET_T_PDU::size_expected() const
{
    return size() == HCI_ULP_READ_CHANNEL_MAP_RET_T_pduSize;
}

/*******************************************************************/
