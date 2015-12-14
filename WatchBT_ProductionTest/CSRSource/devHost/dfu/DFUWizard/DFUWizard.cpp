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
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/DFUWizard.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUWizard.h"
#include "versioninfo\VersionInfo.h"
#include "dfu\DFUEngine.h"
#include "DFUCmdLineInfo.h"

// Visual C++ debugging code
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// Registry keys
static const TCHAR app_key_connection[] = _T("Connection Type");
static const TCHAR app_key_usb_device[] = _T("USB Device");
static const TCHAR app_key_com_port[] = _T("Serial Port");
static const TCHAR app_key_com_auto[] = _T("Autodetect Protocol");
static const TCHAR app_key_com_baud[] = _T("Serial Baud Rate");
static const TCHAR app_key_com_protocol[] = _T("Serial Protocol");
static const TCHAR app_key_com_tunnel[] = _T("HCI Tunnelling");
static const TCHAR app_key_com_link[] = _T("Link Establishment");
static const TCHAR app_key_file[] = _T("DFU Filename");
static const TCHAR app_key_saved_file[] = _T("DFU Backup Filename");
static const TCHAR app_key_file_mru[] = _T("DFU Filename MRU #%d");
static const int app_key_mru_max = 5;

// Program exit codes
static const int rc_success = 0;
static const int rc_fail = 1;
static const int rc_cancel = 2;

// The application all sits inside this object
DFUWizardApp theApp;

// Message map
BEGIN_MESSAGE_MAP(DFUWizardApp, CWinApp)
	//{{AFX_MSG_MAP(DFUWizardApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

// Constructor
DFUWizardApp::DFUWizardApp()
{
#ifdef _DEBUG
	// Enable debugging of memory management
	int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpFlag |= _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF;
	tmpFlag &= ~_CRTDBG_CHECK_CRT_DF;
	_CrtSetDbgFlag(tmpFlag);
	//_CrtSetBreakAlloc(alocnum);
#endif
#ifndef NO_HTML_HELP
	EnableHtmlHelp();
#endif
}

// Persistent store of settings
void DFUWizardApp::GetProfile(DFUSheet &wizard)
{
	wizard.pageConnection.valueConnection = GetProfileInt(version, app_key_connection, -1);
	wizard.pageUSB.valueUSBNum = GetProfileString(version, app_key_usb_device);
	wizard.pageCOM.valueCOMPort = GetProfileString(version, app_key_com_port);
	wizard.pageCOM.valueCOMAuto = GetProfileInt(version, app_key_com_auto, 1);
	wizard.pageCOM.valueBaud = GetProfileString(version, app_key_com_baud);
	wizard.pageCOM.valueProtocol = GetProfileInt(version, app_key_com_protocol, -1);
	wizard.pageCOM.valueTunnel = GetProfileInt(version, app_key_com_tunnel, -1);
	wizard.pageCOM.valueLink = GetProfileInt(version, app_key_com_link, -1);
	wizard.pageFile.valueName = GetProfileString(version, app_key_file);
	CString DFUBackup = GetProfileString(version, app_key_saved_file);
	if(!DFUBackup.IsEmpty()) wizard.pageFile.valueSaved = DFUBackup;

	CStringList &listNames = wizard.pageFile.listNames;
	listNames.RemoveAll();
	for (int i = 1; i <= app_key_mru_max; ++i)
	{
		CString key;
		key.Format(app_key_file_mru, i);
		CString file = GetProfileString(version, key);
		if (!file.IsEmpty()) listNames.AddTail(file);
	}
}

void DFUWizardApp::WriteProfile(DFUSheet &wizard)
{
	WriteProfileInt(version, app_key_connection, wizard.pageConnection.valueConnection);
	WriteProfileString(version, app_key_usb_device, wizard.pageUSB.valueUSBNum);
	WriteProfileString(version, app_key_com_port, wizard.pageCOM.valueCOMPort);
	WriteProfileInt(version, app_key_com_auto, wizard.pageCOM.valueCOMAuto);
	WriteProfileString(version, app_key_com_baud, wizard.pageCOM.valueBaud);
	WriteProfileInt(version, app_key_com_protocol, wizard.pageCOM.valueProtocol);
	WriteProfileInt(version, app_key_com_tunnel, wizard.pageCOM.valueTunnel);
	WriteProfileInt(version, app_key_com_link, wizard.pageCOM.valueLink);
	WriteProfileString(version, app_key_file, wizard.pageFile.valueName);
	WriteProfileString(version, app_key_saved_file, wizard.pageFile.valueSaved);
	CStringList &listNames = wizard.pageFile.listNames;
	POSITION file = listNames.GetHeadPosition();
	for (int i = 1; (i <= app_key_mru_max) && file; ++i)
	{
		CString key;
		key.Format(app_key_file_mru, i);
		WriteProfileString(version, key, listNames.GetNext(file));
	}
}

// Command line settings
void DFUWizardApp::GetCommandLine(DFUSheet &wizard)
{
	// Process the command line
	DFUCmdLineInfo cmdLineInfo;
	ParseCommandLine(cmdLineInfo);

	// Set options from the command line
	wizard.developmentMode = cmdLineInfo.developmentMode;
	if (cmdLineInfo.doUpload) wizard.pageAction.valueAction = PageAction::upload_download;
	else if	(cmdLineInfo.doDownload) wizard.pageAction.valueAction = PageAction::download;
	else if (cmdLineInfo.doRestore) wizard.pageAction.valueAction = PageAction::restore;
	if (!cmdLineInfo.doDownloadFile.IsEmpty()) wizard.pageFile.valueName = cmdLineInfo.doDownloadFile;
	if (cmdLineInfo.connectCOM) wizard.pageConnection.valueConnection = PageConnection::com;
	else if (cmdLineInfo.connectUSB) wizard.pageConnection.valueConnection = PageConnection::usb;
	if (!cmdLineInfo.connectCOMPort.IsEmpty()) wizard.pageCOM.valueCOMPort = cmdLineInfo.connectCOMPort;
	if (!cmdLineInfo.connectUSBDevice.IsEmpty()) wizard.pageUSB.valueUSBNum = cmdLineInfo.connectUSBDevice;
	wizard.automatic = cmdLineInfo.autoStart;
	wizard.pageResults.valueExit = cmdLineInfo.autoExit;
	wizard.pageResults.valueExitOnError = cmdLineInfo.autoExitOnError;
	wizard.pageProgress.valueNoAbort = cmdLineInfo.noAbort;
}

// Initialisation
BOOL DFUWizardApp::InitInstance()
{
	// Enable use of Active X controls
	AfxEnableControlContainer();

#if WINVER < 0x0500 
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	// Initialise the DLL
	DFUEngine::InitDLL();

	// Use the registry for persistent settings
	SetRegistryKey(IDS_REGISTRY_COMPANY);

    // Change to look for the chm file in a "help" subdirectory
    CString sAppPath = m_pszHelpFilePath;
    int iPos = sAppPath.ReverseFind( '\\' );
    CString newHelpFile = sAppPath.Mid(0, iPos) + "\\help" + sAppPath.Right(sAppPath.GetLength()-iPos);
    free((void*)m_pszHelpFilePath);
    m_pszHelpFilePath=_tcsdup(newHelpFile);

	// Successfully initialised
	return true;
}

// The body of the application
int DFUWizardApp::Run() 
{
	// Obtain the application version number
#ifdef UNICODE
	VersionInfo<std::wstring> versionInfo;
#else
	VersionInfo<std::string> versionInfo;
#endif
	version = versionInfo.GetProductVersion().c_str();

	// Restore any previous settings
	DFUSheet wizard;
	GetProfile(wizard);

	// Set options from the command line
	GetCommandLine(wizard);

	// Set the version number and copyright messages
	CString textVersion = versionInfo.GetFileVersion().c_str();
	CString textBuild = versionInfo.GetSpecialBuild().c_str();
	CString textCopyright = versionInfo.GetLegalCopyright().c_str();
	if (textBuild.IsEmpty()) wizard.pageIntro.valueVersion.Format(IDS_VERSION_FORMAT, (LPCTSTR) textVersion);
	else wizard.pageIntro.valueVersion.Format(IDS_VERSION_BUILD_FORMAT, (LPCTSTR) textVersion, (LPCTSTR) textBuild);
	wizard.pageIntro.valueCopyright = textCopyright;

	// Enable simulated connections for demonstration builds only
	wizard.pageConnection.enableSimulated = textBuild == _T("Demonstration");

	// Run the wizard
	int rc;
	if (wizard.DoModal() == ID_WIZFINISH)
	{
		// The wizard completed normally so save the settings
		WriteProfile(wizard);
		rc = wizard.pageResults.valueAnimation ? rc_success : rc_fail;
	}
	else
	{
		// The wizard was cancelled
		rc = rc_cancel;
	}

	// Value to be returned by WinMain
	return rc;
}
