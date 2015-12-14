///////////////////////////////////////////////////////////////////////
//
//	File	: StaticEdit.cpp
//		  Copyright (C) 2002-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: StaticEdit
//
//	Purpose	: A class derived from CEdit to make Edit controls behave
//			  more like Static controls. The primary difference is
//			  that mouse clicks within the client are are ignored.
//
//			  A single member method is provided to dynamically
//			  enable or disable one or both scroll bars:
//				AutoScrollBar	- Enable scroll bar(s) if required.
//
//	Modification history:
//	
//		1.1		15:Feb:02	at	File created.
//		1.2		15:Feb:02	at	Improved emulation of static control.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/StaticEdit.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "StaticEdit.h"

// Visual C++ debugging code
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(StaticEdit, CEdit)

// Message map
BEGIN_MESSAGE_MAP(StaticEdit, CEdit)
	//{{AFX_MSG_MAP(StaticEdit)
	ON_WM_GETDLGCODE()
	ON_WM_SETFOCUS()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Automatically enable or disable a scroll bar
void StaticEdit::AutoScrollBar(int bar)
{
	// Try the vertical scroll bar first if using both
	if (bar == SB_BOTH)
	{
		EnableScrollBarCtrl(SB_HORZ, false);
		EnableScrollBarCtrl(SB_VERT, true);
		if (GetScrollLimit(SB_VERT) == 0)
		{
			EnableScrollBarCtrl(SB_VERT, false);
		}
	}

	// Try the horizontal scroll bar
	if ((bar == SB_HORZ) || (bar == SB_BOTH))
	{
		EnableScrollBarCtrl(SB_HORZ, true);
		if (GetScrollLimit(SB_HORZ) == 0)
		{
			EnableScrollBarCtrl(SB_HORZ, false);
		}
	}

	// Try the vertical scroll bar
	if ((bar == SB_VERT) || (bar == SB_BOTH))
	{
		EnableScrollBarCtrl(SB_VERT, true);
		if (GetScrollLimit(SB_VERT) == 0)
		{
			EnableScrollBarCtrl(SB_VERT, false);
		}
	}
}

// Behave like a static control
UINT StaticEdit::OnGetDlgCode() 
{
	return DLGC_STATIC;
}

// Do not display a caret
void StaticEdit::OnSetFocus(CWnd* pOldWnd) {}

// Use the parent window's cursor shape
BOOL StaticEdit::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	return 0;
}

// Discard mouse events within the client area
void StaticEdit::OnLButtonDown(UINT nFlags, CPoint point) {}
void StaticEdit::OnLButtonUp(UINT nFlags, CPoint point) {}
void StaticEdit::OnLButtonDblClk(UINT nFlags, CPoint point) {}
void StaticEdit::OnRButtonDown(UINT nFlags, CPoint point) {}
void StaticEdit::OnRButtonDblClk(UINT nFlags, CPoint point) {}
void StaticEdit::OnRButtonUp(UINT nFlags, CPoint point) {}
