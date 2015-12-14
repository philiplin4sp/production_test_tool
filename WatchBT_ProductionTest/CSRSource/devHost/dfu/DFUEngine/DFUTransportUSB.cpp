///////////////////////////////////////////////////////////////////////
//
//	File	: DFUTransportUSB.cpp
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
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUTransportUSB.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUTransportUSB.h"
#include "Device.h"
#include "resource.h"
#include "thread/critical_section.h"
#include "thread/atomic_counter.h"
#include "CMapX.h"

// Visual C++ debugging code
#ifdef _DEBUG
#include <crtdbg.h>
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define new DEBUG_NEW
#endif

// Version string
const char *dfutransportusb_id = "$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUTransportUSB.cpp#1 $ Copyright (C) 2000-2006 Cambridge Silicon Radio Limited";

// Separators between sections of a port specification
static const TCHAR dfutransportusbSeparator = '&';

// Map of friendly names to full symbolic paths
static CMapX<CStringX, const TCHAR *, CStringX, const TCHAR *> *map = NULL;
static CriticalSection *lockMap = NULL;

static AtomicCounter init_counter(0);

void DFUTransportUSB::init()
{
	if(init_counter.inc() == 1)
	{
		map = new CMapX<CStringX, const TCHAR *, CStringX, const TCHAR *>;
		lockMap = new CriticalSection;
	}
}

void DFUTransportUSB::deinit()
{
	if(init_counter.dec() == 0)
	{
		delete map;
		map = NULL;
		delete lockMap;
		lockMap = NULL;
	}
}

// Constructor
DFUTransportUSB::DFUTransportUSB()
{
}

// Destructor
DFUTransportUSB::~DFUTransportUSB()
{
}

// Pre-download preparation step
DFUEngine::Result DFUTransportUSB::PreDnload(DFUFile &file)
{
	// This function is not normally required
	return DFUEngine::success;
}

// Convert a symbolic path between internal and external forms
CStringX DFUTransportUSB::PathExternal(const TCHAR *device)
{
	// Start by canonicalising the path
	CStringX canonical = Device::Canonicalise(device);

	// Attempt to extract the hardware port specification
	CStringX port = Device::Port(canonical);
	if (port.IsEmpty()) return canonical;

	// Find the separators in the port specification
	int first = port.Find(dfutransportusbSeparator);
	int second = port.Find(dfutransportusbSeparator, first + 1);
	int third = port.Find(dfutransportusbSeparator, second + 1);

	// Protect the path map
	CriticalSection::Lock lock(lockMap);

	// Prefix for the friendly alias
	CStringX prefix;
	prefix = GetStringTableEntry(IDS_USB_ALIAS_PREFIX);

	// Attempt to construct a friendlier version of the port specification
	if ((0 <= first) && (0 <= second) && (0 <= third))
	{
		// Construct a simple name
		CStringX simple;
		simple.Format(GetStringTableEntry(IDS_USB_ALIAS_SIMPLE),
		             (const TCHAR*)port.Left(first),
                     (const TCHAR*)port.Mid(third + 1));

		// Check whether the name is unique
		CStringX existing;
		if (!map->Lookup(prefix + simple, existing)
			|| (Device::Port(existing) == port))
		{
			// Use the simplified port description
			port = simple;
		}
	}

	// Construct and remember the friendly name mapping
	CStringX friendly = prefix + port;
	map->SetAt(friendly, canonical);

	// Return the friendly name
	return friendly;
}

CStringX DFUTransportUSB::PathInternal(const TCHAR *device)
{
	// Start by canonicalising the path
	CStringX canonical = Device::Canonicalise(device);

	// Prefix for the friendly alias
	CStringX prefix;
	prefix = GetStringTableEntry(IDS_USB_ALIAS_PREFIX);

	// Only friendly names need to be converted
	if (canonical.Left(prefix.GetLength()) != prefix)
	{
		return canonical;
	}

	// Protect the path map
	CriticalSection::Lock lock(lockMap);

	// Reverse the friendly name mapping
	CStringX full;
	if (!map->Lookup(canonical, full))
		full = canonical;

	// Return the full symbolic path
	return full;
}

// Enable or disable HID mode.
DFUEngine::Result DFUTransportUSB::EnableHidMode(bool enable)
{
	return DFUEngine::fail_unimplemented;
}