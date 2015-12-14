///////////////////////////////////////////////////////////////////////
//
//	File	: DFUTransportUSBBTW.h
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
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUTransportUSBBTW.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef DFUTRANSPORTUSBBTW_H
#define DFUTRANSPORTUSBBTW_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "DFUTransportCOM.h"
#include "DFUTransportUSB.h"
#include "../SmartPtr.h"
#include "DeviceBTW.h"

// DFUTransportUSBBTW class
class DFUTransportUSBBTW : public DFUTransportUSB
{
public:

	// Device driver control request return codes
	enum
	{
		rc_ok = 0,
		rc_error = 1,
		rc_not_supported = 2,
		rc_busy = 3,
		rc_file_error = 4,
		rc_download_error = 5,
		rc_system_error = 6,
		rc_pending = 7,
		rc_vendor_error = 8
	};

	// Status codes (similar to the standard DFU codes)
	enum
	{
		ok = 0x00,
		err_target = 0x01,
		err_file = 0x02,
		err_write = 0x03,
		err_erase = 0x04,
		err_check_erased = 0x05,
		err_prog = 0x06,
		err_verify = 0x07,
		err_check_verify = 0x08,// Memory verify check failed
		err_address = 0x09,
		err_not_done = 0x0a,
		err_firmware = 0x0b,
		err_vendor = 0x0c,
		err_usbr = 0x0d,
		err_por = 0x0e,
		err_unknown = 0x0f,
		err_stalledpkt = 0x10,
		err_stalledreset = 0x11	// Device stalled on USB reset
	};

	// Phase codes
	enum
	{
		phase_enumeration = 0,
		phase_reconfiguration = 1,
		phase_transfer = 2,
		phase_manifest = 3
	};

	// Mode codes
	enum
	{
		mode_neutral = 0,
		mode_file_downloaded = 1,
		mode_dfu_downloaded = 2,
		mode_dfu_complete = 3,
		mode_dfu_stalled = 4,
		mode_reloading = 5
	};

	// Constructor
	DFUTransportUSBBTW();

	// Destructor
	virtual ~DFUTransportUSBBTW();

	// Device enumeration
	static int EnumerateDevices(CStringListX &devices);
	static bool IsA(const CStringX device);

	// HCI transport instantiation
	static DFUEngine::Result ConnectDevice(const TCHAR *device,
	                                       DFUTransportCOM &transport,
										   HANDLE handle = INVALID_HANDLE_VALUE);
	
	// Opening and closing a device
	virtual DFUEngine::Result Open(const CStringX device);
	virtual DFUEngine::Result Close();

	// Pre-download preparation step
	virtual DFUEngine::Result PreDnload(DFUFile &file);

	// Non USB DFU control requests
	virtual DFUEngine::Result Reset();
	virtual DFUEngine::Result GetInterfaceDFU(DFURequests::InterfaceDescriptor &descriptor);
	virtual DFUEngine::Result GetDevice(DFURequests::DeviceDescriptor &descriptor);
	virtual DFUEngine::Result GetFunct(DFURequests::DFUFunctionalDescriptor &descriptor);
	virtual DFUEngine::Result GetString(uint8 index, CStringX &descriptor);

	// Generic control requests
	virtual DFUEngine::Result ControlRequest(const DFURequests::Setup &setup,
	                                         void *buffer = 0,
							                 uint16 bufferLength = 0,
	                                         uint16 *replyLength = 0);

protected:

	// The USB device
	CStringX name;
	DeviceBTW device;
	bool isDFU;
	bool doneDetach;
	bool doneDownload;

	// Device driver status
	struct Status
	{
		uint32 status;			// Similar to bStatus in DFUStatus
		uint32 state;			// Equivalent to bState in DFUStatus
		uint32 phase;			// Stage of the DFU procedure
		uint32 mode;			// Mode of the DFU procedure
		uint32 bytes;
	};

	// File being downloaded
	uint32 fileLength;
	uint32 fileDone;

	// The device descriptor
	DFURequests::DeviceDescriptor deviceDescriptor;

	// The individual device driver control requests
	DFUEngine::Result IoctlStop();
	DFUEngine::Result IoctlWriteFile(const void *buffer, uint32 bufferLength);
	DFUEngine::Result IoctlStartFileDownload(uint32 fileLength, uint32 blockLength);
	DFUEngine::Result IoctlGetDriverStatus(Status &status);
	DFUEngine::Result IoctlDFUDone();
	DFUEngine::Result IoctlGoDFU();

	// Conversion of device driver return codes to DFUEngine result codes
	DFUEngine::Result MapReturnCode(uint32 rc);

	// Conversion of device driver status codes to standard DFU codes
	uint32 MapStatus(uint32 status);
};

#endif
