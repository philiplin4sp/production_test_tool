///////////////////////////////////////////////////////////////////////
//
//	File	: DFUPage.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFUPage
//
//	Purpose	: Class derived from CPropertyPage to add common
//			  functionality for all pages of the wizard.
//
//	Modification history:
//	
//		1.1		18:Sep:00	at	File created.
//		1.2		24:Oct:00	at	Corrected debugging macros.
//		1.3		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.4		17:May:01	at	Added automatic navigation.
//		1.5		25:May:01	at	Added intelligent string matching.
//		1.6		14:Nov:01	at	Added basic Unicode support.
//		1.7		13:Feb:02	at	Added QueryClose method.
//		1.8		13:Feb:02	at	Automatic scroll bar support.
//		1.9		14:Feb:02	at	Improved scroll bar support.
//		1.10	15:Feb:02	at	Scroll bar support moved to StaticEdit.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/DFUPage.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef DFUPAGE_H
#define DFUPAGE_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "DFUSheet.h"

// Declare the class to avoid recursive includes
class DFUSheet;

// DFUPage class
class DFUPage : public CPropertyPage
{
	DECLARE_DYNCREATE(DFUPage)

public:

	// Special page identifiers
	enum
	{
		automatic = 0,
		disable = -1,
		finish = -2,
		disablefinish = -3
	};

	// Constructors
	DFUPage();
	DFUPage(UINT id);

	// Obtain a pointer to the parent property sheet
	DFUSheet *GetSheet() const;

	// Button management
	virtual void SetButtons();
	virtual LRESULT GetPageBack();
	virtual LRESULT GetPageNext();

	// Page navigation
	void SetBack();
	void SetNext();

	// String matching
	static CString FindUnique(const CStringList &list, const CString &value,
	                          bool matchEmpty = true);

	//{{AFX_DATA(DFUPage)
	CStatic	staticTitle;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(DFUPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnQueryCancel();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	// Delayed initialisation
	virtual void OnSetActiveDelayed();

	// Exit control
	virtual bool QueryClose(bool finish);

	//{{AFX_MSG(DFUPage)
	afx_msg LRESULT OnSetActiveDelayedMsg(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif
