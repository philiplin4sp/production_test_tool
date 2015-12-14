///////////////////////////////////////////////////////////////////////
//
//	File	: DFURequestsUSB.h
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
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFURequestsUSB.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef DFUREQUESTSUSB_H
#define DFUREQUESTSUSB_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "DFURequests.h"
#include "DFUTransportCOM.h"
#include "DFUTransportUSB.h"
#include "../SmartPtr.h"

// DFURequestsUSB class
class DFURequestsUSB : public DFURequests  
{
public:

	// Constructor
	DFURequestsUSB(const TCHAR *device);

	// Destructor
	virtual ~DFURequestsUSB();

	// Device enumeration
	static int EnumerateDevices(CStringListX &devices);

	// HCI transport instantiation
	static DFUEngine::Result ConnectDevice(const TCHAR *device,
	                                       DFUTransportCOM &transport);

protected:

	// The DFU transport implementation
	SmartPtr<DFUTransportUSB> transport;

	// The USB device
	CStringX name;

	// List of devices before disconnection
	CStringListX listDevices;
	bool listDevicesUpdated;

	// Has a connection attempt failed
	bool connectionFailed;

	// Pre-download preparation step
	virtual DFUEngine::Result PreDnload(DFUFile &file);
	
	// Connection and disconnection
	virtual DFUEngine::Result RPCConnect(bool hintDFU);
	virtual DFUEngine::Result RPCDisconnect();

	// Non USB DFU control requests
	virtual DFUEngine::Result RPCReset();
	virtual DFUEngine::Result RPCGetInterfaceDFU(InterfaceDescriptor &descriptor);
	virtual DFUEngine::Result RPCGetDevice(DeviceDescriptor &descriptor);
	virtual DFUEngine::Result RPCGetFunct(DFUFunctionalDescriptor &descriptor);
	virtual DFUEngine::Result RPCGetString(uint8 index, CStringX &descriptor);

	// Generic control requests
	virtual DFUEngine::Result ControlRequest(const Setup &setup,
	                                         void *buffer = 0,
							                 uint16 bufferLength = 0,
	                                         uint16 *replyLength = 0);

	// Individual USB DFU control requests (modifying default behaviour)
	virtual DFUEngine::Result RPCDetach(uint16 timeout);

	// Hid operations
	virtual DFUEngine::Result RPCEnableHidMode(bool enable);

private:

	// Attempt to start a transport for a device
	DFUEngine::Result Open(const TCHAR *device);

	// Enumerate devices without conversion to friendly aliases
	static int EnumerateDevicesRaw(CStringListX &devices, const TCHAR *port);
};

#endif
