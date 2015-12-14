///////////////////////////////////////////////////////////////////////
//
//	File	: DFURequestsUSB.cpp
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFURequestsUSB
//
//	Purpose	: Implementation of basic DFU operations specific to
//			  a USB transport.
//
//			  Most of the methods are implementations of virtual
//			  functions provided by DFURequests, so see that file for
//			  descriptions.
//
//			  The constructor should be called with the name of the
//			  device to use. This can either be the raw device name
//			  (with or without a leading "\\.\" prefix), or one of the
//			  device descriptions supplied by EnumerateDevices.
//
//			  A list of suitable USB devices may be obtained using
//			  the following member function:
//				EnumerateDevices	- Set the supplied list to a list
//									  of strings describing the
//									  available devices.
//
//			  A standard HCI transport can be created for a named
//			  device using the following member function:
//				ConnectDevice		- Create a transport to the named
//									  device.
//
//	Modification history:
//	
//		1.1		02:Oct:00	at	File created.
//		1.2		12:Oct:00	at	Removed preliminary HCI support that is
//								now provided by DFUTransportCOM.
//								Removed GetUSBDeviceNames member
//								function that was inadverdently copied.
//		1.3		12:Oct:00	at	Added provisional USB transport support
//								based on the expected device driver
//								interface.
//		1.4		24:Oct:00	at	Updated based on changes to base class.
//		1.5		10:Nov:00	at	Corrected USB device IO control codes,
//								and accepted shorter device descriptor.
//		1.6		14:Nov:00	at	Tidied up comments for the addition of
//								control transfers.
//		1.7		14:Nov:00	at	Completed USB support.
//		1.8		15:Nov:00	at	Corrected comments.
//		1.9		15:Nov:00	at	Changed control transfer method from
//								Buffered to Out Direct, and removed
//								bodge for shorter device descriptor.
//		1.10	17:Nov:00	at	Multiple retry attempts to find and
//								open device driver to allow time for
//								different driver to load after reset.
//		1.11	17:Nov:00	at	Mapped stall responses to correct
//								result code.
//		1.12	21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.13	30:Nov:00	at	Moved mapping of CRC errors to stall
//								responses to Device to avoid the error
//								code being overwritten under
//								Windows 95/98. Improved reset and
//								reconnect behaviour.
//		1.14	06:Mar:01	at	Added support for activity description.
//		1.15	15:Mar:01	at	Unsuccessful connection disables
//								subsequent attempts.
//		1.16	11:Apr:01	at	Delay increased between successive
//								connection attempts.
//		1.17	23:Jul:01	at	Added version string.
//		1.18	05:Oct:01	at	Separated out device driver specifics.
//		1.19	09:Oct:01	at	Added ConnectDevice method.
//		1.20	25:Jul:02	at	Attempt to restrict connection to
//								the same physical port.
//		1.21	25:Jul:02	at	Symbolic paths converted to friendly
//								aliases for external presentation.
//		1.22	26:Jul:02	at	Retry opening device when connecting.
//		1.23	30:Jul:02	at	Filter on port during enumeration.
//		1.24	05:Nov:02	at	Shorter connection attempt if no abort.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFURequestsUSB.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFURequestsUSB.h"
#include "DFUTransportUSBCSR.h"
#include "DFUTransportUSBBTW.h"

// Visual C++ debugging code
#ifdef _DEBUG
#include <crtdbg.h>
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define new DEBUG_NEW
#endif

// Version string
const char *dfurequestsusb_id = "$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFURequestsUSB.cpp#1 $ Copyright (C) 2000-2006 Cambridge Silicon Radio Limited";

// Delays to allow operations to complete
static const uint32 delayPreConnect = 1500;
static const uint32 delayConnectMin = 250;
static const uint32 delayConnectMax = 10000;
static const uint32 delayConnectFactor = 2;
static const uint32 timeoutConnect = 60000;
static const uint32 timeoutConnectAbort = 5000;

// Retry behaviour when opening a device
static const uint32 delayRetry = 25;
static const uint32 maxRetry = 4;

// Constructor
DFURequestsUSB::DFURequestsUSB(const TCHAR *device)
: name(DFUTransportUSB::PathInternal(device))
{
	listDevicesUpdated = false;
	connectionFailed = false;
}

// Destructor
DFURequestsUSB::~DFURequestsUSB()
{
}

// Non USB DFU control requests
DFUEngine::Result DFURequestsUSB::RPCReset()
{
	// Ensure that there is an active transport
	if (!transport) return DFUEngine::fail_no_transport;

	// Enumerate other devices before reset if not already done
	if (!listDevicesUpdated)
	{
		EnumerateDevicesRaw(listDevices, name);
		listDevicesUpdated = true;
	}

	// Pass on to the transport
	return transport->Reset();
}

DFUEngine::Result DFURequestsUSB::RPCGetInterfaceDFU(InterfaceDescriptor &descriptor)
{
	// Ensure that there is an active transport
	if (!transport) return DFUEngine::fail_no_transport;

	// Pass on to the transport
	return transport->GetInterfaceDFU(descriptor);
}

DFUEngine::Result DFURequestsUSB::RPCGetDevice(DeviceDescriptor &descriptor)
{
	// Ensure that there is an active transport
	if (!transport) return DFUEngine::fail_no_transport;

	// Pass on to the transport
	return transport->GetDevice(descriptor);
}

DFUEngine::Result DFURequestsUSB::RPCGetFunct(DFUFunctionalDescriptor &descriptor)
{
	// Ensure that there is an active transport
	if (!transport) return DFUEngine::fail_no_transport;

	// Pass on to the transport
	return transport->GetFunct(descriptor);
}

DFUEngine::Result DFURequestsUSB::RPCGetString(uint8 index, CStringX &descriptor)
{
	// Ensure that there is an active transport
	if (!transport) return DFUEngine::fail_no_transport;

	// Pass on to the transport
	return transport->GetString(index, descriptor);
}

// Pre-download preparation step
DFUEngine::Result DFURequestsUSB::PreDnload(DFUFile &file)
{
	// Ensure that there is an active transport
	if (!transport) return DFUEngine::fail_no_transport;

	// Pass on to the transport
	return transport->PreDnload(file);
}
	
// Connection and disconnection
DFUEngine::Result DFURequestsUSB::RPCConnect(bool hintDFU)
{
	DFUEngine::Result result;

	// Fail immediately if failed previously
	if (connectionFailed) return DFUEngine::Result(DFUEngine::fail_usb_open, name);

	// Delay before attempting to connect if after a reset
	Progress(DFUEngine::transport_usb_wait);
	if (listDevicesUpdated)
	{
		listDevicesUpdated = false;
		result = CheckAbort(delayPreConnect);
		if (!result) return result;
	}

	// First try to connect using the original name
	Progress(DFUEngine::transport_usb_open, name);
	result = Open(name);

	// Retry at intervals if failed
	uint32 end = GetTickCount()
				 + (eventAbort ? timeoutConnect : timeoutConnectAbort);
	uint32 delayConnect = delayConnectMin;
	while (!result && (int32(GetTickCount() - end) < 0))
	{
		// Allow time for the device driver to load
		Progress(DFUEngine::transport_usb_wait);
		result = CheckAbort(delayConnect);
		if (!result) return result;

		// Increase the delay for the next attempt
		delayConnect = min(delayConnect * delayConnectFactor, delayConnectMax);

		// Attempt to identify the new device name
		Progress(DFUEngine::transport_usb_check);
		CStringListX newDevices;
		EnumerateDevicesRaw(newDevices, name);
		for (CStringListX::POSITION pos = listDevices.GetHeadPosition(); pos != 0;)
		{
			CStringListX::POSITION found = newDevices.Find(listDevices.GetNext(pos));
			if (found) newDevices.RemoveAt(found);
		}

		// Must have a unique device name
		if (newDevices.IsEmpty())
		{
			result = DFUEngine::Result(DFUEngine::fail_usb_open, name);
		}
		else if (1 < newDevices.GetCount())
		{
			result = DFUEngine::Result(DFUEngine::fail_usb_id, name);
		}
		else
		{
			// Attempt to connect to the identified device
			CStringX newName(newDevices.GetHead());
			Progress(DFUEngine::transport_usb_open, newName);
			result = Open(newName);
			if (result) name = newName;
			else if (newName == name)
			{
				result = DFUEngine::Result(DFUEngine::fail_usb_open, name);
			}
		}
	}

	// Return the last error if failed after retries
	if (!result)
	{
		connectionFailed = true;
		return result;
	}

	// Successful if this point reached
	Progress(DFUEngine::transport_established);
	return DFUEngine::success;
}

DFUEngine::Result DFURequestsUSB::RPCDisconnect()
{
	// No action unless there is an active transport
	if (transport)
	{
		// Close the device
		DFUEngine::Result result = transport->Close();
		if (!result) return result;

		// Delete the transport
		transport = SmartPtr<DFUTransportUSB>();
	}

	// Successful if this point reached
	return DFUEngine::success;
}

// Generic control requests
DFUEngine::Result DFURequestsUSB::ControlRequest(const Setup &setup,
                                                 void *buffer,
                                                 uint16 bufferLength,
                                                 uint16 *replyLength)
{
	// Ensure that there is an active transport
	if (!transport) return DFUEngine::fail_no_transport;

	// Pass on to the transport
	return transport->ControlRequest(setup, buffer, bufferLength, replyLength);
}

// Individual USB DFU control requests (modifying default behaviour)
DFUEngine::Result DFURequestsUSB::RPCDetach(uint16 timeout)
{
	// Enumerate other devices before detach to avoid timeout problems
	EnumerateDevicesRaw(listDevices, name);
	listDevicesUpdated = true;

	// Use default implementation
	return DFURequests::RPCDetach(timeout);
}

// Device enumeration
int DFURequestsUSB::EnumerateDevices(CStringListX &devices)
{
	// Enumerate all device drivers
	CStringListX rawDevices;
	EnumerateDevicesRaw(rawDevices, _T(""));

	// Convert the paths to friendly form
		devices.RemoveAll();
		for (CStringListX::POSITION pos = rawDevices.GetHeadPosition(); pos != 0;)
		{
			devices.AddTail(DFUTransportUSB::PathExternal(rawDevices.GetNext(pos)));
	}

	// Return the number of available USB devices
	return devices.GetCount();
}

// HCI transport instantiation
DFUEngine::Result DFURequestsUSB::ConnectDevice(const TCHAR *device,
                                                DFUTransportCOM &transport)
{
	DFUEngine::Result result;
	CStringX name(DFUTransportUSB::PathInternal(device));
	CStringX canonical(Device::Canonicalise(name, true));

	// HERE - should probably try to open the device a few times...

	// Use the appropriate transport
	if (DFUTransportUSBCSR::IsA(name))
	{
		result = transport.ConnectUSBCSR(canonical);
	}
	else if (DFUTransportUSBBTW::IsA(name))
	{
		result = transport.ConnectUSBBTW(canonical);
	}
	else
	{
		result = DFUEngine::fail_usb_none;
	}

	// Return the result
	return result;
}

// Attempt to start a transport for a device
DFUEngine::Result DFURequestsUSB::Open(const TCHAR *device)
{
	SmartPtr<DFUTransportUSB> transport;

	// Instantiate the appropriate transport
	if (DFUTransportUSBCSR::IsA(device))
	{
		transport = SmartPtr<DFUTransportUSB>(new DFUTransportUSBCSR);
	}
	else if (DFUTransportUSBBTW::IsA(device))
	{
		transport = SmartPtr<DFUTransportUSB>(new DFUTransportUSBBTW);
	}
	else return DFUEngine::Result(DFUEngine::fail_usb_open, device);

	// Attempt to open the device, with a few retries
	int retries = maxRetry;
	DFUEngine::Result result = transport->Open(device);
	while (retries-- && !result)
	{
		Sleep(delayRetry);
		result = transport->Open(device);
	}
	if (!result) return result;

	// Store the pointer to the transport
	this->transport = transport;

	// Successful if this point reached
	return DFUEngine::success;
}

// Enumerate devices without conversion to friendly aliases
int DFURequestsUSB::EnumerateDevicesRaw(CStringListX &devices, const TCHAR *port)
{
	// Enumerate CSR device drivers
	DFUTransportUSBCSR::EnumerateDevices(devices, port);

	// Enumerate Widcomm device drivers also
	CStringListX devicesBTW;
	DFUTransportUSBBTW::EnumerateDevices(devicesBTW);
	devices.AddTail(&devicesBTW);

	// Return the number of available USB devices
	return devices.GetCount();
}


// Hid operations
DFUEngine::Result DFURequestsUSB::RPCEnableHidMode(bool enable)
{
	// Ensure that there is an active transport
	if (!transport) 
		return DFUEngine::fail_no_transport;

	// Pass on to the transport
	return transport->EnableHidMode(enable);
}