////////////////////////////////////////////////////////////////////////////////
//
//  WARNING THIS IS AN AUTOGENERATED FILE from hci_dm_xtra_hdr.tpl
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GEN_XTRAHCICommandPDUS_H
#define _GEN_XTRAHCICommandPDUS_H

#include "hcipacker/hcicommandpdu.h"

// handwritten pdus common to both HCI and DM...

class HCI_SET_EVENT_MASK_T_PDU : public HCICommandPDU
{
public:
    HCI_SET_EVENT_MASK_T_PDU() ;
    HCI_SET_EVENT_MASK_T_PDU( const PDU& ) ;
    HCI_SET_EVENT_MASK_T_PDU( const uint8* data , uint32 len ) ;
    bool build ( const uint32 * const parameters );
    const HCIEventMask get_event_mask() const;
    void set_event_mask ( const HCIEventMask& data) ;
};

class HCI_SET_EVENT_FILTER_T_PDU : public HCICommandPDU
{
public:
    HCI_SET_EVENT_FILTER_T_PDU();
    HCI_SET_EVENT_FILTER_T_PDU( const PDU& );
    HCI_SET_EVENT_FILTER_T_PDU( const uint8* data , uint32 len );
    bool build ( const uint32 * const parameters );
    uint8 get_filter_type() const;
    void set_filter_type ( uint8 data );
    uint8 get_filter_condition_type () const;
    void set_filter_condition_type ( uint8 data );
    bool get_condition ( BluetoothDeviceAddress& ) const;
    bool get_condition ( ClassOfDevice& val, ClassOfDevice& mask ) const;
    bool get_condition ( uint8& auto_accept_flag ) const;
    bool set_condition ( const BluetoothDeviceAddress& );
    bool set_condition ( const ClassOfDevice& val, const ClassOfDevice& mask );
    bool set_condition ( uint8 auto_accept_flag );
};

class HCI_WRITE_CURRENT_IAC_LAP_T_PDU : public HCICommandPDU
{
public:
    HCI_WRITE_CURRENT_IAC_LAP_T_PDU() ;
    HCI_WRITE_CURRENT_IAC_LAP_T_PDU( const PDU& ) ;
    HCI_WRITE_CURRENT_IAC_LAP_T_PDU( const uint8* data , uint32 len ) ;
    bool build ( const uint32 * const parameters );
    uint8 get_num_current_iac(void) const;
    void set_num_current_iac(uint8 data) ;
    uint24 get_iac_lap(uint8 index) const;
    void set_iac_lap(uint8 index,uint24 data) ;
};


class HCI_HOST_NUM_COMPLETED_PACKETS_T_PDU : public HCICommandPDU
{
public:
    HCI_HOST_NUM_COMPLETED_PACKETS_T_PDU() ;
    HCI_HOST_NUM_COMPLETED_PACKETS_T_PDU( const PDU& ) ;
    HCI_HOST_NUM_COMPLETED_PACKETS_T_PDU( const uint8* data , uint32 len ) ;
    bool build ( const uint32 * const parameters );
    uint8 get_num_handles(void) const;
    void set_num_handles(uint8 data) ;
    void get_num_completed_pkts(uint8 index,uint16 &handle,uint16 &pkts) const;
    void set_num_completed_pkts(uint8 index,uint16 handle,uint16 pkts) ;
};

class HCI_WRITE_STORED_LINK_KEY_T_PDU : public HCICommandPDU
{
public:
    HCI_WRITE_STORED_LINK_KEY_T_PDU() ;
    HCI_WRITE_STORED_LINK_KEY_T_PDU( const PDU& ) ;
    HCI_WRITE_STORED_LINK_KEY_T_PDU( const uint8* data , uint32 len ) ;
    bool build ( const uint32 * const parameters );
    uint8 get_number_keys() const;
    void set_number_keys ( uint8 data );
    void get_link_key_bd_addr ( uint8 index , BluetoothDeviceAddress& addr , LinkKey& key ) const;
    void set_link_key_bd_addr ( uint8 index , const BluetoothDeviceAddress& addr , const LinkKey& key );
};

class HCI_CHANGE_LOCAL_NAME_T_PDU : public HCICommandPDU
{
public:
    HCI_CHANGE_LOCAL_NAME_T_PDU() ;
    HCI_CHANGE_LOCAL_NAME_T_PDU( const PDU& ) ;
    HCI_CHANGE_LOCAL_NAME_T_PDU( const uint8* data , uint32 len ) ;
    bool build ( const uint32 * const parameters );
    BluetoothName get_name_part() const;
    void set_name_part(BluetoothName) ;
};

#endif//_GEN_XTRAHCICommandPDUS_H
