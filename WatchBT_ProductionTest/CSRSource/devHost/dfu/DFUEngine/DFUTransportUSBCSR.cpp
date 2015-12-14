///////////////////////////////////////////////////////////////////////
//
//	File	: DFURequestsUSBCSR.h
//		  Copyright (C) 2001-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFURequestsUSBCSR
//
//	Purpose	: Implementation of basic DFU operations specific to
//			  a USB transport via the CSR device driver.
//
//			  Most of the methods are implementations of virtual
//			  functions provided by DFURequests, so see that file for
//			  descriptions.
//
//			  A list of suitable CSR USB devices may be obtained using
//			  the following member function:
//				EnumerateDevices	- Set the supplied list to a list
//									  of strings describing the
//									  available devices.
//
//			  A standard HCI transport can be created for a named
//			  device using the following member function:
//				ConnectDevice		- Create a transport to the named
//									  device.
//
//	Modification history:
//	
//		1.1		05:Oct:01	at	File created.
//		1.2		09:Oct:01	at	Added ConnectDevice method.
//		1.3		09:Oct:01	at	Reverted back to trying all device
//								indexes during enumeration.
//		1.4		14:Nov:01	at	Added basic Unicode support.
//		1.5		24:Jul:02	at	Interface GUID used for enumeration.
//		1.6		30:Jul:02	at	Port matching used during enumeration.
//		1.7		04:Nov:02	at	Added workaround for Windows XP SP1.
//		1.8		11:Nov:02	at	No cold reset at end of upgrade.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUTransportUSBCSR.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "objbase.h"
#include "initguid.h"
#include "DFUTransportUSBCSR.h"

// Visual C++ debugging code
#ifdef _DEBUG
#include <crtdbg.h>
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define new DEBUG_NEW
#endif

// Version string
const char *dfutransportusbcsr_id = "$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUTransportUSBCSR.cpp#1 $ Copyright (C) 2000-2006 Cambridge Silicon Radio Limited";

// Range of USB device numbers
static const int firstDevice = 0;
static const int lastDevice = 126;
static const int lastDeviceDFU = 126;

// USB device names
static const TCHAR deviceFormat[] = _T("CSR%d");
static const TCHAR deviceFormatDFU[] = _T("CSRDFU%d");

// Device driver interface GUID (F12D3CF8-B11D-457E-8641-BE2AF2D6D204)
DEFINE_GUID(GUID_INTERFACE, 0xF12D3CF8, 0xB11D, 0x457E, 0x86, 0x41, 0xBE, 0x2A, 0xF2, 0xD6, 0xD2, 0x04);

// Device IO control codes
static const uint32 ioctl_send_hci_command = 0x220002;
static const uint32 ioctl_get_hci_event = 0x220004;
static const uint32 ioctl_get_version = 0x220008;
static const uint32 ioctl_get_driver_name = 0x22000C;
static const uint32 ioctl_get_config_descriptor = 0x220010;
static const uint32 ioctl_get_device_descriptor = 0x220014;
static const uint32 ioctl_reset_device = 0x220018;
static const uint32 ioctl_reset_pipe = 0x22001c;
static const uint32 ioctl_dfu_control_transfer = 0x220038;
static const uint32 ioctl_block_hci_event = 0x220030;
static const uint32 ioctl_block_hci_data = 0x220034;

// Size of buffer for reading configuration descriptor
static const uint32 minConfigurationLength = 1024;
static const uint32 maxConfigurationLength = 1024 * 1024;

// Constructor
DFUTransportUSBCSR::DFUTransportUSBCSR()
{
}

// Destructor
DFUTransportUSBCSR::~DFUTransportUSBCSR()
{
	// Ensure that the device is closed cleanly
	Close();
}
	
// Opening and closing a device
DFUEngine::Result DFUTransportUSBCSR::Open(const CStringX device)
{
	// Attempt to open the device
	name = device;
	DFUEngine::Result result = this->device.Open(device);
	if (!result) return result;

	// Successful if this point reached
	return DFUEngine::success;
}

DFUEngine::Result DFUTransportUSBCSR::Close()
{
	// Close the device
	DFUEngine::Result result = device.Close();
	if (!result) return result;

	// Successful if this point reached
	return DFUEngine::success;
}

// Device enumeration
int DFUTransportUSBCSR::EnumerateDevices(CStringListX &devices)
{
	// Enumerate without attempting to match a port specification
	return EnumerateDevices(devices, _T(""));
}

int DFUTransportUSBCSR::EnumerateDevices(CStringListX &devices, const TCHAR *port)
{
	// Start by trying to enumerate devices by their interface GUID
	Device::Enumerate(GUID_INTERFACE, port, devices);

	// Fallback to the pattern matching the device name if none found
	if (Device::Port(port).IsEmpty() && devices.IsEmpty())
	{
		// Enumerate normal mode devices
		Device::Enumerate(deviceFormat, firstDevice, lastDevice, devices);

		// Enumerate DFU mode devices also
		CStringListX devicesDFU;
		Device::Enumerate(deviceFormatDFU, firstDevice, lastDeviceDFU, devicesDFU);
		devices.AddTail(&devicesDFU);
	}

	// Return the number of available USB devices
	return devices.GetCount();
}

bool DFUTransportUSBCSR::IsA(const CStringX device)
{
	return Device::IsA(device, GUID_INTERFACE)
	       || Device::IsA(device, deviceFormat, firstDevice, lastDevice)
	       || Device::IsA(device, deviceFormatDFU, firstDevice, lastDeviceDFU);
}

// HCI transport instantiation
DFUEngine::Result DFUTransportUSBCSR::ConnectDevice(const TCHAR *device,
                                                    DFUTransportCOM &transport,
													HANDLE handle)
{
	// DFU mode drivers do not present an HCI interface
	if (Device::IsA(device, deviceFormatDFU, firstDevice, lastDeviceDFU))
	{
		return DFUEngine::Result(DFUEngine::fail_usb_open, device);
	}

	// Attempt to start the transport
	return transport.ConnectUSBCSR(Device::Canonicalise(device, true), handle);
}

// Non USB DFU control requests
DFUEngine::Result DFUTransportUSBCSR::Reset()
{
	// Perform a USB cycle port
	DFUEngine::Result result = device.IOControl(ioctl_reset_device);
	if (!result) return result;

	// Successful if this point reached
	return DFUEngine::success;
}

DFUEngine::Result DFUTransportUSBCSR::GetInterfaceDFU(DFURequests::InterfaceDescriptor &descriptor)
{
	// Read the configuration descriptor and interface descriptors
	SmartPtr<uint8, true> descriptor_data;
	uint32 descriptorLength;
	DFUEngine::Result result = GetConfig(descriptor_data, descriptorLength);
	if (!result) return result;

	// Parse all descriptors until the required interface is found
	for (DFURequests::Descriptor *ptr = (DFURequests::Descriptor *) (uint8 *) descriptor_data;
	     ((uint8 *) ptr) < descriptor_data + descriptorLength;
		 ptr = (DFURequests::Descriptor *) (((uint8 *) ptr) + ptr->bLength))
	{
		// Only interested in interface descriptors
		if (ptr->bDescriptorType == DFURequests::descriptor_interface)
		{
			// Verify that the interface descriptor is valid
			if (ptr->bLength != sizeof(DFURequests::InterfaceDescriptor))
			{
				return DFUEngine::fail_usb_desc_bad;
			}

			// Check if it is the DFU descriptor
			DFURequests::InterfaceDescriptor *inter = (DFURequests::InterfaceDescriptor *) ptr;
			if ((inter->bInterfaceClass == dfuInterfaceClass)
				&& (inter->bInterfaceSubClass == dfuInterfaceSubClass))
			{
				// Verify that the DFU interface is valid
				if ((inter->bAlternateSetting != 0)
				    || (inter->bNumEndpoints != 0)
				    || (inter->bInterfaceProtocol != dfu10InterfaceProtocol
				        && inter->bInterfaceProtocol != dfu11RuntimeInterfaceProtocol
				        && inter->bInterfaceProtocol != dfu11DFUInterfaceProtocol))
				{
					return DFUEngine::fail_usb_desc_bad;
				}

				// Return the interface
				descriptor = *inter;
				return DFUEngine::success;
			}
		}
	}

	// Interface descriptor not found if this point reached
	return DFUEngine::fail_usb_no_dfu;
}

DFUEngine::Result DFUTransportUSBCSR::GetDevice(DFURequests::DeviceDescriptor &descriptor)
{
	// Read the device descriptor (device driver returns first 16 of 18 bytes)
	uint32 descriptorLength;
	DFUEngine::Result result = device.IOControl(ioctl_get_device_descriptor, 0, 0, &descriptor, sizeof(DFURequests::DeviceDescriptor), &descriptorLength);
	if (!result) return result;
	if (descriptorLength < sizeof(DFURequests::DeviceDescriptor))
	{
		return DFUEngine::fail_usb_desc_short;
	}
	if (sizeof(DFURequests::DeviceDescriptor) < descriptorLength)
	{
		return DFUEngine::fail_usb_desc_long;
	}

	// Successful if this point reached
	return DFUEngine::success;
}

DFUEngine::Result DFUTransportUSBCSR::GetFunct(DFURequests::DFUFunctionalDescriptor &descriptor)
{
	// Read the configuration descriptor including the functional descriptor
	SmartPtr<uint8, true> config;
	uint32 configLength;
	DFUEngine::Result result = GetConfig(config, configLength);
	if (!result) return result;

	// Parse all descriptors until the DFU functional descriptor is found
	for (DFURequests::Descriptor *ptr = (DFURequests::Descriptor *) (uint8 *) config;
	     ((uint8 *) ptr) < config + configLength;
	     ptr = (DFURequests::Descriptor *) (((uint8 *) ptr) + ptr->bLength))
	{
		// Only interested in the DFU functional descriptors
		if (ptr->bDescriptorType == DFURequests::descriptor_dfu_functional)
		{
			if (ptr->bLength == sizeof(DFURequests::DFU10FunctionalDescriptor))
			{
				descriptor = *(DFURequests::DFU10FunctionalDescriptor *) ptr;
			}
			else if (ptr->bLength == sizeof(DFURequests::DFU11FunctionalDescriptor))
			{
				descriptor = *(DFURequests::DFU11FunctionalDescriptor *) ptr;
			}
			else
			{
				return DFUEngine::fail_usb_desc_bad;
			}
			return DFUEngine::success;
		}
	}

	// Interface descriptor not found if this point reached
	return DFUEngine::fail_usb_no_dfu;
}

DFUEngine::Result DFUTransportUSBCSR::GetString(uint8 index, CStringX &descriptor)
{
	return DFUEngine::Result(DFUEngine::fail_unimplemented, _T("DFUTransportUSBCSR::GetString()"));
}

// Generic control requests
DFUEngine::Result DFUTransportUSBCSR::ControlRequest(const DFURequests::Setup &setup,
                                                     void *buffer,
                                                     uint16 bufferLength,
                                                     uint16 *replyLength)
{
	struct Header
	{
		uint8 bmRequest;
		uint8 bRequest;
		uint16 wValue;
		uint16 wIndex;
	};

	// Check that the buffer is large enough
	if (bufferLength < setup.wLength)
	{
		return DFUEngine::fail_com_buffer;
	}

	// Workaround for USB cycle port behaviour on Windows XP SP1
	if (setup.bRequest == DFURequests::request_detach)
	{
		// Attempt to start an HCI transport and disable deep sleep
		DFUTransportCOM transport;
		if (ConnectDevice(name, transport, device))
		{
			transport.BCCMDRadiotestPause();
		}
	}

	// Construct the request to send
	SmartPtr<uint8, true> request;
	uint16 requestLength;
	if (setup.bmRequestTypeDirection
		== DFURequests::dir_host_to_device)
	{
		// Setup packet and buffer to send
		requestLength = sizeof(Header) + setup.wLength;
		request = SmartPtr<uint8, true>(new uint8[requestLength]);
		memcpy(request, &setup, sizeof(Header));
		memcpy(request + sizeof(Header), buffer, setup.wLength);
	}
	else
	{
		// Only the setup packet to send
		requestLength = sizeof(Header);
		request = SmartPtr<uint8, true>(new uint8[requestLength]);
		memcpy(request, &setup, sizeof(Header));
	}

	// Perform the request
	uint32 read;
	DFUEngine::Result result = device.IOControl(ioctl_dfu_control_transfer, request, requestLength, buffer, bufferLength, &read);
	if (!result) return result;

	// Decode the reply
	if (replyLength) *replyLength = uint16(read);

	// Successful if this point reached
	return DFUEngine::success;
}

// Read the whole configuration descriptor
DFUEngine::Result DFUTransportUSBCSR::GetConfig(SmartPtr<uint8, true> &descriptor,
                                                uint32 &descriptorLength)
{
	// Keep trying larger buffers until configuration descriptor read
	DFUEngine::Result result = DFUEngine::fail_unknown;
	for (uint32 bufferLength = minConfigurationLength;
	     !result && (bufferLength <= maxConfigurationLength);
	     bufferLength *= 2)
	{
		descriptor = SmartPtr<uint8, true>(new uint8[bufferLength]);
		result = device.IOControl(ioctl_get_config_descriptor, 0, 0, descriptor, bufferLength, &descriptorLength);
	}

	// Return any error
	if (!result) return result;

	// Verify that the configuration descriptor is valid
	if (descriptorLength < sizeof(DFURequests::ConfigurationDescriptor))
	{
		return DFUEngine::fail_usb_desc_short;
	}
	DFURequests::ConfigurationDescriptor *configuration = (DFURequests::ConfigurationDescriptor *) (uint8 *) descriptor;
	if ((configuration->bLength != sizeof(DFURequests::ConfigurationDescriptor))
		|| (configuration->bDescriptorType != DFURequests::descriptor_configuration))
	{
		return DFUEngine::fail_usb_desc_bad;
	}
	if (descriptorLength < configuration->wTotalLength)
	{
		return DFUEngine::fail_usb_desc_short;
	}
	if (configuration->wTotalLength < descriptorLength)
	{
		return DFUEngine::fail_usb_desc_long;
	}

	// Successful if this point reached
	return DFUEngine::success;
}

// Enable or disable HID mode.
DFUEngine::Result DFUTransportUSBCSR::EnableHidMode(bool enable)
{
	DFUEngine::Result result = DFUEngine::fail_unknown;
	DFUTransportCOM transport;
	
	// Ensure that the transport is valid
	if (!bool(device)) 
	{
		return DFUEngine::fail_usb_none;
	}

	// Attempt to start an HCI transport and delete the initial boot mode key
	if (ConnectDevice(name, transport, device))
	{
		uint16 byValue = 0;

		if (enable)
		{
			byValue = 1;
		}
		else
		{
			byValue = 0;
		}

		//Set user key 9
		result = transport.BCCMDSetPS(0x3CD, &byValue, 1);
		transport.Disconnect();
	}

	return result;
}