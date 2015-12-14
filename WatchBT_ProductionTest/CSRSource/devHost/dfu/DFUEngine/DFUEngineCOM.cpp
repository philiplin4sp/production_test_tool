///////////////////////////////////////////////////////////////////////
//
//	File	: DFUEngineCOM.cpp
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
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUEngineCOM.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUEngineCOM.h"
#include "DFURequestsCOM.h"
#include "resource.h"

// Visual C++ debugging code
#ifdef _DEBUG
#include <crtdbg.h>
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define new DEBUG_NEW
#endif

// Version string
const char *dfuenginecom_id = "$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUEngineCOM.cpp#1 $ Copyright (C) 2000-2006 Cambridge Silicon Radio Limited";

// Constructors
DFUEngineCOM::DFUEngineCOM(DFUEngine *engine, const TCHAR *port)
: DFUEngineBase(engine)
{
	// Create a COM transport with autodetection of protocol
	transport = SmartPtr<DFURequests>(new DFURequestsCOM(port));
}

DFUEngineCOM::DFUEngineCOM(DFUEngine *engine, const TCHAR *port,
                           const TCHAR *baud)
: DFUEngineBase(engine)
{
	// Create a COM transport using H4
	transport = SmartPtr<DFURequests>(new DFURequestsCOM(port, _ttoi(baud)));
}

DFUEngineCOM::DFUEngineCOM(DFUEngine *engine, const TCHAR *port,
                           const TCHAR *baud, bool tunnel,
						   DFUEngine::Link link)
: DFUEngineBase(engine)
{
	// Create a COM transport using BCSP
	transport = SmartPtr<DFURequests>(new DFURequestsCOM(port, _ttoi(baud), tunnel, link));
}

// Destructor
DFUEngineCOM::~DFUEngineCOM()
{
}

// COM port and baud rate enumeration
int DFUEngineCOM::GetCOMPortNames(CStringListX &ports)
{
	// Generate list of COM ports
	return DFURequestsCOM::EnumeratePorts(ports);
}

int DFUEngineCOM::GetCOMBaudRates(CStringListX &baudRates,
	                              const TCHAR *port)
{
	// Generate list of baud rates
	DFURequestsCOM::BaudRateList *list = NULL;
	if (!*port /*port.IsEmpty()*/)
	{
		DFURequestsCOM::EnumerateBaudRates(list);
	}
	else
	{
		CStringListX ports;
		ports.AddTail(port);
		DFURequestsCOM::EnumerateBaudRates(ports, list);
	}

	// Construct the baud rate names
	baudRates.RemoveAll();
	for (DFURequestsCOM::BaudRateList *pos = list; pos != 0; pos=pos->next)
	{
		CStringX name;
		name.Format(GetStringTableEntry(IDS_BAUD_NAME), pos->baud);
		baudRates.AddTail(name);
	}
	DFURequestsCOM::FreeBaudRateList(list);

	// Return the number of available baud rates
	return baudRates.GetCount();
}
