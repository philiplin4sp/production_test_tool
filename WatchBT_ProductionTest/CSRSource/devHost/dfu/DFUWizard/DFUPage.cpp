///////////////////////////////////////////////////////////////////////
//
//	File	: DFUPage.cpp
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFUPage
//
//	Purpose	: Class derived from CPropertyPage to add common
//			  functionality for all pages of the wizard.
//
//	Modification history:
//	
//		1.1		18:Sep:00	at	File created.
//		1.2		24:Oct:00	at	Corrected debugging macros.
//		1.3		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.4		17:May:01	at	Added automatic navigation.
//		1.5		25:May:01	at	Added intelligent string matching.
//		1.6		14:Nov:01	at	Added basic Unicode support.
//		1.7		13:Feb:02	at	Added QueryClose method.
//		1.8		13:Feb:02	at	Automatic scroll bar support.
//		1.9		14:Feb:02	at	Improved scroll bar support.
//		1.10	15:Feb:02	at	Scroll bar support moved to StaticEdit.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/DFUPage.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUWizard.h"
#include "DFUPage.h"

// Visual C++ debugging code
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// User messages
#define UM_ACTIVEDELAYED (WM_USER + 1)

// Details of the title font
static const int fontTitleSize = 8;
static const TCHAR fontTitleFace[] = _T("MS Sans Serif");

IMPLEMENT_DYNCREATE(DFUPage, CPropertyPage)

// Constructors
DFUPage::DFUPage()
{
}

DFUPage::DFUPage(UINT id) : CPropertyPage(id)
{
	//{{AFX_DATA_INIT(DFUPage)
	//}}AFX_DATA_INIT
}

// Data exchange
void DFUPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DFUPage)
	//}}AFX_DATA_MAP
}

// Message map
BEGIN_MESSAGE_MAP(DFUPage, CPropertyPage)
	//{{AFX_MSG_MAP(DFUPage)
	ON_MESSAGE(UM_ACTIVEDELAYED, OnSetActiveDelayedMsg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Initialise the page when displayed
BOOL DFUPage::OnSetActive() 
{
	// Set the title font
	CFont fontTitle;
	fontTitle.CreatePointFont(fontTitleSize, fontTitleFace);
	staticTitle.SetFont(&fontTitle);

	// Set the buttons initially
	SetButtons();

	// Allow delayed initialisation
	PostMessage(UM_ACTIVEDELAYED);
	
	// Perform any other initialisation
	return CPropertyPage::OnSetActive();
}

// Perform any delayed initialisation
LRESULT DFUPage::OnSetActiveDelayedMsg(WPARAM wParam, LPARAM lParam)
{
	// Ensure that the new page has been displayed
	UpdateWindow();

	// Perform whatever delayed initialisation is required
	OnSetActiveDelayed();
	return 0;
}

void DFUPage::OnSetActiveDelayed()
{
	// No delayed initialisation by default
}

// Obtain a pointer to the parent property sheet
DFUSheet *DFUPage::GetSheet() const
{
	return dynamic_cast<DFUSheet *>(GetParent());
}

// Button management
LRESULT DFUPage::GetPageBack()
{
	return GetSheet()->GetPageIndex(this) == 0
	       ? disable : automatic;
}

LRESULT DFUPage::GetPageNext()
{
	return GetSheet()->GetPageIndex(this)
		   == GetSheet()->GetPageCount() - 1
		   ? finish
		   : automatic;
}

void DFUPage::SetButtons()
{
	DWORD buttons = 0;
	BOOL finished = false;
	BOOL disabled = true;

	// Validate the controls
	UpdateData();

	// Choose the buttons to display
	switch (GetPageBack())
	{
	case disable:
		break;

	default:
		buttons |= PSWIZB_BACK;
		disabled = false;
		break;
	}
	switch (GetPageNext())
	{
	case disable:
		break;

	case finish:
		buttons |= PSWIZB_FINISH;
		finished = true;
		disabled = false;
		break;

	case disablefinish:
		buttons |= PSWIZB_DISABLEDFINISH;
		break;

	default:
		buttons |= PSWIZB_NEXT;
		disabled = false;
		break;
	}

	// Set the wizard buttons
	GetSheet()->SetWizardButtons(buttons);
	GetSheet()->GetDlgItem(IDCANCEL)->EnableWindow(!(disabled || finished));
}

// Page navigation
void DFUPage::SetBack()
{
	// Only change page if enabled
	if (GetPageBack() != disable) GetSheet()->PressButton(PSBTN_BACK);
}

void DFUPage::SetNext()
{
	// Only change page if enabled
	if (GetPageNext() != disable) GetSheet()->PressButton(PSBTN_NEXT);
}

LRESULT DFUPage::OnWizardBack() 
{
	LRESULT page;

	// Disable any further automatic behaviour
	GetSheet()->automatic = false;

	// Move back to the appropriate page
	page = GetPageBack();
	switch (page)
	{
	case automatic:
		return CPropertyPage::OnWizardBack();

	case disable:
	case finish:
	case disablefinish:
		return disable;

	default:
		return page;
	}
}

LRESULT DFUPage::OnWizardNext() 
{
	LRESULT page;

	// Move forward to the appropriate page
	page = GetPageNext();
	switch (page)
	{
	case automatic:
		return CPropertyPage::OnWizardNext();

	case finish:
		if (OnWizardFinish()) EndDialog(ID_WIZFINISH);
		return disable;

	case disable:
	case disablefinish:
		return disable;

	default:
		return page;
	}
}

BOOL DFUPage::OnWizardFinish() 
{
	// Check if the user really wants to quit
	if (!QueryClose(true)) return false;
	
	// Perform any default behaviour
	return CPropertyPage::OnWizardFinish();
}

// Exit control
bool DFUPage::QueryClose(bool finish)
{
	// Default behaviour is to exit quietly on the last page
	if (finish) return true;

	// Otherwise query the user
	return AfxMessageBox(IDP_EXIT_QUERY, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2)
	       == IDYES;
}

BOOL DFUPage::OnQueryCancel() 
{
	LRESULT page = GetPageNext();

	// Check if the user really wants to quit
	if (!QueryClose(page == finish)) return false;

	// Convert a cancel on the finish page
	if (page == finish)
	{
		GetSheet()->EndDialog(ID_WIZFINISH);
		return false;
	}

	// Perform any default behaviour
	return CPropertyPage::OnQueryCancel();
}

// String matching
static CString FindUniqueSubstring(const CStringList &list, const CString &value, bool initial = false, bool exact = false)
{
	// Compare lower case strings for case insensitivity
	CString substring = value;
	substring.MakeLower();
	CString match;
	for (POSITION pos = list.GetHeadPosition(); pos;)
	{
		CString entry = list.GetNext(pos);
		entry.MakeLower();
		int found = entry.Find(substring);
		if ((initial ? found == 0 : 0 <= found)
			&& (!exact || (entry.GetLength() == substring.GetLength())))
		{
			if (!match.IsEmpty()) return _T("");
			match = entry;
		}
	}
	return match;
}

CString DFUPage::FindUnique(const CStringList &list, const CString &value, bool matchEmpty)
{
	// Find the best unique match that is acceptable
	CString found;
	if ((!value.IsEmpty() || matchEmpty)
		&& (found = FindUniqueSubstring(list, value, true, true)).IsEmpty()
		&& (found = FindUniqueSubstring(list, value, true)).IsEmpty()
		&& (found = FindUniqueSubstring(list, value)).IsEmpty())
	{
		// Try substrings if failed on full string
		CString remain = value;
		while (!remain.IsEmpty())
		{
			// Find the next substring to try
			int pos = remain.FindOneOf(_T("\\/() "));
			CString substring;
			if (0 <= pos)
			{
				substring = remain.Left(pos);
				remain.Delete(0, pos + 1);
			}
			else
			{
				substring = remain;
				remain.Empty();
			}

			// Check for a possible match
			CString substringFound = FindUniqueSubstring(list, substring);
			if (!substringFound.IsEmpty())
			{
				// Check for conflicting matches
				if (!found.IsEmpty() && (found != substringFound))
				{
					found.Empty();
					remain.Empty();
				}
				else
				{
					found = substringFound;
				}
			}
		}
	}

	// Return the result
	return found;
}
