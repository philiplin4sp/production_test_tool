///////////////////////////////////////////////////////////////////////
//
//	File	: PageUSBEnum.cpp
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: PageUSBEnum
//
//	Purpose	: DFU wizard page displayed while attached USB devices
//			  are enumerated. This automatically advances to either
//			  the IDD_PAGE_USB or IDD_PAGE_CONNECTION page when
//			  complete, depending on whether any suitable devices
//			  were found.
//
//	Modification history:
//	
//		1.1		20:Sep:00	at	File created.
//		1.2		24:Oct:00	at	Corrected debugging macros.
//		1.3		27:OCt:00	at	Corrected constant used for the title
//								control.
//		1.4		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.5		28:Feb:01	at	IDD_PAGE_USB no longer displayed if
//								only a single device found.
//							at	IDD_PAGE_USB displayed for a single
//								device in development mode.
//		1.6		23:Mar:01	at	Added directory to DFUEngine include.
//		1.7		17:May:01	at	Corrected change history.
//		1.8		17:May:01	at	Added automatic behaviour.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/PageUSBEnum.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUWizard.h"
#include "PageUSBEnum.h"
#include "dfu\DFUEngine.h"

// Visual C++ debugging code
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(PageUSBEnum, DFUPage)

// Constructor
PageUSBEnum::PageUSBEnum() : DFUPage(PageUSBEnum::IDD)
{
	//{{AFX_DATA_INIT(PageUSBEnum)
	//}}AFX_DATA_INIT
}

// Destructor
PageUSBEnum::~PageUSBEnum()
{
}

// Data exchange
void PageUSBEnum::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PageUSBEnum)
	DDX_Control(pDX, IDC_PAGE_USBENUM_ANIMATE, animateUSBEnum);
	DDX_Control(pDX, IDC_PAGE_USBENUM_TITLE, staticTitle);
	//}}AFX_DATA_MAP
}

// Message map
BEGIN_MESSAGE_MAP(PageUSBEnum, DFUPage)
	//{{AFX_MSG_MAP(PageUSBEnum)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Delayed initialisation
void PageUSBEnum::OnSetActiveDelayed()
{
	// Perform any other initialisation required
	DFUPage::OnSetActiveDelayed();

	// Show the hourglass and start an animation
	CWaitCursor wait;
	animateUSBEnum.Open(IDR_AVI_SEARCH);
	animateUSBEnum.Play(0, -1, -1);

	// Enumerate the USB devices
	DFUSheet *sheet = GetSheet();
	CStringListX listUSB;
	int devices = DFUEngine::GetUSBDeviceNames(listUSB);
	Convert(listUSB, sheet->pageUSB.listUSB);

	// Stop the animation and advance to the appropriate page
	animateUSBEnum.Close();
	if (devices == 0)
	{
		AfxMessageBox(IDP_ERROR_USB_NONE, MB_OK | MB_ICONEXCLAMATION);
		GetSheet()->automatic = false;
		sheet->SetActivePage(&sheet->pageConnection);
	}
	else if ((devices == 1) && !sheet->developmentMode)
	{
		sheet->pageUSB.valueUSBNum = sheet->pageUSB.listUSB.GetHead();
		sheet->SetActivePage(&sheet->pageAction);
	}
	else
	{
		sheet->SetActivePage(&sheet->pageUSB);
	}
}

// Button management
LRESULT PageUSBEnum::GetPageBack()
{
	return disable;
}

LRESULT PageUSBEnum::GetPageNext()
{
	return disable;
}
