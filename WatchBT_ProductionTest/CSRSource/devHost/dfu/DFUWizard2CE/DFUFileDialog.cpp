///////////////////////////////////////////////////////////////////////
//
//	File	: DFUFileDialog.cpp
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
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard2CE/DFUFileDialog.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUFileDialog.h"
#include "dfu\DFUEngine.h"
#include "resource.h"

// Visual C++ debugging code
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// File filters
static const TCHAR ext_default[] = _T("dfu");
static const TCHAR ext_filter[] = _T("DFU files (*.dfu)|*.dfu|All files (*.*)|*.*||");

IMPLEMENT_DYNAMIC(DFUFileDialog, CFileDialog)

// Constructor
DFUFileDialog::DFUFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszFileName,
							 DWORD dwFlags, CWnd* pParentWnd)
: CFileDialog(bOpenFileDialog, ext_default, lpszFileName, dwFlags,
			  ext_filter, pParentWnd)
{
	//{{AFX_DATA_INIT(DFUFileDialog)
	//}}AFX_DATA_INIT

	// Specify a template containing the additional controls
	SetTemplate(0, IDD_FILE_DIALOG);

	// Set the dialog caption
	caption.Format(bOpenFileDialog
	               ? IDS_FILE_CAPTION_OPEN
				   : IDS_FILE_CAPTION_SAVE);
}

// Data exchange
void DFUFileDialog::DoDataExchange(CDataExchange* pDX)
{
	CFileDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DFUFileDialog)
	//}}AFX_DATA_MAP
}

// Message map
BEGIN_MESSAGE_MAP(DFUFileDialog, CFileDialog)
	//{{AFX_MSG_MAP(DFUFileDialog)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Virtual function overrides
BOOL DFUFileDialog::OnFileNameOK()
{
	// Let the base class check the name first
	if (CFileDialog::OnFileNameOK())
	{
		return TRUE;
	}

	// Check if the selected file is a valid DFU file
	if (!Update() && (m_ofn.Flags & OFN_FILEMUSTEXIST))
	{
		CString message;
		message.Format(m_ofn.Flags & OFN_EXTENSIONDIFFERENT
		               ? IDP_FILE_NOT_DFU
					   : IDP_FILE_NOT_VALID,
					   GetPathName());
		AfxMessageBox(message, MB_OK | MB_ICONEXCLAMATION);
		return TRUE;
	}

	// The file appears to be valid
	return FALSE;
}

BOOL DFUFileDialog::OnInitDialog()
{
	// Pass on to base class
	CFileDialog::OnInitDialog();

	// Record the additional height added to the dialog
	RECT rect;
	GetWindowRect(&rect);
	extraHeight = rect.bottom - rect.top;

	// Focus not explicitly set
	return true;
}

void DFUFileDialog::OnInitDone()
{
	// Set the OK control text
	CString ok;
	ok.Format(m_bOpenFileDialog
	          ? IDS_FILE_OK_OPEN
			  : IDS_FILE_OK_SAVE);
	char * string = new char[ok.GetLength()];
	BOOL default_char_used;
	char space = ' ';
	WideCharToMultiByte(0, 0, ok,
						ok.GetLength(),
						string,
						ok.GetLength(),
						&space, 
						&default_char_used);
	SetControlText(IDOK, string);
	
	// Restore the dialog to its original size
	CWnd *parent = GetParent();
	RECT rect;
	parent->GetWindowRect(&rect);
	parent->MoveWindow(rect.left, rect.top,
	                   rect.right - rect.left,
					   (rect.bottom - rect.top) - extraHeight,
					   true);

	delete [] string;
}

void DFUFileDialog::OnFileNameChange()
{
	// Notify the base class first
	CFileDialog::OnFileNameChange();

	// Update the preview
	Update();
}

void DFUFileDialog::OnSize(UINT nType, int cx, int cy) 
{
	// Pass on to the base class first
	CFileDialog::OnSize(nType, cx, cy);
}

// Check filename and update preview
BOOL DFUFileDialog::Update()
{
	// Determine whetehr valid
	bool valid = false;
	CStringX comment;
	if (!GetPathName().IsEmpty()
		&& DFUEngine::IsDFUFileValid(GetPathName(), comment))
	{
		valid = true;
	}

	// Return whether the file is valid
	return valid;

}