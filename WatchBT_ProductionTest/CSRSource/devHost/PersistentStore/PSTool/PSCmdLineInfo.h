///////////////////////////////////////////////////////////////////////
//
//	File	: PSCmdLineInfo.h
//		  Copyright (C) 2001-2006 Cambridge Silicon Radio Limited
//
//	Author	: Mark Marshall (based on a file by A. Thoukydides_
//
//	Class	: PSCmdLineInfo
//
//	Purpose	: A replacement for the CCommandLineInfo class that
//		  processes command line options specific to the BC01
//		  Flash application.
//
//	  The currently supported options are:
//
//	/mul [portn]
//	    Enable the multispi port, (using portn)
//
//	Modification history:
//	
//	1.1	09:Oct:01	mm01	File created.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/PersistentStore/PSTool/PSCmdLineInfo.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef PSCmdLineInfo_H
#define PSCmdLineInfo_H

#if _MSC_VER > 1000
#pragma once
#endif

// DFUFileDialog class
class PSCmdLineInfo : public CCommandLineInfo  
{
public:

	// Parsed options
	bool m_bUseMultiSpi;
	int m_nMultiSpiPort;

	// Was an error found
	bool error;

	// Constructor
	PSCmdLineInfo();

	// Parse a single parameter
	virtual void ParseParam(const TCHAR *pszParam, BOOL bFlag, BOOL bLast);

private:

	// The most recent flag
	CString flag;
};

#endif
