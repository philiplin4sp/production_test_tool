///////////////////////////////////////////////////////////////////////
//
//	File	: DFURequestsCOM.cpp
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFURequestsCOM
//
//	Purpose	: Implementation of basic DFU operations specific to
//			  a COM port transport.
//
//			  Most of the methods are implementations of virtual
//			  functions provided by DFURequests, so see that file for
//			  descriptions.
//
//			  Three constructors are provided to allow the protocol
//			  to be selected:
//				2 parameters	- Autodetect settings.
//				3 parameters	- H4 transport.
//				5 parameters	- BCSP transport with optional
//								  tunnelling and link establishment.
//
//			  All of the constructors should be called with an event
//			  object that can be used to abort an operation in
//			  progress. The second parameter should be the name of the
//			  device to use. This can either be the raw device name
//			  (with or without a leading "\\.\" prefix), or one of the
//			  device descriptions supplied by EnumeratePorts. The third
//			  parameter should be one of the baud rates returned by
//			  EnumerateBaudRates. The fourth parameter indicates
//			  whether tunnelling should be used for a BCSP connection.
//
//			  A list of available serial ports and baud rates may be
//			  obtained using the following member functions:
//				EnumeratePorts		- Set the supplied list to a list
//									  of strings describing the
//									  available devices.
//				EnumerateBaudRates	- Set the supplied list to a list
//									  of integers giving the available
//									  baud rates. If called with two
//									  parameters then the first
//									  parameter should be the list of
//									  serial ports to check, otherwise
//									  all available ports are checked.
//
//	Modification history:
//	
//		1.1		02:Oct:00	at	File created.
//		1.2		04:Oct:00	at	Separated transport into separate class
//								and improved constructor interface.
//								Baud rates are tested before being
//								offered. Added 1382400 as a supported
//								baud rate. Added interface to underlying
//								transport, and implemented automatic
//								baud rate and protocol identification.
//		1.3		12:Oct:00	at	DFU request used to test private
//								channels rather than BCCMD.
//		1.4		17:Oct:00	at	Added delay after resetting device.
//		1.5		19:Oct:00	at	Reduced reset delay after improving
//								speed of bc01 CRC calculation.
//		1.6		24:Oct:00	at	Updated based on changes to base class.
//		1.7		27:Oct:00	at	Started adding support for BCSP without
//								link establishment.
//		1.8		02:Nov:00	at	Moved post-reset delay to the transport
//								independent base class.
//		1.9		02:Nov:00	at	Added 14400 baud to list of supported
//								baud rates.
//		1.10	08:Nov:00	at	Maximum message length set based on
//								DFU functional descriptor.
//		1.11	16:Nov:00	at	Changed BCSP link establishment
//								detection to use overlapped reading.
//		1.12	16:Nov:00	at	COM port only opened and closed once.
//		1.13	21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.14	24:Nov:00	at	GetOverlappedResult used in
//								non-blocking mode.
//		1.20	23:Feb:01	at	Improved result code returned for
//								failure to connect. Added support for
//								aborting initial connection.
//		1.15	24:Nov:00	at	Serial ports enumerated from registry
//								if possible, and flushed before
//								looking for BCSP link establishment.
//		1.16	27:Nov:00	at	Corrected handling of serial port
//								timeout failure.
//		1.17	29:Nov:00	at	BCSP link establishment allowed to
//								take longer if baud rate known.
//		1.18	30:Nov:00	at	Removed redundant abort event
//								parameter from constructors.
//		1.19	08:Dec:00	at	Added support for passive or disabled
//								BCSP link establishment.
//		1.21	06:Mar:01	at	Added support for activity description.
//		1.22	06:Mar:01	at	Removed support for 14400 baud.
//		1.23	24:Apr:01	at	Restored support for 14400 baud.
//		1.24	30:Apr:01	ckl	Added Windows CE support.
//		1.25	23:May:01	at	Tidied up Windows CE support.
//		1.26	23:May:01	at	Corrected Windows CE support.
//		1.27	23:Jul:01	at	Added version string.
//		1.28	14:Nov:01	at	Added basic Unicode support.
//		1.29	27:Nov:01	at	Buffer deleted using correct type.
//		1.30	08:Mar:02	at	Sorted list of COM ports from registry.
//		1.31	06:Aug:02	ckl	Modified port enumeration on Windows CE.
//		1.32	07:Aug:02	at	Tidied up Windows CE support.
//

//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFURequestsCOM.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFURequestsCOM.h"

// Visual C++ debugging code
#ifdef _DEBUG
#include <crtdbg.h>
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define new DEBUG_NEW
#endif

// Version string
const char *dfurequestscom_id = "$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFURequestsCOM.cpp#1 $ Copyright (C) 2000-2006 Cambridge Silicon Radio Limited";

// Range of COM port numbers
static const int firstPort = 1;
#if !defined _WINCE && !defined _WIN32_WCE
static const int lastPort = 128;
#else
static const int lastPort = 20;
#endif

// COM port device names
#if !defined _WINCE && !defined _WIN32_WCE
static const TCHAR deviceFormat[] = _T("COM%d");
#else
static const TCHAR deviceFormat[] = _T("COM%d:");
#endif

// Standard baud rates
static const int standardBaudRates[] = {9600, 14400, 19200, 38400, 57600, 115200, 230400, 460800, 921600, 1382400};

// COM port settings
static const int comDataBits = 8;
static const int comStopBits = ONESTOPBIT;
static const int comParity = EVENPARITY;
static const uint32 comHandshakeMilliseconds = 100;
static const uint32 comReceiveMilliseconds = 500;
static const uint32 comReceiveMillisecondsSingle = 2000;
static const uint32 comInitialiseMilliseconds = 1300;

// BCSP sync message
static const uint8 bcspSync[] = {0xDA, 0xDC, 0xED, 0xED};

// USB control request status
enum usbStatus
{
	ack = 0,				// Completed successfully
	nak = 1,				// Still processing command (no data)
	stall = 2				// Error prevented completion (no data)
};

// Vendor specific DFU control requests for UART use only
static const uint16 dfuReset = 7;
static const uint16 dfuGetDevice = 8;
static const uint16 dfuGetFunct = 9;

// Fixed DFU interface number
static const uint16 interfaceNumber = 0;

// Maximum number of connection retry attempts
static const int retriesConnect = 3;

class DFURequestsCOMImpl
{
public:
	DFURequestsCOMImpl();

	DFURequestsCOM *parent;

	// The port settings
	CStringX port;
	DFURequestsCOM::BaudRateList *baud;	// this list is highest rate first.
	DFURequestsCOM::Protocol protocol;
	DFURequestsCOM::LinkEstablishment sync;

	// The COM port
	Device com;

	// The current transport
	DFUTransportCOM transport;
};

// Constructors
DFURequestsCOM::DFURequestsCOM(const TCHAR *port) : impl_(new DFURequestsCOMImpl)
{
	impl_->parent = this;

	// Store the communications settings
	impl_->port = Device::Canonicalise(port);
	CStringListX ports;
	ports.AddTail(port);
	EnumerateBaudRates(ports, impl_->baud);
	impl_->protocol = protocol_unknown;
	impl_->sync = sync_unknown;
}

DFURequestsCOM::DFURequestsCOM(const TCHAR *port, int baud) : impl_(new DFURequestsCOMImpl)
{
	impl_->parent = this;

	// Store the communications settings
	impl_->port = Device::Canonicalise(port);

	DFURequestsCOM::BaudRateList *entry = new DFURequestsCOM::BaudRateList;
	entry->baud = baud;
	entry->next = impl_->baud;
	impl_->baud = entry;

	impl_->protocol = protocol_h4;
	impl_->sync = sync_unknown;
}

DFURequestsCOM::DFURequestsCOM(const TCHAR *port, int baud, bool tunnel,
                               DFUEngine::Link link) : impl_(new DFURequestsCOMImpl)
{
	impl_->parent = this;

	// Store the communications settings
	impl_->port = Device::Canonicalise(port);

	DFURequestsCOM::BaudRateList *entry = new DFURequestsCOM::BaudRateList;
	entry->baud = baud;
	entry->next = impl_->baud;
	impl_->baud = entry;

	impl_->protocol = tunnel ? protocol_bcsp_tunnel : protocol_bcsp;
	impl_->sync = link == DFUEngine::link_passive
	            ? sync_passive
	            : (link == DFUEngine::link_disabled ? sync_disabled : sync_enabled);
}

DFURequestsCOMImpl::DFURequestsCOMImpl() : baud(0)
{
}

// Destructor
DFURequestsCOM::~DFURequestsCOM()
{
	FreeBaudRateList(impl_->baud);
}

// Non USB DFU control requests (implemented here as USB control requests)
DFUEngine::Result DFURequestsCOM::RPCReset()
{
	// Prepare the setup data
	Setup setup;
	setup.bmRequestTypeRecipient = recipient_interface;
	setup.bmRequestTypeType = type_vendor;
	setup.bmRequestTypeDirection = dir_host_to_device;
	setup.bRequest = dfuReset;
	setup.wValue = 0;
	setup.wIndex = interfaceNumber;
	setup.wLength = 0;

	// Perform the request (failure to receive a reply is not a failure)
	DFUEngine::Result result = ControlRequest(setup);
	if (!result && (result.GetCode() != DFUEngine::fail_com_timeout_rx))
	{
		return result;
	}

	// Need to disconnect the transport before the device has reset
	result = Disconnect();
	if (!result) return result;

	// Successful if this point reached
	return DFUEngine::success;
}

DFUEngine::Result DFURequestsCOM::RPCGetInterfaceDFU(InterfaceDescriptor &descriptor)
{
	descriptor.bLength = sizeof(InterfaceDescriptor);
	descriptor.bDescriptorType = DFURequests::descriptor_interface;
	// Interface number is constant for UART transports
	descriptor.bInterfaceNumber = interfaceNumber;
	descriptor.bAlternateSetting = 0;
	descriptor.bNumEndpoints = 0;
	descriptor.bInterfaceClass = dfuInterfaceClass;
	descriptor.bInterfaceSubClass = dfuInterfaceSubClass;
	descriptor.bInterfaceProtocol = dfu10InterfaceProtocol;
	descriptor.iInterface = 0;
	return DFUEngine::success;
}

DFUEngine::Result DFURequestsCOM::RPCGetDevice(DeviceDescriptor &descriptor)
{
	// Buffer to receive the reply
	struct
	{
		uint16 bDeviceClass;
		uint16 bDeviceSubClass;
		uint16 bDeviceProtocol;
		uint16 idVendor;
		uint16 idProduct;
		uint16 bcdDevice;
	} reply;

	// Prepare the setup data
	Setup setup;
	setup.bmRequestTypeRecipient = recipient_interface;
	setup.bmRequestTypeType = type_vendor;
	setup.bmRequestTypeDirection = dir_device_to_host;
	setup.bRequest = dfuGetDevice;
	setup.wValue = 0;
	setup.wIndex = interfaceNumber;
	setup.wLength = sizeof(reply);

	// Perform the request
	uint16 replyLength;
	DFUEngine::Result result = ControlRequest(setup, &reply, sizeof(reply), &replyLength);

	// Extract the data from the reply
	if (result && (replyLength < sizeof(reply))) result = DFUEngine::fail_reply_short;
	if (result)
	{
		descriptor.bLength = sizeof(DeviceDescriptor);
		descriptor.bDescriptorType = descriptor_device;
		descriptor.bDeviceClass = uint8(reply.bDeviceClass);
		descriptor.bDeviceSubClass = uint8(reply.bDeviceSubClass);
		descriptor.bDeviceProtocol = uint8(reply.bDeviceProtocol);
		descriptor.idVendor = reply.idVendor;
		descriptor.idProduct = reply.idProduct;
		descriptor.bcdDevice = reply.bcdDevice;
	}

	// Return the result
	return result;
}

DFUEngine::Result DFURequestsCOM::RPCGetFunct(DFUFunctionalDescriptor &descriptor)
{
	// Buffer to receive the reply
	struct
	{
		uint16 bmAttributes;
		uint16 wDetachTimeout;
		uint16 wTransferSize;
	} reply;

	// Prepare the setup data
	Setup setup;
	setup.bmRequestTypeRecipient = recipient_interface;
	setup.bmRequestTypeType = type_vendor;
	setup.bmRequestTypeDirection = dir_device_to_host;
	setup.bRequest = dfuGetFunct;
	setup.wValue = 0;
	setup.wIndex = interfaceNumber;
	setup.wLength = sizeof(reply);

	// Perform the request
	uint16 replyLength;
	DFUEngine::Result result = ControlRequest(setup, &reply, sizeof(reply), &replyLength);

	// Extract the data from the reply
	if (result && (replyLength < sizeof(reply))) result = DFUEngine::fail_reply_short;
	if (result)
	{
		descriptor.bLength = sizeof(DFUFunctionalDescriptor);
		descriptor.bDescriptorType = descriptor_dfu_functional;
		descriptor.bmAttributes = uint8(reply.bmAttributes);
		descriptor.wDetachTimeout = reply.wDetachTimeout;
		descriptor.wTransferSize = reply.wTransferSize;
		descriptor.bcdDFUVersion = DFU_VERSION_1_0;
		impl_->transport.MaximumLength(sizeof(Setup) + reply.wDetachTimeout);
	}

	// Return the result
	return result;
}

DFUEngine::Result DFURequestsCOM::RPCGetString(uint8 index, CStringX &descriptor)
{
	return DFUEngine::Result(DFUEngine::fail_unimplemented, _T("DFURequestsCOM::RPCGetString()"));
}

// A request that safely tests private channels
DFUEngine::Result DFURequestsCOM::TestPrivateChannel()
{
	DeviceDescriptor descriptor;
	return RPCGetDevice(descriptor);
}

// Automatic protocol detection
DFUEngine::Result DFURequestsCOM::AutomaticPassiveBCSP()
{
	// Ensure that any previous transport is disconnected
	impl_->transport.Disconnect();

	// Purge any operations in progress
	if (!PurgeComm(impl_->com, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR))
	{
		return DFUEngine::fail_os;
	}

	// Read the current COM port settings
	DCB dcb;
	COMMTIMEOUTS timeouts;
	if (!GetCommState(impl_->com, &dcb) || !GetCommTimeouts(impl_->com, &timeouts))
	{
		return DFUEngine::fail_os;
	}
	
	// Set the basic COM port configuration
	dcb.fBinary = true;
	dcb.fParity = comParity != NOPARITY;
	dcb.fOutxCtsFlow = true;
	dcb.fOutxDsrFlow = false;
	dcb.fDsrSensitivity = false;
	dcb.fOutX = false;
	dcb.fInX = false;
	dcb.fErrorChar = false;
	dcb.fNull = false;
	dcb.fAbortOnError = false;
	dcb.ByteSize = comDataBits;
	dcb.Parity = comParity;
	dcb.StopBits = comStopBits;
	if (!SetCommState(impl_->com, &dcb))
	{
		return DFUEngine::fail_os;
	}

	// Set the timeouts for reading
	uint32 timeout = impl_->baud && impl_->baud->next
		             ? comReceiveMilliseconds
					 : comReceiveMillisecondsSingle;
	timeouts.ReadIntervalTimeout = MAXDWORD;
	timeouts.ReadTotalTimeoutConstant = timeout;
	timeouts.ReadTotalTimeoutMultiplier = MAXDWORD;
	if (!SetCommTimeouts(impl_->com, &timeouts))
	{
		return DFUEngine::fail_os;
	}

	// Attempt to reset the connection using the handshaking lines
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	if (!SetCommState(impl_->com, &dcb))
	{
		return DFUEngine::fail_os;
	}
	Sleep(comHandshakeMilliseconds);
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
	if (!SetCommState(impl_->com, &dcb))
	{
		return DFUEngine::fail_os;
	}

	// Loop through each of the baud rates
	int bufferLength = 2 * sizeof(bcspSync);
	SmartPtr<uint8, true> buffer(new uint8[bufferLength]);
	bool connected = false;
	for (BaudRateList *pos = impl_->baud;
	     !connected && pos; pos = pos->next)
	{
		// Check for an abort request
		DFUEngine::Result result = CheckAbort();
		if (!result) return result;

		// Configure the port for this baud rate
		int baudRate = pos->baud;
		Progress(DFUEngine::transport_bcsp_passive_le, baudRate);
		dcb.BaudRate = baudRate;
		if (!SetCommState(impl_->com, &dcb))
		{
			return DFUEngine::fail_os;
		}

		// Look for the BCSP sync message
		int bufferUsed = 0;
		uint32 endTick = GetTickCount() + timeout;
		while (!connected && (int32(GetTickCount() - endTick) < 0))
		{
			// Ensure that there is sufficient space in the buffer
			DWORD read;
			int bufferLeave = sizeof(bcspSync) - 1;
			if (bufferLeave < bufferUsed)
			{
				// Shuffle the existing contents down
				memmove(buffer, buffer + bufferUsed - bufferLeave,
				        bufferLeave);
				bufferUsed = bufferLeave;
			}

			// Read some more data from the serial port
#if !defined _WINCE && !defined _WIN32_WCE
			OVERLAPPED overlapped;
			overlapped.Offset = 0;
			overlapped.OffsetHigh = 0;
			HANDLE event = CreateEvent(NULL, TRUE, FALSE, NULL);
			if(!event) return DFUEngine::fail_os;

			overlapped.hEvent = event;
			if (!ReadFile(impl_->com, buffer + bufferUsed,
			              bufferLength - bufferUsed, &read, &overlapped)
				&& (GetLastError() != ERROR_IO_PENDING))
			{
				CloseHandle(event);
				return DFUEngine::fail_os;
			}

			// Wait for the operation to complete
			WaitForSingleObject(event, timeout);
			CloseHandle(event);

			// Get the result
			if (!GetOverlappedResult(impl_->com, &overlapped, &read, false))
			{
				return DFUEngine::fail_os;
			}
#else
			// Set the serial port timeouts to return immediately
			COMMTIMEOUTS timeouts;
			memset(&timeouts, 0, sizeof(timeouts));
			timeouts.ReadIntervalTimeout = MAXDWORD;
			timeouts.ReadTotalTimeoutConstant = 0;
			timeouts.ReadTotalTimeoutMultiplier = 0;
			timeouts.WriteTotalTimeoutMultiplier = 0;
			timeouts.WriteTotalTimeoutConstant = 0;
			if (!SetCommTimeouts(impl_->com, &timeouts))
			{
				return DFUEngine::fail_os;
			}

			// Read any waiting data
			if (!ReadFile(impl_->com, buffer + bufferUsed,
				bufferLength - bufferUsed, &read, 0))
			{
				return DFUEngine::fail_os;
			}
#endif

			// Update the buffer position
			bufferUsed += read;

			// Check if the BCSP sync message has been received
			for (int offset = 0;
			     !connected && (offset + int(sizeof(bcspSync)) <= bufferUsed);
				 ++offset)
			{
				connected = !memcmp(buffer + offset,
				                    bcspSync, sizeof(bcspSync));
			}
		}

		// Store the baud rate and link establishment if successful
		if (connected)
		{
			FreeBaudRateList(impl_->baud);
			impl_->baud = new BaudRateList;
			impl_->baud->next = 0;
			impl_->baud->baud = baudRate;

			impl_->sync = sync_enabled;
		}
	}

	// Return an error if failed to connect
	if (!connected) return DFUEngine::fail_com_connect;

	// Attempt to start the actual transport
	DFUEngine::Result result = impl_->transport.ConnectBCSP(impl_->port, impl_->baud->baud, true, false, impl_->com);
	if (!result) return result;

	// Check if private channels work
	result = TestPrivateChannel();
	if (!result)
	{
		// Try tunnelling if appropriate
		if ((impl_->protocol == protocol_unknown)
			|| (impl_->protocol == protocol_bcsp_tunnel))
		{
			// Try again with tunnelling
            result = impl_->transport.ConnectBCSP(impl_->port, impl_->baud->baud, true, true, impl_->com);
            if (result)
                result = TestPrivateChannel();
			if (!result) return result;

			// Store the protocol if successful
			impl_->protocol = protocol_bcsp_tunnel;
		}
		else return result;
	}

	// Successful if this point reached
	return DFUEngine::success;
}

DFUEngine::Result DFURequestsCOM::AutomaticActiveBCSP(bool link)
{
	DFUEngine::Result result = DFUEngine::fail_com_connect;

	// Try all of the available baud rates
	for (BaudRateList *pos = impl_->baud;
	     !result && pos; pos = pos->next)
	{
		// Check for an abort request
		result = CheckAbort();
		if (!result) return result;

		// Attempt a connection at this baud rate
		int baudRate = pos->baud;
		Progress(link ? DFUEngine::transport_bcsp_active_le : DFUEngine::transport_bcsp_no_le, baudRate);
		result = impl_->transport.ConnectBCSP(impl_->port, baudRate, link, false, impl_->com);
		if (result)
		{
			result = TestPrivateChannel();
			if (!result)
			{
				// Try tunnelling if appropriate
				if (result.GetCode() == DFUEngine::fail_com_timeout_tx)
				{
					// Give a more sensible error if failed
					result = DFUEngine::fail_com_connect;
				}
				else if ((impl_->protocol == protocol_unknown)
					     || (impl_->protocol == protocol_bcsp_tunnel))
				{
					// Try again with tunnelling
		            result = impl_->transport.ConnectBCSP(impl_->port, baudRate, link, true, impl_->com);
                    if ( result )
                        result = TestPrivateChannel();

					// Store the protocol if successful
					if (result) impl_->protocol = protocol_bcsp_tunnel;
				}
			}

		}

		// Store the baud rate and link establishment if successful
		if (result)
		{
			FreeBaudRateList(impl_->baud);
			impl_->baud = new BaudRateList;
			impl_->baud->next = 0;
			impl_->baud->baud = baudRate;
			impl_->sync = link ? sync_passive : sync_disabled;
		}
	}

	// Return the result
	return result;
}

#if !defined _WINCE && !defined _WIN32_WCE

DFUEngine::Result DFURequestsCOM::AutomaticH4()
{
	DFUEngine::Result result = DFUEngine::fail_com_connect;

	// Try all of the available baud rates
	for (BaudRateList *pos = impl_->baud;
	     !result && pos; pos = pos->next)
	{
		// Check for an abort request
		result = CheckAbort();
		if (!result) return result;

		// Attempt a connection at this baud rate
		int baudRate = pos->baud;
		Progress(DFUEngine::transport_h4, baudRate);
		result = impl_->transport.ConnectH4(impl_->port, baudRate, impl_->com);
		if (result)
		{
			result = TestPrivateChannel();
			if (!result) result = impl_->transport.HCIReset();
			if ((result.GetCode() == DFUEngine::fail_com_timeout_rx)
				|| (result.GetCode() == DFUEngine::fail_com_fail))
			{
				result = DFUEngine::fail_com_connect;
			}
		}

		// Store the baud rate and protocol if successful
		if (result)
		{
			FreeBaudRateList(impl_->baud);
			impl_->baud = new BaudRateList;
			impl_->baud->next = NULL;
			impl_->baud->baud = baudRate;
			impl_->protocol = protocol_h4;
		}
	}

	// Return the result
	return result;
}

#endif
	
// Connection and disconnection
DFUEngine::Result DFURequestsCOM::RPCConnect(bool hintDFU)
{
	// Fail immediately if failed previously
	if (impl_->protocol == protocol_failed) return DFUEngine::fail_com_connect;

	// Ensure that the COM port has been opened
	DFUEngine::Result result;
	if (!bool(impl_->com))
	{
		result = impl_->com.Open(impl_->port);
		if (result) Sleep(comInitialiseMilliseconds);
	}
	if (!result || !bool(impl_->com))
	{
		impl_->protocol = protocol_failed;
		return DFUEngine::Result(DFUEngine::fail_com_open, impl_->port);
	}

	// Only try H4 first if protocol known and not likely to be DFU
	result = DFUEngine::fail_com_connect;
#if !defined _WINCE && !defined _WIN32_WCE
	if (!hintDFU && (impl_->protocol == protocol_h4)) result = AutomaticH4();
#endif

	// Try passive BCSP link establishment once only
	if (!result && ((impl_->sync == sync_unknown) || (impl_->sync == sync_enabled)))
	{
		result = AutomaticPassiveBCSP();
	}

	// Retry connecting until successful or give up
	for (int retries = 0;
	     !result && (retries < retriesConnect);
		 ++retries)
	{
		// Try BCSP with active link establishment
		if (impl_->sync != sync_disabled) result = AutomaticActiveBCSP(true);

		// Try H4 again if protocol not already known to be BCSP
#if !defined _WINCE && !defined _WIN32_WCE
		if (!result && (impl_->protocol != protocol_bcsp)
			&& (impl_->protocol != protocol_bcsp_tunnel))

		{
			result = AutomaticH4();
		}
#endif

		// Try BCSP without link establishment if still not connected
		if (!result && ((impl_->sync == sync_unknown) || (impl_->sync == sync_disabled)))
		{
			result = AutomaticActiveBCSP(false);
		}
	}

	// Return the last error if failed after retries
	if (!result)
	{
		impl_->protocol = protocol_failed;
		return result;
	}

	// Successful if this point reached
	Progress(DFUEngine::transport_established);
	return DFUEngine::success;
}

DFUEngine::Result DFURequestsCOM::RPCDisconnect()
{
	return impl_->transport.Disconnect();
}

// Generic control requests
DFUEngine::Result DFURequestsCOM::ControlRequest(const Setup &setup,
                                                 void *buffer,
                                                 uint16 bufferLength,
                                                 uint16 *replyLength)
{
	return impl_->transport.ControlRequest(setup, buffer, bufferLength, replyLength);
}

// COM port and baud rate enumeration
int DFURequestsCOM::EnumeratePorts(CStringListX &ports)
{
#if !defined _WINCE && !defined _WIN32_WCE
	// Try to read the COM ports from the registry first
	ports.RemoveAll();
	HKEY handle;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DEVICEMAP\\SERIALCOMM"),
		             0, KEY_ENUMERATE_SUB_KEYS | KEY_EXECUTE | KEY_QUERY_VALUE | KEY_READ,
					 &handle)
		== ERROR_SUCCESS)
	{
		// Determine the maximum buffer sizes required
		DWORD maxNameLength;
		DWORD maxDataLength;
		if (RegQueryInfoKey(handle, 0, 0, 0, 0, 0, 0, 0,
			                &maxNameLength, &maxDataLength, 0, 0)
			== ERROR_SUCCESS)
		{
			// Allocate buffers to receive the key data
			SmartPtr<TCHAR, true> nameBuffer(new TCHAR[++maxNameLength]);
			SmartPtr<BYTE, true> dataBuffer(new BYTE[maxDataLength]);
			
			// Enumerate the values
			DWORD index = 0;
			DWORD type;
			DWORD nameBufferLength = maxNameLength;
			DWORD dataBufferLength = maxDataLength;
			while (RegEnumValue(handle, index++,
				                nameBuffer, &nameBufferLength, 0, &type,
								(BYTE *) dataBuffer, &dataBufferLength)
				   == ERROR_SUCCESS)
			{
				// Add this port if the type is correct
				if (type == REG_SZ) ports.AddTail((TCHAR *) (BYTE *) dataBuffer);

				// Restore the buffer lengths
				nameBufferLength = maxNameLength;
				dataBufferLength = maxDataLength;
			}
		}

		// Close the registry
		RegCloseKey(handle);
	}

	// If any ports found then check if they can be opened
	if (!ports.IsEmpty())
	{
		// Try to open all of the ports
		for (CStringListX::POSITION port = ports.GetHeadPosition(); port;)
		{
			CStringListX::POSITION portPos = port;
			if (!bool(Device(ports.GetNext(port))))
			{
				ports.RemoveAt(portPos);
			}
		}

		// Sort the list and return the number of ports found
		return Device::Sort(deviceFormat, firstPort, lastPort, ports);
	}
#endif

	// Try opening all possible COM ports
	return Device::Enumerate(deviceFormat, firstPort, lastPort, ports);
}

int DFURequestsCOM::EnumerateBaudRates(BaudRateList *&baudRates)
{
	// Generate list of baud rates for all ports
	CStringListX ports;
	EnumeratePorts(ports);
	return EnumerateBaudRates(ports, baudRates);
}

int DFURequestsCOM::EnumerateBaudRates(const CStringListX &ports, BaudRateList *&baudRates)
{
	// Construct list of standard baud rates
	unsigned remaining = sizeof(standardBaudRates) / sizeof(*standardBaudRates);
	SmartPtr<bool, true> rates(new bool[remaining]);
	unsigned index;

	for (index = 0; index < remaining; ++index) rates[index]=false;

	// Try all of the standard rates on each of the ports specified
	for (CStringListX::POSITION posPort = ports.GetHeadPosition();
	     (posPort != 0) && remaining;)
	{
		// Try selecting each of the standard baud rates in turn
		DCB dcb;
		Device com(ports.GetNext(posPort));
		if (bool(com) && GetCommState(com, &dcb))
		{
			for (index = 0; index < sizeof(standardBaudRates) / sizeof(*standardBaudRates); ++index)
			{
				if(!rates[index])
				{
					dcb.BaudRate = standardBaudRates[index];
					if (SetCommState(com, &dcb))
					{
						// it worked. mark it as good.
						rates[index] = true;
						--remaining;
					}
				}
			}
		}
	}

	FreeBaudRateList(baudRates);
	baudRates = 0;
	for(index = 0; index < sizeof(standardBaudRates) / sizeof(*standardBaudRates); ++index)
	{
		if(rates[index])
		{
			BaudRateList *entry = new BaudRateList;
			entry->baud = standardBaudRates[index];
			entry->next = baudRates;
			baudRates = entry;
		}
	}

	// Return the number of available baud rates
	return sizeof(standardBaudRates) / sizeof(*standardBaudRates) - remaining;
}

void DFURequestsCOM::FreeBaudRateList(BaudRateList *list)
{
	if(list)
	{
		FreeBaudRateList(list->next);
		delete list;
	}
}

// Set the abort event object
void DFURequestsCOM::SetAbort(DFUInterface::Event *abort)
{
	// Pass on to the base class
	DFURequests::SetAbort(abort);

	// Notify the tranport
	impl_->transport.SetAbort(abort);
}

// Enable or disable HID
DFUEngine::Result DFURequestsCOM::RPCEnableHidMode(bool enable)
{
	return DFUEngine::fail_unimplemented;
}
