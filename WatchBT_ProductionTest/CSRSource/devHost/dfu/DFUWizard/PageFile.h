///////////////////////////////////////////////////////////////////////
//
//	File	: PageFile.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: PageFile
//
//	Purpose	: DFU wizard page to prompt the user to select the DFU
//			  file to download. The following member variable
//			  contains the selection:
//				valueName	- The selected file name.
//
//			  In development mode a list of recently used file names
//			  can be provided in the following member variable:
//				listNames	- Recently used file names.
//
//			  The name of the internally saved file for upload or
//			  restore operations is available from another member
//			  variable:
//				valueSaved	- The saved file name.
//
//	Modification history:
//	
//		1.1		20:Sep:00	at	File created.
//		1.2		24:Oct:00	at	Corrected debugging macros.
//		1.3		02:Nov:00	at	File open dialog initialised with
//								current filename.
//		1.4		08:Nov:00	at	Saved file name has full path
//								specified.
//		1.5		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.6		15:Dec:00	at	Added message giving reason for
//								disabling the next button.
//		1.7		27:Feb:01	at	DFU file description displayed if
//								available. File browser opened
//								automatically if file not valid.
//		1.8		28:Feb:01	at	Corrected disabling of Next button.
//		1.9		28:Feb:01	at	Made filename field editable in
//								development mode.
//		1.10	28:Feb:01	at	Prevented file browser from changing
//								directory.
//		1.11	02:Mar:01	at	Improved display of file comment.
//		1.12	23:Mar:01	at	Added directory to DFUEngine include.
//		1.13	17:May:01	at	Added automatic behaviour.
//		1.14	18:May:01	at	Uploaded file stored in user profile
//								if possible, otherwise the application
//								directory is used.
//		1.15	21:May:01	at	Used SHGetFolderPath instead of
//								SHGetSpecialFolderPath
//								(requires SHFolder.dll).
//		1.16	23:May:01	at	Replaced SHGFP_TYPE_CURRENT by value
//								to reduce dependency on include file
//								ordering.
//		1.17	14:Nov:01	at	Added basic Unicode support.
//		1.18	08:Mar:02	at	Added MRU list in development mode.
//		1.19	11:Mar:02	at	Ensure drop down list is wide enough.
//		1.20	11:Mar:02	at	Correct font used for width calculation.
//		1.21	28:Mar:02	at	Corrected comment update after browse.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/PageFile.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef PAGEFILE_H
#define PAGEFILE_H

#if _MSC_VER > 1000
#pragma once
#endif

// PageFile class
class PageFile : public DFUPage
{
	DECLARE_DYNCREATE(PageFile)

public:

	// The selected file name
	CString valueName;

	// Saved file
	CString valueSaved;

	// Recently used files
	CStringList listNames;

	// Constructor
	PageFile();

	// Button management
	virtual LRESULT GetPageNext();

	//{{AFX_DATA(PageFile)
	enum { IDD = IDD_PAGE_FILE };
	CComboBox comboName;
	CStatic staticName;
	StaticEdit editComment;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(PageFile)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	// Delayed initialisation
	virtual void OnSetActiveDelayed();

	//{{AFX_MSG(PageFile)
	afx_msg void OnClickedBrowse();
	afx_msg void OnEditChanged();
	afx_msg void OnSelChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif
