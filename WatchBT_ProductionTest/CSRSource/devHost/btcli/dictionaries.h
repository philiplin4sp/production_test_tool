/*
 * Copyright (C) 2000-2006 Cambridge Silicon Radio Ltd.; all rights reserved.
 * btcli Dictionaries, and some constants.
 *
 * MODIFICATION HISTORY
 *	#67    23:jul:04 sk03   B-3349: Improvements to eSCO test code.
 *      #70    25:nov:04 sk03   B-4672: Replace pause_user_data flag with
 *                              semaphore.
 *
 * $Id: //depot/bc/bluesuite_2_4/devHost/btcli/dictionaries.h#2 $
 */

#ifndef DICTIONARIES_H
#define DICTIONARIES_H


#include "dictionary.h"

#include "../common/types_t.h"

#include "host/debug/debug_hci.h"

extern Dictionary transportErrors_d;


extern Dictionary iac_d;
#define GIAC		0x9e8b33

extern Dictionary packetType_d;
#define ACL_ALL		0xcc18

extern Dictionary epacketType_d;

extern Dictionary psrm_d;
#define PSRM_R0		0
#define PSRM_R1		1

extern Dictionary pspm_d;

extern Dictionary psm_d;
#define MANDATORY	0

extern Dictionary ms_d;
#define MASTER		0
#define SLAVE		1

extern Dictionary disconnectReason_d;
#define UEC		0x13

extern Dictionary rejectConnectionReason_d;
#define SR		0x0e

extern Dictionary offOn_d;
#define OFFON_OFF	0
#define OFFON_ON	1

extern Dictionary keyFlag_d;

extern Dictionary keyType_d;

extern Dictionary serviceType_d;

extern Dictionary linkPolicySetting_d;

extern Dictionary filterType_d;

extern Dictionary filterConditionType_d;

extern Dictionary autoAccept_d;

extern Dictionary pinType_d;

extern Dictionary all_d;

extern Dictionary oneOrAll_d;
#define ONEORALL_ONE	0 /* sic */
#define ONEORALL_ALL	1 /* sic */

extern Dictionary scanEnable_d;

extern Dictionary disabledEnabled_d;

extern Dictionary enabledDisabled_d;

extern Dictionary encryptionMode_d;

extern Dictionary powerLevelType_d;

extern Dictionary hciVersion_d;

extern Dictionary lmpVersion_d;

extern Dictionary manufacturerName_d;

extern Dictionary countryCode_d;

extern Dictionary loopbackMode_d;

enum BF/*BROADCAST_TYPE*/
{
    bf_p2p = 0,
    bf_active = 1,
    bf_piconet = 2,
    bf_unused = 3
};

enum PBF/*BOUNDARY_TYPE*/
{
    pbf_start_not_auto_flush = 0,
    pbf_continuation = 1,
    pbf_start = 2,
    pbf_reserved = 3,
};

extern Dictionary pbf_d;
#define START_NAF 	0      /* First non-auto flushable packet */
#define CONTIN		1
#define START		2      /* First auto flushable packet */
#define LCH_LMP		3

extern Dictionary bf_d;
#define PTP		0
#define ACTIVE_BC		1
#define PICONET_BC		2

extern Dictionary cid_d;

extern Dictionary errorCode_d;
extern Dictionary ulp_errorCode_d;
extern Dictionary ulp_reasonCode_d;

extern Dictionary linkType_d;
#define LT_SCO		0
#define LT_ACL		1
#define LT_ESCO		2

extern Dictionary mode_d;

extern Dictionary bcCmdStatus_d;
#define BCCMD_OK	0

extern Dictionary bcCmdVarID_d;

extern Dictionary builddef_d;

extern Dictionary evtCntID_d;

extern Dictionary radiotest_d;

extern Dictionary hqStatus_d;

extern Dictionary hqVarID_d;

extern Dictionary psKey_d;

extern Dictionary id_d;

extern Dictionary lmpDebug_d;
extern Dictionary ulpAdvDebug_d;

extern Dictionary lmp_d;

extern Dictionary ulp_llc_d;

extern Dictionary testScenario_d;
#define PAUSE_TEST_MODE	0
#define EXIT_TEST_MODE	255

extern Dictionary hoppingMode_d;

extern Dictionary powerControlMode_d;

extern Dictionary ptt_d;

extern Dictionary basebandPacketType_d;

extern Dictionary testPacketType_d;

extern Dictionary fsm_d;

extern Dictionary scoPacket_d;

extern Dictionary escoPacket_d;

extern Dictionary airMode_d;

extern Dictionary tcf_d;

extern Dictionary i_d;

extern Dictionary transport_d;
#define TRANSPORT_NONE	0
#define TRANSPORT_BCSP	1
#define TRANSPORT_USB	2
#define TRANSPORT_H4	3
#define TRANSPORT_USER	4
#define TRANSPORT_H5	6
#define TRANSPORT_H4DS	7

extern Dictionary faultID_d;

extern Dictionary badPDUreason_d;

extern Dictionary stores_d;

extern Dictionary storesSet_d;

extern Dictionary storesClr_d;

extern Dictionary pattern_d;

extern Dictionary unpark_d;

extern Dictionary l2cap_reason_d;

extern Dictionary l2cap_psm_value_d;

extern Dictionary l2cap_result_value_d;

extern Dictionary l2cap_status_value_d;

extern Dictionary l2cap_config_result_value_d;

extern Dictionary l2cap_config_type_value_d;

extern Dictionary l2cap_infotype_value_d;

extern Dictionary l2cap_info_result_value_d;

extern Dictionary l2cap_config_qos_service_type_value_d;

extern Dictionary sdp_uuid_d;

extern Dictionary sdp_attribute_d;

extern Dictionary sdp_error_code_d;

extern Dictionary bnep_type_d;

extern Dictionary bnep_control_type_d;

extern Dictionary ethertype_d;

extern Dictionary ip_protocol_d;

extern Dictionary ar_op_d;

extern Dictionary in_port_d;

extern Dictionary icmpv4_type_d;

extern Dictionary icmpv6_type_d;

extern Dictionary timer_d;



extern Dictionary rfcomm_dlci_d;

extern Dictionary rfcomm_control_field_d;

extern Dictionary rfcomm_control_message_d;

extern Dictionary afh_channel_classification_d;
#define SACC_ALL 0x00
#define SACC_EVEN 0x01
#define SACC_ODD 0x02
#define SACC_DOT11_CHANNEL_1 2412
#define SACC_DOT11_CHANNEL_2 2417
#define SACC_DOT11_CHANNEL_3 2422
#define SACC_DOT11_CHANNEL_4 2427
#define SACC_DOT11_CHANNEL_5 2432
#define SACC_DOT11_CHANNEL_6 2437
#define SACC_DOT11_CHANNEL_7 2442
#define SACC_DOT11_CHANNEL_8 2447
#define SACC_DOT11_CHANNEL_9 2452
#define SACC_DOT11_CHANNEL_10 2457
#define SACC_DOT11_CHANNEL_11 2462
#define SACC_DOT11_CHANNEL_12 2467
#define SACC_DOT11_CHANNEL_13 2472
#define SACC_DOT11_CHANNEL_14 2484

extern Dictionary wc_d;

extern Dictionary block_rx_d;
#define LM_BLOCK_RX_LMP_SET_AFH 0
#define LM_BLOCK_RX_TDD_POLL 1

extern Dictionary st_d;

extern Dictionary im_d;

extern Dictionary fd_d;

extern Dictionary re_d;

extern Dictionary psmt_d;

extern Dictionary lc_esco_debug_d;

extern Dictionary lm_esco_ns_d;

extern Dictionary afh_class_source_d;

extern Dictionary l2capCrcRxTx_d;

extern Dictionary lockedAclPacketTypes_d;

extern Dictionary et_fns_d;

extern Dictionary et_modes_d;

extern Dictionary pause_d;

extern Dictionary eirdt_d;

extern Dictionary eirdtf_d;

extern Dictionary fecr_d;

extern Dictionary fm_reg_d;
extern Dictionary fmtx_reg_d;

extern Dictionary fm_flag_vals; 
extern Dictionary fm_int_mask_vals;

extern Dictionary fmtx_flag_vals; 
extern Dictionary fmtx_int_mask_vals;

/* Simple Pairing Dictionaries */

extern Dictionary spm_d;

extern Dictionary spdm_d;

extern Dictionary iocap_d;

extern Dictionary oobpresent_d;

extern Dictionary authenticationrequirements_d;

extern Dictionary notificationtype_d;


/* Enhanced Power Control dictionaries */
extern Dictionary enhanced_power_control_request_d;

extern Dictionary enhanced_power_control_response_field_d;


/* Power Control debug command dictionary */
extern Dictionary force_power_control_request_d;


/* Encapsulated PDU dictionaries */

extern Dictionary encapsulated_header_major_minor_d;


/* LE Host Support Dictionaries */

extern Dictionary lesh_d;

extern Dictionary sleh_d;


/* BLE dictionaries */

extern Dictionary am_d;
extern Dictionary sm_d;
extern Dictionary filtdup_d;
extern Dictionary afp_d;
extern Dictionary sfp_d;
extern Dictionary ifp_d;
extern Dictionary sac_d;
extern Dictionary stype_d;
extern Dictionary at_d;
extern Dictionary pktpayload_d;
extern Dictionary et_d;
extern Dictionary rdfscan_d;
extern Dictionary aet_d;
extern Dictionary clkacc_d;
extern Dictionary mp_d;
extern Dictionary er_d;
extern Dictionary pt_d;
extern Dictionary prof_d;
extern Dictionary enc_d;
extern Dictionary ulp_radio_test_d;
extern Dictionary ulp_adv_filt_operations;
extern Dictionary ulp_adv_filt_ad_type;

/* FM dictionaries */

extern Dictionary miscRegisterMode_d;

extern Dictionary miscFmAudioRoutingModeAra_d;

extern Dictionary miscFmAudioRoutingModeArb_d;

extern Dictionary miscFmAudioRoutingModeArc_d;

extern Dictionary miscFmAudioRoutingModeVsa_d;

extern Dictionary miscFmAudioRoutingModeVsb_d;

extern Dictionary miscFmAudioRoutingModeVsc_d;

/* Dictionaries shared by the WLAN Coexistence Interface Test BCCMD and FBCMD */
extern Dictionary LwctMode_d;
extern Dictionary LwctValue_d;

extern Dictionary fastpipe_result_d;
extern Dictionary fastpipe_engine_result_d;

extern Dictionary stream_source_sink_d;
extern Dictionary stream_config_d;

extern Dictionary SetPinFunction_d;
extern Dictionary mic_bias_configure_d;

extern Dictionary led_config_d;

extern Dictionary siflash_d;

/* DSPManager dictionaries */

extern Dictionary stibbons_error_codes_d;
extern Dictionary baton_messages_d;
extern Dictionary create_operator_skip_flag_d;

/* Spi Lock Status */
extern Dictionary spilock_d;


#endif /* DICTIONARIES_H */
