///////////////////////////////////////////////////////////////////////
//
//	File	: DFUEngineCOM.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFUEngineCOM
//
//	Purpose	: Implementation of DFU engine functionality specific to
//			  a COM port transport.
//
//			  Three constructors are provided to allow the protocol
//			  to be selected:
//				2 parameters	- Autodetect settings.
//				3 parameters	- H4 transport.
//				5 parameters	- BCSP transport with optional
//								  tunnelling and link establishment.
//
//			  The constructor should be called with a pointer to the
//			  DFUEngine object that is providing an interface to the
//			  client. This is required to enable the callbacks to
//			  return this pointer to the client. The second parameter
//			  should be the name of the device to use. This can either
//			  be the raw device name (with or without a leading "\\.\"
//			  prefix), or one of the device descriptions supplied by
//			  GetCOMPortNames. The third parameter should be one of
//			  the baud rates returned by GetCOMBaudRates. The fourth
//			  parameter indicates whether tunnelling should be used
//			  for a BCSP connection.
//
//			  The following member functions can be used to obtain
//			  a list of the available device and baud rates:
//				GetCOMPortNames		- Set the supplied list to a list
//									  of strings describing the
//									  available devices.
//				GetCOMBaudRates		- Set the supplied list to a list
//									  of strings describing the
//									  available baud rates, either on
//									  any port or on the particular
//									  port specified.
//
//			  See the definition of DFUInterface for the operations
//			  supported.
//
//	Modification history:
//	
//		1.1		25:Sep:00	at	File created.
//		1.2		28:Sep:00	at	Started adding real DFU support.
//		1.3		05:Oct:00	at	Abort event object supplied to
//								transport, and smart pointer
//								constructor used for polymorphic
//								pointer assignment. Added option to
//								return baud rates for a specific port.
//		1.4		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.5		30:Nov:00	at	Removed redundant abort event
//								parameter from DFURequestsCOM
//								constructors.
//		1.6		08:Dec:00	at	Added control over BCSP link
//								establishment mode.
//		1.7		30:Apr:01	ckl	Added Windows CE support.
//		1.8		23:May:01	at	Tidied Windows CE support.
//		1.9		23:Jul:01	at	Added version string.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUEngineCOM.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef DFUENGINECOM_H
#define DFUENGINECOM_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "DFUEngineBase.h"

// DFUEngineCOM class
class DFUEngineCOM : public DFUEngineBase  
{
public:

	// Constructors
	DFUEngineCOM(DFUEngine *engine, const TCHAR *port);
	DFUEngineCOM(DFUEngine *engine, const TCHAR *port,
	             const TCHAR *baud);
	DFUEngineCOM(DFUEngine *engine, const TCHAR *port,
	             const TCHAR *baud, bool tunnel,
				 DFUEngine::Link link);

	// Destructor
	virtual ~DFUEngineCOM();

	// COM port and baud rate enumeration
	static int GetCOMPortNames(CStringListX &ports);
	static int GetCOMBaudRates(CStringListX &baudRates,
	                           const TCHAR *port = _T(""));
};

#endif
