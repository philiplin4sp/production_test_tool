///////////////////////////////////////////////////////////////////////
//
//	File	: DeviceBTW.h
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
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DeviceBTW.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef DEVICEBTW_H
#define DEVICEBTW_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "Device.h"
#include "../SmartPtr.h"
#include "thread/system_wide_mutex.h"

// Device class
class DeviceBTW : public Device
{
public:

	// Constructors
	DeviceBTW();
	DeviceBTW(const TCHAR *device);

	// Destructor
	virtual ~DeviceBTW();

	// Enumeration of devices
	static int Enumerate(const TCHAR *format, int first, int last,
	                     CStringListX &list);

protected:

	// Should overlapped I/O be used
	virtual bool UseOverlappedIO() const;

	// Support for external device locking
	virtual DFUEngine::Result PreOpen(const TCHAR *device);
	virtual DFUEngine::Result PostClose();

private:

	// Mutex to claim exclusive access to the device
	SmartPtr<SystemWideMutex> mutex;
};

#endif
