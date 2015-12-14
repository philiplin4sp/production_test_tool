///////////////////////////////////////////////////////////////////////
//
//	File	: DFURequests.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFURequests
//
//	Purpose	: Generic implementation of basic DFU operations, with
//			  virtual functions used to provide the transport and
//			  protocol specific behaviour in derived classes.
//
//			  The following member functions should be called to
//			  start and end a connection:
//				Connect				- Start a connection. The
//									  DFU interface number is
//									  determined.
//				Disconnect			- End a connection.
//
//			  It is the responsibility of the user of this class
//			  to call Connect at the appropriate time. Disconnect
//			  is called automatically after a Reset or when this
//			  object is destroyed, but it may also be called
//			  explicitly if required. These functions have no
//			  effect if the transport is already in the required
//			  state.
//
//			  Corresponding member functions may be overridden by
//			  the transport implementation:
//				RPCConnect			- Start a connection. The
//									  transport may use this to
//									  trigger automatic
//									  identification of connection
//									  parameters. A hint concerning
//									  the current device mode can
//									  be provided to help the
//									  transport layer establish a
//									  connection quicker.
//				RPCDisconnect		- End a connection.
//
//			  The following member functions allow the basic DFU
//			  operations to be performed:
//				Detach				- Prepare the device to enter
//									  DFU mode when reset.
//				Dnload				- Perform a stage of downloading
//									  to the device, or with zero
//									  length data to end.
//				Upload				- Perform a stage of uploading
//									  from the device.
//				GetStatus			- Read the current status of the
//									  device, triggering pending
//									  operations.
//				ClrStatus			- Clear any error condition and
//									  return to idle mode.
//				GetState			- Read the current state.
//				Abort				- Abort an upload or download
//									  in progress.
//				Reset				- Perform a device reset,
//									  starting DFU mode if
//									  appropriate.
//				GetInterfaceDFU		- Find the DFU interface
//									  descriptor.
//				GetDevice			- Read information from the
//									  device descriptor.
//				GetFunct			- Read information from the
//									  DFU functional descriptor.
//				GetString			- Read a USB string descriptor.
//
//			  These functions automatically call GetStatus and
//			  ClrStatus as appropriate if the result code indicates
//			  an error condition.
//
//			  All of the above operations have corresponding virtual
//			  member functions with an "RPC" prefix. Implementations
//			  of these should be provided by derived classes. Default
//			  implementations of standard USB DFU control requests
//			  are provided using an additional generic control
//			  request virtual method:
//				ControlRequest		- Perform a generic control
//									  request. The
//									  bmRequestTypeDirection flag
//									  is used to determine the
//									  transfer direction.
//
//			  A method is provided to let the transport prepare itself
//			  for the download. This is primarily provided for device
//			  drivers that implement the DFU algorithm themselves:
//				PreDnload			- Supply a copy of the DFU file
//									  prior to any Dnload requests.
//
//			  A method is provided to map DFU status codes to
//			  DFUEngine result codes. This is a non-static function
//			  because it may be necessary to read USB string
//			  descriptors if a vendor-specific error code has been
//			  used:
//				MapDFUStatus		- Convert a DFU status code to the
//									  corresponding DFUEngine result
//									  code.
//
//			  The Abort and ClrStatus methods can only be used
//			  when the device is in specific states. Two member
//			  functions are provided to check whether they can be used:
//				CanAbort			- Check whether an abort can be
//									  performed.
//				CanClrStatus		- Check whether the status can be
//									  cleared.
//
//			  A set of member functions are also provided to allow
//			  an abort event object to be used:
//				SetAbort			- Change the abort event.
//				GetAbort			- Obtain a pointer to the abort
//									  event.
//				CheckAbort			- Check for an abort condition
//									  while sleeping for the specified
//									  length of time.
//
//			  The transport implementation may override SetAbort to
//			  receive notification of changes, but it must still call
//			  function in the base class.
//
//			  Overloaded member functions are also provided to update
//			  the sub-operation of the activity:
//				SetProgress			- Change the object to be notified.
//				Progress			- Set the sub-operation.
//
//	Modification history:
//	
//		1.1		27:Sep:00	at	File created.
//		1.2		12:Oct:00	at	Transport no longer disconnected if
//								Connect called when already connected.
//		1.3		12:Oct:00	at	Added generic descriptor header.
//		1.4		17:Oct:00	at	Added timeout value to detach request.
//		1.5		19:Oct:00	at	Added methods to replace stalled result
//								codes with the actual error, and to
//								check if abort or clear status
//								operations are valid.
//		1.6		24:Oct:00	at	Provided separate functions for client
//								use and implementation. All operations
//								attempt to obtain the actual error code
//								for stalled responses. Added support
//								for aborting operations.
//		1.7		02:Nov:00	at	Added delay after reset.
//		1.8		02:Nov:00	at	Increased delay after reset from 1s to
//								1.2s to ensure H4 works reliably.
//		1.9		03:Nov:00	at	Increased delay after reset again to
//								1.3s to ensure H4 works reliably.
//		1.10	15:Nov:00	at	Prevented abort being performed
//								needlessly in idle state.
//		1.11	16:Nov:00	at	Increased delay after reset again to
//								2.5s to ensure USB works reliably.
//		1.12	17:Nov:00	at	Reduced delay after reset back to
//								1.3s now that delay for USB moved
//								elsewhere.
//		1.13	21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.14	29:Nov:00	at	Corrected operation of delay after
//								reset.
//		1.15	30:Nov:00	at	Added function to sleep while checking
//								for an abort request.
//		1.16	15:Dec:00	at	Corrected states in which operations
//								can be aborted.
//		1.17	06:Mar:01	at	Added stubs for activity description.
//		1.18	06:Mar:01	at	Added implementation of activity
//								description update.
//		1.19	11:Apr:01	at	Corrected implementation of CheckAbort.
//		1.20	30:Apr:01	at	Added Windows CE support.
//		1.21	23:May:01	at	Tidied Windows CE support.
//		1.22	23:Jul:01	at	Added version string.
//		1.23	05:Oct:01	at	Moved control request codes to header
//								and added PreDnload method.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFURequests.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef DFUREQUESTS_H
#define DFUREQUESTS_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "DFUEngine.h"
#include "DFUInterface.h"
#include "types.h"

// Device Descriptor
static const uint8 dfu10DeviceClass = 0xFE;
static const uint8 dfu10DeviceSubClass = 0x01;
static const uint8 dfu10DeviceProtocol = 0x00;

// Interface Descriptor
const uint8 dfuInterfaceClass = 0xFE;
const uint8 dfuInterfaceSubClass = 0x01;
const uint8 dfu10InterfaceProtocol = 0x00;
const uint8 dfu11RuntimeInterfaceProtocol = 0x01;
const uint8 dfu11DFUInterfaceProtocol = 0x02;

// Functional Descriptor
const uint16 DFU_VERSION_1_0 = 0x0100;

// DFURequests class
class DFURequests
{
public:

	// Request type values
	enum
	{
		dir_host_to_device = 0,
		dir_device_to_host = 1
	};
	enum
	{
		type_standard = 0,
		type_class = 1,
		type_vendor = 2
	};
	enum
	{
		recipient_device = 0,
		recipient_interface = 1,
		recipient_endpoint = 2,
		recipient_other = 3
	};

	// Request numbers
	enum
	{
		request_detach = 0,
		request_dnload = 1,
		request_upload = 2,
		request_get_status = 3,
		request_clr_status = 4,
		request_get_state = 5,
		request_abort = 6
	};

	// Descriptor values
	enum
	{
		descriptor_device = 0x01,
		descriptor_configuration = 0x02,
		descriptor_string = 0x03,
		descriptor_interface = 0x04,
		descriptor_endpoint = 0x05,
		descriptor_device_qualifier = 0x06,
		descriptor_other_speed_configuration = 0x07,
		descriptor_interface_power = 0x08,
		descriptor_dfu_functional = 0x21
	};

	// DFU functional attributes
	enum
	{
		attribute_can_download = 1,
		attribute_can_upload = 2,
		attribute_manifestation_tolerant = 4
	};

	// DFU status codes
	enum
	{
		ok = 0x00,				// No error condition is present
		err_target = 0x01,		// DFU file not for this device
		err_file = 0x02,		// DFU file fails verification
		err_write = 0x03,		// Device unable to write memory
		err_erase = 0x04,		// Memory erase function failed
		err_check_erased = 0x05,// Memory erase check failed
		err_prog = 0x06,		// Program memory function failed
		err_verify = 0x07,		// Memory failed verification
		err_address = 0x08,		// Address outside valid range
		err_not_done = 0x09,	// Unexpected end of data
		err_firmware = 0x0a,	// Firmware corrupt - DFU mode only
		err_vendor = 0xb,		// Vendor specific error (string)
		err_usbr = 0x0c,		// Unexpected USB reset signalling
		err_por = 0x0d,			// Unexpected power-on reset
		err_unknown = 0x0e,		// Unknown DFU failure
		err_stalledpkt = 0x0f	// Unexpected request received
	};

	// DFU device states
	enum
	{
		app_idle = 0,			// Application operating normally
		app_detach = 1,			// Waiting for USB reset for DFU mode
		dfu_idle = 2,			// Waiting for requests in DFU mode
		dfu_dnload_sync = 3,	// Data received, waiting for query
		dfu_dnload_busy = 4,	// Programming memory in progress
		dfu_dnload_idle = 5,	// Waiting for download to continue
		dfu_manifest_sync = 6,	// End of data, waiting for query
		dfu_manifest = 7,		// Processing end of data
		dfu_manifest_wait_reset = 8,// Waiting for USB reset
		dfu_upload_idle = 9,	// Waiting for upload to continue
		dfu_error = 10			// Error occurred, waiting for clear
	};

#pragma pack(push, 1)

	// USB control requests setup data
	struct Setup
	{
		uint8 bmRequestTypeRecipient : 5;// Recipient
		uint8 bmRequestTypeType : 2;// Type of request
		uint8 bmRequestTypeDirection : 1;// Transfer direction
		uint8 bRequest;			// Specific request number
		uint16 wValue;			// Request dependent data
		uint16 wIndex;			// Request dependent data
		uint16 wLength;			// Amount of data to transfer
	};

	// USB descriptor header
	struct Descriptor
	{
		uint8 bLength;
		uint8 bDescriptorType;
	};

	// USB Configuration Descriptor
	struct ConfigurationDescriptor
	{
		uint8 bLength;
		uint8 bDescriptorType;
		uint16 wTotalLength;
		uint8 bNumInterfaces;
		uint8 bConfigurationValue;
		uint8 iConfiguration;
		uint8 bmAttributes;
		uint8 bMaxPower;
	};

	// USB Interface Descriptor
	struct InterfaceDescriptor
	{
		uint8 bLength;
		uint8 bDescriptorType;
		uint8 bInterfaceNumber;
		uint8 bAlternateSetting;
		uint8 bNumEndpoints;
		uint8 bInterfaceClass;
		uint8 bInterfaceSubClass;
		uint8 bInterfaceProtocol;
		uint8 iInterface;
	};

	// USB Device Descriptor (only used fields commented)
	struct DeviceDescriptor
	{
		uint8 bLength;
		uint8 bDescriptorType;
		uint16 bcdUSB;
		uint8 bDeviceClass;		// Class code
		uint8 bDeviceSubClass;	// Subclass code
		uint8 bDeviceProtocol;	// Protocol code
		uint8 bMaxPacketSize0;
		uint16 idVendor;		// Vendor ID
		uint16 idProduct;		// Product ID
		uint16 bcdDevice;		// Device release number
		uint8 iManufacturer;
		uint8 iProduct;
		uint8 iSerialNumber;
		uint8 bNumConfigurations;
	};

	// DFU Functional Descriptor (only used fields commented)

	struct DFU10FunctionalDescriptor
	{
		uint8 bLength;
		uint8 bDescriptorType;
		uint8 bmAttributes;		// DFU attributes
		uint16 wDetachTimeout;	// Maximum detact timeout
		uint16 wTransferSize;	// Maximum transfer size
	};
	struct DFUFunctionalDescriptor
	{
		uint8 bLength;
		uint8 bDescriptorType;
		uint8 bmAttributes;		// DFU attributes
		uint16 wDetachTimeout;	// Maximum detact timeout
		uint16 wTransferSize;	// Maximum transfer size
		uint16 bcdDFUVersion;	// DFU protocol version (0x0100 = DFU1.0, 0x0101 = DFU1.1)

		DFUFunctionalDescriptor &operator=(const DFU10FunctionalDescriptor &o)
		{
			bLength = o.bLength;
			bDescriptorType = o.bDescriptorType;
			bmAttributes = o.bmAttributes;
			wDetachTimeout = o.wDetachTimeout;
			wTransferSize = o.wTransferSize;
			bcdDFUVersion = DFU_VERSION_1_0;
			return *this;
		}
	};
	typedef DFUFunctionalDescriptor DFU11FunctionalDescriptor;

	// DFU Status
	struct DFUStatus
	{
		uint32 bStatus : 8;
		uint32 bwPollTimeout : 24;
		uint8 bState;
		uint8 iString;
	};

#pragma pack(pop)

	// Constructor
	DFURequests();

	// Destructor
	virtual ~DFURequests();

	// Connection and disconnection
	DFUEngine::Result Connect(bool hintDFU = false);
	DFUEngine::Result Disconnect();

	// Individual USB DFU control requests
	DFUEngine::Result Detach(uint16 timeout);
	DFUEngine::Result Dnload(uint16 blockNum, const void *buffer = 0, uint16 bufferLength = 0);
	DFUEngine::Result Upload(uint16 blockNum, void *buffer, uint16 bufferLength, uint16 &replyLength);
	DFUEngine::Result GetStatus(DFUStatus &status);
	DFUEngine::Result ClrStatus();
	DFUEngine::Result GetState(uint8 &state);
	DFUEngine::Result Abort();

	// Non USB DFU control requests
	DFUEngine::Result Reset(bool wait = true);
	DFUEngine::Result GetInterfaceDFU(InterfaceDescriptor &descriptor);
	DFUEngine::Result GetDevice(DeviceDescriptor &descriptor);
	DFUEngine::Result GetFunct(DFUFunctionalDescriptor &descriptor);
	DFUEngine::Result GetString(uint8 index, CStringX &descriptor);

	// Pre-download preparation step
	virtual DFUEngine::Result PreDnload(DFUFile &file);

	// Conversion of DFU status codes to DFUEngine result codes
	DFUEngine::Result MapDFUStatus(uint8 status, uint8 stringIndex);

	// Check whether actions are appropriate to the current state
	bool CanAbort(uint8 state);
	bool CanClrStatus(uint8 state);

	// Set or get the abort event object
	virtual void SetAbort(DFUInterface::Event *abort = 0);

	// Set object to receive progress notifications
	void SetProgress(DFUInterface *progress);

	// HID operations
	virtual DFUEngine::Result EnableHidMode(bool enable = true);

protected:
	// Abort event
	DFUInterface::Event *eventAbort;
	DFUEngine::Result CheckAbort(uint32 sleep = 0);

	// Update activity description
	void Progress(DFUEngine::SubOperation subOperation, int baudRate = 0);
	void Progress(DFUEngine::SubOperation subOperation, const TCHAR *device);

	// Connection and disconnection
	virtual DFUEngine::Result RPCConnect(bool hintDFU);
	virtual DFUEngine::Result RPCDisconnect();

	// Individual USB DFU control requests (with default implementation)
	virtual DFUEngine::Result RPCDetach(uint16 timeout);
	virtual DFUEngine::Result RPCDnload(uint16 blockNum, const void *buffer = 0, uint16 bufferLength = 0);
	virtual DFUEngine::Result RPCUpload(uint16 blockNum, void *buffer, uint16 bufferLength, uint16 &replyLength);
	virtual DFUEngine::Result RPCGetStatus(DFUStatus &status);
	virtual DFUEngine::Result RPCClrStatus();
	virtual DFUEngine::Result RPCGetState(uint8 &state);
	virtual DFUEngine::Result RPCAbort();

	// Non USB DFU control requests
	virtual DFUEngine::Result RPCReset() = 0;
	virtual DFUEngine::Result RPCGetInterfaceDFU(InterfaceDescriptor &descriptor) = 0;
	virtual DFUEngine::Result RPCGetDevice(DeviceDescriptor &descriptor) = 0;
	virtual DFUEngine::Result RPCGetFunct(DFUFunctionalDescriptor &descriptor) = 0;
	virtual DFUEngine::Result RPCGetString(uint8 index, CStringX &descriptor) = 0;
	
	// Generic control requests
	virtual DFUEngine::Result ControlRequest(const Setup &setup,
	                                         void *buffer = 0,
							                 uint16 bufferLength = 0,
	                                         uint16 *replyLength = 0) = 0;

	// Hid operations
	virtual DFUEngine::Result RPCEnableHidMode(bool enable = true) = 0;

private:
	friend class DFURequestsImpl;
	SmartPtr<DFURequestsImpl> impl_;
};

#endif
