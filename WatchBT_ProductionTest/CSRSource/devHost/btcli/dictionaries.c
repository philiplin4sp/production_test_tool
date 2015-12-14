// Copyright (C) 2000-2006 Cambridge Silicon Radio Ltd.; all rights reserved.
// btcli Dictionaries.
// $Id: //depot/bc/bluesuite_2_4/devHost/btcli/dictionaries.c#5 $

#include "btcli.h"

#include "dictionaries.h"
#include "globals.h"		/* For the patterns */

#include "host/bccmd/bccmdpdu.h"
#include "host/fm/fm_api.h"
#include "host/fastpipe/fastpipe_types.h"
#include "fastpipe_engine_iface.h"
#include "csrhci/transportapi.h"


Dictionary transportErrors_d =
{
    bcsp_sync_recvd, "Unexpected BCSP sync received",
    bcsp_retry_limit, "BCSP retry limit exceeded",
    h4_sync_loss, "H4 sync lost",
    uart_failure, "UART failure",
    usb_handle_loss, "USB handle lost",
    sdio_transport_fail, "SDIO transport failure",
    0, NULL
};


Dictionary iac_d =
{
	0x9e8b00,	"liac",
	GIAC,		"giac",
	0,		NULL
};


Dictionary packetType_d =
{
	0x0008,		"dm1",
	0x0010,		"dh1",
	0x0400,		"dm3",
	0x0800,		"dh3",
	0x4000,		"dm5",
	0x8000,		"dh5",
	0x4408,		"dm",
	0x8810,		"dh",
	0x0018,		"d1",
	0x0c00,		"d3",
	0xc000,		"d5",
	0x0c18,		"d13",
	0xcc00,		"d35",
	0xc018,		"d15",
	ACL_ALL,	"d135",
	ACL_ALL,	"d",
	0x330e,		"dm1-",
	0x3316,		"dh1-",
	0x3706,		"dm3-",
	0x3b06,		"dh3-",
	0x7306,		"dm5-",
	0xb306,		"dh5-",
	0x770e,		"dm-",
	0xbb16,		"dh-",
	0x331e,		"d1-",
	0x3f0e,		"d3-",
	0xf306,		"d5-",
	0x3f1e,		"d13-",
	0xff06,		"d35-",
	0xf31e,		"d15-",
	0xff1e,		"d135-",
	0xff1e,		"d-",
	0x3304,		"2dh1",
	0x3302,		"3dh1",
	0x3206,		"2dh3",
	0x3106,		"3dh3",
	0x2306,		"2dh5",
	0x1306,		"3dh5",
	0x2204,		"2dh",
	0x1102,		"3dh",
	0x3300,		"mr1",
	0x3006,		"mr3",
	0x0306,		"mr5",
	0x0000,		"mr",
	0x0020,		"hv1",
	0x0020,		"h1",
	0x0040,		"hv2",
	0x0040,		"h2",
	0x0080,		"hv3",
	0x0080,		"h3",
	0x0060,		"hv12",
	0x0060,		"h12",
	0x00a0,		"hv13",
	0x00a0,		"h13",
	0x00c0,		"hv23",
	0x00c0,		"h23",
	0x00e0,		"hv123",
	0x00e0,		"h123",
	0x00e0,		"hv",
	0x00e0,		"h",
	0,		NULL
};


Dictionary epacketType_d =
{
	0x0001,		"hv1",
	0x0001,		"h1",
	0x0002,		"hv2",
	0x0002,		"h2",
	0x0004,		"hv3",
	0x0004,		"h3",
	0x0007,		"hv",
	0x0007,		"h",
	0x0008,		"ev3",
	0x0008,		"e3",
	0x0010,		"ev4",
	0x0010,		"e4",
	0x0020,		"ev5",
	0x0020,		"e5",
	0x0038,		"ev",
	0x0038,		"e",
	0x003f,		"all",
	0x003f,		"a",
	0x03c1,		"hv1-",
	0x03c1,		"h1-",
	0x03c2,		"hv2-",
	0x03c2,		"h2-",
	0x03c4,		"hv3-",
	0x03c4,		"h3-",
	0x03c7,		"hv-",
	0x03c7,		"h-",
	0x03c8,		"ev3-",
	0x03c8,		"e3-",
	0x03d0,		"ev4-",
	0x03d0,		"e4-",
	0x03e0,		"ev5-",
	0x03e0,		"e5-",
	0x03f8,		"ev-",
	0x03f8,		"e-",
	0x0380,		"2ev3",
	0x0380,		"2ev3-", /* - variants for consistency */
	0x0340,		"3ev3",
	0x0340,		"3ev3-",
	0x02c0,		"2ev5",
	0x02c0,		"2ev5-",
	0x01c0,		"3ev5",
	0x01c0,		"3ev5-",
	0x0280,		"2ev",
	0x0280,		"2ev-",
	0x0140,		"3ev",
	0x0140,		"3ev-",
	0,		NULL
};


Dictionary psrm_d =
{
	PSRM_R0,	"r0",
	PSRM_R1,	"r1",
	2,		"r2",
	0,		NULL
};


Dictionary pspm_d =
{
	0,		"p0",
	1,		"p1",
	2,		"p2",
	0,		NULL
};


Dictionary psm_d =
{
	MANDATORY,	"mandatory_page_scan_mode",
	MANDATORY,	"m",
	1,		"optional_page_scan_mode_i",
	1,		"o1",
	2,		"optional_page_scan_mode_ii",
	2,		"o2",
	3,		"optional_page_scan_mode_iii",
	3,		"o3",
	0,		NULL
};


Dictionary ms_d =
{
	MASTER,		"master",
	MASTER,		"m",
	1,		"slave",
	1,		"s",
	0,		NULL
};


Dictionary disconnectReason_d =
{
	0x05,		"authentication_failure",
	0x05,		"af",
	UEC,		"other_end_terminated_connection:_user_ended_connection",
	UEC,		"user_ended_connection",
	UEC,		"uec",
	0x14,		"other_end_terminated_connection:_low_resources",
	0x14,		"low_resources",
	0x14,		"lr",
	0x15,		"other_end_terminated_connection:_about_to_power_off",
	0x15,		"about_to_power_off",
	0x15,		"atpo",
	0x1a,		"unsupported_remote_feature",
	0x1a,		"urf",
	0x29,		"pairing_with_unit_key_not_supported",
	0x29,		"unit_key_not_supported",
	0x29,		"ukns",
	0,		NULL
};


Dictionary rejectConnectionReason_d =
{
	0x0d,		"host_rejected_due_to_limited_resources",
	0x0d,		"limited_resources",
	0x0d,		"lr",
	0x0e,		"host_rejected_due_to_security_reasons",
	0x0e,		"security_reasons",
	0x0e,		"sr",
	0x0f,		"host_rejected_due_to_remote_device_is_only_a_personal_device",
	0x0f,		"remote_device_is_only_a_personal_device",
	0x0f,		"personal_device",
	0x0f,		"pd",
	0,		NULL
};


Dictionary offOn_d =
{
	0,		"off",
	1,		"on",
	0,		NULL
};


Dictionary keyFlag_d =
{
	0,		"use_semi-permanent_link_key",
	0,		"sp",
	1,		"use_temporary_link_key",
	1,		"t",
	0,		NULL
};


Dictionary keyType_d =
{
	0,		"combination_key",
	1,		"local_unit_key",
	2,		"remote_unit_key",
	3,		"debug_combination_key",
	4,		"unauthenticated_combination_key",
	5,		"authenticated_combination_key",
	6,		"changed_combination_key",
	0,		NULL
};


Dictionary serviceType_d =
{
	0,		"no_traffic",
	0,		"nt",
	1,		"best_effort",
	1,		"be",
	2,		"guaranteed",
	2,		"g",
	0,		NULL
};


Dictionary linkPolicySetting_d =
{
	0,		"disable_all",
	0,		"dall",
	0,		"d",
	0,		"none",
	0,		"n",
	1,		"enable_master_slave_switch",
	1,		"emss",
	1,		"mss",
	1,		"m",
	2,		"enable_hold_mode",
	2,		"ehm",
	2,		"hm",
	2,		"h",
	4,		"enable_sniff_mode",
	4,		"esm",
	4,		"sm",
	4,		"s",
	8,		"enable_park_mode",
	8,		"epm",
	8,		"pm",
	8,		"p",
	0xf,		"enable_all",
	0xf,		"eall",
	0xf,		"e",
	0xf,		"all",
	0xf,		"a",
	0xf,		"enable_all_1.1",
	0xf,		"eall11",
	0xf,		"e11",
	0xf,		"all11",
	0xf,		"a11",
	16,		"enable_scatter_mode",
	16,		"escm",
	16,		"scm",
	16,		"sc",
	0x1f,		"enable_all_1.2",
	0x1f,		"eall12",
	0x1f,		"e12",
	0x1f,		"all12",
	0x1f,		"a12",
	0,		NULL
};


Dictionary filterType_d =
{
	0,		"clear",
	1,		"inquiry_result",
	1,		"ir",
	2,		"connection_setup",
	2,		"cs",
	0,		NULL
};


Dictionary filterConditionType_d =
{
	0,		"all",
	1,		"specific_class_of_device",
	1,		"cod",
	2,		"specific_bd_addr",
	2,		"ba",
	0,		NULL
};


Dictionary autoAccept_d =
{
	1,		"manual_accept",
	1,		"m",
	2,		"auto-accept",
	2,		"auto_accept",
	2,		"a",
	3,		"auto-accept_with_switch",
	3,		"auto_accept_with_switch",
	3,		"s",
	0,		NULL
};


Dictionary pinType_d =
{
	0,		"variable",
	0,		"v",
	1,		"fixed",
	1,		"f",
	0,		NULL
};


Dictionary all_d =
{
	1,		"all",
	0,		NULL
};


Dictionary oneOrAll_d =
{
	ONEORALL_ONE,	"one",
	ONEORALL_ALL,	"all",
	0,		NULL
};


Dictionary scanEnable_d =
{
	0,		"none",
	0,		"n",
	1,		"inquiry",
	1,		"i",
	2,		"page",
	2,		"p",
	3,		"inquiry_page",
	3,		"page_inquiry",
	3,		"ip",
	3,		"pi",
	0,		NULL
};


Dictionary disabledEnabled_d =
{
	0,		"disabled",
	0,		"disable",
	0,		"d",
	1,		"enabled",
	1,		"enable",
	1,		"e",
	0,		NULL
};

Dictionary enabledDisabled_d =
{
	0,		"enabled",
	0,		"enable",
	0,		"e",
	1,		"disabled",
	1,		"disable",
	1,		"d",
	0,		NULL
};



Dictionary encryptionMode_d =
{
	0,		"encryption_disabled",
	0,		"d",
	1,		"encryption_only_for_point-to-point_packets",
	1,		"eptp",
	2,		"encryption_for_both_point-to-point_and_broadcast_packets",
	2,		"eboth",
	0,		NULL
};


Dictionary powerLevelType_d =
{
	0,		"current",
	0,		"c",
	1,		"maximum",
	1,		"m",
	0,		NULL
};

/* https://www.bluetooth.org/Technical/AssignedNumbers/hci.htm */
Dictionary hciVersion_d =
{
	0,		"hci_1.0b",
	1,		"hci_1.1",
	2,		"hci_1.2",
	3,		"hci_2.0",
	4,		"hci_2.1",
	5,		"hci_3.0",
	6,		"hci_4.0",
	0,		NULL
};


Dictionary lmpVersion_d =
{
	0,		"lmp_1.0",
	1,		"lmp_1.1",
	2,		"lmp_1.2",
	3,		"lmp_2.0",
	4,		"lmp_2.1",
	5,		"lmp_3.0",
	6,		"lmp_4.0",
	0,		NULL
};


Dictionary manufacturerName_d = /* Last checked 2010-02-12 against https://www.bluetooth.org/Technical/AssignedNumbers/identifiers.htm */
{
	0,		"ericsson_technology_licensing",
	1,		"nokia_mobile_phones",
	2,		"intel_corp.",
	3,		"ibm_corp.",
	4,		"toshiba_corp.",
	5,		"3com",
	6,		"microsoft",
	7,		"lucent",
	8,		"motorola",
	9,		"infineon_technologies_ag",
	10,		"cambridge_silicon_radio",
	11,		"silicon_wave",
	12,		"digianswer_a/s",
	13,		"texas_instruments_inc.",
	14,		"parthus_technologies_inc.",
	15,		"broadcom_corporation",
	16,		"mitel_semiconductor",
	17,		"widcomm,_inc.",
	18,		"zeevo,_inc.",
	19,		"atmel_corporation",
	20,		"mitsubishi_electric_corporation",
	21,		"rtx_telecom_a/s",
	22,		"kc_technology_inc.",
	23,		"newlogic",
	24,		"transilica,_inc.",
	25,		"rohde_&_schwarz_gmbh_&_co._kg",
	26,		"ttpcom_limited",
	27,		"signia_technologies,_inc.",
	28,		"conexant_systems_inc.",
	29,		"qualcomm",
	30,		"inventel",
	31,		"avm_berlin",
	32,		"bandspeed,_inc.",
	33,		"mansella_ltd",
	34,		"nec_corporation",
	35,		"waveplus_technology_co.,_ltd.",
	36,		"alcatel",
	37,		"philips_semiconductors",
	38,		"c_technologies",
	39,		"open_interface",
	40,		"r_f_micro_devices",
	41,		"hitachi_ltd",
	42,		"symbol_technologies,_inc.",
	43,		"tenovis",
	44,		"macronix_international_co._ltd.",
	45,		"gct_semiconductor",
	46,		"norwood_systems",
	47,		"mewtel_technology_inc.",
	48,		"st_microelectronics",
	49,		"synopsys",
	50,		"red-m_(communications)_ltd",
	51,		"commil_ltd",
	52,		"computer_access_technology_corporation_(catc)",
	53,		"eclipse_(hq_espana)_s.l.",
	54,		"renesas_technology_corp.",
	55,		"mobilian_corporation",
	56,		"terax",
	57,		"integrated_system_solution_corp.",
	58,		"matsushita_electric_industrial_co.,_ltd.",
	59,		"gennum_corporation",
	60,		"research_in_motion",
	61,		"ipextreme,_inc.",
	62,		"systems_and_chips,_inc",
	63,		"bluetooth_sig,_inc",
	64,		"seiko_epson_corporation",
	65,		"integrated_silicon_solution_taiwan,_inc.",
	66,		"conwise_technology_corporation_ltd",
	67,		"parrot_sa",
	68,		"socket_mobile",
	69,		"atheros_communications,_inc.",
	70,		"mediatek,_inc.",
	71,		"bluegiga",
	72,		"marvell_technology_group_ltd.",
	73,		"3dsp_corporation",
	74,		"accel_semiconductor_ltd.",
	75,		"continental_automotive_systems",
	76,		"apple,_inc.",
	77,		"staccato_communications,_inc.",
	78,		"avago_technologies",
	79,		"apt_ltd.",
	80,		"sirf_technology,_inc.",
	81,		"tzero_technologies,_inc.",
	82,		"j&m_corporation",
	83,		"free2move_ab",
	84,		"dijoy_corporation",
	85,		"plantronics,_inc.",
	86,		"sony_ericsson_mobile_communications",
	87,		"harman_international_industries,_inc.",
	88,		"vizio,_inc.",
	89,		"nordic_semiconductor_asa",
	90,		"em_microelectronic-marin_sa",
	0xffff,		"unassigned",
	0,		NULL
};


Dictionary countryCode_d =
{
	0,		"north_america_&_europe_[except_france]_and_japan",
	0,		"north_america_and_europe_except_spain_and_france", /* 1.0B */
	1,		"france",
	2,		"spain", /* 1.0B */
	3,		"japan", /* 1.0B */
	0,		NULL
};


Dictionary loopbackMode_d =
{
	0,		"no_loopback",
	0,		"n",
	1,		"local_loopback",
	1,		"l",
	2,		"remote_loopback",
	2,		"r",
	0,		NULL
};


Dictionary pbf_d =
{
	pbf_start_not_auto_flush,	"start_non_auto_flushable",
	pbf_start_not_auto_flush,	"start_naf",
	pbf_start_not_auto_flush,	"snaf",
	pbf_continuation,		"contin",
	pbf_continuation,		"c",
	pbf_continuation,		"continuation",
	pbf_continuation,		"continuing",
	pbf_start,			"start",
	pbf_start,			"s",
	pbf_start,			"first",
	0,				NULL
};


Dictionary bf_d =
{
	bf_p2p,		"point-to-point",
	bf_p2p,		"ptp",
	bf_active,	"active_broadcast",
	bf_active,	"ab",
	bf_piconet,	"piconet_broadcast",
	bf_piconet,	"pb",
	0,		NULL
};


Dictionary cid_d =
{
	0,		"null",
	0,		"n",
	1,		"signalling",
	1,		"s",
	2,		"connectionless",
	2,		"cl",
	0x0040,		"d40",
	0,		NULL
};


Dictionary errorCode_d =
{
	0x00,		"success",
	0x01,		"unknown_hci_command",
	0x02,		"no_connection",
	0x03,		"hardware_failure",
	0x04,		"page_timeout",
	0x05,		"authentication_failure",
	0x06,		"key_missing",
	0x07,		"memory_full",
	0x08,		"connection_timeout",
	0x09,		"max_number_of_connections",
	0x0a,		"max_number_of_sco_connections_to_a_device",
	0x0b,		"acl_connection_already_exists",
	0x0c,		"command_disallowed",
	0x0d,		"host_rejected_due_to_limited_resources",
	0x0e,		"host_rejected_due_to_security_reasons",
	0x0f,		"host_rejected_due_to_remote_device_is_only_a_personal_device",
	0x10,		"host_timeout",
	0x11,		"unsupported_feature_or_parameter_value",
	0x12,		"invalid_hci_command_parameters",
	0x13,		"other_end_terminated_connection:_user_ended_connection",
	0x14,		"other_end_terminated_connection:_low_resources",
	0x15,		"other_end_terminated_connection:_about_to_power_off",
	0x16,		"connection_terminated_by_local_host",
	0x17,		"repeated_attempts",
	0x18,		"pairing_not_allowed",
	0x19,		"unknown_lmp_pdu",
	0x1a,		"unsupported_remote_feature",
	0x1b,		"sco_offset_rejected",
	0x1c,		"sco_interval_rejected",
	0x1d,		"sco_air_mode_rejected",
	0x1e,		"invalid_lmp_parameters",
	0x1f,		"unspecified_error",
	0x20,		"unsupported_lmp_parameter_value",
	0x21,		"role_change_not_allowed",
	0x22,		"lmp_response_timeout",
	0x23,		"lmp_error_transaction_collision",
	0x24,		"lmp_pdu_not_allowed",
	0x25,		"encryption_mode_not_acceptable",
	0x26,		"unit_key_used",
	0x27,		"qos_not_supported",
	0x28,		"instant_passed",
	0x29,		"pairing_with_unit_key_not_supported",
	0x2a,		"different_transaction_collision",
	0x2b,		"insufficient_resources_to_accept_scatter_mode_request",
	0x2c,		"qos_unacceptable_parameter",
	0x2d,		"qos_rejected",
	0x2e,		"channel_classification_not_supported",
	0x2f,		"insufficient_security",
	0x30,		"parameter_out_of_mandatory_range",
	0x31,		"scatter_mode_no_longer_required",
	0x32,		"role_switch_pending",
	0x33,		"scatter_mode_parameter_change_pending",
	0x34,		"reserved_slot_violation",
	0x35,		"switch_failed",
	0x36,		"inquiry_response_data_too_large",
	0x37,		"simple_pairing_not_supported_by_host",
	0x38,		"host_busy_pairing",
	0x39,		"connection_rejected_due_to_no_suitable_channel_found",
	0x3a,		"controller_busy",
	0x3b,		"unacceptable_connection_interval",
	0x3c,		"directed_advertising_timeout",
	0x3d,		"connection_terminated_due_to_mic_failure",
	0x3e,		"connection_failed_to_be_established",
	0,		NULL
};

Dictionary ulp_reasonCode_d =
{
	0x00,		"advertising_stopped_by_the_host",
	0,		NULL
};

Dictionary linkType_d =
{
	LT_SCO,		"sco",
	LT_ACL,		"acl",
	LT_ESCO,	"esco",
	0,		NULL
};


Dictionary mode_d =
{
	0,		"active",
	1,		"hold",
	2,		"sniff",
	3,		"park",
	4,		"scatter",
	0,		NULL
};


Dictionary bcCmdStatus_d = /* Last checked against bccmdpdu.h#60 */
{
	BCCMD_OK,	"ok",
	1,		"no_such_varid",
	2,		"too_big",
	3,		"no_value",
	4,		"bad_pdu",
	5,		"no_access",
	6,		"read_only",
	7,		"write_only",
	8,		"error",
	9,		"permission_denied",
       10,              "timeout",
	0,		NULL
};

Dictionary bcCmdVarID_d = /* Last checked against bccmdpdu.h#70 */
{
	BCCMDVARID_NO_VARIABLE,
		"no_variable",
	BCCMDVARID_COLD_RESET,
		"cold_reset",
	BCCMDVARID_WARM_RESET,
		"warm_reset",
	BCCMDVARID_COLD_HALT,
		"cold_halt",
	BCCMDVARID_WARM_HALT,
		"warm_halt",
	BCCMDVARID_INIT_BT_STACK,
		"init_bt_stack",
	BCCMDVARID_ACTIVATE_BT_STACK,
		"activate_bt_stack",
	BCCMDVARID_ENABLE_TX,
		"enable_tx",
	BCCMDVARID_DISABLE_TX,
		"disable_tx",
	BCCMDVARID_RECAL,
		"recal",
	BCCMDVARID_PSCLRALL,
		"psclrall",
	BCCMDVARID_PS_FACTORY_SET,
		"ps_factory_set",
	BCCMDVARID_PS_FACTORY_RESTORE,
		"ps_factory_restore",
	BCCMDVARID_PS_FACTORY_RESTORE_ALL,
		"ps_factory_restore_all",
	BCCMDVARID_DEFRAG_RESET,
		"defrag_reset",
	BCCMDVARID_KILL_VM_APPLICATION,
		"kill_vm_application",
	BCCMDVARID_HOPPING_ON,
		"hopping_on",
	BCCMDVARID_CANCEL_PAGE,
		"cancel_page",
	BCCMDVARID_CHECK_RAM,
		"check_ram",
	BCCMDVARID_BYPASS_UART,
		"bypass_uart",
	BCCMDVARID_SYNC_CLOCK,
		"sync_clock",
	BCCMDVARID_ENABLE_DUT_MODE,
		"enable_dut_mode",
	BCCMDVARID_BABEL_SPI_RESET_AND_GO,
		"babel_spi_reset_and_go",
	BCCMDVARID_BABEL_SPI_RESET_AND_STOP,
		"babel_spi_reset_and_stop",
	BCCMDVARID_BABEL_SPI_GO,
		"babel_spi_go",
	BCCMDVARID_BABEL_SPI_STOP,
		"babel_spi_stop",
	BCCMDVARID_ROUTE_CLOCK,
		"route_clock",
	BCCMDVARID_ENABLE_DEV_CONNECT,
		"enable_dev_connect",
	BCCMDVARID_BC01_STATUS,
		"bc01_status",
	BCCMDVARID_CONFIG_UART,
		"config_uart",
	BCCMDVARID_TESTA_AMUX,
		"testa_amux",
	BCCMDVARID_TESTB_AMUX,
		"testb_amux",
	BCCMDVARID_PANIC_ARG,
		"panic_arg",
	BCCMDVARID_FAULT_ARG,
		"fault_arg",
	BCCMDVARID_PRESERVE_VALID,
		"preserve_valid",
	BCCMDVARID_ADC0,
		"adc0",
	BCCMDVARID_ADC1,
		"adc1",
	BCCMDVARID_ADC2,
		"adc2",
	BCCMDVARID_ADC3,
		"adc3",
	BCCMDVARID_ADC4,
		"adc4",
	BCCMDVARID_ADC5,
		"adc5",
	BCCMDVARID_ADC6,
		"adc6",
	BCCMDVARID_ADC7,
		"adc7",
	BCCMDVARID_ADC8,
		"adc8",
	BCCMDVARID_ADC9,
		"adc9",
	BCCMDVARID_ADC10,
		"adc10",
	BCCMDVARID_ADC11,
		"adc11",
	BCCMDVARID_ADC12,
		"adc12",
	BCCMDVARID_ADC13,
		"adc13",
	BCCMDVARID_ADC14,
		"adc14",
	BCCMDVARID_ADC15,
		"adc15",
	BCCMDVARID_PSCLR,
		"psclr",
	BCCMDVARID_BUILDID,
		"buildid",
	BCCMDVARID_CHIPVER,
		"chipver",
	BCCMDVARID_CHIPREV,
		"chiprev",
	BCCMDVARID_MAP_SCO_PCM,
		"map_sco_pcm",
	BCCMDVARID_SPI_BUFSIZE,
		"spi_bufsize",
	BCCMDVARID_PIO_DIRECTION_MASK,
		"pio_direction_mask",
	BCCMDVARID_PIO,
		"pio",
	BCCMDVARID_PROVOKE_PANIC,
		"provoke_panic",
	BCCMDVARID_PROVOKE_DELAYED_PANIC,
		"provoke_delayed_panic",
	BCCMDVARID_PROVOKE_FAULT,
		"provoke_fault",
	BCCMDVARID_PIO_PROTECT_MASK,
		"pio_protect_mask",
	BCCMDVARID_INTERFACE_VERSION,
		"interface_version",
	BCCMDVARID_RADIOTEST_INTERFACE_VERSION,
		"radiotest_interface_version",
	BCCMDVARID_MAX_TX_POWER,
		"max_tx_power",
	BCCMDVARID_ADC,
		"adc",
	BCCMDVARID_ADC_READ,
		"adc_read",
	BCCMDVARID_RAND,
		"rand",
	BCCMDVARID_DEFAULT_TX_POWER,
		"default_tx_power",
	BCCMDVARID_BLE_DEFAULT_TX_POWER,
		"ble_default_tx_power",
	BCCMDVARID_MAXCRYPTKEYLEN,
		"maxcryptkeylen",
	BCCMDVARID_PSCLRALLS,
		"psclralls",
	BCCMDVARID_SINGLE_CHAN,
		"single_chan",
	BCCMDVARID_CHECK_ROM,
		"check_rom",
	BCCMDVARID_CHECK_RAM_RESULT,
		"check_ram_result",
	BCCMDVARID_PCM_ATTENUATION,
		"pcm_attenuation",
	BCCMDVARID_E2_APP_SIZE,
		"e2_app_size",
	BCCMDVARID_FIRMWARE_CHECK,
		"firmware_check",
	BCCMDVARID_FIRMWARE_CHECK_MASK,
		"firmware_check_mask",
	BCCMDVARID_CHIPANAVER,
		"chipanaver",
	BCCMDVARID_ANA_FTRIM,
		"ana_ftrim",
	BCCMDVARID_BUILDID_LOADER,
		"buildid_loader",
	BCCMDVARID_PIO_STRONG_BIAS,
		"pio_strong_bias",
	BCCMDVARID_BOOTMODE,
		"bootmode",
	BCCMDVARID_SCATTERNET_OVERRIDE_SCO,
		"scatternet_override_sco",
	BCCMDVARID_LOCK_ACL_PACKET_TYPE,
		"lock_acl_packet_type",
	BCCMDVARID_BABEL_SPI_DELAY,
		"babel_spi_delay",
	BCCMDVARID_BABEL_SPI_RUN_STATE,
		"babel_spi_run_state",
	BCCMDVARID_ANA_FTRIM_READWRITE,
		"ana_ftrim_readwrite",
	BCCMDVARID_SNIFF_MULTISLOT_ENABLE,
		"sniff_multislot_enable",
	BCCMDVARID_BT_CLOCK,
		"bt_clock",
	BCCMDVARID_DEEP_SLEEP_PERIOD,
		"deep_sleep_period",
	BCCMDVARID_CHECK_ROM_CRC32,
		"check_rom_crc32",
	BCCMDVARID_MEMORY,
		"memory",
	BCCMDVARID_BUFFER,
		"buffer",
	BCCMDVARID_PS,
		"ps",
	BCCMDVARID_RADIOTEST,
		"radiotest",
	BCCMDVARID_PSNEXT,
		"psnext",
	BCCMDVARID_PSSIZE,
		"pssize",
	BCCMDVARID_ADCRES,
		"adcres",
	BCCMDVARID_CRYPTKEYLEN,
		"cryptkeylen",
	BCCMDVARID_PICONET_INSTANT,
		"piconet_instant",
	BCCMDVARID_GET_CLR_EVT,
		"get_clr_evt",
	BCCMDVARID_GET_NEXT_BUILDDEF,
		"get_next_builddef",
	BCCMDVARID_PSCLRS,
		"psclrs",
	BCCMDVARID_I2C_TRANSFER,
		"i2c_transfer",
	BCCMDVARID_E2_DEVICE,
		"e2_device",
	BCCMDVARID_E2_APP_DATA,
		"e2_app_data",
	BCCMDVARID_ENABLE_AFH,
		"enable_afh",
	BCCMDVARID_BER_THRESHOLD,
		"ber_threshold",
	BCCMDVARID_PS_MEMORY_TYPE,
		"ps_memory_type",
	BCCMDVARID_PSNEXT_ALL,
		"psnext_all",
	BCCMDVARID_L2CAP_CRC,
		"l2cap_crc",
	BCCMDVARID_ENABLE_MEDIUM_RATE,
		"enable_medium_rate",
	BCCMDVARID_KALDEBUG,
		"kaldebug",
	BCCMDVARID_KALEXTADDRDEBUG,
		"kalextaddrdebug",
	BCCMDVARID_VM_DEBUG,
		"vm_debug",
	BCCMDVARID_REMOTE_SNIFF_LIMITS,
		"remote_sniff_limits",
	BCCMDVARID_BABEL_SPI_RAW,
		"babel_spi_raw",
	BCCMDVARID_BABEL_SPI_VERIFIED,
		"babel_spi_verified",
	BCCMDVARID_BUILD_NAME,
		"build_name",
	BCCMDVARID_RSSI_ACL,
		"rssi_acl",
	BCCMDVARID_ESCOTEST,
		"escotest",
	BCCMDVARID_BC3PSU,
		"bc3psu",
	BCCMDVARID_LIMIT_EDR_POWER,
		"limit_edr_power",
	BCCMDVARID_EXT_CLOCK_PERIOD,
		"get_external_clock_period",
	BCCMDVARID_LC_RX_FRACS,
		"get_rx_fracs",
	BCCMDVARID_LC_FRAC_COUNT,
		"get_fracs_count",
	BCCMDVARID_SNIFF_SUB_RATE,
		"sniff_sub_rate",
	BCCMDVARID_HOST_DECIDES_TIMING,
		"host_decides_timing",
	BCCMDVARID_HOSTIO_ENABLE_DEBUG,
		"hostio_enable_debug",
	BCCMDVARID_SAVE_CONNECTION_EVENT_FILTERS,
		"save_connection_event_filters",
	BCCMDVARID_CHARGER_PSU_TRIM,
		"charger_psu_trim",
	BCCMDVARID_PSU_SMPS_ENABLE,
		"psu_buck_reg",
	BCCMDVARID_PSU_HV_LINEAR_ENABLE,
		"psu_mic_en",
	BCCMDVARID_LED0_ENABLE,
		"led0",
	BCCMDVARID_LED1_ENABLE,
		"led1",
	BCCMDVARID_CHARGER_STATUS,
		"charger_status",
	BCCMDVARID_CHARGER_DISABLE,
		"charger_disable",
	BCCMDVARID_CHARGER_SUPPRESS_LED0,
		"charger_supress_led0",
	BCCMDVARID_COEX_SYNC_PERIOD,
		"coex_sync_period",
	BCCMDVARID_COEX_PRIORITY_PIOS_ON,
		"coex_priority_pios_on",
	BCCMDVARID_FM_AUDIO_ROUTING,
		"fm_audio_routing",
	BCCMDVARID_FM_REG,
		"fm_reg",
	BCCMDVARID_FM_RDS,
		"fm_rds",
	BCCMDVARID_PCM_CONFIG32,
		"pcm_config32",
	BCCMDVARID_PCM_LOW_JITTER,
		"pcm_low_jitter",
	BCCMDVARID_PCM_RATE_AND_ROUTE,
		"pcm_rate_and_route",
	BCCMDVARID_DIGITAL_AUDIO_RATE,
		"digital_audio_rate",
	BCCMDVARID_DIGITAL_AUDIO_CONFIG,
		"digital_audio_config",
	BCCMDVARID_SHA256_INIT,
		"sha256_init",
	BCCMDVARID_SHA256_DATA,
		"sha256_data",
	BCCMDVARID_SHA256_RES,
		"sha256_res",
	BCCMDVARID_COMBO_DOT11_ESCO_RTX_PRIORITY,
		"combo_dot11_esco_rtx_priority",
	BCCMDVARID_HQ_SCRAPING,
		"hq_scraping",
	BCCMDVARID_HQ_SCRAPING_LEN,
		"hq_scraping_len",
	BCCMDVARID_INQUIRY_PRIORITY,
		"inquiry_priority",
	BCCMDVARID_HQ_SCRAPING_ENABLE,
		"hq_scraping_enable",
	BCCMDVARID_PANIC_ON_FAULT,
	"panic_on_fault",
	BCCMDVARID_AUX_DAC,
		"aux_dac",
	BCCMDVARID_AUX_DAC_ENABLE,
		"aux_dac_enable",
	BCCMDVARID_WLAN_COEX_PRIORITY,
		"wlan_coex_priority",
	BCCMDVARID_COEX_INT_TEST,
		"wlan_coex_int_test",
        BCCMDVARID_COEX_DUMP,
                "coex_dump",
        BCCMDVARID_COEX_ENABLE,
                "coex_enable",
	BCCMDVARID_GPS_CONFIG,
		"gps_config",
	BCCMDVARID_GPS_START,
		"gps_start",
	BCCMDVARID_GPS_STOP,
		"gps_stop",
	BCCMDVARID_GPS_DSP_ENABLE,
		"gps_dsp_enable",
	BCCMDVARID_COASTER_START,
		"coaster_start",
	BCCMDVARID_COASTER_STOP,
		"coaster_stop",
	BCCMDVARID_COASTER_DEL_SV,
		"coaster_del_sv",
	BCCMDVARID_COASTER_INDICATIONS,
		"coaster_indications",
	BCCMDVARID_COASTER_ADD_SV,
		"coaster_add_sv",
	BCCMDVARID_COASTER_SV_STATE,
		"coaster_sv_state",
	BCCMDVARID_EGPS_PULSE_CONFIG,
		"egps_pulse_config",
	BCCMDVARID_EGPS_PULSE_INPUT_START,
		"egps_pulse_input_start",
	BCCMDVARID_EGPS_PULSE_OUTPUT_START,
		"egps_pulse_output_start",
	BCCMDVARID_EGPS_PULSE_STOP,
		"egps_pulse_stop",
	BCCMDVARID_INQUIRY_TX_POWER,
		"inquiry_tx_power",
	BCCMDVARID_FMTX_REG,
		"fmtx_reg",
	BCCMDVARID_FMTX_TEXT,
		"fmtx_text",
        BCCMDVARID_FMTX_SET_CDAC,
                "fmtx_set_cdac",
	BCCMDVARID_FASTPIPE_ENABLE,
		"fastpipe_enable",
	BCCMDVARID_FASTPIPE_CREATE,
		"fastpipe_create",
	BCCMDVARID_FASTPIPE_DESTROY,
		"fastpipe_destroy",
	BCCMDVARID_FASTPIPE_RESIZE,
		"fastpipe_resize",
	BCCMDVARID_VREF_CONSTANT,
		"vref_constant",
	BCCMDVARID_SCO_PARAMETERS,
		"sco_parameters",
	BCCMDVARID_CLKSW_CURRENT_CLOCKS,
		"clksw_current_clocks",
	BCCMDVARID_CLK_SKEW_COUNT,
		"clk_skew_count",
	BCCMDVARID_DEEP_SLEEP_TIME,
		"deep_sleep_time",
	BCCMDVARID_CODEC_INPUT_GAIN,
		"codec_input_gain",
	BCCMDVARID_CODEC_OUTPUT_GAIN,
		"codec_output_gain",
	BCCMDVARID_PCM2_ATTENUATION,
		"pcm2_attenuation",
	BCCMDVARID_AFH_CHANNEL_RELEASE_TIME,
		"afh_channel_release_time",
	BCCMDVARID_STREAM_GET_SOURCE,
		"stream_get_source",
	BCCMDVARID_STREAM_GET_SINK,
		"stream_get_sink",
	BCCMDVARID_STREAM_CLOSE_SOURCE,
		"stream_close_source",
	BCCMDVARID_STREAM_CLOSE_SINK,
		"stream_close_sink",
	BCCMDVARID_STREAM_CONFIGURE,
		"stream_configure",
	BCCMDVARID_STREAM_ALIAS_SINK,
		"stream_alias_sink",
	BCCMDVARID_STREAM_CONNECT,
		"stream_connect",
	BCCMDVARID_STREAM_TRANSFORM_DISCONNECT,
		"stream_transform_disconnect",
	BCCMDVARID_STREAM_TRANSACTION_START,
		"stream_transaction_start",
	BCCMDVARID_STREAM_TRANSACTION_COMPLETE,
		"stream_transaction_complete",
	BCCMDVARID_STREAM_TRANSACTION_CANCEL,
		"stream_transaction_cancel",
	BCCMDVARID_ALLOCATE_RAM_RESERVE,
		"allocate_ram_reserve",
	BCCMDVARID_RECLAIM_RAM_RESERVE,
		"reclaim_ram_reserve",
	BCCMDVARID_GSM_RX_MODE,
		"gsm_rx_mode",
	BCCMDVARID_GSM_RX_ARFCN,
		"gsm_rx_arfcn",
	BCCMDVARID_BLE_T_IFS,
		"ble_t_ifs",
	BCCMDVARID_STREAM_SYNC_SID,
		"stream_sync_sid",
	BCCMDVARID_KALSPIDEBUG_SETDEBUGMODE,
		"kalspidebug_setdebugmode",
        BCCMDVARID_CACHED_TEMPERATURE,
                "cached_temperature",
        BCCMDVARID_BLE_WHITELIST_FREE_SPACE,
                "ble_whitelist_free_space",
        BCCMDVARID_BLE_SCAN_BACKOFF,
                "ble_scan_backoff",
        BCCMDVARID_MAP_SCO_AUDIO,
                "map_sco_audio",
        BCCMDVARID_ENABLE_SCO_STREAMS,
                "enable_sco_streams",
        BCCMDVARID_PCM_CLOCK_RATE,
                "pcm_clock_rate",
        BCCMDVARID_PCM_SLOTS_PER_FRAME,
                 "pcm_slots_per_frame",
        BCCMDVARID_PCM_SYNC_RATE,
                "pcm_sync_rate",
        BCCMDVARID_ENABLE_COEX_MESSAGE_REPORTING,
                "enable_coex_message_reporting",
        BCCMDVARID_CLOCK_MODULATION_ENABLE,
                "clock_modulation_enable",
        BCCMDVARID_CLOCK_MODULATION_CONFIG,
                "clock_modulation_config",
        BCCMDVARID_PIO32_SET_PIN_FUNCTION,
                "pio32_set_pin_function",
        BCCMDVARID_CHARGER_TRIMS,
                "charger_trims",
		BCCMDVARID_PIO32_STRONG_BIAS,
			"pio32_strong_bias",
		BCCMDVARID_PIO32_DIRECTION_MASK,
			"pio32_direction_mask",
		BCCMDVARID_PIO32,
			"pio32",
		BCCMDVARID_PIO32_PROTECT_MASK,
			"pio32_protect_mask",
		BCCMDVARID_PIO32_MAP_PINS,
			"pio32_map_pins",
		BCCMDVARID_LED2_ENABLE,
			"led2",
        BCCMDVARID_MIC_BIAS_CONFIGURE,
            "mic_bias_configure",
        BCCMDVARID_MIC_BIAS_CTRL,
            "mic_bias_ctrl",
        BCCMDVARID_LED_CONFIG,
            "led_config",
        BCCMDVARID_BLE_ADV_FILT_ADD,
            "ble_adv_filt_add",
        BCCMDVARID_BLE_ADV_FILT_CLEAR,
            "ble_adv_filt_clear",
        BCCMDVARID_BLE_RADIO_TEST, 
            "ble_radio_test",
        BCCMDVARID_SIFLASH, 
            "siflash",
		BCCMDVARID_CAPSENSE_RT_PADS,
			"capsense_rt_pads",
        BCCMDVARID_REMOVE_DOWNLOADED_CAPABILITY,
            "remove_dl_cap",
        BCCMDVARID_START_OPERATOR,
            "start_operator",
        BCCMDVARID_STOP_OPERATOR,
            "stop_operator",
        BCCMDVARID_RESET_OPERATOR,
            "reset_operator",
        BCCMDVARID_DESTROY_OPERATOR,
            "destroy_operator",
        BCCMDVARID_OPERATOR_MESSAGE,
            "operator_message",
        BCCMDVARID_CREATE_OPERATOR_C,
            "create_operator_c",
        BCCMDVARID_CREATE_OPERATOR_P,
            "create_operator_p",
        BCCMDVARID_DOWNLOAD_CAPABILITY,
            "download_capability",
        BCCMDVARID_DSPMANAGER_DEBUG,
            "dspmanager_debug",
        BCCMDVARID_SPI_LOCK_INITIATE_LOCK,
            "spi_lock_stay_locked",
        BCCMDVARID_SPI_LOCK_CUSTOMER_KEY,
            "spi_lock_customer_key",
        BCCMDVARID_SPI_LOCK_STATUS,
            "spi_lock_status",
	0,
		NULL
};


#include "host/builddef/builddefs.h"

Dictionary builddef_d = /* Last checked against builddefs.h#27 */
{
	BUILDDEF_CHIP_BASE_BC01,
		"chip_base_bc01",
	BUILDDEF_CHIP_BASE_BC02,
		"chip_base_bc02",
	BUILDDEF_CHIP_BC01B,
		"chip_bc01b",
	BUILDDEF_CHIP_BC02_EXTERNAL,
		"chip_bc02_external",
	BUILDDEF_BUILD_HCI,
		"build_hci",
	BUILDDEF_BUILD_RFCOMM,
		"build_rfcomm",
	BUILDDEF_BT_VER_1_1,
		"bt_ver_1_1",
	BUILDDEF_TRANSPORT_ALL_HCI,
		"transport_all_hci",
	BUILDDEF_TRANSPORT_BCSP,
		"transport_bcsp",
	BUILDDEF_TRANSPORT_H4,
		"transport_h4",
	BUILDDEF_TRANSPORT_USB,
		"transport_usb",
	BUILDDEF_MAX_CRYPT_KEY_LEN_56,
		"max_crypt_key_len_56",
	BUILDDEF_MAX_CRYPT_KEY_LEN_128,
		"max_crypt_key_len_128",
	BUILDDEF_TRANSPORT_USER,
		"transport_user",
	BUILDDEF_CHIP_BC02_KATO,
		"chip_bc02_kato",
	BUILDDEF_TRANSPORT_NONE,
		"transport_none",
	BUILDDEF_REQUIRE_8MBIT,
		"require_8mbit",
	BUILDDEF_INSTALL_RADIOTEST_MODULE,
		"install_radiotest_module",
	BUILDDEF_RADIOTEST_LITE,
		"radiotest_lite",
	BUILDDEF_INSTALL_FLASH,
		"install_flash",
	BUILDDEF_INSTALL_EEPROM,
		"install_eeprom",
	BUILDDEF_INSTALL_COMBO_DOT11,
		"install_combo_dot11",
	BUILDDEF_LOWPOWER_TX,
		"lowpower_tx",
	BUILDDEF_TRANSPORT_TWUTL,
		"transport_twutl",
	BUILDDEF_TRANSPORT_H5,
		"transport_h5",
	BUILDDEF_COMPILER_GCC,
		"compiler_gcc",
	BUILDDEF_CHIP_BC02_CLOUSEAU,
		"chip_bc02_clouseau",
	BUILDDEF_CHIP_BC02_TOULOUSE,
		"chip_bc02_toulouse",
	BUILDDEF_CHIP_BASE_BC3,
		"chip_base_bc3",
	BUILDDEF_CHIP_BC3_NICKNACK,
		"chip_bc3_nicknack",
	BUILDDEF_CHIP_BC3_KALIMBA,
		"chip_bc3_kalimba",
	BUILDDEF_INSTALL_HCI_MODULE,
		"install_hci_module",
	BUILDDEF_INSTALL_L2CAP_MODULE,
		"install_l2cap_module",
	BUILDDEF_INSTALL_DM_MODULE,
		"install_dm_module",
	BUILDDEF_INSTALL_SDP_MODULE,
		"install_sdp_module",
	BUILDDEF_INSTALL_RFCOMM_MODULE,
		"install_rfcomm_module",
	BUILDDEF_INSTALL_HIDIO_MODULE,
		"install_hidio_module",
	BUILDDEF_INSTALL_PAN_MODULE,
		"install_pan_module",
	BUILDDEF_INSTALL_IPV4_MODULE,
		"install_ipv4_module",
	BUILDDEF_INSTALL_IPV6_MODULE,
		"install_ipv6_module",
	BUILDDEF_INSTALL_TCP_MODULE,
		"install_tcp_module",
	BUILDDEF_BT_VER_1_2,
		"bt_ver_1_2",
	BUILDDEF_INSTALL_UDP_MODULE,
		"install_udp_module",
	BUILDDEF_REQUIRE_0_WAIT_STATES,
		"require_0_wait_states",
	BUILDDEF_CHIP_BC3_PADDYWACK,
		"chip_bc3_paddywack",
	BUILDDEF_CHIP_BC4_COYOTE,
		"chip_bc4_coyote",
	BUILDDEF_CHIP_BC4_ODDJOB,
		"chip_bc4_oddjob",
	BUILDDEF_TRANSPORT_H4DS,
		"transport_h4ds",
	BUILDDEF_CHIP_BASE_BC4,
		"chip_base_bc4",
	0,
		NULL
};


Dictionary evtCntID_d = /* Last checked against bccmdpdu.h#60 */
{
	BCCMDPDU_GCE_NONE,
		"none",
	BCCMDPDU_GCE_TXACL,
		"txacl",
	BCCMDPDU_GCE_RXACL,
		"rxacl",
	BCCMDPDU_GCE_TXSCO,
		"txsco",
	BCCMDPDU_GCE_RXSCO,
		"rxsco",
	BCCMDPDU_GCE_TXHOST,
		"txhost",
	BCCMDPDU_GCE_RXHOST,
		"rxhost",
	BCCMDPDU_GCE_RADIOACTIVE,
		"radioactive",
	BCCMDPDU_GCE_PIODEBOUNCE,
		"piodebounce",
	BCCMDPDU_GCE_USRADC,
		"usradc",
	BCCMDPDU_GCE_BCSTAT,
		"bcstat",
	BCCMDPDU_GCE_DEEPSLEEP,
		"deepsleep",
	0,
		NULL
};


#include "host/bccmd/bccmdradiotestpdu.h"

Dictionary radiotest_d = /* Last checked against bccmdradiotestpdu.h#14 */
{
	RADIOTEST_PAUSE,
		"pause",
	RADIOTEST_TXSTART,
		"txstart",
	RADIOTEST_RXSTART1,
		"rxstart1",
	RADIOTEST_RXSTART2,
		"rxstart2",
	RADIOTEST_TXDATA1,
		"txdata1",
	RADIOTEST_TXDATA2,
		"txdata2",
	RADIOTEST_TXDATA3,
		"txdata3",
	RADIOTEST_TXDATA4,
		"txdata4",
	RADIOTEST_RXDATA1,
		"rxdata1",
	RADIOTEST_RXDATA2,
		"rxdata2",
	RADIOTEST_DEEP_SLEEP,
		"deep_sleep",
	RADIOTEST_PCM_LB,
		"pcm_lb",
	RADIOTEST_SETTLE,
		"settle",
	RADIOTEST_IF_RESP,
		"if_resp",
	RADIOTEST_VCOTRIM,
		"vcotrim",
	RADIOTEST_RF_IQ_MATCH,
		"rf_iq_match",
	RADIOTEST_IF_IQ_MATCH,
		"if_iq_match",
	RADIOTEST_BUILD_LUT,
		"build_lut",
	RADIOTEST_READ_LUT,
		"read_lut",
	RADIOTEST_BER1,
		"ber1",
	RADIOTEST_BER2,
		"ber2",
	RADIOTEST_LOOP_BACK,
		"loop_back",
	RADIOTEST_CFG_FREQ,
		"cfg_freq",
	RADIOTEST_CFG_PKT,
		"cfg_pkt",
	RADIOTEST_CFG_BIT_ERR,
		"cfg_bit_err",
	RADIOTEST_RX_LOOP_BACK,
		"rx_loop_back",
	RADIOTEST_BER_LOOP_BACK,
		"ber_loop_back",
	RADIOTEST_CONFIG_TX_IF,
		"config_tx_if",
	RADIOTEST_PCM_EXT_LB,
		"pcm_ext_lb",
	RADIOTEST_CFG_XTAL_FTRIM,
		"cfg_xtal_ftrim",
	RADIOTEST_CFG_UAP_LAP,
		"cfg_uap_lap",
	RADIOTEST_CFG_ACC_ERRS,
		"cfg_acc_errs",
	RADIOTEST_CFG_IQ_TRIM,
		"cfg_iq_trim",
	RADIOTEST_CFG_TX_TRIM,
		"cfg_tx_trim",
	RADIOTEST_RADIO_STATUS,
		"radio_status",
	RADIOTEST_CFG_LO_LVL,
		"cfg_lo_lvl",
	RADIOTEST_CFG_TX_COMP,
		"cfg_tx_comp",
	RADIOTEST_CFG_SETTLE,
		"cfg_settle",
	RADIOTEST_SETTLE_RPT,
		"settle_rpt",
	RADIOTEST_RADIO_STATUS_ARRAY,
		"radio_status_array",
	RADIOTEST_CFG_TX_RF_TRIM,
		"cfg_tx_rf_trim",
	RADIOTEST_PCM_TONE,
		"pcm_tone",
	RADIOTEST_CTS_RTS_LB,
		"cts_rts_lb",
	RADIOTEST_PCM_TIMING_IN,
		"pcm_timing_in",
	RADIOTEST_DEEP_SLEEP_SELECT,
		"deep_sleep_select",
	RADIOTEST_CFG_HOPPING_SEQUENCE,
		"cfg_hopping_sequence",
	RADIOTEST_CFG_PIO_CHANNEL_MASK,
		"cfg_pio_channel_mask",
	RADIOTEST_CFG_PIO_ENABLE_MASK,
		"cfg_pio_enable_mask",
	RADIOTEST_CODEC_STEREO_LB,
		"codec_stereo_lb",
	RADIOTEST_PCM_TONE_STEREO,
		"pcm_tone_stereo",
	RADIOTEST_CFG_TXPOWER,
		"cfg_tx_power",
	RADIOTEST_PCM_TONE_INTERFACE,
	    "pcm_tone_if",
    RADIOTEST_PCM_LB_INTERFACE,
	    "pcm_lb_if",
    RADIOTEST_PCM_EXT_LB_INTERFACE,
	    "pcm_ext_lb_if",
    RADIOTEST_PCM_TIMING_IN_INTERFACE,
	    "pcm_timing_in_if",
	0,
		NULL
};


Dictionary hqStatus_d = /* Last checked against hqpdu.h#7 */
{
	0,		"ok",
	1,		"no_such_varid",
	2,		"too_big",
	3,		"no_value",
	4,		"bad_pdu",
	5,		"no_access",
	6,		"read_only",
	7,		"write_only",
	8,		"error",
	9,		"permission_denied",
	0,		NULL
};


#include "host/hq/hqpdu.h"

Dictionary hqVarID_d = /* Last checked against hqpdu.h#10 */
{
	HQVARID_NO_VARIABLE,
		"no_variable",
	HQVARID_BOOTED,
		"booted",
	HQVARID_DELAYED_PANIC,
		"delayed_panic",
	HQVARID_RSSI,
		"rssi",
	HQVARID_FAULT,
		"fault",
	HQVARID_RXPKTSTATS,
		"rxpktstats",
	HQVARID_LUT_ENTRY,
		"lut_entry",
	HQVARID_SYNTH_SETTLE,
		"synth_settle",
	HQVARID_FILTER_RESP,
		"filter_resp",
	HQVARID_IQ_MATCH,
		"iq_match",
	HQVARID_BIT_ERR,
		"bit_err",
	HQVARID_RADIO_STATUS,
		"radio_status",
	HQVARID_RADIO_STATUS_ARRAY,
		"radio_status_array",
	HQVARID_PROTOCOL_ERROR,
		"protocol_error",
	HQVARID_BER_TRIGGER,
		"ber_trigger",
	HQVARID_DRAIN_CALIBRATE,
		"drain_calibrate",
	HQVARID_FM_EVENT,
		"fm_event",
	HQVARID_FM_RDS_DATA,
		"rds_data",
	HQVARID_BLE_RADIO_TEST,
		"ble_radio_test_packet_count",
        HQVARID_SCO_STREAM_HANDLES,
                "sco_handles",
        HQVARID_CAPABILITY_DOWNLOAD_COMPLETE,
                "cap_dl_complete",
        HQVARID_CAPABILITY_DOWNLOAD_FAILED,
                "cap_dl_failed",
        HQVARID_MESSAGE_FROM_OPERATOR,
                "message_from_operator",
        HQVARID_DSPMANAGER_DEBUG_INFO_B,
                "dspmgr_debug_b",
	0,
		NULL
};


#include "psbcallkeys.h"
#include "dictionary_ps.c" /* This pulls in

Dictionary psKey_d =
{
	...,
		...,
	0,
		NULL
};

*/


#include "dictionary_id.c" /* This pulls in

Dictionary id_d =
{
	...,
		...,
	0,
		NULL
};

*/


Dictionary lmpDebug_d =
{
	HCI_DEBUG_EV_FSM,				"lm_fsm",
	HCI_DEBUG_EV_STATS,				"lm_stats",
	HCI_DEBUG_EV_ACL,				"lm_acl",
	HCI_DEBUG_EV_SCO,				"lm_sco",
	HCI_DEBUG_EV_ESCO,				"lm_esco",
	HCI_DEBUG_EV_MEM,				"lm_mem",
	HCI_DEBUG_EV_RAND,				"lm_rand",
	HCI_DEBUG_EV_KEY,				"lm_key",
	HCI_DEBUG_EV_SRES,				"lm_sres",
	HCI_DEBUG_EV_PARKED,			"lm_parked",
	HCI_DEBUG_EV_RX_LMP,			"lm_rx",
	HCI_DEBUG_EV_TX_LMP,			"lm_tx",
	HCI_DEBUG_EV_ACO,				"lm_aco",
	HCI_DEBUG_EV_ACL_HANDLE,		"lm_acl_handle",
	HCI_DEBUG_EV_ULP_ADV_CHANNEL,	"ll_adv",
	HCI_DEBUG_EV_ULP_ACL_MASTER,	"ll_m",
	HCI_DEBUG_EV_ULP_ACL_SLAVE,	    "ll_s",
	HCI_DEBUG_EV_TID_FIX,			"lm_tid_fix",
	HCI_DEBUG_EV_PKT_TYPES,			"lm_pkt_types",
	HCI_DEBUG_EV_SLAVE_MAX_SLOTS,	"lm_slave_max_slots",
	HCI_DEBUG_EV_TO_START,			"lm_timeout_start",
	HCI_DEBUG_EV_TO_CANCEL,			"lm_timeout_cancel",
	HCI_DEBUG_EV_TO,				"lm_timeout",
	HCI_DEBUG_EV_BDW_TO,			"lm_bdw_timeout",
	HCI_DEBUG_EV_DETACH_TO,			"lm_detach_timeout",
	HCI_DEBUG_EV_TO_LOST,			"lm_timeout_lost",
	HCI_DEBUG_EV_RX_LMP_DUPLICATE,	"lm_rx_duplicate",
	HCI_DEBUG_EV_SNIFF_STATS,		"lm_sniff_stats",
	HCI_DEBUG_EV_SLOTS,				"lm_slots",
	HCI_DEBUG_EV_INSTANT,			"lm_instant",
	HCI_DEBUG_EV_CONVERT_DSCO,		"lm_conv_dsco",
	HCI_DEBUG_EV_SLOW_CLOCK,		"lm_slow_clock",
	HCI_DEBUG_EV_UNPARK,			"lm_unpark",
	HCI_DEBUG_EV_CQDDR,				"lm_cqddr",
	HCI_DEBUG_EV_DRAIN,				"drain",
	HCI_DEBUG_EV_DOT11_CHANNEL,		"dot11_channel",
	HCI_DEBUG_EV_ECC_PRIVATE,		"lm_ecc_private",
	HCI_DEBUG_EV_ECC_PUBLIC_X,		"lm_ecc_public_x",
	HCI_DEBUG_EV_ECC_PUBLIC_Y,		"lm_ecc_public_y",
	HCI_DEBUG_EV_ECC_RESULT,		"lm_ecc_result",
	HCI_DEBUG_EV_U,					"lm_sp_u",
	HCI_DEBUG_EV_V,					"lm_sp_v",
	HCI_DEBUG_EV_X,					"lm_sp_x",
	HCI_DEBUG_EV_F1,				"lm_sp_f1",
	HCI_DEBUG_EV_Y,					"lm_sp_y",
	HCI_DEBUG_EV_W,					"lm_sp_w",
	HCI_DEBUG_EV_N,					"lm_sp_n",
	HCI_DEBUG_EV_KEYA1A2,			"lm_sp_keya1a2",
	HCI_DEBUG_EV_F,					"lm_sp_f",
	HCI_DEBUG_EV_IOCAP1A2,			"lm_sp_ioa1a2",
	HCI_DEBUG_EV_R,					"lm_sp_r",
	HCI_DEBUG_EV_ULP_INST_UPDATED,		"lm_ulp_instant_updated",
	0,					NULL
};

Dictionary ulpAdvDebug_d =
{
	HCI_DEBUG_EV_ULP_ADV_CHAN_CONN_REQ_PT1, "rx_connect_req_1",
	HCI_DEBUG_EV_ULP_ADV_CHAN_CONN_REQ_PT2, "rx_connect_req_2",
	HCI_DEBUG_EV_ULP_ADV_CHAN_CONN_REQ_PT1 | HCI_DEBUG_EV_ULP_ADV_CHAN_TX, "tx_connect_req_1",
	HCI_DEBUG_EV_ULP_ADV_CHAN_CONN_REQ_PT2 | HCI_DEBUG_EV_ULP_ADV_CHAN_TX, "tx_connect_req_2",
	0, NULL
};

Dictionary lmp_d =
{
	1,		"name_req",
	2,		"name_res",
	3,		"accepted",
	4,		"not_accepted",
	5,		"clkoffset_req",
	6,		"clkoffset_res",
	7,		"detach",
	8,		"in_rand",
	9,		"comb_key",
	10,		"unit_key",
	11,		"au_rand",
	12,		"sres",
	13,		"temp_rand",
	14,		"temp_key",
	15,		"encryption_mode_req",
	16,		"encryption_key_size_req",
	17,		"start_encryption_req",
	18,		"stop_encryption_req",
	19,		"switch_req",
	20,		"hold",
	21,		"hold_req",
	22,		"sniff",
	23,		"sniff_req",
	24,		"unsniff_req",
	25,		"park_req",
	26,		"park",
	27,		"set_broadcast_scan_window",
	28,		"modify_beacon",
	29,		"unpark_bd_addr_req",
	30,		"unpark_pm_addr_req",
	31,		"incr_power_req",
	32,		"decr_power_req",
	33,		"max_power",
	34,		"min_power",
	35,		"auto_rate",
	36,		"preferred_rate",
	37,		"version_req",
	38,		"version_res",
	39,		"features_req",
	40,		"features_res",
	41,		"quality_of_service",
	42,		"quality_of_service_req",
	43,		"sco_link_req",
	44,		"remove_sco_link_req",
	45,		"max_slot",
	46,		"max_slot_req",
	47,		"timing_accuracy_req",
	48,		"timing_accuracy_res",
	49,		"setup_complete",
	50,		"use_semi_permanent_key",
	51,		"host_connection_req",
	52,		"slot_offset",
	53,		"page_mode_req",
	54,		"page_scan_mode_req",
	55,		"supervision_timeout",
	56,		"test_activate",
	57,		"test_control",
	58,		"encryption_key_size_mask_req",
	59,		"encryption_key_size_mask_res",
	60,		"set_afh",
	61,		"encapsulated_header",
	62,		"encapsulated_payload",
	63,		"simple_pairing_confirm",
	64,		"simple_pairing_number",
	65,		"dhkey_check",
	0x7f01,		"accepted_ext",
	0x7f02,		"not_accepted_ext",
	0x7f03,		"features_req_ext",
	0x7f04,		"features_res_ext",
	0x7f05,		"scatter_req",
	0x7f06,		"unscatter_req",
	0x7f07,		"set_subrate",
	0x7f08,		"scatter_algorithms_req",
	0x7f09,		"scatter_algorithms_res",
	0x7f0a,		"pp_extension_req",
	0x7f0b,		"packet_type_table_req",
	0x7f0c,		"esco_link_req",
	0x7f0d,		"remove_esco_link_req",
	0x7f0e,		"absence_notify",
	0x7f0f,		"max_absences",
	0x7f10,		"channel_classification_req",
	0x7f11,		"channel_classification",
	0x7f12,		"alias_address",
	0x7f13,		"active_address",
	0x7f14,		"fixed_address",
	0x7f15,		"sniff_sub_rate_req",
	0x7f16,		"sniff_sub_rate_res",
	0x7f17,		"pause_encryption_req",
	0x7f18,		"resume_encryption_req",
	0x7f19,		"io_capability_req",
	0x7f1a,		"io_capability_res",
	0x7f1b,		"numeric_comparision_failed",
	0x7f1c,		"passkey_entry_failed",
	0x7f1d,		"oob_failed",
	0x7f1e,		"keypress_notification",
	0x7f1f,		"power_control_req",
	0x7f20,		"power_control_res",
	/* These are here for LMP timeout debug */
	0x017f,		"accepted_ext",
	0x027f,		"not_accepted_ext",
	0x037f,		"features_req_ext",
	0x047f,		"features_res_ext",
	0x057f,		"scatter_req",
	0x067f,		"unscatter_req",
	0x077f,		"set_subrate",
	0x087f,		"scatter_algorithms_req",
	0x097f,		"scatter_algorithms_res",
	0x0a7f,		"pp_extension_req",
	0x0b7f,		"packet_type_table_req",
	0x0c7f,		"esco_link_req",
	0x0d7f,		"remove_esco_link_req",
	0x0e7f,		"absence_notify",
	0x0f7f,		"max_absences",
	0x107f,		"channel_classification_req",
	0x117f,		"channel_classification",
	0x127f,		"alias_address",
	0x137f,		"active_address",
	0x147f,		"fixed_address",
	0x157f,		"sniff_sub_rate_req",
	0x167f,		"sniff_sub_rate_res",
	0x177f,		"pause_encryption_req",
	0x187f,		"resume_encryption_req",
	0x197f,		"io_capability_req",
	0x1a7f,		"io_capability_res",
	0x1b7f,		"numeric_comparision_failed",
	0x1c7f,		"passkey_entry_failed",
	0x1d7f,		"oob_failed",
	0x1e7f,		"keypress_notification",
	0,		NULL
};

Dictionary ulp_llc_d =
{
	0,		"connection_update_req",
	1,		"channel_map_req",
	2,		"terminate_ind",
	3,		"ll_enc_req",
	4,		"ll_enc_rsp",
	5,		"ll_start_enc_req",
	6,		"ll_start_enc_rsp",
	7,		"unknown_rsp",
	8,		"feature_req",
	9,		"feature_rsp",
	10,		"ll_pause_enc_req",
	11,		"ll_pause_enc_rsp",
	12,		"version_ind",
	13,		"ll_reject_ind",
	0,		NULL
};


Dictionary testScenario_d =
{
	PAUSE_TEST_MODE,"p",
	PAUSE_TEST_MODE,"ptm",
	1,		"tt0",
	1,		"tt0p",
	2,		"tt1",
	2,		"tt1p",
	3,		"tt1010",
	3,		"tt1010p",
	4,		"prbs",
	4,		"pbs",
	5,		"clba",
	5,		"clbap",
	6,		"clbs",
	6,		"clbsp",
	7,		"aww",
	7,		"apww",
	8,		"sww",
	8,		"spww",
	9,		"tt11110000",
	9,		"tt11110000p",
	EXIT_TEST_MODE,	"etm",
	EXIT_TEST_MODE,	"e",
	0,		NULL
};


Dictionary hoppingMode_d =
{
	0,		"sf",
	0,		"n",
	0,		"rtosf",
	1,		"eu",
	1,		"heu",
	2,		"j",
	2,		"hj",
	3,		"f",
	3,		"hf",
	4,		"s",
	4,		"hs",
	5,		"r",
	5,		"rh",
	0,		NULL
};


Dictionary powerControlMode_d =
{
	0,		"f",
	0,		"ftop",
	1,		"a",
	1,		"apc",
	0,		NULL
};


Dictionary ptt_d =
{
	0,		"basic_rate",
	1,		"medium_rate",
	0,		NULL
};


Dictionary basebandPacketType_d =
{
	0,		"null",
	0,		"n",
	1,		"poll",
	1,		"p",
	2,		"fhs",
	2,		"f",
	3,		"dm1",
	4,		"dh1",
	5,		"hv1",
	6,		"hv2",
	7,		"hv3/ev3",
	7,		"hv3",
	7,		"ev3",
	8,		"dv",
	9,		"aux1",
	9,		"a",
	10,		"dm3",
	11,		"dh3",
	12,		"ev4",
	13,		"ev5",
	14,		"dm5",
	15,		"dh5",
	0,		NULL
};

/* Packet types for use with the ltc command */
Dictionary testPacketType_d =
{
	0x00,		"null",
        0x00,		"n",
        0x10,		"esco-null",
        0x10,           "en",
        0x20,		"edr-acl-null",
        0x20,		"edr-null",
        0x20,		"ean",
        0x30,		"edr-esco-null",
        0x30,		"een",
        0x01,		"poll",
        0x01,           "p",
        0x11,		"esco-poll",
        0x11,		"ep",
        0x21,		"edr-acl-poll",
        0x21,		"edr-poll",
        0x21,		"eap",
        0x31,		"edr-esco-poll",
        0x31,		"eep",
        0x02,		"fhs",
        0x02,		"f",
        0x22,		"edr-fhs",
        0x22,		"ef",
	0x03,		"dm1",
        0x23,		"edr-dm1",
        0x23,		"edm1",
	0x04,		"dh1",
	0x0a,		"dm3",
	0x0b,		"dh3",
	0x0e,		"dm5",
	0x0f,		"dh5",
        0x24,		"2-dh1",
	0x24,		"2dh1",
        0x28,		"3-dh1",
	0x28,		"3dh1",
        0x2a,		"2-dh3",
	0x2a,		"2dh3",
        0x2b,		"3-dh3",
	0x2b,		"3dh3",
        0x2e,		"2-dh5",
	0x2e,		"2dh5",
        0x2f,		"3-dh5",
	0x2f,		"3dh5",
	0x05,		"hv1",
	0x05,		"h1",
	0x06,		"hv2",
	0x06,		"h2",
	0x07,		"hv3",
	0x07,		"h3",
        0x08,		"dv",
        0x17,		"ev3",
        0x17,		"e3",
        0x1c,		"ev4",
        0x1c,		"e4",
        0x1d,		"ev5",
        0x1d,		"e5",
        0x36,		"2-ev3",
        0x36,		"2ev3",
        0x37,		"3-ev3",
        0x37,		"3ev3",
        0x3c,		"2-ev5",
        0x3c,		"2ev5",
        0x3d,		"3-ev5",
        0x3d,		"3ev5",
        0x09,		"aux1",
        0x09,		"a",
        0x29,		"edr-aux1",
        0x29,		"ea",
	0,		NULL
};


Dictionary fsm_d =
{
	0,		"slave",
	1,		"master",
	2,		"device",
	3,		"power",
	4,		"init_auth",
	5,		"resp_auth",
	6,		"bandwidth",
	7,		"mlk",
	8,		"slave_enc",
	9,		"master_enc",
	10,		"rnr",
	11,		"afh",
	13,		"ble_master_init",
	14,		"ble_master_resp",
	15,		"ble_slave_init",
	16,		"ble_slave_resp",
	0,		NULL
};


Dictionary scoPacket_d =
{
	0,		"hv1",
	1,		"hv2",
	2,		"hv3",
	0,		NULL
};


Dictionary escoPacket_d =
{
	0x00,		"p/n",
	0x00,		"poll",
	0x00,		"null",
	0x07,		"ev3",
	0x0c,		"ev4",
	0x0d,		"ev5",
	0x26,		"2ev3",
	0x26,		"2-ev3",
	0x2c,		"2ev5",
	0x2c,		"2-ev5",
	0x37,		"3ev3",
	0x37,		"3-ev3",
	0x3d,		"3ev5",
	0x3d,		"3-ev5",
	0,		NULL
};


Dictionary airMode_d =
{
	0,		"u",
	1,		"a",
	2,		"c",
	3,		"t",
	0,		NULL
};

Dictionary tcf_d =
{
	0,		"~tc|i1|~naw",
	1,		"tc|i1|~naw",
	2,		"~tc|i2|~naw",
	3,		"tc|i2|~naw",
	4,		"~tc|i1|naw",
	5,		"tc|i1|naw",
	6,		"~tc|i2|naw",
	7,		"tc|i2|naw",
	0,		NULL
};


Dictionary i_d =
{
	0,		"i1",
	2,		"i2",
	0,		NULL
};


Dictionary transport_d =
{
	TRANSPORT_NONE,		"none",
	TRANSPORT_NONE,		"no",
	TRANSPORT_BCSP,		"bcsp",
	TRANSPORT_USB,		"usb",
	TRANSPORT_H4,		"h4",
	TRANSPORT_H4,		"uart",
	TRANSPORT_USER,		"raw",
	TRANSPORT_USER,		"vm",
	TRANSPORT_USER,		"user",
	TRANSPORT_USER,		"usr",
	TRANSPORT_H5,		"h5",
	TRANSPORT_H5,		"twu",
	TRANSPORT_H5,		"twutl",
	TRANSPORT_H4DS,		"h4ds",
	0,			NULL
};


Dictionary faultID_d =
{
#define FAULT_IFACE_X(enum, hexv, sdesc, ldesc) hexv, sdesc
#define FAULT_IFACE_SEP  , 
#include "host/hq/fault_codes.h"
#undef FAULT_IFACE_X
#undef FAULT_IFACE_SEP
,
	0,		NULL
};



Dictionary badPDUreason_d = /* Last checked against pduapi.h#4 */
{
	0,		"hci_wrong_size",
	1,		"pc_too_small",
	2,		"no_such_varid",
	3,		"no_such_event_code",
	4,		"no_such_command",
	5,		"out_of_data",
	6,		"warning_unknown_format",
	7,		"hci_data_reserved_pbf",
	8,		"hci_data_wrong_size",
	9,		"hci_data_reserved_bcf",
	10,		"bp_wrong_type",
	11,		"unhandled_channel",
	0,		NULL
};


Dictionary stores_d =
{
	PS_STORES_DEFAULT, "d",
	PS_STORES_I,	"i",
	PS_STORES_F,	"f",
	PS_STORES_IF,	"if",
	PS_STORES_R,	"r",
	PS_STORES_IFR,	"ifr",
	PS_STORES_T,	"t",
	PS_STORES_TI,	"ti",
	PS_STORES_TIF,	"tif",
	PS_STORES_TIFR,	"tifr",
	0,		NULL
};


Dictionary storesSet_d =
{
	PS_STORES_DEFAULT, "d",
	PS_STORES_I,	"i",
	PS_STORES_F,	"f",
	PS_STORES_T,	"t",
	0,		NULL
};


Dictionary storesClr_d =
{
	PS_STORES_DEFAULT, "d",
	PS_STORES_I,	"i",
	PS_STORES_F,	"f",
	PS_STORES_IF,	"if",
	PS_STORES_T,	"t",
	PS_STORES_TI,	"ti",
	PS_STORES_TIF,	"tif",
	0,		NULL
};


Dictionary pattern_d =
{
	P0,		"0",
	P1,		"1",
	P10,		"10",
	P10,		"1010",
	P11110000,	"11110000",
	PRBS,		"prbs",
	PRBS,		"prbs9",
	0,		NULL
};


Dictionary unpark_d =
{
	1,		"unpark_pm",
	2,		"unpark_pm_error",
	3,		"unpark_pm_not_us",
	4,		"unpark_bd",
	5,		"unpark_bd_us_error",
	6,		"unpark_bd_not_us",
	0x0f,		"unpark",
	0x10,		"unpark_pm_failed",
	0x11,		"unpark_pm_success",
	0x12,		"unpark_bd_failed",
	0x13,		"unpark_bd_success",
	0x14,		"unpark_failed",
	0x15,		"unpark_success",
	0,		NULL
};


Dictionary l2cap_reason_d =
{
	0,		"command_not_understood",
	1,		"signalling_mtu_exceeded",
	2,		"invlid_cid_in_request",
	0,		NULL
};

Dictionary l2cap_psm_value_d =
{
	0x01,		"sdp",
	0x03,		"rfcomm",
	0x05,		"tcs-bin",
	0x07,		"tcs-bin_cordless",
	0x0f,		"bnep",
	0x11,		"hid_control",
	0x13,		"hid_interrupt",
	0x15,		"upnp",
	0x17,		"avctp",
	0x19,		"avdtp",
	0,		NULL
};

Dictionary l2cap_result_value_d =
{
	0,		"successful",
	1,		"pending",
	2,		"refused:_psm_not_supported",
	3,		"refused:_security_block",
	4,		"refused:_no_resources",
	0,		NULL
};

Dictionary l2cap_status_value_d =
{
	0,		"no_further_information_available",
	1,		"authentication_pending",
	2,		"authorization_pending",
	0,		NULL
};

Dictionary l2cap_config_result_value_d =
{
	0,		"success",
	1,		"failure:_unacceptable_parameters",
	2,		"failure:_rejected_without_reason",
	3,		"failure:_unknown_options",
	0,		NULL
};

Dictionary l2cap_config_type_value_d =
{
	0x01,		"mtu",
	0x02,		"flush_timeout",
	0x03,		"qos",
	0,		NULL
};

Dictionary l2cap_infotype_value_d =
{
	1,		"connectionless_mtu",
	0,		NULL
};

Dictionary l2cap_info_result_value_d =
{
	0,		"success",
	1,		"not_supported",
	0,		NULL
};

Dictionary l2cap_config_qos_service_type_value_d =
{
	0,		"no_traffic",
	1,		"best_effort",
	2,		"guaranteed",
	0,		NULL
};


Dictionary sdp_uuid_d =
{
	0x0001,		"sdp",
	0x0002,		"udp",
	0x0003,		"rfcomm",
	0x0004,		"tcp",
	0x0005,		"tcs-bin",
	0x0006,		"tcs-at",
	0x0008,		"obex",
	0x0009,		"ip",
	0x000a,		"ftp",
	0x000c,		"http",
	0x000e,		"wsp",
	0x000f,		"bnep",
	0x0010,		"upnp",
	0x0011,		"hidp",
	0x0012,		"hardcopycontrolchannel",
	0x0014,		"hardcopydatachannel",
	0x0016,		"hardcopynotification",
	0x0017,		"avctp",
	0x0019,		"avdtp",
	0x001b,		"cmpt",
	0x001d,		"udi_c-plane",
	0x0100,		"l2cap",
	0x1000,		"servicediscoveryserverserviceclassid",
	0x1001,		"browsegroupdescriptorserviceclassid",
	0x1002,		"publicbrowsegroup",
	0x1101,		"serialport",
	0x1102,		"lanaccessusingppp",
	0x1103,		"dialupnetworking",
	0x1104,		"irmcsync",
	0x1105,		"obexobjectpush",
	0x1106,		"obexfiletransfer",
	0x1107,		"irmcsynccommand",
	0x1108,		"headset",
	0x1109,		"cordlesstelephony",
	0x110a,		"audiosource",
	0x110b,		"audiosink",
	0x110c,		"a/v_remotecontroltarget",
	0x110d,		"advancedaudiodistribution",
	0x110e,		"a/v_remotecontrol",
	0x110f,		"videoconferencing",
	0x1110,		"intercom",
	0x1111,		"fax",
	0x1112,		"headsetaudiogateway",
	0x1113,		"wap",
	0x1114,		"wap_client",
	0x1115,		"panu",
	0x1116,		"nap",
	0x1117,		"gn",
	0x1118,		"directprinting",
	0x1119,		"referenceprinting",
	0x111a,		"imaging",
	0x111b,		"imagingresponder",
	0x111c,		"imagingautomaticarchive",
	0x111d,		"imagingreferencedobjects",
	0x111e,		"handsfree",
	0x111f,		"handsfreeaudiogateway",
	0x1120,		"directprintingreferenceobjectsservice",
	0x1121,		"reflectedui",
	0x1122,		"basicprinting",
	0x1123,		"printingstatus",
	0x1124,		"humaninterfacedeviceservice",
	0x1125,		"hardcopycablereplacement",
	0x1126,		"hcr_print",
	0x1127,		"hcr_scan",
	0x1128,		"common_isdn_access",
	0x1129,		"videoconferencinggw",
	0x112a,		"udi_mt",
	0x112b,		"udi_ta",
	0x112c,		"audio/video",
	0x112d,		"sim_access",
	0x1200,		"pnpinformation",
	0x1201,		"genericnetworking",
	0x1202,		"genericfiletransfer",
	0x1203,		"genericaudio",
	0x1204,		"generictelephony",
	0x1205,		"upnp_service",
	0x1206,		"upnp_ip_service",
	0x1300,		"esdp_upnp_ip_pan",
	0x1301,		"esdp_upnp_ip_lap",
	0x1302,		"esdp_upnp_l2cap",
	0,		NULL
};

Dictionary sdp_attribute_d =
{
	0x0000,		"servicerecordhandle",
	0x0001,		"serviceclassidlist",
	0x0002,		"servicerecordstate",
	0x0003,		"serviceid",
	0x0004,		"protocoldescriptorlist",
	0x0005,		"browsegrouplist",
	0x0006,		"languagebaseattributeidlist",
	0x0007,		"serviceinfotimetolive",
	0x0008,		"serviceavailability",
	0x0009,		"bluetoothprofiledescriptorlist",
	0x000a,		"documentationurl",
	0x000b,		"clientexecutableurl",
	0x000c,		"iconurl",
	0x000d,		"additionalprotocoldescriptorlists",
	0x0200,		"groupid",
	0x0200,		"ipsubnet",
	0x0200,		"versionnumberlist",
	0x0201,		"servicedatabasestate",
	0x0300,		"service_version",
	0x0301,		"external_network",
	0x0301,		"network",
	0x0301,		"supporteddatastoreslist",
	0x0302,		"faxclass1support",
	0x0302,		"remoteaudiovolumecontrol",
	0x0303,		"faxclass2.0support",
	0x0303,		"supportedformatslist",
	0x0304,		"faxclass2support",
	0x0305,		"audiofeedbacksupport",
	0x0306,		"networkaddress",
	0x0307,		"wapgateway",
	0x0308,		"homepageurl",
	0x0309,		"wapstacktype",
	0x030a,		"securitydescription",
	0x030b,		"netaccesstype",
	0x030c,		"maxnetaccessrate",
	0x030d,		"ipv4subnet",
	0x030e,		"ipv6subnet",
	0x0310,		"supportedcapabalities",
	0x0311,		"supportedfeatures",
	0x0312,		"supportedfunctions",
	0x0313,		"totalimagingdatacapacity",
	0,		NULL
};

Dictionary sdp_error_code_d =
{
	1,	"invalid/unsupported_sdp_version",
	2,	"invalid_service_record_handle",
	3,	"invalid_request_syntax",
	4,	"invalid_pdu_size",
	5,	"invalid_continuation_state",
	6,	"insufficient_resources_to_satisfy_request",
	0,	NULL
};


Dictionary bnep_type_d =
{
	0,		"general_ethernet",
	1,		"control",
	2,		"compressed_ethernet",
	3,		"compressed_ethernet_source_only",
	4,		"compressed_ethernet_dest_only",
	0xdb,		"debug", /* Outside spec! */
	0,		NULL
};

Dictionary bnep_control_type_d =
{
	0,		"control_command_not_understood",
	1,		"setup_connection_request_msg",
	2,		"setup_connection_response_msg",
	3,		"filter_net_type_set_msg",
	4,		"filter_net_type_response_msg",
	5,		"filter_multi_addr_set_msg",
	6,		"filter_multi_addr_response_msg",
	7,		"handover_req",
	8,		"handover_resp",
	9,		"handover_ind",
	10,		"handover_cfm",
	11,		"mpi_ind",
	12,		"mpi_cfm",
	13,		"bm_ind",
	14,		"bm_cfm",
	15,		"end_session_ind",
	16,		"end_session_cfm",
	0,		NULL
};

Dictionary ethertype_d =
{
	0x0800,		"ipv4",
	0x0806,		"arp",
	0x8035,		"rarp",
	0x809b,		"ethertalk",
	0x80f3,		"aarp",
	0x8100,		"802.1q",
	0x86dd,		"ipv6",
	0,		NULL
};

Dictionary ip_protocol_d =
{
	0,		"hopoptv6",
	1,		"icmpv4",
	2,		"igmpv4",
	6,		"tcp",
	17,		"udp",
	43,		"routev6",
	44,		"fragv6",
	50,		"esp",
	51,		"ah",
	58,		"icmpv6",
	59,		"nonxtv6",
	60,		"optsv6",
	0,		NULL
};

Dictionary ar_op_d =
{
	1,		"request",
	2,		"reply",
	3,		"reverse_request",
	4,		"reverse_reply",
	8,		"inverse_request",
	9,		"inverse_reply",
	10,		"nak",
	0,		NULL
};

Dictionary in_port_d =
{
	7,		"echo",
	9,		"discard",
	13,		"daytime",
	17,		"quote", /* a.k.a. qotd */
	19,		"chargen",
	20,		"ftp-data",
	21,		"ftp",
	22,		"ssh",
	23,		"telnet",
	25,		"smtp",
	53,		"domain",
	67,		"bootps",
	68,		"bootpc",
	69,		"tftp",
	80,		"http",
	110,		"pop3",
	111,		"sunrpc",
	119,		"nntp",
	123,		"ntp",
	137,		"netbios-ns",
	138,		"netbios-dgm",
	139,		"netbios-ssn",
	143,		"imap",
	161,		"snmp",
	162,		"snmptrap",
	389,		"ldap",
	443,		"ssl",
	445,		"smb",
	520,		"route",
	5353,		"mdns",
	0,		NULL
};

Dictionary icmpv4_type_d =
{
	0,		"echo_reply",
	3,		"destination_unreachable",
	4,		"source_quench",
	5,		"redirect",
	6,		"alternate_host_address",
	8,		"echo",
	9,		"router_advertisement",
	10,		"router_solicitation",
	11,		"time_exceeded",
	12,		"parameter_problem",
	13,		"timestamp",
	14,		"timestamp_reply",
	15,		"information_request",
	16,		"information_reply",
	17,		"address_mask_request",
	18,		"address_mask_reply",
	30,		"traceroute",
	31,		"datagram_conversion_error",
	32,		"mobile_host_redirect",
	33,		"ipv6_where-are-you",
	34,		"ipv6_i-am-here",
	35,		"mobile_registration_request",
	36,		"mobile_registration_reply",
	37,		"domain_name_request",
	38,		"domain_name_reply",
	39,		"skip",
	40,		"photuris",
	0,		NULL
};

Dictionary icmpv6_type_d =
{
	1,		"destination_unreachable",
	2,		"packet_too_big",
	3,		"time_exceeded",
	4,		"parameter_problem",
	128,		"echo_request",
	129,		"echo_reply",
	130,		"multicast_listener_query",
	131,		"multicast_listener_report",
	132,		"multicast_listener_done",
	133,		"router_solicitation",
	134,		"router_advertisement",
	135,		"neighbor_solicitation",
	136,		"neighbor_advertisement",
	137,		"redirect",
	138,		"router_renumbering",
	139,		"icmp_node_information_query",
	140,		"icmp_node_information_response",
	141,		"inverse_neighbor_discovery_solicitation",
	142,		"inverse_neighbor_discovery_advertisement",
	143,		"home_agent_address_discovery_request",
	144,		"home_agent_address_discovery_reply",
	145,		"mobile_prefix_solicitation",
	146,		"mobile_prefix_advertisement",
	0,		NULL
};


Dictionary timer_d =
{
	0,	"m/",
	1,	"s/",
	2,	"connection ",
	3,	"responder ",
	4,	"epr ",
	5,	"rnr ",
	6,	"ulp_procedure ",
	0,	NULL
};



Dictionary rfcomm_dlci_d =
{
	0,	"control",
	1,	"reserved_server_channel",
	62,	"reserved_etsi",
	63,	"reserved_hdlc",
	0,	NULL
};

Dictionary rfcomm_control_field_d =
{
	0x2f,	"sabm",
	0x63,	"ua",
	0x0f,	"dm",
	0x43,	"disc",
	0xef,	"uih",
	/* 0x03,   "ui",  not suported (BT1.1/F1/4.2) */
	0,	NULL
};

Dictionary rfcomm_control_message_d =
{
	0x08,	"test",
	0x28,	"fcon",
	0x18,	"fcoff",
	0x38,	"msc",
	0x24,	"rpn",
	0x14,	"rls",
	0x20,	"pn",
	0x04,	"msc",
	0,	NULL
};


/* Remember to exclude literal numbers from the getvalue... */
Dictionary afh_channel_classification_d =
{
	SACC_ALL,		"all",
	SACC_EVEN,		"even",
	SACC_ODD,		"odd",
	SACC_DOT11_CHANNEL_1,	"c1",
	SACC_DOT11_CHANNEL_2,	"c2",
	SACC_DOT11_CHANNEL_3,	"c3",
	SACC_DOT11_CHANNEL_4,	"c4",
	SACC_DOT11_CHANNEL_5,	"c5",
	SACC_DOT11_CHANNEL_6,	"c6",
	SACC_DOT11_CHANNEL_7,	"c7",
	SACC_DOT11_CHANNEL_8,	"c8",
	SACC_DOT11_CHANNEL_9,	"c9",
	SACC_DOT11_CHANNEL_10,	"c10",
	SACC_DOT11_CHANNEL_11,	"c11",
	SACC_DOT11_CHANNEL_12,	"c12",
	SACC_DOT11_CHANNEL_13,	"c13",
	SACC_DOT11_CHANNEL_14,	"c14",
	0,			NULL
};


Dictionary block_rx_d =
{
	LM_BLOCK_RX_LMP_SET_AFH,	"lmp_set_afh",
	LM_BLOCK_RX_LMP_SET_AFH,	"lsa",
	LM_BLOCK_RX_TDD_POLL,		"master_slave_switch_poll",
	LM_BLOCK_RX_TDD_POLL,		"mssp",
	0,				NULL
};


Dictionary st_d =
{
	MANDATORY,	"standard_scan",
	MANDATORY,	"s",
	1,		"interlaced",
	1,		"i",
	0,		NULL
};


Dictionary im_d =
{
	0,	"standard",
	0,	"s",
	1,	"rssi",
	1,	"r",
	2,	"extended",
	2,	"e",
	0,	NULL
};


Dictionary wc_d =
{
	0x00,	"local",
	0x00,	"l",
	0x01,	"remote",
	0x01,	"r",
	0,	NULL
};


Dictionary fd_d =
{
	0,	"outgoing",
	0,	"outgoing_flow",
	0,	"o",
	1,	"incoming",
	1,	"incoming_flow",
	1,	"i",
	0,	NULL
};


Dictionary re_d =
{
	0,	"no_retransmissions",
	0,	"n",
	0,	"nr",
	0,	"no",
	0,	"none",
	1,	"optimize_for_power_consumption",
	1,	"opc",
	1,	"pc",
	1,	"p",
	1,	"power",
	1,	"power_consumption",
	2,	"optimize_for_link_quality",
	2,	"olq",
	2,	"lq",
	2,	"l",
	2,	"q",
	2,	"quality",
	2,	"link_quality",
	0,	NULL
};


Dictionary psmt_d =
{
	PSMEMORY_FLASH,		"flash",
	PSMEMORY_EEPROM,	"eeprom",
	PSMEMORY_RAM,		"ram",
	PSMEMORY_ROM,		"rom",
	0,			NULL
};


Dictionary lc_esco_debug_d =
{
	0x60,	"lc_init",
	0x61,	"lc_next",
	0x62,	"lc_end",
	0x63,	"lc_add",
	0x64,	"lc_remove",
	0x65,	"lc_removed",
	0x66,	"lc_change",
	0x67,	"lc_check",
	0x68,	"lc_pause_user_data",
	0,	NULL
};


Dictionary lm_esco_ns_d =
{
	0x0,	"init",
	0x1,	"pos",
	0x2,	"rsv",
	0x3,	"lv",
	0x4,	"ns",
	0,	NULL
};


Dictionary afh_class_source_d =
{
	7,	"other",
	8,	"plr  ",
	9,	"rssi ",
	0,	NULL
};


Dictionary l2capCrcRxTx_d =
{
	0,	"rx",
	1,	"tx",
	0,	NULL
};


Dictionary lockedAclPacketTypes_d =
{
	0,	"all",
	0,	"off",
	0,	"none",
	1,	"dm1",
	2,	"dh1/2dh1",
	2,	"dh1",
	2,	"2dh1",
	2,	"2-dh1",
	3,	"dm3/3dh1",
	3,	"dm3",
	3,	"3dh1",
	3,	"3-dh1",
	4,	"dh3/2dh3",
	4,	"dh3",
	4,	"2dh3",
	4,	"2-dh3",
	5,	"dm5/3dh3",
	5,	"dm5",
	5,	"3dh3",
	5,	"3-dh3",
	6,	"dh5/2dh5",
	6,	"dh5",
	6,	"2dh5",
	6,	"2-dh5",
	7,	"3dh5",
	7,	"3-dh5",
	0,	NULL
};


Dictionary et_fns_d = /* must be in (full, abbreviated) pairs (see et()) */
{
	BCCMDPDU_ESCOTEST_ACTIVE_MODES,		"active_modes",			/* Read/write the enabled debugging modes */
	BCCMDPDU_ESCOTEST_ACTIVE_MODES,		"am",				/* Read/write the enabled debugging modes */
	BCCMDPDU_ESCOTEST_SCO_TX_ERR_RATE,	"sco_tx_err_rate",		/* Insert errors into (e)SCO transmissions */
	BCCMDPDU_ESCOTEST_SCO_TX_ERR_RATE,	"ster",				/* Insert errors into (e)SCO transmissions */
	BCCMDPDU_ESCOTEST_SCO_RX_ERR_RATE,	"sco_rx_err_rate",		/* Insert errors into (e)SCO receptions */
	BCCMDPDU_ESCOTEST_SCO_RX_ERR_RATE,	"srer",				/* Insert errors into (e)SCO receptions */
	BCCMDPDU_ESCOTEST_SCO_RTX_ERR_RATE,	"sco_rtx_err_rate",		/* Insert errors into (e)SCO retransmissions */
	BCCMDPDU_ESCOTEST_SCO_RTX_ERR_RATE,	"srter",			/* Insert errors into (e)SCO retransmissions */
	BCCMDPDU_ESCOTEST_SCO_RRX_ERR_RATE,	"sco_rrx_err_rate",		/* Insert errors into receptions of retransmitted (e)SCO packets */
	BCCMDPDU_ESCOTEST_SCO_RRX_ERR_RATE,	"srrer",			/* Insert errors into receptions of retransmitted (e)SCO packets */
	BCCMDPDU_ESCOTEST_SCO_TX_FRAME_RATE,	"sco_tx_frame_err_rate",	/* Rate at which entire eSCO frames should be dropped */
	BCCMDPDU_ESCOTEST_SCO_TX_FRAME_RATE,	"stfer",			/* Rate at which entire eSCO frames should be dropped */
	BCCMDPDU_ESCOTEST_PAUSE_STATE,		"pause_state",			/* The current value of the pause_user_data semaphore (Read Only) */
	BCCMDPDU_ESCOTEST_PAUSE_STATE,		"ps",				/* The current value of the pause_user_data semaphore (Read Only) */
	0,					NULL
};


Dictionary et_modes_d =
{
	BCCMDPDU_ESCOTEST_MODE_SCO_TXERR,	"sco_tx_err",		/* Insert errors into (e)SCO transmissions */
	BCCMDPDU_ESCOTEST_MODE_SCO_RXERR,	"sco_rx_err",		/* Insert errors into (e)SCO receptions */
	BCCMDPDU_ESCOTEST_MODE_SCO_RTXERR,	"sco_rtx_err",		/* Insert errors into (e)SCO retransmissions */
	BCCMDPDU_ESCOTEST_MODE_SCO_RRXERR,	"sco_rrx_err",		/* Insert errors into receptions of retransmitted (e)SCO packets */
	BCCMDPDU_ESCOTEST_MODE_SCO_TXFRAME,	"sco_tx_frame_err",	/* Rate at which entire eSCO frames should be dropped */
	0,					NULL
};

Dictionary pause_d =
{
	0x00,	"unpause",
	0x01,	"pause  ",
	0x02,	"report ",
	0x03,	"ble_rx_unpause ",
	0x04,	"ble_rx_pause ",
	0x05,	"ble_tx_unpause ",
	0x06,	"ble_tx_pause ",
	0x07,	"ble_unpause_rx_in_lc ",
	0x08,	"ble_pause_rx_in_lc ",
	0x09,	"ble_pause_rx_in_lc_cancel ",
	0,	NULL
};

/* Register definitions for the FM/RDS radio interface */
/* XXX this should use symbols defined in fm_api.h */
Dictionary fm_reg_d =
{
	0x00,	"flag",
	0x02,	"int_mask",
	0x04,	"status",
	0x05,	"rssi_lvl",
	0x06,	"if_count",
	0x08,	"rssi_limit",
	0x09,	"if_limit",
	0x0a,	"search_lvl",
	0x0b,	"search_dir",
	0x0c,	"freq",
	0x0e,	"af_freq",
	0x10,	"tuner_mode",
	0x11,	"mute_state",
	0x12,	"most_mode",
	0x13,	"most_blend",
	0x14,	"pause_lvl",
	0x15,	"pause_dur",
	0x16,	"misc",
	0x18,	"man_id",
	0x1a,	"asic_id",
	0x1c,	"firm_ver",
	0x1e,	"asic_ver",
	0x1f,	"band",
	0x20,	"hilo",
	0x21,	"demph_mode",
	0x22,	"rds_system",
	0x23,	"rds_mem",
	0x24,	"power",
	0x25,	"rds_cntr",
	0x26,	"rds_blk_b",
	0x28,	"rds_msk_b",
	0x2a,	"rds_pi",
	0x2c,	"rds_pi_mask",
	0x30,	"audio_routing",
	0x32,	"audio_gain",
	0x34,	"force_mute",
	0x36,	"pcm_intermediate_mode",
	0x38,	"fm_params",
	0x3a,	"snr",
	0x3c,	"rssi_end",
	0x3e,	"audio_io",
	0x3f,	"demod_if",
	0x80,	"rds_data",
	0,	NULL
};

/* FM flag register bit map */
Dictionary fm_flag_vals = {
	0x8000,		"FLRST",
	0x0400,		"FLRSSI",
	0x0200,		"FLSTR",
	0x0100,		"FLPD",
	0x0080,		"FLPI",
	0x0040,		"FLBB",
	0x0020,		"FLRDS",
	0x0010,		"FLSYN",
	0x0008,		"FLLVL",
	0x0004,		"FLIF",
	0x0002,		"FLBL",
	0x0001,		"FLFR",
	0,		NULL
};

/* FM Interrupt masks */
Dictionary fm_int_mask_vals = {
	0x0400,		"IMRSSI",
	0x0200,		"IMSTR",
	0x0100,		"IMPD",
	0x0080,		"IMPI",
	0x0040,		"IMBB",
	0x0020,		"IMRDS",
	0x0010,		"IMSYN",
	0x0008,		"IMLVL",
	0x0004,		"IMIF",
	0x0002,		"IMBL",
	0x0001,		"IMFR",
	0,		NULL
};

/* Misc register mode */
Dictionary miscRegisterMode_d =
{
	FM_API_MISC_MODE_AUDIO_ROUTING,		"fm_audio_routing",
	FM_API_MISC_MODE_FM_CONTROL,		"fm_control",
	FM_API_MISC_MODE_RDS_CONTROL,		"rds_control",
	FM_API_MISC_MODE_AUDIO_GAIN,		"fm_audio_gain",
	FM_API_MISC_MODE_FORCE_MUTE,		"fm_force_mute",
	FM_API_MISC_MODE_INTERMEDIATE_STATE,	"pcm_intermediate_mode",
	FM_API_MISC_MODE_READ_PAGE,		"fm_read_page",
	FM_API_MISC_MODE_FM_PARAMS,		"fm_params",
	FM_API_MISC_MODE_SNR,			"snr",
	0,					NULL
};


/* Misc register FM Audio routing mode Ara field */
Dictionary miscFmAudioRoutingModeAra_d =
{
	0,	"pcm1_unused",
	1,	"pcm1_pcm_format",
	2,	"pcm1_i2s_master",
	3,	"pcm1_i2s_slave",
	0,	NULL
};

/* Misc register FM Audio routing mode Arb field */
Dictionary miscFmAudioRoutingModeArb_d =
{
	0,	"pcm2_unused",
	1,	"pcm2_pcm_format",
	2,	"pcm2_i2s_master",
	3,	"pcm2_i2s_slave",
	0,	NULL
};

/* Misc register FM Audio routing mode arc field */
Dictionary miscFmAudioRoutingModeArc_d =
{
	0,	"codec_unused",
	1,	"codec_used",
	0,	NULL
};

/* Misc register FM Audio routing mode vsa field */
Dictionary miscFmAudioRoutingModeVsa_d =
{
	0,	"pcm1_vs_default",
	1,	"pcm1_vs_01",
	2,	"pcm1_vs_23",
	3,	"pcm1_vs_reserved3",
	0,	NULL
};

/* Misc register FM Audio routing mode vsb field */
Dictionary miscFmAudioRoutingModeVsb_d =
{
	0,	"pcm2_vs_default",
	1,	"pcm2_vs_reserved1",
	2,	"pcm2_vs_23",
	3,	"pcm2_vs_45",
	0,	NULL
};

/* Misc register FM Audio routing mode vsc field */
Dictionary miscFmAudioRoutingModeVsc_d =
{
	0,	"codec_vs_default",
	1,	"codec_vs_01",
	2,	"codec_vs_23",
	3,	"codec_vs_45",
	0,	NULL
};

/*FMTX*/
/* Register difinitions for the FMTX/RDS transmit interface */
/* XXX this should use symbols defined in fm_api.h */
Dictionary fmtx_reg_d =
{
	0x00,	"power",
	0x01,	"pwrlvl",
	0x02,	"freq",
	0x03,	"audio_dev",
	0x04,	"pilot_dev",
	0x05,	"force_mono",
	0x06,	"preemp",
	0x07,	"mute_state",
	0x08,	"mpx_lmt",
	0x09,	"pi",
	0x0a,	"pty",
	0x0b,	"af_freq",
	0x0c,	"disp_mode",
	0x0d,	"disp_upd",
	0x0e,	"rds_dev",
	0x0f,	"drc_mode",
	0x10,	"ana_test",
	0x11,	"ana_lvl",
	0x12,	"tuner_mode",
	0x13,	"status",
	0x14,	"ant_chk",
	0x15,	"audio_range",
	0x16,	"ref_err",
	0x17,	"audio_io",
	0x18,	"drc_config",
	0x19,	"flag",
	0x1A,	"int_mask",
	0x1B,	"aud_thres_low",
	0x1C,	"aud_thres_high",
	0x1D,	"aud_dur_low",
	0x1E,	"aud_dur_high",
	0x1F,	"aud_lvl_l",
	0x20,	"aud_lvl_r",
	0x21,	"drc_cmp_thresh",
	0x22,	"drc_cmp_boost",
	0x23,	"drc_cmp_limit",
	0x24,	"rf_mute",
	0x25,	"info_bits",
	0,	NULL
};

/* FMTX flag register bit map */
Dictionary fmtx_flag_vals = {
	0x0040,		"FLTOM",
	0x0020,		"FLARH",
	0x0010,		"FLARL",
	0x0008,		"FLALH",
	0x0004,		"FLALL",
	0x0002,		"FLPAF",
	0x0001,		"FLFR",
	0,		NULL
};

/* FMTX Interrupt masks */
Dictionary fmtx_int_mask_vals = {
	0x0040,		"IMTOM",
	0x0020,		"IMARH",
	0x0010,		"IMARL",
	0x0008,		"IMALH",
	0x0004,		"IMALL",
	0x0002,		"IMPAF",
	0x0001,		"IMFR",
	0,		NULL
};

Dictionary eirdt_d =
{
	0x01,	"flags",
	0x02,	"incomplete_16bit_uuids",
	0x03,	"complete_16bit_uuids",
	0x04,	"incomplete_32bit_uuids",
	0x05,	"complete_32bit_uuids",
	0x06,	"incomplete_128bit_uuids",
	0x07,	"complete_128bit_uuids",
	0x08,	"incomplete_local_name",
	0x09,	"complete_local_name",
	0xff,	"manufacturer_specific",
	0,	NULL
};


Dictionary eirdtf_d =
{
	0,	NULL
};


Dictionary fecr_d =
{
	0,	"fec_not_required",
	0,	"n",
	1,	"fec_required",
	1,	"r",
	0,	NULL
};


/* Simple Pairing: Simple_Pairing_Mode */
Dictionary spm_d =
{
	0,	"disabled",
	0,	"d",
	0,	"not_set",
	0,	"ns",
	1,	"enabled",
	1,	"e",
	0,	NULL
};


/* Simple Pairing: Simple_Pairing_Debug_Mode */
Dictionary spdm_d =
{
	0,	"disabled",
	0,	"d",
	1,	"enabled",
	1,	"e",
	0,	NULL
};


/* Simple Pairing: IO_Capability */
Dictionary iocap_d = {
	0,	"display_only",
	0,	"d",
	1,	"display_yes_no",
	1,	"yn",
	1,	"dyn",
	2,	"keyboard_only",
	2,	"kb",
	3,	"no_input_no_output",
	3,	"none",
	3,	"n",
	0,	NULL
};


/* Simple Pairing: OOB_Data_Present */
Dictionary oobpresent_d = {
	0,	"oob_not_present",
	0,	"a",
	0,	"np",
	1,	"oob_present",
	1,	"p",
	0,	NULL
};


/* Simple Pairing: Authentication_Requirements */
Dictionary authenticationrequirements_d = {
	0,	"mitm_protection_not_required_no_bonding",
	0,	"mitm_not_req_no_bond",
	0,	"mpnrnb",
	0,	"mitm_protection_not_required_single",
	0,	"mitm_not_req_single",
	0,	"mnrs",
	1,	"mitm_protection_required_no_bonding",
	1,	"mitm_req_no_bond",
	1,	"mprnb",
	1,	"mitm_protection_required_single",
	1,	"mitm_req_single",
	1,	"mrs",
	2,	"mitm_protection_not_required_decicated_bonding",
	2,	"mitm_not_req_dead_bond",
	2,	"mpnrdb",
	2,	"mitm_protection_not_required_all",
	2,	"mitm_not_req_all",
	2,	"mnra",
	3,	"mitm_protection_required_dedicated_bonding",
	3,	"mitm_req_dead_bond",
	3,	"mprdb",
	3,	"mitm_protection_required_all",
	3,	"mitm_req_all",
	3,	"mra",
	4,	"mitm_protection_not_required_general_bonding",
	4,	"mitm_not_req_gen_bond",
	4,	"mpnrgb",
	5,	"mitm_protection_required_general_bonding",
	5,	"mitm_not_req_bond",
	5,	"mprgb",
	0,	NULL
};


/* Simple Pairing: Notification_Type */
Dictionary notificationtype_d = {
	0,	"passkey_entry_started",
	0,	"pes",
	1,	"passkey_digit_entered",
	1,	"pde",
	2,	"passkey_digit_erased",
	2,	"passkey_digit_deleted",
	2,	"pdd",
	3,	"passkey_cleared",
	3,	"pc",
	4,	"passkey_entry_complete",
	4,	"pec",
	0,	NULL
};

/* Enhanced power control: Request_type */
Dictionary enhanced_power_control_request_d = {
	0,	"decrement_power_one_step",
	1,	"increment_power_one_step",
	2,	"increase_to_maximum_power",
	0,	NULL
};

/* Enhanced power control: Response_type */
Dictionary enhanced_power_control_response_field_d = {
	0,	"not_supported",
	1,	"changed_one_step",
	2,	"max_power",
	3,	"min_power",
	0,	NULL
};

/* Power control debug: command */
Dictionary force_power_control_request_d = {
	DEBUG_PARAM_FPC_ENABLE_PC,	"automatic",
	DEBUG_PARAM_FPC_ENABLE_PC,	"on",
	DEBUG_PARAM_FPC_DISABLE_PC,	"manual",
	DEBUG_PARAM_FPC_DISABLE_PC,	"off",
	DEBUG_PARAM_FPC_SEND_UP,	"up",
	DEBUG_PARAM_FPC_SEND_DOWN,	"down",
	DEBUG_PARAM_FPC_SEND_MAX,	"max",
	DEBUG_PARAM_FPC_CLEAR_STATUS,	"reset",
	0,				NULL
};

/* Encapsulated PDU */
Dictionary encapsulated_header_major_minor_d = {
	0x0101,	"P-192 Public Key, X Y Format",
	0,	NULL
};

/* LE: Le_Supported_Host */
Dictionary lesh_d =
{
	0,	"disabled",
	0,	"d",
	1,	"enabled",
	1,	"e",
	0,	NULL
};

/* LE: Simultaneous_LE_Host */
Dictionary sleh_d =
{
	0,	"disabled",
	0,	"d",
	1,	"enabled",
	1,	"e",
	0,	NULL
};

/****/
/* Dictionary entries for BLE */
/****/
/* Advertising Mode */
Dictionary am_d =
{
	0,	"disabled",
	0,	"d",
	0,	"not_set",
	0,	"ns",
	1,	"enabled",
	1,	"e",
	0,	NULL
};

/* Scan Mode */
Dictionary sm_d =
{
	0,	"disabled",
	0,	"d",
	0,	"not_set",
	0,	"ns",
	1,	"enabled",
	1,	"e",
	0,	NULL
};

/* Filter Duplicates */
Dictionary filtdup_d =
{
	0,	"disabled",
	0,	"d",
	0,	"not_set",
	0,	"ns",
	1,	"enabled",
	1,	"e",
	0,	NULL
};

/* Advertising filter policy */
Dictionary afp_d =
{
	0,		"scan_conn",
	0,		"conn_scan",
	0,		"sc",
	0,		"cs",
	0,		"eall",
	0,      "all",
	0,		"allow_all",
	1,		"conn",
	1,		"c",
	2,		"scan",
	2,		"s",
	3,		"dall",
	3,		"d",
	3,		"n",
	3,		"whitelist",
	3,		"w",
	0,		NULL
};

/* Scanning filter policy */
Dictionary sfp_d =
{
	0,		"d",
	0,		"allow_all",
	0,		"all",
	0,		"a",
	1,		"whitelist",
	1,		"w",
	0,		NULL
};

/* Initiator filter policy */
Dictionary ifp_d =
{
	0,		"peer",
	0,		"p",
	1,		"whitelist",
	1,		"w",
	0,		NULL
};

/* Advertising channels */
Dictionary sac_d =
{
	0,		"dall",
	0,		"none",
	1,		"37",
	2,		"38",
	4,		"39",
	3,		"37&38",
	3,		"3738",
	3,		"38&37",
	3,		"3837",
	5,		"37&39",
	5,		"3739",
	5,		"39&37",
	5,		"3937",
	6,		"38&39",
	6,		"3839",
	6,		"39&38",
	6,		"3938",
	7,		"eall",
	7,		"all",
	7,		"37&38&39",
	7,		"373839",
	0,		NULL
};

/* Scan type */
Dictionary stype_d =
{
	0,		"p",
	0,		"passive",
	1,		"a",
	1,		"active",
	0,		NULL
};

/* Address type */
Dictionary at_d =
{
	0,		"pub",
	0,		"public",
	1,		"rand",
	1,		"random",
	1,		"pri",
	1,		"private",
    2,      "rdf",
	0,		NULL
};

/* Packet payload */
Dictionary pktpayload_d =
{
	0,		"prbs9",
	1,		"altnib1",
	2,		"altbit1",
	3,		"prbs15",
	4,		"all1",
	5,		"all0",
	6,		"altnib0",
	7,		"altbit0",
	0,		NULL
};

/* Advertising packet type in le_set_advertising_parameters/rdf advertising */
Dictionary aet_d =
{
	0,		"cu",
	0,		"connectable-undirected",
	1,		"cd",
	1,		"connectable-directed",
	2,		"du",
	2,		"discoverable-undirected",
	3,		"nc",
	3,		"non-connectable",
	0,		NULL
};

/* Advertising packet type in advertising events */
Dictionary et_d =
{
	0,		"cu",
	0,		"connectable-undirected",
	1,		"cd",
	1,		"connectable-directed",
	2,		"du",
	2,		"discoverable-undirected",
	3,		"nc",
	3,		"non-connectable",
	4,		"sr",
	4,		"scan_response",
	0,		NULL
};

/* rdf scanning mode */
Dictionary rdfscan_d =
{
	0,		"n",
	0,		"none",
	1,		"df",
	2,		"pos",
	0,		NULL
};


/* Clock accuracy */
Dictionary clkacc_d =
{
	0,		"500ppm",
	1,		"250ppm",
	2,		"150ppm",
	3,		"100ppm",
	4,		"75ppm",
	5,		"50ppm",
	6,		"30ppm",
	7,		"20ppm"
};

Dictionary mp_d =
{
	0,		"n",
	1,		"y",
	0,		NULL
};

Dictionary er_d =
{
	0,		"om",
	1,		"em",
	0,		NULL
};

Dictionary prof_d =
{
	0,		"no_other_profile_supported",
	1,		"other_profiles_supported",
	0,		NULL
};

Dictionary enc_d =
{
	0,		"open_mode",
	1,		"encrypted_mode",
	0,		NULL
};

/* Dictionaries shared by the WLAN Coexistence Interface Test BCCMD and FBCMD */
Dictionary LwctMode_d =
{
	BCCMDPDU_COEX_INT_TEST_MODE_START,	"START",
	BCCMDPDU_COEX_INT_TEST_MODE_WRITE_DIR,	"WRITE_DIR",
	BCCMDPDU_COEX_INT_TEST_MODE_WRITE_LVL,	"WRITE_LVL",
	BCCMDPDU_COEX_INT_TEST_MODE_READ,	"READ",
	BCCMDPDU_COEX_INT_TEST_MODE_STOP,	"STOP",
	0,					NULL
};

Dictionary LwctValue_d =
{
	0,	"0x00",
	1,	"0x01",
	2,	"0x02",
	3,	"0x03",
	4,	"0x04",
	5,	"0x05",
	6,	"0x06",
	7,	"0x07",
	0,	NULL
};

Dictionary fastpipe_result_d =
{
	FASTPIPE_CANNOT_BE_ENABLED, "cannot_be_enabled",
	FASTPIPE_ALREADY_ENABLED, "already_enabled",
	FASTPIPE_NOT_ENABLED, "not_enabled",
	FASTPIPE_CREDIT_PIPE_NOT_CREATED, "credit_pipe_not_created",
	FASTPIPE_ID_IN_USE, "id_in_use",
	FASTPIPE_TX_CAPACITY_REFUSED, "tx_capacity_refused",
	FASTPIPE_RX_CAPACITY_REFUSED, "rx_capacity_refused",
	FASTPIPE_TOO_MANY_PIPES_ACTIVE, "too_many_pipes_active",
	FASTPIPE_WOULD_EXCEED_CONTROLLER_LIMIT, "would_exceed_controller_limit",
	FASTPIPE_INVALID_PARAMETERS, "invalid_parameters",
	FASTPIPE_INVALID_PIPE_ID, "invalid_pipe_id",
	FASTPIPE_IS_CREDIT_PIPE, "is_credit_pipe",
	FASTPIPE_SUCCESS, "success",
	FASTPIPE_WOULD_EXCEED_HOST_LIMIT, "would_exceed_host_limit",
	0, NULL
};

Dictionary fastpipe_engine_result_d =
{
	FASTPIPE_ENGINE_SUCCESS, "success",
	FASTPIPE_ENGINE_ERROR_NOT_ENABLED, "error_not_enabled",
	FASTPIPE_ENGINE_ERROR_INCORRECT_TOKEN_PIPE_ID, "error_incorrect_token_pipe_id",
	FASTPIPE_ENGINE_ERROR_RECYCLED_PIPE_ID, "error_recycled_pipe_id",
	FASTPIPE_ENGINE_ERROR_RECYCLED_HANDLE, "error_recycled_handle",
	FASTPIPE_ENGINE_ERROR_UNCONNECTED_PIPE_ID, "error_unconnected_pipe_id",
	FASTPIPE_ENGINE_ERROR_DESTROYING_TOKEN_PIPE, "error_destroying_token_pipe",
	FASTPIPE_ENGINE_ERROR_UNREQUESTED_DESTROY, "error_unrequested_destroy",
	FASTPIPE_ENGINE_ERROR_VIOLATES_LIMITH, "error_violates_limith",
	FASTPIPE_ENGINE_ERROR_VIOLATES_LIMITC, "error_violates_limitc",
	FASTPIPE_ENGINE_ERROR_UNCONNECTED_HANDLE, "error_unconnected_handle",
	FASTPIPE_ENGINE_ERROR_ALREADY_RECEIVING, "error_already_receiving",
	FASTPIPE_ENGINE_ERROR_NOT_RECEIVING, "error_not_receiving",
	FASTPIPE_ENGINE_ERROR_COULD_NOT_OPEN_FILE, "error_could_not_open_file",
	FASTPIPE_ENGINE_ERROR_CURRENTLY_REDUCING_LIMITC, "error_currently_reducing_limitc",
	FASTPIPE_ENGINE_SUCCESS_REDUCED_LIMITC, "success_reduced_limitc",
	FASTPIPE_ENGINE_SUCCESS_REDUCING_LIMITC, "success_reducing_limitc",
	FASTPIPE_ENGINE_ERROR_REDUCTION_VIOLATES_LIMITC, "error_reduction_violates_limitc",
	FASTPIPE_ENGINE_ERROR_UNRECOGNISED, "error_unrecognised",
	0, NULL
};

Dictionary stream_source_sink_d =
{
	1,	"pcm",
	1,	"p",
	2,	"i2s",
	2,	"i",
	3,	"codec",
	3,	"c",
	4,	"fm",
	4,	"f",
	5,	"spdif",
	5,	"s",
	6,	"dig_mic",
	6,	"dm",
        7,      "a2dp",
        7,      "shunt",
        8,      "fastpipe",
        8,      "fp",
	0,	NULL
};

Dictionary stream_config_d =
{
	0x0100,		"pcm_sync_rate",
	0x0100,		"psr",
	0x0101,		"pcm_master_clock_rate",
	0x0101,		"pmcr",
	0x0102,		"pcm_master_mode",
	0x0102,		"pmm",
	0x0103,		"pcm_slot_count",
	0x0103,		"psc",
	0x0104,     "pcm_manch_mode_enable",
	0x0104,     "pmme",
	0x0105,     "pcm_short_sync_enable",
	0x0105,     "psse",
	0x0106,     "pcm_manch_slave_mode_enable",
	0x0106,     "pmsme",
	0x0107,     "pcm_sign_extend_enable",
	0x0107,     "psee",
	0x0108,     "pcm_lsb_first_enable",
	0x0108,     "plfe",
	0x0109,     "pcm_tx_tristate_enable",
	0x0109,     "ptte",
	0x010a,     "pcm_tx_tristate_rising_edge_enable",
	0x010a,     "pttree",
	0x010b,     "pcm_sync_suppress_enable",
	0x010b,     "pssue",
	0x010c,     "pcm_gci_mode_enable",
	0x010c,     "pgme",
	0x010d,     "pcm_mute_enable",
	0x010d,     "pme",
	0x010e,     "pcm_long_length_sync_enable",
	0x010e,     "pllse",
	0x010f,     "pcm_sample_rising_edge_enable",
	0x010f,     "psree",
	0x0110,     "pcm_extend_features_enable",
	0x0110,     "pefe",
	0x0111,     "pcm_48m_dds_update_enable",
	0x0111,     "pdde",
	0x0112,     "pcm_slow_clock_rate",
	0x0112,     "psscr",
	0x0113,     "pcm_rx_rate_delay",
	0x0113,     "prrd",
	0x0114,     "pcm_sample_format",
	0x0114,     "psf",
	0x0115,     "pcm_manch_rx_offset",
	0x0115,     "pmro",
	0x0116,     "pcm_audio_gain",
	0x0116,     "pag",
	0x0117,     "pcm_long_format",
	0x0117,     "plf",
	0x0200,		"i2s_sync_rate",
	0x0200,		"isr",
	0x0201,		"i2s_master_clock_rate",
	0x0201,		"imcr",
	0x0202,		"i2s_master_mode",
	0x0202,		"imm",
    0x0203,     "i2s_jstfy_format",
	0x0203,     "ijf",
	0x0204,     "i2s_lft_jstfy_dly",
	0x0204,     "iljd",
	0x0205,     "i2s_chnl_plrty",
	0x0205,     "icp",
    0x0206,     "i2s_audio_atten_enable",
	0x0206,     "iaae",
	0x0207,     "i2s_audio_atten",
	0x0207,     "iaa",
	0x0208,     "i2s_jstfy_res",
	0x0208,     "ijr",
	0x0209,     "i2s_crop_enable",
	0x0209,     "ice",
	0x020a,     "i2s_bits_per_sample",
	0x020b,     "i2s_tx_start_sample",
	0x020c,     "i2s_rx_start_sample",
	0x0300,		"codec_input_rate",
	0x0300,		"cir",
	0x0301,		"codec_output_rate",
	0x0301,		"cor",
	0x0302,		"codec_input_gain",
	0x0302,		"cig",
	0x0303,		"codec_output_gain",
	0x0303,		"cog",
	0x0304,		"codec_raw_input_gain",
	0x0304,		"crig",
	0x0305,		"codec_raw_output_gain",
	0x0305,		"crog",
	0x0306,		"codec_output_gain_boost_enable",
	0x0306,		"cogbe",
	0x0307,		"codec_sidetone_gain",
	0x0307,		"csg",
	0x0308,		"codec_sidetone_enable",
	0x0308,		"cse",
	0x0309,		"codec_mic_input_gain_enable",
	0x0309,		"cmige",
	0x030a,		"codec_low_power_output_stage_enable",
	0x030a,		"clpose",
	0x030b,		"codec_quality_mode",
	0x030b,		"cqm",
    0x030c,     "codec_output_interp_filter_mode",
    0x030c,     "coifm",
    0x030d,     "audio_output_power_mode",
	0x030e,		"codec_sidetone_source",
	0x030e,		"css",
	0x0400,		"fm_input_rate",
	0x0400,		"fir",
	0x0401,		"fm_output_rate",
	0x0401,		"for",
	0x0402,		"fm_input_gain",
	0x0402,		"fig",
	0x0403,		"fm_output_gain",
	0x0403,		"fog",
	0x0500,		"spdif_output_rate",
	0x0500,		"sor",
	0x0600,		"digital_mic_input_rate",
	0x0600,		"dmir",
	0x0601,		"digital_mic_input_gain",
	0x0601,		"dmig",
	0x0602,		"digital_mic_sidetone_gain",
	0x0602,		"dmsg",
	0x0603,		"digital_mic_sidetone_enable",
	0x0603,		"dmse",
	0x0700,		"audio_mute_enable",
	0x0700,		"ame",
	0,		NULL
};

/* functions list for SetPinFunction command*/
Dictionary SetPinFunction_d = 
{
    0, "UART_RX",
    1, "UART_TX",
    2, "UART_RTS",
    3, "UART_CTS",
    4, "PCM_IN",
    5, "PCM_OUT",
    6, "PCM_SYNC",
    7, "PCM_CLK",
    0, NULL
};

Dictionary mic_bias_configure_d =
{
    0x0,        "enable",
    0x1,        "voltage",
    0x2,        "current"
};

Dictionary led_config_d =
{
    0,          "LED_ENABLE",
    1,          "LED_DUTY_CYCLE",
    2,          "LED_PERIOD",
    3,          "LED_FLASH_ENABLE",
    4,          "LED_FLASH_RATE",
    5,          "LED_FLASH_MAX_HOLD",
    6,          "LED_FLASH_MIN_HOLD",
    7,          "LED_FLASH_SEL_INVERT",
    8,          "LED_FLASH_SEL_DRIVE",
    9,          "LED_FLASH_SEL_TRISTATE",
    0,          NULL
};

Dictionary ulp_radio_test_d =
{
    BCCMDPDU_BLE_RADIO_TEST_STOP,           "stop",
    BCCMDPDU_BLE_RADIO_TEST_STOP,           "s",
    BCCMDPDU_BLE_RADIO_TEST_TRANSMITTER,    "t",
    BCCMDPDU_BLE_RADIO_TEST_TRANSMITTER,    "transmit",
    BCCMDPDU_BLE_RADIO_TEST_TRANSMITTER,    "transmitter",  
    BCCMDPDU_BLE_RADIO_TEST_RECEIVER,       "r",
    BCCMDPDU_BLE_RADIO_TEST_RECEIVER,       "receive",
    BCCMDPDU_BLE_RADIO_TEST_RECEIVER,       "receiver",
    0xff,                                   NULL
};

Dictionary ulp_adv_filt_operations =
{
    BCCMDPDU_BLE_ADV_REP_FILT_OR,           "or",
    0,                                      NULL
};

Dictionary ulp_adv_filt_ad_type =
{
    0x01,   "flags",
    0x02,   "incomplete_16bit_uuids",
    0x03,   "complete_16bit_uuids",
    0x04,   "incomplete_32bit_uuids",
    0x05,   "complete_32bit_uuids",
    0x06,   "incomplete_128bit_uuids",
    0x07,   "complete_128bit_uuids",
    0x08,   "incomplete_local_name",
    0x09,   "complete_local_name",
    0x0a,   "tx_power_level",
    /* 0x0b and 0x0c are not defined in the gat spec. */
    0x0d,   "class_of_device",
    0x0e,   "simple_pairing_hash_c",
    0x0f,   "simple_pairing_randomizer",
    0x10,   "tk_value",
    0x11,   "security_manager_oob_flags",
    0x12,   "slave_connection_interval_range",
    0x14,   "service_solicitation_uuids_16bit",
    0x15,   "service_solicitation_uuids_128bit",
    0x16,   "service_data",
    /* Range not defined by the spec */
    0xff,   NULL
};

#include "dspmanager_status.h"

Dictionary stibbons_error_codes_d =
{
    DSPMANAGER_STATUS_ALREADY_CONNECTED, "already_connected",
    DSPMANAGER_STATUS_BAD_OPID, "bad_opid",
    DSPMANAGER_STATUS_CANT_LAUNCH_SUBSYSTEM, "cant_launch_subsystem",
    DSPMANAGER_STATUS_COMMAND_LENGTH_ERROR, "command_length_error",
    DSPMANAGER_STATUS_COMMAND_SEQUENCE_ERROR, "command_sequence_error",
    DSPMANAGER_STATUS_DISCONNECT_ERROR, "disconnect_error",
    DSPMANAGER_STATUS_INCOMPATIBLE_DATA_TYPES, "incompatible_data_types",
    DSPMANAGER_STATUS_INSUFFICIENT_RESOURCE, "insufficient_resource",
    DSPMANAGER_STATUS_INTERNAL_BAD_VALUE, "internal_bad_value",
    DSPMANAGER_STATUS_INTERNAL_COMMS_ERROR, "internal_comms_error",
    DSPMANAGER_STATUS_INVALID_DATA, "invalid_data",
    DSPMANAGER_STATUS_INVALID_OPERAND, "invalid_operand",
    DSPMANAGER_STATUS_INVALID_TERMINAL, "invalid_terminal",
    DSPMANAGER_STATUS_NO_FREE_IDENTIFIERS, "no_free_identifiers",
    DSPMANAGER_STATUS_OPERATOR_CANNOT_CONNECT_WHEN_RUNNING, "cannot_connect_when_running",
    DSPMANAGER_STATUS_OPERATOR_RUNNING, "operator_running",
    DSPMANAGER_STATUS_OPID_IN_USE, "opid_in_use",
    DSPMANAGER_STATUS_OUT_OF_MEMORY, "out_of_memory",
    DSPMANAGER_STATUS_OUT_OF_RAM, "out_of_ram",
    DSPMANAGER_STATUS_PORT_STATE_WRONG, "port_state_wrong",
    DSPMANAGER_STATUS_SINK_PORT_IN_USE, "sink_port_in_use",
    DSPMANAGER_STATUS_SINK_PORT_NOT_CONNECTED, "sink_port_not_connected",
    DSPMANAGER_STATUS_SOURCE_PORT_IN_USE, "source_port_in_use",
    DSPMANAGER_STATUS_SOURCE_PORT_NOT_CONNECTED, "source_port_not_connected",
    DSPMANAGER_STATUS_STREAM_IN_WRONG_STATE, "stream_in_wrong_state",
    DSPMANAGER_STATUS_SUCCESS, "success",
    DSPMANAGER_STATUS_UNKNOWN_BCID, "unknown_bcid",
    DSPMANAGER_STATUS_UNKNOWN_CAP, "unknown_cap",
    DSPMANAGER_STATUS_UNKNOWN_CAPABILITY, "unknown_capability",
    DSPMANAGER_STATUS_UNKNOWN_ERROR, "unknown_error",
    DSPMANAGER_STATUS_UNKNOWN_MESSAGE, "unknown_message",
    DSPMANAGER_STATUS_UNKNOWN_OID, "unknown_oid",
    DSPMANAGER_STATUS_UNKNOWN_PORT_ID, "unknown_port_id",
    DSPMANAGER_STATUS_UNKNOWN_PROBLEM, "unknown_problem",
    DSPMANAGER_STATUS_UNKNOWN_SUBCOMMAND, "unknown_subcommand",
    DSPMANAGER_STATUS_UNKNOWN_SUBSYSTEM, "unknown_subsystem",
    DSPMANAGER_STATUS_UNSUPPORTED_MESSAGE, "unsupported_message",
    DSPMANAGER_STATUS_WRONG_STATE, "wrong_state",
    DSPMANAGER_STATUS_ZERO_LENGTH, "zero_length",
    0, NULL
};

#include "app/kalimba/kalimba_messages.h"

Dictionary baton_messages_d =
{
    BATON_COMMAND_REPLY, "reply",
    BATON_MSG_FROM_OPERATOR, "msg_from_op",
    BATON_DSP_PANIC, "previn_panic",
    BATON_CONNECT, "connect",
    BATON_START_OPERATOR, "start_op",
    BATON_STOP_OPERATOR, "stop_op",
    BATON_RESET_OPERATOR, "reset_op",
    BATON_DESTROY_OPERATOR, "destroy_op",
    BATON_DISCONNECT, "disconnect",
    BATON_OPERATOR_MESSAGE, "op_message",
    BATON_CREATE_OPERATOR_C, "create_op_c",
    BATON_CREATE_OPERATOR_P, "create_op_p",
    BATON_DSPMANAGER_DEBUG, "previn_debug",
    BATON_SYNC_CONNECTIONS, "sync",
    BATON_UNSYNC_CONNECTIONS, "unsync",
    0, NULL
};

Dictionary create_operator_skip_flag_d =
{
    1, "all_patches",
    2, "skip_patches",
    0, NULL
};

Dictionary siflash_d =
{
    BCCMDPDU_SIFLASH_READ_CMD, "read",
    BCCMDPDU_SIFLASH_WRITE_CMD, "write",
    BCCMDPDU_SIFLASH_CHIP_ERASE_CMD, "chip_erase",
    BCCMDPDU_SIFLASH_SECTOR_ERASE_CMD, "sector_erase",
    BCCMDPDU_SIFLASH_BLOCK64_ERASE_CMD, "block64_erase",
    0, NULL
};

Dictionary spilock_d =
{
    SPI_LOCK_STATUS_UNLOCKED_AND_CUST_KEY_UNSET,
		"SPI Unlocked, customer key unset",
    SPI_LOCK_STATUS_LOCKED_AND_CUST_KEY_UNSET,
		"SPI Locked, customer key unset",
    SPI_LOCK_STATUS_UNLOCKED_AND_CUST_KEY_SET,
		"SPI Unlocked, customer key set",
    SPI_LOCK_STATUS_LOCKED_AND_CUST_KEY_SET,
		"SPI Locked, customer key set",
	4, NULL
};



