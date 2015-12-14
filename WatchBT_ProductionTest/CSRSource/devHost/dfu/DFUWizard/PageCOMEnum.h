///////////////////////////////////////////////////////////////////////
//
//	File	: PageCOMEnum.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: PageCOMEnum
//
//	Purpose	: DFU wizard page displayed while available COM ports
//			  are enumerated. This automatically advances to either
//			  the IDD_PAGE_COM or IDD_PAGE_CONNECTION page when
//			  complete, depending on whether any suitable ports were
//			  found.
//
//	Modification history:
//	
//		1.1		27:Oct:00	at	File created.
//		1.2		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.3		24:Nov:00	at	Removed enumeration of baud rates.
//		1.4 	23:Mar:01	at	Added directory to DFUEngine include.
//		1.5		17:May:01	at	Added automatic behaviour.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/PageCOMEnum.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef PAGECOMENUM_H
#define PAGECOMENUM_H

#if _MSC_VER > 1000
#pragma once
#endif

// PageCOMEnum class
class PageCOMEnum : public DFUPage
{
	DECLARE_DYNCREATE(PageCOMEnum)

public:

	// Constructor
	PageCOMEnum();

	// Destructor
	~PageCOMEnum();

	// Button management
	virtual LRESULT GetPageBack();
	virtual LRESULT GetPageNext();

	//{{AFX_DATA(PageCOMEnum)
	enum { IDD = IDD_PAGE_COMENUM };
	CAnimateCtrl animateCOMEnum;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(PageCOMEnum)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	// Delayed initialisation
	virtual void OnSetActiveDelayed();

	//{{AFX_MSG(PageCOMEnum)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif
