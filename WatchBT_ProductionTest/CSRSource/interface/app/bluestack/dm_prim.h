/*******************************************************************************
 
    (C) COPYRIGHT Cambridge Consultants Ltd 1998

    CCL Project Reference C6066 - Lindisfarne

FILE
        $Workfile: dm_prim.h $
        $Revision: #1 $
    

ORIGINAL AUTHOR
        Chris Swannack

CONTAINS
        Device Manager Interface Primitives.

DESCRIPTION
    Provides the top interface to the Device Manager.
    The top interface will be split into three entities:
    1) An ACL Interface for setting up ACL connections
    2) A SYNC Interface for setting up SCO/eSCO connections
    3) A Management Interface for all other HCI / Device Manager commands

    NOTE: Since many of the Device manager commands map exactly on to HCI 
    commands, the HCI structures have been reused for the DM Interface. Where
    the command does not exist in HCI or where the expected parameters are 
    different, specific structures have been redefined.

    **************************************************************************
    *WARNING: THIS FILE IS NOT JUST C
    **************************************************************************
    *IT IS PARSED BY devHost/HCIPacker/Autogen/Makefile
    *Any lines that look like one of the following will be assumed to contain
    *primitive names, which are extracted so display routines can be generated
    *for them:
    *
    *    } SOME_KIND_OF_NAME;            
    *    typedef FOO ANOTHER_NAME; 
    *
    *If you write one of these lines and it's not a primitive, include the 
    *magic words:
    *
    *    autogen_makefile_ignore_this 
    *
    *in a comment on the same line, so it can be ignored.
    **************************************************************************
    
NOTES/LIMITATIONS
    

REFERENCES
    c6066-s-001 Bluetooth Software Architecture
    c6066-s-002 Link Manager Subsystem design
    c6066-s-003 L2CAP Subsystem design
    c6066-s-004 Device Manager Subsystem design
  

*******************************************************************************/
#ifndef BLUESTACK__DM_PRIM_H
#define BLUESTACK__DM_PRIM_H

#include "hci.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="dm_prim_t" */
/* conversion_rule="ADD_UNDERSCORE_AND_UPPERCASE_T" */

/*============================================================================*
                    Header files on which this one depends
 *============================================================================*/

/* None */

/*============================================================================*
                       Interface #defined parameters
 *============================================================================*/

#define DEFAULT_ACL_PACKET_TYPE (HCI_PKT_DM1 | HCI_PKT_DH1 | HCI_PKT_DM3 | HCI_PKT_DH3 | HCI_PKT_DM5 | HCI_PKT_DH5)
#define DEFAULT_SCO_PACKET_TYPE (HCI_PKT_HV3)

/* dm_prim_t allocation
 *
 * 0x0000 -> 0x0100 HCI events
 * 0x0100 -> 0x0180 RESERVED
 * 0x0180 -> 0x0200 DM_INTERNAL (RESERVED for internal DM communication)
 * 0x0200 -> 0x0280 DM_AMPM
 * 0x0280 -> 0x0300 DM_SYNC
 * 0x0300 -> 0x0380 DM_PRIV
 * 0x0380 -> 0x0400 DM_SM
 * 0x0400 ->        HCI commands 
 * (but bit 10 - 0x0200 - which is the top bit of the OGC is used to indicate
 * whether the command requires a connection handle to be filled in by the DM)
 */

/* send_to_hci_with_handle */
#define DM_HCI_WITH_HANDLE          0x0200
#define DM_HCI_OP_CODE_MASK         (~DM_HCI_WITH_HANDLE)

/* DM_HCI_ types */
#define DM_HCI_EVENT_PRIM           0x0010
#define DM_LC_PRIM                  HCI_LINK
#define DM_LP_PRIM                  HCI_POLICY
#define DM_BB_PRIM                  HCI_HOST_BB
#define DM_INF_PRIM                 HCI_INFO
#define DM_STATUS_PRIM              HCI_STATUS
#define DM_TEST_PRIM                HCI_TEST
#define DM_ULP_PRIM                 HCI_ULP
#define DM_OGF_MASK                 0xFC00
#define DM_HCI_COMMANDS             DM_LC_PRIM

/* DM types used internally - not exported to the application. */
#define DM_INTERNAL_DOWN_PRIM       0x0180
#define DM_INTERNAL_UP_PRIM         0x01C0
#define DM_INERNAL_PRIM             DM_INTERNAL_DOWN_PRIM
/* Native DM types - not DM_HCI_ */
/* AMPM interface to DM */
#define DM_AMPM_DOWN_PRIM           0x0200
#define DM_AMPM_UP_PRIM             0x0240
#define DM_AMPM_PRIM                DM_AMPM_DOWN_PRIM
/* SCO primitives */
#define DM_SYNC_DOWN_PRIM           0x0280
#define DM_SYNC_UP_PRIM             0x02C0
#define DM_SYNC_PRIM                DM_SYNC_DOWN_PRIM
/* Commands specific to this Device Manager */
#define DM_PRIV_DOWN_PRIM           0x0300
#define DM_PRIV_UP_PRIM             0x0340
#define DM_PRIV_PRIM                DM_PRIV_DOWN_PRIM
/* Security manager primitives */
#define DM_SM_DOWN_PRIM             0x0380
#define DM_SM_UP_PRIM               0x03C0
#define DM_SM_PRIM                  DM_SM_DOWN_PRIM

/* 64 primitives is enough for anyone... */
#define DM_TYPE_MASK                0x003F
#define DM_GROUP_MASK               0xFFC0

/* First add your primitive here... */
typedef enum dm_prim_tag
{
    /* DM_AM_REGISTER_REQ for old API.
      
       This is just to help catch an application sending DM_AM_REGISTER_REQ
       using the wrong API. If you do this from the VM (which should be
       quite difficult) then the firmware will panic. If you do it via an
       external physical interface (setting the top bit of the phandle) then
       the firmware will reply with INVALID_PHANDLE.*/
    ENUM_DM_AM_REGISTER_WRONG_API_REQ = 0,
    ENUM_DM_AM_REGISTER_WRONG_API_CFM = 1,

    /*-------------------------------------------------------------
      Device Manager private primitives
      Included here are also special DM_HCI commands that the DM
      must intercept before sending them accross HCI.
      ---------------------------------------------------------------*/
    ENUM_DM_AM_REGISTER_REQ = DM_PRIV_DOWN_PRIM,
    ENUM_DM_SET_BT_VERSION_REQ,
    ENUM_DM_WRITE_CACHED_PAGE_MODE_REQ,
    ENUM_DM_WRITE_CACHED_CLOCK_OFFSET_REQ,
    ENUM_DM_CLEAR_PARAM_CACHE_REQ,
    ENUM_DM_LP_WRITE_POWERSTATES_REQ,
    ENUM_DM_SET_DEFAULT_LINK_POLICY_REQ,
    ENUM_DM_ACL_OPEN_REQ,
    ENUM_DM_ACL_CLOSE_REQ,
    ENUM_DM_HCI_REMOTE_NAME_REQUEST_REQ,
    ENUM_DM_HCI_SNIFF_MODE_REQ,
    ENUM_DM_HCI_EXIT_SNIFF_MODE_REQ,
    ENUM_DM_HCI_PARK_MODE_REQ,
    ENUM_DM_HCI_EXIT_PARK_MODE_REQ,
    ENUM_DM_HCI_SWITCH_ROLE_REQ,
    ENUM_DM_HCI_WRITE_LINK_POLICY_SETTINGS_REQ,
    ENUM_DM_LP_WRITE_ROLESWITCH_POLICY_REQ,
    ENUM_DM_LP_WRITE_ALWAYS_MASTER_DEVICES_REQ,
    ENUM_DM_HCI_WRITE_SCAN_ENABLE_REQ,
    ENUM_DM_HCI_ULP_SET_ADVERTISE_ENABLE_REQ,
    ENUM_DM_HCI_ULP_SET_ADVERTISING_PARAMETERS_REQ,
    ENUM_DM_SET_BLE_CONNECTION_PARAMETERS_REQ,
    ENUM_DM_BLE_UPDATE_CONNECTION_PARAMETERS_REQ,
    ENUM_DM_HCI_ULP_SET_SCAN_PARAMETERS_REQ,
    ENUM_DM_HCI_ULP_SET_SCAN_ENABLE_REQ,

    ENUM_DM_AM_REGISTER_CFM = DM_PRIV_UP_PRIM,
    ENUM_DM_SET_BT_VERSION_CFM,
    ENUM_DM_WRITE_CACHED_PAGE_MODE_CFM,
    ENUM_DM_WRITE_CACHED_CLOCK_OFFSET_CFM,
    ENUM_DM_CLEAR_PARAM_CACHE_CFM,
    ENUM_DM_LP_WRITE_POWERSTATES_CFM,
    ENUM_DM_ACL_OPEN_CFM,
    ENUM_DM_ACL_OPENED_IND,
    ENUM_DM_ACL_CLOSED_IND,
    ENUM_DM_HCI_REMOTE_NAME_CFM,
    ENUM_DM_HCI_SWITCH_ROLE_CFM,
    ENUM_DM_LP_WRITE_ROLESWITCH_POLICY_CFM,
    ENUM_DM_BLUESTACK_STATE_TRANSITION_ENTRY_IND,       /* DEBUG */
    ENUM_DM_BLUESTACK_STATE_TRANSITION_EXIT_IND,        /* DEBUG */
    ENUM_DM_LP_WRITE_ALWAYS_MASTER_DEVICES_CFM,
    ENUM_DM_BAD_MESSAGE_IND,
    ENUM_DM_SET_BLE_CONNECTION_PARAMETERS_CFM,
    ENUM_DM_BLE_UPDATE_CONNECTION_PARAMETERS_CFM,

    /*-------------------------------------------------------------
      Security Management Primitives
      ---------------------------------------------------------------*/
    ENUM_DM_SM_INIT_REQ = DM_SM_DOWN_PRIM,
    ENUM_DM_SM_REGISTER_REQ,
    ENUM_DM_SM_UNREGISTER_REQ,
    ENUM_DM_SM_REGISTER_OUTGOING_REQ,
    ENUM_DM_SM_UNREGISTER_OUTGOING_REQ,
    ENUM_DM_SM_ACCESS_REQ,
    ENUM_DM_SM_ADD_DEVICE_REQ,
    ENUM_DM_SM_REMOVE_DEVICE_REQ,
    ENUM_DM_SM_KEY_REQUEST_RSP,
    ENUM_DM_SM_PIN_REQUEST_RSP,
    ENUM_DM_SM_AUTHORISE_RSP,
    ENUM_DM_SM_AUTHENTICATE_REQ,
    ENUM_DM_SM_ENCRYPT_REQ,
    ENUM_DM_SM_IO_CAPABILITY_REQUEST_RSP,
    ENUM_DM_SM_BONDING_REQ,
    ENUM_DM_SM_BONDING_CANCEL_REQ,
    ENUM_DM_SM_READ_DEVICE_REQ,
    ENUM_DM_SM_IO_CAPABILITY_REQUEST_NEG_RSP,
    ENUM_DM_SM_USER_CONFIRMATION_REQUEST_RSP,
    ENUM_DM_SM_USER_CONFIRMATION_REQUEST_NEG_RSP,
    ENUM_DM_SM_USER_PASSKEY_REQUEST_RSP,
    ENUM_DM_SM_USER_PASSKEY_REQUEST_NEG_RSP,
    ENUM_DM_SM_SECURITY_REQ,
    ENUM_DM_SM_CONFIGURE_LOCAL_ADDRESS_REQ,
    ENUM_DM_SM_GENERATE_NONRESOLVABLE_PRIVATE_ADDRESS_REQ,
    ENUM_DM_SM_DATA_SIGN_REQ,
    ENUM_DM_SM_LINK_KEY_REQUEST_RSP,    /* Deprecated */

    ENUM_DM_SM_INIT_CFM = DM_SM_UP_PRIM,
    ENUM_DM_SM_ACCESS_CFM,
    ENUM_DM_SM_ADD_DEVICE_CFM,
    ENUM_DM_SM_REMOVE_DEVICE_CFM,
    ENUM_DM_SM_KEY_REQUEST_IND,
    ENUM_DM_SM_PIN_REQUEST_IND,
    ENUM_DM_SM_KEYS_IND,
    ENUM_DM_SM_AUTHORISE_IND,
    ENUM_DM_SM_AUTHENTICATE_CFM,
    ENUM_DM_SM_ENCRYPT_CFM,
    ENUM_DM_SM_ENCRYPTION_CHANGE_IND,
    ENUM_DM_SM_IO_CAPABILITY_RESPONSE_IND,
    ENUM_DM_SM_IO_CAPABILITY_REQUEST_IND,
    ENUM_DM_SM_SIMPLE_PAIRING_COMPLETE_IND,
    ENUM_DM_SM_USER_CONFIRMATION_REQUEST_IND,
    ENUM_DM_SM_USER_PASSKEY_REQUEST_IND,
    ENUM_DM_SM_USER_PASSKEY_NOTIFICATION_IND,
    ENUM_DM_SM_KEYPRESS_NOTIFICATION_IND,
    ENUM_DM_SM_BONDING_CFM,
    ENUM_DM_SM_READ_DEVICE_CFM,
    ENUM_DM_SM_ACCESS_IND,
    ENUM_DM_SM_REGISTER_CFM,
    ENUM_DM_SM_UNREGISTER_CFM,
    ENUM_DM_SM_REGISTER_OUTGOING_CFM,
    ENUM_DM_SM_UNREGISTER_OUTGOING_CFM,
    ENUM_DM_SM_SECURITY_CFM,
    ENUM_DM_SM_SECURITY_IND,
    ENUM_DM_SM_CONFIGURE_LOCAL_ADDRESS_CFM,
    ENUM_DM_SM_GENERATE_NONRESOLVABLE_PRIVATE_ADDRESS_CFM,
    ENUM_DM_SM_DATA_SIGN_CFM,
    ENUM_DM_SM_CSRK_COUNTER_CHANGE_IND,
    ENUM_DM_SM_LINK_KEY_IND,            /* Deprecated */
    ENUM_DM_SM_LINK_KEY_REQUEST_IND,    /* Deprecated */

    /*-------------------------------------------------------------
      Internal Device Manager/AMP Manager Interface
      ---------------------------------------------------------------*/
    ENUM_DM_AMPM_REGISTER_REQ = DM_AMPM_DOWN_PRIM,
    ENUM_DM_AMPM_CONNECT_RSP,
    ENUM_DM_AMPM_CONNECT_CHANNEL_RSP,
    ENUM_DM_AMPM_DISCONNECT_CHANNEL_RSP,
    ENUM_DM_AMPM_DISCONNECT_REQ,
    ENUM_DM_AMPM_READ_BD_ADDR_REQ,
    ENUM_DM_AMPM_VERIFY_PHYSICAL_LINK_RSP,
    ENUM_DM_AMPM_READ_DATA_BLOCK_SIZE_RSP,
    ENUM_DM_AMPM_NUMBER_COMPLETED_DATA_BLOCKS_REQ,

    ENUM_DM_AMPM_REGISTER_CFM = DM_AMPM_UP_PRIM,
    ENUM_DM_AMPM_CONNECT_IND,
    ENUM_DM_AMPM_CONNECT_CHANNEL_IND,
    ENUM_DM_AMPM_DISCONNECT_CHANNEL_IND,
    ENUM_DM_AMPM_LINK_KEY_NOTIFICATION_IND,
    ENUM_DM_AMPM_READ_BD_ADDR_CFM,
    ENUM_DM_AMPM_VERIFY_PHYSICAL_LINK_IND,
    ENUM_DM_AMPM_READ_DATA_BLOCK_SIZE_IND,
    ENUM_DM_AMPM_SIMPLE_PAIRING_ENABLED_IND,
    ENUM_DM_AMPM_REMOTE_FEATURES_IND,

    /*-------------------------------------------------------------
      Internal Device Manager HCI data interface. Not for application use.
      ---------------------------------------------------------------*/
    ENUM_DM_DATA_FROM_HCI_REQ = DM_INTERNAL_DOWN_PRIM,
    ENUM_DM_DATA_TO_HCI_IND = DM_INTERNAL_UP_PRIM,

    /*-------------------------------------------------------------
      Synchronous Connection Interface Primitives
      ---------------------------------------------------------------*/
    ENUM_DM_SYNC_REGISTER_REQ = DM_SYNC_DOWN_PRIM,
    ENUM_DM_SYNC_UNREGISTER_REQ,
    ENUM_DM_SYNC_CONNECT_REQ,
    ENUM_DM_SYNC_CONNECT_RSP,
    ENUM_DM_SYNC_DISCONNECT_REQ,
    ENUM_DM_SYNC_RENEGOTIATE_REQ,

    ENUM_DM_SYNC_REGISTER_CFM = DM_SYNC_UP_PRIM,
    ENUM_DM_SYNC_UNREGISTER_CFM,
    ENUM_DM_SYNC_CONNECT_CFM,
    ENUM_DM_SYNC_CONNECT_IND,
    ENUM_DM_SYNC_DISCONNECT_CFM,
    ENUM_DM_SYNC_RENEGOTIATE_CFM,
    ENUM_DM_SYNC_CONNECT_COMPLETE_IND,
    ENUM_DM_SYNC_DISCONNECT_IND,
    ENUM_DM_SYNC_RENEGOTIATE_IND,


    /*-------------------------------------------------------------
      HCI API primitives
      ---------------------------------------------------------------*/

    /******************************************************************************
      HCI Events
      *****************************************************************************/
    ENUM_DM_HCI_INQUIRY_RESULT_IND = DM_HCI_EVENT_PRIM,
    ENUM_DM_HCI_RETURN_LINK_KEYS_IND,
    ENUM_DM_HCI_MODE_CHANGE_EVENT_IND,
    ENUM_DM_HCI_INQUIRY_RESULT_WITH_RSSI_IND,
    ENUM_DM_HCI_SNIFF_SUB_RATING_IND,
    ENUM_DM_HCI_EXTENDED_INQUIRY_RESULT_IND,
    ENUM_DM_HCI_LINK_SUPERV_TIMEOUT_IND,
    ENUM_DM_HCI_REM_HOST_SUPPORTED_FEATURES_IND,
    ENUM_DM_HCI_CONN_PACKET_TYPE_CHANGED_IND,
    ENUM_DM_HCI_ULP_ADVERTISING_REPORT_IND,

    /******************************************************************************
      Link Control 
      *****************************************************************************/

    /*-------------------------------------------------------------
      Downstream primitives
      ---------------------------------------------------------------*/
    ENUM_DM_HCI_INQUIRY_REQ                         = HCI_INQUIRY,
    ENUM_DM_HCI_INQUIRY_CANCEL_REQ                  = HCI_INQUIRY_CANCEL,
    ENUM_DM_HCI_PERIODIC_INQUIRY_MODE_REQ           = HCI_PERIODIC_INQUIRY_MODE,
    ENUM_DM_HCI_EXIT_PERIODIC_INQUIRY_MODE_REQ      = HCI_EXIT_PERIODIC_INQUIRY_MODE,
    ENUM_DM_HCI_CREATE_CONNECTION_CANCEL_REQ        = HCI_CREATE_CONNECTION_CANCEL,
    ENUM_DM_HCI_CHANGE_CONN_PKT_TYPE_REQ            = HCI_CHANGE_CONN_PKT_TYPE,
    ENUM_DM_HCI_CHANGE_CONN_LINK_KEY_REQ            = HCI_CHANGE_CONN_LINK_KEY              | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_MASTER_LINK_KEY_REQ                 = HCI_MASTER_LINK_KEY,
    ENUM_DM_HCI_REMOTE_NAME_REQ_CANCEL_REQ          = HCI_REMOTE_NAME_REQ_CANCEL,
    ENUM_DM_HCI_READ_REMOTE_SUPP_FEATURES_REQ       = HCI_READ_REMOTE_SUPP_FEATURES         | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_REMOTE_EXT_FEATURES_REQ        = HCI_READ_REMOTE_EXT_FEATURES          | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_REMOTE_VER_INFO_REQ            = HCI_READ_REMOTE_VER_INFO              | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_CLOCK_OFFSET_REQ               = HCI_READ_CLOCK_OFFSET                 | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_LMP_HANDLE_REQ                 = HCI_READ_LMP_HANDLE,

    /*-------------------------------------------------------------
      Upstream primitives
      ---------------------------------------------------------------*/
    ENUM_DM_HCI_INQUIRY_CFM                         = HCI_INQUIRY                       | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_INQUIRY_CANCEL_CFM                  = HCI_INQUIRY_CANCEL                | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_PERIODIC_INQUIRY_MODE_CFM           = HCI_PERIODIC_INQUIRY_MODE         | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_EXIT_PERIODIC_INQUIRY_MODE_CFM      = HCI_EXIT_PERIODIC_INQUIRY_MODE    | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_CREATE_CONNECTION_CANCEL_CFM        = HCI_CREATE_CONNECTION_CANCEL      | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_CHANGE_CONN_LINK_KEY_CFM            = HCI_CHANGE_CONN_LINK_KEY,
    ENUM_DM_HCI_MASTER_LINK_KEY_CFM                 = HCI_MASTER_LINK_KEY               | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_REMOTE_NAME_REQ_CANCEL_CFM          = HCI_REMOTE_NAME_REQ_CANCEL        | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_REMOTE_SUPP_FEATURES_CFM       = HCI_READ_REMOTE_SUPP_FEATURES,
    ENUM_DM_HCI_READ_REMOTE_EXT_FEATURES_CFM        = HCI_READ_REMOTE_EXT_FEATURES,
    ENUM_DM_HCI_READ_REMOTE_VER_INFO_CFM            = HCI_READ_REMOTE_VER_INFO,
    ENUM_DM_HCI_READ_CLOCK_OFFSET_CFM               = HCI_READ_CLOCK_OFFSET,
    ENUM_DM_HCI_READ_LMP_HANDLE_CFM                 = HCI_READ_LMP_HANDLE               | DM_HCI_WITH_HANDLE,

    /******************************************************************************
      Link Policy 
      *****************************************************************************/
    /*-------------------------------------------------------------
      Downstream primitives
      ---------------------------------------------------------------*/
    ENUM_DM_HCI_HOLD_MODE_REQ                           = HCI_HOLD_MODE                             | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_QOS_SETUP_REQ                           = HCI_QOS_SETUP                             | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_ROLE_DISCOVERY_REQ                      = HCI_ROLE_DISCOVERY                        | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_LINK_POLICY_SETTINGS_REQ           = HCI_READ_LINK_POLICY_SETTINGS             | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_DEFAULT_LINK_POLICY_SETTINGS_REQ   = HCI_READ_DEFAULT_LINK_POLICY_SETTINGS,
    ENUM_DM_HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS_REQ  = HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS,
    ENUM_DM_HCI_FLOW_SPEC_REQ                           = HCI_FLOW_SPEC                             | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_SNIFF_SUB_RATE_REQ                      = HCI_SNIFF_SUB_RATE                        | DM_HCI_WITH_HANDLE,

    /*-------------------------------------------------------------
      Upstream primitives
      ---------------------------------------------------------------*/
    ENUM_DM_HCI_QOS_SETUP_CFM                           = HCI_QOS_SETUP,
    ENUM_DM_HCI_ROLE_DISCOVERY_CFM                      = HCI_ROLE_DISCOVERY,
    ENUM_DM_HCI_READ_LINK_POLICY_SETTINGS_CFM           = HCI_READ_LINK_POLICY_SETTINGS,
    ENUM_DM_HCI_WRITE_LINK_POLICY_SETTINGS_CFM          = HCI_WRITE_LINK_POLICY_SETTINGS,
    ENUM_DM_HCI_READ_DEFAULT_LINK_POLICY_SETTINGS_CFM   = HCI_READ_DEFAULT_LINK_POLICY_SETTINGS     | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS_CFM  = HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS    | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_FLOW_SPEC_CFM                           = HCI_FLOW_SPEC,
    ENUM_DM_HCI_SNIFF_SUB_RATE_CFM                      = HCI_SNIFF_SUB_RATE,

    /******************************************************************************
      Host Controller and Baseband
      *****************************************************************************/
    /*-------------------------------------------------------------
      Downstream primitives
      ---------------------------------------------------------------*/
    ENUM_DM_HCI_SET_EVENT_MASK_REQ                          = HCI_SET_EVENT_MASK,
    ENUM_DM_HCI_RESET_REQ                                   = HCI_RESET,
    ENUM_DM_HCI_SET_EVENT_FILTER_REQ                        = HCI_SET_EVENT_FILTER,
    ENUM_DM_HCI_FLUSH_REQ                                   = HCI_FLUSH                         | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_PIN_TYPE_REQ                           = HCI_READ_PIN_TYPE,
    ENUM_DM_HCI_WRITE_PIN_TYPE_REQ                          = HCI_WRITE_PIN_TYPE,
    ENUM_DM_HCI_CREATE_NEW_UNIT_KEY_REQ                     = HCI_CREATE_NEW_UNIT_KEY,
    ENUM_DM_HCI_READ_STORED_LINK_KEY_REQ                    = HCI_READ_STORED_LINK_KEY,
    ENUM_DM_HCI_WRITE_STORED_LINK_KEY_REQ                   = HCI_WRITE_STORED_LINK_KEY,
    ENUM_DM_HCI_DELETE_STORED_LINK_KEY_REQ                  = HCI_DELETE_STORED_LINK_KEY,
    ENUM_DM_HCI_CHANGE_LOCAL_NAME_REQ                       = HCI_CHANGE_LOCAL_NAME,
    ENUM_DM_HCI_READ_LOCAL_NAME_REQ	                    = HCI_READ_LOCAL_NAME,
    ENUM_DM_HCI_READ_CONN_ACCEPT_TIMEOUT_REQ                = HCI_READ_CONN_ACCEPT_TIMEOUT,
    ENUM_DM_HCI_WRITE_CONN_ACCEPT_TIMEOUT_REQ               = HCI_WRITE_CONN_ACCEPT_TIMEOUT,
    ENUM_DM_HCI_READ_PAGE_TIMEOUT_REQ                       = HCI_READ_PAGE_TIMEOUT,
    ENUM_DM_HCI_WRITE_PAGE_TIMEOUT_REQ                      = HCI_WRITE_PAGE_TIMEOUT,
    ENUM_DM_HCI_READ_SCAN_ENABLE_REQ                        = HCI_READ_SCAN_ENABLE,
    ENUM_DM_HCI_READ_PAGESCAN_ACTIVITY_REQ                  = HCI_READ_PAGESCAN_ACTIVITY,
    ENUM_DM_HCI_WRITE_PAGESCAN_ACTIVITY_REQ                 = HCI_WRITE_PAGESCAN_ACTIVITY,
    ENUM_DM_HCI_READ_INQUIRYSCAN_ACTIVITY_REQ               = HCI_READ_INQUIRYSCAN_ACTIVITY,
    ENUM_DM_HCI_WRITE_INQUIRYSCAN_ACTIVITY_REQ              = HCI_WRITE_INQUIRYSCAN_ACTIVITY,
    ENUM_DM_HCI_READ_AUTH_ENABLE_REQ                        = HCI_READ_AUTH_ENABLE,
    ENUM_DM_HCI_READ_ENC_MODE_REQ                           = HCI_READ_ENC_MODE,
    ENUM_DM_HCI_READ_CLASS_OF_DEVICE_REQ                    = HCI_READ_CLASS_OF_DEVICE,
    ENUM_DM_HCI_WRITE_CLASS_OF_DEVICE_REQ                   = HCI_WRITE_CLASS_OF_DEVICE,
    ENUM_DM_HCI_READ_VOICE_SETTING_REQ                      = HCI_READ_VOICE_SETTING,
    ENUM_DM_HCI_WRITE_VOICE_SETTING_REQ                     = HCI_WRITE_VOICE_SETTING,
    ENUM_DM_HCI_READ_AUTO_FLUSH_TIMEOUT_REQ                 = HCI_READ_AUTO_FLUSH_TIMEOUT       | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_WRITE_AUTO_FLUSH_TIMEOUT_REQ                = HCI_WRITE_AUTO_FLUSH_TIMEOUT      | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_NUM_BCAST_RETXS_REQ                    = HCI_READ_NUM_BCAST_RETXS,
    ENUM_DM_HCI_WRITE_NUM_BCAST_RETXS_REQ                   = HCI_WRITE_NUM_BCAST_RETXS,
    ENUM_DM_HCI_READ_HOLD_MODE_ACTIVITY_REQ                 = HCI_READ_HOLD_MODE_ACTIVITY,
    ENUM_DM_HCI_WRITE_HOLD_MODE_ACTIVITY_REQ                = HCI_WRITE_HOLD_MODE_ACTIVITY,
    ENUM_DM_HCI_READ_TX_POWER_LEVEL_REQ                     = HCI_READ_TX_POWER_LEVEL           | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_SCO_FLOW_CON_ENABLE_REQ                = HCI_READ_SCO_FLOW_CON_ENABLE,
    ENUM_DM_HCI_WRITE_SCO_FLOW_CON_ENABLE_REQ               = HCI_WRITE_SCO_FLOW_CON_ENABLE,
    ENUM_DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ              = HCI_HOST_NUM_COMPLETED_PACKETS,
    ENUM_DM_HCI_READ_LINK_SUPERV_TIMEOUT_REQ                = HCI_READ_LINK_SUPERV_TIMEOUT      | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_WRITE_LINK_SUPERV_TIMEOUT_REQ               = HCI_WRITE_LINK_SUPERV_TIMEOUT     | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_NUM_SUPPORTED_IAC_REQ                  = HCI_READ_NUM_SUPPORTED_IAC,
    ENUM_DM_HCI_READ_CURRENT_IAC_LAP_REQ                    = HCI_READ_CURRENT_IAC_LAP,
    ENUM_DM_HCI_WRITE_CURRENT_IAC_LAP_REQ                   = HCI_WRITE_CURRENT_IAC_LAP,
    ENUM_DM_HCI_READ_PAGESCAN_PERIOD_MODE_REQ               = HCI_READ_PAGESCAN_PERIOD_MODE,
    ENUM_DM_HCI_WRITE_PAGESCAN_PERIOD_MODE_REQ              = HCI_WRITE_PAGESCAN_PERIOD_MODE,
    ENUM_DM_HCI_READ_PAGESCAN_MODE_REQ                      = HCI_READ_PAGESCAN_MODE,
    ENUM_DM_HCI_WRITE_PAGESCAN_MODE_REQ                     = HCI_WRITE_PAGESCAN_MODE,
    ENUM_DM_HCI_SET_AFH_CHANNEL_CLASS_REQ                   = HCI_SET_AFH_CHANNEL_CLASS,
    ENUM_DM_HCI_READ_INQUIRY_SCAN_TYPE_REQ                  = HCI_READ_INQUIRY_SCAN_TYPE,
    ENUM_DM_HCI_WRITE_INQUIRY_SCAN_TYPE_REQ                 = HCI_WRITE_INQUIRY_SCAN_TYPE,
    ENUM_DM_HCI_READ_INQUIRY_MODE_REQ                       = HCI_READ_INQUIRY_MODE,
    ENUM_DM_HCI_WRITE_INQUIRY_MODE_REQ                      = HCI_WRITE_INQUIRY_MODE,
    ENUM_DM_HCI_READ_PAGE_SCAN_TYPE_REQ                     = HCI_READ_PAGE_SCAN_TYPE,
    ENUM_DM_HCI_WRITE_PAGE_SCAN_TYPE_REQ                    = HCI_WRITE_PAGE_SCAN_TYPE,
    ENUM_DM_HCI_READ_AFH_CHANNEL_CLASS_M_REQ                = HCI_READ_AFH_CHANNEL_CLASS_M,
    ENUM_DM_HCI_WRITE_AFH_CHANNEL_CLASS_M_REQ               = HCI_WRITE_AFH_CHANNEL_CLASS_M,
    ENUM_DM_HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_REQ     = HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA,
    ENUM_DM_HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_REQ    = HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA,
    ENUM_DM_HCI_REFRESH_ENCRYPTION_KEY_REQ                  = HCI_REFRESH_ENCRYPTION_KEY                | DM_HCI_WITH_HANDLE,
    ENUM_DM_SM_READ_LOCAL_OOB_DATA_REQ                      = HCI_READ_LOCAL_OOB_DATA,
    ENUM_DM_HCI_READ_INQUIRY_RESPONSE_TX_POWER_LEVEL_REQ    = HCI_READ_INQUIRY_RESPONSE_TX_POWER_LEVEL,
    ENUM_DM_HCI_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL_REQ      = HCI_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL,
    ENUM_DM_HCI_ENHANCED_FLUSH_REQ                          = HCI_ENHANCED_FLUSH                        | DM_HCI_WITH_HANDLE,
    ENUM_DM_SM_SEND_KEYPRESS_NOTIFICATION_REQ               = HCI_SEND_KEYPRESS_NOTIFICATION,

    /*-------------------------------------------------------------
      Upstream primitives
      ---------------------------------------------------------------*/
    ENUM_DM_HCI_SET_EVENT_MASK_CFM                          = HCI_SET_EVENT_MASK                | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_RESET_CFM                                   = HCI_RESET                         | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_SET_EVENT_FILTER_CFM                        = HCI_SET_EVENT_FILTER              | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_FLUSH_CFM                                   = HCI_FLUSH,
    ENUM_DM_HCI_READ_PIN_TYPE_CFM                           = HCI_READ_PIN_TYPE                 | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_WRITE_PIN_TYPE_CFM                          = HCI_WRITE_PIN_TYPE                | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_CREATE_NEW_UNIT_KEY_CFM                     = HCI_CREATE_NEW_UNIT_KEY           | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_STORED_LINK_KEY_CFM                    = HCI_READ_STORED_LINK_KEY          | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_WRITE_STORED_LINK_KEY_CFM                   = HCI_WRITE_STORED_LINK_KEY         | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_DELETE_STORED_LINK_KEY_CFM                  = HCI_DELETE_STORED_LINK_KEY        | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_CHANGE_LOCAL_NAME_CFM                       = HCI_CHANGE_LOCAL_NAME             | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_LOCAL_NAME_CFM                         = HCI_READ_LOCAL_NAME               | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_CONN_ACCEPT_TIMEOUT_CFM                = HCI_READ_CONN_ACCEPT_TIMEOUT      | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_WRITE_CONN_ACCEPT_TIMEOUT_CFM               = HCI_WRITE_CONN_ACCEPT_TIMEOUT     | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_PAGE_TIMEOUT_CFM                       = HCI_READ_PAGE_TIMEOUT             | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_WRITE_PAGE_TIMEOUT_CFM                      = HCI_WRITE_PAGE_TIMEOUT            | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_SCAN_ENABLE_CFM                        = HCI_READ_SCAN_ENABLE              | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_WRITE_SCAN_ENABLE_CFM                       = HCI_WRITE_SCAN_ENABLE             | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_PAGESCAN_ACTIVITY_CFM                  = HCI_READ_PAGESCAN_ACTIVITY        | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_WRITE_PAGESCAN_ACTIVITY_CFM                 = HCI_WRITE_PAGESCAN_ACTIVITY       | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_INQUIRYSCAN_ACTIVITY_CFM               = HCI_READ_INQUIRYSCAN_ACTIVITY     | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_WRITE_INQUIRYSCAN_ACTIVITY_CFM              = HCI_WRITE_INQUIRYSCAN_ACTIVITY    | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_AUTH_ENABLE_CFM                        = HCI_READ_AUTH_ENABLE              | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_ENC_MODE_CFM                           = HCI_READ_ENC_MODE                 | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_CLASS_OF_DEVICE_CFM                    = HCI_READ_CLASS_OF_DEVICE          | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_WRITE_CLASS_OF_DEVICE_CFM                   = HCI_WRITE_CLASS_OF_DEVICE         | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_VOICE_SETTING_CFM                      = HCI_READ_VOICE_SETTING            | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_WRITE_VOICE_SETTING_CFM                     = HCI_WRITE_VOICE_SETTING           | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_AUTO_FLUSH_TIMEOUT_CFM                 = HCI_READ_AUTO_FLUSH_TIMEOUT,
    ENUM_DM_HCI_WRITE_AUTO_FLUSH_TIMEOUT_CFM                = HCI_WRITE_AUTO_FLUSH_TIMEOUT,
    ENUM_DM_HCI_READ_NUM_BCAST_RETXS_CFM                    = HCI_READ_NUM_BCAST_RETXS          | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_WRITE_NUM_BCAST_RETXS_CFM                   = HCI_WRITE_NUM_BCAST_RETXS         | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_HOLD_MODE_ACTIVITY_CFM                 = HCI_READ_HOLD_MODE_ACTIVITY       | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_WRITE_HOLD_MODE_ACTIVITY_CFM                = HCI_WRITE_HOLD_MODE_ACTIVITY      | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_TX_POWER_LEVEL_CFM                     = HCI_READ_TX_POWER_LEVEL,
    ENUM_DM_HCI_READ_SCO_FLOW_CON_ENABLE_CFM                = HCI_READ_SCO_FLOW_CON_ENABLE      | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_WRITE_SCO_FLOW_CON_ENABLE_CFM               = HCI_WRITE_SCO_FLOW_CON_ENABLE     | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_HOST_NUM_COMPLETED_PACKETS_CFM              = HCI_HOST_NUM_COMPLETED_PACKETS    | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_LINK_SUPERV_TIMEOUT_CFM                = HCI_READ_LINK_SUPERV_TIMEOUT,
    ENUM_DM_HCI_WRITE_LINK_SUPERV_TIMEOUT_CFM               = HCI_WRITE_LINK_SUPERV_TIMEOUT,
    ENUM_DM_HCI_READ_NUM_SUPPORTED_IAC_CFM                  = HCI_READ_NUM_SUPPORTED_IAC        | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_CURRENT_IAC_LAP_CFM                    = HCI_READ_CURRENT_IAC_LAP          | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_WRITE_CURRENT_IAC_LAP_CFM                   = HCI_WRITE_CURRENT_IAC_LAP         | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_PAGESCAN_PERIOD_MODE_CFM               = HCI_READ_PAGESCAN_PERIOD_MODE     | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_WRITE_PAGESCAN_PERIOD_MODE_CFM              = HCI_WRITE_PAGESCAN_PERIOD_MODE    | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_PAGESCAN_MODE_CFM                      = HCI_READ_PAGESCAN_MODE            | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_WRITE_PAGESCAN_MODE_CFM                     = HCI_WRITE_PAGESCAN_MODE           | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_SET_AFH_CHANNEL_CLASS_CFM                   = HCI_SET_AFH_CHANNEL_CLASS         | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_INQUIRY_SCAN_TYPE_CFM                  = HCI_READ_INQUIRY_SCAN_TYPE        | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_WRITE_INQUIRY_SCAN_TYPE_CFM                 = HCI_WRITE_INQUIRY_SCAN_TYPE       | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_INQUIRY_MODE_CFM                       = HCI_READ_INQUIRY_MODE             | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_WRITE_INQUIRY_MODE_CFM                      = HCI_WRITE_INQUIRY_MODE            | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_PAGE_SCAN_TYPE_CFM                     = HCI_READ_PAGE_SCAN_TYPE           | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_WRITE_PAGE_SCAN_TYPE_CFM                    = HCI_WRITE_PAGE_SCAN_TYPE          | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_AFH_CHANNEL_CLASS_M_CFM                = HCI_READ_AFH_CHANNEL_CLASS_M      | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_WRITE_AFH_CHANNEL_CLASS_M_CFM               = HCI_WRITE_AFH_CHANNEL_CLASS_M     | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_CFM     = HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA  | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_CFM    = HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_REFRESH_ENCRYPTION_KEY_IND                  = HCI_REFRESH_ENCRYPTION_KEY,
    ENUM_DM_SM_READ_LOCAL_OOB_DATA_CFM                      = HCI_READ_LOCAL_OOB_DATA           | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_INQUIRY_RESPONSE_TX_POWER_LEVEL_CFM    = HCI_READ_INQUIRY_RESPONSE_TX_POWER_LEVEL | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL_CFM      = HCI_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL   | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_ENHANCED_FLUSH_CFM                          = HCI_ENHANCED_FLUSH,

    /******************************************************************************
      Informational parameters
      *****************************************************************************/
    /*-------------------------------------------------------------
      Downstream primitives
      ---------------------------------------------------------------*/
    ENUM_DM_HCI_READ_LOCAL_VER_INFO_REQ         = HCI_READ_LOCAL_VER_INFO,
    ENUM_DM_HCI_READ_LOCAL_SUPP_FEATURES_REQ    = HCI_READ_LOCAL_SUPP_FEATURES,
    ENUM_DM_HCI_READ_LOCAL_EXT_FEATURES_REQ     = HCI_READ_LOCAL_EXT_FEATURES,
    ENUM_DM_HCI_READ_COUNTRY_CODE_REQ           = HCI_READ_COUNTRY_CODE,
    ENUM_DM_HCI_READ_BD_ADDR_REQ                = HCI_READ_BD_ADDR,

    /*-------------------------------------------------------------
      Upstream primitives
      ---------------------------------------------------------------*/
    ENUM_DM_HCI_READ_LOCAL_VER_INFO_CFM        = HCI_READ_LOCAL_VER_INFO       | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_LOCAL_SUPP_FEATURES_CFM   = HCI_READ_LOCAL_SUPP_FEATURES  | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_LOCAL_EXT_FEATURES_CFM    = HCI_READ_LOCAL_EXT_FEATURES   | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_COUNTRY_CODE_CFM          = HCI_READ_COUNTRY_CODE         | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_BD_ADDR_CFM               = HCI_READ_BD_ADDR              | DM_HCI_WITH_HANDLE,

    /******************************************************************************
      Status parameters
      *****************************************************************************/
    /*-------------------------------------------------------------
      Downstream primitives
      ---------------------------------------------------------------*/
    ENUM_DM_HCI_READ_FAILED_CONTACT_COUNT_REQ   = HCI_READ_FAILED_CONTACT_COUNT     | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_RESET_FAILED_CONTACT_COUNT_REQ  = HCI_RESET_FAILED_CONTACT_COUNT    | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_GET_LINK_QUALITY_REQ            = HCI_GET_LINK_QUALITY              | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_RSSI_REQ                   = HCI_READ_RSSI                     | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_AFH_CHANNEL_MAP_REQ        = HCI_READ_AFH_CHANNEL_MAP          | DM_HCI_WITH_HANDLE, 
    ENUM_DM_HCI_READ_CLOCK_REQ                  = HCI_READ_CLOCK                    | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_READ_ENCRYPTION_KEY_SIZE_REQ    = HCI_READ_ENCRYPTION_KEY_SIZE      | DM_HCI_WITH_HANDLE,

    /*-------------------------------------------------------------
      Upstream primitives
      ---------------------------------------------------------------*/
    ENUM_DM_HCI_READ_FAILED_CONTACT_COUNT_CFM   = HCI_READ_FAILED_CONTACT_COUNT,
    ENUM_DM_HCI_RESET_FAILED_CONTACT_COUNT_CFM  = HCI_RESET_FAILED_CONTACT_COUNT,
    ENUM_DM_HCI_GET_LINK_QUALITY_CFM            = HCI_GET_LINK_QUALITY,
    ENUM_DM_HCI_READ_RSSI_CFM                   = HCI_READ_RSSI,
    ENUM_DM_HCI_READ_AFH_CHANNEL_MAP_CFM        = HCI_READ_AFH_CHANNEL_MAP,
    ENUM_DM_HCI_READ_CLOCK_CFM                  = HCI_READ_CLOCK,
    ENUM_DM_HCI_READ_ENCRYPTION_KEY_SIZE_CFM    = HCI_READ_ENCRYPTION_KEY_SIZE,

    /************************************************************************
      Testing primitives
      ***********************************************************************/
    /*-------------------------------------------------------------
      Downstream primitives
      ---------------------------------------------------------------*/
    ENUM_DM_HCI_ENABLE_DUT_MODE_REQ = HCI_ENABLE_DUT_MODE,

    /*-------------------------------------------------------------
      Upstream primitives
      ---------------------------------------------------------------*/
    ENUM_DM_HCI_ENABLE_DUT_MODE_CFM = HCI_ENABLE_DUT_MODE | DM_HCI_WITH_HANDLE,

    /************************************************************************
      ULP primitives
      ***********************************************************************/
    /*-------------------------------------------------------------
      Downstream primitives
      ---------------------------------------------------------------*/
    ENUM_DM_HCI_ULP_SET_EVENT_MASK_REQ                      = HCI_ULP_SET_EVENT_MASK,
    ENUM_DM_HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_REQ       = HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES,
    ENUM_DM_HCI_ULP_SET_RANDOM_ADDRESS_REQ                  = HCI_ULP_SET_RANDOM_ADDRESS,
    /* ENUM_DM_HCI_ULP_SET_ADVERTISING_PARAMETERS_REQ is part of DM_PRIV_PRIM  */
    ENUM_DM_HCI_ULP_READ_ADVERTISING_CHANNEL_TX_POWER_REQ   = HCI_ULP_READ_ADVERTISING_CHANNEL_TX_POWER,
    ENUM_DM_HCI_ULP_SET_ADVERTISING_DATA_REQ                = HCI_ULP_SET_ADVERTISING_DATA,
    ENUM_DM_HCI_ULP_SET_SCAN_RESPONSE_DATA_REQ              = HCI_ULP_SET_SCAN_RESPONSE_DATA,
    /* ENUM_DM_HCI_ULP_SET_ADVERTISE_ENABLE_REQ is part of DM_PRIV_PRIM */
    /* ENUM_DM_HCI_ULP_SET_SCAN_PARAMETERS_REQ is part of DM_PRIV_PRIM */
    /* ENUM_DM_HCI_ULP_SET_SCAN_ENABLE_REQ is part of DM_PRIV_PRIM */
    ENUM_DM_HCI_ULP_CREATE_CONNECTION_CANCEL_REQ            = HCI_ULP_CREATE_CONNECTION_CANCEL,
    ENUM_DM_HCI_ULP_READ_WHITE_LIST_SIZE_REQ                = HCI_ULP_READ_WHITE_LIST_SIZE,
    ENUM_DM_HCI_ULP_CLEAR_WHITE_LIST_REQ                    = HCI_ULP_CLEAR_WHITE_LIST,
    ENUM_DM_HCI_ULP_ADD_DEVICE_TO_WHITE_LIST_REQ            = HCI_ULP_ADD_DEVICE_TO_WHITE_LIST,
    ENUM_DM_HCI_ULP_REMOVE_DEVICE_FROM_WHITE_LIST_REQ       = HCI_ULP_REMOVE_DEVICE_FROM_WHITE_LIST,
    ENUM_DM_HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_REQ     = HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION,
    ENUM_DM_HCI_ULP_READ_CHANNEL_MAP_REQ                    = HCI_ULP_READ_CHANNEL_MAP                      | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_ULP_READ_REMOTE_USED_FEATURES_REQ           = HCI_ULP_READ_REMOTE_USED_FEATURES             | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_ULP_ENCRYPT_REQ                             = HCI_ULP_ENCRYPT,
    ENUM_DM_HCI_ULP_RAND_REQ                                = HCI_ULP_RAND,
    ENUM_DM_HCI_ULP_READ_SUPPORTED_STATES_REQ               = HCI_ULP_READ_SUPPORTED_STATES,
    ENUM_DM_HCI_ULP_RECEIVER_TEST_REQ                       = HCI_ULP_RECEIVER_TEST,
    ENUM_DM_HCI_ULP_TRANSMITTER_TEST_REQ                    = HCI_ULP_TRANSMITTER_TEST,
    ENUM_DM_HCI_ULP_TEST_END_REQ                            = HCI_ULP_TEST_END,

    /*-------------------------------------------------------------
      Upstream primitives
      ---------------------------------------------------------------*/
    ENUM_DM_HCI_ULP_SET_EVENT_MASK_CFM                      = HCI_ULP_SET_EVENT_MASK                        | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_CFM       = HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES         | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_ULP_SET_RANDOM_ADDRESS_CFM                  = HCI_ULP_SET_RANDOM_ADDRESS                    | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_ULP_SET_ADVERTISING_PARAMETERS_CFM          = HCI_ULP_SET_ADVERTISING_PARAMETERS            | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_ULP_READ_ADVERTISING_CHANNEL_TX_POWER_CFM   = HCI_ULP_READ_ADVERTISING_CHANNEL_TX_POWER     | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_ULP_SET_ADVERTISING_DATA_CFM                = HCI_ULP_SET_ADVERTISING_DATA                  | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_ULP_SET_SCAN_RESPONSE_DATA_CFM              = HCI_ULP_SET_SCAN_RESPONSE_DATA                | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_ULP_SET_ADVERTISE_ENABLE_CFM                = HCI_ULP_SET_ADVERTISE_ENABLE                  | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_ULP_SET_SCAN_PARAMETERS_CFM                 = HCI_ULP_SET_SCAN_PARAMETERS                   | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_ULP_SET_SCAN_ENABLE_CFM                     = HCI_ULP_SET_SCAN_ENABLE                       | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_ULP_CREATE_CONNECTION_CANCEL_CFM            = HCI_ULP_CREATE_CONNECTION_CANCEL              | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_ULP_READ_WHITE_LIST_SIZE_CFM                = HCI_ULP_READ_WHITE_LIST_SIZE                  | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_ULP_CLEAR_WHITE_LIST_CFM                    = HCI_ULP_CLEAR_WHITE_LIST                      | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_ULP_ADD_DEVICE_TO_WHITE_LIST_CFM            = HCI_ULP_ADD_DEVICE_TO_WHITE_LIST              | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_ULP_REMOVE_DEVICE_FROM_WHITE_LIST_CFM       = HCI_ULP_REMOVE_DEVICE_FROM_WHITE_LIST         | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_ULP_CONNECTION_UPDATE_COMPLETE_IND          = HCI_ULP_CONNECTION_UPDATE                     ,
    ENUM_DM_HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_CFM     = HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION       | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_ULP_READ_CHANNEL_MAP_CFM                    = HCI_ULP_READ_CHANNEL_MAP                      ,
    ENUM_DM_HCI_ULP_READ_REMOTE_USED_FEATURES_CFM           = HCI_ULP_READ_REMOTE_USED_FEATURES             ,
    ENUM_DM_HCI_ULP_ENCRYPT_CFM                             = HCI_ULP_ENCRYPT                               | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_ULP_RAND_CFM                                = HCI_ULP_RAND                                  | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_ULP_READ_SUPPORTED_STATES_CFM               = HCI_ULP_READ_SUPPORTED_STATES                 | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_ULP_RECEIVER_TEST_CFM                       = HCI_ULP_RECEIVER_TEST                         | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_ULP_TRANSMITTER_TEST_CFM                    = HCI_ULP_TRANSMITTER_TEST                      | DM_HCI_WITH_HANDLE,
    ENUM_DM_HCI_ULP_TEST_END_CFM                            = HCI_ULP_TEST_END                              | DM_HCI_WITH_HANDLE
} DM_PRIM_T;

/* ... and then reflect it here */
#define DM_AM_REGISTER_REQ                      ((dm_prim_t)ENUM_DM_AM_REGISTER_REQ)
#define DM_AM_REGISTER_CFM                      ((dm_prim_t)ENUM_DM_AM_REGISTER_CFM)
#define DM_SET_BT_VERSION_REQ                   ((dm_prim_t)ENUM_DM_SET_BT_VERSION_REQ)
#define DM_SET_BT_VERSION_CFM                   ((dm_prim_t)ENUM_DM_SET_BT_VERSION_CFM)
#define DM_AM_REGISTER_WRONG_API_REQ            ((dm_prim_t)ENUM_DM_AM_REGISTER_WRONG_API_REQ)
#define DM_AM_REGISTER_WRONG_API_CFM            ((dm_prim_t)ENUM_DM_AM_REGISTER_WRONG_API_CFM)
#define DM_BAD_MESSAGE_IND                      ((dm_prim_t)ENUM_DM_BAD_MESSAGE_IND)
#define DM_SET_BLE_CONNECTION_PARAMETERS_REQ    ((dm_prim_t)ENUM_DM_SET_BLE_CONNECTION_PARAMETERS_REQ)
#define DM_SET_BLE_CONNECTION_PARAMETERS_CFM    ((dm_prim_t)ENUM_DM_SET_BLE_CONNECTION_PARAMETERS_CFM)
#define DM_BLE_UPDATE_CONNECTION_PARAMETERS_REQ ((dm_prim_t)ENUM_DM_BLE_UPDATE_CONNECTION_PARAMETERS_REQ)
#define DM_BLE_UPDATE_CONNECTION_PARAMETERS_CFM ((dm_prim_t)ENUM_DM_BLE_UPDATE_CONNECTION_PARAMETERS_CFM)


/*-------------------------------------------------------------
  Synchronous Connection Interface Primitives
  ---------------------------------------------------------------*/
#define DM_SYNC_REGISTER_REQ            ((dm_prim_t)(ENUM_DM_SYNC_REGISTER_REQ))
#define DM_SYNC_REGISTER_CFM            ((dm_prim_t)(ENUM_DM_SYNC_REGISTER_CFM))
#define DM_SYNC_UNREGISTER_REQ          ((dm_prim_t)(ENUM_DM_SYNC_UNREGISTER_REQ))
#define DM_SYNC_UNREGISTER_CFM          ((dm_prim_t)(ENUM_DM_SYNC_UNREGISTER_CFM))
#define DM_SYNC_CONNECT_REQ             ((dm_prim_t)(ENUM_DM_SYNC_CONNECT_REQ))
#define DM_SYNC_CONNECT_CFM             ((dm_prim_t)(ENUM_DM_SYNC_CONNECT_CFM))
#define DM_SYNC_CONNECT_COMPLETE_IND    ((dm_prim_t)(ENUM_DM_SYNC_CONNECT_COMPLETE_IND))
#define DM_SYNC_CONNECT_IND             ((dm_prim_t)(ENUM_DM_SYNC_CONNECT_IND))
#define DM_SYNC_CONNECT_RSP             ((dm_prim_t)(ENUM_DM_SYNC_CONNECT_RSP))
#define DM_SYNC_RENEGOTIATE_REQ         ((dm_prim_t)(ENUM_DM_SYNC_RENEGOTIATE_REQ))
#define DM_SYNC_RENEGOTIATE_IND         ((dm_prim_t)(ENUM_DM_SYNC_RENEGOTIATE_IND))
#define DM_SYNC_RENEGOTIATE_CFM         ((dm_prim_t)(ENUM_DM_SYNC_RENEGOTIATE_CFM))
#define DM_SYNC_DISCONNECT_REQ          ((dm_prim_t)(ENUM_DM_SYNC_DISCONNECT_REQ))
#define DM_SYNC_DISCONNECT_IND          ((dm_prim_t)(ENUM_DM_SYNC_DISCONNECT_IND))
#define DM_SYNC_DISCONNECT_CFM          ((dm_prim_t)(ENUM_DM_SYNC_DISCONNECT_CFM))

/*-------------------------------------------------------------
  Security Management Primitives
  ---------------------------------------------------------------*/

#define DM_SM_REGISTER_REQ              ((dm_prim_t)(ENUM_DM_SM_REGISTER_REQ))
#define DM_SM_REGISTER_CFM              ((dm_prim_t)(ENUM_DM_SM_REGISTER_CFM))
#define DM_SM_UNREGISTER_REQ            ((dm_prim_t)(ENUM_DM_SM_UNREGISTER_REQ))
#define DM_SM_UNREGISTER_CFM            ((dm_prim_t)(ENUM_DM_SM_UNREGISTER_CFM))
#define DM_SM_REGISTER_OUTGOING_REQ     ((dm_prim_t)(ENUM_DM_SM_REGISTER_OUTGOING_REQ))
#define DM_SM_REGISTER_OUTGOING_CFM     ((dm_prim_t)(ENUM_DM_SM_REGISTER_OUTGOING_CFM))
#define DM_SM_UNREGISTER_OUTGOING_REQ   ((dm_prim_t)(ENUM_DM_SM_UNREGISTER_OUTGOING_REQ))
#define DM_SM_UNREGISTER_OUTGOING_CFM   ((dm_prim_t)(ENUM_DM_SM_UNREGISTER_OUTGOING_CFM))
#define DM_SM_ACCESS_REQ                ((dm_prim_t)(ENUM_DM_SM_ACCESS_REQ))
#define DM_SM_ACCESS_CFM                ((dm_prim_t)(ENUM_DM_SM_ACCESS_CFM))
#define DM_SM_ACCESS_IND                ((dm_prim_t)(ENUM_DM_SM_ACCESS_IND))
#define DM_SM_INIT_REQ                  ((dm_prim_t)(ENUM_DM_SM_INIT_REQ))
#define DM_SM_INIT_CFM                  ((dm_prim_t)(ENUM_DM_SM_INIT_CFM))
#define DM_SM_ADD_DEVICE_REQ            ((dm_prim_t)(ENUM_DM_SM_ADD_DEVICE_REQ))
#define DM_SM_ADD_DEVICE_CFM            ((dm_prim_t)(ENUM_DM_SM_ADD_DEVICE_CFM))
#define DM_SM_REMOVE_DEVICE_REQ         ((dm_prim_t)(ENUM_DM_SM_REMOVE_DEVICE_REQ))
#define DM_SM_REMOVE_DEVICE_CFM         ((dm_prim_t)(ENUM_DM_SM_REMOVE_DEVICE_CFM))
#define DM_SM_LINK_KEY_REQUEST_IND      ((dm_prim_t)(ENUM_DM_SM_LINK_KEY_REQUEST_IND))
#define DM_SM_LINK_KEY_REQUEST_RSP      ((dm_prim_t)(ENUM_DM_SM_LINK_KEY_REQUEST_RSP))
#define DM_SM_PIN_REQUEST_IND           ((dm_prim_t)(ENUM_DM_SM_PIN_REQUEST_IND))
#define DM_SM_PIN_REQUEST_RSP           ((dm_prim_t)(ENUM_DM_SM_PIN_REQUEST_RSP))
#define DM_SM_LINK_KEY_IND              ((dm_prim_t)(ENUM_DM_SM_LINK_KEY_IND))
#define DM_SM_AUTHORISE_IND             ((dm_prim_t)(ENUM_DM_SM_AUTHORISE_IND))
#define DM_SM_AUTHORISE_RSP             ((dm_prim_t)(ENUM_DM_SM_AUTHORISE_RSP))
#define DM_SM_AUTHENTICATE_REQ          ((dm_prim_t)(ENUM_DM_SM_AUTHENTICATE_REQ))
#define DM_SM_AUTHENTICATE_CFM          ((dm_prim_t)(ENUM_DM_SM_AUTHENTICATE_CFM))
#define DM_SM_ENCRYPT_REQ               ((dm_prim_t)(ENUM_DM_SM_ENCRYPT_REQ))
#define DM_SM_ENCRYPT_CFM               ((dm_prim_t)(ENUM_DM_SM_ENCRYPT_CFM))
#define DM_SM_ENCRYPTION_CHANGE_IND         ((dm_prim_t)(ENUM_DM_SM_ENCRYPTION_CHANGE_IND))

#define DM_SM_IO_CAPABILITY_RESPONSE_IND        \
                ((dm_prim_t)(ENUM_DM_SM_IO_CAPABILITY_RESPONSE_IND))
#define DM_SM_IO_CAPABILITY_REQUEST_IND         \
                ((dm_prim_t)(ENUM_DM_SM_IO_CAPABILITY_REQUEST_IND))
#define DM_SM_IO_CAPABILITY_REQUEST_RSP         \
                ((dm_prim_t)(ENUM_DM_SM_IO_CAPABILITY_REQUEST_RSP))
#define DM_SM_IO_CAPABILITY_REQUEST_NEG_RSP     \
                ((dm_prim_t)(ENUM_DM_SM_IO_CAPABILITY_REQUEST_NEG_RSP))
#define DM_SM_USER_CONFIRMATION_REQUEST_IND     \
                ((dm_prim_t)(ENUM_DM_SM_USER_CONFIRMATION_REQUEST_IND))
#define DM_SM_USER_CONFIRMATION_REQUEST_RSP     \
                ((dm_prim_t)(ENUM_DM_SM_USER_CONFIRMATION_REQUEST_RSP))
#define DM_SM_USER_CONFIRMATION_REQUEST_NEG_RSP \
                ((dm_prim_t)(ENUM_DM_SM_USER_CONFIRMATION_REQUEST_NEG_RSP))
#define DM_SM_USER_PASSKEY_REQUEST_IND          \
                ((dm_prim_t)(ENUM_DM_SM_USER_PASSKEY_REQUEST_IND))
#define DM_SM_USER_PASSKEY_REQUEST_RSP          \
                ((dm_prim_t)(ENUM_DM_SM_USER_PASSKEY_REQUEST_RSP))
#define DM_SM_USER_PASSKEY_REQUEST_NEG_RSP      \
                ((dm_prim_t)(ENUM_DM_SM_USER_PASSKEY_REQUEST_NEG_RSP))
#define DM_SM_SIMPLE_PAIRING_COMPLETE_IND       \
                ((dm_prim_t)(ENUM_DM_SM_SIMPLE_PAIRING_COMPLETE_IND))
#define DM_SM_READ_LOCAL_OOB_DATA_REQ           \
                ((dm_prim_t)(ENUM_DM_SM_READ_LOCAL_OOB_DATA_REQ))
#define DM_SM_READ_LOCAL_OOB_DATA_CFM           \
                ((dm_prim_t)(ENUM_DM_SM_READ_LOCAL_OOB_DATA_CFM))
#define DM_SM_USER_PASSKEY_NOTIFICATION_IND     \
                ((dm_prim_t)(ENUM_DM_SM_USER_PASSKEY_NOTIFICATION_IND))
#define DM_SM_SEND_KEYPRESS_NOTIFICATION_REQ    \
                ((dm_prim_t)(ENUM_DM_SM_SEND_KEYPRESS_NOTIFICATION_REQ))
#define DM_SM_KEYPRESS_NOTIFICATION_IND         \
                ((dm_prim_t)(ENUM_DM_SM_KEYPRESS_NOTIFICATION_IND))
#define DM_SM_BONDING_REQ           ((dm_prim_t)(ENUM_DM_SM_BONDING_REQ))
#define DM_SM_BONDING_CFM           ((dm_prim_t)(ENUM_DM_SM_BONDING_CFM))
#define DM_SM_BONDING_CANCEL_REQ    ((dm_prim_t)(ENUM_DM_SM_BONDING_CANCEL_REQ))
#define DM_SM_READ_DEVICE_REQ       ((dm_prim_t)(ENUM_DM_SM_READ_DEVICE_REQ))
#define DM_SM_READ_DEVICE_CFM       ((dm_prim_t)(ENUM_DM_SM_READ_DEVICE_CFM))
#define DM_SM_KEYS_IND              ((dm_prim_t)(ENUM_DM_SM_KEYS_IND))
#define DM_SM_SECURITY_REQ          ((dm_prim_t)(ENUM_DM_SM_SECURITY_REQ))
#define DM_SM_SECURITY_CFM          ((dm_prim_t)(ENUM_DM_SM_SECURITY_CFM))
#define DM_SM_SECURITY_IND          ((dm_prim_t)(ENUM_DM_SM_SECURITY_IND))
#define DM_SM_KEY_REQUEST_IND       ((dm_prim_t)(ENUM_DM_SM_KEY_REQUEST_IND))
#define DM_SM_KEY_REQUEST_RSP       ((dm_prim_t)(ENUM_DM_SM_KEY_REQUEST_RSP))
#define DM_SM_CONFIGURE_LOCAL_ADDRESS_REQ ((dm_prim_t)(ENUM_DM_SM_CONFIGURE_LOCAL_ADDRESS_REQ))
#define DM_SM_CONFIGURE_LOCAL_ADDRESS_CFM ((dm_prim_t)(ENUM_DM_SM_CONFIGURE_LOCAL_ADDRESS_CFM))
#define DM_SM_GENERATE_NONRESOLVABLE_PRIVATE_ADDRESS_REQ \
        ((dm_prim_t)(ENUM_DM_SM_GENERATE_NONRESOLVABLE_PRIVATE_ADDRESS_REQ))
#define DM_SM_GENERATE_NONRESOLVABLE_PRIVATE_ADDRESS_CFM \
        ((dm_prim_t)(ENUM_DM_SM_GENERATE_NONRESOLVABLE_PRIVATE_ADDRESS_CFM))
#define DM_SM_DATA_SIGN_REQ         ((dm_prim_t)(ENUM_DM_SM_DATA_SIGN_REQ))
#define DM_SM_DATA_SIGN_CFM         ((dm_prim_t)(ENUM_DM_SM_DATA_SIGN_CFM))
#define DM_SM_CSRK_COUNTER_CHANGE_IND ((dm_prim_t)(ENUM_DM_SM_CSRK_COUNTER_CHANGE_IND))


/*-------------------------------------------------------------
  DM private primitives
  ---------------------------------------------------------------*/

#define DM_WRITE_CACHED_PAGE_MODE_REQ    ((dm_prim_t)(ENUM_DM_WRITE_CACHED_PAGE_MODE_REQ))
#define DM_WRITE_CACHED_PAGE_MODE_CFM    ((dm_prim_t)(ENUM_DM_WRITE_CACHED_PAGE_MODE_CFM))
#define DM_WRITE_CACHED_CLOCK_OFFSET_REQ ((dm_prim_t)(ENUM_DM_WRITE_CACHED_CLOCK_OFFSET_REQ))
#define DM_WRITE_CACHED_CLOCK_OFFSET_CFM ((dm_prim_t)(ENUM_DM_WRITE_CACHED_CLOCK_OFFSET_CFM))
#define DM_CLEAR_PARAM_CACHE_REQ         ((dm_prim_t)(ENUM_DM_CLEAR_PARAM_CACHE_REQ))
#define DM_CLEAR_PARAM_CACHE_CFM         ((dm_prim_t)(ENUM_DM_CLEAR_PARAM_CACHE_CFM))
#define DM_ACL_OPEN_REQ                  ((dm_prim_t)(ENUM_DM_ACL_OPEN_REQ))
#define DM_ACL_OPEN_CFM                  ((dm_prim_t)(ENUM_DM_ACL_OPEN_CFM))
#define DM_ACL_CLOSE_REQ                 ((dm_prim_t)(ENUM_DM_ACL_CLOSE_REQ))
#define DM_ACL_OPENED_IND                ((dm_prim_t)(ENUM_DM_ACL_OPENED_IND))
#define DM_ACL_CLOSED_IND                ((dm_prim_t)(ENUM_DM_ACL_CLOSED_IND))
#define DM_SET_DEFAULT_LINK_POLICY_REQ   ((dm_prim_t)(ENUM_DM_SET_DEFAULT_LINK_POLICY_REQ))

#define DM_LP_WRITE_POWERSTATES_REQ      ((dm_prim_t)(ENUM_DM_LP_WRITE_POWERSTATES_REQ))
#define DM_LP_WRITE_POWERSTATES_CFM      ((dm_prim_t)(ENUM_DM_LP_WRITE_POWERSTATES_CFM))
#define DM_LP_WRITE_ROLESWITCH_POLICY_REQ   ((dm_prim_t)(ENUM_DM_LP_WRITE_ROLESWITCH_POLICY_REQ))
#define DM_LP_WRITE_ROLESWITCH_POLICY_CFM   ((dm_prim_t)(ENUM_DM_LP_WRITE_ROLESWITCH_POLICY_CFM))
#define DM_LP_WRITE_ALWAYS_MASTER_DEVICES_REQ ((dm_prim_t)(ENUM_DM_LP_WRITE_ALWAYS_MASTER_DEVICES_REQ))
#define DM_LP_WRITE_ALWAYS_MASTER_DEVICES_CFM ((dm_prim_t)(ENUM_DM_LP_WRITE_ALWAYS_MASTER_DEVICES_CFM))


#define DM_BLUESTACK_STATE_TRANSITION_ENTRY_IND ((dm_prim_t)(ENUM_DM_BLUESTACK_STATE_TRANSITION_ENTRY_IND))
#define DM_BLUESTACK_STATE_TRANSITION_EXIT_IND ((dm_prim_t)(ENUM_DM_BLUESTACK_STATE_TRANSITION_EXIT_IND))

/*-------------------------------------------------------------
  Internal DM/AMPM Interface
  ---------------------------------------------------------------*/
#define DM_AMPM_REGISTER_REQ                ((dm_prim_t)(ENUM_DM_AMPM_REGISTER_REQ))
#define DM_AMPM_CONNECT_RSP                 ((dm_prim_t)(ENUM_DM_AMPM_CONNECT_RSP))
#define DM_AMPM_CONNECT_CHANNEL_RSP         ((dm_prim_t)(ENUM_DM_AMPM_CONNECT_CHANNEL_RSP))
#define DM_AMPM_DISCONNECT_CHANNEL_RSP      ((dm_prim_t)(ENUM_DM_AMPM_DISCONNECT_CHANNEL_RSP))
#define DM_AMPM_DISCONNECT_REQ              ((dm_prim_t)(ENUM_DM_AMPM_DISCONNECT_REQ))
#define DM_AMPM_READ_BD_ADDR_REQ            ((dm_prim_t)(ENUM_DM_AMPM_READ_BD_ADDR_REQ))
#define DM_AMPM_VERIFY_PHYSICAL_LINK_RSP    ((dm_prim_t)(ENUM_DM_AMPM_VERIFY_PHYSICAL_LINK_RSP))
#define DM_AMPM_READ_DATA_BLOCK_SIZE_RSP    ((dm_prim_t)(ENUM_DM_AMPM_READ_DATA_BLOCK_SIZE_RSP))
#define DM_AMPM_NUMBER_COMPLETED_DATA_BLOCKS_REQ ((dm_prim_t)(ENUM_DM_AMPM_NUMBER_COMPLETED_DATA_BLOCKS_REQ))

#define DM_AMPM_REGISTER_CFM                ((dm_prim_t)(ENUM_DM_AMPM_REGISTER_CFM))
#define DM_AMPM_CONNECT_IND                 ((dm_prim_t)(ENUM_DM_AMPM_CONNECT_IND))
#define DM_AMPM_CONNECT_CHANNEL_IND         ((dm_prim_t)(ENUM_DM_AMPM_CONNECT_CHANNEL_IND))
#define DM_AMPM_DISCONNECT_CHANNEL_IND      ((dm_prim_t)(ENUM_DM_AMPM_DISCONNECT_CHANNEL_IND))
#define DM_AMPM_LINK_KEY_NOTIFICATION_IND   ((dm_prim_t)(ENUM_DM_AMPM_LINK_KEY_NOTIFICATION_IND))
#define DM_AMPM_READ_BD_ADDR_CFM            ((dm_prim_t)(ENUM_DM_AMPM_READ_BD_ADDR_CFM))
#define DM_AMPM_VERIFY_PHYSICAL_LINK_IND    ((dm_prim_t)(ENUM_DM_AMPM_VERIFY_PHYSICAL_LINK_IND))
#define DM_AMPM_READ_DATA_BLOCK_SIZE_IND    ((dm_prim_t)(ENUM_DM_AMPM_READ_DATA_BLOCK_SIZE_IND))
#define DM_AMPM_SIMPLE_PAIRING_ENABLED_IND  ((dm_prim_t)(ENUM_DM_AMPM_SIMPLE_PAIRING_ENABLED_IND))
#define DM_AMPM_REMOTE_FEATURES_IND         ((dm_prim_t)(ENUM_DM_AMPM_REMOTE_FEATURES_IND))

/*-------------------------------------------------------------
  Internal Device Manager HCI data interface. Not for application use.
  ---------------------------------------------------------------*/
#define DM_DATA_FROM_HCI_REQ    ((dm_prim_t)ENUM_DM_DATA_FROM_HCI_REQ)
#define DM_DATA_TO_HCI_IND      ((dm_prim_t)ENUM_DM_DATA_TO_HCI_IND)

/*-------------------------------------------------------------
HCI API primitives
---------------------------------------------------------------*/

/******************************************************************************
 Link Control 
 *****************************************************************************/

/*-------------------------------------------------------------
Downstream primitives
---------------------------------------------------------------*/
#define DM_HCI_INQUIRY_REQ                    ((dm_prim_t) ENUM_DM_HCI_INQUIRY_REQ)
#define DM_HCI_INQUIRY_CANCEL_REQ             ((dm_prim_t) ENUM_DM_HCI_INQUIRY_CANCEL_REQ)
#define DM_HCI_PERIODIC_INQUIRY_MODE_REQ      ((dm_prim_t) ENUM_DM_HCI_PERIODIC_INQUIRY_MODE_REQ)
#define DM_HCI_EXIT_PERIODIC_INQUIRY_MODE_REQ ((dm_prim_t) ENUM_DM_HCI_EXIT_PERIODIC_INQUIRY_MODE_REQ)
#define DM_HCI_CHANGE_CONN_PKT_TYPE_REQ       ((dm_prim_t) ENUM_DM_HCI_CHANGE_CONN_PKT_TYPE_REQ)
#define DM_HCI_CHANGE_CONN_LINK_KEY_REQ       ((dm_prim_t) ENUM_DM_HCI_CHANGE_CONN_LINK_KEY_REQ)
#define DM_HCI_MASTER_LINK_KEY_REQ            ((dm_prim_t) ENUM_DM_HCI_MASTER_LINK_KEY_REQ)
#define DM_HCI_REMOTE_NAME_REQUEST_REQ        ((dm_prim_t) ENUM_DM_HCI_REMOTE_NAME_REQUEST_REQ)
#define DM_HCI_READ_REMOTE_SUPP_FEATURES_REQ  ((dm_prim_t) ENUM_DM_HCI_READ_REMOTE_SUPP_FEATURES_REQ)
#define DM_HCI_READ_REMOTE_VER_INFO_REQ       ((dm_prim_t) ENUM_DM_HCI_READ_REMOTE_VER_INFO_REQ)
#define DM_HCI_READ_CLOCK_OFFSET_REQ          ((dm_prim_t) ENUM_DM_HCI_READ_CLOCK_OFFSET_REQ)

/*-------------------------------------------------------------
Upstream primitives
---------------------------------------------------------------*/
#define DM_HCI_INQUIRY_CANCEL_CFM             ((dm_prim_t)ENUM_DM_HCI_INQUIRY_CANCEL_CFM)
#define DM_HCI_PERIODIC_INQUIRY_MODE_CFM      ((dm_prim_t)ENUM_DM_HCI_PERIODIC_INQUIRY_MODE_CFM)
#define DM_HCI_EXIT_PERIODIC_INQUIRY_MODE_CFM ((dm_prim_t)ENUM_DM_HCI_EXIT_PERIODIC_INQUIRY_MODE_CFM)
#define DM_HCI_INQUIRY_RESULT_IND             ((dm_prim_t)ENUM_DM_HCI_INQUIRY_RESULT_IND)
#define DM_HCI_INQUIRY_CFM                    ((dm_prim_t)ENUM_DM_HCI_INQUIRY_CFM)
#define DM_HCI_CHANGE_CONN_LINK_KEY_CFM       ((dm_prim_t)ENUM_DM_HCI_CHANGE_CONN_LINK_KEY_CFM)
#define DM_HCI_RETURN_LINK_KEYS_IND           ((dm_prim_t)ENUM_DM_HCI_RETURN_LINK_KEYS_IND)
#define DM_HCI_MASTER_LINK_KEY_CFM            ((dm_prim_t)ENUM_DM_HCI_MASTER_LINK_KEY_CFM)
#define DM_HCI_REMOTE_NAME_CFM                ((dm_prim_t)ENUM_DM_HCI_REMOTE_NAME_CFM)
#define DM_HCI_READ_REMOTE_SUPP_FEATURES_CFM  ((dm_prim_t)ENUM_DM_HCI_READ_REMOTE_SUPP_FEATURES_CFM)
#define DM_HCI_READ_REMOTE_VER_INFO_CFM       ((dm_prim_t)ENUM_DM_HCI_READ_REMOTE_VER_INFO_CFM)
#define DM_HCI_READ_CLOCK_OFFSET_CFM          ((dm_prim_t)ENUM_DM_HCI_READ_CLOCK_OFFSET_CFM)

/******************************************************************************
   Link Policy 
 *****************************************************************************/
/*-------------------------------------------------------------
Downstream primitives
---------------------------------------------------------------*/
#define DM_HCI_HOLD_MODE_REQ                    ((dm_prim_t) ENUM_DM_HCI_HOLD_MODE_REQ)
#define DM_HCI_SNIFF_MODE_REQ                   ((dm_prim_t) ENUM_DM_HCI_SNIFF_MODE_REQ)
#define DM_HCI_EXIT_SNIFF_MODE_REQ              ((dm_prim_t) ENUM_DM_HCI_EXIT_SNIFF_MODE_REQ)
#define DM_HCI_PARK_MODE_REQ                    ((dm_prim_t) ENUM_DM_HCI_PARK_MODE_REQ)
#define DM_HCI_EXIT_PARK_MODE_REQ               ((dm_prim_t) ENUM_DM_HCI_EXIT_PARK_MODE_REQ)
#define DM_HCI_QOS_SETUP_REQ                    ((dm_prim_t) ENUM_DM_HCI_QOS_SETUP_REQ)
#define DM_HCI_ROLE_DISCOVERY_REQ               ((dm_prim_t) ENUM_DM_HCI_ROLE_DISCOVERY_REQ)
#define DM_HCI_SWITCH_ROLE_REQ                  ((dm_prim_t) ENUM_DM_HCI_SWITCH_ROLE_REQ)
#define DM_HCI_READ_LINK_POLICY_SETTINGS_REQ    ((dm_prim_t) ENUM_DM_HCI_READ_LINK_POLICY_SETTINGS_REQ)
#define DM_HCI_WRITE_LINK_POLICY_SETTINGS_REQ   ((dm_prim_t) ENUM_DM_HCI_WRITE_LINK_POLICY_SETTINGS_REQ)

/*-------------------------------------------------------------
Upstream primitives
---------------------------------------------------------------*/
#define DM_HCI_MODE_CHANGE_EVENT_IND            ((dm_prim_t)ENUM_DM_HCI_MODE_CHANGE_EVENT_IND)
#define DM_HCI_QOS_SETUP_CFM                    ((dm_prim_t)ENUM_DM_HCI_QOS_SETUP_CFM)
#define DM_HCI_ROLE_DISCOVERY_CFM               ((dm_prim_t)ENUM_DM_HCI_ROLE_DISCOVERY_CFM)
#define DM_HCI_SWITCH_ROLE_CFM                  ((dm_prim_t)ENUM_DM_HCI_SWITCH_ROLE_CFM)
#define DM_HCI_READ_LINK_POLICY_SETTINGS_CFM    ((dm_prim_t)ENUM_DM_HCI_READ_LINK_POLICY_SETTINGS_CFM)
#define DM_HCI_WRITE_LINK_POLICY_SETTINGS_CFM   ((dm_prim_t)ENUM_DM_HCI_WRITE_LINK_POLICY_SETTINGS_CFM)

/******************************************************************************
   Host Controller and Baseband
 *****************************************************************************/
/*-------------------------------------------------------------
Downstream primitives
---------------------------------------------------------------*/
#define DM_HCI_SET_EVENT_MASK_REQ                   ((dm_prim_t) ENUM_DM_HCI_SET_EVENT_MASK_REQ)
#define DM_HCI_RESET_REQ                            ((dm_prim_t) ENUM_DM_HCI_RESET_REQ)
#define DM_HCI_SET_EVENT_FILTER_REQ                 ((dm_prim_t) ENUM_DM_HCI_SET_EVENT_FILTER_REQ)
#define DM_HCI_FLUSH_REQ                            ((dm_prim_t) ENUM_DM_HCI_FLUSH_REQ)
#define DM_HCI_READ_PIN_TYPE_REQ                    ((dm_prim_t) ENUM_DM_HCI_READ_PIN_TYPE_REQ)
#define DM_HCI_WRITE_PIN_TYPE_REQ                   ((dm_prim_t) ENUM_DM_HCI_WRITE_PIN_TYPE_REQ)
#define DM_HCI_CREATE_NEW_UNIT_KEY_REQ              ((dm_prim_t) ENUM_DM_HCI_CREATE_NEW_UNIT_KEY_REQ)
#define DM_HCI_READ_STORED_LINK_KEY_REQ             ((dm_prim_t) ENUM_DM_HCI_READ_STORED_LINK_KEY_REQ)
#define DM_HCI_WRITE_STORED_LINK_KEY_REQ            ((dm_prim_t) ENUM_DM_HCI_WRITE_STORED_LINK_KEY_REQ)
#define DM_HCI_DELETE_STORED_LINK_KEY_REQ           ((dm_prim_t) ENUM_DM_HCI_DELETE_STORED_LINK_KEY_REQ)
#define DM_HCI_CHANGE_LOCAL_NAME_REQ                ((dm_prim_t) ENUM_DM_HCI_CHANGE_LOCAL_NAME_REQ)
#define DM_HCI_READ_LOCAL_NAME_REQ                  ((dm_prim_t) ENUM_DM_HCI_READ_LOCAL_NAME_REQ)
#define DM_HCI_READ_CONN_ACCEPT_TIMEOUT_REQ         ((dm_prim_t) ENUM_DM_HCI_READ_CONN_ACCEPT_TIMEOUT_REQ)
#define DM_HCI_WRITE_CONN_ACCEPT_TIMEOUT_REQ        ((dm_prim_t) ENUM_DM_HCI_WRITE_CONN_ACCEPT_TIMEOUT_REQ)
#define DM_HCI_READ_PAGE_TIMEOUT_REQ                ((dm_prim_t) ENUM_DM_HCI_READ_PAGE_TIMEOUT_REQ)
#define DM_HCI_WRITE_PAGE_TIMEOUT_REQ               ((dm_prim_t) ENUM_DM_HCI_WRITE_PAGE_TIMEOUT_REQ)
#define DM_HCI_READ_SCAN_ENABLE_REQ                 ((dm_prim_t) ENUM_DM_HCI_READ_SCAN_ENABLE_REQ)
#define DM_HCI_WRITE_SCAN_ENABLE_REQ                ((dm_prim_t) ENUM_DM_HCI_WRITE_SCAN_ENABLE_REQ)
#define DM_HCI_READ_PAGESCAN_ACTIVITY_REQ           ((dm_prim_t) ENUM_DM_HCI_READ_PAGESCAN_ACTIVITY_REQ)
#define DM_HCI_WRITE_PAGESCAN_ACTIVITY_REQ          ((dm_prim_t) ENUM_DM_HCI_WRITE_PAGESCAN_ACTIVITY_REQ)
#define DM_HCI_READ_INQUIRYSCAN_ACTIVITY_REQ        ((dm_prim_t) ENUM_DM_HCI_READ_INQUIRYSCAN_ACTIVITY_REQ)
#define DM_HCI_WRITE_INQUIRYSCAN_ACTIVITY_REQ       ((dm_prim_t) ENUM_DM_HCI_WRITE_INQUIRYSCAN_ACTIVITY_REQ)
#define DM_HCI_READ_AUTH_ENABLE_REQ                 ((dm_prim_t) ENUM_DM_HCI_READ_AUTH_ENABLE_REQ)
#define DM_HCI_READ_ENC_MODE_REQ                    ((dm_prim_t) ENUM_DM_HCI_READ_ENC_MODE_REQ)
#define DM_HCI_READ_CLASS_OF_DEVICE_REQ             ((dm_prim_t) ENUM_DM_HCI_READ_CLASS_OF_DEVICE_REQ)
#define DM_HCI_WRITE_CLASS_OF_DEVICE_REQ            ((dm_prim_t) ENUM_DM_HCI_WRITE_CLASS_OF_DEVICE_REQ)
#define DM_HCI_READ_VOICE_SETTING_REQ               ((dm_prim_t) ENUM_DM_HCI_READ_VOICE_SETTING_REQ)
#define DM_HCI_WRITE_VOICE_SETTING_REQ              ((dm_prim_t) ENUM_DM_HCI_WRITE_VOICE_SETTING_REQ)
#define DM_HCI_READ_AUTO_FLUSH_TIMEOUT_REQ          ((dm_prim_t) ENUM_DM_HCI_READ_AUTO_FLUSH_TIMEOUT_REQ)
#define DM_HCI_WRITE_AUTO_FLUSH_TIMEOUT_REQ         ((dm_prim_t) ENUM_DM_HCI_WRITE_AUTO_FLUSH_TIMEOUT_REQ)
#define DM_HCI_READ_NUM_BCAST_RETXS_REQ             ((dm_prim_t) ENUM_DM_HCI_READ_NUM_BCAST_RETXS_REQ)
#define DM_HCI_WRITE_NUM_BCAST_RETXS_REQ            ((dm_prim_t) ENUM_DM_HCI_WRITE_NUM_BCAST_RETXS_REQ)
#define DM_HCI_READ_HOLD_MODE_ACTIVITY_REQ          ((dm_prim_t) ENUM_DM_HCI_READ_HOLD_MODE_ACTIVITY_REQ)
#define DM_HCI_WRITE_HOLD_MODE_ACTIVITY_REQ         ((dm_prim_t) ENUM_DM_HCI_WRITE_HOLD_MODE_ACTIVITY_REQ)
#define DM_HCI_READ_TX_POWER_LEVEL_REQ              ((dm_prim_t) ENUM_DM_HCI_READ_TX_POWER_LEVEL_REQ)
#define DM_HCI_READ_SCO_FLOW_CON_ENABLE_REQ         ((dm_prim_t) ENUM_DM_HCI_READ_SCO_FLOW_CON_ENABLE_REQ)
#define DM_HCI_WRITE_SCO_FLOW_CON_ENABLE_REQ        ((dm_prim_t) ENUM_DM_HCI_WRITE_SCO_FLOW_CON_ENABLE_REQ)
#define DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ       ((dm_prim_t) ENUM_DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ)
#define DM_HCI_READ_LINK_SUPERV_TIMEOUT_REQ         ((dm_prim_t) ENUM_DM_HCI_READ_LINK_SUPERV_TIMEOUT_REQ)
#define DM_HCI_WRITE_LINK_SUPERV_TIMEOUT_REQ        ((dm_prim_t) ENUM_DM_HCI_WRITE_LINK_SUPERV_TIMEOUT_REQ)
#define DM_HCI_READ_NUM_SUPPORTED_IAC_REQ           ((dm_prim_t) ENUM_DM_HCI_READ_NUM_SUPPORTED_IAC_REQ)
#define DM_HCI_READ_CURRENT_IAC_LAP_REQ             ((dm_prim_t) ENUM_DM_HCI_READ_CURRENT_IAC_LAP_REQ)
#define DM_HCI_WRITE_CURRENT_IAC_LAP_REQ            ((dm_prim_t) ENUM_DM_HCI_WRITE_CURRENT_IAC_LAP_REQ)
#define DM_HCI_READ_PAGESCAN_PERIOD_MODE_REQ        ((dm_prim_t) ENUM_DM_HCI_READ_PAGESCAN_PERIOD_MODE_REQ)
#define DM_HCI_WRITE_PAGESCAN_PERIOD_MODE_REQ       ((dm_prim_t) ENUM_DM_HCI_WRITE_PAGESCAN_PERIOD_MODE_REQ)
#define DM_HCI_READ_PAGESCAN_MODE_REQ               ((dm_prim_t) ENUM_DM_HCI_READ_PAGESCAN_MODE_REQ)
#define DM_HCI_WRITE_PAGESCAN_MODE_REQ              ((dm_prim_t) ENUM_DM_HCI_WRITE_PAGESCAN_MODE_REQ)

/*-------------------------------------------------------------
Upstream primitives
---------------------------------------------------------------*/
#define DM_HCI_SET_EVENT_MASK_CFM               ((dm_prim_t)ENUM_DM_HCI_SET_EVENT_MASK_CFM)
#define DM_HCI_RESET_CFM                        ((dm_prim_t)ENUM_DM_HCI_RESET_CFM)
#define DM_HCI_SET_EVENT_FILTER_CFM             ((dm_prim_t)ENUM_DM_HCI_SET_EVENT_FILTER_CFM)
#define DM_HCI_FLUSH_CFM                        ((dm_prim_t)ENUM_DM_HCI_FLUSH_CFM)
#define DM_HCI_READ_PIN_TYPE_CFM                ((dm_prim_t)ENUM_DM_HCI_READ_PIN_TYPE_CFM)
#define DM_HCI_WRITE_PIN_TYPE_CFM               ((dm_prim_t)ENUM_DM_HCI_WRITE_PIN_TYPE_CFM)
#define DM_HCI_CREATE_NEW_UNIT_KEY_CFM          ((dm_prim_t)ENUM_DM_HCI_CREATE_NEW_UNIT_KEY_CFM)
#define DM_HCI_READ_STORED_LINK_KEY_CFM         ((dm_prim_t)ENUM_DM_HCI_READ_STORED_LINK_KEY_CFM)
#define DM_HCI_WRITE_STORED_LINK_KEY_CFM        ((dm_prim_t)ENUM_DM_HCI_WRITE_STORED_LINK_KEY_CFM)
#define DM_HCI_DELETE_STORED_LINK_KEY_CFM       ((dm_prim_t)ENUM_DM_HCI_DELETE_STORED_LINK_KEY_CFM)
#define DM_HCI_CHANGE_LOCAL_NAME_CFM            ((dm_prim_t)ENUM_DM_HCI_CHANGE_LOCAL_NAME_CFM)
#define DM_HCI_READ_LOCAL_NAME_CFM              ((dm_prim_t)ENUM_DM_HCI_READ_LOCAL_NAME_CFM)
#define DM_HCI_READ_CONN_ACCEPT_TIMEOUT_CFM     ((dm_prim_t)ENUM_DM_HCI_READ_CONN_ACCEPT_TIMEOUT_CFM)
#define DM_HCI_WRITE_CONN_ACCEPT_TIMEOUT_CFM    ((dm_prim_t)ENUM_DM_HCI_WRITE_CONN_ACCEPT_TIMEOUT_CFM)
#define DM_HCI_READ_PAGE_TIMEOUT_CFM            ((dm_prim_t)ENUM_DM_HCI_READ_PAGE_TIMEOUT_CFM)
#define DM_HCI_WRITE_PAGE_TIMEOUT_CFM           ((dm_prim_t)ENUM_DM_HCI_WRITE_PAGE_TIMEOUT_CFM)
#define DM_HCI_READ_SCAN_ENABLE_CFM             ((dm_prim_t)ENUM_DM_HCI_READ_SCAN_ENABLE_CFM)
#define DM_HCI_WRITE_SCAN_ENABLE_CFM            ((dm_prim_t)ENUM_DM_HCI_WRITE_SCAN_ENABLE_CFM)
#define DM_HCI_READ_PAGESCAN_ACTIVITY_CFM       ((dm_prim_t)ENUM_DM_HCI_READ_PAGESCAN_ACTIVITY_CFM)
#define DM_HCI_WRITE_PAGESCAN_ACTIVITY_CFM      ((dm_prim_t)ENUM_DM_HCI_WRITE_PAGESCAN_ACTIVITY_CFM)
#define DM_HCI_READ_INQUIRYSCAN_ACTIVITY_CFM    ((dm_prim_t)ENUM_DM_HCI_READ_INQUIRYSCAN_ACTIVITY_CFM)
#define DM_HCI_WRITE_INQUIRYSCAN_ACTIVITY_CFM   ((dm_prim_t)ENUM_DM_HCI_WRITE_INQUIRYSCAN_ACTIVITY_CFM)
#define DM_HCI_READ_AUTH_ENABLE_CFM             ((dm_prim_t)ENUM_DM_HCI_READ_AUTH_ENABLE_CFM)
#define DM_HCI_READ_ENC_MODE_CFM                ((dm_prim_t)ENUM_DM_HCI_READ_ENC_MODE_CFM)
#define DM_HCI_READ_CLASS_OF_DEVICE_CFM         ((dm_prim_t)ENUM_DM_HCI_READ_CLASS_OF_DEVICE_CFM)
#define DM_HCI_WRITE_CLASS_OF_DEVICE_CFM        ((dm_prim_t)ENUM_DM_HCI_WRITE_CLASS_OF_DEVICE_CFM)
#define DM_HCI_READ_VOICE_SETTING_CFM           ((dm_prim_t)ENUM_DM_HCI_READ_VOICE_SETTING_CFM)
#define DM_HCI_WRITE_VOICE_SETTING_CFM          ((dm_prim_t)ENUM_DM_HCI_WRITE_VOICE_SETTING_CFM)
#define DM_HCI_READ_AUTO_FLUSH_TIMEOUT_CFM      ((dm_prim_t)ENUM_DM_HCI_READ_AUTO_FLUSH_TIMEOUT_CFM)
#define DM_HCI_WRITE_AUTO_FLUSH_TIMEOUT_CFM     ((dm_prim_t)ENUM_DM_HCI_WRITE_AUTO_FLUSH_TIMEOUT_CFM)
#define DM_HCI_READ_NUM_BCAST_RETXS_CFM         ((dm_prim_t)ENUM_DM_HCI_READ_NUM_BCAST_RETXS_CFM)
#define DM_HCI_WRITE_NUM_BCAST_RETXS_CFM        ((dm_prim_t)ENUM_DM_HCI_WRITE_NUM_BCAST_RETXS_CFM)
#define DM_HCI_READ_HOLD_MODE_ACTIVITY_CFM      ((dm_prim_t)ENUM_DM_HCI_READ_HOLD_MODE_ACTIVITY_CFM)
#define DM_HCI_WRITE_HOLD_MODE_ACTIVITY_CFM     ((dm_prim_t)ENUM_DM_HCI_WRITE_HOLD_MODE_ACTIVITY_CFM)
#define DM_HCI_READ_TX_POWER_LEVEL_CFM          ((dm_prim_t)ENUM_DM_HCI_READ_TX_POWER_LEVEL_CFM)
#define DM_HCI_READ_SCO_FLOW_CON_ENABLE_CFM     ((dm_prim_t)ENUM_DM_HCI_READ_SCO_FLOW_CON_ENABLE_CFM)
#define DM_HCI_WRITE_SCO_FLOW_CON_ENABLE_CFM    ((dm_prim_t)ENUM_DM_HCI_WRITE_SCO_FLOW_CON_ENABLE_CFM)
#define DM_HCI_HOST_NUM_COMPLETED_PACKETS_CFM   ((dm_prim_t)ENUM_DM_HCI_HOST_NUM_COMPLETED_PACKETS_CFM)
#define DM_HCI_READ_LINK_SUPERV_TIMEOUT_CFM     ((dm_prim_t)ENUM_DM_HCI_READ_LINK_SUPERV_TIMEOUT_CFM)
#define DM_HCI_WRITE_LINK_SUPERV_TIMEOUT_CFM    ((dm_prim_t)ENUM_DM_HCI_WRITE_LINK_SUPERV_TIMEOUT_CFM)
#define DM_HCI_READ_NUM_SUPPORTED_IAC_CFM       ((dm_prim_t)ENUM_DM_HCI_READ_NUM_SUPPORTED_IAC_CFM)
#define DM_HCI_READ_CURRENT_IAC_LAP_CFM         ((dm_prim_t)ENUM_DM_HCI_READ_CURRENT_IAC_LAP_CFM)
#define DM_HCI_WRITE_CURRENT_IAC_LAP_CFM        ((dm_prim_t)ENUM_DM_HCI_WRITE_CURRENT_IAC_LAP_CFM)
#define DM_HCI_READ_PAGESCAN_PERIOD_MODE_CFM    ((dm_prim_t)ENUM_DM_HCI_READ_PAGESCAN_PERIOD_MODE_CFM)
#define DM_HCI_WRITE_PAGESCAN_PERIOD_MODE_CFM   ((dm_prim_t)ENUM_DM_HCI_WRITE_PAGESCAN_PERIOD_MODE_CFM)
#define DM_HCI_READ_PAGESCAN_MODE_CFM           ((dm_prim_t)ENUM_DM_HCI_READ_PAGESCAN_MODE_CFM)
#define DM_HCI_WRITE_PAGESCAN_MODE_CFM          ((dm_prim_t)ENUM_DM_HCI_WRITE_PAGESCAN_MODE_CFM)

/******************************************************************************
   Informational parameters
 *****************************************************************************/
/*-------------------------------------------------------------
Downstream primitives
---------------------------------------------------------------*/
#define DM_HCI_READ_LOCAL_VER_INFO_REQ          ((dm_prim_t) ENUM_DM_HCI_READ_LOCAL_VER_INFO_REQ)
#define DM_HCI_READ_LOCAL_SUPP_FEATURES_REQ     ((dm_prim_t) ENUM_DM_HCI_READ_LOCAL_SUPP_FEATURES_REQ)
#define DM_HCI_READ_COUNTRY_CODE_REQ            ((dm_prim_t) ENUM_DM_HCI_READ_COUNTRY_CODE_REQ)
#define DM_HCI_READ_BD_ADDR_REQ                 ((dm_prim_t) ENUM_DM_HCI_READ_BD_ADDR_REQ)

/*-------------------------------------------------------------
Upstream primitives
---------------------------------------------------------------*/
#define DM_HCI_READ_LOCAL_VER_INFO_CFM          ((dm_prim_t)ENUM_DM_HCI_READ_LOCAL_VER_INFO_CFM)
#define DM_HCI_READ_LOCAL_SUPP_FEATURES_CFM     ((dm_prim_t)ENUM_DM_HCI_READ_LOCAL_SUPP_FEATURES_CFM)
#define DM_HCI_READ_COUNTRY_CODE_CFM            ((dm_prim_t)ENUM_DM_HCI_READ_COUNTRY_CODE_CFM)
#define DM_HCI_READ_BD_ADDR_CFM                 ((dm_prim_t)ENUM_DM_HCI_READ_BD_ADDR_CFM)

/******************************************************************************
   Status parameters
 *****************************************************************************/
/*-------------------------------------------------------------
Downstream primitives
---------------------------------------------------------------*/
#define DM_HCI_READ_FAILED_CONTACT_COUNT_REQ    ((dm_prim_t) ENUM_DM_HCI_READ_FAILED_CONTACT_COUNT_REQ)
#define DM_HCI_RESET_FAILED_CONTACT_COUNT_REQ   ((dm_prim_t) ENUM_DM_HCI_RESET_FAILED_CONTACT_COUNT_REQ)
#define DM_HCI_GET_LINK_QUALITY_REQ             ((dm_prim_t) ENUM_DM_HCI_GET_LINK_QUALITY_REQ)
#define DM_HCI_READ_RSSI_REQ                    ((dm_prim_t) ENUM_DM_HCI_READ_RSSI_REQ)

/*-------------------------------------------------------------
Upstream primitives
---------------------------------------------------------------*/
#define DM_HCI_READ_FAILED_CONTACT_COUNT_CFM    ((dm_prim_t)ENUM_DM_HCI_READ_FAILED_CONTACT_COUNT_CFM)
#define DM_HCI_RESET_FAILED_CONTACT_COUNT_CFM   ((dm_prim_t)ENUM_DM_HCI_RESET_FAILED_CONTACT_COUNT_CFM)
#define DM_HCI_GET_LINK_QUALITY_CFM             ((dm_prim_t)ENUM_DM_HCI_GET_LINK_QUALITY_CFM)
#define DM_HCI_READ_RSSI_CFM                    ((dm_prim_t)ENUM_DM_HCI_READ_RSSI_CFM)

/******************************************************************************
   Testing primitives
 *****************************************************************************/
/*-------------------------------------------------------------
Downstream primitives
---------------------------------------------------------------*/
#define DM_HCI_ENABLE_DUT_MODE_REQ            ((dm_prim_t) ENUM_DM_HCI_ENABLE_DUT_MODE_REQ)

/*-------------------------------------------------------------
Upstream primitives
---------------------------------------------------------------*/
#define DM_HCI_ENABLE_DUT_MODE_CFM ((dm_prim_t)ENUM_DM_HCI_ENABLE_DUT_MODE_CFM)

/***********************************************************************
   ULP primitives
 ***********************************************************************/

/*-------------------------------------------------------------
Downstream primitives
---------------------------------------------------------------*/
#define DM_HCI_ULP_SET_EVENT_MASK_REQ ((dm_prim_t)ENUM_DM_HCI_ULP_SET_EVENT_MASK_REQ)
#define DM_HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_REQ ((dm_prim_t)ENUM_DM_HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_REQ)
#define DM_HCI_ULP_SET_RANDOM_ADDRESS_REQ ((dm_prim_t)ENUM_DM_HCI_ULP_SET_RANDOM_ADDRESS_REQ)
#define DM_HCI_ULP_SET_ADVERTISING_PARAMETERS_REQ ((dm_prim_t)ENUM_DM_HCI_ULP_SET_ADVERTISING_PARAMETERS_REQ)
#define DM_HCI_ULP_READ_ADVERTISING_CHANNEL_TX_POWER_REQ ((dm_prim_t)ENUM_DM_HCI_ULP_READ_ADVERTISING_CHANNEL_TX_POWER_REQ)
#define DM_HCI_ULP_SET_ADVERTISING_DATA_REQ ((dm_prim_t)ENUM_DM_HCI_ULP_SET_ADVERTISING_DATA_REQ)
#define DM_HCI_ULP_SET_SCAN_RESPONSE_DATA_REQ ((dm_prim_t)ENUM_DM_HCI_ULP_SET_SCAN_RESPONSE_DATA_REQ)
#define DM_HCI_ULP_SET_ADVERTISE_ENABLE_REQ ((dm_prim_t)ENUM_DM_HCI_ULP_SET_ADVERTISE_ENABLE_REQ)
#define DM_HCI_ULP_SET_SCAN_PARAMETERS_REQ ((dm_prim_t)ENUM_DM_HCI_ULP_SET_SCAN_PARAMETERS_REQ)
#define DM_HCI_ULP_SET_SCAN_ENABLE_REQ ((dm_prim_t)ENUM_DM_HCI_ULP_SET_SCAN_ENABLE_REQ)
#define DM_HCI_ULP_CREATE_CONNECTION_CANCEL_REQ ((dm_prim_t)ENUM_DM_HCI_ULP_CREATE_CONNECTION_CANCEL_REQ)
#define DM_HCI_ULP_READ_WHITE_LIST_SIZE_REQ ((dm_prim_t)ENUM_DM_HCI_ULP_READ_WHITE_LIST_SIZE_REQ)
#define DM_HCI_ULP_CLEAR_WHITE_LIST_REQ ((dm_prim_t)ENUM_DM_HCI_ULP_CLEAR_WHITE_LIST_REQ)
#define DM_HCI_ULP_ADD_DEVICE_TO_WHITE_LIST_REQ ((dm_prim_t)ENUM_DM_HCI_ULP_ADD_DEVICE_TO_WHITE_LIST_REQ)
#define DM_HCI_ULP_REMOVE_DEVICE_FROM_WHITE_LIST_REQ ((dm_prim_t)ENUM_DM_HCI_ULP_REMOVE_DEVICE_FROM_WHITE_LIST_REQ)
#define DM_HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_REQ ((dm_prim_t)ENUM_DM_HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_REQ)
#define DM_HCI_ULP_READ_CHANNEL_MAP_REQ ((dm_prim_t)ENUM_DM_HCI_ULP_READ_CHANNEL_MAP_REQ)
#define DM_HCI_ULP_READ_REMOTE_USED_FEATURES_REQ ((dm_prim_t)ENUM_DM_HCI_ULP_READ_REMOTE_USED_FEATURES_REQ)
#define DM_HCI_ULP_ENCRYPT_REQ ((dm_prim_t)ENUM_DM_HCI_ULP_ENCRYPT_REQ)
#define DM_HCI_ULP_RAND_REQ ((dm_prim_t)ENUM_DM_HCI_ULP_RAND_REQ)
#define DM_HCI_ULP_READ_SUPPORTED_STATES_REQ ((dm_prim_t)ENUM_DM_HCI_ULP_READ_SUPPORTED_STATES_REQ)
#define DM_HCI_ULP_RECEIVER_TEST_REQ ((dm_prim_t)ENUM_DM_HCI_ULP_RECEIVER_TEST_REQ)
#define DM_HCI_ULP_TRANSMITTER_TEST_REQ ((dm_prim_t)ENUM_DM_HCI_ULP_TRANSMITTER_TEST_REQ)
#define DM_HCI_ULP_TEST_END_REQ ((dm_prim_t)ENUM_DM_HCI_ULP_TEST_END_REQ)

/*-------------------------------------------------------------
Upstream primitives
---------------------------------------------------------------*/
#define DM_HCI_ULP_SET_EVENT_MASK_CFM ((dm_prim_t)ENUM_DM_HCI_ULP_SET_EVENT_MASK_CFM)
#define DM_HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_CFM ((dm_prim_t)ENUM_DM_HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_CFM)
#define DM_HCI_ULP_SET_RANDOM_ADDRESS_CFM ((dm_prim_t)ENUM_DM_HCI_ULP_SET_RANDOM_ADDRESS_CFM)
#define DM_HCI_ULP_SET_ADVERTISING_PARAMETERS_CFM ((dm_prim_t)ENUM_DM_HCI_ULP_SET_ADVERTISING_PARAMETERS_CFM)
#define DM_HCI_ULP_READ_ADVERTISING_CHANNEL_TX_POWER_CFM ((dm_prim_t)ENUM_DM_HCI_ULP_READ_ADVERTISING_CHANNEL_TX_POWER_CFM)
#define DM_HCI_ULP_SET_ADVERTISING_DATA_CFM ((dm_prim_t)ENUM_DM_HCI_ULP_SET_ADVERTISING_DATA_CFM)
#define DM_HCI_ULP_SET_SCAN_RESPONSE_DATA_CFM ((dm_prim_t)ENUM_DM_HCI_ULP_SET_SCAN_RESPONSE_DATA_CFM)
#define DM_HCI_ULP_SET_ADVERTISE_ENABLE_CFM ((dm_prim_t)ENUM_DM_HCI_ULP_SET_ADVERTISE_ENABLE_CFM)
#define DM_HCI_ULP_SET_SCAN_PARAMETERS_CFM ((dm_prim_t)ENUM_DM_HCI_ULP_SET_SCAN_PARAMETERS_CFM)
#define DM_HCI_ULP_SET_SCAN_ENABLE_CFM ((dm_prim_t)ENUM_DM_HCI_ULP_SET_SCAN_ENABLE_CFM)
#define DM_HCI_ULP_CREATE_CONNECTION_CANCEL_CFM ((dm_prim_t)ENUM_DM_HCI_ULP_CREATE_CONNECTION_CANCEL_CFM)
#define DM_HCI_ULP_READ_WHITE_LIST_SIZE_CFM ((dm_prim_t)ENUM_DM_HCI_ULP_READ_WHITE_LIST_SIZE_CFM)
#define DM_HCI_ULP_CLEAR_WHITE_LIST_CFM ((dm_prim_t)ENUM_DM_HCI_ULP_CLEAR_WHITE_LIST_CFM)
#define DM_HCI_ULP_ADD_DEVICE_TO_WHITE_LIST_CFM ((dm_prim_t)ENUM_DM_HCI_ULP_ADD_DEVICE_TO_WHITE_LIST_CFM)
#define DM_HCI_ULP_REMOVE_DEVICE_FROM_WHITE_LIST_CFM ((dm_prim_t)ENUM_DM_HCI_ULP_REMOVE_DEVICE_FROM_WHITE_LIST_CFM)
#define DM_HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_CFM ((dm_prim_t)ENUM_DM_HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_CFM)
#define DM_HCI_ULP_READ_CHANNEL_MAP_CFM ((dm_prim_t)ENUM_DM_HCI_ULP_READ_CHANNEL_MAP_CFM)
#define DM_HCI_ULP_READ_REMOTE_USED_FEATURES_CFM ((dm_prim_t)ENUM_DM_HCI_ULP_READ_REMOTE_USED_FEATURES_CFM)
#define DM_HCI_ULP_ENCRYPT_CFM ((dm_prim_t)ENUM_DM_HCI_ULP_ENCRYPT_CFM)
#define DM_HCI_ULP_RAND_CFM ((dm_prim_t)ENUM_DM_HCI_ULP_RAND_CFM)
#define DM_HCI_ULP_READ_SUPPORTED_STATES_CFM ((dm_prim_t)ENUM_DM_HCI_ULP_READ_SUPPORTED_STATES_CFM)
#define DM_HCI_ULP_RECEIVER_TEST_CFM ((dm_prim_t)ENUM_DM_HCI_ULP_RECEIVER_TEST_CFM)
#define DM_HCI_ULP_TRANSMITTER_TEST_CFM ((dm_prim_t)ENUM_DM_HCI_ULP_TRANSMITTER_TEST_CFM)
#define DM_HCI_ULP_TEST_END_CFM ((dm_prim_t)ENUM_DM_HCI_ULP_TEST_END_CFM)

#define DM_HCI_ULP_ADVERTISING_REPORT_IND ((dm_prim_t)ENUM_DM_HCI_ULP_ADVERTISING_REPORT_IND)
#define DM_HCI_ULP_CONNECTION_UPDATE_COMPLETE_IND ((dm_prim_t)ENUM_DM_HCI_ULP_CONNECTION_UPDATE_COMPLETE_IND)

/* end of #define of primitives */

typedef uint16_t                amp_link_id_t;

/* primitive type for dm */
typedef uint16_t                 dm_prim_t;

/* BLE connection parameters */
typedef struct
{
    uint16_t  conn_interval;
    uint16_t  conn_latency;
    uint16_t  supervision_timeout;
    uint8_t   clock_accuracy;  

}DM_ACL_BLE_CONN_PARAMS_T;


/*------------------------------------------------------------------------
 *
 *      REGISTER PRIMITIVES
 *
 *-----------------------------------------------------------------------*/

/* Note1:

        A value of 0 in the 'status' field of returned primitives indicates 
        success. All other values indicate failure. 
        The error codes are defined in the Bluetooth HCI specification.
        (www.bluetooth.com para. 6.1 in v1.0b)."
*/

 /*----------------------------------------------------------------------------*
 * PURPOSE
 *      Standard command complete
 *
 *----------------------------------------------------------------------------*/

typedef struct
{
    dm_prim_t           type;               /* Prim ID */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* Success or failure - See Note1 */
} DM_HCI_STANDARD_COMMAND_CFM_T;/* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */ 

typedef struct
{
    dm_prim_t           type;               /* Prim ID */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* Success or failure - See Note1 */
    BD_ADDR_T           bd_addr;            /* Bluetooth address */
} DM_HCI_BD_ADDR_COMMAND_CFM_T;/* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */

typedef struct
{
    dm_prim_t           type;               /* Prim ID */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* Success or failure - See Note1 */
    TYPED_BD_ADDR_T     addrt;              /* Bluetooth address and type */
} DM_HCI_TYPED_BD_ADDR_COMMAND_CFM_T;/* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */

typedef struct
{
    dm_prim_t           type;               /* Prim ID */
    BD_ADDR_T           bd_addr;            /* Bluetooth address */
} DM_COMMON_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Register request to AM Interface, registering a destination phandle
 *      for upstream application primitives.
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t               type;           /* Always DM_AM_REGISTER_REQ */
    phandle_t               phandle;        /* protocol handle */
    uint16_t                flags;          /* Reserved - set to 0 */
} DM_AM_REGISTER_REQ_T;

typedef struct
{   
    dm_prim_t               type;           /* Always DM_AM_REGISTER_WRONG_API_REQ */
    phandle_t               phandle;        /* protocol handle */
} DM_AM_REGISTER_WRONG_API_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Confirmation that the registration request has been received.
 *  
 *  RETURNS
 *      phandle specified in DM_AM_REGISTER_REQ.
 *      Bluestack version. MSB is major version, LSB is minor version.
 *      Bluestack patchlevel. This is incremented for every release
 *      with a given Bluestack version number and reset to zero for new
 *      Bluestack version numbers.
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t               type;           /* Always DM_AM_REGISTER_CFM */
    phandle_t               phandle;        /* destination phandle */
    uint16_t                version;        /* Bluestack version */
    uint16_t                patch_level;    /* Bluestack patch level */
} DM_AM_REGISTER_CFM_T;

typedef struct
{
    dm_prim_t               type;     /* Always DM_AM_REGISTER_WRONG_API_CFM */
    phandle_t               phandle;  /* destination phandle */
} DM_AM_REGISTER_WRONG_API_CFM_T;

/*---------------------------------------------------------------------------*
 *  PURPOSE
 *      Set Bluetooth version. On successful completion, status will
 *      be set to HCI_SUCCESS == 0 and version will be set to the
 *      BT version requested. On unsuccessful completion, status will
 *      be set to an HCI error code and version will be set to the
 *      current version.
 *
 *      You can use this command to read the current BT version by
 *      setting version to BT_VERSION_CURRENT in the command primitive.
 *      (In this case a failure indication in the status parameter would
 *      be unusual and would indicate that the current version settings
 *      are no longer supported by the device.)
 *---------------------------------------------------------------------------*/

#define BT_VERSION_CURRENT          0x00
#define BT_VERSION_2p0              0x01
#define BT_VERSION_2p1              0x02
#define BT_VERSION_3p0              0x03
#define BT_VERSION_4p0              0x04
#define BT_VERSION_MAX              BT_VERSION_4p0

typedef struct
{
    dm_prim_t               type;           /* Always DM_SET_BT_VERSION_REQ */
    uint8_t                 version;        /* Requested version */
} DM_SET_BT_VERSION_REQ_T;

typedef struct
{
    dm_prim_t               type;           /* Always DM_SET_BT_VERSION_CFM */
    phandle_t               phandle;        /* destination phandle */
    uint8_t                 version;        /* Requested version if success, */
    uint8_t                 status;         /* or current version if failure */
} DM_SET_BT_VERSION_CFM_T;

/*------------------------------------------------------------------------
 *  PURPOSE
 *  Indication sent to phandle registered with DM_AM_REGISTER_REQ when
 *  Bluestack receives a message to which it cannot respond in the
 *  usual fashion. This might be because the message is not recognised
 *  or because there is no obvious phandle to use for the reply. In
 *  these cases, Bluestack will send DM_BAD_MESSAGE_IND.
 *-----------------------------------------------------------------------*/

/* Reasons */
#define DM_BAD_MESSAGE_NO_PHANDLE       ((uint16_t)0x0001)
#define DM_BAD_MESSAGE_NOT_RECOGNISED   ((uint16_t)0x0002)

typedef struct
{
    dm_prim_t               type;           /* Always DM_BAD_MESSAGE_IND */
    phandle_t               phandle;        /* destination phandle */
    uint16_t                reason;         /* What went wrong */
    uint8_t                 protocol;       /* L2CAP_PRIM/RFCOMM_PRIM/etc */
    uint16_t                request_type;   /* Enumerated type within protocol*/
    uint16_t                message_length; /* Length of returned message */
    uint8_t                 *message;       /* Returned message */
} DM_BAD_MESSAGE_IND_T;

/*---------------------------------------------------------------------------*
 *  Provides an API to allow an application to able to specify the connection interval, 
 *  slave latency, etc, that the local device uses when creating connections in the central role
 *
 *  RETURNS
 *     A CFM
 *---------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t type;                 /* DM_SET_BLE_CONNECTION_PARAMETERS_REQ */
    uint16_t scan_interval;         /* These six parameters correspond to */
    uint16_t scan_window;           /* those in HCI_ULP_CREATE_CONNECTION_T */
    uint16_t conn_interval_min;     /* and are used to fill that primitive */
    uint16_t conn_interval_max;     /* when it is created. If they are not */
    uint16_t conn_latency;          /* set explicitly here then the Device */
    uint16_t supervision_timeout;   /* Manager will choose defaults. */
    uint16_t conn_attempt_timeout;  /* Equivalent of Page Timeout in BR/EDR */
    uint16_t adv_interval_min;      /* Used when setting parameters for */
    uint16_t adv_interval_max;      /* undirected connect-as-slave advertising*/
    uint16_t conn_latency_max;        /* The 3 parameters are used to define the conditions */
    uint16_t supervision_timeout_min; /* to accept a  */
    uint16_t supervision_timeout_max; /* L2CA_CONNECTION_PAR_UPDATE_REQ_T*/
} DM_SET_BLE_CONNECTION_PARAMETERS_REQ_T;
 
typedef struct
{
    dm_prim_t type;
    phandle_t phandle;
    uint8_t status;
} DM_SET_BLE_CONNECTION_PARAMETERS_CFM_T;




/*------------------------------------------------------------------------
 *
 *      SYNCHRONOUS CONNECTION INTERFACE PRIMITIVES
 *      These primitives are all sent to the DM_IFACEQUEUE input queue.
 *
 *-----------------------------------------------------------------------*/

typedef struct
{
    dm_prim_t               type;
    phandle_t               phandle;
    uint16_t                pv_cbarg;
    hci_connection_handle_t handle;
    uint8_t                 status_or_reason;
} DM_SYNC_UPSTREAM_WITH_HANDLE_COMMON_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */

typedef struct
{
    dm_prim_t                   type;
    hci_connection_handle_t     handle;
} DM_SYNC_DOWNSTREAM_WITH_HANDLE_COMMON_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */

typedef struct
{

    dm_prim_t               type;           /* Always DM_SYNC_REGISTER_REQ */
    phandle_t               phandle;        /* protocol handle */

    /* Opaque argument to be returned in all upstream SCO primitives, can be
     * used by the destination task for discrimination purposes.
     * The Bluelab connection library uses this to store the application
     * task and routes messages accordingly.
     */
    uint16_t                pv_cbarg;
    uint16_t                flags;

    /* Stream sizes - only applicable if stream flag set */
    STREAM_BUFFER_SIZES_T   streams;
} DM_SYNC_REGISTER_REQ_T;

typedef struct
{
    dm_prim_t               type;
    phandle_t               phandle;
    uint16_t                pv_cbarg;
} DM_SYNC_REGISTER_CFM_T;

typedef struct
{
    dm_prim_t               type;           /* Always DM_SYNC_UNREGISTER_REQ */
    phandle_t               phandle;        /* client phandle */
    uint16_t                pv_cbarg;
} DM_SYNC_UNREGISTER_REQ_T;

#define DM_SYNC_UNREGISTER_SUCCESS          ((uint8_t)0)
#define DM_SYNC_UNREGISTER_NOT_FOUND        ((uint8_t)1)
#define DM_SYNC_UNREGISTER_BUSY             ((uint8_t)2)
typedef struct
{
    dm_prim_t               type;           /* Always DM_SYNC_UNREGISTER_CFM */
    phandle_t               phandle;        /* client phandle */
    uint16_t                pv_cbarg;
    uint8_t                 status;
} DM_SYNC_UNREGISTER_CFM_T;

typedef struct
{
    uint16_t                max_latency;
    uint16_t                retx_effort;
    hci_pkt_type_t          packet_type;    /* Set to zero for default */
    uint16_t                voice_settings;
    uint32_t                tx_bdw;
    uint32_t                rx_bdw;
} DM_SYNC_CONFIG_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */

typedef union
{
    DM_SYNC_CONFIG_T    *config;        /* Pointer to config structure. */
    uint16_t            *conftab;       /* Not currently used. */
} DM_SYNC_CONFIG_UNION_T;

typedef struct
{
    dm_prim_t               type;           /* Always DM_SYNC_CONNECT_REQ */
    phandle_t               phandle;        /* client phandle */
    uint16_t                pv_cbarg;
    BD_ADDR_T               bd_addr;        /* Bluetooth device address */
    uint16_t                length;         /* Reserved. Shall be set to zero.*/
    DM_SYNC_CONFIG_UNION_T  u;
} DM_SYNC_CONNECT_REQ_T;

typedef struct
{
    dm_prim_t               type;           /* Always DM_SYNC_CONNECT_CFM */
    phandle_t               phandle;        /* client phandle */
    uint16_t                pv_cbarg;
    hci_connection_handle_t handle;         /* HCI Connection Handle */
    uint8_t                 status;         /* Success or failure - See Note1 */
    BD_ADDR_T               bd_addr;        /* Bluetooth device address */
    uint8_t                 link_type;
    uint8_t                 tx_interval;
    uint8_t                 wesco;
    uint16_t                rx_packet_length;
    uint16_t                tx_packet_length;
    uint8_t                 air_mode;
} DM_SYNC_CONNECT_COMPLETE_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */
typedef DM_SYNC_CONNECT_COMPLETE_T DM_SYNC_CONNECT_CFM_T;
typedef DM_SYNC_CONNECT_COMPLETE_T DM_SYNC_CONNECT_COMPLETE_IND_T;

typedef struct
{
    dm_prim_t               type;           /* Always DM_SYNC_CONNECT_IND */
    phandle_t               phandle;        /* client phandle */
    uint16_t                pv_cbarg;
    BD_ADDR_T               bd_addr;        /* Bluetooth device address */
    uint8_t                 link_type;      /* HCI_LINK_TYPE_SCO/ESCO */
    uint24_t                dev_class;
} DM_SYNC_CONNECT_IND_T;

typedef struct
{
    dm_prim_t               type;           /* Always DM_SYNC_CONNECT_RSP */
    BD_ADDR_T               bd_addr;        /* Bluetooth device address */
    uint8_t                 response;       /* HCI_SUCCESS or appropriate HCI_ERROR_ code */
    DM_SYNC_CONFIG_T        config;
} DM_SYNC_CONNECT_RSP_T;

/* If you just want to change the packet type then set max_latency
   and retx_effort to 0. This is the equivalent of sending
   DM_HCI_CHANGE_CONN_PKT_TYPE_REQ. */
typedef struct
{
    dm_prim_t                   type;       /* Always DM_SYNC_RENEGOTIATE_REQ */
    hci_connection_handle_t     handle;     /* HCI Connection Handle */
    uint16_t                    length;     /* Reserved. Shall be set to zero.*/
    DM_SYNC_CONFIG_UNION_T      u;
} DM_SYNC_RENEGOTIATE_REQ_T;

typedef struct
{
    dm_prim_t                   type;       /* Always DM_SYNC_RENEGOTIATE_IND */
    phandle_t                   phandle;    /* destination phandle */
    uint16_t                    pv_cbarg;
    hci_connection_handle_t     handle;     /* HCI Connection Handle */
    hci_return_t                status;

    /* If the following parameters are all 0 then they should be ignored. */
    uint8_t                     tx_interval;
    uint8_t                     wesco;
    uint16_t                    rx_packet_length;
    uint16_t                    tx_packet_length;
} DM_SYNC_RENEGOTIATE_INDCFM_T; /* autogen_makefile_ignore_this */

typedef DM_SYNC_RENEGOTIATE_INDCFM_T DM_SYNC_RENEGOTIATE_IND_T;
typedef DM_SYNC_RENEGOTIATE_INDCFM_T DM_SYNC_RENEGOTIATE_CFM_T;

typedef struct
{
    dm_prim_t                   type;           /* Always DM_SYNC_DISCONNECT_REQ */
    hci_connection_handle_t     handle;         /* HCI Connection Handle */
    hci_reason_t                reason;         /* reason for disconnection */
} DM_SYNC_DISCONNECT_REQ_T;

typedef struct
{
    dm_prim_t                   type;           /* Always DM_SYNC_DISCONNECT_IND */
    phandle_t                   phandle;        /* destination phandle */
    uint16_t                    pv_cbarg;
    hci_connection_handle_t     handle;         /* HCI Connection Handle */
    hci_reason_t                reason;         /* reason for disconnection */
    BD_ADDR_T                   bd_addr;        /* Bluetooth device address */
} DM_SYNC_DISCONNECT_IND_T;

typedef struct
{
    dm_prim_t                   type;           /* Always DM_SYNC_DISCONNECT_CFM */
    phandle_t                   phandle;        /* destination phandle */
    uint16_t                    pv_cbarg;
    hci_connection_handle_t     handle;         /* HCI Connection Handle */
    uint8_t                     status;         /* Success or failure - See Note1 */
    BD_ADDR_T                   bd_addr;        /* Bluetooth device address */
} DM_SYNC_DISCONNECT_CFM_T;

/*----------------------------------------------------------------------------*
 *
 *  SECURITY MANAGEMENT INTERFACE
 *
 *  Downstream primitives are sent to the DM_IFACEQUEUE, upstream primitives
 *  are sent either to the registered AM queue (see DM_AM_REGISTER_REQ), or to
 *  the queue of the protocol layer requesting access (for DM_SM_ACCESS_REQ).
 *  There is a private L2CAP/SM interface for controlling security of received
 *  connectionless packets.
 *
 *----------------------------------------------------------------------------*/

typedef uint8_t dm_security_mode_t;
#define SEC_MODE0_OFF                   ((dm_security_mode_t) 0x00)
#define SEC_MODE1_NON_SECURE            ((dm_security_mode_t) 0x01)
#define SEC_MODE2_SERVICE               ((dm_security_mode_t) 0x02)
#define SEC_MODE3_LINK                  ((dm_security_mode_t) 0x03)
#define SEC_MODE4_SSP                   ((dm_security_mode_t) 0x04)
/*
   SEC_MODE2a_DEVICE is a custom security mode that authenticates on a per
   device basis using the Security Manager's device database.
   On starting a new service (incoming and outgoing), the device will be
   authenticated unless it is marked as 'trusted' in the device database
   (see DM_SM_ADD_DEVICE_REQ).
*/
#define SEC_MODE2a_DEVICE               ((dm_security_mode_t) 0x80)

/* Security level settings
 *
 * SECL_(IN|OUT)_AUTHORISATION (both mode 4 and legacy)
 * Results in a DM_SM_AUTHORISE_IND on a connection attempt from a
 * device that is not marked as 'trusted' in the device database.
 *
 * SECL_(IN|OUT)_AUTHENTICATION (legacy)
 * For connections where at least one of the devices is not in mode 4
 * (i.e. it's a legacy device) then the legacy pairing procedure
 * should be used. The users may have to enter PINs.
 *
 * SECL_(IN|OUT)_ENCRYPTION (legacy)
 * This setting indicates that the link should be encrypted after legacy
 * authentication.
 *
 * SECL4_(IN|OUT)_SSP (mode 4)
 * For connections where both devices are in mode 4, Secure Simple Pairing
 * is used to generate a shared link key and the link is then encrypted. Only
 * SDP is allowed not to have these bits set. All other services must set
 * them to achieve compliance.
 *
 * SECL4_(IN|OUT)_MITM (mode 4)
 * For connections where both devices are in mode 4, Secure Simple Pairing
 * is used to generate a shared authenticated link key and the link is
 * then encrypted. If you set this then you should also set
 * SECL4_(IN|OUT)_SSP as it is implied.
 *
 * SECL4_(IN|OUT)_NO_LEGACY
 * Don't support legacy security procedures. This is a mode4-only setting.
 * By setting this, you are preventing the device from using legacy pairing
 * procedures to authenticate with remote devices that do not support
 * simple pairing. If you set this then the device will be
 * incapable of pairing using legacy procedures and so will reject all
 * connection attempts with legacy devices where security is required.
 * If you set this then you may severely reduce interoperability.
 *
 */

typedef uint16_t dm_security_level_t;
#define SECL_NONE                           ((dm_security_level_t) 0x0000)
#define SECL_IN_AUTHORISATION               ((dm_security_level_t) 0x0001)
#define SECL_IN_AUTHENTICATION              ((dm_security_level_t) 0x0002)
#define SECL_IN_ENCRYPTION                  ((dm_security_level_t) 0x0004)
#define SECL_OUT_AUTHORISATION              ((dm_security_level_t) 0x0008)
#define SECL_OUT_AUTHENTICATION             ((dm_security_level_t) 0x0010)
#define SECL_OUT_ENCRYPTION                 ((dm_security_level_t) 0x0020)

#define SECL4_IN_SSP                        ((dm_security_level_t) 0x0100)
#define SECL4_IN_MITM                       ((dm_security_level_t) 0x0200)
#define SECL4_IN_NO_LEGACY                  ((dm_security_level_t) 0x0400)
#define SECL4_OUT_SSP                       ((dm_security_level_t) 0x0800)
#define SECL4_OUT_MITM                      ((dm_security_level_t) 0x1000)
#define SECL4_OUT_NO_LEGACY                 ((dm_security_level_t) 0x2000)

/* 
 * Security Levels defined in Vol 3, Part C, Section 5.2.2.5 of BT2.1 Spec
 *
 * N.B. These only affect authentication and encryption. Authorisation is
 *      separate.
 */
/* No security - permitted for SDP only */
#define SECL4_IN_LEVEL_0    SECL_NONE
#define SECL4_OUT_LEVEL_0   SECL_NONE
#define SECL4_LEVEL_0       (SECL4_IN_LEVEL_0 | SECL4_OUT_LEVEL_0)
/* Low security */
#define SECL4_IN_LEVEL_1    SECL4_IN_SSP
#define SECL4_OUT_LEVEL_1   SECL4_OUT_SSP
#define SECL4_LEVEL_1       (SECL4_IN_LEVEL_1 | SECL4_OUT_LEVEL_1)
/* Medium security */
#define SECL4_IN_LEVEL_2    (SECL4_IN_SSP | SECL_IN_AUTHENTICATION \
                                                        | SECL_IN_ENCRYPTION)
#define SECL4_OUT_LEVEL_2   (SECL4_OUT_SSP | SECL_OUT_AUTHENTICATION \
                                                        | SECL_OUT_ENCRYPTION)
#define SECL4_LEVEL_2       (SECL4_IN_LEVEL_2 | SECL4_OUT_LEVEL_2)
/* High security */
#define SECL4_IN_LEVEL_3    (SECL4_IN_SSP | SECL4_IN_MITM \
                               | SECL_IN_AUTHENTICATION | SECL_IN_ENCRYPTION)
#define SECL4_OUT_LEVEL_3    (SECL4_OUT_SSP | SECL4_OUT_MITM \
                               | SECL_OUT_AUTHENTICATION | SECL_OUT_ENCRYPTION)
#define SECL4_LEVEL_3       (SECL4_IN_LEVEL_3 | SECL4_OUT_LEVEL_3)
/* 
 * Security level combinations useful internally
 */
/* Incoming security settings */
#define SECL_IN    (SECL_IN_AUTHORISATION | SECL_IN_AUTHENTICATION      \
        | SECL_IN_ENCRYPTION | SECL4_IN_SSP | SECL4_IN_MITM \
        | SECL4_IN_NO_LEGACY)

/* Outgoing security settings */
#define SECL_OUT   (SECL_OUT_AUTHORISATION | SECL_OUT_AUTHENTICATION    \
        | SECL_OUT_ENCRYPTION | SECL4_OUT_SSP | SECL4_OUT_MITM          \
        | SECL4_OUT_NO_LEGACY)

/* Authorisation required (either in or out) */
#define SECL_AUTHORISATION (SECL_IN_AUTHORISATION | SECL_OUT_AUTHORISATION)

/* Authentication required (legacy) */
#define SECL_AUTHENTICATION (SECL_IN_AUTHENTICATION \
        | SECL_OUT_AUTHENTICATION | SECL_IN_ENCRYPTION | SECL_OUT_ENCRYPTION)

/* Encryption required (legacy) */
#define SECL_ENCRYPTION (SECL_IN_ENCRYPTION | SECL_OUT_ENCRYPTION)

/* Simple Secure Pairing required (either in or out) */
#define SECL4_SSP (SECL4_IN_SSP | SECL4_OUT_SSP | SECL4_IN_MITM \
                                                            | SECL4_OUT_MITM)

/* MITM protection required in mode 4 (either in or out) */
#define SECL4_MITM (SECL4_IN_MITM | SECL4_OUT_MITM)

/* Legacy pairing prohibited in mode 4 (either in or out) */
#define SECL4_NO_LEGACY (SECL4_IN_NO_LEGACY | SECL4_OUT_NO_LEGACY)

/* Mask of legacy mode settings */
#define SECL_MODE_LEGACY                ((dm_security_level_t) 0x007F)

/* The default security level, applied in security modes 2, 3 and 4 when no
 * specific security level has been registered for a service. This is set to
 * require authentication and encryption of both incoming and outgoing
 * connections. It also requires authorisation of incoming connections.
 * It may be overridden at run-time using the DM_SET_DEFAULT_SECURITY_REQ
 * primitive.
 */
#define SECL_DEFAULT (SECL_IN_AUTHORISATION | SECL4_LEVEL_2)

typedef struct
{
    DM_SM_SERVICE_T service;
    TYPED_BD_ADDR_T addrt;
} DM_CONNECTION_T; /* autogen_makefile_ignore_this */

typedef struct
{
    DM_CONNECTION_T connection;
    bool_t incoming;
} DM_CONNECTION_SETUP_T; /* autogen_makefile_ignore_this */

/* Standard security protocol identifiers provided by Bluestack. Higher
 * multiplexing protocols must define their own identifiers, from the range
 * SEC_PROTOCOL_USER and above.
 */
#define SEC_PROTOCOL_NONE               ((dm_protocol_id_t) 0x0000)
#define SEC_PROTOCOL_SM_BONDING         ((dm_protocol_id_t) 0x0001)
#define SEC_PROTOCOL_L2CAP              ((dm_protocol_id_t) 0x0002)
#define SEC_PROTOCOL_RFCOMM             ((dm_protocol_id_t) 0x0003)
/* Higher layers base their protocol IDs from here */
#define SEC_PROTOCOL_USER               ((dm_protocol_id_t) 0x8000)

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Registers the security requirements for access to a service or a
 *      multiplexing protocol layer when the Security Manager is in security
 *      mode 2 or 3. The registered security level is applied to all incoming
 *      connections on the specified 'channel', and optionally to all outgoing
 *      connections on that channel if 'outgoing_ok' is TRUE.
 *
 *      It does not always make sense to apply security to outgoing connections,
 *      for example a local RFCOMM server channel is only valid for incoming
 *      connections. An L2CAP PSM, however, may be valid in both directions.
 *      Therefore use the 'outgoing_ok' flag to specify whether security
 *      should be applied to outgoing connections as well as incoming.
 *      When the local and remote L2CAP PSMs are different, the local PSM should
 *      be passed to the Security Manager.  For outgoing connections where the
 *      channel may be different for each remote device, security requirements
 *      may be registered using the DM_SM_REGISTER_OUTGOING_REQ primitive.
 *
 *      NOTE
 *          The Security Manager enforces a default security level in modes 2,
 *          3 and 4 given by the SECL_DEFAULT definition. Any access request
 *          for which there is no registered security level will be subject to
 *          the default security. Therefore it is recommended that certain
 *          services and protocols are always registered with no security,
 *          for example, SDP and RFCOMM. In Security mode 4, no service, other
 *          than SDP, should be registered with no security.
 *
 *          If 'channel' is set to 0 then the security manager will choose
 *          a channel itself. It starts from 0xFFFF and works its way down
 *          until it finds the first unused channel for the protocol in
 *          question. This is currently only used for profiles that are
 *          RFCOMM clients but not servers.
 *
 *      Some security examples are presented:
 *
 *      Eg 1 - A TCS Terminal registers with 'protocol_id' = SEC_PROTOCOL_L2CAP,
 *      'channel' = 7 (cordless telephony PSM), and 'outgoing_ok' = TRUE. Then
 *      L2CAP will request access from the Security Manager for all connections,
 *      incoming and outgoing, on PSM 7.
 *
 *      Eg 2 - A headset profile might register with 'protocol_id' =
 *      SEC_PROTOCOL_RFCOMM, 'channel' = the RFCOMM server channel number of the
 *      headset service, and 'outgoing_ok' = FALSE. Then RFCOMM will request
 *      access from the Security Manager for all incoming connections on that
 *      server channel. Access requests for outgoing connections will be
 *      subjected to the default security level.
 *
 *      Eg 3 - As for (Eg 2), but whenever the profile makes an outgoing
 *      connection to an Audio Gateway it first performs service discovery to
 *      obtain the server channel number on the AG device, then registers using
 *      the DM_SM_REGISTER_OUTGOING_REQ primitive with 'protocol_id' =
 *      SEC_PROTOCOL_RFCOMM, 'channel' = remote server channel, 'bd_addr' =
 *      device address of the AG. When the outgoing connection is started,
 *      RFCOMM will request access from the Security Manager which will apply
 *      the security level registered for the outgoing connection. Incoming
 *      connections proceed as in (Eg 2).
 *
 *      Eg 4 - An application can specify the security requirements for access
 *      to RFCOMM by sending DM_SM_REGISTER_REQ with 'protocol_id' =
 *      SEC_PROTOCOL_L2CAP, 'channel' = 3, and 'outgoing_ok' = TRUE. Then L2CAP
 *      will request access from the Security Manager for all new RFCOMM
 *      connections.
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;           /* Always DM_SM_REGISTER_REQ */
    phandle_t           phandle;        /* destination phandle */
    uint16_t            context;        /* Opaque context */
    DM_SM_SERVICE_T     service;        /* Protocol and channel */
    bool_t              outgoing_ok;    /* TRUE if applies to outgoing */
    dm_security_level_t security_level; /* Level of security to be applied */
} DM_SM_REGISTER_REQ_T;

typedef DM_SM_REGISTER_REQ_T DM_SM_REGISTER_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Registers the security requirements for outgoing connections to the
 *      specified protocol and channel on the specified remote device. This is
 *      typically used to control security when connecting to a remote RFCOMM
 *      server channel.
 *
 *      See the description of DM_SM_REGISTER_REQ (above) for more details.
 /
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;           /* Always DM_SM_REGISTER_OUTGOING_REQ */
    phandle_t           phandle;        /* destination phandle */
    uint16_t            context;        /* Opaque context */
    DM_CONNECTION_T     connection;     /* Protocol, channel and BD_ADDR */
    dm_security_level_t security_level; /* Level of security to be applied */
} DM_SM_REGISTER_OUTGOING_REQ_T;

typedef DM_SM_REGISTER_OUTGOING_REQ_T DM_SM_REGISTER_OUTGOING_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Unregisters the security requirements for a service previously
 *      registered with DM_SM_REGISTER_REQ. 'protocol_id' and 'channel' are the
 *      same as in the register request.
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;           /* Always DM_SM_UNREGISTER_REQ */
    phandle_t           phandle;
    uint16_t            context;
    DM_SM_SERVICE_T     service;        /* Protocol and channel */
} DM_SM_UNREGISTER_REQ_T;

typedef DM_SM_UNREGISTER_REQ_T DM_SM_UNREGISTER_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Unregisters the security requirements for a service previously
 *      registered with DM_SM_REGISTER_OUTGOING_REQ. 'bd_addr', 'protocol_id'
 *      and 'remote_channel' are the same as in the register request.
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;           /* Always DM_SM_UNREGISTER_OUTGOING_REQ */
    phandle_t           phandle;
    uint16_t            context;
    DM_CONNECTION_T     connection;     /* Protocol, channel and BD_ADDR */
} DM_SM_UNREGISTER_OUTGOING_REQ_T;

typedef DM_SM_UNREGISTER_OUTGOING_REQ_T DM_SM_UNREGISTER_OUTGOING_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Request from a protocol layer (e.g. L2CAP, RFCOMM, or a higher layer) to
 *      determine if the remote device 'bd_addr' is to be granted access to that
 *      protocol layer's channel.
 *      The Security Manager will search its service and device databases, may
 *      perform some security procedures, and may request authorisation from
 *      the application. The result is returned in a DM_SM_ACCESS_CFM_T
 *      primitive.
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t               type;               /* Always DM_SM_ACCESS_REQ */
    phandle_t               phandle;            /* phandle for the response */

    /* Protocol, channel, BD_ADDR and incoming/outgoing. */
    DM_CONNECTION_SETUP_T   conn_setup;

    /* An opaque value supplied by the requesting entity, and returned in the
     * access confirm primitive. It may be used to match a confirm to the
     * correct request.
     */
    uint32_t                context;
} DM_SM_ACCESS_REQ_T;


/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Response to an access request from a protocol layer. This is sent to
 *      the phandle that was sent in the DM_SM_ACCESS_REQ primitive.
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t               type;               /* Always DM_SM_ACCESS_CFM */
    phandle_t               phandle;            /* Destination phandle */

    /* The parameters from the access request */
    DM_CONNECTION_SETUP_T   conn_setup;

    /* Context value supplied in the request */
    uint32_t                context;

    /* Anything other than HCI_SUCCESS is a failure */
    hci_return_t            status;
} DM_SM_ACCESS_CFM_T;

typedef DM_SM_ACCESS_CFM_T DM_SM_ACCESS_IND_T;

typedef struct
{
    dm_prim_t               type;
    phandle_t               phandle;
    TYPED_BD_ADDR_T         addrt;
    uint16_t                context;
    bool_t                  verify;
    uint16_t                length;
    uint8_t                *data;
    uint8_t                 status;
} DM_SM_DATA_SIGN_REQ_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */

typedef DM_SM_DATA_SIGN_REQ_T DM_SM_DATA_SIGN_COMMON_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */
typedef DM_SM_DATA_SIGN_COMMON_T DM_SM_DATA_SIGN_CFM_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */

typedef struct
{
    dm_prim_t               type;
    phandle_t               phandle;
    bool_t                  local_csrk;
    TYPED_BD_ADDR_T         addrt;
    uint32_t                counter;
} DM_SM_CSRK_COUNTER_CHANGE_IND_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Initialise Security Manager. The result is returned in a 
 *      DM_SM_INIT_CFM primitive.
 *
 *      It is recommended that this is used once as part of the device
 *      initialisation, but it can be used multiple times using the 'options'
 *      field to specify which parameters are being initialised.
 *      
 *      It is recommended that the security mode is changed only when the
 *      system is not in the process of creating or accepting any new
 *      connections. A device conforming to Bluetooth 2.0 or earlier must
 *      never enter Security Mode 4. A device conforming to Bluetooth 2.1
 *      or later must enter Security Mode 4 before making itself connectable
 *      or discoverable and before initiating any connections. It is
 *      recommended that a device conforming to Bluetooth 2.1 or later
 *      should enter Security Mode 4 as part of its initialisation process.
 *
 *      Mode 0 is for use by applications which perform their own security
 *      procedures. The service database is unused in this mode, whereas the
 *      device database is used only for link keys. Unknown link keys result in
 *      a link key request being forwarded to the application. This is the
 *      default mode at startup.
 *
 *      In mode 1, the Security Manager automatically accepts all access
 *      requests without initiating authentication, encryption, or
 *      authorisation procedures. Any attempt by an application to perform
 *      authentication or encryption procedures will be rejected. The Security
 *      Manager will respond to peer-invoked security procedures as for mode 2.
 *
 *      In mode 2, access requests are subject to the security requirements
 *      registered in the service and device databases. The Security Manager
 *      will respond to link key requests using the device database - if a link
 *      key is unknown, the request is forwarded to the application. PIN code
 *      requests are forwarded to the application. The application may invoke
 *      security procedures in this mode.
 *
 *      In mode 3, the host controller is configured to perform authentication
 *      and possibly encryption at LMP link setup. The Security Manager still
 *      uses the service database to control authorisation and encryption.
 *      The encryption level for mode 3 is specified in 'mode3_enc'. If the host
 *      controller will not enter authentication mode, then the request fails
 *      (DM_SM_INIT_CFM has a negative response). If the host controller
 *      does enter authentication mode, then the request succeeds. Note that the
 *      requested encryption level may not be supported - in this case the
 *      request still succeeds and the actual encryption level is returned in
 *      DM_SM_INIT_CFM.
 *
 *      In mode 4, security is service-based, like in mode 2. Secure simple
 *      pairing is turned on in mode 4 and we now do authorisation before
 *      authentication. A device must be put into mode 4 to be V2.1 compliant
 *      Once in mode 4, you cannot change to any other mode.
 *
 *      Sets the default security level to be applied in security modes 2, 3
 *      and 4 in cases where a service has not been registered with the
 *      Security Manager. The "default" default security level is defined by
 *      SECL_DEFAULT, and using this primitive overrides SECL_DEFAULT.
 *
 *      write_auth_enable
 *          This controls when the security manager issues WRITE_AUTH_ENABLE
 *          to do mode3-style pairing with legacy devices.
 *          See DM_SM_BONDING_REQ for a description of this parameter.
 *
 *      config
 *          This is a bitfield for general config.
 *
 *          bit 0 - Reserved. Shall be set to zero.
 *          bit 1 - Reserved. Shall be set to zero.
 *          bit 2 - DM_SM_SEC_MODE_CONFIG_DEBUG_MODE
 *          This bit should only be set for debugging using traces.
 *          If set then the security manager will be put into debug mode.
 *          HCI_WRITE_SIMPLE_PAIRING_DEBUG_MODE will be issued to the
 *          controller and the security manager will start generating
 *          link keys with debug type. You can exit debug mode by
 *          resending this primitive with this bit unset.
 *
 *          bit 3 - DM_SM_SEC_MODE_CONFIG_LEGACY_AUTO_PAIR_MISSING_LINK_KEY
 *          If set then the security manager will automatically start
 *          pairing if authentication of a legacy device fails with 
 *          'missing link key'.
 *
 *          bit 4 - DM_SM_SEC_MODE_CONFIG_NEVER_AUTO_ACCEPT
 *          Setting this bit is not recommended and should not be necessary.
 *          If set then the device manager will always issue a 
 *          DM_SM_USER_CONFIRMATION_REQUEST_IND primitive instead of
 *          auto-accepting, irrespective of the local IO capabilities or
 *          the authentication requirements of either device.
 *
 *          bit 5 - DM_SM_SEC_MODE_CONFIG_ALWAYS_TRY_MITM
 *          Setting this bit is not recommended and should not be necessary.
 *          If set then the local device will always request MITM protection
 *          during pairing. If it isn't possible to generate an authenticated
 *          link key and the local device would not otherwise have requested
 *          MITM protection then the local device will not abort pairing and
 *          will allow the generation of an unauthenticated link key.
 *
 *----------------------------------------------------------------------------*/

 /*
 * write_auth_enable (bitfield)
 */

/* Never pair during link setup. Use this if you know that the remote host
 * supports SSP and SM4 or if you plan to avoid timeouts some other way
 */
#define DM_SM_WAE_NEVER             0x0000

/* Pair during link setup if an ACL does not exist */
#define DM_SM_WAE_ACL_NONE          0x0001

/* Note: The values below are chosen so that they will mask out all
   ACL ownership locks that they effectively override. */

/* Pair during link setup, first tearing down any existing ownerless ACL
 * This implies DM_SM_WAE_ACL_NONE
 */
#define DM_SM_WAE_ACL_OWNER_NONE    0x000F

/* Pair during link setup, first tearing down any existing ownerless or
 * APP-owned ACL.
 * This implies DM_SM_WAE_ACL_NONE and DM_SM_WAE_ACL_OWNER_NONE 
 */
#define DM_SM_WAE_ACL_OWNER_APP     0x00FF

/* Pair during link setup, first tearing down any existing ownerless or
 * L2CAP-owned ACL. This will of course close the l2cap connection(s).
 * This implies DM_SM_WAE_ACL_NONE and DM_SM_WAE_ACL_OWNER_NONE
 */
#define DM_SM_WAE_ACL_OWNER_L2CAP   0x0F0F

/* Pair during link setup, first tearing down any existing ownerless or
 * SYNC Manager-owned ACL. This will of course close any existing (e)SCO
 * connection(s). This implies DM_SM_WAE_ACL_NONE and 
 * DM_SM_WAE_ACL_OWNER_NONE
 */
#define DM_SM_WAE_ACL_OWNER_SYNC    0xF00F

/* Pair during link setup, first tearing down any existing ACL */
#define DM_SM_WAE_ALWAYS            0xFFFF

/*
 * config (bitfield)
 */
#define DM_SM_SEC_MODE_CONFIG_NONE                              0x0000
#define DM_SM_SEC_MODE_CONFIG_RESERVED1                         0x0001
#define DM_SM_SEC_MODE_CONFIG_RESERVED2                         0x0002
#define DM_SM_SEC_MODE_CONFIG_DEBUG_MODE                        0x0004
#define DM_SM_SEC_MODE_CONFIG_LEGACY_AUTO_PAIR_MISSING_LINK_KEY 0x0008
#define DM_SM_SEC_MODE_CONFIG_NEVER_AUTO_ACCEPT                 0x0010
#define DM_SM_SEC_MODE_CONFIG_ALWAYS_TRY_MITM                   0x0020

/*
 * options
 *
 * Note that for implementation reasons, multiple bits are assigned to some
 * options. Use the definitions below to ensure that you set either all
 * required bits or none.
 */
#define DM_SM_INIT_SECURITY_MODE            ((1<<0) | (1<<1) | (1<<2) | (1<<8))
#define DM_SM_INIT_SECURITY_LEVEL_DEFAULT   (1<<3)
#define DM_SM_INIT_CONFIG                   ((1<<4) | (1<<5))
#define DM_SM_INIT_WRITE_AUTH_ENABLE        (1<<6)
#define DM_SM_INIT_MODE3_ENC                ((1<<7) | (1<<8))
#define DM_SM_INIT_SM_STATE                 (1<<9)
#define DM_SM_INIT_ALL_BREDR (DM_SM_INIT_SECURITY_MODE \
                            | DM_SM_INIT_SECURITY_LEVEL_DEFAULT \
                            | DM_SM_INIT_CONFIG \
                            | DM_SM_INIT_WRITE_AUTH_ENABLE \
                            | DM_SM_INIT_MODE3_ENC)
#define DM_SM_INIT_ALL_LE    (DM_SM_INIT_CONFIG \
                            | DM_SM_INIT_SM_STATE )
#define DM_SM_INIT_ALL ( DM_SM_INIT_ALL_BREDR | DM_SM_INIT_ALL_LE )

typedef struct
{
    uint16_t er[8];
    uint16_t ir[8];
} DM_SM_KEY_STATE_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */

typedef struct
{
    dm_prim_t           type;               /* Always DM_SM_INIT_REQ */

    /* Bitfield of options.

       Just bitwise OR required options together.

       Note: write_auth_enable and mode3_enc are mutually exclusive.
       mode3_enc only applies to Security Mode 3. write_auth_enable
       applies to Security Modes 2 and 4. The Security Manager will
       process the security mode option first (if present) and then
       will try to set mode3_enc or write_auth_enable depending on
       what security mode it finds itself in and whether the relevant
       option has been switched on. It's safe to try to set both.
       The Security Manager will just ignore the one that doesn't
       apply. */
    uint16_t            options;

    /* Which security mode to use */
    dm_security_mode_t  security_mode;

    /* Default security level. */
    dm_security_level_t security_level_default;

    /* Configuration flags. */
    uint16_t            config;

    /* Specify when we issue HCI_WRITE_AUTH_ENABLE - See DM_SM_BONDING_REQ */
    uint16_t            write_auth_enable;

    /* Encryption level for mode 3. One of:
     *  HCI_ENC_MODE_OFF
     *  HCI_ENC_MODE_PT_TO_PT
     *  HCI_ENC_MODE_PT_TO_PT_AND_BCAST
     */
    uint8_t             mode3_enc;

    /* This records the state of the BLE Security Manager. It contains the
       master identity and encryption keys, together with the encryption
       key diversifier state.

       When an application boots for the first time, it should not attempt to
       set these. The Security Manager will then generate the keys and initiate
       the diversifier. It will return these values to the application in the
       DM_SM_INIT_CFM_T. The application must then store these values in a
       non-volatile area (e.g. PSKEY).
       
       During some pairing processes, the security manager may send new values
       of the diversifier - div - to the application. The application must
       update the diversifier in non-volatile memory.
       
       During subsequent boots, the application must return its stored values
       for the keys and diversifier to the Security Manager in DM_SM_INIT_REQ.

       Failure to do this will mean that any keys distributed from this
       device will be rendered invalid at the next reboot. */
    DM_SM_KEY_STATE_T   sm_key_state;
    uint16_t            sm_div_state;
    uint32_t            sm_sign_counter;
} DM_SM_INIT_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Confirmation of Security Manager INIT request.
 *
 *----------------------------------------------------------------------------*/
/* Bitfield to indicate success or failure.
   0 is success, otherwise set bits indicate any problems that were
   encountered. */
/* The security manager does not recognise the security mode. */
#define DM_SM_INIT_FAIL_UNSUPPORTED_SECURITY_MODE           ((uint16_t)0x0001)
/* HCI_WRITE_SIMPLE_PAIRING_MODE failed. Can't start Security Mode 4 */
#define DM_SM_INIT_FAIL_WRITE_SIMPLE_PAIRING_MODE           ((uint16_t)0x0002)
/* HCI_WRITE_SIMPLE_PAIRING_DEBUG_MODE failed. Can't debug Security Mode 4 */
#define DM_SM_INIT_FAIL_WRITE_SIMPLE_PAIRING_DEBUG_MODE     ((uint16_t)0x0004)
/* HCI_WRITE_AUTH_ENABLE failed. Can't enter/exit Security Mode 3 */
#define DM_SM_INIT_FAIL_WRITE_AUTHENTICATION_ENABLE         ((uint16_t)0x0008)
/* HCI_WRITE_ENC_MODE failed. Can't change Security Mode 3 encryption mode */
#define DM_SM_INIT_FAIL_WRITE_ENCRYPTION_MODE               ((uint16_t)0x0010)
typedef struct
{
    dm_prim_t           type;               /* Always DM_SM_INIT_CFM */
    phandle_t           phandle;            /* Destination phandle */

    /* Current security mode. */
    dm_security_mode_t  security_mode;

    /* Current default security level. */
    dm_security_level_t security_level_default;

    /* Current configuration flags. */
    uint16_t            config;

    /* Current specification for use of HCI_WRITE_AUTH_ENABLE. */
    uint16_t            write_auth_enable;

    /* Current encryption level for mode 3. */
    uint8_t             mode3_enc;

    uint16_t            status;

    DM_SM_KEY_STATE_T   sm_key_state;
    uint16_t            sm_div_state;
    uint32_t            sm_sign_counter;
} DM_SM_INIT_CFM_T;

/*---------------------------------------------------------------------------*
 * PURPOSE
 *      A request from the Security Manager for a link key required for
 *      authentication with the specified remote device. The application must
 *      respond with a DM_SM_LINK_KEY_REQUEST_RSP primitive containing
 *      the link and type, or with no link key and 'key_type' set to 
 *      DM_SM_KEY_TYPE_NONE to reject the request. 'authenticated' will be
 *      set to TRUE if an authenticated link key is required.
 *
 *      The Security Manager will only issue a link key request if it does
 *      not already have a suitable link key for that device in the device
 *      database.
 *
 *      If an ACL exists then rhsf will provide the first 15 bits of the
 *      remote host's supported features. Currently only one bit is defined
 *      (bit 0), which is 1 if the remote host supports SSP and 0 if it
 *      doesn't (legacy pre-2.1 host). If no ACL exists then we return 0xFFFF.
 *
 *      If this parameter is set to 0 and initiated_outgoing is TRUE then sending
 *      a negative response will tell the security manager to initiate legacy
 *      pairing on an existing ACL. This can potentially lead to timeouts on
 *      some devices. To try to avoid timing out, you may want to prefetch
 *      the PIN from the user, set up an SDP 'ping' to keep the ACL alive, or
 *      whatever you did to avoid these problems in SM2.
 *
 *---------------------------------------------------------------------------*/

/* rhsf parameter */
#define DM_SM_RHSF_NO_ACL   0xFFFF
#define DM_SM_RHSF_NONE     0x0000
#define DM_SM_RHSF_SSP      0x0001

typedef struct
{
    dm_prim_t           type;           /* DM_SM_LINK_KEY_REQUEST_IND */
    phandle_t           phandle;        /* Destination phandle */
    BD_ADDR_T           bd_addr;        /* address of remote device */

    /* Class of device for peer, valid for incoming connections only else 0 */
    uint24_t            dev_class;

    bool_t              authenticated;  /* TRUE if we need authenticated key */
    /* TRUE if this is security for an outgoing connection that we've initiated */
    bool_t              initiated_outgoing;
    uint16_t            rhsf;           /* Remote host's supported features */
} DM_SM_LINK_KEY_REQUEST_IND_T;

/*---------------------------------------------------------------------------*
 * PURPOSE
 *      A response to DM_SM_LINK_KEY_REQUEST_RES
 *
 *      To respond with a suitable link key, set 'key_type' according to
 *      'HCI Link Key Notification Event Key Type', in hci.h, and then put
 *      the link key at the end of the primitive as normal.
 *
 *      To reject the request, set 'key_type' to DM_SM_KEY_TYPE_NONE and
 *      add nothing to the end of the primitive.
 *
 *---------------------------------------------------------------------------*/

/* key_type */
#define DM_SM_LINK_KEY_NONE                 ((uint8_t) 0xFE)
#define DM_SM_LINK_KEY_LEGACY               HCI_COMBINATION_KEY
#define DM_SM_LINK_KEY_DEBUG                HCI_DEBUG_COMBINATION_KEY
#define DM_SM_LINK_KEY_UNAUTHENTICATED      HCI_UNAUTHENTICATED_COMBINATION_KEY
#define DM_SM_LINK_KEY_AUTHENTICATED        HCI_AUTHENTICATED_COMBINATION_KEY
#define DM_SM_LINK_KEY_CHANGED              HCI_CHANGED_COMBINATION_KEY

typedef struct
{
    dm_prim_t           type;           /* DM_SM_LINK_KEY_REQUEST_RSP */

    BD_ADDR_T           bd_addr;        /* Remote device address */

    /* See HCI Link Key Notification Event Key Type in hci.h */
    uint8_t            key_type;

    uint8_t             *key;
} DM_SM_LINK_KEY_REQUEST_RSP_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      A request from the Security Manager for a PIN code required for
 *      pairing with the specified remote device. The application must respond
 *      with a DM_SM_PIN_REQUEST_RSP primitive containing the PIN code, or
 *      with 'pin_length' set to 0 to reject the request.
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_SM_PIN_REQUEST_IND */
    phandle_t           phandle;            /* Destination phandle */

    /* The address of the remote device for which a PIN is required */
    TYPED_BD_ADDR_T     addrt;
    /* Class of device for peer, valid for incoming connections only else 0 */
    uint24_t            dev_class;

    bool_t              initiator;          /* TRUE if we initiated pairing */
} DM_SM_PIN_REQUEST_IND_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      A response to the Security Manager containing the PIN for remote device
 *      'bd_addr'. If no PIN is known, then set 'pin_length' to zero to reject
 *      the request.
 *
 *      If pairing succeeds as a result of the PIN, then the Security Manager
 *      will issue a DM_SM_LINK_KEY_IND primitive, containing the new link key.
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_SM_PIN_REQUEST_RSP */

    /* The address of the remote device for which a PIN was requested */
    TYPED_BD_ADDR_T     addrt;

    /* The PIN code - set 'pin_length' to zero to reject */
    uint8_t             pin_length;
    uint8_t             pin[HCI_MAX_PIN_LENGTH];
} DM_SM_PIN_REQUEST_RSP_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      A notification from the Security Manager that a new link key is now
 *      in force for the remote device 'bd_addr'. This can be as a result of
 *      pairing, or as a result of one of the devices requesting a change of
 *      link key with the Change_Connection_Link_Key HCI command.
 *
 *      The Security Manager keeps a copy of the new link key, and passes it up
 *      to the application for non-volatile storage.
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_SM_LINK_KEY_IND */
    phandle_t           phandle;            /* Destination phandle */

    /* The address of the remote device with which pairing completed */
    BD_ADDR_T           bd_addr;

    /* The link key type and value. The type can be:
    *   HCI_COMBINATION_KEY
    *   HCI_LOCAL_UNIT_KEY
    *   HCI_REMOTE_UNIT_KEY
    */
    uint8_t             key_type;
    uint8_t             key[SIZE_LINK_KEY];
} DM_SM_LINK_KEY_IND_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      A request for authorisation from the Security Manager to the
 *      application. This is only performed when an untrusted or unknown device
 *      is attempting to access a service that requires authorisation in
 *      security mode 2.
 *
 *      The application must make a decision and then respond with a
 *      DM_SM_AUTHORISE_RSP primitive containing the result. Devices can be
 *      marked as trusted using the DM_SM_ADD_DEVICE_REQ primitive.
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t               type;       /* Always DM_SM_AUTHORISE_IND */
    phandle_t               phandle;    /* Destination phandle */
    DM_CONNECTION_SETUP_T   cs;         /* Protocol, channel, bd_addr and direction */
} DM_SM_AUTHORISE_IND_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Authorisation response from the application to the Security Manager.
 *
 *----------------------------------------------------------------------------*/
#define DM_SM_AUTHORISE_ACCEPT_BIT          0   /* Set to authorise connection */
#define DM_SM_AUTHORISE_TWICE_BIT           1   /* Set to apply to next connection as well (within life of ACL) */
#define DM_SM_AUTHORISE_LIFE_OF_ACL_BIT     2   /* Set to apply to all connections for life of ACL */

#define DM_SM_AUTHORISE_REJECT_ONCE         0x0000
#define DM_SM_AUTHORISE_ACCEPT_ONCE         (1 << DM_SM_AUTHORISE_ACCEPT_BIT)
#define DM_SM_AUTHORISE_REJECT_TWICE        (1 << DM_SM_AUTHORISE_TWICE_BIT)
#define DM_SM_AUTHORISE_ACCEPT_TWICE        \
    ((1 << DM_SM_AUTHORISE_ACCEPT_BIT) | (1 << DM_SM_AUTHORISE_TWICE_BIT))
#define DM_SM_AUTHORISE_REJECT_LIFE_OF_ACL  \
    (1 << DM_SM_AUTHORISE_LIFE_OF_ACL_BIT)
#define DM_SM_AUTHORISE_ACCEPT_LIFE_OF_ACL  \
    ((1 << DM_SM_AUTHORISE_ACCEPT_BIT) | (1 << DM_SM_AUTHORISE_LIFE_OF_ACL_BIT))

typedef struct
{
    dm_prim_t               type;       /* Always DM_SM_AUTHORISE_RSP */
    DM_CONNECTION_SETUP_T   cs;         /* Protocol, channel, bd_addr and direction */
    uint16_t                authorisation;  /* Nature of authorisation */
} DM_SM_AUTHORISE_RSP_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Applications may request authentication of a link at any time, using
 *      the DM_SM_AUTHENTICATE_REQ primitive. If the link has already been
 *      authenticated, the Security Manager responds immediately, otherwise it
 *      performs the HCI authentication procedure, which may involve pairing.
 *
 *      This request will be rejected in security mode 1.
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_SM_AUTHENTICATE_REQ */

    /* Address of device to authenticate */
    BD_ADDR_T           bd_addr;
} DM_SM_AUTHENTICATE_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Result of an authentication request.
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_SM_AUTHENTICATE_CFM */
    phandle_t           phandle;            /* Destination phandle */

    /* Address of device being authenticated */
    BD_ADDR_T           bd_addr;

    /* TRUE if successful */
    bool_t              success;
} DM_SM_AUTHENTICATE_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Applications may request encryption activation or de-activation of a
 *      link at any time, using the DM_SM_ENCRYPT_REQ primitive. If the link
 *      encryption is already in the requested state, the Security Manager
 *      responds immediately, otherwise it performs the relevant HCI procedure.
 *      The link must have been authenticated before encryption is allowed.
 *
 *      This request will be rejected in security mode 1.
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_SM_ENCRYPT_REQ */

    /* Link to encrypt */
    BD_ADDR_T           bd_addr;

    /* TRUE to activate encryption, FALSE to stop encryption */
    bool_t              encrypt;
} DM_SM_ENCRYPT_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Result of a locally-initiated encryption request (DM_SM_ENCRYPT_REQ).
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_SM_ENCRYPT_CFM */
    phandle_t           phandle;            /* Destination phandle */

    /* Link being encrypted */
    BD_ADDR_T           bd_addr;

    /* TRUE if the command was successful, FALSE if it failed. The actual
     * encryption state of the link is in 'encrypted'.
     */
    bool_t              success;

    /* TRUE if encrypted, FALSE if not encrypted */
    bool_t              encrypted;
} DM_SM_ENCRYPT_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Notification of a possible change in the encryption status of a link
 *      due to a peer-initiated encryption procedure.
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_SM_ENCRYPTION_CHANGE_IND */
    phandle_t           phandle;            /* Destination phandle */

    /* Link being encrypted */
    TYPED_BD_ADDR_T     addrt;

    /* TRUE if encrypted, FALSE if not encrypted */
    bool_t              encrypted;
} DM_SM_ENCRYPTION_CHANGE_IND_T;

typedef HCI_REFRESH_ENCRYPTION_KEY_T DM_HCI_REFRESH_ENCRYPTION_KEY_REQ_T;
typedef DM_HCI_TYPED_BD_ADDR_COMMAND_CFM_T DM_HCI_REFRESH_ENCRYPTION_KEY_IND_T;

#define DM_HCI_REFRESH_ENCRYPTION_KEY_REQ  ((dm_prim_t)(ENUM_DM_HCI_REFRESH_ENCRYPTION_KEY_REQ))
#define DM_HCI_REFRESH_ENCRYPTION_KEY_IND  ((dm_prim_t)(ENUM_DM_HCI_REFRESH_ENCRYPTION_KEY_IND))

/*---------------------------------------------------------------------------*
 *  PURPOSE
 *      Common RES and IND primitive for Secure Simple Pairing
 *
 *---------------------------------------------------------------------------*/
/* flags field */
#define DM_SM_FLAGS_NONE                0x0000
#define DM_SM_FLAGS_INITIATOR           0x0001      /* Pairing initiated locally */
#define DM_SM_FLAGS_SECURITY_MANAGER    0x0002      /* Security Manager used instead of LMP */
#define DM_SM_FLAGS_RESPONSE_REQUIRED   0x0004      /* for DM_SM_USER_CONFIRMATION_REQUEST_IND_T */
typedef struct
{
    dm_prim_t               type;
    phandle_t               phandle;    /* Destination phandle */
    TYPED_BD_ADDR_T         addrt;

    /* Class of device for peer, valid for incoming connections only else 0 */
    uint24_t                dev_class;

    uint16_t                flags;
} DM_COMMON_IND_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */

typedef struct
{
    dm_prim_t               type;
    TYPED_BD_ADDR_T         addrt;
} DM_COMMON_RSP_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */

/*---------------------------------------------------------------------------*
 *  DM_SM_IO_CAPABILITY_RESPONSE_IND
 *
 *      Reports parameters given by the other device.
 *      It is purely informative and you should not respond to it.
 *--------------------------------------------------------------------------*/
/* key_distribution field */
#define DM_SM_KEY_DIST_INITIATOR_ENC_CENTRAL    0x0001
#define DM_SM_KEY_DIST_INITIATOR_ID             0x0002
#define DM_SM_KEY_DIST_INITIATOR_SIGN           0x0004
#define DM_SM_KEY_DIST_RESPONDER_ENC_CENTRAL    0x0100
#define DM_SM_KEY_DIST_RESPONDER_ID             0x0200
#define DM_SM_KEY_DIST_RESPONDER_SIGN           0x0400

typedef struct
{
    dm_prim_t               type;       /* DM_SM_IO_CAPABILITY_RESPONSE_IND */
    phandle_t               phandle;    /* Destination phandle */
    TYPED_BD_ADDR_T         addrt;
    uint8_t                 io_capability;
    uint8_t                 oob_data_present;
    uint8_t                 authentication_requirements;
    uint16_t                flags;  /* See flags field in DM_COMMON_IND_T */
    uint16_t                key_distribution;
} DM_SM_IO_CAPABILITY_RESPONSE_IND_T;

/*---------------------------------------------------------------------------*
 *  DM_SM_IO_CAPABILITY_REQUEST_IND
 *
 *  The Security Manager asks the application for IO capability information
 *
 *  authentication_requirements is present so that the application can
 *  request General Bonding during service-based security and so that
 *  the application can insist on MITM protection, overriding the
 *  security manager. Effectively, the application can upgrade the security
 *  requirements. Setting authentication_requirements for dedicated bonding
 *  will always be ignored by the security manager as this will only be done
 *  when you issue a DM_SM_BONDING_REQ or when you are the responder and the
 *  initiator has set its authentication_requirements for dedicated bonding.
 *
 *  For BLE security, the authentication_requirements field should be set
 *  using the DM_SM_SECURITY_ defines. See DM_SM_SECURITY_REQ.
 *
 *  Notes for BR/EDR:
 *      1. An Application cannot downgrade the security requirements. If the
 *      security manager believes that MITM protection is required then
 *      it will request an authenticated link key, irrespective of the
 *      authentication_requirements set in this primitive.
 *
 *      2. If you are doing dedicated bonding then the security manager will
 *      send appropriate authentication_requirements (i.e. it will send either
 *      HCI_MITM_NOT_REQUIRED_DEDICATED_BONDING or 
 *      HCI_MITM_REQUIRED_DEDICATED_BONDING) to the remote device.
 *
 *      3. If you are doing service-based security then the security manager
 *      will send appropriate authentication_requirements (i.e. it will send
 *      HCI_MITM_NOT_REQUIRED_NO_BONDING, HCI_MITM_REQUIRED_NO_BONDING,
 *      HCI_MITM_NOT_REQUIRED_GENERAL_BONDING or 
 *      HCI_MITM_REQUIRED_GENERAL_BONDING) to the remote device.
 *
 *      4. You should be careful about upgrading the security requirements
 *      to MITM protection required. It is probably only worth doing if
 *      you know that MITM protection is going to be required anyway. This
 *      might be the case when you are opening the RFCOMM MUX (which might
 *      not require MITM protection) in order to open an RFCOMM channel
 *      that does require MITM protection. You might be able to save some
 *      time by upgrading to MITM_REQUIRED when opening the MUX as this
 *      will avoid pairing twice.
 *
 *      5. Remember that if you insist on MITM protection and the
 *      io_capabilities of the two devices prevent the creation of
 *      an authenticated link key then pairing will fail.
 *
 *  If oob_data_present == 0 then both oob_hash_c and oob_rand_r shall be
 *  NULL and there shall be no associated data with the primitive.
 *
 *  For BR/EDR, if oob_data_present == 1 then oob_hash_c and oob_rand_r
 *  shall each point to SIZE_OOB_DATA words of OOB data. If you only have
 *  oob_hash_c then zero the oob_rand_r data.
 *
 *  For BLE, if oob_data_present == 1 then oob_hash_c shall point to
 *  SIZE_OOB_DATA words of OOB data. oob_rand_r shall be NULL.
 *
 *---------------------------------------------------------------------------*/
typedef DM_COMMON_IND_T DM_SM_IO_CAPABILITY_REQUEST_IND_T;

typedef struct
{
    dm_prim_t               type;       /* DM_SM_IO_CAPABILITY_REQUEST_RSP */
    TYPED_BD_ADDR_T         addrt;
    uint8_t                 io_capability;
    uint8_t                 oob_data_present;
    uint8_t                 authentication_requirements;
    uint8_t                 *oob_hash_c;
    uint8_t                 *oob_rand_r;
    uint16_t                key_distribution;   /* BLE only */
} DM_SM_IO_CAPABILITY_REQUEST_RSP_T;

typedef struct
{
    dm_prim_t               type;     /* DM_SM_IO_CAPABILITY_REQUEST_NEG_RSP */
    TYPED_BD_ADDR_T         addrt;
    hci_reason_t            reason;
} DM_SM_IO_CAPABILITY_REQUEST_NEG_RSP_T;

/*---------------------------------------------------------------------------*
 *  DM_SM_USER_CONFIRMATION_REQUEST_IND
 *
 *  This primitive is sent when the numeric comparison authentication model
 *  is being used for pairing and provides the application with the generated
 *  numeric value.
 *
 *  If io_cap_local indicates that the local device is capable of displaying
 *  the numeric value then the application shall display it until
 *  DM_SM_SIMPLE_PAIRING_COMPLETE_IND is received.
 *
 *  io_cap_remote is there for information only and indicates whether or
 *  not a correctly implemented remote device will be displaying the number.
 *
 *  flags is a bitfield defined above. See DM_COMMON_IND.
 *  If DM_SM_FLAGS_INITIATOR is set then this pairing has been initiated by
 *  the local device. If DM_SM_FLAGS_RESPONSE_REQUIRED is set then the
 *  application shall respond with either DM_SM_USER_CONFIRMATION_REQUEST_RSP
 *  or DM_SM_USER_CONFIRMATION_REQUEST_NEG_RSP. If
 *  DM_SM_FLAGS_RESPONSE_REQUIRED is not set then the application shall not
 *  respond (but shall still display the numeric value if io_cap_local
 *  indicates that it shall).
 *
 *---------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t               type;               /* DM_SM_USER_CONFIRMATION_REQUEST_IND */
    phandle_t               phandle;            /* Destination phandle */
    TYPED_BD_ADDR_T         addrt;

    /* Class of device for peer, valid for incoming connections only else 0 */
    uint24_t                dev_class;

    uint32_t                numeric_value;      /* Value to compare */
    uint8_t                 io_cap_local;       /* Effective IO capabilities of local device. */
    uint8_t                 io_cap_remote;      /* Effective IO capabilities of remote device. */
    uint16_t                flags;              /* See DM_COMMON_IND_T */
} DM_SM_USER_CONFIRMATION_REQUEST_IND_T;

typedef DM_COMMON_RSP_T DM_SM_USER_CONFIRMATION_REQUEST_RSP_T;
typedef DM_COMMON_RSP_T DM_SM_USER_CONFIRMATION_REQUEST_NEG_RSP_T;

/*---------------------------------------------------------------------------*
 *  DM_SM_USER_PASSKEY_REQUEST
 *
 *  The user must enter a six digit passkey.
 *  There is a separate primitive to reject the request
 *
 *---------------------------------------------------------------------------*/
typedef DM_COMMON_IND_T DM_SM_USER_PASSKEY_REQUEST_IND_T;

typedef struct
{
    dm_prim_t               type;       /* DM_SM_USER_PASSKEY_REQUEST_RSP */
    TYPED_BD_ADDR_T         addrt;
    uint32_t                numeric_value;
} DM_SM_USER_PASSKEY_REQUEST_RSP_T;

typedef DM_COMMON_RSP_T DM_SM_USER_PASSKEY_REQUEST_NEG_RSP_T;

/*---------------------------------------------------------------------------*
 *  DM_SM_USER_PASSKEY_NOTIFICATION_IND
 *
 *  The passkey should be displayed on the local device so that it may be
 *  entered on the remote device.
 *
 *---------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t               type;     /* DM_SM_USER_PASSKEY_NOTIFICATION_IND */
    phandle_t               phandle;  /* destination phandle */
    TYPED_BD_ADDR_T         addrt;
    uint32_t                passkey;

    /* Class of device for peer, valid for incoming connections only */
    uint24_t                dev_class;
    uint16_t                flags;    /* See DM_COMMON_IND_T */
} DM_SM_USER_PASSKEY_NOTIFICATION_IND_T;

/*---------------------------------------------------------------------------*
 *  DM_SM_SEND_KEYPRESS_NOTIFICATION_REQ
 *
 *  Tell the remote device about keypresses during local passkey entry.
 *
 *---------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t               type;    /* DM_SM_SEND_KEYPRESS_NOTIFICATION_REQ */
    uint8_t                 unused;  /* Always 0 */
    BD_ADDR_T               bd_addr;
    uint8_t                 notification_type;
} DM_SM_SEND_KEYPRESS_NOTIFICATION_REQ_T;

/*---------------------------------------------------------------------------*
 *  DM_SM_KEYPRESS_NOTIFICATION_IND
 *
 *  Notification of keypresses during passkey entry on the remote device.
 *
 *---------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t               type;      /* DM_SM_KEYPRESS_NOTIFICATION_IND */
    phandle_t               phandle;   /* destination phandle */
    TYPED_BD_ADDR_T         addrt;
    uint8_t                 notification_type;
    uint16_t                flags;    /* See DM_COMMON_IND_T */
} DM_SM_KEYPRESS_NOTIFICATION_IND_T;

/*---------------------------------------------------------------------------*
 *  DM_SM_SIMPLE_PAIRING_COMPLETE_IND
 *
 *  Indication that authentication by SSP has finished
 *
 *--------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t               type;      /* DM_SM_SIMPLE_PAIRING_COMPLETE_IND */
    phandle_t               phandle;   /* destination phandle */
    TYPED_BD_ADDR_T         addrt;
    uint8_t                 status;
    uint16_t                flags;     /* See DM_COMMON_IND_T */
} DM_SM_SIMPLE_PAIRING_COMPLETE_IND_T;

/*---------------------------------------------------------------------------*
 *  DM_SM_READ_LOCAL_OOB_DATA_REQ
 *
 *  The application may request local OOB data from the controller. This
 *  should then be transferred to the remote device via an OOB mechanism.
 *  Only the most recent set of OOB data retrieved may be used by a
 *  remote device for authentication
 *
 *---------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t               type;      /* DM_SM_READ_LOCAL_OOB_DATA_REQ */
    uint8_t                 unused;    /* Always 0 */
} DM_SM_READ_LOCAL_OOB_DATA_REQ_T;

typedef struct
{
    dm_prim_t               type;       /* DM_SM_READ_LOCAL_OOB_DATA_CFM */
    phandle_t               phandle;    /* destination phandle */
    uint8_t                 status;
    uint8_t                 *oob_hash_c;
    uint8_t                 *oob_rand_r;
} DM_SM_READ_LOCAL_OOB_DATA_CFM_T;

/*---------------------------------------------------------------------------*
 * DM_SM_BONDING_REQ/CFM
 *
 * The application requests bonding with a remote device. First we delete
 * any link key we have in the device database. 
 *
 * If the remote device is in SM4 then we make sure an ACL exists before
 * authenticating it. 
 *
 * If the remote device is legacy then the write_auth_enable parameter
 * determines in what circumstances we try to pair during link setup
 * (just like mode 3).
 *---------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t               type;       /* DM_SM_BONDING_REQ */
    TYPED_BD_ADDR_T         addrt;      /* Remote device address */
    uint16_t                flags;      /* Reserved and shall be set to 0 */
} DM_SM_BONDING_REQ_T;

typedef struct
{
    dm_prim_t               type;       /* DM_SM_BONDING_CFM */
    phandle_t               phandle;    /* destination phandle */
    TYPED_BD_ADDR_T         addrt;      /* Remote device address */
    uint16_t                flags;      /* See DM_COMMON_IND_T */
    hci_error_t             status;
} DM_SM_BONDING_CFM_T;

/* The status field in DM_SM_BONDING_CFM_T returns a standard HCI error code,
   or one of the following two additional error codes. */
#define DM_SM_BONDING_CANCELLED   ((hci_error_t)0xFF)
/* DM_SM_BONDING_NO_PAIRING is generated when we cannot prevent the baseband
   from automatically authenticating an existing link key. This will only occur
   for legacy security modes, i.e., it will never happen in Security Mode 4.
   To avoid this error, close the ACL before bonding. */
#define DM_SM_BONDING_NO_PAIRING  ((hci_error_t)0xFE)

/*---------------------------------------------------------------------------*
 * DM_SM_BONDING_CANCEL_REQ
 *
 * The application requests that a previous DM_SM_BONDING_REQ be cancelled.
 *
 * This is not guaranteed to work but it stands a better chance of working
 * the earlier it is used.
 *
 * If the DM_SM_BONDING_REQ request is still queued then the security
 * manager will just remove it. If the ACL manager is currently trying to
 * connect an ACL for bonding, then the security manager will try to
 * cancel this. If the ACL is already up and the pairing process has started
 * then the security manager will issue negative replies to halt it as
 * soon as possible. If you set force = TRUE then, if all else fails,
 * the DM will cancel bonding by bringing down the ACL, even if there are
 * l2cap connections open.
 *
 * There is no DM_SM_BONDING_CANCEL_CFM, but if cancellation was successful
 * then the DM_SM_BONDING_CFM associated with the original bonding request
 * will return with status = DM_SM_BONDING_CANCELLED. If it was too late to
 * cancel bonding and pairing has already succeeded, DM_SM_BONDING_CFM will
 * return with status = DM_SM_BONDING_SUCCESS and it is then up to the
 * application to unbond the device.
 *
 *---------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t               type;       /* DM_SM_BONDING_CANCEL_REQ */
    TYPED_BD_ADDR_T         addrt;      /* Remote device address */
    uint16_t                flags;      /* See DM_ACL_OPEN_REQ */
} DM_SM_BONDING_CANCEL_REQ_T;


/* Legacy definitions */
#define DM_SM_LINK_KEY_NONE                 ((uint8_t) 0xFE)
#define DM_SM_LINK_KEY_LEGACY               HCI_COMBINATION_KEY
#define DM_SM_LINK_KEY_DEBUG                HCI_DEBUG_COMBINATION_KEY
#define DM_SM_LINK_KEY_UNAUTHENTICATED      HCI_UNAUTHENTICATED_COMBINATION_KEY
#define DM_SM_LINK_KEY_AUTHENTICATED        HCI_AUTHENTICATED_COMBINATION_KEY
#define DM_SM_LINK_KEY_CHANGED              HCI_CHANGED_COMBINATION_KEY

typedef enum
{
    DM_SM_KEY_NONE,
    DM_SM_KEY_ENC_BREDR,        /* BR/EDR link key */
    DM_SM_KEY_ENC_CENTRAL,      /* LTK + EDIV + RAND for central */
    DM_SM_KEY_DIV,              /* DIV for peripheral */
    DM_SM_KEY_SIGN,             /* CSRK */
    DM_SM_KEY_ID                /* IRK */
} DM_SM_KEY_TYPE_T;

/* Combination/authenticated/unauthenticated link key for BR/EDR
   links */
typedef struct
{
    uint16_t link_key_type;
    uint16_t link_key[8];
} DM_SM_KEY_ENC_BREDR_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */

/* This is the encryption information provided to us by the peer.
   It will be requested when we're in the central role and want
   to authenticate and encrypt the link. */
typedef struct
{
    uint16_t ediv;
    uint16_t rand[4];
    uint16_t ltk[8];
} DM_SM_KEY_ENC_CENTRAL_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */

/* This is the identity information provided to us by the peer.
   The IRK can be used to identify devices using a resolvable
   private address. The identity address is the public or static
   address of the peer. */
typedef struct
{
    uint16_t irk[8];
} DM_SM_KEY_ID_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */

/* This is the peer's signing key. We can use this to sign certain
   attribute protocol PDUs when sending them over an unencrypted link. */
typedef struct
{
    uint16_t csrk[8];
    uint32_t counter;
} DM_SM_KEY_SIGN_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */

typedef union
{
    DM_SM_KEY_ENC_BREDR_T enc_bredr;
    DM_SM_KEY_ENC_CENTRAL_T enc_central;
    uint16_t div;
    DM_SM_KEY_ID_T id;
    DM_SM_KEY_SIGN_T sign;
} DM_SM_RFCLI_UKEY_T;

/* Union of pointers to key structures, except for DM_SM_KEY_ENC_PERIPHERAL_T
   which is not a pointer. */
typedef union
{
    DM_SM_KEY_ENC_BREDR_T *enc_bredr;
    DM_SM_KEY_ENC_CENTRAL_T *enc_central;
    uint16_t div;   /* Allows peripheral to reconstruct LTK and CSRK given to central */
    DM_SM_KEY_ID_T *id;
    DM_SM_KEY_SIGN_T *sign;
    void *none;
} DM_SM_UKEY_T;

#define DM_SM_MAX_NUM_KEYS 5
#define DM_SM_NUM_KEY_BITS 3
#define DM_SM_KEYS_EXISTING         0x8000
#define DM_SM_KEYS_UPDATE_EXISTING  DM_SM_KEYS_EXISTING
#define DM_SM_KEYS_REPLACE_EXISTING 0x0000
#define DM_SM_KEYS_PRESENT(one, two, three, four, five, update_or_replace) \
    ((DM_SM_KEY_MASK(one)   << (DM_SM_NUM_KEY_BITS * 0)) | \
     (DM_SM_KEY_MASK(two)   << (DM_SM_NUM_KEY_BITS * 1)) | \
     (DM_SM_KEY_MASK(three) << (DM_SM_NUM_KEY_BITS * 2)) | \
     (DM_SM_KEY_MASK(four)  << (DM_SM_NUM_KEY_BITS * 3)) | \
     (DM_SM_KEY_MASK(five)  << (DM_SM_NUM_KEY_BITS * 4)) | \
     (update_or_replace))
typedef struct
{
    /* The security requirements met by these keys. */
    uint16_t security_requirements;

    /* The number of significant octets in the LTK */
    uint16_t encryption_key_size;

    /* Set the pointers to point to the appropriate keys and use values
       defined in the DM_SM_KEY_TYPE_T enum with the macro. The final
       argument to the macro is either DM_SM_KEYS_UPDATE_EXISTING, which
       will cause just the addition or replacement of any keys specified,
       or DM_SM_KEYS_REPLACE_EXISTING, which will delete all existing keys
       before adding the new keys specified. */
    uint16_t present;

    DM_SM_UKEY_T u[DM_SM_MAX_NUM_KEYS]; /* Array of pointers to keys */
} DM_SM_KEYS_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */

typedef struct
{
    dm_prim_t type;             /* DM_SM_KEYS_IND */
    phandle_t phandle;          /* destination phandle */
    TYPED_BD_ADDR_T addrt;              /* Peer's address used during pairing */
    DM_SM_KEYS_T keys;          /* Security keys and requirements */
    TYPED_BD_ADDR_T id_addrt;           /* Peer's static or public address
                                   (zeroed if not present) */
} DM_SM_KEYS_IND_T;

/* Security requirements bitfield definitions

   DM_SM_SECURITY_BONDING - In Bluetooth Low Energy, devices bond by
   exchaning keys after the link has been encrypted. If the devices are
   not bonding then no key distribution should take place. Note that
   key distribution is determined by the key_distribution field in the
   DM_SM_IO_CAPABILITY_REQUEST_RSP primitive. (That field should be set
   to zero by any device that does not want to bond.)

   DM_SM_SECURITY_ENCRYPTION - This bit specifies a requirement for an
   encrypted link. If there is no Out of Band data available then the
   security manager will default to using the Just Works pairing method
   and will produce an unauthenticated key with no MITM protection.

   DM_SM_SECURITY_MITM_PROTECTION - This bit specifies a requirement for
   MITM protection. If there is no Out of Band data available then the
   security manager will try to use passkey pairing. If the IO capabilities
   of the two devices do not allow this then the pairing will fail.

   DM_SM_SECURITY_QUERY_ONLY - This bit determines the security manager's
   behaviour upon finding that the current state of the link does not
   provide the security requested. If this bit is set then the secruity
   manager will simply report back failure. If it is not set then the 
   security manager will initiate pairing to increase the security level.
   This bit is intended to be set only by ATT, which will need to query
   the current security level. The application will want to increase the
   security level to match its request and so should not set this bit.
*/
#define DM_SM_SECURITY_BONDING          0x0001  /* Create a bond */
#define DM_SM_SECURITY_MITM_PROTECTION  0x0004  /* Protect against MITM */
#define DM_SM_SECURITY_ENCRYPTION       0x4000  /* Encrypt the link */
#define DM_SM_SECURITY_QUERY_ONLY       0x8000  /* Query current state */

/* Security levels derived from bitfield definitions above */
#define DM_SM_SECURITY_NONE 0x0000
#define DM_SM_SECURITY_UNAUTHENTICATED_NO_BONDING \
    (DM_SM_SECURITY_ENCRYPTION)
#define DM_SM_SECURITY_UNAUTHENTICATED_BONDING \
    (DM_SM_SECURITY_UNAUTHENTICATED_NO_BONDING | DM_SM_SECURITY_BONDING)
#define DM_SM_SECURITY_AUTHENTICATED_NO_BONDING \
    (DM_SM_SECURITY_ENCRYPTION | DM_SM_SECURITY_MITM_PROTECTION)
#define DM_SM_SECURITY_AUTHENTICATED_BONDING \
    (DM_SM_SECURITY_AUTHENTICATED_NO_BONDING | DM_SM_SECURITY_BONDING)

/* DM_SM_SECURITY_REQ_T, DM_SM_SECURITY_CFM_T and DM_SM_SECURITY_IND_T
   are identical. We use DM_SM_SECURITY_REQ_T as the original typedef
   with the structure definition because otherwise the VM does not
   auto-generate code to write the phandle. See B-91749. This can be
   rewritten more naturally (defining DM_SM_SECURITY_COMMON_T first)
   once B-91747 has been fixed. */
typedef struct
{
    dm_prim_t type;                 /* DM_SM_SECURITY_REQ */
    phandle_t phandle;              /* destination phandle */
    TYPED_BD_ADDR_T addrt;                  /* Peer's address */
    uint16_t connection_flags;      /* See l2ca_conflags_t in l2cap_prim.h */
    uint16_t context;               /* Opaque context returned in CFM */
    uint16_t security_requirements;
    hci_return_t status;            /* Indication of success or failure. Shall
                                       be set to 0 in DM_SM_SECURITY_REQ. */
} DM_SM_SECURITY_REQ_T;

typedef DM_SM_SECURITY_REQ_T DM_SM_SECURITY_COMMON_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */
typedef DM_SM_SECURITY_COMMON_T DM_SM_SECURITY_CFM_T;
typedef DM_SM_SECURITY_COMMON_T DM_SM_SECURITY_IND_T;

typedef struct
{
    dm_prim_t type;
    phandle_t phandle;
    TYPED_BD_ADDR_T addrt;
    uint16_t security_requirements;
    DM_SM_KEY_TYPE_T key_type;
} DM_SM_KEY_REQUEST_IND_T;

typedef struct
{
    dm_prim_t type;
    TYPED_BD_ADDR_T addrt;
    uint16_t security_requirements;
    DM_SM_KEY_TYPE_T key_type;
    DM_SM_UKEY_T key;
} DM_SM_KEY_REQUEST_RSP_T;

/* The trust level of the remote device. In downstream primitives, this
   is used to change the trust level and any of the three values may be
   used. Setting DM_SM_TRUST_UNCHANGED will not change the trust level.
   (For new device records, this is equivalent to DM_SM_TRUST_DISABLED.)
   In upstream primitives, only DM_SM_TRUST_DISABLED or DM_SM_TRUST_ENABLED
   will be used.

   Untrusted devices result in a DM_SM_AUTHORISE_IND for access to
   protocols that require authorisation.
*/
typedef enum
{
    DM_SM_TRUST_UNCHANGED,  /* Don't change the trust level. */
    DM_SM_TRUST_DISABLED,   /* (make the device) untrusted. */
    DM_SM_TRUST_ENABLED     /* (make the device) trusted. */
} DM_SM_TRUST_T;

typedef enum
{
    DM_SM_PRIVACY_UNCHANGED,/* Keep privacy the way it is. */
    DM_SM_PRIVACY_DISABLED, /* Turn off privacy. */
    DM_SM_PRIVACY_ENABLED   /* Turn on privacy. */
} DM_SM_PRIVACY_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */

typedef struct
{
    dm_prim_t           type;       /* Always DM_SM_ADD_DEVICE_REQ */
    phandle_t           phandle;    /* phandle for response */
    TYPED_BD_ADDR_T     addrt;      /* Bluetooth address of remote device */

    /* If privacy is DM_SM_PRIVACY_UNCHANGED then reconnection_bd_addr
       shall be zeroed and will be ignored by Bluestack. Bluestack will
       change neither the privacy state nor the reconnection address
       associated with this remote device.
       If privacy is DM_SM_PRIVACY_DISABLED then reconnection_bd_addr
       shall be zeroed and will be ignored by Bluestack. Bluestack will
       disable privacy for this device and discard any reconnection
       address.
       If privacy is DM_SM_PRIVACY_ENABLED then reconnection_bd_addr shall
       either be zeroed or set to a valid non-resolvable private address.
       Bluestack will enable privacy for this device. If reconnection_bd_addr
       is zeroed then it will discard any reconnection address, otherwise it
       will set the reconnection address to reconnection_bd_addr. */
    DM_SM_PRIVACY_T     privacy;
    BD_ADDR_T           reconnection_bd_addr;

    DM_SM_TRUST_T       trust;      /* Update trust level */
    DM_SM_KEYS_T        keys;       /* Security keys and requirements */
} DM_SM_ADD_DEVICE_REQ_T;

typedef struct
{
    dm_prim_t           type;       /* Always DM_SM_ADD_DEVICE_CFM */
    phandle_t           phandle;    /* Destination phandle */
    TYPED_BD_ADDR_T     addrt;
    uint8_t             status;
} DM_SM_ADD_DEVICE_CFM_T;

typedef struct
{
    dm_prim_t           type;       /* DM_SM_READ_DEVICE_REQ */
    phandle_t           phandle;    /* phandle for response */
    TYPED_BD_ADDR_T     addrt;
} DM_SM_READ_DEVICE_REQ_T;

typedef struct
{
    dm_prim_t           type;       /* DM_SM_READ_DEVICE_CFM */
    phandle_t           phandle;    /* Destination phandle */
    TYPED_BD_ADDR_T     addrt;      /* BD_ADDR of device */
    uint8_t             status;
    DM_SM_TRUST_T       trust;      /* Trust level */
    DM_SM_KEYS_T        keys;       /* Security keys and requirements */
} DM_SM_READ_DEVICE_CFM_T;


typedef struct
{
    dm_prim_t           type;       /* DM_SM_REMOVE_DEVICE_REQ */
    phandle_t           phandle;    /* phandle for response */
    TYPED_BD_ADDR_T     addrt;
} DM_SM_REMOVE_DEVICE_REQ_T;

typedef struct
{
    dm_prim_t           type;       /* Always DM_SM_ADD_DEVICE_CFM */
    phandle_t           phandle;    /* Destination phandle */
    TYPED_BD_ADDR_T     addrt;
    uint8_t             status;
} DM_SM_REMOVE_DEVICE_CFM_T;

typedef enum
{
    DM_SM_PERMANENT_ADDRESS_UNCHANGED,
    DM_SM_PERMANENT_ADDRESS_PUBLIC,         /* Use public address */
    DM_SM_PERMANENT_ADDRESS_WRITE_STATIC,   /* Use included static address */
    DM_SM_PERMANENT_ADDRESS_GENERATE_STATIC /* Generate static address */
} DM_SM_PERMANENT_ADDRESS_T;

/* Enable/disable privacy on local device */
typedef struct
{
    dm_prim_t type;                     /* DM_SM_SET_PRIVACY_REQ */
    DM_SM_PERMANENT_ADDRESS_T set_permanent_address_type;
    TYPED_BD_ADDR_T static_addrt;
    DM_SM_PRIVACY_T privacy;            /* Enabled/Disable privacy */
} DM_SM_CONFIGURE_LOCAL_ADDRESS_REQ_T;

/* Confirmation */
typedef struct
{
    dm_prim_t type; /* DM_SM_SET_PRIVACY_CFM */
    phandle_t phandle; /* am_phandle */
    uint8_t status;
    uint8_t permanent_address_type;
    TYPED_BD_ADDR_T static_addrt;
    DM_SM_PRIVACY_T privacy;
} DM_SM_CONFIGURE_LOCAL_ADDRESS_CFM_T;

/* Ask the SM to generate a nonresolvable random address. */
typedef struct
{
    dm_prim_t type; /* DM_SM_GENERATE_NONRESOLVABLE_PRIVATE_ADDRESS_REQ */
} DM_SM_GENERATE_NONRESOLVABLE_PRIVATE_ADDRESS_REQ_T;

/* Get the random address we asked for. */
typedef struct
{
    dm_prim_t type; /* DM_SM_GENERATE_NONRESOLVABLE_PRIVATE_ADDRESS_REQ */
    phandle_t phandle; /* am_phandle */
    uint8_t status;
    TYPED_BD_ADDR_T addrt;
} DM_SM_GENERATE_NONRESOLVABLE_PRIVATE_ADDRESS_CFM_T;


/*-------------------------------------------------------------
 *
 *   CONNECTION PARAMETER CACHE INTERFACE
 *
 *---------------------------------------------------------------*/

/* Cache the page mode parameters for a given bd_addr */
typedef struct
{
    dm_prim_t            type;              /* Always DM_WRITE_CACHED_PAGE_MODE_REQ */
    BD_ADDR_T            bd_addr;           /* Bluetooth device address */
    page_scan_mode_t     page_scan_mode;
    page_scan_rep_mode_t page_scan_rep_mode;
} DM_WRITE_CACHED_PAGE_MODE_REQ_T;

        /* Always DM_WRITE_CACHED_PAGE_MODE_CFM */
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_WRITE_CACHED_PAGE_MODE_CFM_T;

/* Cache the clock offset parameter for a given bd_addr */
typedef struct
{
    dm_prim_t           type;               /* Always DM_WRITE_CACHED_CLOCK_OFFSET_REQ */
    BD_ADDR_T           bd_addr;            /* Bluetooth device address */
    uint16_t            clock_offset;       /* the clock offset */
} DM_WRITE_CACHED_CLOCK_OFFSET_REQ_T;

        /* Always DM_WRITE_CACHED_CLOCK_OFFSET_CFM */
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_WRITE_CACHED_CLOCK_OFFSET_CFM_T;

/* Clear all cached parameters for a given bd_addr */
typedef struct
{
    dm_prim_t           type;               /* Always DM_CLEAR_PARAM_CACHE_REQ */
    BD_ADDR_T           bd_addr;            /* Bluetooth device address */
} DM_CLEAR_PARAM_CACHE_REQ_T;

        /* Always DM_CLEAR_PARAM_CACHE_CFM */
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_CLEAR_PARAM_CACHE_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Request from Application task for the DM to create an ACL to the
 *      specified remote device. This might be used for pairing, for example.
 *      The DM allows both L2CAP and the application to request ACLs.
 *      The DM keeps track of who has an interest in
 *      each ACL, and will only disconnect the ACL when all interested parties
 *      have released the ACL (by DM_ACL_DISCONNECT_REQ and DM_ACL_CLOSE_REQ).
 *
 *      DM_ACL_OPEN_REQ registers the application's interest in the ACL. DM will
 *      create the link if it does not already exist. DM will always respond
 *      with DM_ACL_OPEN_CFM, indicating success or failure. If successful, the
 *      application may subsequently call DM_ACL_CLOSE_REQ to relinquish its
 *      interest in the ACL - DM will then release the link if L2CAP is not
 *      using it.
 *
 *      The DM keeps the application informed of the state of all ACLs via the
 *      DM_ACL_OPENED_IND and DM_ACL_CLOSED_IND primitives. Note that there is
 *      no specific response to DM_ACL_CLOSE_REQ, as the DM_ACL_CLOSED_IND is
 *      only issued when all users of the ACL have released it.
 *
 *----------------------------------------------------------------------------*/

/* Bitfield of options

   Bit 0     DM_ACL_FLAG_INCOMING
             Will be set in DM_ACL_OPENED_IND and DM_ACL_CLOSED_IND primitives
             if the ACL was created by the remote device. It shall be set to
             0 in DM_ACL_OPEN_REQ, DM_ACL_CLOSE_REQ, DM_SM_BONDING_REQ and
             DM_SM_BONDING_CANCEL_REQ.

   Bit 1     DM_ACL_FLAG_ALL
             Indicates that request should apply to all connections. If this
             is set then any Bluetooth address in the primitives will be
             ignored. This shall be set to 0 in DM_ACL_OPEN_REQ,
             DM_SM_BONDING_REQ and DM_SM_BONDING_CANCEL_REQ and will be
             set to 0 in DM_ACL_OPENED_IND and DM_ACL_CLOSED_IND.

   Bit 2     DM_ACL_FLAG_FORCE
             Indicates that a more aggressive version of the command should
             be used. This shall be set to 0 in DM_ACL_OPEN_REQ,
             DM_SM_BONDING_REQ and will be set to 0 in DM_ACL_OPENED_IND and
             DM_ACL_CLOSED_IND. If set in DM_ACL_CLOSE_REQ then the ACL will
             be disconnected irrespective of any L2CAP connections that are
             active. If set in DM_SM_BONDING_CANCEL_REQ then, if all else
             fails, the device manager will cancel bonding by bringing down
             the ACL, even if there are active L2CAP connections.

   Bit 3     DM_ACL_FLAG_ULP
             Reserved - shall be set to 0.

   Bits 4-15 Reserved
             Shall be set to 0.
*/
#define DM_ACL_FLAG_INCOMING 0x0001
#define DM_ACL_FLAG_ALL 0x0002
#define DM_ACL_FLAG_FORCE 0x0004
#define DM_ACL_FLAG_ULP 0x0008 

typedef struct
{
    dm_prim_t               type;           /* Always DM_ACL_OPEN_REQ */
    TYPED_BD_ADDR_T         addrt;          /* Remote device address */
    uint16_t                flags;          /* Bitfield of options */
} DM_ACL_OPEN_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Confirmation of an application request to create an ACL. See text for
 *      DM_ACL_OPEN_REQ for more details.
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t               type;           /* Always DM_ACL_OPEN_CFM */
    phandle_t               phandle;        /* Destination phandle */
    TYPED_BD_ADDR_T         addrt;          /* Remote device address */
    bool_t                  success;        /* TRUE if open, FALSE if not */
} DM_ACL_OPEN_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Request from the application to close the ACL to the specified device.
 *      An application may send this request only if it previously requested
 *      ACL creation via the DM_ACL_OPEN_REQ primitive.
 *
 *      There is no specific response to this primitive - DM_ACL_CLOSED_IND is
 *      issued when the ACL is closed, but the DM may keep the ACL open if L2CAP
 *      is still using it.
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t               type;      /* Always DM_ACL_CLOSE_REQ */
    TYPED_BD_ADDR_T         addrt;     /* Remote device address */
    uint16_t                flags;     /* See definitions for DM_ACL_OPEN_REQ */
    uint8_t                 reason;    /* Only used if 'FORCE' flag set */
} DM_ACL_CLOSE_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Letting higher layers know about ACL comings and goings. This primitive
 *      is issued to the application interface whenever an ACL is successfully
 *      established (incoming or outgoing).
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t               type;           /* Always DM_ACL_OPENED_IND */
    phandle_t               phandle;        /* Destination phandle */
    TYPED_BD_ADDR_T         addrt;          /* Peer device address */

    /* See definitions for DM_ACL_OPEN_REQ */
    uint16_t                flags;

    /* Class of device for peer, valid for incoming connections only */
    uint24_t                dev_class;
    hci_return_t            status;         /* Indication of success or failure. */
    
    /* Parameters valid when status is HCI_SUCCESS and 'flags' has DM_ACL_FLAG_ULP set*/
    DM_ACL_BLE_CONN_PARAMS_T  ble_conn_params;

} DM_ACL_OPENED_IND_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Letting higher layers know about ACL comings and goings. This primitive
 *      is issued to the application interface whenever a successfully
 *      established ACL is disconnected.
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t               type;      /* Always DM_ACL_CLOSED_IND */
    phandle_t               phandle;   /* Destination phandle */
    TYPED_BD_ADDR_T         addrt;     /* Peer device address */
    hci_error_t             reason;    /* Reason */
    uint16_t                flags;     /* See definitions for DM_ACL_OPEN_REQ */
} DM_ACL_CLOSED_IND_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Define the default link policy to be applied to all new connections.
 *      This does not affect existing connections.
 *
 *      For valid policy settings see 'HCI Link Policy Settings' in hci.h.
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t               type;           /* Always DM_SET_DEFAULT_LINK_POLICY_REQ */

    /* Link Manager policy settings for incoming and outgoing connections */
    link_policy_settings_t  link_policy_settings_in;
    link_policy_settings_t  link_policy_settings_out;
} DM_SET_DEFAULT_LINK_POLICY_REQ_T;

/*
 * Internal DM/AMPM Interface.
 */

/* Tell the DM how to contact the AMPM. I guess we don't need this if the
   DM is compiled to know where AMPM is found, but having it makes things
   more flexible. We could test the DM's AMPM interface from rfcli, or
   even have an AMPM in the VM.

            AMPM                                         DM
                        DM_AMPM_REGISTER_REQ
            --------------------------------------------->
                        DM_AMPM_REGISTER_CFM
            <---------------------------------------------
*/
typedef struct
{
    dm_prim_t       type;       /* Always DM_AMPM_REGISTER_REQ */
    phandle_t       phandle;    /* AMPM Message Queue ID */
} DM_AMPM_REGISTER_REQ_T;

typedef struct
{
    dm_prim_t       type;       /* Always DM_AMPM_REGISTER_CFM */
    phandle_t       phandle;    /* AMPM Message Queue ID */
} DM_AMPM_REGISTER_CFM_T;




/* Copy of DM_SM_LINK_KEY_IND_T to be sent to AMPM. Whenever the DM gets
   HCI_EV_LINK_KEY_NOTIFICATION, it passes DM_SM_LINK_KEY_IND up to the
   application. While it is doing that, it can just make a copy, change
   the 'type' field to DM_AMPM_LINK_KEY_NOTIFICATION_IND and send to AMPM.

            AMPM                                         DM
                    DM_AMPM_LINK_KEY_NOTIFICATION_IND
            <---------------------------------------------
*/
typedef DM_SM_LINK_KEY_IND_T DM_AMPM_LINK_KEY_NOTIFICATION_IND_T;




/* DM tells AMPM about need for physical connection. This happens after
   l2cap receives a connect request from a higher layer and after the ACL
   is up and encryption has been enabled, but before any l2cap connection
   signalling. On the initiating side, the AMPM will receive one of these
   for every l2cap channel to be connected, irrespective of whether or not
   the required physical connection exists. This is not sent on the responding
   side as by the time the responder knows about the l2cap connection, the
   physical channel is already connected.
 
            AMPM                                         DM
                        DM_AMPM_CONNECT_IND
            <---------------------------------------------
                        DM_AMPM_CONNECT_RSP
            --------------------------------------------->
*/
typedef struct
{
    dm_prim_t           type;           /* Always DM_AMPM_CONNECT_IND */
    phandle_t           phandle;        /* AMPM Message Queue ID */
    BD_ADDR_T           bd_addr;        /* Remote Bluetooth Address */
    l2ca_cid_t          cid;
    l2ca_controller_t   local_amp_id;
    l2ca_controller_t   remote_amp_id;
} DM_AMPM_CONNECT_IND_T;


typedef struct
{
    dm_prim_t           type;           /* Always DM_AMPM_CONNECT_RSP */
    BD_ADDR_T           bd_addr;        /* Remote Bluetooth Address */
    l2ca_cid_t          cid;            /* l2cap channel ID */
    l2ca_controller_t   local_amp_id;
    l2ca_controller_t   remote_amp_id;
    hci_return_t        status;         /* Anything other than 0 is failure */
} DM_AMPM_CONNECT_RSP_T;




/* DM tells AMPM about need for logical connection. This is sent after the
   l2cap connection and configuration signalling has successfully completed.
   AMPM is passed the final flow spec and is given the signal to establish
   a logical link. The AMPM will receive one of these for every l2cap
   channel to be connected even if the channel can be put on an existing
   logical link.
*/   
typedef struct
{
    dm_prim_t               type;           /* DM_AMPM_CONNECT_CHANNEL_IND */
    phandle_t               phandle;        /* AMPM Message Queue ID */
    hci_connection_handle_t handle;         /* HCI connection handle */
    BD_ADDR_T               bd_addr;        /* Remote Bluetooth Address */
    l2ca_cid_t              cid;            /* l2cap channel ID */
    l2ca_controller_t       local_amp_id;
    L2CA_FLOWSPEC_T         tx_flowspec;   /* TX QOS parameters */
    L2CA_FLOWSPEC_T         rx_flowspec;   /* RX QOS parameters */
} DM_AMPM_CONNECT_CHANNEL_IND_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */

typedef struct
{
    dm_prim_t           type;            /* DM_AMPM_CONNECT_CHANNEL_RSP */
    BD_ADDR_T           bd_addr;         /* Remote Bluetooth Address */
    l2ca_cid_t          cid;             /* l2cap channel ID */
    l2ca_controller_t   local_amp_id;    /* AMP ID */
    uint8_t             physical_link_id;/* AMP physical handle */
    amp_link_id_t       logical_link_id; /* AMP logical handle */
    phandle_t           hci_data_queue;  /* Where L2CAP sends to-air data */
    L2CA_FLOWSPEC_T     tx_flowspec;     /* TX QOS parameters */
    L2CA_FLOWSPEC_T     rx_flowspec;     /* RX QOS parameters */
    hci_return_t        status;          /* Anything other than 0 is failure */
} DM_AMPM_CONNECT_CHANNEL_RSP_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */

/* What follows is temporary stuff to allow the use of primitives equivalent
 * to the above in Rfcli. The problem is the repeated L2CA_FLOWSPEC_T
 * structures. Rfcli names its functions after the elements of the structures
 * and so we get duplicate function names, which annoys the compiler.
 * So we define equivalent primitives below that don't annoy Rfcli.
 */
typedef struct
{
    l2ca_fs_identifier_t   rx_fs_identifier;
    l2ca_fs_service_t      rx_fs_service_type;
    l2ca_fs_mtu_t          rx_fs_max_sdu;
    l2ca_bandwidth_t       rx_fs_interarrival;
    l2ca_latency_t         rx_fs_latency;
    l2ca_fs_flush_t        rx_fs_flush_to;
} L2CA_RX_FLOWSPEC_T;/* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT)*/
typedef struct
{
    dm_prim_t           type;           /* DM_AMPM_CONNECT_CHANNEL_IND */
    phandle_t           phandle;        /* AMPM Message Queue ID */
    hci_connection_handle_t handle;     /* HCI connection handle */
    BD_ADDR_T           bd_addr;        /* Remote Bluetooth Address */
    l2ca_cid_t          cid;            /* l2cap channel ID */
    l2ca_controller_t   local_amp_id;
    L2CA_FLOWSPEC_T     tx_flowspec;   /* TX QOS parameters */
    L2CA_RX_FLOWSPEC_T  rx_flowspec;   /* RX QOS parameters */
} DM_AMPM_CONN_CHANNEL_IND_T;
typedef struct
{
    dm_prim_t           type;            /* DM_AMPM_CONNECT_CHANNEL_RSP */
    l2ca_cid_t          cid;             /* l2cap channel ID */
    l2ca_controller_t   local_amp_id;    /* AMP ID */
    uint8_t             physical_link_id;/* AMP physical handle */
    amp_link_id_t       logical_link_id; /* AMP logical handle */
    phandle_t           hci_data_queue;  /* Where L2CAP sends to-air data */
    L2CA_FLOWSPEC_T     tx_flowspec;     /* TX QOS parameters */
    L2CA_RX_FLOWSPEC_T  rx_flowspec;     /* RX QOS parameters */
    hci_return_t        status;          /* Anything other than 0 is failure */
} DM_AMPM_CONN_CHANNEL_RSP_T;
#define DM_AMPM_CONN_CHANNEL_IND DM_AMPM_CONNECT_CHANNEL_IND
#define DM_AMPM_CONN_CHANNEL_RSP DM_AMPM_CONNECT_CHANNEL_RSP
/* End of Rfcli workaround */

/* DM tells AMPM about closed l2cap channel. When ever an l2cap channel is
   disconnected, the AMPM will receive one of these.
*/
typedef struct
{
    dm_prim_t           type;           /* DM_AMPM_DISCONNECT_CHANNEL_IND */
    phandle_t           phandle;        /* AMPM Message Queue ID */
    l2ca_cid_t          cid;            /* l2cap channel ID */
    l2ca_controller_t   local_amp_id;
} DM_AMPM_DISCONNECT_CHANNEL_IND_T;

typedef struct
{
    dm_prim_t           type;           /* DM_AMPM_DISCONNECT_CHANNEL_RSP */
    l2ca_cid_t          cid;            /* l2cap channel ID */
    l2ca_controller_t   local_amp_id;
    hci_return_t        status;         /* Anything other than 0 is failure */
} DM_AMPM_DISCONNECT_CHANNEL_RSP_T;




/* AMPM tells DM about link loss.

            AMPM                                         DM
                        DM_AMPM_DISCONNECT_REQ
            --------------------------------------------->
*/
typedef struct
{
    dm_prim_t           type;           /* Always DM_AMPM_DISCONNECT_REQ */
    BD_ADDR_T           bd_addr;        /* Remote Bluetooth Address */
    l2ca_controller_t   local_amp_id;
    hci_error_t         reason;         /* Reason for disconnect. */
    uint8_t             active_links;   /* Number of active links remaining */
} DM_AMPM_DISCONNECT_REQ_T;


/* AMPM reads the local Bluetooth address. */
typedef struct
{
    dm_prim_t           type;       /* Always DM_READ_BD_ADDR_REQ */
} DM_AMPM_READ_BD_ADDR_REQ_T;

typedef struct
{
    dm_prim_t           type;       /* Always DM_READ_BD_ADDR_CFM */
    phandle_t           phandle;    /* AMPM Message Queue ID */
    uint8_t             status;     /* Anything other than 0 is failure */
    BD_ADDR_T           bd_addr;    /* Local Bluetooth address */
} DM_AMPM_READ_BD_ADDR_CFM_T;

/* DM asks AMPM about whether a physical link (given by the local
 * controller ID) actually exists */
typedef struct
{
    dm_prim_t           type;         /* Always DM_AMPM_VERIFY_PHYSICAL_LINK_IND */
    phandle_t           phandle;      /* AMPM Message Queue ID */
    BD_ADDR_T           bd_addr;      /* Remote Bluetooth address */
    uint16_t            identifier;   /* Request identifier to match up with CFM */
    l2ca_controller_t   local_amp_id; /* Local AMP controller ID */
} DM_AMPM_VERIFY_PHYSICAL_LINK_IND_T;

typedef struct
{
    dm_prim_t           type;         /* Always DM_AMPM_VERIFY_PHYSICAL_LINK_RSP */
    BD_ADDR_T           bd_addr;      /* Remote Bluetooth address */
    uint16_t            identifier;   /* Identifier given in the REQ */
    bool_t              exists;       /* TRUE if link exists, FALSE if not */
    l2ca_fs_flush_t     link_supervision_timeout;  /* AMP LSTO - unit is in microseconds */
    l2ca_fs_flush_t     best_effort_flush_timeout; /* Default flush - uint is in microseconds */
} DM_AMPM_VERIFY_PHYSICAL_LINK_RSP_T;

/* DM requests Buffer size information from AMPM */
typedef struct
{
    dm_prim_t           type;                       /* Always DM_AMPM_READ_DATA_BLOCK_SIZE_IND */
    phandle_t           phandle;                    /* AMPM Message Queue ID */
    l2ca_controller_t   local_amp_id;               /* Local AMP controller ID */
} DM_AMPM_READ_DATA_BLOCK_SIZE_IND_T;

typedef struct
{
    dm_prim_t           type;                       /* Always DM_AMPM_READ_DATA_BLOCK_SIZE_RSP_T */
    l2ca_controller_t   local_amp_id;               /* Local AMP controller ID */
    uint8_t             status;                     /* Anything other than 0 is failure */
    bool_t              fragmentable;               /* Whether we can fragment packets over HCI */
    uint16_t            max_pdu_length;             /* Maximum l2cap PDU size */
    uint16_t            max_acl_data_packet_length; /* Maximum size of ACL data packets over HCI */
    uint16_t            data_block_length;          /* Size of data blocks */
    uint16_t            total_num_data_blocks;      /* Total number of data blocks */
} DM_AMPM_READ_DATA_BLOCK_SIZE_RSP_T;

/* Handle/Complete structure for returning credits. */
typedef struct
{
    amp_link_id_t       logical_link_handle;
    uint16_t            num_of_completed_packets;
    uint16_t            num_of_completed_blocks;
} DM_AMPM_NCB_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */

/* Calculate how many pointers we might need. */
#define DM_AMPM_NCB_HCI_BYTE_SIZE           6
#define DM_AMPM_NCB_XAP_SIZE                6
#define DM_AMPM_HANDLE_COMPLETES_PER_PTR    (HCI_VAR_ARG_POOL_SIZE/DM_AMPM_NCB_XAP_SIZE)
#define DM_AMPM_NUM_COMPLETED_BLOCKS_PTRS   ((254/DM_AMPM_NCB_HCI_BYTE_SIZE + DM_AMPM_HANDLE_COMPLETES_PER_PTR - 1) \
                                                / DM_AMPM_HANDLE_COMPLETES_PER_PTR)

/* AMPM informs DM of number of completed data blocks events */
typedef struct
{
    dm_prim_t           type;                       /* Always DM_AMPM_NUMBER_COMPLETED_DATA_BLOCKS_REQ */
    l2ca_controller_t   local_amp_id;               /* Local AMP controller ID */
    uint16_t            total_num_data_blocks;      /* Change total number of credits. Maximum reduction equal to credits
                                                       returned by this primitive. 0 means: Send me a new
                                                       DM_AMPM_READ_DATA_BLOCK_SIZE_IND */
    uint8_t             number_of_handles;          /* Total number of handles referenced by this primitive */

    DM_AMPM_NCB_T       *num_completed_blks_ptr[DM_AMPM_NUM_COMPLETED_BLOCKS_PTRS];
} DM_AMPM_NUMBER_COMPLETED_DATA_BLOCKS_REQ_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */

/* DM informs AMPM when it's in Security Mode 4 */
typedef struct
{
    dm_prim_t type;
    phandle_t phandle;
} DM_AMPM_SIMPLE_PAIRING_ENABLED_IND_T;

/* DM informs AMPM of relevant remote supported features */
#define DM_AMPM_REMOTE_FEATURES_NONE    0x0000  /* Nothing supported. */
#define DM_AMPM_REMOTE_FEATURES_SSP     0x0001  /* Full Secure Simple Pairing
                                                   support (controller+host) */
typedef struct
{
    dm_prim_t type;                     /* DM_AMPM_REMOTE_FEATURES_IND */
    phandle_t phandle;                  /* AMPM Message Queue ID */
    BD_ADDR_T bd_addr;                  /* Remote device Bluetooth address */
    uint16_t flags;                     /* Feature bitfield. See above. */
} DM_AMPM_REMOTE_FEATURES_IND_T;



typedef struct {
    dm_prim_t type;                     /* DM_DATA_FROM_HCI_REQ or
                                           DM_DATA_TO_HCI_IND */
    l2ca_controller_t controller;       /* AMP ID */
    uint8_t physical_handle;            /* AMP physical link handle. Not used for BR/EDR */
    uint16_t logical_handle;            /* AMP logical link handle or BR/EDR ACL handle */
    MBLK_T *data;                       /* Pointer to MBLK data. */
} DM_DATA_FROM_HCI_REQ_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */
typedef DM_DATA_FROM_HCI_REQ_T DM_DATA_TO_HCI_IND_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */

/*------------------------------------------------------------------------
 *
 *      HCI API PRIMITIVES
 *
 *-----------------------------------------------------------------------*/

/******************************************************************************
 Link Control 
 *****************************************************************************/

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Inquiry command 
 *
 *----------------------------------------------------------------------------*/

typedef HCI_INQUIRY_T DM_HCI_INQUIRY_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Inquiry cancel command 
 *
 *----------------------------------------------------------------------------*/

typedef HCI_INQUIRY_CANCEL_T DM_HCI_INQUIRY_CANCEL_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Notification of Inquiry cancel command complete
 *
 *----------------------------------------------------------------------------*/
        /* Always DM_HCI_INQUIRY_CANCEL_CFM_T */
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_INQUIRY_CANCEL_CFM_T;


/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Enter periodic inquiry mode
 *
 *----------------------------------------------------------------------------*/

typedef HCI_PERIODIC_INQUIRY_MODE_T DM_HCI_PERIODIC_INQUIRY_MODE_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Notification of entered periodic inquiry mode
 *
 *----------------------------------------------------------------------------*/
        /* Always DM_HCI_PERIODIC_INQUIRY_MODE_CFM_T */
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_PERIODIC_INQUIRY_MODE_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Exit periodic inquiry mode
 *
 *----------------------------------------------------------------------------*/

typedef HCI_EXIT_PERIODIC_INQUIRY_MODE_T DM_HCI_EXIT_PERIODIC_INQUIRY_MODE_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Notification of exited periodic inquiry mode
 *
 *----------------------------------------------------------------------------*/
        /* Always DM_HCI_EXIT_PERIODIC_INQUIRY_MODE_CFM_T */
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_EXIT_PERIODIC_INQUIRY_MODE_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Notification of inquiry results
 *
 *              See HCI_EV_INQUIRY_RESULT_T.
 *----------------------------------------------------------------------------*/

typedef struct
{
    dm_prim_t               type;           /* Always DM_HCI_INQUIRY_RESULT_IND_T */
    phandle_t               phandle;        /* destination phandle */
    uint8_t                 num_responses;

    HCI_INQ_RESULT_T *          result[HCI_MAX_INQ_RESULT_PTRS];

} DM_HCI_INQUIRY_RESULT_IND_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Notification of inquiry results with rssi information
 *
 *              See HCI_EV_INQUIRY_RESULT_WITH_RSSI_T.
 *----------------------------------------------------------------------------*/

#define DM_HCI_INQUIRY_RESULT_WITH_RSSI_IND ((dm_prim_t)(ENUM_DM_HCI_INQUIRY_RESULT_WITH_RSSI_IND))

typedef struct
{
    dm_prim_t               type;           /* Always DM_HCI_INQUIRY_RESULT_IND_T */
    phandle_t               phandle;        /* destination phandle */
    uint8_t                 num_responses;

    HCI_INQ_RESULT_WITH_RSSI_T *                result[HCI_MAX_INQ_RESULT_PTRS];

} DM_HCI_INQUIRY_RESULT_WITH_RSSI_IND_T;


/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Notification of inquiry complete
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_INQUIRY_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Request change of packet type on a connection.
 *
 *----------------------------------------------------------------------------*/
typedef HCI_CHANGE_CONN_PKT_TYPE_T DM_HCI_CHANGE_CONN_PKT_TYPE_REQ_T;

typedef DM_HCI_BD_ADDR_COMMAND_CFM_T DM_HCI_CONN_PACKET_TYPE_CHANGED_IND_T;
#define DM_HCI_CONN_PACKET_TYPE_CHANGED_IND \
                            ((dm_prim_t)(ENUM_DM_HCI_CONN_PACKET_TYPE_CHANGED_IND))

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Change link key for a device
 *
 *----------------------------------------------------------------------------*/

typedef HCI_CHANGE_CONN_LINK_KEY_T DM_HCI_CHANGE_CONN_LINK_KEY_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Notification of change of link key for a device
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_BD_ADDR_COMMAND_CFM_T DM_HCI_CHANGE_CONN_LINK_KEY_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Notification of link keys for devices
 *
 *              See HCI_EV_RETURN_LINK_KEYS_T for details.
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_RETURN_LINK_KEYS_IND_T */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             num_keys;           /* Number of link keys */

    LINK_KEY_BD_ADDR_T  *link_key_bd_addr[HCI_STORED_LINK_KEY_MAX];

} DM_HCI_RETURN_LINK_KEYS_IND_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Use the master device link keys
 *
 *----------------------------------------------------------------------------*/

typedef HCI_MASTER_LINK_KEY_T DM_HCI_MASTER_LINK_KEY_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Notification of use of the master device link keys
 *
 *----------------------------------------------------------------------------*/

typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_MASTER_LINK_KEY_CFM_T */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* Success or failure - See Note1 */
    uint8_t             key_flag;           /* Regular/Temporary link key */

} DM_HCI_MASTER_LINK_KEY_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Remote name request
 *
 *----------------------------------------------------------------------------*/

typedef HCI_REMOTE_NAME_REQ_T DM_HCI_REMOTE_NAME_REQUEST_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Notification of remote name
 *
 *----------------------------------------------------------------------------*/

typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_REMOTE_NAME_CFM_T */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* Success or failure - See Note1 */
    BD_ADDR_T           bd_addr;            /* Bluetooth device address */
    /* User friendly name */
    uint8_t             *name_part[HCI_LOCAL_NAME_BYTE_PACKET_PTRS]; 
} DM_HCI_REMOTE_NAME_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Read remote features
 *
 *----------------------------------------------------------------------------*/

typedef HCI_READ_REMOTE_SUPP_FEATURES_T DM_HCI_READ_REMOTE_SUPP_FEATURES_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Notification of remote features
 *
 *----------------------------------------------------------------------------*/

typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_READ_REMOTE_SUPP_FEATURES_CFM_T */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* Success or failure - See Note1 */
    BD_ADDR_T           bd_addr;            /* Bluetooth device address */
    uint16_t            features[4];        /* LMP features */

} DM_HCI_READ_REMOTE_SUPP_FEATURES_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Read remote version
 *
 *----------------------------------------------------------------------------*/

typedef HCI_READ_REMOTE_VER_INFO_T DM_HCI_READ_REMOTE_VER_INFO_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Notification of remote version information
 *
 *----------------------------------------------------------------------------*/

typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_READ_REMOTE_VER_INFO_CFM_T */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* Success or failure - See Note1 */
    TYPED_BD_ADDR_T     addrt;              /* Bluetooth device address */
    uint8_t             LMP_version;        /* Version of LMP */
    uint16_t            manufacturer_name;  /* Name of manufacturer */
    uint16_t            LMP_subversion;     /* Subversion of LMP */

} DM_HCI_READ_REMOTE_VER_INFO_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Read clock offset
 *
 *----------------------------------------------------------------------------*/

typedef HCI_READ_CLOCK_OFFSET_T DM_HCI_READ_CLOCK_OFFSET_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Read clock offset
 *
 *----------------------------------------------------------------------------*/

typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_READ_CLOCK_OFFSET_CFM_T */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* Success or failure - See Note1 */
    BD_ADDR_T           bd_addr;            /* Bluetooth device address */
    uint16_t            clock_offset;       /* Device's clock offset */

} DM_HCI_READ_CLOCK_OFFSET_CFM_T;

/******************************************************************************
   Link Policy 
 *****************************************************************************/

 /*----------------------------------------------------------------------------*
 * PURPOSE
 *      Put local or remote device into hold mode
 *      Use the remote Bluetooth device address to map onto  
 *      connection handle
 *
 *----------------------------------------------------------------------------*/

typedef HCI_HOLD_MODE_T DM_HCI_HOLD_MODE_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Put local device (for a particular connection) into sniff mode
 *      Note, SCO connections cannot be put into sniff.
 *
 *----------------------------------------------------------------------------*/

typedef HCI_SNIFF_MODE_T DM_HCI_SNIFF_MODE_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      End local device (for a particular connection) in sniff mode
 *      Note, SCO connections cannot be put into sniff.
 *
 *----------------------------------------------------------------------------*/

typedef HCI_EXIT_SNIFF_MODE_T DM_HCI_EXIT_SNIFF_MODE_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Put local or remote device into park mode
 *
 *----------------------------------------------------------------------------*/

typedef HCI_PARK_MODE_T DM_HCI_PARK_MODE_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Exit local or remote device in park mode
 *
 *----------------------------------------------------------------------------*/

typedef HCI_EXIT_PARK_MODE_T DM_HCI_EXIT_PARK_MODE_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Mode change event
 *
 *----------------------------------------------------------------------------*/

typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_MODE_CHANGE_EVENT_IND_T */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* Success or failure - See Note1 */
    BD_ADDR_T           bd_addr;            /* Bluetooth device address */
    uint8_t             mode;               /* Current mode */
    uint16_t            length;             /* Length of mode (or similar) */

} DM_HCI_MODE_CHANGE_EVENT_IND_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Quality of service setup from L2CAP
 *
 *----------------------------------------------------------------------------*/

typedef HCI_QOS_SETUP_T DM_HCI_QOS_SETUP_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Completion of quality of service setup 
 *
 *----------------------------------------------------------------------------*/
typedef struct 
{
    dm_prim_t                type;    /* always DM_HCI_QOS_SETUP_CFM */             
    phandle_t                phandle; /* destination phandle */
    hci_return_t             status;  /* cast to error if error */
    BD_ADDR_T                bd_addr;            /* Bluetooth device address */
    uint8_t                  flags;              /* reserved */
    hci_qos_type_t           service_type;
    uint32_t                 token_rate;
    uint32_t                 peak_bandwidth;
    uint32_t                 latency;
    uint32_t                 delay_variation;
} DM_HCI_QOS_SETUP_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Discover role
 *      Use the bd_addr to identify the (ACL) link at this interface
 *
 *----------------------------------------------------------------------------*/

typedef HCI_ROLE_DISCOVERY_T DM_HCI_ROLE_DISCOVERY_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Discover role complete
 *
 *----------------------------------------------------------------------------*/

typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_ROLE_DISCOVERY_CFM_T */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* Success or failure - See Note1 */
    BD_ADDR_T           bd_addr;            /* BD_ADDR of link */
    uint8_t             role;               /* Master or slave */
} DM_HCI_ROLE_DISCOVERY_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Switch role
 *      Use the bd_addr to identify the (ACL) link at this interface
 *
 *----------------------------------------------------------------------------*/

typedef HCI_SWITCH_ROLE_T DM_HCI_SWITCH_ROLE_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Switch role complete
 *      The current role is contained in 'role', even in case of failure.
 *
 *----------------------------------------------------------------------------*/

typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_SWITCH_ROLE_CFM_T */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* Success or failure - See Note1 */
    BD_ADDR_T           bd_addr;            /* BD_ADDR of link */
    uint8_t             role;               /* Master or slave */

} DM_HCI_SWITCH_ROLE_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Read link policy settings from LM
 *      Use the bd_addr to identify the (ACL) link at this interface
 *
 *----------------------------------------------------------------------------*/

typedef HCI_READ_LINK_POLICY_SETTINGS_T DM_HCI_READ_LINK_POLICY_SETTINGS_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Read link policy settings complete from LM
 *
 *----------------------------------------------------------------------------*/

typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_READ_LINK_POLICY_SETTINGS_CFM_T */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    BD_ADDR_T           bd_addr;            /* bd_addr of link */
    uint16_t            link_policy;        /* link policy settings */
} DM_HCI_READ_LINK_POLICY_SETTINGS_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Write link policy settings.
 *      Use the bd_addr to identify the ACL link at this interface.
 *
 *      Link Manager settings define the
 *      connection modes that are allowed (hold/sniff/park).
 *
 *      In all modes, the DM will keep the application informed of any changes
 *      of mode by issuing DM_HCI_MODE_CHANGE_EVENT_IND events.
 *
 *----------------------------------------------------------------------------*/

typedef HCI_WRITE_LINK_POLICY_SETTINGS_T DM_HCI_WRITE_LINK_POLICY_SETTINGS_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Defines the powerstate(s) to be applied to the specified ACL. It
 *      takes the form of a list of successive power modes which the ACL moves
 *      through over periods of increasing inactivity.  If at anytime there
 *      is activity on the ACL, control will reset to the first powerstate.
 *      
 *      Each powerstate is defined using the LP_POWERSTATE_T structure.
 *      The 'mode' entry is used to define the Bluetooth power mode.
 *      The 'min_interval' etc fields define the parameters of the Bluetooth
 *          power mode - see the HCI spec.
 *      The 'duration' is the length of time, in seconds, that the ACL will
 *          remain in this state.  When this period expires, control moves
 *          to the next state.  The last powerstate therefore MUST have an
 *          infinite duration (zero).
 *      
 *----------------------------------------------------------------------------*/
typedef uint8_t lp_powermode_t;
#define LP_POWERMODE_ACTIVE ((lp_powermode_t)0x00)
#define LP_POWERMODE_SNIFF  ((lp_powermode_t)0x01)
/* Passive mode is a "don't care" setting where the local device will not
   attempt to alter the power mode. */
#define LP_POWERMODE_PASSIVE ((lp_powermode_t)0xff)

typedef struct
{
    lp_powermode_t mode;
    uint16_t min_interval;
    uint16_t max_interval;
    uint16_t attempt;
    uint16_t timeout;
    uint16_t duration; /* time to spend in this mode */
} LP_POWERSTATE_T;/* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */ 

typedef struct
{
    dm_prim_t       type;
    BD_ADDR_T       bd_addr;    /* Bluetooth Device Address */
    uint16_t        num_states; /* Number of states in list */
    LP_POWERSTATE_T *states;    /* Table of power states */
} DM_LP_WRITE_POWERSTATES_REQ_T;


/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Generated in response to DM_LP_WRITE_POWERSTATES_REQ.
 *
 *----------------------------------------------------------------------------*/
#define LP_POWERSTATES_SUCCESS          ((uint8_t)0) /* powerstates set successfully */
#define LP_POWERSTATES_UNSUPPORTED_MODE ((uint8_t)1) /* unrecognised LP_POWERMODE */
#define LP_POWERSTATES_UNKNOWN_DEVICE   ((uint8_t)2) /* unknown Bluetooth device */
#define LP_POWERSTATES_BAD_TERMINATION  ((uint8_t)3) /* last state duration not zero */
#define LP_POWERSTATES_ERROR            ((uint8_t)4) /* error described above */
typedef struct
{
    dm_prim_t   type;
    phandle_t   phandle;            /* destination phandle */
    uint8_t     result;
    BD_ADDR_T   bd_addr;
} DM_LP_WRITE_POWERSTATES_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Allows the application to modify role-switch policy for ACL
 *      connection setup.
 *
 *      The policy takes the form of a table of conditions and actions.
 *      When the device manager is about to initiate a connection to a
 *      remote device, or is about to accept an incoming connection
 *      request, it walks down the table until it reaches an entry for
 *      which all conditions are met. Role-switch policy for that ACL
 *      connection is then determined by the actions specified.
 *
 *      There are four condition criteria:
 *
 *      1) Minimum number of connections to remote slaves.
 *      2) Minimum number of connections to remote masters.
 *      3) Minimum number of connections to remote unsniffed masters.
 *      4) Minimum number of connections.
 *
 *      Each entry in the table specifies separately the minimum number of 
 *      existing connections for each individual condition to be met. So the
 *      resolution operator for each condition is always >= 'greater than
 *      or equal to'.
 *
 *      There are two actions:
 *
 *      1) Accept incoming connection with role-switch (becoming master).
 *      2) Request outgoing connection, prohibiting role-switch (staying master).
 *
 *      Either, both, or neither action can be set for any specified 
 *      combination of conditions. If an action isn't specified, then the
 *      reverse happens, i.e. outgoing connections are requested allowing
 *      role-switch and incoming connections are accepted without requesting
 *      role-switch.
 *
 *      Encoding
 *      Each 'row' in the table is encoded into a single uint16.
 *      Each condition is encoded into 3 bits, giving a range of 0-7.
 *      Each action is encoded into 1 bit - setting the bit turns on the action.
 *
 *      Bits
 *        0-2  Condition - minimum number of remote slaves.
 *        3-5  Condition - minimum number of remote masters.
 *        6-8  Condition - minimum number of remote unsniffed masters.
 *        9-11 Condition - minimum number of connections.
 *       12    Action - Accept incoming connection with role-switch.
 *       13    Action - Request outgoing connection, prohibiting role-switch.
 *       15-16 Reserved - must be set to zero.
 *
 *       There are macros defined below to make packing the conditions easier.
 *
 *      Restrictions
 *      1) The final table entry must be 0 - i.e. 'always do nothing'.
 *      2) The reserved bits must all be set to 0.
 *      3) All table entries (except the last) must have at least one
 *         non-zero condition.
 *
 *      Operation
 *      Subsequent valid tables sent will replace any existing table.
 *      Sending a zero-length table will delete any existing table.
 *      If the device manager does not have a table then it will default
 *      to its legacy behaviour, which was to never prohibit role-switch
 *      when requesting connection creation and only to request a role-switch
 *      when accepting remote connection requests if there is already an
 *      existing connection to some other device.
 *
 *      Notes
 *      The purpose of this feature is to help manage and hopefully simplify
 *      scatternet topology. Using it for any other purpose is likely to
 *      severely reduce device interoperability. In particular, prohibiting 
 *      role-switch on outgoing connections should be used sparingly, as it
 *      may be that the topology that the remote device finds itself in is
 *      even more complicated than that for the local device and the only way
 *      for the connection to succeed is with a role-switch.
 *
 *--------------------------------------------------------------------------*/

/* Bits reserved per condition */
#define DM_LP_ROLESWITCH_CONDITION_BITS                     3

/* Conditions */
#define DM_ACL_NUM_SLAVES                                   0
#define DM_ACL_NUM_MASTERS                                  1
#define DM_ACL_NUM_UNSNIFFED_MASTERS                        2
#define DM_ACL_NUM_CONNECTIONS                              3
#define DM_ACL_NUM_END                                      4

/* Actions */
#define DM_LP_ROLESWITCH_ACTION_INCOMING_RS                 0x1000
#define DM_LP_ROLESWITCH_ACTION_OUTGOING_NO_RS              0x2000

 /* Reserved (must not be set) */
#define DM_LP_ROLESWITCH_RESERVED_1                         0x4000
#define DM_LP_ROLESWITCH_RESERVED_2                         0x8000

/* Macro to encode conditions */
#define DM_LP_ROLESWITCH_WRITE_CONDITION(value, condition)  \
    ((value) << ((condition) * DM_LP_ROLESWITCH_CONDITION_BITS))

/* Macro to decode conditions */
#define DM_LP_ROLESWITCH_READ_CONDITION(policy, condition)  \
    (((policy) >> ((condition) * DM_LP_ROLESWITCH_CONDITION_BITS)) & 7)


/* Macro to encode complete table entry
 *
 * slv, mst, usm, con are the four conditions - enter a number 0-7 for each
 * rs_in - enter 0 or DM_LP_ROLESWITCH_ACTION_INCOMING_RS
 * no_rs_out - enter 0 or DM_LP_ROLESWITCH_ACTION_OUTGOING_NO_RS
 */
#define DM_LP_ROLESWITCH_TABLE_ENTRY(slv, mst, usm, con, rs_in, no_rs_out) \
     (DM_LP_ROLESWITCH_WRITE_CONDITION((slv), DM_ACL_NUM_SLAVES) \
    | DM_LP_ROLESWITCH_WRITE_CONDITION((mst), DM_ACL_NUM_MASTERS) \
    | DM_LP_ROLESWITCH_WRITE_CONDITION((usm), DM_ACL_NUM_UNSNIFFED_MASTERS) \
    | DM_LP_ROLESWITCH_WRITE_CONDITION((con), DM_ACL_NUM_CONNECTIONS) \
    | rs_in \
    | no_rs_out)

/* Masks */
#define DM_LP_ROLESWITCH_CONDITION_MASK                     0x0FFF
#define DM_LP_ROLESWITCH_ACTION_MASK                        0x3000
#define DM_LP_ROLESWITCH_RESERVED_MASK                      0xC000

typedef struct
{
    dm_prim_t           type;               /* Always DM_LP_WRITE_ROLESWITCH_POLICY_REQ */
    uint16_t            version;            /* Must be set to 0 */
    uint16_t            length;             /* Number of uint16s in table */
    uint16_t            *rs_table;          /* Pointer to role-switch table */
} DM_LP_WRITE_ROLESWITCH_POLICY_REQ_T;

/*---------------------------------------------------------------------------*
 * PURPOSE
 *      Response to DM_LP_WRITE_ROLESWITCH_POLICY_REQ.
 *
 *---------------------------------------------------------------------------*/ 


#define DM_LP_WRITE_ROLESWITCH_POLICY_SUCCESS               ((uint8_t)0)
#define DM_LP_WRITE_ROLESWITCH_POLICY_ILLEGAL               ((uint8_t)1)
#define DM_LP_WRITE_ROLESWITCH_POLICY_UNSUPPORTED           ((uint8_t)2)
#define DM_LP_WRITE_ROLESWITCH_POLICY_NOT_ZERO_TERMINATED   ((uint8_t)3)
typedef struct
{
    dm_prim_t           type;               /* Always DM_LP_WRITE_ROLESWITCH_POLICY_CFM */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* Indication of success or failure */
} DM_LP_WRITE_ROLESWITCH_POLICY_CFM_T;

/*---------------------------------------------------------------------------*
 * PURPOSE
 *      Maintain list of remote devices for which the Device Manager will
 *      always try to become master during any ACL connection creation,
 *      even if there are no existing ACLs connected.
 *
 *      For locally-initiated connection requests to devices in the list,
 *      the Device Manager will prohibit role-switch, thus ensuring that
 *      the local device becomes master.
 *
 *      For remotely-initiated connection requests to devices in the list,
 *      the Device Manager will request a role-switch during connection
 *      creation. This may or may not be accepted by the remote device.
 *
 *      This primitive may be used to:
 *
 *      1) ADD a new device to the list
 *      2) DELETE a device from the list
 *      3) CLEAR the entire list
 *
 *      This primitive should be used only to work around problems with 
 *      severely misbehaving remote devices. Any other use is likely to
 *      produce a severely misbehaving local device and lead to major
 *      interoperability problems.
 *
 *      Role-switches should only be used to simplify topology. In most
 *      cases, DM_LP_WRITE_ROLESWITCH_POLICY_REQ/CFM should be sufficient.
 *      DM_LP_WRITE_ALWAYS_MASTER_DEVICES_REQ/CFM should only be used
 *      when it is necessary to become master, even when there are no
 *      existing connections, because the remote device is badly behaved
 *      and will not role-switch after connection creation and it is likely
 *      that further ACLs will soon be connected. (If it turns out that
 *      no further ACLs materialise after a connection affected by the
 *      device list is created then the local device might want to
 *      consider initiating a role-switch to become slave.)
 *---------------------------------------------------------------------------*/
#define DM_LP_WRITE_ALWAYS_MASTER_DEVICES_CLEAR     0x0000
#define DM_LP_WRITE_ALWAYS_MASTER_DEVICES_ADD       0x0001
#define DM_LP_WRITE_ALWAYS_MASTER_DEVICES_DELETE    0x0002
typedef struct
{
    dm_prim_t           type;           /* Always DM_LP_WRITE_ALWAYS_MASTER_DEVICES_REQ */
    uint16_t            operation;      /* ADD, DELETE, CLEAR. */
    BD_ADDR_T           bd_addr;        /* Pointer to array of Bluetooth addresses. */
} DM_LP_WRITE_ALWAYS_MASTER_DEVICES_REQ_T;

#define DM_LP_WRITE_ALWAYS_MASTER_DEVICES_SUCCESS   0x00
#define DM_LP_WRITE_ALWAYS_MASTER_DEVICES_ILLEGAL   0x01
typedef struct
{
    dm_prim_t           type;           /* Always DM_LP_WRITE_ALWAYS_MASTER_DEVICES_CFM */
    phandle_t           phandle;        /* destination phandle */
    uint8_t             status;         /* Indication of success or failure */
} DM_LP_WRITE_ALWAYS_MASTER_DEVICES_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Write link policy settings complete from LM
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_BD_ADDR_COMMAND_CFM_T DM_HCI_WRITE_LINK_POLICY_SETTINGS_CFM_T;

/*---------------------------------------------------------------------------*
 * PURPOSE
 *      FSM Debugging primitives. Provides details of Bluestack state 
 *      machine transitions.
 *      Will not be compiled into production firmware.
 *---------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t       type;
    phandle_t       phandle;
    uint16_t        state_machine;
    uint32_t        event;
    uint32_t        old_state;
    uint32_t        new_state;
    uint32_t        action;
} DM_BLUESTACK_STATE_TRANSITION_ENTRY_IND_T;

typedef struct
{
    dm_prim_t       type;
    phandle_t       phandle;
    uint16_t        state_machine;
    uint32_t        event;
} DM_BLUESTACK_STATE_TRANSITION_EXIT_IND_T;

/******************************************************************************
   Host Controller and Baseband
 *****************************************************************************/
/*
 * These primitives will map directly onto HCI primitives
 *
 */
typedef HCI_SET_EVENT_MASK_T DM_HCI_SET_EVENT_MASK_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Notification of set event mask
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_SET_EVENT_MASK_CFM_T;

typedef HCI_RESET_T DM_HCI_RESET_REQ_T;
/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Notification of reset command
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_RESET_CFM_T;

typedef HCI_SET_EVENT_FILTER_T DM_HCI_SET_EVENT_FILTER_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Request for flush
 *
 *----------------------------------------------------------------------------*/
typedef HCI_FLUSH_T DM_HCI_FLUSH_REQ_T;

        /* Always DM_HCI_SET_EVENT_FILTER_CFM_T */
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_SET_EVENT_FILTER_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Notification of flush complete
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_BD_ADDR_COMMAND_CFM_T DM_HCI_FLUSH_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Notification of set event filter command
 *
 *----------------------------------------------------------------------------*/
        
typedef HCI_READ_PIN_TYPE_T DM_HCI_READ_PIN_TYPE_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Read pin type complete event
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_READ_PIN_TYPE_CFM_T */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    pin_type_t          pin_type;           /* the pin type */

} DM_HCI_READ_PIN_TYPE_CFM_T;

typedef HCI_WRITE_PIN_TYPE_T DM_HCI_WRITE_PIN_TYPE_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Write pin type complete event
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_WRITE_PIN_TYPE_CFM_T;

typedef HCI_CREATE_NEW_UNIT_KEY_T DM_HCI_CREATE_NEW_UNIT_KEY_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Create new unit key complete event
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_CREATE_NEW_UNIT_KEY_CFM_T;

typedef HCI_READ_STORED_LINK_KEY_T DM_HCI_READ_STORED_LINK_KEY_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Read stored link key complete event
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_READ_STORED_LINK_KEY_CFM_T */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    uint16_t            max_num_keys;
    uint16_t            num_keys_read;
} DM_HCI_READ_STORED_LINK_KEY_CFM_T;

typedef HCI_WRITE_STORED_LINK_KEY_T DM_HCI_WRITE_STORED_LINK_KEY_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Write stored link key complete event
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_WRITE_STORED_LINK_KEY_CFM_T */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    uint8_t             num_keys_written;
} DM_HCI_WRITE_STORED_LINK_KEY_CFM_T;

typedef HCI_DELETE_STORED_LINK_KEY_T DM_HCI_DELETE_STORED_LINK_KEY_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Delete stored link keys complete event
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_DELETE_STORED_LINK_KEY_CFM */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    uint16_t            num_keys_deleted;
} DM_HCI_DELETE_STORED_LINK_KEY_CFM_T;

typedef HCI_CHANGE_LOCAL_NAME_T DM_HCI_CHANGE_LOCAL_NAME_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Change local name complete event
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_CHANGE_LOCAL_NAME_CFM_T;

typedef HCI_READ_CONN_ACCEPT_TIMEOUT_T DM_HCI_READ_CONN_ACCEPT_TIMEOUT_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      read local name 
 *
 *----------------------------------------------------------------------------*/
typedef HCI_READ_LOCAL_NAME_T DM_HCI_READ_LOCAL_NAME_REQ_T;

typedef struct 
{
    dm_prim_t           type;               /* Always DM_HCI_READ_LOCAL_NAME_CFM */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    /* See HCI_READ_LOCAL_NAME_RET_T */
    uint8_t            *name_part[HCI_LOCAL_NAME_BYTE_PACKET_PTRS]; 
} DM_HCI_READ_LOCAL_NAME_CFM_T;

typedef HCI_WRITE_CONN_ACCEPT_TIMEOUT_T DM_HCI_WRITE_CONN_ACCEPT_TIMEOUT_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Read connection accept timeout complete event
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_READ_CONN_ACCEPT_TIMEOUT_CFM */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    uint16_t            conn_accept_timeout;/* the timeout value */
} DM_HCI_READ_CONN_ACCEPT_TIMEOUT_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Write connection accept timeout complete event
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_WRITE_CONN_ACCEPT_TIMEOUT_CFM_T;

typedef HCI_READ_PAGE_TIMEOUT_T DM_HCI_READ_PAGE_TIMEOUT_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Read page timeout complete event
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_READ_PAGE_TIMEOUT_CFM_T */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    uint16_t            page_timeout;       /* the timeout value */
} DM_HCI_READ_PAGE_TIMEOUT_CFM_T;

typedef HCI_WRITE_PAGE_TIMEOUT_T DM_HCI_WRITE_PAGE_TIMEOUT_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Write page timeout complete event
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_WRITE_PAGE_TIMEOUT_CFM_T;

typedef HCI_READ_SCAN_ENABLE_T DM_HCI_READ_SCAN_ENABLE_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Read scan enable complete event
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_READ_SCAN_ENABLE_CFM_T */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    uint8_t             scan_enable;        /* return parameter */
} DM_HCI_READ_SCAN_ENABLE_CFM_T;

typedef HCI_WRITE_SCAN_ENABLE_T DM_HCI_WRITE_SCAN_ENABLE_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Write scan enable complete event
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_WRITE_SCAN_ENABLE_CFM_T;

typedef HCI_READ_PAGESCAN_ACTIVITY_T DM_HCI_READ_PAGESCAN_ACTIVITY_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Read pagescan activity complete
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_READ_PAGESCAN_ACTIVITY_CFM_T */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    uint16_t            pagescan_interval;
    uint16_t            pagescan_window;
} DM_HCI_READ_PAGESCAN_ACTIVITY_CFM_T;

typedef HCI_WRITE_PAGESCAN_ACTIVITY_T DM_HCI_WRITE_PAGESCAN_ACTIVITY_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Write pagescan activity complete
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_WRITE_PAGESCAN_ACTIVITY_CFM_T;

typedef HCI_READ_INQUIRYSCAN_ACTIVITY_T DM_HCI_READ_INQUIRYSCAN_ACTIVITY_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Read inquiryscan activity complete
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_READ_INQUIRYSCAN_ACTIVITY_CFM_T */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    uint16_t            inqscan_interval;
    uint16_t            inqscan_window;
} DM_HCI_READ_INQUIRYSCAN_ACTIVITY_CFM_T;

typedef HCI_WRITE_INQUIRYSCAN_ACTIVITY_T DM_HCI_WRITE_INQUIRYSCAN_ACTIVITY_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Write inquiryscan activity complete
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_WRITE_INQUIRYSCAN_ACTIVITY_CFM_T;

typedef HCI_READ_AUTH_ENABLE_T DM_HCI_READ_AUTH_ENABLE_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Read authentication enable complete
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_READ_AUTH_ENABLE_CFM */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    uint8_t             auth_enable;
} DM_HCI_READ_AUTH_ENABLE_CFM_T;

typedef HCI_READ_ENC_MODE_T DM_HCI_READ_ENC_MODE_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Read encryption mode complete
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_READ_ENC_MODE_CFM */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    uint8_t             mode;
} DM_HCI_READ_ENC_MODE_CFM_T;

typedef HCI_READ_CLASS_OF_DEVICE_T DM_HCI_READ_CLASS_OF_DEVICE_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Read class of device complete
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_READ_CLASS_OF_DEVICE_CFM_T */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    uint32_t            dev_class;
} DM_HCI_READ_CLASS_OF_DEVICE_CFM_T;

typedef HCI_WRITE_CLASS_OF_DEVICE_T DM_HCI_WRITE_CLASS_OF_DEVICE_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Write class of device complete
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_WRITE_CLASS_OF_DEVICE_CFM_T;

typedef HCI_READ_VOICE_SETTING_T DM_HCI_READ_VOICE_SETTING_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Read voice setting complete
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_READ_VOICE_SETTING_CFM */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    uint16_t            voice_setting;
} DM_HCI_READ_VOICE_SETTING_CFM_T;

typedef HCI_WRITE_VOICE_SETTING_T DM_HCI_WRITE_VOICE_SETTING_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Write voice setting complete
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_WRITE_VOICE_SETTING_CFM_T;

typedef HCI_READ_NUM_BCAST_RETXS_T DM_HCI_READ_NUM_BCAST_RETXS_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Request for Reading the automatic flush timeout
 *
 *----------------------------------------------------------------------------*/
typedef HCI_READ_AUTO_FLUSH_TIMEOUT_T DM_HCI_READ_AUTO_FLUSH_TIMEOUT_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Read Auto Flush Timeout complete
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_READ_AUTO_FLUSH_CFM */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    BD_ADDR_T           bd_addr;            /* bd_addr of link (Always ACL)*/
    uint16_t            FlushTimeout;       /* 11 bits of this number * 0.625ms */
} DM_HCI_READ_AUTO_FLUSH_TIMEOUT_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Request for Writing the automatic flush timeout
 *
 *----------------------------------------------------------------------------*/
typedef HCI_WRITE_AUTO_FLUSH_TIMEOUT_T DM_HCI_WRITE_AUTO_FLUSH_TIMEOUT_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Write Auto Flush Timeout complete
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_BD_ADDR_COMMAND_CFM_T DM_HCI_WRITE_AUTO_FLUSH_TIMEOUT_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Request for Reading SCO Flow Control Settings
 *
 *----------------------------------------------------------------------------*/
typedef HCI_READ_SCO_FLOW_CON_ENABLE_T DM_HCI_READ_SCO_FLOW_CON_ENABLE_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Write Auto Flush Timeout complete
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    uint8_t             FlowControlEnabled; 
} DM_HCI_READ_SCO_FLOW_CON_ENABLE_CFM_T;/* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */ 

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Request for Writing SCO Flow Control Settings
 *
 *----------------------------------------------------------------------------*/
typedef HCI_WRITE_SCO_FLOW_CON_ENABLE_T DM_HCI_WRITE_SCO_FLOW_CON_ENABLE_REQ_T;/* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */ 

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Write SCO Flow Control complete
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_WRITE_SCO_FLOW_CON_ENABLE_CFM_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Read number of broadcast transmissions complete
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_READ_NUM_BCAST_RETXS_CFM_T */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    uint8_t             num;
} DM_HCI_READ_NUM_BCAST_RETXS_CFM_T;

typedef HCI_WRITE_NUM_BCAST_RETXS_T DM_HCI_WRITE_NUM_BCAST_RETXS_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Write number of broadcast transmissions complete
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_WRITE_NUM_BCAST_RETXS_CFM_T;

typedef HCI_READ_HOLD_MODE_ACTIVITY_T DM_HCI_READ_HOLD_MODE_ACTIVITY_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Write number of broadcast transmissions complete
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_READ_HOLD_MODE_ACTIVITY_CFM_T */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    uint8_t             activity;
} DM_HCI_READ_HOLD_MODE_ACTIVITY_CFM_T;

typedef HCI_WRITE_HOLD_MODE_ACTIVITY_T DM_HCI_WRITE_HOLD_MODE_ACTIVITY_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Write hold mode activity complete
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_WRITE_HOLD_MODE_ACTIVITY_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Read transmit power level - use the bd_addr to reference the link
 *
 *----------------------------------------------------------------------------*/
typedef HCI_READ_TX_POWER_LEVEL_T DM_HCI_READ_TX_POWER_LEVEL_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Read transmit power level complete - use the bd_addr to reference the link
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_READ_TX_POWER_LEVEL_CFM_T */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    TYPED_BD_ADDR_T     addrt;              /* bd_addr of the link */
    int8_t              pwr_level;
} DM_HCI_READ_TX_POWER_LEVEL_CFM_T;

typedef HCI_HOST_NUM_COMPLETED_PACKETS_T DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Num completed packets complete
 *      Only used if there is an error
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_HOST_NUM_COMPLETED_PACKETS_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      read link supervision timeout 
 *
 *----------------------------------------------------------------------------*/
typedef HCI_READ_LINK_SUPERV_TIMEOUT_T DM_HCI_READ_LINK_SUPERV_TIMEOUT_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      read link supervision timeout complete
 *
 *----------------------------------------------------------------------------*/
typedef struct 
{
    dm_prim_t           type;               /* Always DM_HCI_READ_LINK_SUPERV_TIMEOUT_CFM_T */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    BD_ADDR_T           bd_addr;
    uint16_t            timeout;
} DM_HCI_READ_LINK_SUPERV_TIMEOUT_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Write link supervision timeout
 *
 *----------------------------------------------------------------------------*/
typedef HCI_WRITE_LINK_SUPERV_TIMEOUT_T DM_HCI_WRITE_LINK_SUPERV_TIMEOUT_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      write link supervision timeout complete
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_BD_ADDR_COMMAND_CFM_T DM_HCI_WRITE_LINK_SUPERV_TIMEOUT_CFM_T;

typedef HCI_READ_NUM_SUPPORTED_IAC_T DM_HCI_READ_NUM_SUPPORTED_IAC_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      read num support iac complete
 *
 *----------------------------------------------------------------------------*/
typedef struct 
{
    dm_prim_t           type;               /* Always DM_HCI_READ_NUM_SUPPORTED_IAC_CFM_T */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    uint8_t             num;
} DM_HCI_READ_NUM_SUPPORTED_IAC_CFM_T;

typedef HCI_READ_CURRENT_IAC_LAP_T DM_HCI_READ_CURRENT_IAC_LAP_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      read current iac lap complete
 *
 *----------------------------------------------------------------------------*/
typedef struct 
{
    dm_prim_t           type;               /* Always DM_HCI_READ_CURRENT_IAC_LAP_CFM_T */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    uint8_t             num_current_iac;

    /* Array of HCI_IAC_LAP_PTRS pointers to HCI_IAC_LAP_PER_PTR iac */
    uint24_t *			iac_lap[HCI_IAC_LAP_PTRS];
} DM_HCI_READ_CURRENT_IAC_LAP_CFM_T;

typedef HCI_WRITE_CURRENT_IAC_LAP_T DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      write current iac lap complete
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_WRITE_CURRENT_IAC_LAP_CFM_T;
  
typedef HCI_READ_PAGESCAN_PERIOD_MODE_T DM_HCI_READ_PAGESCAN_PERIOD_MODE_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      read pagescan period mode complete
 *
 *----------------------------------------------------------------------------*/
typedef struct 
{
    dm_prim_t           type;               /* Always DM_HCI_READ_PAGESCAN_PERIOD_MODE_CFM_T */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    uint8_t             mode;
} DM_HCI_READ_PAGESCAN_PERIOD_MODE_CFM_T;

typedef HCI_WRITE_PAGESCAN_PERIOD_MODE_T DM_HCI_WRITE_PAGESCAN_PERIOD_MODE_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      write pagescan period mode complete
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_WRITE_PAGESCAN_PERIOD_MODE_CFM_T;

typedef HCI_READ_PAGESCAN_MODE_T DM_HCI_READ_PAGESCAN_MODE_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      read pagescan mode complete
 *
 *----------------------------------------------------------------------------*/
typedef struct 
{
    dm_prim_t           type;               /* Always DM_HCI_READ_PAGESCAN_MODE_CFM_T */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    uint8_t             mode;
} DM_HCI_READ_PAGESCAN_MODE_CFM_T;

typedef HCI_WRITE_PAGESCAN_MODE_T DM_HCI_WRITE_PAGESCAN_MODE_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      write pagescan mode complete
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_WRITE_PAGESCAN_MODE_CFM_T;

/******************************************************************************
   Informational parameters
 *****************************************************************************/

typedef HCI_READ_LOCAL_VER_INFO_T DM_HCI_READ_LOCAL_VER_INFO_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Read local version complete
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_READ_LOCAL_VER_INFO_CFM */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    hci_version_t       hci_version;
    uint16_t            hci_revision;
    uint8_t             lmp_version;        /* defined in LMP */
    uint16_t            manuf_name;         /* defined in LMP */
    uint16_t            lmp_subversion;
} DM_HCI_READ_LOCAL_VER_INFO_CFM_T;

typedef HCI_READ_LOCAL_SUPP_FEATURES_T DM_HCI_READ_LOCAL_SUPP_FEATURES_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Read local features complete
 *
 *----------------------------------------------------------------------------*/
typedef struct 
{
    dm_prim_t           type;               /* Always DM_HCI_READ_LOCAL_SUPP_FEATURES_CFM */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    uint8_t             lmp_supp_features[8]; 
} DM_HCI_READ_LOCAL_SUPP_FEATURES_CFM_T;

typedef HCI_READ_COUNTRY_CODE_T DM_HCI_READ_COUNTRY_CODE_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Read country code complete
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t           type;               /* Always DM_HCI_READ_COUNTRY_CODE_CFM */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* status */
    uint8_t             country_code;
} DM_HCI_READ_COUNTRY_CODE_CFM_T;

typedef HCI_READ_BD_ADDR_T DM_HCI_READ_BD_ADDR_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      read bd_addr return primitive
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_BD_ADDR_COMMAND_CFM_T DM_HCI_READ_BD_ADDR_CFM_T;

/******************************************************************************
   Status parameters
 *****************************************************************************/

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      read failed contact counter
 *
 *----------------------------------------------------------------------------*/
typedef HCI_READ_FAILED_CONTACT_COUNT_T DM_HCI_READ_FAILED_CONTACT_COUNT_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      read failed contact counter complete
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t               type;    /* always DM_HCI_READ_FAILED_CONTACT_COUNT_CFM */
    phandle_t               phandle; /* destination phandle */
    uint8_t                 status;         /* Success or failure - See Note1 */
    BD_ADDR_T               bd_addr;
    uint16_t                failed_contact_count;
} DM_HCI_READ_FAILED_CONTACT_COUNT_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      reset contact counter request
 *
 *----------------------------------------------------------------------------*/
typedef HCI_RESET_FAILED_CONTACT_COUNT_T DM_HCI_RESET_FAILED_CONTACT_COUNT_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      reset contact counter complete
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_BD_ADDR_COMMAND_CFM_T DM_HCI_RESET_FAILED_CONTACT_COUNT_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      get link quality
 *
 *----------------------------------------------------------------------------*/
typedef HCI_GET_LINK_QUALITY_T DM_HCI_GET_LINK_QUALITY_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      get link quality complete
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t               type;    /* always DM_HCI_GET_LINK_QUALITY_CFM */
    phandle_t               phandle; /* destination phandle */
    uint8_t                 status;         /* Success or failure - See Note1 */
    BD_ADDR_T               bd_addr;
    uint8_t                 link_quality;
} DM_HCI_GET_LINK_QUALITY_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      read RSSI
 *
 *----------------------------------------------------------------------------*/
typedef HCI_READ_RSSI_T DM_HCI_READ_RSSI_REQ_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      read RSSI complete
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t               type;    /* always DM_HCI_READ_RSSI_CFM */
    phandle_t               phandle; /* destination phandle */
    uint8_t                 status;         /* Success or failure - See Note1 */
    TYPED_BD_ADDR_T         addrt;
    int8_t                  rssi;
} DM_HCI_READ_RSSI_CFM_T;

/******************************************************************************
   Testing primitives
 *****************************************************************************/
/*
 * These primitives will map directly onto HCI primitives
 *
 */

typedef HCI_ENABLE_DUT_MODE_T DM_HCI_ENABLE_DUT_MODE_REQ_T;            

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Enable device under test complete
 *
 *----------------------------------------------------------------------------*/
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_ENABLE_DUT_MODE_CFM_T;

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Common elements of an "upstream" primitive
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t               type; 
    phandle_t               phandle;
} DM_UPSTREAM_COMMON_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */ 

typedef struct
{
    dm_prim_t               type;
    phandle_t               phandle;
    BD_ADDR_T               bd_addr;
} DM_UPSTREAM_BD_ADDR_COMMON_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */

typedef struct
{
    dm_prim_t               type;
    phandle_t               phandle;
    TYPED_BD_ADDR_T         addrt;
} DM_UPSTREAM_TYPED_BD_ADDR_COMMON_T; /* autogen_makefile_ignore_this (DO NOT REMOVE THIS COMMENT) */

typedef HCI_CREATE_CONNECTION_CANCEL_T DM_HCI_CREATE_CONNECTION_CANCEL_REQ_T;
#define DM_HCI_CREATE_CONNECTION_CANCEL_REQ ((dm_prim_t)(ENUM_DM_HCI_CREATE_CONNECTION_CANCEL_REQ)) 
#define DM_HCI_CREATE_CONNECTION_CANCEL_CFM ((dm_prim_t)(ENUM_DM_HCI_CREATE_CONNECTION_CANCEL_CFM)) 

typedef DM_HCI_BD_ADDR_COMMAND_CFM_T DM_HCI_CREATE_CONNECTION_CANCEL_CFM_T;

typedef HCI_REMOTE_NAME_REQ_CANCEL_T DM_HCI_REMOTE_NAME_REQ_CANCEL_REQ_T;
#define DM_HCI_REMOTE_NAME_REQ_CANCEL_REQ ((dm_prim_t)(ENUM_DM_HCI_REMOTE_NAME_REQ_CANCEL_REQ))
#define DM_HCI_REMOTE_NAME_REQ_CANCEL_CFM ((dm_prim_t)(ENUM_DM_HCI_REMOTE_NAME_REQ_CANCEL_CFM))

typedef DM_HCI_BD_ADDR_COMMAND_CFM_T DM_HCI_REMOTE_NAME_REQ_CANCEL_CFM_T;

typedef HCI_READ_REMOTE_EXT_FEATURES_T DM_HCI_READ_REMOTE_EXT_FEATURES_REQ_T;
#define DM_HCI_READ_REMOTE_EXT_FEATURES_REQ ((dm_prim_t)(ENUM_DM_HCI_READ_REMOTE_EXT_FEATURES_REQ))
#define DM_HCI_READ_REMOTE_EXT_FEATURES_CFM ((dm_prim_t)(ENUM_DM_HCI_READ_REMOTE_EXT_FEATURES_CFM))

typedef struct
{
     dm_prim_t               type;           /* DM_HCI_READ_REMOTE_EXT_FEATURES_CFM_T */
     phandle_t               phandle;        /* destination phandle */
     uint8_t                 status;         /* Success or failure - See Note1 */
     BD_ADDR_T               bd_addr ;
     uint8_t                 page_num;
     uint8_t                 max_page_num;
     uint16_t                lmp_ext_features[4];
} DM_HCI_READ_REMOTE_EXT_FEATURES_CFM_T;


typedef HCI_READ_LMP_HANDLE_T DM_HCI_READ_LMP_HANDLE_REQ_T;
#define DM_HCI_READ_LMP_HANDLE_REQ ((dm_prim_t)(ENUM_DM_HCI_READ_LMP_HANDLE_REQ))
#define DM_HCI_READ_LMP_HANDLE_CFM ((dm_prim_t)(ENUM_DM_HCI_READ_LMP_HANDLE_CFM))

typedef struct
{
    dm_prim_t               type;           /* DM_HCI_READ_LMP_HANDLE_CFM_T */
    phandle_t               phandle;        /* destination phandle */
    uint8_t                 status;         /* Success or failure - See Note1 */
    hci_connection_handle_t handle;
    uint8_t                 lmp_handle;
    uint32_t                reserved;
} DM_HCI_READ_LMP_HANDLE_CFM_T;


typedef HCI_READ_DEFAULT_LINK_POLICY_SETTINGS_T DM_HCI_READ_DEFAULT_LINK_POLICY_SETTINGS_REQ_T;
#define DM_HCI_READ_DEFAULT_LINK_POLICY_SETTINGS_REQ ((dm_prim_t)(ENUM_DM_HCI_READ_DEFAULT_LINK_POLICY_SETTINGS_REQ))
#define DM_HCI_READ_DEFAULT_LINK_POLICY_SETTINGS_CFM ((dm_prim_t)(ENUM_DM_HCI_READ_DEFAULT_LINK_POLICY_SETTINGS_CFM))

typedef struct
{
    dm_prim_t               type;           /* DM_HCI_READ_DEFAULT_LINK_POLICY_SETTINGS_CFM_T */
    phandle_t               phandle;        /* destination phandle */
    uint8_t                 status;         /* Success or failure - See Note1 */
    link_policy_settings_t  default_lps;   
} DM_HCI_READ_DEFAULT_LINK_POLICY_SETTINGS_CFM_T;


typedef HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS_T DM_HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS_REQ_T;
#define DM_HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS_REQ ((dm_prim_t)(ENUM_DM_HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS_REQ))
#define DM_HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS_CFM ((dm_prim_t)(ENUM_DM_HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS_CFM))

typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS_CFM_T;

typedef HCI_FLOW_SPEC_T DM_HCI_FLOW_SPEC_REQ_T;
#define DM_HCI_FLOW_SPEC_REQ ((dm_prim_t)(ENUM_DM_HCI_FLOW_SPEC_REQ))
#define DM_HCI_FLOW_SPEC_CFM ((dm_prim_t)(ENUM_DM_HCI_FLOW_SPEC_CFM))

typedef struct
{
    dm_prim_t               type;           /* DM_HCI_FLOW_SPEC_CFM_T */
    phandle_t               phandle;        /* destination phandle */
    uint8_t                 status;         /* Success or failure - See Note1 */
    BD_ADDR_T              bd_addr ;
    uint8_t                 flags;
    uint8_t                 flow_direction; /* 0=out (to air), 1=in (from air) */
    uint8_t                 service_type;
    uint32_t                token_rate;
    uint32_t                token_bucket_size;
    uint32_t                peak_bandwidth;
    uint32_t                access_latency;    
} DM_HCI_FLOW_SPEC_CFM_T;

typedef struct
{
    dm_prim_t               type;           /* DM_HCI_SNIFF_SUB_RATING_IND */
    phandle_t               phandle;        /* destination phandle */
    uint8_t                 status;
    BD_ADDR_T               bd_addr;
    uint16_t                transmit_latency;
    uint16_t                receive_latency;
    uint16_t                remote_timeout;
    uint16_t                local_timeout;
} DM_HCI_SNIFF_SUB_RATING_IND_T;
typedef HCI_SNIFF_SUB_RATE_T DM_HCI_SNIFF_SUB_RATE_REQ_T;
#define DM_HCI_SNIFF_SUB_RATE_REQ ((dm_prim_t)(ENUM_DM_HCI_SNIFF_SUB_RATE_REQ))
#define DM_HCI_SNIFF_SUB_RATE_CFM \
                            ((dm_prim_t)(ENUM_DM_HCI_SNIFF_SUB_RATE_CFM))
#define DM_HCI_SNIFF_SUB_RATING_IND \
                            ((dm_prim_t)(ENUM_DM_HCI_SNIFF_SUB_RATING_IND))

typedef DM_HCI_BD_ADDR_COMMAND_CFM_T DM_HCI_SNIFF_SUB_RATE_CFM_T;

typedef HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_T DM_HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_REQ_T;
#define DM_HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_REQ \
                ((dm_prim_t)(ENUM_DM_HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_REQ))
typedef HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T DM_HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_REQ_T;
#define DM_HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_REQ \
                ((dm_prim_t)(ENUM_DM_HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_REQ))

typedef struct
{
    /* DM_HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_CFM */
    dm_prim_t               type;
    uint16_t                phandle;
    uint8_t                 status;
    uint8_t                 fec_required;
    uint8_t                 *eir_data_part[HCI_EIR_DATA_PACKET_PTRS];
} DM_HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_CFM_T;

#define DM_HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_CFM \
    ((dm_prim_t)(ENUM_DM_HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_CFM))

typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_CFM_T;

#define DM_HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_CFM \
    ((dm_prim_t)(ENUM_DM_HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_CFM))

typedef struct
{
    dm_prim_t               type;   /* DM_HCI_EXTENDED_INQUIRY_RESULT_IND */
    uint16_t                phandle;
    uint8_t                 num_responses;
    HCI_INQ_RESULT_WITH_RSSI_T  result;
    uint8_t                 *eir_data_part[HCI_EIR_DATA_PACKET_PTRS];
} DM_HCI_EXTENDED_INQUIRY_RESULT_IND_T;

#define DM_HCI_EXTENDED_INQUIRY_RESULT_IND \
    ((dm_prim_t)(ENUM_DM_HCI_EXTENDED_INQUIRY_RESULT_IND))

typedef DM_HCI_BD_ADDR_COMMAND_CFM_T DM_HCI_ENHANCED_FLUSH_CFM_T;

typedef HCI_ENHANCED_FLUSH_T DM_HCI_ENHANCED_FLUSH_REQ_T;
#define DM_HCI_ENHANCED_FLUSH_REQ  ((dm_prim_t)(ENUM_DM_HCI_ENHANCED_FLUSH_REQ))

#define DM_HCI_ENHANCED_FLUSH_CFM \
                            ((dm_prim_t)(ENUM_DM_HCI_ENHANCED_FLUSH_CFM))

/*----------------------------------------------------------------------------*
 * PURPOSE
 *      Event generated when the link supervision timeout for an ACL changes.
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    dm_prim_t               type;   /* DM_HCI_LINK_SUPERV_TIMEOUT_IND */
    uint16_t                phandle;
    BD_ADDR_T               bd_addr;    
    uint16_t                timeout;
} DM_HCI_LINK_SUPERV_TIMEOUT_IND_T;
#define DM_HCI_LINK_SUPERV_TIMEOUT_IND ((dm_prim_t)(ENUM_DM_HCI_LINK_SUPERV_TIMEOUT_IND))

typedef HCI_SET_AFH_CHANNEL_CLASS_T DM_HCI_SET_AFH_CHANNEL_CLASS_REQ_T;
#define DM_HCI_SET_AFH_CHANNEL_CLASS_REQ ((dm_prim_t)(ENUM_DM_HCI_SET_AFH_CHANNEL_CLASS_REQ))
#define DM_HCI_SET_AFH_CHANNEL_CLASS_CFM ((dm_prim_t)(ENUM_DM_HCI_SET_AFH_CHANNEL_CLASS_CFM))

typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_SET_AFH_CHANNEL_CLASS_CFM_T;

typedef HCI_READ_INQUIRY_SCAN_TYPE_T DM_HCI_READ_INQUIRY_SCAN_TYPE_REQ_T;
#define DM_HCI_READ_INQUIRY_SCAN_TYPE_REQ ((dm_prim_t)(ENUM_DM_HCI_READ_INQUIRY_SCAN_TYPE_REQ))
#define DM_HCI_READ_INQUIRY_SCAN_TYPE_CFM ((dm_prim_t)(ENUM_DM_HCI_READ_INQUIRY_SCAN_TYPE_CFM))

typedef struct
{
    dm_prim_t               type;           /* DM_HCI_READ_INQUIRY_SCAN_TYPE_CFM_T */
    phandle_t               phandle;        /* destination phandle */
    uint8_t                 status;         /* Success or failure - See Note1 */
    uint8_t                 mode ;
} DM_HCI_READ_INQUIRY_SCAN_TYPE_CFM_T;


typedef HCI_WRITE_INQUIRY_SCAN_TYPE_T DM_HCI_WRITE_INQUIRY_SCAN_TYPE_REQ_T;
#define DM_HCI_WRITE_INQUIRY_SCAN_TYPE_REQ ((dm_prim_t)(ENUM_DM_HCI_WRITE_INQUIRY_SCAN_TYPE_REQ))
#define DM_HCI_WRITE_INQUIRY_SCAN_TYPE_CFM ((dm_prim_t)(ENUM_DM_HCI_WRITE_INQUIRY_SCAN_TYPE_CFM))

typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_WRITE_INQUIRY_SCAN_TYPE_CFM_T;

typedef HCI_READ_INQUIRY_MODE_T DM_HCI_READ_INQUIRY_MODE_REQ_T;
#define DM_HCI_READ_INQUIRY_MODE_REQ ((dm_prim_t)(ENUM_DM_HCI_READ_INQUIRY_MODE_REQ))
#define DM_HCI_READ_INQUIRY_MODE_CFM ((dm_prim_t)(ENUM_DM_HCI_READ_INQUIRY_MODE_CFM))

typedef struct
{
    dm_prim_t               type;           /* DM_HCI_READ_INQUIRY_MODE_CFM_T */
    phandle_t               phandle;        /* destination phandle */
    uint8_t                 status;         /* Success or failure - See Note1 */
    uint8_t                 mode;
} DM_HCI_READ_INQUIRY_MODE_CFM_T;


typedef HCI_WRITE_INQUIRY_MODE_T DM_HCI_WRITE_INQUIRY_MODE_REQ_T;
#define DM_HCI_WRITE_INQUIRY_MODE_REQ ((dm_prim_t)(ENUM_DM_HCI_WRITE_INQUIRY_MODE_REQ))
#define DM_HCI_WRITE_INQUIRY_MODE_CFM ((dm_prim_t)(ENUM_DM_HCI_WRITE_INQUIRY_MODE_CFM))

typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_WRITE_INQUIRY_MODE_CFM_T;

typedef HCI_READ_PAGE_SCAN_TYPE_T DM_HCI_READ_PAGE_SCAN_TYPE_REQ_T;
#define DM_HCI_READ_PAGE_SCAN_TYPE_REQ ((dm_prim_t)(ENUM_DM_HCI_READ_PAGE_SCAN_TYPE_REQ))
#define DM_HCI_READ_PAGE_SCAN_TYPE_CFM ((dm_prim_t)(ENUM_DM_HCI_READ_PAGE_SCAN_TYPE_CFM))

typedef struct
{
    dm_prim_t               type;           /* DM_HCI_READ_PAGE_SCAN_TYPE_CFM_T */
    phandle_t               phandle;        /* destination phandle */
    uint8_t                 status;         /* Success or failure - See Note1 */
    uint8_t                 mode;
} DM_HCI_READ_PAGE_SCAN_TYPE_CFM_T;


typedef HCI_WRITE_PAGE_SCAN_TYPE_T DM_HCI_WRITE_PAGE_SCAN_TYPE_REQ_T;
#define DM_HCI_WRITE_PAGE_SCAN_TYPE_REQ ((dm_prim_t)(ENUM_DM_HCI_WRITE_PAGE_SCAN_TYPE_REQ))
#define DM_HCI_WRITE_PAGE_SCAN_TYPE_CFM ((dm_prim_t)(ENUM_DM_HCI_WRITE_PAGE_SCAN_TYPE_CFM))

typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_WRITE_PAGE_SCAN_TYPE_CFM_T;

typedef HCI_READ_AFH_CHANNEL_CLASS_M_T DM_HCI_READ_AFH_CHANNEL_CLASS_M_REQ_T;
#define DM_HCI_READ_AFH_CHANNEL_CLASS_M_REQ ((dm_prim_t)(ENUM_DM_HCI_READ_AFH_CHANNEL_CLASS_M_REQ))
#define DM_HCI_READ_AFH_CHANNEL_CLASS_M_CFM ((dm_prim_t)(ENUM_DM_HCI_READ_AFH_CHANNEL_CLASS_M_CFM))

typedef struct
{
    dm_prim_t               type;           /* DM_HCI_READ_AFH_CHANNEL_CLASS_M_CFM_T */
    phandle_t               phandle;        /* destination phandle */
    uint8_t                 status;         /* Success or failure - See Note1 */
    uint8_t                 class_mode;
} DM_HCI_READ_AFH_CHANNEL_CLASS_M_CFM_T;


typedef HCI_WRITE_AFH_CHANNEL_CLASS_M_T DM_HCI_WRITE_AFH_CHANNEL_CLASS_M_REQ_T;
#define DM_HCI_WRITE_AFH_CHANNEL_CLASS_M_REQ ((dm_prim_t)(ENUM_DM_HCI_WRITE_AFH_CHANNEL_CLASS_M_REQ))
#define DM_HCI_WRITE_AFH_CHANNEL_CLASS_M_CFM ((dm_prim_t)(ENUM_DM_HCI_WRITE_AFH_CHANNEL_CLASS_M_CFM))

typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_WRITE_AFH_CHANNEL_CLASS_M_CFM_T;

typedef HCI_READ_LOCAL_EXT_FEATURES_T DM_HCI_READ_LOCAL_EXT_FEATURES_REQ_T;
#define DM_HCI_READ_LOCAL_EXT_FEATURES_REQ ((dm_prim_t)(ENUM_DM_HCI_READ_LOCAL_EXT_FEATURES_REQ))
#define DM_HCI_READ_LOCAL_EXT_FEATURES_CFM ((dm_prim_t)(ENUM_DM_HCI_READ_LOCAL_EXT_FEATURES_CFM))

typedef struct
{
    dm_prim_t               type;           /* DM_HCI_READ_LOCAL_EXT_FEATURES_CFM_T */
    phandle_t               phandle;        /* destination phandle */
    uint8_t                 status;         /* Success or failure - See Note1 */
    uint8_t                 page_num;
    uint8_t                 max_page_num;
    uint8_t                 lmp_ext_features[8]; 
} DM_HCI_READ_LOCAL_EXT_FEATURES_CFM_T;

typedef HCI_READ_AFH_CHANNEL_MAP_T DM_HCI_READ_AFH_CHANNEL_MAP_REQ_T;
#define DM_HCI_READ_AFH_CHANNEL_MAP_REQ ((dm_prim_t)(ENUM_DM_HCI_READ_AFH_CHANNEL_MAP_REQ))
#define DM_HCI_READ_AFH_CHANNEL_MAP_CFM ((dm_prim_t)(ENUM_DM_HCI_READ_AFH_CHANNEL_MAP_CFM))

typedef struct
{
    dm_prim_t               type;           /* DM_HCI_READ_AFH_CHANNEL_MAP_CFM_T */
    phandle_t               phandle;        /* destination phandle */
    uint8_t                 status;         /* Success or failure - See Note1 */
    BD_ADDR_T               bd_addr ;
    uint8_t                 mode;
    uint8_t                 map[10];
} DM_HCI_READ_AFH_CHANNEL_MAP_CFM_T;


typedef HCI_READ_CLOCK_T DM_HCI_READ_CLOCK_REQ_T;
#define DM_HCI_READ_CLOCK_REQ ((dm_prim_t)(ENUM_DM_HCI_READ_CLOCK_REQ))
#define DM_HCI_READ_CLOCK_CFM ((dm_prim_t)(ENUM_DM_HCI_READ_CLOCK_CFM))

typedef struct
{
    dm_prim_t               type;           /* DM_HCI_READ_CLOCK_CFM_T */
    phandle_t               phandle;        /* destination phandle */
    uint8_t                 status;         /* Success or failure - See Note1 */
    BD_ADDR_T               bd_addr ;
    uint32_t                clock;
    uint16_t                accuracy;
} DM_HCI_READ_CLOCK_CFM_T;

#define DM_HCI_READ_INQUIRY_RESPONSE_TX_POWER_LEVEL_REQ \
    ((dm_prim_t)(ENUM_DM_HCI_READ_INQUIRY_RESPONSE_TX_POWER_LEVEL_REQ))
#define DM_HCI_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL_REQ \
    ((dm_prim_t)(ENUM_DM_HCI_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL_REQ))
#define DM_HCI_READ_INQUIRY_RESPONSE_TX_POWER_LEVEL_CFM \
    ((dm_prim_t)(ENUM_DM_HCI_READ_INQUIRY_RESPONSE_TX_POWER_LEVEL_CFM))
#define DM_HCI_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL_CFM \
    ((dm_prim_t)(ENUM_DM_HCI_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL_CFM))

typedef HCI_READ_INQUIRY_RESPONSE_TX_POWER_LEVEL_T DM_HCI_READ_INQUIRY_RESPONSE_TX_POWER_LEVEL_REQ_T;
typedef HCI_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL_T DM_HCI_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL_REQ_T;

typedef struct
{
    /* Always DM_HCI_READ_INQUIRY_RESPONSE_TX_POWER_LEVEL_CFM */
    dm_prim_t               type;
    phandle_t               phandle;        /* destination phandle */
    uint8_t                 status;
    int8_t                 tx_power;
} DM_HCI_READ_INQUIRY_RESPONSE_TX_POWER_LEVEL_CFM_T;

typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL_CFM_T;

#define DM_HCI_REM_HOST_SUPPORTED_FEATURES_IND \
    ((dm_prim_t)(ENUM_DM_HCI_REM_HOST_SUPPORTED_FEATURES_IND))
typedef struct
{
    /* Always DM_HCI_REM_HOST_SUPPORTED_FEATURES_IND */
    dm_prim_t               type;
    phandle_t               phandle;        /* destination phandle */
    BD_ADDR_T               bd_addr;        /* ODD event */
    uint16_t                host_features[4];
} DM_HCI_REM_HOST_SUPPORTED_FEATURES_IND_T;

/*! \brief Read Encryption Key Size */
#define DM_HCI_READ_ENCRYPTION_KEY_SIZE_REQ \
    ((dm_prim_t)(ENUM_DM_HCI_READ_ENCRYPTION_KEY_SIZE_REQ))
#define DM_HCI_READ_ENCRYPTION_KEY_SIZE_CFM \
    ((dm_prim_t)(ENUM_DM_HCI_READ_ENCRYPTION_KEY_SIZE_CFM))
typedef HCI_READ_ENCRYPTION_KEY_SIZE_T DM_HCI_READ_ENCRYPTION_KEY_SIZE_REQ_T;

typedef struct
{
    dm_prim_t               type;
    phandle_t               phandle;
    uint8_t                 status;
    BD_ADDR_T               bd_addr;
    uint8_t                 key_size;
} DM_HCI_READ_ENCRYPTION_KEY_SIZE_CFM_T;


/*-------------------------------------------------------------
  Downstream ULP primitives
 ---------------------------------------------------------------*/
typedef HCI_ULP_SET_EVENT_MASK_T DM_HCI_ULP_SET_EVENT_MASK_REQ_T;
typedef HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_T DM_HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_REQ_T;
typedef HCI_ULP_SET_RANDOM_ADDRESS_T DM_HCI_ULP_SET_RANDOM_ADDRESS_REQ_T;
typedef HCI_ULP_SET_ADVERTISING_PARAMETERS_T DM_HCI_ULP_SET_ADVERTISING_PARAMETERS_REQ_T;
typedef HCI_ULP_READ_ADVERTISING_CHANNEL_TX_POWER_T DM_HCI_ULP_READ_ADVERTISING_CHANNEL_TX_POWER_REQ_T;
typedef HCI_ULP_SET_ADVERTISING_DATA_T DM_HCI_ULP_SET_ADVERTISING_DATA_REQ_T;
typedef HCI_ULP_SET_SCAN_RESPONSE_DATA_T DM_HCI_ULP_SET_SCAN_RESPONSE_DATA_REQ_T;
typedef HCI_ULP_SET_ADVERTISE_ENABLE_T DM_HCI_ULP_SET_ADVERTISE_ENABLE_REQ_T;
typedef HCI_ULP_SET_SCAN_PARAMETERS_T DM_HCI_ULP_SET_SCAN_PARAMETERS_REQ_T;
typedef HCI_ULP_SET_SCAN_ENABLE_T DM_HCI_ULP_SET_SCAN_ENABLE_REQ_T;
typedef HCI_ULP_CREATE_CONNECTION_CANCEL_T DM_HCI_ULP_CREATE_CONNECTION_CANCEL_REQ_T;
typedef HCI_ULP_READ_WHITE_LIST_SIZE_T DM_HCI_ULP_READ_WHITE_LIST_SIZE_REQ_T;
typedef HCI_ULP_CLEAR_WHITE_LIST_T DM_HCI_ULP_CLEAR_WHITE_LIST_REQ_T;
typedef HCI_ULP_ADD_DEVICE_TO_WHITE_LIST_T DM_HCI_ULP_ADD_DEVICE_TO_WHITE_LIST_REQ_T;
typedef HCI_ULP_REMOVE_DEVICE_FROM_WHITE_LIST_T DM_HCI_ULP_REMOVE_DEVICE_FROM_WHITE_LIST_REQ_T;
typedef HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_T DM_HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_REQ_T;
typedef HCI_ULP_READ_CHANNEL_MAP_T DM_HCI_ULP_READ_CHANNEL_MAP_REQ_T;
typedef HCI_ULP_READ_REMOTE_USED_FEATURES_T DM_HCI_ULP_READ_REMOTE_USED_FEATURES_REQ_T;
typedef HCI_ULP_ENCRYPT_T DM_HCI_ULP_ENCRYPT_REQ_T;
typedef HCI_ULP_RAND_T DM_HCI_ULP_RAND_REQ_T;
typedef HCI_ULP_READ_SUPPORTED_STATES_T DM_HCI_ULP_READ_SUPPORTED_STATES_REQ_T;
typedef HCI_ULP_RECEIVER_TEST_T DM_HCI_ULP_RECEIVER_TEST_REQ_T;
typedef HCI_ULP_TRANSMITTER_TEST_T DM_HCI_ULP_TRANSMITTER_TEST_REQ_T;
typedef HCI_ULP_TEST_END_T DM_HCI_ULP_TEST_END_REQ_T;

/*-------------------------------------------------------------
  Upstream ULP primitives
 ---------------------------------------------------------------*/
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_ULP_SET_EVENT_MASK_CFM_T;
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_ULP_SET_RANDOM_ADDRESS_CFM_T;
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_ULP_SET_ADVERTISING_PARAMETERS_CFM_T;
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_ULP_SET_ADVERTISING_DATA_CFM_T;
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_ULP_SET_SCAN_RESPONSE_DATA_CFM_T;
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_ULP_SET_ADVERTISE_ENABLE_CFM_T;
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_ULP_SET_SCAN_PARAMETERS_CFM_T;
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_ULP_SET_SCAN_ENABLE_CFM_T;
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_ULP_CREATE_CONNECTION_CANCEL_CFM_T;
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_ULP_CLEAR_WHITE_LIST_CFM_T;
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_ULP_ADD_DEVICE_TO_WHITE_LIST_CFM_T;
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_ULP_REMOVE_DEVICE_FROM_WHITE_LIST_CFM_T;
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_CFM_T;
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_ULP_RECEIVER_TEST_CFM_T;
typedef DM_HCI_STANDARD_COMMAND_CFM_T DM_HCI_ULP_TRANSMITTER_TEST_CFM_T;

typedef struct
{
    dm_prim_t           type;               /* DM_HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_CFM */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* Success or failure - See Note1 */
    uint8_t             feature_set[8];
} DM_HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_CFM_T;

typedef struct
{
    dm_prim_t           type;
    phandle_t           phandle;
    uint8_t             status;
    int8_t              tx_power;
} DM_HCI_ULP_READ_ADVERTISING_CHANNEL_TX_POWER_CFM_T;

typedef struct
{
    dm_prim_t           type;
    phandle_t           phandle;
    uint8_t             status;
    uint8_t             white_list_size;
} DM_HCI_ULP_READ_WHITE_LIST_SIZE_CFM_T;

typedef struct
{
    dm_prim_t           type;
    phandle_t           phandle;
    uint8_t             status;
    TYPED_BD_ADDR_T     addrt;
    uint8_t             ulp_channel_map[5];
} DM_HCI_ULP_READ_CHANNEL_MAP_CFM_T;
    
typedef struct
{
    dm_prim_t           type;               /* DM_HCI_ULP_READ_REMOTE_USED_FEATURES_CFM */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;             /* Success or failure - See Note1 */
    TYPED_BD_ADDR_T     addrt;              /* Bluetooth address */
    uint8_t             feature_set[8];
} DM_HCI_ULP_READ_REMOTE_USED_FEATURES_CFM_T;

typedef struct
{
    dm_prim_t           type;
    phandle_t           phandle;
    uint8_t             status;
    uint8_t             encrypted_data[16];
} DM_HCI_ULP_ENCRYPT_CFM_T;

typedef struct
{
    dm_prim_t           type;
    phandle_t           phandle;
    uint8_t             status;
    uint8_t             random_number[8];
} DM_HCI_ULP_RAND_CFM_T;

typedef struct
{
    dm_prim_t           type;
    phandle_t           phandle;
    uint8_t             status;
    uint8_t             supported_states[8];
} DM_HCI_ULP_READ_SUPPORTED_STATES_CFM_T;

typedef struct
{
    dm_prim_t           type;
    phandle_t           phandle;
    uint8_t             status;
    uint16_t            number_of_packets;
} DM_HCI_ULP_TEST_END_CFM_T;

typedef struct
{
    dm_prim_t           type;               /* DM_HCI_ULP_ADVERTISING_REPORT_IND */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             num_reports;
    uint8_t             event_type;
    TYPED_BD_ADDR_T     current_addrt;
    TYPED_BD_ADDR_T     permanent_addrt;
    uint8_t             length_data;
    uint8_t             *data;
    int8_t              rssi;
} DM_HCI_ULP_ADVERTISING_REPORT_IND_T;


/*---------------------------------------------------------------------------*
 *  PURPOSE
 *  Provides an API to allow an application to update change the Link Layer connection
 *  parameters of a connection
 *
 *  There could be potential race between DM_BLE_UPDATE_CONNECTION_PARAMETERS_CFM
 *  and DM_HCI_ULP_CONNECTION_UPDATE_COMPLETE_IND. The application should in anycase
 *  wait for DM_HCI_ULP_CONNECTION_UPDATE_COMPLETE_IND.
 *
 *  RETURNS
 *     A CFM
 *---------------------------------------------------------------------------*/

typedef HCI_ULP_CONNECTION_UPDATE_T DM_BLE_UPDATE_CONNECTION_PARAMETERS_REQ_T;

typedef struct
{
    dm_prim_t           type;   /* DM_BLE_UPDATE_CONNECTION_PARAMETERS_CFM */
    phandle_t           phandle;/* destination phandle */
    uint8_t             status;
    TYPED_BD_ADDR_T     addrt;
} DM_BLE_UPDATE_CONNECTION_PARAMETERS_CFM_T;

typedef struct
{
    dm_prim_t           type;               /* DM_HCI_ULP_CONNECTION_UPDATE_COMPLETE_IND */
    phandle_t           phandle;            /* destination phandle */
    uint8_t             status;
    TYPED_BD_ADDR_T     addrt;
    uint16_t            conn_interval;
    uint16_t            conn_latency;
    uint16_t            supervision_timeout;
} DM_HCI_ULP_CONNECTION_UPDATE_COMPLETE_IND_T;

/*------------------------------------------------------------------------
 *
 *      UNION OF       PRIMITIVES
 *
 *-----------------------------------------------------------------------*/
typedef union
{
    dm_prim_t                                               type;
    DM_COMMON_T                                             dm_common;
    DM_COMMON_RSP_T                                         dm_common_rsp;
    HCI_COMMAND_COMMON_T                                    dm_hci_common;
    HCI_WITH_CH_COMMON_T                                    dm_hci_with_handle;
    HCI_ULP_WITH_CH_COMMON_T                                dm_hci_ulp_with_handle;
    DM_UPSTREAM_COMMON_T                                    dm_upstream_common;
    DM_UPSTREAM_BD_ADDR_COMMON_T                            dm_upstream_bd_addr_common;
    DM_UPSTREAM_TYPED_BD_ADDR_COMMON_T                      dm_upstream_typed_bd_addr_common;
    DM_HCI_STANDARD_COMMAND_CFM_T                           dm_standard_command_cfm;
    DM_HCI_BD_ADDR_COMMAND_CFM_T                            dm_bd_addr_command_cfm;
    DM_HCI_TYPED_BD_ADDR_COMMAND_CFM_T                      dm_typed_bd_addr_command_cfm;
    DM_SYNC_UPSTREAM_WITH_HANDLE_COMMON_T                   dm_sync_upstream_with_handle_common;
    DM_SYNC_DOWNSTREAM_WITH_HANDLE_COMMON_T                 dm_sync_downstream_with_handle_common;

    DM_ACL_CLOSED_IND_T                                     dm_acl_closed_ind;
    DM_ACL_CLOSE_REQ_T                                      dm_acl_close_req;
    DM_ACL_OPEN_CFM_T                                       dm_acl_open_cfm;
    DM_ACL_OPENED_IND_T                                     dm_acl_opened_ind;
    DM_ACL_OPEN_REQ_T                                       dm_acl_open_req;

    DM_AM_REGISTER_CFM_T                                    dm_am_register_cfm;
    DM_AM_REGISTER_REQ_T                                    dm_am_register_req;
    DM_AM_REGISTER_WRONG_API_CFM_T                          dm_am_register_wrong_api_cfm;
    DM_AM_REGISTER_WRONG_API_REQ_T                          dm_am_register_wrong_api_req;

    DM_AMPM_REGISTER_REQ_T                                  dm_ampm_register_req;
    DM_AMPM_CONNECT_RSP_T                                   dm_ampm_connect_rsp;
    DM_AMPM_CONNECT_CHANNEL_RSP_T                           dm_ampm_connect_channel_rsp;
    DM_AMPM_DISCONNECT_CHANNEL_RSP_T                        dm_ampm_disconnect_channel_rsp;
    DM_AMPM_DISCONNECT_REQ_T                                dm_ampm_disconnect_req;
    DM_AMPM_READ_BD_ADDR_REQ_T                              dm_ampm_read_bd_addr_req;
    DM_AMPM_VERIFY_PHYSICAL_LINK_RSP_T                      dm_ampm_verify_physical_link_rsp;
    DM_AMPM_READ_DATA_BLOCK_SIZE_RSP_T                      dm_ampm_read_data_block_size_rsp;
    DM_AMPM_NUMBER_COMPLETED_DATA_BLOCKS_REQ_T              dm_ampm_number_completed_data_blocks_req;

    DM_AMPM_REGISTER_CFM_T                                  dm_ampm_register_cfm;
    DM_AMPM_CONNECT_IND_T                                   dm_ampm_connect_ind;
    DM_AMPM_CONNECT_CHANNEL_IND_T                           dm_ampm_connect_channel_ind;
    DM_AMPM_DISCONNECT_CHANNEL_IND_T                        dm_ampm_disconnect_channel_ind;
    DM_AMPM_LINK_KEY_NOTIFICATION_IND_T                     dm_ampm_link_key_notification_ind;
    DM_AMPM_READ_BD_ADDR_CFM_T                              dm_ampm_read_bd_addr_cfm;
    DM_AMPM_VERIFY_PHYSICAL_LINK_IND_T                      dm_ampm_verify_physical_link_ind;
    DM_AMPM_READ_DATA_BLOCK_SIZE_IND_T                      dm_ampm_read_data_block_size_ind;
    DM_AMPM_SIMPLE_PAIRING_ENABLED_IND_T                    dm_ampm_simple_pairing_enabled_ind;
    DM_AMPM_REMOTE_FEATURES_IND_T                           dm_ampm_remote_features_ind;

    DM_SET_BLE_CONNECTION_PARAMETERS_REQ_T                  dm_set_ble_connection_parameters_req;
    DM_SET_BLE_CONNECTION_PARAMETERS_CFM_T                  dm_set_ble_connection_parameters_cfm;

    DM_BLE_UPDATE_CONNECTION_PARAMETERS_REQ_T               dm_ble_update_connection_parameters_req;
    DM_BLE_UPDATE_CONNECTION_PARAMETERS_CFM_T               dm_ble_update_connection_parameters_cfm;    

    DM_BAD_MESSAGE_IND_T                                    dm_bad_message_ind;
    DM_BLUESTACK_STATE_TRANSITION_ENTRY_IND_T               dm_bluestack_state_transition_entry_ind;
    DM_BLUESTACK_STATE_TRANSITION_EXIT_IND_T                dm_bluestack_state_transition_exit_ind;

    DM_CLEAR_PARAM_CACHE_CFM_T                              dm_clear_param_cache_cfm;
    DM_CLEAR_PARAM_CACHE_REQ_T                              dm_clear_param_cache_req;

    DM_DATA_FROM_HCI_REQ_T                                  dm_data_from_hci_req;
    DM_DATA_TO_HCI_IND_T                                    dm_data_to_hci_ind;

    DM_HCI_CHANGE_CONN_LINK_KEY_REQ_T                       dm_change_link_key_req;
    DM_HCI_CHANGE_LOCAL_NAME_CFM_T                          dm_change_local_name_cfm;
    DM_HCI_CHANGE_LOCAL_NAME_REQ_T                          dm_change_local_name_req;
    DM_HCI_CHANGE_CONN_PKT_TYPE_REQ_T                       dm_change_packet_type_req;
    DM_HCI_CONN_PACKET_TYPE_CHANGED_IND_T                   dm_conn_packet_type_changed_ind;
    DM_HCI_CREATE_CONNECTION_CANCEL_CFM_T                   dm_create_connection_cancel_cfm;
    DM_HCI_CREATE_CONNECTION_CANCEL_REQ_T                   dm_create_connection_cancel_req;
    DM_HCI_CREATE_NEW_UNIT_KEY_CFM_T                        dm_create_new_unit_key_cfm;
    DM_HCI_CREATE_NEW_UNIT_KEY_REQ_T                        dm_create_new_unit_key_req;
    DM_HCI_DELETE_STORED_LINK_KEY_CFM_T                     dm_delete_stored_link_key_cfm;
    DM_HCI_DELETE_STORED_LINK_KEY_REQ_T                     dm_delete_stored_link_key_req;
    DM_HCI_ENABLE_DUT_MODE_CFM_T                            dm_enable_device_ut_mode_cfm;
    DM_HCI_ENABLE_DUT_MODE_REQ_T                            dm_enable_device_ut_mode_req;
    DM_HCI_REFRESH_ENCRYPTION_KEY_IND_T                     dm_encryption_key_refresh_ind;
    DM_HCI_ENHANCED_FLUSH_CFM_T                             dm_enhanced_flush_cfm;
    DM_HCI_ENHANCED_FLUSH_REQ_T                             dm_enhanced_flush_req;
    DM_HCI_EXIT_PARK_MODE_REQ_T                             dm_exit_park_mode_req;
    DM_HCI_EXIT_PERIODIC_INQUIRY_MODE_CFM_T                 dm_exit_periodic_inquiry_cfm;
    DM_HCI_EXIT_PERIODIC_INQUIRY_MODE_REQ_T                 dm_exit_periodic_inquiry_req;
    DM_HCI_EXIT_SNIFF_MODE_REQ_T                            dm_exit_sniff_mode_req;
    DM_HCI_EXTENDED_INQUIRY_RESULT_IND_T                    dm_extended_inquiry_result_ind;
    DM_HCI_READ_FAILED_CONTACT_COUNT_CFM_T                  dm_failed_contact_counter_cfm;
    DM_HCI_READ_FAILED_CONTACT_COUNT_REQ_T                  dm_failed_contact_counter_req;
    DM_HCI_FLOW_SPEC_CFM_T                                  dm_flow_spec_cfm;
    DM_HCI_FLOW_SPEC_REQ_T                                  dm_flow_spec_req;
    DM_HCI_FLUSH_CFM_T                                      dm_flush_cfm;
    DM_HCI_FLUSH_REQ_T                                      dm_flush_req;
    DM_HCI_GET_LINK_QUALITY_CFM_T                           dm_get_link_quality_cfm;
    DM_HCI_GET_LINK_QUALITY_REQ_T                           dm_get_link_quality_req;
    DM_HCI_HOLD_MODE_REQ_T                                  dm_hold_mode_req;
    DM_HCI_HOST_NUM_COMPLETED_PACKETS_CFM_T                 dm_host_num_completed_pkts_cfm;
    DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ_T                 dm_host_num_completed_packets_req;
    DM_HCI_INQUIRY_CANCEL_CFM_T                             dm_inquiry_cancel_cfm;
    DM_HCI_INQUIRY_CANCEL_REQ_T                             dm_inquiry_cancel_req;
    DM_HCI_INQUIRY_CFM_T                                    dm_inquiry_cfm;
    DM_HCI_INQUIRY_RESULT_IND_T                             dm_inquiry_result_ind;
    DM_HCI_INQUIRY_RESULT_WITH_RSSI_IND_T                   dm_inquiry_result_with_rssi_ind;
    DM_HCI_INQUIRY_REQ_T                                    dm_inquiry_req;
    DM_HCI_CHANGE_CONN_LINK_KEY_CFM_T                       dm_link_key_change_cfm;
    DM_HCI_LINK_SUPERV_TIMEOUT_IND_T                        dm_link_superv_timeout_ind;
    DM_HCI_MASTER_LINK_KEY_CFM_T                            dm_master_link_key_cfm;
    DM_HCI_MASTER_LINK_KEY_REQ_T                            dm_master_link_key_req;
    DM_HCI_MODE_CHANGE_EVENT_IND_T                          dm_mode_change_event_ind;
    DM_HCI_PARK_MODE_REQ_T                                  dm_park_mode_req;
    DM_HCI_PERIODIC_INQUIRY_MODE_CFM_T                      dm_periodic_inquiry_cfm;
    DM_HCI_PERIODIC_INQUIRY_MODE_REQ_T                      dm_periodic_inquiry_req;
    DM_HCI_QOS_SETUP_CFM_T                                  dm_qos_setup_cfm;
    DM_HCI_QOS_SETUP_REQ_T                                  dm_qos_setup_req;
    DM_HCI_READ_AFH_CHANNEL_CLASS_M_CFM_T                   dm_read_afh_channel_class_m_cfm;
    DM_HCI_READ_AFH_CHANNEL_CLASS_M_REQ_T                   dm_read_afh_channel_class_m_req;
    DM_HCI_READ_AFH_CHANNEL_MAP_CFM_T                       dm_read_afh_channel_map_cfm;
    DM_HCI_READ_AFH_CHANNEL_MAP_REQ_T                       dm_read_afh_channel_map_req;
    DM_HCI_READ_AUTH_ENABLE_CFM_T                           dm_read_auth_enable_cfm;
    DM_HCI_READ_AUTH_ENABLE_REQ_T                           dm_read_auth_enable_req;
    DM_HCI_READ_AUTO_FLUSH_TIMEOUT_CFM_T                    dm_read_auto_flush_timeout_cfm;
    DM_HCI_READ_AUTO_FLUSH_TIMEOUT_REQ_T                    dm_read_auto_flush_timeout_req;
    DM_HCI_READ_BD_ADDR_CFM_T                               dm_read_bd_addr_cfm;
    DM_HCI_READ_BD_ADDR_REQ_T                               dm_read_bd_addr_req;
    DM_HCI_READ_CLASS_OF_DEVICE_CFM_T                       dm_read_class_of_device_cfm;
    DM_HCI_READ_CLASS_OF_DEVICE_REQ_T                       dm_read_class_of_device_req;
    DM_HCI_READ_CLOCK_CFM_T                                 dm_read_clock_cfm;
    DM_HCI_READ_CLOCK_OFFSET_CFM_T                          dm_read_clock_offset_cfm;
    DM_HCI_READ_CLOCK_OFFSET_REQ_T                          dm_read_clock_offset_req;
    DM_HCI_READ_CLOCK_REQ_T                                 dm_read_clock_req;
    DM_HCI_READ_CONN_ACCEPT_TIMEOUT_CFM_T                   dm_read_conn_accept_to_cfm;
    DM_HCI_READ_CONN_ACCEPT_TIMEOUT_REQ_T                   dm_read_conn_accept_to_req;
    DM_HCI_READ_COUNTRY_CODE_CFM_T                          dm_read_country_code_cfm;
    DM_HCI_READ_COUNTRY_CODE_REQ_T                          dm_read_country_code_req;
    DM_HCI_READ_CURRENT_IAC_LAP_CFM_T                       dm_read_current_iac_lap_cfm;
    DM_HCI_READ_CURRENT_IAC_LAP_REQ_T                       dm_read_current_iac_lap_req;
    DM_HCI_READ_DEFAULT_LINK_POLICY_SETTINGS_CFM_T          dm_read_default_link_policy_settings_cfm;
    DM_HCI_READ_DEFAULT_LINK_POLICY_SETTINGS_REQ_T          dm_read_default_link_policy_settings_req;
    DM_HCI_READ_ENC_MODE_CFM_T                              dm_read_encryption_mode_cfm;
    DM_HCI_READ_ENC_MODE_REQ_T                              dm_read_encryption_mode_req;
    DM_HCI_READ_ENCRYPTION_KEY_SIZE_CFM_T                   dm_read_encryption_key_size_cfm;
    DM_HCI_READ_ENCRYPTION_KEY_SIZE_REQ_T                   dm_read_encryption_key_size_req;
    DM_HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_CFM_T        dm_read_extended_inquiry_response_data_cfm;
    DM_HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_REQ_T        dm_read_extended_inquiry_response_data_req;
    DM_HCI_READ_HOLD_MODE_ACTIVITY_CFM_T                    dm_read_hold_mode_activity_cfm;
    DM_HCI_READ_HOLD_MODE_ACTIVITY_REQ_T                    dm_read_hold_mode_activity_req;
    DM_HCI_READ_INQUIRY_MODE_CFM_T                          dm_read_inquiry_mode_cfm;
    DM_HCI_READ_INQUIRY_MODE_REQ_T                          dm_read_inquiry_mode_req;
    DM_HCI_READ_INQUIRY_RESPONSE_TX_POWER_LEVEL_CFM_T       dm_read_inquiry_response_tx_power_level_cfm;
    DM_HCI_READ_INQUIRY_RESPONSE_TX_POWER_LEVEL_REQ_T       dm_read_inquiry_response_tx_power_level_req;
    DM_HCI_READ_INQUIRYSCAN_ACTIVITY_CFM_T                  dm_read_inquiryscan_activity_cfm;
    DM_HCI_READ_INQUIRYSCAN_ACTIVITY_REQ_T                  dm_read_inquiryscan_activity_req;
    DM_HCI_READ_INQUIRY_SCAN_TYPE_CFM_T                     dm_read_inquiry_scan_type_cfm;
    DM_HCI_READ_INQUIRY_SCAN_TYPE_REQ_T                     dm_read_inquiry_scan_type_req;
    DM_HCI_READ_LINK_SUPERV_TIMEOUT_CFM_T                   dm_read_link_superv_timeout_cfm;
    DM_HCI_READ_LINK_SUPERV_TIMEOUT_REQ_T                   dm_read_link_superv_timeout_req;
    DM_HCI_READ_LMP_HANDLE_CFM_T                            dm_read_lmp_handle_cfm;
    DM_HCI_READ_LMP_HANDLE_REQ_T                            dm_read_lmp_handle_req;
    DM_HCI_READ_LOCAL_EXT_FEATURES_CFM_T                    dm_read_local_ext_features_cfm;
    DM_HCI_READ_LOCAL_EXT_FEATURES_REQ_T                    dm_read_local_ext_features_req;
    DM_HCI_READ_LOCAL_SUPP_FEATURES_CFM_T                   dm_read_local_features_cfm;
    DM_HCI_READ_LOCAL_SUPP_FEATURES_REQ_T                   dm_read_local_features_req;
    DM_HCI_READ_LOCAL_NAME_CFM_T                            dm_read_local_name_cfm;
    DM_HCI_READ_LOCAL_NAME_REQ_T                            dm_read_local_name_req;
    DM_HCI_READ_LOCAL_VER_INFO_CFM_T                        dm_read_local_version_cfm;
    DM_HCI_READ_LOCAL_VER_INFO_REQ_T                        dm_read_local_version_req;
    DM_HCI_READ_LINK_POLICY_SETTINGS_CFM_T                  dm_read_lp_settings_cfm;
    DM_HCI_READ_LINK_POLICY_SETTINGS_REQ_T                  dm_read_lp_settings_req;
    DM_HCI_READ_NUM_BCAST_RETXS_CFM_T                       dm_read_num_bcast_txs_cfm;
    DM_HCI_READ_NUM_BCAST_RETXS_REQ_T                       dm_read_num_bcast_txs_req;
    DM_HCI_READ_NUM_SUPPORTED_IAC_CFM_T                     dm_read_num_supported_iac_cfm;
    DM_HCI_READ_NUM_SUPPORTED_IAC_REQ_T                     dm_read_num_supported_iac_req;
    DM_HCI_READ_PAGESCAN_ACTIVITY_CFM_T                     dm_read_pagescan_activity_cfm;
    DM_HCI_READ_PAGESCAN_ACTIVITY_REQ_T                     dm_read_pagescan_activity_req;
    DM_HCI_READ_PAGESCAN_MODE_CFM_T                         dm_read_pagescan_mode_cfm;
    DM_HCI_READ_PAGESCAN_MODE_REQ_T                         dm_read_pagescan_mode_req;
    DM_HCI_READ_PAGESCAN_PERIOD_MODE_CFM_T                  dm_read_pagescan_period_mode_cfm;
    DM_HCI_READ_PAGESCAN_PERIOD_MODE_REQ_T                  dm_read_pagescan_period_mode_req;
    DM_HCI_READ_PAGE_SCAN_TYPE_CFM_T                        dm_read_page_scan_type_cfm;
    DM_HCI_READ_PAGE_SCAN_TYPE_REQ_T                        dm_read_page_scan_type_req;
    DM_HCI_READ_PAGE_TIMEOUT_CFM_T                          dm_read_page_to_cfm;
    DM_HCI_READ_PAGE_TIMEOUT_REQ_T                          dm_read_page_to_req;
    DM_HCI_READ_PIN_TYPE_CFM_T                              dm_read_pin_type_cfm;
    DM_HCI_READ_PIN_TYPE_REQ_T                              dm_read_pin_type_req;
    DM_HCI_READ_REMOTE_EXT_FEATURES_CFM_T                   dm_read_remote_ext_features_cfm;
    DM_HCI_READ_REMOTE_EXT_FEATURES_REQ_T                   dm_read_remote_ext_features_req;
    DM_HCI_READ_REMOTE_SUPP_FEATURES_CFM_T                  dm_read_remote_features_cfm;
    DM_HCI_READ_REMOTE_SUPP_FEATURES_REQ_T                  dm_read_remote_features_req;
    DM_HCI_READ_REMOTE_VER_INFO_CFM_T                       dm_read_remote_version_cfm;
    DM_HCI_READ_REMOTE_VER_INFO_REQ_T                       dm_read_remote_version_req;
    DM_HCI_READ_RSSI_CFM_T                                  dm_read_rssi_cfm;
    DM_HCI_READ_RSSI_REQ_T                                  dm_read_rssi_req;
    DM_HCI_READ_SCAN_ENABLE_CFM_T                           dm_read_scan_enable_cfm;
    DM_HCI_READ_SCAN_ENABLE_REQ_T                           dm_read_scan_enable_req;
    DM_HCI_READ_SCO_FLOW_CON_ENABLE_CFM_T                   dm_read_sco_flow_control_enable_cfm;
    DM_HCI_READ_SCO_FLOW_CON_ENABLE_REQ_T                   dm_read_sco_flow_control_enable_req;
    DM_HCI_READ_STORED_LINK_KEY_CFM_T                       dm_read_stored_link_key_cfm;
    DM_HCI_READ_STORED_LINK_KEY_REQ_T                       dm_read_stored_link_key_req;
    DM_HCI_READ_TX_POWER_LEVEL_CFM_T                        dm_read_tx_power_level_cfm;
    DM_HCI_READ_TX_POWER_LEVEL_REQ_T                        dm_read_tx_power_level_req;
    DM_HCI_READ_VOICE_SETTING_CFM_T                         dm_read_voice_setting_cfm;
    DM_HCI_READ_VOICE_SETTING_REQ_T                         dm_read_voice_setting_req;
    DM_HCI_REFRESH_ENCRYPTION_KEY_REQ_T                     dm_refresh_encryption_key_req;
    DM_HCI_REM_HOST_SUPPORTED_FEATURES_IND_T                dm_rem_host_supported_features_ind;
    DM_HCI_REMOTE_NAME_CFM_T                                dm_remote_name_cfm;
    DM_HCI_REMOTE_NAME_REQ_CANCEL_CFM_T                     dm_remote_name_req_cancel_cfm;
    DM_HCI_REMOTE_NAME_REQ_CANCEL_REQ_T                     dm_remote_name_req_cancel_req;
    DM_HCI_REMOTE_NAME_REQUEST_REQ_T                        dm_remote_name_request_req;
    DM_HCI_RESET_CFM_T                                      dm_reset_cfm;
    DM_HCI_RESET_FAILED_CONTACT_COUNT_CFM_T                 dm_reset_contact_counter_cfm;
    DM_HCI_RESET_FAILED_CONTACT_COUNT_REQ_T                 dm_reset_contact_counter_req;
    DM_HCI_RESET_REQ_T                                      dm_reset_req;
    DM_HCI_RETURN_LINK_KEYS_IND_T                           dm_return_link_keys_ind;
    DM_HCI_ROLE_DISCOVERY_CFM_T                             dm_role_discovery_cfm;
    DM_HCI_ROLE_DISCOVERY_REQ_T                             dm_role_discovery_req;
    DM_HCI_SET_AFH_CHANNEL_CLASS_CFM_T                      dm_set_afh_channel_class_cfm;
    DM_HCI_SET_AFH_CHANNEL_CLASS_REQ_T                      dm_set_afh_channel_class_req;
    DM_HCI_SET_EVENT_FILTER_CFM_T                           dm_set_event_filter_cfm;
    DM_HCI_SET_EVENT_FILTER_REQ_T                           dm_set_event_filter_req;
    DM_HCI_SET_EVENT_MASK_CFM_T                             dm_set_event_mask_cfm;
    DM_HCI_SET_EVENT_MASK_REQ_T                             dm_set_event_mask_req;
    DM_HCI_SNIFF_MODE_REQ_T                                 dm_sniff_mode_req;
    DM_HCI_SNIFF_SUB_RATE_CFM_T                             dm_sniff_sub_rate_cfm;
    DM_HCI_SNIFF_SUB_RATE_REQ_T                             dm_sniff_sub_rate_req;
    DM_HCI_SNIFF_SUB_RATING_IND_T                           dm_sniff_sub_rating_ind;
    DM_HCI_SWITCH_ROLE_CFM_T                                dm_switch_role_cfm;
    DM_HCI_SWITCH_ROLE_REQ_T                                dm_switch_role_req;

#ifdef INSTALL_ULP
    DM_HCI_ULP_ADVERTISING_REPORT_IND_T                     dm_ulp_advertising_report_ind;
    DM_HCI_ULP_SET_EVENT_MASK_REQ_T                         dm_ulp_set_event_mask_req;
    DM_HCI_ULP_SET_EVENT_MASK_CFM_T                         dm_ulp_set_event_mask_cfm;
    DM_HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_REQ_T          dm_ulp_read_local_supported_features_req;
    DM_HCI_ULP_READ_LOCAL_SUPPORTED_FEATURES_CFM_T          dm_ulp_read_local_supported_features_cfm;
    DM_HCI_ULP_SET_RANDOM_ADDRESS_REQ_T                     dm_ulp_set_random_address_req;
    DM_HCI_ULP_SET_RANDOM_ADDRESS_CFM_T                     dm_ulp_set_random_address_cfm;
    DM_HCI_ULP_SET_ADVERTISING_PARAMETERS_REQ_T             dm_ulp_set_advertising_parameters_req;
    DM_HCI_ULP_SET_ADVERTISING_PARAMETERS_CFM_T             dm_ulp_set_advertising_parameters_cfm;
    DM_HCI_ULP_READ_ADVERTISING_CHANNEL_TX_POWER_REQ_T      dm_ulp_read_advertising_channel_tx_power_req;
    DM_HCI_ULP_READ_ADVERTISING_CHANNEL_TX_POWER_CFM_T      dm_ulp_read_advertising_channel_tx_power_cfm;
    DM_HCI_ULP_SET_ADVERTISING_DATA_REQ_T                   dm_ulp_set_advertising_data_req;
    DM_HCI_ULP_SET_ADVERTISING_DATA_CFM_T                   dm_ulp_set_advertising_data_cfm;
    DM_HCI_ULP_SET_SCAN_RESPONSE_DATA_REQ_T                 dm_ulp_set_scan_response_data_req;
    DM_HCI_ULP_SET_SCAN_RESPONSE_DATA_CFM_T                 dm_ulp_set_scan_response_data_cfm;
    DM_HCI_ULP_SET_ADVERTISE_ENABLE_REQ_T                   dm_ulp_set_advertise_enable_req;
    DM_HCI_ULP_SET_ADVERTISE_ENABLE_CFM_T                   dm_ulp_set_advertise_enable_cfm;
    DM_HCI_ULP_SET_SCAN_PARAMETERS_REQ_T                    dm_ulp_set_scan_parameters_req;
    DM_HCI_ULP_SET_SCAN_PARAMETERS_CFM_T                    dm_ulp_set_scan_parameters_cfm;
    DM_HCI_ULP_SET_SCAN_ENABLE_REQ_T                        dm_ulp_set_scan_enable_req;
    DM_HCI_ULP_SET_SCAN_ENABLE_CFM_T                        dm_ulp_set_scan_enable_cfm;
    DM_HCI_ULP_CREATE_CONNECTION_CANCEL_REQ_T               dm_ulp_create_connection_cancel_req;
    DM_HCI_ULP_CREATE_CONNECTION_CANCEL_CFM_T               dm_ulp_create_connection_cancel_cfm;
    DM_HCI_ULP_READ_WHITE_LIST_SIZE_REQ_T                   dm_ulp_read_white_list_size_req;
    DM_HCI_ULP_READ_WHITE_LIST_SIZE_CFM_T                   dm_ulp_read_white_list_size_cfm;
    DM_HCI_ULP_CLEAR_WHITE_LIST_REQ_T                       dm_ulp_clear_white_list_req;
    DM_HCI_ULP_CLEAR_WHITE_LIST_CFM_T                       dm_ulp_clear_white_list_cfm;
    DM_HCI_ULP_ADD_DEVICE_TO_WHITE_LIST_REQ_T               dm_ulp_add_device_to_white_list_req;
    DM_HCI_ULP_ADD_DEVICE_TO_WHITE_LIST_CFM_T               dm_ulp_add_device_to_white_list_cfm;
    DM_HCI_ULP_REMOVE_DEVICE_FROM_WHITE_LIST_REQ_T          dm_ulp_remove_device_from_white_list_req;
    DM_HCI_ULP_REMOVE_DEVICE_FROM_WHITE_LIST_CFM_T          dm_ulp_remove_device_from_white_list_cfm;
    DM_HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_REQ_T        dm_ulp_set_host_channel_classification_req;
    DM_HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_CFM_T        dm_ulp_set_host_channel_classification_cfm;
    DM_HCI_ULP_READ_CHANNEL_MAP_REQ_T                       dm_ulp_read_channel_map_req;
    DM_HCI_ULP_READ_CHANNEL_MAP_CFM_T                       dm_ulp_read_channel_map_cfm;
    DM_HCI_ULP_READ_REMOTE_USED_FEATURES_REQ_T              dm_ulp_read_remote_used_features_req;
    DM_HCI_ULP_READ_REMOTE_USED_FEATURES_CFM_T              dm_ulp_read_remote_used_features_cfm;
    DM_HCI_ULP_ENCRYPT_REQ_T                                dm_ulp_encrypt_req;
    DM_HCI_ULP_ENCRYPT_CFM_T                                dm_ulp_encrypt_cfm;
    DM_HCI_ULP_RAND_REQ_T                                   dm_ulp_rand_req;
    DM_HCI_ULP_RAND_CFM_T                                   dm_ulp_rand_cfm;
    DM_HCI_ULP_READ_SUPPORTED_STATES_REQ_T                  dm_ulp_read_supported_states_req;
    DM_HCI_ULP_READ_SUPPORTED_STATES_CFM_T                  dm_ulp_read_supported_states_cfm;
    DM_HCI_ULP_RECEIVER_TEST_REQ_T                          dm_ulp_receiver_test_req;
    DM_HCI_ULP_RECEIVER_TEST_CFM_T                          dm_ulp_receiver_test_cfm;
    DM_HCI_ULP_TRANSMITTER_TEST_REQ_T                       dm_ulp_transmitter_test_req;
    DM_HCI_ULP_TRANSMITTER_TEST_CFM_T                       dm_ulp_transmitter_test_cfm;
    DM_HCI_ULP_TEST_END_REQ_T                               dm_ulp_test_end_req;
    DM_HCI_ULP_TEST_END_CFM_T                               dm_ulp_test_end_cfm;
#endif

    DM_HCI_WRITE_AFH_CHANNEL_CLASS_M_CFM_T                  dm_write_afh_channel_class_m_cfm;
    DM_HCI_WRITE_AFH_CHANNEL_CLASS_M_REQ_T                  dm_write_afh_channel_class_m_req;
    DM_HCI_WRITE_AUTO_FLUSH_TIMEOUT_CFM_T                   dm_write_auto_flush_timeout_cfm;
    DM_HCI_WRITE_AUTO_FLUSH_TIMEOUT_REQ_T                   dm_write_auto_flush_timeout_req;
    DM_HCI_WRITE_CLASS_OF_DEVICE_CFM_T                      dm_write_class_of_device_cfm;
    DM_HCI_WRITE_CLASS_OF_DEVICE_REQ_T                      dm_write_class_of_device_req;
    DM_HCI_WRITE_CONN_ACCEPT_TIMEOUT_CFM_T                  dm_write_conn_accept_to_cfm;
    DM_HCI_WRITE_CONN_ACCEPT_TIMEOUT_REQ_T                  dm_write_conn_accept_to_req;
    DM_HCI_WRITE_CURRENT_IAC_LAP_CFM_T                      dm_write_current_iac_lap_cfm;
    DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_T                      dm_write_current_iac_lap_req;
    DM_HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS_CFM_T         dm_write_default_link_policy_settings_cfm;
    DM_HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS_REQ_T         dm_write_default_link_policy_settings_req;
    DM_HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_CFM_T       dm_write_extended_inquiry_response_data_cfm;
    DM_HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_REQ_T       dm_write_extended_inquiry_response_data_req;
    DM_HCI_WRITE_HOLD_MODE_ACTIVITY_CFM_T                   dm_write_hold_mode_activity_cfm;
    DM_HCI_WRITE_HOLD_MODE_ACTIVITY_REQ_T                   dm_write_hold_mode_activity_req;
    DM_HCI_WRITE_INQUIRY_MODE_CFM_T                         dm_write_inquiry_mode_cfm;
    DM_HCI_WRITE_INQUIRY_MODE_REQ_T                         dm_write_inquiry_mode_req;
    DM_HCI_WRITE_INQUIRYSCAN_ACTIVITY_CFM_T                 dm_write_inquiryscan_activity_cfm;
    DM_HCI_WRITE_INQUIRYSCAN_ACTIVITY_REQ_T                 dm_write_inquiryscan_activity_req;
    DM_HCI_WRITE_INQUIRY_SCAN_TYPE_CFM_T                    dm_write_inquiry_scan_type_cfm;
    DM_HCI_WRITE_INQUIRY_SCAN_TYPE_REQ_T                    dm_write_inquiry_scan_type_req;
    DM_HCI_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL_CFM_T         dm_write_inquiry_transmit_power_level_cfm;
    DM_HCI_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL_REQ_T         dm_write_inquiry_transmit_power_level_req;
    DM_HCI_WRITE_LINK_SUPERV_TIMEOUT_CFM_T                  dm_write_link_superv_timeout_cfm;
    DM_HCI_WRITE_LINK_SUPERV_TIMEOUT_REQ_T                  dm_write_link_superv_timeout_req;
    DM_HCI_WRITE_LINK_POLICY_SETTINGS_CFM_T                 dm_write_link_policy_settings_cfm;
    DM_HCI_WRITE_LINK_POLICY_SETTINGS_REQ_T                 dm_write_link_policy_settings_req;
    DM_HCI_WRITE_NUM_BCAST_RETXS_CFM_T                      dm_write_num_bcast_txs_cfm;
    DM_HCI_WRITE_NUM_BCAST_RETXS_REQ_T                      dm_write_num_bcast_txs_req;
    DM_HCI_WRITE_PAGESCAN_ACTIVITY_CFM_T                    dm_write_pagescan_activity_cfm;
    DM_HCI_WRITE_PAGESCAN_ACTIVITY_REQ_T                    dm_write_pagescan_activity_req;
    DM_HCI_WRITE_PAGESCAN_MODE_CFM_T                        dm_write_pagescan_mode_cfm;
    DM_HCI_WRITE_PAGESCAN_MODE_REQ_T                        dm_write_pagescan_mode_req;
    DM_HCI_WRITE_PAGESCAN_PERIOD_MODE_CFM_T                 dm_write_pagescan_period_mode_cfm;
    DM_HCI_WRITE_PAGESCAN_PERIOD_MODE_REQ_T                 dm_write_pagescan_period_mode_req;
    DM_HCI_WRITE_PAGE_SCAN_TYPE_CFM_T                       dm_write_page_scan_type_cfm;
    DM_HCI_WRITE_PAGE_SCAN_TYPE_REQ_T                       dm_write_page_scan_type_req;
    DM_HCI_WRITE_PAGE_TIMEOUT_CFM_T                         dm_write_page_to_cfm;
    DM_HCI_WRITE_PAGE_TIMEOUT_REQ_T                         dm_write_page_to_req;
    DM_HCI_WRITE_PIN_TYPE_CFM_T                             dm_write_pin_type_cfm;
    DM_HCI_WRITE_PIN_TYPE_REQ_T                             dm_write_pin_type_req;
    DM_HCI_WRITE_SCAN_ENABLE_CFM_T                          dm_write_scan_enable_cfm;
    DM_HCI_WRITE_SCAN_ENABLE_REQ_T                          dm_write_scan_enable_req;
    DM_HCI_WRITE_SCO_FLOW_CON_ENABLE_CFM_T                  dm_write_sco_flow_control_enable_cfm;
    DM_HCI_WRITE_SCO_FLOW_CON_ENABLE_REQ_T                  dm_write_sco_flow_control_enable_req;
    DM_HCI_WRITE_STORED_LINK_KEY_CFM_T                      dm_write_stored_link_key_cfm;
    DM_HCI_WRITE_STORED_LINK_KEY_REQ_T                      dm_write_stored_link_key_req;
    DM_HCI_WRITE_VOICE_SETTING_CFM_T                        dm_write_voice_setting_cfm;
    DM_HCI_WRITE_VOICE_SETTING_REQ_T                        dm_write_voice_setting_req;

    DM_LP_WRITE_POWERSTATES_CFM_T                           dm_lp_write_powerstates_cfm;
    DM_LP_WRITE_POWERSTATES_REQ_T                           dm_lp_write_powerstates_req;
    DM_LP_WRITE_ROLESWITCH_POLICY_REQ_T                     dm_lp_write_roleswitch_policy_req;
    DM_LP_WRITE_ROLESWITCH_POLICY_CFM_T                     dm_lp_write_roleswitch_policy_cfm;
    DM_LP_WRITE_ALWAYS_MASTER_DEVICES_REQ_T                 dm_lp_write_always_master_devices_req;
    DM_LP_WRITE_ALWAYS_MASTER_DEVICES_CFM_T                 dm_lp_write_always_master_devices_cfm;

    DM_SET_BT_VERSION_CFM_T                                 dm_set_bt_version_cfm;
    DM_SET_BT_VERSION_REQ_T                                 dm_set_bt_version_req;

    DM_SET_DEFAULT_LINK_POLICY_REQ_T                        dm_set_default_link_policy_req;

    DM_SM_ACCESS_CFM_T                                      dm_sm_access_cfm;
    DM_SM_ACCESS_IND_T                                      dm_sm_access_ind;
    DM_SM_ACCESS_REQ_T                                      dm_sm_access_req;
    DM_SM_ADD_DEVICE_CFM_T                                  dm_sm_add_device_cfm;
    DM_SM_ADD_DEVICE_REQ_T                                  dm_sm_add_device_req;
    DM_SM_AUTHENTICATE_CFM_T                                dm_sm_authenticate_cfm;
    DM_SM_AUTHENTICATE_REQ_T                                dm_sm_authenticate_req;
    DM_SM_AUTHORISE_IND_T                                   dm_sm_authorise_ind;
    DM_SM_AUTHORISE_RSP_T                                   dm_sm_authorise_rsp;
    DM_SM_BONDING_CANCEL_REQ_T                              dm_sm_bonding_cancel_req;
    DM_SM_BONDING_CFM_T                                     dm_sm_bonding_cfm;
    DM_SM_BONDING_REQ_T                                     dm_sm_bonding_req;
    DM_SM_CSRK_COUNTER_CHANGE_IND_T                         dm_sm_csrk_counter_change_ind;
    DM_SM_DATA_SIGN_COMMON_T                                dm_sm_data_sign_common;
    DM_SM_DATA_SIGN_CFM_T                                   dm_sm_data_sign_cfm;
    DM_SM_DATA_SIGN_REQ_T                                   dm_sm_data_sign_req;
    DM_SM_ENCRYPT_CFM_T                                     dm_sm_encrypt_cfm;
    DM_SM_ENCRYPTION_CHANGE_IND_T                           dm_sm_encryption_change_ind;
    DM_SM_ENCRYPT_REQ_T                                     dm_sm_encrypt_req;
    DM_SM_GENERATE_NONRESOLVABLE_PRIVATE_ADDRESS_CFM_T      dm_sm_generate_nonresolvable_private_address_cfm;
    DM_SM_GENERATE_NONRESOLVABLE_PRIVATE_ADDRESS_REQ_T      dm_sm_generate_nonresolvable_private_address_req;
    DM_SM_IO_CAPABILITY_REQUEST_IND_T                       dm_sm_io_capability_request_ind;
    DM_SM_IO_CAPABILITY_REQUEST_NEG_RSP_T                   dm_sm_io_capability_request_neg_rsp;
    DM_SM_IO_CAPABILITY_REQUEST_RSP_T                       dm_sm_io_capability_request_rsp;
    DM_SM_IO_CAPABILITY_RESPONSE_IND_T                      dm_sm_io_capability_response_ind;
    DM_SM_KEYPRESS_NOTIFICATION_IND_T                       dm_sm_keypress_notification_req;
    DM_SM_LINK_KEY_IND_T                                    dm_sm_link_key_ind;
    DM_SM_LINK_KEY_REQUEST_IND_T                            dm_sm_link_key_request_ind;
    DM_SM_LINK_KEY_REQUEST_RSP_T                            dm_sm_link_key_request_rsp;
    DM_SM_PIN_REQUEST_IND_T                                 dm_sm_pin_request_ind;
    DM_SM_PIN_REQUEST_RSP_T                                 dm_sm_pin_request_rsp;
    DM_SM_READ_DEVICE_CFM_T                                 dm_sm_read_device_cfm;
    DM_SM_READ_DEVICE_REQ_T                                 dm_sm_read_device_req;
    DM_SM_READ_LOCAL_OOB_DATA_CFM_T                         dm_sm_read_local_oob_data_cfm;
    DM_SM_READ_LOCAL_OOB_DATA_REQ_T                         dm_sm_read_local_oob_data_req;
    DM_SM_REGISTER_OUTGOING_CFM_T                           dm_sm_register_outgoing_cfm;
    DM_SM_REGISTER_OUTGOING_REQ_T                           dm_sm_register_outgoing_req;
    DM_SM_REGISTER_CFM_T                                    dm_sm_register_cfm;
    DM_SM_REGISTER_REQ_T                                    dm_sm_register_req;
    DM_SM_REMOVE_DEVICE_CFM_T                               dm_sm_remove_device_cfm;
    DM_SM_REMOVE_DEVICE_REQ_T                               dm_sm_remove_device_req;
    DM_SM_SEND_KEYPRESS_NOTIFICATION_REQ_T                  dm_sm_send_keypress_notification_req;
    DM_SM_CONFIGURE_LOCAL_ADDRESS_CFM_T                     dm_sm_configure_local_address_cfm;
    DM_SM_CONFIGURE_LOCAL_ADDRESS_REQ_T                     dm_sm_configure_local_address_req;
    DM_SM_INIT_REQ_T                                        dm_sm_init_req;
    DM_SM_INIT_CFM_T                                        dm_sm_init_cfm;
    DM_SM_KEY_REQUEST_IND_T                                 dm_sm_key_request_ind;
    DM_SM_KEY_REQUEST_RSP_T                                 dm_sm_key_request_rsp;
    DM_SM_KEYS_IND_T                                        dm_sm_keys_ind;
    DM_SM_SECURITY_CFM_T                                    dm_sm_security_cfm;
    DM_SM_SECURITY_COMMON_T                                 dm_sm_security_common;
    DM_SM_SECURITY_IND_T                                    dm_sm_security_ind;
    DM_SM_SECURITY_REQ_T                                    dm_sm_security_req;
    DM_SM_SIMPLE_PAIRING_COMPLETE_IND_T                     dm_sm_simple_pairing_complete_ind;
    DM_SM_UNREGISTER_OUTGOING_CFM_T                         dm_sm_unregister_outgoing_cfm;
    DM_SM_UNREGISTER_OUTGOING_REQ_T                         dm_sm_unregister_outgoing_req;
    DM_SM_UNREGISTER_CFM_T                                  dm_sm_unregister_cfm;
    DM_SM_UNREGISTER_REQ_T                                  dm_sm_unregister_req;
    DM_SM_USER_CONFIRMATION_REQUEST_IND_T                   dm_sm_user_confirmation_request_ind;
    DM_SM_USER_CONFIRMATION_REQUEST_NEG_RSP_T               dm_sm_user_confirmation_request_neg_rsp;
    DM_SM_USER_CONFIRMATION_REQUEST_RSP_T                   dm_sm_user_confirmation_request_rsp;
    DM_SM_USER_PASSKEY_NOTIFICATION_IND_T                   dm_sm_user_passkey_notification_ind;
    DM_SM_USER_PASSKEY_REQUEST_IND_T                        dm_sm_user_passkey_request_ind;
    DM_SM_USER_PASSKEY_REQUEST_NEG_RSP_T                    dm_sm_user_passkey_request_neg_rsp;
    DM_SM_USER_PASSKEY_REQUEST_RSP_T                        dm_sm_user_passkey_request_rsp;

    DM_SYNC_CONNECT_CFM_T                                   dm_sync_connect_cfm;
    DM_SYNC_CONNECT_COMPLETE_IND_T                          dm_sync_connect_complete_ind;
    DM_SYNC_CONNECT_IND_T                                   dm_sync_connect_ind;
    DM_SYNC_CONNECT_REQ_T                                   dm_sync_connect_req;
    DM_SYNC_CONNECT_RSP_T                                   dm_sync_connect_rsp;
    DM_SYNC_DISCONNECT_CFM_T                                dm_sync_disconnect_cfm;
    DM_SYNC_DISCONNECT_IND_T                                dm_sync_disconnect_ind;
    DM_SYNC_DISCONNECT_REQ_T                                dm_sync_disconnect_req;
    DM_SYNC_REGISTER_CFM_T                                  dm_sync_register_cfm;
    DM_SYNC_REGISTER_REQ_T                                  dm_sync_register_req;
    DM_SYNC_RENEGOTIATE_CFM_T                               dm_sync_renegotiate_cfm;
    DM_SYNC_RENEGOTIATE_IND_T                               dm_sync_renegotiate_ind;
    DM_SYNC_RENEGOTIATE_REQ_T                               dm_sync_renegotiate_req;
    DM_SYNC_UNREGISTER_CFM_T                                dm_sync_unregister_cfm;
    DM_SYNC_UNREGISTER_REQ_T                                dm_sync_unregister_req;

    DM_WRITE_CACHED_CLOCK_OFFSET_CFM_T                      dm_write_cached_clock_offset_cfm;
    DM_WRITE_CACHED_CLOCK_OFFSET_REQ_T                      dm_write_cached_clock_offset_req;
    DM_WRITE_CACHED_PAGE_MODE_CFM_T                         dm_write_cached_page_mode_cfm;
    DM_WRITE_CACHED_PAGE_MODE_REQ_T                         dm_write_cached_page_mode_req;
} DM_UPRIM_T;



/*============================================================================*
 *                               PUBLIC FUNCTIONS
 *============================================================================*/
/* None */

#ifdef __cplusplus
}
#endif 

/*=============================== END OF FILE ================================*/
#endif /* ndef _DM_PRIM_H */
