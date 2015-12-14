///////////////////////////////////////////////////////////////////////////////
//
//  NAME:     xtraeventpdu.cpp
//
//  PURPOSE:  define non-autogenable event pdu classes.
//
///////////////////////////////////////////////////////////////////////////////

#include "xtraeventpdus.h"
#include "app/bluestack/bluetooth.h"
#include "app/bluestack/hci.h"
#include "hcilayout.h"

#define OFFSET_START(name) 


enum {
    OFFSET_START(HCI_EV_LINK_KEY_NOTIFICATION_T)
    OFFSET_uint8(HCI_EV_LINK_KEY_NOTIFICATION_T_event_code),
    OFFSET_uint8(HCI_EV_LINK_KEY_NOTIFICATION_T_length),
    OFFSET_BluetoothDeviceAddress(HCI_EV_LINK_KEY_NOTIFICATION_T_bd_addr),
    OFFSET_uint32(HCI_EV_LINK_KEY_NOTIFICATION_T_key_part0),
    OFFSET_uint32(HCI_EV_LINK_KEY_NOTIFICATION_T_key_part1),
    OFFSET_uint32(HCI_EV_LINK_KEY_NOTIFICATION_T_key_part2),
    OFFSET_uint32(HCI_EV_LINK_KEY_NOTIFICATION_T_key_part3),
    OFFSET_uint8(HCI_EV_LINK_KEY_NOTIFICATION_T_key_type),
    HCI_EV_LINK_KEY_NOTIFICATION_T_pduSize
} ;

enum {
    OFFSET_START(HCI_EV_INQUIRY_RESULT_T)
    OFFSET_uint8(HCI_EV_INQUIRY_RESULT_T_event_code),
    OFFSET_uint8(HCI_EV_INQUIRY_RESULT_T_length),
    OFFSET_uint8(HCI_EV_INQUIRY_RESULT_T_num_responses),
    //OFFSET_UNHANDLED_HCI_INQ_RESULT_T(HCI_EV_INQUIRY_RESULT_T_result),
    HCI_EV_INQUIRY_RESULT_T_pduSize
} ;

enum {
    OFFSET_START(HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T)
    OFFSET_uint8(HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T_event_code),
    OFFSET_uint8(HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T_length),
    OFFSET_uint8(HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T_status),
    OFFSET_uint16(HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T_handle),
    //OFFSET_UNHANDLED_uint16(HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T_lmp_supp_features),
    OFFSET_uint32(HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T_features_part0),
    OFFSET_uint32(HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T_features_part1),
    HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T_pduSize
} ;

enum {
    OFFSET_START(HCI_EV_REMOTE_NAME_REQ_COMPLETE_T)
    OFFSET_uint8(HCI_EV_REMOTE_NAME_REQ_COMPLETE_T_event_code),
    OFFSET_uint8(HCI_EV_REMOTE_NAME_REQ_COMPLETE_T_length),
    OFFSET_uint8(HCI_EV_REMOTE_NAME_REQ_COMPLETE_T_status),
    OFFSET_BluetoothDeviceAddress(HCI_EV_REMOTE_NAME_REQ_COMPLETE_T_bd_addr),
    //OFFSET_UNHANDLED_uint8(HCI_EV_REMOTE_NAME_REQ_COMPLETE_T_name_part),
    HCI_EV_REMOTE_NAME_REQ_COMPLETE_T_pduSize
} ;

enum {
    OFFSET_START(HCI_EV_RETURN_LINK_KEYS_T)
    OFFSET_uint8(HCI_EV_RETURN_LINK_KEYS_T_event_code),
    OFFSET_uint8(HCI_EV_RETURN_LINK_KEYS_T_length),
    OFFSET_uint8(HCI_EV_RETURN_LINK_KEYS_T_number_keys),
    //OFFSET_UNHANDLED_LINK_KEY_BD_ADDR_T(HCI_EV_RETURN_LINK_KEYS_T_link_key_bd_addr),
    HCI_EV_RETURN_LINK_KEYS_T_pduSize
} ;

enum {
    OFFSET_START(HCI_EV_NUMBER_COMPLETED_PKTS_T)
    OFFSET_uint8(HCI_EV_NUMBER_COMPLETED_PKTS_T_event_code),
    OFFSET_uint8(HCI_EV_NUMBER_COMPLETED_PKTS_T_length),
    OFFSET_uint8(HCI_EV_NUMBER_COMPLETED_PKTS_T_num_handles),
    //OFFSET_UNHANDLED_HANDLE_COMPLETE_T(HCI_EV_NUMBER_COMPLETED_PKTS_T_num_completed_pkts_ptr),
    HCI_EV_NUMBER_COMPLETED_PKTS_T_pduSize
} ;

enum {
	OFFSET_START(HCI_EV_MNFR_EXTENSION_T)
	OFFSET_uint8(HCI_EV_MNFR_EXTENSION_T_event_code),
	OFFSET_uint8(HCI_EV_MNFR_EXTENSION_T_evt_parameter_length),
	OFFSET_uint8(HCI_EV_MNFR_EXTENSION_T_payload_descriptor),
    //OFFSET_EV_MNFR_EXTN_PAYLOAD_T(HCI_EV_MNFR_EXTENSION_T_payload),
	HCI_EV_MNFR_EXTENSION_T_pduSize
} ;

enum {
	OFFSET_START(HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T)
	OFFSET_uint8(HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_event_code),
	OFFSET_uint8(HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_evt_parameter_length),
	OFFSET_uint8(HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_status),
	OFFSET_uint16(HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_handle),
	OFFSET_uint8(HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_page_num),
	OFFSET_uint8(HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_max_page_num),
    OFFSET_uint32(HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_lmp_ext_features),
    OFFSET_uint32(HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_lmp_ext_features_p2),
	HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_pduSize
} ;

enum {
	OFFSET_START(HCI_EV_LOOPBACK_COMMAND_T)
	OFFSET_uint8(HCI_EV_LOOPBACK_COMMAND_T_event_code),
	OFFSET_uint8(HCI_EV_LOOPBACK_COMMAND_T_evt_parameter_length),
	HCI_EV_LOOPBACK_COMMAND_T_pduSize
} ;

enum {
    OFFSET_START(HCI_EV_EXTENDED_INQUIRY_RESULT_T_PDU)
    OFFSET_uint8(HCI_EV_EXTENDED_INQUIRY_RESULT_T_event_code),
    OFFSET_uint8(HCI_EV_EXTENDED_INQUIRY_RESULT_T_length),
    OFFSET_uint8(HCI_EV_EXTENDED_INQUIRY_RESULT_T_num_responses),
    OFFSET_BluetoothDeviceAddress(HCI_EV_EXTENDED_INQUIRY_RESULT_T_bd_addr),
    OFFSET_uint8(HCI_EV_EXTENDED_INQUIRY_RESULT_T_page_scan_rep_mode),
    OFFSET_uint8(HCI_EV_EXTENDED_INQUIRY_RESULT_T_page_scan_period_mode),
    OFFSET_uint24(HCI_EV_EXTENDED_INQUIRY_RESULT_T_dev_class),
    OFFSET_uint16(HCI_EV_EXTENDED_INQUIRY_RESULT_T_clock_offset),
    OFFSET_uint8(HCI_EV_EXTENDED_INQUIRY_RESULT_T_rssi),
    OFFSET_uint8_array(HCI_EV_EXTENDED_INQUIRY_RESULT_T_eir_data_part,240),
    HCI_EV_EXTENDED_INQUIRY_RESULT_T_pduSize
};

enum {
	OFFSET_START(HCI_EV_REM_HOST_SUPPORTED_FEATURES_T)
    OFFSET_uint8(HCI_EV_REM_HOST_SUPPORTED_FEATURES_T_event_code),
    OFFSET_uint8(HCI_EV_REM_HOST_SUPPORTED_FEATURES_T_length),
	OFFSET_BluetoothDeviceAddress(HCI_EV_REM_HOST_SUPPORTED_FEATURES_T_bd_addr),
    OFFSET_uint32(HCI_EV_REM_HOST_SUPPORTED_FEATURES_T_host_features),
    OFFSET_uint32(HCI_EV_REM_HOST_SUPPORTED_FEATURES_T_host_features_p2),
	HCI_EV_REM_HOST_SUPPORTED_FEATURES_T_pduSize
};

enum {
	OFFSET_START(HCI_EV_ULP_T)
    OFFSET_uint8(HCI_EV_ULP_T_event_code),
    OFFSET_uint8(HCI_EV_ULP_T_length),
	OFFSET_uint8(HCI_EV_ULP_T_ulp_sub_opcode),
	OFFSET_uint8_array(HCI_EV_ULP_T_ulp_sub_event, 100),
	HCI_EV_ULP_T_pduSize
};

#include "hcipdu.h"
#include <stdio.h>


/*******************************************************************/

HCI_EV_LINK_KEY_NOTIFICATION_T_PDU::HCI_EV_LINK_KEY_NOTIFICATION_T_PDU()
:   HCIEventPDU(HCI_EV_LINK_KEY_NOTIFICATION,HCI_EV_LINK_KEY_NOTIFICATION_T_pduSize)
{
}

HCI_EV_LINK_KEY_NOTIFICATION_T_PDU::HCI_EV_LINK_KEY_NOTIFICATION_T_PDU( const PDU& from )
:   HCIEventPDU(from)
{
}

HCI_EV_LINK_KEY_NOTIFICATION_T_PDU::HCI_EV_LINK_KEY_NOTIFICATION_T_PDU( const uint8* data , uint32 len )
:   HCIEventPDU(data,len)
{
}

BluetoothDeviceAddress HCI_EV_LINK_KEY_NOTIFICATION_T_PDU::get_bd_addr(void) const
{
    return get_BluetoothDeviceAddress(HCI_EV_LINK_KEY_NOTIFICATION_T_bd_addr) ;
}

LinkKey HCI_EV_LINK_KEY_NOTIFICATION_T_PDU::get_link_key(void) const
{
    uint8 k[16] ;
    get_uint8Array(k,HCI_EV_LINK_KEY_NOTIFICATION_T_key_part0,16) ;
    LinkKey key(k) ;
    return key ;
}

uint8 HCI_EV_LINK_KEY_NOTIFICATION_T_PDU::get_key_type() const
{
    return get_uint8 ( HCI_EV_LINK_KEY_NOTIFICATION_T_key_type );
}

bool HCI_EV_LINK_KEY_NOTIFICATION_T_PDU::size_expected() const
{
    return size() == HCI_EV_LINK_KEY_NOTIFICATION_T_pduSize;
}

/*******************************************************************/


HCI_EV_INQUIRY_RESULT_T_PDU::HCI_EV_INQUIRY_RESULT_T_PDU()
:   HCIEventPDU(HCI_EV_INQUIRY_RESULT,HCI_EV_INQUIRY_RESULT_T_pduSize)
{
}

HCI_EV_INQUIRY_RESULT_T_PDU::HCI_EV_INQUIRY_RESULT_T_PDU( const PDU& from )
:   HCIEventPDU(from)
{
}

HCI_EV_INQUIRY_RESULT_T_PDU::HCI_EV_INQUIRY_RESULT_T_PDU( const uint8* data , uint32 len )
:   HCIEventPDU(data,len)
{
}

uint8 HCI_EV_INQUIRY_RESULT_T_PDU::get_num_responses(void) const
{
    return get_uint8(HCI_EV_INQUIRY_RESULT_T_num_responses) ;
}

InquiryResult HCI_EV_INQUIRY_RESULT_T_PDU::get_result(uint8 index) const
{
    return InquiryResult ( this->get_uint8ArrayPtr( HCI_EV_INQUIRY_RESULT_T_pduSize + ( index * 14 ) ) ) ;
}

bool HCI_EV_INQUIRY_RESULT_T_PDU::size_expected() const
{
    return size() >  HCI_EV_RETURN_LINK_KEYS_T_number_keys
        && size() == HCI_EV_INQUIRY_RESULT_T_pduSize + ( get_num_responses() * 14u );
}

/*******************************************************************/

HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T_PDU::HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T_PDU()
:   HCIEventPDU(HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE,HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T_pduSize)
{
}

HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T_PDU::HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T_PDU( const PDU& from )
:   HCIEventPDU(from)
{
}

HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T_PDU::HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T_PDU( const uint8* data , uint32 len )
:   HCIEventPDU(data,len)
{
}

uint8 HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T_PDU::get_status(void) const
{
    return get_uint8(HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T_status) ;
}

uint16 HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T_PDU::get_handle(void) const
{
    return get_uint16(HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T_handle) ;
}

LMPSupportedFeatures HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T_PDU::get_lmp_supp_features(void) const
{
    uint8 a[8] ;
    get_uint8Array(a,HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T_features_part0,8) ;
    LMPSupportedFeatures f (a) ;
    return f ;
}

bool HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T_PDU::size_expected() const
{
    return size() == HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T_pduSize;
}

/*******************************************************************/


HCI_EV_REMOTE_NAME_REQ_COMPLETE_T_PDU::HCI_EV_REMOTE_NAME_REQ_COMPLETE_T_PDU()
:   HCIEventPDU(HCI_EV_REMOTE_NAME_REQ_COMPLETE,HCI_EV_REMOTE_NAME_REQ_COMPLETE_T_pduSize)
{
}

HCI_EV_REMOTE_NAME_REQ_COMPLETE_T_PDU::HCI_EV_REMOTE_NAME_REQ_COMPLETE_T_PDU( const PDU& from )
:   HCIEventPDU(from)
{
}

HCI_EV_REMOTE_NAME_REQ_COMPLETE_T_PDU::HCI_EV_REMOTE_NAME_REQ_COMPLETE_T_PDU( const uint8* data , uint32 len )
:   HCIEventPDU(data,len)
{
}

uint8 HCI_EV_REMOTE_NAME_REQ_COMPLETE_T_PDU::get_status(void) const
{
    return get_uint8(HCI_EV_REMOTE_NAME_REQ_COMPLETE_T_status) ;
}

BluetoothDeviceAddress HCI_EV_REMOTE_NAME_REQ_COMPLETE_T_PDU::get_bd_addr(void) const
{
    return get_BluetoothDeviceAddress(HCI_EV_REMOTE_NAME_REQ_COMPLETE_T_bd_addr) ;
}

BluetoothName HCI_EV_REMOTE_NAME_REQ_COMPLETE_T_PDU::get_name_part(void) const
{
    return BluetoothName (get_uint8ArrayPtr(HCI_EV_REMOTE_NAME_REQ_COMPLETE_T_pduSize) , size() - HCI_EV_REMOTE_NAME_REQ_COMPLETE_T_pduSize ) ;
}

bool HCI_EV_REMOTE_NAME_REQ_COMPLETE_T_PDU::size_expected() const
{
    return size() == HCI_EV_REMOTE_NAME_REQ_COMPLETE_T_pduSize + 248u;
}

/*******************************************************************/


HCI_EV_RETURN_LINK_KEYS_T_PDU::HCI_EV_RETURN_LINK_KEYS_T_PDU()
:   HCIEventPDU(HCI_EV_RETURN_LINK_KEYS,HCI_EV_RETURN_LINK_KEYS_T_pduSize)
{
}

HCI_EV_RETURN_LINK_KEYS_T_PDU::HCI_EV_RETURN_LINK_KEYS_T_PDU( const PDU& from )
:   HCIEventPDU(from)
{
}

HCI_EV_RETURN_LINK_KEYS_T_PDU::HCI_EV_RETURN_LINK_KEYS_T_PDU( const uint8* data , uint32 len )
:   HCIEventPDU(data,len)
{
}

uint8 HCI_EV_RETURN_LINK_KEYS_T_PDU::get_number_keys(void) const
{
    return get_uint8(HCI_EV_RETURN_LINK_KEYS_T_number_keys) ;
}


void HCI_EV_RETURN_LINK_KEYS_T_PDU::get_link_key_bd_addr(uint8 index,BluetoothDeviceAddress &addr,LinkKey &key) const
{
    uint16 offset = HCI_EV_RETURN_LINK_KEYS_T_pduSize ;
    offset += index * (6 + 16) ; //bdaddr = 6 bytes, linkkey is 16
    addr = get_BluetoothDeviceAddress(offset) ;
    offset+=6 ;
    uint8 keydata[16] ;
    get_uint8Array(keydata,offset,16) ;
    LinkKey k(keydata) ;
    key = k ;
}

bool HCI_EV_RETURN_LINK_KEYS_T_PDU::size_expected() const
{
    return size() >  HCI_EV_RETURN_LINK_KEYS_T_number_keys
        && size() == HCI_EV_RETURN_LINK_KEYS_T_pduSize + ( get_number_keys() * 22u );
}

/*******************************************************************/


HCI_EV_NUMBER_COMPLETED_PKTS_T_PDU::HCI_EV_NUMBER_COMPLETED_PKTS_T_PDU()
:   HCIEventPDU(HCI_EV_NUMBER_COMPLETED_PKTS,HCI_EV_NUMBER_COMPLETED_PKTS_T_pduSize)
{
}

HCI_EV_NUMBER_COMPLETED_PKTS_T_PDU::HCI_EV_NUMBER_COMPLETED_PKTS_T_PDU ( const PDU& from )
:   HCIEventPDU ( from )
{
}

HCI_EV_NUMBER_COMPLETED_PKTS_T_PDU::HCI_EV_NUMBER_COMPLETED_PKTS_T_PDU ( const uint8* data , uint32 len )
:   HCIEventPDU (data,len)
{
}

uint8 HCI_EV_NUMBER_COMPLETED_PKTS_T_PDU::get_num_handles(void) const
{
    return get_uint8(HCI_EV_NUMBER_COMPLETED_PKTS_T_num_handles) ;
}


void HCI_EV_NUMBER_COMPLETED_PKTS_T_PDU::get_num_completed_pkts(uint8 index,uint16 &handle,uint16 &pkts ) const
{
    uint16 offset = HCI_EV_NUMBER_COMPLETED_PKTS_T_pduSize ;
    offset += index * (2+2) ; //2 = handle size, 2=num size
    handle = get_uint16(offset) ; offset += 2 ;
    pkts = get_uint16(offset) ; 

}

bool HCI_EV_NUMBER_COMPLETED_PKTS_T_PDU::size_expected() const
{
    return size() > HCI_EV_NUMBER_COMPLETED_PKTS_T_num_handles
        && size() == HCI_EV_NUMBER_COMPLETED_PKTS_T_pduSize + ( get_num_handles() * 4u );
}

/*******************************************************************/

HCI_EV_MNFR_EXTENSION_T_PDU::HCI_EV_MNFR_EXTENSION_T_PDU()
: HCIEventPDU ( HCI_EV_MNFR_EXTENSION , HCI_EV_MNFR_EXTENSION_T_pduSize )
{
}

HCI_EV_MNFR_EXTENSION_T_PDU::HCI_EV_MNFR_EXTENSION_T_PDU( const PDU& pdu )
: HCIEventPDU ( pdu )
{
}

HCI_EV_MNFR_EXTENSION_T_PDU::HCI_EV_MNFR_EXTENSION_T_PDU( const uint8 * data , uint32 len )
: HCIEventPDU ( data , len )
{
}

uint8 HCI_EV_MNFR_EXTENSION_T_PDU::get_payload_descriptor() const
{
    return get_uint8(HCI_EV_MNFR_EXTENSION_T_payload_descriptor);
}

uint32 HCI_EV_MNFR_EXTENSION_T_PDU::get_payload_length() const
{
    return size() - HCI_EV_MNFR_EXTENSION_T_pduSize;
}

const uint8 * HCI_EV_MNFR_EXTENSION_T_PDU::get_payload() const
{
    return get_uint8ArrayPtr (HCI_EV_MNFR_EXTENSION_T_pduSize);
}


bool HCI_EV_MNFR_EXTENSION_T_PDU::size_expected() const
{
    return size() > 2;
}

/*******************************************************************/


HCI_EV_INQUIRY_RESULT_WITH_RSSI_T_PDU::HCI_EV_INQUIRY_RESULT_WITH_RSSI_T_PDU()
:   HCIEventPDU ( HCI_EV_INQUIRY_RESULT_WITH_RSSI,HCI_EV_INQUIRY_RESULT_T_pduSize )
{
}

HCI_EV_INQUIRY_RESULT_WITH_RSSI_T_PDU::HCI_EV_INQUIRY_RESULT_WITH_RSSI_T_PDU( const PDU& from )
:   HCIEventPDU ( from )
{
}

HCI_EV_INQUIRY_RESULT_WITH_RSSI_T_PDU::HCI_EV_INQUIRY_RESULT_WITH_RSSI_T_PDU( const uint8 * data , uint32 len )
:   HCIEventPDU ( data , len )
{
}

uint8 HCI_EV_INQUIRY_RESULT_WITH_RSSI_T_PDU::get_num_responses() const
{
    return get_uint8(HCI_EV_INQUIRY_RESULT_T_num_responses) ;
}

InquiryResultWithRSSI HCI_EV_INQUIRY_RESULT_WITH_RSSI_T_PDU::get_result( uint8 index ) const
{
    return InquiryResultWithRSSI ( this->get_uint8ArrayPtr( HCI_EV_INQUIRY_RESULT_T_pduSize + ( index * 14 ) ) ) ;
}

bool HCI_EV_INQUIRY_RESULT_WITH_RSSI_T_PDU::size_expected() const
{
    return size() >  HCI_EV_INQUIRY_RESULT_T_num_responses
        && size() == HCI_EV_INQUIRY_RESULT_T_pduSize + ( get_num_responses() * 14u ) ;
}

/*******************************************************************/


HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_PDU::HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_PDU()
: HCIEventPDU ( HCI_EV_READ_REM_EXT_FEATURES_COMPLETE , HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_pduSize )
{
}

HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_PDU::HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_PDU( const PDU& pdu )
: HCIEventPDU ( pdu )
{
}

HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_PDU::HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_PDU( const uint8 * data , uint32 len )
: HCIEventPDU ( data , len )
{
}

uint8 HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_PDU::get_status() const
{
    return get_uint8(HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_status);
}

uint16 HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_PDU::get_handle() const
{
    return get_uint16(HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_handle);
}

uint8 HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_PDU::get_page_num() const
{
    return get_uint8(HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_page_num);
}

uint8 HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_PDU::get_max_page_num() const
{
    return get_uint8(HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_max_page_num);
}

LMPExtFeatures HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_PDU::get_lmp_ext_features() const
{
    uint8 a[8] ;
    get_uint8Array(a,HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_lmp_ext_features,8) ;
    LMPExtFeatures f (a) ;
    return f ;
}

bool HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_PDU::size_expected() const
{
    return size() == HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_pduSize;
}

/*******************************************************************/

HCI_EV_LOOPBACK_COMMAND_T_PDU::HCI_EV_LOOPBACK_COMMAND_T_PDU()
: HCIEventPDU ( HCI_EV_LOOPBACK_COMMAND , HCI_EV_LOOPBACK_COMMAND_T_pduSize )
{
}

HCI_EV_LOOPBACK_COMMAND_T_PDU::HCI_EV_LOOPBACK_COMMAND_T_PDU( const PDU& pdu )
: HCIEventPDU ( pdu )
{
}

HCI_EV_LOOPBACK_COMMAND_T_PDU::HCI_EV_LOOPBACK_COMMAND_T_PDU( const uint8 * data , uint32 len )
: HCIEventPDU ( data , len )
{
}

const PDU HCI_EV_LOOPBACK_COMMAND_T_PDU::get_loopback_part() const
{
    return PDU ( channel() , get_uint8ArrayPtr(HCI_EV_LOOPBACK_COMMAND_T_pduSize) , size() - HCI_EV_LOOPBACK_COMMAND_T_pduSize );
}

bool HCI_EV_LOOPBACK_COMMAND_T_PDU::size_expected() const
{
    return size() > 2;
}

/*******************************************************************/

HCI_EV_EXTENDED_INQUIRY_RESULT_T_PDU::HCI_EV_EXTENDED_INQUIRY_RESULT_T_PDU()
: HCIEventPDU (HCI_EV_EXTENDED_INQUIRY_RESULT , HCI_EV_EXTENDED_INQUIRY_RESULT_T_pduSize )
{
}

HCI_EV_EXTENDED_INQUIRY_RESULT_T_PDU::HCI_EV_EXTENDED_INQUIRY_RESULT_T_PDU( const PDU& pdu )
: HCIEventPDU(pdu)
{
}

HCI_EV_EXTENDED_INQUIRY_RESULT_T_PDU::HCI_EV_EXTENDED_INQUIRY_RESULT_T_PDU( const uint8 * data , uint32 len )
: HCIEventPDU(data,len)
{
}

uint8 HCI_EV_EXTENDED_INQUIRY_RESULT_T_PDU::get_num_responses () const
{
    return get_uint8(HCI_EV_EXTENDED_INQUIRY_RESULT_T_num_responses);
}

const BluetoothDeviceAddress HCI_EV_EXTENDED_INQUIRY_RESULT_T_PDU::get_bd_addr() const
{
    return get_BluetoothDeviceAddress(HCI_EV_EXTENDED_INQUIRY_RESULT_T_bd_addr);
}

uint8 HCI_EV_EXTENDED_INQUIRY_RESULT_T_PDU::get_page_scan_rep_mode() const
{
    return get_uint8(HCI_EV_EXTENDED_INQUIRY_RESULT_T_page_scan_rep_mode);
}

uint8 HCI_EV_EXTENDED_INQUIRY_RESULT_T_PDU::get_page_scan_period_mode() const
{
    return get_uint8(HCI_EV_EXTENDED_INQUIRY_RESULT_T_page_scan_period_mode);
}

const ClassOfDevice HCI_EV_EXTENDED_INQUIRY_RESULT_T_PDU::get_class_of_device() const
{
    return get_ClassOfDevice(HCI_EV_EXTENDED_INQUIRY_RESULT_T_dev_class);
}

uint24 HCI_EV_EXTENDED_INQUIRY_RESULT_T_PDU::get_dev_class() const
{
    return get_uint24(HCI_EV_EXTENDED_INQUIRY_RESULT_T_dev_class);
}

uint16 HCI_EV_EXTENDED_INQUIRY_RESULT_T_PDU::get_clock_offset() const
{
    return get_uint16(HCI_EV_EXTENDED_INQUIRY_RESULT_T_clock_offset);
}

uint8 HCI_EV_EXTENDED_INQUIRY_RESULT_T_PDU::get_rssi() const
{
    return get_uint8(HCI_EV_EXTENDED_INQUIRY_RESULT_T_rssi);
}

const EIRData HCI_EV_EXTENDED_INQUIRY_RESULT_T_PDU::get_eir_data_part () const
{
    uint8 buffer[240];
    //  get as much as there is in the pdu. size = total size - offset of start
    uint8 toget = size() - HCI_EV_EXTENDED_INQUIRY_RESULT_T_eir_data_part;
    //  but not more than 240 bytes.
    if ( toget > sizeof(buffer) )
	toget = sizeof(buffer);
    get_uint8Array(buffer, HCI_EV_EXTENDED_INQUIRY_RESULT_T_eir_data_part, toget);
    return EIRData(buffer,toget);
}

bool HCI_EV_EXTENDED_INQUIRY_RESULT_T_PDU::size_expected() const
{
    return size() >= HCI_EV_EXTENDED_INQUIRY_RESULT_T_eir_data_part;
}

/*******************************************************************/

HCI_EV_REM_HOST_SUPPORTED_FEATURES_T_PDU::HCI_EV_REM_HOST_SUPPORTED_FEATURES_T_PDU()
: HCIEventPDU (HCI_EV_REM_HOST_SUPPORTED_FEATURES , HCI_EV_REM_HOST_SUPPORTED_FEATURES_T_pduSize )
{
}

HCI_EV_REM_HOST_SUPPORTED_FEATURES_T_PDU::HCI_EV_REM_HOST_SUPPORTED_FEATURES_T_PDU( const PDU& pdu )
: HCIEventPDU(pdu)
{
}

HCI_EV_REM_HOST_SUPPORTED_FEATURES_T_PDU::HCI_EV_REM_HOST_SUPPORTED_FEATURES_T_PDU( const uint8 * data , uint32 len )
: HCIEventPDU(data,len)
{
}

const BluetoothDeviceAddress HCI_EV_REM_HOST_SUPPORTED_FEATURES_T_PDU::get_bd_addr() const
{
    return get_BluetoothDeviceAddress(HCI_EV_REM_HOST_SUPPORTED_FEATURES_T_bd_addr);
}

LMPExtFeatures HCI_EV_REM_HOST_SUPPORTED_FEATURES_T_PDU::get_host_features() const
{
    uint8 a[8] ;
    get_uint8Array(a,HCI_EV_REM_HOST_SUPPORTED_FEATURES_T_host_features,8) ;
    LMPExtFeatures f (a) ;
    return f ;
}

bool HCI_EV_REM_HOST_SUPPORTED_FEATURES_T_PDU::size_expected() const
{
	return size() == HCI_EV_REM_HOST_SUPPORTED_FEATURES_T_pduSize;
}

/*******************************************************************/

HCI_EV_ULP_T_PDU::HCI_EV_ULP_T_PDU()
: HCIEventPDU (HCI_EV_ULP , HCI_EV_ULP_T_pduSize )
{
}

HCI_EV_ULP_T_PDU::HCI_EV_ULP_T_PDU( const PDU& pdu )
: HCIEventPDU(pdu)
{
}

HCI_EV_ULP_T_PDU::HCI_EV_ULP_T_PDU( const uint8 * data , uint32 len )
: HCIEventPDU(data,len)
{
}

uint8 HCI_EV_ULP_T_PDU::get_ulp_sub_opcode() const
{
	return *(this->get_uint8ArrayPtr(2));
}

bool HCI_EV_ULP_T_PDU::size_expected() const
{
	return 1;
}

uint8* HCI_EV_ULP_T_PDU::get_ulp_sub_event() const
{
	return this->get_uint8ArrayPtr(2);
}

uint8 HCI_EV_ULP_T_PDU::get_event_length() const
{
	return *(this->get_uint8ArrayPtr(1));
}

/*******************************************************************/


