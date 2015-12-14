///////////////////////////////////////////////////////////////////////
//
//	File	: PageResults.cpp
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: PageResults
//
//	Purpose	: DFU wizard page displayed at the end of the procedure
//			  to display the results. The following member variables
//			  should be set before this page is activated:
//				valueTitle		- The title for the page.
//				valueResults	- A summary of the results.
//				valueDetails	- Optional details of the results.
//				valueAnimate	- Should the success animation be
//								  displayed.
//				valueSaved		- Was the saved DFU file updated.
//				valueExit		- Should the wizard finish if
//								  successful.
//				valueExitOnError- Should the wizard finish if unsuccessful.
//
//	Modification history:
//	
//		1.1		20:Sep:00	at	File created.
//		1.2		09:Oct:00	at	Back button disabled on final page.
//								Details button added.
//		1.3		12:Oct:00	at	Improved formatting of details text.
//		1.4		24:Oct:00	at	Corrected debugging macros.
//		1.5		02:Oct:00	at	Added support for a success animation.
//		1.6		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.7		29:Nov:00	at	Back button enabled if not successful.
//		1.8		28:Feb:01	at	Back button not disabled in development
//								mode.
//		1.9		19:Mar:01	at	Window caption flashed when page
//								displayed without the focus.
//		1.10	17:May:01	at	Added automatic finish for success.
//		1.11	29:May:01	at	Added option to set description for
//								upload file.
//		1.12	14:Nov:01	at	Added basic Unicode support.
//		1.13	07:Jan:02	at	Added support for BC2 format DFU files.
//		1.14	11:Jan:02	at	Corrected handling of saved DFU file
//								comment if upgrade failed.
//		1.15	11:Jan:02	at	Comment not editable if upgrade failed.
//		1.16	13:Feb:02	at	Ask user if quit with changed comment.
//		1.17	14:Feb:02	at	Controls dynamically repositioned.
//		1.18	15:Feb:02	at	StaticEdit control used.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/PageResults.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUWizard.h"
#include "PageResults.h"

// Visual C++ debugging code
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// Vertical gap between repositioned controls
static const int vertical_gap = 5;

IMPLEMENT_DYNCREATE(PageResults, DFUPage)

// Constructor
PageResults::PageResults() : DFUPage(PageResults::IDD)
{
	//{{AFX_DATA_INIT(PageResults)
	valueTitle = _T("");
	//}}AFX_DATA_INIT

	valueDetails = _T("");

	// Window caption not flashing initially
	flashTimer = 0;

	// Comment not changed initially
	commentChanged = false;
}

// Data exchange
void PageResults::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PageResults)
	DDX_Control(pDX, IDC_PAGE_RESULTS_EDIT, editResults);
	DDX_Control(pDX, IDC_PAGE_RESULTS_CHANGE_BUTTON, buttonChange);
	DDX_Control(pDX, IDC_PAGE_RESULTS_COMMENT_EDIT, editComment);
	DDX_Control(pDX, IDC_PAGE_RESULTS_COMMENT_STATIC, staticComment);
	DDX_Control(pDX, IDC_PAGE_RESULTS_DETAILS_BUTTON, buttonDetails);
	DDX_Control(pDX, IDC_PAGE_RESULTS_ANIMATE, animateResults);
	DDX_Control(pDX, IDC_PAGE_RESULTS_TITLE, staticTitle);
	//}}AFX_DATA_MAP
}

// Message map
BEGIN_MESSAGE_MAP(PageResults, DFUPage)
	//{{AFX_MSG_MAP(PageResults)
	ON_BN_CLICKED(IDC_PAGE_RESULTS_DETAILS_BUTTON, OnDetailsClicked)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_PAGE_RESULTS_CHANGE_BUTTON, OnChangeClicked)
	ON_EN_CHANGE(IDC_PAGE_RESULTS_COMMENT_EDIT, OnCommentChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Button management
LRESULT PageResults::GetPageBack()
{
	return (valueAnimation && !GetSheet()->developmentMode) || commentChanged
	       ? disable : IDD_PAGE_SUMMARY;
}

// Initialise the page when displayed
BOOL PageResults::OnSetActive() 
{
	// Set the title text
	staticTitle.SetWindowText(valueTitle);

	// Start or hide the animation
	if (valueAnimation)
	{
		animateResults.ShowWindow(SW_SHOWNORMAL);
		animateResults.Open(IDR_AVI_MANIFEST);
	}
	else animateResults.ShowWindow(SW_HIDE);

	// Update the main results description
	CString results = valueResults;
	results.Replace(_T("\n"), _T("\r\n"));
	editResults.SetWindowText(results);

	// Show or hide the details button as appropriate
	buttonDetails.ShowWindow(valueDetails.IsEmpty() ? SW_HIDE : SW_SHOWNORMAL);

	// Show or hide the uploaded file comment as appropriate
	CStringX comment;
	if (valueSaved) DFUEngine::IsDFUFileValid(GetSheet()->pageFile.valueSaved, comment);
	staticComment.ShowWindow(!comment.IsEmpty());
	editComment.SetWindowText(comment);
	editComment.ShowWindow(!comment.IsEmpty());
	editComment.SetReadOnly(comment.IsEmpty());
	buttonChange.ShowWindow(!comment.IsEmpty());

	// Update the controls
	UpdateData(false);
	OnCommentChanged();

	// Determine the position of the movable controls
	long bottom;
	WINDOWPLACEMENT placement;
	if (comment.IsEmpty())
	{
		// No comment field so use its normal position
		buttonChange.GetWindowPlacement(&placement);
		bottom = placement.rcNormalPosition.bottom;
	}
	else
	{
		// Place above the comment field's label
		staticComment.GetWindowPlacement(&placement);
		bottom = placement.rcNormalPosition.top - vertical_gap;
	}

	// Reposition the details button if visible
	if (!valueDetails.IsEmpty())
	{
		buttonDetails.GetWindowPlacement(&placement);
		RECT rect = placement.rcNormalPosition;
		rect.top = bottom + rect.top - rect.bottom;
		rect.bottom = bottom;
		buttonDetails.MoveWindow(&rect);
		bottom = rect.top - vertical_gap;
	}

	// Resize the main results description to fill the remaining space
	editResults.GetWindowPlacement(&placement);
	placement.rcNormalPosition.bottom = bottom;
	editResults.MoveWindow(&placement.rcNormalPosition);
	editResults.AutoScrollBar(SB_VERT);

	// Start flashing the window caption
	flashTimer = SetTimer(1, 500, 0);
	
	// Perform any other initialisation required
	return DFUPage::OnSetActive();
}

// Delayed initialisation
void PageResults::OnSetActiveDelayed()
{
	// Perform any other initialisation required
	DFUPage::OnSetActiveDelayed();

	// Set the focus to either the Details or Finish button
	GotoDlgCtrl(valueDetails.IsEmpty()
	            ? GetSheet()->GetDlgItem(ID_WIZFINISH)
				: &buttonDetails);

	// Exit automatically if appropriate
	if ((valueAnimation || valueExitOnError) && valueExit) GetSheet()->EndDialog(ID_WIZFINISH);
}

// Exit control
bool PageResults::QueryClose(bool finish)
{
	// Query the user if comment changed but not saved
	if (commentChanged)
	{
		switch (AfxMessageBox(IDP_EXIT_COMMENT, MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON3))
		{
		case IDYES:
			OnChangeClicked();
			break;
		case IDNO:
			break;
		case IDCANCEL:
		default:
			return false;
		}
	}

	// Perform any default behaviour
	return DFUPage::QueryClose(finish);
}

// Tidy up when the page is hidden
BOOL PageResults::OnKillActive() 
{
	// Stop the animation
	animateResults.Close();

	// Stop flashing the window caption
	if (flashTimer) KillTimer(flashTimer);
	
	// Perform any other tidying up required
	return DFUPage::OnKillActive();
}

// Display details when button clicked
void PageResults::OnDetailsClicked() 
{
	// Format the details text across multiple lines
	CString details = valueDetails;
	details.Replace(_T(". "), _T(".\n"));
	int pos;
	while ((pos = details.Find(_T(": "))) != -1)
	{
		details.Delete(pos + 1);
		details.Insert(pos + 1, _T("\n\t"));
		pos = details.Find('\n', pos + 2);
		if ((pos != -1) && (details[pos + 1] != '\n'))
		{
			details.Insert(pos, '\n');
		}
	}

	// Display the details text in a message box
	AfxMessageBox(details, MB_OK | MB_ICONINFORMATION);
}

// Flash the window caption
void PageResults::OnTimer(UINT nIDEvent) 
{
	// Stop flashing the window if focus gained
	CWnd *parent = GetParent();
	if (parent == GetForegroundWindow())
	{
		// Restore the caption state
		parent->FlashWindow(false);

		// Stop the timer
		KillTimer(flashTimer);
		flashTimer = 0;
	}
	else
	{
		// Flash the window caption
		parent->FlashWindow(true);
	}

	// Pass on to the base class
	DFUPage::OnTimer(nIDEvent);
}

// Change the DFU file comment
void PageResults::OnChangeClicked() 
{
	// Read the new comment
	CString comment;
	editComment.GetWindowText(comment);

	// Change the file comment
	DFUEngine::SetDFUFileComment(GetSheet()->pageFile.valueSaved, comment);

	// Update the change button
	OnCommentChanged();
}

// Update the change button when the comment is edited
void PageResults::OnCommentChanged() 
{
	// No action unless a file was saved
	if (valueSaved)
	{
		// Read the old comment
		CStringX oldComment;
		DFUEngine::IsDFUFileValid(GetSheet()->pageFile.valueSaved, oldComment);

		// Read the new comment
		CString newComment;
		editComment.GetWindowText(newComment);
		newComment.TrimRight();

		// Enable the change button if comment changed
		commentChanged = !oldComment.IsEmpty() && !newComment.IsEmpty()
		                 && (newComment != oldComment);
		buttonChange.EnableWindow(commentChanged);

		// Update the wizard buttons
		SetButtons();
	}
}
