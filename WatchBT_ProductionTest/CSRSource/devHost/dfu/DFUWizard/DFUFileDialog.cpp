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
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/DFUFileDialog.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUWizard.h"
#include "DFUFileDialog.h"
#include "dfu\DFUEngine.h"
#include "dlgs.h"

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
	m_ofn.Flags |= OFN_EXPLORER | OFN_ENABLESIZING;
	SetTemplate(0, IDD_FILE_DIALOG);

	// Set the dialog caption
	caption.Format(bOpenFileDialog
	               ? IDS_FILE_CAPTION_OPEN
				   : IDS_FILE_CAPTION_SAVE);
	m_ofn.lpstrTitle = LPCTSTR(caption);
}

// Data exchange
void DFUFileDialog::DoDataExchange(CDataExchange* pDX)
{
	CFileDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DFUFileDialog)
	DDX_Control(pDX, IDC_FILE_DIALOG_PREVIEW_EDIT, editPreview);
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
	CWnd *parent = GetParent();

	// Hide the filter combo control and label
	HideControl(stc2);
	HideControl(cmb1);

	// Set the OK control text
	CString ok;
	ok.Format(m_bOpenFileDialog
	          ? IDS_FILE_OK_OPEN
			  : IDS_FILE_OK_SAVE);
	parent->GetDlgItem(IDOK)->SetWindowText(ok);

	// Restore the dialog to its original size
	RECT rect;
	parent->GetWindowRect(&rect);
	parent->MoveWindow(rect.left, rect.top,
	                   rect.right - rect.left,
					   (rect.bottom - rect.top) - extraHeight,
					   true);
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

	// Move the preview control
	CWnd *parent = GetParent();
	WINDOWPLACEMENT placement;
	RECT rect;
	parent->GetDlgItem(cmb1)->GetWindowPlacement(&placement);
	rect = placement.rcNormalPosition;
	parent->GetDlgItem(stc2)->GetWindowPlacement(&placement);
	rect.left = placement.rcNormalPosition.left;
	editPreview.MoveWindow(&rect);
}

// Check filename and update preview
BOOL DFUFileDialog::Update()
{
	// Update the preview
	CString comment;
	CStringX xcomment;
	bool valid = false;
	if (GetPathName().IsEmpty()
		|| !DFUEngine::IsDFUFileValid(GetPathName(), xcomment))
	{
		comment.Format(m_bOpenFileDialog
		               ? IDS_FILE_SELECT_NONE_OPEN
					   : IDS_FILE_SELECT_NONE_SAVE);
	}
	else
	{
		comment = xcomment;

		valid = true;
		if (comment.IsEmpty())
		{
			comment.Format(IDS_FILE_SELECT_NO_COMMENT, (LPCTSTR) GetFileName());
		}
	}
	editPreview.SetWindowText(comment);
	editPreview.EnableScrollBarCtrl(SB_VERT, true);
	if (editPreview.GetScrollLimit(SB_VERT) == 0)
	{
		editPreview.EnableScrollBarCtrl(SB_VERT, false);
	}

	// Return whether the file is valid
	return valid;
}
