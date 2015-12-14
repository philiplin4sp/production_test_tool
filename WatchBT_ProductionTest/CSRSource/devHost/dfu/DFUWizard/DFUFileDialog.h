///////////////////////////////////////////////////////////////////////
//
//	File	: DFUFileDialog.cpp
//		  Copyright (C) 2001-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFUFileDialog
//
//	Purpose	: A replacement for the CFileDialog class that adds DFU
//			  file related functionality.
//
//			  The main enhancements are that the filter combo is
//			  replaced by a description of the selected file, and only
//			  valid DFU files are accepted.
//
//			  The only change to the API is that the constructor takes
//			  two less paramters - the default extension and filter
//			  list are automatically set. Consult the MFC documentation
//			  for the CFileDialog class for more usage information.
//
//	Modification history:
//	
//		1.1		26:Feb:01	at	File created.
//		1.2		28:Feb:01	at	Removed disabling of Open button - it
//								directory change from the keyboard.
//								Preview updated on file name check.
//								Added support for saving files.
//		1.3		23:Mar:01	at	Added directory to DFUEngine include.
//		1.4		30:Apr:01	ckl	Added Windows CE support.
//		1.5		01:May:01	ckl	Backed out the previous change.
//		1.6		17:May:01	at	Updated the change history.
//		1.7		14:Nov:01	at	Added basic Unicode support.
//		1.8		14:Feb:01	at	Scroll bar added if necessary.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/DFUFileDialog.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef DFUFILEDIALOG_H
#define DFUFILEDIALOG_H

#if _MSC_VER > 1000
#pragma once
#endif

// DFUFileDialog class
class DFUFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(DFUFileDialog)

public:

	// Constructor
	DFUFileDialog(BOOL bOpenFileDialog,
	              LPCTSTR lpszFileName = NULL,
	              DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	              CWnd *pParentWnd = NULL);

	//{{AFX_DATA(DFUFileDialog)
	CEdit	editPreview;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(DFUFileDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	// Virtual function overrides
	virtual BOOL OnInitDialog();
	virtual void OnInitDone();
	virtual BOOL OnFileNameOK();
	virtual void OnFileNameChange();

	//{{AFX_MSG(DFUFileDialog)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	// The additional height of the template
	LONG extraHeight;

	// Dialog caption
	CString caption;

	// Check filename and update preview
	BOOL Update();
};

//{{AFX_INSERT_LOCATION}}

#endif
