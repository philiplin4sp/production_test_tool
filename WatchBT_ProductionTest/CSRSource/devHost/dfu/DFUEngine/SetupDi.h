///////////////////////////////////////////////////////////////////////
//
//	File	: SetupDi.h
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
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/SetupDi.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef SETUPDI_H
#define SETUPDI_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "DFUEngine.h"
#include <windows.h>
#include <setupapi.h>

// SetupDi class
class SetupDi
{
public:

	// A subset of the SetupDi functions
	static HDEVINFO GetClassDevs(LPGUID ClassGuid, LPCTSTR Enumerator,
	                             HWND hwndParent, DWORD flags);
	static bool DestroyDeviceInfoList(HDEVINFO DeviceInfoSet);
	static bool EnumDeviceInterfaces(HDEVINFO DeviceInfoSet,
	                                 PSP_DEVINFO_DATA DeviceInfoData,
	                                 LPGUID InterfaceClassGuid,
	                                 DWORD MemberIndex,
	                                 PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData);
	static bool GetDeviceInterfaceDetail(HDEVINFO DeviceInfoSet,
	                                     PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData,
	                                     PSP_INTERFACE_DEVICE_DETAIL_DATA DeviceInterfaceDetailData,
	                                     DWORD DeviceInterfaceDetailDataSize,
	                                     PDWORD RequiredSize,
	                                     PSP_DEVINFO_DATA DeviceInfoData);

private:

	// Perform dynamic linking on first use
	static void Link();
};

#endif
