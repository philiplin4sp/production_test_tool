#include "bluetoothclasses.h"

#include <memory.h>
#include <string.h>
#include <stdio.h>

PinCode::PinCode ( uint8 * src, uint8 length )
: mLength ( length > sizeof(mData) ? sizeof(mData) : length )
{
    memcpy(mData,src,mLength) ;
    memset(mData+mLength,0,sizeof(mData)-mLength);
}

const uint8 * PinCode::get_data() const
{
    return mData;
}

uint8 PinCode::get_length() const
{
    return mLength;
}

HCIEventMask::HCIEventMask ( const uint8 * mask )
{
    memcpy ( mData , mask , 8 );
}

void HCIEventMask::set_all ( bool a )
{
    mData[0] = a ? 0xFF : 0;
    mData[1] = a ? 0xFF : 0;
    mData[2] = a ? 0xFF : 0;
    mData[3] = a ? 0xFF : 0;
    mData[4] = 0;
    mData[5] = 0;
    mData[6] = 0;
    mData[7] = 0;
}

void HCIEventMask::set_inquiry_complete_event_mask ( bool a )
{
    if ( a )
        mData[0] |= 1;
    else
        mData[0] &= 0xFE;
}

void HCIEventMask::set_inquiry_result_event_mask ( bool a )
{
    if ( a )
        mData[0] |= 2;
    else
        mData[0] &= 0xFD;
}

void HCIEventMask::set_connection_complete_event_mask ( bool a )
{
    if ( a )
        mData[0] |= 4;
    else
        mData[0] &= 0xFB;
}

void HCIEventMask::set_connection_request_event_mask ( bool a )
{
    if ( a )
        mData[0] |= 8;
    else
        mData[0] &= 0xF7;
}

void HCIEventMask::set_disconnection_complete_event_mask ( bool a )
{
    if ( a )
        mData[0] |= 0x10;
    else
        mData[0] &= 0xEF;
}

void HCIEventMask::set_authentication_complete_event_mask ( bool a )
{
    if ( a )
        mData[0] |= 0x20;
    else
        mData[0] &= 0xDF;
}

void HCIEventMask::set_remote_name_request_complete_event_mask ( bool a )
{
    if ( a )
        mData[0] |= 0x40;
    else
        mData[0] &= 0xBF;
}

void HCIEventMask::set_encryption_change_event_mask ( bool a )
{
    if ( a )
        mData[0] |= 0x80;
    else
        mData[0] &= 0x7F;
}

void HCIEventMask::set_change_connection_link_key_complete_event_mask ( bool a )
{
    if ( a )
        mData[1] |= 1;
    else
        mData[1] &= 0xFE;
}

void HCIEventMask::set_master_link_key_complete_event_mask ( bool a )
{
    if ( a )
        mData[1] |= 2;
    else
        mData[1] &= 0xFD;
}

void HCIEventMask::set_read_remote_supported_features_complete_event_mask ( bool a )
{
    if ( a )
        mData[1] |= 4;
    else
        mData[1] &= 0xFB;
}

void HCIEventMask::set_read_remote_version_information_complete_event_mask ( bool a )
{
    if ( a )
        mData[1] |= 8;
    else
        mData[1] &= 0xF7;
}

void HCIEventMask::set_qos_setup_complete_event_mask ( bool a )
{
    if ( a )
        mData[1] |= 0x10;
    else
        mData[1] &= 0xEF;
}

void HCIEventMask::set_command_complete_event_mask ( bool a )
{
    if ( a )
        mData[1] |= 0x20;
    else
        mData[1] &= 0xDF;
}

void HCIEventMask::set_command_status_event_mask ( bool a )
{
    if ( a )
        mData[1] |= 0x40;
    else
        mData[1] &= 0xBF;
}

void HCIEventMask::set_hardware_error_event_mask ( bool a )
{
    if ( a )
        mData[1] |= 0x80;
    else
        mData[1] &= 0x7F;
}

void HCIEventMask::set_flush_occurred_event_mask ( bool a )
{
    if ( a )
        mData[2] |= 1;
    else
        mData[2] &= 0xFE;
}

void HCIEventMask::set_role_change_event_mask ( bool a )
{
    if ( a )
        mData[2] |= 2;
    else
        mData[2] &= 0xFD;
}

void HCIEventMask::set_number_of_completed_packets_event_mask ( bool a )
{
    if ( a )
        mData[2] |= 4;
    else
        mData[2] &= 0xFB;
}

void HCIEventMask::set_mode_change_event_mask ( bool a )
{
    if ( a )
        mData[2] |= 8;
    else
        mData[2] &= 0xF7;
}

void HCIEventMask::set_return_link_keys_event_mask ( bool a )
{
    if ( a )
        mData[2] |= 0x10;
    else
        mData[2] &= 0xEF;
}

void HCIEventMask::set_pin_code_request_event_mask ( bool a )
{
    if ( a )
        mData[2] |= 0x20;
    else
        mData[2] &= 0xDF;
}

void HCIEventMask::set_link_key_request_event_mask ( bool a )
{
    if ( a )
        mData[2] |= 0x40;
    else
        mData[2] &= 0xBF;
}

void HCIEventMask::set_link_key_notification_event_mask ( bool a )
{
    if ( a )
        mData[2] |= 0x80;
    else
        mData[2] &= 0x7F;
}

void HCIEventMask::set_loopback_command_event_mask ( bool a )
{
    if ( a )
        mData[3] |= 1;
    else
        mData[3] &= 0xFE;
}

void HCIEventMask::set_data_buffer_overflow_event_mask ( bool a )
{
    if ( a )
        mData[3] |= 2;
    else
        mData[3] &= 0xFD;
}

void HCIEventMask::set_max_slots_change_event_mask ( bool a )
{
    if ( a )
        mData[3] |= 4;
    else
        mData[3] &= 0xFB;
}

void HCIEventMask::set_read_clock_offset_complete_event_mask ( bool a )
{
    if ( a )
        mData[3] |= 8;
    else
        mData[3] &= 0xF7;
}

void HCIEventMask::set_connection_packet_type_changed_event_mask ( bool a )
{
    if ( a )
        mData[3] |= 0x10;
    else
        mData[3] &= 0xEF;
}

void HCIEventMask::set_qos_violation_event_mask ( bool a )
{
    if ( a )
        mData[3] |= 0x20;
    else
        mData[3] &= 0xDF;
}

void HCIEventMask::set_page_scan_mode_change_event_mask ( bool a )
{
    if ( a )
        mData[3] |= 0x40;
    else
        mData[3] &= 0xBF;
}

void HCIEventMask::set_page_scan_repetition_mode_change_event_mask ( bool a )
{
    if ( a )
        mData[3] |= 0x80;
    else
        mData[3] &= 0x7F;
}

bool HCIEventMask::get_inquiry_complete_event_mask () const
{
    return ( mData[0] & 0x01 );
}

bool HCIEventMask::get_inquiry_result_event_mask () const
{
    return ( ( mData[0] & 0x02 ) == 0x02 );
}

bool HCIEventMask::get_connection_complete_event_mask () const
{
    return ( ( mData[0] & 0x04 ) == 0x04 );
}

bool HCIEventMask::get_connection_request_event_mask () const
{
    return ( ( mData[0] & 0x08 ) == 0x08 );
}

bool HCIEventMask::get_disconnection_complete_event_mask () const
{
    return ( ( mData[0] & 0x10 ) == 0x10 );
}

bool HCIEventMask::get_authentication_complete_event_mask () const
{
    return ( ( mData[0] & 0x20 ) == 0x20 );
}

bool HCIEventMask::get_remote_name_request_complete_event_mask () const
{
    return ( ( mData[0] & 0x40 ) == 0x40 );
}

bool HCIEventMask::get_encryption_change_event_mask () const
{
    return ( ( mData[0] & 0x80 ) == 0x80 );
}

bool HCIEventMask::get_change_connection_link_key_complete_event_mask () const
{
    return ( ( mData[1] & 0x01 ) == 0x01 );
}

bool HCIEventMask::get_master_link_key_complete_event_mask () const
{
    return ( ( mData[1] & 0x02 ) == 0x02 );
}

bool HCIEventMask::get_read_remote_supported_features_complete_event_mask () const
{
    return ( ( mData[1] & 0x04 ) == 0x04 );
}

bool HCIEventMask::get_read_remote_version_information_complete_event_mask () const
{
    return ( ( mData[1] & 0x08 ) == 0x08 );
}

bool HCIEventMask::get_qos_setup_complete_event_mask () const
{
    return ( ( mData[1] & 0x10 ) == 0x10 );
}

bool HCIEventMask::get_command_complete_event_mask () const
{
    return ( ( mData[1] & 0x20 ) == 0x20 );
}

bool HCIEventMask::get_command_status_event_mask () const
{
    return ( ( mData[1] & 0x40 ) == 0x40 );
}

bool HCIEventMask::get_hardware_error_event_mask () const
{
    return ( ( mData[1] & 0x80 ) == 0x80 );
}

bool HCIEventMask::get_flush_occurred_event_mask () const
{
    return ( ( mData[2] & 0x01 ) == 0x01 );
}

bool HCIEventMask::get_role_change_event_mask () const
{
    return ( ( mData[2] & 0x02 ) == 0x02 );
}

bool HCIEventMask::get_number_of_completed_packets_event_mask () const
{
    return ( ( mData[2] & 0x04 ) == 0x04 );
}

bool HCIEventMask::get_mode_change_event_mask () const
{
    return ( ( mData[2] & 0x08 ) == 0x08 );
}

bool HCIEventMask::get_return_link_keys_event_mask () const
{
    return ( ( mData[2] & 0x10 ) == 0x10 );
}

bool HCIEventMask::get_pin_code_request_event_mask () const
{
    return ( ( mData[2] & 0x20 ) == 0x20 );
}

bool HCIEventMask::get_link_key_request_event_mask () const
{
    return ( ( mData[2] & 0x40 ) == 0x40 );
}

bool HCIEventMask::get_link_key_notification_event_mask () const
{
    return ( ( mData[2] & 0x80 ) == 0x80 );
}

bool HCIEventMask::get_loopback_command_event_mask () const
{
    return ( mData[3] & 0x01 );
}

bool HCIEventMask::get_data_buffer_overflow_event_mask () const
{
    return ( ( mData[3] & 0x02 ) == 0x02 );
}

bool HCIEventMask::get_max_slots_change_event_mask () const
{
    return ( ( mData[3] & 0x04 ) == 0x04 );
}

bool HCIEventMask::get_read_clock_offget_complete_event_mask () const
{
    return ( ( mData[3] & 0x08 ) == 0x08 );
}

bool HCIEventMask::get_connection_packet_type_changed_event_mask () const
{
    return ( ( mData[3] & 0x10 ) == 0x10 );
}

bool HCIEventMask::get_qos_violation_event_mask () const
{
    return ( ( mData[3] & 0x20 ) == 0x20 );
}

bool HCIEventMask::get_page_scan_mode_change_event_mask () const
{
    return ( ( mData[3] & 0x40 ) == 0x40 );
}

bool HCIEventMask::get_page_scan_repetition_mode_change_event_mask () const
{
    return ( ( mData[3] & 0x80 ) == 0x80 );
}

const uint8 * HCIEventMask::get_data() const
{
    return mData;
}

BluetoothDeviceAddress::BluetoothDeviceAddress(uint16 nap,uint8 uap,uint32 lap) 
{
    set_lap(lap) ;
    set_uap(uap) ;
    set_nap(nap) ;
}

BluetoothDeviceAddress::BluetoothDeviceAddress( const uint8 * src )
:   mLap ( uint24(src[0]) | ( uint24(src[1]) << 8 ) | ( uint24(src[2]) << 16 ) ),
    mUap ( src[3] ),
    mNap ( uint16(src[4]) | (uint16(src[5]) << 8) )
{
}

uint8 BluetoothDeviceAddress::get_uap() const
{
    return mUap ;
}
    
uint16 BluetoothDeviceAddress::get_nap() const
{
    return mNap ;
}

uint32 BluetoothDeviceAddress::get_lap() const
{
    return mLap ;
}

void BluetoothDeviceAddress::set_uap(uint8 val) 
{
    mUap = val ;
}

void BluetoothDeviceAddress::set_nap(uint16 val) 
{
    mNap = val ;
}

void BluetoothDeviceAddress::set_lap(uint32 val) 
{
    mLap = val ;
}


bool BluetoothDeviceAddress::equals ( const BluetoothDeviceAddress &another) const
{
    return  mLap == another.mLap && mUap == another.mUap && mNap == another.mNap;
}

void BluetoothDeviceAddress::MakeAddressString(char* address_string)
{
	sprintf(address_string, 
                "LAP:%06x  UAP:%02x  NAP:%04x",	
                (unsigned int)mLap,
                mUap,
                mNap);
}

BluetoothDeviceAddressWithType::BluetoothDeviceAddressWithType ( uint8 type ,
                                                                 uint16 nap ,
                                                                 uint8 uap ,
                                                                 uint32 lap )
{
    set_type(type);
    set_addr(BluetoothDeviceAddress(nap, uap, lap));
}

BluetoothDeviceAddressWithType::BluetoothDeviceAddressWithType ( uint8 type,
                                                                 const BluetoothDeviceAddress &addr)
{
    set_type(type);
    set_addr(addr);

}
BluetoothDeviceAddressWithType::BluetoothDeviceAddressWithType ( const uint8 * src)
{
    set_type(src[0]);
    set_addr(BluetoothDeviceAddress(&src[1]));
}

uint8 BluetoothDeviceAddressWithType::get_type() const
{
    return mType;
}

const char *BluetoothDeviceAddressWithType::get_type_text() const
{
    switch(get_type())
    {
        case 0:
            return text_public;

        case 1:
            switch(get_addr().get_nap() & 0xC000)
            {
                case 0x0000:
                    return text_private_nonresolvable;

                case 0x4000:
                    return text_private_resolvable;

                case 0xC000:
                    return text_static;
            }

            return text_invalid_random;
    }

    return text_invalid_type;

}

const char BluetoothDeviceAddressWithType::text_public[] = "Public";
const char BluetoothDeviceAddressWithType::text_static[] = "Static";
const char BluetoothDeviceAddressWithType::text_private_resolvable[] = "Private Resolvable";
const char BluetoothDeviceAddressWithType::text_private_nonresolvable[] = "Private Non-Resolvable";
const char BluetoothDeviceAddressWithType::text_invalid_type[] = "Invalid Type";
const char BluetoothDeviceAddressWithType::text_invalid_random[] = "Invalid Random";

BluetoothDeviceAddress BluetoothDeviceAddressWithType::get_addr() const
{
    return mAddr;
}

void BluetoothDeviceAddressWithType::set_type(uint8 type)
{
    mType = type;
}

void BluetoothDeviceAddressWithType::set_addr(const BluetoothDeviceAddress &addr)
{
    mAddr = addr;
}

bool BluetoothDeviceAddressWithType::equals( const BluetoothDeviceAddressWithType &another ) const
{
    return get_type() == another.get_type() && get_addr().equals(another.get_addr());
}


LinkKey::LinkKey ( const uint8 * src )
{
    if (src)
    {
        memcpy(mData,src,sizeof(mData)) ;
        valid = true;
    }
    else
        valid = false;
}

const uint8 * LinkKey::get_data() const
{
    return valid ? mData : 0;
}

bool LinkKey::equals( const LinkKey &another ) const
{
    if (valid && another.get_data())
        return !memcmp ( get_data() , another.get_data() , sizeof(mData) ) ;
    else
        return (valid == (another.get_data() != 0));
}


BluetoothName::BluetoothName ( const uint8 * src , uint8 length , bool isBinary)
: mLength ( length >= sizeof(mData) ? sizeof(mData) - 1 : length )
{
    /* Check is the data is to be treated as binary data or a string */
    if (isBinary)
    {
        memset ( mData, 0 , sizeof(mData) );
        memcpy((char*) mData,(const char*) src , mLength ) ;
    }
    else
    {
        memset ( mData, 0 , sizeof(mData) );
        strncpy((char*) mData,(const char*) src , mLength ) ;
        mLength =(uint8) strlen((const char*) mData) ;
    }
}

const uint8 * BluetoothName::get_str() const
{
    return mData ;
}

size_t BluetoothName::get_length() const
{
    return mLength ;
}

EIRData::EIRData ( const uint8 * src , uint8 length )
: mLength ( length >= sizeof(mData) ? sizeof(mData) : length )
{
    memset( mData, 0 , sizeof(mData) );
    memcpy((char*) mData, (const char*) src , mLength ) ;
}

const uint8 * EIRData::get_data() const
{
    return mData ;
}

uint8 EIRData::get_length() const
{
    return mLength ;
}

LMPSupportedFeatures::LMPSupportedFeatures ( const uint8 * src )
{
    if ( src )
        memcpy(mData,src,sizeof(mData)) ;
}

const uint8 * LMPSupportedFeatures::get_data() const
{
    return mData;
}
LMPExtFeatures::LMPExtFeatures ( const uint8 * src )
{
    if ( src )
        memcpy(mData,src,sizeof(mData)) ;
}

const uint8 * LMPExtFeatures::get_data() const
{
    return mData;
}

SupportedCommands::SupportedCommands ( const uint8 * src )
{
    if ( src )
        memcpy(mData,src,sizeof(mData)) ;
}

const uint8 * SupportedCommands::get_data() const
{
    return mData;
}

ClassOfDevice::ClassOfDevice( const uint8 * src )
{
    if ( src )
        memcpy(mData,src,sizeof(mData)) ;
}

ClassOfDevice::ClassOfDevice( const uint24 cod )
{
    mData[0] = (uint8)(   cod         & 0xFF);
    mData[1] = (uint8)( ( cod >> 8  ) & 0xFF);
    mData[2] = (uint8)( ( cod >> 16 ) & 0xFF);
}

uint24 ClassOfDevice::get_class_of_device() const
{
    return uint24(mData[0]) | ( uint24(mData[1]) << 8 ) | ( uint24(mData[2]) << 16 );
}

BasicInquiryResult::BasicInquiryResult ()
:   mBdAddr (0,0,0),
    mPSrepMode ( 0 ),
    mPSperiodMode ( 0 ),
    mCod () ,
    mClockOffset ( 0 )
{
}

BasicInquiryResult::BasicInquiryResult( BluetoothDeviceAddress aBdAddr,
                                        uint8 aPSrepMode,
                                        uint8 aPSperiodMode,
                                        ClassOfDevice aCod,
                                        uint16 aClockOffset )
:   mBdAddr (aBdAddr),
    mPSrepMode (aPSrepMode),
    mPSperiodMode (aPSperiodMode),
    mCod (aCod) ,
    mClockOffset (aClockOffset)
{
}

const BluetoothDeviceAddress& BasicInquiryResult::get_bd_addr() const
{
    return mBdAddr;
}

uint8 BasicInquiryResult::get_page_scan_repetition_mode() const
{
    return mPSrepMode;
}

uint8 BasicInquiryResult::get_page_scan_period_mode() const
{
    return mPSperiodMode ;
}

uint24 BasicInquiryResult::get_class_of_device() const
{
    return mCod.get_class_of_device();
}

const ClassOfDevice& BasicInquiryResult::get_ClassOfDevice() const
{
    return mCod ;
}

uint16 BasicInquiryResult::get_clock_offset() const
{
    return mClockOffset ;
}

void BasicInquiryResult::set_bd_addr ( const BluetoothDeviceAddress& a )
{
    mBdAddr = a;
}

void BasicInquiryResult::set_page_scan_repetition_mode ( uint8 a )
{
    mPSrepMode = a;
}

void BasicInquiryResult::set_page_scan_period_mode( uint8 a )
{
    mPSperiodMode = a;
}

void BasicInquiryResult::set_class_of_device( uint24 a )
{
    mCod = a;
}

void BasicInquiryResult::set_ClassOfDevice( const ClassOfDevice& a )
{
    mCod = a;
}

void BasicInquiryResult::set_clock_offset( uint16 a )
{
    mClockOffset = a;
}

InquiryResult::InquiryResult()
:    mPSM ( 0 )
{
}

InquiryResult::InquiryResult ( const uint8 * src )
:   BasicInquiryResult ( BluetoothDeviceAddress (src),
                         src[6] , src[7] ,
                         ClassOfDevice ( src + 9 ) ,
                         ( uint16(src[12]) | ( uint16(src[13]) << 8 ) ) ),
    mPSM ( src[8] )
{
}

uint8 InquiryResult::get_page_scan_mode() const
{
    return mPSM ;
}

void InquiryResult::set_page_scan_mode( uint8 a )
{
    mPSM = a;
}

InquiryResultWithRSSI::InquiryResultWithRSSI ( const uint8 * src )
:   BasicInquiryResult ( BluetoothDeviceAddress (src),
                         src[6] , src[7] ,
                         ClassOfDevice ( src + 8 ) ,
                         ( uint16(src[11]) | ( uint16(src[12]) << 8 ) ) ),
    mRssi ( src[13] )
{
}

uint8 InquiryResultWithRSSI::get_rssi() const
{
    return mRssi;
}

void InquiryResultWithRSSI::set_rssi( uint8 rssi )
{
    mRssi = rssi;
}


AFHmap::AFHmap ( const uint8 * src )
{
    if ( src )
        memcpy ( data , src , 10 );
    else
        memset ( data , 0 , 10 );
}

const uint8* AFHmap::get_ptr() const
{
    return data;
}

bool AFHmap::get_val ( uint8 channel ) const
{
    if ( channel < 80 )
        return ( ( data[channel/8] & ( 1 << (channel % 8) ) ) == ( 1 << (channel % 8) ) );
    else
        return false;
}

void AFHmap::set_val ( uint8 channel , bool val )
{
    if ( val )
        data[channel/8] |= ( 1 << (channel % 8) );
    else
        data[channel/8] &= ( 1 << (channel % 8) );

}

SPhashC::SPhashC ( const uint8 * src )
{
    if (src)
    {
        memcpy(c, src, sizeof(c));
        valid = true;
    }
    else
        valid = false;
}

const uint8 * SPhashC::get_data() const
{
    return valid ? c : 0;
}

SPrandomizerR::SPrandomizerR ( const uint8 * src )
{
    if (src)
    {
        memcpy(r, src, sizeof(r));
        valid = true;
    }
    else
        valid = false;
}

const uint8 * SPrandomizerR::get_data() const
{
    return valid ? r : 0;
}

IPAddress::IPAddress ( uint16 * src )
{
    if ( src )
        memcpy ( data , src , sizeof(data) );
    else
        memset ( data , 0 , sizeof(data) );
}

const uint16* IPAddress::get_data() const
{
    return data;
}

EtherNetAddress::EtherNetAddress ( uint16 * src )
{
    if ( src )
    {
        data[0] = src[0];
        data[1] = src[1];
        data[2] = src[2];
    }
    else
        memset ( data , 0 , sizeof(data) );
}

const uint16* EtherNetAddress::get_data() const
{
    return data;
}

ULPEventMask::ULPEventMask ( const uint8 * mask )
{
    if (mask == 0)
        memset (mData, 0, 8);
    else
        memcpy ( mData , mask , 8 );
}

const uint8 * ULPEventMask::get_data() const
{
    return mData;
}

ULPChannelMap::ULPChannelMap ( const uint8 * src )
{
    if ( src )
        memcpy ( data , src , 5 );
    else
        memset ( data , 0 , 5 );
}

const uint8* ULPChannelMap::get_ptr() const
{
    return data;
}

bool ULPChannelMap::get_val ( uint8 channel ) const
{
    if ( channel < 40 )
        return ( ( data[channel/8] & ( 1 << (channel % 8) ) ) == ( 1 << (channel % 8) ) );
    else
        return false;
}

void ULPChannelMap::set_val ( uint8 channel , bool val )
{
    if ( val )
        data[channel/8] |= ( 1 << (channel % 8) );
    else
        data[channel/8] &= ( 1 << (channel % 8) );

}

ULPKey::ULPKey ( const uint8 * src )
{
    if ( src )
        memcpy ( key , src , 16 );
    else
        memset ( key, 0 , 16 );
}

const uint8* ULPKey::get_str() const
{
    return key;
}

ULPRandom::ULPRandom(const uint8 *src )
{
    if ( src )
        memcpy ( data , src , 8 );
    else
        memset ( data , 0 , 8 );
}

const uint8* ULPRandom::get_ptr() const
{
    return data;
}

DATA_uint8_len_16_ret::DATA_uint8_len_16_ret( const uint8 * src )
{
    if (src)
    {
        memcpy(data, src, sizeof(data));
        valid = true;
    }
    else
        valid = false;
}

const uint8* DATA_uint8_len_16_ret::get_ptr() const
{
    return valid ? data : 0;
}


DATA_uint8_len_32_ret::DATA_uint8_len_32_ret( const uint8 * src )
{
    if (src)
    {
        memcpy(data, src, sizeof(data));
        valid = true;
    }
    else
        valid = false;
}

const uint8* DATA_uint8_len_32_ret::get_ptr() const
{
    return valid ? data : 0;
}

ULPSupportedFeatures::ULPSupportedFeatures ( const uint8 * src )
{
    if ( src )
        memcpy(Features, src, sizeof(Features)) ;
}

const uint8 * ULPSupportedFeatures::get_ptr() const
{
    return Features;
}

ULPSupportedStates::ULPSupportedStates ( const uint8 * src )
{
    if ( src )
        memcpy(States, src, sizeof(States)) ;
}

const uint8 * ULPSupportedStates::get_ptr() const
{
    return States;
}


// Bluestack DM role-switch policy table.
DMRoleSwitchPolicyTable::DMRoleSwitchPolicyTable(uint16 * new_rs_table)
{
    rs_table = new_rs_table;
}

uint16* DMRoleSwitchPolicyTable::get_rs_table() const
{
    return rs_table;
}

void DMRoleSwitchPolicyTable::put_rs_table(uint16 * new_rs_table)
{
    rs_table = new_rs_table;
}

uint8 DMRoleSwitchPolicyTable::slaves(const uint8 index)
{
    return rs_table == 0 ? 0xFF : rs_table[index] & 7;
}

uint8 DMRoleSwitchPolicyTable::masters(const uint8 index)
{
    return rs_table == 0 ? 0xFF : (rs_table[index] >> 3) & 7;
}

uint8 DMRoleSwitchPolicyTable::unsniffed_masters(const uint8 index)
{
    return rs_table == 0 ? 0xFF : (rs_table[index] >> 6) & 7;
}

uint8 DMRoleSwitchPolicyTable::connections(const uint8 index)
{
    return rs_table == 0 ? 0xFF : (rs_table[index] >> 9) & 7;
}

bool DMRoleSwitchPolicyTable::roleswitch_incoming(const uint8 index)
{
    return rs_table == 0 ? false : (rs_table[index] & 0x1000) != 0;
}

bool DMRoleSwitchPolicyTable::forbid_roleswitch_outgoing(const uint8 index)
{
    return rs_table == 0 ? false : (rs_table[index] & 0x2000) != 0;
}

/* L2caConfigBase */
L2caConfigBase::L2caConfigBase(void)
{
    write = data;
}

L2caConfigBase::L2caConfigBase(const uint8* src,
                               const uint8 length)
{
    write = data;

    if (src != 0)
        put_data(length, src);
}

L2caConfigBase::L2caConfigBase(const uint32 *src,
                               const uint8 size,
                               const Config_Offsets_T *offsets)
{
    write = data;

    write_uints(src, size, offsets);
}

void L2caConfigBase::write_uint_basic(uint32 value, const uint8 octets)
{
    if (write - data + octets <= (int)sizeof(data))
    {
        for (uint8 i = octets; i != 0; --i)
        {
            *write++ = static_cast<uint8>(value & 0xFF);
            value >>= 8;
        }
    }
}

uint32 L2caConfigBase::read_uint(const Config_Offsets_T index, const uint8 octets) const
{
    uint32 value = 0;

    if (index < (int)sizeof(data))
    {
        for (uint8 i = octets; i != 0; --i)
        {
            value <<= 8;
            value |= data[index + i - 1];
        }
    }

    return value;
}

const uint8* L2caConfigBase::get_data() const
{
    return data;
}

const uint32* L2caConfigBase::get_decode() const
{
    return decoded_data;
}

void L2caConfigBase::put_data(uint8 length, const uint8 *src)
{
    while (length-- != 0)
        *write++ = *src++;
}

size_t L2caConfigBase::get_length() const
{
    return write - data;
}

void L2caConfigBase::write_uints(const uint32 *src,
                                 const uint8 size,
                                 const Config_Offsets_T *offsets)
{
    if (src != 0)
        for (uint8 i = 0; i != size; ++i)
            write_uint_basic(src[i], offsets[i + 1] - offsets[i]);
}

// QoS on-wire structure
L2caQos::L2caQos( const uint32 *src )
: L2caConfigBase(src, L2CAQOS_NUM_ELEMENTS, offsets)
{
    decode_data();
}

L2caQos::L2caQos( const uint8 *src )
: L2caConfigBase(src, get_expected_length())
{
    decode_data();
}

uint8 L2caQos::get_expected_length() const
{
    return QOS_SIZE;
}

uint8 L2caQos::get_num_elements() const
{
    return L2CAQOS_NUM_ELEMENTS;
}

void L2caQos::decode_data(void)
{
    const Config_Offsets_T *co;
    uint32* data = decoded_data;

    for (co = offsets; *co != QOS_SIZE; ++co)
        *data++ = read_uint(*co, *(co+1) - *co);
}

uint8 L2caQos::flags() const
{
    return (uint8)read_uint(QOS_flags, 2);
}

uint8 L2caQos::service_type() const
{
    return (uint8)read_uint(QOS_service_type, 2);
}

uint32 L2caQos::token_rate() const
{
    return read_uint(QOS_token_rate, 4);
}

uint32 L2caQos::token_bucket() const
{
    return read_uint(QOS_token_bucket, 4);
}

uint32 L2caQos::peak_bw() const
{
    return read_uint(QOS_peak_bw, 4);
}

uint32 L2caQos::latency() const
{
    return read_uint(QOS_latency, 4);
}

uint32 L2caQos::delay_var() const
{
    return read_uint(QOS_delay_var, 4);
}

const Config_Offsets_T L2caQos::offsets[L2CAQOS_NUM_ELEMENTS+1] =
    {QOS_flags, QOS_service_type, QOS_token_rate, QOS_token_bucket,
    QOS_peak_bw, QOS_latency, QOS_delay_var, QOS_SIZE};

// FEC on-wire structure
L2caFlow::L2caFlow( const uint32 *src )
: L2caConfigBase(src, L2CAFLOW_NUM_ELEMENTS, offsets)
{
    decode_data();
}

L2caFlow::L2caFlow( const uint8 *src )
: L2caConfigBase(src, get_expected_length())
{
    decode_data();
}


uint8 L2caFlow::get_expected_length() const
{
    return Flow_SIZE;
}

uint8 L2caFlow::get_num_elements() const
{
    return L2CAFLOW_NUM_ELEMENTS;
}

void L2caFlow::decode_data(void)
{
    const Config_Offsets_T *co;
    uint32* data = decoded_data;

    for (co = offsets; *co != Flow_SIZE; ++co)
        *data++ = read_uint(*co, *(co+1) - *co);
}

uint8 L2caFlow::mode() const
{
    return (uint8)read_uint(Flow_mode, 2);
}

uint8 L2caFlow::tx_window() const
{
    return (uint8)read_uint(Flow_tx_window, 2);
}

uint8 L2caFlow::max_retransmit() const
{
    return (uint8)read_uint(Flow_max_retransmit, 2);
}

uint16 L2caFlow::retrans_timeout() const
{
    return (uint16)read_uint(Flow_retrans_timeout, 2);
}

uint16 L2caFlow::monitor_timeout() const
{
    return (uint16)read_uint(Flow_monitor_timeout, 2);
}

uint16 L2caFlow::maximum_pdu() const
{
    return (uint16)read_uint(Flow_maximum_pdu, 2);
}

const Config_Offsets_T L2caFlow::offsets[L2CAFLOW_NUM_ELEMENTS+1] =
    {Flow_mode, Flow_tx_window, Flow_max_retransmit,
    Flow_retrans_timeout, Flow_monitor_timeout, Flow_maximum_pdu, Flow_SIZE};

// AMP FlowSpec on-wire structure
L2caFlowSpec::L2caFlowSpec( const uint32 *src )
: L2caConfigBase(src, L2CAFLOWSPEC_NUM_ELEMENTS, offsets)
{
    decode_data();
}

L2caFlowSpec::L2caFlowSpec( const uint8 *src )
: L2caConfigBase(src, get_expected_length())
{
    decode_data();
}

uint8 L2caFlowSpec::get_expected_length() const
{
    return FS_SIZE;
}

uint8 L2caFlowSpec::get_num_elements() const
{
    return L2CAFLOWSPEC_NUM_ELEMENTS;
}

void L2caFlowSpec::decode_data(void)
{
    const Config_Offsets_T *co;
    uint32* data = decoded_data;

    for (co = offsets; *co != FS_SIZE; ++co)
        *data++ = read_uint(*co, *(co+1) - *co);
}

uint8 L2caFlowSpec::fs_identifier() const
{
    return (uint8)read_uint(FS_identifier, 2);
}

uint8 L2caFlowSpec::fs_service_type() const
{
    return (uint8)read_uint(FS_service_type, 2);
}

uint16 L2caFlowSpec::fs_max_sdu() const
{
    return (uint16)read_uint(FS_max_sdu, 2);
}

uint32 L2caFlowSpec::fs_interarrival() const
{
    return read_uint(FS_interarrival, 4);
}

uint32 L2caFlowSpec::fs_latency() const
{
    return read_uint(FS_latency, 4);
}

uint32 L2caFlowSpec::fs_flush_to() const
{
    return read_uint(FS_flush_to, 4);
}

const Config_Offsets_T L2caFlowSpec::offsets[L2CAFLOWSPEC_NUM_ELEMENTS+1] =
    {FS_identifier, FS_service_type, FS_max_sdu, FS_interarrival,
    FS_latency, FS_flush_to, FS_SIZE};

// DM_SYNC_CONFIG_T
const uint8 *DMSyncConfig::get_data() const
{
    return data;
}

uint32 DMSyncConfig::encode_uint32(const uint32 value, const uint8 nbits) const
{
    // (un)pack uint32 Bluecore Friendly Format.
    if (nbits > 16)
        return (value >> 16) | ((value & 0xFFFF) << 16);

    return value;
}

void DMSyncConfig::write_uint(uint32 value, uint8 &offset, uint8 nbits)
{
    value = encode_uint32(value, nbits);

    while (nbits != 0)
    {
        data[offset++] = (uint8)(value & 0xFF);
        value >>= 8;
        nbits -= 8;
    }
}
uint32 DMSyncConfig::read_uint(uint8 offset, const uint8 nbits) const
{
    uint32 value = 0;

    for (uint8 shift = 0; shift != nbits; shift += 8)
        value |= (((uint32)(data[offset++] & 0xFF)) << shift);

    return encode_uint32(value, nbits);
}

DMSyncConfig::DMSyncConfig(const uint32 *src)
{
    uint8 offset = 0;
    const uint8 nbits[] = {16, 16, 16, 16, 32, 32};

    for (uint8 i = 0; i != sizeof(nbits); ++i)
        write_uint(src[i], offset, nbits[i]);
}

DMSyncConfig::DMSyncConfig(const uint8 *src, uint8 dummy)
{
    memcpy(data, src, get_data_size());
}

uint16 DMSyncConfig::max_latency() const
{
    return (uint16)read_uint(0, 16);
}

uint16 DMSyncConfig::retx_effort() const
{
    return (uint16)read_uint(2, 16);
}

uint16 DMSyncConfig::packet_type() const
{
    return (uint16)read_uint(4, 16);
}

uint16 DMSyncConfig::voice_settings() const
{
    return (uint16)read_uint(6, 16);
}

uint32 DMSyncConfig::tx_bdw() const
{
    return read_uint(8, 32);
}

uint32 DMSyncConfig::rx_bdw() const
{
    return read_uint(12, 32);
}

SMKeyState::SMKeyState(const uint16 *er, const uint16 *ir)
{
    set_er(er);
    set_ir(ir);
}

const uint16 *SMKeyState::get_ir(void) const
{
    return mIR;
}

const uint16 *SMKeyState::get_er(void) const
{
    return mER;
}

void SMKeyState::set_ir(const uint16 *value)
{
    memcpy(mIR, value, sizeof(mIR));
}

void SMKeyState::set_er(const uint16 *value)
{
    memcpy(mER, value, sizeof(mER));
}
