/****************************************************************************
FILE
        fm_api.h  -  constants and material relating to the host FM API

DESCRIPTION
    A standard interface (the "FM API") has been defined for the host to
    talk to the firmware which controls the FM radio introduced on
    John Peel (BC5-FM). The protocol can be run over one of several
    transports, such as I2C, BCCMD
    On Jemima (BC7-FM) The I2C interface has been deleted.

    This header file defines protocol constants such as message numbers.
    (It does not provide any way of getting at an implementation of the
    API.)

    It also defines other public values relating to the FM radio, such as
    enumeration constants for PSKEYs.

MODIFICATION HISTORY
    #1   06:mar:06  sk03    B-12548: Move fm_api.h into interface directory.
    #2   06:mar:06  sk03    B-12548: Shuffle bits between fm.h and fm_api.h.
    #3   07:mar:06  jn01    B-12548: Another enum, fix for psenum.h.
    #5   12:apr:06  sk03    B-13010: Added AUDIO_ROUTING control to FM API.
    #6   24:may:06  sk03    B-12692: Prepare FM code for audio aliasing.
    #7   24:may:06  sk03    B-12692: Initial attempt at integrating alias.
    #8   20:jun:06  jn01    B-14671: Tidying MISC definitions; comments.
    #9   18:jul:06  sk03    B-14719: Added audio gain command and tidied.
    #10  07:aug:06  pb04    B-15798: Add extra PCM hard mute functionality.
    #11  11:aug:06  jn01    B-16064: Add support for paged MISC reads.
    #12  11:aug:06  jn01    B-14665,B-16066: Add tone rejection and HILO read
    #13  11:aug:06  jn01    B-15561: More bits for PSKEY_FM_API_OPTIONS.
    #14  11:aug:06  sk03    B-16070: Add PCM intermediate modes.
    #15  13:aug:06  sk03    B-15798: Add PCM interface mutes.
    #16  14:aug:06  sk03    B-16070: Better auto intermediate mode control.
    #17  07:sep:06  jn01    CommentChange: Maintenance note.
    #18  25:sep:06  jn01    B-17014: Add SNR register / misc read page.
    ------------------------------------------------------------------
    --- This modification history is now closed. Do not update it. ---
    ------------------------------------------------------------------

REFERENCES
    The protocol is defined in CS-101760-SP (aka bcore-sp-020P) /
    CS-101761-SP (aka bcore-sp-021P). At the time of writing, this
    time of writing, this header file reflects bcore-sp-020Pe plus
    as-yet-unpublished additions.

*/

#ifndef __FM_API_H__
#define __FM_API_H__


/* NOTE: we do NOT #include "bt.h" here, because this header file will
 * be #included in psenum.h on the host side, which needs to minimise
 * extraneous stuff; and we don't need it, anyway. */

/*
 * "Addresses" of FM API registers.
 * These values are significant in as much as the address after that of
 * a 2-octet register is always unused (over I2C, it refers to the MSBs
 * of the register, but no-one should rely on that).
 */

enum fm_api_subaddr_enum {
    FM_API_SUBADDR_FLAG             = 0x00,    /* r/o, 16 bits */
    FM_API_SUBADDR_INT_MASK         = 0x02,    /* r/w, 16 bits */
    FM_API_SUBADDR_STATUS           = 0x04,    /* r/o, 8 bits */
    FM_API_SUBADDR_RSSI_LVL         = 0x05,    /* r/o, 8 bits */
    FM_API_SUBADDR_IF_COUNT         = 0x06,    /* r/o, 16 bits */
    FM_API_SUBADDR_RSSI_LIMIT       = 0x08,    /* r/w, 8 bits */
    FM_API_SUBADDR_IF_LIMIT         = 0x09,    /* r/w, 8 bits */
    FM_API_SUBADDR_SEARCH_LVL       = 0x0a,    /* r/w, 8 bits */
    FM_API_SUBADDR_SEARCH_DIR       = 0x0b,    /* r/w, 8 bits */
    FM_API_SUBADDR_FREQ             = 0x0c,    /* r/w, 16 bits */
    FM_API_SUBADDR_AF_FREQ          = 0x0e,    /* r/w, 16 bits */
    FM_API_SUBADDR_TUNER_MODE       = 0x10,    /* r/w, 8 bits */
    FM_API_SUBADDR_MUTE_STATE       = 0x11,    /* r/w, 8 bits */
    FM_API_SUBADDR_MOST_MODE        = 0x12,    /* r/w, 8 bits */
    FM_API_SUBADDR_MOST_BLEND       = 0x13,    /* r/w, 8 bits */
    FM_API_SUBADDR_PAUSE_LVL        = 0x14,    /* r/w, 8 bits */
    FM_API_SUBADDR_PAUSE_DUR        = 0x15,    /* r/w, 8 bits */
    FM_API_SUBADDR_MISC             = 0x16,    /* r/w, 16 bits */
    FM_API_SUBADDR_MAN_ID           = 0x18,    /* r/o, 16 bits */
    FM_API_SUBADDR_ASIC_ID          = 0x1a,    /* r/o, 16 bits */
    FM_API_SUBADDR_FIRM_VER         = 0x1c,    /* r/o, 16 bits */
    FM_API_SUBADDR_ASIC_VER         = 0x1e,    /* r/o, 8 bits */
    FM_API_SUBADDR_BAND             = 0x1f,    /* r/w, 8 bits */
    FM_API_SUBADDR_HILO             = 0x20,    /* r/w, 8 bits */
    FM_API_SUBADDR_DEMPH_MODE       = 0x21,    /* r/w, 8 bits */
    FM_API_SUBADDR_RDS_SYSTEM       = 0x22,    /* r/w, 8 bits */
    FM_API_SUBADDR_RDS_MEM          = 0x23,    /* r/w, 8 bits */
    FM_API_SUBADDR_POWER            = 0x24,    /* r/w, 8 bits */
    FM_API_SUBADDR_RDS_CNTR         = 0x25,    /* write-only, 8 bits */
    FM_API_SUBADDR_RDS_BLK_B        = 0x26,    /* r/w, 16 bits */
    FM_API_SUBADDR_RDS_MSK_B        = 0x28,    /* r/w, 16 bits */
    FM_API_SUBADDR_RDS_PI           = 0x2a,    /* r/w, 16 bits */
    FM_API_SUBADDR_RDS_PI_MASK      = 0x2c,    /* r/w, 16 bits */

    /* The following commands have been added since the I2C hardware was
     * defined and so are not supported by the I2C interface. */
    FM_API_SUBADDR_AUDIO_ROUTING    = 0x30,    /* r/w, 16 bits, not on I2C */
    FM_API_SUBADDR_AUDIO_GAIN       = 0x32,    /* r/w, 16 bits, not on I2C */
    FM_API_SUBADDR_FORCE_MUTE       = 0x34,    /* r/w, 16 bits, not on I2C */
    FM_API_SUBADDR_INTERMEDIATE_STATE=0x36,    /* r/w, 8 bits, not on I2C */
    FM_API_SUBADDR_FM_PARAMS        = 0x38,    /* r/o, 16 bits, not on I2C */
    FM_API_SUBADDR_SNR              = 0x3a,    /* r/o, 16 bits, not on I2C */
    FM_API_SUBADDR_RSSI_END         = 0x3c,    /* r/w, 16 bits, not on I2C */
    FM_API_SUBADDR_AUDIO_IO         = 0x3e,    /* o/w, 8 bits, not on I2C */
    FM_API_SUBADDR_DEMOD_IF         = 0x3f,    /* r/o, 16 bits, not on I2C */

    /* Note about adding new registers:
     * Historically, the address after that of a 16-bit register has been
     * reserved due to the design of the BC5-FM I2C hardware.
     * To preserve the possibility of supporting  I2C hardware in a future
     * chip to encompass new registers, we should continue this policy.
     * Thus it's recommended only to use even addresses for new registers. */

    FM_API_SUBADDR_RDS_DATA         = 0x80,    /* r/o, MMU buffer */
    FM_API_SUBADDR_INVALID          = 0xff     /* Not a valid API sub-addr */
};

/* Definition of the bits within the FLAG soft register */

enum fm_api_flags_enum {
    FM_API_FLAGS_FR                     = (int)0x0001,
    FM_API_FLAGS_BL                     = (int)0x0002,
    FM_API_FLAGS_IF                     = (int)0x0004,
    FM_API_FLAGS_LVL                    = (int)0x0008,
    FM_API_FLAGS_SYNC                   = (int)0x0010,
    FM_API_FLAGS_RDS                    = (int)0x0020,
    FM_API_FLAGS_BB                     = (int)0x0040,
    FM_API_FLAGS_PI                     = (int)0x0080,
    FM_API_FLAGS_PD                     = (int)0x0100,
    FM_API_FLAGS_STR                    = (int)0x0200,
    FM_API_FLAGS_RSSI                   = (int)0x0400,
    FM_API_FLAGS_RST                    = (int)0x8000
};

/* Definitions of bits within the FLAG soft register */

enum fm_api_int_mask_enum {
    FM_API_INT_MASK_FR_MASK             = (int)0x0001,
    FM_API_INT_MASK_BL_MASK             = (int)0x0002,
    FM_API_INT_MASK_IF_MASK             = (int)0x0004,
    FM_API_INT_MASK_LVL_MASK            = (int)0x0008,
    FM_API_INT_MASK_SYN_MASK            = (int)0x0010,
    FM_API_INT_MASK_RDS_MASK            = (int)0x0020,
    FM_API_INT_MASK_BB_MASK             = (int)0x0040,
    FM_API_INT_MASK_PI_MASK             = (int)0x0080,
    FM_API_INT_MASK_PD_MASK             = (int)0x0100,
    FM_API_INT_MASK_STR_MASK            = (int)0x0200,
    FM_API_INT_MASK_RSSI_MASK           = (int)0x0400
};

/* Definitions of bits within the STATUS soft register */

enum fm_api_status_enum {
    FM_API_STATUS_TS_MASK               = (int)0x0007,
    FM_API_STATUS_RDA_MASK              = (int)0x0008,
    FM_API_STATUS_STR_MASK              = (int)0x0010,
    FM_API_STATUS_IFR_MASK              = (int)0x0020,
    FM_API_STATUS_RSR_MASK              = (int)0x0040
};

/* Definitions of values of STATUS:TS[2:0] field */

typedef enum {
    FM_API_STATUS_TS_RESET              = (int)0,
    FM_API_STATUS_TS_NOT_TUNED          = (int)1,
    FM_API_STATUS_TS_TUNING             = (int)2,
    FM_API_STATUS_TS_TUNED_RDS_DIS      = (int)3,
    FM_API_STATUS_TS_TUNED_NO_RDS       = (int)4,
    FM_API_STATUS_TS_TUNED_RDS_UNSYNC   = (int)5,
    FM_API_STATUS_TS_TUNED_RDS_OK       = (int)6
} fm_api_status_ts_enum;

/* Definitions of bits within the SEARCH_DIR soft register */

enum fm_api_search_dir_enum {
    FM_API_SEARCH_DIR_SRD_MASK          = (int)0x01
};

/* Definitions of bits within the TUNER_MODE soft register */

enum fm_api_tuner_mode_enum {
    FM_API_TUNER_MODE_TM_MASK           = (int)0x07,
    FM_API_TUNER_MODE_FSD_MASK          = (int)0x10,
    FM_API_TUNER_MODE_FDD_MASK          = (int)0x20,
    FM_API_TUNER_MODE_SKP_MASK          = (int)0x40,
    FM_API_TUNER_MODE_AS_MASK           = (int)0x80
};

/* Definitions of values of TUNER_MODE:TM[1:0] field */

enum fm_api_tuner_mode_tm_enum {
    FM_API_TUNER_MODE_TM_NONE           = (int)0,
    FM_API_TUNER_MODE_TM_PRESET         = (int)1,
    FM_API_TUNER_MODE_TM_SEARCH         = (int)2,
    FM_API_TUNER_MODE_TM_AF_JUMP        = (int)3,
    FM_API_TUNER_MODE_TM_RSSI_SCAN      = (int)4
};

/* Definitions of bits within the MUTE_STATE soft register */

enum fm_api_mute_state_enum {
    FM_API_MUTE_STATE_MSS_MASK          = (int)0x01,
    FM_API_MUTE_STATE_MSA_MASK          = (int)0x02,
    FM_API_MUTE_STATE_MSL_MASK          = (int)0x04,
    FM_API_MUTE_STATE_MSR_MASK          = (int)0x08
};

/* Definitions of bits within the MOST_MODE soft register */

enum fm_api_most_mode_enum {
    FM_API_MOST_MODE_STM_MASK           = (int)0x01
};

/* Definitions of bits within the MOST_BLEND soft register */

enum fm_api_most_blend_enum {
    FM_API_MOST_BLEND_STB_MASK          = (int)0x01
};

/* Definitions of fields within the MISC soft register */

enum fm_api_misc_enum {
    FM_API_MISC_MODE_MASK               = (int)0xf000
};

#define FM_API_MISC_MODE_PACK(mode) \
    (((mode) << 12u) & FM_API_MISC_MODE_MASK)
#define FM_API_MISC_MODE_UNPACK(reg) \
    (((reg) & FM_API_MISC_MODE_MASK) >> 12u)

/* MISC page ("mode") definitions */
enum fm_api_misc_mode_enum {
    /* These are read/write pages. */
    FM_API_MISC_MODE_AUDIO_ROUTING      = (int)0x0000,
    FM_API_MISC_MODE_FM_CONTROL         = (int)0x0001,
    FM_API_MISC_MODE_RDS_CONTROL        = (int)0x0002,
    FM_API_MISC_MODE_AUDIO_GAIN         = (int)0x0003,
    FM_API_MISC_MODE_FORCE_MUTE         = (int)0x0004,
    FM_API_MISC_MODE_INTERMEDIATE_STATE = (int)0x0005,
    FM_API_MISC_MODE_READ_PAGE          = (int)0x000f,
    /* Pages 0x10 and above are read-only. */
    FM_API_MISC_MODE_FM_PARAMS          = (int)0x0010,
    FM_API_MISC_MODE_SNR                = (int)0x0011
};

/* Definitions pertaining to particular MISC pages: */

/* MISC/AUDIO_ROUTING */
enum fm_api_misc_routing_enum {
    /* Operating mode for the audio interfaces. */
    FM_API_MISC_ROUTING_PCM1M_LSB_POSN  = 0,
    FM_API_MISC_ROUTING_PCM1M_MSB_POSN  = 1,
    FM_API_MISC_ROUTING_PCM2M_LSB_POSN  = 2,
    FM_API_MISC_ROUTING_PCM2M_MSB_POSN  = 3,
    FM_API_MISC_ROUTING_CODECM_POSN     = 4,
    FM_API_MISC_ROUTING_HOSTIOM_POSN    = 5,
    /* Voice slots to be used by the interfaces. */
    FM_API_MISC_ROUTING_PCM1VS_LSB_POSN = 6,
    FM_API_MISC_ROUTING_PCM1VS_MSB_POSN = 7,
    FM_API_MISC_ROUTING_PCM2VS_LSB_POSN = 8,
    FM_API_MISC_ROUTING_PCM2VS_MSB_POSN = 9,
    FM_API_MISC_ROUTING_CODECVS_LSB_POSN= 10,
    FM_API_MISC_ROUTING_CODECVS_MSB_POSN= 11,
    /* Masks for the above fields. */
    FM_API_MISC_ROUTING_PCM1_MODE_MASK  = (int)0x0003,
    FM_API_MISC_ROUTING_PCM2_MODE_MASK  = (int)0x000c,
    FM_API_MISC_ROUTING_CODEC_MODE_MASK = (int)0x0010,
    FM_API_MISC_ROUTING_HOSTIO_MODE_MASK= (int)0x0020,
    FM_API_MISC_ROUTING_PCM1_SLOT_MASK  = (int)0x00c0,
    FM_API_MISC_ROUTING_PCM2_SLOT_MASK  = (int)0x0300,
    FM_API_MISC_ROUTING_CODEC_SLOT_MASK = (int)0x0c00
};

/* MISC/FM_CONTROL */
enum fm_api_misc_fm_enum {
    FM_API_MISC_FM_AHL_MASK             = (int)0x01,
    FM_API_MISC_FM_NEB_MASK             = (int)0x02,
    FM_API_MISC_FM_NTR_MASK             = (int)0x04
};

/* MISC/RDS_CONTROL */
/* (the same bit definitions are also used for the
 * RDS_CNTR first-class register) */
enum fm_api_misc_rds_enum {
    FM_API_MISC_RDS_FLS_MASK            = (int)0x01,
    FM_API_MISC_RDS_UNS_MASK            = (int)0x02
};

/* MISC/AUDIO_GAIN */
enum fm_api_misc_gain_enum {
    FM_API_MISC_GAIN_PCM1_LSB_POSN      = 0,
    FM_API_MISC_GAIN_PCM1_MSB_POSN      = 2,
    FM_API_MISC_GAIN_PCM2_LSB_POSN      = 3,
    FM_API_MISC_GAIN_PCM2_MSB_POSN      = 5,
    FM_API_MISC_GAIN_CODEC_LSB_POSN     = 6,
    FM_API_MISC_GAIN_CODEC_MSB_POSN     = 10,
    FM_API_MISC_GAIN_PCM1_MASK          = (int)0x0007,
    FM_API_MISC_GAIN_PCM2_MASK          = (int)0x0038,
    FM_API_MISC_GAIN_CODEC_MASK         = (int)0x07c0
};

/* MISC/FORCE_MUTE */
enum fm_api_misc_force_mute_enum {
    FM_API_MISC_FORCE_MUTE_PCM1_LEFT_MASK     = 0x0001,
    FM_API_MISC_FORCE_MUTE_PCM1_RIGHT_MASK    = 0x0002,
    FM_API_MISC_FORCE_MUTE_PCM1_INTERF_MASK   = 0x0004,
    FM_API_MISC_FORCE_MUTE_PCM2_LEFT_MASK     = 0x0008,
    FM_API_MISC_FORCE_MUTE_PCM2_RIGHT_MASK    = 0x0010,
    FM_API_MISC_FORCE_MUTE_PCM2_INTERF_MASK   = 0x0020,
    FM_API_MISC_FORCE_MUTE_CODEC_LEFT_MASK    = 0x0040,
    FM_API_MISC_FORCE_MUTE_CODEC_RIGHT_MASK   = 0x0080
};

/* MISC/FM_PARAMS (read-only) */
enum fm_api_misc_fm_params_enum {
    FM_API_MISC_FM_PARAMS_HISIDE_MASK         = 0x0001,
    FM_API_MISC_FM_PARAMS_TONEREJ_MASK        = 0x0002
};

/* Definitions of bits within the BAND soft register */

enum fm_api_band_enum {
    FM_API_BAND_BND_MASK                = (int)0x01
};

/* Definitions of bits within the HILO soft register */

enum fm_api_hilo_enum {
    FM_API_HILO_HLM_MASK                = (int)0x01,
    /* NB: the following bit is not implemented in the
     * I2C hardware; use the MISC/FM_CONTROL instead. */
    FM_API_HILO_AHL_MASK                = (int)0x02
};

/* Definitions of bits within the DEMPH soft register */

enum fm_api_demph_enum {
    FM_API_DEMPH_DMF_MASK               = (int)0x01
};

/* Definitions of bits within the RDS_SYSTEM soft register */

enum fm_api_rds_system_enum {
    FM_API_RDS_SYSTEM_RDS_MASK          = (int)0x01,
    /* NB: the following bit is not implemented in the
     * I2C hardware; use the MISC/FM_CONTROL instead. */
    FM_API_RDS_SYSTEM_NEB_MASK          = (int)0x02
};

/* Definitions of bits within the POWER soft register */

enum fm_api_power_enum {
    FM_API_POWER_PWF_MASK               = (int)0x01,
    FM_API_POWER_PWR_MASK               = (int)0x02
};

/* Definitions of bits within the AUDIO_GAIN soft register */

enum fm_api_audio_gain_enum {
    FM_API_AUDIO_GAIN_PCM1_LSB_POSN     = 0,
    FM_API_AUDIO_GAIN_PCM1_MSB_POSN     = 2,
    FM_API_AUDIO_GAIN_PCM2_LSB_POSN     = 3,
    FM_API_AUDIO_GAIN_PCM2_MSB_POSN     = 5,
    FM_API_AUDIO_GAIN_CODEC_L_LSB_POSN  = 6,
    FM_API_AUDIO_GAIN_CODEC_L_MSB_POSN  = 10,
    FM_API_AUDIO_GAIN_CODEC_R_LSB_POSN  = 11,
    FM_API_AUDIO_GAIN_CODEC_R_MSB_POSN  = 15,
    FM_API_AUDIO_GAIN_PCM1_MASK         = (int)0x0007,
    FM_API_AUDIO_GAIN_PCM2_MASK         = (int)0x0038,
    FM_API_AUDIO_GAIN_CODEC_L_MASK      = (int)0x07c0,
    FM_API_AUDIO_GAIN_CODEC_R_MASK      = (int)0xf800
};

/* Definitions of bits within the INTERMEDIATE_STATE soft register */

enum fm_api_intermediate_state_enum {
    FM_API_INTERMEDIATE_STATE_PCM1_LSB_POSN  = (int)0x0000,
    FM_API_INTERMEDIATE_STATE_PCM1_MSB_POSN  = (int)0x0003,
    FM_API_INTERMEDIATE_STATE_PCM2_LSB_POSN  = (int)0x0004,
    FM_API_INTERMEDIATE_STATE_PCM2_MSB_POSN  = (int)0x0007,
    FM_API_INTERMEDIATE_STATE_PCM1_MASK      = (int)0x000f,
    FM_API_INTERMEDIATE_STATE_PCM2_MASK      = (int)0x00f0,
    FM_API_INTERMEDIATE_STATE_PCM1_AUTO_MASK = (int)0x0100,
    FM_API_INTERMEDIATE_STATE_PCM2_AUTO_MASK = (int)0x0200,
    FM_API_INTERMEDIATE_STATE_NO_CHANGE      = 0,
    FM_API_INTERMEDIATE_STATE_DISABLE        = 1,
    FM_API_INTERMEDIATE_STATE_PCM_MASTER     = 2,
    FM_API_INTERMEDIATE_STATE_PCM_SLAVE      = 3,
    FM_API_INTERMEDIATE_STATE_I2S_MASTER     = 4,
    FM_API_INTERMEDIATE_STATE_I2S_SLAVE      = 5,
    FM_API_INTERMEDIATE_STATE_TRISTATED      = 6
};

/* Definitions of bits within the AUDIO_IO soft register */
enum fm_api_audio_io_enum {
    FM_API_RX_USE_ANALOGUE_MASK         = (int)0x01,
    FM_API_RX_USE_DIGITAL_MASK          = (int)0x02
};


/* Definitions of bits within the RDS_DATA status word */

enum fm_api_rds_data_enum {
    FM_API_RDS_DATA_BLOCKTYPE_POSN      = (int)0x00,
    FM_API_RDS_DATA_ERRORS_POSN         = (int)0x03,
    FM_API_RDS_DATA_AVAILABLE_MASK      = (int)0x20
};
typedef enum {
    FM_API_RDS_BLKTYPE_A                = (int)0x00,
    FM_API_RDS_BLKTYPE_B                = (int)0x01,
    FM_API_RDS_BLKTYPE_C                = (int)0x02,
    FM_API_RDS_BLKTYPE_C2               = (int)0x03,
    FM_API_RDS_BLKTYPE_D                = (int)0x04,
    FM_API_RDS_BLKTYPE_E                = (int)0x05,
    FM_API_RDS_BLKTYPE_E_INVALID        = (int)0x06,
    FM_API_RDS_BLKTYPE_INVALID          = (int)0x07
} fm_api_rds_block_type_enum;
typedef enum {
    FM_API_RDS_ERRORS_NONE              = (int)0x00,
    FM_API_RDS_ERRORS_2_CORRECTED       = (int)0x01,
    FM_API_RDS_ERRORS_5_CORRECTED       = (int)0x02,
    FM_API_RDS_ERRORS_UNCORRECTABLE     = (int)0x03
} fm_api_rds_errors_enum;

/* Define the format of the RDS_DATA register */
#define FM_API_RDS_BLOCK_SIZE           3
#define FM_API_RDS_BLOCK_OFFSET_BLK_LSB 0
#define FM_API_RDS_BLOCK_OFFSET_BLK_MSB 1
#define FM_API_RDS_BLOCK_OFFSET_STATUS  2

/* Maximum allowed depth of RDS buffer (in blocks) */
#define FM_MAX_RDS_MEM_DEPTH            (85)

/***************************************************************************/

/*
 * Ancillary values used in PSKEYs, etc.
 */

/* Error codes used when using HCI FM API.
 (16 bit enum because xap2 has 16bit registers) */
typedef enum {
    FM_REG_OK                           = (int)0x0000,
    FM_REG_UNKNOWN                      = (int)0x0001, /* unknown FM register */
    FM_REG_READ_ONLY                    = (int)0x0002,
    FM_REG_WRITE_ONLY                   = (int)0x0003,
    FM_REG_ERROR                        = (int)0xffff /* none of the above */
} fm_reg_status;

/* Select the FM control interface to be used. */
typedef enum {
    FM_API_TRANSPORT_NONE               = (int)0x00,
    FM_API_TRANSPORT_I2C                = (int)0x01,
    FM_API_TRANSPORT_BCCMD              = (int)0x02,
    FM_API_TRANSPORT_MAX                = (int)0x02
} fm_api_transport_t;

/* FM API configuration bits (PSKEY_FM_API_OPTIONS) */
typedef enum {
    FM_API_OPTION_PRESERVE_INT_MASK     = (int)0x0001,
    FM_API_OPTION_PRESERVE_FLAG         = (int)0x0002,
    FM_API_OPTION_PRESERVE_SUB_ADDR     = (int)0x0004,
    FM_API_OPTION_RDS_TRANSFER_MODE     = (int)0x0008, /* BC PUSH */
    FM_API_OPTION_EVENT_DETECT_MODE     = (int)0x0010, /* EDGE */
    FM_API_OPTION_FLRST_AT_BOOT         = (int)0x0020,
    FM_API_OPTION_FLRST_CAL_COMPLETE    = (int)0x0040,
    FM_API_OPTION_FLRST_OVER_HCI        = (int)0x0080
} fm_api_options_t;

/* Routing options for the FM audio */
/* These values are used internally, in the MISC/AUDIO_ROUTING register,
 * in PSKEY_FM_AUDIO_ROUTING, ... */

typedef enum {
    FM_PCM_ENABLE_MODE_OFF              = (int)0x00,
    FM_PCM_ENABLE_MODE_PCM              = (int)0x01,
    FM_PCM_ENABLE_MODE_I2S              = (int)0x02,
    FM_PCM_ENABLE_MODE_I2S_SLAVE        = (int)0x03
} fm_pcm_enable_mode_enum;

typedef enum {
    FM_API_VOICE_SLOTS_DEFAULT          = (int)0x00,
    FM_API_VOICE_SLOTS_01               = (int)0x01,
    FM_API_VOICE_SLOTS_23               = (int)0x02,
    FM_API_VOICE_SLOTS_45               = (int)0x03
} fm_api_voice_slots;

/* Error codes used when using HCI FM API.
 (16 bit enum because xap2 has 16bit registers) */
typedef enum {
    FMTX_REG_OK                           = (int)0x0000,
    FMTX_REG_UNKNOWN                      = (int)0x0001, /* unknown FM register */
    FMTX_REG_READ_ONLY                    = (int)0x0002,
    FMTX_REG_WRITE_ONLY                   = (int)0x0003,
    FMTX_REG_ERROR                        = (int)0xffff /* none of the above */
} fmtx_reg_status;

enum fmtx_api_subaddr_enum {
    FMTX_API_SUBADDR_POWER            = 0x00,    /* r/w, 8 bits */
    FMTX_API_SUBADDR_PWRLVL           = 0x01,    /* r/w, 16 bits */
    FMTX_API_SUBADDR_FREQ             = 0x02,    /* r/w, 16 bits */
    FMTX_API_SUBADDR_AUDIO_DEV        = 0x03,    /* r/w, 8 bits */
    FMTX_API_SUBADDR_PILOT_DEV        = 0x04,    /* r/w, 8 bits */
    FMTX_API_SUBADDR_FORCE_MONO       = 0x05,    /* r/w, 8 bits */
    FMTX_API_SUBADDR_PREEMP           = 0x06,    /* r/w, 8 bits */
    FMTX_API_SUBADDR_MUTE_STATE       = 0x07,    /* r/w, 8 bits */
    FMTX_API_SUBADDR_MPX_LMT          = 0x08,    /* r/w, 8 bits */
    FMTX_API_SUBADDR_PI               = 0x09,    /* r/w, 16 bits */
    FMTX_API_SUBADDR_PTY              = 0x0A,    /* r/w, 8 bits */
    FMTX_API_SUBADDR_AF_FREQ          = 0x0B,    /* r/w, 16 bits */
    FMTX_API_SUBADDR_DISP_MODE        = 0x0C,    /* r/w, 8 bits */
    FMTX_API_SUBADDR_DISP_UPD         = 0x0D,    /* r/w, 8 bits */
    FMTX_API_SUBADDR_RDS_DEV          = 0x0E,    /* r/w, 8 bits */
    FMTX_API_SUBADDR_DRC_MODE         = 0x0F,    /* r/w, 8 bits */
    FMTX_API_SUBADDR_ANA_TEST         = 0x10,    /* r/w, 8 bits */
    FMTX_API_SUBADDR_ANA_LVL          = 0x11,    /* r/o, 16 bits */
    FMTX_API_SUBADDR_TUNER_MODE       = 0x12,    /* r/w, 8 bits */
    FMTX_API_SUBADDR_STATUS           = 0x13,    /* r /o   , 8 bits */
    FMTX_API_SUBADDR_ANT_CHK          = 0x14,    /* r /o    , 8 bits */
    FMTX_API_SUBADDR_AUDIO_RANGE      = 0x15,    /* o/w,   8 bits */
    FMTX_API_SUBADDR_REF_ERR          = 0x16,    /* o/w  , 16 bits */
    FMTX_API_SUBADDR_AUDIO_IO         = 0x17,    /* o/w  , 8 bits */
    FMTX_API_SUBADDR_DRC_CONFIG       = 0x18,    /* r/w  , 16 bits */
    FMTX_API_SUBADDR_FLAG             = 0x19,    /* r/o  , 16 bits */
    FMTX_API_SUBADDR_INT_MASK         = 0x1A,    /* r/o  , 16 bits */
    FMTX_API_SUBADDR_AUD_THRES_LOW    = 0x1B,    /* r/w  , 16 bits */
    FMTX_API_SUBADDR_AUD_THRES_HIGH   = 0x1C,    /* r/w  , 16 bits */
    FMTX_API_SUBADDR_AUD_DUR_LOW      = 0x1D,    /* r/w  , 16 bits */
    FMTX_API_SUBADDR_AUD_DUR_HIGH     = 0x1E,    /* r/w  , 16 bits */
    FMTX_API_SUBADDR_AUD_LVL_L        = 0x1F,    /* r/o  , 16 bits */
    FMTX_API_SUBADDR_AUD_LVL_R        = 0x20,    /* r/o  , 16 bits */
    FMTX_API_SUBADDR_DRC_CMP_THRESH   = 0x21,    /* r/w  , 16 bits */
    FMTX_API_SUBADDR_DRC_CMP_BOOST    = 0x22,    /* r/w  , 16 bits */
    FMTX_API_SUBADDR_DRC_CMP_LIMIT    = 0x23,    /* r/w  , 16 bits */
    FMTX_API_SUBADDR_RF_MUTE          = 0x24,    /* r/w  , 16 bits */
    FMTX_API_SUBADDR_INFO_BITS        = 0x25     /* r/w  , 8 bits */
};

/* Definitions of bits within the POWER soft register */
enum fmtx_api_power_enum {
    FMTX_API_POWER_PWFTX_MASK         = (int)0x01,
    FMTX_API_POWER_PWRTX_MASK         = (int)0x02
};

/* Definitions of bits within the PWRLVL soft register */
enum fmtx_api_pwrlvl_enum {
    FMTX_API_PWRLVL_LSB_POSN        = 0,
    FMTX_API_PWRLVL_MSB_POSN        = 15,
    FMTX_API_PWRLVL_MASK            = (int)0xffff
};

/* Definitions of bits within the MONO soft register */
enum fmtx_api_force_mono_enum {
    FMTX_API_FORCE_MONO_MO_MASK         = (int)0x01
};

/* Definitions of bits within the Pre-emphasis soft register */
enum fmtx_api_preemp_enum {
    FMTX_API_PREEMP_PREEN_MASK         = (int)0x01,
    FMTX_API_PREEMP_PRETC_MASK         = (int)0x02
};

/* Definitions of bits within the Pre-emphasis soft register */
enum fmtx_api_mute_state_enum {
    FMTX_API_MUTE_STATE_MS_MASK         = (int)0x01
};

/* Definitions of bits within the multiplex clip limit soft register */
enum fmtx_api_mpx_lmt_enum {
    FMTX_API_MPL_LMT_MPX_MASK         = (int)0x01
};

/* The multiplex clip can only be disabled by setting to max value */
#define FMTX_API_MPL_MAX ((uint16)0x7fff)

/* Definitions of bits within the PTY soft register */
enum fmtx_api_pty_enum {
    FMTX_API_PTY_LSB_POSN        = 0,
    FMTX_API_PTY_MSB_POSN        = 4,
    FMTX_API_PTY_MASK            = (int)0x001f
};

/* Definitions of bits within the DISPLAY MODE soft register */
enum fmtx_api_text_type_enum {
    FMTX_API_TEXT_TYPE_PS       = 0,
    FMTX_API_TEXT_TYPE_RT       = 1,
    FMTX_API_TEXT_TYPE_RAW      = 2,
    FMTX_API_TEXT_TYPE_RAW_SS   = 3  /* SS = SingleShot */
};

/* Definitions of bits within the DISPLAY MODE soft register */
enum fmtx_api_disp_mode_enum {
    FMTX_API_DISP_MODE_STATIC       = 0,
    FMTX_API_DISP_MODE_SCROLL       = 1,
    FMTX_API_DISP_MODE_PAGED        = 2,
    FMTX_API_DISP_MODE_MAX          = (int)0x0003,
    FMTX_API_DISP_MODE_MASK         = (int)0x0003
};

/* Definitions of bits within the Dyn. Range Copressor soft register */
enum fmtx_api_drc_enum {
    FMTX_API_DRC_EN_MASK         = (int)0x01,
    FMTX_API_DRC_SK_MASK         = (int)0x02
};

/* Definitions of bits within the Audio Test soft register */
enum fmtx_api_ana_tst_enum {
    FMTX_API_ANA_TST_EN_MASK         = (int)0x01,
    FMTX_API_ANA_TST_MC_MASK         = (int)0x02
};

/* Definitions of bits within the Analogue levels soft register */
enum fmtx_api_ana_lvl_enum {
    FMTX_API_ANA_LVLA_LSB_POSN        = 0,
    FMTX_API_ANA_LVLA_MSB_POSN        = 7,
    FMTX_API_ANA_LVLA_MASK            = (int)0x00FF,
    FMTX_API_ANA_LVLB_LSB_POSN        = 8,
    FMTX_API_ANA_LVLB_MSB_POSN        = 15,
    FMTX_API_ANA_LVLB_MASK            = (int)0xFF00
};

/* Definitions of bits within the AUDIO levels soft registers */
enum fmtx_api_aud_lvl_enum {
    FMTX_API_AUD_LVL_MASK            = (int)0x1FFF
};

/* Definitions of values of Tuner Mode field */
enum fmtx_api_tuner_mode{
    FM_API_TUNER_MODE_NONE        = (int)0x00,
    FM_API_TUNER_MODE_TUNE        = (int)0x01,
    FM_API_TUNER_MODE_CAL         = (int)0x02,
    FM_API_TUNER_MODE_MASK        = (int)0x03
    } ;

/* Definitions of values of STATUS:TS[2:0] field */
enum fmtx_api_status{
    FM_API_STATUS_TX_RESET        = (int)0x00,
    FM_API_STATUS_UNTUNED         = (int)0x01,
    FM_API_STATUS_TUNING          = (int)0x02,
    FM_API_STATUS_TUNED           = (int)0x03
    } ;

/* Definitions of values of ANTENNA Check register*/
enum fmtx_api_ant_chk_mode{
    FMTX_API_ANT_CHK_CAL_CMP_MASK        = (int)0x01,
    FMTX_API_ANT_CHK_CAL_STS_MASK        = (int)0x02
} ;

/* The audio range has the following max value */
#define FMTX_API_AUDIO_RANGE_MAX ((uint16) 22)

/* Definitions of values within the AUDIO_IO soft register */
enum fmtx_api_audio_io_enum {
    FM_API_TX_USE_ANALOGUE              = (int)0x00,
    FM_API_TX_USE_DIGITAL               = (int)0x01
};

/* Definitions of bits within the DRC_CONFIG soft registers */
/*Passed into the hardware, without processing individual fields */
enum fmtx_api_drc_config_enum {
    FMTX_API_DRC_CONFIG_MASK            = (int)0x03FF
};

/* Definition of the bits within the FLAG soft register */

enum fmtx_api_flags_enum {
    FMTX_API_FLAGS_FR                     = (int)0x0001,
    FMTX_API_FLAGS_PAF                    = (int)0x0002,
    FMTX_API_FLAGS_FLALL                  = (int)0x0004,
    FMTX_API_FLAGS_FLALH                  = (int)0x0008,
    FMTX_API_FLAGS_FLARL                  = (int)0x0010,
    FMTX_API_FLAGS_FLARH                  = (int)0x0020,
    FMTX_API_FLAGS_FLTOM                  = (int)0x0040,
    FMTX_API_FLAGS_RST                    = (int)0x8000
};

/* Definitions of bits within the FLAG soft register */

enum fmtx_api_int_mask_enum {
    FMTX_API_INT_MASK_FR                     = (int)0x0001,
    FMTX_API_INT_MASK_PAF                    = (int)0x0002,
    FMTX_API_INT_MASK_FLALL                  = (int)0x0004,
    FMTX_API_INT_MASK_FLALH                  = (int)0x0008,
    FMTX_API_INT_MASK_FLARL                  = (int)0x0010,
    FMTX_API_INT_MASK_FLARH                  = (int)0x0020,
    FMTX_API_INT_MASK_FLTOM                  = (int)0x0040,
    FMTX_API_INT_MASK_RST                    = (int)0x8000
};

/* FM API configuration bits (PSKEY_FMTX_API_OPTIONS) */
typedef enum {
    FMTX_API_OPTION_PRESERVE_INT_MASK     = (int)0x0001,
    FMTX_API_OPTION_PRESERVE_FLAG         = (int)0x0002
} fmtx_api_options_t;

/* Definitions of bits within the DRC_CMP_THRESH soft registers */
enum fmtx_api_drc_cmp_thresh_enum {
    FMTX_API_DRC_CMP_THRESH_MASK          = (int)0x0FFF
};

/* Definitions of bits within the DRC_CMP_BOOST soft registers */
enum fmtx_api_drc_cmp_boost_enum {
    FMTX_API_DRC_CMP_BOOST_MASK          = (int)0x07FF
};

/* Definitions of bits within the DRC_CMP_LIMIT soft registers */
enum fmtx_api_drc_cmp_limit_enum {
    FMTX_API_DRC_CMP_LIMIT_MASK          = (int)0x0FFF
};

/* Definitions of bits within the RF_MUTE soft registers */
enum fmtx_api_rf_mute_enum {
    FMTX_API_RF_MUTE_MASK                = (int)0x0001
};

/* Definitions of the bits within the info bits soft register */
enum fmtx_api_info_bits_enum {
    FMTX_API_INFO_TP_MASK                = (int)0x40,
    FMTX_API_INFO_TA_MASK                = (int)0x20,
    FMTX_API_INFO_MS_MASK                = (int)0x10,
    FMTX_API_INFO_DI_MASK                = (int)0x0F
};


#endif  /* __FM_API_H__ */
