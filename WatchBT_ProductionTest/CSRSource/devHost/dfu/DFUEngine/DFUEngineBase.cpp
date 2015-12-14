///////////////////////////////////////////////////////////////////////
//
//	File	: DFUEngineBase.cpp
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFUEngineBase
//
//	Purpose	: Implementation of DFU engine functionality that is
//			  common for all transports.
//
//			  Derived classes implementing specific transports
//			  should initialise the following member variable from
//			  their constructor:
//				transport	- The implementation of the transport.
//
//			  Since a smart pointer is used, there is no need to
//			  explicitly delete the transport object; it will happen
//			  automatically during the destruction of this object.
//			  The Terminate function from the base class is called
//			  before the tranport is deleted.
//
//			  See the definition of DFUInterface for the operations
//			  supported.
//
//	Modification history:
//	
//		1.1		25:Sep:00	at	File created.
//		1.2		28:Sep:00	at	Moved DFUFile reading and writing to
//								base class, and implemented real DFU
//								operations.
//		1.3		12:Oct:00	at	The recovery operation attempts to end
//								and restart the transport if problems
//								are encountererd.
//		1.4		20:Oct:00	at	Stalled result codes converted to
//								actual errors. Completion of download
//								operations prior to requesting the
//								status now handled sensibly. Attempts
//								to abort operations or clear status
//								only made if likely to be successful.
//		1.5		24:Oct:00	at	Updated following changes to
//								DFURequests.
//		1.6		02:Nov:00	at	Disabled delay after final reset.
//		1.7		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.8		30:Nov:00	at	Increased detach timeout.
//		1.9		14:Dec:00	at	Abort not checked during delay after
//								download operation. Alternative status
//								checking behaviour removed.
//		1.10	15:Dec:00	at	Removed incorrect clearing of status
//								after failed upload.
//		1.11	02:Mar:01	at	Added missing check of return code.
//		1.12	05:Mar:01	at	Added delay at start of recovery
//								action and support for activity
//								description.
//		1.13	06:Mar:01	at	Added support for DFURequests to
//								updated the activity description.
//		1.14	22:Mar:01	at	More accurate poll timeouts.
//		1.15	23:Mar:01	at	More efficient accurate sleeping.
//		1.16	11:Apr:01	at	Increased maximum supported delay.
//		1.17	12:Apr:01	at	Improved download activity description.
//								Retries used for reconfigure.
//		1.18	20:Apr:01	at	Remaining time indicated for long
//								waits during download.
//		1.19	30:Apr:01	ckl	Added Windows CE support.
//		1.20	23:May:01	at	Tidied Windows CE support.
//		1.21	23:Jul:01	at	Added version string.
//		1.22	06:Oct:01	at	Added support for Widcomm USB driver.
//		1.23	30:Jul:02	at	Disconnect at end of manifest.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUEngineBase.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUEngineBase.h"
#include "DFUFile.h"
#include <windows.h>

// Visual C++ debugging code
#ifdef _DEBUG
#include <crtdbg.h>
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define new DEBUG_NEW
#endif

// Version string
const char *dfuenginebase_id = "$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUEngineBase.cpp#1 $ Copyright (C) 2000-2006 Cambridge Silicon Radio Limited";

// Maximum detach timeout and retries
static const uint16 detachTimeout = 15000;
static const uint8 detachRetries = 5;

// Delay before start of recovery action
static const uint16 recoverDelay = 1000;

// Download timing to trigger additional description
static const uint32 downloadProgressMilliseconds = 4000;

// Assumed maximum upload size for progress indication
static const uint32 maximumUploadSize = 256 * 2 * 1024;

// Number of milliseconds to use accurate sleep timing for
static const uint32 accurateMilliseconds = 10;

#ifndef OLD_CODE
// Use this timeout instead of the timeout from the firmware to speed things up
static const uint32 shortTimeout =10;
#endif

// Constructor
DFUEngineBase::DFUEngineBase(DFUEngine *engine)
: DFUInterface(engine)
{
}

// Destructor
DFUEngineBase::~DFUEngineBase()
{
	// Kill thread before object destroyed
	Terminate();
}

// Accurate sleeping
void DFUEngineBase::AccurateSleep(uint32 milliseconds)
{
	LARGE_INTEGER frequency;

	// Check if a high performance timer is available
	if (QueryPerformanceFrequency(&frequency))
	{
		// Calculate the required end time
		LARGE_INTEGER end;
		QueryPerformanceCounter(&end);
		end.QuadPart += LONGLONG((float) frequency.QuadPart * ((float) milliseconds / 1000.0));

		// Use the simple sleep for the bulk of long delays
		if (accurateMilliseconds < milliseconds)
		{
			Sleep(milliseconds - accurateMilliseconds);
		}

		// Sleep until the time has elapsed
		LARGE_INTEGER now;
		while (QueryPerformanceCounter(&now) && ((now.QuadPart - end.QuadPart) < 0))
		{
			Sleep(0);
		}
	}
	else
	{
		// Use a simple sleep otherwise
		Sleep(milliseconds);
	}
}

void DFUEngineBase::AccurateSleep(uint32 milliseconds, DFUEngine::SubOperation subOperation)
{
	// Loop until done
	while (0 < milliseconds)
	{
		// Update the progress indicator
		Progress(subOperation, milliseconds / 1000);

		// Sleep for a second (plus any fractional part) of the remaining time
		uint32 sleep = min(milliseconds, 1000 + milliseconds % 1000);
		AccurateSleep(sleep);
		milliseconds -= sleep;
	}
}

// Status and callback handling
DFUEngine::Result DFUEngineBase::ProgressCheck(int percent, uint32 sleep)
{
	// Update the progress and check for abort
	Progress(percent);
	return CheckAbort(sleep);
}

// Implementation of operations
DFUEngine::Result DFUEngineBase::DoReconfigure(bool checkUpload,
											   bool checkDownload,
											   DFUFile *checkFile)
{
	DFUEngine::Result result;
	DFURequests::DeviceDescriptor device;
	DFURequests::InterfaceDescriptor iface;
	DFURequests::DFUFunctionalDescriptor functional;
	DFURequests::DFUStatus status;

	// Attempt to start the transport (could be slow for UART)
	Progress(DFUEngine::reconfigure_connect);
	result = DoConnect(false);
	if (!result) return result;

	// Read the device descriptor to determine mode
	result = ProgressCheck(30);
	if (result) result = transport->GetDevice(device);
	if (!result) return result;
	result = transport->GetInterfaceDFU(iface);
	if (!result) return result;

	// Set the initial DFU file information
	appVendor = device.idVendor;
	appProduct = DFUFile::unused;
	appDevice = DFUFile::unused;

	// Special case if already in DFU mode
	if (IsDFUMode(device, iface))
	{
		// Abort any operation, ignoring result code
		Progress(DFUEngine::reconfigure_start_runtime);
		result = ProgressCheck(35);
		if (result) result = DoDFUIdle(true, true);
		if (!result) return result;

		// Perform a reset and restart transport
		result = ProgressCheck(40);
		if (result) result = transport->Reset();
		if (result) result = DoConnect(false);
		if (!result) return result;

		// Read device descriptor again to determine mode
		result = ProgressCheck(50);
		if (result) result = transport->GetDevice(device);
		if (!result) return result;
		result = transport->GetInterfaceDFU(iface);
		if (!result) return result;

		if(IsDFUMode(device, iface) && checkUpload)
			return DFUEngine::fail_no_upload;
	}

	// Perform a detach and reset if not in DFU mode
	uint8 retries = 0;
	while (!IsDFUMode(device, iface) && (retries < detachRetries))
	{
		// Save the run-time mode details
		appVendor = device.idVendor;
		appProduct = device.idProduct;
		appDevice = device.bcdDevice;

		// Read maximum timeout from the DFU functional descriptor
		result = ProgressCheck(55);
		if (result) result = transport->GetFunct(functional);
		if (!result) return result;

		// Perform detach, reset, and restart transport
		Progress(DFUEngine::reconfigure_start_dfu);
		result = ProgressCheck(60);
		if (result) result = transport->Detach(min(functional.wDetachTimeout, detachTimeout));
		if (result) result = transport->Reset();
		if (result) result = DoConnect(true);
		if (!result) return result;

		// Read device descriptor again to determine mode
		result = ProgressCheck(70);
		if (result) result = transport->GetDevice(device);
		if (!result) return result;
		result = transport->GetInterfaceDFU(iface);
		if (!result) return result;

		// Increase the retry count
		++retries;
	}

	// Should be operating in DFU mode, no more retries if not
	if (!IsDFUMode(device, iface)) return DFUEngine::fail_mode_no_dfu;

	// Check the DFU status
	Progress(DFUEngine::reconfigure_checking);
	result = ProgressCheck(80);
	if (result) result = DoDFUIdle(true, true);
	if (result) result = transport->GetStatus(status);
	if (!result) return result;

	// Check suitability of DFU file if any supplied
	if ((checkFile != 0)
		&& (checkFile->GetVendor() != DFUFile::unused))
	{
		if (checkFile->GetVendor() != appVendor)
		{
			// Vendor ID does not match
			return DFUEngine::Result(DFUEngine::fail_file_vendor,
			                         (*checkFile)->GetFilePath());
		}
		if ((checkFile->GetProduct() != DFUFile::unused)
			&& (appProduct != DFUFile::unused)
			&& (checkFile->GetProduct() != appProduct))
		{
			// Product ID does not match
			return DFUEngine::Result(DFUEngine::fail_file_product,
			                         (*checkFile)->GetFilePath());
		}
	}

	// Check support for upload or download if requested
	if (checkUpload || checkDownload)
	{
		// Read the DFU mode functional descriptor
		result = ProgressCheck(90);
		if (result) result = transport->GetFunct(functional);
		if (!result) return result;

		// Check the attributes
		if (checkUpload
			&& !(functional.bmAttributes
			     & DFURequests::attribute_can_upload))
		{
			return DFUEngine::fail_no_upload;
		}
		if (checkDownload
			&& !(functional.bmAttributes
			     & DFURequests::attribute_can_download))
		{
			return DFUEngine::fail_no_download;
		}
	}

	// Successful if this point reached
	return DFUEngine::success;
}

DFUEngine::Result DFUEngineBase::DoUpload(DFUFile &file)
{
	DFUEngine::Result result;
	DFURequests::DFUFunctionalDescriptor functional;

	// Ensure that the transport has been started
	Progress(DFUEngine::upload_prepare);
	result = DoConnect(true);
	if (!result) return result;

	// Verify the starting state
	result = ProgressCheck(1);
	if (result) result = DoDFUIdle(true, false);
	if (!result) return result;

	// Read maximum transfer size and check upload supported
	result = ProgressCheck(2);
	if (result) result = transport->GetFunct(functional);
	if (!result) return result;
	if (!(functional.bmAttributes
	      & DFURequests::attribute_can_upload))
	{
		return DFUEngine::fail_no_upload;
	}

	// Perform the upload
	file.SetVendor(appVendor);
	file.SetProduct(appProduct);
	file.SetDevice(appDevice);
	file->SetLength(0);
	bool finished = false;
	uint16 blockNum = 0;
	uint16 bufferLength = functional.wTransferSize;
	SmartPtr<uint8, true> buffer(new uint8[bufferLength]);
	while (!finished)
	{
		// Update the progress indicator
		Progress(DFUEngine::upload_progress, file->GetLength());
		uint32 done = min(file->GetLength(), maximumUploadSize);
		result = ProgressCheck(3 + (95 * done) / maximumUploadSize);
		if (!result) return result;

		// Upload the next block
		uint16 replyLength;
		result = transport->Upload(blockNum++, buffer, bufferLength, replyLength);
		if (!result) return result;

		// Add to the DFU file
		if (replyLength) file->Write(buffer, replyLength);
		finished = replyLength < bufferLength;
	}

	// Verify the final state
	Progress(DFUEngine::upload_verify);
	result = ProgressCheck(99);
	if (result) result = DoDFUIdle();
	if (!result) return result;

	// Successful if this point reached
	return DFUEngine::success;
}

DFUEngine::Result DFUEngineBase::DoDownload(DFUFile &file)
{
	DFUEngine::Result result;
	DFURequests::DFUFunctionalDescriptor functional;
	DFURequests::DFUStatus status;
#ifndef OLD_CODE
	DFURequests::DFUStatus newStatus;
#endif

	// Ensure that the transport has been started
	Progress(DFUEngine::download_prepare);
	result = DoConnect(true);
	if (!result) return result;

	// Verify the starting state
	result = ProgressCheck(1);
	if (result) result = DoDFUIdle(true, false);
	if (!result) return result;

	// Read maximum transfer size and check download supported
	result = ProgressCheck(2);
	if (result) result = transport->GetFunct(functional);
	if (!result) return result;
	if (!(functional.bmAttributes
	      & DFURequests::attribute_can_download))
	{
		return DFUEngine::fail_no_download;
	}

	// Perform the download
	Progress(DFUEngine::download_progress, 0, file->GetLength());
	result = transport->PreDnload(file);
	if (!result) return result;
	file->SeekToBegin();
	uint16 blockNum = 0;
	uint16 bufferLength = functional.wTransferSize;
	SmartPtr<uint8, true> buffer(new uint8[bufferLength]);
	while (file->GetPosition() != file->GetLength())
	{
		// Update the progress indicator
		int percent = 3 + (92 * file->GetPosition())
		                   / file->GetLength();
		result = ProgressCheck(percent);
		if (!result) return result;

		// Extract the next block from the DFU file
		uint16 next = file->Read(buffer, bufferLength);

		// Check the status
		result = DoDownloadStatus(status);
		if (!result) return result;

		// Download the next block
		result = transport->Dnload(blockNum++, buffer, next);
		if (!result) return result;

		// Update the progress indicator again
		Progress(DFUEngine::download_progress,
		         file->GetPosition(), file->GetLength());

#ifdef OLD_CODE
		// Wait for the operation to complete (do not check for abort)
		if (status.bwPollTimeout < downloadProgressMilliseconds) AccurateSleep(status.bwPollTimeout);
		else AccurateSleep(status.bwPollTimeout, DFUEngine::download_processing);
#else
		AccurateSleep(shortTimeout);

		// Check the stutus is OK (use newStatus to ensure we don't overwrite old time)
			
		// Use different status so we don't get next write time!!
		result = DoDownloadStatus(newStatus);
		if (!result) return result;

		// Use new bState!!
		if (newStatus.bState == DFURequests::dfu_dnload_busy)
		{
			// Wait the proper amout of time!!
			if (status.bwPollTimeout < downloadProgressMilliseconds) 
			{
			  // Use original status
			  AccurateSleep(status.bwPollTimeout);
			}
			else 
			{
			  // Use original status
			  AccurateSleep(status.bwPollTimeout, DFUEngine::download_processing);
			}
		}
#endif
	}

	// Check the status
	Progress(DFUEngine::download_verify);
	result = ProgressCheck(96);
	if (result) result = DoDownloadStatus(status);
	if (!result) return result;

	// End the download
	result = ProgressCheck(97);
	if (result) result = transport->Dnload(blockNum, 0, 0);
	if (!result) return result;

	// Wait for the operation to complete (do not check for abort)
	Progress(98);

	if (status.bwPollTimeout < downloadProgressMilliseconds) AccurateSleep(status.bwPollTimeout);
	else AccurateSleep(status.bwPollTimeout, DFUEngine::download_processing);

	// Check the status if manifestation tolerant
	if (functional.bmAttributes
	    & DFURequests::attribute_manifestation_tolerant)
	{
		result = ProgressCheck(99);
		if (result) result = DoDFUIdle();
		if (!result) return result;
	}

	// Successful if this point reached
	return DFUEngine::success;
}

DFUEngine::Result DFUEngineBase::DoManifest()
{
	DFUEngine::Result result;
	DFURequests::DeviceDescriptor device;
	DFURequests::InterfaceDescriptor iface;
	DFURequests::DFUStatus status;

	// Ensure that the transport has been started
	Progress(DFUEngine::manifest_start_runtime);
	result = DoConnect(true);
	if (!result) return result;

	// May not be manifestation tolerant so do not check status

	// Perform a reset and restart transport
	result = ProgressCheck(10);
	if (result) result = transport->Reset();
	if (result) result = DoConnect(false);
	if (!result) return result;

	// Read the device descriptor to determine mode
	result = ProgressCheck(30);
	if (result) result = transport->GetDevice(device);
	if (!result) return result;
	result = transport->GetInterfaceDFU(iface);
	if (!result) return result;

	// Special case if still in DFU mode
	if (IsDFUMode(device, iface))
	{
		// Check the status
		result = ProgressCheck(50);
		if (result) result = transport->GetStatus(status);
		if (!result) return result;

		// Most likely reason is corrupt firmware
		if (status.bStatus == DFURequests::err_firmware)
		{
			return DFUEngine::fail_dfu_firmware;
		}

		// Abort any operation, ignoring result code
		Progress(DFUEngine::manifest_retry_runtime);
		result = ProgressCheck(60);
		if (result) result = DoDFUIdle(true, true);
		if (!result) return result;

		// Try to reset the device again and restart the transport
		result = ProgressCheck(70);
		if (result) result = transport->Reset();
		if (result) result = DoConnect(false);

		// Read the device descriptor again to determine mode
		result = ProgressCheck(80);
		if (result) result = transport->GetDevice(device);
		if (!result) return result;
		result = transport->GetInterfaceDFU(iface);
		if (!result) return result;

	}

	// Generate an error if still in DFU mode
	if (IsDFUMode(device, iface))
	{
		// Check the status
		Progress(DFUEngine::manifest_fail);
		result = ProgressCheck(90);
		if (result) result = transport->GetStatus(status);
		if (!result) return result;

		// Generate a suitable error
		result = transport->MapDFUStatus(status.bStatus,
			                             status.iString);
		if (result) result =  DFUEngine::fail_dfu_firmware;
		return result;
	}

	// End with a reset to allow subsequent connections
	result = transport->Reset(false);
	if (!result) result = transport->Disconnect();
	if (!result) return result;

	// Successful if this point reached
	return DFUEngine::success;
}

DFUEngine::Result DFUEngineBase::DoRecover(const DFUEngine::Result &failure)
{
	DFUEngine::Result result;
	DFURequests::DFUStatus status;

	// Delay a little before starting the recovery action
	Progress(DFUEngine::recover_clean_up, failure);
	Sleep(recoverDelay);

	// Ensure that the transport has been started
	result = DoConnect(false, false);
	if (!result) return result;

	// Abort any DFU operation
	result = transport->GetStatus(status);
	if (result && transport->CanAbort(status.bState))
	{
		result = transport->Abort();
	}
	if (result && transport->CanClrStatus(status.bState))
	{
		result = transport->ClrStatus();
	}
	if (!result)
	{
		// Disconnect and reconnect the transport if any problem
		Progress(DFUEngine::recover_fail, failure);
		result = transport->Disconnect();
		if (result) result = DoConnect(false, false);
		if (!result) return result;
	}

	// Perform a reset and disconnect the transport
	result = transport->Reset(false);
	if (!result) return result;

	// Assume successful if this point reached
	return DFUEngine::success;
}

// Composite operations
DFUEngine::Result DFUEngineBase::DoConnect(bool hintDFU, bool abort)
{
	DFUEngine::Result result;

	// Ensure that there is a transport
	if (!transport) return DFUEngine::fail_no_transport;

	// Enable or disable abort event checking as appropriate
	transport->SetAbort(abort ? &threadAbort : 0);

	// Set object to receive progress notifications
	transport->SetProgress(this);

	// Ensure that the transport has been started
	result = transport->Connect(hintDFU);
	if (!result) return result;

	// Successful if this point reached
	return DFUEngine::success;
}

DFUEngine::Result DFUEngineBase::DoDFUIdle(bool clear, bool abort)
{
	DFUEngine::Result result;
	DFURequests::DFUStatus status;

	// Read the status
	result = transport->GetStatus(status);
	if (!result) return result;

	// Attempt to abort any active operation if enabled
	if (abort && transport->CanAbort(status.bState))
	{
		result = transport->Abort();
		if (result) result = transport->GetStatus(status);
		if (!result) return result;
	}

	// Attempt to clear any error condition if enabled
	if (clear && transport->CanClrStatus(status.bState))
	{
		result = transport->ClrStatus();
		if (result) result = transport->GetStatus(status);
		if (!result) return result;
	}

	// Fail if still not idle and error free
	if (status.bStatus != DFURequests::ok)
	{
		return transport->MapDFUStatus(status.bStatus,
			                           status.iString);
	}
	if (status.bState != DFURequests::dfu_idle)
	{
		return DFUEngine::fail_mode_not_idle;
	}

	// Successful if this point reached
	return DFUEngine::success;
}

DFUEngine::Result DFUEngineBase::DoDownloadStatus(DFURequests::DFUStatus &status)
{
	DFUEngine::Result result;

	// Read the status
	result = transport->GetStatus(status);
	if (!result) return result;

	// Special case if busy
	if ((status.bState == DFURequests::dfu_dnload_busy)
		|| (status.bState == DFURequests::dfu_manifest))
	{
		// Wait for the operation to complete (do not check for abort)
		Sleep(status.bwPollTimeout);

		// Check the status again
		result = transport->GetStatus(status);
		if (!result) return result;
	}

	// Special case if error
	if ((status.bState == DFURequests::dfu_error)
		&& (status.bStatus != DFURequests::ok))
	{
		return transport->MapDFUStatus(status.bStatus,
			                           status.iString);
	}

	// Ensure that the download is idle
	if ((status.bState != DFURequests::dfu_idle)
		&& (status.bState != DFURequests::dfu_dnload_idle))
	{
		return DFUEngine::fail_mode_not_dnload_idle;
	}

	// Successful if this point reached
	return DFUEngine::success;
}

// Status checking
bool DFUEngineBase::IsDFUMode(const DFURequests::DeviceDescriptor &device, const DFURequests::InterfaceDescriptor &iface)
{
	return (device.bDeviceClass == 0 && device.bDeviceSubClass == 0 && device.bDeviceProtocol == 0)
		?
			((iface.bInterfaceClass == dfuInterfaceClass)
			&& (iface.bInterfaceSubClass == dfuInterfaceSubClass)
			&& ((iface.bInterfaceProtocol == dfu10InterfaceProtocol) || (iface.bInterfaceProtocol == dfu11DFUInterfaceProtocol)))
		:
			((device.bDeviceClass == dfu10DeviceClass)
			&& (device.bDeviceSubClass == dfu10DeviceSubClass)
			&& (device.bDeviceProtocol == dfu10DeviceProtocol));
}

// Enable or disable HID mode for chips that support this.
bool DFUEngineBase::EnableHidMode(bool enable)
{
	bool bRet = false;
	DFUEngine::Result result;

	// Ensure that there is a transport
	if (!transport) return 
		false;

	// Ensure that the transport has been started
	result = DoConnect(true);
	if (!result) 
		return false;

	result = transport->EnableHidMode(enable);

	if (result) 
		bRet = true;

	return bRet;
}
