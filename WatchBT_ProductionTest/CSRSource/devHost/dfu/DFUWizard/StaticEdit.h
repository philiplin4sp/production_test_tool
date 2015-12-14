///////////////////////////////////////////////////////////////////////
//
//	File	: StaticEdit.h
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
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/StaticEdit.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef STATICEDIT_H
#define STATICEDIT_H

#if _MSC_VER > 1000
#pragma once
#endif

// StaticEdit class
class StaticEdit : public CEdit
{
	DECLARE_DYNCREATE(StaticEdit)

public:

	//{{AFX_DATA(StaticEdit)
	//}}AFX_DATA

	//{{AFX_VIRTUAL(StaticEdit)
	//}}AFX_VIRTUAL

	// Automatically enable or disable a scroll bar
	void AutoScrollBar(int bar = SB_BOTH);

protected:

	//{{AFX_MSG(StaticEdit)
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif
