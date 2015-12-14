///////////////////////////////////////////////////////////////////////
//
//	File	: DeviceBTW.cpp
//		  Copyright (C) 2001-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DeviceBTW
//
//	Purpose	: Widcomm USB device driver manager. This implements
//			  driver locking using a named mutex and disables use of
//			  overlapped I/O.
//
//			  Most of the methods are the same as those provided by
//			  Device, so see that file for descriptions.
//
//	Modification history:
//	
//		1.1		10:Oct:01	at	File created.
//		1.2		11:Oct:01	jbs	Changed name of mutex class.
//		1.3		12:Oct:01	at	Updated revision history.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DeviceBTW.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DeviceBTW.h"

// Visual C++ debugging code
#ifdef _DEBUG
#include <crtdbg.h>
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define new DEBUG_NEW
#endif

// Version string
const char *devicebtw_id = "$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DeviceBTW.cpp#1 $ Copyright (C) 2001-2006 Cambridge Silicon Radio Limited";

// Constructors
DeviceBTW::DeviceBTW()
{
}

DeviceBTW::DeviceBTW(const TCHAR *device)
{
	// The base class uses virtual methods to open the device
	Open(device);
}

// Destructor
DeviceBTW::~DeviceBTW()
{
	// The mutex may be destroyed before the base class
	Close();
}

// Enumeration of devices
int DeviceBTW::Enumerate(const TCHAR *format, int first, int last,
	                     CStringListX &list)
{
	// Generate all the possible device names
	GenerateNames(format, first, last, list);

	// Filter out the names that cannot be opened
	return DeviceBTW().FilterNames(list);
}

// Opening and closing devices
DFUEngine::Result DeviceBTW::PreOpen(const TCHAR *device)
{
	// Attempt to acquire the mutex
	mutex = SmartPtr<SystemWideMutex>(new SystemWideMutex(Canonicalise(device), true));
	if (!mutex->IsAcquired())
	{
		return DFUEngine::Result(DFUEngine::fail_usb_open, device);
	}

	// Successful if this point reached
	return DFUEngine::success;
}

DFUEngine::Result DeviceBTW::PostClose()
{
	// Release the mutex
	mutex = SmartPtr<SystemWideMutex>();

	// Successful if this point reached
	return DFUEngine::success;
}

// Should overlapped I/O be used
bool DeviceBTW::UseOverlappedIO() const
{
	// The Widcomm USB device driver does not support overlapped I/O
	return false;
}