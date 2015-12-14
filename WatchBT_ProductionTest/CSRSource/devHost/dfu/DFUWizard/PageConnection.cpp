///////////////////////////////////////////////////////////////////////
//
//	File	: PageConnection.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: PageConnection
//
//	Purpose	: DFU wizard page to allow USB or COM port connection to
//			  be selected. The following member variable contains the
//			  selection:
//				valueConnection	- USB, COM port or simulated
//								  connection.
//
//			  The following member variable can be used to enable
//			  simulated connections:
//				enableSimulated	- Set to true to enable simulated
//								  connections. The default value is
//								  false.
//
//	Modification history:
//	
//		1.1		14:Sep:00	at	File created.
//		1.2		02:Oct:00	at	Added simulated connection option.
//		1.3		24:Oct:00	at	Corrected debugging macros.
//		1.4		27:Oct:00	at	Added support for separate COM port
//								enumeration page.
//		1.5		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.6		17:May:01	at	Added automatic behaviour.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/PageConnection.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUWizard.h"
#include "PageConnection.h"

// Visual C++ debugging code
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(PageConnection, DFUPage)

// Constructor
PageConnection::PageConnection() : DFUPage(PageConnection::IDD)
{
	//{{AFX_DATA_INIT(PageConnection)
	valueConnection = -1;
	//}}AFX_DATA_INIT

	// Initially simulated connections are disabled
	enableSimulated = false;
}

// Data exchange
void PageConnection::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PageConnection)
	DDX_Radio(pDX, IDC_PAGE_CONNECTION_RADIO_USB, valueConnection);
	DDX_Control(pDX, IDC_PAGE_CONNECTION_RADIO_SIMULATED, radioSimulated);
	DDX_Control(pDX, IDC_PAGE_CONNECTION_TITLE, staticTitle);
	//}}AFX_DATA_MAP
}

// Message map
BEGIN_MESSAGE_MAP(PageConnection, DFUPage)
	//{{AFX_MSG_MAP(PageConnection)
	ON_BN_CLICKED(IDC_PAGE_CONNECTION_RADIO_COM, OnPageChanged)
	ON_BN_CLICKED(IDC_PAGE_CONNECTION_RADIO_USB, OnPageChanged)
	ON_BN_CLICKED(IDC_PAGE_CONNECTION_RADIO_SIMULATED, OnPageChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Initialise the page when displayed
BOOL PageConnection::OnSetActive() 
{
	// Show the simulated connection button if enabled
	radioSimulated.ShowWindow(enableSimulated ? SW_SHOWNORMAL : SW_HIDE);
	
	// Perform any other initialisation required
	return DFUPage::OnSetActive();
}

// Delayed initialisation
void PageConnection::OnSetActiveDelayed()
{
	// Perform any other initialisation required
	DFUPage::OnSetActiveDelayed();

	// Advance to the next page is automatic behaviour requested
	if (GetSheet()->automatic) SetNext();
}

// Update the buttons on any change
void PageConnection::OnPageChanged() 
{
	SetButtons();
}

// Button management
LRESULT PageConnection::GetPageNext()
{
	// Choose the next page
	switch (valueConnection)
	{
	case usb:
		return IDD_PAGE_USBENUM;

	case com:
		return IDD_PAGE_COMENUM;

	case simulated:
		return IDD_PAGE_ACTION;

	default:
		return disable;
	}
}
