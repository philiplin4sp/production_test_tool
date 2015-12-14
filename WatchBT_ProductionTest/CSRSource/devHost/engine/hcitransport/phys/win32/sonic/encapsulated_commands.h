/*
    File:       encapsulated_commands.h
    Copyright:  CSR (C) 2009
    Purpose:    Module abstracting Encapsulated Commands for Sonic with the Jungo Drivers
*/
#ifdef ENCAPSULATED_COMMANDS_H
#error encapsulated_commands.h already included.
#else
#define ENCAPSULATED_COMMANDS_H

#include "common/types.h"

/* Version Numbers ID */
typedef enum
{
    verID_limit_first = 0,
    verID_firmware = verID_limit_first,
    verID_FPGA,
    verID_PCB,
    verID_limit_last = verID_PCB,
} versionID_e;

/* Flow control modes */
typedef enum
{
    UART_flowctrl_limit_first = 0,  /* Used for range checking */
    UART_flowctrl_noFC = UART_flowctrl_limit_first, /* ignore CTS, deassert RTS */
    UART_flowctrl_hardware,         /* respect CTS, assert RTS when buffer fills */
    UART_flowctrl_H4PlusPacket,
    UART_flowctrl_H4PlusSegment,
    UART_flowctrl_H4PlusPadded,
    UART_flowctrl_CTSyRTSa,         /* respect CTS, assert RTS */
    UART_flowctrl_CTSyRTSd,         /* respect CTS, deassert RTS */
    UART_flowctrl_CTSnRTSa,         /* ignore CTS, assert RTS */
    UART_flowctrl_CTSnRTSd,         /* ignore CTS, deassert RTS */
    UART_flowctrl_limit_last        /* invalid. Used for range checking and array size */
} UART_flowctrl_e;

/* ADC Channel Allocation */
typedef enum
{
    adc_channel_limit_first = 0,
    adc_channel_vtrack_1 = adc_channel_limit_first,
    adc_channel_vtrack_2,
    adc_channel_vtrack_3,
    adc_channel_aio_0,
    adc_channel_aio_1,
    adc_channel_3v3,
    adc_channel_2v5,
    adc_channel_power_1,
    adc_channel_power_2,
    adc_channel_power_3,
    adc_channel_vext,
    adc_channel_5v0_raw,
    adc_channel_v_bank,
    adc_channel_1v2,
    adc_channel_1v8,
    adc_channel_limit_last /* Not a channel. Used for range checking */
} adc_channel_e;

typedef enum
{
    DCID_elementID_first,                       /* job lot. for range checking */
    DCID_element_first = DCID_elementID_first,  /* for range checking */
    DCID_reset_polarity = DCID_element_first,
    DCID_PIO_direction,
    DCID_PIO_out,
    DCID_reset_V,
    DCID_power_V_1,
    DCID_power_V_2,
    DCID_power_V_3,
    DCID_power_V_bank,
    DCID_VEXT_enable,
    DCID_SDIObus_enable,
    DCID_element_last = DCID_SDIObus_enable,   /* for range checking */
    /* reserved */
    DCID_IO_voltage_first = 0x100,
    DCID_IO_voltage_00 = DCID_IO_voltage_first,
    DCID_IO_voltage_01,
    DCID_IO_voltage_02,
    DCID_IO_voltage_03,
    DCID_IO_voltage_04,
    DCID_IO_voltage_05,
    DCID_IO_voltage_06,
    DCID_IO_voltage_07,
    DCID_IO_voltage_08,
    DCID_IO_voltage_09,
    DCID_IO_voltage_10,
    DCID_IO_voltage_11,
    DCID_IO_voltage_12,
    DCID_IO_voltage_13,
    DCID_IO_voltage_14,
    DCID_IO_voltage_15,
    DCID_IO_voltage_16,
    DCID_IO_voltage_17,
    DCID_IO_voltage_18,
    DCID_IO_voltage_19,
    DCID_IO_voltage_20,
    DCID_IO_voltage_21,
    DCID_IO_voltage_22,
    DCID_IO_voltage_23,
    DCID_IO_voltage_24,
    DCID_IO_voltage_25,
    DCID_IO_voltage_26,
    DCID_IO_voltage_27,
    DCID_IO_voltage_28,
    DCID_IO_voltage_29,
    DCID_IO_voltage_30,
    DCID_IO_voltage_31,
    DCID_IO_voltage_last = DCID_IO_voltage_31,
    /* reserved */
    DCID_MUX_outbus_func_first = 0x200, /* for range checking */
    DCID_MUX_outbus_func_00 = DCID_MUX_outbus_func_first,
    DCID_MUX_outbus_func_01,
    DCID_MUX_outbus_func_02,
    DCID_MUX_outbus_func_03,
    DCID_MUX_outbus_func_04,
    DCID_MUX_outbus_func_05,
    DCID_MUX_outbus_func_06,
    DCID_MUX_outbus_func_07,
    DCID_MUX_outbus_func_08,
    DCID_MUX_outbus_func_09,
    DCID_MUX_outbus_func_10,
    DCID_MUX_outbus_func_11,
    DCID_MUX_outbus_func_12,
    DCID_MUX_outbus_func_13,
    DCID_MUX_outbus_func_14,
    DCID_MUX_outbus_func_15,
    DCID_MUX_outbus_func_16,
    DCID_MUX_outbus_func_17,
    DCID_MUX_outbus_func_18,
    DCID_MUX_outbus_func_19,
    DCID_MUX_outbus_func_20,
    DCID_MUX_outbus_func_21,
    DCID_MUX_outbus_func_22,
    DCID_MUX_outbus_func_23,
    DCID_MUX_outbus_func_24,
    DCID_MUX_outbus_func_25,
    DCID_MUX_outbus_func_26,
    DCID_MUX_outbus_func_27,
    DCID_MUX_outbus_func_28,
    DCID_MUX_outbus_func_29,
    DCID_MUX_outbus_func_30,
    DCID_MUX_outbus_func_31,
    DCID_MUX_outbus_func_last = DCID_MUX_outbus_func_31, /* for range checking */
    /* reserved */
    DCID_MUX_outbus_dir_first = 0x300,
    DCID_MUX_outbus_dir_00 = DCID_MUX_outbus_dir_first, /* for range checking */
    DCID_MUX_outbus_dir_01,
    DCID_MUX_outbus_dir_02,
    DCID_MUX_outbus_dir_03,
    DCID_MUX_outbus_dir_04,
    DCID_MUX_outbus_dir_05,
    DCID_MUX_outbus_dir_06,
    DCID_MUX_outbus_dir_07,
    DCID_MUX_outbus_dir_08,
    DCID_MUX_outbus_dir_09,
    DCID_MUX_outbus_dir_10,
    DCID_MUX_outbus_dir_11,
    DCID_MUX_outbus_dir_12,
    DCID_MUX_outbus_dir_13,
    DCID_MUX_outbus_dir_14,
    DCID_MUX_outbus_dir_15,
    DCID_MUX_outbus_dir_16,
    DCID_MUX_outbus_dir_17,
    DCID_MUX_outbus_dir_18,
    DCID_MUX_outbus_dir_19,
    DCID_MUX_outbus_dir_20,
    DCID_MUX_outbus_dir_21,
    DCID_MUX_outbus_dir_22,
    DCID_MUX_outbus_dir_23,
    DCID_MUX_outbus_dir_24,
    DCID_MUX_outbus_dir_25,
    DCID_MUX_outbus_dir_26,
    DCID_MUX_outbus_dir_27,
    DCID_MUX_outbus_dir_28,
    DCID_MUX_outbus_dir_29,
    DCID_MUX_outbus_dir_30,
    DCID_MUX_outbus_dir_31,
    DCID_MUX_outbus_dir_last = DCID_MUX_outbus_dir_31, /* for range checking */
    /* reserved */
    DCID_MUX_inbus_func_first = 0x400,
    DCID_MUX_inbus_func_00 = DCID_MUX_inbus_func_first, /* for range checking */
    DCID_MUX_inbus_func_01,
    DCID_MUX_inbus_func_02,
    DCID_MUX_inbus_func_03,
    DCID_MUX_inbus_func_04,
    DCID_MUX_inbus_func_05,
    DCID_MUX_inbus_func_06,
    DCID_MUX_inbus_func_07,
    DCID_MUX_inbus_func_08,
    DCID_MUX_inbus_func_09,
    DCID_MUX_inbus_func_10,
    DCID_MUX_inbus_func_11,
    DCID_MUX_inbus_func_12,
    DCID_MUX_inbus_func_13,
    DCID_MUX_inbus_func_14,
    DCID_MUX_inbus_func_15,
    DCID_MUX_inbus_func_16,
    DCID_MUX_inbus_func_17,
    DCID_MUX_inbus_func_18,
    DCID_MUX_inbus_func_19,
    DCID_MUX_inbus_func_20,
    DCID_MUX_inbus_func_21,
    DCID_MUX_inbus_func_22,
    DCID_MUX_inbus_func_23,
    DCID_MUX_inbus_func_24,
    DCID_MUX_inbus_func_25,
    DCID_MUX_inbus_func_26,
    DCID_MUX_inbus_func_27,
    DCID_MUX_inbus_func_28,
    DCID_MUX_inbus_func_29,
    DCID_MUX_inbus_func_30,
    DCID_MUX_inbus_func_31,
    DCID_MUX_inbus_func_32,
    DCID_MUX_inbus_func_33,
    DCID_MUX_inbus_func_34,
    DCID_MUX_inbus_func_35,
    DCID_MUX_inbus_func_36,
    DCID_MUX_inbus_func_last = DCID_MUX_inbus_func_36, /* for range checking */
    /* reserved */
    DCID_elementID_last = DCID_MUX_inbus_func_last  /* job lot. for range checking */
} DUT_cfg_elementID_e;

typedef enum
{   /* Output Bus function Mapping enum */
    DUT_MUX_outFunc_first,                              /* for range checking */
    DUT_MUX_outFunc_PIO_first = DUT_MUX_outFunc_first,  /* for range checking */
    DUT_MUX_outFunc_PIO_00 = DUT_MUX_outFunc_PIO_first,
    DUT_MUX_outFunc_PIO_01,
    DUT_MUX_outFunc_PIO_02,
    DUT_MUX_outFunc_PIO_03,
    DUT_MUX_outFunc_PIO_04,
    DUT_MUX_outFunc_PIO_05,
    DUT_MUX_outFunc_PIO_06,
    DUT_MUX_outFunc_PIO_07,
    DUT_MUX_outFunc_PIO_08,
    DUT_MUX_outFunc_PIO_09,
    DUT_MUX_outFunc_PIO_10,
    DUT_MUX_outFunc_PIO_11,
    DUT_MUX_outFunc_PIO_12,
    DUT_MUX_outFunc_PIO_13,
    DUT_MUX_outFunc_PIO_14,
    DUT_MUX_outFunc_PIO_15,
    DUT_MUX_outFunc_PIO_16,
    DUT_MUX_outFunc_PIO_17,
    DUT_MUX_outFunc_PIO_18,
    DUT_MUX_outFunc_PIO_19,
    DUT_MUX_outFunc_PIO_20,
    DUT_MUX_outFunc_PIO_21,
    DUT_MUX_outFunc_PIO_22,
    DUT_MUX_outFunc_PIO_23,
    DUT_MUX_outFunc_PIO_24,
    DUT_MUX_outFunc_PIO_25,
    DUT_MUX_outFunc_PIO_26,
    DUT_MUX_outFunc_PIO_27,
    DUT_MUX_outFunc_PIO_28,
    DUT_MUX_outFunc_PIO_29,
    DUT_MUX_outFunc_PIO_30,
    DUT_MUX_outFunc_PIO_31,
    DUT_MUX_outFunc_PIO_last = DUT_MUX_outFunc_PIO_31,  /* for range checking */
    DUT_MUX_outFunc_UART_TX,
    DUT_MUX_outFunc_UART_RTS,
    DUT_MUX_outFunc_SPI_CLK,
    DUT_MUX_outFunc_SPI_CSB,
    DUT_MUX_outFunc_SPI_MOSI,
    /* gap - reserved */
    DUT_MUX_outFunc_HI = 62,
    DUT_MUX_outFunc_LO,
    DUT_MUX_outFunc_last = DUT_MUX_outFunc_LO           /* for range checking */
} DUT_MUX_outFunc_e;

 /* The direction is in, out or is specified by bit nn in PIO direction register */
typedef enum
{   /* Output Bus Direction Mapping enum */
    /* These map the direction to the value of the specified bit in the PIO direction register */
    DUT_MUX_outDir_first = 0,                           /* for range checking */
    DUT_MUX_outDir_PIO_first = DUT_MUX_outDir_first,    /* for range checking */
    DUT_MUX_outDir_PIO_00 = DUT_MUX_outDir_PIO_first,
    DUT_MUX_outDir_PIO_01,
    DUT_MUX_outDir_PIO_02,
    DUT_MUX_outDir_PIO_03,
    DUT_MUX_outDir_PIO_04,
    DUT_MUX_outDir_PIO_05,
    DUT_MUX_outDir_PIO_06,
    DUT_MUX_outDir_PIO_07,
    DUT_MUX_outDir_PIO_08,
    DUT_MUX_outDir_PIO_09,
    DUT_MUX_outDir_PIO_10,
    DUT_MUX_outDir_PIO_11,
    DUT_MUX_outDir_PIO_12,
    DUT_MUX_outDir_PIO_13,
    DUT_MUX_outDir_PIO_14,
    DUT_MUX_outDir_PIO_15,
    DUT_MUX_outDir_PIO_16,
    DUT_MUX_outDir_PIO_17,
    DUT_MUX_outDir_PIO_18,
    DUT_MUX_outDir_PIO_19,
    DUT_MUX_outDir_PIO_20,
    DUT_MUX_outDir_PIO_21,
    DUT_MUX_outDir_PIO_22,
    DUT_MUX_outDir_PIO_23,
    DUT_MUX_outDir_PIO_24,
    DUT_MUX_outDir_PIO_25,
    DUT_MUX_outDir_PIO_26,
    DUT_MUX_outDir_PIO_27,
    DUT_MUX_outDir_PIO_28,
    DUT_MUX_outDir_PIO_29,
    DUT_MUX_outDir_PIO_30,
    DUT_MUX_outDir_PIO_31,
    DUT_MUX_outDir_PIO_last = DUT_MUX_outDir_PIO_31,    /* for range checking */
    /* gap - reserved */
    /* These map to input or output */
    DUT_MUX_outDir_out = 62,
    DUT_MUX_outDir_in,
    DUT_MUX_outDir_last = DUT_MUX_outDir_in             /* for range checking */
} DUT_MUX_outDir_e;

typedef enum
{   /* Input Bus function mapping enum */
    DUT_MUX_inFunc_first = 0,                           /* for range checking */
    DUT_MUX_inFunc_IF_first = DUT_MUX_inFunc_first,    /* for range checking */
    DUT_MUX_inFunc_IF_00 = 0,
    DUT_MUX_inFunc_IF_01,
    DUT_MUX_inFunc_IF_02,
    DUT_MUX_inFunc_IF_03,
    DUT_MUX_inFunc_IF_04,
    DUT_MUX_inFunc_IF_05,
    DUT_MUX_inFunc_IF_06,
    DUT_MUX_inFunc_IF_07,
    DUT_MUX_inFunc_IF_08,
    DUT_MUX_inFunc_IF_09,
    DUT_MUX_inFunc_IF_10,
    DUT_MUX_inFunc_IF_11,
    DUT_MUX_inFunc_IF_12,
    DUT_MUX_inFunc_IF_13,
    DUT_MUX_inFunc_IF_14,
    DUT_MUX_inFunc_IF_15,
    DUT_MUX_inFunc_IF_16,
    DUT_MUX_inFunc_IF_17,
    DUT_MUX_inFunc_IF_18,
    DUT_MUX_inFunc_IF_19,
    DUT_MUX_inFunc_IF_20,
    DUT_MUX_inFunc_IF_21,
    DUT_MUX_inFunc_IF_22,
    DUT_MUX_inFunc_IF_23,
    DUT_MUX_inFunc_IF_24,
    DUT_MUX_inFunc_IF_25,
    DUT_MUX_inFunc_IF_26,
    DUT_MUX_inFunc_IF_27,
    DUT_MUX_inFunc_IF_28,
    DUT_MUX_inFunc_IF_29,
    DUT_MUX_inFunc_IF_30,
    DUT_MUX_inFunc_IF_31,
    DUT_MUX_inFunc_IF_last = DUT_MUX_inFunc_IF_31,    /* for range checking */
    /* gap - reserved */
    DUT_MUX_inFunc_HI = 62,
    DUT_MUX_inFunc_LO,
    DUT_MUX_inFunc_last = DUT_MUX_inFunc_LO             /* for range checking */
} DUT_MUX_inFunc_e;


typedef enum
{
    DCE_ok,
    DCE_invalid_ID,
    DCE_invalid_value,
    DCE_enabled,
} DUT_cfg_err_t;

/* Bit definitions for DUT_status() */
#define DUT_status_DUT_fitted       BIT_0
#define DUT_status_DUT_cfg_enabled  BIT_1
#define DUT_status_DUT_active       BIT_2

int cdc_port_open(HANDLE*, const char *, BOOL);
int cdc_port_close(HANDLE);
int cdc_encapsulated_send(HANDLE, void*, DWORD, PDWORD);
int cdc_encapsulated_get(HANDLE, void*, DWORD, PDWORD);
int enc_version_get(HANDLE com_h, versionID_e verID, uint32 *value);

int write_fpga_register(HANDLE, uint16 , unsigned char);
int read_fpga_register(HANDLE, uint16 , unsigned char*);
int enc_fpga_registers_read(HANDLE com_h, uint16 offset, unsigned char cbSize, unsigned char* value);

int enc_pio_get(HANDLE com_h, uint32 *pio);
int enc_pio_set(HANDLE com_h, uint32 mask, uint32 pio);
int enc_piocdne_get(HANDLE com_h, uint32 *rise, uint32 *fall);
int enc_piocdne_set(HANDLE com_h, uint32 mask_rise, uint32 rise, uint32 mask_fall, uint32 fall);
int enc_piocdnp_get(HANDLE com_h, uint32 *rise, uint32 *fall);

int enc_UART_flowctrl_get(HANDLE com_h, UART_flowctrl_e *flowctrl);
int enc_UART_flowctrl_set(HANDLE com_h, UART_flowctrl_e flowctrl);
int enc_UART_RTS_set(HANDLE com_h, BOOL rts_assert);
int enc_UART_RTS_get(HANDLE com_h, BOOL *rts_assert);
int enc_UART_CTS_get(HANDLE com_h, BOOL *cts);

int enc_DUT_status_get(HANDLE com_h, uint8 *DUT_status);
#define sonic_DUT_fitted(DUT_status)      (DUT_status & DUT_status_DUT_fitted)
#define sonic_DUT_cfg_enabled(DUT_status) (DUT_status & DUT_status_DUT_cfg_enabled)
#define sonic_DUT_active(DUT_status)      (DUT_status & DUT_status_DUT_active)
int enc_DUT_reset_set(HANDLE com_h, BOOL rst_assert);
int enc_DUT_reset(HANDLE com_h, DWORD dwMilliseconds);

int enc_DUT_cfg_enable_set(HANDLE com_h, BOOL DUT_cfg_enable, uint8 *error);
int enc_DUT_cfg_element_get(HANDLE com_h, uint16 id, uint32 *value, uint8 *error);
int enc_DUT_cfg_element_set(HANDLE com_h, uint16 id, uint32 value, uint8 *error);

int enc_DUT_ADC_get(HANDLE com_h, adc_channel_e channel, uint16 *value);

#endif
