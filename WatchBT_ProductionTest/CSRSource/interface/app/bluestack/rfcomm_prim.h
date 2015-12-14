/*! 
******************************************************************************
        Copyright (C) CSR plc 2009
        All rights reserved

\file   rfcomm_prim.h 
\brief  This file contains the RFCOMM Interface primitives - the top interface 
        to RFCOMM. Multiple applications will be able to post to this interface.
        Each protocol is responsible for registering a PSM and associated
        handle so that RFCOMM knows where to send incoming events.

        **************************************************************************
        *WARNING: THIS FILE IS NOT JUST C
        **************************************************************************
        *IT IS PARSED BY devHost/HCIPacker/Autogen/Makefile
        *Any lines that look like one of the following will be assumed to contain
        *primitive names, which are extracted so display routines can be generated
        *for them:
        *
        *    } SOME_KIND_OF_NAME;            
        *    typedef FOO ANOTHER_NAME;          autogen_makefile_ignore_this
        *
        *If you write one of these lines and it's not a primitive, include the 
        *magic words:
        *
        *    autogen_makefile_ignore_this 
        *
        *in a comment on the same line, so it can be ignored.
        **************************************************************************

REFERENCES

****************************************************************************** 
*/ 

#ifndef BLUESTACK__RFCOMM_PRIM_H /* Once is enough */

#define BLUESTACK__RFCOMM_PRIM_H

#include "hci.h"
#include "bluetooth.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Comments required to help the Synergy serialisation code. Please maintain
   exact format of the lines as the tool is very fussy!
*/
/* search_string="RFC_PRIM_T" */
/* conversion_rule="ADD_UNDERSCORE_AND_UPPERCASE_T" */
 

/*----------------------------------------------------------------------------*
 RFCOMM primitive types
 *----------------------------------------------------------------------------*/

/* NB RFC_RESPONSE_T can take a status from either RFC_RESULT_T or
   L2CA_RESULT_T.
*/ 
typedef uint16_t  RFC_RESPONSE_T;

#define RFCOMM_DOWN_PRIM RFCOMM_PRIM_BASE
#define RFCOMM_UP_PRIM  (RFCOMM_PRIM_BASE | 0x80)

/* Add primitives here... */
typedef enum
{
    RFC_INIT_REQ= RFCOMM_DOWN_PRIM,
    RFC_REGISTER_REQ,
    RFC_UNREGISTER_REQ,
    RFC_FC_REQ,
    RFC_CLIENT_CONNECT_REQ,
    RFC_SERVER_CONNECT_RSP,
    RFC_DISCONNECT_REQ,
    RFC_DISCONNECT_RSP,
    RFC_DATAWRITE_REQ,
    RFC_DATAREAD_RSP,
    RFC_PORTNEG_REQ,
    RFC_PORTNEG_RSP,
    RFC_MODEM_STATUS_REQ,
    RFC_LINESTATUS_REQ,
    RFC_TEST_REQ,
    RFC_L2CA_MOVE_CHANNEL_REQ,
    RFC_L2CA_MOVE_CHANNEL_RSP,

    RFC_INIT_CFM = RFCOMM_UP_PRIM,
    RFC_REGISTER_CFM,
    RFC_UNREGISTER_CFM,
    RFC_FC_IND,
    RFC_FC_CFM,
    RFC_CLIENT_CONNECT_CFM,
    RFC_SERVER_CONNECT_IND,
    RFC_SERVER_CONNECT_CFM,
    RFC_DISCONNECT_IND,
    RFC_DISCONNECT_CFM,
    RFC_DATAWRITE_CFM,
    RFC_DATAREAD_IND,
    RFC_PORTNEG_CFM,
    RFC_PORTNEG_IND,
    RFC_MODEM_STATUS_CFM,
    RFC_MODEM_STATUS_IND,
    RFC_LINESTATUS_IND,
    RFC_LINESTATUS_CFM,
    RFC_TEST_CFM,
    RFC_NSC_IND,
    RFC_ERROR_IND,
    RFC_L2CA_MOVE_CHANNEL_CFM,
    RFC_L2CA_MOVE_CHANNEL_CMP_IND,
    RFC_L2CA_MOVE_CHANNEL_IND,
    RFC_L2CA_AMP_LINK_LOSS_IND

#ifndef BUILD_FOR_HOST
} RFC_PRIM_T; /* autogen_makefile_ignore_this */

#else
} RFC_PRIMITIVE_T; /* autogen_makefile_ignore_this */

typedef uint16_t RFC_PRIM_T;

#endif 


/*============================================================================*
Public Data Types
*============================================================================*/


/*----------------------------------------------------------------------------*
other parameter typedefs
*----------------------------------------------------------------------------*/
typedef uint8_t   RFC_LINE_STATUS_T;

typedef enum
{
    RFC_FC_ON = 0xA1,
    RFC_FC_OFF = 0x61

} RFC_FC_T; /* autogen_makefile_ignore_this */

/*----------------------------------------------------------------------------*
  RFCOMM externally visible status codes.
*----------------------------------------------------------------------------*/
#define RFC_RES_TABLE \
    RFC_RES_TABLE_X(RFC_SUCCESS,                    = 0)                        RFC_RES_TABLE_SEP \
    RFC_RES_TABLE_X(RFC_ACCEPT_SERVER_CONNECTION,   = RFCOMM_ERRORCODE_BASE)    RFC_RES_TABLE_SEP \
    RFC_RES_TABLE_X(RFC_DECLINE_SERVER_CONNECTION,  RFC_RES_TABLE_DUMMY)        RFC_RES_TABLE_SEP \
    RFC_RES_TABLE_X(RFC_CONNECTION_PENDING,         RFC_RES_TABLE_DUMMY)        RFC_RES_TABLE_SEP \
    RFC_RES_TABLE_X(RFC_CONNECTION_REJ_SECURITY,    RFC_RES_TABLE_DUMMY)        RFC_RES_TABLE_SEP \
    RFC_RES_TABLE_X(RFC_NORMAL_DISCONNECT,          RFC_RES_TABLE_DUMMY)        RFC_RES_TABLE_SEP \
    RFC_RES_TABLE_X(RFC_ABNORMAL_DISCONNECT,        RFC_RES_TABLE_DUMMY)        RFC_RES_TABLE_SEP \
    RFC_RES_TABLE_X(RFC_REMOTE_REFUSAL,             RFC_RES_TABLE_DUMMY)        RFC_RES_TABLE_SEP \
    RFC_RES_TABLE_X(RFC_CHANNEL_ALREADY_EXISTS,     RFC_RES_TABLE_DUMMY)        RFC_RES_TABLE_SEP \
    RFC_RES_TABLE_X(RFC_INVALID_CHANNEL,            RFC_RES_TABLE_DUMMY)        RFC_RES_TABLE_SEP \
    RFC_RES_TABLE_X(RFC_UNKNOWN_PRIMITIVE,          RFC_RES_TABLE_DUMMY)        RFC_RES_TABLE_SEP \
    RFC_RES_TABLE_X(RFC_INVALID_PAYLOAD,            RFC_RES_TABLE_DUMMY)        RFC_RES_TABLE_SEP \
    RFC_RES_TABLE_X(RFC_INCONSISTENT_PARAMETERS,    RFC_RES_TABLE_DUMMY)        RFC_RES_TABLE_SEP \
    RFC_RES_TABLE_X(RFC_PEER_VIOLATED_FLOW_CONTROL, RFC_RES_TABLE_DUMMY)        RFC_RES_TABLE_SEP \
    RFC_RES_TABLE_X(RFC_RES_ACK_TIMEOUT,            RFC_RES_TABLE_DUMMY)        RFC_RES_TABLE_SEP \
    RFC_RES_TABLE_X(RFC_DATAWRITE_PENDING,          RFC_RES_TABLE_DUMMY)

#define RFC_RES_TABLE_DUMMY
#define RFC_RES_TABLE_X(a, b) a b
#define RFC_RES_TABLE_SEP ,

typedef enum
{
    RFC_RES_TABLE
} RFC_RESULT_T;

#undef RFC_RES_TABLE_DUMMY
#undef RFC_RES_TABLE_X
#undef RFC_RES_TABLE_SEP

/*----------------------------------------------------------------------------*
Primitive typedefs
*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*
* PURPOSE
*     To initialise RFCOMM from a system point of view (rather than at stack
*     start-up. It registers the protocol handle to which remote requests will
*     be sent. This call will cause RFCOMM to register itself with L2CAP using
*     the RFCOMM psm value. There is a user specifiable context which will be
*     echoed back in the corresponding CFM primitive.
*----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T type;             /* Always RFC_INIT_REQ */
    phandle_t  phandle;          /* Protocol handle */ 

} RFC_INIT_REQ_T;

/*----------------------------------------------------------------------------*
* PURPOSE
*     This event confirms initialisation of RFCOMM.
*
*----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T  type;             /* Always RFC_INIT_CFM */
    phandle_t   phandle;          /* Protocol handle */ 

} RFC_INIT_CFM_T;

/*----------------------------------------------------------------------------*
* PURPOSE
*     This event allows the application to register a protocol handle with 
*     RFCOMM. RFCOMM will return a RFC_REGISTER_CFM primitive with an
*     assigned server channel number.
*     The registered protocol handle will be used for notifying the 
*     application of the arrival of events for the given server channel.
*     There is a user specifiable context which will be echoed back in the
*     corresponding CFM primitive.
*  
*     There are currently no flag options available for this prim and thus the
*     field should be set to 0.
*  
*----------------------------------------------------------------------------*/

#define RFC_INVALID_SERV_CHANNEL   0x00

typedef struct
{
    RFC_PRIM_T type;          /* Always RFC_REGISTER_REQ */
    phandle_t  phandle;       /* Protocol handle for callback */   
    uint8_t    flags;         /* Additional options that can be set. */
    uint16_t   context;       /* User specified context */
    uint8_t    loc_serv_chan_req; /* Serv. chan num the app would like to register */

} RFC_REGISTER_REQ_T;

/*----------------------------------------------------------------------------*
* PURPOSE
*     This event will be used to indicate to the higher layer that its
*     previous registration of a protocol handle with an RFC_REGISTER_REQ
*     event had been accepted (if accept is set to True) or denied (if accept
*     is set to false). The server channel number assigned is also returned.
*
*----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T    type;             /* Always RFC_REGISTER_CFM */
    phandle_t     phandle;          /* Protocol handle */
    uint8_t       loc_serv_chan;    /* Local server channel */
    bool_t        accept;           /* Accept flag */
    uint16_t      context;          /* Echoed back from REQ */

} RFC_REGISTER_CFM_T;

/*----------------------------------------------------------------------------*
* PURPOSE
*     This event allows the application to unregister a server channel with
*     RFCOMM. RFCOMM will return a RFC_UNREGISTER_CFM with the status of the
*     request. There is a user specifiable context which will also be echoed
*     back in the corresponding CFM primitive.
*----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T  type;            /* Always RFC_UNREGISTER_REQ */
    uint8_t     loc_serv_chan;   /* The local server channel to be unregistered */

} RFC_UNREGISTER_REQ_T;

/*----------------------------------------------------------------------------*
* PURPOSE
*     This event will be used to indicate to the higher layer the status of its
*     previous unregister request for the specified server channel. 
*----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T       type;            /* Always RFC_UNREGISTER_CFM */
    phandle_t        phandle;         /* Protocol handle */
    uint8_t          loc_serv_chan;   /* The local server channel unregistered */
    RFC_RESPONSE_T   status;          /* Status of the request */
    uint16_t         context;         /* Previously stored for this channel */

} RFC_UNREGISTER_CFM_T;


/*----------------------------------------------------------------------------*
* PURPOSE
* 
* This event allows the higher layers to initiate a new DLC connection, using 
* the auto connect mechanism. 
*  
* flags usage:  Bit 0 = 0 , Use basic mode 
*               Bit 0 = 1 , Use ERTM mode
*               Bit 1 = 0 , Mode is preferred
*               Bit 1 = 1 , Mode is required
*  
* Please NOTE for on-chip applications basic mode only should be used and thus 
* these flags will be overridden.  
*----------------------------------------------------------------------------*/
#define RFC_CONNFLAG_BASIC          ((uint16_t)0x0000)
#define RFC_CONNFLAG_ERTM           ((uint16_t)0x0001)
#define RFC_CONNFLAG_REQUIRE_MODE   ((uint16_t)0x0002)

typedef struct
{
    RFC_PRIM_T  type;          /* Always RFC_CLIENT_CONNECT_REQ */
    phandle_t   phandle;       /* phandle for all messages associated with this channel */
    BD_ADDR_T   bd_addr;       /* Bluetooth address of remote device */
    uint8_t     rem_serv_chan; /* The remote server channel to connect as a client to */
    uint8_t     flags;         /* Additional options that can be set. */
    uint16_t    context;       /* Host specified context */

    /* Requested connection parameters.
    */ 
    uint16_t    client_security_chan; /* Client chan id used for validating security access */
    uint16_t    max_payload_size;
    uint8_t     priority;
    uint16_t    total_credits;
    l2ca_controller_t   remote_l2cap_control;   /*!< Remote L2CAP controller ID */
    l2ca_controller_t   local_l2cap_control;    /*!< Local L2CAP controller ID */

    uint16_t    reserved_length;      
    uint16_t    *reserved;   

    /* Initial MSC parameters. See Modem Status Request for details of modem
       and break signals. msc_timeout is in Milliseconds and is the time allowed
       for the initial (automatic) handshaking of the MSC commands immediately
       after a connection has been established and before any data is sent.
       Should the timeout fire then this implies a badly behaving peer and the
       action will be to assume the handshake has completed.
    */ 
    uint8_t    modem_signal;  
    uint8_t    break_signal;     
    uint16_t   msc_timeout;

} RFC_CLIENT_CONNECT_REQ_T;


/*----------------------------------------------------------------------------*
* PURPOSE
*
* This event notifies the higher layers that a peer device wishes to initiate a
* new DLC connection.
* 
* There are currently no flag options available for this prim and thus the
* flags field should be 0.

*----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T   type;           /* Always RFC_CONNECT_IND */
    phandle_t    phandle;        /* phandle for all messages associated with this channel */
    BD_ADDR_T    bd_addr;        /* Bluetooth address of remote device */
    uint8_t      loc_serv_chan;  /* The local server channel to which a client connection has been requested */
    uint8_t      flags;          /* Additional options that can be set. */
    uint16_t     conn_id;        /* Connection id - either stream or channel depending on flags */
    uint16_t     context;        /* Previously stored for this channel */

    /* This is required for AMP devices. */
    l2ca_controller_t  local_l2cap_control;    /*!< Local L2CAP controller ID */

} RFC_SERVER_CONNECT_IND_T;

/*----------------------------------------------------------------------------*
* PURPOSE
*
* This event allows the higher layers to specify its parameters for a new DLC
* connection initiated by the peer device. It is sent in response to a
* RFC_SERVER_CONNECT_IND.
* 
* There are currently no flag options available for this prim and thus the
* flags field should be set to 0.
* 
*----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T       type;          /* Always RFC_SERVER_CONNECT_RSP */
    uint8_t          flags;         /* Additional options that can be set. */
    uint16_t         conn_id;       /* Connection identifier. Unique whether the connection is a stream or not. */
    RFC_RESPONSE_T   response;      /* Accept or decline the connection */

    /* Requested connection parameters 
    */ 
    uint16_t    max_payload_size;
    uint8_t     priority;
    uint16_t    total_credits;
    l2ca_controller_t   remote_l2cap_control;   /*!< Remote L2CAP controller ID */
    l2ca_controller_t   local_l2cap_control;    /*!< Local L2CAP controller ID */

    /* Initial MSC parameters. See Modem Status Request for details of modem
       and break signals. msc_timeout is in Milliseconds and is the time allowed
       for the initial (automatic) handshaking of the MSC commands immediately
       after a connection has been established and before any data is sent.
       Should the timeout fire then this implies a badly behaving peer and the
       action will be to assume the handshake has completed.
    */ 
    uint8_t    modem_signal;  
    uint8_t    break_signal;     
    uint16_t   msc_timeout;

} RFC_SERVER_CONNECT_RSP_T;


/*----------------------------------------------------------------------------*
* PURPOSE
*
* This event is sent when a new DLC connection has been successfully opened.
* The actual type used will depend on whether it is a client (outbound) or
* server (inbound) connection.
* 
* flags usage:  Bit 0 = 0 , Basic mode being used
*               Bit 0 = 1 , ERTM mode being used

*----------------------------------------------------------------------------*/ 
typedef struct
{
    RFC_PRIM_T       type;          /* RFC_SERVER_CONNECT_CFM or RFC_CLIENT_CONNECT_CFM */
    phandle_t        phandle;       /* phandle for all messages associated with this channel */
    BD_ADDR_T        bd_addr;       /* Bluetooth address of remote device */
    uint8_t          serv_chan;     /* server channel connected */
    uint8_t          flags;         /* Additional options that were set. */
    uint16_t         conn_id;       /* Connection identifier. Unique whether the connection is a stream or not. */
    RFC_RESPONSE_T   status;        /* Success or failure */
    uint16_t         context;       /* Previously stored for this channel */

    uint16_t         max_payload_size;
    l2ca_controller_t   remote_l2cap_control;   /*!< Remote L2CAP controller ID */
    l2ca_controller_t   local_l2cap_control;    /*!< Local L2CAP controller ID */

} RFC_CONNECT_CFM_T; /* autogen_makefile_ignore_this */

typedef RFC_CONNECT_CFM_T RFC_SERVER_CONNECT_CFM_T;
typedef RFC_CONNECT_CFM_T RFC_CLIENT_CONNECT_CFM_T;

/*----------------------------------------------------------------------------*
* PURPOSE
*
* This event is sent when the upper layers wish to close a DLC connection.
*----------------------------------------------------------------------------*/ 
typedef struct
{
    RFC_PRIM_T       type;         /* Always RFC_DISCONNECT_REQ */
    uint16_t         conn_id;      /* Connection id - either stream or channel depending on flags */
} RFC_DISCONNECT_REQ_T;

/*----------------------------------------------------------------------------*
* PURPOSE
*
* This event is sent by the upper layers, in response to a RFC_DISCONNECT_IND,
* once they have finished with the connection.
*----------------------------------------------------------------------------*/ 
typedef RFC_DISCONNECT_REQ_T RFC_DISCONNECT_RSP_T;

/*----------------------------------------------------------------------------*
* PURPOSE
*
* This event notifies the upper layers that the a DLC connection has been
* closed down remotely, either by request from the peer or by the L2CAP channel
* going down.
* ----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T       type;        /* Always RFC_DISCONNECT_IND */
    phandle_t        phandle;     /* phandle for all messages associated with this channel */
    uint16_t         conn_id;     /* Connection id - either stream or channel depending on flags */
    RFC_RESPONSE_T   reason;      /* Reason for disconnection (other end terminated, link loss, etc... */

} RFC_DISCONNECT_IND_T;


/*----------------------------------------------------------------------------*
* PURPOSE
*
* This event is sent to the upper layers in response to a RFC_DISCONNECT_REQ.
*----------------------------------------------------------------------------*/ 
typedef struct
{
    RFC_PRIM_T       type;         /* Always RFC_DISCONNECT_CFM */
    phandle_t        phandle;      /* phandle for all messages associated with this channel */
    uint16_t         conn_id;      /* Connection id - either stream or channel depending on flags */
    RFC_RESPONSE_T   status;       /* Status of the request E.g. Success etc */

} RFC_DISCONNECT_CFM_T;


/*===========================================================================*
 Port Emulation Services

    These primitives are used to communicate the port parameters - port speed,
    data bits, stop bits, parity.

        Local Device               Remote Device
    Initiator:
         ------------RFC_PORTNEG_REQ------------>
        <------------RFC_PORTNEG_CFM------------
    Responder:
        <------------RFC_PORTNEG_IND------------
         ------------RFC_PORTNEG_RES------------>

*===========================================================================*/

#define RFC_2400_BAUD        0x00
#define RFC_4800_BAUD        0x01
#define RFC_7200_BAUD        0x02
#define RFC_9600_BAUD        0x03
#define RFC_19200_BAUD       0x04
#define RFC_38400_BAUD       0x05
#define RFC_57600_BAUD       0x06
#define RFC_115200_BAUD      0x07
#define RFC_230400_BAUD      0x08
#define RFC_UNKNOWN_BAUD     0xFF

#define RFC_DATA_BITS_5          0x00       /* 5 data bits */
#define RFC_DATA_BITS_6          0x02       /* 6 data bits */
#define RFC_DATA_BITS_7          0x01       /* 7 data bits */
#define RFC_DATA_BITS_8          0x03       /* 8 data bits */

/* Stop bits for RPN primitives */
#define RFC_STOP_BITS_ONE             0x00  /* 1 stop bit    */
#define RFC_STOP_BITS_ONE_AND_A_HALF  0x01  /* 1.5 stop bits */

/* Parity and Parity type indicators for RPN primitives */
#define RFC_PARITY_OFF           0x00
#define RFC_PARITY_ON            0x01

/* Parity types when parity on */
#define RFC_PARITY_TYPE_ODD      0x00
#define RFC_PARITY_TYPE_MARK     0x01
#define RFC_PARITY_TYPE_EVEN     0x02
#define RFC_PARITY_TYPE_SPACE    0x03

/* Flow Control Mask bits */
#define RFC_FLC_XONXOFF_INPUT    0x01
#define RFC_FLC_XONXOFF_OUTPUT   0x02
#define RFC_FLC_RTR_INPUT        0x04
#define RFC_FLC_RTR_OUTPUT       0x08
#define RFC_FLC_RTC_INPUT        0x10
#define RFC_FLC_RTC_OUTPUT       0x20

/* XON and XOFF characters */
#define RFC_XON_CHAR_DEFAULT     0x11  /* DC1 character */
#define RFC_XOFF_CHAR_DEFAULT    0x13  /* DC3 character */

/* Parameter mask bits */
#define RFC_PM_BIT_RATE          0x0001
#define RFC_PM_DATA_BITS         0x0002
#define RFC_PM_STOP_BITS         0x0004
#define RFC_PM_PARITY            0x0008
#define RFC_PM_PARITY_TYPE       0x0010
#define RFC_PM_XON               0x0020
#define RFC_PM_XOFF              0x0040
#define RFC_PM_XONXOFF_INPUT     0x0100
#define RFC_PM_XONXOFF_OUTPUT    0x0200
#define RFC_PM_RTR_INPUT         0x0400
#define RFC_PM_RTR_OUTPUT        0x0800
#define RFC_PM_RTC_INPUT         0x1000
#define RFC_PM_RTC_OUTPUT        0x2000

typedef struct
{
    uint8_t  baud_rate;       /* port speed indicator - see #defines above */
    uint8_t  data_bits;        /* DATA_BITS_5, _6, _7 or _8 - see above */
    uint8_t  stop_bits;        /* STOP_BITS_ONE or _ONE_AND_A_HALF - see above */
    uint8_t  parity;           /* PARITY_OFF or PARITY_ON */
    uint8_t  parity_type;      /* PARITY_TYPE_ODD, _EVEN, _MARK or _SPACE */
    uint8_t  flow_ctrl_mask;   /* 6 bits - use FLC_ #defines above (see 07.10) */
    uint8_t  xon;              /* xon character  (default DC1) */
    uint8_t  xoff;             /* xoff character (default DC3) */
    uint16_t parameter_mask;   /* 16 bits (top two reserved) see PM_ #defines */
} RFC_PORTNEG_VALUES_T;  /* autogen_makefile_ignore_this */



/*----------------------------------------------------------------------------*
 * PURPOSE
 *     This command indicates to the remote port entity that the local
 *     port entity requests it to set port parameters for the
 *     
 *----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T            type;        /* Always RFC_PORTNEG_REQ */
    uint16_t              conn_id;     /* Connection id - either stream or channel depending on flags */
    bool_t                request;     /* Request for remote parameters */
    RFC_PORTNEG_VALUES_T  port_pars;   /* Port Parameters */

} RFC_PORTNEG_REQ_T;


/*----------------------------------------------------------------------------*
 * PURPOSE
 *     This event is the confirmation of the locally-initiated port negotiation
 *     procedure. The contents of the port parameters will be the same as those
 *     in the original request, except where the corresponding bit in the
 *     parameter_mask is set to zero, in which case that parameter is set to
 *     an alternative value proposed by the remote device.
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T            type;        /* Always RFC_PORTNEG_CFM */
    phandle_t             phandle;     /* Routing phandle */
    uint16_t              conn_id;     /* Connection id - either stream or channel depending on flags */
    RFC_PORTNEG_VALUES_T  port_pars;   /* Port Parameters */
    RFC_RESPONSE_T        status;      /* Status of the request E.g. Success etc */

} RFC_PORTNEG_CFM_T;


/*----------------------------------------------------------------------------*
 * PURPOSE
 *     This event is indicates to the higher port entity that the remote
 *     port entity requests it to set port parameters for the DLCI as given in
 *     the Port Parameters. The port entity should respond with RFC_PORTNEG_RES.
 *
 *     If request is true, the indication shows the values of the port at the
 *     remote port entity.
 *----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T      type;             /* Always RFC_PORTNEG_IND */
    phandle_t       phandle;          /* Routing phandle */
    uint16_t        conn_id;          /* Connection id - either stream or channel depending on flags */
    bool_t          request;          /* a request (See TS07.10 5.4.3.6.9) */
    RFC_PORTNEG_VALUES_T  port_pars;  /* Port Parameters */

} RFC_PORTNEG_IND_T;


/*----------------------------------------------------------------------------*
 * PURPOSE
 *      This event is the response to the previous RFC_PORTNEG_REQ event. The
 *      responding port entity must set the 'port_pars.parameter_mask' bits to
 *      '1' to indicate which new parameters have been accepted, or to '0' to
 *      indicate any which are rejected.
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T      type;             /* Always RFC_PORTNEG_RES */
    uint16_t        conn_id;          /* Connection id - either stream or channel depending on flags */
    RFC_PORTNEG_VALUES_T  port_pars;   /* Port Parameters */

} RFC_PORTNEG_RSP_T;



/*===========================================================================*
Modem Status 

A modem status command is used to convey control parameters for a specific 
dlci such as such as flow control, Modem signals, Break,  between Port 
Emulation Entities. The modem signal is compulsory however the break signal is
optional. The modem status command itself MUST be sent after opening a DLC but
prior to sending any data on that DLC. It is the host's responsibility to 
ensure that it sends its own MSC request before commencing data transfer. 

    Local Device               Remote Device
     ------------RFC_MODEM_STATUS_REQ------------>
    <------------RFC_MODEM_STATUS_CFM------------ 
 
    <------------RFC_MODEM_STATUS_IND------------
 
*===========================================================================*/

/* Modem signal bits. Or'd together.
*/ 
#define RFC_MSC_FC_BIT     0x02
#define RFC_MSC_RTC_BIT    0x04
#define RFC_MSC_RTR_BIT    0x08
#define RFC_MSC_IC_BIT     0x40
#define RFC_MSC_DV_BIT     0x80

/*----------------------------------------------------------------------------*
* PURPOSE
*     This event is used by application to convey control 
*     information to the remote port (including flow control on a server
*     channel). 
*
*----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T type;             /* Always RFC_MODEM_STATUS_REQ */
    uint16_t   conn_id;          /* Connection id - either stream or channel depending on flags */
    uint8_t    modem_signal;     /* modem signal - see above */

    /* Bit 0: 1 if break signal encoded, 0 otherwise.
       Bits 1-2: Reserved.
       Bits 3-6: break value 0x0 - 0xF.
    */
    uint8_t    break_signal;     

} RFC_MODEM_STATUS_REQ_T;


/*----------------------------------------------------------------------------*
* PURPOSE
*     This event indicates to the port entity that the remote port emulation
*     entity wishes to use the control parameters provided.
*
*----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T type;          /* Always RFC_MODEM_STATUS_IND */
    phandle_t  phandle;       /* Routing phandle */
    uint16_t   conn_id;       /* Connection id - either stream or channel depending on flags */
    uint8_t    modem_signal;  /* modem signal - see above */

    /* Bit 0: 1 if break signal encoded, 0 otherwise.
       Bits 1-2: Reserved.
       Bits 3-6: break value 0x0 - 0xF.
    */
    uint8_t    break_signal;  

} RFC_MODEM_STATUS_IND_T;

/*----------------------------------------------------------------------------*
* PURPOSE
*     This event notifies the port entity the status of the modem status
*     command.
*
*----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T      type;          /* Always RFC_MODEM_STATUS_CFM */
    phandle_t       phandle;       /* Routing phandle */
    uint16_t        conn_id;       /* Connection id - either stream or channel depending on flags */
    RFC_RESPONSE_T  status;        /* Result of the command */

} RFC_MODEM_STATUS_CFM_T;

/*===========================================================================*
Port Flow control services (aggregate).

This primitive is used to communicate the flow control for all connections on
the RFCOMM multiplexer. 

This mechanism should not be used if the DLC has been established with credit 
based flow_ctrl enabled. NB for all client connections (ie outbound) , credit 
based flow control is enabled by default! The service is retained for 
backwards compatability with v1.0b of the Bluetooth specification. 

    Local Device               Remote Device
    <--Data arrives from remote port------
    -------------RFC_FC_REQ (OFF) ------->
    <------------RFC_FC_CFM (OFF) -------- 
    ---------remote device stops sending--
    -------------RFC_FC_REQ (ON) -------->
    <------------RFC_FC_CFM (ON) ---------
    <--Data arrives from remote port------

    ---Sending data to remote port------->
    <------------RFC_FC_IND (OFF) -------- 
    ---Stop sending data------------------
    <------------RFC_FC_IND (ON) ---------
    --Sending data to remote port-------->
    
*===========================================================================*/

/*----------------------------------------------------------------------------*
* PURPOSE
*     This event is used to handle aggregate flow.
* 
*     RFC_FC_OFF: Indicates that the local calling entity is not able to
*     receive new information. The opposite entity is not allowed to transmit
*     frames except on the control channel.
* 
*     RFC_FC_ON: Indicates that the local calling entity is able to receive new
*     information
*----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T       type;          /* Always RFC_FC_REQ */
    BD_ADDR_T        bd_addr;       /* Bluetooth address of remote device */
    RFC_FC_T         fc;            /* On or Off */
} RFC_FC_REQ_T;

/*----------------------------------------------------------------------------*
* PURPOSE
*     This event is used to report the status of the FC Req command
*
*----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T  type;          /* Always RFC_FC_CFM */
    phandle_t   phandle;       /* Routing phandle */
    BD_ADDR_T   bd_addr;       /* Bluetooth address of remote device */
    RFC_FC_T    fc;            /* On or Off */
    RFC_RESPONSE_T  status;    /* Result of the command */
} RFC_FC_CFM_T;


/*----------------------------------------------------------------------------*
* PURPOSE
*     This event is used to handle aggregate flow.
* 
*     RFC_FC_OFF: Indicates that the remote entity is not able to receive new
*     information. The local entity is not allowed to transmit frames except on
*     the control channel.
* 
*     RFC_FC_ON: Indicates that the remote entity is able to receive new
*     information.
*----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T       type;          /* Always RFC_FC_IND */
    phandle_t        phandle;       /* Routing phandle */
    BD_ADDR_T        bd_addr;       /* Bluetooth address of remote device */
    RFC_FC_T         fc;            /* On or Off */
} RFC_FC_IND_T;



/*----------------------------------------------------------------------------*
* PURPOSE
*     This event is a request to RFCOMM to transmit the data given in the 
*     payload to the remote address along the specified connection. The payload
*     will be transmitted in the form of UIH frames. The maximum frame size
*     will either be the default, or negotiated during mux startup or parameter
*     negotiation. This primitive will result in an RFC_DATAWRITE_CFM.
*
*----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T     type;             /* Always RFC_DATAWRITE_REQ */
    uint16_t       conn_id;          /* Connection id */
    uint16_t       payload_length;   /* Length of the payload */
    MBLK_T         *payload;         /* Pointer to the data */

    /* Normally the host would be expected to set rx_credits to 0, however by
       having this field it does allow them to increase the number of credits
       available to the peer for this channel.
    */ 
    uint16_t       rx_credits;       

} RFC_DATAWRITE_REQ_T;


/*----------------------------------------------------------------------------*
* PURPOSE
*     This event indicates that the previous RFC_DATAWRITE_REQ was successfully
*     sent from L2CAP. 
*
*----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T      type;      /* Always RFC_DATAWRITE_CFM */
    phandle_t       phandle;   /* Routing phandle */
    uint16_t        conn_id;   /* Connection id */
    RFC_RESPONSE_T  status;    /* Result of the command */

} RFC_DATAWRITE_CFM_T;


/*----------------------------------------------------------------------------*
* PURPOSE
*     This event indicates to the higher layers that data has arrived from the
*     remote device and is stored in the location given by payload.
*
*       NOTE:   To minimise copy operations within RFCOMM, 'payload' points
*               to the first byte of RFCOMM payload data within the L2CAP packet
*               supplied. When the payload has been processed, the recipient
*               must release the data.
*               
*----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T    type;             /* Always RFC_DATA_IND */
    phandle_t     phandle;          /* Routing phandle */
    uint16_t      conn_id;          /* Connection id - either stream or channel depending on flags */
    uint16_t      payload_length;   /* Length of the payload */
    MBLK_T        *payload;         /* Pointer to the data */

} RFC_DATAREAD_IND_T;

/*----------------------------------------------------------------------------*
* PURPOSE
*               
*     This event is sent by the higher layers to acknowledge receipt of a data
*     packet.
*----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T   type;        /* Always RFC_DATAREAD_RSP */
    uint16_t     conn_id;     /* Connection id - either stream or channel depending on flags */

} RFC_DATAREAD_RSP_T;


/*===========================================================================*
DLC Line status indication service

These primitives are used to indicate a DLC's line status.
 
error flag : 1 = error has occurred (described in line_status) 
             0 = no error
 
line_status: 0x1 = Overrun Error - Received character overwrote an 
                   unread characte
             0x2 = Parity Error - Received character's parity was
                   incorrect
             0x4 = Framing Error - a character did not terminate
                   with a stop bit

    Local Device               Remote Device
     ---------RFC_LINESTATUS_REQ----------->
    <---------RFC_LINESTATUS_CFM-----------
 
    <---------RFC_LINESTATUS_IND-----------
 
*===========================================================================*/

#define RFC_OVERRUN_ERROR  0x1
#define RFC_PARITY_ERROR   0x2
#define RFC_FRAMING_ERROR  0x4

/*----------------------------------------------------------------------------*
* PURPOSE
*     This primitive is used to send the line status to the remote port entity.
*
*----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T    type;           /* Always RFC_LINESTATUS_REQ */
    uint16_t      conn_id;        /* Connection id */
    bool_t        error_flag;     /* error flag - TRUE if error */
    RFC_LINE_STATUS_T line_status;    /* Line Status Parameter */

} RFC_LINESTATUS_REQ_T;


/*----------------------------------------------------------------------------*
* PURPOSE
*     This event indicates the line status of the remote port entity.
*     There is no response.
*
*----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T    type;          /* Always RFC_LINESTATUS_IND */
    phandle_t     phandle;       /* Routing phandle */
    uint16_t      conn_id;       /* Connection id */
    bool_t        error_flag;    /* error flag - TRUE if error */
    RFC_LINE_STATUS_T line_status;   /* Line Status Parameter */

} RFC_LINESTATUS_IND_T;

/*----------------------------------------------------------------------------*
* PURPOSE
*     This event notifies the port entity the status of the line status
*     command.
*
*----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T type;          /* Always RFC_LINESTATUS_CFM */
    phandle_t  phandle;       /* Routing phandle */
    uint16_t   conn_id;       /* Connection id */
    RFC_RESPONSE_T  status;   /* Result of the command */

} RFC_LINESTATUS_CFM_T;

/*===========================================================================*
Test service

These primitives are used to test the communication link between two 
RFCOMM entities.

    Local Device               Remote Device
     ------------RFC_TEST_REQ-------------->
    <------------RFC_TEST_CFM--------------

*===========================================================================*/

/*----------------------------------------------------------------------------*
* PURPOSE
*     This event requests a test of the communication link. The test data is
*     sent to the remote RFCOMM on the control channel which shall 
*     automatically loop it back. If the data arrives back then a RFC_TEST_CFM 
*     event shall be sent to the application.    
*
*----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T type;             /* Always RFC_TEST_REQ */
    BD_ADDR_T  bd_addr;          /* Bluetooth address of remote device */
    uint16_t   test_data_length; /* Length of the test data */
    MBLK_T     *test_data;       /* Pointer to the data */

} RFC_TEST_REQ_T;


/*----------------------------------------------------------------------------*
* PURPOSE
*     This event is confirmation of a previously sent RFC_TEST_REQ event. The
*     contents of test_data will be that which has been looped back from the
*     remote RFCOMM.
*
*----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T      type;             /* Always RFC_TEST_CFM */
    phandle_t       phandle;          /* Routing phandle */
    BD_ADDR_T       bd_addr;          /* Bluetooth address of remote device */
    uint16_t        test_data_length; /* Length of the test data */
    MBLK_T          *test_data;       /* Pointer to the data */
    RFC_RESPONSE_T  status;           /* Result of the test request */

} RFC_TEST_CFM_T;


/*===========================================================================*
Non Supported Command Response (NSC)

This primitive is sent to the application whenever a command type is not 
supported by the receiving entity.

    Local Device               Remote Device
     ----------Any _REQ or _RES------------>
    <------------RFC_NSC_IND---------------

*===========================================================================*/

/*----------------------------------------------------------------------------*
* PURPOSE
*      Non Supported Command Response. See TS07.10 5.4.6.3.8. Indicates that
*      the remote device did not understand the last frame sent. NB the
*      command_type given is the RFCOMM command type for the command, not one
*      of the RFC_PRIM_T 'type' enumerations.
*----------------------------------------------------------------------------*/
typedef struct
{
    RFC_PRIM_T type;             /* Always RFC_NSC_IND */
    phandle_t  phandle;          /* Routing phandle */ 
    BD_ADDR_T  bd_addr;          /* Bluetooth address of remote device */
    uint8_t    command_type;     /* NSC command type */
    uint8_t    cmd_res;          /* C/R bit value */
} RFC_NSC_IND_T;

/*-------------------------------------------------------------------------
* PURPOSE
*     This primitive is used to report a generic error caused by a specific
*     primitive.
*-------------------------------------------------------------------------*/ 
typedef struct
{
    RFC_PRIM_T type;           /* Always RFC_ERROR_IND */
    phandle_t  phandle;        /* Routing phandle */ 
    RFC_PRIM_T err_prim_type;  /* Type of the prim causing the error */
    RFC_RESPONSE_T  status;    /* Actual error */

} RFC_ERROR_IND_T;


/*-------------------------------------------------------------------------
* PURPOSE
*     This primitive is used to support an L2CAP move channel request, via
*     rfcomm, issued by the host.
*-------------------------------------------------------------------------*/ 
typedef struct
{
    RFC_PRIM_T          type;             /* Always RFC_L2CA_MOVE_CHANNEL_REQ */
    BD_ADDR_T           bd_addr;          /* Bluetooth address of remote device 
                                             for which the l2cap channel will be 
                                             moved */
    l2ca_controller_t   remote_control;   /* Remote controller ID */
    l2ca_controller_t   local_control;    /* Local controller ID */
} RFC_L2CA_MOVE_CHANNEL_REQ_T;

/*-------------------------------------------------------------------------
* PURPOSE
*     This primitive is used to support an L2CAP move channel confirmation, via
*     rfcomm, issued to the host. It is sent in response to the equivalent L2CAP
*     move channel request.
*-------------------------------------------------------------------------*/ 
typedef struct
{
    RFC_PRIM_T          type;             /* Always RFC_L2CA_MOVE_CHANNEL_CFM */
    phandle_t           phandle;          /* Destination phandle */
    BD_ADDR_T           bd_addr;          /* Bluetooth address of remote device 
                                             for which the l2cap channel will be 
                                             moved */
    l2ca_controller_t   local_control;    /* Local controller ID actually in use */
    l2ca_controller_t   remote_control;   /* Remote controller ID acutally in use */
    RFC_RESPONSE_T      status;           /* Result of the move channel request */
} RFC_L2CA_MOVE_CHANNEL_CFM_T;

/*-------------------------------------------------------------------------
* PURPOSE
*     This primitive is used to support an L2CAP move channel confirmation, via
*     rfcomm, issued to the host. It is sent in response when a move channel
*     request issued by the peer, has completed.
*-------------------------------------------------------------------------*/ 
typedef struct
{
    RFC_PRIM_T          type;             /* Always RFC_L2CA_MOVE_CHANNEL_CMP_IND */
    phandle_t           phandle;          /* Destination phandle */
    BD_ADDR_T           bd_addr;          /* Bluetooth address of remote device 
                                             for which the l2cap channel will be 
                                             moved */
    l2ca_controller_t   local_control;    /* Local controller ID actually in use */
    RFC_RESPONSE_T      status;           /* Result of the move channel request */
} RFC_L2CA_MOVE_CHANNEL_CMP_IND_T;

/*-------------------------------------------------------------------------
* PURPOSE
*     This primitive is used to support an L2CAP move channel indication, via
*     rfcomm, issued to the host. It is sent when a move channel request is
*     issued by the peer.
*-------------------------------------------------------------------------*/ 
typedef struct
{
    RFC_PRIM_T          type;             /* Always RFC_L2CA_MOVE_CHANNEL_IND */
    phandle_t           phandle;          /* Destination phandle */
    BD_ADDR_T           bd_addr;          /* Bluetooth address of remote device 
                                             for which the l2cap channel will be 
                                             moved */
    l2ca_identifier_t   identifier;       /* Used to identify the move signal */
    l2ca_controller_t   local_control;    /* Local controller ID */
} RFC_L2CA_MOVE_CHANNEL_IND_T;

/*-------------------------------------------------------------------------
* PURPOSE
*     This primitive is used to support an L2CAP move channel response, via
*     rfcomm, issued by the host in response to a move channel request issued by
*     the peer.
*-------------------------------------------------------------------------*/ 
typedef struct
{
    RFC_PRIM_T          type;             /* Always RFC_L2CA_MOVE_CHANNEL_RSP */
    BD_ADDR_T           bd_addr;          /* Bluetooth address of remote device 
                                             for which the l2cap channel will be 
                                             moved */
    l2ca_identifier_t   identifier;       /* Used to identify the move signal */
    RFC_RESPONSE_T      status;           /* Result of the move channel indication */
} RFC_L2CA_MOVE_CHANNEL_RSP_T;


/*-------------------------------------------------------------------------
* PURPOSE
*     This primitive is used to support an L2CAP AMP link loss indication, via
*     rfcomm, to the host
*-------------------------------------------------------------------------*/ 
typedef struct
{
    RFC_PRIM_T          type;             /* Always RFC_L2CA_AMP_LINK_LOSS_IND */
    phandle_t           phandle;          /* Destination phandle */
    BD_ADDR_T           bd_addr;          /* Bluetooth address of remote device 
                                             for which there is notification of
                                             AMP link loss */
    RFC_RESPONSE_T      reason;           /* Reason for AMP link loss */

} RFC_L2CA_AMP_LINK_LOSS_IND_T;

/*----------------------------------------------------------------------------*
* PURPOSE
*     Union of the primitives
*
*----------------------------------------------------------------------------*/
typedef union
{
    RFC_PRIM_T                       type;
    RFC_INIT_REQ_T                   rfc_init_req;    
    RFC_INIT_CFM_T                   rfc_init_cfm;    
    RFC_REGISTER_REQ_T               rfc_register_req;    
    RFC_REGISTER_CFM_T               rfc_register_cfm;   
    RFC_UNREGISTER_REQ_T             rfc_unregister_req;
    RFC_UNREGISTER_CFM_T             rfc_unregister_cfm;
    RFC_CLIENT_CONNECT_REQ_T         rfc_client_connect_req;
    RFC_CLIENT_CONNECT_CFM_T         rfc_client_connect_cfm;
    RFC_SERVER_CONNECT_RSP_T         rfc_server_connect_rsp;
    RFC_SERVER_CONNECT_IND_T         rfc_server_connect_ind;
    RFC_SERVER_CONNECT_CFM_T         rfc_server_connect_cfm;
    RFC_DISCONNECT_REQ_T             rfc_disconnect_req;
    RFC_DISCONNECT_CFM_T             rfc_disconnect_cfm;
    RFC_DISCONNECT_IND_T             rfc_disconnect_ind;
    RFC_DISCONNECT_RSP_T             rfc_disconnect_rsp;
    RFC_PORTNEG_REQ_T                rfc_portneg_req;
    RFC_PORTNEG_CFM_T                rfc_portneg_cfm;
    RFC_PORTNEG_IND_T                rfc_portneg_ind;
    RFC_PORTNEG_RSP_T                rfc_portneg_rsp;
    RFC_FC_REQ_T                     rfc_fc_req;    
    RFC_FC_IND_T                     rfc_fc_ind;
    RFC_FC_CFM_T                     rfc_fc_cfm;
    RFC_DATAWRITE_REQ_T              rfc_datawrite_req;    
    RFC_DATAWRITE_CFM_T              rfc_datawrite_cfm;    
    RFC_MODEM_STATUS_REQ_T           rfc_modem_status_req;
    RFC_MODEM_STATUS_IND_T           rfc_modem_status_ind;
    RFC_MODEM_STATUS_CFM_T           rfc_modem_status_cfm;
    RFC_DATAREAD_IND_T               rfc_dataread_ind;
    RFC_DATAREAD_RSP_T               rfc_dataread_rsp;
    RFC_LINESTATUS_REQ_T             rfc_linestatus_req;    
    RFC_LINESTATUS_IND_T             rfc_linestatus_ind; 
    RFC_LINESTATUS_CFM_T             rfc_linestatus_cfm; 
    RFC_TEST_REQ_T                   rfc_test_req;    
    RFC_TEST_CFM_T                   rfc_test_cfm;
    RFC_NSC_IND_T                    rfc_nsc_ind;
    RFC_ERROR_IND_T                  rfc_error_ind;

    RFC_L2CA_MOVE_CHANNEL_REQ_T      rfc_l2ca_move_req;
    RFC_L2CA_MOVE_CHANNEL_CFM_T      rfc_l2ca_move_cfm;
    RFC_L2CA_MOVE_CHANNEL_CMP_IND_T  rfc_l2ca_move_cmp_ind;
    RFC_L2CA_MOVE_CHANNEL_IND_T      rfc_l2ca_move_ind;
    RFC_L2CA_MOVE_CHANNEL_RSP_T      rfc_l2ca_move_rsp;
    RFC_L2CA_AMP_LINK_LOSS_IND_T     rfc_l2ca_amp_link_loss_ind;

} RFCOMM_UPRIM_T;/* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */ 

#ifdef __cplusplus
}
#endif 

#endif /* Endif _RFCOMM_PRIM_H */
