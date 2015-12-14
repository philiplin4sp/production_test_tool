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
//				DFUEngine.pch	- The pre-compiled header.
//				stdafx.obj		- The pre-compiled type information.
//
//	Modification history:
//	
//		1.1		20:Sep:00	at	File created.
//		1.2		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.3		30:Apr:01	ckl	Added Windows CE support.
//		1.4		23:May:01	at	Tidied up Windows CE support.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/StdAfx.h#1 $
//
///////////////////////////////////////////////////////////////////////

#ifndef STDAFX_H
#define STDAFX_H

#if _MSC_VER > 1000
#pragma once
#endif

// Exclude rarely-used stuff from Windows headers
#define VC_EXTRALEAN

#include <tchar.h>
#include "CStringX.h"
#include "CExceptionX.h"
#include "StringTable.h"

#endif
