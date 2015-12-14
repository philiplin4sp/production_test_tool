///////////////////////////////////////////////////////////////////////
//
//	File	: Device.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: Device
//
//	Purpose	: Manage device handles in a similar manner to CFile. If
//			  the device is open when this object is destroyed then
//			  it is automatically closed.
//
//			  The device can optionally be opened by supplying the
//			  name to the constructor. However, this does not allow
//			  error information to be returned, so it is recommended
//			  that the following pair of member functions are used:
//				Open			- Open the named device.
//				Close			- Close the device (if open).
//
//			  Supplied device names are automatically canonicalised
//			  using the following member function:
//				Canonicalise	- Remove any path prefix and trailing
//								  comments (starting from the first
//								  space character), and convert to
//								  upper case.
//
//			  Some device names include a physical port specification
//			  that can be extracted using the following member
//			  function:
//				Port			- Extract the port specification from
//								  suitable paths.
//
//			  The standard "\\.\" prefix is added automatically when
//			  the device is opened.
//
//			  Device IO control functions can be performed with the
//			  following member function:
//				IOControl		- Perform a device IO control
//								  function. An optional timeout value
//								  may be specified.
//
//			  Static member functions allow devices to be
//			  enumerated:
//				Enumerate		- Construct a list of device names
//								  based on either a common format or
//								  an interface GUID. In the former
//								  case this attempts to open all of
//								  the devices in the specified range,
//								  replacing the first occurrence of %d
//								  in the format string, so devices that
//								  are already open will not be listed.
//				Sort			- Sort a list of device names that
//								  may be based on a common format.
//				IsA				- Check if a specified device name
//								  matches an enumeration format.
//
//	Modification history:
//	
//		1.1		02:Oct:00	at	File created.
//		1.2		10:Nov:00	at	Added option to include path prefix in
//								canonicalised name.
//		1.3		10:Nov:00	at	Devices now opened for overlapped I/O.
//		1.4		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.5		30:Nov:00	at	Mapping of CRC errors to stall result
//								codes moved from DFURequestsUSB to
//								avoid problems with the error code
//								being overwritten under Windows 95/98.
//		1.6		30:Apr:01	ckl	Added Windows CE support.
//		1.7		23:May:01	at	Tidied Windows CE support.
//		1.8		23:May:01	at	Corrected Windows CE support.
//		1.9		23:Jul:01	at	Added version string.
//		1.10	05:Oct:01	at	Added the IsA method.
//		1.11	06:Oct:01	at	Corrected the IsA method.
//		1.12	08:Oct:01	at	Added non-overlapped I/O as an option.
//		1.13	10:Oct:01	at	Added support for derived classes.
//		1.14	11:Oct:01	at	External lock cleared if open fails.
//		1.15	14:Nov:01	at	Added basic Unicode support.
//		1.16	27:Nov:01	at	Corrected lifetime of temporary name.
//		1.17	08:Mar:02	at	Added Sort function.
//		1.18	13:Mar:02	at	Corrected sorting of device names.
//		1.19	24:Jul:02	at	Added enumeration by interface GUID.
//		1.20	25:Jul:02	at	SetupDi functions not used on WindowsCE.
//		1.21	25:Jul:02	at	SetupDi class used for explicit linking.
//		1.22	30:Jul:02	at	Added extraction of port specification.
//		1.23	06:Dec:02	at	Remove unused variable on Windows CE.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/Device.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef DEVICE_H
#define DEVICE_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "DFUEngine.h"
#include "types.h"
#include <windows.h>

// Device class
class Device  
{
public:

	// Constructors
	Device();
	Device(const TCHAR *device);

	// Destructor
	virtual ~Device();

	// Opening and closing devices
	DFUEngine::Result Open(const TCHAR *device);
	DFUEngine::Result Close();

	// Accessors
	operator HANDLE() const;
	operator bool() const;

	// Device IO control
	DFUEngine::Result IOControl(uint32 code,
	                            void *in = 0, uint32 inLength = 0,
								void *out = 0, uint32 outLength = 0,
								uint32 *read = 0, uint32 timeout = INFINITE);

	// Extraction of device name
	static CStringX Canonicalise(const TCHAR *device, bool prefix = false);
	static CStringX Port(const TCHAR *device);

	// Enumeration of devices
	static int Enumerate(const TCHAR *format, int first, int last,
	                     CStringListX &list);
	static int Enumerate(const GUID &guid, const TCHAR *port,
	                     CStringListX &list);
	static int Sort(const TCHAR *format, int first, int last,
	                CStringListX &list);
	static bool IsA(const TCHAR *device, const TCHAR *format,
	                int first, int last);
	static bool IsA(const TCHAR *device, const GUID &guid);

protected:

	// The device handle
	HANDLE handle;

	// Should overlapped I/O be used
	virtual bool UseOverlappedIO() const;

	// Support for external device locking
	virtual DFUEngine::Result PreOpen(const TCHAR *device);
	virtual DFUEngine::Result PostClose();

	// Generation of possible device names
	static int GenerateNames(const TCHAR *format, int first, int last,
	                         CStringListX &list);

	// Filter out device names that cannot be opened
	int FilterNames(CStringListX &list);

	// Convert a GUID to a string
	static CStringX GUIDToString(const GUID &guid);

private:

	// Prevent copying
	Device(const Device &);
	Device operator=(const Device &);
};

#endif
