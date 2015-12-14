/*
MODIFICATION HISTORY
		1.1   04:dec:00  Chris Lowe 	Working
		1.2   28:mar:01  Chris Lowe 	#inc's changed for Result dir

RCS IDENTIFIER
  $Id: //depot/bc/bluesuite_2_4/devHost/PersistentStore/PSTool/PSRawDlg.h#1 $

*/



#if !defined(AFX_PSRAWDLG_H__7807C7F6_C9D2_11D4_900C_00B0D065A91C__INCLUDED_)
#define AFX_PSRAWDLG_H__7807C7F6_C9D2_11D4_900C_00B0D065A91C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PSRawDlg.h : header file
//

#include "ps/psserver.h"
#include "ps/pshelp.h"

/////////////////////////////////////////////////////////////////////////////
// CPSRawDlg dialog

class CPSRawDlg : public CDialog
{
// Construction
public:
	CPSRawDlg(CWnd* pParent = NULL);   // standard constructor
	CPSHelper *m_helper;
	int m_startkeynum;

// Dialog Data
	//{{AFX_DATA(CPSRawDlg)
	enum { IDD = IDD_PSRAWDLG_DIALOG };
	CStatic	m_caption_present;
	CEdit	m_ed_str;
	CSpinButtonCtrl	m_spin_key;
	CString	m_ed_val_str;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPSRawDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_raw_len;
	bool m_changed;
	bool m_present;

	// Generated message map functions
	//{{AFX_MSG(CPSRawDlg)
	afx_msg void OnClose();
	afx_msg void OnRead();
	afx_msg void OnSet();
	afx_msg void OnRadDecKey();
	afx_msg void OnRadHexKey();
	virtual BOOL OnInitDialog();
	afx_msg void OnDelete();
	afx_msg void OnRadDecVal();
	afx_msg void OnRadHexVal();
	afx_msg void OnRadStrVal();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PSRAWDLG_H__7807C7F6_C9D2_11D4_900C_00B0D065A91C__INCLUDED_)
