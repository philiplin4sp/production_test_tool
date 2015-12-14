///////////////////////////////////////////////////////////////////////
//
//	File	: UpgradeDialog.cpp
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: UpgradeDialog
//
//	Purpose	: Perform a continuous cycle of upgrades until an error
//			  occurs.
//
//			  The following member variables should be set before
//			  invoking the dialog:
//				dfuConnection		- The name of the COM port or USB
//									  device.
//				dfuDownload			- Name of file or directory to
//									  download, or empty for none.
//
//	Modification history:
//	
//		1.1		26:Jul:02	doc	Created from Alex Thoukydides' class in DFUTest
//		1.2		16:Sep:02	doc	Cut down from Windows version to one suitable for CE
//		1.3		16:Sep:02	doc	Further trimming
//		1.4		11:Oct:02	doc	Added abort query
//		1.5		30:Oct:02	doc	Updated modification history
//		1.6		20:Nov:02	doc	Prevented System shutting down during lifetime
//								and ClearDisplay() fxn.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard2CE/UpgradeDialog.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUWizardCE.h"
#include "UpgradeDialog.h"

// Visual C++ debugging code
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// Uncomment the next line to simulate downloads being aborted
//#define ABORT_DOWNLOADS

// Target height for log scrolling
static const int logScrollLimit = 30;

#define UM_CALLBACK (WM_USER + 1)

// Constructor
UpgradeDialog::UpgradeDialog(CWnd* pParent) : CDialog(UpgradeDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(UpgradeDialog)
	//}}AFX_DATA_INIT

	// Not active initially
	dfuActive = false;

	// Window not flashing initially
	flashTitle = false;
	flashTimer = 0;

	// Size not initialised
	minSize = prevSize = CSize(0, 0);

}

// Data exchange
void UpgradeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UpgradeDialog)
	DDX_Control(pDX, IDC_UPGRADE_LOG_EDIT, EditLog);
	DDX_Control(pDX, IDC_UPGRADE_PROGRSS, progressUpgrade);
	//}}AFX_DATA_MAP
}

// Message map
BEGIN_MESSAGE_MAP(UpgradeDialog, CDialog)
	//{{AFX_MSG_MAP(UpgradeDialog)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_OK_BUTTON, OnOK)
	ON_MESSAGE(UM_CALLBACK, OnCallback)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Initialise
BOOL UpgradeDialog::OnInitDialog()
{
	// Let the base class perform any necessary initialisation
	CDialog::OnInitDialog();
	
	// Record the initial window size
	CRect rect;
	GetWindowRect(&rect);
	minSize = rect.Size();

	// Set the icon for this dialog
	HICON icon = AfxGetApp()->LoadIcon(IDI_MAINFRAME);
	SetIcon(icon, TRUE);
	SetIcon(icon, FALSE);

	// Initialise the log control settings
	red = PALETTERGB(0xff, 0, 0);
	green = PALETTERGB(0, 0x80, 0);
	blue = PALETTERGB(0, 0, 0xff);
	logTemporary_Min = logTemporary_Max = 0;

	// Reset the status
	Reset(); 

	// Configure and start the DFU Engine
	Start();

	// Input focus has not been set
	return TRUE;
}

// Reset the list of download files and statistics
void UpgradeDialog::Reset()
{
	// Prepare a list of the files to download
	if (dfuDownload.IsEmpty())
	{
		// No files to download
	}
	else
	{
		// Just a single file to download
		dfuDownloadList.AddTail(dfuDownload);
	}

	// Start from the first file .
	dfuDownloadPos = 0;
	
}

// Start and stop upgrade cycles
void UpgradeDialog::Start()
{
	// Start a timer to flash the title bar (and reset the timeouts so
	// the device does not switch off mid operation!)
	flashTimer = SetTimer(1, 500, 0);

	// Choose the download file
	if (!dfuDownloadPos)
	{
		dfuDownloadPos = dfuDownloadList.GetHeadPosition();
	}
	if (dfuDownloadPos) dfuDownloadFile = dfuDownloadList.GetNext(dfuDownloadPos);
	else dfuDownloadFile.Empty();

	// We wish to upload to saved file location (which is constant for now)
	if (dfuDownloadFile != saved_file)
		dfuUploadFile = dfuUpload = saved_file;
	else
		// if we are restoring the saved file we do not wish to update
		dfuUploadFile.Empty();

	// disable ok button to start with
	GetDlgItem(IDC_OK_BUTTON)->SetWindowText(CString("Abort"));

	// Configure the DFU engine if not already done
	if (!dfuActive)
	{
		dfuActive = true;
		dfu.SetCallback(this);
		if (!dfu.CreateCOM(dfuConnection))
		{
			CString fail;
			fail.Format(IDS_MSG_FAIL_CREATE, dfuConnection);
			MessageBox(fail);
			Stop();
			return;
		}
	}

	// Start the first stage
	if (!dfu.StartReconfigure(!dfuUploadFile.IsEmpty(),
	                          !dfuDownloadFile.IsEmpty(),
	                          dfuDownloadFile))
	{
		CString fail;
		DFUEngine::Status status;
		dfu.GetStatus(status);
		fail.Format(IDS_MSG_FAIL_START, (const TCHAR *) status.result());
		MessageBox(fail);
		Stop();
		return;
	}
	// Add a blank line to the log window */
	Log(_T("\r\n"));
}

void UpgradeDialog::Stop(bool disable)
{
	// Kill our timer (so power saving modes can act normally)
	VERIFY(KillTimer(1));
	
	// Force update
	if (disable)
	{
		UpdateData(false);
	}

	// Destroy any active connection
	dfu.Destroy();
	dfuActive = false;

	// Update the percentage and window caption
	DoProgress(0, DFUEngine::Status());
	// Timer is gone so do this manually.
	RefreshTitle();

	// set ok button back to ok!
	GetDlgItem(IDC_OK_BUTTON)->SetWindowText(CString("OK"));
}

LRESULT UpgradeDialog::OnCallback(WPARAM wParam, LPARAM lParam)
{
	CallbackData *pmsg = (CallbackData *) lParam;

	switch(pmsg->type)
	{
	case started:
		DoStarted(pmsg->engine, pmsg->status);
		break;

	case progress:
		DoProgress(pmsg->engine, pmsg->status);
		break;

	case complete:
		DoComplete(pmsg->engine, pmsg->status);
		break;
	}

	delete pmsg;

	return 0;
}

UpgradeDialog::CallbackData::CallbackData(UpgradeDialog::CallbackType type, DFUEngine *engine, const DFUEngine::Status &status)
{
	this->type = type;
	this->engine = engine;
	this->status = status;
}

void UpgradeDialog::CallbackRequest(CallbackType type, DFUEngine *engine, const DFUEngine::Status &status)
{
	CallbackData *pmsg = new CallbackData(type, engine, status);
	SendMessage(UM_CALLBACK, 0, (LPARAM)pmsg);
}

// A DFU operation has started
void UpgradeDialog::Started(DFUEngine *engine, const DFUEngine::Status &status)
{
	CallbackRequest(started, engine, status);
}

// A DFU operation is progressing
void UpgradeDialog::Progress(DFUEngine *engine, const DFUEngine::Status &status)
{
	CallbackRequest(progress, engine, status);
}

// A DFU operation has completed
void UpgradeDialog::Complete(DFUEngine *engine, const DFUEngine::Status &status)
{
	CallbackRequest(complete, engine, status);
}

void UpgradeDialog::DoStarted(DFUEngine *engine, const DFUEngine::Status &status)
{
	// Reset the percentage and window caption
	DoProgress(engine, status);

	// Add a description to the log
	CString log;
	switch (status.state)
	{
	case DFUEngine::reconfigure:
		LogStart(IDS_LOG_ACTION_RECONFIGURE);
		break;

	case DFUEngine::upload:
		LogStart(IDS_LOG_ACTION_UPLOAD, dfuUploadFile);
		break;

	case DFUEngine::download:
		LogStart(IDS_LOG_ACTION_DOWNLOAD, dfuDownloadFile);
		break;

	case DFUEngine::manifest:
		LogStart(IDS_LOG_ACTION_MANIFEST);
		break;

	default:
		break;
	}

	// Start a timer
	timeStart = CTime::GetCurrentTime();
}

void UpgradeDialog::DoProgress(DFUEngine *engine, const DFUEngine::Status &status)
{
	// Set the progress bar
	progressUpgrade.SetPos(status.percent);

	// Choose the description
	UINT format;
	switch (status.state)
	{
	case DFUEngine::inactive:		format = IDS_OP_INACTIVE;		break;
	case DFUEngine::idle:			format = IDS_OP_IDLE;			break;
	case DFUEngine::reconfigure:	format = IDS_OP_RECONFIGURE;	break;
	case DFUEngine::upload:			format = IDS_OP_UPLOAD;			break;
	case DFUEngine::download:		format = IDS_OP_DOWNLOAD;		break;
	case DFUEngine::manifest:		format = IDS_OP_MANIFEST;		break;
	default:						format = IDS_OP_UNKNOWN;		break;
	}

	// Set the window caption
	textTitle.Format(format, status.percent);

	// Display the current activity
	LogTemporary(status.activity.GetOperation() ? _T("\r\n") + CString(status.activity()) : _T(""));

#ifdef ABORT_DOWNLOADS
	if ((status.state == DFUEngine::download) && (5 < status.percent))
	{
		Sleep((restartDelay * rand()) / RAND_MAX);
		Stop(false);
		Start();
	}
#endif
}

void UpgradeDialog::DoComplete(DFUEngine *engine, const DFUEngine::Status &status)
{
	// Stop the timer
	CTimeSpan taken(CTime::GetCurrentTime() - timeStart);

	// Update the percentage and window caption
	DoProgress(engine, status);

	// Check the result of the operation
	if (status.result)
	{
		// Process the end of the operation if successful
		CStringX comment;
		switch (status.state)
		{
		case DFUEngine::reconfigure:
			LogEnd(status, taken);
			break;

		case DFUEngine::upload:
			if (!DFUEngine::IsDFUFileValid(dfuUploadFile, comment)) comment.Empty();
			LogEnd(status, taken, CString(comment));
			break;

		case DFUEngine::download:
			if (!DFUEngine::IsDFUFileValid(dfuDownloadFile, comment)) comment.Empty();
			LogEnd(status, taken, CString(comment));
			break;

		case DFUEngine::manifest:
			LogEnd(status, taken);
			break;

		default:
			break;
		}
	}
	else
	{
		// Add an entry to the log
		LogEnd(status, taken);

		// Start the window flashing
		flashTitle = true;

		// Handle error results
		// Report the error to the user
		CString fail;
		fail.Format(IDS_MSG_FAIL, (const TCHAR *) status.result());
		switch (MessageBox(fail, MB_ABORTRETRYIGNORE | MB_ICONEXCLAMATION))
		{
		case IDABORT:
			LogComment(IDS_LOG_COMMENT_ABORT);
			Stop();
			return;

		case IDRETRY:
			ClearDisplay();
			LogComment(IDS_LOG_COMMENT_RETRY);
			Stop(false);
			Start();
			return;

		case IDIGNORE:
			LogComment(IDS_LOG_COMMENT_IGNORE);
			break;
		}
	}

	// Start the next operation (note deliberate fall-through)
	switch (status.state)
	{
	case DFUEngine::reconfigure:
		if (!dfuUploadFile.IsEmpty())
		{
			dfu.StartUpload(dfuUploadFile);
			break;
		}

	case DFUEngine::upload:
		if (!dfuDownloadFile.IsEmpty())
		{
			dfu.StartDownload(dfuDownloadFile);
			break;
		}

	case DFUEngine::download:
		dfu.StartManifest();
		break;

	case DFUEngine::manifest:
		flashTitle = true;
		LogComment(IDS_LOG_COMMENT_STOPPED);
		Stop();

		break;

	default:
		CString fail;
		fail.Format(IDS_MSG_STATE, status.state);
		MessageBox(fail, MB_OK | MB_ICONSTOP);
		EndDialog(IDCANCEL);
		break;
	}
}

// Update the log control
void UpgradeDialog::Log(CString text)
{
	int first_char = 0;
	int last_char = 0;
	Log(text, first_char, last_char);
}

void UpgradeDialog::Log(CString text, int& first_char, int& last_char)
{
	// Special case if no text being added or removed
	if (text.IsEmpty() && (first_char == last_char)) return;

	EditLogText.Delete(first_char, last_char-first_char);

	// Another special case if no text being added
	if (text.IsEmpty())
	{
		first_char = last_char = 0;
	}
	else
	{
		// Add the new text to the log
		EditLogText += text;
		first_char = 0;
		last_char = EditLogText.GetLength();
	}

	EditLog.SetWindowText(EditLogText);
}

void UpgradeDialog::LogComment(const CString &comment)
{
	CString log;
	log.Format(IDS_LOG_FORMAT_COMMENT, comment);
	Log(log);
}

void UpgradeDialog::LogComment(UINT comment)
{
	CString log;
	log.Format(comment);
	LogComment(log);
}

void UpgradeDialog::LogStart(UINT action, const CString &file)
{
	CString log;
	log.Format(action);
	Log(log);
	if (!file.IsEmpty())
	{
		log.Format(IDS_LOG_FORMAT_FILE, file);
		Log(log);
	}
}

void UpgradeDialog::LogEnd(const DFUEngine::Status &status, const CTimeSpan &time, const CString &comment)
{
	// Display a summary of the result
	CString log;
	log.Format(status.result ? IDS_LOG_FORMAT_OK : IDS_LOG_FORMAT_FAIL);
	Log(log);
	Log(_T("\r\n"));

	// Add the time taken and number of counts to the end
	Log(time.Format(IDS_LOG_FORMAT_TIME));
	Log(_T("\r\n"));

	// Display any error on a separate line
	if (!status.result)
	{
		log = status.result();
		log.Replace(_T(". "), _T(".\r\n"));
		log.Replace(_T(": "), _T(":\r\n"));
		log += _T("\r\n");
		Log(log);
	}

	// Display any comment on a separate line
	if (!comment.IsEmpty())
	{
		Log(comment + _T("\r\n"));
	}
}

void UpgradeDialog::LogTemporary(const CString &info)
{
	// Replace any existing temporary text
	Log(info, logTemporary_Min, logTemporary_Max);
}

void UpgradeDialog::RefreshTitle()
{
	// Update the window caption immediately before flashing
	CString oldTitle;
	GetWindowText(oldTitle);
	if (textTitle != oldTitle) SetWindowText(textTitle);
}

void UpgradeDialog::OnTimer(UINT nIDEvent) 
{
	// Do not flash the window if it has the focus
	if (flashTitle && (this == GetForegroundWindow())) flashTitle = false;

	// Reset the system idle timers so we don't switch off during operation!!
	SystemIdleTimerReset();
	// Pass on to the base class
	CDialog::OnTimer(nIDEvent);
}

void UpgradeDialog::OnSize(UINT nType, int cx, int cy) 
{
	// Pass on to the base class
	CDialog::OnSize(nType, cx, cy);
	
	// Only interested in some types of resize
	CSize size(cx, cy);
	switch (nType)
	{
	case SIZE_MAXIMIZED:
	case SIZE_RESTORED:
		// Update the size of the controls
		if (prevSize.cx) Resize(size - prevSize);
		break;

	default:
		// Ignore all other size changes
		break;
	}

	// Store the new size
	prevSize = size;
}

// Window resizing
void UpgradeDialog::Resize(const CSize &delta)
{
	CRect rect;
	CSize deltaHoriz(delta.cx, 0);
	CSize deltaVert(0, delta.cy);

	// Move the controls
	Resize(&progressUpgrade, deltaHoriz, false);
	Resize(&EditLog, delta, false);
	Resize(GetDlgItem(IDC_OK_BUTTON), delta, true);
}

void UpgradeDialog::Resize(CWnd *control, const CSize &delta, bool move)
{
	// Get the current position of the control
	CRect rect;
	control->GetWindowRect(&rect);
	ScreenToClient(&rect);

	// Calculate the new position
	if (move) rect += delta;
	else rect.BottomRight() += delta;

	// Reposition the control
	control->MoveWindow(&rect, true);
}

// Display of a message box
int UpgradeDialog::MessageBox(const CString &message, UINT type)
{
	// Improve the formatting of the message
	CString formatted(message);
	formatted.Replace(_T(". "), _T(".\r\n"));
	formatted.Replace(_T(": "), _T(".\r\n"));

	// Display the message box
	return AfxMessageBox(formatted, type /*| MB_TASKMODAL*/);
}

void UpgradeDialog::OnOK()
{
	// if the dfu is active...
	if (dfuActive)
	{
		// ... ask the user if they are really sure.
		CString query;
		query.Format(IDS_ABORT_QUERY);
		int reply = MessageBox(query, MB_YESNO | MB_ICONEXCLAMATION);
		// if they are then abort the operation
		if (reply == IDYES)
		{
			LogComment(IDS_LOG_COMMENT_ABORT);
			Stop();
		}
	}
	else
	{
		// otherwise just do the normal thing
		CDialog::OnOK();
	}
}


void UpgradeDialog::ClearDisplay()
{
	EditLog.SetWindowText(NULL);
}