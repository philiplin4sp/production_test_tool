////////////////////////////////////////////////////////////////////////////////
//
//  WARNING THIS IS AN AUTOGENERATED FILE from hci_dm_xtra.tpl
//
////////////////////////////////////////////////////////////////////////////////

#include "bcflayout.h"
#include "hcipacker/gen_xtradm.h"


#define OFFSET_START(name) 

enum {
    OFFSET_START(DM_HCI_SET_EVENT_MASK_REQ_T_PDU)
    OFFSET_uint16(DM_HCI_SET_EVENT_MASK_REQ_T_op_code),
    OFFSET_uint8(DM_HCI_SET_EVENT_MASK_REQ_T_length),
    OFFSET_EventMask(DM_HCI_SET_EVENT_MASK_REQ_T_event_mask),
    DM_HCI_SET_EVENT_MASK_REQ_T_pduSize
} ;

DM_HCI_SET_EVENT_MASK_REQ_T_PDU::DM_HCI_SET_EVENT_MASK_REQ_T_PDU()
: DM_PDU(DM_HCI_SET_EVENT_MASK_REQ,DM_HCI_SET_EVENT_MASK_REQ_T_pduSize)
{
}

DM_HCI_SET_EVENT_MASK_REQ_T_PDU::DM_HCI_SET_EVENT_MASK_REQ_T_PDU( const PDU& from )
: DM_PDU(from)
{
}

DM_HCI_SET_EVENT_MASK_REQ_T_PDU::DM_HCI_SET_EVENT_MASK_REQ_T_PDU( const uint8 * data , uint32 size )
: DM_PDU(data,size)
{
}

const HCIEventMask DM_HCI_SET_EVENT_MASK_REQ_T_PDU::get_event_mask() const
{
    uint8 mask[8];
    // byte spaced on both HCI and DM
    PDU::get_uint8Array ( mask , DM_HCI_SET_EVENT_MASK_REQ_T_event_mask , 8 );
    return HCIEventMask ( mask );
}

void DM_HCI_SET_EVENT_MASK_REQ_T_PDU::set_event_mask ( const HCIEventMask& mask )
{
    // byte spaced on both HCI and DM
    PDU::set_uint8Array ( mask.get_data() , DM_HCI_SET_EVENT_MASK_REQ_T_event_mask , 8 ) ;
}


enum {
    OFFSET_START(DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_T_PDU)
    OFFSET_uint16(DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_T_op_code),
    OFFSET_uint8(DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_T_length),
    OFFSET_uint8(DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_T_num_current_iac),
    OFFSET_PtrArray(DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_T_iac_lap,HCI_IAC_LAP_PTRS),
    DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_T_pduSize,
    HCI_WRITE_CURRENT_IAC_LAP_REQ_T_pduSize = DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_T_iac_lap
} ;

DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_T_PDU::DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_T_PDU()
: DM_PDU(DM_HCI_WRITE_CURRENT_IAC_LAP_REQ,DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_T_pduSize)
{
}

DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_T_PDU::DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_T_PDU( const PDU& from )
: DM_PDU(from)
{
}

DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_T_PDU::DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_T_PDU( const uint8 * data , uint32 size )
: DM_PDU(data,size)
{
}

uint8 DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_T_PDU::get_num_current_iac(void) const
{
    return get_uint8(DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_T_num_current_iac) ;
}

void DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_T_PDU::set_num_current_iac(uint8 val)
{
    set_uint8(DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_T_num_current_iac,val) ;
}

uint24 DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_T_PDU::get_iac_lap(uint8 index) const
{
    return get_uint24(DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_T_pduSize + index*4) ;
}

void DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_T_PDU::set_iac_lap(uint8 index,uint24 val)
{
    uint16 required_length = DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_T_pduSize + (index+1) *4;
    if (size() < required_length) 
    {
        resize (required_length) ;
    }
    set_uint24(required_length-4,val) ;
    if (index >= get_num_current_iac()) set_num_current_iac(index+1) ; 
}



enum {
    OFFSET_START(DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ_T_PDU)
    OFFSET_uint16(DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ_T_op_code),
    OFFSET_uint8(DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ_T_length),
    OFFSET_uint8(DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ_T_num_handles),
    DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ_T_pduSize
} ;




DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ_T_PDU::DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ_T_PDU()
: DM_PDU(DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ,DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ_T_pduSize)
{
}

DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ_T_PDU::DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ_T_PDU( const PDU& from )
: DM_PDU(from)
{
}

DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ_T_PDU::DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ_T_PDU( const uint8 * data , uint32 size )
: DM_PDU(data,size)
{
}

uint8 DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ_T_PDU::get_num_handles(void) const
{
    return get_uint8(DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ_T_num_handles) ;
}

void DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ_T_PDU::set_num_handles(uint8 val)
{
    resize(DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ_T_pduSize + val * 4);
    set_uint8(DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ_T_num_handles,val) ;
}

void DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ_T_PDU::get_num_completed_pkts(uint8 index,uint16 &handle,uint16 &pkts ) const
{
    uint16 offset = DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ_T_pduSize;
    offset += index *(2+2) ;
    handle = get_uint16(offset) ; offset +=2 ;
    pkts = get_uint16(offset) ;         
}

void DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ_T_PDU::set_num_completed_pkts(uint8 index,uint16 handle,uint16 pkts )
{
    if (index>= get_num_handles()) 
    {
        //TODO - throw an exception 
    }

    uint16 offset = DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ_T_pduSize;
    offset += index *(2+2) ;
    set_uint16(offset,handle) ; offset +=2 ;
    set_uint16(offset,pkts) ;           
}

enum {
    OFFSET_START(DM_HCI_WRITE_STORED_LINK_KEY_T)
    OFFSET_uint16(DM_HCI_WRITE_STORED_LINK_KEY_REQ_T_op_code),
    OFFSET_uint8(DM_HCI_WRITE_STORED_LINK_KEY_REQ_T_length),
    OFFSET_uint8(DM_HCI_WRITE_STORED_LINK_KEY_REQ_T_number_keys),
    OFFSET_PtrArray(DM_HCI_WRITE_STORED_LINK_KEY_REQ_T_link_key_bd_addr,HCI_STORED_LINK_KEY_MAX),
    DM_HCI_WRITE_STORED_LINK_KEY_REQ_T_pduSize,
    HCI_WRITE_STORED_LINK_KEY_REQ_T_pduSize = DM_HCI_WRITE_STORED_LINK_KEY_REQ_T_link_key_bd_addr
} ;

DM_HCI_WRITE_STORED_LINK_KEY_REQ_T_PDU::DM_HCI_WRITE_STORED_LINK_KEY_REQ_T_PDU()
: DM_PDU(DM_HCI_WRITE_STORED_LINK_KEY_REQ,DM_HCI_WRITE_STORED_LINK_KEY_REQ_T_pduSize)
{
}

DM_HCI_WRITE_STORED_LINK_KEY_REQ_T_PDU::DM_HCI_WRITE_STORED_LINK_KEY_REQ_T_PDU( const PDU& from )
: DM_PDU(from)
{
}

DM_HCI_WRITE_STORED_LINK_KEY_REQ_T_PDU::DM_HCI_WRITE_STORED_LINK_KEY_REQ_T_PDU( const uint8 * data , uint32 size )
: DM_PDU(data,size)
{
}

uint8 DM_HCI_WRITE_STORED_LINK_KEY_REQ_T_PDU::get_number_keys(void) const
{
    return get_uint8(DM_HCI_WRITE_STORED_LINK_KEY_REQ_T_number_keys) ;
}

void DM_HCI_WRITE_STORED_LINK_KEY_REQ_T_PDU::set_number_keys(uint8 val)
{
    //  DM_HCI bdaddr size = 8, link key size = 32
    resize ( DM_HCI_WRITE_STORED_LINK_KEY_REQ_T_pduSize + val * (8+32) );
    set_uint8(DM_HCI_WRITE_STORED_LINK_KEY_REQ_T_number_keys,val) ;
}

void DM_HCI_WRITE_STORED_LINK_KEY_REQ_T_PDU::get_link_key_bd_addr ( uint8 index , BluetoothDeviceAddress &addr , LinkKey &key ) const
{
    if (DM_HCI_WRITE_STORED_LINK_KEY_REQ_T_pduSize + index * (8 + 32) + 8 + 32 > size())
    {
        return;
    }
    uint16 offset = DM_HCI_WRITE_STORED_LINK_KEY_REQ_T_pduSize ;
    offset += index * (8+32) ; //8 = bdaddr size, 32 = link key size
    addr = get_BluetoothDeviceAddress(offset) ;
    offset+=8 ;
    uint8 lKey[16];
    get_uint8Array( lKey , offset , 16 );
    LinkKey k(lKey) ;
    key = k ;
}

void DM_HCI_WRITE_STORED_LINK_KEY_REQ_T_PDU::set_link_key_bd_addr ( uint8 index , const BluetoothDeviceAddress& addr , const LinkKey& key )
{
    if (DM_HCI_WRITE_STORED_LINK_KEY_REQ_T_pduSize + index * (8 + 32) + 8 + 32 > size())
    {
        return;
    }
    uint16 offset = DM_HCI_WRITE_STORED_LINK_KEY_REQ_T_pduSize ;
    offset += index * (8+32) ; //8 = bdaddr size, 32 = link key size
    set_BluetoothDeviceAddress(offset,addr) ;
    offset+=8 ;
    // byte spaced on HCI, word spaced on DM
    set_uint8Array(key.get_data(),offset,16) ;
}

/*******************************************************************/

enum {
    OFFSET_START(DM_HCI_SET_EVENT_FILTER_T)
    OFFSET_uint16(DM_HCI_SET_EVENT_FILTER_REQ_T_op_code),
    OFFSET_uint8(DM_HCI_SET_EVENT_FILTER_REQ_T_length),
    OFFSET_uint8(DM_HCI_SET_EVENT_FILTER_REQ_T_filter_type),
    OFFSET_uint8(DM_HCI_SET_EVENT_FILTER_REQ_T_filter_condition_type),
    OFFSET_uint24(DM_HCI_SET_EVENT_FILTER_REQ_T_COD),
    OFFSET_uint24(DM_HCI_SET_EVENT_FILTER_REQ_T_COD_mask),
    OFFSET_uint8(DM_HCI_SET_EVENT_FILTER_REQ_T_AAF_COD),
    DM_HCI_SET_EVENT_FILTER_REQ_T_pduSize,
    HCI_SET_EVENT_FILTER_REQ_T_pduSize = DM_HCI_SET_EVENT_FILTER_REQ_T_filter_condition_type,
    DM_HCI_SET_EVENT_FILTER_REQ_T_BDA = DM_HCI_SET_EVENT_FILTER_REQ_T_COD,
    DM_HCI_SET_EVENT_FILTER_REQ_T_AAF_all = DM_HCI_SET_EVENT_FILTER_REQ_T_COD,
    DM_HCI_SET_EVENT_FILTER_REQ_T_AAF_BDA = DM_HCI_SET_EVENT_FILTER_REQ_T_AAF_COD
} ;

DM_HCI_SET_EVENT_FILTER_REQ_T_PDU::DM_HCI_SET_EVENT_FILTER_REQ_T_PDU()
:   DM_PDU ( DM_HCI_SET_EVENT_FILTER_REQ , DM_HCI_SET_EVENT_FILTER_REQ_T_pduSize )
{
}

DM_HCI_SET_EVENT_FILTER_REQ_T_PDU::DM_HCI_SET_EVENT_FILTER_REQ_T_PDU( const PDU& from )
:   DM_PDU ( from )
{
}

DM_HCI_SET_EVENT_FILTER_REQ_T_PDU::DM_HCI_SET_EVENT_FILTER_REQ_T_PDU( const uint8* buffer , uint32 len )
:   DM_PDU ( buffer , len )
{
}

uint8 DM_HCI_SET_EVENT_FILTER_REQ_T_PDU::get_filter_type() const
{
    return get_uint8 ( DM_HCI_SET_EVENT_FILTER_REQ_T_filter_type );
}

uint8 DM_HCI_SET_EVENT_FILTER_REQ_T_PDU::get_filter_condition_type() const
{
    if ( size() > DM_HCI_SET_EVENT_FILTER_REQ_T_filter_condition_type )
        return get_uint8 ( DM_HCI_SET_EVENT_FILTER_REQ_T_filter_condition_type );
    else
        return 0xFF;
}

void DM_HCI_SET_EVENT_FILTER_REQ_T_PDU::set_filter_type ( uint8 type )
{
    if ( type == 0 )
         ( DM_HCI_SET_EVENT_FILTER_REQ_T_filter_condition_type );
    set_uint8 ( DM_HCI_SET_EVENT_FILTER_REQ_T_filter_type , type );
}

////////////////////////////////
//  FT  FCT   len
////////////////////////////////
//  0    -     4  (3+1)
//  1    0     5  (3+1+1)
//  1    1     11 (3+1+1+3+3)
//  1    2     11 (3+1+1+6)
//  2    0     6  (3+1+1+1)
//  2    1     12 (3+1+1+1+3+3)
//  2    2     12 (3+1+1+1+6)
////////////////////////////////

void DM_HCI_SET_EVENT_FILTER_REQ_T_PDU::set_filter_condition_type ( uint8 type )
{
    uint8 ft = get_filter_type();
    uint16 size = 4; // basic, 3 header + 1 FT
    switch ( ft )
    {
    case 0:
        return;
    case 1: //  must have FCT.
        size++;
        break;
    case 2: //  must have FCT and AAF
        size += 2;
        break;
    default:
        return;
    }
    switch ( type )
    {
    case 0: //  nothing
        break;
    case 1: //  COD and COD mask
        size += 6;
        break;
    case 2: //  BDA
        size += 6;
        break;
    default:
        return;
    }
     ( size );
    set_uint8 ( DM_HCI_SET_EVENT_FILTER_REQ_T_filter_condition_type , type );
}

bool DM_HCI_SET_EVENT_FILTER_REQ_T_PDU::get_condition ( BluetoothDeviceAddress& bda ) const
{
    if ( get_filter_type() && ( get_filter_condition_type() == 2 ) )
    {
        bda = get_BluetoothDeviceAddress( DM_HCI_SET_EVENT_FILTER_REQ_T_BDA );
        return true;
    }
    return false;
}

bool DM_HCI_SET_EVENT_FILTER_REQ_T_PDU::get_condition ( ClassOfDevice& val, ClassOfDevice& mask ) const
{
    if ( get_filter_type() && ( get_filter_condition_type() == 1 ) )
    {
        val  = get_ClassOfDevice ( DM_HCI_SET_EVENT_FILTER_REQ_T_COD );
        mask = get_ClassOfDevice ( DM_HCI_SET_EVENT_FILTER_REQ_T_COD_mask );
        return true;
    }
    return false;
}

bool DM_HCI_SET_EVENT_FILTER_REQ_T_PDU::get_condition ( uint8& auto_accept_flag ) const
{
    bool ok = (get_filter_type() == 2);
    if ( ok )
    {
        switch ( get_filter_condition_type() )
        {
        case 0:
            auto_accept_flag = get_uint8(DM_HCI_SET_EVENT_FILTER_REQ_T_AAF_all );
            break;
        case 1:
            auto_accept_flag = get_uint8(DM_HCI_SET_EVENT_FILTER_REQ_T_AAF_COD );
            break;
        case 2:
            auto_accept_flag = get_uint8(DM_HCI_SET_EVENT_FILTER_REQ_T_AAF_BDA );
            break;
        default:
            ok = false;
            break;
        }
    }
    return ok;
}

bool DM_HCI_SET_EVENT_FILTER_REQ_T_PDU::set_condition ( const BluetoothDeviceAddress& bda )
{
    if ( get_filter_type() && ( get_filter_condition_type() == 2 ) )
    {
        set_BluetoothDeviceAddress( DM_HCI_SET_EVENT_FILTER_REQ_T_BDA , bda );
        return true;
    }
    return false;
}

bool DM_HCI_SET_EVENT_FILTER_REQ_T_PDU::set_condition ( const ClassOfDevice& val, const ClassOfDevice& mask )
{
    uint8 ft = get_filter_type();
    if ( ft && ( get_filter_condition_type() == 1 ) )
    {
        set_ClassOfDevice ( DM_HCI_SET_EVENT_FILTER_REQ_T_COD , val );
        set_ClassOfDevice ( DM_HCI_SET_EVENT_FILTER_REQ_T_COD_mask , mask );
        return true;
    }
    return false;
}

bool DM_HCI_SET_EVENT_FILTER_REQ_T_PDU::set_condition ( uint8 auto_accept_flag )
{
    bool ok = (get_filter_type() == 2);
    if ( ok )
    {
        switch ( get_filter_condition_type() )
        {
        case 0:
            set_uint8 ( DM_HCI_SET_EVENT_FILTER_REQ_T_AAF_all , auto_accept_flag );
            break;
        case 1:
            set_uint8 ( DM_HCI_SET_EVENT_FILTER_REQ_T_AAF_COD , auto_accept_flag );
            break;
        case 2:
            set_uint8 ( DM_HCI_SET_EVENT_FILTER_REQ_T_AAF_BDA , auto_accept_flag );
            break;
        default:
            ok = false;
            break;
        }
    }
    return ok;
}

