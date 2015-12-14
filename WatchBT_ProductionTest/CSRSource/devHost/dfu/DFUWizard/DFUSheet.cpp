///////////////////////////////////////////////////////////////////////
//
//	File	: DFUSheet.cpp
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFUSheet
//
//	Purpose	: The BlueCore Device Firmware Upgrade Wizard property
//			  sheet. This has all of the pages as member variables.
//
//	Modification history:
//	
//		1.1		14:Sep:00	at	File created.
//		1.2		24:Oct:00	at	Corrected debugging macros.
//		1.3		27:Oct:00	at	Added page for COM port enumeration.
//		1.4		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.5		28:Feb:01	at	Added development specific behaviour.
//		1.6		17:May:01	at	Added automatic behaviour.
//		1.7		14:Nov:01	at	Added basic Unicode support.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/DFUSheet.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUWizard.h"
#include "DFUSheet.h"

// Visual C++ debugging code
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(DFUSheet, CPropertySheet)

// Constructor
DFUSheet::DFUSheet()
{
	// Default to not development-mode
	developmentMode = false;

	// Add all of the pages to the property sheet
	AddPage(&pageIntro);
	AddPage(&pageConnection);
	AddPage(&pageUSBEnum);
	AddPage(&pageUSB);
	AddPage(&pageCOMEnum);
	AddPage(&pageCOM);
	AddPage(&pageAction);
	AddPage(&pageFile);
	AddPage(&pageSummary);
	AddPage(&pageProgress);
	AddPage(&pageResults);

	// Change the property sheet to a wizard
	SetWizardMode();
} 

// Message map
BEGIN_MESSAGE_MAP(DFUSheet, CPropertySheet)
	//{{AFX_MSG_MAP(DFUSheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL DFUSheet::OnInitDialog() 
{
	// Pass on to the base class first
	BOOL result = CPropertySheet::OnInitDialog();
	
	// Modified behaviour for development mode
	if (developmentMode)
	{
		// Add a minimize icon
		ModifyStyle(0, WS_MINIMIZEBOX, 0);
		GetSystemMenu(false)->AppendMenu(MF_STRING, SC_MINIMIZE, _T("Minimize"));
	}
	
	// Return the result
	return result;
}
