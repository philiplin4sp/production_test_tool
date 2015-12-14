///////////////////////////////////////////////////////////////////////
//
//	File	: DFUTransportUSBBTW.cpp
//		  Copyright (C) 2001-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFUTransportUSBBTW
//
//	Purpose	: Implementation of the raw USB transport for Widcomm
//			  device drivers.
//
//			  A list of suitable USB devices may be obtained using
//			  the following member function:
//				EnumerateDevices	- Set the supplied list to a list
//									  of strings describing the
//									  available devices.
//
//			  A corresponding member function tests whether a named
//			  device is supported by this transport:
//				IsA					- Test whether named device is
//									  an instance of the driver
//									  supported by this transport.
//
//			  A standard HCI transport can be created for a named
//			  device using the following member function:
//				ConnectDevice		- Create a transport to the named
//									  device.
//
//			  All of the other methods are implementations of virtual
//			  functions provided by DFUTransportUSB, so see that file
//			  for descriptions.
//
//	Modification history:
//	
//		1.1		05:Oct:01	at	File created.
//		1.2		08:Oct:01	at	Partial support for DFU mode driver.
//		1.3		08:Oct:01	at	Completed support for DFU mode driver.
//		1.4		08:Oct:01	at	Non-overlapped I/O used.
//		1.5		08:Oct:01	at	Corrected DFU mode driver support.
//		1.6		09:Oct:01	at	Set replyLength parameter of
//								ControlRequest.
//		1.7		09:Oct:01	at	Reverted back to trying all device
//								indexes during enumeration. Faked
//								GetStatus request in run-time mode.
//		1.8		09:Oct:01	at	Corrected test of IoctlDFUDone status.
//		1.9		09:Oct:01	at	Corrected silly typo.
//		1.10	10:Oct:01	at	Corrected device names for more than
//								ten driver instances and improved
//								recover behaviour.
//		1.11	10:Oct:01	at	Result of IoctlStop ignored during
//								reset.
//		1.12	10:Oct:01	at	Extra delay for first download block.
//		1.13	11:Oct:01	at	Only support first ten devices.
//		1.14	14:Nov:01	at	Added basic Unicode support.
//		1.15	09:Jan:02	at	Alternative mechanism for reading
//								build identifier via BCCMD.
//		1.16	04:Nov:02	at	Added workaround for Windows XP SP1.
//		1.17	06:Nov:02	at	Fixed workaround for Windows XP SP1.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUTransportUSBBTW.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUTransportUSBBTW.h"
#include "DFUTransportCOM.h"

// Visual C++ debugging code
#ifdef _DEBUG
#include <crtdbg.h>
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define new DEBUG_NEW
#endif

// Version string
const char *dfutransportusbbtw_id = "$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUTransportUSBBTW.cpp#1 $ Copyright (C) 2000-2006 Cambridge Silicon Radio Limited";

// DFU configuration settings
static const uint16 dfuDetachTimeout = 5000;
static const uint16 dfuTransferSize = 1000;
static const uint16 dfuBufferSize = 32 * 1024;
static const bool dfuManifestationTolerant = false;
static const bool dfuCanUpload = false;
static const bool dfuCanDownload = true;

// The DFU interface number is constant
static const uint16 dfuInterfaceNumber = 0;

// Fake device descriptor for DFU mode
static const uint16 dfuVendor = 0x0A12;
static const uint16 dfuProduct = 0xFFFF;
static const uint16 dfuDevice = 0x0001;

// Persistent store key numbers
static const uint16 pskey_usb_device_class_codes = 0x02bd;
static const uint16 pskey_usb_vendor_id = 0x02be;
static const uint16 pskey_usb_product_id = 0x02bf;
static const uint16 pskey_usb_release_num = 0x02c0;

// Range of USB device numbers
static const int firstDevice = 0;
static const int lastDevice = 9;
static const int lastDeviceDFU = 9;

// USB device names
static const TCHAR deviceFormat[] = _T("BTWUSB-%d");
static const TCHAR deviceFormatDFU[] = _T("WIDCOMDFU%d");

// Device IO control codes
static const uint32 ioctl_stop = 0x222004;
static const uint32 ioctl_write_file = 0x222010;
static const uint32 ioctl_start_file_download = 0x222014;
static const uint32 ioctl_get_driver_status = 0x222018;
static const uint32 ioctl_dfu_done = 0x222024;
static const uint32 ioctl_go_dfu = 0x2222a0;

// Microsecond delay between successive status polls
static const uint32 delayFirstPollStatus = 1000;
static const uint32 delayPollStatus = 100;

// Mapping of device driver return codes to DFUEngine result codes
struct returnCodeMapEntry
{
	uint32 rc;
	DFUEngine::ResultCode result;
};
static const returnCodeMapEntry returnCodeMap[] =
{
	{DFUTransportUSBBTW::rc_ok,				DFUEngine::success},
	{DFUTransportUSBBTW::rc_error,			DFUEngine::fail_btw_error},
	{DFUTransportUSBBTW::rc_not_supported,	DFUEngine::fail_btw_not_suppored},
	{DFUTransportUSBBTW::rc_busy,			DFUEngine::fail_btw_busy},
	{DFUTransportUSBBTW::rc_file_error,		DFUEngine::fail_btw_file},
	{DFUTransportUSBBTW::rc_download_error,	DFUEngine::fail_btw_download},
	{DFUTransportUSBBTW::rc_system_error,	DFUEngine::fail_btw_system},
	{DFUTransportUSBBTW::rc_pending,		DFUEngine::success},
	{DFUTransportUSBBTW::rc_vendor_error,	DFUEngine::fail_btw_vendor}
};

// Mapping of device driver status codes to DFU status codes
struct statusCodeMapEntry
{
	uint32 status;
	uint32 statusDFU;
};
static const statusCodeMapEntry statusCodeMap[] =
{
	{DFUTransportUSBBTW::ok,				DFURequests::ok},
	{DFUTransportUSBBTW::err_target,		DFURequests::err_target},
	{DFUTransportUSBBTW::err_file,			DFURequests::err_file},
	{DFUTransportUSBBTW::err_write,			DFURequests::err_write},
	{DFUTransportUSBBTW::err_erase,			DFURequests::err_erase},
	{DFUTransportUSBBTW::err_check_erased,	DFURequests::err_check_erased},
	{DFUTransportUSBBTW::err_prog,			DFURequests::err_prog},
	{DFUTransportUSBBTW::err_verify,		DFURequests::err_verify},
	{DFUTransportUSBBTW::err_check_verify,	DFURequests::err_verify},
	{DFUTransportUSBBTW::err_address,		DFURequests::err_address},
	{DFUTransportUSBBTW::err_not_done,		DFURequests::err_not_done},
	{DFUTransportUSBBTW::err_firmware,		DFURequests::err_firmware},
	{DFUTransportUSBBTW::err_vendor,		DFURequests::err_vendor},
	{DFUTransportUSBBTW::err_usbr,			DFURequests::err_usbr},
	{DFUTransportUSBBTW::err_por,			DFURequests::err_por},
	// DFUTransportUSBBTW::err_unknown is handled as a special case
	{DFUTransportUSBBTW::err_stalledpkt,	DFURequests::err_stalledpkt},
	{DFUTransportUSBBTW::err_stalledreset,	DFURequests::err_por}
};

// Constructor
DFUTransportUSBBTW::DFUTransportUSBBTW()
{
}

// Destructor
DFUTransportUSBBTW::~DFUTransportUSBBTW()
{
	// Ensure that the device is closed cleanly
	Close();
}

// Device enumeration
int DFUTransportUSBBTW::EnumerateDevices(CStringListX &devices)
{
	// Enumerate normal mode devices
	DeviceBTW::Enumerate(deviceFormat, firstDevice, lastDevice, devices);

	// Enumerate DFU mode devices also
	CStringListX devicesDFU;
	DeviceBTW::Enumerate(deviceFormatDFU, firstDevice, lastDeviceDFU, devicesDFU);
	devices.AddTail(&devicesDFU);

	// Return the number of available USB devices
	return devices.GetCount();
}

bool DFUTransportUSBBTW::IsA(const CStringX device)
{
	return DeviceBTW::IsA(device, deviceFormat, firstDevice, lastDevice)
	       || DeviceBTW::IsA(device, deviceFormatDFU, firstDevice, lastDeviceDFU);
}

// HCI transport instantiation
DFUEngine::Result DFUTransportUSBBTW::ConnectDevice(const TCHAR *device,
                                                    DFUTransportCOM &transport,
													HANDLE handle)
{
	// DFU mode drivers do not present an HCI interface
	if (DeviceBTW::IsA(device, deviceFormatDFU, firstDevice, lastDeviceDFU))
	{
		return DFUEngine::Result(DFUEngine::fail_usb_open, device);
	}

	// Attempt to start the transport
	return transport.ConnectUSBBTW(DeviceBTW::Canonicalise(device, true), handle);
}
	
// Opening and closing a device
DFUEngine::Result DFUTransportUSBBTW::Open(const CStringX device)
{
	// Attempt to open the device
	name = device;
	doneDetach = false;
	doneDownload = false;
	DFUEngine::Result result = this->device.Open(device);
	if (!result) return result;

	// Attempt to start an HCI transport
	DFUTransportCOM transport;
	result = ConnectDevice(device, transport, this->device);
	if (result) isDFU = false;
	else if (result.GetCode() == DFUEngine::fail_usb_open) isDFU = true;
	else return result;

	// Construct a fake device descriptor
	deviceDescriptor.bLength = sizeof(DFURequests::DeviceDescriptor);
	deviceDescriptor.bDescriptorType = DFURequests::descriptor_device;
	if (isDFU)
	{
		// Invent a DFU mode device descriptor
		deviceDescriptor.bDeviceClass = dfu10DeviceClass;
		deviceDescriptor.bDeviceSubClass = dfu10DeviceSubClass;
		deviceDescriptor.bDeviceProtocol = dfu10DeviceProtocol;
		deviceDescriptor.idVendor = dfuVendor;
		deviceDescriptor.idProduct = dfuProduct;
		deviceDescriptor.bcdDevice = dfuDevice;
	}
	else
	{
		uint16 read;

		// Read the device class codes from persistent store
		struct
		{
			uint16 deviceClass;
			uint16 deviceSubClass;
			uint16 deviceProtocol;
		} deviceClassCodes;
		result = transport.BCCMDGetPS(pskey_usb_device_class_codes, &deviceClassCodes, sizeof(deviceClassCodes), &read);
		if (!result) return result;
		if (read != sizeof(deviceClassCodes)) return DFUEngine::fail_usb_ps_read;
		deviceDescriptor.bDeviceClass = uint8(deviceClassCodes.deviceClass);
		deviceDescriptor.bDeviceSubClass = uint8(deviceClassCodes.deviceSubClass);
		deviceDescriptor.bDeviceProtocol = uint8(deviceClassCodes.deviceProtocol);

		// Read the vendor identifier from persistent store
		uint16 vendor;
		result = transport.BCCMDGetPS(pskey_usb_vendor_id, &vendor, sizeof(vendor), &read);
		if (!result) return result;
		if (read != sizeof(vendor)) return DFUEngine::fail_usb_ps_read;
		deviceDescriptor.idVendor = vendor;

		// Read the product identifier from persistent store
		uint16 product;
		result = transport.BCCMDGetPS(pskey_usb_product_id, &product, sizeof(product), &read);
		if (!result) return result;
		if (read != sizeof(product)) return DFUEngine::fail_usb_ps_read;
		deviceDescriptor.idProduct = product;

		// Read the build identifier from persistent store
		uint16 device;
		result = transport.BCCMDGetPS(pskey_usb_release_num, &device, sizeof(device), &read);
		if (result)
		{
			// Successfully read from persistent store
			if (read != sizeof(device)) return DFUEngine::fail_usb_ps_read;
			deviceDescriptor.bcdDevice = device;
		}
		else
		{
			// Try a different BCCMD command if not in persistent store
			result = transport.BCCMDGet(DFUTransportCOM::bccmd_varid_buildid, device);
			if (!result) return result;
			deviceDescriptor.bcdDevice = (((device / 1000) % 10) << 12)
			                             + (((device / 100) % 10) << 8)
			                             + (((device / 10) % 10) << 4)
			                             + (device % 10);
		}
	}

	// Successful if this point reached
	return DFUEngine::success;
}

DFUEngine::Result DFUTransportUSBBTW::Close()
{
	// Close the raw device
	return device.Close();
}

// Pre-download preparation step
DFUEngine::Result DFUTransportUSBBTW::PreDnload(DFUFile &file)
{
	// Prepare the file image to send
	fileLength = file->GetLength() + sizeof(DFUFile::Suffix);
	SmartPtr<uint8, true> buffer(new uint8[fileLength]);
	file->SeekToBegin();
	file->Read(buffer, fileLength);
	file.GetSuffix(*(DFUFile::Suffix *) (buffer + file->GetLength()));

	// Start the file download
	DFUEngine::Result result = IoctlStartFileDownload(fileLength, min(dfuBufferSize, fileLength));
	if (!result) return result;

	// Supply the file data to the device driver
	for (uint32 offset = 0; offset < fileLength; offset += dfuBufferSize)
	{
		result = IoctlWriteFile(buffer + offset, min(dfuBufferSize, fileLength - offset));
		if (!result) return result;
	}

	// Reset the download progress
	fileDone = 0;

	// Successful if this point reached
	return DFUEngine::success;
}

// Non USB DFU control requests
DFUEngine::Result DFUTransportUSBBTW::Reset()
{
	// Ensure that the transport is valid
	if (!bool(device)) return DFUEngine::fail_usb_none;

	// No action required unless a DFU mode driver
	if (isDFU)
	{
		// Complete or abort the DFU process as appropriate
		if (doneDownload)
		{
			DFUEngine::Result result = IoctlDFUDone();
			if (!result) return result;
		}
		else IoctlStop();
	}
	else if (!doneDetach)
	{
		// Attempt to use a BCCMD cold reset to clean up
		DFUTransportCOM transport;
		DFUEngine::Result result = ConnectDevice(name, transport, device);
		if (result) result = transport.BCCMDColdReset();
		if (!result) return result;
	}

	// Successful if this point reached
	return DFUEngine::success;
}

DFUEngine::Result DFUTransportUSBBTW::GetInterfaceDFU(DFURequests::InterfaceDescriptor &descriptor)
{
	// Ensure that the transport is valid
	if (!bool(device)) return DFUEngine::fail_usb_none;

	// Set a dummy value
	descriptor.bLength = sizeof(DFURequests::InterfaceDescriptor);
	descriptor.bDescriptorType = DFURequests::descriptor_interface;
	// Interface number is constant for UART transports
	descriptor.bInterfaceNumber = dfuInterfaceNumber;
	descriptor.bAlternateSetting = 0;
	descriptor.bNumEndpoints = 0;
	descriptor.bInterfaceClass = dfuInterfaceClass;
	descriptor.bInterfaceSubClass = dfuInterfaceSubClass;
	descriptor.bInterfaceProtocol = dfu10InterfaceProtocol;
	descriptor.iInterface = 0;

	// Successful if this point reached
	return DFUEngine::success;
}

DFUEngine::Result DFUTransportUSBBTW::GetDevice(DFURequests::DeviceDescriptor &descriptor)
{
	// Ensure that the transport is valid
	if (!bool(device)) return DFUEngine::fail_usb_none;

	// Return the device descriptor recorded when the device was opened
	descriptor = deviceDescriptor;

	// Successful if this point reached
	return DFUEngine::success;
}

DFUEngine::Result DFUTransportUSBBTW::GetFunct(DFURequests::DFUFunctionalDescriptor &descriptor)
{
	// Ensure that the transport is valid
	if (!bool(device)) return DFUEngine::fail_usb_none;

	// Set dummy values
	descriptor.bLength = sizeof(DFURequests::DFUFunctionalDescriptor);
	descriptor.bDescriptorType = DFURequests::descriptor_dfu_functional;
	descriptor.bmAttributes = 0;
	if (dfuManifestationTolerant) descriptor.bmAttributes |= DFURequests::attribute_manifestation_tolerant;
	if (dfuCanUpload) descriptor.bmAttributes |= DFURequests::attribute_can_upload;
	if (dfuCanDownload) descriptor.bmAttributes |= DFURequests::attribute_can_download;
	descriptor.wDetachTimeout = dfuDetachTimeout;
	descriptor.wTransferSize = dfuTransferSize;
	descriptor.bcdDFUVersion = DFU_VERSION_1_0;

	// Successful if this point reached
	return DFUEngine::success;
}

DFUEngine::Result DFUTransportUSBBTW::GetString(uint8 index, CStringX &descriptor)
{
	return DFUEngine::Result(DFUEngine::fail_unimplemented, _T("DFUTransportUSBBTW::GetString()"));
}

// Generic control requests
DFUEngine::Result DFUTransportUSBBTW::ControlRequest(const DFURequests::Setup &setup,
                                                     void *buffer,
                                                     uint16 bufferLength,
                                                     uint16 *replyLength)
{
	// Ensure that the transport is valid
	if (!bool(device)) return DFUEngine::fail_usb_none;

	// Action depends on the type of request
	DFURequests::DFUStatus *statusDFU = (DFURequests::DFUStatus *) buffer;
	switch (setup.bRequest)
	{
	case DFURequests::request_detach:
		// Start DFU mode
		{
			// Workaround for USB cycle port behaviour on Windows XP SP1
			DFUTransportCOM transport;
			if (ConnectDevice(name, transport, device))
			{
				transport.BCCMDRadiotestPause();
			}

			// Remember that a detach was performed
			doneDetach = true;
		}
		if (replyLength) *replyLength = 0;
		return IoctlGoDFU();
		break;

	case DFURequests::request_dnload:
		// Wait for the next block to be downloaded
		if (replyLength) *replyLength = 0;
		if (!fileDone) Sleep(delayFirstPollStatus);
		fileDone += setup.wLength;
		while (true)
		{
			// Check the current status
			Status status;
			DFUEngine::Result result = IoctlGetDriverStatus(status);
			if (!result) return result;

			// Return any error status
			if (status.status != ok) return DFUEngine::fail_dfu_stalledpkt;

			// The final zero length download request is handled differently
			if (setup.wLength == 0)
			{
				// Wait for the download to complete
				if (status.mode == mode_dfu_complete)
				{
					doneDownload = true;
					break;
				}
			}
			else
			{
				// Wait for the next block to have been downloaded
				if (fileDone <= status.bytes) break;
			}

			// Delay before trying again
			Sleep(delayPollStatus);
		}
		break;

	case DFURequests::request_upload:
		// Upload is not supported by the Widcomm driver
		return DFUEngine::fail_no_upload;
		break;

	case DFURequests::request_get_status:
		// Read the current status
		if (bufferLength < sizeof(DFURequests::DFUStatus)) return DFUEngine::fail_usb_buffer;
		if (replyLength) *replyLength = sizeof(DFURequests::DFUStatus);
		if (isDFU)
		{
			// Read the status from the DFU mode device driver
			Status status;
			DFUEngine::Result result = IoctlGetDriverStatus(status);
			if (!result) return result;
			statusDFU->bStatus = MapStatus(status.status);
			statusDFU->bState = status.state;
			/*
				This should probably set statusDFU->bStatus to DFURequests::err_firmware
				after a reset from DFU to DFU mode.
			*/
		}
		else
		{
			// Fake the status in run-time mode
			statusDFU->bStatus = DFURequests::ok;
			statusDFU->bState = DFURequests::app_idle;
		}
		statusDFU->bwPollTimeout = 0;
		statusDFU->iString = 0;
		break;

	case DFURequests::request_get_state:
		// Read the current state
		{
			if (bufferLength < sizeof(uint8)) return DFUEngine::fail_usb_buffer;
			if (replyLength) *replyLength = sizeof(uint8);
			Status status;
			DFUEngine::Result result = IoctlGetDriverStatus(status);
			if (!result) return result;
			*(uint8 *) buffer = status.state;
		}
		break;

	case DFURequests::request_clr_status:
	case DFURequests::request_abort:
		// Treat clear status and abort the same
		if (replyLength) *replyLength = 0;
		return IoctlStop();
		break;

	default:
		// There should not be any other requests
		return DFUEngine::Result(DFUEngine::fail_unimplemented, _T("DFUTransportUSBBTW::ControlRequest()"));
		break;
	}

	// Successful if this point reached
	return DFUEngine::success;
}

// The individual device driver control requests
DFUEngine::Result DFUTransportUSBBTW::IoctlStop()
{
	// Perform the request
	uint32 rc;
	uint32 read;
	DFUEngine::Result result = device.IOControl(ioctl_stop, 0, 0, &rc, sizeof(rc), &read);
	if (!result) return result;
	if (read < sizeof(rc)) return DFUEngine::fail_btw_status_short;
	if (sizeof(rc) < read) return DFUEngine::fail_btw_status_long;

	// Return the status
	return MapReturnCode(rc);
}

DFUEngine::Result DFUTransportUSBBTW::IoctlWriteFile(const void *buffer, uint32 bufferLength)
{
	// Prepare the parameter block
	uint32 paramsLength = sizeof(uint32) + bufferLength;
	SmartPtr<uint8, true> params(new uint8[paramsLength]);
	*(uint32 *) (uint8 *) params = bufferLength;
	memcpy(params + sizeof(uint32), buffer, bufferLength);

	// Perform the request
	uint32 rc;
	uint32 read;
	DFUEngine::Result result = device.IOControl(ioctl_write_file, params, paramsLength, &rc, sizeof(rc), &read);
	if (!result) return result;
	if (read < sizeof(rc)) return DFUEngine::fail_btw_status_short;
	if (sizeof(rc) < read) return DFUEngine::fail_btw_status_long;

	// Return the status
	return MapReturnCode(rc);
}

DFUEngine::Result DFUTransportUSBBTW::IoctlStartFileDownload(uint32 fileLength, uint32 blockLength)
{
	// Prepare the parameter block
	struct
	{
		uint32 fileLength;
		uint32 blockLength;
	} params;
	params.fileLength = fileLength;
	params.blockLength = blockLength;

	// Perform the request
	uint32 rc;
	uint32 read;
	DFUEngine::Result result = device.IOControl(ioctl_start_file_download, &params, sizeof(params), &rc, sizeof(rc), &read);
	if (!result) return result;
	if (read < sizeof(rc)) return DFUEngine::fail_btw_status_short;
	if (sizeof(rc) < read) return DFUEngine::fail_btw_status_long;

	// Return the status
	return MapReturnCode(rc);
}

DFUEngine::Result DFUTransportUSBBTW::IoctlGetDriverStatus(Status &status)
{
	// Perform the request
	uint32 read;
	DFUEngine::Result result = device.IOControl(ioctl_get_driver_status, 0, 0, &status, sizeof(status), &read);
	if (!result) return result;
	if (read < sizeof(status)) return DFUEngine::fail_btw_status_short;
	if (sizeof(status) < read) return DFUEngine::fail_btw_status_long;

	// Successful if this point reached
	return DFUEngine::success;
}

DFUEngine::Result DFUTransportUSBBTW::IoctlDFUDone()
{
	// Perform the request
	uint32 status;
	uint32 read;
	DFUEngine::Result result = device.IOControl(ioctl_dfu_done, 0, 0, &status, sizeof(status), &read);
	if (!result) return result;
	if (read < sizeof(status)) return DFUEngine::fail_btw_status_short;
	if (sizeof(status) < read) return DFUEngine::fail_btw_status_long;

	// Return the status
	return status ? DFUEngine::success : DFUEngine::fail_btw_error;
}

DFUEngine::Result DFUTransportUSBBTW::IoctlGoDFU()
{
	// Perform the request
	uint32 mode;
	uint32 read;
	DFUEngine::Result result = device.IOControl(ioctl_go_dfu, 0, 0, &mode, sizeof(mode), &read);
	if (!result) return result;
	if (read < sizeof(mode)) return DFUEngine::fail_btw_status_short;
	if (sizeof(mode) < read) return DFUEngine::fail_btw_status_long;

	// Check the mode
	if ((mode != mode_dfu_stalled) && (mode != mode_reloading))
	{
		return DFUEngine::fail_btw_start_dfu;
	}

	// Successful if this point reached
	return DFUEngine::success;
}

// Conversion of device driver return codes to DFUEngine result codes
DFUEngine::Result DFUTransportUSBBTW::MapReturnCode(uint32 rc)
{
	// All unrecognised codes map to the unknown DFU error code
	DFUEngine::Result result(DFUEngine::fail_dfu_unknown);

	// Attempt to find the appropriate result code
	for (int index = 0;
		 index < (sizeof(returnCodeMap) / sizeof(returnCodeMapEntry));
		 ++index)
	{
		if (returnCodeMap[index].rc == rc)
		{
			result = returnCodeMap[index].result;
			break;
		}
	}

	// Return the result
	return result;
}

// Conversion of device driver status codes to standard DFU codes
uint32 DFUTransportUSBBTW::MapStatus(uint32 status)
{
	// All unrecognised codes map to the unknown DFU error code
	uint32 statusDFU = DFURequests::err_unknown;

	// Attempt to find the appropriate result code
	for (int index = 0;
		 index < (sizeof(statusCodeMap) / sizeof(statusCodeMapEntry));
		 ++index)
	{
		if (statusCodeMap[index].status == status)
		{
			statusDFU = statusCodeMap[index].statusDFU;
			break;
		}
	}

	// Return the result
	return statusDFU;
}
