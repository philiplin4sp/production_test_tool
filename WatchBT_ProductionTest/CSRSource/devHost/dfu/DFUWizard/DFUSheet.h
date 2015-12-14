///////////////////////////////////////////////////////////////////////
//
//	File	: DFUSheet.h
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
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/DFUSheet.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef DFUSHEET_H
#define DFUSHEET_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "PageIntro.h"
#include "PageConnection.h"
#include "PageUSBEnum.h"
#include "PageUSB.h"
#include "PageCOMEnum.h"
#include "PageCOM.h"
#include "PageAction.h"
#include "PageFile.h"
#include "PageSummary.h"
#include "PageProgress.h"
#include "PageResults.h"

// DFUSheet class
class DFUSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(DFUSheet)

public:

	// The individual pages
	PageIntro pageIntro;
	PageConnection pageConnection;
	PageUSBEnum pageUSBEnum;
	PageUSB pageUSB;
	PageCOMEnum pageCOMEnum;
	PageCOM pageCOM;
	PageAction pageAction;
	PageFile pageFile;
	PageSummary pageSummary;
	PageProgress pageProgress;
	PageResults pageResults;

	// Modify behaviour for development use
	bool developmentMode;

	// Advance through the wizard automatically for command line invocation
	bool automatic;

	// Constructor
	DFUSheet();

	//{{AFX_VIRTUAL(DFUSheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(DFUSheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif
