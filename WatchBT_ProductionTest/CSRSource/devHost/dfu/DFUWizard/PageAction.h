///////////////////////////////////////////////////////////////////////
//
//	File	: PageAction.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: PageAction
//
//	Purpose	: DFU wizard page to allow the upgrade or downgrade
//			  action to be selected. Unless a DFU has been uploaded
//			  previously only one option is offered. The following
//			  member variable contains the selection:
//				valueAction	- The action to perform.
//
//	Modification history:
//	
//		1.1		14:Sep:00	at	File created.
//		1.2		02:Oct:00	at	Added support for the simulated
//								connection option.
//		1.3		24:Oct:00	at	Corrected debugging macros.
//		1.4		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.5		28:Feb:01	at	Focus moved to an appropriate control
//								if no option selected.
//		1.6		28:Feb:01	at	Upload not forced in development mode.
//		1.7		23:Mar:01	at	Added directory to DFUEngine include.
//		1.8		12:Apr:01	at	Download without upload made default
//								for development mode.
//		1.9		17:May:01	at	Added automatic behaviour.
//		1.10	25:May:01	at	Improved validation of selected option
//								and automatic behaviour interaction.
//		1.11	29:May:01	at	Scroll bar added to description.
//		1.12	14:Nov:01	at	Added basic Unicode support.
//		1.13	13:Feb:02	at	Improved scroll bar handling.
//		1.14	15:Feb:02	at	StaticEdit control used.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/PageAction.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef PAGEACTION_H
#define PAGEACTION_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "DFUPage.h"
#include "StaticEdit.h"

// PageAction class
class PageAction : public DFUPage
{
	DECLARE_DYNCREATE(PageAction)

public:

	// Upgrade actions
	enum
	{
		upload_download,
		download,
		restore
	};

	// Constructor
	PageAction();

	// Button management
	virtual LRESULT GetPageBack();
	virtual LRESULT GetPageNext();

	//{{AFX_DATA(PageAction)
	enum { IDD = IDD_PAGE_ACTION };
	StaticEdit editComment;
	StaticEdit editBackupFile;
	CButton radioActionUploadDownload;
	CButton radioActionDownload;
	CButton radioActionRestore;
	int valueAction;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(PageAction)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	// Delayed initialisation
	virtual void OnSetActiveDelayed();

	// Helper
	void RefreshBackupFile();

	//{{AFX_MSG(PageAction)
	afx_msg void OnPageChanged();
	afx_msg void OnFileNameClick();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif
