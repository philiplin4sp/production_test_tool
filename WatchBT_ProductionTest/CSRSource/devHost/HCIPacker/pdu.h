///////////////////////////////////////////////////////////////////////////////
//
//  FILE:       pdu.h
//
//  CLASS:      PDU
//
//  PURPOSE:    A PDU is the root of the whole pdu heirarchy.
//              Defining characterstics of a PDU are that in can be created in
//              one of two ways:
//              1) Directly, as an object.  This is for outgoing PDUs - the
//                 app fills in the fields and posts the pdu over the
//                 transport.
//              2) Indirectly from a byte-array.  This is primarily done from
//                 a factory which knows how to figure out the appropriate
//                 type from a field in the byte-array.  Typical scenario is
//                 that a byte-array arrives over the transport and we then
//                 want to turn it into an object.  Alternatively, a debugging
//                 utility might create objects from a trace.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _PDU_H
#define _PDU_H

#include <stddef.h>
#include "common/types.h"

class PDU
{
public :
    enum bc_channel
    {
        zero    = 0,  other      = 0,
        one     = 1,  le         = 1,
        two     = 2,  bccmd      = 2, h2hc = 2,
        three   = 3,  hq         = 3, hc2h = 3,
        four    = 4,  dm         = 4,
        five    = 5,  hciCommand = 5,
        six     = 6,  hciACL     = 6,
        seven   = 7,  hciSCO     = 7,
        eight   = 8,  l2cap      = 8,
        nine    = 9,  rfcomm     = 9,
        ten     = 10, sdp        = 10,
        eleven  = 11, debug      = 11,
        twelve  = 12, upgrade    = 12,
        thirteen= 13, vm         = 13,
        fourteen= 14,
        fifteen = 15,
        bcsp_channel_count = 16, // 0x10
        lmp_debug = 20,
        h4dp = 21,
        raw = 22,
        csr_hci_extensions_count = 64 // 0x40
    };
    PDU ( bc_channel channel , uint32 pduSize = 0 ) ;
    PDU ( bc_channel , const uint8* , uint32 );

    PDU& operator= ( const PDU& );
    PDU ( const PDU& );
    virtual ~PDU() ;

    // return a pointer to the buffer holding the pdu
    const uint8 * data() const;
    // return the length of the pdu in bytes. 
    size_t size() const;
    // return the BCSP channel that this pdu is supposed to use.
    bc_channel channel() const;

    PDU clone () const;
protected:
    void resize ( uint32 newLen );

    uint8  get_uint8 (uint32 offset) const;
    int8   get_int8  (uint32 offset) const;
    uint16 get_uint16(uint32 offset) const;
    uint24 get_uint24(uint32 offset) const; // BlueCore friendly format
    uint32 get_uint32(uint32 offset) const; // BlueCore friendly format
    uint8* get_uint8ArrayPtr(uint32 offset) const;
    uint16* get_uint16ArrayPtr(uint32 offset) const;
    //  return the number of bytes copied
    uint32 get_uint8Array(uint8* dest,uint32 offset,uint32 size) const;
    uint32 get_uint16Array(uint16* dest,uint32 offset,uint32 size) const;
    void set_uint8 (uint32 offset,uint8 data);
	void set_int8(uint32 offset, int8 data);
    void set_uint16(uint32 offset,uint16 data);
    void set_uint24(uint32 offset,uint24 data); // BlueCore friendly format
    void set_uint32(uint32 offset,uint32 data); // BlueCore friendly format
    //  copy as much as will fit. return the amount copied.
    uint32 set_uint8Array(const uint8* src,uint32 offset,uint32 size);
    void set_uint16Array(const uint16* src, uint32 offset,uint32 size);
private:
    class PDUData;
    PDUData * pData;
};

extern const PDU null_pdu;

#endif //guard
