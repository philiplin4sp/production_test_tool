///////////////////////////////////////////////////////////////////////
//
//	File	: DFURequests.cpp
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
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFURequests.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFURequests.h"
#include "thread/signal_box.h"

// Visual C++ debugging code
#ifdef _DEBUG
#include <crtdbg.h>
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define new DEBUG_NEW
#endif

// Version string
const char *dfurequests_id = "$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFURequests.cpp#1 $ Copyright (C) 2000-2006 Cambridge Silicon Radio Limited";

class DFURequestsImpl
{
public:
	DFURequestsImpl();

	DFURequests *parent;

	// Is a connection active
	bool connected;

	// Number of DFU interface, set by Connect
	DFURequests::InterfaceDescriptor dfuInterfaceDescriptor;

	// Object to receive progress notifications
	DFUInterface *progress;

	// Pre- and post-processing of RPC operations
	DFUEngine::Result PreRPC();
	DFUEngine::Result PostRPC(DFUEngine::Result result);
};

// Length of time after reset before device responds to communications
static const uint32 delayResetMilliseconds = 1300;

// Mapping of DFU status codes to DFUEngine result codes
struct statusCodeMapEntry
{
	uint8 status;
	DFUEngine::ResultCode result;
};
static const statusCodeMapEntry statusCodeMap[] =
{
	{DFURequests::ok,				DFUEngine::success},
	{DFURequests::err_target,		DFUEngine::fail_dfu_target},
	{DFURequests::err_file,			DFUEngine::fail_dfu_file},
	{DFURequests::err_write,		DFUEngine::fail_dfu_write},
	{DFURequests::err_erase,		DFUEngine::fail_dfu_erase},
	{DFURequests::err_check_erased,	DFUEngine::fail_dfu_check_erased},
	{DFURequests::err_prog,			DFUEngine::fail_dfu_prog},
	{DFURequests::err_verify,		DFUEngine::fail_dfu_verify},
	{DFURequests::err_address,		DFUEngine::fail_dfu_address},
	{DFURequests::err_not_done,		DFUEngine::fail_dfu_not_done},
	{DFURequests::err_firmware,		DFUEngine::fail_dfu_firmware},
	// DFURequests::err_vendor is handled as a special case
	{DFURequests::err_usbr,			DFUEngine::fail_dfu_usbr},
	{DFURequests::err_por,			DFUEngine::fail_dfu_por},
	// DFURequests::err_unknown is handled as a special case
	{DFURequests::err_stalledpkt,	DFUEngine::fail_dfu_stalledpkt}
};

// Constructor
DFURequests::DFURequests() : impl_(new DFURequestsImpl)
{
	impl_->parent = this;
}

DFURequestsImpl::DFURequestsImpl() : connected(false)
{
}

// Destructor
DFURequests::~DFURequests()
{
	Disconnect();
}

// Abort event
DFUEngine::Result DFURequests::CheckAbort(uint32 sleep)
{
	// Delay for the requested length of time
	if (eventAbort)
	{
		if (DFUInterface::Event::CheckMultiple(1, &eventAbort, sleep) != -1) return DFUEngine::aborted;
	}
	else SingleSignal().wait(sleep);

	// Successful if this point reached
	return DFUEngine::success;
}

// Update activity description
void DFURequests::Progress(DFUEngine::SubOperation subOperation, int baudRate)
{
	if (impl_->progress) impl_->progress->Progress(subOperation, baudRate);
}

void DFURequests::Progress(DFUEngine::SubOperation subOperation, const TCHAR *device)
{
	if (impl_->progress) impl_->progress->Progress(subOperation, device);
}

// Connection and disconnection
DFUEngine::Result DFURequests::Connect(bool hintDFU)
{
	if (!impl_->connected)
	{
		DFUEngine::Result result = RPCConnect(hintDFU);
		if (!result) return result;

		impl_->connected = true;
		result = RPCGetInterfaceDFU(impl_->dfuInterfaceDescriptor);
		if (!result) return result;
	}
	return DFUEngine::success;
}

DFUEngine::Result DFURequests::Disconnect()
{
	if (impl_->connected)
	{
		impl_->connected = false;
		return RPCDisconnect();
	}
	return DFUEngine::success;
}

DFUEngine::Result DFURequests::RPCConnect(bool hintDFU)
{
	return DFUEngine::success;
}

DFUEngine::Result DFURequests::RPCDisconnect()
{
	return DFUEngine::success;
}

// Individual USB DFU control requests
DFUEngine::Result DFURequests::Detach(uint16 timeout)
{
	DFUEngine::Result result = impl_->PreRPC();
	if (result) result = RPCDetach(timeout);
	return impl_->PostRPC(result);
}

DFUEngine::Result DFURequests::Dnload(uint16 blockNum, const void *buffer,
									  uint16 bufferLength)
{
	DFUEngine::Result result = impl_->PreRPC();
	if (result) result = RPCDnload(blockNum, buffer, bufferLength);
	return impl_->PostRPC(result);
}

DFUEngine::Result DFURequests::Upload(uint16 blockNum, void *buffer,
									  uint16 bufferLength, uint16 &replyLength)
{
	DFUEngine::Result result = impl_->PreRPC();
	if (result) result = RPCUpload(blockNum, buffer, bufferLength, replyLength);
	return impl_->PostRPC(result);
}

DFUEngine::Result DFURequests::GetStatus(DFUStatus &status)
{
	DFUEngine::Result result = impl_->PreRPC();
	if (result) result = RPCGetStatus(status);
	return impl_->PostRPC(result);
}

DFUEngine::Result DFURequests::ClrStatus()
{
	DFUEngine::Result result = impl_->PreRPC();
	if (result) result = RPCClrStatus();
	return impl_->PostRPC(result);
}

DFUEngine::Result DFURequests::GetState(uint8 &state)
{
	DFUEngine::Result result = impl_->PreRPC();
	if (result) result = RPCGetState(state);
	return impl_->PostRPC(result);
}

DFUEngine::Result DFURequests::Abort()
{
	DFUEngine::Result result = impl_->PreRPC();
	if (result) result = RPCAbort();
	return impl_->PostRPC(result);
}

DFUEngine::Result DFURequests::RPCDetach(uint16 timeout)
{
	// Prepare the setup data
	Setup setup;
	setup.bmRequestTypeRecipient = recipient_interface;
	setup.bmRequestTypeType = type_class;
	setup.bmRequestTypeDirection = dir_host_to_device;
	setup.bRequest = request_detach;
	setup.wValue = timeout;
	setup.wIndex = impl_->dfuInterfaceDescriptor.bInterfaceNumber;
	setup.wLength = 0;

	// Perform the request
	return ControlRequest(setup);
}

DFUEngine::Result DFURequests::RPCDnload(uint16 blockNum, const void *buffer, uint16 bufferLength)
{
	// Prepare the setup data
	Setup setup;
	setup.bmRequestTypeRecipient = recipient_interface;
	setup.bmRequestTypeType = type_class;
	setup.bmRequestTypeDirection = dir_host_to_device;
	setup.bRequest = request_dnload;
	setup.wValue = blockNum;
	setup.wIndex = impl_->dfuInterfaceDescriptor.bInterfaceNumber;
	setup.wLength = bufferLength;

	// Perform the request
	return ControlRequest(setup, const_cast<void *>(buffer), bufferLength);
}

DFUEngine::Result DFURequests::RPCUpload(uint16 blockNum, void *buffer, uint16 bufferLength, uint16 &replyLength)
{
	// Prepare the setup data
	Setup setup;
	setup.bmRequestTypeRecipient = recipient_interface;
	setup.bmRequestTypeType = type_class;
	setup.bmRequestTypeDirection = dir_device_to_host;
	setup.bRequest = request_upload;
	setup.wValue = blockNum;
	setup.wIndex = impl_->dfuInterfaceDescriptor.bInterfaceNumber;
	setup.wLength = bufferLength;

	// Perform the request
	return ControlRequest(setup, buffer, bufferLength, &replyLength);
}

DFUEngine::Result DFURequests::RPCGetStatus(DFUStatus &status)
{
	// Prepare the setup data
	Setup setup;
	setup.bmRequestTypeRecipient = recipient_interface;
	setup.bmRequestTypeType = type_class;
	setup.bmRequestTypeDirection = dir_device_to_host;
	setup.bRequest = request_get_status;
	setup.wValue = 0;
	setup.wIndex = impl_->dfuInterfaceDescriptor.bInterfaceNumber;
	setup.wLength = sizeof(DFUStatus);

	// Perform the request
	uint16 replyLength;
	DFUEngine::Result result = ControlRequest(setup, &status, sizeof(status), &replyLength);
	if (!result) return result;

	// Extract the data from the reply
	if (replyLength < sizeof(DFUStatus)) result = DFUEngine::fail_reply_short;
	if (sizeof(DFUStatus) < replyLength) result = DFUEngine::fail_reply_long;

	// Return the result
	return DFUEngine::success;
}

DFUEngine::Result DFURequests::RPCClrStatus()
{
	// Prepare the setup data
	Setup setup;
	setup.bmRequestTypeRecipient = recipient_interface;
	setup.bmRequestTypeType = type_class;
	setup.bmRequestTypeDirection = dir_host_to_device;
	setup.bRequest = request_clr_status;
	setup.wValue = 0;
	setup.wIndex = impl_->dfuInterfaceDescriptor.bInterfaceNumber;
	setup.wLength = 0;

	// Perform the request
	return ControlRequest(setup);
}

DFUEngine::Result DFURequests::RPCGetState(uint8 &state)
{
	// Prepare the setup data
	Setup setup;
	setup.bmRequestTypeRecipient = recipient_interface;
	setup.bmRequestTypeType = type_class;
	setup.bmRequestTypeDirection = dir_device_to_host;
	setup.bRequest = request_get_state;
	setup.wValue = 0;
	setup.wIndex = impl_->dfuInterfaceDescriptor.bInterfaceNumber;
	setup.wLength = sizeof(uint8);

	// Perform the request
	uint16 replyLength;
	DFUEngine::Result result = ControlRequest(setup, &state, sizeof(uint8), &replyLength);
	if (!result) return result;

	// Extract the data from the reply
	if (replyLength < sizeof(uint8)) result = DFUEngine::fail_reply_short;
	if (sizeof(uint8) < replyLength) result = DFUEngine::fail_reply_long;

	// Return the result
	return DFUEngine::success;
}

DFUEngine::Result DFURequests::RPCAbort()
{
	// Prepare the setup data
	Setup setup;
	setup.bmRequestTypeRecipient = recipient_interface;
	setup.bmRequestTypeType = type_class;
	setup.bmRequestTypeDirection = dir_host_to_device;
	setup.bRequest = request_abort;
	setup.wValue = 0;
	setup.wIndex = impl_->dfuInterfaceDescriptor.bInterfaceNumber;
	setup.wLength = 0;

	// Perform the request
	return ControlRequest(setup);
}

// Non USB DFU control requests
DFUEngine::Result DFURequests::Reset(bool wait)
{
	// Perform the reset
	DFUEngine::Result result = impl_->PreRPC();
	if (result) result = RPCReset();
	if (result) result = Disconnect();

	// Allow the device to complete its reset
	if (result && wait) result = CheckAbort(delayResetMilliseconds);

	// Check the result
	return impl_->PostRPC(result);
}

DFUEngine::Result DFURequests::GetInterfaceDFU(InterfaceDescriptor &descriptor)
{
	if(impl_->connected)
	{
		descriptor = impl_->dfuInterfaceDescriptor;
		return DFUEngine::success;
	}
	else
	{
		DFUEngine::Result result = impl_->PreRPC();
		if (result) result = RPCGetInterfaceDFU(descriptor);
		return impl_->PostRPC(result);
	}
}

DFUEngine::Result DFURequests::GetDevice(DeviceDescriptor &descriptor)
{
	DFUEngine::Result result = impl_->PreRPC();
	if (result) result = RPCGetDevice(descriptor);
	return impl_->PostRPC(result);
}

DFUEngine::Result DFURequests::GetFunct(DFUFunctionalDescriptor &descriptor)
{
	DFUEngine::Result result = impl_->PreRPC();
	if (result) result = RPCGetFunct(descriptor);
	return impl_->PostRPC(result);
}

DFUEngine::Result DFURequests::GetString(uint8 index, CStringX &descriptor)
{
	DFUEngine::Result result = impl_->PreRPC();
	if (result) result = RPCGetString(index, descriptor);
	return impl_->PostRPC(result);
}


// Pre-download preparation step
DFUEngine::Result DFURequests::PreDnload(DFUFile &file)
{
	// This function is not normally required
	return DFUEngine::success;
}

// Conversion of DFU status codes to DFUEngine result codes
DFUEngine::Result DFURequests::MapDFUStatus(uint8 status, uint8 stringIndex)
{
	// Special case for vendor-specific failures
	DFUEngine::Result result(DFUEngine::fail_dfu_unknown);
	if (status == err_vendor)
	{
		// Attempt to obtain a description of the error
		CStringX description;
		if (RPCGetString(stringIndex, description) && !description.IsEmpty())
		{
			result = DFUEngine::Result(DFUEngine::fail_dfu_vendor_str, description);
		}
		else
		{
			description.Format(_T("%d"), stringIndex);
			result = DFUEngine::Result(DFUEngine::fail_dfu_vendor_num, description);
		}
	}
	else
	{
		// Attempt to find the appropriate result code
		for (int index = 0;
			 index < (sizeof(statusCodeMap) / sizeof(statusCodeMapEntry));
			 ++index)
		{
			if (statusCodeMap[index].status == status)
			{
				result = statusCodeMap[index].result;
				break;
			}
		}
	}

	// Return the result
	return result;
}

// Check whether actions are appropriate to the current state
bool DFURequests::CanAbort(uint8 state)
{
	return (state == dfu_dnload_sync)
	       || (state == dfu_manifest_sync);
}

bool DFURequests::CanClrStatus(uint8 state)
{
	return state == dfu_error;
}

// Pre- and post-processing of RPC operations
DFUEngine::Result DFURequestsImpl::PreRPC()
{
	if (!connected) return DFUEngine::fail_no_transport;
	return DFUEngine::success;
}

DFUEngine::Result DFURequestsImpl::PostRPC(DFUEngine::Result result)
{
	if (connected && (result.GetCode() == DFUEngine::fail_dfu_stalledpkt))
	{
		// Attempt to read and clear the DFU error code if stalled response
		DFURequests::DFUStatus status;
		if (parent->RPCGetStatus(status))
		{
			if (status.bStatus != DFURequests::ok)
			{
				result = parent->MapDFUStatus(status.bStatus, status.iString);
			}
			if (status.bState == DFURequests::dfu_error) parent->RPCClrStatus();
		}
	}
	return result;
}

// Set or get the abort event object
void DFURequests::SetAbort(DFUInterface::Event *abort)
{
	eventAbort = abort;
}

// Set object to receive progress notifications
void DFURequests::SetProgress(DFUInterface *progress)
{
	impl_->progress = progress;
}

// Enable or disable HID mode.
DFUEngine::Result DFURequests::EnableHidMode(bool enable)
{
	DFUEngine::Result result = impl_->PreRPC();
	if (result) 
		result = RPCEnableHidMode(enable);
	return impl_->PostRPC(result);
}