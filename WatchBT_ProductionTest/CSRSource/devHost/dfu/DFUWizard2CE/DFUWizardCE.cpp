///////////////////////////////////////////////////////////////////////
//
//	File	: DFUWizardCE.cpp
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: D. Coultous
//
//	Class	: DFUWizardCE
//
//	Purpose	: The DFUWizard for Windows CE. This performs upgrade of existing 
//			  firmware and download of selected firmware
//
//	Modification history (as DFUTest.cpp):
//	
//		1.1		26:Jul:02	doc	File created.
//		1.2		06:Aug:02	doc	Simplified for Windows CE
//		1.3		16:Sep:02	doc	Some minor alterations.
//		1.4		11:Oct:02	doc Added default browse location.
//		1.5		30:oct:02	doc	Updated modification history
//		1.6		19:Nov:02	doc	Added Registry changes to disable system timeouts
//		1.7		20:Nov:02	doc Retracted 1.6 changes, now using SystemIdleTimerReset()
//
//  and as DFUWizardCE.cpp :
//
//		1.1		21:Nov:02	doc	Recreated as DFUWizardCE.cpp
//		1.2		12:dec:02	doc Tidied application structure
//		1.3		13:dec:02	doc	Cleared default file selection if not valid
//		1.4		16:dec:02	doc	Changed default default location to "My Documents\bluecore.dfu"
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard2CE/DFUWizardCE.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUWizardCE.h"
#include "PromptDialog.h"
#include "UpgradeDialog.h"

// Visual C++ debugging code
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// Registry keys
static const TCHAR keyRoot[] = _T("0.01");
static const TCHAR keyDownload[] = _T("Download");

// The application all sits inside this object
DFUWizardCEApp theApp;

// Message map
BEGIN_MESSAGE_MAP(DFUWizardCEApp, CWinApp)
	//{{AFX_MSG_MAP(DFUWizardCEApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

// Constructor
DFUWizardCEApp::DFUWizardCEApp()
{
}

// The main program
BOOL DFUWizardCEApp::InitInstance()
{
	// Initialise the DLL
	DFUEngine::InitDLL();

	// Use the registry for persistent settings
	SetRegistryKey(IDS_REGISTRY_COMPANY);

	// Initialistion succeeds
	return TRUE;
}

// the body of the application
int DFUWizardCEApp::Run()
{
	// Restore any previous options
	PromptDialog prompt;
	prompt.valueDownload = GetProfileString(keyRoot, keyDownload);
	if (prompt.valueDownload.IsEmpty())
		prompt.valueDownload = CString("\\My Documents\\bluecore.dfu");
	// empty the stored settings if not valid anymore!!
	if (!DFUEngine::IsDFUFileValid(prompt.valueDownload))
		prompt.valueDownload = CString("");
	
	// Prompt the user for the options
	if (prompt.DoModal() == IDOK)
	{
		// Save the options (if not the saved ones :-)
		if (prompt.valueDownload != CString(saved_file)) 
			WriteProfileString(keyRoot, keyDownload, prompt.valueDownload);

		// Start performing upgrades
		UpgradeDialog upgrade;
		upgrade.dfuDownload = prompt.valueDownload;
		upgrade.dfuConnection = prompt.valueConnection;
		int reply = upgrade.DoModal(); 		
	}
	return 0;
}