///////////////////////////////////////////////////////////////////////
//
//	File	: DFUTransportUSBCSR.h
//		  Copyright (C) 2001-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFUTransportUSBCSR
//
//	Purpose	: Implementation of the raw USB transport for CSR device
//			  drivers.
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
//		1.2		09:Oct:01	at	Added ConnectDevice method.
//		1.3		09:Oct:01	at	Reverted back to trying all device
//								indexes during enumeration.
//		1.4		14:Nov:01	at	Added basic Unicode support.
//		1.5		24:Jul:02	at	Interface GUID used for enumeration.
//		1.6		30:Jul:02	at	Port matching used during enumeration.
//		1.7		04:Nov:02	at	Added workaround for Windows XP SP1.
//		1.8		11:Nov:02	at	No cold reset at end of upgrade.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUTransportUSBCSR.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef DFUTRANSPORTUSBCSR_H
#define DFUTRANSPORTUSBCSR_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "DFUTransportCOM.h"
#include "DFUTransportUSB.h"
#include "Device.h"
#include "../SmartPtr.h"

// DFUTransportUSBCSR class
class DFUTransportUSBCSR : public DFUTransportUSB
{
public:

	// Constructor
	DFUTransportUSBCSR();

	// Destructor
	virtual ~DFUTransportUSBCSR();

	// Device enumeration
	static int EnumerateDevices(CStringListX &devices);
	static int EnumerateDevices(CStringListX &devices, const TCHAR *port);
	static bool IsA(const CStringX device);

	// HCI transport instantiation
	static DFUEngine::Result ConnectDevice(const TCHAR *device,
	                                       DFUTransportCOM &transport,
										   HANDLE handle = INVALID_HANDLE_VALUE);
	
	// Opening and closing a device
	virtual DFUEngine::Result Open(const CStringX device);
	virtual DFUEngine::Result Close();

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

	virtual DFUEngine::Result EnableHidMode(bool enable = true);
protected:

	// The USB device
	CStringX name;
	Device device;
	
	// Read the whole configuration descriptor
	virtual DFUEngine::Result GetConfig(SmartPtr<uint8, true> &descriptor,
	                                    uint32 &descriptorLength);
};

#endif
