// WatchBT_ProductionTest.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CWatchBT_ProductionTestApp:
// See WatchBT_ProductionTest.cpp for the implementation of this class
//

class CWatchBT_ProductionTestApp : public CWinApp
{
public:
	CWatchBT_ProductionTestApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CWatchBT_ProductionTestApp theApp;