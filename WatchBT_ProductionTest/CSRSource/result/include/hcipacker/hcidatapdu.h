///////////////////////////////////////////////////////////////////////////////
//
//  FILE:       hcidatapdu.h
//
//  CLASSES:    HCIDataPDU, HCIACLPDU, HCISCOPDU
//
//  PURPOSE:    bluetooth data packets.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __HCIDATAPDU_H__
#define __HCIDATAPDU_H__

#include "hcipdu.h"

class HCIDataPDU : public HCIPDU
{
public:
    void set_handle( uint16 handle );
    uint16 get_handle() const;
    ~HCIDataPDU() {};
protected:
    HCIDataPDU( PDU::bc_channel channel , size_t len ) : HCIPDU ( channel , len ) {}
    HCIDataPDU( PDU::bc_channel channel , const uint8* data , uint32 len ) : HCIPDU ( channel , data, len ) {}
    HCIDataPDU( const PDU& from ) : HCIPDU ( from ) {}
};

class HCIACLPDU : public HCIDataPDU
{
public:
    static const size_t header_size;
    enum packet_boundary_flag { start = 0x2000 , cont = 0x1000 , start_not_auto_flush = 0 , pbf_reserved1 = 0x3000 };
    enum broadcast_type { ptp = 0 , act = 0x4000 , pic = 0x8000 , bct_reserved = 0xc000 };

    HCIACLPDU ( const uint8 *payload, uint32 length, uint16 handle, packet_boundary_flag pbf, broadcast_type bct);
    HCIACLPDU ( uint16 data_length ) : HCIDataPDU ( PDU::hciACL , size_t(data_length + header_size) ) { set_length (); }
    HCIACLPDU ( const uint8 * buffer , uint32 length ) : HCIDataPDU ( PDU::hciACL , buffer , length ) {}
    explicit HCIACLPDU ( const PDU& from ) : HCIDataPDU ( from ) {}

    void set_header ( uint16 handle , packet_boundary_flag pbf = start , broadcast_type bct = ptp );
    void set_pbf ( packet_boundary_flag pbf );
    void set_bct ( broadcast_type bct );
    void set_length ();

    packet_boundary_flag get_pbf () const;
    broadcast_type get_bct () const;
    uint16 get_length() const;

    void set_data ( const uint8* copyfrom , uint16 length );
    void get_data ( uint8* copyto , uint16 length ) const;
    const uint8 * get_dataPtr () const;
};

class HCISCOPDU : public HCIDataPDU
{
public:
    static const size_t header_size;
    HCISCOPDU ( uint16 data_length ) : HCIDataPDU ( PDU::hciSCO , size_t(data_length + header_size) ) { set_length(); }
    HCISCOPDU ( const uint8 * data , uint32 len ) : HCIDataPDU ( PDU::hciSCO , data , len ) {}
    explicit HCISCOPDU ( const PDU& from ) : HCIDataPDU ( from ) {}

    void set_length();
    uint8 get_length() const;
    void set_data ( const uint8* copyfrom , uint16 length );
    void get_data ( uint8* copyto , uint16 length ) const;
    const uint8 * get_dataPtr () const;
};

#endif /*__HCIDATAPDU_H__*/
