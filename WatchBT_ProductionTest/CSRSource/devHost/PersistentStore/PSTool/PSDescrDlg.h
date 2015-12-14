#if !defined(AFX_PSDESCRDLG_H__94177FCD_BECD_11D4_9001_00B0D065A91C__INCLUDED_)
#define AFX_PSDESCRDLG_H__94177FCD_BECD_11D4_9001_00B0D065A91C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PSDescrDlg.h : header file
//
#include "ps/pshelp.h"

/////////////////////////////////////////////////////////////////////////////
// CPSDescrDlg dialog

class CPSDescrDlg : public CDialog
{
// Construction
public:
	CPSDescrDlg(const pskey_entry *entry, CPSHelper *helper, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPSDescrDlg)
	enum { IDD = IDD_PSDESCRDLG_DIALOG };
	CButton	m_bt_ok;
	CStatic	m_st_preskeys;
	CEdit	m_ed_preskeys;
	CEdit	m_ed_descr;
	CString	m_descr;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPSDescrDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	// Called when the dialog box is initialised
	BOOL OnInitDialog(void);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPSDescrDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	// The entry to display information about
	const pskey_entry *m_entry;
	// Helper object used for checking if there are any presentation keys for the entry
	CPSHelper *m_helper;
	// Defines whether presentation key info is shown
	bool m_show_pres_keys;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PSDESCRDLG_H__94177FCD_BECD_11D4_9001_00B0D065A91C__INCLUDED_)
