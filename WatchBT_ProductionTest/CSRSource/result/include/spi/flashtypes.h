#if !defined(FLASHTYPES_H_INCLUDED)
#define FLASHTYPES_H_INCLUDED

/**********************************************************************

    flashtypes.h

    Copyright (C) Cambridge Silicon Radio Ltd

    flash type definitions

 ***********************************************************************/

#ifndef WIN32
#define WM_USER 0
#endif // WIN32

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif // def MAX

// MSG_FLASH_PROGRESS: wParam is the progress, lParam, is the total
#define MSG_FLASH_PROGRESS (WM_USER + 1)

// MSG_FLASH_COMPLETE: wparam is the status, one of FLASH_STATUS_DONE or FLASH_STATUS_TIMED_OUT
#define MSG_FLASH_COMPLETE (MSG_FLASH_PROGRESS + 1)

// MSG_FLASH_DEBUG: lParam is a LPCSTR containing the new caption
#define MSG_FLASH_DEBUG (MSG_FLASH_PROGRESS + 2)

// MSG_FLASH_TITLE: lParam is a LPCSTR containing the new caption
#define MSG_FLASH_TITLE (MSG_FLASH_PROGRESS + 3)

enum flash_error_level_t {
    flash_error_level_debug = 0,
    flash_error_level_info = 1,
    flash_error_level_warning = 2,
    flash_error_level_major = 3,
    flash_error_level_failure = 4
};

enum flash_error_t
{
    FLASH_ERROR_NONE = 0,
    FLASH_ERROR_DOWNLOAD_FAILED,
    FLASH_ERROR_VERIFY_FAILED,
    FLASH_ERROR_TIMED_OUT,
    FLASH_ERROR_CRC_FAILED,
    FLASH_ERROR_READBACK_FAILED,
    FLASH_ERROR_COULD_NOT_DOWNLOAD_PROG,
    FLASH_ERROR_COULD_NOT_STOP_XAP,
    FLASH_ERROR_BOOT_PROG_HALTED,
    FLASH_ERROR_ERASE_FAILED,
    FLASH_ERROR_XAP_ERROR,
    FLASH_ERROR_UNKNOWN_CHIP_TYPE,
    FLASH_ERROR_BROADCAST_MIXED_CHIP_TYPES,
    FLASH_ERROR_OPEN_FILE,
    FLASH_ERROR_NO_IMAGE,
    FLASH_ERROR_BUSY,
    FLASH_ERROR_NO_FLASH,
    FLASH_ERROR_OOM
};

// This should match up to the  OperationStrings in flash.cpp
enum flash_op_t
{
    FLASH_OP_DONE = 0,
    FLASH_OP_DUMPING,
    FLASH_OP_DOWNLOADING_BOOT,
    FLASH_OP_WRITING, 
    FLASH_OP_VERIFYING,
    FLASH_OP_ERASING,
    FLASH_OP_ACQUIRING_SPI 
};

// These values are taken from common/psfl.c
enum boot_prog_test_mode_t
{
    BOOT_PROG_TEST_NONE = 0,
    BOOT_PROG_TEST_ID_SPIN = 1,
    BOOT_PROG_TEST_ID_NO_FLASH = 2,
    BOOT_PROG_TEST_BURN_SPIN = 3,
    BOOT_PROG_TEST_ERASE_SPIN = 4
};

enum flash_dump_t
{
    FLASH_DUMP_XPV = 0,
    FLASH_DUMP_XUV,
    FLASH_DUMP_XBV,
    FLASH_DUMP_RAW
};

// Copied from psfl.c
enum flash_id_t
{
    ID_UNKNOWN = 0, 
    ID_AMD, 
    ID_PAIR, 
    ID_INTEL, 
    ID_BC02 = 0x20,
    ID_BC03_BC04_EFLASH = 0x40,
    ID_BC07_EFLASH,
    ID_ROBINSON_FLASH,
    ID_ERROR = -1
};

enum flash_target_t
{
    FLASH_TARGET_FLASH,
    FLASH_TARGET_SPI_FLASH,
    FLASH_TARGET_SQI_FLASH
};

enum flash_interface_t {
    FLASH_INTERFACE_UNKNOWN = 0,
    FLASH_INTERFACE_ONCHIP,
    FLASH_INTERFACE_LPC
};

struct flash_info_t {
    flash_interface_t flash_interface;
    union {
        struct {
            uint16 flash_sectors;
            flash_id_t flash_type;
            long manuf_id;
            long device_id;
        } info_onchip;
        struct {
            uint16 sram_size;
            uint16 build_year;
            uint8 build_month;
            uint8 build_day;
            uint8 build_hour;
            uint8 build_minute;
            uint16 build_variant;
            const char *build_variant_str;
            uint16 board_id;
            const char *board_id_str;
        } info_lpc;
    };
};

typedef void (FlashBack)(flash_error_level_t errorLevel,
                         flash_error_t       errorCode,
                         const char         *errorStr,
                               void         *param);
typedef void (ProgressFlashBack)(uint32 progress, uint32 max, void *param);

static const int SECTOR_SIZE = 0x1000; ///< Number of (16-bit) Words in a sector
static const uint16 BLOCKSIZE = SECTOR_SIZE; ///< @deprecated Use ::SECTOR_SIZE instead
static const int NUM_SECTORS_TO_MBITS_CONVERSION = SECTOR_SIZE / 256; ///< Convert between sector size and number of bits

static const int BC01_SECTORS_AVAILABLE = 64;
static const int BC02_SECTORS_AVAILABLE = 256;
static const int MAX_SECTORS = 1024;

static const int errorBufSize = 8192;

/* Embedded Flash InfoBlock Lengths */
static const uint16 BC03_BC04_EFLASH_INFOBLOCKLEN = 0x0800;
static const uint16 BC07_EFLASH_INFOBLOCKLEN = 0x1000;

/* Work out the maximum buffer size needed at compile time */
static const uint16 EFLASH_INFOBLOCKLEN_MAX = MAX(BC07_EFLASH_INFOBLOCKLEN, BC03_BC04_EFLASH_INFOBLOCKLEN);

#endif // FLASHTYPES_H_INCLUDED
