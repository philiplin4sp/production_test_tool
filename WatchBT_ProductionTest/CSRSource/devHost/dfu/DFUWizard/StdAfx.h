///////////////////////////////////////////////////////////////////////
//
//	File	: StdAfx.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Purpose	: Include file for standard system include files that are
//			  used frequently, but are changed infrequently. The
//			  following files will be created:
//				DFUWizard.pch	- The pre-compiled header.
//				stdafx.obj		- The pre-compiled type information.
//
//	Modification history:
//	
//		1.1		20:Sep:00	at	File created.
//		1.2		26:Oct:00	at	Added afxtempl.h.
//		1.3		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.4		30:Apr:01	ckl	Added Windows CE support.
//		1.5		01:May:01	ckl	Backed out the previous change.
//		1.6		17:May:01	at	Updated the change history.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/StdAfx.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef STDAFX_H
#define STDAFX_H

#if _MSC_VER > 1000
#pragma once
#endif

// Exclude rarely-used stuff from Windows headers
#define VC_EXTRALEAN

// Include project header files
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxtempl.h>		// MFC templates
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif

//{{AFX_INSERT_LOCATION}}

#endif
