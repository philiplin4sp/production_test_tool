////////////////////////////////////////////////////////////////////////////////
//
//  FILE     : display_pdu.h
//
//  AUTHOR   : Adam Hughes
//
//  Copyright CSR 2003.  All rights reserved.
//
//  PURPOSE  : declares a function to display the innards of a PDU
//             in a useful (debug) way.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __HCIPACKER_DISPLAY_PDU_H__
#define __HCIPACKER_DISPLAY_PDU_H__

#include "hcipacker/pdu.h"
#include <string>

class VM_PDU;
class TimeStamp;

void SetVmPduCallback(std::string (*_vmPduCallback) (int indent, const VM_PDU &pdu));
std::string display_pdu ( const PDU& pdu );
std::string display_pdu ( const PDU& pdu, const TimeStamp & );

//  PDUs don't know if they are upstream or downstream.
//  For all but HCI this is unimportant.
//  For HCI there might be an event and a command which are indistinguishable...
//  So we'll cheat a bit.

#include "hcipacker/hcipacker.h"

std::string display_pdu ( const HCIEventPDU & event );
std::string display_pdu ( const HCICommandPDU & event );

//  For use printing histories:
std::string display_pdu_no_time ( const PDU& pdu );

class PDU_displayer;

//  Identical functions, which allow a policy class to be passed in.
std::string display_pdu( const PDU& pdu , PDU_displayer * policy );
std::string display_pdu ( const PDU& pdu, const TimeStamp & , PDU_displayer * policy );
std::string display_pdu ( const HCIEventPDU & event , PDU_displayer * policy );
std::string display_pdu ( const HCICommandPDU & event , PDU_displayer * policy );
std::string display_pdu_no_time ( const PDU& pdu , PDU_displayer * policy );

//  a_string class is used internally to avoid code bloat through
//  excessive in-lining of std::string methods.
class a_string
{
public:
    a_string ( const char * s = 0 );
    a_string& operator+ ( const a_string& );
    a_string& operator+ ( const char * );
    a_string& operator+ ( const std::string& );
    void operator= ( const a_string& );
    void operator= ( const char * );
    void operator= ( const std::string& );
    void operator+= ( const a_string& );
    void operator+= ( const char * );
    void operator+= ( const uint8 * );
    void operator+= ( const std::string& );
    a_string& append ( int count , const char );
    const std::string& get_string() const;
    void erase() { internal.erase(); }
    bool empty() { return internal.empty(); }
private:
    std::string internal;
};

a_string operator+ ( const char * , const a_string& );

//  Use this same code for multiple output formats.
class PDU_displayer
{
public:
    virtual ~PDU_displayer() {}
    virtual a_string prefix () = 0;
    virtual a_string prefix_time ( const TimeStamp& when ) = 0;
    virtual a_string postfix() = 0;
    virtual a_string display_default(int indent , const char * name , const PDU& pdu )
    { return display_uint8Array ( indent , name , pdu , 0 , pdu.size() ); }
    virtual a_string display_unknown_type ( uint16 aType , const char * aChannel , const PDU& pdu ) = 0;
    virtual a_string display_prim_start   ( int indent , const char * name ) = 0;
    virtual a_string display_prim_end     ( int indent , const char * name ) = 0;
    
    virtual a_string display_indent ( int indent ) = 0;
    virtual a_string display_text   ( int indent , const char * name ) = 0;
    virtual a_string display_name   ( int indent , const char * name , const unsigned char * string ) = 0;
    virtual a_string display_data   ( int indent , const char * name , const a_string& string ) = 0;
    virtual a_string display_bool   ( int indent , const char * name , bool   value ) = 0;
    virtual a_string display_uint8  ( int indent , const char * name , uint8  value ) = 0;
    virtual a_string display_int8   ( int indent , const char * name , int8   value ) = 0;
    virtual a_string display_uint16 ( int indent , const char * name , uint16 value ) = 0;
    virtual a_string display_int16  ( int indent , const char * name , int16  value ) = 0;
    virtual a_string display_uint24 ( int indent , const char * name , uint24 value ) = 0;
    virtual a_string display_uint32 ( int indent , const char * name , uint32 value ) = 0;
    virtual a_string display_int32  ( int indent , const char * name , int32 value  ) = 0;

    virtual a_string display_uint8Array ( int indent , const char * name , const uint8* , size_t size ) = 0;
    virtual a_string display_uint8Array ( int indent , const char * name , const PDU& ,
                                     size_t startdex , size_t size ) = 0;
    virtual a_string display_uint16Array ( int indent , const char * name , const uint16* , size_t count ) = 0;
    virtual a_string display_uint24Array ( int indent , const char * name , const uint24* , size_t count ) = 0;
    virtual a_string display_uint32Array ( int indent , const char * name , const uint32* , size_t count ) = 0;

    virtual a_string display_BluetoothDeviceAddress ( int indent , const char * name , const BluetoothDeviceAddress& value ) = 0;
    virtual a_string display_BluetoothDeviceAddressWithType ( int indent , const char * name , const BluetoothDeviceAddressWithType& value ) = 0;
    virtual a_string display_EtherNetAddress ( int indent , const char * name , const EtherNetAddress& value ) = 0;
    virtual a_string display_IPAddress ( int indent , const char * name , const IPAddress& value ) = 0;
    virtual a_string display_HCIEventMask ( int indent , const char * name , const HCIEventMask& value ) = 0;
    virtual a_string display_ULPEventMask ( int indent , const char * name , const ULPEventMask& value ) = 0;
    virtual a_string display_DMHCIEventMask ( int indent , const char * name , const HCIEventMask& value ) = 0;
    virtual a_string display_InquiryResult ( int indent , const char * name , const InquiryResult& ) = 0;
    virtual a_string display_InquiryResultWithRSSI ( int indent , const char * name , const InquiryResultWithRSSI& ) = 0;
    virtual a_string display_CryptoBlob ( int indent , const char * name , uint8 length, const uint16 *array ) = 0;
    virtual a_string display_CryptoBlob ( uint8 length, const uint16 *array );
    struct ncp_element
    {
        uint16 handle;
        uint16 pkts;
    };
    virtual a_string display_ncp_elementArray ( int indent , const char * name , const ncp_element* , size_t count ) = 0;
    struct lb_element
    {
        LinkKey key;
        BluetoothDeviceAddress bd_addr;
    };

    // Base classes that override this function are responsible for checking the link key is valid.
    virtual a_string display_link_key_bd_addrArray ( int indent , const char * name , const lb_element* , size_t count ) = 0;
    virtual a_string display_lp_powerstate ( int indent , const char * name , const LP_POWERSTATE_T& value ) = 0;

	virtual a_string display_ULP_SUB_EVENTS_T ( int indent , const char * name , uint8* value ) = 0;
    virtual a_string display_STREAM_BUFFER_SIZES_T ( int indent , const char * name , STREAM_BUFFER_SIZES_T streams) = 0;
    virtual a_string display_DM_SM_SERVICE_T ( int indent , const char * name , DM_SM_SERVICE_T service) = 0;
    virtual a_string display_rs_table ( int indent , DM_LP_WRITE_ROLESWITCH_POLICY_REQ_T_PDU &prim ) = 0;
    virtual a_string display_uint16_symbolic( int indent , const char * name , uint16 value, const char * symbol) = 0;
    virtual a_string display_l2ca_conn_result_t ( int indent , const char * name , uint16 value);
    virtual a_string display_l2ca_move_result_t ( int indent , const char * name , uint16 value);
    virtual a_string display_l2ca_disc_result_t ( int indent , const char * name , uint16 value);
    virtual a_string display_l2ca_conf_result_t ( int indent , const char * name , uint16 value);
    virtual a_string display_l2ca_info_result_t ( int indent , const char * name , uint16 value);
    virtual a_string display_l2ca_data_result_t ( int indent , const char * name , uint16 value);
    virtual a_string display_l2ca_misc_result_t ( int indent , const char * name , uint16 value);
    virtual a_string display_RFC_CONNECT_CFM_RESPONSE_T ( int indent , const char * name , uint16 value);
    virtual a_string display_RFC_DATAWRITE_CFM_RESPONSE_T ( int indent , const char * name , uint16 value);
    virtual a_string display_RFC_MOVE_L2CAP_CHANNEL_RESPONSE_T ( int indent , const char * name , uint16 value);
    virtual a_string display_RFC_RESPONSE_T ( int indent , const char * name , uint16 value);
    virtual a_string display_SDC_RESPONSE_T ( int indent , const char * name , uint16 value);
    virtual a_string display_SDS_RESPONSE_T ( int indent , const char * name , uint16 value);
    virtual a_string display_DM_SM_TRUST_T (int indent, const char * name, uint16 value);
    virtual a_string display_DM_SM_KEY_TYPE_T (int indent, const char * name, uint16 value);
    virtual a_string display_DM_SM_PERMANENT_ADDRESS_T (int indent, const char * name, uint16 value);
    virtual a_string display_DM_SM_UKEY_T (int indent, const char * name, DM_SM_KEY_TYPE_T type, DM_SM_RFCLI_UKEY_T value) = 0;
    virtual a_string display_DM_SM_UKEY_T_array (int indent, const char * name, uint16 keys_present, const DM_SM_RFCLI_UKEY_T *array) = 0;
    virtual a_string display_SMKeyState( int indent , const char * name , const SMKeyState& value ) = 0;
};

class RfcliPDUDisplayer : public PDU_displayer
{
public:
    a_string prefix ();
    a_string prefix_time ( const TimeStamp& when );
    a_string postfix();
    a_string display_unknown_type ( uint16 aType , const char * aChannel , const PDU& pdu );
    a_string display_prim_start   ( int indent , const char * name );
    a_string display_prim_end     ( int indent , const char * name );
   
    a_string display_indent ( int indent );
    a_string display_text   ( int indent , const char * name );
    a_string display_name   ( int indent , const char * name , const unsigned char * string );
    a_string display_data   ( int indent , const char * name , const a_string& string );
    a_string display_bool   ( int indent , const char * name , bool   value );
    a_string display_uint8  ( int indent , const char * name , uint8  value );
    a_string display_int8   ( int indent , const char * name , int8   value );
    a_string display_uint16 ( int indent , const char * name , uint16 value );
    a_string display_int16  ( int indent , const char * name , int16  value );
    a_string display_uint24 ( int indent , const char * name , uint24 value );
    a_string display_uint32 ( int indent , const char * name , uint32 value );
    a_string display_int32  ( int indent , const char * name , int32 value );

    a_string display_uint8Array ( int indent , const char * name , const uint8* , size_t size );
    a_string display_uint8Array ( int indent , const char * name , const PDU& ,
                                     size_t startdex , size_t size );
    a_string display_uint16Array ( int indent , const char * name , const uint16* , size_t count );
    a_string display_uint24Array ( int indent , const char * name , const uint24* , size_t count );
    a_string display_uint32Array ( int indent , const char * name , const uint32* , size_t count );

    a_string display_BluetoothDeviceAddress ( int indent , const char * name , const BluetoothDeviceAddress& value );
    a_string display_BluetoothDeviceAddressWithType ( int indent , const char * name , const BluetoothDeviceAddressWithType& value );
    a_string display_EtherNetAddress ( int indent , const char * name , const EtherNetAddress& value );
    a_string display_IPAddress ( int indent , const char * name , const IPAddress& value );
    a_string display_HCIEventMask ( int indent , const char * name , const HCIEventMask& value );
    a_string display_ULPEventMask ( int indent , const char * name , const ULPEventMask& value );
    a_string display_DMHCIEventMask ( int indent , const char * name , const HCIEventMask& value );
    a_string display_InquiryResult ( int indent , const char * name , const InquiryResult& );
    a_string display_InquiryResultWithRSSI ( int indent , const char * name , const InquiryResultWithRSSI& );
    a_string display_ncp_elementArray ( int indent , const char * name , const PDU_displayer::ncp_element* , size_t count );
    a_string display_link_key_bd_addrArray ( int indent , const char * name , const PDU_displayer::lb_element* , size_t count );
    a_string display_lp_powerstate ( int indent , const char * name , const LP_POWERSTATE_T& value );
    a_string display_ULP_SUB_EVENTS_T ( int indent , const char * name , uint8* value );
    a_string display_STREAM_BUFFER_SIZES_T ( int indent , const char * name , STREAM_BUFFER_SIZES_T streams);
    a_string display_DM_SM_SERVICE_T ( int indent , const char * name , DM_SM_SERVICE_T service);
    a_string display_rs_table ( int indent , DM_LP_WRITE_ROLESWITCH_POLICY_REQ_T_PDU &prim );
    a_string display_uint16_symbolic( int indent , const char * name , uint16 value, const char * symbol);
    a_string display_CryptoBlob ( int indent , const char * name , uint8 length, const uint16 *array );
    a_string display_DM_SM_UKEY_T (int indent, const char * name, DM_SM_KEY_TYPE_T type, DM_SM_RFCLI_UKEY_T value);
    a_string display_DM_SM_UKEY_T_array (int indent, const char * name, uint16 keys_present, const DM_SM_RFCLI_UKEY_T *array);
    a_string display_SMKeyState( int indent , const char * name , const SMKeyState& value );
};

class TCLArrayDisplayer : public PDU_displayer
{
public:
    a_string prefix ();
    a_string prefix_time ( const TimeStamp& when );
    a_string postfix();
    a_string display_unknown_type ( uint16 aType , const char * aChannel , const PDU& pdu );
    a_string display_prim_start   ( int indent , const char * name );
    a_string display_prim_end     ( int indent , const char * name );
   
    a_string display_indent ( int indent );
    a_string display_text   ( int indent , const char * name );
    a_string display_name   ( int indent , const char * name , const unsigned char * string );
    a_string display_data   ( int indent , const char * name , const a_string& string );
    a_string display_bool   ( int indent , const char * name , bool   value );
    a_string display_uint8  ( int indent , const char * name , uint8  value );
    a_string display_int8   ( int indent , const char * name , int8   value );
    a_string display_uint16 ( int indent , const char * name , uint16 value );
    a_string display_int16  ( int indent , const char * name , int16  value );
    a_string display_uint24 ( int indent , const char * name , uint24 value );
    a_string display_uint32 ( int indent , const char * name , uint32 value );
    a_string display_int32  ( int indent , const char * name , int32 value );

    a_string display_uint8Array ( int indent , const char * name , const uint8* , size_t size );
    a_string display_uint8Array ( int indent , const char * name , const PDU& ,
                                     size_t startdex , size_t size );
    a_string display_uint16Array ( int indent , const char * name , const uint16* , size_t count );
    a_string display_uint24Array ( int indent , const char * name , const uint24* , size_t count );
    a_string display_uint32Array ( int indent , const char * name , const uint32* , size_t count );

    a_string display_BluetoothDeviceAddress ( int indent , const char * name , const BluetoothDeviceAddress& value );
    a_string display_BluetoothDeviceAddressWithType ( int indent , const char * name , const BluetoothDeviceAddressWithType& value );
    a_string display_EtherNetAddress ( int indent , const char * name , const EtherNetAddress& value );
    a_string display_IPAddress ( int indent , const char * name , const IPAddress& value );
    a_string display_HCIEventMask ( int indent , const char * name , const HCIEventMask& value );
    a_string display_ULPEventMask ( int indent , const char * name , const ULPEventMask& value );
    a_string display_DMHCIEventMask ( int indent , const char * name , const HCIEventMask& value );
    a_string display_InquiryResult ( int indent , const char * name , const InquiryResult& );
    a_string display_InquiryResultWithRSSI ( int indent , const char * name , const InquiryResultWithRSSI& );
    a_string display_ncp_elementArray ( int indent , const char * name , const PDU_displayer::ncp_element* , size_t count );
    a_string display_link_key_bd_addrArray ( int indent , const char * name , const PDU_displayer::lb_element* , size_t count );
    a_string display_lp_powerstate ( int indent , const char * name , const LP_POWERSTATE_T& value );

    a_string display_ULP_SUB_EVENTS_T ( int indent , const char * name , uint8* value );
    a_string display_STREAM_BUFFER_SIZES_T ( int indent , const char * name , STREAM_BUFFER_SIZES_T streams);
    a_string display_DM_SM_SERVICE_T ( int indent , const char * name , DM_SM_SERVICE_T service);
    a_string display_rs_table ( int indent , DM_LP_WRITE_ROLESWITCH_POLICY_REQ_T_PDU &prim );
    a_string display_uint16_symbolic( int indent , const char * name , uint16 value, const char * symbol);
    a_string display_CryptoBlob ( int indent , const char * name , uint8 length, const uint16 *array );
    a_string display_DM_SM_UKEY_T (int indent, const char * name, DM_SM_KEY_TYPE_T type, DM_SM_RFCLI_UKEY_T value);
    a_string display_DM_SM_UKEY_T_array (int indent, const char * name, uint16 keys_present, const DM_SM_RFCLI_UKEY_T *array);
    a_string display_SMKeyState( int indent , const char * name , const SMKeyState& value );
};

#endif//__HCIPACKER_DISPLAY_PDU_H__
