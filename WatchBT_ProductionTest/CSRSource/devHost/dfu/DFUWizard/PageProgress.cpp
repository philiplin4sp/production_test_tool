///////////////////////////////////////////////////////////////////////
//
//	File	: PageProgress.cpp
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: PageProgress
//
//	Purpose	: DFU wizard page displaying the status of the upgrade
//			  procedure. This uses the settings from the previous
//			  wizard pages to control the operation performed. When
//			  the procedure is complete, the IDD_PAGE_RESULTS page
//			  will be selected automatically. The following member
//			  variables should be set before this page is activated:
//				valueNoAbort	- Disable the abort button.
//
//	Modification history:
//	
//		1.1		20:Sep:00	at	File created.
//		1.2		28:Sep:00	at	Simplified sequencing code and
//								improved generation of failure
//								messages. Added support for HCI
//								tunnelling.
//		1.3		09:Oct:00	at	Added support for details button on
//								results page.
//		1.4		19:Oct:00	at	Updated relative timings, and prevented
//								incorrect upload.
//		1.5		24:Oct:00	at	Corrected debugging macros. Improved
//								behaviour if firmware corrupt and
//								upload attempted.
//		1.6		25:Oct:00	at	Added display of total time taken if
//								successful.
//		1.7		31:Oct:00	at	Return codes from DFUEngine methods
//								checked and handled.
//		1.8		02:Oct:00	at	Set relative timings based on upgrade
//								via UART at 115200 baud. Different
//								animations used for each stage of the
//								upgrade.
//		1.9		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.10	08:Dec:00	at	Added control of BCSP link
//								establishment.
//		1.11	15:Dec:00	at	Added (but not used) comment control.
//		1.12	06:Mar:01	at	Comment control updated.
//		1.13	23:Mar:01	at	Added directory to DFUEngine include.
//		1.14	30:Apr:01	ckl	Added Windows CE support.
//		1.15	01:May:01	ckl	Backed out the previous change.
//		1.16	17:May:01	at	Updated the change history.
//		1.17	29:May:01	at	Time taken formatted as minutes and
//								seconds only.
//		1.18	08:Oct:01	at	Added /noabort command line option.
//		1.19	09:Oct:01	at	No upload check during reconfiguration.
//		1.20	11:Jan:02	at	Upload status supplied to results page.
//		1.21	12:Feb:02	at	Path name truncated if necessary.
//		1.22	08:Mar:02	at	Download file name added to MRU list.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/PageProgress.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUWizard.h"
#include "PageProgress.h"
#include "SHLWapi.h"

// Visual C++ debugging code
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// Relative timings for the different stages
static const int timeReconfigure = 8;
static const int timeUpload = 35;
static const int timeDownload = 51;
static const int timeManifest = 6;

#define UM_CALLBACK (WM_USER + 2)

IMPLEMENT_DYNCREATE(PageProgress, DFUPage)

// Constructor
PageProgress::PageProgress() : DFUPage(PageProgress::IDD)
{
	//{{AFX_DATA_INIT(PageProgress)
	//}}AFX_DATA_INIT
}

// Data exchange
void PageProgress::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PageProgress)
	DDX_Control(pDX, IDC_PAGE_PROGRESS_COMMENT_STATIC, staticComment);
	DDX_Control(pDX, IDC_PAGE_PROGRESS_ABORT_BUTTON, buttonAbort);
	DDX_Control(pDX, IDC_PAGE_PROGRESS_MAIN_STATIC, staticMain);
	DDX_Control(pDX, IDC_PAGE_PROGRESS_MAIN_PROGRESS, progressMain);
	DDX_Control(pDX, IDC_PAGE_PROGRESS_SUB_STATIC, staticSub);
	DDX_Control(pDX, IDC_PAGE_PROGRESS_SUB_PROGRESS, progressSub);
	DDX_Control(pDX, IDC_PAGE_PROGRESS_ANIMATE, animateProgress);
	DDX_Control(pDX, IDC_PAGE_PROGRESS_TITLE, staticTitle);
	//}}AFX_DATA_MAP
}

// Message map
BEGIN_MESSAGE_MAP(PageProgress, DFUPage)
	//{{AFX_MSG_MAP(PageProgress)
	ON_BN_CLICKED(IDC_PAGE_PROGRESS_ABORT_BUTTON, OnAbortClicked)
	ON_MESSAGE(UM_CALLBACK, OnCallback)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Initialise the page when displayed
BOOL PageProgress::OnSetActive() 
{
	// Enable the abort button if appropriate
	buttonAbort.EnableWindow(!valueNoAbort);
	valueNoAbort = false;

	// Initially no successful upload
	GetSheet()->pageResults.valueSaved = false;

	// Start an animation
	animateProgress.Open(IDR_AVI_RECONFIGURE);

	// Reset the timers
	timeTakenUpload.Empty();
	timeTakenDownload.Empty();
	timeTakenTotal.Empty();
	timeTotal = CTimeSpan(0, 0, 0, 0);

	// Choose the filenames
	switch (GetSheet()->pageAction.valueAction)
	{
	case PageAction::upload_download:
		fileUpload = GetSheet()->pageFile.valueSaved;
		fileDownload = GetSheet()->pageFile.valueName;
		break;

	case PageAction::download:
		fileUpload.Empty();
		fileDownload = GetSheet()->pageFile.valueName;
		break;

	case PageAction::restore:
		fileUpload.Empty();
		fileDownload = GetSheet()->pageFile.valueSaved;
		break;
	}

	// Add the download filename to the list of recently used files
	CStringList &listNames = GetSheet()->pageFile.listNames;
	POSITION pos = listNames.Find(fileDownload);
	if (pos) listNames.RemoveAt(pos);
	listNames.AddHead(fileDownload);

	// Configure the DFU engine
	DFUSheet *sheet = GetSheet();
	switch (sheet->pageConnection.valueConnection)
	{
	case PageConnection::usb:
		dfu.CreateUSB(sheet->pageUSB.valueUSBNum);
		break;

	case PageConnection::com:
		if (sheet->pageCOM.valueCOMAuto)
		{
			dfu.CreateCOM(sheet->pageCOM.valueCOMPort);
		}
		else
		{
			switch (sheet->pageCOM.valueProtocol)
			{
			case PageCOM::bcsp:
				dfu.CreateCOMBCSP(sheet->pageCOM.valueCOMPort,
				                  sheet->pageCOM.valueBaud,
				                  sheet->pageCOM.valueTunnel != 0,
								  DFUEngine::Link(sheet->pageCOM.valueLink));
				break;

			case PageCOM::h4:
				dfu.CreateCOMH4(sheet->pageCOM.valueCOMPort,
				                sheet->pageCOM.valueBaud);
				break;
			}
		}
		break;

	case PageConnection::simulated:
		dfu.CreateSimulated();
		break;
	}
	dfu.SetCallback(this);
	
	// Perform any other initialisation required
	return DFUPage::OnSetActive();
}

// Tidy up when the page is hidden
BOOL PageProgress::OnKillActive() 
{
	// Display the hourglass
	CWaitCursor wait;

	// Terminate the DFU engine
	dfu.SetCallback();
	dfu.Destroy();

	// Stop the animation
	animateProgress.Close();
	
	// Perform any other tidying up required
	return DFUPage::OnKillActive();
}

// Delayed initialisation
void PageProgress::OnSetActiveDelayed()
{
	// Perform any other initialisation required
	DFUPage::OnSetActiveDelayed();

	// Start the procedure
	if (!dfu.StartReconfigure(!fileUpload.IsEmpty() && !GetSheet()->developmentMode,
	                          !fileDownload.IsEmpty(),
			                  fileDownload))
	{
		// Display the results page
		GetSheet()->pageResults.valueTitle.Format(IDS_RESULTS_TITLE_FAIL);
		CString &results = GetSheet()->pageResults.valueResults;
		results.Format(IDS_RESULTS_MSG_FAIL_INTERNAL);
		CString recover;
		recover.Format(IDS_RESULTS_MSG_RECOVER_FAIL);
		if (!recover.IsEmpty()) results += "\n\n" + recover;
		GetSheet()->pageResults.valueAnimation = false;
		GetSheet()->pageResults.valueDetails.Empty();
		GetSheet()->SetActivePage(&GetSheet()->pageResults);
	}
}

// Button management
LRESULT PageProgress::GetPageBack()
{
	return disable;
}

LRESULT PageProgress::GetPageNext()
{
	return disable;
}

// Handle clicks on the Abort button
void PageProgress::OnAbortClicked() 
{
	// Check if the user really wants to abort
	int result = AfxMessageBox(IDP_ABORT_PROGRESS_QUERY, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);

	// Ignore result if operation completed
	if (GetSheet()->GetActivePage() != this) return;

	// Abort the operation if requested
	if (result == IDYES)
	{
		// Disable the abort button
		buttonAbort.EnableWindow(false);

		// Set the title of the results page
		GetSheet()->pageResults.valueTitle.Format(IDS_RESULTS_TITLE_ABORT);

		// Add a description of the operations performed
		CString &results = GetSheet()->pageResults.valueResults;
		DFUEngine::Status status;
		dfu.GetStatus(status);
		switch (status.state)
		{
		case DFUEngine::reconfigure:
			results.Format(IDS_RESULTS_MSG_FAIL_RECONFIGURE);
			break;

		case DFUEngine::upload:
			results.Format(IDS_RESULTS_MSG_FAIL_UPLOAD);
			break;

		case DFUEngine::download:
			results.Format(IDS_RESULTS_MSG_FAIL_DOWNLOAD);
			break;

		case DFUEngine::manifest:
			results.Format(IDS_RESULTS_MSG_FAIL_MANIFEST);
			break;

		default:
			results.Format(IDS_RESULTS_MSG_FAIL_UNKNOWN);
			break;
		}

		// Add the recovery information
		CString recover;
		recover.Format(IDS_RESULTS_MSG_RECOVER_ABORT);
		if (!recover.IsEmpty()) results += "\n\n" + recover;

		// No details available
		GetSheet()->pageResults.valueAnimation = false;
		GetSheet()->pageResults.valueDetails.Format(IDS_RESULTS_MSG_ABORT);

		// Display the results page
		GetSheet()->SetActivePage(&GetSheet()->pageResults);
	}
}

// Callback functions
void PageProgress::Started(DFUEngine *engine, const DFUEngine::Status &status)
{
	CallbackRequest(started, engine, status);
}

void PageProgress::Progress(DFUEngine *engine, const DFUEngine::Status &status)
{
	// Suppress progress callbacks if other callbacks pending
	lockListData.Lock();
	bool empty = listData.IsEmpty() != 0;
	lockListData.Unlock();
	if (empty) CallbackRequest(progress, engine, status);
}

void PageProgress::Complete(DFUEngine *engine, const DFUEngine::Status &status)
{
	CallbackRequest(complete, engine, status);
}

PageProgress::CallbackData::CallbackData(CallbackType type, DFUEngine *engine,
                          const DFUEngine::Status &status)
{
	this->type = type;
	this->engine = engine;
	this->status = status;
}

// Callback implementation
void PageProgress::CallbackRequest(CallbackType type,
                                   DFUEngine *engine,
                                   const DFUEngine::Status &status)
{
	// Construct an object with the callback details
	lockListData.Lock();
	{
		SmartPtr<CallbackData> data(new CallbackData(type, engine, status));
		listData.AddTail(data);
	}
	lockListData.Unlock();

	// Request a callback
	PostMessage(UM_CALLBACK, 0, 0);
}

int PageProgress::ProgressTime(DFUEngine::State state)
{
	int time = 0;
	switch (state)
	{
	case DFUEngine::manifest:
		time += timeManifest;

	case DFUEngine::download:
		time += timeDownload;

	case DFUEngine::upload:
		if (GetSheet()->pageAction.valueAction == PageAction::upload_download)
		{
			time += timeUpload;
		}

	case DFUEngine::reconfigure:
		time += timeReconfigure;
		break;
	}
	return time;
}

void PageProgress::CompleteSuccess(const DFUEngine::Status &status)
{
	bool ok = true;

	// Start the next operation
	CTimeSpan timeSpan = timeEnd - timeStart;
	switch (status.state)
	{
	case DFUEngine::reconfigure:
		if (fileUpload.IsEmpty()) ok = dfu.StartDownload(fileDownload);
		else ok = dfu.StartUpload(fileUpload);
		break;

	case DFUEngine::upload:
		if (status.result) GetSheet()->pageResults.valueSaved = true;
		timeTakenUpload.Format(IDS_RESULTS_MSG_TIME, (int)timeSpan.GetTotalMinutes(), (int)timeSpan.GetSeconds());
		ok = dfu.StartDownload(fileDownload);
		break;

	case DFUEngine::download:
		timeTakenDownload.Format(IDS_RESULTS_MSG_TIME, (int)timeSpan.GetTotalMinutes(), (int)timeSpan.GetSeconds());
		ok = dfu.StartManifest();
		break;

	default:
		// Otherwise successfully completed
		CompleteSuccessLast(status);
		break;
	}

	// Handle failure to start the next operation
	if (!ok)
	{
		// Display the results page
		GetSheet()->pageResults.valueTitle.Format(IDS_RESULTS_TITLE_FAIL);
		CString &results = GetSheet()->pageResults.valueResults;
		results.Format(IDS_RESULTS_MSG_FAIL_INTERNAL);
		CString recover;
		recover.Format(IDS_RESULTS_MSG_RECOVER_FAIL);
		if (!recover.IsEmpty()) results += "\n\n" + recover;
		GetSheet()->pageResults.valueAnimation = false;
		GetSheet()->pageResults.valueDetails.Empty();
		GetSheet()->SetActivePage(&GetSheet()->pageResults);
	}
}

void PageProgress::CompleteSuccessLast(const DFUEngine::Status &status)
{
	// Display the results page
	GetSheet()->pageResults.valueTitle.Format(IDS_RESULTS_TITLE_SUCCESS);
	CString &results = GetSheet()->pageResults.valueResults;
	switch (GetSheet()->pageAction.valueAction)
	{
	case PageAction::upload_download:
		results.Format(IDS_RESULTS_MSG_SUCCESS_UPLOAD_DOWNLOAD,
		               fileDownload, timeTakenUpload,
					   timeTakenDownload, timeTakenTotal);
		break;

	case PageAction::download:
		results.Format(IDS_RESULTS_MSG_SUCCESS_DOWNLOAD,
		               fileDownload, timeTakenDownload, timeTakenTotal);
		break;

	case PageAction::restore:
		results.Format(IDS_RESULTS_MSG_SUCCESS_RESTORE,
		               timeTakenDownload, timeTakenTotal);
		break;
	}
	GetSheet()->pageResults.valueAnimation = true;
	GetSheet()->pageResults.valueDetails.Empty();
	GetSheet()->SetActivePage(&GetSheet()->pageResults);
}

void PageProgress::CompleteFail(const DFUEngine::Status &status)
{
	// Construct a description of the problem
	GetSheet()->pageResults.valueTitle.Format(IDS_RESULTS_TITLE_FAIL);

	// Add a description of the operations performed
	CString &results = GetSheet()->pageResults.valueResults;
	switch (status.state)
	{
	case DFUEngine::reconfigure:
		results.Format(IDS_RESULTS_MSG_FAIL_RECONFIGURE);
		break;

	case DFUEngine::upload:
		results.Format(IDS_RESULTS_MSG_FAIL_UPLOAD);
		break;

	case DFUEngine::download:
		results.Format(IDS_RESULTS_MSG_FAIL_DOWNLOAD);
		break;

	case DFUEngine::manifest:
		results.Format(IDS_RESULTS_MSG_FAIL_MANIFEST);
		break;

	default:
		results.Format(IDS_RESULTS_MSG_FAIL_UNKNOWN);
		break;
	}

	// Add the recovery information
	CString recover;
	recover.Format(IDS_RESULTS_MSG_RECOVER_FAIL);
	if (!recover.IsEmpty()) results += "\n\n" + recover;

	// Set the details
	GetSheet()->pageResults.valueAnimation = false;
	GetSheet()->pageResults.valueDetails = status.result();

	// Display the results page
	GetSheet()->SetActivePage(&GetSheet()->pageResults);
}

// on callback message.
LRESULT PageProgress::OnCallback(WPARAM wParam, LPARAM lParam)
{
	// Process the next callback
	lockListData.Lock();
	SmartPtr<CallbackData> data;
	if (!listData.IsEmpty()) data = listData.RemoveHead();
	lockListData.Unlock();
	if (!data) return 0;

	// Call the callback function if valid
	switch (data->type)
	{
	case started:
		{
			// Record the start time
			timeStart = CTime::GetCurrentTime();

			// Set the progress bars
			Progress(data->engine, data->status);

			// Display a description of the operation
			CString description;
			switch (data->status.state)
			{
			case DFUEngine::reconfigure:
				description.Format(IDS_PROGRESS_OPERATION_RECONFIGURE);
				break;

			case DFUEngine::upload:
				animateProgress.Open(IDR_AVI_UPLOAD);
				description.Format(IDS_PROGRESS_OPERATION_UPLOAD);
				break;

			case DFUEngine::download:
				animateProgress.Open(IDR_AVI_DOWNLOAD);
				description.Format(IDS_PROGRESS_OPERATION_DOWNLOAD,
								   fileDownload);
				break;

			case DFUEngine::manifest:
				animateProgress.Open(IDR_AVI_MANIFEST);
				description.Format(IDS_PROGRESS_OPERATION_MANIFEST);
				break;
			}
			PathSetDlgItemPath(m_hWnd, IDC_PAGE_PROGRESS_SUB_STATIC, description);
		}
		break;
	case progress:
		{
			// Set the progress bar for the whole procedure
			int start = ProgressTime((DFUEngine::State) (data->status.state - 1));
			int end = ProgressTime(data->status.state);
			int total = ProgressTime(DFUEngine::manifest);
			progressMain.SetPos((start * 100 + (end - start) * data->status.percent) / total);

			// Set the progress bar for this operation
			progressSub.SetPos(data->status.percent);

			// Set the comment
			staticComment.SetWindowText(data->status.activity());
		}
		break;
	case complete:
		{
			// Record the end time
			timeEnd = CTime::GetCurrentTime();
			timeTotal += timeEnd - timeStart;
			timeTakenTotal.Format(IDS_RESULTS_MSG_TIME, (int)timeTotal.GetTotalMinutes(), (int)timeTotal.GetSeconds());

			// Special case if failed to upload due to corrupt firmware
			bool ignoreResult = false;
			if ((data->status.state == DFUEngine::upload)
				&& ((data->status.result.GetCode() == DFUEngine::fail_dfu_firmware)
					|| (data->status.result.GetCode() == DFUEngine::fail_no_upload)))
			{
				// Check if the user wants to skip the upload and download anyway
				int result = AfxMessageBox(data->status.result.GetCode() == DFUEngine::fail_dfu_firmware
										   ? IDP_SKIP_UPLOAD_CORRUPT_QUERY
										   : IDP_SKIP_UPLOAD_UNSUPPORTED_QUERY,
										   MB_YESNO | MB_ICONQUESTION);
				
				// Continue with the download if required
				if (result == IDYES)
				{
					GetSheet()->pageAction.valueAction = PageAction::download;
					fileUpload.Empty();
					ignoreResult = true;
				}
			}

			// Set the progress bars
			if (!ignoreResult) Progress(data->engine, data->status);

			// Action depends on whether the operation was successful
			if (data->status.result || ignoreResult) CompleteSuccess(data->status);
			else CompleteFail(data->status);
		}
		break;
	}

	return 0;
}

// Exit control
BOOL PageProgress::OnQueryCancel() 
{
	// Check if the user really wants to quit
	int result = AfxMessageBox(IDP_EXIT_PROGRESS_QUERY, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);

	// Ignore result if operation completed
	if (GetSheet()->GetActivePage() != this) return false;

	// Cancel the close if requested
	if (result == IDNO) return false;

	// Disable the abort button
	buttonAbort.EnableWindow(false);

	// Terminate the DFU engine
	dfu.SetCallback();
	dfu.Destroy();

	// Display a final prompt to the user
	AfxMessageBox(IDP_EXIT_PROGRESS_PROMPT);
	
	// Perform any default behaviour (skipping the DFUPage query)
	return CPropertyPage::OnQueryCancel();
}
