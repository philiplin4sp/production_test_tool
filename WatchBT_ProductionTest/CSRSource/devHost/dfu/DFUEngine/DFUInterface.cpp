///////////////////////////////////////////////////////////////////////
//
//	File	: DFUInterface.cpp
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
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUInterface.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "thread/critical_section.h"
#include "thread/thread.h"
#include "thread/signal_box.h"
#include "DFUEngine.h"
#include "DFUInterface.h"

// Visual C++ debugging code
#ifdef _DEBUG
#include <crtdbg.h>
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define new DEBUG_NEW
#endif

// Version string
const char *dfuinterface_id = "$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUInterface.cpp#1 $ Copyright (C) 2000-2006 Cambridge Silicon Radio Limited";

class DFUInterfaceImpl
{
public:
	DFUInterfaceImpl();
	DFUInterface *parent;

	// The DFU engine using this implementation
	DFUEngine *engine;

	// Current status
	DFUEngine::Status status;
	mutable CriticalSection lockStatus;

	// Temporary store for thread control information
	DFUEngine::State threadOperation;
	CStringX threadFile;
	bool threadCheckUpload;
	bool threadCheckDownload;

	// The worker thread
	class DFUInterfaceThread : public Threadable
	{
		friend class DFUInterface;
	public:
		DFUInterfaceThread(DFUInterface *p) : parent(p) {}
	private:
		DFUInterface *parent;
		int ThreadFunc()
		{
			DFUInterfaceImpl::Thread(parent);
			return 0;
		}
	};
	friend class DFUInterfaceThread;
	SmartPtr<DFUInterfaceThread> thread;
	// Thread control
	static void Thread(DFUInterface *dfu);
};

// Constructor
DFUInterface::DFUInterface(DFUEngine *engine)
: impl_(new DFUInterfaceImpl)
{
	impl_->parent = this;

	// Initialise the status
	DFUEngine::Status status;
	status.state = DFUEngine::idle;
	SetStatus(status);

	// Store the DFU engine pointer
	impl_->engine = engine;
}

DFUInterfaceImpl::DFUInterfaceImpl()
{
}

// Destructor
DFUInterface::~DFUInterface()
{
	// Terminate any active thread
	Terminate();
}

// Start operations
bool DFUInterface::StartReconfigure(bool checkUpload,
	                                bool checkDownload,
						            const TCHAR *checkFile)
{
	return Start(DFUEngine::reconfigure, checkFile,
	             checkUpload, checkDownload);
}

bool DFUInterface::StartUpload(const TCHAR *file)
{
	return Start(DFUEngine::upload, file);
}

bool DFUInterface::StartDownload(const TCHAR *file)
{
	return Start(DFUEngine::download, file);
}

bool DFUInterface::StartManifest()
{
	return Start(DFUEngine::manifest);
}

// Callback control
void DFUInterface::SetCallback(DFUEngine::Callback *callback)
{
	this->callback = callback;
}

DFUEngine::Callback *DFUInterface::GetCallback() const
{
	return callback;
}

// Status access
void DFUInterface::SetStatus(const DFUEngine::Status &status)
{
	CriticalSection::Lock(impl_->lockStatus);
	impl_->status = status;
}

void DFUInterface::GetStatus(DFUEngine::Status &status) const
{
	CriticalSection::Lock(impl_->lockStatus);
	status = impl_->status;
}


// HID operations
bool DFUInterface::EnableHidMode(bool enable)
{
	//Not implemented in the base interface;
	return false;
}

// Status and callback handling
void DFUInterface::Started(DFUEngine::State state)
{
	// Set the status
	DFUEngine::Status status;
	status.state = state;
	SetStatus(status);

	// Trigger a callback
	if(callback) callback->Started(impl_->engine, status);
}

void DFUInterface::Progress(int percent)
{
	// Set the status
	DFUEngine::Status status;
	GetStatus(status);
	if (status.percent < percent)
	{
		// Set the new percentage completed
		status.percent = percent;
		SetStatus(status);

		// Trigger a callback
		if(callback) callback->Progress(impl_->engine, status);
	}
}

void DFUInterface::Progress(const DFUEngine::Activity &activity)
{
	// Set the status
	DFUEngine::Status status;
	GetStatus(status);
	status.activity = activity;
	SetStatus(status);

	// Trigger a callback
	if(callback) callback->Progress(impl_->engine, status);
}

void DFUInterface::Complete(const DFUEngine::Result &result)
{
	// Update the status for completion
	DFUEngine::Status preStatus;
	GetStatus(preStatus);
	preStatus.result = result;
	preStatus.percent = 100;

	// Set the status to idle
	DFUEngine::Status postStatus(preStatus);
	postStatus.state = DFUEngine::idle;
	SetStatus(postStatus);

	// Trigger a callback using the previous status
	if(callback) callback->Complete(impl_->engine, preStatus);
}

// Simplified interfaces to update the activity progress
void DFUInterface::Progress(DFUEngine::Operation operation, int bytesDone, int bytesTotal)
{
	Progress(DFUEngine::Activity(operation, bytesDone, bytesTotal));
}

void DFUInterface::Progress(DFUEngine::Operation operation, const DFUEngine::Result &result)
{
	Progress(DFUEngine::Activity(operation, result));
}

void DFUInterface::Progress(DFUEngine::SubOperation subOperation, int baudRate)
{
	// Preserve the top-level operation
	DFUEngine::Status status;
	GetStatus(status);
	Progress(DFUEngine::Activity(status.activity, subOperation, baudRate));
}

void DFUInterface::Progress(DFUEngine::SubOperation subOperation, const TCHAR *device)
{
	// Preserve the top-level operation
	DFUEngine::Status status;
	GetStatus(status);
	Progress(DFUEngine::Activity(status.activity, subOperation, device));
}

// Termination event for threads
DFUEngine::Result DFUInterface::CheckAbort(uint32 sleep)
{
	Event *pthreadAbort = &threadAbort;
	return Event::CheckMultiple(1, &pthreadAbort, sleep) != -1
	       ? DFUEngine::aborted
		   : DFUEngine::success;
}

// Kill thread before destruction of derived classes
void DFUInterface::Terminate(bool abort)
{
	// Terminate any active thread
	if (impl_->thread)
	{
		threadAbort.Set();
		impl_->thread->Kill(10000);	// this seems very long, but it really can take > 1 second to abort!
		impl_->thread = SmartPtr<DFUInterfaceImpl::DFUInterfaceThread>();
	}
}

// Thread control
bool DFUInterface::Start(DFUEngine::State operation,
						 const TCHAR *file,
						 bool checkUpload,
						 bool checkDownload)
{
	// Unable to start if previous operation still active
	DFUEngine::Status status;
	GetStatus(status);
	if (status.state != DFUEngine::idle) return false;

	// Ensure that previous thread has completed
	Terminate(false);

	// Start a new thread
	impl_->threadOperation = operation;
	impl_->threadFile = file;
	impl_->threadCheckUpload = checkUpload;
	impl_->threadCheckDownload = checkDownload;
	threadAbort.Unset();
	impl_->thread = SmartPtr<DFUInterfaceImpl::DFUInterfaceThread>(new DFUInterfaceImpl::DFUInterfaceThread(this));
	if (!impl_->thread) return false;
	if(!impl_->thread->Start()) return false;

	// Successful if this point reached
	return true;
}

void DFUInterfaceImpl::Thread(DFUInterface *dfu)
{
	// Set the initial status
	dfu->Started(dfu->impl_->threadOperation);

	// Perform the operation
	DFUEngine::Result result;
	DFUFile file;
	switch (dfu->impl_->threadOperation)
	{
	case DFUEngine::reconfigure:
		if (dfu->impl_->threadFile.IsEmpty())
		{
			result = dfu->DoReconfigure(dfu->impl_->threadCheckUpload,
			                            dfu->impl_->threadCheckDownload, 0);
			if (!result) dfu->DoRecover(result);
		}
		else
		{
			result = file.ReadFile(dfu->impl_->threadFile);
			if (result)
			{
				result = dfu->DoReconfigure(dfu->impl_->threadCheckUpload,
											dfu->impl_->threadCheckDownload,
											&file);
				if (!result) dfu->DoRecover(result);
			}
		}
		break;

	case DFUEngine::upload:
		result = dfu->DoUpload(file);
		if (result) result = file.WriteFile(dfu->impl_->threadFile);
		if (!result
			&& (result.GetCode() != DFUEngine::fail_no_upload)
			&& (result.GetCode() != DFUEngine::fail_dfu_firmware))
		{
			dfu->DoRecover(result);
		}
		break;

	case DFUEngine::download:
		result = file.ReadFile(dfu->impl_->threadFile);
		if (result) result = dfu->DoDownload(file);
		if (!result) dfu->DoRecover(result);
		break;

	case DFUEngine::manifest:
		result = dfu->DoManifest();
		if (!result) dfu->DoRecover(result);
		break;

	default:
		result = DFUEngine::success;
		break;
	}

	// Set the final status
	dfu->Progress(DFUEngine::ready, result);
	dfu->Complete(result);
}

#include "thread/critical_section.h"
#include "thread/signal_box.h"
#include <assert.h>

class EventImpl
{
public:
	EventImpl() : items(NULL), signalled(false) {}
	CriticalSection lock;
	bool signalled;
	struct Items
	{
		SignalBox::Item *item;
		Items *next;
	} *items;

	static void Cleanup(unsigned int len, DFUInterface::Event **events, SmartPtr<SignalBox::Item> *items);
};

DFUInterface::Event::Event() : impl_(new EventImpl)
{
}

DFUInterface::Event::~Event()
{
	assert(!impl_->items);
	delete impl_;
}

void DFUInterface::Event::Set()
{
	CriticalSection::Lock l(impl_->lock);
	for(EventImpl::Items *i = impl_->items; i; i = i->next)
	{
		i->item->set();
	}
	impl_->signalled = true;
}

void DFUInterface::Event::Unset()
{
	CriticalSection::Lock l(impl_->lock);
	for(EventImpl::Items *i = impl_->items; i; i = i->next)
	{
		i->item->unset();
	}
	impl_->signalled = false;
}

int DFUInterface::Event::CheckMultiple(unsigned int len, Event **events, uint32 sleep)
{
	int ret = -1;
	SignalBox box;
	{
		SmartPtr<SmartPtr<SignalBox::Item>, true> items(new SmartPtr<SignalBox::Item>[len]);
		unsigned int i;
		for(i = 0; i<len; ++i) items[i] = SmartPtr<SignalBox::Item>(new SignalBox::Item(box));

#if !defined _WINCE && !defined _WIN32_WCE
		try
#endif
		{
			for(i = 0; i < len; ++i)
			{
				CriticalSection::Lock l(events[i]->impl_->lock);
				EventImpl::Items *item = new EventImpl::Items;
				item->item = items[i];
				item->next = events[i]->impl_->items;
				events[i]->impl_->items = item;

				if(events[i]->impl_->signalled) items[i]->set();
			}
			if(box.wait(sleep))
			{
				for(i = 0; i < len && ret == -1; ++i)
				{
					if(items[i]->getState()) ret = i;
				}
			}
		}
#if !defined _WINCE && !defined _WIN32_WCE
		catch(...)
		{
			EventImpl::Cleanup(len, events, items);
			throw;
		}
#endif
		EventImpl::Cleanup(len, events, items);
	}
	return ret;

}

void EventImpl::Cleanup(unsigned int len, DFUInterface::Event **events, SmartPtr<SignalBox::Item> *items)
{
	for(unsigned int i = 0; i < len; ++i)
	{
		CriticalSection::Lock l(events[i]->impl_->lock);
		EventImpl::Items **pitem = &events[i]->impl_->items;
		while(*pitem && (*pitem)->item != items[i]) pitem = &(*pitem)->next;
		if(*pitem)	// did we find it?
		{
			assert((*pitem)->item == items[i]);
			EventImpl::Items *item = *pitem;
			*pitem = item->next;
			delete item;
		}
	}
}
