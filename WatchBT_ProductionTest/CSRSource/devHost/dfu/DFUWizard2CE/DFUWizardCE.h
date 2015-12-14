///////////////////////////////////////////////////////////////////////
//
//	File	: DFUWizardCE.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: D.Coultous
//
//	Class	: DFUWizardCE
//
//	Purpose	: The DFUWizard for Windows CE. This performs upgrade of existing 
//			  firmware and download of selected firmware.
//
//	Modification history (as DFUTest.h):
//	
//		1.1		26:Jul:00	doc	File created fro AT's DFUTest
//		1.2		11:Oct:02	doc	Added saved.dfu constant
//		1.3		30:Oct:02	doc Updated modification history
//		1.4		19:Nov:02	doc	Added Registry changes to disable system timeouts
//		1.5		20:Nov:02	doc Retracted 1.4 changes, now using SystemIdleTimerReset()
//		1.6		12:dec:02	doc Tidied application structure
//
//	(and as DFUWizardCE.h):
//	
//		1.1		21:Nov:02	doc Recreated as DFUWizardCE.h
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard2CE/DFUWizardCE.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef DFUWizardCE_H
#define DFUWizardCE_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "resource.h"

// Saved file
static const TCHAR saved_file[] = _T("saved.dfu");

// DFUWizardCEApp class
class DFUWizardCEApp : public CWinApp
{
public:

	// Constructor
	DFUWizardCEApp();

	//{{AFX_VIRTUAL(DFUWizardCEApp)
	public:
	virtual BOOL InitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(DFUWizardCEApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif
