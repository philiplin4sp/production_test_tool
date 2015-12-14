///////////////////////////////////////////////////////////////////////
//
//	File	: PageIntro.cpp
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: PageIntro
//
//	Purpose	: DFU wizard page to explain the purpose of the wizard
//			  to the user. The following member variables should be
//			  set before this page is activated:
//				valueVersion	- Program version number.
//				valueCopyright	- Program copyright text.
//
//	Modification history:
//	
//		1.1		14:Sep:00	at	File created.
//		1.2		24:Oct:00	at	Corrected debugging macros.
//		1.3		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.4		17:May:01	at	Added automatic behaviour.
//		1.5		13:Feb:02	at	Suppress quit query on first display.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/PageIntro.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUWizard.h"
#include "PageIntro.h"

// Visual C++ debugging code
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(PageIntro, DFUPage)

// Constructor
PageIntro::PageIntro() : DFUPage(PageIntro::IDD)
{
	//{{AFX_DATA_INIT(PageIntro)
	valueVersion = _T("");
	valueCopyright = _T("");
	//}}AFX_DATA_INIT

	// No page change initially
	pageChanged = false;
}

// Data exchange
void PageIntro::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PageIntro)
	DDX_Control(pDX, IDC_PAGE_INTRO_TITLE, staticTitle);
	DDX_Text(pDX, IDC_PAGE_INTRO_VERSION_STATIC, valueVersion);
	DDX_Text(pDX, IDC_PAGE_INTRO_COPYRIGHT_STATIC, valueCopyright);
	//}}AFX_DATA_MAP
}

// Message map
BEGIN_MESSAGE_MAP(PageIntro, DFUPage)
	//{{AFX_MSG_MAP(PageIntro)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Tidy up when the page is hidden
BOOL PageIntro::OnKillActive() 
{
	// Mark the page as having been changed
	pageChanged = true;
	
	// Perform any other tidying up required
	return DFUPage::OnKillActive();
}

// Delayed initialisation
void PageIntro::OnSetActiveDelayed()
{
	// Perform any other initialisation required
	DFUPage::OnSetActiveDelayed();

	// Advance to the next page is automatic behaviour requested
	if (GetSheet()->automatic) SetNext();
}

// Exit control
bool PageIntro::QueryClose(bool finish)
{
	// Exit quietly if page never changed
	if (!pageChanged) return true;

	// Perform any default behaviour
	return DFUPage::QueryClose(finish);
}
