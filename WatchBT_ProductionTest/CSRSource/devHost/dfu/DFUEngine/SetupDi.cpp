///////////////////////////////////////////////////////////////////////
//
//	File	: SetupDi.cpp
//		  Copyright (C) 2002-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: SetupDi
//
//	Purpose	: Explicit linking to the SetupDi...() functions from the
//			  SetupAPI library. This is used instead of the normal
//			  implicit linking, to allow operation on platforms that
//			  do not provide this API, such as Windows NT 4.
//
//	Modification history:
//	
//		1.1		25:Jul:02	at	File created.
//		1.2		26:Jul:02	at	Some functions implicitly linked.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/SetupDi.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "SetupDi.h"

// Visual C++ debugging code
#ifdef _DEBUG
#include <crtdbg.h>
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define new DEBUG_NEW
#endif

// Version string
const char *setupdi_id = "$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/SetupDi.cpp#1 $ Copyright (C) 2000-2006 Cambridge Silicon Radio Limited";

// Required DLL name
static const TCHAR setupdiDLL[]= _T("setupapi");

// Required function names
static const char setupdiEnumDeviceInterfacesName[] = "SetupDiEnumDeviceInterfaces";
#ifdef UNICODE
static const char setupdiGetDeviceInterfaceDetailName[] = "SetupDiGetDeviceInterfaceDetailW";
#else
static const char setupdiGetDeviceInterfaceDetailName[] = "SetupDiGetDeviceInterfaceDetailA";
#endif

// Function pointer types
typedef BOOL (WINAPI * setupdiEnumDeviceInterfacesPtr)(HDEVINFO DeviceInfoSet, PSP_DEVINFO_DATA DeviceInfoData, LPGUID InterfaceClassGuid, DWORD MemberIndex, PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData);
typedef BOOL (WINAPI * setupdiGetDeviceInterfaceDetailPtr)(HDEVINFO DeviceInfoSet, PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData, PSP_DEVICE_INTERFACE_DETAIL_DATA DeviceInterfaceDetailData, DWORD DeviceInterfaceDetailDataSize, PDWORD RequiredSize, PSP_DEVINFO_DATA DeviceInfoData);

// Function pointers
static setupdiEnumDeviceInterfacesPtr setupdiEnumDeviceInterfaces = NULL;
static setupdiGetDeviceInterfaceDetailPtr setupdiGetDeviceInterfaceDetail = NULL;

// A subset of the SetupDi functions
HDEVINFO SetupDi::GetClassDevs(LPGUID ClassGuid, LPCTSTR Enumerator,
	                           HWND hwndParent, DWORD flags)
{
	// Pass on directly to the SetupDi function
	return SetupDiGetClassDevs(ClassGuid, Enumerator, hwndParent, flags);
}

bool SetupDi::DestroyDeviceInfoList(HDEVINFO DeviceInfoSet)
{
	// Pass on directly to the SetupDi function
	return SetupDiDestroyDeviceInfoList(DeviceInfoSet) != 0;
}

bool SetupDi::EnumDeviceInterfaces(HDEVINFO DeviceInfoSet,
	                               PSP_DEVINFO_DATA DeviceInfoData,
	                               LPGUID InterfaceClassGuid,
	                               DWORD MemberIndex,
	                               PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData)
{
	// Link on first use
	Link();

	// Pass on to the SetupDi function if possible
	return setupdiEnumDeviceInterfaces
	       && setupdiEnumDeviceInterfaces(DeviceInfoSet, DeviceInfoData, InterfaceClassGuid, MemberIndex, DeviceInterfaceData);
}

bool SetupDi::GetDeviceInterfaceDetail(HDEVINFO DeviceInfoSet,
	                                   PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData,
	                                   PSP_INTERFACE_DEVICE_DETAIL_DATA DeviceInterfaceDetailData,
	                                   DWORD DeviceInterfaceDetailDataSize,
	                                   PDWORD RequiredSize,
	                                   PSP_DEVINFO_DATA DeviceInfoData)
{
	// Link on first use
	Link();

	// Pass on to the SetupDi function if possible
	return setupdiGetDeviceInterfaceDetail
	       && setupdiGetDeviceInterfaceDetail(DeviceInfoSet, DeviceInterfaceData, DeviceInterfaceDetailData, DeviceInterfaceDetailDataSize, RequiredSize, DeviceInfoData) != 0;
}

// Perform dynamic linking on first use
void SetupDi::Link()
{
	static bool initialised = false;

	// No action if already initialised
	if (initialised) return;
	initialised = true;

	// Attempt to obtain a handle to the DLL (should never fail since it is linked)
	HINSTANCE dll = GetModuleHandle(setupdiDLL);
	if (!dll) return;

	// Find the addresses of the required functions
	setupdiEnumDeviceInterfaces = (setupdiEnumDeviceInterfacesPtr)
	                              GetProcAddress(dll, setupdiEnumDeviceInterfacesName);
	setupdiGetDeviceInterfaceDetail = (setupdiGetDeviceInterfaceDetailPtr)
	                                  GetProcAddress(dll, setupdiGetDeviceInterfaceDetailName);
}
