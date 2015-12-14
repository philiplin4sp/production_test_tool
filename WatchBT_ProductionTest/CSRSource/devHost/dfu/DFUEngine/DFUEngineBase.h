///////////////////////////////////////////////////////////////////////
//
//	File	: DFUEngineBase.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFUEngineBase
//
//	Purpose	: Implementation of DFU engine functionality that is
//			  common for all transports.
//
//			  Derived classes implementing specific transports
//			  should initialise the following member variable from
//			  their constructor:
//				transport	- The implementation of the transport.
//
//			  Since a smart pointer is used, there is no need to
//			  explicitly delete the transport object; it will happen
//			  automatically during the destruction of this object.
//			  The Terminate function from the base class is called
//			  before the tranport is deleted.
//
//			  See the definition of DFUInterface for the operations
//			  supported.
//
//	Modification history:
//	
//		1.1		25:Sep:00	at	File created.
//		1.2		28:Sep:00	at	Moved DFUFile reading and writing to
//								base class, and implemented real DFU
//								operations.
//		1.3		12:Oct:00	at	The recovery operation attempts to end
//								and restart the transport if problems
//								are encountererd.
//		1.4		20:Oct:00	at	Stalled result codes converted to
//								actual errors. Completion of download
//								operations prior to requesting the
//								status now handled sensibly. Attempts
//								to abort operations or clear status
//								only made if likely to be successful.
//		1.5		24:Oct:00	at	Updated following changes to
//								DFURequests.
//		1.6		02:Nov:00	at	Disabled delay after final reset.
//		1.7		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.8		30:Nov:00	at	Increased detach timeout.
//		1.9		14:Dec:00	at	Abort not checked during delay after
//								download operation. Alternative status
//								checking behaviour removed.
//		1.10	15:Dec:00	at	Removed incorrect clearing of status
//								after failed upload.
//		1.11	02:Mar:01	at	Added missing check of return code.
//		1.12	05:Mar:01	at	Added delay at start of recovery
//								action and support for activity
//								description.
//		1.13	06:Mar:01	at	Added support for DFURequests to
//								updated the activity description.
//		1.14	22:Mar:01	at	More accurate poll timeouts.
//		1.15	23:Mar:01	at	More efficient accurate sleeping.
//		1.16	11:Apr:01	at	Increased maximum supported delay.
//		1.17	12:Apr:01	at	Improved download activity description.
//								Retries used for reconfigure.
//		1.18	20:Apr:01	at	Remaining time indicated for long
//								waits during download.
//		1.19	30:Apr:01	ckl	Added Windows CE support.
//		1.20	23:May:01	at	Tidied Windows CE support.
//		1.21	23:Jul:01	at	Added version string.
//		1.22	06:Oct:01	at	Added support for Widcomm USB driver.
//		1.23	30:Jul:02	at	Disconnect at end of manifest.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUEngineBase.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef DFUENGINEBASE_H
#define DFUENGINEBASE_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "DFUInterface.h"
#include "DFURequests.h"
#include "../SmartPtr.h"

// DFUEngineBase class
class DFUEngineBase : public DFUInterface
{
public:

	// Constructor
	DFUEngineBase(DFUEngine *engine);

	// Destructor
	virtual ~DFUEngineBase();

protected:

	// The DFU transport implementation
	SmartPtr<DFURequests> transport;

private:

	// Information required for DFU upload
	uint16 appVendor;
	uint16 appProduct;
	uint16 appDevice;

	// Accurate sleeping
	void AccurateSleep(uint32 milliseconds);
	void AccurateSleep(uint32 milliseconds, DFUEngine::SubOperation subOperation);

	// Status and callback handling
	DFUEngine::Result ProgressCheck(int percent, uint32 sleep = 0);

	// Implementation of operations
	virtual DFUEngine::Result DoReconfigure(bool checkUpload = false,
	                                        bool checkDownload = false,
											DFUFile *checkFile = 0);
	virtual DFUEngine::Result DoUpload(DFUFile &file);
	virtual DFUEngine::Result DoDownload(DFUFile &file);
	virtual DFUEngine::Result DoManifest();
	virtual DFUEngine::Result DoRecover(const DFUEngine::Result &failure);

	//Hid operations
	virtual bool EnableHidMode(bool enable = true);

	// Composite operations
	DFUEngine::Result DoConnect(bool hintDFU, bool abort = true);
	DFUEngine::Result DoDFUIdle(bool clear = false, bool abort = false);
	DFUEngine::Result DoDownloadStatus(DFURequests::DFUStatus &status);

	// Status checking
	bool IsDFUMode(const DFURequests::DeviceDescriptor &dev_descriptor, const DFURequests::InterfaceDescriptor &if_descriptor);
};

#endif
