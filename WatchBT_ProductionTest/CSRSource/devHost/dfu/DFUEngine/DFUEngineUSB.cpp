///////////////////////////////////////////////////////////////////////
//
//	File	: DFUEngineUSB.cpp
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
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUEngineUSB.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUEngineUSB.h"
#include "DFURequestsUSB.h"
#include "DFUTransportCOM.h"
#include "DFUTransportUSBCSR.h"
#include "DFUTransportUSBBTW.h"
#include "Device.h"
#include "resource.h"

// Visual C++ debugging code
#ifdef _DEBUG
#include <crtdbg.h>
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define new DEBUG_NEW
#endif

// Version string
const char *dfuengineusb_id = "$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUEngineUSB.cpp#1 $ Copyright (C) 2000-2006 Cambridge Silicon Radio Limited";

// Constructor
DFUEngineUSB::DFUEngineUSB(DFUEngine *engine, const TCHAR *device)
: DFUEngineBase(engine)
{
	// Create a USB transport
	transport = SmartPtr<DFURequests>(new DFURequestsUSB(device));
}

// Destructor
DFUEngineUSB::~DFUEngineUSB()
{
}

// USB device enumeration
int DFUEngineUSB::GetUSBDeviceNames(CStringListX &devices)
{
	// Initially no USB devices
	devices.RemoveAll();

	// Enumerate all USB devices
	CStringListX found;
	DFURequestsUSB::EnumerateDevices(found);
	for (CStringListX::POSITION pos = found.GetHeadPosition(); pos != 0;)
	{
		CStringX device(found.GetNext(pos));
		DFUTransportCOM transport;
		CStringX name;
		DFUTransportCOM::BD_ADDR bdAddr;
		if (DFURequestsUSB::ConnectDevice(device, transport)
			&& transport.HCIReadBDADDR(bdAddr))
		{
			name.Format(GetStringTableEntry(IDS_USB_NAME_ADDR), (const TCHAR *) device,
			            bdAddr[5], bdAddr[4], bdAddr[3],
						bdAddr[2], bdAddr[1], bdAddr[0]);
		}
		else
		{
			name.Format(GetStringTableEntry(IDS_USB_NAME), (const TCHAR *) device);
		}
		devices.AddTail(name);
	}

	// Return the number of available USB devices
	return devices.GetCount();
}
