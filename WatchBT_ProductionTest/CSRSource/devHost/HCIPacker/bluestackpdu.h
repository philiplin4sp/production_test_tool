///////////////////////////////////////////////////////////////////////////////
//
//  FILE:       bluestackpdu.h
//
//  CLASSES:    RFCOMM_PDU, DM_PDU, L2CAP_PDU, SDP_PDU, BlueStackPDU
//
//  PURPOSE:    Declare BlueStack specific pdus.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __BLUESTACKPDU_H__
#define __BLUESTACKPDU_H__

#include "hcipacker/bluecorepdu.h"
typedef uint16 phandle_t;
#include "bluetoothclasses/bluetoothclasses.h"
#include "common/types_t.h"
#include "app/bluestack/bluetooth.h"
#include "app/bluestack/l2cap_prim.h"
#include "app/bluestack/hci.h"
#include "app/bluestack/dm_prim.h"

class BlueStackPDU : public PCPDU // in bluecore friendly format
{
public:
    BlueStackPDU ( PDU::bc_channel channel , uint32 size );
    BlueStackPDU ( const PDU& );
    BlueStackPDU ( PDU::bc_channel channel , const uint8* buffer , uint32 len );
    const BluetoothDeviceAddress get_BluetoothDeviceAddress ( uint32 offset ) const;
    void set_BluetoothDeviceAddress ( uint32 offset , const BluetoothDeviceAddress& value );
    const BluetoothDeviceAddressWithType get_BluetoothDeviceAddressWithType ( uint32 offset ) const;
    void set_BluetoothDeviceAddressWithType( uint32 offset , const BluetoothDeviceAddressWithType& value );
    const ClassOfDevice get_ClassOfDevice ( uint32 offset ) const;
    void set_ClassOfDevice ( uint32 offset , const ClassOfDevice& data );
    uint16 get_type () const;
    void set_type ( uint16 type );
protected:
    void set_final_uint8Ptr ( uint32 size_index ,
                              uint32 pdu_size ,
                              const uint8* copyFrom , uint32 size );
    void set_penult_uint8Ptr ( uint32 size_index , uint32 size_index2 ,
                               uint32 pdu_size ,
                               const uint8* copyFrom , uint32 size );
};

class RFCOMM_PDU : public BlueStackPDU
{
public:
    RFCOMM_PDU ( uint16 type , uint32 size );
    RFCOMM_PDU ( const PDU& );
    RFCOMM_PDU ( const uint8* buffer , uint32 len );
};

class L2CAP_PDU : public BlueStackPDU
{
public:
    L2CAP_PDU ( uint16 type , uint32 size );
    L2CAP_PDU ( const PDU& );
    L2CAP_PDU ( const uint8* buffer , uint32 len );

    STREAM_BUFFER_SIZES_T get_STREAM_BUFFER_SIZES_T( uint32 offset ) const;
    void set_STREAM_BUFFER_SIZES_T( uint32 offset, STREAM_BUFFER_SIZES_T value);
    DM_SM_SERVICE_T get_DM_SM_SERVICE_T( uint32 offset ) const;
    void set_DM_SM_SERVICE_T( uint32 offset, DM_SM_SERVICE_T value);
    l2ca_conn_result_t get_l2ca_conn_result_t ( uint32 offset ) const { return (l2ca_conn_result_t)get_uint16(offset); }
    l2ca_data_result_t get_l2ca_data_result_t ( uint32 offset ) const { return (l2ca_data_result_t)get_uint16(offset); }
    l2ca_disc_result_t get_l2ca_disc_result_t ( uint32 offset ) const { return (l2ca_disc_result_t)get_uint16(offset); }
    l2ca_misc_result_t get_l2ca_misc_result_t ( uint32 offset ) const { return (l2ca_misc_result_t)get_uint16(offset); }
    l2ca_move_result_t get_l2ca_move_result_t ( uint32 offset ) const { return (l2ca_move_result_t)get_uint16(offset); }
    void set_l2ca_conn_result_t ( uint32 offset, l2ca_conn_result_t value ) { set_uint16(offset, (l2ca_conn_result_t)value); }
    void set_l2ca_data_result_t ( uint32 offset, l2ca_data_result_t value ) { set_uint16(offset, (l2ca_data_result_t)value); }
    void set_l2ca_disc_result_t ( uint32 offset, l2ca_disc_result_t value ) { set_uint16(offset, (l2ca_disc_result_t)value); }
    void set_l2ca_misc_result_t ( uint32 offset, l2ca_misc_result_t value ) { set_uint16(offset, (l2ca_misc_result_t)value); }
    void set_l2ca_move_result_t ( uint32 offset, l2ca_move_result_t value ) { set_uint16(offset, (l2ca_misc_result_t)value); }
};

class DM_PDU : public BlueStackPDU
{
public:
    DM_PDU ( uint16 type , uint32 size );
    DM_PDU ( const PDU& );
    DM_PDU ( const uint8* buffer , uint32 len );

    DM_SM_TRUST_T get_DM_SM_TRUST_T ( uint32 offset ) const { return (DM_SM_TRUST_T)get_uint16(offset); }
    DM_SM_KEY_TYPE_T get_DM_SM_KEY_TYPE_T ( uint32 offset ) const { return (DM_SM_KEY_TYPE_T)get_uint16(offset); }
    DM_SM_PERMANENT_ADDRESS_T get_DM_SM_PERMANENT_ADDRESS_T ( uint32 offset ) const { return (DM_SM_PERMANENT_ADDRESS_T)get_uint16(offset); }
    void set_DM_SM_TRUST_T ( uint32 offset, DM_SM_TRUST_T value ) { set_uint16(offset, (uint16)value); }
    void set_DM_SM_KEY_TYPE_T ( uint32 offset,  DM_SM_KEY_TYPE_T value ) { set_uint16(offset, (uint16)value); }
    void set_DM_SM_PERMANENT_ADDRESS_T ( uint32 offset,  DM_SM_PERMANENT_ADDRESS_T  value ) { set_uint16(offset, (uint16)value); }
    void get_DM_SM_UKEY_T ( uint32 offset, uint32 u_offset, uint16 keys, DM_SM_RFCLI_UKEY_T *u ) const ;
    void set_DM_SM_UKEY_T ( uint32 &offset, uint32 u_offset, uint16 keys, DM_SM_RFCLI_UKEY_T *u );
    SMKeyState get_SMKeyState( uint32 offset ) const;
    void set_SMKeyState( uint32 offset, const SMKeyState &value );
};

class SDP_PDU : public BlueStackPDU
{
public:
    SDP_PDU ( uint16 type , uint32 size );
    SDP_PDU ( const PDU& );
    SDP_PDU ( const uint8* buffer , uint32 len );
};

class TCP_PDU : public BlueStackPDU
{
public:
    TCP_PDU ( uint16 type , uint32 size );
    TCP_PDU ( const PDU& );
    TCP_PDU ( const uint8* buffer , uint32 len );
    IPAddress get_IPAddress( uint32 offset ) const;
    void set_IPAddress ( uint32 offset , const IPAddress& );
};

typedef TCP_PDU UDP_PDU;

class BNEP_PDU : public BlueStackPDU
{
public:
    BNEP_PDU ( uint16 type , uint32 size );
    BNEP_PDU ( const PDU& );
    BNEP_PDU ( const uint8* buffer , uint32 len );
    EtherNetAddress get_EtherNetAddress( uint32 offset ) const;
    void set_EtherNetAddress ( uint32 offset , const EtherNetAddress& );
};

class ATT_PDU : public BlueStackPDU
{
public:
    ATT_PDU ( uint16 type , uint32 size );
    ATT_PDU ( const PDU& );
    ATT_PDU ( const uint8* buffer , uint32 len );
    l2ca_conn_result_t get_l2ca_conn_result_t ( uint32 offset ) const { return (l2ca_conn_result_t)get_uint16(offset); }
    l2ca_disc_result_t get_l2ca_disc_result_t ( uint32 offset ) const { return (l2ca_disc_result_t)get_uint16(offset); }
    void set_l2ca_conn_result_t ( uint32 offset, l2ca_conn_result_t value ) { set_uint16(offset, (l2ca_conn_result_t)value); }
    void set_l2ca_disc_result_t ( uint32 offset, l2ca_disc_result_t value ) { set_uint16(offset, (l2ca_disc_result_t)value); }
};

#include "hcipacker/gen_dm.h"
#include "hcipacker/gen_sdp.h"
#include "hcipacker/gen_l2cap.h"
#include "hcipacker/gen_rfcomm.h"
#include "hcipacker/gen_att.h"

#endif /*__BLUESTACKPDU_H__*/
