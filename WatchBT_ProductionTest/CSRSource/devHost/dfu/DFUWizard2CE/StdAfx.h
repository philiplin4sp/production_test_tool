///////////////////////////////////////////////////////////////////////
//
//	File	: StdAfx.cpp
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Purpose	: Include file for standard system include files that are
//			  used frequently, but are changed infrequently. The
//			  following files will be created:
//				DFUTest.pch		- The pre-compiled header.
//				stdafx.obj		- The pre-compiled type information.
//
//	Modification history:
//	
//		1.1		02:Nov:00	at	File created.
//		1.2		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard2CE/StdAfx.h#1 $
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
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif

//{{AFX_INSERT_LOCATION}}

#endif
