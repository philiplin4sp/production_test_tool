#if !defined(AFX_PSEDITDLG_H__94177FCC_BECD_11D4_9001_00B0D065A91C__INCLUDED_)
#define AFX_PSEDITDLG_H__94177FCC_BECD_11D4_9001_00B0D065A91C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PSEditDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPSEditDlg dialog

class CPSEditDlg : public CDialog
{
// Construction
public:
	CPSEditDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPSEditDlg)
	enum { IDD = IDD_PS_INSERT };
	CButton	m_but_ok;
	CButton	m_but_cancel;
	int		m_butval_dbm;
	int		m_butval_external_pa;
	int		m_butval_internal_pa;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPSEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPSEditDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PSEDITDLG_H__94177FCC_BECD_11D4_9001_00B0D065A91C__INCLUDED_)
