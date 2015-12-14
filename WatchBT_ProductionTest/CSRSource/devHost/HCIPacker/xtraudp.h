///////////////////////////////////////////////////////////////////////////////
//
//  FILENAME:   xtraudp.h
//
//  PURPOSE :   Headers for manually maintained udp pdus
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __XTRAUDP_H__
#define __XTRAUDP_H__

#include "common/types.h"
#include "hcipacker/bluestackpdu.h"
#include "common/types_t.h"
#include "app/bluestack/bluetooth.h"
#include "app/pan/udp_hacked.h"


class UDP_EXTENDED_DATA_IND_T_PDU : public UDP_PDU
{
public:
    UDP_EXTENDED_DATA_IND_T_PDU() ;
    UDP_EXTENDED_DATA_IND_T_PDU( const PDU& );
    UDP_EXTENDED_DATA_IND_T_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_phandle(void) const ;
    void set_phandle(uint16 value) ;
    IPAddress get_rem_addr(void) const ;
    void set_rem_addr(IPAddress value) ;
    uint16 get_rem_port(void) const ;
    void set_rem_port(uint16 value) ;
    IPAddress get_loc_addr(void) const ;
    void set_loc_addr(IPAddress value) ;
    uint16 get_id(void) const ;
    void set_id(uint16 value) ;
    uint16 get_len(void) const ;
    const uint8 * get_dataPtr(void) const ;
    void set_data( const uint8 * data , uint16 len ) ;
};

class UDP_DATA_IND_T_PDU : public UDP_PDU
{
public:
    UDP_DATA_IND_T_PDU() ;
    UDP_DATA_IND_T_PDU( const PDU& );
    UDP_DATA_IND_T_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_phandle(void) const ;
    void set_phandle(uint16 value) ;
    uint16 get_id(void) const ;
    void set_id(uint16 value) ;
    uint16 get_len(void) const ;
    const uint8 * get_dataPtr(void) const ;
    void set_data( const uint8 * data , uint16 len ) ;
};

class UDP_DATA_REQ_T_PDU : public UDP_PDU
{
public:
    UDP_DATA_REQ_T_PDU() ;
    UDP_DATA_REQ_T_PDU( const PDU& );
    UDP_DATA_REQ_T_PDU( const uint8 * buffer , uint32 len ) ;
    uint16 get_id(void) const ;
    void set_id(uint16 value) ;
    uint16 get_len(void) const ;
    void set_len(uint16 len);
    const uint8 * get_dataPtr(void) const ;
    void set_data( const uint8 * data , uint16 len ) ;
};

class UDP_EXTENDED_DATA_REQ_T_PDU : public UDP_PDU
{
public:
    UDP_EXTENDED_DATA_REQ_T_PDU() ;
    UDP_EXTENDED_DATA_REQ_T_PDU( const PDU& );
    UDP_EXTENDED_DATA_REQ_T_PDU( const uint8 * buffer , uint32 len ) ;
    IPAddress get_rem_addr(void) const ;
    void set_rem_addr(IPAddress value) ;
    uint16 get_rem_port(void) const ;
    void set_rem_port(uint16 value) ;
    IPAddress get_loc_addr(void) const ;
    void set_loc_addr(IPAddress value) ;
    uint16 get_id(void) const ;
    void set_id(uint16 value) ;
    uint16 get_len(void) const ;
    void set_len(uint16 len);
    const uint8 * get_dataPtr(void) const ;
    void set_data( const uint8 * data , uint16 len ) ;
};

#endif//__XTRAUDP_H__
