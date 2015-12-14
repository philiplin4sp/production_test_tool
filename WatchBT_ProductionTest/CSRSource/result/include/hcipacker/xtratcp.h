///////////////////////////////////////////////////////////////////////////////
//
//  FILENAME:   xtratcp.h
//
//  PURPOSE :   Headers for hand crafted tcp pdus
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __XTRATCP_H__
#define __XTRATCP_H__

#include "common/types.h"
#include "hcipacker/bluestackpdu.h"
#include "common/types_t.h"
#include "app/bluestack/bluetooth.h"
#include "app/pan/tcp_hacked.h"


class TCP_DATA_IND_T_PDU : public TCP_PDU
{
public:
    TCP_DATA_IND_T_PDU() ;
    TCP_DATA_IND_T_PDU( const PDU& );
    TCP_DATA_IND_T_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_phandle(void) const ;
    void set_phandle(uint16 value) ;
    uint16 get_b_out(void) const ;
    void set_b_out(uint16 value) ;
    uint16 get_len(void) const ;
    void get_data( uint8 * copyTo ) const ;
    const uint8 * get_dataPtr() const ;
    void set_data( const uint8* copyFrom , uint16 length ) ;
};

class TCP_DATA_REQ_T_PDU : public TCP_PDU
{
public:
    TCP_DATA_REQ_T_PDU() ;
    TCP_DATA_REQ_T_PDU( const PDU& );
    TCP_DATA_REQ_T_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_b_out(void) const ;
    void set_b_out(uint16 value) ;
    uint16 get_len(void) const ;
    void set_len(uint16 len);
    void get_data( uint8 * copyTo ) const ;
    const uint8 * get_dataPtr() const ;
    void set_data( const uint8* copyFrom , uint16 length ) ;
};

#endif//__XTRATCP_H__
