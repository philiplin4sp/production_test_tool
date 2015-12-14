
#ifndef _BLUETOOTHCLASSES_H
#define _BLUETOOTHCLASSES_H

#include "common/types.h"

class PinCode
{
public :
    PinCode(uint8 * src,uint8 length) ;
    const uint8 * get_data() const;
    uint8 get_length() const;
private:
    uint8 mData[16] ;
    uint8 mLength;
} ;

class HCIEventMask
{
public:
    HCIEventMask ( const uint8* mask );
    void set_all ( bool );
    void set_inquiry_complete_event_mask ( bool );
    void set_inquiry_result_event_mask ( bool );
    void set_connection_complete_event_mask ( bool );
    void set_connection_request_event_mask ( bool );
    void set_disconnection_complete_event_mask ( bool );
    void set_authentication_complete_event_mask ( bool );
    void set_remote_name_request_complete_event_mask ( bool );
    void set_encryption_change_event_mask ( bool );
    void set_change_connection_link_key_complete_event_mask ( bool );
    void set_master_link_key_complete_event_mask ( bool );
    void set_read_remote_supported_features_complete_event_mask ( bool );
    void set_read_remote_version_information_complete_event_mask ( bool );
    void set_qos_setup_complete_event_mask ( bool );
    void set_command_complete_event_mask ( bool );
    void set_command_status_event_mask ( bool );
    void set_hardware_error_event_mask ( bool );
    void set_flush_occurred_event_mask ( bool );
    void set_role_change_event_mask ( bool );
    void set_number_of_completed_packets_event_mask ( bool );
    void set_mode_change_event_mask ( bool );
    void set_return_link_keys_event_mask ( bool );
    void set_pin_code_request_event_mask ( bool );
    void set_link_key_request_event_mask ( bool );
    void set_link_key_notification_event_mask ( bool );
    void set_loopback_command_event_mask ( bool );
    void set_data_buffer_overflow_event_mask ( bool );
    void set_max_slots_change_event_mask ( bool );
    void set_read_clock_offset_complete_event_mask ( bool );
    void set_connection_packet_type_changed_event_mask ( bool );
    void set_qos_violation_event_mask ( bool );
    void set_page_scan_mode_change_event_mask ( bool );
    void set_page_scan_repetition_mode_change_event_mask ( bool );
    bool get_inquiry_complete_event_mask () const;
    bool get_inquiry_result_event_mask () const;
    bool get_connection_complete_event_mask () const;
    bool get_connection_request_event_mask () const;
    bool get_disconnection_complete_event_mask () const;
    bool get_authentication_complete_event_mask () const;
    bool get_remote_name_request_complete_event_mask () const;
    bool get_encryption_change_event_mask () const;
    bool get_change_connection_link_key_complete_event_mask () const;
    bool get_master_link_key_complete_event_mask () const;
    bool get_read_remote_supported_features_complete_event_mask () const;
    bool get_read_remote_version_information_complete_event_mask () const;
    bool get_qos_setup_complete_event_mask () const;
    bool get_command_complete_event_mask () const;
    bool get_command_status_event_mask () const;
    bool get_hardware_error_event_mask () const;
    bool get_flush_occurred_event_mask () const;
    bool get_role_change_event_mask () const;
    bool get_number_of_completed_packets_event_mask () const;
    bool get_mode_change_event_mask () const;
    bool get_return_link_keys_event_mask () const;
    bool get_pin_code_request_event_mask () const;
    bool get_link_key_request_event_mask () const;
    bool get_link_key_notification_event_mask () const;
    bool get_loopback_command_event_mask () const;
    bool get_data_buffer_overflow_event_mask () const;
    bool get_max_slots_change_event_mask () const;
    bool get_read_clock_offget_complete_event_mask () const;
    bool get_connection_packet_type_changed_event_mask () const;
    bool get_qos_violation_event_mask () const;
    bool get_page_scan_mode_change_event_mask () const;
    bool get_page_scan_repetition_mode_change_event_mask () const;
    const uint8 * get_data() const;
private:
    uint8 mData[8];
};

class ULPEventMask
{
public:
    ULPEventMask ( const uint8* mask = 0);
    const uint8 * get_data() const;
private:
    uint8 mData[8];
};


class BluetoothDeviceAddress
{
public :
    BluetoothDeviceAddress ( uint16 nap = 0 , uint8 uap = 0 , uint32 lap = 0 );
    BluetoothDeviceAddress ( const uint8 * src);
    uint8 get_uap() const;
    uint16 get_nap() const;
    uint24 get_lap() const;
    void set_uap(uint8 val) ;
    void set_nap(uint16 val) ;
    void set_lap(uint24 val) ;
    bool equals( const BluetoothDeviceAddress &another ) const;
	void MakeAddressString(char* address_string);

private :
    uint24 mLap;
    uint8 mUap;
    uint16 mNap;
} ;

class BluetoothDeviceAddressWithType
{
public:
    BluetoothDeviceAddressWithType ( uint8 type, uint16 nap = 0 , uint8 uap = 0 , uint32 lap = 0 );
    BluetoothDeviceAddressWithType ( uint8 type, const BluetoothDeviceAddress &addr);
    BluetoothDeviceAddressWithType ( const uint8 * src);
    uint8 get_type() const;
    const char *get_type_text() const;
    BluetoothDeviceAddress get_addr() const;
    void set_type(uint8 type) ;
    void set_addr(const BluetoothDeviceAddress &addr) ;
    bool equals( const BluetoothDeviceAddressWithType &another ) const;

private:
    uint8 mType;
    BluetoothDeviceAddress mAddr;
    static const char text_public[];
    static const char text_static[];
    static const char text_private_resolvable[];
    static const char text_private_nonresolvable[];
    static const char text_invalid_type[];
    static const char text_invalid_random[];
} ;

class LinkKey
{
public:
    LinkKey ( const uint8 * src = 0 ) ;
    const uint8 * get_data() const;
    bool equals ( const LinkKey &another ) const;
private :
    bool valid;
    uint8 mData[16] ;
} ;

class BluetoothName 
{
public:
    BluetoothName ( const uint8 * src , uint8 length , bool isBinary=false) ;
    const uint8 * get_str() const;
    size_t get_length() const;
private  :
    uint8 mData[249] ;
    uint8 mLength ;
} ;

class EIRData
{
public:
    EIRData ( const uint8 * src , uint8 length ) ;
    const uint8 * get_data() const;
    uint8 get_length() const;
private  :
    uint8 mData[240] ;
    uint8 mLength ;
} ;

class LMPSupportedFeatures
{
public:
    LMPSupportedFeatures ( const uint8 * src );
    const uint8 *get_data () const;
private:
    uint8 mData[8] ;
} ;

class LMPExtFeatures
{
public:
    LMPExtFeatures ( const uint8 * src );
    const uint8 *get_data () const;
private:
    uint8 mData[8] ;
} ;

class SupportedCommands
{
public:
    SupportedCommands ( const uint8 * src );
    const uint8 *get_data () const;
private:
    uint8 mData[64];
} ;


class ClassOfDevice
{
public:
    ClassOfDevice ( const uint8 *src = 0 );
    ClassOfDevice ( const uint24 cod );
    uint24 get_class_of_device() const;
private :
    uint8 mData[3] ;
} ;

class BasicInquiryResult
{
public :
    const BluetoothDeviceAddress& get_bd_addr() const;
    uint8 get_page_scan_repetition_mode() const;
    uint8 get_page_scan_period_mode() const;
    uint24 get_class_of_device() const;
    const ClassOfDevice& get_ClassOfDevice() const;
    uint16 get_clock_offset() const;

    void set_bd_addr ( const BluetoothDeviceAddress& );
    void set_page_scan_repetition_mode ( uint8 );
    void set_page_scan_period_mode( uint8 );
    void set_class_of_device( uint24 );
    void set_ClassOfDevice( const ClassOfDevice& );
    void set_clock_offset( uint16 );

protected:
    BasicInquiryResult();
    BasicInquiryResult( BluetoothDeviceAddress aBdAddr,
                        uint8 aPSrepMode,
                        uint8 aPSperiodMode,
                        ClassOfDevice aCod,
                        uint16 aClockOffset );
private:
    BluetoothDeviceAddress mBdAddr ;
    uint8 mPSrepMode ;
    uint8 mPSperiodMode ;
    ClassOfDevice mCod ;
    uint16 mClockOffset ;
} ;

class InquiryResult : public BasicInquiryResult
{
public:
    InquiryResult();
    InquiryResult( const uint8 * src );
    void set_page_scan_mode( uint8 );
    uint8 get_page_scan_mode() const;
private:
    uint8 mPSM ;
};

class InquiryResultWithRSSI : public BasicInquiryResult
{
public:
    InquiryResultWithRSSI ();
    InquiryResultWithRSSI ( const uint8 * src );

    uint8 get_rssi() const;
    void set_rssi ( uint8 );
private:
    uint8 mRssi;
};

// Adaptive Frquency Hopping
class AFHmap
{
public:
	AFHmap ( const uint8 * src = 0 );

	const uint8* get_ptr() const;
	bool get_val ( uint8 channel ) const;
	void set_val ( uint8 channel , bool val );
private:
	uint8 data[10];
};

//  Simple pairing
class SPhashC
{
public:
	SPhashC ( const uint8 * src = 0 );
	const uint8 * get_data() const;
private:
        bool valid;
	uint8 c[16];
};

class SPrandomizerR
{
public:
	SPrandomizerR ( const uint8 * src = 0 );
	const uint8 * get_data() const;
private:
        bool valid;
	uint8 r[16];
};

//  BNEP/TCP types

class IPAddress
{
public:
    IPAddress ( uint16 * src = 0 );
    const uint16* get_data() const;
private:
    uint16 data[8];
};

class EtherNetAddress
{
public:
    EtherNetAddress ( uint16 * src = 0 );
    const uint16* get_data() const;
private:
    uint16 data[3];
};

class ULPChannelMap
{
public:
	ULPChannelMap ( const uint8 * src = 0 );

	const uint8* get_ptr() const;
	bool get_val ( uint8 channel ) const;
	void set_val ( uint8 channel , bool val );
private:
	uint8 data[5];
};

class ULPKey
{
public:
	ULPKey ( const uint8 * src = 0 );

    const uint8* get_str() const;
private:
	uint8 key[16];
};

class ULPRandom
{
public:
        ULPRandom( const uint8 *src = 0);

        const uint8* get_ptr() const;
private:
        uint8 data[8];
};

// general case for data (16 uint8's) to be displayed
class DATA_uint8_len_16_ret
{
public:
	DATA_uint8_len_16_ret ( const uint8 * src = 0 );
	const uint8* get_ptr() const;
private:
        bool valid;
	uint8 data[16];
};

class DATA_uint8_len_32_ret
{
public:
	DATA_uint8_len_32_ret ( const uint8 * src = 0 );
	const uint8* get_ptr() const;
private:
        bool valid;
	uint8 data[32];
};

class ULPSupportedFeatures
{
public:
	ULPSupportedFeatures ( const uint8 * src = 0 );

	const uint8* get_ptr() const;
private:
	uint8 Features[8];
};

class ULPSupportedStates
{
public:
	ULPSupportedStates ( const uint8 * src = 0 );

	const uint8* get_ptr() const;
private:
	uint8 States[8];
};


// Bluestack DM role-switch policy table.
class DMRoleSwitchPolicyTable
{
public:
    DMRoleSwitchPolicyTable(uint16 * rs_table = 0);

    // Direct table access.
    uint16* get_rs_table() const;
    void put_rs_table(uint16 * rs_table);

    // Conditions
    uint8 slaves(const uint8 index);
    uint8 masters(const uint8 index);
    uint8 unsniffed_masters(const uint8 index);
    uint8 connections(const uint8 index);

    // Actions
    bool roleswitch_incoming(const uint8 index);
    bool forbid_roleswitch_outgoing(const uint8 index);
private:
    uint16* rs_table;
};

// L2CAP configuration blocks
typedef enum
{
    /* QOS */
    QOS_flags = 0,
    QOS_service_type = 2,
    QOS_token_rate = 4,
    QOS_token_bucket = 8,
    QOS_peak_bw = 12,
    QOS_latency = 16,
    QOS_delay_var = 20,
    QOS_SIZE = 24,

    /* Flow */
    Flow_mode = 0,
    Flow_tx_window = 2,
    Flow_max_retransmit = 4,
    Flow_retrans_timeout = 6,
    Flow_monitor_timeout = 8,
    Flow_maximum_pdu = 10,
    Flow_SIZE = 12,

    /* FlowSpec */
    FS_identifier = 0,
    FS_service_type = 2,
    FS_max_sdu = 4,
    FS_interarrival = 6,
    FS_latency = 10,
    FS_flush_to = 14,
    FS_SIZE = 18
} Config_Offsets_T;

#define L2CAQOS_NUM_ELEMENTS 7
#define L2CAFLOW_NUM_ELEMENTS 6
#define L2CAFLOWSPEC_NUM_ELEMENTS 6
#define L2CA_NUM_ELEMENTS_MAX 7
class L2caConfigBase
{
public:
    L2caConfigBase();
    L2caConfigBase(const uint8 *src, const uint8 length);
    L2caConfigBase(const uint32 *src, const uint8 size, const Config_Offsets_T *offsets);

    const uint8 *get_data() const;
    const uint32 *get_decode() const;
    size_t get_length() const;
protected:
    void put_data(uint8 length, const uint8 *put);
    void write_uints(const uint32 *src, const uint8 size, const Config_Offsets_T *offsets);
    uint32 read_uint(const Config_Offsets_T index, const uint8 octets) const;
    uint32 decoded_data[L2CA_NUM_ELEMENTS_MAX];
private:
    void write_uint_basic(uint32 value, const uint8 octets);
    uint8* write;
    uint8 data[24];
};

class L2caQos: public L2caConfigBase
{
public:
    L2caQos(const uint8 *src);
    L2caQos(const uint32 *src);
    uint8 get_expected_length() const;
    uint8 get_num_elements() const;
    void decode_data(void);
    uint8   flags() const;             /* 2/1 Flags */
    uint8   service_type() const;      /* 2/1 Best effort, etc */
    uint32  token_rate() const;        /* 4/4 Token rate */
    uint32  token_bucket() const;      /* 4/4 Token bucket */
    uint32  peak_bw() const;           /* 4/4 Peak bandwidth */
    uint32  latency() const;           /* 4/4 Latency */
    uint32  delay_var() const;         /* 4/4 Delay variation */
private:
    static const Config_Offsets_T offsets[L2CAQOS_NUM_ELEMENTS+1];
};

class L2caFlow: public L2caConfigBase
{
public:
    L2caFlow(const uint8 *src);
    L2caFlow(const uint32 *src);
    uint8 get_expected_length() const;
    uint8 get_num_elements() const;
    void decode_data(void);
    uint8   mode() const;              /*!< 2/1 Retransmission/flow control mode */
    uint8   tx_window() const;         /*!< 2/1 Size of transmit window */
    uint8   max_retransmit() const;    /*!< 2/1 Mmaximum number of retransmissions */
    uint16  retrans_timeout() const;   /*!< 2/2 Retransmission timeout, in ms */
    uint16  monitor_timeout() const;   /*!< 2/2 Monitor timeout, in ms */
    uint16  maximum_pdu() const;       /*!< 2/2 Maximum PDU size */
private:
    static const Config_Offsets_T offsets[L2CAFLOW_NUM_ELEMENTS+1];
};

class L2caFlowSpec: public L2caConfigBase
{
public:
    L2caFlowSpec(const uint8 *src);
    L2caFlowSpec(const uint32 *src);
    uint8 get_expected_length() const;
    uint8 get_num_elements() const;
    void decode_data(void);
    uint8   fs_identifier() const;     /*!< 2/1 Flow spec identifier */
    uint8   fs_service_type() const;   /*!< 2/1 Best effort, etc. */
    uint16  fs_max_sdu() const;        /*!< 2/2 Maximum SDU size */
    uint32  fs_interarrival() const;   /*!< 4/4 Bandwidth: Time between SDUs in us */
    uint32  fs_latency() const;        /*!< 4/4 Maximum latency between tx opportunities */
    uint32  fs_flush_to() const;       /*!< 4/4 High-resolution flush timeout */
private:
    static const Config_Offsets_T offsets[L2CAFLOWSPEC_NUM_ELEMENTS+1];
};

class DMSyncConfig
{
public:
    DMSyncConfig(const uint8 *src, uint8 dummy);
    DMSyncConfig(const uint32 *src);
    uint16 max_latency() const;
    uint16 retx_effort() const;
    uint16 packet_type() const;
    uint16 voice_settings() const;
    uint32 tx_bdw() const;
    uint32 rx_bdw() const;
    const uint8 *get_data() const;
    const uint8 get_data_size() const { return sizeof(data); };

private:
    uint32 encode_uint32(const uint32 value, const uint8 nbits) const;
    void write_uint(uint32 value, uint8 &offset, uint8 nbytes);
    uint32 read_uint(uint8 offset, const uint8 nbytes) const;
    uint8 data[16];
};

class SMKeyState
{
public:
    SMKeyState(const uint16 *er, const uint16 *ir);

    const uint16 *get_ir(void) const;
    const uint16 *get_er(void) const;
    void set_ir(const uint16 *value);
    void set_er(const uint16 *value);

private:
    uint16 mIR[8];
    uint16 mER[8];
};
    
#endif /* _BLUETOOTHCLASSES_H */

