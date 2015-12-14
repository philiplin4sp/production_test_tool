///////////////////////////////////////////////////////////////////////
//
//	File	: DFURequestsSimulated.cpp
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFURequestsSimulated
//
//	Purpose	: Implementation of basic DFU operations specific to
//			  a simulated connection. This does not communicate with
//			  a BlueCore module; it just provides realistic delays and
//			  responses.
//
//			  All of the methods are implementations of virtual
//			  functions provided by DFURequests, so see that file for
//			  descriptions.
//
//	Modification history:
//	
//		1.1		02:Oct:00	at	File created.
//		1.2		12:Oct:00	at	Expanded comment describing this class.
//		1.3		24:Oct:00	at	Corrected scoping of DFU interface
//								number.
//		1.4		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.5		23:May:01	at	Corrected change history.
//		1.6		23:Jul:01	at	Added version string.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFURequestsSimulated.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFURequestsSimulated.h"
#include "thread/signal_box.h"
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
const char *dfurequestssimulated_id = "$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFURequestsSimulated.cpp#1 $ Copyright (C) 2000-2006 Cambridge Silicon Radio Limited";

// DFU configuration settings
static const uint16 dfuDetachTimeout = 5000;
static const uint16 dfuTransferSize = 1024;
static const bool dfuManifestationTolerant = true;
static const bool dfuCanUpload = true;
static const bool dfuCanDownload = true;

// Programming and manifestation timings
static const double programmingBytesPerSecond = 25.0e3;
static const double manifestationSeconds = 0.1;

// Base timing for all operations
static const double dfuOperationSeconds = 0.001;
static const double dfuResetSeconds = 0.05;

// Simulated data transfer rates
static const double downloadBytesPerSecond = 10.0e3;
static const double uploadBytesPerSecond = 100.0e3;

// Simulated firmware size for upload
static const uint32 uploadFirmwareSize = 160 * 2 * 1024;

// The DFU interface number is constant
static const uint16 dfuInterfaceNumber = 0;

// Product identification
static const uint8 appDeviceClass = 0xE0;
static const uint8 appDeviceSubClass = 0x01;
static const uint8 appDeviceProtocol = 0x01;
static const uint16 appVendor = 0x0A12;
static const uint16 appProduct = 0x0001;
static const uint16 appDevice = 0x0001;
static const uint16 dfuProduct = 0xFFFF;
static const uint16 dfuDevice = 0x0001;

// Constructor
DFURequestsSimulated::DFURequestsSimulated()
{
	dfuStatus = ok;
	downloadValid = true;
	SetState(app_idle);
}

// Destructor
DFURequestsSimulated::~DFURequestsSimulated()
{
}

// Individual USB DFU control requests
DFUEngine::Result DFURequestsSimulated::RPCDetach(uint16 timeout)
{
	UpdateState();
	SingleSignal().wait(uint32(dfuOperationSeconds * 1000.0));
	if (dfuState == app_idle) SetState(app_detach, app_idle, timeout);
	else return SetError(err_stalledpkt);
	return DFUEngine::success;
}

DFUEngine::Result DFURequestsSimulated::RPCDnload(uint16 blockNum, const void *buffer, uint16 bufferLength)
{
	UpdateState();
	SingleSignal().wait(uint32((dfuOperationSeconds + bufferLength / downloadBytesPerSecond) * 1000.0));
	if ((dfuState == dfu_idle) && bufferLength && dfuCanDownload)
	{
		SetState(dfu_dnload_sync);
		downloadDone = 0;
		downloadPending = bufferLength;
	}
	else if (dfuState == dfu_dnload_idle)
	{
		if (bufferLength)
		{
			SetState(dfu_dnload_sync);
			downloadPending = bufferLength;
		}
		else SetState(dfu_manifest_sync);
	}
	else return SetError(err_stalledpkt);
	return DFUEngine::success;
}

DFUEngine::Result DFURequestsSimulated::RPCUpload(uint16 blockNum, void *buffer, uint16 bufferLength, uint16 &replyLength)
{
	UpdateState();
	SingleSignal().wait(uint32(dfuOperationSeconds * 1000.0));
	if ((dfuState == dfu_idle) && dfuCanUpload)
	{
		uploadDone = 0;
		SetState(dfu_upload_idle);
	}
	if (dfuState == dfu_upload_idle)
	{
		replyLength = uint16(min(bufferLength, uploadFirmwareSize - uploadDone));
		memset(buffer, 42, replyLength);
		SingleSignal().wait(uint32((dfuOperationSeconds + replyLength / uploadBytesPerSecond) * 1000.0));
		uploadDone += replyLength;
		if (replyLength < bufferLength) SetState(dfu_idle);
	}
	else return SetError(err_stalledpkt);
	return DFUEngine::success;
}

DFUEngine::Result DFURequestsSimulated::RPCGetStatus(DFUStatus &status)
{
	UpdateState();
	SingleSignal().wait(uint32(dfuOperationSeconds * 1000.0));
	status.bwPollTimeout = 0;
	if (dfuState == app_idle)
	{
		return SetError(err_stalledpkt);
	}
	else if (dfuState == dfu_dnload_sync)
	{
		if (downloadPending)
		{
			status.bwPollTimeout = uint32((downloadPending / programmingBytesPerSecond) * 1000.0);
			downloadPending = 0;
			downloadValid = false;
			SetState(dfu_dnload_busy, dfu_dnload_sync, status.bwPollTimeout);
		}
		else SetState(dfu_dnload_idle);
	}
	else if (dfuState == dfu_manifest_sync)
	{
		if (manifestPending)
		{
			status.bwPollTimeout = uint32(manifestationSeconds * 1000.0);
			manifestPending = false;
			SetState(dfu_manifest, dfuManifestationTolerant ? dfu_manifest_sync : dfu_manifest_wait_reset, status.bwPollTimeout);
		}
		else
		{
			downloadValid = true;
			SetState(dfu_idle);
		}
	}
	status.bStatus = dfuStatus;
	status.bState = dfuState;
	status.iString = 0;
	return DFUEngine::success;
}

DFUEngine::Result DFURequestsSimulated::RPCClrStatus()
{
	UpdateState();
	SingleSignal().wait(uint32(dfuOperationSeconds * 1000.0));
	if (dfuState == dfu_error)
	{
		dfuStatus = ok;
		SetState(dfu_idle);
	}
	else return SetError(err_stalledpkt);
	return DFUEngine::success;
}

DFUEngine::Result DFURequestsSimulated::RPCGetState(uint8 &state)
{
	UpdateState();
	SingleSignal().wait(uint32(dfuOperationSeconds * 1000.0));
	if (state == app_idle) return SetError(err_stalledpkt);
	state = dfuState;
	return DFUEngine::success;
}

DFUEngine::Result DFURequestsSimulated::RPCAbort()
{
	UpdateState();
	SingleSignal().wait(uint32(dfuOperationSeconds * 1000.0));
	SetState(dfu_idle);
	if ((dfuState == dfu_upload_idle) || (dfuState == dfu_dnload_idle))
	{
		SetState(dfu_idle);
	}
	else return SetError(err_stalledpkt);
	return DFUEngine::success;
}

// Non USB DFU control requests
DFUEngine::Result DFURequestsSimulated::RPCReset()
{
	UpdateState();
	SingleSignal().wait(uint32(dfuResetSeconds * 1000.0));
	if (!downloadValid)
	{
		dfuStatus = err_firmware;
		SetState(dfu_error);
	}
	else if (dfuState == app_detach) SetState(dfu_idle);
	else SetState(app_idle);
	return DFUEngine::success;
}

DFUEngine::Result DFURequestsSimulated::RPCGetInterfaceDFU(InterfaceDescriptor &descriptor)
{
	UpdateState();
	SingleSignal().wait(uint32(dfuOperationSeconds * 1000.0));

	descriptor.bLength = sizeof(InterfaceDescriptor);
	descriptor.bDescriptorType = DFURequests::descriptor_interface;
	// Interface number is constant for UART transports
	descriptor.bInterfaceNumber = dfuInterfaceNumber;
	descriptor.bAlternateSetting = 0;
	descriptor.bNumEndpoints = 0;
	descriptor.bInterfaceClass = dfuInterfaceClass;
	descriptor.bInterfaceSubClass = dfuInterfaceSubClass;
	descriptor.bInterfaceProtocol = dfu10InterfaceProtocol;
	descriptor.iInterface = 0;

	return DFUEngine::success;
}

DFUEngine::Result DFURequestsSimulated::RPCGetDevice(DeviceDescriptor &descriptor)
{
	UpdateState();
	SingleSignal().wait(uint32(dfuOperationSeconds * 1000.0));
	descriptor.bLength = sizeof(DeviceDescriptor);
	descriptor.bDescriptorType = descriptor_device;
	descriptor.idVendor = appVendor;
	if ((dfuState == app_idle) || (dfuState == app_detach))
	{
		descriptor.bDeviceClass = appDeviceClass;
		descriptor.bDeviceSubClass = appDeviceSubClass;
		descriptor.bDeviceProtocol = appDeviceProtocol;
		descriptor.idProduct = appProduct;
		descriptor.bcdDevice = appDevice;
	}
	else
	{
		descriptor.bDeviceClass = dfu10DeviceClass;
		descriptor.bDeviceSubClass = dfu10DeviceSubClass;
		descriptor.bDeviceProtocol = dfu10DeviceProtocol;
		descriptor.idProduct = dfuProduct;
		descriptor.bcdDevice = dfuDevice;
	}
	return DFUEngine::success;
}

DFUEngine::Result DFURequestsSimulated::RPCGetFunct(DFUFunctionalDescriptor &descriptor)
{
	UpdateState();
	SingleSignal().wait(uint32(dfuOperationSeconds * 1000.0));
	descriptor.bLength = sizeof(DFUFunctionalDescriptor);
	descriptor.bDescriptorType = descriptor_dfu_functional;
	descriptor.bmAttributes = 0;
	if (dfuManifestationTolerant) descriptor.bmAttributes |= attribute_manifestation_tolerant;
	if (dfuCanUpload) descriptor.bmAttributes |= attribute_can_upload;
	if (dfuCanDownload) descriptor.bmAttributes |= attribute_can_download;
	descriptor.wDetachTimeout = dfuDetachTimeout;
	descriptor.wTransferSize = dfuTransferSize;
	descriptor.bcdDFUVersion = DFU_VERSION_1_0;
	return DFUEngine::success;
}

DFUEngine::Result DFURequestsSimulated::RPCGetString(uint8 index, CStringX &descriptor)
{
	return DFUEngine::Result(DFUEngine::fail_unimplemented, _T("DFURequestSimulated::RPCGetString()"));
}

// Generic control requests
DFUEngine::Result DFURequestsSimulated::ControlRequest(const DFURequests::Setup &setup,
	                                                   void *buffer,
							                           uint16 bufferLength,
	                                                   uint16 *replyLength)
{
	return DFUEngine::Result(DFUEngine::fail_unimplemented, _T("DFURequestsSimulated::ControlRequest()"));
}

// State transitions
void DFURequestsSimulated::SetState(uint8 state)
{
	dfuState = state;
	dfuStateTimeout = 0;
}

void DFURequestsSimulated::SetState(uint8 state, uint8 stateNext, uint32 timeout)
{
	dfuState = state;
	dfuStateNext = stateNext;
	dfuStateTimeout = GetTickCount() + timeout;
}

void DFURequestsSimulated::UpdateState()
{
	if (dfuStateTimeout)
	{
		if (0 <= int32(GetTickCount() - dfuStateTimeout))
		{
			dfuState = dfuStateNext;
			dfuStateTimeout = 0;
		}
	}
}

// Status handling
DFUEngine::Result DFURequestsSimulated::SetError(uint8 status)
{
	dfuStatus = status;
	SetState(dfu_error);
	return DFUEngine::fail_dfu_stalledpkt;
}

// Enable or disable HID
DFUEngine::Result DFURequestsSimulated::RPCEnableHidMode(bool enable)
{
	return DFUEngine::fail_unimplemented;
}