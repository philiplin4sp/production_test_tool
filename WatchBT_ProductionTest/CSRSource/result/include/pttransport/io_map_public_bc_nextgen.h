#ifndef _IO_MAP_PUBLIC_BC_NEXTGEN_H_
#define _IO_MAP_PUBLIC_BC_NEXTGEN_H_

/* *************************************************************************  *
   COMMERCIAL IN CONFIDENCE
   Copyright (C) Cambridge Silicon Radio Ltd 2001-2009

   http://www.csr.com

   $Id$
   $Name$
   $Source$

   DESCRIPTION
      Hardware declarations header file (lower level) for use by
      tools driving SPI (such as emulator), not the software stack.
      Lists memory mapped register addresses.

*  *************************************************************************  */
#define BIT(x) (1u << (x))

enum io_map_enum {
GBL_LPC_PCM2_MUX_CONFIG                 = 0xFA1C,   /* RW 2,11,15 bits depending on chip */

DBG_SPI_PROC_SELECT                     = 0xF830,

MMU_BT_GW1_CONFIG                       = 0xF8DD,  /* A   15 bits */
MMU_BT_GW2_CONFIG                       = 0xF8DE,  /* A   15 bits */

MMU_SPI_GW1_CONFIG		                = 0xF8F9,  /*  RW  15 bits */
MMU_SPI_GW2_CONFIG                      = 0xF8FA,  /*  RW  15 bits */
MMU_SPI_GW3_CONFIG                      = 0xF8FB,  /*  RW  15 bits */

XAP_PCH                                 = 0xFFE9,  /*  RW   8 bits */
XAP_PCL                                 = 0xFFEA,  /*  RW  16 bits */

GBL_CHIP_VERSION                        = 0xFE81,  /*  R   16 bits */

dummy

};

typedef enum io_map_enum io_map;

/* GBL_LPC_PCM2_MUX_CONFIG field positions. */
enum gbl_lpc_pcm2_mux_config_posn_enum
{
    GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_EN_POSN                = 0,
    /* JEM only */
    GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_SEL_POSN               = 1,
    /* GDN, DAL, LEO, OXY, GEM, PUR only */
    GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_DATA_SEL_LSB_POSN      = 1,
    GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_DATA_SEL_MSB_POSN      = 8,
    GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_CSB_SEL_POSN           = 9,
    /* GDN, DAL, LEO, OXY, GEM only */
    GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_KAL_SEL_POSN           = 10,
    /* PUR only */
    GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_KAL_DATA_SEL_LSB_POSN  = 10,
    GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_KAL_DATA_SEL_MSB_POSN  = 13,
    GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_KAL_CSB_SEL_POSN       = 14
};
/* GBL_LPC_PCM2_MUX_CONFIG bitmasks. */
static const int GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_EN_BM                         = BIT(0);
/* JEM only */
static const int GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_SEL_BM                        = BIT(1);
/* GDN, DAL, LEO, OXY, GEM, PUR only */
static const int GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_DATA_SEL_BM                   = BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(8);
static const int GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_DATA_SEL_XAP_LPC_D0_BM        = BIT(1);
static const int GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_DATA_SEL_XAP_LPC_D1_BM        = BIT(2);
static const int GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_DATA_SEL_XAP_LPC_D2_BM        = BIT(3);
static const int GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_DATA_SEL_XAP_LPC_D3_BM        = BIT(4);
static const int GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_DATA_SEL_XAP_LPC_D4_BM        = BIT(5);
static const int GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_DATA_SEL_XAP_LPC_D5_BM        = BIT(6);
static const int GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_DATA_SEL_XAP_LPC_D6_BM        = BIT(7);
static const int GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_DATA_SEL_XAP_LPC_D7_BM        = BIT(8);
static const int GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_CSB_SEL_BM                    = BIT(9);
/* GDN, DAL, LEO, OXY, GEM only */
static const int GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_KAL_SEL_BM                    = BIT(10);
/* PUR only */
static const int GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_KAL_DATA_SEL_BM               = BIT(10)|BIT(11)|BIT(12)|BIT(13);
static const int GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_KAL_DATA_SEL_KAL_LPC_D4_BM    = BIT(10);
static const int GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_KAL_DATA_SEL_KAL_LPC_D5_BM    = BIT(11);
static const int GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_KAL_DATA_SEL_KAL_LPC_D6_BM    = BIT(12);
static const int GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_KAL_DATA_SEL_KAL_LPC_D7_BM    = BIT(13);
static const int GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_KAL_CSB_SEL_BM                = BIT(14);
/* PUR only all bits */
static const int GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_KAL_ALL_PUR_BM                = 0x7FFF;
    /* 0x7FFF: This is what it should be but it cause compiler to complain in c files
    GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_EN_BM | GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_DATA_SEL_BM |
    GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_CSB_SEL_BM | GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_KAL_DATA_SEL_BM |
    GBL_LPC_PCM2_MUX_CONFIG_OVERRIDE_KAL_CSB_SEL_BM;
    */


/* MMU_GW_CONFIG field positions. */
enum mmu_gw_config_posn_enum
{
    MMU_GW_CONFIG_BANK_SELECT_LSB_POSN      = 0,
    MMU_GW_CONFIG_BANK_SELECT_MSB_POSN      = 12,
    MMU_GW_CONFIG_WINDOW_MAPPING_LSB_POSN   = 13,
    MMU_GW_CONFIG_WINDOW_MAPPING_MSB_POSN   = 14
};
/* MMU_GW_CONFIG Window_mapping field enum. Applicable to BC7+ but not all */
static const int MMU_GW_CONFIG_SHARED_MEMORY      = 0u << MMU_GW_CONFIG_WINDOW_MAPPING_LSB_POSN; /* Select generic window to access shared RAM */
static const int MMU_GW_CONFIG_PROGRAM_MEMORY     = 1u << MMU_GW_CONFIG_WINDOW_MAPPING_LSB_POSN; /* Select generic window to access this processor's program RAM */
static const int MMU_GW_CONFIG_IO_LOG             = 2u << MMU_GW_CONFIG_WINDOW_MAPPING_LSB_POSN; /* Select generic window to access IO log window */
static const int MMU_GW_CONFIG_ALT_PROGRAM_MEMORY = 3u << MMU_GW_CONFIG_WINDOW_MAPPING_LSB_POSN; /* Select generic window to access other processor's program memory */
static const int MMU_GW_CONFIG_ALT_DATA_MEMORY    = 4u << MMU_GW_CONFIG_WINDOW_MAPPING_LSB_POSN; /* Select generic window to access other processor's data memory */


#endif // _IO_MAP_PUBLIC_BC_NEXTGEN_H_
