///////////////////////////////////////////////////////////////////////
//
//	File	: DFUTransportCOM.cpp
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFUTransportCOM
//
//	Purpose	: Interface to the raw COM port transports. This also
//			  provides limited support for USB transports by treating
//			  them as a special case of a COM port.
//
//			  The constructor should be passed an event object that
//			  can be used to abort operations in progress at any time.
//			  This would typically be used to allow a thread to
//			  terminate normally instead of killing the thread.
//
//			  The following member functions are used to start a
//			  transport with particular settings:
//				ConnectH4		- Start an H4 transport using the
//								  specified COM port and baud rate.
//								  An already opened and configured
//								  port may optionally be used.
//				ConnectBCSP		- Start a BCSP transport using the
//								  specified COM port, baud rate,
//								  and link establishment, with
//								  tunnelling disabled by default.
//								  An already opened and configured
//								  port may optionally be used.
//				ConnectUSBCSR	- Start a USB transport for the
//								  specified CSR device driver.
//								  An already opened USB device
//								  driver may optionally be used.
//				ConnectUSBBTW	- Start a USB transport for the
//								  specified Widcomm device driver.
//								  An already opened USB device
//								  driver may optionally be used.
//
//			  Regardless of how the transport is started, a single
//			  member function is used to stop it:
//				Disconnect		- Stop any active transport.
//
//			  This function is automatically called if an attempt is
//			  made to start a new connection, or when this object is
//			  destroyed.
//
//			  Tunnelling for BCSP connections can be changed at any
//			  time using:
//				SetTunnel		- Enable or disable tunnelling of HCI
//								  extensions.
//
//			  Generic DFU control requests can be performed using
//			  the following member function:
//				ControlRequest	- Perform a generic DFU request.
//
//			  The following member functions allow specific HCI
//			  commands to be performed and the corresponding event
//			  received:
//				HCIReset		- Perform an HCI reset.
//				HCIReadBDADDR	- Read the Bluetooth address of the
//								  device.
//
//			  Generic HCI commands can be performed using the
//			  following member function:
//				HCIRequest		- Perform an HCI command and receive
//								  the corresponding Command Complete
//								  or Command Status event.
//
//			  The following member functions allow specific BCCMD
//			  operations to be performed:
//				BCCMDGet		- Overloaded member function to get
//								  the value of a BCCMD variable.
//				BCCMDGetPS		- Overloaded member function to get
//								  the value of a persistent store
//								  key.
//
//			  Generic BCCMD commands can be performed using the
//			  following member function:
//				BCCMDRequest	- Perform a BCCMD GETREQ or SETREQ
//								  and receive the corresponding
//								  GETRESP.
//
//			  The following utility functions may also be useful:
//				MaximumLength	- Set the retry timeouts based on the
//								  specified maximum expected message
//								  length.
//				Timeout			- Calculate a timeout period in
//								  milliseconds based on the current
//								  baud rate.
//				MapHCIStatus	- Convert an HCI status code into a
//								  DFUEngine results code.
//				MapBCCMDStatus	- Convert a BCCMD status code into a
//								  DFUEngine results code.
//
//	Modification history:
//	
//		1.1		06:Oct:00	at	File created.
//		1.2		16:Oct:00	at	Corrected memory management of PDU
//								buffers and removed trace macros.
//		1.3		18:Oct:00	at	Failure to receive a response is no
//								longer an automatic link failure.
//		1.4		20:Oct:00	at	Added more rigorous tests for the
//								reply length of control requests.
//		1.5		24:Oct:00	at	Added support for changing abort event
//								after construction.
//		1.6		25:Oct:00	at	Modified timeout behaviour to cope
//								with actual BCSP operation.
//		1.7		26:Oct:00	at	Added ability to disable BCSP link
//								establishment and adjust timeouts.
//		1.8		07:Nov:00	at	Improved timeout behaviour.
//		1.9		08:Nov:00	at	Modified timeout behaviour to work
//								around peculiarities of the BCSP
//								stack.
//		1.10	08:Nov:00	at	Timeouts corrected for H4 connections.
//								Transfer status reset when HCI reset
//								performed.
//		1.11	09:Nov:00	at	Transfer status of private channels
//								reset when tunnelling enabled or
//								disabled.
//		1.12	10:Nov:00	at	Abort event pointer initialised in
//								constructor.
//		1.13	16:Nov:00	at	Added support for already opened
//								and configured devices.
//		1.14	21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.15	24:Nov:00	at	Serial port purged before transport
//								started.
//		1.16	27:Nov:00	at	Timeout cleared when new transport
//								started.
//		1.17	08:Dec:00	at	Corrected initial BCSP timeout
//								behaviour to allow retries.
//		1.18	23:Mar:01	at	Added sub-directory to HCI includes.
//		1.19	30:Apr:01	ckl	Added Windows CE support.
//		1.20	23:May:01	at	Tidied Windows CE support.
//		1.21	23:May:01	at	Corrected Windows CE support.
//		1.22	23:May:01	ckl	Added more CE support.
//		1.23	23:Jul:01	at	Added version string.
//		1.24	05:Oct:01	at	Corrected version history.
//		1.25	05:Oct:01	at	Added support for Widcomm USB driver.
//		1.26	14:Nov:01	at	Added basic Unicode support.
//		1.27	09:Jan:02	at	Added BCCMD varid for build identifier.
//		1.28	11:Oct:02	doc	Fixed operation on WindowsCE.
//		1.29	12:Oct:02	at	Updated revision history.
//		1.30	04:Nov:02	at	Added workaround for Windows xP SP1.
//		1.31	04:Nov:02	at	Improved workaround for Windows xP SP1.
//		1.32	06:Nov:02	at	Fixed workaround with Widcomm driver.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUTransportCOM.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUTransportCOM.h"
#include "csrhci/transportconfiguration.h"
#include <thread/signal_box.h>
#include <thread/critical_section.h>

// Visual C++ debugging code
#ifdef _DEBUG
#include <crtdbg.h>
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define new DEBUG_NEW
#endif

// Version string
const char *dfutransportcom_id = "$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUTransportCOM.cpp#1 $ Copyright (C) 2000-2006 Cambridge Silicon Radio Limited";

class DFUTransportCOMImpl
{
public:
	DFUTransportCOMImpl();

	DFUTransportCOM *parent;

	// Abort event
	DFUInterface::Event *eventAbort;

	// The current transport
	int baudRate;
	SmartPtr<TransportConfiguration> config;
	SmartPtr<BlueCoreDeviceController_newStyle> transport;
	DFUInterface::Event eventFailTransport;

	// Pending HCI requests
	DFUInterface::Event eventHCIReceive;
	HCICommandPDU requestHCI;
	HCIEventPDU bufferHCI;
	CriticalSection lockHCI;

	// Pending BCCMD requests
	DFUInterface::Event eventBCCMDReceive;
    BCCMD_PDU requestBCCMD;
    BCCMD_PDU bufferBCCMD;
	CriticalSection lockBCCMD;

	// Pending DFU requests
	DFUInterface::Event eventUpgradeReceive;
	PDU requestUpgrade;
	PDU bufferUpgrade;
	CriticalSection lockUpgrade;

	// Maximum message length and associate timeout
	uint16 maximumLength;
	uint32 maximumLengthTimeout;

	// Wait for events
	DFUEngine::Result Wait(DFUInterface::Event *event);

	// Additional timeout period for next operation
	bool enabledTimeout;
	uint32 nextTimeout;
};

// Bits per byte (including framing) for timing purposes
static const int bitsPerByte = 11; // Includes protocol overheads

// Effective baud rate for USB
static const int usbBaudRate = 115200;

// Timeouts in bytes and milliseconds
static uint32 timeoutConnectionBytes = 640;
#if !defined _WINCE && !defined _WIN32_WCE
static uint32 timeoutConnectionMilliseconds = 2000;
static uint32 timeoutUpgradeTransmitMin = 300;
static uint32 timeoutUpgradeTransmitBase = 50;
static uint32 timeoutUpgradeReceiveMin = 750;
static uint32 timeoutUpgradeReceiveBase = 25;
#else
static uint32 timeoutConnectionMilliseconds = 8000;
static uint32 timeoutUpgradeTransmitMin = 3000;
static uint32 timeoutUpgradeTransmitBase = 1500;
static uint32 timeoutUpgradeReceiveMin = 7500;
static uint32 timeoutUpgradeReceiveBase = 750;
#endif
static uint32 timeoutHCITransmit = 250;
static uint32 timeoutHCIReceive = 250;
static uint32 timeoutBCCMDReceive = 250;
static uint32 timeoutMaximumLengthMin = 100;
static uint32 timeoutMaximumLengthBase = 25;

// Timeout manipulation
static uint32 timeoutRetryScale = 3;
static uint32 timeoutRetryOffset = 100;

// Initial maximum message length
static uint16 defaultMaximumLength = 32;

// Delay after starting transport
static uint32 delayConnection = 300;

// Minimum length of a BCCMD message
static uint16 minBCCMD = 18;

// offset of HCI command complete parameters within a PDU.
const int DFUTransportCOM::HCIDismemberCommandCompletePDU::offset_to_parameters = 6;

#if !defined _WINCE && !defined _WIN32_WCE
// COM port settings for H4
static const int comH4DataBits = 8;
static const int comH4Parity = NOPARITY;
static const int comH4StopBits = ONESTOPBIT;
static const int comH4RtsControl = RTS_CONTROL_HANDSHAKE;
#endif

// COM port settings for BCSP
static const int comBCSPDataBits = 8;
static const int comBCSPParity = EVENPARITY;
static const int comBCSPStopBits = ONESTOPBIT;
static const int comBCSPRtsControl = RTS_CONTROL_DISABLE;
static const char comBCSPErrorChar = '\xc0';

// Mapping of HCI status codes to DFUEngine result codes
struct statusHCICodeMapEntry
{
	uint8 status;
	DFUEngine::ResultCode result;
};
static const statusHCICodeMapEntry statusHCICodeMap[] =
{
	{DFUTransportCOM::hci_success,					DFUEngine::success},
	{DFUTransportCOM::hci_err_no_connection,		DFUEngine::fail_hci_no_connection},
	{DFUTransportCOM::hci_err_hardware_fail,		DFUEngine::fail_hci_hardware_fail},
	{DFUTransportCOM::hci_err_page_timeout,			DFUEngine::fail_hci_page_timeout},
	{DFUTransportCOM::hci_err_auth_fail,			DFUEngine::fail_hci_auth_fail},
	{DFUTransportCOM::hci_err_err_key_missing,		DFUEngine::fail_hci_err_key_missing},
	{DFUTransportCOM::hci_err_memory_full,			DFUEngine::fail_hci_memory_full},
	{DFUTransportCOM::hci_err_connect_timeout,		DFUEngine::fail_hci_connect_timeout},
	{DFUTransportCOM::hci_err_max_connect,			DFUEngine::fail_hci_max_connect},
	{DFUTransportCOM::hci_err_max_sco,				DFUEngine::fail_hci_max_sco},
	{DFUTransportCOM::hci_err_acl_exists,			DFUEngine::fail_hci_acl_exists},
	{DFUTransportCOM::hci_err_cmd_disallowed,		DFUEngine::fail_hci_cmd_disallowed},
	{DFUTransportCOM::hci_err_host_rej_resource,	DFUEngine::fail_hci_host_rej_resource},
	{DFUTransportCOM::hci_err_host_rej_security,	DFUEngine::fail_hci_host_rej_security},
	{DFUTransportCOM::hci_err_host_rej_personal,	DFUEngine::fail_hci_host_rej_personal},
	{DFUTransportCOM::hci_err_host_timeout,			DFUEngine::fail_hci_host_timeout},
	{DFUTransportCOM::hci_err_unsupported,			DFUEngine::fail_hci_unsupported},
	{DFUTransportCOM::hci_err_invalid_params,		DFUEngine::fail_hci_invalid_params},
	{DFUTransportCOM::hci_err_term_user,			DFUEngine::fail_hci_term_user},
	{DFUTransportCOM::hci_err_term_resource,		DFUEngine::fail_hci_term_resource},
	{DFUTransportCOM::hci_err_term_off,				DFUEngine::fail_hci_term_off},
	{DFUTransportCOM::hci_err_term_local,			DFUEngine::fail_hci_term_local},
	{DFUTransportCOM::hci_err_repeated,				DFUEngine::fail_hci_repeated},
	{DFUTransportCOM::hci_err_no_pairing,			DFUEngine::fail_hci_no_pairing},
	{DFUTransportCOM::hci_err_lmp_unknown,			DFUEngine::fail_hci_lmp_unknown},
	{DFUTransportCOM::hci_err_unsupported_rem,		DFUEngine::fail_hci_unsupported_rem},
	{DFUTransportCOM::hci_err_sco_offset,			DFUEngine::fail_hci_sco_offset},
	{DFUTransportCOM::hci_err_sco_interval,			DFUEngine::fail_hci_sco_interval},
	{DFUTransportCOM::hci_err_sco_air_mode,			DFUEngine::fail_hci_sco_air_mode},
	{DFUTransportCOM::hci_err_lmp_invalid,			DFUEngine::fail_hci_lmp_invalid},
	{DFUTransportCOM::hci_err_unspecified,			DFUEngine::fail_hci_unspecified},
	{DFUTransportCOM::hci_err_lmp_unsupported,		DFUEngine::fail_hci_lmp_unsupported},
	{DFUTransportCOM::hci_err_role_change,			DFUEngine::fail_hci_role_change},
	{DFUTransportCOM::hci_err_lmp_timeout,			DFUEngine::fail_hci_lmp_timeout},
	{DFUTransportCOM::hci_err_lmp_error,			DFUEngine::fail_hci_lmp_error},
	{DFUTransportCOM::hci_err_lmp_not_allowed,		DFUEngine::fail_hci_lmp_not_allowed}
};

// Mapping of BCCMD status codes to DFUEngine result codes
struct statusBCCMDCodeMapEntry
{
	uint16 status;
	DFUEngine::ResultCode result;
};
static const statusBCCMDCodeMapEntry statusBCCMDCodeMap[] =
{
	{DFUTransportCOM::bccmd_ok,						DFUEngine::success},
	{DFUTransportCOM::bccmd_no_such_varid,			DFUEngine::fail_bccmd_no_such_varid},
	{DFUTransportCOM::bccmd_too_big,				DFUEngine::fail_bccmd_too_big},
	{DFUTransportCOM::bccmd_no_value,				DFUEngine::fail_bccmd_no_value},
	{DFUTransportCOM::bccmd_bad_req,				DFUEngine::fail_bccmd_bad_req},
	{DFUTransportCOM::bccmd_no_access,				DFUEngine::fail_bccmd_no_access},
	{DFUTransportCOM::bccmd_read_only,				DFUEngine::fail_bccmd_read_only},
	{DFUTransportCOM::bccmd_write_only,				DFUEngine::fail_bccmd_write_only},
	{DFUTransportCOM::bccmd_error,					DFUEngine::fail_bccmd_error},
	{DFUTransportCOM::bccmd_permission_denied,		DFUEngine::fail_bccmd_permission_denied}
};

// Constructor
DFUTransportCOM::DFUTransportCOM() : impl_(new DFUTransportCOMImpl)
{
	impl_->parent = this;
}

DFUTransportCOMImpl::DFUTransportCOMImpl()
: requestBCCMD(null_pdu),
  bufferBCCMD(null_pdu),
  requestUpgrade(null_pdu),
  bufferUpgrade(null_pdu),
  requestHCI(null_pdu),
  bufferHCI(null_pdu)
{
	eventFailTransport.Set();
	eventHCIReceive.Set();
	eventBCCMDReceive.Set();
	eventUpgradeReceive.Set();
	eventAbort = 0;
}

// Destructor
DFUTransportCOM::~DFUTransportCOM()
{
	Disconnect();
}

// Start transport
#if !defined _WINCE && !defined _WIN32_WCE
DFUEngine::Result DFUTransportCOM::ConnectH4(const TCHAR *port,
											 int baud, HANDLE handle)
{
	// End any existing transport
	DFUEngine::Result result = Disconnect();
	if (!result) return result;

	// Start an H4 transport
	H4Configuration *config;
	if (handle == INVALID_HANDLE_VALUE)
	{
		config = new H4Configuration(const_cast<TCHAR *>(LPCTSTR(port)), baud);	
	}
	else
	{
		// Purge any operations in progress
		if (!PurgeComm(handle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR))
		{
			return DFUEngine::fail_os;
		}

		// Configure the COM port for H4
		DCB dcb;
		if (!GetCommState(handle, &dcb))
		{
			return DFUEngine::fail_os;
		}
		dcb.BaudRate = baud;
		dcb.fBinary = true;
		dcb.fParity = comH4Parity != NOPARITY;
		dcb.fOutxCtsFlow = true;
		dcb.fOutxDsrFlow = false;
		dcb.fDtrControl = DTR_CONTROL_ENABLE;
		dcb.fDsrSensitivity = false;
		dcb.fOutX = false;
		dcb.fInX = false;
		dcb.fErrorChar = false;
		dcb.fNull = false;
		dcb.fRtsControl = comH4RtsControl;
		dcb.fAbortOnError = false;
		dcb.ByteSize = comH4DataBits;
		dcb.Parity = comH4Parity;
		dcb.StopBits = comH4StopBits;
		if (!SetCommState(handle, &dcb))
		{
			return DFUEngine::fail_os;
		}
        UARTConfiguration h(const_cast<TCHAR *>((LPCTSTR)(port)),handle);
		config = new H4Configuration(h);
	}

	// Attempt a connection
	result = Connect(port, baud, SmartPtr<TransportConfiguration>(config));
	if (!result) return result;

	// Successful if this point reached
	return DFUEngine::success;
}
#endif

DFUEngine::Result DFUTransportCOM::ConnectBCSP(const TCHAR *port,
											   int baud, bool sync, bool tunneling,
											   HANDLE handle)
{
	// End any existing transport
	DFUEngine::Result result = Disconnect();
	if (!result) return result;

	// Start a BCSP transport
	BCSPConfiguration *config;
	if (handle == INVALID_HANDLE_VALUE)
	{
		config = new BCSPConfiguration(const_cast<TCHAR *>(LPCTSTR(port)), baud,true,tunneling);
	}
	else
	{
		// Purge any operations in progress
		if (!PurgeComm(handle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR))
		{
			return DFUEngine::fail_os;
		}
		
		// Configure the COM port for BCSP
		DCB dcb;
		if (!GetCommState(handle, &dcb))
		{
			return DFUEngine::fail_os;
		}
		dcb.BaudRate = baud;
		dcb.fBinary = true;
		dcb.fParity = comBCSPParity != NOPARITY;
		dcb.fOutxCtsFlow = false;
		dcb.fOutxDsrFlow = false;
		dcb.fDtrControl = DTR_CONTROL_ENABLE;
		dcb.fDsrSensitivity = false;
		dcb.fOutX = false;
		dcb.fInX = false;
		dcb.fErrorChar = false;
		dcb.fNull = false;
		dcb.fRtsControl = comBCSPRtsControl;
		dcb.fAbortOnError = false;
		dcb.ByteSize = comBCSPDataBits;
		dcb.Parity = comBCSPParity;
		dcb.StopBits = comBCSPStopBits;
		dcb.ErrorChar = comBCSPErrorChar;
		if (!SetCommState(handle, &dcb))
		{
			return DFUEngine::fail_os;
		}
		UARTConfiguration h(port,handle);
		config = new BCSPConfiguration(h,true,tunneling);
	}

	// Configure link establishment
    config->setConfig(config->getResendTimeout(),
                      config->getRetryLimit(),
                      config->getTShy(),
                      config->getTConf(),
                      config->getConfLimit(),
                      config->getWindowSize(),
                      sync);

	// Attempt a connection
	result = Connect(port, baud, SmartPtr<TransportConfiguration>(config));
	if (!result) return result;

	// Successful if this point reached
	return DFUEngine::success;
}

#if !defined _WINCE && !defined _WIN32_WCE
DFUEngine::Result DFUTransportCOM::ConnectUSBCSR(const TCHAR *device,
												 HANDLE handle)
{
	// End any existing transport
	DFUEngine::Result result = Disconnect();
	if (!result) return result;

	// Start an H2 transport using the CSR device driver
	H2Configuration *config;
	if (handle == INVALID_HANDLE_VALUE)
		config = new H2Configuration(USBConfiguration(device));
	else
		config = new H2Configuration(USBConfiguration(device,handle));

	// Attempt a connection
	result = Connect(device, usbBaudRate, SmartPtr<TransportConfiguration>(config));
	if (!result) return result;

	// Successful if this point reached
	return DFUEngine::success;
}

DFUEngine::Result DFUTransportCOM::ConnectUSBBTW(const TCHAR *device,
												 HANDLE handle)
{
    // singledll differentiates on name.
    return ConnectUSBCSR(device,handle);
}
#endif

// End transport
DFUEngine::Result DFUTransportCOM::Disconnect()
{
	// Stop and destroy any active transport
	if (impl_->transport)
	{
		// transport->Stop();
		// Sleep(0);
        // delete the transport
		impl_->transport = SmartPtr<BlueCoreDeviceController_newStyle>();
		impl_->baudRate = 0;
	}
	return DFUEngine::success;
}

// Generic control requests
DFUEngine::Result DFUTransportCOM::ControlRequest(const DFURequests::Setup &setup,
                                                  void *buffer,
					                              uint16 bufferLength,
	                                              uint16 *replyLength)
{
	// Reply header
	struct Status
	{
		uint16 wStatus;
		uint16 wReplyLength;
	};

	// Only process a single control request at any time
	CriticalSection::Lock lock(&impl_->lockUpgrade);

	// Ensure that the transport is valid
	if (!impl_->transport) return DFUEngine::fail_com_none;
	DFUInterface::Event *peventFailTransport = &impl_->eventFailTransport;
	if (DFUInterface::Event::CheckMultiple(1, &peventFailTransport, 0) != -1) return DFUEngine::fail_com_fail;

	// Wait for any outstanding transfers to complete
	if (!impl_->Wait(&impl_->eventUpgradeReceive))
	{
		return DFUEngine::fail_com_fail;
	}

	// Check that the buffer is large enough
	if (bufferLength < setup.wLength)
	{
		return DFUEngine::fail_com_buffer;
	}

	// Prepare the transmit buffer
	uint16 requestLength;
	uint16 maxReplyLength;
	if (setup.bmRequestTypeDirection
		== DFURequests::dir_host_to_device)
	{
		// Setup packet and buffer to send
		requestLength = sizeof(setup) + setup.wLength;
		maxReplyLength = sizeof(Status);
        impl_->requestUpgrade = PDU(PDU::upgrade,requestLength);
		memcpy((void*)impl_->requestUpgrade.data(), &setup, sizeof(setup));
		memcpy((void*)(impl_->requestUpgrade.data() + sizeof(setup)), buffer, setup.wLength);
	}
	else
	{
		// Only the setup packet to send
		requestLength = sizeof(DFURequests::Setup);
		maxReplyLength = sizeof(Status) + setup.wLength;
        impl_->requestUpgrade = PDU(PDU::upgrade,(uint8*)&setup,requestLength);
	}

	// Send the command
	impl_->eventUpgradeReceive.Unset();
    if ( !impl_->transport->send(impl_->requestUpgrade) )
	{
        onTransportFailure(FailureMode(7));
		impl_->eventFailTransport.Set();
		return DFUEngine::fail_com_timeout_tx;
	}
	TimeoutSet(Timeout(requestLength + maxReplyLength, timeoutUpgradeTransmitMin, timeoutUpgradeTransmitBase)
                      +Timeout(maxReplyLength, timeoutUpgradeReceiveMin, timeoutUpgradeReceiveBase));
	DFUEngine::Result result = impl_->Wait(&impl_->eventUpgradeReceive);
	if (result.GetCode() == DFUEngine::fail_com_timeout)
	{
		result = DFUEngine::fail_com_timeout_rx;
	}
	if (!result)
	{
		return result;
	}
	TimeoutClear();

	// Decode the reply
	if (impl_->bufferUpgrade.size() < sizeof(Status))
	{
		return DFUEngine::fail_com_short;
	}
	Status *status = (Status *) (uint8 *) impl_->bufferUpgrade.data();
	if (replyLength) *replyLength = status->wReplyLength;
	if (impl_->bufferUpgrade.size() < sizeof(Status) + status->wReplyLength)
	{
		return DFUEngine::fail_com_short;
	}
	if (sizeof(Status) + status->wReplyLength < impl_->bufferUpgrade.size())
	{
		return DFUEngine::fail_com_long;
	}
	if (setup.bmRequestTypeDirection
		== DFURequests::dir_device_to_host)
	{
		// A reply was expected
		if (setup.wLength < status->wReplyLength)
		{
			return DFUEngine::fail_com_long;
		}
		memcpy(buffer, impl_->bufferUpgrade.data() + sizeof(Status), status->wReplyLength);
	}
	else
	{
		// There should be no reply data
		if (status->wReplyLength != 0)
		{
			return DFUEngine::fail_com_long;
		}
	}
	if (status->wStatus != 0)
	{
		return DFUEngine::fail_dfu_stalledpkt;
	}

	// Successful if this point reached
	return DFUEngine::success;
}

DFUEngine::Result DFUTransportCOM::HCIReset()
{
	struct
	{
		uint8 status;
	} event;

	// Perform the HCI request
	uint8 eventLength;
	DFUEngine::Result result = HCIRequest(ogf_baseband, ocf_reset, 0, 0, &event, sizeof(event), &eventLength);
	if (!result) return result;
	if (eventLength < sizeof(event))
	{
			return DFUEngine::fail_com_short;
	}
	if (sizeof(event) < eventLength)
	{
			return DFUEngine::fail_com_long;
	}
	if (event.status != hci_success)
	{
		return MapHCIStatus(event.status);
	}

	// Reset the events for all channels
	impl_->eventHCIReceive.Set();
	impl_->eventBCCMDReceive.Set();
	impl_->eventUpgradeReceive.Set();

	// Successful if this point reached
	return DFUEngine::success;
}

// Specific HCI requests
DFUEngine::Result DFUTransportCOM::HCIReadBDADDR(BD_ADDR bdAddr)
{
	struct
	{
		uint8 status;
		BD_ADDR bdAddr;
	} event;

	// Perform the HCI request
	uint8 eventLength;
	DFUEngine::Result result = HCIRequest(ogf_informational, ocf_read_bd_addr, 0, 0, &event, sizeof(event), &eventLength);
	if (!result) return result;
	if (eventLength < sizeof(event))
	{
			return DFUEngine::fail_com_short;
	}
	if (sizeof(event) < eventLength)
	{
			return DFUEngine::fail_com_long;
	}
	if (event.status != hci_success)
	{
		return MapHCIStatus(event.status);
	}

	// Copy the result
	memcpy(bdAddr, event.bdAddr, sizeof(BD_ADDR));

	// Successful if this point reached
	return DFUEngine::success;
}

// Generic HCI requests
DFUEngine::Result DFUTransportCOM::HCIRequest(uint16 ogf, uint16 ocf,
	                                          void *command,
	                                          uint8 commandLength,
	                                          void *event,
	                                          uint8 eventLength,
	                                          uint8 *eventActualLength)
{
	// Only process a single HCI command at any time
	CriticalSection::Lock lock(&impl_->lockHCI);

	// Ensure that the transport is valid
	if (!impl_->transport) return DFUEngine::fail_com_none;
	DFUInterface::Event *pevent = &impl_->eventFailTransport;
	if (DFUInterface::Event::CheckMultiple(1, &pevent, 0) != -1) return DFUEngine::fail_com_fail;

	// Wait for any outstanding transfers to complete
	if (!impl_->Wait(&impl_->eventHCIReceive))
	{
		return DFUEngine::fail_com_fail;
	}

	// Prepare the transmit buffer
    impl_->requestHCI = HCIMadeCommandPDU(makeopcode(ogf,ocf),commandLength,(const uint8*)command);

	// Send the command
	impl_->eventHCIReceive.Unset();
	if( !impl_->transport->send(impl_->requestHCI) )
    {
	    // Treat failed transmits as link failure
        onTransportFailure(FailureMode(5));
		impl_->eventFailTransport.Set();
		return DFUEngine::fail_com_timeout_tx;
    }
	// Wait for a reply to be received
	TimeoutSet(timeoutHCIReceive);
	DFUEngine::Result result = impl_->Wait(&impl_->eventHCIReceive);
	if (result.GetCode() == DFUEngine::fail_com_timeout)
	{
		result = DFUEngine::fail_com_timeout_rx;
	}
	if (!result)
	{
		return result;
	}
	TimeoutClear();

	// Action depends on the event
	switch (impl_->bufferHCI.get_event_code())
	{
	case hci_event_command_complete:
		{
			HCIDismemberCommandCompletePDU eventCommandComplete = impl_->bufferHCI;
			if (eventCommandComplete.get_op_code() != makeopcode(ogf,ocf))
			{
				return DFUEngine::fail_com_mismatched;
			}
			uint8 length = eventCommandComplete.get_parameter_length();
			if (eventLength < length)
			{
				return DFUEngine::fail_com_buffer;
			}
			if (eventActualLength)
                *eventActualLength = length;
			eventCommandComplete.get_parameters((uint8*)event,length);
		}
		break;

	case hci_event_command_status:
		{
			HCI_EV_COMMAND_STATUS_T_PDU eventCommandStatus = impl_->bufferHCI;
			if (eventCommandStatus.get_op_code() != makeopcode(ogf,ocf))
			{
				return DFUEngine::fail_com_mismatched;
			}
			if (eventActualLength)
                *eventActualLength = 0;
			if (eventCommandStatus.get_status() != hci_success)
			{
				return MapHCIStatus(eventCommandStatus.get_status());
			}
		}
		break;

	default:
		// No other events should reach here
		return DFUEngine::fail_com_timeout_rx;
		break;
	}

	// Successful if this point reached
	return DFUEngine::success;
}


// BCMD get requests
DFUEngine::Result DFUTransportCOM::BCCMDGet(uint16 varid, uint16 &value)
{
	return BCCMDRequest(false, varid, &value, sizeof(value));
}

// BCCMD persistent store read requests
DFUEngine::Result DFUTransportCOM::BCCMDGetPS(uint16 key, void *buffer,
                                              uint16 bufferLength,
                                              uint16 *readLength)
{
	// Prepare the transmit buffer
	uint16 requestLength = sizeof(BCCMDPSHeader) + bufferLength;
	SmartPtr<uint8, true> request(new uint8[requestLength]);
	BCCMDPSHeader *header = (BCCMDPSHeader *) (uint8 *) request;
	header->key = key;
	header->length = bufferLength / 2;
	header->stores = ps_stores_default;
	if (bufferLength)
	{
		memset(request + sizeof(BCCMDPSHeader), 0, bufferLength);
	}

	// Perform the BCCMD command
	DFUEngine::Result result = BCCMDRequest(false, bccmd_varid_ps, request, requestLength);
	if (!result) return result;

	// Decode the reply
	if (buffer)
	{
		memcpy(buffer, request + sizeof(BCCMDPSHeader), bufferLength);
	}
	if (readLength) *readLength = header->length * 2;

	// Successful if this point reached
	return DFUEngine::success;
}

// BCCMD persistent store write requests
DFUEngine::Result DFUTransportCOM::BCCMDSetPS(uint16 key, void *buffer, uint16 bufferLength)
{
	// Prepare the transmit buffer
	uint16 requestLength = sizeof(BCCMDPSHeader) + 2;
	SmartPtr<uint8, true> request(new uint8[requestLength]);
	BCCMDPSHeader *header = (BCCMDPSHeader *) (uint8 *) request;
	header->key = key;
	header->length = bufferLength / 2;
	header->stores = ps_stores_default;
	memset(request + sizeof(BCCMDPSHeader), 0, 2);

	// Copy the buffer
	if (buffer)
	{
		memcpy(request + sizeof(BCCMDPSHeader), buffer, bufferLength);
	}

	// Perform the BCCMD command
	DFUEngine::Result result = BCCMDRequest(true, bccmd_varid_ps, request, requestLength);
	if (!result) 
		return result;

	// Successful if this point reached
	return DFUEngine::success;
}


// BCCMD radiotest pause
DFUEngine::Result DFUTransportCOM::BCCMDRadiotestPause()
{
	uint16 test = radiotest_pause;
	return BCCMDRequest(true, bccmd_varid_radiotest, &test, sizeof(test));
}

// BCCMD cold reset
DFUEngine::Result DFUTransportCOM::BCCMDColdReset()
{
	return BCCMDRequest(true, bccmd_varid_cold_reset, NULL, 0);
}

// Generic BCMD requests
DFUEngine::Result DFUTransportCOM::BCCMDRequest(bool set, uint16 varid,
	                                            void *buffer,
	                                            uint16 bufferLength)
{
	static uint16 seqno = 0;

	// Only process a single BCCMD command at any time
	CriticalSection::Lock lock(&impl_->lockBCCMD);

	// Ensure that the transport is valid
	if (!impl_->transport) return DFUEngine::fail_com_none;
	DFUInterface::Event *pevent = &impl_->eventFailTransport;
	if (DFUInterface::Event::CheckMultiple(1, &pevent, 0) != -1) return DFUEngine::fail_com_fail;

	// Wait for any outstanding transfers to complete
	if (!impl_->Wait(&impl_->eventBCCMDReceive))
	{
		return DFUEngine::fail_com_fail;
	}

	// Prepare the transmit buffer
	impl_->requestBCCMD = BCCMD_PDU(varid);
	impl_->requestBCCMD.set_req_type(set ? bccmd_setreq : bccmd_getreq);
	impl_->requestBCCMD.set_seq_no(++seqno);
	if (buffer)
	{
        // checks minimum length and resizes if needed
		impl_->requestBCCMD.set_payload((const uint16*)buffer, bufferLength);
	}

	// Send the command
	impl_->eventBCCMDReceive.Unset();
	// Treat failed transmits as link failure
	if(!impl_->transport->send(impl_->requestBCCMD))
	{
        onTransportFailure(FailureMode(6));
		impl_->eventFailTransport.Set();
		return DFUEngine::fail_com_timeout_tx;
	}

	// Wait for a reply to be received
	DFUEngine::Result result = impl_->Wait(&impl_->eventBCCMDReceive);
	if (result)
	{
		TimeoutSet(timeoutBCCMDReceive);
		result = impl_->Wait(&impl_->eventBCCMDReceive);
	}
	if (result.GetCode() == DFUEngine::fail_com_timeout)
	{
		result = DFUEngine::fail_com_timeout_rx;
	}
	if (!result)
	{
		return result;
	}
	TimeoutClear();

	// Check the reply
	if (impl_->bufferBCCMD.size() < minBCCMD)
	{
		return DFUEngine::fail_com_short;
	}
	if ((impl_->bufferBCCMD.get_req_type() != bccmd_getresp)
		|| (impl_->bufferBCCMD.get_seq_no() != seqno)
		|| (impl_->bufferBCCMD.get_varid() != varid)
		|| (impl_->bufferBCCMD.size() != impl_->requestBCCMD.size()))
	{
		return DFUEngine::fail_com_mismatched;
	}
	if (impl_->bufferBCCMD.size() < impl_->bufferBCCMD.get_length() * 2)
	{
		return DFUEngine::fail_com_short;
	}
	if (impl_->bufferBCCMD.get_length() * 2 < impl_->bufferBCCMD.size())
	{
		return DFUEngine::fail_com_long;
	}
	if (impl_->bufferBCCMD.get_status() != bccmd_ok)
	{
		return MapBCCMDStatus(impl_->bufferBCCMD.get_status());
	}

	// Copy the response
	if (buffer)
	{
		impl_->bufferBCCMD.get_payload((uint16*)buffer, bufferLength);
	}

	// Successful if this point reached
	return DFUEngine::success;
}

// Conversion of HCI and BCCMD status codes to DFUEngine  codes
DFUEngine::Result DFUTransportCOM::MapHCIStatus(uint8 status)
{
	// Attempt to find the appropriate result code
	DFUEngine::Result result(DFUEngine::fail_hci_unknown);
	for (int index = 0;
		 index < (sizeof(statusHCICodeMap) / sizeof(statusHCICodeMapEntry));
		 ++index)
	{
		if (statusHCICodeMap[index].status == status)
		{
			result = statusHCICodeMap[index].result;
			break;
		}
	}

	// Return the result
	return result;
}

DFUEngine::Result DFUTransportCOM::MapBCCMDStatus(uint16 status)
{
	// Attempt to find the appropriate result code
	DFUEngine::Result result(DFUEngine::fail_bccmd_unknown);
	for (int index = 0;
		 index < (sizeof(statusBCCMDCodeMap) / sizeof(statusBCCMDCodeMapEntry));
		 ++index)
	{
		if (statusBCCMDCodeMap[index].status == status)
		{
			result = statusBCCMDCodeMap[index].result;
			break;
		}
	}

	// Return the result
	return result;
}

// Set or get the abort event object
void DFUTransportCOM::SetAbort(DFUInterface::Event *abort)
{
	impl_->eventAbort = abort;
}

DFUEngine::Result DFUTransportCOM::Connect(const TCHAR *port, int baud,
                                               SmartPtr<TransportConfiguration> &config)
{
	// Store the baud rate
	impl_->baudRate = baud;

	// Initially not failed
	impl_->eventFailTransport.Unset();

	// Reset any previously set timeout
	TimeoutClear();

	// Initially not waiting for transmit or receive to complete
	impl_->eventHCIReceive.Set();
	impl_->eventBCCMDReceive.Set();
	impl_->eventUpgradeReceive.Set();

	// Create the transport
	impl_->transport = SmartPtr<BlueCoreDeviceController_newStyle>(new BlueCoreDeviceController_newStyle(*config,*this));

	// Start the transport
	bool start = impl_->transport->startTransport();
	if (!start)
		return DFUEngine::fail_com_start;
	impl_->transport->createWorker();

	// Check if the transport is ready
	if (!impl_->transport->isTransportReady(Timeout(timeoutConnectionBytes, timeoutConnectionMilliseconds)))
	{
		return DFUEngine::fail_com_connect;
	}

	// Store the transport and configuration if successful
	impl_->config = config;

	// Set the default maximum message length (after transport member set)
	MaximumLength(defaultMaximumLength);

	// Add an additional delay to allow the chip to prepare
	Sleep(delayConnection);

	// Successful if this point reached
	return DFUEngine::success;
}

// Set the maximum expected message size
void DFUTransportCOM::MaximumLength(uint16 length)
{
	// Store the new maximum message length
	impl_->maximumLength = length;

	// Calculate the BCSP retry timeout
    if ( *impl_->config == TransportConfiguration::BCSP )
	{
		// Calculate the timeout associated with the message length
		impl_->maximumLengthTimeout = Timeout(length, timeoutMaximumLengthMin, timeoutMaximumLengthBase);

		// Set the BCSP timeout
//		StackConfiguration cfg;
//		bcsp->getStackConfiguration(cfg);
//		cfg.resendTimeout = maximumLengthTimeout;
//		bcsp->setStackConfiguration(cfg);
	}
	else impl_->maximumLengthTimeout = 0;
}

// Calculate timeouts or delays
uint32 DFUTransportCOM::Timeout(uint32 transferBytes,
	                            uint32 minimumMilliseconds,
	                            uint32 baseMilliseconds)
{
	return 0 < impl_->baudRate
	       ? max(minimumMilliseconds,
	             baseMilliseconds
			     + (transferBytes * bitsPerByte * 1000) / impl_->baudRate)
	       : minimumMilliseconds;
}

// Set and clear timeout periods
void DFUTransportCOM::TimeoutSet(uint32 milliseconds)
{
	// Choose the new expiry time
	uint32 now = GetTickCount();
	uint32 timeout = impl_->enabledTimeout ? impl_->nextTimeout - now : 0;
	if (int32(timeout) < 0) timeout = 0;
	if (timeout < milliseconds) timeout = milliseconds;

	// Store the new timeout
	impl_->enabledTimeout = true;
	impl_->nextTimeout = now + timeout;
}

void DFUTransportCOM::TimeoutClear()
{
	// Clear the timeout
	impl_->enabledTimeout = false;
}

// Wait for events
DFUEngine::Result DFUTransportCOMImpl::Wait(DFUInterface::Event *event)
{
	// Calculate the timeout
	uint32 timeout = 0;
	if (enabledTimeout)
	{
		// Timeout based on message specific details
		int32 milliseconds = nextTimeout - GetTickCount();
		if (0 < milliseconds)
		{
			// Adjust the timeout if BCSP timeouts also used
			if (0 < maximumLengthTimeout)
			{
				if (((uint32) milliseconds) < maximumLengthTimeout)
				{
					milliseconds = maximumLengthTimeout;
				}
				timeout = milliseconds * timeoutRetryScale + timeoutRetryOffset;
			}
			else timeout = milliseconds;
		}
	}

	// Wait for the link failure and abort events also
	DFUInterface::Event *eventsList[3] = { event, &eventFailTransport, eventAbort };
	int result = DFUInterface::Event::CheckMultiple(eventAbort ? 3 : 2, eventsList, timeout);

	// Process the result
	switch (result)
	{
	case 0:
		return DFUEngine::success;

	case 1:
		return DFUEngine::fail_com_fail;

	case 2:
		return DFUEngine::aborted;

	case -1:
		return DFUEngine::fail_com_timeout;

	default:
		return DFUEngine::fail_unknown;
	}
}

// Callbacks
void DFUTransportCOM::onTransportFailure(FailureMode f)
{
	impl_->eventFailTransport.Set();
}

void DFUTransportCOM::onPDU(const PDU& pdu)
{
    switch( pdu.channel() )
    {
    case PDU::hciCommand:
        {
        HCIEventPDU e(pdu);
        // Action depends on the event type
        switch (e.get_event_code())
        {
        case hci_event_command_complete:
	        // Notify the thread waiting for the event
	        impl_->bufferHCI = pdu;
	        impl_->eventHCIReceive.Set();
	        break;

        case hci_event_command_status:
	        {
                HCI_EV_COMMAND_STATUS_T_PDU s(pdu);
		        if (s.get_op_code())// not a nop
		        {
			        // Notify the thread waiting for the event
			        impl_->bufferHCI = pdu;
			        impl_->eventHCIReceive.Set();
		        }
	        }
	        break;

        default:
	        // Not interested in any other event
	        break;
        }
        }
    	break;
    case PDU::bccmd:
        impl_->bufferBCCMD = BCCMD_PDU(pdu);
	    impl_->eventBCCMDReceive.Set();
    	break;
    case PDU::upgrade:
        impl_->bufferUpgrade = pdu;
	    impl_->eventUpgradeReceive.Set();
    	break;
    default:
        break;
    }
}
