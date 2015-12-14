///////////////////////////////////////////////////////////////////////
//
//	File	: DFUEngineUSB.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFUEngineUSB
//
//	Purpose	: Implementation of DFU engine functionality specific to
//			  a USB transport.
//
//			  The constructor should be called with a pointer to the
//			  DFUEngine object that is providing an interface to the
//			  client. This is required to enable the callbacks to
//			  return this pointer to the client. It should also be
//			  supplied with the name of the device to use. This can
//			  either be the raw device name (with or without a leading
//			  "\\.\" prefix), or one of the device descriptions
//			  supplied by GetUSBDeviceNames.
//
//			  The following member function can be used to obtain
//			  a list of the available device:
//				GetUSBDeviceNames	- Set the supplied list to a list
//									  of strings describing the
//									  available devices.
//
//			  See the definition of DFUInterface for the operations
//			  supported.
//
//	Modification history:
//	
//		1.1		25:Sep:00	at	File created.
//		1.2		04:Oct:00	at	Separated transport into separate class
//								and improved constructor interface.
//		1.3		12:Oct:00	at	Smart pointer constructor used for
//								polymorphic pointer assignment, and
//								COM transport used for HCI commands.
//		1.4		24:Oct:00	at	Removed abort event parameter from
//								transport constructor.
//		1.5		10:Nov:00	at	Removed fake USB enumeration option
//								and added path prefix to device name
//								when reading Bluetooth address.
//		1.6		10:Nov:00	at	Corrected enumeration of USB devices
//								for which the Bluetooth address can
//								be read.
//		1.7		10:Nov:00	at	Really removed fake USB enumeration
//								option.
//		1.8		15:Nov:00	at	Corrected ordering of Bluetooth
//								address.
//		1.9		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.10	23:Jul:01	at	Added version string.
//		1.11	05:Oct:01	at	Added support for Widcomm USB driver.
//		1.12	09:Oct:01	at	Transport used to create HCI interface.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUEngineUSB.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef DFUENGINEUSB_H
#define DFUENGINEUSB_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "DFUEngineBase.h"

// DFUEngineUSB class
class DFUEngineUSB : public DFUEngineBase  
{
public:

	// Constructor
	DFUEngineUSB(DFUEngine *engine, const TCHAR *device);

	// Destructor
	virtual ~DFUEngineUSB();

	// USB device enumeration
	static int GetUSBDeviceNames(CStringListX &devices);
};

#endif
