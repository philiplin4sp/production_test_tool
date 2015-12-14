/****************************************************************************
FILE
	debug_hci.h

CONTAINS

	This file contains all of the very nasty defines for the
	debugging messages.  These were originally only used by the
	LM, but have now spread into other parts of the system
	(including the LC).

MODIFICATION HISTORY
	#1 06:aug:07 pr05 B-29035: Created.
        ------------------------------------------------------------------
        --- This modification history is now closed. Do not update it. ---
        ------------------------------------------------------------------
*/

#ifndef DEBUG_HCI_H__
#define DEBUG_HCI_H__

#ifdef RCS_STRINGS
static const char debug_hci_h_id[]
  = "$Id: //depot/bc/main/interface/host/debug/hci_debug.h#1 $";
#endif


/* The numbers here are listed in numerical order, which makes adding
   a new unique number much easier. */

/******************************************************************************
  VENDOR SPECIFIC DEBUG "command" values in HCI_DEBUG_REQUEST_T
 *****************************************************************************/
#define HCI_DEBUG_STATS                ((uint16_t) 0x0001)
#define HCI_DEBUG_STATS_ACL            ((uint16_t) 0x0002)
#define HCI_DEBUG_STATS_SCO            ((uint16_t) 0x0003)
#define HCI_DEBUG_MEM                  ((uint16_t) 0x0004)
#define HCI_DEBUG_RAND                 ((uint16_t) 0x0005)
#define HCI_DEBUG_KEY                  ((uint16_t) 0x0006)
#define HCI_DEBUG_SRES                 ((uint16_t) 0x0007)
#define HCI_DEBUG_LMP_TEST_ACTIVATE    ((uint16_t) 0x0008)
#define HCI_DEBUG_LMP_TEST_CONTROL     ((uint16_t) 0x0009)
#define HCI_DEBUG_MASTER_JITTER        ((uint16_t) 0x000a)
#define HCI_DEBUG_STATS_ESCO           ((uint16_t) 0x000b)
#define HCI_DEBUG_SCATTER_REQ          ((uint16_t) 0x000c)
#define HCI_DEBUG_UNSCATTER_REQ        ((uint16_t) 0x000d)
#define HCI_DEBUG_SET_SUBRATE          ((uint16_t) 0x000e)
#define HCI_DEBUG_PP_REQ               ((uint16_t) 0x000f)
#define HCI_DEBUG_FORCE_POWER_CONTROL  ((uint16_t) 0x0010)

#define HCI_DEBUG_GENERAL              ((uint16_t) 0x0013)
#define HCI_DEBUG_TX_LMP               ((uint16_t) 0x0014)
#define HCI_DEBUG_STOP_LM              ((uint16_t) 0x0015)
#define HCI_DEBUG_START_LM             ((uint16_t) 0x0016)

#define HCI_DEBUG_PS_WRITE             ((uint16_t) 0x0020)


#define HCI_DEBUG_SET_BTCLOCK          ((uint16_t) 0x0040)
#define HCI_DEBUG_BLOCK_RX             ((uint16_t) 0x0041)
#define HCI_DEBUG_SET_ABSENCE          ((uint16_t) 0x0042)
#define HCI_DEBUG_CLEAR_ABSENCE        ((uint16_t) 0x0043)
#define HCI_DEBUG_ESCO_PING            ((uint16_t) 0x0044)



/******************************************************************************
  VENDOR SPECIFIC DEBUG "debug_event" values in HCI_EV_DEBUG_T
 *****************************************************************************/
#define HCI_DEBUG_EV_FSM                    0x00
#define HCI_DEBUG_EV_STATS                  0x01
#define HCI_DEBUG_EV_ACL                    0x02
#define HCI_DEBUG_EV_SCO                    0x03
#define HCI_DEBUG_EV_MEM                    0x04
#define HCI_DEBUG_EV_RAND                   0x05
#define HCI_DEBUG_EV_KEY                    0x06
#define HCI_DEBUG_EV_SRES                   0x07
#define HCI_DEBUG_EV_RADIOBUFFER_ERR        0x08

#define HCI_DEBUG_EV_PARKED                 0x0a
#define HCI_DEBUG_EV_ESCO                   0x0b

#define HCI_DEBUG_EV_RX_LMP                 0x0f
#define HCI_DEBUG_EV_TX_LMP                 0x10
#define HCI_DEBUG_EV_ACO                    0x11
#define HCI_DEBUG_EV_ACL_HANDLE             0x12
#define HCI_DEBUG_EV_ULP_ADV_CHANNEL        0x13
#define HCI_DEBUG_EV_ULP_ACL_MASTER         0x14
#define HCI_DEBUG_EV_ULP_ACL_SLAVE          0x15
#define HCI_DEBUG_EV_ULP_INST_UPDATED       0x16

#define HCI_DEBUG_EV_TID_FIX                0x20


#define HCI_DEBUG_EV_PKT_TYPES              0x37
#define HCI_DEBUG_EV_SLAVE_MAX_SLOTS        0x38



#define HCI_DEBUG_EV_DRAIN                  0x40


#define HCI_DEBUG_EV_MLK_RENEG              0x48
#define HCI_DEBUG_EV_MLK_CONFIRM            0x49


#define HCI_DEBUG_EV_LC_MSG_TO_FG           0x50
#define HCI_DEBUG_EV_LC_MSG_FROM_FG         0x5f


#define HCI_DEBUG_EV_LC_SCO_SELECT_D_INIT   0x60
#define HCI_DEBUG_EV_LC_SCO_SELECT_D_NEXT   0x61
#define HCI_DEBUG_EV_LC_SCO_SELECT_D_END    0x62
#define HCI_DEBUG_EV_LC_ADD_SCO             0x63
#define HCI_DEBUG_EV_LC_REMOVE_SCO          0x64
#define HCI_DEBUG_EV_LC_SCO_REMOVED         0x65
#define HCI_DEBUG_EV_LC_CHANGE_SCO          0x66
#define HCI_DEBUG_EV_LC_SCO_CHECK           0x67
#define HCI_DEBUG_EV_LC_PAUSE_USER_DATA	    0x68
#define HCI_DEBUG_EV_LC_PAUSE_ALL_DATA	    0x69
#define HCI_DEBUG_EV_LC_PAUSE_DATA_CALLBACK 0x6a

#define HCI_DEBUG_EV_TO_START               0x78
#define HCI_DEBUG_EV_TO_CANCEL              0x79
#define HCI_DEBUG_EV_RX_LMP_DUPLICATE       0x7b
#define HCI_DEBUG_EV_SNIFF_STATS            0x7c
#define HCI_DEBUG_EV_TO                     0x80
#define HCI_DEBUG_EV_BDW_TO                 0x81
#define HCI_DEBUG_EV_DETACH_TO              0x82
#define HCI_DEBUG_EV_TO_LOST                0x85
#define HCI_DEBUG_EV_SLOTS                  0x90
#define HCI_DEBUG_EV_AFH_CLASSIFICATION	    0x94
#define HCI_DEBUG_EV_INSTANT                0xa0
#define HCI_DEBUG_EV_CONVERT_DSCO           0xa1
#define HCI_DEBUG_EV_SLOW_CLOCK             0xa8
#define HCI_DEBUG_EV_AFH_HSSI	            0xaa
#define HCI_DEBUG_EV_AFH_GOOD_BAD           0xb2
#define HCI_DEBUG_EV_AFH_TO	            0xb3
#define HCI_DEBUG_EV_AFH_CALCULATE1         0xb4
#define HCI_DEBUG_EV_AFH_CALCULATE2         0xb5
#define HCI_DEBUG_EV_UNPARK                 0xb6
#define HCI_DEBUG_EV_MLK_NEW_SIZE           0xca
#define HCI_DEBUG_EV_MLK_SIZE	            0xcb
#define HCI_DEBUG_EV_ENC_START              0xcc
#define HCI_DEBUG_EV_CQDDR                  0xcd
#define HCI_DEBUG_EV_DOT11_CHANNEL          0xd0
#define HCI_DEBUG_EV_ESCO_NEGOTIATION       0xe0
#define HCI_DEBUG_EV_ESCO_ERROR             0xe1

#define HCI_DEBUG_EV_L2CAP_CRC              0xe8


#define HCI_DEBUG_EV_ECC_PRIVATE            0xf1
#define HCI_DEBUG_EV_ECC_PUBLIC_X           0xf2
#define HCI_DEBUG_EV_ECC_PUBLIC_Y           0xf3
#define HCI_DEBUG_EV_ECC_RESULT             0xf4
#define HCI_DEBUG_EV_U                      0xf5
#define HCI_DEBUG_EV_V                      0xf6
#define HCI_DEBUG_EV_X                      0xf7
#define HCI_DEBUG_EV_F1                     0xf8
#define HCI_DEBUG_EV_Y                      0xf9
#define HCI_DEBUG_EV_W                      0xfa
#define HCI_DEBUG_EV_N                      0xfb
#define HCI_DEBUG_EV_KEYA1A2                0xfc
#define HCI_DEBUG_EV_F                      0xfd
#define HCI_DEBUG_EV_IOCAP1A2               0xfe
#define HCI_DEBUG_EV_R                      0xff


/* Timer types for timer LMP_TIMEOUT_DEBUG */
/* #define HCI_DEBUG_EV_TI_M              LM_MASTER_TRANSACTION_ID */
/* #define HCI_DEBUG_EV_TI_S              LM_SLAVE_TRANSACTION_ID */
#define HCI_DEBUG_EV_TO_CONNECTION      2
#define HCI_DEBUG_EV_TO_RESPONDER       3
#define HCI_DEBUG_EV_TO_EPR             4
#define HCI_DEBUG_EV_TO_RNR             5
#define HCI_DEBUG_EV_TO_ULP_PROCEDURE   6


/******************************************************************************
  Parameters for (specific) debug commands
 *****************************************************************************/

/* HCI_DEBUG_FORCE_POWER_CONTROL */
#define DEBUG_PARAM_FPC_ENABLE_PC               0
#define DEBUG_PARAM_FPC_DISABLE_PC              1
#define DEBUG_PARAM_FPC_SEND_UP                 2
#define DEBUG_PARAM_FPC_SEND_DOWN               3
#define DEBUG_PARAM_FPC_SEND_MAX                4
#define DEBUG_PARAM_FPC_CLEAR_STATUS            5


/******************************************************************************
  Parameters for (specific) debug events
 *****************************************************************************/

/* HCI_DEBUG_EV_FSM */
#define HCI_DEBUG_EV_FSM_LM_SLAVE               0x00
#define HCI_DEBUG_EV_FSM_LM_MASTER              0x01
#define HCI_DEBUG_EV_FSM_LM_DEVICE              0x02
#define HCI_DEBUG_EV_FSM_LM_POWER               0x03
#define HCI_DEBUG_EV_FSM_LM_INIT_AUTH           0x04
#define HCI_DEBUG_EV_FSM_LM_RESP_AUTH           0x05
#define HCI_DEBUG_EV_FSM_LM_BDW                 0x06
#define HCI_DEBUG_EV_FSM_LM_MLK                 0x07
#define HCI_DEBUG_EV_FSM_ENC_SLAVE              0x08
#define HCI_DEBUG_EV_FSM_ENC_MASTER             0x09
#define HCI_DEBUG_EV_FSM_LM_RNR                 0x0a
#define HCI_DEBUG_EV_FSM_LM_AFH                 0x0b
#define HCI_DEBUG_EV_FSM_LM_SCATTER             0x0c
#define HCI_DEBUG_EV_FSM_LM_ULP_MASTER_INIT     0x0d
#define HCI_DEBUG_EV_FSM_LM_ULP_MASTER_RESP     0x0e
#define HCI_DEBUG_EV_FSM_LM_ULP_SLAVE_INIT      0x0f
#define HCI_DEBUG_EV_FSM_LM_ULP_SLAVE_RESP      0x10
#define HCI_DEBUG_EV_FSM_UNKNOWN                0xffff

/* HCI_DEBUG_EV_ULP_ADV_CHANNEL */
#define HCI_DEBUG_EV_ULP_ADV_CHAN_CONN_REQ_PT1  0
#define HCI_DEBUG_EV_ULP_ADV_CHAN_CONN_REQ_PT2  1
/* Value or'ed into above to indicate a TX or RX  */
#define HCI_DEBUG_EV_ULP_ADV_CHAN_TX            0x80

#endif /* ifndef HCI_DEBUG_H__ */
