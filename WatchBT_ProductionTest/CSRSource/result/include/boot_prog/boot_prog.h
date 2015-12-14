/**********************************************************************
*
*  FILE   :  bootprog.h
*
*            Copyright (C) Cambridge Silicon Radio Ltd 2005-2009
*
*  AUTHOR :  David Johnston
*
*  PURPOSE:  Provides boot programs for different chips.
*
*  $Id: $
*
***********************************************************************/

#ifndef BOOT_PROG_H
#define BOOT_PROG_H

#include "pttransport/pttransport.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    BOOTPROG_VER_BLUECORE,
    BOOTPROG_VER_UNIFI,
    BOOTPROG_VER_V20PLUS,
    BOOTPROG_VER_V20P_SPIF,
    BOOTPROG_VER_V20P_SQIF,
    BOOTPROG_VER_ROB
} bootprog_ver_t;

// Members which are common to all boot programs.
typedef struct boot_prog_base
{
    unsigned long prog_code_addr; // Load address for code.
    unsigned short *prog_code;
    unsigned short prog_code_length;
    unsigned long prog_data_addr; // Load address for data.
    unsigned short *prog_data;
    unsigned short prog_data_length;
    unsigned long boot_addr; // Code start-up address.

    unsigned short boot_cmd;  // The command.
    unsigned short boot_result; // The command result. (32-bit, low word in boot_result + 1)
    bootprog_ver_t bootprog_ver; // version of the boot prog runtime
} boot_prog_base;

// This was originally intended to be for a generic boot program that took a few arguments.
// In practice this is just used for the EDR tuning boot program.
typedef struct boot_prog_test
{
    boot_prog_base base;

    unsigned short parameter0;
    unsigned short parameter1;
    unsigned short parameter2;

    unsigned short streams[4]; // Pointers to arrays of uint16s which the caller can set.
} boot_prog_test;

typedef struct boot_prog_definition
{
    boot_prog_base base;

    unsigned short boot_flash_addr; // address of block to be read / written
    unsigned long bank_offset; // Offset applied to MMU_FLASH_BANK_SELECT for different chip versions / flash configurations.
    unsigned short boot_page_buffer0; // Address of 4K sector for the flash page
    unsigned short boot_page_buffer1; // Address of 4K sector for the flash page
    
    unsigned short boot_page_size; // Size of smallest page in flash device.  Most devices use 4Kwords, but some use 2 or 1.
    unsigned short boot_flash_size_pow;  // Size of the flash device (2 ** pow bytes).
    unsigned short boot_flash_sectors_available;  // Usable size of the flash device (2 ** pow bytes).  Could be smaller than boot_flash_size_pow if the chip has insufficient address lines.
    unsigned short boot_flash_type; // Type of flash device

    unsigned short boot_test_mode;  // Test mode for boot program.
} boot_prog_definition;

typedef struct flash_test_prog_definition
{
    boot_prog_base base;

    unsigned short boot_flash_addr; // address of block to be read / written
    unsigned long bank_offset; // Offset applied to MMU_FLASH_BANK_SELECT for different chip versions / flash configurations.
    unsigned short boot_page_buffer; // Address of 4K sector for the flash page
    
    unsigned short published_vpp; /* VPP voltage in 0.1V - linked to PIO */
    unsigned short published_vcc; /* VCC in 0.1V - link to PIO */
    unsigned short XY_location; /* To be programme to info block */
    unsigned short boot_bit_addr; /* address of bit to test */
    unsigned short rw_addr; /* address of bit to test */
    unsigned short rw_value; /* address of bit to test */
    unsigned short rw_time; /* address of bit to test */
} flash_test_prog_definition;

typedef struct flash_test_prog_definition_v20plus
{
    boot_prog_base base;

    unsigned short config;                 /* Holds initial setup information such as whether to use PIO for communication with the ATE */
    unsigned short swpio;                  /* Software communication with ATE when PIO is disabled (in config) and voltage request */
    unsigned long  trim_shadow;            /* Shadows the trimming location (7,0) in IFREN1. Used for maintaining trimming values around erase. */
    unsigned short mp_time;                /* Tmprog time in microseconds to use with mass program. Derived from adaptive programming algorithm. */
    unsigned short pe_time;                /* Terase time in microseconds to use with page erase. Derived from adaptive erase algorithm. */
    unsigned short prog_time;              /* Tprog time to use in microseconds with program word. Derived from adaptive programming algorithm. */
    unsigned short ifren_page;             /* Indicates page in IFREN. Used for ATE to erase/verify/program directly the IFREN */
    unsigned long  ifren_buffer;           /* Buffer used for ATE to write directly to IFREN */
    unsigned short ifren_buffer_len;       /* Length of ifren_buffer in bytes */
    unsigned long  failure_buffer;         /* Array of failures as bit address */
    unsigned short failure_buffer_len;     /* Number of failure entries in failure_buffer */
} flash_test_prog_definition_v20plus;

typedef struct flash_test_memcycle_definition
{
    boot_prog_base base;

    unsigned short config;                 /* Holds initial setup information such as whether to use PIO for communication with the ATE */
    unsigned short swpio;                  /* Software communication with ATE when PIO is disabled (in config) and voltage request */
    unsigned long  trim_shadow;            /* Shadows the trimming location (7,0) in IFREN1. Used for maintaining trimming values around erase. */
    unsigned short mp_time;                /* Tmprog time in microseconds to use with mass program. Derived from adaptive programming algorithm. */
    unsigned short pe_time;                /* Terase time in microseconds to use with page erase. Derived from adaptive erase algorithm. */
    unsigned short prog_time;              /* Tprog time to use in microseconds with program word. Derived from adaptive programming algorithm. */
    unsigned short ifren_page;             /* Indicates page in IFREN. Used for ATE to erase/verify/program directly the IFREN */
    unsigned long  ifren_buffer;           /* Buffer used for ATE to write directly to IFREN */
    unsigned short ifren_buffer_len;       /* Length of ifren_buffer in bytes */
    unsigned long  failure_buffer;         /* Array of failures as bit address */
    unsigned short failure_buffer_len;     /* Number of failure entries in failure_buffer */
} flash_test_memcycle_definition;

typedef struct e2_boot_prog_definition
{
    boot_prog_base base;

    unsigned short boot_rw_addr; // address of block to be read / written
    unsigned short boot_rw_count;// count of octets to be read / written
    unsigned short boot_page_buffer; // Address of 4K sector for the e2 page
    unsigned short dev_sel;          // Which device select bits are needed to talk to the EEPROM
    unsigned short addrmask;         // Which device select bits are part of the address MSBs
    unsigned short single_byte_addr; // If the EEPROM uses one byte for addressing (or two)
    unsigned short pio_sda;          // The PIO to use for I2C SDA line
    unsigned short pio_scl;          // The PIO to use for I2C SCL line
    unsigned short pio_wp;           // The PIO to use for EEPROM write protect line
} e2_boot_prog_definition;


// Returns true if the given crystal is MHz is valid.
int BootProg_ValidXtal(int speed);

// Download the boot program given to the chip.
// If verify is true the writes performed will be verified and the function may
// return false, indicating an error.
// If verify is false the writes will not be verified but assumed to have succeded.
// This can be used if broadcasting to multiple chips.
int BootProg_Download(const boot_prog_base *bootdef, int verify);
int BootProg_Stream_Download(pttrans_stream_t stream, const boot_prog_base *bootdef, int verify);

// Start the downloaded program, 'speed' specifies the crystal speed in MHz.
int BootProg_Start(const boot_prog_base *bootdef, int speed);
int BootProg_Stream_Start(pttrans_stream_t stream, const boot_prog_base *bootdef, int speed);

// Returns which boot_prog to use based on which chip.
bootprog_ver_t BootProg_GetChipVer(void);
bootprog_ver_t BootProg_Stream_GetChipVer(pttrans_stream_t stream);

#ifdef __cplusplus
}
#endif

#endif /* BOOT_PROG_H */
