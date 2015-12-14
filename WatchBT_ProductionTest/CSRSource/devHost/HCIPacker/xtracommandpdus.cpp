///////////////////////////////////////////////////////////////////////////////
//
//  NAME:     xtracommandpdu.cpp
//
//  PURPOSE:  define non-autogenable command pdu classes.
//
///////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include "xtracommandpdus.h"
#include "app/bluestack/bluetooth.h"
#include "app/bluestack/hci.h"
#include "hcilayout.h"

#define OFFSET_START(name) 

enum {
    OFFSET_START(HCI_PIN_CODE_REQ_REPLY_T)
    OFFSET_uint16(HCI_PIN_CODE_REQ_REPLY_T_op_code),
    OFFSET_uint8(HCI_PIN_CODE_REQ_REPLY_T_length),
    OFFSET_BluetoothDeviceAddress(HCI_PIN_CODE_REQ_REPLY_bd_addr),
    OFFSET_uint8(HCI_PIN_CODE_REQ_REPLY_T_pin_code_length),
    OFFSET_uint32(HCI_PIN_CODE_REQ_REPLY_T_pin_start),
    OFFSET_uint32(HCI_PIN_CODE_REQ_REPLY_T_pin_cont0),
    OFFSET_uint32(HCI_PIN_CODE_REQ_REPLY_T_pin_cont1),
    OFFSET_uint32(HCI_PIN_CODE_REQ_REPLY_T_pin_cont2),
    HCI_PIN_CODE_REQ_REPLY_T_pduSize
} ;

HCI_PIN_CODE_REQ_REPLY_T_PDU::HCI_PIN_CODE_REQ_REPLY_T_PDU()
: HCICommandPDU ( HCI_PIN_CODE_REQ_REPLY , HCI_PIN_CODE_REQ_REPLY_T_pduSize )
{
}

HCI_PIN_CODE_REQ_REPLY_T_PDU::HCI_PIN_CODE_REQ_REPLY_T_PDU( const PDU& from )
: HCICommandPDU ( from )
{
}

HCI_PIN_CODE_REQ_REPLY_T_PDU::HCI_PIN_CODE_REQ_REPLY_T_PDU( const uint8* data , uint32 len )
: HCICommandPDU ( data , len )
{
}

const BluetoothDeviceAddress HCI_PIN_CODE_REQ_REPLY_T_PDU::get_bd_addr() const
{
    return get_BluetoothDeviceAddress(HCI_PIN_CODE_REQ_REPLY_bd_addr);
}

void HCI_PIN_CODE_REQ_REPLY_T_PDU::set_bd_addr ( const BluetoothDeviceAddress& data )
{
    set_BluetoothDeviceAddress(HCI_PIN_CODE_REQ_REPLY_bd_addr,data);
}

uint8 HCI_PIN_CODE_REQ_REPLY_T_PDU::get_pin_code_length(void) const
{
    return get_uint8(HCI_PIN_CODE_REQ_REPLY_T_pin_code_length) ;
}

void HCI_PIN_CODE_REQ_REPLY_T_PDU::set_pin_code_length(uint8 val)
{
    set_uint8(HCI_PIN_CODE_REQ_REPLY_T_pin_code_length,val) ;
}

const PinCode HCI_PIN_CODE_REQ_REPLY_T_PDU::get_pin(void) const
{
    uint8 pinData[16] ;
    get_uint8Array(pinData,HCI_PIN_CODE_REQ_REPLY_T_pin_start,16) ;
    return PinCode(pinData,get_pin_code_length()) ;
}

void HCI_PIN_CODE_REQ_REPLY_T_PDU::set_pin( const PinCode& val )
{
    set_uint8Array(val.get_data(),HCI_PIN_CODE_REQ_REPLY_T_pin_start,16) ;
}

/*******************************************************************/

enum {
    OFFSET_START(HCI_LINK_KEY_REQ_REPLY_T)
    OFFSET_uint16(HCI_LINK_KEY_REQ_REPLY_T_op_code),
    OFFSET_uint8(HCI_LINK_KEY_REQ_REPLY_T_length),
    OFFSET_BluetoothDeviceAddress(HCI_LINK_KEY_REQ_REPLY_T_bd_addr),
    OFFSET_uint32(HCI_LINK_KEY_REQ_REPLY_T_key_val_part0),
    OFFSET_uint32(HCI_LINK_KEY_REQ_REPLY_T_key_val_part1),
    OFFSET_uint32(HCI_LINK_KEY_REQ_REPLY_T_key_val_part2),
    OFFSET_uint32(HCI_LINK_KEY_REQ_REPLY_T_key_val_part3),
    HCI_LINK_KEY_REQ_REPLY_T_pduSize
} ;



HCI_LINK_KEY_REQ_REPLY_T_PDU::HCI_LINK_KEY_REQ_REPLY_T_PDU()
:   HCICommandPDU(HCI_LINK_KEY_REQ_REPLY,HCI_LINK_KEY_REQ_REPLY_T_pduSize)
{
}

HCI_LINK_KEY_REQ_REPLY_T_PDU::HCI_LINK_KEY_REQ_REPLY_T_PDU( const PDU& from )
:   HCICommandPDU( from )
{
}

HCI_LINK_KEY_REQ_REPLY_T_PDU::HCI_LINK_KEY_REQ_REPLY_T_PDU( const uint8* data , uint32 len )
:   HCICommandPDU( data , len )
{
}

BluetoothDeviceAddress HCI_LINK_KEY_REQ_REPLY_T_PDU::get_bd_addr() const
{
    return get_BluetoothDeviceAddress(HCI_LINK_KEY_REQ_REPLY_T_bd_addr) ;
}

void HCI_LINK_KEY_REQ_REPLY_T_PDU::set_bd_addr(BluetoothDeviceAddress val)
{
    set_BluetoothDeviceAddress(HCI_LINK_KEY_REQ_REPLY_T_bd_addr,val) ;
}

LinkKey HCI_LINK_KEY_REQ_REPLY_T_PDU::get_key_val() const
{
    return LinkKey (get_uint8ArrayPtr(HCI_LINK_KEY_REQ_REPLY_T_key_val_part0)) ;
}

void HCI_LINK_KEY_REQ_REPLY_T_PDU::set_key_val( const LinkKey& val)
{
    set_uint8Array(val.get_data(),HCI_LINK_KEY_REQ_REPLY_T_key_val_part0,16) ;
}

/*******************************************************************/

enum {
    OFFSET_START(HCI_SET_AFH_CHANNEL_CLASS_T)
    OFFSET_uint16(HCI_SET_AFH_CHANNEL_CLASS_T_op_code),
    OFFSET_uint8(HCI_SET_AFH_CHANNEL_CLASS_T_length),
    OFFSET_uint8(HCI_SET_AFH_CHANNEL_CLASS_T_map),
    OFFSET_uint8(HCI_SET_AFH_CHANNEL_CLASS_T_map1),
    OFFSET_uint8(HCI_SET_AFH_CHANNEL_CLASS_T_map2),
    OFFSET_uint8(HCI_SET_AFH_CHANNEL_CLASS_T_map3),
    OFFSET_uint8(HCI_SET_AFH_CHANNEL_CLASS_T_map4),
    OFFSET_uint8(HCI_SET_AFH_CHANNEL_CLASS_T_map5),
    OFFSET_uint8(HCI_SET_AFH_CHANNEL_CLASS_T_map6),
    OFFSET_uint8(HCI_SET_AFH_CHANNEL_CLASS_T_map7),
    OFFSET_uint8(HCI_SET_AFH_CHANNEL_CLASS_T_map8),
    OFFSET_uint8(HCI_SET_AFH_CHANNEL_CLASS_T_map9),
    HCI_SET_AFH_CHANNEL_CLASS_T_pduSize
} ;

HCI_SET_AFH_CHANNEL_CLASS_T_PDU::HCI_SET_AFH_CHANNEL_CLASS_T_PDU()
:   HCICommandPDU(HCI_SET_AFH_CHANNEL_CLASS,HCI_SET_AFH_CHANNEL_CLASS_T_pduSize)
{}

HCI_SET_AFH_CHANNEL_CLASS_T_PDU::HCI_SET_AFH_CHANNEL_CLASS_T_PDU( const PDU& from )
:   HCICommandPDU( from )
{}

HCI_SET_AFH_CHANNEL_CLASS_T_PDU::HCI_SET_AFH_CHANNEL_CLASS_T_PDU( const uint8* data , uint32 len )
:   HCICommandPDU( data , len )
{}

AFHmap HCI_SET_AFH_CHANNEL_CLASS_T_PDU::get_map() const
{
    return AFHmap ( get_uint8ArrayPtr(HCI_SET_AFH_CHANNEL_CLASS_T_map) );
}

void HCI_SET_AFH_CHANNEL_CLASS_T_PDU::set_map( const AFHmap& data)
{
    set_uint8Array ( data.get_ptr() , HCI_SET_AFH_CHANNEL_CLASS_T_map , 10 );
}

/*******************************************************************/

enum {
	OFFSET_START(HCI_MNFR_EXTENSION_T)
	OFFSET_uint16(HCI_MNFR_EXTENSION_T_op_code),
	OFFSET_uint8(HCI_MNFR_EXTENSION_T_cmd_parameter_length),
	OFFSET_uint8(HCI_MNFR_EXTENSION_T_payload_descriptor),
	HCI_MNFR_EXTENSION_T_pduSize
} ;

HCI_MNFR_EXTENSION_T_PDU::HCI_MNFR_EXTENSION_T_PDU()
: HCICommandPDU ( HCI_MNFR_EXTENSION , HCI_MNFR_EXTENSION_T_pduSize )
{
}

HCI_MNFR_EXTENSION_T_PDU::HCI_MNFR_EXTENSION_T_PDU( const PDU& from )
: HCICommandPDU (from)
{
}

HCI_MNFR_EXTENSION_T_PDU::HCI_MNFR_EXTENSION_T_PDU( const uint8* data , uint32 len )
: HCICommandPDU ( data , len )
{
}

uint8 HCI_MNFR_EXTENSION_T_PDU::get_payload_descriptor(void) const
{
    return get_uint8(HCI_MNFR_EXTENSION_T_payload_descriptor) ;
}

void HCI_MNFR_EXTENSION_T_PDU::set_payload_descriptor(uint8 val)
{
    set_uint8(HCI_MNFR_EXTENSION_T_payload_descriptor,val) ;
}

uint32 HCI_MNFR_EXTENSION_T_PDU::get_payload_length() const
{
    return size() - HCI_MNFR_EXTENSION_T_pduSize;
}

const uint8 * HCI_MNFR_EXTENSION_T_PDU::get_payload() const
{
    return get_uint8ArrayPtr(HCI_MNFR_EXTENSION_T_pduSize) ;
}

void HCI_MNFR_EXTENSION_T_PDU::set_payload( const uint8 * data , uint32 len )
{
    resize ( (uint16) (HCI_MNFR_EXTENSION_T_pduSize + len) );
    set_uint8Array ( data , HCI_MNFR_EXTENSION_T_pduSize , len );
}

/*******************************************************************/

enum {
    OFFSET_START(HCI_CHANGE_LOCAL_NAME_T)
    OFFSET_uint16(HCI_CHANGE_LOCAL_NAME_T_op_code),
    OFFSET_uint8(HCI_CHANGE_LOCAL_NAME_T_length),
    HCI_CHANGE_LOCAL_NAME_T_name_part,
    HCI_CHANGE_LOCAL_NAME_T_pduSize = 251
} ;

HCI_CHANGE_LOCAL_NAME_T_PDU::HCI_CHANGE_LOCAL_NAME_T_PDU()
: HCICommandPDU(HCI_CHANGE_LOCAL_NAME,HCI_CHANGE_LOCAL_NAME_T_pduSize)
{
}

HCI_CHANGE_LOCAL_NAME_T_PDU::HCI_CHANGE_LOCAL_NAME_T_PDU( const PDU& from )
: HCICommandPDU(from)
{
}

HCI_CHANGE_LOCAL_NAME_T_PDU::HCI_CHANGE_LOCAL_NAME_T_PDU( const uint8 * data , uint32 size )
: HCICommandPDU(data,size)
{
}

BluetoothName HCI_CHANGE_LOCAL_NAME_T_PDU::get_name_part(void) const
{
    return BluetoothName ( PDU::get_uint8ArrayPtr ( HCI_CHANGE_LOCAL_NAME_T_name_part), size() - HCI_CHANGE_LOCAL_NAME_T_name_part ) ;
}

void HCI_CHANGE_LOCAL_NAME_T_PDU::set_name_part(BluetoothName name)
{
    PDU::set_uint8Array ( name.get_str() , HCI_CHANGE_LOCAL_NAME_T_name_part , 248 );
}

/*******************************************************************/

enum {
    OFFSET_START(HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T)
    OFFSET_uint16(HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T_op_code),
    OFFSET_uint8(HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T_length),
    OFFSET_uint8(HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T_fec_required),
    HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T_eir_data_part,
    HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T_pduSize = 244
} ;

HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T_PDU::HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T_PDU()
: HCICommandPDU(HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA,HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T_pduSize)
{
}

HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T_PDU::HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T_PDU( const PDU& from )
: HCICommandPDU(from)
{
}

HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T_PDU::HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T_PDU( const uint8 * data , uint32 size )
: HCICommandPDU(data,size)
{
//    ASSERT (size == 241);
}

uint8 HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T_PDU::get_fec_required(void) const
{
    return get_uint8(HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T_fec_required) ;
}

void HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T_PDU::set_fec_required(uint8 val)
{
    set_uint8(HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T_fec_required,val) ;
}

EIRData HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T_PDU::get_eir_data_part(void) const
{
    return EIRData ( PDU::get_uint8ArrayPtr ( HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T_eir_data_part),
            size() - HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T_eir_data_part ) ;
}

void HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T_PDU::set_eir_data_part(EIRData data)
{
    PDU::set_uint8Array ( data.get_data() , HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T_eir_data_part , 240 );
}



/*******************************************************************/

enum {
    OFFSET_START(HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T)
    OFFSET_uint16(HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_op_code),
    OFFSET_uint8(HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_length),
    OFFSET_BluetoothDeviceAddress(HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_bd_addr),
    OFFSET_uint32(HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_c_part0),
    OFFSET_uint32(HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_c_part1),
    OFFSET_uint32(HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_c_part2),
    OFFSET_uint32(HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_c_part3),
    OFFSET_uint32(HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_r_part0),
    OFFSET_uint32(HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_r_part1),
    OFFSET_uint32(HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_r_part2),
    OFFSET_uint32(HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_r_part3),
    HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_pduSize
} ;

HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_PDU::HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_PDU()
:   HCICommandPDU(HCI_REMOTE_OOB_DATA_REQUEST_REPLY,HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_pduSize)
{
}

HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_PDU::HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_PDU( const PDU& from )
:   HCICommandPDU( from )
{
}

HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_PDU::HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_PDU( const uint8* data , uint32 len )
:   HCICommandPDU( data , len )
{
}

BluetoothDeviceAddress HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_PDU::get_bd_addr() const
{
    return get_BluetoothDeviceAddress(HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_bd_addr) ;
}

void HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_PDU::set_bd_addr(BluetoothDeviceAddress val)
{
    set_BluetoothDeviceAddress(HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_bd_addr,val) ;
}

SPhashC HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_PDU::get_c() const
{
	return SPhashC(get_uint8ArrayPtr(HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_c_part0)) ;
}

void HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_PDU::set_c( const SPhashC& data) 
{
	set_uint8Array(data.get_data(),HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_c_part0,16) ;
}

SPrandomizerR HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_PDU::get_r() const
{
	return SPrandomizerR(get_uint8ArrayPtr(HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_r_part0)) ;
}

void HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_PDU::set_r( const SPrandomizerR& data)
{
	set_uint8Array(data.get_data(),HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_r_part0,16) ;
}

/*******************************************************************/

bool HCI_SET_EVENT_FILTER_T_PDU::build ( const uint32 * const parameters )
{
    bool ok = true;
    assert ( HCI_SET_EVENT_FILTER == parameters[1] );
    const uint32 * ptr = &parameters[2];
    const uint32 * const end = parameters + parameters[0];
    if ( ptr < end )
    {
        uint8 ft  = (uint8)( ptr[0] & 0xFF );
        set_filter_type ( ft );
        if ( ptr + 1 < end )
        {
            uint8 fct = (uint8)( ptr[1] & 0xFF );
            switch ( ft )
            {
            case 0:
                break;
            case 1:
            case 2:
                set_filter_condition_type ( fct );
                switch ( fct )
                {
                case 0:
                    ptr += 2;
                    break;
                case 1:
                    if ( ptr + 3 < end )
                    {
                        set_condition ( ClassOfDevice ( ptr[2] ) , ClassOfDevice ( ptr[3] ) );
                        ptr += 4;
                    }
                    else
                        ok = false;
                    break;
                case 2:
                    if ( ptr + 4 < end )
                    {
                        set_condition ( BluetoothDeviceAddress ( (uint16)ptr[4] , (uint8)ptr[3] , ptr[2] ) );
                        ptr += 5;
                    }
                    else
                        ok = false;
                    break;
                default:
                    ok = false;
                    break;
                }
                if ( ft == 2 && ptr < end )
                {
                    // tack the auto accept parameter on the end.
                    set_condition ( (uint8)*ptr++ );
                }
                break;
            default:
                ok = false;
                break;
            }
        }
    }
    return ok;
}

enum {
	OFFSET_START(HCI_ULP_SET_ADVERTISING_DATA_T)
	OFFSET_uint16(HCI_ULP_SET_ADVERTISING_DATA_T_op_code),
	OFFSET_uint8(HCI_ULP_SET_ADVERTISING_DATA_T_cmd_parameter_length),
	OFFSET_uint8(HCI_ULP_SET_ADVERTISING_DATA_T_advertising_data_len),
	HCI_ULP_SET_ADVERTISING_DATA_T_advertising_data,
	HCI_ULP_SET_ADVERTISING_DATA_T_pduSize = 35
} ;

enum {
	OFFSET_START(HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY_T)
	OFFSET_uint16(HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY_T_op_code),
    OFFSET_uint8(HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY_T_cmd_parameter_length),
	OFFSET_uint16(HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY_T_connection_handle),
	HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY_T_long_term_key,
	HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY_T_pduSize = 21
} ;

enum {
	OFFSET_START(HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_T)
	OFFSET_uint16(HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_T_op_code),
	OFFSET_uint8(HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_T_cmd_parameter_length),
	OFFSET_uint8(HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_T_channel_map),
	OFFSET_uint8(HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_T_channel_map1),
	OFFSET_uint8(HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_T_channel_map2),
	OFFSET_uint8(HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_T_channel_map3),
	OFFSET_uint8(HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_T_channel_map4),
	HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_T_pduSize
} ;

enum {
	OFFSET_START(HCI_ULP_SET_SCAN_RESPONSE_DATA_T)
	OFFSET_uint16(HCI_ULP_SET_SCAN_RESPONSE_DATA_T_op_code),
	OFFSET_uint8(HCI_ULP_SET_SCAN_RESPONSE_DATA_T_cmd_parameter_length),
	OFFSET_uint8(HCI_ULP_SET_SCAN_RESPONSE_DATA_T_scan_response_data_len),
	HCI_ULP_SET_SCAN_RESPONSE_DATA_T_scan_response_data,
	HCI_ULP_SET_SCAN_RESPONSE_DATA_T_pduSize = 35
} ;

/*******************************************************************/


HCI_ULP_SET_ADVERTISING_DATA_T_PDU::HCI_ULP_SET_ADVERTISING_DATA_T_PDU()
: HCICommandPDU ( HCI_ULP_SET_ADVERTISING_DATA , HCI_ULP_SET_ADVERTISING_DATA_T_pduSize )
{
}

HCI_ULP_SET_ADVERTISING_DATA_T_PDU::HCI_ULP_SET_ADVERTISING_DATA_T_PDU ( const PDU& from )
: HCICommandPDU ( from )
{
}

HCI_ULP_SET_ADVERTISING_DATA_T_PDU::HCI_ULP_SET_ADVERTISING_DATA_T_PDU ( const uint8* data , uint32 len )
: HCICommandPDU( data , len )
{
}

uint8 HCI_ULP_SET_ADVERTISING_DATA_T_PDU::get_advertising_data_len(void) const
{
    return get_uint8(HCI_ULP_SET_ADVERTISING_DATA_T_advertising_data_len) ;
}

void HCI_ULP_SET_ADVERTISING_DATA_T_PDU::set_advertising_data_len(uint8 val)
{
    set_uint8(HCI_ULP_SET_ADVERTISING_DATA_T_advertising_data_len,val) ;
}

BluetoothName HCI_ULP_SET_ADVERTISING_DATA_T_PDU::get_advertising_data(void) const
{
	return BluetoothName ( PDU::get_uint8ArrayPtr ( HCI_ULP_SET_ADVERTISING_DATA_T_advertising_data), size() - HCI_ULP_SET_ADVERTISING_DATA_T_advertising_data ) ;
}

void HCI_ULP_SET_ADVERTISING_DATA_T_PDU::set_advertising_data(BluetoothName val)
{
	PDU::set_uint8Array ( val.get_str() , HCI_ULP_SET_ADVERTISING_DATA_T_advertising_data , get_advertising_data_len() );
}

/*******************************************************************/

HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY_T_PDU::HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY_T_PDU()
: HCICommandPDU ( HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY , HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY_T_pduSize )
{
}

HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY_T_PDU::HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY_T_PDU ( const PDU& from )
: HCICommandPDU ( from )
{
}

HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY_T_PDU::HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY_T_PDU ( const uint8* data , uint32 len )
: HCICommandPDU( data , len )
{
}

uint16 HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY_T_PDU::get_connection_handle( void ) const
{
    return get_uint16( HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY_T_connection_handle );
}

void HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY_T_PDU::set_connection_handle( uint16 val )
{
    set_uint16( HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY_T_connection_handle, val );
}

ULPKey HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY_T_PDU::get_long_term_key(void) const
{
	return ULPKey ( get_uint8ArrayPtr(HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY_T_long_term_key) );
}

void HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY_T_PDU::set_long_term_key(const ULPKey& data)
{
	set_uint8Array ( data.get_str() , HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY_T_long_term_key, 16 );
}

/*******************************************************************/

HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_T_PDU::HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_T_PDU()
: HCICommandPDU ( HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION , HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_T_pduSize )
{
}

HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_T_PDU::HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_T_PDU ( const PDU& from )
: HCICommandPDU ( from )
{
}

HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_T_PDU::HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_T_PDU ( const uint8* data , uint32 len )
: HCICommandPDU( data , len )
{
}

ULPChannelMap HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_T_PDU::get_channel_map(void) const
{
	return ULPChannelMap ( get_uint8ArrayPtr(HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_T_channel_map) );
}

void HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_T_PDU::set_channel_map(const ULPChannelMap& data)
{
	set_uint8Array ( data.get_ptr() , HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_T_channel_map, 5 );
}

/*******************************************************************/


HCI_ULP_SET_SCAN_RESPONSE_DATA_T_PDU::HCI_ULP_SET_SCAN_RESPONSE_DATA_T_PDU()
: HCICommandPDU ( HCI_ULP_SET_SCAN_RESPONSE_DATA , HCI_ULP_SET_SCAN_RESPONSE_DATA_T_pduSize )
{
}

HCI_ULP_SET_SCAN_RESPONSE_DATA_T_PDU::HCI_ULP_SET_SCAN_RESPONSE_DATA_T_PDU ( const PDU& from )
: HCICommandPDU ( from )
{
}

HCI_ULP_SET_SCAN_RESPONSE_DATA_T_PDU::HCI_ULP_SET_SCAN_RESPONSE_DATA_T_PDU ( const uint8* data , uint32 len )
: HCICommandPDU( data , len )
{
}

uint8 HCI_ULP_SET_SCAN_RESPONSE_DATA_T_PDU::get_scan_response_data_len(void) const
{
    return get_uint8(HCI_ULP_SET_SCAN_RESPONSE_DATA_T_scan_response_data_len) ;
}

void HCI_ULP_SET_SCAN_RESPONSE_DATA_T_PDU::set_scan_response_data_len(uint8 val)
{
    set_uint8(HCI_ULP_SET_SCAN_RESPONSE_DATA_T_scan_response_data_len,val) ;
}

BluetoothName HCI_ULP_SET_SCAN_RESPONSE_DATA_T_PDU::get_scan_response_data(void) const
{
	return BluetoothName ( PDU::get_uint8ArrayPtr ( HCI_ULP_SET_SCAN_RESPONSE_DATA_T_scan_response_data), size() - HCI_ULP_SET_SCAN_RESPONSE_DATA_T_scan_response_data ) ;
}

void HCI_ULP_SET_SCAN_RESPONSE_DATA_T_PDU::set_scan_response_data(BluetoothName val)
{
	PDU::set_uint8Array ( val.get_str() , HCI_ULP_SET_SCAN_RESPONSE_DATA_T_scan_response_data , get_scan_response_data_len() );
}

/*******************************************************************/

enum {
	OFFSET_START(HCI_ULP_START_ENCRYPTION_T)
	OFFSET_uint16(HCI_ULP_START_ENCRYPTION_T_op_code),
	OFFSET_uint8(HCI_ULP_START_ENCRYPTION_T_cmd_parameter_length),
    OFFSET_uint16(HCI_ULP_START_ENCRYPTION_T_connection_handle),
    OFFSET_uint8_array(HCI_ULP_START_ENCRYPTION_T_random_number, 8),
    OFFSET_uint16(HCI_ULP_START_ENCRYPTION_T_encrypted_diversifier),
    OFFSET_uint8_array(HCI_ULP_START_ENCRYPTION_T_long_term_key,16),
    HCI_ULP_START_ENCRYPTION_T_pduSize = 31
} ;


HCI_ULP_START_ENCRYPTION_T_PDU::HCI_ULP_START_ENCRYPTION_T_PDU()
: HCICommandPDU ( HCI_ULP_START_ENCRYPTION , HCI_ULP_START_ENCRYPTION_T_pduSize )
{
}

HCI_ULP_START_ENCRYPTION_T_PDU::HCI_ULP_START_ENCRYPTION_T_PDU ( const PDU& from )
: HCICommandPDU ( from )
{
}

HCI_ULP_START_ENCRYPTION_T_PDU::HCI_ULP_START_ENCRYPTION_T_PDU ( const uint8* data , uint32 len )
: HCICommandPDU( data , len )
{
}


uint16 HCI_ULP_START_ENCRYPTION_T_PDU::get_connection_handle( void ) const
{
    return get_uint16( HCI_ULP_START_ENCRYPTION_T_connection_handle );
}


ULPRandom HCI_ULP_START_ENCRYPTION_T_PDU::get_random_number( void ) const
{
    return ULPRandom( get_uint8ArrayPtr( HCI_ULP_START_ENCRYPTION_T_random_number )); 
}

uint16 HCI_ULP_START_ENCRYPTION_T_PDU::get_encrypted_diversifier( void ) const
{
    return get_uint16( HCI_ULP_START_ENCRYPTION_T_encrypted_diversifier );   
}

ULPKey HCI_ULP_START_ENCRYPTION_T_PDU::get_long_term_key( void ) const
{
    return ULPKey( get_uint8ArrayPtr( HCI_ULP_START_ENCRYPTION_T_long_term_key ));    
}

void HCI_ULP_START_ENCRYPTION_T_PDU::set_connection_handle( uint16 val )
{
    set_uint16( HCI_ULP_START_ENCRYPTION_T_connection_handle, val );
}

void HCI_ULP_START_ENCRYPTION_T_PDU::set_random_number( const ULPRandom& data ) 
{
    set_uint8Array( data.get_ptr(), HCI_ULP_START_ENCRYPTION_T_random_number, 8 );
}

void HCI_ULP_START_ENCRYPTION_T_PDU::set_encrypted_diversifier( uint16 val )
{
    set_uint16( HCI_ULP_START_ENCRYPTION_T_encrypted_diversifier, val );
}

void HCI_ULP_START_ENCRYPTION_T_PDU::set_long_term_key( const ULPKey& data )
{
    set_uint8Array( data.get_str(), HCI_ULP_START_ENCRYPTION_T_long_term_key, 16 );
}


/*******************************************************************/
enum {
	OFFSET_START(HCI_ULP_ENCRYPT_T)
	OFFSET_uint16(HCI_ULP_ENCRYPT_T_op_code),
	OFFSET_uint8(HCI_ULP_ENCRYPT_T_cmd_parameter_length),
	OFFSET_uint8_array(HCI_ULP_ENCRYPT_T_key,16),
	OFFSET_uint8_array(HCI_ULP_ENCRYPT_T_plaintext_data, 16),
	HCI_ULP_ENCRYPT_T_pduSize = 35
} ;


HCI_ULP_ENCRYPT_T_PDU::HCI_ULP_ENCRYPT_T_PDU()
: HCICommandPDU ( HCI_ULP_ENCRYPT , HCI_ULP_ENCRYPT_T_pduSize )
{
}

HCI_ULP_ENCRYPT_T_PDU::HCI_ULP_ENCRYPT_T_PDU ( const PDU& from )
: HCICommandPDU ( from )
{
}

HCI_ULP_ENCRYPT_T_PDU::HCI_ULP_ENCRYPT_T_PDU ( const uint8* data , uint32 len )
: HCICommandPDU( data , len )
{
}


ULPKey HCI_ULP_ENCRYPT_T_PDU::get_aes_key( void ) const
{
    return ULPKey( get_uint8ArrayPtr( HCI_ULP_ENCRYPT_T_key ) );
}

ULPKey HCI_ULP_ENCRYPT_T_PDU::get_plaintext_data( void ) const
{
    return ULPKey( get_uint8ArrayPtr( HCI_ULP_ENCRYPT_T_plaintext_data ) );
}

void HCI_ULP_ENCRYPT_T_PDU::set_aes_key( const ULPKey& data )
{
    set_uint8Array( data.get_str(), HCI_ULP_ENCRYPT_T_key, 16 );
}

void HCI_ULP_ENCRYPT_T_PDU::set_plaintext_data( const ULPKey& data )
{
    set_uint8Array( data.get_str(), HCI_ULP_ENCRYPT_T_plaintext_data, 16 );
}

/*******************************************************************/
enum {
    OFFSET_START(HCI_ULP_SET_EVENT_MASK_T)
    OFFSET_uint16(HCI_ULP_SET_EVENT_MASK_T_op_code),
    OFFSET_uint8(HCI_ULP_SET_EVENT_MASK_T_cmd_parameter_length),
    OFFSET_EventMask(HCI_ULP_SET_EVENT_MASK_T_event_mask),
    HCI_ULP_SET_EVENT_MASK_T_pduSize
} ;


HCI_ULP_SET_EVENT_MASK_T_PDU::HCI_ULP_SET_EVENT_MASK_T_PDU()
: HCICommandPDU ( HCI_ULP_SET_EVENT_MASK , HCI_ULP_SET_EVENT_MASK_T_pduSize )
{
}

HCI_ULP_SET_EVENT_MASK_T_PDU::HCI_ULP_SET_EVENT_MASK_T_PDU ( const PDU& from )
: HCICommandPDU ( from )
{
}

HCI_ULP_SET_EVENT_MASK_T_PDU::HCI_ULP_SET_EVENT_MASK_T_PDU ( const uint8* data , uint32 len )
: HCICommandPDU( data , len )
{
}

ULPEventMask HCI_ULP_SET_EVENT_MASK_T_PDU::get_ulp_event_mask() const
{
    uint8 mask[8];
    // byte spaced on both HCI and DM
    PDU::get_uint8Array ( mask , HCI_ULP_SET_EVENT_MASK_T_event_mask , 8 );
    return ULPEventMask ( mask );
}

void HCI_ULP_SET_EVENT_MASK_T_PDU::set_ulp_event_mask ( const ULPEventMask& mask )
{
    // byte spaced on both HCI and DM
    PDU::set_uint8Array ( mask.get_data() , HCI_ULP_SET_EVENT_MASK_T_event_mask , 8 ) ;
}
