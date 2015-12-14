///////////////////////////////////////////////////////////////////////
//
//	File	: DFUCmdLineInfo.h
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
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/DFUCmdLineInfo.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef DFUCMDLINEINFO_H
#define DFUCMDLINEINFO_H

#if _MSC_VER > 1000
#pragma once
#endif

// DFUFileDialog class
class DFUCmdLineInfo : public CCommandLineInfo  
{
public:

	// Parsed options
	bool developmentMode;
	bool doUpload;
	bool doDownload;
	CString doDownloadFile;
	bool doRestore;
	bool connectCOM;
	CString connectCOMPort;
	bool connectUSB;
	CString connectUSBDevice;
	bool autoStart;
	bool autoExit;
	bool autoExitOnError;
	bool noAbort;

	// Was an error found
	bool error;

	// Constructor
	DFUCmdLineInfo();

	// Parse a single parameter
	virtual void ParseParam(const TCHAR *pszParam, BOOL bFlag, BOOL bLast);

private:

	// The most recent flag
	CString flag;
};

#endif
