/****************************************************************************
FILE
        fruitbat_fm_api.h  -  Fruitbat-specific FM API definitions

DESCRIPTION
    This file contains definitions for the Fruitbat-specific FM API used 
    over H4+ channel 8. 
    
REFERENCES
    Definitive specification is in Cognidox as CS-127992-SP 
*/

#ifndef __FM_FB_API_H__
#define __FM_FB_API_H__

/* Opcode IDs (register addresses) */

typedef enum fm_fb_api_opcode_enum
{
    FM_FB_API_CMD_RX_FREQ                   = 0x00,
    FM_FB_API_CMD_RX_RASTER                 = 0x01,
    FM_FB_API_CMD_RX_AF_FREQ                = 0x02,
    FM_FB_API_CMD_RX_MOST_MODE              = 0x03,
    FM_FB_API_CMD_RX_STEREO                 = 0x04,
    FM_FB_API_CMD_RX_MOST_BLEND             = 0x05,
    FM_FB_API_CMD_RX_DEMPH_MODE             = 0x06,
    FM_FB_API_CMD_RX_SEARCH_LVL             = 0x07,
    FM_FB_API_CMD_RX_RSSI_LVL               = 0x08,
    FM_FB_API_CMD_RX_BAND                   = 0x0A,
    FM_FB_API_CMD_RX_MUTE                   = 0x0B,
    FM_FB_API_CMD_RX_RDS_SYNC               = 0x0E,
    FM_FB_API_CMD_RX_FLAG                   = 0x0F,
    FM_FB_API_CMD_RX_RDS_DATA               = 0x10,
    FM_FB_API_CMD_RX_RDS_CNTRL              = 0x11,
    FM_FB_API_CMD_RX_RDS_MEM                = 0x12,
    FM_FB_API_CMD_RX_RDS_PI_MASK            = 0x13,
    FM_FB_API_CMD_RX_RDS_PI                 = 0x14,
    FM_FB_API_CMD_RX_INT_MASK               = 0x15,
    FM_FB_API_CMD_RX_TUNER_MODE             = 0x16,
    FM_FB_API_CMD_RX_SEARCH_DIR             = 0x17,
    FM_FB_API_CMD_GEN_POWER                 = 0x18,
    FM_FB_API_CMD_GEN_FIRM_VER              = 0x19,
    FM_FB_API_CMD_GEN_ASIC_VER              = 0x1A,
    FM_FB_API_CMD_GEN_ASIC_ID               = 0x1B,
    FM_FB_API_CMD_GEN_MAN_ID                = 0x1C,
    FM_FB_API_CMD_RX_LOWER_BAND_EDGE        = 0x1F,
    FM_FB_API_CMD_RX_UPPER_BAND_EDGE        = 0x20,
    FM_FB_API_CMD_RX_AUDIO_IO               = 0x33,
    FM_FB_API_CMD_TX_CHANL                  = 0x80,
    FM_FB_API_CMD_TX_RASTER                 = 0x81,
    FM_FB_API_CMD_TX_BAND                   = 0x82,
    FM_FB_API_CMD_TX_POWER_ENB              = 0x83,
    FM_FB_API_CMD_TX_POWER_LEV              = 0x84,
    FM_FB_API_CMD_TX_AUDIO_DEV              = 0x85,
    FM_FB_API_CMD_TX_PILOT_DEV              = 0x86,
    FM_FB_API_CMD_TX_RDS_DEV                = 0x87,
    FM_FB_API_CMD_GEN_MODE                  = 0x88,
    FM_FB_API_CMD_TX_AUDIO_IO               = 0x89,
    FM_FB_API_CMD_TX_PREMPH                 = 0x8A,
    FM_FB_API_CMD_TX_MONO                   = 0x8B,
    FM_FB_API_CMD_TX_MUTE                   = 0x8C,
    FM_FB_API_CMD_TX_MPX_LMT                = 0x8D,
    FM_FB_API_CMD_TX_AUDIO_DEV_LIMITER      = 0x8F,
    FM_FB_API_CMD_TX_AUDIO_LIMITER_ATTACK   = 0x90,
    FM_FB_API_CMD_TX_AUDIO_LIMITER_DECAY    = 0x91,
    FM_FB_API_CMD_TX_PI                     = 0x96,
    FM_FB_API_CMD_TX_PTY                    = 0x97,
    FM_FB_API_CMD_TX_PS_NAME                = 0x98,
    FM_FB_API_CMD_TX_RADIOTEXT              = 0x99,
    FM_FB_API_CMD_TX_AF_FREQ                = 0x9A,
    FM_FB_API_CMD_TX_DISPLAY_MODE           = 0x9B,
    FM_FB_API_CMD_TX_DISP_UPDATE_RATE       = 0x9C,
    FM_FB_API_CMD_TX_RDS_DATA_ENB           = 0x9E,
    FM_FB_API_CMD_TX_ANALOG_LEV_TEST        = 0xA0,
    FM_FB_API_CMD_TX_ANALOG_LEV_RESULT      = 0xA1,
    FM_FB_API_CMD_TX_ANALOG_RNG             = 0xA2,
    FM_FB_API_CMD_TX_FLAG                   = 0xA3,
    FM_FB_API_CMD_TX_INT_MASK               = 0xA4,
    FM_FB_API_CMD_TX_AUD_THRESH_LOW         = 0xA5,
    FM_FB_API_CMD_TX_AUD_THRESH_HIGH        = 0xA6,
    FM_FB_API_CMD_TX_AUD_DUR_LOW            = 0xA7,
    FM_FB_API_CMD_TX_AUD_DUR_HIGH           = 0xA8,
    FM_FB_API_CMD_TX_CHIRP_CONTROL          = 0xA9,
    FM_FB_API_CMD_TX_CHIRP_TONE_FREQ        = 0xAA,
    FM_FB_API_CMD_TX_CHIRP_START_CONF       = 0xAB,
    FM_FB_API_CMD_TX_CHIRP_TONE_MARK_TIME   = 0xAC,
    FM_FB_API_CMD_TX_CHIRP_TONE_SPACE_TIME  = 0xAD,
    FM_FB_API_CMD_TX_CHIRP_TONE_VOLUME      = 0xAE,
    FM_FB_API_CMD_TRX_RSSI_SCAN_BOTTOM_FREQ = 0xAF,
    FM_FB_API_CMD_TRX_RSSI_SCAN_TOP_FREQ    = 0xB0,
    FM_FB_API_CMD_TRX_RSSI_SCAN_START       = 0xB1,
    FM_FB_API_CMD_TRX_RSSI_SCAN_RESULT      = 0xB2,
    FM_FB_API_CMD_TRX_SCAN_ANTENNA_SELECT   = 0xB3,
    FM_FB_API_CMD_TX_CW_RAMP_TEST           = 0xB4,
    FM_FB_API_CMD_TRX_FMANT_TUNE_VALUE      = 0xB5,
    FM_FB_API_CMD_TX_LOWER_BAND_EDGE        = 0xB6,
    FM_FB_API_CMD_TX_UPPER_BAND_EDGE        = 0xB7,
    FM_FB_API_EVENT                         = 0xFF
} fm_fb_api_opcode;

/* General definitions */

typedef enum fm_fb_api_rws_enum
{
    FM_FB_API_WRITE  = 0,
    FM_FB_API_READ   = 1,
    FM_FB_API_STATUS = 2
} fm_fb_api_rws;

typedef enum fm_fb_api_result_enum
{
    FM_FB_API_SUCCESS = 0,
    FM_FB_API_FAIL    = 1
} fm_fb_api_result;

#define FM_FB_API_RDS_BLOCK_SIZE 3

/* RX_RDS_DATA definitions */

#define FM_FB_API_RDS_BLKTYPE_MASK  0x07
#define FM_FB_API_RDS_BLER_MASK     0x18
#define FM_FB_API_RDS_FIFO_STATUS   0x20
#define FM_FB_API_RDS_SYNC_INFO     0x40

typedef enum fm_fb_api_rds_blktype_enum
{
    FM_FB_API_RDS_BLKTYPE_A         = 0,
    FM_FB_API_RDS_BLKTYPE_B         = 1,
    FM_FB_API_RDS_BLKTYPE_C         = 2,
    FM_FB_API_RDS_BLKTYPE_C2        = 3,
    FM_FB_API_RDS_BLKTYPE_D         = 4,
    FM_FB_API_RDS_BLKTYPE_E         = 5,
    FM_FB_API_RDS_BLKTYPE_E_INVALID = 6,
    FM_FB_API_RDS_BLKTYPE_INVALID   = 7
} fm_fb_api_rds_blktype;

/* FM Tx events */

#define FM_FB_API_TX_EVENT_TUNE_COMPLETED   0x01
#define FM_FB_API_TX_EVENT_POW_EN           0x02
#define FM_FB_API_TX_EVENT_SCAN_OVER        0x04
#define FM_FB_API_TX_EVENT_SILENCE_DETECT   0x08
#define FM_FB_API_TX_EVENT_AUDIO_DETECT     0x10
#define FM_FB_API_TX_EVENT_OVERMOD          0x20
#define FM_FB_API_TX_EVENT_OVERDRIVE        0x40

/* GEN_POWER definitions */

#define FM_FB_API_GEN_POWER_FM_ON           0x01
#define FM_FB_API_GEN_POWER_RDS_ON          0x02

/* GEN_MODE definitions */

/* Note special case - RSSI scan in TX mode
 * This is a temporary internal mode, not directly 
 * selectable via the API
 */

typedef enum fm_fb_api_mode_enum
{
    FM_FB_API_MODE_RX = 0,
    FM_FB_API_MODE_TX = 1,
    FM_FB_API_MODE_TX_RSSI_SCAN = 2
} fm_fb_api_mode;

/* TRX_SCAN_ANTENNA_SELECT definitions */

typedef enum fm_fb_api_scan_antenna_select_enum
{
    FM_FB_API_SCAN_ANTENNA_SELECT_RX = 0,
    FM_FB_API_SCAN_ANTENNA_SELECT_TX = 1
} fm_fb_api_scan_antenna_select;

/* TX_RASTER definitions */

typedef enum fm_fb_api_tx_raster_enum
{
    FM_FB_API_TX_RASTER_50KHZ  = 0,
    FM_FB_API_TX_RASTER_100KHZ = 1,
    FM_FB_API_TX_RASTER_200KHZ = 2
} fm_fb_api_tx_raster;

/* TX_BAND definitions */

typedef enum fm_fb_api_tx_band_enum
{
    FM_FB_API_TX_BAND_EUR_USA = 0,
    FM_FB_API_TX_BAND_JAPAN   = 1
} fm_fb_api_tx_band;

/* TX_AUDIO_IO definitions */

typedef enum fm_fb_api_tx_audio_io_enum
{
    FM_FB_API_TX_AUDIO_IO_ANALOGUE = 0,
    FM_FB_API_TX_AUDIO_IO_DIGITAL  = 1
} fm_fb_api_tx_audio_io;

/* TX_PREMPH definitions */

typedef enum fm_fb_api_tx_premph_enum
{
    FM_FB_API_TX_PREMPH_50US = 0,
    FM_FB_API_TX_PREMPH_75US = 1,
    FM_FB_API_TX_PREMPH_OFF  = 2
} fm_fb_api_tx_premph;

/* TX_MONO definitions */

typedef enum fm_fb_api_tx_mono_mode_enum
{
    FM_FB_API_TX_MODE_MONO   = 0,
    FM_FB_API_TX_MODE_STEREO = 1
} fm_fb_api_tx_mono_mode;

/* TX_MUTE definitions */

typedef enum fm_fb_api_tx_mute_enum
{
    FM_FB_API_TX_MUTE   = 0,
    FM_FB_API_TX_UNMUTE = 1
} fm_fb_api_tx_mute;

/* TX_DISPLAY_MODE definitions */

typedef enum fm_fb_api_tx_display_mode_enum
{
    FM_FB_API_TX_DISPLAY_MODE_STATIC = 0,
    FM_FB_API_TX_DISPLAY_MODE_SCROLL = 1,
    FM_FB_API_TX_DISPLAY_MODE_PAGED = 2
} fm_fb_api_tx_display_mode;

/* TX_ANALOG_LEV_TEST definitions */

#define FM_FB_API_TX_ANA_LEV_TEST_ENABLE   0x01
#define FM_FB_API_TX_ANA_LEV_TEST_COMPLETE 0x02

/* TX_ANALOG_RNG definitions */

typedef enum fm_fb_api_tx_audio_range_enum
{
    FM_FB_API_TX_AUDIO_RANGE_100 = 0,
    FM_FB_API_TX_AUDIO_RANGE_200 = 1,
    FM_FB_API_TX_AUDIO_RANGE_300 = 2,
    FM_FB_API_TX_AUDIO_RANGE_UNKNOWN = 3
} fm_fb_api_tx_audio_range;

/* TX_CHIRP_CONTROL definitions */

typedef enum fm_fb_api_tx_chirp_control_enum
{
    FM_FB_API_TX_CHIRP_CONTROL_OFF  = 0,
    FM_FB_API_TX_CHIRP_CONTROL_ON   = 1,
    FM_FB_API_TX_CHIRP_CONTROL_AUTO = 2
} fm_fb_api_tx_chirp_control;

/* TX_CHIRP_START_CONF definitions */

typedef enum fm_fb_api_tx_chirp_start_conf_enum
{
    FM_FB_API_TX_CHIRP_START_MARK  = 0,
    FM_FB_API_TX_CHIRP_START_SPACE = 1
} fm_fb_api_tx_chirp_start_conf;

/* RX_RASTER definitions */

typedef enum fm_fb_api_rx_raster_enum
{
    FM_FB_API_RX_RASTER_50KHZ  = 0,
    FM_FB_API_RX_RASTER_100KHZ = 1,
    FM_FB_API_RX_RASTER_200KHZ = 2
} fm_fb_api_rx_raster;

/* RX_MOST_MODE definitions */

typedef enum fm_fb_api_rx_most_mode_enum
{
    FM_FB_API_RX_MOST_MODE_MONO   = 0,
    FM_FB_API_RX_MOST_MODE_STEREO = 1
} fm_fb_api_rx_most_mode;

/* RX_MOST_BLEND definitions */

typedef enum fm_fb_api_rx_most_blend_enum
{
    FM_FB_API_RX_MOST_BLEND_SOFT     = 0,
    FM_FB_API_RX_MOST_BLEND_SWITCHED = 1
} fm_fb_api_rx_most_blend;

/* RX_DEMPH definitions */

typedef enum fm_fb_api_rx_demph_enum
{
    FM_FB_API_RX_DEMPH_50US = 0,
    FM_FB_API_RX_DEMPH_75US = 1
} fm_fb_api_rx_demph;

/* RX_BAND definitions */

typedef enum fm_fb_api_rx_band_enum
{
    FM_FB_API_RX_BAND_EUR_USA = 0,
    FM_FB_API_RX_BAND_JAPAN   = 1
} fm_fb_api_rx_band;

/* RX_MUTE_STATUS definitions */

#define FM_FB_API_RX_MUTE_STATUS_SOFT_MUTE       0x01
#define FM_FB_API_RX_MUTE_STATUS_AC_MUTE         0x02
#define FM_FB_API_RX_MUTE_STATUS_HARD_MUTE_LEFT  0x04
#define FM_FB_API_RX_MUTE_STATUS_HARD_MUTE_RIGHT 0x08

/* RX_FLAG definitions */

#define FM_FB_API_RX_FLAG_FR          0x01
#define FM_FB_API_RX_FLAG_RDS         0x02
#define FM_FB_API_RX_FLAG_LSYNC       0x04
#define FM_FB_API_RX_FLAG_LEV         0x08
#define FM_FB_API_RX_FLAG_PI          0x10
#define FM_FB_API_RX_FLAG_RSSISCAN    0x20
#define FM_FB_API_RX_FLAG_MOST_CHANGE 0x40
#define FM_FB_API_RX_FLAG_PD          0x80

/* RX_RDS_CNTRL definitions */

#define FM_FB_API_RX_RDS_CNTRL_FLUSH  0x01

/* RX_INT_MASK definitions */

#define FM_FB_API_RX_INT_MASK_FR          0x01
#define FM_FB_API_RX_INT_MASK_RDS         0x02
#define FM_FB_API_RX_INT_MASK_LSYNC       0x04
#define FM_FB_API_RX_INT_MASK_LEV         0x08
#define FM_FB_API_RX_INT_MASK_PI          0x10
#define FM_FB_API_RX_INT_MASK_RSSISCAN    0x20
#define FM_FB_API_RX_INT_MASK_MOST_CHANGE 0x40
#define FM_FB_API_RX_INT_MASK_PD          0x80

/* RX_TUNER_MODE definitions */

typedef enum fm_fb_api_rx_tuner_mode_enum
{
    FM_FB_API_RX_TUNER_MODE_PRESET      = 0,
    FM_FB_API_RX_TUNER_MODE_AUTO_SEARCH = 1,
    FM_FB_API_RX_TUNER_MODE_AF_JUMP     = 2,
    FM_FB_API_RX_TUNER_MODE_STOP_SEARCH = 3
} fm_fb_api_rx_tuner_mode;

/* RX_SEARCH_DIR definitions */

typedef enum fm_fb_api_rx_search_dir_enum
{
    FM_FB_API_RX_SEARCH_DIR_UP   = 0,
    FM_FB_API_RX_SEARCH_DIR_DOWN = 1
} fm_fb_api_rx_search_dir;

/* RX_AUDIO_IO definitions */

typedef enum fm_fb_api_rx_audio_io_enum
{
    FM_FB_API_RX_AUDIO_IO_ANALOGUE = 0,
    FM_FB_API_RX_AUDIO_IO_DIGITAL  = 1
} fm_fb_api_rx_audio_io;

#endif /*__FM_FB_API_H__*/
