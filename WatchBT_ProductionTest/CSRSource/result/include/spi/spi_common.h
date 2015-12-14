/**********************************************************************
 *
 *  spi_common.h
 *
 *  Copyright (C) Cambridge Silicon Radio Ltd
 *
 *  Global SPI definitions
 *
 ***********************************************************************/

#ifndef SPI_H
#define SPI_H

static const int NUM_MULTISPI_PORTS = 16;

#define MAX_BABELS 32	// The maximum number of babels we can have.

// Slowest SPI clock speed
static const int SPI_SLOW_SPEED = 20;

// BFT chips are identified specially by reading this address.
static const int GBL_BFT_IDENT_ADDR = 0x002B;

// Pre BC7 chips have version at this address
static const int GBL_CHIP_VERSION_GEN1_ADDR = 0xFF9A;

// Unifi chips and post BC7 chips have version at this address
static const int GBL_CHIP_VERSION_GEN2_ADDR = 0xFE81;

//Constants specific to the flash of CSR8670 chip
static const int GBL_SPI_LOCKED_GDN_VAL              = 0xDEAF;
static const int GBL_GDN_STAY_LOCKED_START_ADDR      = 0x76;
static const int GBL_GDN_STAY_LOCKED_NUM_WORDS       = 2;
static const int GBL_GDN_CUST_KEY_START_ADDR         = 0x78;
static const int GBL_GDN_CUST_KEY_NUM_WORDS          = 8;
static const int GBL_GDN_ERASE_CODE                  = 0xEEEE;
static const int GBL_GDN_CHIP_SERIAL_NUM             = 0xfc31;
static const int GBL_GDN_CHIP_SERIAL_NUM_SIZE_16_BIT = 4;
//See the ApplicationNote link on wiki page GordonWaferTestDeveloperGuide
//for details of 'GBL_GDN_TNVH', 'GBL_GDN_TNVS', 'GBL_GDN_TPROG'. Times represent microseconds
static const int GBL_GDN_TNVH                        = 5;
static const int GBL_GDN_TNVS                        = 5;
static const int GBL_GDN_TPROG                       = 20;

//General Memory and Register locations
static const int FLASH_GW2                           = 0x2000; //Address of SPI Generic Window 2 in flash
static const int FLASH_CTRL                          = 0xfb86; //Address of flash control register

static const int BANK_SELECT_PCM         = 0xf1f3;
static const int BANK_SELECT_SDIO        = 0xF927;
static const int BANK_SELECT_USB         = 0xFCE0;

static const int BANK_SELECT_PCM_MASK    = 0x0001;
static const int BANK_SELECT_SDIO_MASK   = 0x00FF;
static const int BANK_SELECT_USB_MASK    = 0x0003;

static const int BANK_SELECT_PCM_VALUES  = 1;
static const int BANK_SELECT_SDIO_VALUES = 17;
static const int BANK_SELECT_USB_VALUES  = 3;

static const int SLUT_LOCATION_GEN1      = 0x100;
static const int SLUT_LOCATION_GEN2      = 0x080;

/* GW1,2,3 window base address */
static const int MMU_GW1_BASE            = 0x0100;
static const int MMU_GW2_BASE            = 0x2000;
static const int MMU_GW3_BASE            = 0x4000;
/* GW1,2,3 window size */
static const int MMU_GW1_SIZE            = 0x1F00;
static const int MMU_GW2_SIZE            = 0x2000;
static const int MMU_GW3_SIZE            = 0x2000;

// Macros for converting address / page values to appropriate GW values
// Applicable to BC7+
//  (GW = generic window)
//  FLASH_PAGE_TO_GWPAGE converts flash page or sector number to GW page number
//  FLASH_ADDR_TO_GWPAGE converts address in flash into GW page number
//           Note that while a GW2 page is the same size as a flash
//           page (4k), the GW2 window is incremented in 2k increments
//           (so that successive GW2 pages have a 2k overlap).So
//           for a given flash address, 2 choices could be made for
//           the GW2 page. The unique choice made by this macro is
//           addresses 0->4095 (i.e. page 0 addresses) map to GW2 page 0,
//           addresses 4096->8181 (i.e. page 1 addresses) map to GW2 page 2,
//           addresses 8192->12287 (i.e. page 2 addresses) map to GW2 page 4 etc.
//  FLASH_ADDR_TO_GWOFFSET converts address in flash into offset into GWx window

#define FLASH_PAGE_TO_GWPAGE(page)          (page << 1)
#define FLASH_ADDR_TO_GWPAGE(addr)          (((unsigned short)FLASH_PAGE_TO_GWPAGE((addr) >> 12)))
#define FLASH_ADDR_TO_GWOFFSET(addr)        ((addr) & 0xFFF)
#define FLASH_ADDR_TO_MMU_GW_CONFIG(addr)   (FLASH_ADDR_TO_GWPAGE(addr) | (v20plus::MMU_GW_CONFIG_PROGRAM_MEMORY))

//options supplied to pttransport via the -trans argument
#define SPIDEBUG               "SPIDEBUG"              //Whether to output debug info on SPI transactions to a debug log / attached debugger
#define SPIDEBUG_WIRE          "SPIDEBUG_WIRE"         //Whether to log more detailed transport specific information
#define SPIDEBUG_FILE          "SPIDEBUG_FILE"         //The name of the file to which to write the spi debug log
#define SPIDEBUG_READS         "SPIDEBUG_READS"        //Whether to log SPI reads
#define SPIDEBUG_WRITES        "SPIDEBUG_WRITES"       //Whether to log SPI writes
#define SPIDEBUG_REPRODUCIBLE  "SPIDEBUG_REPRODUCIBLE" //If sets to ON, enables the SPI trace to replayed later with spiutil using –PLAY <tracefile>
#define SPI_DEBUG_FULL         "SPIDEBUG_FULL"         //If sets to ON, get full debug to spi log
#define SPI_DELAY_MODE         "SPI_DELAY_MODE"        //Sets which algorithm to use to obtain reliable SPI activity
#define SPI_STOP_CHIP          "SPI_STOP_CHIP"         //Specifies the circumstances under which the chip is stopped
#define SPI_DUMMY_FILE         "SPI_DUMMY_FILE"        //Specify dummy SPI data in a file
#define SPI_DUMMY_MODE         "SPI_DUMMY_MODE"        //Mode to run with dummy data
#define SPIDEBUG_STREAMS       "SPIDEBUG_STREAMS"      //Whether to switch on SPIDEBUG for specific streams
#define SPIDEBUG_FILE_PREFIX   "SPIDEBUG_FILE_PREFIX"  //The base file name used to generate a stream-specific log file
#define SPIPORT                "SPIPORT"               //The spi port number to use
#define SPIMUL                 "SPIMUL"                //The device to use if the port is multiplexed
#define SPI_DELAY              "SPI_DELAY"             //Specifies the delay for the spi clock
#define SPICLOCK               "SPICLOCK"              //The SPI clock speed in KHz
#define SPICMDBITS             "SPICMDBITS"            //Sets the command bits in the read write commands
#define SPICMDREADBITS         "SPICMDREADBITS"        //Sets the command bits in the read commands
#define SPICMDWRITEBITS        "SPICMDWRITEBITS"       //Sets the command bits in the write commands
#define SPIMAXCLOCK            "SPIMAXCLOCK"           //The maximum SPI clock speed in KHz
#define SPIREMOTEIP            "SPIREMOTEIP"           //Sets the IP address of the remote machine if SPITRANS=REMOTE
#define BABELMODE              "BABELMODE"             //Changes the Babel between 'SPI' and 'JTAG' modes
#define SPI_HI_ADDR            "SPI_HI_ADDR"           // Sets the high 16-bits of address for 32-bit transports.

// Mode options for dummy SPI data
#define SPI_DUMMY_MODE_FULL         "FULL"
#define SPI_DUMMY_MODE_OVERRIDE     "OVERRIDE"

// Debug output block markers
#define SPI_DEBUG_BLOCK_ENTRY "[IN ]"
#define SPI_DEBUG_BLOCK_EXIT "[OUT]"

#endif
