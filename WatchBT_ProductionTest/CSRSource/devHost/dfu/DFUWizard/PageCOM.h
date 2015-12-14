///////////////////////////////////////////////////////////////////////
//
//	File	: PageCOM.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: PageCOM
//
//	Purpose	: DFU wizard page to allow COM port settings to be
//			  selected. The following member variables contain the
//			  selection:
//				valueCOMPort	- Name of COM port.
//				valueCOMAuto	- Should the baud rate and protocol
//								  be detected automatically.
//				valueBaud		- Baud rate to use if not automatic.
//				valueProtocol	- Protocol to use if not automatic.
//				valueLink		- Link establishment mode for BCSP.
//				valueTunnel		- Should tunnelling be used via BCSP.
//
//	Modification history:
//	
//		1.1		18:Sep:00	at	File created.
//		1.2		29:Sep:00	at	Added tunnelling option for BCSP, and
//								ensured that controls are correctly
//								enabled or disabled initially.
//		1.3		24:Oct:00	at	Corrected debugging macros.
//		1.4		26:Oct:00	at	Only baud rates appropriate to the
//								selected port and offered.
//		1.5		27:Oct:00	at	Moved initial COM port enumeration to
//								a separate page.
//		1.6		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.7		24:Nov:00	at	Baud rate list only updated if COM
//								port changed.
//		1.8		08:Dec:00	at	Added link establishment modes.
//		1.9		23:Mar:01	at	Added directory to DFUEngine include.
//		1.10	17:May:01	at	Added automatic behaviour.
//		1.11	25:May:01	at	Improved automatic behaviour.
//		1.12	19:Feb:02	at	Transport settings hidden if auto.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/PageCOM.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef PAGECOM_H
#define PAGECOM_H

#if _MSC_VER > 1000
#pragma once
#endif 

// Include project header files
#include "DFUPage.h"

// PageCOM class
class PageCOM : public DFUPage
{
	DECLARE_DYNAMIC(PageCOM)

public:

	// Protocol types
	enum
	{
		bcsp,
		h4
	};

	// List of COM ports
	CStringList listPorts;

	// Constructor
	PageCOM();

	// Button management
	virtual LRESULT GetPageBack();
	virtual LRESULT GetPageNext();

	//{{AFX_DATA(PageCOM)
	enum { IDD = IDD_PAGE_COM };
	CButton	radioDisabled;
	CStatic staticLink;
	CButton	radioEnabled;
	CButton	radioPassive;
	CButton	buttonTunnel;
	CComboBox comboCOMPort;
	CString valueCOMPort;
	BOOL valueCOMAuto;
	CStatic staticBaud;
	CComboBox comboBaud;
	CString valueBaud;
	CStatic staticProtocol;
	CButton radioBCSP;
	CButton radioH4;
	int valueProtocol;
	BOOL	valueTunnel;
	int		valueLink;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(PageCOM)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	// Delayed initialisation
	virtual void OnSetActiveDelayed();

	//{{AFX_MSG(PageCOM)
	afx_msg void OnPortChanged();
	afx_msg void OnPageChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif
