///////////////////////////////////////////////////////////////////////
//
//	File	: PageFile.cpp
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: PageFile
//
//	Purpose	: DFU wizard page to prompt the user to select the DFU
//			  file to download. The following member variable
//			  contains the selection:
//				valueName	- The selected file name.
//
//			  In development mode a list of recently used file names
//			  can be provided in the following member variable:
//				listNames	- Recently used file names.
//
//			  The name of the internally saved file for upload or
//			  restore operations is available from another member
//			  variable:
//				valueSaved	- The saved file name.
//
//	Modification history:
//	
//		1.1		20:Sep:00	at	File created.
//		1.2		24:Oct:00	at	Corrected debugging macros.
//		1.3		02:Nov:00	at	File open dialog initialised with
//								current filename.
//		1.4		08:Nov:00	at	Saved file name has full path
//								specified.
//		1.5		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.6		15:Dec:00	at	Added message giving reason for
//								disabling the next button.
//		1.7		27:Feb:01	at	DFU file description displayed if
//								available. File browser opened
//								automatically if file not valid.
//		1.8		28:Feb:01	at	Corrected disabling of Next button.
//		1.9		28:Feb:01	at	Made filename field editable in
//								development mode.
//		1.10	28:Feb:01	at	Prevented file browser from changing
//								directory.
//		1.11	02:Mar:01	at	Improved display of file comment.
//		1.12	23:Mar:01	at	Added directory to DFUEngine include.
//		1.13	17:May:01	at	Added automatic behaviour.
//		1.14	18:May:01	at	Uploaded file stored in user profile
//								if possible, otherwise the application
//								directory is used.
//		1.15	21:May:01	at	Used SHGetFolderPath instead of
//								SHGetSpecialFolderPath
//								(requires SHFolder.dll).
//		1.16	23:May:01	at	Replaced SHGFP_TYPE_CURRENT by value
//								to reduce dependency on include file
//								ordering.
//		1.17	14:Nov:01	at	Added basic Unicode support.
//		1.18	08:Mar:02	at	Added MRU list in development mode.
//		1.19	11:Mar:02	at	Ensure drop down list is wide enough.
//		1.20	11:Mar:02	at	Correct font used for width calculation.
//		1.21	28:Mar:02	at	Corrected comment update after browse.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/PageFile.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "SHFolder.h"
#include "DFUWizard.h"
#include "PageFile.h"
#include "dfu\DFUEngine.h"
#include "DFUFileDialog.h"
#include "SHLWapi.h"

// Visual C++ debugging code
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// Saved file
static const TCHAR file_saved[] = _T("saved.dfu");

IMPLEMENT_DYNCREATE(PageFile, DFUPage)

// Constructor
PageFile::PageFile() : DFUPage(PageFile::IDD)
{
	//{{AFX_DATA_INIT(PageFile)
	//}}AFX_DATA_INIT

	// Generate the name of the saved file
	TCHAR buffer[MAX_PATH];
	TCHAR *pos;
	if (SUCCEEDED(SHGetFolderPath(0, CSIDL_APPDATA | CSIDL_FLAG_CREATE, 0, 0, buffer)))
	{
		// Use the application data directory within the user's profile
		CString company;
		company.Format(IDS_REGISTRY_COMPANY);
		valueSaved.Format(_T("%s\\%s"), buffer, (LPCTSTR) company);
		CreateDirectory(valueSaved, 0);
		CString application;
		application.Format(AFX_IDS_APP_TITLE);
		valueSaved += _T("\\") + application;
		CreateDirectory(valueSaved, 0);
		valueSaved += CString(_T("\\")) + file_saved;
	}
	else if ((GetModuleFileName(0, buffer, sizeof(buffer) / sizeof(TCHAR)) != 0)
	         && ((pos = _tcsrchr(buffer, '\\')) != 0))
	{
		// Use the directory containing the application
		pos[1] = '\0';
		valueSaved.Format(_T("%s%s"), buffer, file_saved);
	}
	else if (_tfullpath(buffer, file_saved, sizeof(buffer) / sizeof(TCHAR)) != 0)
	{
		// Use the working directory, e.g. set by a shortcut
		valueSaved = buffer;
	}
	else
	{
		// Just use the raw filename and hope the directory if alright
		valueSaved = file_saved;
	}
}

// Data exchange
void PageFile::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PageFile)
	DDX_Control(pDX, IDC_PAGE_FILE_NAME_COMBO, comboName);
	DDX_Control(pDX, IDC_PAGE_FILE_NAME_STATIC, staticName);
	DDX_Control(pDX, IDC_PAGE_FILE_COMMENT_EDIT, editComment);
	DDX_Control(pDX, IDC_PAGE_FILE_TITLE, staticTitle);
	//}}AFX_DATA_MAP
}

// Message map
BEGIN_MESSAGE_MAP(PageFile, DFUPage)
	//{{AFX_MSG_MAP(PageFile)
	ON_BN_CLICKED(IDC_PAGE_FILE_BROWSE_BUTTON, OnClickedBrowse)
	ON_CBN_EDITCHANGE(IDC_PAGE_FILE_NAME_COMBO, OnEditChanged)
	ON_CBN_SELCHANGE(IDC_PAGE_FILE_NAME_COMBO, OnSelChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Initialise the page when displayed
BOOL PageFile::OnSetActive() 
{
	// Use a combo box rather than an edit control in development mode
	staticName.ShowWindow(!GetSheet()->developmentMode);
	comboName.ShowWindow(GetSheet()->developmentMode);

	// Set the edit control to show the filename
	PathSetDlgItemPath(m_hWnd, IDC_PAGE_FILE_NAME_STATIC, valueName);

	// Prepare the list of recently used files
	comboName.ResetContent();
    CClientDC dc(&comboName);
    int saved = dc.SaveDC();
    dc.SelectObject(comboName.GetFont());
	int width = 0;
	for (POSITION file = listNames.GetHeadPosition(); file;)
	{
		CString name = listNames.GetNext(file);
		comboName.AddString(name);
		CSize size = dc.GetTextExtent(name);
		if (width < size.cx) width = size.cx;
	}
	width += GetSystemMetrics(SM_CXVSCROLL) + 2 * GetSystemMetrics(SM_CXEDGE);
	CRect rectangle;
	comboName.GetWindowRect(&rectangle);
	if (width < rectangle.Width()) width = rectangle.Width();
	comboName.SetDroppedWidth(width);
    dc.RestoreDC(saved);
	comboName.SetWindowText(valueName);
	
	// Perform any other initialisation required
	return DFUPage::OnSetActive();
}

// Delayed initialisation
void PageFile::OnSetActiveDelayed()
{
	static bool threaded = false;

	// No action required if already been called
	if (threaded)
	{
		return;
	}
	threaded = true;

	// Perform any other initialisation required
	DFUPage::OnSetActiveDelayed();

	// Open the browse dialog initially if file not valid
	if (!DFUEngine::IsDFUFileValid(valueName)
		&& !GetSheet()->developmentMode)
	{
		OnClickedBrowse();
	}

	// Advance to the next page is automatic behaviour requested
	if (GetSheet()->automatic) SetNext();

	// Clear the threaded flag
	threaded = false;
}

// Handle clicks on the Browse button
void PageFile::OnClickedBrowse() 
{
	DFUFileDialog file(true, valueName,
	                   OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_READONLY | OFN_NOCHANGEDIR, this);
	if (file.DoModal() == IDOK)
	{
		// Update the edit control with the new name
		valueName = file.GetPathName();
		PathSetDlgItemPath(m_hWnd, IDC_PAGE_FILE_NAME_STATIC, valueName);
		comboName.SetWindowText(valueName);
		SetButtons();
	}
}

// Update the controls on any change
void PageFile::OnEditChanged() 
{
	// Read the new file name
	comboName.GetWindowText(valueName);

	// Update the buttons
	SetButtons();
}

void PageFile::OnSelChanged() 
{
	// Read the new file name
	int selection = comboName.GetCurSel();
	if (selection != CB_ERR) comboName.GetLBText(selection, valueName);

	// Update the buttons
	SetButtons();
}

// Button management
LRESULT PageFile::GetPageNext()
{
	// Check if the file is valid
	bool valid = false;
	CString message;
	CStringX comment;
	if (valueName.IsEmpty())
	{
		message.Format(IDS_FILE_BLANK);
	}
	else if (!DFUEngine::IsDFUFileValid(valueName, comment))
	{
		message.Format(IDS_FILE_INVALID);
	}
	else
	{
		valid = true;
		if (comment.IsEmpty()) message.Format(IDS_FILE_NO_COMMENT);
		else message.Format(IDS_FILE_COMMENT, (const TCHAR *) comment);
	}

	// Set the comment text
	message.Replace(_T("\n"), _T("\r\n"));
	CString previous;
	editComment.GetWindowText(previous);
	if (message != previous)
	{
		editComment.SetWindowText(message);
		editComment.AutoScrollBar(SB_VERT);
	}

	// Return whether to enable the next button
	return valid ? automatic : disable;
}
