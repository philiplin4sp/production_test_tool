///////////////////////////////////////////////////////////////////////
//
//	File	: PageSummary.h
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
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/PageSummary.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef PAGESUMMARY_H
#define PAGESUMMARY_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "DFUPage.h"
#include "StaticEdit.h"

// PageSummary class
class PageSummary : public DFUPage
{
	DECLARE_DYNCREATE(PageSummary)

public:

	// Constructor
	PageSummary();

	// Destructor
	~PageSummary();

	// Button management
	virtual LRESULT GetPageBack();

	//{{AFX_DATA(PageSummary)
	enum { IDD = IDD_PAGE_SUMMARY };
	StaticEdit editOptions;
	//}}AFX_DATA

	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(PageSummary)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	// Delayed initialisation
	virtual void OnSetActiveDelayed();

	//{{AFX_MSG(PageSummary)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}

#endif
