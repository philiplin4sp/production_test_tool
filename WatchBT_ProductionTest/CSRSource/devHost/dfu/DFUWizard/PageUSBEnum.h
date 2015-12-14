///////////////////////////////////////////////////////////////////////
//
//	File	: PageUSBEnum.h
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
//								IDD_PAGE_USB displayed for a single
//								device in development mode.
//		1.6		23:Mar:01	at	Added directory to DFUEngine include.
//		1.7		17:May:01	at	Corrected change history.
//		1.8		17:May:01	at	Added automatic behaviour.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/PageUSBEnum.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef PAGEUSBENUM_H
#define PAGEUSBENUM_H

#if _MSC_VER > 1000
#pragma once
#endif

// PageUSBEnum class
class PageUSBEnum : public DFUPage
{
	DECLARE_DYNCREATE(PageUSBEnum)

public:

	// Constructor
	PageUSBEnum();

	// Destructor
	~PageUSBEnum();

	// Button management
	virtual LRESULT GetPageBack();
	virtual LRESULT GetPageNext();

	//{{AFX_DATA(PageUSBEnum)
	enum { IDD = IDD_PAGE_USBENUM };
	CAnimateCtrl animateUSBEnum;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(PageUSBEnum)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	// Delayed initialisation
	virtual void OnSetActiveDelayed();

	//{{AFX_MSG(PageUSBEnum)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}

#endif
