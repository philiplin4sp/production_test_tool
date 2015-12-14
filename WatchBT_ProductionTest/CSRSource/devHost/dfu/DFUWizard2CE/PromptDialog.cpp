///////////////////////////////////////////////////////////////////////
//
//	File	: PromptDialog.cpp
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: PromptDialog
//
//	Purpose	: Display and handle the options dialog.
//
//			  The following member variables can be set before invoking
//			  the dialog and checked after it has closed:
//				valueConnection		- The name of the COM port or USB
//									  device.
//				valueDownload		- Name of file or directory to
//									  download, or empty for none.
//
//	Modification history:
//	
//		1.1		26:Jul:02	doc	File created.
//		1.2		06:Aug:02	doc	Simplified for Windows CE
//		1.3		16:Sep:02	doc	Some minor alterations.
//		1.4		11:Oct:02	doc Added browse and Restore button fxnality
//		1.5		30:oct:02	doc	Updated modification history
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard2CE/PromptDialog.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "dfu\DFUEngine.h"
#include "DFUWizardCE.h"
#include "PromptDialog.h"
#include "DFUFileDialog.h"

// Visual C++ debugging code
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// timer id for updates
#define TIMER_ID_CHECK_OK	WM_USER + 42
// Constructor
PromptDialog::PromptDialog(CWnd* pParent) : CDialog(PromptDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(PromptDialog)
	valueDownload = _T("");
	valueDownloadComment = _T("");
	//}}AFX_DATA_INIT
}

// Data exchange
void PromptDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PromptDialog)
	DDX_Control(pDX, IDOK, buttonOK);
	DDX_Control(pDX, IDC_PROMPT_CONNECTION_COMBO, comboConnection);
	
	DDX_CBString(pDX, IDC_PROMPT_CONNECTION_COMBO, valueConnection);
	DDX_Text(pDX, IDC_PROMPT_DOWNLOAD_EDIT, valueDownload);
	DDX_Text(pDX, IDC_PROMPT_DOWNLOAD_COMMENT_EDIT, valueDownloadComment);
	//}}AFX_DATA_MAP
}

// Message map
BEGIN_MESSAGE_MAP(PromptDialog, CDialog)
	//{{AFX_MSG_MAP(PromptDialog)
	ON_BN_CLICKED(IDC_PROMPT_DOWNLOAD_BUTTON, OnDownloadButton)
	ON_BN_CLICKED(IDC_RESTORE_BUTTON, OnRestoreButton)
	ON_EN_CHANGE(IDC_PROMPT_DOWNLOAD_EDIT, OnChange)
	ON_CBN_SELCHANGE(IDC_PROMPT_CONNECTION_COMBO, OnChange)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Initialise
BOOL PromptDialog::OnInitDialog()
{
	// Let the base class perform any necessary initialisation
	CDialog::OnInitDialog();

	// Set the icon for this dialog
	HICON icon = AfxGetApp()->LoadIcon(IDI_MAINFRAME);
	SetIcon(icon, TRUE);
	SetIcon(icon, FALSE);

	// Disable the maximize and size options on the system menu
	HMENU menu = GetSystemMenu(m_hWnd, false);
	EnableMenuItem(menu, 0/*SC_MAXIMIZE*/, MF_BYCOMMAND | MF_GRAYED);
	EnableMenuItem(menu, 0/*SC_SIZE*/, MF_BYCOMMAND | MF_GRAYED);

	// Set the initial list of connections
	OnChangeCOM();
	UpdateData(false);

	// Enable the OK button if appropriate
	OnChange();

	// Input focus has not been set
	return TRUE;
}

// Enable or disable the OK button
void PromptDialog::OnChange() 
{
	SetTimer(TIMER_ID_CHECK_OK, 10, NULL);
}

void PromptDialog::OnTimer(UINT id_event)
{
	if (id_event == TIMER_ID_CHECK_OK)
	{
		KillTimer(id_event);
		UpdateData();
		// Enable the restore button if valid saved file (use valueDownloadComment as a dummy, as about
		// to be overwritten
		CWnd * buttonDownload = GetDlgItem(IDC_RESTORE_BUTTON);
		{
			CStringX valueDownloadCommentx;
			buttonDownload->EnableWindow(DFUEngine::IsDFUFileValid(saved_file, valueDownloadCommentx));
			valueDownloadComment = valueDownloadCommentx;
		}
		// enable/Disable the Ok button depending on validity of options
		bool valid = !valueConnection.IsEmpty();
		if (valueDownload.IsEmpty())
		{
			valueDownloadComment.Empty();
		}
		else
		{
			CStringX valueDownloadCommentx;
			if (DFUEngine::IsDFUFileValid(valueDownload, valueDownloadCommentx))
			{
				valueDownloadComment = valueDownloadCommentx;
				if (valueDownloadComment.IsEmpty())
				{
					valueDownloadComment.Format(IDS_DOWNLOAD_NO_COMMENT);
				}
			}
			else
			{
				valid = false;
				valueDownloadComment.Format(IDS_DOWNLOAD_NOT_VALID);
			}
		}
		buttonOK.EnableWindow(valid);
		UpdateData(false);
	}
	else
	{
		// only one timer at the moment
		ASSERT(FALSE);
	}
}

// Set the list of serial ports
void PromptDialog::OnChangeCOM() 
{
	CStringListX listPorts;
	DFUEngine::GetCOMPortNames(listPorts);
	comboConnection.ResetContent();
	for (CStringListX::POSITION port = listPorts.GetHeadPosition(); port;)
	{
		comboConnection.AddString(listPorts.GetNext(port));
	}
	int ports = listPorts.GetCount();
	comboConnection.EnableWindow(0 < ports);
	if (ports == 1) comboConnection.SetCurSel(0);
	else comboConnection.SelectString(-1, valueConnection);
	OnChange();
}

void PromptDialog::OnDownloadButton() 
{
	UpdateData();

	// Browse for a file
	DFUFileDialog file(true, valueDownload,
		               OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_READONLY, this);
	if (file.DoModal() != IDOK) return;
	valueDownload = file.GetPathName();

	// Update the dialog
	UpdateData(false);
	OnChange();
}

void PromptDialog::OnRestoreButton() 
{
	UpdateData();

	// Set the download file to the saved file
	valueDownload = saved_file;

	// Update the dialog
	UpdateData(false);
	OnChange();
}
