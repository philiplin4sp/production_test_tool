///////////////////////////////////////////////////////////////////////
//
//	File	: DFURequestsCOM.h
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
//		1.20	23:Feb:01	at	Improved result code returned for
//								failure to connect. Added support for
//								aborting initial connection.
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
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFURequestsCOM.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef DFUREQUESTSCOM_H
#define DFUREQUESTSCOM_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "Device.h"
#include "DFURequests.h"
#include "DFUTransportCOM.h"

// DFURequestsCOM class
class DFURequestsCOM : public DFURequests
{
public:

	// Constructors
	DFURequestsCOM(const TCHAR *port);
	DFURequestsCOM(const TCHAR *port, int baud);
	DFURequestsCOM(const TCHAR *port, int baud, bool tunnel,
	               DFUEngine::Link link);

	// Destructor
	virtual ~DFURequestsCOM();

	// COM port and baud rate enumeration
	struct BaudRateList {
		int baud;
		BaudRateList *next;
	};
	static int EnumeratePorts(CStringListX &ports);
	static int EnumerateBaudRates(BaudRateList *&baudRates);
	static int EnumerateBaudRates(const CStringListX &ports,
	                              BaudRateList *&baudRates);
	static void FreeBaudRateList(BaudRateList *);

	// Set the abort event object
	virtual void SetAbort(DFUInterface::Event *abort = 0);

protected:

	// Protocols
	enum Protocol
	{
		protocol_unknown,		// Protocol not determined
		protocol_bcsp,			// BCSP with private channels
		protocol_bcsp_tunnel,	// BCSP with HCI tunnelling
		protocol_h4,			// H4 with HCI tunnelling
		protocol_failed			// Previous connection attempts failed
	};
	enum LinkEstablishment
	{
		sync_unknown,			// Link establishment not determined
		sync_enabled,			// Link establishment enabled
		sync_disabled,			// Link establishment disabled
		sync_passive			// Link establishment enabled but passive
	};

	// A request that safely tests private channels
	DFUEngine::Result TestPrivateChannel();

	// Automatic protocol detection
	DFUEngine::Result AutomaticPassiveBCSP();
	DFUEngine::Result AutomaticActiveBCSP(bool link);
#if !defined _WINCE && !defined _WIN32_WCE
	DFUEngine::Result AutomaticH4();
#endif
	
	// Connection and disconnection
	virtual DFUEngine::Result RPCConnect(bool hintDFU);
	virtual DFUEngine::Result RPCDisconnect();

	// Non USB DFU control requests (implemented here as USB control requests)
	virtual DFUEngine::Result RPCReset();
	virtual DFUEngine::Result RPCGetInterfaceDFU(InterfaceDescriptor &descriptor);
	virtual DFUEngine::Result RPCGetDevice(DeviceDescriptor &descriptor);
	virtual DFUEngine::Result RPCGetFunct(DFUFunctionalDescriptor &descriptor);
	virtual DFUEngine::Result RPCGetString(uint8 index, CStringX &descriptor);

	// Generic control requests
	virtual DFUEngine::Result ControlRequest(const Setup &setup,
	                                         void *buffer = 0,
							                 uint16 bufferLength = 0,
	                                         uint16 *replyLength = 0);


	// Hid operations
	virtual DFUEngine::Result RPCEnableHidMode(bool enable = true);

private:
	friend class DFURequestsCOMImpl;
	SmartPtr<DFURequestsCOMImpl> impl_;
};

#endif
