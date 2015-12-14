///////////////////////////////////////////////////////////////////////
//
//	File	: DFUFileDialog.h
//		  Copyright (C) 2002-2006 Cambridge Silicon Radio Limited
//
//	Author	: D. Coultous
//
//	Class	: DFUFileDialog
//
//	Purpose	: A simplification of Alex.Thoukydides's replacement 
//			  for the CFileDialog class that adds DFU
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
//		1.1		16:oct:02	doc	Shell created to allow a hostbuild.
//		1.2		16:oct:02	doc	Real creation
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard2CE/DFUFileDialog.h#1 $
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
