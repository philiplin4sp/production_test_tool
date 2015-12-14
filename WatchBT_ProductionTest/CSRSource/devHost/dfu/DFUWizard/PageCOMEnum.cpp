///////////////////////////////////////////////////////////////////////
//
//	File	: PageCOMEnum.cpp
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: PageCOMEnum
//
//	Purpose	: DFU wizard page displayed while available COM ports
//			  are enumerated. This automatically advances to either
//			  the IDD_PAGE_COM or IDD_PAGE_CONNECTION page when
//			  complete, depending on whether any suitable ports were
//			  found.
//
//	Modification history:
//	
//		1.1		27:Oct:00	at	File created.
//		1.2		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.3		24:Nov:00	at	Removed enumeration of baud rates.
//		1.4 	23:Mar:01	at	Added directory to DFUEngine include.
//		1.5		17:May:01	at	Added automatic behaviour.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/PageCOMEnum.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUWizard.h"
#include "PageCOMEnum.h"
#include "dfu\DFUEngine.h"

// Visual C++ debugging code
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(PageCOMEnum, DFUPage)

// Constructor
PageCOMEnum::PageCOMEnum() : DFUPage(PageCOMEnum::IDD)
{
	//{{AFX_DATA_INIT(PageCOMEnum)
	//}}AFX_DATA_INIT
}

// Destructor
PageCOMEnum::~PageCOMEnum()
{
}

// Data exchange
void PageCOMEnum::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PageCOMEnum)
	DDX_Control(pDX, IDC_PAGE_COMENUM_ANIMATE, animateCOMEnum);
	DDX_Control(pDX, IDC_PAGE_COMENUM_TITLE, staticTitle);
	//}}AFX_DATA_MAP
}

// Message map
BEGIN_MESSAGE_MAP(PageCOMEnum, DFUPage)
	//{{AFX_MSG_MAP(PageCOMEnum)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Delayed initialisation
void PageCOMEnum::OnSetActiveDelayed()
{
	// Perform any other initialisation required
	DFUPage::OnSetActiveDelayed();

	// Show the hourglass and start an animation
	CWaitCursor wait;
	animateCOMEnum.Open(IDR_AVI_SEARCH);
	animateCOMEnum.Play(0, -1, -1);

	// Enumerate the COM ports
	DFUSheet *sheet = GetSheet();
	CStringListX listPorts;
	Convert(sheet->pageCOM.listPorts, listPorts);
	DFUEngine::GetCOMPortNames(listPorts);
	for (CStringListX::POSITION port = listPorts.GetHeadPosition(); port;)
	{
		CStringListX::POSITION portPos = port;
		CStringListX listBaudRates;
		if (DFUEngine::GetCOMBaudRates(listBaudRates, listPorts.GetNext(port)) == 0)
		{
			listPorts.RemoveAt(portPos);
		}
	}
	Convert(listPorts, sheet->pageCOM.listPorts);

	// Stop the animation and advance to the appropriate page
	animateCOMEnum.Close();
	if (sheet->pageCOM.listPorts.IsEmpty())
	{
		AfxMessageBox(IDP_ERROR_COM_NONE, MB_OK | MB_ICONEXCLAMATION);
		GetSheet()->automatic = false;
		sheet->SetActivePage(&sheet->pageConnection);
	}
	else
	{
		sheet->SetActivePage(&sheet->pageCOM);
	}
}

// Button management
LRESULT PageCOMEnum::GetPageBack()
{
	return disable;
}

LRESULT PageCOMEnum::GetPageNext()
{
	return disable;
}
