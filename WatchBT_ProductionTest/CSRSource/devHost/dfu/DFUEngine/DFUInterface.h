///////////////////////////////////////////////////////////////////////
//
//	File	: DFUInterface.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFUInterface
//
//	Purpose	: Interface to the implementation of the DFU engine for a
//			  particular transport. This provides basic multi-threading
//			  and thread-safety support.
//
//			  The constructor should be called with a pointer to the
//			  DFUEngine object that is providing an interface to the
//			  client. This is required to enable the callbacks to
//			  return this pointer to the client.
//
//			  The user of this class can use the following member
//			  functions to start operations:
//				StartReconfigure	- Establish a connection to the
//									  device and reconfigure it for
//									  firmware upgrade. This can
//									  optionally check whether the
//									  device is capable of performing
//									  firmware upload or download, and
//									  whether the specified DFU file
//									  is suitable for the device.
//				StartUpload			- Start an upload to the named
//									  file. The file is only created or
//									  overwritten if successful.
//				StartDownload		- Start a download of the named
//									  file.
//				StartManifest		- Reset the device and check
//									  whether the firmware is valid.
//
//			  The following member functions can be used to manage
//			  callbacks:
//				SetCallback			- Set the object to receive
//									  callbacks.
//				GetCallback			- Obtain a pointer to the object
//									  that receives callbacks.
//
//			  The status can be accessed at any time using the
//			  following member functions:
//				SetStatus			- Update the status. This is
//									  intended primarily for use by
//									  the implementation of the
//									  operations, but it may also be
//									  used by the client when no
//									  operations are in progress.
//				GetStatus			- Read the current status.
//
//			  The implementation of the various operations is performed
//			  by a derived class defining the following virtual
//			  functions:
//				DoReconfigure	- Establish a connection, and reset the
//								  device to start the boot loader.
//				DoUpload		- Upload a file from the device to the
//								  host.
//				DoDownload		- Download a file from the host to the
//								  device.
//				DoManifest		- Reset the device to return to normal
//								  operation.
//				DoRecover		- Attempt to restore the device to
//								  normal operation after a failure or
//								  abort.
//
//			  These functions are invoked within their own worker
//			  thread. They should call Progress regularly to indicate
//			  the percentage of the operation that has been completed.
//			  The return value from the function is the result code.
//
//			  The eventThreadAbort object should be checked frequently,
//			  for example by using WaitForSingleObject or a CMultiLock
//			  object, and if signalled the function should tidy up and
//			  return as quickly as possible. This is used if the DFU
//			  engine is destroyed by the client application. The
//			  CheckAbort function may also be used for this purpose.
//
//			  The following member function should be called before
//			  any derived class that implements virtual functions is
//			  destroyed:
//				Terminate		- Kill the worker thread.
//
//	Modification history:
//	
//		1.1		25:Sep:00	at	File created.
//		1.2		28:Sep:00	at	Moved DFUFile reading and writing from
//								derived class. Improved Progress
//								reporting. Ensured destruction is
//								thread safe.
//		1.3		12:Oct:00	at	Expanded the comment describing how
//								to use this class.
//		1.4		20:Oct:00	at	Recovery action also invoked for failed
//								downloads.
//		1.5		31:Oct:00	at	Status set to idle before the Complete
//								callback is called to avoid race
//								condition under Windows 2000.
//		1.6		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.7		14:Dec:00	at	Recovery action not performed if
//								upload failed due to corrupt firmware.
//		1.8		06:Mar:01	at	Added support for activity description.
//		1.9		06:Mar:01	at	Added support for DFURequests to
//								updated the activity description.
//		1.10	06:Mar:01	at	Activity always updated on completion.
//		1.11	13:Mar:01	at	Removed race condition from thread
//								termination checks.
//		1.12	11:Apr:01	at	Increased maximum supported delay.
//		1.13	30:Apr:01	ckl	Added Windows CE support.
//		1.14	23:May:01	at	Tidied up Windows CE support.
//		1.15	23:Jul:01	at	Added version string.
//		1.16	09:Oct:01	at	No recovery if upload unsupported.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUInterface.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef DFUINTERFACE_H
#define DFUINTERFACE_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "DFUFile.h"
#include "../SmartPtr.h"

// Implementation for a specific transport
class DFURequests;

// DFUInterface class
class DFUInterface
{
public:

	// Constructor
	DFUInterface(DFUEngine *engine);

	// Destructor
	virtual ~DFUInterface();

	// Start operations
	bool StartReconfigure(bool checkUpload = false,
	                      bool checkDownload = false,
						  const TCHAR *checkFile = _T(""));
	bool StartUpload(const TCHAR *file);
	bool StartDownload(const TCHAR *file);
	bool StartManifest();
	
	// Callback control
	void SetCallback(DFUEngine::Callback *callback = 0);
	DFUEngine::Callback *GetCallback() const;

	// Status access
	void SetStatus(const DFUEngine::Status &status);
	void GetStatus(DFUEngine::Status &status) const;

	// HID operations
	virtual bool EnableHidMode(bool enable = true);

	// abort event interface.
	class Event
	{
	public:
		Event();
		~Event();

		void Set();
		void Unset();
		static int CheckMultiple(unsigned int len, Event **events, uint32 sleep);

	private:
		friend class EventImpl;
		EventImpl *impl_;

	};

protected:

	// Status and callback handling
	virtual void Started(DFUEngine::State state);
	virtual void Progress(int percent);
	virtual void Progress(const DFUEngine::Activity &activity);
	virtual void Complete(const DFUEngine::Result &result);

	// Simplified interfaces to update the activity progress
	void Progress(DFUEngine::Operation operation, int bytesDone = 0, int bytesTotal = 0);
	void Progress(DFUEngine::Operation operation, const DFUEngine::Result &result);
	void Progress(DFUEngine::SubOperation subOperation, int baudRate = 0);
	void Progress(DFUEngine::SubOperation subOperation, const TCHAR *device);

	// Implementation of operations
	virtual DFUEngine::Result DoReconfigure(bool checkUpload = false,
	                                        bool checkDownload = false,
											DFUFile *checkFile = 0) = 0;
	virtual DFUEngine::Result DoUpload(DFUFile &file) = 0;
	virtual DFUEngine::Result DoDownload(DFUFile &file) = 0;
	virtual DFUEngine::Result DoManifest() = 0;
	virtual DFUEngine::Result DoRecover(const DFUEngine::Result &failure) = 0;

	virtual DFUEngine::Result CheckAbort(uint32 sleep = 0);

	// Kill thread before destruction of derived classes
	void Terminate(bool abort = true);

	// Termination event for threads
	Event threadAbort;

private:
	friend class DFUInterfaceImpl;
	SmartPtr<DFUInterfaceImpl> impl_;

	// Thread control
	bool Start(DFUEngine::State operation, const TCHAR *file = _T(""),
	           bool checkUpload = false, bool checkDownload = false);

	// Allow the implementation to perform progress callbacks
	friend DFURequests;

	DFUEngine::Callback *callback;
};

#endif
