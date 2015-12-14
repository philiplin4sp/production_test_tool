/****************************************************************************
FILE
	io_digital.h  -  low-level I/O definitions required for accessing ps

DESCRIPTION
	The values in this file are defined as part of the BlueCore digital
	design.  Most of these files live in the file io_defs.h in the
	io module, but the ones in this file are necessary for setting
	the values of persistent store keys from host side tools and so
	are more widely visible.

	Unlike io_defs.h itself, enums here are declared unconditionally;
	if we need different values for BlueCore 1 and 2, they are defined
	in different enums.

MODIFICATION HISTORY
	1.1  11:jan:02  pws     Created.
	1.2  21:jan:02  pws     Move more stuff from io_defs.h for XAP tools.
	1.3  25:nov:02  pws     H17.70: copied from mm01 and committed.
	#4   11:dec:03  pws     B-1522: support 32 kHz external slow clock.
        #5   22:jan:04  doc     B-1809: no change (overzealous integrate).
	#6   07:may:05  pws     B-2706: added baud rates > 1382k4.
	#7   17:may:05  sms     B-7011: Added AMUX clock settings.
        #8   01:jun:05  rgb     B-5206: Added ANA_AMUX_B_SEL_SPKR_DCPL.
        #9   15:nov:05  sm      B-7395: Additional AMUX clock frequencies.
	#10  02:feb:06  jn01	B-4284: H4+ tx-only byte-FC h/w accel from 19
        #11  06:feb:06  sv01    B-10839: Add 2 ANA_AMUX_B_SEL... enum values. 
        #13  30:may:06  rb01    B-14340: Update AIO as PIO enum names.
        #14  27:sep:06  sv01    B-16655: Update with more info from digits.
        #15  13:nov:06  jn01    B-17350: amux_clk_freq_enum is not for BC5.
        ------------------------------------------------------------------
        --- This modification history is now closed. Do not update it. ---
        ------------------------------------------------------------------
*/

#ifndef IO_DIGITAL_H
#define IO_DIGITAL_H

#ifdef RCS_STRINGS
static char io_digital_h_id[]
  = "$Id: //depot/bc/bluesuite_2_4/interface/host/io/io_digital.h#1 $";
#endif

#ifndef CHIP_BASE_JEMIMA
enum uart_rate_enum
{
   UART_RATE_9K6                            = 39,
   UART_RATE_19K2                           = 79,
   UART_RATE_38K4                           = 157,
   UART_RATE_57K6                           = 236,
   UART_RATE_115K2                          = 472,
   UART_RATE_230K4                          = 944,
   UART_RATE_460K8                          = 1887,
   UART_RATE_921K6                          = 3775,
   UART_RATE_1382K4                         = 5662,
   /*
    * Starting with BC3-EXT and BC3-ROM-MR, higher baud rates
    * are available.
    */
   UART_RATE_1843K2                         = 7550,
   UART_RATE_2764K8                         = 11325,
   UART_RATE_3686K4                         = 15099
};
typedef enum uart_rate_enum uart_rate;
#endif


#ifndef CHIP_BASE_JEMIMA
enum uart_config_posn_enum
{
   UART_CONFIG_TWO_STOP_BITS_POSN           = 0,
   UART_CONFIG_ONE_PARITY_BIT_POSN          = 1,
   UART_CONFIG_EVEN_PARITY_POSN             = 2,
   UART_CONFIG_FLOW_CTRL_EN_POSN            = 3,
   UART_CONFIG_RTS_AUTO_EN_POSN             = 4,
   UART_CONFIG_RTS_POSN                     = 5,
   UART_CONFIG_TX_ZERO_EN_POSN              = 6,
   UART_CONFIG_NON_BCSP_EN_POSN             = 7,
   UART_CONFIG_RX_RATE_DELAY_LSB_POSN       = 8,
   UART_CONFIG_RX_RATE_DELAY_MSB_POSN       = 10,
   UART_CONFIG_CTS_FILTER_EN_POSN	    = 11,
   UART_CONFIG_H5_EN_POSN                   = 12,
   UART_CONFIG_H4_EN_POSN                   = 13,
   UART_CONFIG_H4_MODE_LSB_POSN             = 14,
   UART_CONFIG_H4_MODE_MSB_POSN             = 15
};
typedef enum uart_config_posn_enum uart_config_posn;


enum uart_config_mask_enum
{
   UART_CONFIG_TWO_STOP_BITS_MASK           = (int)0x0001,
   UART_CONFIG_ONE_PARITY_BIT_MASK          = (int)0x0002,
   UART_CONFIG_EVEN_PARITY_MASK             = (int)0x0004,
   UART_CONFIG_FLOW_CTRL_EN_MASK            = (int)0x0008,
   UART_CONFIG_RTS_AUTO_EN_MASK             = (int)0x0010,
   UART_CONFIG_RTS_CTRL_EN_MASK             = (int)0x0010,
   UART_CONFIG_RTS_MASK                     = (int)0x0020,
   UART_CONFIG_TX_ZERO_EN_MASK              = (int)0x0040,
   UART_CONFIG_NON_BCSP_EN_MASK             = (int)0x0080,
   UART_CONFIG_RX_RATE_DELAY_LSB_MASK       = (int)0x0100,
   UART_CONFIG_RX_RATE_DELAY_MSB_MASK       = (int)0x0400,
   UART_CONFIG_RX_RATE_DELAY_MASK           = (int)0x0700,
   UART_CONFIG_CTS_FILTER_EN_MASK           = (int)0x0800,
   UART_CONFIG_H5_EN_MASK                   = (int)0x1000,
   UART_CONFIG_H4_EN_MASK                   = (int)0x2000,
   UART_CONFIG_H4_MODE_LSB_MASK             = (int)0x4000,
   UART_CONFIG_H4_MODE_MSB_MASK             = (int)0x8000,
   UART_CONFIG_H4_MODE_MASK                 = (int)0xC000
};
typedef enum uart_config_mask_enum uart_config_mask;
#endif 


enum rst_enable_posn_enum
{
   RST_WATCHDOG_EN_POSN                     = 0,
   RST_UART_EN_POSN                         = 1,
   RST_FULL_CHIP_EN_POSN                    = 2
};
typedef enum rst_enable_posn_enum rst_enable_posn;


enum rst_enable_mask_enum
{
   RST_WATCHDOG_EN_MASK                     = (int)0x0001,
   RST_UART_EN_MASK                         = (int)0x0002,
   RST_FULL_CHIP_EN_MASK                    = (int)0x0004
};
typedef enum rst_enable_mask_enum rst_enable_mask;



enum spi_emu_cmd_posn_enum
{
   SPI_EMU_CMD_XAP_STEP_POSN                = 0,
   SPI_EMU_CMD_XAP_RUN_B_POSN               = 1,
   SPI_EMU_CMD_XAP_BRK_POSN                 = 2,
   SPI_EMU_CMD_XAP_WAKEUP_POSN              = 3
};
typedef enum spi_emu_cmd_posn_enum spi_emu_cmd_posn;


enum spi_emu_cmd_mask_enum
{
   SPI_EMU_CMD_XAP_STEP_MASK                = (int)0x0001,
   SPI_EMU_CMD_XAP_RUN_B_MASK               = (int)0x0002,
   SPI_EMU_CMD_XAP_BRK_MASK                 = (int)0x0004,
   SPI_EMU_CMD_XAP_WAKEUP_MASK              = (int)0x0008
};
typedef enum spi_emu_cmd_mask_enum spi_emu_cmd_mask;

/* Clock rate values for CLKGEN_MINMAX_MMU_RATE on Jemima.
 * This isn't the full set, just some handy looking values
 * The actual MMU period is (Rate[6:0] << (Rate[7] * 8))+1 80Mhz clock cycles
 * These numbers were taken from the table in document CS-110148-SP.
 *
 * NOTE: the code assumes that lower numbers are faster, so values in the jump
 * shown in the graph in CS-110148-SP (i.e. 128 to 135 inclusive) must not be
 * used.
 */
enum mmu_rate_jemima_enum
{
    MMU_RATE_MAXIMUM = 0, /* 80 MHz, or 40 MHz on chips limited to that */
    MMU_RATE_40M     = 1,
    MMU_RATE_26M     = 2, /* actually 26667 kHz */
    MMU_RATE_20M     = 3,
    MMU_RATE_16M     = 4,
    MMU_RATE_10M     = 7,
    MMU_RATE_8M      = 9,
    MMU_RATE_4M      = 19,
    MMU_RATE_2M      = 39,
    MMU_RATE_1M      = 79,
    MMU_RATE_500K    = 138, /* actually 497 kHz */
    MMU_RATE_250K    = 148, /* actually 249 kHz */
    MMU_RATE_125K    = 168,
    MMU_RATE_100K    = 178,
    MMU_RATE_50K     = 227,
    MMU_RATE_40K     = 254
};
typedef enum mmu_rate_jemima_enum mmu_rate_jemima;

#endif /* IO_DIGITAL_H */
