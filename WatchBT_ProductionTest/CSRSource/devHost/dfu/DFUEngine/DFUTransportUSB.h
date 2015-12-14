///////////////////////////////////////////////////////////////////////
//
//	File	: DFUTransportUSB.h
//		  Copyright (C) 2001-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFUTransportUSB
//
//	Purpose	: Interface to the raw USB transports. This allows
//			  different USB device drivers to be accessed via
//			  common methods.
//
//			  The following member functions are used to open or
//			  close a specific device:
//				Open				- Open named device.
//				Close				- Close the current device.
//
//			  A pair of member functions are provided to convert
//			  symbolic paths to and from a simpler alias suitable
//			  for display to the user:
//				PathExternal		- Convert a path to a user
//									  friendly form.
//				PathInternal		- Convert a user friendly path
//									  back to a usable form.
//
//			  The remaining methods are similar to the corresponding
//			  RPC method from DFURequests, so see that file for
//			  descriptions.
//
//	Modification history:
//	
//		1.1		05:Oct:01	at	File created.
//		1.2		25:Jul:02	at	Added PathPort method.
//		1.3		25:Jul:02	at	Made PathPort a static method.
//		1.4		25:Jul:02	at	Added conversions to friendly alias.
//		1.5		25:Jul:02	at	Resource file used for friendly alias.
//		1.6		30:Jul:02	at	Removed PathPort method.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUTransportUSB.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef DFUTRANSPORTUSB_H
#define DFUTRANSPORTUSB_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "DFURequests.h"

// DFUTransportUSB class
class DFUTransportUSB
{
public:
	static void init();
	static void deinit();

	// Constructor
	DFUTransportUSB();

	// Destructor
	virtual ~DFUTransportUSB();
	
	// Opening and closing a device
	virtual DFUEngine::Result Open(const CStringX device) = 0;
	virtual DFUEngine::Result Close() = 0;

	// Pre-download preparation step
	virtual DFUEngine::Result PreDnload(DFUFile &file);

	// Non USB DFU control requests
	virtual DFUEngine::Result Reset() = 0;
	virtual DFUEngine::Result GetInterfaceDFU(DFURequests::InterfaceDescriptor &descriptor) = 0;
	virtual DFUEngine::Result GetDevice(DFURequests::DeviceDescriptor &descriptor) = 0;
	virtual DFUEngine::Result GetFunct(DFURequests::DFUFunctionalDescriptor &descriptor) = 0;
	virtual DFUEngine::Result GetString(uint8 index, CStringX &descriptor) = 0;

	// Generic control requests
	virtual DFUEngine::Result ControlRequest(const DFURequests::Setup &setup,
	                                         void *buffer = 0,
							                 uint16 bufferLength = 0,
	                                         uint16 *replyLength = 0) = 0;

	// Convert a symbolic path between internal and external forms
	static CStringX PathExternal(const TCHAR *device);
	static CStringX PathInternal(const TCHAR *device);

	virtual DFUEngine::Result EnableHidMode(bool enable = true);
};

#endif
