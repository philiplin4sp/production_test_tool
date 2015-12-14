///////////////////////////////////////////////////////////////////////
//
//	File	: PromptDialog.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: PromptDialog
//
//	Purpose	: Display and handle the options dialog.
//
//			  The following member variables can be set before invoking
//			  the dialog and checked after it has closed:
//				valueConnection		- The name of the COM port
//				valueDownload		- Name of file or directory to
//									  download, or empty for none.
//
//	Modification history:
//	
//		1.1		26:Jul:02	doc	File created.
//		1.2		06:Aug:02	doc	Simplified for Windows CE
//		1.3		16:Sep:02	doc	Some minor alterations.
//		1.4		11:Oct:02	doc Added browse and Restore button fxnality
//		1.5		30:oct:02	doc	Updated modification history
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard2CE/PromptDialog.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef PROMPTDIALOG_H
#define PROMPTDIALOG_H

#if _MSC_VER > 1000
#pragma once
#endif

// PromptDialog class
class PromptDialog : public CDialog
{
public:

	// Constructor
	PromptDialog(CWnd* pParent = 0);

	//{{AFX_DATA(PromptDialog)
	enum { IDD = IDD_PROMPT_DIALOG };
	CButton	buttonOK;
	CComboBox	comboConnection;
	CString	valueConnection;
	CString	valueDownload;
	CString	valueDownloadComment;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(PromptDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(PromptDialog)
	afx_msg void OnDownloadButton();
	afx_msg void OnRestoreButton();
	afx_msg void OnChangeCOM();
	afx_msg void OnChange();
	afx_msg void OnTimer(UINT id_event);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif
