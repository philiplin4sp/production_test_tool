///////////////////////////////////////////////////////////////////////
//
//	File	: PageSummary.cpp
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: PageSummary
//
//	Purpose	: DFU wizard page to display a summary of the selected
//			  options before starting the upgrade procedure. This
//			  enables the user to make changes if required.
//
//	Modification history:
//	
//		1.1		25:Sep:00	at	File created.
//		1.2		04:Oct:00	at	Added support for a simulated
//								connection.
//		1.3		11:Oct:00	at	Added support for HCI tunnelling.
//		1.4		24:Oct:00	at	Corrected debugging macros.
//		1.5		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.6		08:Dec:00	at	Added display of link establishment
//								option and changed control type to
//								allow a scroll bar to be added.
//		1.7		25:May:01	at	Added automatic start. Vertical scroll
//								bar added if necessary.
//		1.8		14:Nov:01	at	Added basic Unicode support.
//		1.9		13:Feb:02	at	Improved scroll bar handling.
//		1.10	14:Feb:02	at	DFUPage now handles scroll bars.
//		1.11	15:Feb:02	at	StaticEdit control used.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/PageSummary.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUWizard.h"
#include "PageSummary.h"

// Visual C++ debugging code
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(PageSummary, DFUPage)

// Constructor
PageSummary::PageSummary() : DFUPage(PageSummary::IDD)
{
	//{{AFX_DATA_INIT(PageSummary)
	//}}AFX_DATA_INIT
}

// Destructor
PageSummary::~PageSummary()
{
}

// Data exchange
void PageSummary::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PageSummary)
	DDX_Control(pDX, IDC_PAGE_SUMMARY_OPTIONS_EDIT, editOptions);
	DDX_Control(pDX, IDC_PAGE_SUMMARY_TITLE, staticTitle);
	//}}AFX_DATA_MAP
}

// Message map
BEGIN_MESSAGE_MAP(PageSummary, DFUPage)
	//{{AFX_MSG_MAP(PageSummary)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Initialise the page when displayed
BOOL PageSummary::OnSetActive() 
{
	// Create a summary of the selected options
	DFUSheet *sheet = GetSheet();
	CString connection;
	switch (sheet->pageConnection.valueConnection)
	{
	case PageConnection::usb:
		connection.Format(IDS_SUMMARY_CONNECTION_USB, (LPCTSTR) sheet->pageUSB.valueUSBNum);
		break;

	case PageConnection::com:
		if (sheet->pageCOM.valueCOMAuto)
		{
			connection.Format(IDS_SUMMARY_CONNECTION_COM_AUTO, (LPCTSTR) sheet->pageCOM.valueCOMPort);
		}
		else
		{
			CString link;
			switch (sheet->pageCOM.valueLink)
			{
			case DFUEngine::link_disabled:
				link.Format(IDS_SUMMARY_LINK_DISABLED);
				break;

			case DFUEngine::link_enabled:
				link.Format(IDS_SUMMARY_LINK_ENABLED);
				break;

			case DFUEngine::link_passive:
				link.Format(IDS_SUMMARY_LINK_PASSIVE);
				break;
			}
			CString protocol;
			switch (sheet->pageCOM.valueProtocol)
			{
			case PageCOM::bcsp:
				protocol.Format(sheet->pageCOM.valueTunnel
				                ? IDS_SUMMARY_PROTOCOL_BCSP_TUNNEL
								: IDS_SUMMARY_PROTOCOL_BCSP,
								link);
				break;

			case PageCOM::h4:
				protocol.Format(IDS_SUMMARY_PROTOCOL_H4);
				break;
			}
			connection.Format(IDS_SUMMARY_CONNECTION_COM_MANUAL, (LPCTSTR) sheet->pageCOM.valueCOMPort, (LPCTSTR) sheet->pageCOM.valueBaud, (LPCTSTR) protocol);
		}
		break;

	case PageConnection::simulated:
		connection.Format(IDS_SUMMARY_CONNECTION_SIMULATED);
		break;
	}
	CStringX comment;
	CString action;
	switch (sheet->pageAction.valueAction)
	{
	case PageAction::upload_download:
		DFUEngine::IsDFUFileValid(sheet->pageFile.valueName, comment);
		if (comment.IsEmpty()) action.Format(IDS_SUMMARY_UPLOAD_DOWNLOAD, (LPCTSTR) sheet->pageFile.valueName);
		else action.Format(IDS_SUMMARY_UPLOAD_DOWNLOAD_COMMENT, sheet->pageFile.valueName, (const TCHAR *) comment);
		break;

	case PageAction::download:
		DFUEngine::IsDFUFileValid(sheet->pageFile.valueName, comment);
		if (comment.IsEmpty()) action.Format(IDS_SUMMARY_DOWNLOAD, (LPCTSTR) sheet->pageFile.valueName);
		else action.Format(IDS_SUMMARY_DOWNLOAD_COMMENT, sheet->pageFile.valueName, (const TCHAR *) comment);
		break;

	case PageAction::restore:
		DFUEngine::IsDFUFileValid(sheet->pageFile.valueSaved, comment);
		if (comment.IsEmpty()) action.Format(IDS_SUMMARY_RESTORE);
		else action.Format(IDS_SUMMARY_RESTORE_COMMENT, (const TCHAR *) comment);
		break;
	}

	// Update the controls
	CString summary = connection + _T("\n\n") + action;
	summary.Replace(_T("\n"), _T("\r\n"));
	editOptions.SetWindowText(summary);
	editOptions.AutoScrollBar();
	
	// Perform any other initialisation required
	return DFUPage::OnSetActive();
}

// Delayed initialisation
void PageSummary::OnSetActiveDelayed()
{
	// Perform any other initialisation required
	DFUPage::OnSetActiveDelayed();

	// Advance to the next page is automatic behaviour requested
	if (GetSheet()->automatic && GetSheet()->pageResults.valueExit) SetNext();
}

// Button management
LRESULT PageSummary::GetPageBack()
{
	// Choose the previous page
	switch (GetSheet()->pageAction.valueAction)
	{
	case PageAction::upload_download:
	case PageAction::download:
		return IDD_PAGE_FILE;

	case PageAction::restore:
		return IDD_PAGE_ACTION;

	default:
		return disable;
	}
}
