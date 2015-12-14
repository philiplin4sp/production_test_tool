///////////////////////////////////////////////////////////////////////
//
//	File	: PageUSB.h
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
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/PageUSB.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef PAGEUSB_H
#define PAGEUSB_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "DFUPage.h"

// PageUSB class
class PageUSB : public DFUPage
{
	DECLARE_DYNCREATE(PageUSB)

public:

	// List of suitable USB devices
	CStringList listUSB;

	// Constructor
	PageUSB();

	// Destructor
	~PageUSB();

	// Button management
	virtual LRESULT GetPageBack();
	virtual LRESULT GetPageNext();

	//{{AFX_DATA(PageUSB)
	enum { IDD = IDD_PAGE_USB };
	CComboBox comboUSBNum;
	CString valueUSBNum;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(PageUSB)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	// Delayed initialisation
	virtual void OnSetActiveDelayed();

	//{{AFX_MSG(PageUSB)
	afx_msg void OnPageChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}

#endif
