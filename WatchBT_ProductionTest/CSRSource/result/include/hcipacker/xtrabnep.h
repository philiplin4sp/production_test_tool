///////////////////////////////////////////////////////////////////////////////
//
//  FILENAME:   xtrabnep.h
//
//  PURPOSE :   Headers for hand crafted bnep pdus
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __XTRABNEP_H__
#define __XTRABNEP_H__

#include "common/types.h"
#include "hcipacker/bluestackpdu.h"
#include "common/types_t.h"
#include "app/bluestack/bluetooth.h"
#include "app/pan/bnep_hacked.h"


class BNEP_DATA_REQ_T_PDU : public BNEP_PDU
{
public:
    BNEP_DATA_REQ_T_PDU() ;
    BNEP_DATA_REQ_T_PDU( const PDU& );
    BNEP_DATA_REQ_T_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_ether_type(void) const ;
    void set_ether_type(uint16 value) ;
    EtherNetAddress get_rem_addr(void) const ;
    void set_rem_addr(EtherNetAddress value) ;
    uint16 get_len(void) const ;
    void set_len(uint16 len);
    void get_data( uint8 * copyTo ) const ;
    const uint8 * get_dataPtr() const ;
    void set_data( const uint8* copyFrom , uint16 length ) ;
};

class BNEP_DEBUG_IND_T_PDU : public BNEP_PDU
{
public:
    BNEP_DEBUG_IND_T_PDU() ;
    BNEP_DEBUG_IND_T_PDU( const PDU& );
    BNEP_DEBUG_IND_T_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_id(void) const ;
    void set_id(uint16 value) ;
    uint16 get_test(void) const ;
    void set_test(uint16 value) ;
    void get_args( uint16 * copyTo ) const ;
    void set_args( const uint16 * copyFrom ) ;
};

class BNEP_DEBUG_REQ_T_PDU : public BNEP_PDU
{
public:
    BNEP_DEBUG_REQ_T_PDU() ;
    BNEP_DEBUG_REQ_T_PDU( const PDU& );
    BNEP_DEBUG_REQ_T_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_id(void) const ;
    void set_id(uint16 value) ;
    uint16 get_test(void) const ;
    void set_test(uint16 value) ;
    void get_args( uint16 * copyTo ) const ;
    void set_args( const uint16 * copyFrom ) ;
};

class BNEP_EXTENDED_MULTICAST_DATA_REQ_T_PDU : public BNEP_PDU
{
public:
    BNEP_EXTENDED_MULTICAST_DATA_REQ_T_PDU() ;
    BNEP_EXTENDED_MULTICAST_DATA_REQ_T_PDU( const PDU& );
    BNEP_EXTENDED_MULTICAST_DATA_REQ_T_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_id_not(void) const ;
    void set_id_not(uint16 value) ;
    uint16 get_ether_type(void) const ;
    void set_ether_type(uint16 value) ;
    EtherNetAddress get_dst_addr(void) const ;
    void set_dst_addr(EtherNetAddress value) ;
    EtherNetAddress get_src_addr(void) const ;
    void set_src_addr(EtherNetAddress value) ;
    uint16 get_len(void) const ;
    void set_len(uint16 len);
    void get_data( uint8 * copyTo ) const ;
    const uint8 * get_dataPtr() const ;
    void set_data( const uint8* copyFrom , uint16 length ) ;
};

class BNEP_LOOPBACK_DATA_IND_T_PDU : public BNEP_PDU
{
public:
    BNEP_LOOPBACK_DATA_IND_T_PDU() ;
    BNEP_LOOPBACK_DATA_IND_T_PDU( const PDU& );
    BNEP_LOOPBACK_DATA_IND_T_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_phandle(void) const ;
    void set_phandle(uint16 value) ;
    uint16 get_len(void) const ;
    void get_data( uint8 * copyTo ) const ;
    const uint8 * get_dataPtr() const ;
    void set_data( const uint8* copyFrom , uint16 length ) ;
};

class BNEP_EXTENDED_DATA_IND_T_PDU : public BNEP_PDU
{
public:
    BNEP_EXTENDED_DATA_IND_T_PDU() ;
    BNEP_EXTENDED_DATA_IND_T_PDU( const PDU& );
    BNEP_EXTENDED_DATA_IND_T_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_id(void) const ;
    void set_id(uint16 value) ;
    uint16 get_ether_type(void) const ;
    void set_ether_type(uint16 value) ;
    EtherNetAddress get_dst_addr(void) const ;
    void set_dst_addr(EtherNetAddress value) ;
    EtherNetAddress get_src_addr(void) const ;
    void set_src_addr(EtherNetAddress value) ;
    uint16 get_len(void) const ;
    void get_data( uint8 * copyTo ) const ;
    const uint8 * get_dataPtr() const ;
    void set_data( const uint8* copyFrom , uint16 length ) ;
};

class BNEP_DATA_IND_T_PDU : public BNEP_PDU
{
public:
    BNEP_DATA_IND_T_PDU() ;
    BNEP_DATA_IND_T_PDU( const PDU& );
    BNEP_DATA_IND_T_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_phandle(void) const ;
    void set_phandle(uint16 value) ;
    uint16 get_len(void) const ;
    void get_data( uint8 * copyTo ) const ;
    const uint8 * get_dataPtr() const ;
    void set_data( const uint8* copyFrom , uint16 length ) ;
};

class BNEP_MULTICAST_DATA_IND_T_PDU : public BNEP_PDU
{
public:
    BNEP_MULTICAST_DATA_IND_T_PDU() ;
    BNEP_MULTICAST_DATA_IND_T_PDU( const PDU& );
    BNEP_MULTICAST_DATA_IND_T_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_phandle(void) const ;
    void set_phandle(uint16 value) ;
    uint16 get_len(void) const ;
    void get_data( uint8 * copyTo ) const ;
    const uint8 * get_dataPtr() const ;
    void set_data( const uint8* copyFrom , uint16 length ) ;
};

class BNEP_EXTENDED_DATA_REQ_T_PDU : public BNEP_PDU
{
public:
    BNEP_EXTENDED_DATA_REQ_T_PDU() ;
    BNEP_EXTENDED_DATA_REQ_T_PDU( const PDU& );
    BNEP_EXTENDED_DATA_REQ_T_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_id(void) const ;
    void set_id(uint16 value) ;
    uint16 get_ether_type(void) const ;
    void set_ether_type(uint16 value) ;
    EtherNetAddress get_dst_addr(void) const ;
    void set_dst_addr(EtherNetAddress value) ;
    EtherNetAddress get_src_addr(void) const ;
    void set_src_addr(EtherNetAddress value) ;
    uint16 get_len(void) const ;
    void set_len(uint16 len);
    void get_data( uint8 * copyTo ) const ;
    const uint8 * get_dataPtr() const ;
    void set_data( const uint8* copyFrom , uint16 length ) ;
};

#endif//__XTRABNEP_H__
