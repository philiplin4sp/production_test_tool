///////////////////////////////////////////////////////////////////////
//
//	File	: DFUTransportCOM.h
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
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUTransportCOM.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef DFUTRANSPORTCOM_H
#define DFUTRANSPORTCOM_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "DFUEngine.h"
#include "DFURequests.h"
#include "csrhci/bluecontroller.h"
class TransportConfiguration;
#include "../SmartPtr.h"
#include <windows.h>

// DFUTransportCOM class
class DFUTransportCOM : private BlueCoreDeviceController_newStyle::CallBackProvider
{
public:

	// HCI command OGFs
	enum
	{
		ogf_nop = 0x00,
		ogf_baseband = 0x03,
		ogf_informational = 0x04
	};

	// HCI command OCFs
	enum
	{
		ocf_nop = 0x0000,

		// HCI controller and baseband commands
		ocf_reset = 0x0003,

		// Informational parameters
		ocf_read_bd_addr = 0x0009
	};

	// HCI events
	enum
	{
		hci_event_command_complete = 0x0e,
		hci_event_command_status = 0x0f
	};

	// HCI status codes
	enum
	{
		hci_success = 0x00,					// Success
		hci_err_unknown_cmd = 0x01,			// Unknown HCI command
		hci_err_no_connection = 0x02,		// No suitable connection exists
		hci_err_hardware_fail = 0x03,		// Could not execute due to HW fail
		hci_err_page_timeout = 0x04,		// Device did not respond to page
		hci_err_auth_fail = 0x05,			// PIN or link key was wrong
		hci_err_err_key_missing = 0x06,		// PIN or link key not supplied
		hci_err_memory_full = 0x07,			// Insufficient memory to complete
		hci_err_connect_timeout = 0x08,		// Device did not respond to connect
		hci_err_max_connect = 0x09,			// Too many connections
		hci_err_max_sco = 0x0a,				// Too many SCO links
		hci_err_acl_exists = 0x0b,			// An ACL link already exists
		hci_err_cmd_disallowed = 0x0c,		// Command not supported now
		hci_err_host_rej_resource = 0x0d,	// Insufficient resources on host
		hci_err_host_rej_security = 0x0e,	// Rejected due to security
		hci_err_host_rej_personal = 0x0f,	// Personal device only
		hci_err_host_timeout = 0x10,		// Host did not respond
		hci_err_unsupported = 0x11,			// Command not supported by device
		hci_err_invalid_params = 0x12,		// Command not according to spec
		hci_err_term_user = 0x13,			// Link terminated by used
		hci_err_term_resource = 0x14,		// Link terminated due to resources
		hci_err_term_off = 0x15,			// Link terminated due to power off
		hci_err_term_local = 0x16,			// Link terminated locally
		hci_err_repeated = 0x17,			// Too soon after failed auth
		hci_err_no_pairing = 0x18,			// Pairing not allowed now
		hci_err_lmp_unknown = 0x19,			// Unknown LMP operation
		hci_err_unsupported_rem = 0x1a,		// Remote device does not support
		hci_err_sco_offset = 0x1b,			// SCO offset rejected
		hci_err_sco_interval = 0x1c,		// SCO interval rejected
		hci_err_sco_air_mode = 0x1d,		// SCO air mode rejected
		hci_err_lmp_invalid = 0x1e,			// Invalid LMP parameters
		hci_err_unspecified = 0x1f,			// Other LMP error
		hci_err_lmp_unsupported = 0x20,		// LMP operation not supported
		hci_err_role_change = 0x21,			// Role change not available
		hci_err_lmp_timeout = 0x22,			// LMP timeout occurred
		hci_err_lmp_error = 0x23,			// LMP error transaction collision
		hci_err_lmp_not_allowed = 0x24		// LMP PDU not allowed
	};

	// BCCMD message types
	enum
	{
		bccmd_getreq = 0x0000,
		bccmd_getresp = 0x0001,
		bccmd_setreq = 0x0002
	};

	// BCCMD status values
	enum
	{
		bccmd_ok = 0x0000,					// No problem found
		bccmd_no_such_varid = 0x0001,		// Variable not recognised
		bccmd_too_big = 0x0002,				// Data exceeded capacity
		bccmd_no_value = 0x0003,			// Variable has no value
		bccmd_bad_req = 0x0004,				// GETREQ or SEQREQ had error
		bccmd_no_access = 0x0005,			// Value of variable inaccessible
		bccmd_read_only = 0x0006,			// Value of variable unwritable
		bccmd_write_only = 0x0007,			// Value of variable unreadable
		bccmd_error = 0x0008,				// Other error
		bccmd_permission_denied = 0x0009	// Request not allowed
	};

	// BCCMD varids
	enum
	{
		bccmd_varid_buildid = 0x2819,		// Read build identifier
		bccmd_varid_cold_reset = 0x4001,	// Cold reset of the BlueCore
		bccmd_varid_radiotest = 0x5004,		// Radiotest command
		bccmd_varid_ps = 0x7003				// Read or write persistent store
	};

	// Radiotest commands
	enum
	{
		radiotest_pause = 0x0000			// Pause
	};

	// Persistent stores
	enum
	{
		ps_stores_default = 0x0000,
		ps_stores_i = 0x0001,				// psfl
		ps_stores_f = 0x0002,				// psf
		ps_stores_r = 0x0004,				// psrom
		ps_stores_if = 0x0003,				// psfl and psf
		ps_stores_ifr = 0x0007				// psfl, psf and psrom
	};

    // HCI helpers
    inline uint16 getogf(uint16 opcode) { return (opcode & 0xFC00) >> 10; }
    inline uint16 getocf(uint16 opcode) { return (opcode & 0x03FF); }
    inline uint16 makeopcode( uint16 ogf, uint16 ocf ) { return ((ogf<< 10)&0xFC00)|(ocf&0x3FF); }
    class HCIMadeCommandPDU : public HCICommandPDU
    {
    public:
        HCIMadeCommandPDU(uint16 opcode, uint8 parameter_length, const uint8*parameters)
        : HCICommandPDU(opcode,parameter_length+3)
        {
            set_uint8Array(parameters,3,parameter_length);
        }
    };
    class HCIDismemberCommandCompletePDU :public HCICommandCompletePDU
    {
    private:
        static const int offset_to_parameters;
    public:
        HCIDismemberCommandCompletePDU( const PDU& p ) : HCICommandCompletePDU(p) {}
        uint8 get_parameter_length()
        {
            return size() - offset_to_parameters;
        }
        //  returns number of bytes copied
        uint32 get_parameters(uint8* ToFill, uint32 length)
        {
            uint32 toget = length;
            if ( length > get_parameter_length() )
                length = get_parameter_length();
            return get_uint8Array(ToFill,offset_to_parameters,toget);
        }
    };

#pragma pack(push, 1)
	// BCCMD PS header
	struct BCCMDPSHeader
	{
		uint16 key;
		uint16 length;
		uint16 stores;
	};
#pragma pack(pop)

	// A Bluetooth address
	typedef uint8 BD_ADDR[6];

	// Constructor
	DFUTransportCOM();

	// Destructor
	virtual ~DFUTransportCOM();

	// Start transport
#if !defined _WINCE && !defined _WIN32_WCE
	DFUEngine::Result ConnectH4(const TCHAR *port, int baud,
	                            HANDLE handle = INVALID_HANDLE_VALUE);
#endif
	DFUEngine::Result ConnectBCSP(const TCHAR *port, int baud,
	                              bool sync = true, bool tunneling = false,
								  HANDLE handle = INVALID_HANDLE_VALUE);
	DFUEngine::Result ConnectUSBCSR(const TCHAR *device,
								    HANDLE handle = INVALID_HANDLE_VALUE);
	DFUEngine::Result ConnectUSBBTW(const TCHAR *device,
								    HANDLE handle = INVALID_HANDLE_VALUE);

	// End transport
	DFUEngine::Result Disconnect();

	// Generic control requests
	DFUEngine::Result ControlRequest(const DFURequests::Setup &setup,
	                                 void *buffer = 0,
	                                 uint16 bufferLength = 0,
	                                 uint16 *replyLength = 0);

	// Specific HCI requests
	DFUEngine::Result HCIReset();
	DFUEngine::Result HCIReadBDADDR(BD_ADDR bdAddr);

	// Generic HCI requests
	DFUEngine::Result HCIRequest(uint16 ogf, uint16 ocf,
	                             void *command = 0,
	                             uint8 commandLength = 0,
	                             void *event = 0,
	                             uint8 eventLength = 0,
	                             uint8 *eventActualLength = 0);

	// BCMD get requests
	DFUEngine::Result BCCMDGet(uint16 varid, uint16 &value);

	// BCCMD persistent store read/write requests
	DFUEngine::Result BCCMDGetPS(uint16 key, void *buffer,
	                             uint16 bufferLength,
								 uint16 *readLength = 0);

	DFUEngine::Result BCCMDSetPS(uint16 key, void *buffer,
								 uint16 bufferLength);

	// BCCMD cold reset
	DFUEngine::Result BCCMDColdReset();

	// BCCMD radiotest pause
	DFUEngine::Result BCCMDRadiotestPause();

	// Generic BCMD requests
	DFUEngine::Result BCCMDRequest(bool set, uint16 varid,
	                               void *buffer = 0,
								   uint16 bufferLength = 0);

	// Calculate timeouts or delays
	uint32 Timeout(uint32 transferBytes,
	               uint32 minimumMilliseconds = 0,
	               uint32 baseMilliseconds = 0);

	// Set the maximum expected message size
	void MaximumLength(uint16 length);

	// Conversion of HCI and BCCMD status codes to DFUEngine codes
	DFUEngine::Result MapHCIStatus(uint8 status);
	DFUEngine::Result MapBCCMDStatus(uint16 status);

	// Set or get the abort event object

	virtual void SetAbort(DFUInterface::Event *abort = 0);

protected:
	// Start transport
	DFUEngine::Result Connect(const TCHAR *port, int baud,
	                          SmartPtr<TransportConfiguration> &transport);

	// Set and clear timeout periods
	void TimeoutSet(uint32 milliseconds);
	void TimeoutClear();

	// Callbacks
    void onPDU(const PDU& pdu);
    void onTransportFailure(FailureMode f);

	void OnTransmitHCI(bool success);
	void OnTransmitBCCMD(bool success);
	void OnTransmitUpgrade(bool success);


private:
	friend class DFUTransportCOMImpl;
	SmartPtr<DFUTransportCOMImpl> impl_;
};

#endif
