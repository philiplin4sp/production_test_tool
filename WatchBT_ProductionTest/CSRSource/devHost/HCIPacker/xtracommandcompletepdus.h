#ifndef _XTRACOMMANDCOMPLETEPDUS_H
#define _XTRACOMMANDCOMPLETEPDUS_H

#include "hcicommandcompletepdu.h"

class HCI_READ_LOCAL_NAME_RET_T_PDU : public HCICommandCompletePDU
{
public:
    HCI_READ_LOCAL_NAME_RET_T_PDU();
    HCI_READ_LOCAL_NAME_RET_T_PDU( const PDU& );
    HCI_READ_LOCAL_NAME_RET_T_PDU( uint8 * data , uint32 len );
    const BluetoothName get_name_part(void) const;
    bool size_expected() const;
    virtual bool decompose(uint32 * toFill, BadPDUReason& why_failed, uint32 &length) const;
};

class HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_RET_T_PDU : public HCICommandCompletePDU
{
public:
    HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_RET_T_PDU();
    HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_RET_T_PDU( const PDU& );
    HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_RET_T_PDU( uint8 * data , uint32 len );
    bool get_fec_required(void) const;
    const EIRData get_eir_data_part(void) const;
    bool size_expected() const;
    virtual bool decompose(uint32 * toFill, BadPDUReason& why_failed, uint32 &length) const;
};

class HCI_READ_LOCAL_SUPP_COMMANDS_RET_T_PDU : public HCICommandCompletePDU
{
public:
    HCI_READ_LOCAL_SUPP_COMMANDS_RET_T_PDU();
    HCI_READ_LOCAL_SUPP_COMMANDS_RET_T_PDU( const PDU& );
    HCI_READ_LOCAL_SUPP_COMMANDS_RET_T_PDU( uint8 * data , uint32 len );
    const SupportedCommands get_supp_commands(void) const;
    bool size_expected() const;
    virtual bool decompose(uint32 * toFill, BadPDUReason& why_failed, uint32 &length) const;
};

class HCI_READ_LOCAL_SUPP_FEATURES_RET_T_PDU : public HCICommandCompletePDU
{
public:
    HCI_READ_LOCAL_SUPP_FEATURES_RET_T_PDU();
    HCI_READ_LOCAL_SUPP_FEATURES_RET_T_PDU( const PDU& );
    HCI_READ_LOCAL_SUPP_FEATURES_RET_T_PDU( uint8 * data , uint32 len );
    const LMPSupportedFeatures get_lmp_supp_features(void) const;
    bool size_expected() const;
    virtual bool decompose(uint32 * toFill, BadPDUReason& why_failed, uint32 &length) const;
};

class HCI_READ_CURRENT_IAC_LAP_RET_T_PDU : public HCICommandCompletePDU
{
public:
    HCI_READ_CURRENT_IAC_LAP_RET_T_PDU();
    HCI_READ_CURRENT_IAC_LAP_RET_T_PDU( const PDU& );
    HCI_READ_CURRENT_IAC_LAP_RET_T_PDU( uint8 * data , uint32 len );
    uint8 get_num_current_iac () const;
    uint24 get_iac_lap ( uint8 index ) const;
    bool size_expected() const;
    virtual bool decompose(uint32 * toFill, BadPDUReason& why_failed, uint32 &length) const;
};

// BT1.2

class HCI_READ_AFH_CHANNEL_MAP_RET_T_PDU : public HCICommandCompletePDU
{
public:
    HCI_READ_AFH_CHANNEL_MAP_RET_T_PDU() ;
    HCI_READ_AFH_CHANNEL_MAP_RET_T_PDU( const PDU& ) ;
    HCI_READ_AFH_CHANNEL_MAP_RET_T_PDU( const uint8* data , uint32 length ) ;
    uint16 get_handle() const;
    uint8 get_mode() const;
    AFHmap get_map() const;
    bool size_expected() const;
    virtual bool decompose(uint32 * toFill, BadPDUReason& why_failed, uint32 &length) const;
};

class HCI_READ_LOCAL_EXT_FEATURES_RET_T_PDU : public HCICommandCompletePDU
{
public:
	HCI_READ_LOCAL_EXT_FEATURES_RET_T_PDU ();
	HCI_READ_LOCAL_EXT_FEATURES_RET_T_PDU ( const PDU& );
	HCI_READ_LOCAL_EXT_FEATURES_RET_T_PDU ( const uint8 * data , uint32 len );
	uint8 get_page_num () const;
	uint8 get_max_page_num () const;
	const LMPSupportedFeatures get_lmp_ext_features () const;
    bool size_expected() const;
    virtual bool decompose(uint32 * toFill, BadPDUReason& why_failed, uint32 &length) const;
};

class HCI_READ_LOCAL_OOB_DATA_RET_T_PDU : public HCICommandCompletePDU
{
public:
	HCI_READ_LOCAL_OOB_DATA_RET_T_PDU ();
	HCI_READ_LOCAL_OOB_DATA_RET_T_PDU ( const PDU& );
	HCI_READ_LOCAL_OOB_DATA_RET_T_PDU ( const uint8 * data , uint32 len );
	const SPhashC get_c(void) const;
	const SPrandomizerR get_r(void) const;
    bool size_expected() const;
    virtual bool decompose(uint32 * toFill, BadPDUReason& why_failed, uint32 &length) const;
};

class HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_RET_T_PDU : public HCICommandCompletePDU
{
    public:
        HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_RET_T_PDU ();
        HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_RET_T_PDU ( const PDU& );
        HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_RET_T_PDU ( const uint8 * data , uint32 len );
        virtual bool decompose(uint32 * toFill, BadPDUReason& why_failed, uint32 &length) const;
        ULPSupportedFeatures get_feature_set () const;
        virtual bool size_expected() const;// sanity check the number of bytes
};

class HCI_ULP_RAND_RET_T_PDU : public HCICommandCompletePDU
{
    public:
        HCI_ULP_RAND_RET_T_PDU ();
        HCI_ULP_RAND_RET_T_PDU ( const PDU& );
        HCI_ULP_RAND_RET_T_PDU ( const uint8 * data , uint32 len );
        virtual bool decompose(uint32 * toFill, BadPDUReason& why_failed, uint32 &length) const;
        DATA_uint8_len_32_ret get_random_number () const;
        virtual bool size_expected() const;// sanity check the number of bytes
};

class HCI_ULP_READ_SUPPORTED_STATES_RET_T_PDU : public HCICommandCompletePDU
{
    public:
        HCI_ULP_READ_SUPPORTED_STATES_RET_T_PDU ();
        HCI_ULP_READ_SUPPORTED_STATES_RET_T_PDU ( const PDU& );
        HCI_ULP_READ_SUPPORTED_STATES_RET_T_PDU ( const uint8 * data , uint32 len );
        virtual bool decompose(uint32 * toFill, BadPDUReason& why_failed, uint32 &length) const;
        ULPSupportedStates get_supported_states () const;
        virtual bool size_expected() const;// sanity check the number of bytes
};

class HCI_ULP_ENCRYPT_RET_T_PDU : public HCICommandCompletePDU
{
public:
	HCI_ULP_ENCRYPT_RET_T_PDU ();
	HCI_ULP_ENCRYPT_RET_T_PDU ( const PDU& );
	HCI_ULP_ENCRYPT_RET_T_PDU ( const uint8 * data , uint32 len );
	const DATA_uint8_len_16_ret get_encrypted_data(void) const;
    bool size_expected() const;
    virtual bool decompose(uint32 * toFill, BadPDUReason& why_failed, uint32 &length) const;
};

class HCI_ULP_READ_CHANNEL_MAP_RET_T_PDU : public HCICommandCompletePDU
{
public:
    HCI_ULP_READ_CHANNEL_MAP_RET_T_PDU ();
    HCI_ULP_READ_CHANNEL_MAP_RET_T_PDU ( const PDU& );
    HCI_ULP_READ_CHANNEL_MAP_RET_T_PDU ( const uint8 * data , uint32 len );
    bool size_expected() const;
    virtual bool decompose(uint32 * toFill, BadPDUReason& why_failed, uint32 &length) const;

    uint16 get_connection_handle() const;
    ULPChannelMap get_ulp_channel_map() const;
};

#endif
