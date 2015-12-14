///////////////////////////////////////////////////////////////////////
//
//	File	: PageResults.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: PageResults
//
//	Purpose	: DFU wizard page displayed at the end of the procedure
//			  to display the results. The following member variables
//			  should be set before this page is activated:
//				valueTitle		- The title for the page.
//				valueResults	- A summary of the results.
//				valueDetails	- Optional details of the results.
//				valueAnimate	- Should the success animation be
//								  displayed.
//				valueSaved		- Was the saved DFU file updated.
//				valueExit		- Should the wizard finish if
//								  successful.
//				valueExitOnError- Should the wizard finish if unsuccessful.
//
//	Modification history:
//	
//		1.1		20:Sep:00	at	File created.
//		1.2		09:Oct:00	at	Back button disabled on final page.
//								Details button added.
//		1.3		12:Oct:00	at	Improved formatting of details text.
//		1.4		24:Oct:00	at	Corrected debugging macros.
//		1.5		02:Oct:00	at	Added support for a success animation.
//		1.6		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.7		29:Nov:00	at	Back button enabled if not successful.
//		1.8		28:Feb:01	at	Back button not disabled in development
//								mode.
//		1.9		19:Mar:01	at	Window caption flashed when page
//								displayed without the focus.
//		1.10	17:May:01	at	Added automatic finish for success.
//		1.11	29:May:01	at	Added option to set description for
//								upload file.
//		1.12	14:Nov:01	at	Added basic Unicode support.
//		1.13	07:Jan:02	at	Added support for BC2 format DFU files.
//		1.14	11:Jan:02	at	Corrected handling of saved DFU file
//								comment if upgrade failed.
//		1.15	11:Jan:02	at	Comment not editable if upgrade failed.
//		1.16	13:Feb:02	at	Ask user if quit with changed comment.
//		1.17	14:Feb:02	at	Controls dynamically repositioned.
//		1.18	15:Feb:02	at	StaticEdit control used.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/PageResults.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef PAGERESULTS_H
#define PAGERESULTS_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "DFUPage.h"
#include "StaticEdit.h"

// PageResults class
class PageResults : public DFUPage
{
	DECLARE_DYNCREATE(PageResults)

public:

	// The title, results and optional details
	CString valueTitle;
	CString valueResults;
	CString valueDetails;

	// Control of animation
	bool valueAnimation;

	// Control of saved DFU file comment
	bool valueSaved;

	// Automatically finish if successful
	bool valueExit;

	// Automatically finish if unsuccessful (and valueExit is set)
	bool valueExitOnError;

	// Constructor
	PageResults();

	// Button management
	virtual LRESULT GetPageBack();

	//{{AFX_DATA(PageResults)
	enum { IDD = IDD_PAGE_RESULTS };
	StaticEdit editResults;
	CButton	buttonChange;
	CEdit editComment;
	CStatic	staticComment;
	CButton	buttonDetails;
	CAnimateCtrl animateResults;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(PageResults)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	// Timer to flash window caption
	UINT flashTimer;

	// Has the comment been changed
	bool commentChanged;

	// Delayed initialisation
	virtual void OnSetActiveDelayed();

	// Exit control
	virtual bool QueryClose(bool finish);

	//{{AFX_MSG(PageResults)
	afx_msg void OnDetailsClicked();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnChangeClicked();
	afx_msg void OnCommentChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif
