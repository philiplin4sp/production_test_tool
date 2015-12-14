///////////////////////////////////////////////////////////////////////
//
//	File	: DFUWizard.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFUWizardApp
//
//	Purpose	: The main BlueCore Device Firmware Upgrade Wizard
//			  application. This prepares the wizard by restoring any
//			  previously saved settings, and then runs it as a modal
//			  dialog. If the wizard completes then the settings are
//			  saved and the application exits.
//
//			  The application version number and copyright message
//			  are obtained from the VS_VERSION_INFO resource. The
//			  ProductVersion, SpecialBuild and LegalCopyright
//			  strings are used. If the SpecialBuild string is set to
//			  "Demonstration" then a simulated connection options is
//			  made available to allow testing and demonstrations
//			  without requiring additional hardware.
//
//	Modification history:
//	
//		1.1		14:Sep:00	at	File created.
//		1.2		29:Sep:00	at	Added support for HCI tunnelling
//								and simulated connections.
//		1.3		09:Oct:00	at	Added example of memory allocation
//								breakpoints.
//		1.4		24:Oct:00	at	Corrected debugging macros.
//		1.5		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.6		08:Dec:00	at	Added support for different BCSP link
//								establishment modes.
//		1.7		28:Feb:01	at	Added processing of command line
//								options.
//		1.8		23:Mar:01	at	Added directory to DFUEngine include.
//		1.9		17:May:01	at	Improved command line support.
//		1.10	18:May:01	at	Changed exit code to indicate whether
//								the most recent upgrade was successful.
//		1.11	25:Jun:01	at	Moved VersionInfo to stand-alone lib.
//		1.12	03:Jul:01	ckl	Uses simplified VersionInfo.
//		1.13	04:Jul:01	ckl	Updated version history.
//		1.14	08:Oct:01	at	Added /noabort command line option.
//		1.15	14:Nov:01	at	Added basic Unicode support.
//		1.16	13:Dec:01	ckl	VersionInfo now uses std::string.
//		1.17	12:Feb:02	at	VersionInfo now templated.
//		1.18	08:Mar:02	at	Added MRU list in development mode.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/DFUWizard.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef DFUWIZARD_H
#define DFUWIZARD_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "resource.h"
#include "DFUSheet.h"

// DFUWizardApp class
class DFUWizardApp : public CWinApp
{
public:

	// Constructor
	DFUWizardApp();

	//{{AFX_VIRTUAL(DFUWizardApp)
	public:
	virtual BOOL InitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(DFUWizardApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:

	// Program version
	CString version;

	// Persistent store of settings
	void GetProfile(DFUSheet &wizard);
	void WriteProfile(DFUSheet &wizard);

	// Command line settings
	void GetCommandLine(DFUSheet &wizard);
};

//{{AFX_INSERT_LOCATION}}

#endif
