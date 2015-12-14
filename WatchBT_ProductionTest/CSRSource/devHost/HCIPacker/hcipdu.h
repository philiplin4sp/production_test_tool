#ifndef _HCIPDU_H
#define _HCIPDU_H

#include "hcipacker/pdu.h"
#include "bluetoothclasses/bluetoothclasses.h"
#include "common/types_t.h"

class HCIPDU : public PDU
{
protected :
    HCIPDU( PDU::bc_channel channel , uint16 pduSize ) ;
    HCIPDU( const PDU& );
    HCIPDU( PDU::bc_channel channel , const uint8 * data , uint32 len ) ;
    uint32 get_uint32 ( uint16 offset ) const; 
    uint24 get_uint24 ( uint16 offset ) const;
    BluetoothDeviceAddress get_BluetoothDeviceAddress ( uint16 offset ) const;
    ClassOfDevice get_ClassOfDevice ( uint16 offset ) const;

    void set_uint32 ( uint16 offset , uint32 data ); 
    void set_uint24 ( uint16 offset , uint24 data );
    void set_BluetoothDeviceAddress ( uint16 offset , const BluetoothDeviceAddress& data );
    void set_ClassOfDevice ( uint16 offset , const ClassOfDevice& data );
} ;

#endif
