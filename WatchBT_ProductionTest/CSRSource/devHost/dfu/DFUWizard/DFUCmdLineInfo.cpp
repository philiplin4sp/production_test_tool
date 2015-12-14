///////////////////////////////////////////////////////////////////////
//
//	File	: DFUCmdLineInfo.cpp
//		  Copyright (C) 2001-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFUCmdLineInfo
//
//	Purpose	: A replacement for the CCommandLineInfo class that
//			  processes command line options specific to the DFU
//			  Wizard application.
//
//			  The currently supported options are:
//
//				/dev
//					Enable development mode features. This sets the
//					developmentMode member variable.
//
//				/upload
//					Perform a file upload before downloading a new
//					file. This sets the doUpload member variable.
//
//				/download [file]
//					Perform a download of the specified file. This
//					sets the doDownload and doDownloadFile member
//					variables.
//
//				/restore
//					Restore the previously uploaded file. This sets
//					the doRestore member variable.
//
//				/serial [port]
//					Use a COM port connection. This sets the
//					connectCOM and connectCOMPort member variables.
//
//				/usb [device]
//					Use a USB connection. This sets the connectUSB
//					and connectUSBDevice member variables.
//
//				/auto
//					Start the upgrade operation automatically if
//					possible. This sets the autoStart member variable.
//
//				/exit
//					Exit automatically if the upgrade was successful.
//					This sets the autoExit member variable.
//
//				/skiperr
//					If /exit is specified then exit automatically if errors occurred as well.
//					This sets the autoExitOnError member variable.
//
//				/noabort
//					Disable the Abort button. This sets the noAbort
//					member variable.
//
//	Modification history:
//	
//		1.1		28:Feb:01	at	File created.
//		1.2		17:May:01	at	Added transport and action options.
//		1.3		08:Oct:01	at	Added /noabort option.
//		1.4		14:Nov:01	at	Added basic Unicode support.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/DFUCmdLineInfo.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUCmdLineInfo.h"

// Visual C++ debugging code
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// The available options
static const TCHAR option_dev[] = _T("dev");
static const TCHAR option_upload[] = _T("upload");
static const TCHAR option_download[] = _T("download");
static const TCHAR option_restore[] = _T("restore");
static const TCHAR option_serial[] = _T("serial");
static const TCHAR option_usb[] = _T("usb");
static const TCHAR option_auto[] = _T("auto");
static const TCHAR option_exit[] = _T("exit");
static const TCHAR option_skiperr[] = _T("skiperr");
static const TCHAR option_abort[] = _T("noabort");

// Constructor
DFUCmdLineInfo::DFUCmdLineInfo()
{
	// Set the default options
	developmentMode = false;
	doUpload = false;
	doDownload = false;
	doDownloadFile.Empty();
	doRestore = false;
	connectCOM = false;
	connectCOMPort.Empty();
	connectUSB = false;
	connectUSBDevice.Empty();
	autoStart = false;
	autoExit = false;
	autoExitOnError = false;
	noAbort = false;

	// No errors initially
	error = false;
}

// Parse a single parameter
void DFUCmdLineInfo::ParseParam(const TCHAR *pszParam, BOOL bFlag, BOOL bLast)
{
	CString param(pszParam);

	// Store as the most recent flag if appropriate
	if (bFlag) flag = param;

	// Action depends on the current flag
	if (flag.CompareNoCase(option_dev) == 0)
	{
		developmentMode = true;
	}
	else if (flag.CompareNoCase(option_upload) == 0)
	{
		doUpload = true;
	}
	else if (flag.CompareNoCase(option_download) == 0)
	{
		doDownload = true;
		if (!bFlag) doDownloadFile = param;
	}
	else if (flag.CompareNoCase(option_restore) == 0)
	{
		doRestore = true;
	}
	else if (flag.CompareNoCase(option_serial) == 0)
	{
		connectCOM = true;
		if (!bFlag) connectCOMPort = param;
	}
	else if (flag.CompareNoCase(option_usb) == 0)
	{
		connectUSB = true;
		if (!bFlag) connectUSBDevice = param;
	}
	else if (flag.CompareNoCase(option_auto) == 0)
	{
		autoStart = true;
	}
	else if (flag.CompareNoCase(option_exit) == 0)
	{
		autoExit = true;
	}
	else if (flag.CompareNoCase(option_skiperr) == 0)
	{
		autoExitOnError = true;
	}
	else if (flag.CompareNoCase(option_abort) == 0)
	{
		noAbort = true;
	}
}
