///////////////////////////////////////////////////////////////////////
//
//	File	: DFURequestsSimulated.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFURequestsSimulated
//
//	Purpose	: Implementation of basic DFU operations specific to
//			  a simulated connection. This does not communicate with
//			  a BlueCore module; it just provides realistic delays and
//			  responses.
//
//			  All of the methods are implementations of virtual
//			  functions provided by DFURequests, so see that file for
//			  descriptions.
//
//	Modification history:
//	
//		1.1		02:Oct:00	at	File created.
//		1.2		12:Oct:00	at	Expanded comment describing this class.
//		1.3		24:Oct:00	at	Corrected scoping of DFU interface
//								number.
//		1.4		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.5		23:May:01	at	Corrected change history.
//		1.6		23:Jul:01	at	Added version string.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFURequestsSimulated.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef DFUREQUESTSSIMULATED_H
#define DFUREQUESTSSIMULATED_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "DFURequests.h"

// DFURequestsSimulated class
class DFURequestsSimulated : public DFURequests
{
public:

	// Constructor
	DFURequestsSimulated();

	// Destructor
	virtual ~DFURequestsSimulated();

protected:

	// Individual USB DFU control requests
	virtual DFUEngine::Result RPCDetach(uint16 timeout);
	virtual DFUEngine::Result RPCDnload(uint16 blockNum,
	                                    const void *buffer = 0,
										uint16 bufferLength = 0);
	virtual DFUEngine::Result RPCUpload(uint16 blockNum,
	                                    void *buffer,
										uint16 bufferLength,
										uint16 &replyLength);
	virtual DFUEngine::Result RPCGetStatus(DFUStatus &status);
	virtual DFUEngine::Result RPCClrStatus();
	virtual DFUEngine::Result RPCGetState(uint8 &state);
	virtual DFUEngine::Result RPCAbort();

	// Non USB DFU control requests
	virtual DFUEngine::Result RPCReset();
	virtual DFUEngine::Result RPCGetInterfaceDFU(InterfaceDescriptor &descriptor);
	virtual DFUEngine::Result RPCGetDevice(DeviceDescriptor &descriptor);
	virtual DFUEngine::Result RPCGetFunct(DFUFunctionalDescriptor &descriptor);
	virtual DFUEngine::Result RPCGetString(uint8 index,
	                                       CStringX &descriptor);

	// Generic control requests
	virtual DFUEngine::Result ControlRequest(const Setup &setup,
	                                         void *buffer = 0,
							                 uint16 bufferLength = 0,
	                                         uint16 *replyLength = 0);

	// Hid operations
	virtual DFUEngine::Result RPCEnableHidMode(bool enable);

private:

	// Current state
	uint8 dfuState;
	uint8 dfuStateNext;
	uint32 dfuStateTimeout;

	// Current status
	uint8 dfuStatus;

	// Upload progress
	uint32 uploadDone;

	// Download progress
	uint32 downloadDone;
	uint32 downloadPending;
	bool downloadValid;

	// Manifest progress
	bool manifestPending;

	// State transitions
	void SetState(uint8 state);
	void SetState(uint8 state, uint8 stateNext, uint32 timeout);
	void UpdateState();

	// Status handling
	DFUEngine::Result SetError(uint8 status);
};

#endif
