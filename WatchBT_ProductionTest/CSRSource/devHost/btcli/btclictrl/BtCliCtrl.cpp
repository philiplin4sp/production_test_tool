// BtCliCtrl.cpp : Defines the class behaviors for the application.
//

/* 

  $Log:$

*/


#include "stdafx.h"
#include "BtCliCtrl.h"
#include "BtCliCtrlDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBtCliCtrlApp

BEGIN_MESSAGE_MAP(CBtCliCtrlApp, CWinApp)
	//{{AFX_MSG_MAP(CBtCliCtrlApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBtCliCtrlApp construction

CBtCliCtrlApp::CBtCliCtrlApp()
{
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CBtCliCtrlApp object

CBtCliCtrlApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CBtCliCtrlApp initialization

BOOL CBtCliCtrlApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if WINVER < 0x0500
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	CBtCliCtrlDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
