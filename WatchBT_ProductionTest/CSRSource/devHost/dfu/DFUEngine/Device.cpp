///////////////////////////////////////////////////////////////////////
//
//	File	: Device.cpp
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
//								  function. This assumes non-blocking
//								  behaviour.
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
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/Device.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "Device.h"
#include "../SmartPtr.h"
#if !defined _WINCE && !defined _WIN32_WCE
#include "SetupDi.h"
#endif

// Visual C++ debugging code
#ifdef _DEBUG
#include <crtdbg.h>
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define new DEBUG_NEW
#endif

// Version string
const char *device_id = "$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/Device.cpp#1 $ Copyright (C) 2000-2006 Cambridge Silicon Radio Limited";

// Standard device prefix
static const CStringX devicePrefix = _T("\\\\.\\");

// Path prefix to disable parsing
static const CStringX deviceDisableParsing = _T("\\\\?\\");

// Separators between sections of a symbolic device path
static const TCHAR devicePortSeparator = '#';

// Comment separator
static const TCHAR deviceSeparator = ' ';

// GUID format
static const TCHAR deviceGUID[] = _T("{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}");

// Constructors
Device::Device()
{
	handle = INVALID_HANDLE_VALUE;
}

Device::Device(const TCHAR *device)
{
	handle = INVALID_HANDLE_VALUE;
	Open(device);
}

// Destructor
Device::~Device()
{
	Close();
}

// Opening and closing devices
DFUEngine::Result Device::Open(const TCHAR *device)
{
	// Close any currently open device
	DFUEngine::Result result = Close();
	if (!result) return result;

	// Claim any external lock that may be required
	result = PreOpen(device);
	if (!result) return result;

	// Open the device
	CStringX canonical = Canonicalise(device, true);
	handle = CreateFile(canonical,
		                GENERIC_READ | GENERIC_WRITE, 0, 0,
						OPEN_EXISTING,
						UseOverlappedIO() ? FILE_FLAG_OVERLAPPED : 0, 0);
	if (handle == INVALID_HANDLE_VALUE)
	{
		PostClose();
		return DFUEngine::fail_os;
	}

	// Successful if this point reached
	return DFUEngine::success;
}

DFUEngine::Result Device::Close()
{
	// No action required unless the device is currently open
	if (handle != INVALID_HANDLE_VALUE)
	{
		// Close the device
		if (!CloseHandle(handle))
		{
			return DFUEngine::fail_os;
		}
		handle = INVALID_HANDLE_VALUE;

		// Release any external lock
		DFUEngine::Result result = PostClose();
		if (!result) return result;
	}

	// Successful if this point reached
	return DFUEngine::success;
}

// Accessors
Device::operator HANDLE() const
{
	return handle;
}

Device::operator bool() const
{
	return handle != INVALID_HANDLE_VALUE;
}

// Device IO control
DFUEngine::Result Device::IOControl(uint32 code,
	                                void *in, uint32 inLength,
							        void *out, uint32 outLength,
							        uint32 *read, uint32 timeout)
{
	bool useOverlapped(UseOverlappedIO());

	// Create an overlapped data structure
	OVERLAPPED overlapped;
	overlapped.Offset = 0;
	overlapped.OffsetHigh = 0;
	HANDLE event = CreateEvent(NULL, TRUE, FALSE, NULL);
	if(!event) return DFUEngine::fail_os;
	overlapped.hEvent = event;

	// Attempt to perform the requested operation
	DWORD bytesReturned;
	if (!DeviceIoControl(handle, code, in, inLength,
	                     out, outLength, &bytesReturned,
						 useOverlapped ? &overlapped : NULL)
		&& (GetLastError() != ERROR_IO_PENDING))
	{
		CloseHandle(event);
		return GetLastError() == ERROR_CRC
		       ? DFUEngine::fail_dfu_stalledpkt
			   : DFUEngine::fail_os;
	}

	// Wait for the operation to complete
	if (useOverlapped) WaitForSingleObject(event, INFINITE);
	CloseHandle(event);

	// Get the result
#if !defined _WINCE && !defined _WIN32_WCE
	if (useOverlapped
		&& !GetOverlappedResult(handle, &overlapped, &bytesReturned, false))
	{
		return DFUEngine::fail_os;
	}
#endif
	if (read) *read = bytesReturned;

	// Successful if this point reached
	return DFUEngine::success;
}

// Extraction of device name
CStringX Device::Canonicalise(const TCHAR *device, bool prefix)
{
	CStringX result(device);

#if !defined _WINCE && !defined _WIN32_WCE
	// Remove any standard prefix
	if (result.Left(devicePrefix.GetLength()) == devicePrefix)
	{
		result.Delete(0, devicePrefix.GetLength());
	}

	// Remove any trailing comments
	int pos = result.Find(deviceSeparator);
	if (0 <= pos) result.Delete(pos, result.GetLength() - pos);
	
	// Convert to upper case
	result.MakeUpper();

	// Add a standard prefix if required
	if (prefix && (result.Left(deviceDisableParsing.GetLength()) != deviceDisableParsing))
	{
		result = devicePrefix + result;
	}
#endif

	// Return the result
	return result;
}

CStringX Device::Port(const TCHAR *device)
{
	CStringX device_ = device;
	// Find the first three hash characters in the path
	int first = device_.Find(devicePortSeparator);
	int second = device_.Find(devicePortSeparator, first + 1);
	int third = device_.Find(devicePortSeparator, second + 1);

	// Only interested in paths that match the expected pattern
	if ((device_.Left(deviceDisableParsing.GetLength())
	     != deviceDisableParsing)
		|| (first < 0) || (second < 0) || (third < 0))
		return _T("");

	// Extract the hardware port specification
	CStringX port = device_.Mid(second + 1, third - second - 1);

	// Convert to upper case to ensure reliable comparisons
	port.MakeUpper();

	// Return the hardware port specification
	return port;
}

// Enumeration of devices
int Device::Enumerate(const TCHAR *format, int first, int last,
                      CStringListX &list)
{
	// Generate all the possible device names
	GenerateNames(format, first, last, list);

	// Filter out the names that cannot be opened
	return Device().FilterNames(list);
}

int Device::Enumerate(const GUID &guid, const TCHAR *port,
                      CStringListX &list)
{
	// Attempt to extract a port specification from the path
	CStringX matchPort = Port(port);

	// Start with an empty list of device names
	list.RemoveAll();

#if !defined _WINCE && !defined _WIN32_WCE
	// Retrieve an information set for all matching devices
	GUID interfaceGuid = guid;
	HDEVINFO deviceHardwareInfo;
	deviceHardwareInfo = SetupDi::GetClassDevs(&interfaceGuid, NULL, NULL,
	                                           DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
	if (deviceHardwareInfo != INVALID_HANDLE_VALUE)
	{
		// Enumerate the interfaces for the matching devices
		SP_INTERFACE_DEVICE_DATA deviceInterfaceData;
		deviceInterfaceData.cbSize = sizeof(SP_INTERFACE_DEVICE_DATA);
		int index = 0;
		while (SetupDi::EnumDeviceInterfaces(deviceHardwareInfo, 0,
		                                     &interfaceGuid, index++,
		                                     &deviceInterfaceData))
		{
			// Prepare a buffer for the device path symbolic name
			DWORD requiredSize;
			SetupDi::GetDeviceInterfaceDetail(deviceHardwareInfo,
			                                  &deviceInterfaceData, NULL, 0,
 			                                  &requiredSize, NULL);
			SmartPtr<uint8, true> buffer(new uint8[requiredSize]);
			PSP_INTERFACE_DEVICE_DETAIL_DATA deviceInterfaceDetail = (PSP_INTERFACE_DEVICE_DETAIL_DATA) (uint8 *) buffer;
			deviceInterfaceDetail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);

			// Obtain and store the device path symbolic name
			if (SetupDi::GetDeviceInterfaceDetail(deviceHardwareInfo,
			                                      &deviceInterfaceData,
			                                      deviceInterfaceDetail,
			                                      requiredSize, NULL, NULL))
			{
				// Attempt to extract the port information from the path
				CStringX devicePort = Port(deviceInterfaceDetail->DevicePath);
				if (!devicePort.IsEmpty()
				    && (matchPort.IsEmpty() || (devicePort == matchPort)))
				{
					list.AddTail(deviceInterfaceDetail->DevicePath);
				}
			}
		}
		
		// Destroy the device information set
		SetupDi::DestroyDeviceInfoList(deviceHardwareInfo);
	}
#endif

	// Filter out the names that cannot be opened
	return Device().FilterNames(list);
}

int Device::Sort(const TCHAR *format, int first, int last,
                 CStringListX &list)
{
	// Generate all the possible device names
	CStringListX all;
	GenerateNames(format, first, last, all);

	// Compare the canonicalised device names
	CStringListX matched;
	for (CStringListX::POSITION posAll = all.GetHeadPosition(); posAll != 0;)
	{
		CStringX nameAll = all.GetNext(posAll);
		for (CStringListX::POSITION posList = list.GetHeadPosition(); posList != 0;)
		{
			CStringListX::POSITION atList = posList;
			CStringX nameList = list.GetNext(posList);
			if (Canonicalise(nameList) == nameAll)
			{
				list.RemoveAt(atList);
				matched.AddTail(nameList);
			}
		}
	}

	// Add the matched device names back in at the start of the list
	list.AddHead(&matched);

	// Return the number of devices in the list
	return list.GetCount();
}

bool Device::IsA(const TCHAR *device, const TCHAR *format,
                 int first, int last)
{
	// Generate all the possible device names
	CStringListX list;
	GenerateNames(format, first, last, list);

	// Return whether there is a match
	return list.Find(Canonicalise(device)) != NULL;
}

bool Device::IsA(const TCHAR *device, const GUID &guid)
{
	// Canonicalise the device name
	CStringX canonical = Canonicalise(device);

	// Convert the GUID to an upper case string
	CStringX part = GUIDToString(guid);
	part.MakeUpper();

	// Return whether the device name contains the GUID
	return 0 <= canonical.Find(part);
}

// Should overlapped I/O be used
bool Device::UseOverlappedIO() const
{
	// A derived class can disable use of overlapped I/O
#if !defined _WINCE && !defined _WIN32_WCE
	return true;
#else
	return false;
#endif
}

// Support for external device locking
DFUEngine::Result Device::PreOpen(const TCHAR *device)
{
	return DFUEngine::success;
}

DFUEngine::Result Device::PostClose()
{
	return DFUEngine::success;
}

// Generation of possible device names
int Device::GenerateNames(const TCHAR *format, int first, int last,
                          CStringListX &list)
{
	// Generate all the possible device names
	list.RemoveAll();
	for (int index = first; index <= last; index++)
	{
		CStringX name;
		name.Format(format, index);
		list.AddTail(Canonicalise(name));
	}

	// Return the number of device names
	return list.GetCount();
}

// Filter out device names that cannot be opened
int Device::FilterNames(CStringListX &list)
{
	// Remove from the list device names that cannot be opened
	for (CStringListX::POSITION posDevice = list.GetHeadPosition(); posDevice != 0;)
	{
		CStringListX::POSITION atDevice = posDevice;
		if (!Open(list.GetNext(posDevice)))
		{
			list.RemoveAt(atDevice);
		}
	}

	// Return the number of available devices
	return list.GetCount();
}

// Convert a GUID to a string
CStringX Device::GUIDToString(const GUID &guid)
{
	// Convert the GUID to a string
	CStringX format;
	format.Format(deviceGUID, guid.Data1, guid.Data2, guid.Data3,
	              guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
	              guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

	// Return the formatted GUID
	return format;
}
