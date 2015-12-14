///////////////////////////////////////////////////////////////////////
//
//	File	: PageUSB.cpp
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: PageUSB
//
//	Purpose	: DFU wizard page to allow a USB device number to be
//			  selected. Enumeration of USB devices will have been
//			  completed by the time this page is displayed. This is
//			  only used if at least two suitable USB devices have
//			  been found. The following member variable contains the
//			  selection:
//				valueUSBNum	- Name of selected device.
//
//	Modification history:
//	
//		1.1		18:Sep:00	at	File created.
//		1.2		24:Oct:00	at	Corrected debugging macros.
//		1.3		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.4		28:Feb:01	at	Automatic skipping for a single device
//								removed.
//		1.5		23:Mar:01	at	Added directory to DFUEngine include.
//		1.6		17:May:01	at	Added automatic behaviour.
//		1.7		25:May:01	at	Improved automatic behaviour.
//		1.8		24:Jul:02	at	Ensure drop down list is wide enough.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/PageUSB.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUWizard.h"
#include "PageUSB.h"
#include "dfu\DFUEngine.h"

// Visual C++ debugging code
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(PageUSB, DFUPage)

// Constructor
PageUSB::PageUSB() : DFUPage(PageUSB::IDD)
{
	//{{AFX_DATA_INIT(PageUSB)
	valueUSBNum = _T("");
	//}}AFX_DATA_INIT
}

// Destructor
PageUSB::~PageUSB()
{
}

// Data exchange
void PageUSB::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PageUSB)
	DDX_Control(pDX, IDC_PAGE_USB_USBNUM_COMBO, comboUSBNum);
	DDX_CBString(pDX, IDC_PAGE_USB_USBNUM_COMBO, valueUSBNum);
	DDX_Control(pDX, IDC_PAGE_USB_TITLE, staticTitle);
	//}}AFX_DATA_MAP
}

// Message map
BEGIN_MESSAGE_MAP(PageUSB, DFUPage)
	//{{AFX_MSG_MAP(PageUSB)
	ON_CBN_SELCHANGE(IDC_PAGE_USB_USBNUM_COMBO, OnPageChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Initialise the page when displayed
BOOL PageUSB::OnSetActive() 
{
	// Display the list of USB devices
	comboUSBNum.ResetContent();
    CClientDC dc(&comboUSBNum);
    int saved = dc.SaveDC();
    dc.SelectObject(comboUSBNum.GetFont());
	int width = 0;
	for (POSITION device = listUSB.GetHeadPosition(); device;)
	{
		CString num = listUSB.GetNext(device);
		comboUSBNum.AddString(num);
		CSize size = dc.GetTextExtent(num);
		if (width < size.cx) width = size.cx;
	}
	width += GetSystemMetrics(SM_CXVSCROLL) + 2 * GetSystemMetrics(SM_CXEDGE);
	CRect rectangle;
	comboUSBNum.GetWindowRect(&rectangle);
	if (width < rectangle.Width()) width = rectangle.Width();
	comboUSBNum.SetDroppedWidth(width);
    dc.RestoreDC(saved);

	// Automatically select the device if only one
	valueUSBNum = FindUnique(listUSB, valueUSBNum);

	// Ensure that the controls reflect the current selection
	UpdateData(false);
	
	// Perform any other initialisation required
	return DFUPage::OnSetActive();
}

// Delayed initialisation
void PageUSB::OnSetActiveDelayed()
{
	// Perform any other initialisation required
	DFUPage::OnSetActiveDelayed();

	// Advance to the next page is automatic behaviour requested
	if (GetSheet()->automatic) SetNext();
}

// Update the buttons on any change
void PageUSB::OnPageChanged() 
{
	// Update the buttons
	SetButtons();	
}

// Button management
LRESULT PageUSB::GetPageBack()
{
	return IDD_PAGE_CONNECTION;
}

LRESULT PageUSB::GetPageNext()
{
	return !valueUSBNum.IsEmpty() ? IDD_PAGE_ACTION : disable;
}
