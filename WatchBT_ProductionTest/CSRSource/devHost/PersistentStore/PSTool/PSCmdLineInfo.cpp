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
//	$Id: //depot/bc/bluesuite_2_4/devHost/PersistentStore/PSTool/PSCmdLineInfo.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "PSCmdLineInfo.h"
#include "unicode/ichar.h"

// Visual C++ debugging code
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif



// The available options
static const ichar option_mul[] = II("mul");

// Constructor
PSCmdLineInfo::PSCmdLineInfo()
{
	// Set the default options
	m_bUseMultiSpi = false;
	m_nMultiSpiPort = 0;

	// No errors initially
	error = false;
}

// Parse a single parameter
void PSCmdLineInfo::ParseParam(const TCHAR *pszParam, BOOL bFlag, BOOL bLast)
{
	CString param(pszParam);

	// Store as the most recent flag if appropriate
	if (bFlag) flag = param;

	// Action depends on the current flag
	if (flag.CompareNoCase(option_mul) == 0)
	{
		m_bUseMultiSpi = true;
		if (!bFlag) m_nMultiSpiPort = iatoi(param);
		if(m_nMultiSpiPort < 0 || m_nMultiSpiPort >= 16) m_nMultiSpiPort = 0;
	}
}
