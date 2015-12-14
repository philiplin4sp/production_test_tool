///////////////////////////////////////////////////////////////////////
//
//	File	: UpgradeDialog.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: UpgradeDialog
//
//	Purpose	: Perform a continuous cycle of upgrades until an error
//			  occurs.
//
//			  The following member variables should be set before
//			  invoking the dialog:
//				dfuConnection		- The name of the COM port or USB
//									  device.
//				dfuDownload			- Name of file or directory to
//									  download, or empty for none.
//
//	Modification history:
//	
//		1.1		26:Jul:02	doc	Created from Alex Thoukydides' class in DFUTest
//		1.2		16:Sep:02	doc	Cut down from Windows version to one suitable for CE
//		1.3		11:Oct:02	doc	Further trimming
//		1.4		30:Oct:02	doc	Updated modification history
//		1.5		20:Nov:02	doc	Added ClearDisplay() function
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard2CE/UpgradeDialog.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef UPGRADEDIALOG_H
#define UPGRADEDIALOG_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "dfu\DFUEngine.h"

// UpgradeDialog class
class UpgradeDialog : public CDialog, public DFUEngine::Callback
{
public:

	// Options
	CString dfuConnection;
	CString dfuUpload;
	CString dfuDownload;

	// Constructor
	UpgradeDialog(CWnd* pParent = 0);

	// Callback functions
	virtual void Started(DFUEngine *engine, const DFUEngine::Status &status);
	virtual void Progress(DFUEngine *engine, const DFUEngine::Status &status);
	virtual void Complete(DFUEngine *engine, const DFUEngine::Status &status);

	//{{AFX_DATA(UpgradeDialog)
	enum { IDD = IDD_UPGRADE_DIALOG };
	CEdit	EditLog;
	CProgressCtrl	progressUpgrade;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(UpgradeDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

protected:
	CString EditLogText;

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

		// Generic callback request
	void CallbackRequest(CallbackType type, DFUEngine *engine,
	                     const DFUEngine::Status &status);

	void DoStarted(DFUEngine *engine, const DFUEngine::Status &status);
	void DoProgress(DFUEngine *engine, const DFUEngine::Status &status);
	void DoComplete(DFUEngine *engine, const DFUEngine::Status &status);

	// List of download files
	CStringList dfuDownloadList;
	POSITION dfuDownloadPos;

	// File names currently being used
	CString dfuUploadFile;
	CString dfuDownloadFile;

	// The DFU engine
	DFUEngine dfu;
	bool dfuActive;

	// Time operation started
	CTime timeStart;

	// Window caption
	CString textTitle;
	bool flashTitle;
	UINT flashTimer;

	void RefreshTitle();

	// Window resizing
	CSize minSize;
	CSize prevSize;
	void Resize(const CSize &delta);
	void Resize(CWnd *control, const CSize &delta, bool move = true);

	// Reset the list of download files and statistics
	void Reset();

	// Start and stop upgrade 
	void Start();
	void Stop(bool disable = true);
	 
	// Add an entry to the log
	COLORREF red;
	COLORREF green;
	COLORREF blue;
	int logTemporary_Min;
	int logTemporary_Max;
	void Log(CString text);
	void Log(CString text, int& first_char, int&last_char);
	void LogComment(const CString &comment);
	void LogComment(UINT comment);
	void LogStart(UINT action, const CString &file = "");
	void LogEnd(const DFUEngine::Status &status, const CTimeSpan &time, const CString &comment = "");
	void LogTemporary(const CString &info);
	void ClearDisplay();

	// Display of a message box
	int MessageBox(const CString &message, UINT type = MB_OK | MB_ICONEXCLAMATION);
	
	//{{AFX_MSG(UpgradeDialog)
	afx_msg void OnChangedWindow();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnOK();
//	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg LRESULT OnCallback(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif
