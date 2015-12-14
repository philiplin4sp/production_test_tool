///////////////////////////////////////////////////////////////////////
//
//	File	: PageAction.cpp
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: PageAction
//
//	Purpose	: DFU wizard page to allow the upgrade or downgrade
//			  action to be selected. Unless a DFU has been uploaded
//			  previously only one option is offered. The following
//			  member variable contains the selection:
//				valueAction	- The action to perform.
//
//	Modification history:
//	
//		1.1		14:Sep:00	at	File created.
//		1.2		02:Oct:00	at	Added support for the simulated
//								connection option.
//		1.3		24:Oct:00	at	Corrected debugging macros.
//		1.4		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.5		28:Feb:01	at	Focus moved to an appropriate control
//								if no option selected.
//		1.6		28:Feb:01	at	Upload not forced in development mode.
//		1.7		23:Mar:01	at	Added directory to DFUEngine include.
//		1.8		12:Apr:01	at	Download without upload made default
//								for development mode.
//		1.9		17:May:01	at	Added automatic behaviour.
//		1.10	25:May:01	at	Improved validation of selected option
//								and automatic behaviour interaction.
//		1.11	29:May:01	at	Scroll bar added to description.
//		1.12	14:Nov:01	at	Added basic Unicode support.
//		1.13	13:Feb:02	at	Improved scroll bar handling.
//		1.14	15:Feb:02	at	StaticEdit control used.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/PageAction.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUWizard.h"
#include "PageAction.h"
#include "DFUFileDialog.h"
#include "dfu\DFUEngine.h"
#include "SHLWapi.h"

// Visual C++ debugging code
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(PageAction, DFUPage)

// Constructor
PageAction::PageAction() : DFUPage(PageAction::IDD)
{
	//{{AFX_DATA_INIT(PageAction)
	valueAction = -1;
	//}}AFX_DATA_INIT
}

// Data exchange
void PageAction::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PageAction)
	DDX_Control(pDX, IDC_PAGE_ACTION_COMMENT_EDIT, editComment);
	DDX_Control(pDX, IDC_PAGE_ACTION_FILENAME_EDIT, editBackupFile);
	DDX_Control(pDX, IDC_PAGE_ACTION_RADIO_UPLOAD_DOWNLOAD, radioActionUploadDownload);
	DDX_Control(pDX, IDC_PAGE_ACTION_RADIO_DOWNLOAD, radioActionDownload);
	DDX_Control(pDX, IDC_PAGE_ACTION_RADIO_RESTORE, radioActionRestore);
	DDX_Radio(pDX, IDC_PAGE_ACTION_RADIO_UPLOAD_DOWNLOAD, valueAction);
	DDX_Control(pDX, IDC_PAGE_ACTION_TITLE, staticTitle);
	//}}AFX_DATA_MAP
}

// Message map
BEGIN_MESSAGE_MAP(PageAction, DFUPage)
	//{{AFX_MSG_MAP(PageAction)
	ON_BN_CLICKED(IDC_PAGE_ACTION_RADIO_DOWNLOAD, OnPageChanged)
	ON_BN_CLICKED(IDC_PAGE_ACTION_RADIO_RESTORE, OnPageChanged)
	ON_BN_CLICKED(IDC_PAGE_ACTION_RADIO_UPLOAD_DOWNLOAD, OnPageChanged)
	ON_BN_CLICKED(IDC_PAGE_ACTION_FILENAME_CHANGE_BUTTON, OnFileNameClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void PageAction::RefreshBackupFile()
{
	PathSetDlgItemPath(m_hWnd, IDC_PAGE_ACTION_FILENAME_EDIT, GetSheet()->pageFile.valueSaved);

	CString comment;
	CStringX xcomment;
	bool exists = DFUEngine::IsDFUFileValid(GetSheet()->pageFile.valueSaved, xcomment);
	comment = xcomment;

	// Enable the appropriate options
	radioActionDownload.EnableWindow(exists || GetSheet()->developmentMode);
	radioActionRestore.EnableWindow(exists);

	// Include a description of the saved file if possible
	CString previous;
	if (!exists) previous.Format(IDS_ACTION_INVALID);
	else if (comment.IsEmpty()) previous.Format(IDS_ACTION_NO_COMMENT);
	else previous.Format(IDS_ACTION_COMMENT, (LPCTSTR) comment);
	previous.Replace(_T("\n"), _T("\r\n"));
	editComment.SetWindowText(previous);
	editComment.AutoScrollBar(SB_VERT);

	// Modify the current selection if appropriate
	switch (valueAction)
	{
	case upload_download:
		// Can always perform an upload followed by a download
		break;

	case download:
		// Must have a previous upload or be in development mode
		if (!exists && !GetSheet()->developmentMode) valueAction = upload_download;
		break;

	case restore:
		// Must have a previous upload to restore
		if (!exists)
		{
			if (GetSheet()->automatic) valueAction = -1;
			else if (GetSheet()->developmentMode) valueAction = download;
			else valueAction = upload_download;
		}
		break;

	default:
		// Select a default option if no automatic behaviour
		if (!GetSheet()->automatic)
		{
			if (GetSheet()->developmentMode) valueAction = download;
			else if (!exists) valueAction = upload_download;
		}
	}

	// Ensure that the controls reflect the current selection
	UpdateData(false);

	if(valueAction != -1) SetButtons();
}

// Initialise the page when displayed
BOOL PageAction::OnSetActive() 
{
	RefreshBackupFile();

	// Perform any other initialisation required
	return DFUPage::OnSetActive();
}

// Delayed initialisation
void PageAction::OnSetActiveDelayed()
{
	// Perform any other initialisation required
	DFUPage::OnSetActiveDelayed();

	// Set the focus to the first option if none selected
	if (valueAction == -1)
	{
		GotoDlgCtrl(&radioActionUploadDownload);
	}

	// Advance to the next page is automatic behaviour requested
	if (GetSheet()->automatic) SetNext();
}

// Update the buttons on any change
void PageAction::OnPageChanged() 
{
	// Update the buttons
	SetButtons();
}

void PageAction::OnFileNameClick() 
{
	DFUFileDialog file(true, GetSheet()->pageFile.valueSaved, OFN_HIDEREADONLY | OFN_NOCHANGEDIR, this);
	if (file.DoModal() == IDOK)
	{
		// Update the edit control with the new name
		CString path_name = file.GetPathName();
		GetSheet()->pageFile.valueSaved = path_name;

		RefreshBackupFile();
	}
}

// Button management
LRESULT PageAction::GetPageBack()
{
	// Choose the previous page
	switch (GetSheet()->pageConnection.valueConnection)
	{
	case PageConnection::usb:
		return 1 < GetSheet()->pageUSB.listUSB.GetCount()
		       ? IDD_PAGE_USB : IDD_PAGE_CONNECTION;

	case PageConnection::com:
		return IDD_PAGE_COM;

	case PageConnection::simulated:
		return IDD_PAGE_CONNECTION;

	default:
		return disable;
	}
}

LRESULT PageAction::GetPageNext()
{
	// Choose the next page
	switch (valueAction)
	{
	case upload_download:
	case download:
		return IDD_PAGE_FILE;

	case restore:
		return IDD_PAGE_SUMMARY;

	default:
		return disable;
	}
}
