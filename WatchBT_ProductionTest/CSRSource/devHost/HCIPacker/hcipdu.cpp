///////////////////////////////////////////////////////////////////////////////
//
//  FILE:   hcipdu.cpp
//
//  PURPOSE:frustrating Adam
//
///////////////////////////////////////////////////////////////////////////////

#include "hcipdu.h"
#include "string.h"


HCIPDU::HCIPDU( PDU::bc_channel channel , uint16 pduSize )
: PDU( channel , pduSize)
{
}

HCIPDU::HCIPDU( const PDU& pdu )
: PDU(pdu)
{
}

HCIPDU::HCIPDU( PDU::bc_channel channel , const uint8 * data ,uint32 len ) : PDU( channel , data , len )
{
}
        
uint32 HCIPDU::get_uint32(uint16 offset) const
{
    uint32 v = get_uint16(offset) ;
    v += (((uint32)get_uint16(offset+2)) <<16);
    return v ;
}

uint24 HCIPDU::get_uint24(uint16 offset) const
{
    uint32 v = get_uint16(offset) ;
    v += (((uint32)get_uint8(offset+2)) <<16);
    return v ;
}

void HCIPDU::set_uint32(uint16 offset,uint32 data) 
{
    set_uint16(offset,uint16(data & 0xffff)) ;
    set_uint16(offset+2,uint16(data >> 16)) ;
}

void HCIPDU::set_uint24(uint16 offset,uint24 data)
{
    set_uint16(offset,uint16(data & 0xffff)) ;
    set_uint8(offset+2,uint8(data >> 16)) ;
}

void HCIPDU::set_BluetoothDeviceAddress ( uint16 offset , const BluetoothDeviceAddress& addr)
{
    set_uint24 ( offset     , addr.get_lap() );
    set_uint8  ( offset + 3 , addr.get_uap() );
    set_uint16 ( offset + 4 , addr.get_nap() );
}

BluetoothDeviceAddress HCIPDU::get_BluetoothDeviceAddress(uint16 offset) const
{
    return BluetoothDeviceAddress ( get_uint16 ( offset + 4 ),
                                    get_uint8  ( offset + 3 ),
                                    get_uint24 ( offset ) ) ; 
}

void HCIPDU::set_ClassOfDevice ( uint16 offset , const ClassOfDevice& cod )
{
    set_uint24 ( offset , cod.get_class_of_device() );
}

ClassOfDevice HCIPDU::get_ClassOfDevice(uint16 offset) const
{
    return ClassOfDevice ( get_uint8ArrayPtr ( offset ) );
}

/****************************************************************************
**  PARAMETER ARRAY LEGACY SUPPORT FOR BTCLI
****************************************************************************/
#include "legacy.h"
/****************************************************************************
NAME
    encode_bytes  -  copies bytewise data into the PARAMETER_ARRAY
                     from a buffer

PARAMETERS
        a buffer, a length and a parameter array list.

RETURNS
        true if things work out, false if there isn't enough space...
****************************************************************************/
bool HCIPDUFactory::encodeBytes ( const uint8 *aDataStart ,
                                  uint32 aDataLength ,
                                  const uint32 * const aParameterArray ,
                                  uint32 **aCurrentIndex )
{
    uint32 lArrayLength ;
    uint32 lCounter ;
    
    lArrayLength = aParameterArray[0] ;

    /*  Check we have the space for all that data */
    if ( aParameterArray + lArrayLength >= *aCurrentIndex + aDataLength )
    {
        /*  write the data, sparsely placed */
        for ( lCounter = 0 ; lCounter < aDataLength ; lCounter ++ )
            *(*aCurrentIndex)++ = *aDataStart++;
    }
    else
        return false;

    return true;
}

/****************************************************************************
NAME
    decode_bytes  -  copies bytewise data out of the PARAMETER_ARRAY
                     to a buffer

PARAMETERS
        a buffer and a parameter array list.

RETURNS
        true if things work out, false if it gets confused...
****************************************************************************/
uint32 HCIPDUFactory::decodeBytes ( uint8 *aPointerToBuffer ,
                                    uint32 aLengthOfReceivingBuffer ,
                                    const uint32 * const aParameterArray ,
                                    const uint32 **aCurrentOutdex )
{
    uint32 lHowMuchData;
    uint32 lCounter;

    /* total is how much is in the whole array (in the first element) less */
    /* how far we've read already */
    lHowMuchData = aParameterArray[0] + aParameterArray - *aCurrentOutdex ;

    if ( lHowMuchData != 0 )
    {
        if ( aPointerToBuffer == NULL || aLengthOfReceivingBuffer == 0 )
        {
            return lHowMuchData;
        }
        if ( lHowMuchData > aLengthOfReceivingBuffer )
            lHowMuchData = aLengthOfReceivingBuffer;
        for ( lCounter = 0 ; lCounter < lHowMuchData ; lCounter++ )
            (aPointerToBuffer)[lCounter] = (uint8) *(*aCurrentOutdex)++;
    }

    return lHowMuchData;
}

