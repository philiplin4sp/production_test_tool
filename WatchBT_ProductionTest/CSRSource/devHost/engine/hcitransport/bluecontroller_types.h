// $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bluecontroller_types.h#1 $

#ifndef __BLUECONTROLLER_TYPES_H__
#define __BLUECONTROLLER_TYPES_H__

#include "common/types.h"
#include "hcipacker/hcipacker.h"

//  Handles cannot be zero, so set the default value to 0.
const uint16 DEFAULT_CALL_BACK_HANDLE = 0;

//  There is no zero event, so set the default value to 0.
const uint16 DEFAULT_CALL_BACK_EVENT = 0;

//  Call-back function types
#ifdef _WIN32_WCE
#define HCI_TRANS_CALLING_CONVENTION __stdcall
#else
#define HCI_TRANS_CALLING_CONVENTION
#endif 

enum BROADCAST_TYPE
{
    bc_type_p2p = 0,
    bc_type_active = 1,
    bc_type_piconet = 2,
    bc_type_unused = 3
};

enum BOUNDARY_TYPE
{
    pb_type_start_not_auto_flush = 0,
    pb_type_continuation = 1,
    pb_type_start = 2,
    pb_type_reserved = 3,
};

//  HCI EVENTS
typedef void (HCI_TRANS_CALLING_CONVENTION *EventCallBackFunction)
    ( uint8 eventcode, const PARAMETER_ARRAY eventParameters , void * parameters);
//  HCI ACL
typedef void (HCI_TRANS_CALLING_CONVENTION *DataCallBackFunction)
    ( uint16 connectionHandle , BOUNDARY_TYPE packetBoundaryFlag , BROADCAST_TYPE broadcastFlag ,
      const uint8 *dataStart ,uint16 dataLength, void * parameters);
//  HCI SCO
typedef void (HCI_TRANS_CALLING_CONVENTION *VoiceCallBackFunction)
    ( const uint8 *dataStart , uint32 dataLength, void * parameters);

typedef void (*PlainDataCallBackFunction)
    ( const uint8 * buffer , uint32 length , void * parameters);
typedef void (*PCCallBackFunction)
    ( const PARAMETER_ARRAY eventParameters , void * parameters);
typedef void (*BlueStackPrimCallBackFunction) ( uint16 type , const void * prim , void * parameters );
typedef PlainDataCallBackFunction DebugDataCallBackFunction;
typedef void (*VMDataCallBackFunction)
    ( const uint16 * data , void * parameters);
typedef PlainDataCallBackFunction DFUDataCallBackFunction;

#endif
