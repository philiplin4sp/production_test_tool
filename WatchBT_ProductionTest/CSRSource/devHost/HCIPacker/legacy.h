///////////////////////////////////////////////////////////////////////////////
//
//  FILE:       legacy.h
//
//  CLASS:      PDUFactory
//
//  PURPOSE:    Support "parameter array" interface to packer.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef HCIPACKER_LEGACY_HEADER
#define HCIPACKER_LEGACY_HEADER

#ifdef __cplusplus
#include "hcipacker/hcipdu.h"
#include "hcipacker/bluecorepdu.h"
#endif

/*********************/
/*  LEGACY INCLUDES  */
/****************************************************************************
DATA STRUCTURE
        PARAMETER_ARRAY

        An array of uint32s.
        The first contains length data (ie the size of the array, one
        more than the number of data elements.).
        All data is packed as uint8s, uint16s, uint24s or uint32s
        into uint32s.
        Bluetooth device addresses are treated as Lap, Uap, Nap.
        Link keys, event masks, pin codes, local names are wastefully
        kept as arrays of uint8s spread into the array of uint32s.
****************************************************************************/

typedef uint32 PARAMETER_VALUE;

typedef PARAMETER_VALUE* PARAMETER_ARRAY;

enum BadPDUReason
{
    BP_HCI_WRONG_SIZE ,
    BP_PC_TOO_SMALL ,
    BP_NO_SUCH_VARID ,
    BP_NO_SUCH_EVENT_CODE ,
    BP_NO_SUCH_COMMAND ,
    BP_OUT_OF_DATA ,
    BP_WARNING_UNKNOWN_FORMAT,
    BP_HCI_DATA_RESERVED_PBF,
    BP_HCI_DATA_WRONG_SIZE,
    BP_HCI_DATA_RESERVED_BCF,
    BP_WRONG_TYPE,
    BP_UNHANDLED_CHANNEL
};

#ifdef __cplusplus
class HCIPDUFactory
{
public:
    static HCIPDU * createCommandPDU( uint32 * parameters ) ;
    static BCCMD_PDU createBCCMD_PDU ( const uint32 * );
    static HQ_PDU createHQ_PDU ( const uint32 * );

    //  returns length.  Pass in Null to parameters, to get a length calculation,
    //  then find the memory, and pass it in a second time.
    //  First uint32 MUST contain the length of the array.
    static uint32 decomposeEventPDU( const PDU& , uint32 * parameters , BadPDUReason& why_failed ) ;
    static uint32 decomposeCommandCompletePDU( const PDU& , uint32 * parameters , BadPDUReason& why_failed ) ;

    //  returns length.  Pass in zero to parameters, to get a length calculation,
    //  then find a block of memory, and call it a second time, passing in the
    //  block.
    //  First uint32 MUST contain the length of the array.
    static uint32 decomposeBCCMD_PDU( const PDU& , uint32 * parameters , BadPDUReason& why_failed );
    static uint32 decomposeHQ_PDU( const PDU& , uint32 * parameters , BadPDUReason& why_failed );

/****************************************************************************
SUPPORTING FUNCTIONS
        encodeBytes
        decodeBytes

        To make it easy (or at least unnecessary) to worry about packing
        uint8 arrays into the array, there are these two functions
        to do it.

        Pass a pointer and a size to encodeBytes, and it'll put them into
        your array, checking first to make sure there is enough space.

        Pass a pointer to an address of a buffer, and the size of that buffer
        to decodeBytes and it will read as much data as it can (limited by
        the size of data in the array and the size of the buffer it is given.
        If you pass it a pointer to a NULL, and/or a tell it the length is
        zero, then it changes the null to a pointer to a "new uint8[]" bit
        of memory, and puts the whole of the rest of the array into the buffer.
****************************************************************************/

    static bool   encodeBytes ( const uint8 *aDataStart ,
                                uint32 aDataLength ,
                                const uint32 * const aParameterArray ,
                                uint32 **aCurrentIndex );

    static uint32 decodeBytes ( uint8 *aPointerToBuffer ,
                                uint32  aLengthOfReceivingBuffer ,
                                const uint32 * const aParameterArray ,
                                const uint32 **aCurrentOutdex );
} ;
#endif

#endif/*HCIPACKER_LEGACY_HEADER*/

