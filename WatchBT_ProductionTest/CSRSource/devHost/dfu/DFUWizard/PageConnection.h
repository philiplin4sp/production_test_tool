///////////////////////////////////////////////////////////////////////
//
//	File	: PageConnection.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: PageConnection
//
//	Purpose	: DFU wizard page to allow USB or COM port connection to
//			  be selected. The following member variable contains the
//			  selection:
//				valueConnection	- USB, COM port or simulated
//								  connection.
//
//			  The following member variable can be used to enable
//			  simulated connections:
//				enableSimulated	- Set to true to enable simulated
//								  connections. The default value is
//								  false.
//
//	Modification history:
//	
//		1.1		14:Sep:00	at	File created.
//		1.2		02:Oct:00	at	Added simulated connection option.
//		1.3		24:Oct:00	at	Corrected debugging macros.
//		1.4		27:Oct:00	at	Added support for separate COM port
//								enumeration page.
//		1.5		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.6		17:May:01	at	Added automatic behaviour.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/PageConnection.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef PAGECONNECTION_H
#define PAGECONNECTION_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "DFUPage.h"

// PageConnection class
class PageConnection : public DFUPage
{
	DECLARE_DYNCREATE(PageConnection)

public:

	// Connection types
	enum
	{
		usb,
		com,
		simulated
	};

	// Enable or disable simulated connections
	bool enableSimulated;

	// Constructor
	PageConnection();

	// Button management
	virtual LRESULT GetPageNext();

	//{{AFX_DATA(PageConnection)
	enum { IDD = IDD_PAGE_CONNECTION };
	CButton	radioSimulated;
	int valueConnection;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(PageConnection)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	// Delayed initialisation
	virtual void OnSetActiveDelayed();

	//{{AFX_MSG(PageConnection)
	afx_msg void OnPageChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif
