/******************************************************************************
Copyright (C) Cambridge Silicon Radio Ltd 2000-2009

Project:        Flash.dll
File Name:      Flash.h
Description:    Implementation of the flash loader class.

Program your flash over spi with this class.  Follow these steps.
 * use dll's use_port(1) to select comm port 1.  
 * Construct a flash.  (Construct with a hwnd if you want to receive debug, title, progress and completion messages.)
 * Construct a FlashImage, construct a FlashImageSerial, passing it the image.
 * Call FlashImageSerial::ReadFiles to get your .xpv and .xdv files read in.
 * get the flash to use the image by calling flash::UseImage(FlashImage *)
 * Call flash_burn() to burn the image
 * Call flash_verify() to verify the contents of the flash against the image.
 * Call flash_dump(const char *filename) to read back contents of the flash.
 * Call flash_erase() if you want to lose your persistent store.
 * Call identify() to recognise the flash.  Results availabe after the MSG_FLASH_COMPLETE message in flash_id.
*/


// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the FLASHDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// FLASHDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

// flash.h: interface for the flash class.

// $Id: //depot/bc/bluesuite_2_4/devHost/spi/flashdll/flash.h#1 $
//
//////////////////////////////////////////////////////////////////////

#if !defined(FLASHDLL_H_INCLUDED)
#define FLASHDLL_H_INCLUDED

#ifdef WIN32
#ifdef FLASHDLL_EXPORTS
#define FLASHDLL_API __declspec(dllexport)
#else
#define FLASHDLL_API __declspec(dllimport)
#endif
#else
#define FLASHDLL_API
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "common/portability.h"
#include "abstract/memory_reader.h"
#include "pttransport/pttransport.h"

#include "flashtypes.h"
#include "flashid.h"
#include "flashimg.h"

#ifdef WIN32
#include <windows.h>
#endif // WIN32

class FlashInternal;

class flash: public IMemoryReaderGP
{
public:
    FLASHDLL_API flash();
    FLASHDLL_API explicit flash(pttrans_stream_t stream);
    FLASHDLL_API ~flash();
    
    // Identifies the chip and flash.
    // Will spawn a thread and return immediately if 'threaded' is true.
    FLASHDLL_API int flash_identify();

    // Writes the associated image to a single device.
    // Will spawn a thread and return immediately if 'threaded' is true.
    FLASHDLL_API int flash_burn();
    
    // Writes to several connected devices over the MultiSpi device using broadcast mode.
    // Bits in devsToUse select the devices.
    // Set eraseFirst if all chips should be fully erased before programming.
    // Set ignoreUnused to skip stop and restart of unused positions.
    // Will stop and reset ALL devices connected to the MultiSpi device (Broadcast SPI
    // will stomp on the the RAM), unless ignoreUnused is set (Ensure that only devices
    // selected by devsToUse are connected).
    // Will spawn a thread and return immediately if 'threaded' is true.
    FLASHDLL_API int flash_broadcast_burn(unsigned long devsToUse, bool eraseFirst, bool restartAfter, bool ignoreUnused);

    // Checks if the associated image matches what is on flash.  Displays a list of the sectors
    // which are different.
    // Will spawn a thread and return immediately if 'threaded' is true.
    FLASHDLL_API int flash_verify();

    // Erases all of the flash.
    // Will spawn a thread and return immediately if 'threaded' is true.
    FLASHDLL_API int flash_erase();

    // Dumps the contents of flash to the given file.  'format' specified the file format used.
    // Will spawn a thread and return immediately if 'threaded' is true.
    FLASHDLL_API int flash_dump(const char *filename, const flash_dump_t format = FLASH_DUMP_XPV);
    // Dumps the contents of flash to the associated FlashImage.
    // Will spawn a thread and return immediately if 'threaded' is true.
    FLASHDLL_API int flash_dump_to_image();
    FLASHDLL_API int flash_dump_to_image_partial(uint32 start, uint32 end);

    FLASHDLL_API void SetFlashTarget(flash_target_t target);

    FLASHDLL_API uint8 GetLPCCount();
    FLASHDLL_API uint8 SetLPCIndex(uint8 lpc_index);
    FLASHDLL_API uint8 GetLPCIndex();

    FLASHDLL_API unsigned int flash_get_burn_time();
    FLASHDLL_API int GetProgress();
    FLASHDLL_API int GetProgressMax();
    FLASHDLL_API flash_op_t GetCurrentOperation();
    FLASHDLL_API const char *GetOperationString(flash_op_t op);
    FLASHDLL_API const char *GetErrorString();
    FLASHDLL_API flash_error_t GetErrorCode();
    FLASHDLL_API void UseImage(FlashImage *image);
    FLASHDLL_API bool UsingImage();
    FLASHDLL_API bool IsBusy();
    FLASHDLL_API void Terminate();
    FLASHDLL_API unsigned long GetDetectedDevices();
    FLASHDLL_API unsigned long GetFailedDevices();
    FLASHDLL_API void GetFlashInfo(flash_info_t &info);

    FLASHDLL_API void SetBroadcastDelays(unsigned int delays);
    FLASHDLL_API unsigned int GetBroadcastDelays(void);
#ifdef WIN32
    FLASHDLL_API void SetCallbackWindow(HWND hwnd);
#endif
    FLASHDLL_API void SetCallbackFn(FlashBack *fn, void *param = NULL);
    FLASHDLL_API void SetProgressFn(ProgressFlashBack *fn, void *param = NULL);

    FLASHDLL_API bool GetFirmwareId(uint32 *loaderId, char *loaderName,
                                    uint32 *stackId,  char *stackName,
                                    const size_t sNameBuffer);
    FLASHDLL_API virtual bool read_mem(GenericPointer addr, uint16 len, uint16 * data);

    FLASHDLL_API int GetXapCrystalSpeedMhz(void);
    FLASHDLL_API bool SetXapCrystalSpeedMhz(int speed);  // Will return true if value is appropriate.
    
    FLASHDLL_API void SetTestMode(boot_prog_test_mode_t mode);

    FLASHDLL_API void SetSuppressReset(bool suppress);

    BOOL verbose;

    // Indicates if the flash_identify, flash_burn, flash_broadcast_burn,
    // flash_verify, flash_erase and flash_dump functions should run in a
    // seperate thread.  Defaults to true on Windows and false elsewhere.
    bool threaded;

    // This is necessary to share the text for the info message
    FLASHDLL_API static const char TxtImageTrunc[];

protected:
    FlashInternal *mpImpl;

private:
    void Init();
};

#endif // FLASHDLL_H_INCLUDED



