///////////////////////////////////////////////////////////////////////////////
//
//  FILE:       bluestackpdu.cpp
//
//  CLASSES:    RFCOMM_PDU, DM_PDU, L2CAP_PDU, SDP_PDU, BlueStackPDU
//
//  PURPOSE:    Declare BlueStack specific pdus.
//
///////////////////////////////////////////////////////////////////////////////

#include "bluestackpdu.h"

BlueStackPDU::BlueStackPDU ( PDU::bc_channel channel , uint32 size )
: PCPDU ( channel , size )
{}

BlueStackPDU::BlueStackPDU ( const PDU& from )
: PCPDU (from)
{}

BlueStackPDU::BlueStackPDU ( PDU::bc_channel channel , const uint8* buffer , uint32 len )
: PCPDU (channel,buffer,len)
{}

const BluetoothDeviceAddress BlueStackPDU::get_BluetoothDeviceAddress ( uint32 offset ) const
{
    uint24 lap = get_uint24 ( offset );
    uint8 uap = get_uint8 ( offset + 4 );
    uint16 nap = get_uint16 ( offset + 6 );
    return BluetoothDeviceAddress ( nap , uap , lap );
}

void BlueStackPDU::set_BluetoothDeviceAddress ( uint32 offset , const BluetoothDeviceAddress& value )
{
    set_uint24 ( offset ,  value.get_lap() );
    set_uint8 ( offset + 4 , value.get_uap() );
    set_uint16 ( offset + 6 , value.get_nap() );
}

const BluetoothDeviceAddressWithType BlueStackPDU::get_BluetoothDeviceAddressWithType ( uint32 offset ) const
{
    uint8 type = get_uint8 ( offset );
    BluetoothDeviceAddress addr = get_BluetoothDeviceAddress ( offset + 2 );
    return BluetoothDeviceAddressWithType( type, addr );
}

void BlueStackPDU::set_BluetoothDeviceAddressWithType( uint32 offset , const BluetoothDeviceAddressWithType& value )
{
    set_uint8 ( offset , value.get_type() );
    set_BluetoothDeviceAddress ( offset + 2 , value.get_addr() );
}

void BlueStackPDU::set_ClassOfDevice ( uint32 offset , const ClassOfDevice& data )
{
    set_uint24 ( offset , data.get_class_of_device() );
}

const ClassOfDevice BlueStackPDU::get_ClassOfDevice ( uint32 offset ) const
{
    return ClassOfDevice ( get_uint24 ( offset ) );
}

uint16 BlueStackPDU::get_type () const
{
    return get_uint16 ( 0 );
}

void BlueStackPDU::set_type ( uint16 type )
{
    set_uint16 ( 0 , type );
}

void BlueStackPDU::set_final_uint8Ptr
     ( uint32 size_index ,
       uint32 pdu_size ,
       const uint8* copyFrom , uint32 size )
{
    //  calculate the new size of the pdu.
    uint32 newSize = pdu_size + size;
    resize ( newSize );
    //  set the size...
    set_uint16 ( size_index , size );
    //  and write the data after the pdu. close packed uint8s.
    PDU::set_uint8Array ( copyFrom , pdu_size , size );
}

void BlueStackPDU::set_penult_uint8Ptr
     ( uint32 size_index , uint32 size_index2 ,
       uint32 pdu_size ,
       const uint8* copyFrom , uint32 size )
{
    //  calculate the new size of the pdu.
    uint32 last_ptr_data_size = get_uint16 ( size_index2 );
    uint32 newSize = pdu_size + last_ptr_data_size + size ;
    if ( newSize != size )
    {
        uint8 * last_ptr_data = 0;
        //  save the data assoc with other ptr.
        if ( last_ptr_data_size )
        {
            last_ptr_data = new uint8[last_ptr_data_size];
            //  close packed uint8s.
            PDU::get_uint8Array ( last_ptr_data ,
                                  pdu_size ,
                                  last_ptr_data_size );
        }
        //  resize.
        resize(newSize);
        //  restore the other data.
        if ( last_ptr_data_size )
        {
            set_final_uint8Ptr ( size_index2 ,
                                 pdu_size + size * 2 ,
                                 last_ptr_data , last_ptr_data_size );
            delete[] last_ptr_data;
        }
    }
    //  set the size...
    set_uint16 ( size_index , size );
    //  and write the data after the pdu.  close packed uint8s.
    PDU::set_uint8Array ( copyFrom , pdu_size , size );
}

RFCOMM_PDU::RFCOMM_PDU ( uint16 type , uint32 size )
: BlueStackPDU ( PDU::rfcomm , size )
{
    set_type ( type );
}

RFCOMM_PDU::RFCOMM_PDU ( const PDU& from )
: BlueStackPDU ( from )
{}

RFCOMM_PDU::RFCOMM_PDU ( const uint8* buffer , uint32 len )
: BlueStackPDU ( PDU::rfcomm , buffer , len )
{}

L2CAP_PDU::L2CAP_PDU ( uint16 type , uint32 size )
: BlueStackPDU ( PDU::l2cap , size )
{
    set_type ( type );
}

L2CAP_PDU::L2CAP_PDU ( const PDU& from )
: BlueStackPDU ( from )
{}

L2CAP_PDU::L2CAP_PDU ( const uint8* buffer , uint32 len )
: BlueStackPDU ( PDU::l2cap , buffer , len )
{}

STREAM_BUFFER_SIZES_T L2CAP_PDU::get_STREAM_BUFFER_SIZES_T( uint32 offset ) const
{
    STREAM_BUFFER_SIZES_T streams;
    uint16 a[2];

    get_uint16Array ( a , offset , 2 );

    streams.buffer_size_sink = a[0];
    streams.buffer_size_source = a[1];

    return streams;
}

void L2CAP_PDU::set_STREAM_BUFFER_SIZES_T(uint32 offset, STREAM_BUFFER_SIZES_T streams)
{
    uint16 a[2];

    a[0] = streams.buffer_size_sink;
    a[1] = streams.buffer_size_source;

    set_uint16Array ( a , offset , 2 );
}

DM_SM_SERVICE_T L2CAP_PDU::get_DM_SM_SERVICE_T( uint32 offset ) const
{
    DM_SM_SERVICE_T service;
    uint16 a[2];

    get_uint16Array ( a , offset , 2 );

    service.protocol_id = a[0];
    service.channel = a[1];

    return service;
}

void L2CAP_PDU::set_DM_SM_SERVICE_T( uint32 offset, DM_SM_SERVICE_T value)
{
    uint16 a[2];

    a[0] = value.protocol_id;
    a[1] = value.channel;

    set_uint16Array ( a , offset , 2 );
}

SDP_PDU::SDP_PDU ( uint16 type , uint32 size )
: BlueStackPDU ( PDU::sdp , size )
{
    set_type ( type );
}

SDP_PDU::SDP_PDU ( const PDU& from )
: BlueStackPDU ( from )
{}

SDP_PDU::SDP_PDU ( const uint8* buffer , uint32 len )
: BlueStackPDU ( PDU::sdp , buffer , len )
{}

DM_PDU::DM_PDU ( uint16 type , uint32 size )
: BlueStackPDU ( PDU::dm , size )
{
    set_type ( type );
}

DM_PDU::DM_PDU ( const PDU& from )
: BlueStackPDU ( from )
{}

DM_PDU::DM_PDU ( const uint8* buffer , uint32 len )
: BlueStackPDU ( PDU::dm , buffer , len )
{}


void DM_PDU::get_DM_SM_UKEY_T ( uint32 offset,
                                uint32 u_offset,
                                uint16 keys,
                                DM_SM_RFCLI_UKEY_T *u ) const
{
    for (uint16_t i = 0; i != DM_SM_MAX_NUM_KEYS && keys != 0; ++i, keys >>= DM_SM_NUM_KEY_BITS)
    {
        switch ((DM_SM_KEY_TYPE_T)(keys & ((1 << DM_SM_NUM_KEY_BITS) - 1)))
        {
            case DM_SM_KEY_ENC_BREDR:
                get_uint16Array(u[i].enc_bredr.link_key, offset, 8);
                offset += 16;
                break;

            case DM_SM_KEY_SIGN:
                get_uint16Array(u[i].sign.csrk, offset, 8);
                offset += 16;
                break;

            case DM_SM_KEY_ID:
                get_uint16Array(u[i].id.irk, offset, 8);
                offset += 16;
                break;

            case DM_SM_KEY_ENC_CENTRAL:
                u[i].enc_central.ediv = get_uint16(offset);
                offset += 2;
                get_uint16Array(u[i].enc_central.rand, offset, 4);
                offset += 8;
                get_uint16Array(u[i].enc_central.ltk, offset, 8);
                offset += 16;
                break;

            case DM_SM_KEY_DIV:
                u[i].div = get_uint16(u_offset + 2*i);
                break;

            case DM_SM_KEY_NONE:
            default:
                break;
        }
    }
}

void DM_PDU::set_DM_SM_UKEY_T ( uint32 &offset,
                                uint32 u_offset,
                                uint16 keys,
                                DM_SM_RFCLI_UKEY_T *u )
{
    for (uint16_t i = 0; i != DM_SM_MAX_NUM_KEYS; ++i, keys >>= DM_SM_NUM_KEY_BITS)
    {
        switch (keys & ((1 << DM_SM_NUM_KEY_BITS) - 1))
        {
            case DM_SM_KEY_ENC_BREDR:
                set_uint16Array(u[i].enc_bredr.link_key, offset, 8);
                offset += 16;
                break;

            case DM_SM_KEY_SIGN:
                set_uint16Array(u[i].sign.csrk, offset, 8);
                offset += 16;
                break;

            case DM_SM_KEY_ID:
                set_uint16Array(u[i].id.irk, offset, 8);
                offset += 16;
                break;

            case DM_SM_KEY_ENC_CENTRAL:
                set_uint16Array(&u[i].enc_central.ediv, offset, 1);
                offset += 2;
                set_uint16Array(u[i].enc_central.rand, offset, 4);
                offset += 8;
                set_uint16Array(u[i].enc_central.ltk, offset, 8);
                offset += 16;
                break;

            case DM_SM_KEY_DIV:
                set_uint16(u_offset + 2*i, u[i].div);
                break;

            case DM_SM_KEY_NONE:
            default:
                break;
        }
    }
}

SMKeyState DM_PDU::get_SMKeyState( uint32 offset ) const
{
    uint16 er[8], ir[8];

    get_uint16Array ( er , offset , 8 );
    get_uint16Array ( ir , offset + 16, 8 );

    return SMKeyState( er, ir);
}

void DM_PDU::set_SMKeyState( uint32 offset, const SMKeyState &value )
{
    set_uint16Array ( value.get_er(), offset, 8 );
    set_uint16Array ( value.get_ir(), offset + 16, 8 );
}

TCP_PDU::TCP_PDU ( uint16 type , uint32 size )
: BlueStackPDU ( PDU::l2cap , size )
{
    set_type ( type );
}

TCP_PDU::TCP_PDU ( const PDU& from )
: BlueStackPDU ( from )
{}

TCP_PDU::TCP_PDU ( const uint8* buffer , uint32 len )
: BlueStackPDU ( PDU::l2cap , buffer , len )
{}

IPAddress TCP_PDU::get_IPAddress( uint32 offset ) const
{
    uint16 addr[8];
    get_uint16Array ( addr , offset , 8 );
    return IPAddress ( addr );
}

void TCP_PDU::set_IPAddress ( uint32 offset , const IPAddress&a )
{
    set_uint16Array ( a.get_data() , offset , 8 );
}

BNEP_PDU::BNEP_PDU ( uint16 type , uint32 size )
: BlueStackPDU ( PDU::l2cap , size )
{
    set_type ( type );
}

BNEP_PDU::BNEP_PDU ( const PDU& from )
: BlueStackPDU ( from )
{}

BNEP_PDU::BNEP_PDU ( const uint8* buffer , uint32 len )
: BlueStackPDU ( PDU::l2cap , buffer , len )
{}

EtherNetAddress BNEP_PDU::get_EtherNetAddress( uint32 offset ) const
{
    uint16 addr[3];
    get_uint16Array ( addr , offset , 3 );
    return EtherNetAddress ( addr );
}

void BNEP_PDU::set_EtherNetAddress ( uint32 offset , const EtherNetAddress&a )
{
    set_uint16Array ( a.get_data() , offset , 3 );
}

ATT_PDU::ATT_PDU ( uint16 type , uint32 size )
: BlueStackPDU ( PDU::sdp , size )
{
    set_type ( type );
}

ATT_PDU::ATT_PDU ( const PDU& from )
: BlueStackPDU ( from )
{}

ATT_PDU::ATT_PDU ( const uint8* buffer , uint32 len )
: BlueStackPDU ( PDU::sdp , buffer , len )
{}

