/****************************************************************************
FILE
        slutids.h  -  identifiers of elements in the slut[]

DESCRIPTION
	This file contains identifiers used in the slut[] symbol look up
	table.

	This file deliberately does not #include bt.h, to allow its use in
	host (spi) code.

	Do not reallocate identifiers - it may break existing (host) code.

MODIFICATION HISTORY
	1.1   14:jul:00	 cjo	Created.
	1.2   7:sep:00   cjo	Improved comments.
	1.3   21:sep:00	 cjo	Updated comments.
	1.4   19:dec:00	 at	Added build identifiers.
	1.5   8:feb:01	 cjo	Changed bccmd_spi identifiers.
	1.6   8:feb:01	 cjo	Untangle cvs merge.
	1.7   8:feb:01   mm     Changed new BCCMD ID so that it doesn't clash
	                        with the old id's.
	1.8   26:apr:01  cjo	Added SLUTID_SPI_USER_CMD.
	1.9   13:mar:01  ajh	Moved to 'interface' directory.
	1.1   13:mar:01  ajh    Moved to 'interface' directory.
	1.2   26:apr:01  cjo    Improved comments.
	1.3   12:nov:01  mm01   Added SLUTID_INTERNAL_BUILDID.
	1.4   15:nov:01  at     Added SLUTID_ID_STRING_FULL.
	1.5   10:dec:01  pws    hci-13 #141: add SLUTID_BUILDDEF_LIST
	1.6   19:dec:01  at     Added support for bc02 dfu files.
	1.7   28:jan:02  at     Added SLUTID_PANIC_BACKTRACE.
	1.8   18:feb:02  pws    H15.37: SLUTID_BUILDDEF_LIST is duplicated.
	1.9   22:feb:02  cjo    Correct comment.
	1.10  18:mar:02  pws    H15.84: added spicheck module.
	1.11  21:mar:02  pws    H15.89:  Add check_rom_crc16().
	1.12  22:mar:02  pws    H15.91: add check_rom_crc32().
	1.13  16:sep:02  at     H16.122: Add SLUTID_STACK_CHECKSUM.
	1.14  12:jun:03  cjo    Correct trivial typo.
	1.15  13:jun:03  jbs    B-440. Bring SLUTID_FS_VERSION from
				rfc-16p8-branch.
	1.16  02:sep:03  sm     B-919: Enable baud rate detection to be
				aborted by spi access.
	#17   25:sep:03  pws    B-1088: Add dorm_force_fast_clock.
	#18   06:oct:03  cjo    B-1168: Added SLUTID_ALL_PSKEYS_LIST.
	#19   22:jan:04  bcsw
	#20   08:mar:04  bs01
	#21   25:jan:05  npm
	#22   17:mar:05  bs01
	#23   29:jul:05  jn01   B-3091: Added SLUTID_RADIOTEST_FNS.
        #24   20:dec:06  jd07   B-19932 Added SLUTID_HQ_SCRAPING.
        ------------------------------------------------------------------
        --- This modification history is now closed. Do not update it. ---
        ------------------------------------------------------------------
*/

#ifndef	__SLUTIDS_H__
#define	__SLUTIDS_H__
 
#ifdef RCS_STRINGS
static const char slutids_h_id[]
  = "$Id: //depot/bc/bluesuite_2_4/interface/host/slut/slutids.h#1 $";
#endif

/* Identifiers of items in the slut[]. */

#define	SLUTID_NONE		(0)	/* Identifies nothing. */

/* SPI BCCMD interface. */
#define	SLUTID_BCCMD_INTERFACE	(9)	/* Dynamic bccmd_spi interface. */
#define	SLUTID_SPI_USER_CMD	(11)	/* Addr of SPI_USER_CMD register. */
/* #define SLUTID_BCCMD_BUFFER	(1)	   bccmd ram buffer. */
/* #define SLUTID_BCCMD_FLAG	(2)	   bccmd buffer access flag. */

/* Dead identifiers; feature no longer present */
/*#define SLUTID_LC_NEXT_COMMAND   (3)     Next debug command (misnamed). */
/*#define SLUTID_DEBUG_PSSET_LEN   (4)     Debug access to ps_[sg]et(). */
/*#define SLUTID_DEBUG_PSSET_KEY   (5)     Debug access to ps_[sg]et(). */
/*#define SLUTID_DEBUG_PSSET_VALUE (6)     Debug access to ps_[sg]et(). */

/* Build identifiers. */
#define	SLUTID_ID_STRING	(7)	/* Build identifier string. */
#define	SLUTID_ID_INTEGER	(8)	/* Build identifier integer. */
#define	SLUTID_ID_STRING_FULL	(13)	/* Long build identifier string. */

/* Loader pointer to its stack public key. */
#define SLUTID_STACK_PUBLIC_KEY (10)	/* Pointer to public key struct. */

/* Firmware build identifiers. */
#define SLUTID_INTERNAL_BUILDID (12)	/* No longer used (see B-41600) */
#define SLUTID_BUILDDEF_LIST	(21)	/* Array of build definitions. */
#define SLUTID_BUILDDEF_BLUECORE (14)	/* Supported hardware variants. */
#define SLUTID_BUILDDEF_FLASH	(15)	/* Supported flash geometries. */

/* Virtual machine and application information. */
#define SLUTID_VM_VERSION	(16)	/* Virtual machine version. */
#define SLUTID_VM_TRAPSETS	(17)	/* Virtual machine trap sets. */

/* NO LONGER PRESENT IN FIRMWARE */
#define SLUTID_VM_APP_START	(18)	/* Physical flash start address. */
#define SLUTID_VM_APP_SIZE	(19)	/* Maximum size (words). */

/* Debugging information. */
#define SLUTID_PANIC_BACKTRACE	(20)	/* No longer used (see B-59536) */

/* Memory checking routines called from SPI in production tests */
#define SLUTID_CHECK_RAM_PTR	(22)	/* Pointer to check_ram(). */
#define SLUTID_CHECK_ROM_PTR	(23)	/* Pointer to check_rom(). */
#define SLUTID_CHECK_ROM_CRC16_PTR (24)	/* Pointer to check_rom_crc16(). */
#define SLUTID_CHECK_ROM_CRC32_PTR (25)	/* Pointer to check_rom_crc32(). */

/* NO LONGER PRESENT IN FIRMWARE */
#define SLUTID_FW_CRC_TABLE	(26)	/* Table required for 32-bit CRCs */

/* Data to allow checksum verification of loader and stack. */
#define SLUTID_FW_CHECKSUM	(27)	/* Checksum range and value. */

/* Revision of the file system this firmware understands */
#define SLUTID_FS_VERSION       (28)    /* File system version */

/* Uart baud rate determined by the baud rate detection algorithm */
#define SLUTID_UART_AUTO_BAUD_RATE (29) /* Auto-detected uart baud rate. */

/* 16-bit word which if non-zero will force the CPU to full rate */
#define SLUTID_FORCE_FAST_CLOCK (30) /* Register to force full clock rate */

/* Address of psbc_all_keys[], list of all pskeys "known to" the firmware.  */
#define SLUTID_ALL_PSKEYS_LIST	(31)

/* No longer used as of B-24576 */

#define SLUTID_VM_PROFILE_BLOCK	(32)

#define SLUTID_RADIOTEST_FNS    (33)

/* 
   Points to the start and end of the filesystem extension in the data
   segment.  Appears above the top of physical flash via software
   remapping.

   No longer used, as of B-42592
*/

#define SLUTID_FS_WRAP          (34)

#define SLUTID_HQ_SCRAPING      (35)

/* Literal information to allow SLUT-reading tools to determine the
 * layout of Flash/ROM, and thus to interpret pointers in this SLUT,
 * etc. */

#define SLUTID_MEMMAP           (36)

/* Revision of the DSP that this firmware supports */
#define SLUTID_DSP_REVISION     (37)

/* DSP Debug through SPI interface */
#define SLUTID_SPI_DSP_DEBUG     (38)

/* Uart protocol determined by the uart auto protocol detect routine */
#define SLUTID_UART_AUTO_PROTOCOL (39)

/* 
   Explicit list of VM trap numbers, terminated with 0xFFFF.
   Replaces SLUTID_VM_TRAPSETS on heavily optimised ROM parts.
*/
#define SLUTID_VM_TRAPLIST	(40)	/* Virtual machine trap list. */

/* UPDATE ME: next free ID is 41. */


#endif	/* __SLUTIDS_H__ */
