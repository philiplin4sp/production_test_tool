// PSTool.cpp : Defines the class behaviors for the application.
//

/*
 MODIFICATION HISTORY
		1.11   30:nov:00  Chris Lowe 	Working
		$Id: //depot/bc/bluesuite_2_4/devHost/PersistentStore/PSTool/PSTool.cpp#1 $
*/


#include "stdafx.h"
#include "PSTool.h"
#include "PSToolDlg.h"
#include "ChooseTransDlg.h"
#include "PSCmdLineInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPSToolApp

BEGIN_MESSAGE_MAP(CPSToolApp, CWinApp)
	//{{AFX_MSG_MAP(CPSToolApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPSToolApp construction

CPSToolApp::CPSToolApp()
{
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPSToolApp object

CPSToolApp theApp;

////////////////////////////////////////////////////////////////////////////
// CPSToolApp initialization
BOOL CPSToolApp::InitInstance()
{
	AfxEnableControlContainer();

	SetRegistryKey(II("Cambridge Silicon Radio\\Developement Kit"));

    CPSHelper helper;
    CPSToolDlg dlg(NULL, this);
    m_pMainWnd = &dlg;
    dlg.SetHelper(&helper);
    dlg.DoModal();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	if(PsServer *pss = helper.GetServer())
	{
		pss->close();
		delete pss;
	}
	return FALSE;
}
