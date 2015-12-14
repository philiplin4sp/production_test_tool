#ifndef _XTRACOMMANDPDUS_H
#define _XTRACOMMANDPDUS_H

#include "hcipacker/hcicommandpdu.h"

#define HCI_MNFR_EXTENSION HCI_MANUFACTURER_EXTENSION

// Handwritten pdus, unique to HCI
// (ones shared with DM are in gen_xtrahcicommand.h)

class HCI_PIN_CODE_REQ_REPLY_T_PDU : public HCICommandPDU
{
public:
    HCI_PIN_CODE_REQ_REPLY_T_PDU() ;
    HCI_PIN_CODE_REQ_REPLY_T_PDU( const PDU& ) ;
    HCI_PIN_CODE_REQ_REPLY_T_PDU( const uint8* data , uint32 len ) ;
    const BluetoothDeviceAddress get_bd_addr() const;
    void set_bd_addr ( const BluetoothDeviceAddress& data );
    uint8 get_pin_code_length() const;
    void set_pin_code_length(uint8 data) ;
    const PinCode get_pin(void) const;
    void set_pin( const PinCode& data ) ;
    bool build ( const uint32 * const parameters );
};

class HCI_LINK_KEY_REQ_REPLY_T_PDU : public HCICommandPDU
{
public:
    HCI_LINK_KEY_REQ_REPLY_T_PDU() ;
    HCI_LINK_KEY_REQ_REPLY_T_PDU( const PDU& ) ;
    HCI_LINK_KEY_REQ_REPLY_T_PDU( const uint8* data , uint32 len ) ;
    BluetoothDeviceAddress get_bd_addr(void) const;
    void set_bd_addr(BluetoothDeviceAddress val) ;
    LinkKey get_key_val() const;
    void set_key_val( const LinkKey& data) ;
    bool build ( const uint32 * const parameters );
};

class HCI_SET_AFH_CHANNEL_CLASS_T_PDU : public HCICommandPDU
{
public:
    HCI_SET_AFH_CHANNEL_CLASS_T_PDU() ;
    HCI_SET_AFH_CHANNEL_CLASS_T_PDU( const PDU& ) ;
    HCI_SET_AFH_CHANNEL_CLASS_T_PDU( const uint8* data , uint32 len ) ;
    AFHmap get_map() const;
    void set_map (const AFHmap& data) ;
    bool build ( const uint32 * const parameters );
};

class HCI_MNFR_EXTENSION_T_PDU : public HCICommandPDU
{
public:
    HCI_MNFR_EXTENSION_T_PDU() ;
    HCI_MNFR_EXTENSION_T_PDU( const PDU& ) ;
    HCI_MNFR_EXTENSION_T_PDU( const uint8* data , uint32 len ) ;
    uint8 get_payload_descriptor() const ;
    void set_payload_descriptor(uint8 data) ;
    uint32 get_payload_length() const ;
    const uint8* get_payload() const ;
    void set_payload ( const uint8 * data , uint32 len );
    inline const uint8* get_payloadPtr() const
    { return get_payload();}
    bool build ( const uint32 * const parameters );
};

class HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T_PDU : public HCICommandPDU
{
public:
    HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T_PDU() ;
    HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T_PDU( const PDU& ) ;
    HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T_PDU( const uint8* data , uint32 len ) ;
    uint8 get_fec_required() const ;
    void set_fec_required(uint8 data) ;
    EIRData get_eir_data_part() const ;
    void set_eir_data_part(EIRData data) ;
    bool build ( const uint32 * const parameters );
};


// Simple Pairing
class HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_PDU : public HCICommandPDU
{
public:
    HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_PDU() ;
    HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_PDU( const PDU& ) ;
    HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_PDU( const uint8* data , uint32 len ) ;
    BluetoothDeviceAddress get_bd_addr(void) const;
    void set_bd_addr(BluetoothDeviceAddress val) ;
    SPhashC get_c() const;
    void set_c( const SPhashC& data) ;
    SPrandomizerR get_r() const;
    void set_r( const SPrandomizerR& data) ;
    bool build ( const uint32 * const parameters );
};

class HCI_ULP_SET_ADVERTISING_DATA_T_PDU : public HCICommandPDU
{
public:
    HCI_ULP_SET_ADVERTISING_DATA_T_PDU() ;
    HCI_ULP_SET_ADVERTISING_DATA_T_PDU( const PDU& ) ;
    HCI_ULP_SET_ADVERTISING_DATA_T_PDU( const uint8* data , uint32 len ) ;
    uint8 get_advertising_data_len(void) const ;
    void set_advertising_data_len(uint8 data) ;
    BluetoothName get_advertising_data(void) const ;
    void set_advertising_data(BluetoothName data) ;
    bool build ( const uint32 * const parameters );
};

class HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY_T_PDU : public HCICommandPDU
{
public:
    HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY_T_PDU() ;
    HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY_T_PDU( const PDU& ) ;
    HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY_T_PDU( const uint8* data , uint32 len ) ;
	uint16 get_connection_handle(void) const ;
    void set_connection_handle(uint16 data) ;
    ULPKey get_long_term_key(void) const;
    void set_long_term_key(const ULPKey& data);
    bool build ( const uint32 * const parameters );
};

class HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_T_PDU : public HCICommandPDU
{
public:
    HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_T_PDU() ;
    HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_T_PDU( const PDU& ) ;
    HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_T_PDU( const uint8* data , uint32 len ) ;
    ULPChannelMap get_channel_map(void) const ;
    void set_channel_map(const ULPChannelMap& data) ;
    bool build ( const uint32 * const parameters );
};

class HCI_ULP_SET_SCAN_RESPONSE_DATA_T_PDU : public HCICommandPDU
{
public:
    HCI_ULP_SET_SCAN_RESPONSE_DATA_T_PDU() ;
    HCI_ULP_SET_SCAN_RESPONSE_DATA_T_PDU( const PDU& ) ;
    HCI_ULP_SET_SCAN_RESPONSE_DATA_T_PDU( const uint8* data , uint32 len ) ;
    uint8 get_scan_response_data_len(void) const ;
    void set_scan_response_data_len(uint8 data) ;
    BluetoothName get_scan_response_data(void) const ;
    void set_scan_response_data(BluetoothName data) ;
    bool build ( const uint32 * const parameters );
};

class HCI_ULP_START_ENCRYPTION_T_PDU : public HCICommandPDU
{
public:
    HCI_ULP_START_ENCRYPTION_T_PDU() ;
    HCI_ULP_START_ENCRYPTION_T_PDU( const PDU& ) ; 
    HCI_ULP_START_ENCRYPTION_T_PDU( const uint8* data , uint32 len ) ;

    uint16 get_connection_handle(void) const ;
    void set_connection_handle(uint16 val) ;
    ULPRandom get_random_number(void) const ;
    void set_random_number(const ULPRandom& data ) ;
    uint16 get_encrypted_diversifier(void) const ;
    void set_encrypted_diversifier(uint16 val) ;
    ULPKey get_long_term_key(void) const ;
    void set_long_term_key(const ULPKey& data) ;
    bool build( const uint32 * const parameters );
};

class HCI_ULP_ENCRYPT_T_PDU : public HCICommandPDU
{
public:
    HCI_ULP_ENCRYPT_T_PDU() ;
    HCI_ULP_ENCRYPT_T_PDU( const PDU& ) ; 
    HCI_ULP_ENCRYPT_T_PDU( const uint8* data , uint32 len ) ;

    ULPKey get_aes_key(void) const ;
    void set_aes_key(const ULPKey& data ) ;

    ULPKey get_plaintext_data(void) const ;
    void set_plaintext_data(const ULPKey& data) ;

    bool build( const uint32 * const parameters );
};

class HCI_ULP_SET_EVENT_MASK_T_PDU : public HCICommandPDU
{
public:
    HCI_ULP_SET_EVENT_MASK_T_PDU() ;
    HCI_ULP_SET_EVENT_MASK_T_PDU( const PDU& ) ; 
    HCI_ULP_SET_EVENT_MASK_T_PDU( const uint8* data , uint32 len ) ;
    bool build( const uint32 * const parameters );

    ULPEventMask get_ulp_event_mask(void) const ;
    void set_ulp_event_mask(const ULPEventMask& data ) ;
};

#include "hcipacker/gen_xtrahcicommand.h"
#endif
