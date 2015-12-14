///////////////////////////////////////////////////////////////////////
//
//	File	: PageIntro.h
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
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/PageIntro.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef PAGEINTEO_H
#define PAGEINTRO_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "DFUPage.h"

// PageIntro class
class PageIntro : public DFUPage
{
	DECLARE_DYNCREATE(PageIntro)

public:

	// Constructor
	PageIntro();

	//{{AFX_DATA(PageIntro)
	enum { IDD = IDD_PAGE_INTRO };
	CString	valueVersion;
	CString	valueCopyright;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(PageIntro)
	public:
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	// Has the user advanced to another page
	bool pageChanged;

	// Delayed initialisation
	virtual void OnSetActiveDelayed();

	// Exit control
	virtual bool QueryClose(bool finish);
	
	//{{AFX_MSG(PageIntro)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif
