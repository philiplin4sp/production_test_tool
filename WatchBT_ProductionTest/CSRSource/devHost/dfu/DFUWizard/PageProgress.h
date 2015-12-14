///////////////////////////////////////////////////////////////////////
//
//	File	: PageProgress.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: PageProgress
//
//	Purpose	: DFU wizard page displaying the status of the upgrade
//			  procedure. This uses the settings from the previous
//			  wizard pages to control the operation performed. When
//			  the procedure is complete, the IDD_PAGE_RESULTS page
//			  will be selected automatically. The following member
//			  variables should be set before this page is activated:
//				valueNoAbort	- Disable the abort button.
//
//	Modification history:
//	
//		1.1		20:Sep:00	at	File created.
//		1.2		28:Sep:00	at	Simplified sequencing code and
//								improved generation of failure
//								messages. Added support for HCI
//								tunnelling.
//		1.3		09:Oct:00	at	Added support for details button on
//								results page.
//		1.4		19:Oct:00	at	Updated relative timings, and prevented
//								incorrect upload.
//		1.5		24:Oct:00	at	Corrected debugging macros. Improved
//								behaviour if firmware corrupt and
//								upload attempted.
//		1.6		25:Oct:00	at	Added display of total time taken if
//								successful.
//		1.7		31:Oct:00	at	Return codes from DFUEngine methods
//								checked and handled.
//		1.8		02:Oct:00	at	Set relative timings based on upgrade
//								via UART at 115200 baud. Different
//								animations used for each stage of the
//								upgrade.
//		1.9		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.10	08:Dec:00	at	Added control of BCSP link
//								establishment.
//		1.11	15:Dec:00	at	Added (but not used) comment control.
//		1.12	06:Mar:01	at	Comment control updated.
//		1.13	23:Mar:01	at	Added directory to DFUEngine include.
//		1.14	30:Apr:01	ckl	Added Windows CE support.
//		1.15	01:May:01	ckl	Backed out the previous change.
//		1.16	17:May:01	at	Updated the change history.
//		1.17	29:May:01	at	Time taken formatted as minutes and
//								seconds only.
//		1.18	08:Oct:01	at	Added /noabort command line option.
//		1.19	09:Oct:01	at	No upload check during reconfiguration.
//		1.20	11:Jan:02	at	Upload status supplied to results page.
//		1.21	12:Feb:02	at	Path name truncated if necessary.
//		1.22	08:Mar:02	at	Download file name added to MRU list.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/PageProgress.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef PAGEPROGRESS_H
#define PAGEPROGRESS_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "dfu\DFUEngine.h"
#include "../SmartPtr.h"

#include <afxmt.h>
#include <afxtempl.h>

// PageProgress class
class PageProgress : public DFUPage, public DFUEngine::Callback
{
	DECLARE_DYNCREATE(PageProgress)

public:

	// Disable the abort button
	bool valueNoAbort;

	// Constructor
	PageProgress();

	// Button management
	virtual LRESULT GetPageBack();
	virtual LRESULT GetPageNext();

	// DFUEngine callback functions
	virtual void Started(DFUEngine *engine, const DFUEngine::Status &status);
	virtual void Progress(DFUEngine *engine, const DFUEngine::Status &status);
	virtual void Complete(DFUEngine *engine, const DFUEngine::Status &status);

	// Callback support functions
	int ProgressTime(DFUEngine::State state);
	void CompleteSuccess(const DFUEngine::Status &status);
	void CompleteSuccessLast(const DFUEngine::Status &status);
	void CompleteFail(const DFUEngine::Status &status);

	//{{AFX_DATA(PageProgress)
	enum { IDD = IDD_PAGE_PROGRESS };
	CStatic	staticComment;
	CButton	buttonAbort;
	CStatic	staticMain;
	CProgressCtrl progressMain;
	CStatic	staticSub;
	CProgressCtrl progressSub;
	CAnimateCtrl animateProgress;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(PageProgress)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual BOOL OnQueryCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	// The DFU engine
	DFUEngine dfu;

	// The files actually being used
	CString fileUpload;
	CString fileDownload;

	// Time taken for different actions
	CString timeTakenUpload;
	CString timeTakenDownload;
	CString timeTakenTotal;
	CTime timeStart;
	CTime timeEnd;
	CTimeSpan timeTotal;

	// Possible callback types
	enum CallbackType
	{
		started,
		progress,
		complete
	};

	// Objects to hold data for callbacks
	struct CallbackData
	{
		// Constructor
		CallbackData(CallbackType type, DFUEngine *engine,
		     const DFUEngine::Status &status);

		// The data
		CallbackType type;
		DFUEngine *engine;
		DFUEngine::Status status;
	};

	// List of outstanding callbacks
	CList<SmartPtr<CallbackData>, const SmartPtr<CallbackData> &> listData;
	mutable CCriticalSection lockListData;

		// Generic callback request
	void CallbackRequest(CallbackType type, DFUEngine *engine,
	                     const DFUEngine::Status &status);

	// Delayed initialisation
	virtual void OnSetActiveDelayed();

	//{{AFX_MSG(PageProgress)
	afx_msg void OnAbortClicked();
	afx_msg LRESULT OnCallback(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}

#endif
