#ifndef _XTRAEVENTPDUS_H
#define _XTRAEVENTPDUS_H
#include "hcieventpdu.h"

#define HCI_EV_MNFR_EXTENSION HCI_EV_MANUFACTURER_EXTENSION

class HCI_EV_LINK_KEY_NOTIFICATION_T_PDU : public HCIEventPDU
{
public:
    HCI_EV_LINK_KEY_NOTIFICATION_T_PDU ();
    HCI_EV_LINK_KEY_NOTIFICATION_T_PDU ( const PDU& );
    HCI_EV_LINK_KEY_NOTIFICATION_T_PDU ( const uint8* data , uint32 length );
    BluetoothDeviceAddress get_bd_addr (void) const;
    LinkKey get_link_key ( void ) const;
    uint8 get_key_type() const;
    bool size_expected() const;
};

class HCI_EV_INQUIRY_RESULT_T_PDU : public HCIEventPDU
{
public:
    HCI_EV_INQUIRY_RESULT_T_PDU() ;
    HCI_EV_INQUIRY_RESULT_T_PDU( const PDU& ) ;
    HCI_EV_INQUIRY_RESULT_T_PDU( const uint8* data , uint32 length ) ;
    uint8 get_num_responses(void) const;
    InquiryResult get_result(uint8 index) const;
    bool size_expected() const;
};

class HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T_PDU : public HCIEventPDU
{
public:
    HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T_PDU() ;
    HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T_PDU( const PDU& ) ;
    HCI_EV_READ_REM_SUPP_FEATURES_COMPLETE_T_PDU( const uint8* data , uint32 length ) ;
    uint8 get_status(void) const;
    uint16 get_handle(void) const;
    LMPSupportedFeatures get_lmp_supp_features(void) const;
    bool size_expected() const;
};
class HCI_EV_REMOTE_NAME_REQ_COMPLETE_T_PDU : public HCIEventPDU
{
public:
    HCI_EV_REMOTE_NAME_REQ_COMPLETE_T_PDU() ;
    HCI_EV_REMOTE_NAME_REQ_COMPLETE_T_PDU( const PDU& ) ;
    HCI_EV_REMOTE_NAME_REQ_COMPLETE_T_PDU( const uint8* data , uint32 length ) ;
    uint8 get_status(void) const;
    BluetoothDeviceAddress get_bd_addr(void) const;
    BluetoothName get_name_part(void) const;
    bool size_expected() const;
};

class HCI_EV_RETURN_LINK_KEYS_T_PDU : public HCIEventPDU
{
public:
    HCI_EV_RETURN_LINK_KEYS_T_PDU() ;
    HCI_EV_RETURN_LINK_KEYS_T_PDU( const PDU& ) ;
    HCI_EV_RETURN_LINK_KEYS_T_PDU( const uint8* data , uint32 length ) ;
    uint8 get_number_keys(void) const;
    void get_link_key_bd_addr(uint8 index,BluetoothDeviceAddress &addr,LinkKey &key) const;
    bool size_expected() const;
};

class HCI_EV_NUMBER_COMPLETED_PKTS_T_PDU : public HCIEventPDU
{
public:
    HCI_EV_NUMBER_COMPLETED_PKTS_T_PDU ();
    HCI_EV_NUMBER_COMPLETED_PKTS_T_PDU ( const PDU& );
    HCI_EV_NUMBER_COMPLETED_PKTS_T_PDU ( const uint8* data , uint32 length );
    uint8 get_num_handles () const;
    void get_num_completed_pkts ( uint8 index , uint16& handle , uint16& pkts ) const;
    bool size_expected() const;
};

class HCI_EV_LOOPBACK_COMMAND_T_PDU : public HCIEventPDU
{
public:
    HCI_EV_LOOPBACK_COMMAND_T_PDU() ;
    HCI_EV_LOOPBACK_COMMAND_T_PDU( const PDU& ) ;
    HCI_EV_LOOPBACK_COMMAND_T_PDU( const uint8* data , uint32 length ) ;
    const PDU get_loopback_part() const;
    bool size_expected() const;
};

class HCI_EV_MNFR_EXTENSION_T_PDU : public HCIEventPDU
{
public:
    HCI_EV_MNFR_EXTENSION_T_PDU();
    HCI_EV_MNFR_EXTENSION_T_PDU( const PDU& );
    HCI_EV_MNFR_EXTENSION_T_PDU( const uint8 * data , uint32 len );
    uint8 get_payload_descriptor () const;
    uint32 get_payload_length () const;
    const uint8* get_payload () const;
    inline const uint8* get_payloadPtr() const
    { return get_payload();}
    bool size_expected() const;
};

// BT1.2

class HCI_EV_INQUIRY_RESULT_WITH_RSSI_T_PDU : public HCIEventPDU
{
public:
    HCI_EV_INQUIRY_RESULT_WITH_RSSI_T_PDU() ;
    HCI_EV_INQUIRY_RESULT_WITH_RSSI_T_PDU( const PDU& ) ;
    HCI_EV_INQUIRY_RESULT_WITH_RSSI_T_PDU( const uint8* data , uint32 length ) ;
    uint8 get_num_responses(void) const;
    InquiryResultWithRSSI get_result(uint8 index) const;
    bool size_expected() const;
};

class HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_PDU : public HCIEventPDU
{
public:
    HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_PDU();
    HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_PDU( const PDU& );
    HCI_EV_READ_REM_EXT_FEATURES_COMPLETE_T_PDU( const uint8 * data , uint32 len );
    uint8 get_status () const;
    uint16 get_handle () const;
    uint8 get_page_num () const;
    uint8 get_max_page_num () const;
    LMPExtFeatures get_lmp_ext_features () const;
    bool size_expected() const;
};

// BT2.1

class HCI_EV_EXTENDED_INQUIRY_RESULT_T_PDU : public HCIEventPDU
{
public:
    HCI_EV_EXTENDED_INQUIRY_RESULT_T_PDU();
    HCI_EV_EXTENDED_INQUIRY_RESULT_T_PDU( const PDU& );
    HCI_EV_EXTENDED_INQUIRY_RESULT_T_PDU( const uint8 * data , uint32 len );
    uint8 get_num_responses () const; // always 1
    const BluetoothDeviceAddress get_bd_addr() const;
    uint8 get_page_scan_rep_mode() const;
    uint8 get_page_scan_period_mode() const;
    const ClassOfDevice get_class_of_device() const;
    uint24 get_dev_class() const;
    uint16 get_clock_offset() const;
    uint8 get_rssi() const;
    const EIRData get_eir_data_part () const;
    bool size_expected() const;
};

class HCI_EV_REM_HOST_SUPPORTED_FEATURES_T_PDU : public HCIEventPDU
{
public:
    HCI_EV_REM_HOST_SUPPORTED_FEATURES_T_PDU();
    HCI_EV_REM_HOST_SUPPORTED_FEATURES_T_PDU( const PDU& );
    HCI_EV_REM_HOST_SUPPORTED_FEATURES_T_PDU( const uint8 * data , uint32 len );
    const BluetoothDeviceAddress get_bd_addr() const;
    LMPExtFeatures get_host_features () const;
    bool size_expected() const;
};

class HCI_EV_ULP_T_PDU : public HCIEventPDU
{
public:
    HCI_EV_ULP_T_PDU();
    HCI_EV_ULP_T_PDU( const PDU& );
    HCI_EV_ULP_T_PDU( const uint8 * data , uint32 len );
	uint8 get_ulp_sub_opcode() const;
	bool size_expected() const;
	uint8* get_ulp_sub_event() const;
	uint8 get_event_length() const;
};


#endif /*_XTRAEVENTPDUS_H*/
