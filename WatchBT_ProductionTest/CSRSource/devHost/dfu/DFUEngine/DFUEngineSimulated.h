///////////////////////////////////////////////////////////////////////
//
//	File	: DFUEngineSimulated.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFUEngineSimulated
//
//	Purpose	: Implementation of DFU engine functionality specific to
//			  a simulated connection. This does not communicate with
//			  a BlueCore module; it just provides realistic delays and
//			  responses.
//
//			  The constructor should be called with a pointer to the
//			  DFUEngine object that is providing an interface to the
//			  client. This is required to enable the callbacks to
//			  return this pointer to the client.
//
//			  See the definition of DFUInterface for the operations
//			  supported.
//
//	Modification history:
//	
//		1.1		29:Sep:00	at	File created.
//		1.2		12:Oct:00	at	Smart pointer constructor used for
//								polymorphic pointer assignment.
//		1.3		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.4		23:Jul:01	at	Added version string.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUEngineSimulated.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef DFUENGINESIMULATED_H
#define DFUENGINESIMULATED_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "DFUEngineBase.h"

// DFUEngineSimulated class
class DFUEngineSimulated : public DFUEngineBase  
{
public:

	// Constructor
	DFUEngineSimulated(DFUEngine *engine);

	// Destructor
	virtual ~DFUEngineSimulated();
};

#endif
