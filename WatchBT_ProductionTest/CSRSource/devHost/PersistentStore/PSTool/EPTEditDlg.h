#if !defined(AFX_EPTEDITDLG_H__456D8C0F_80B6_4EDC_8982_2C576EDE4616__INCLUDED_)
#define AFX_EPTEDITDLG_H__456D8C0F_80B6_4EDC_8982_2C576EDE4616__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EPTEditDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// EPTEditDlg dialog

class EPTEditDlg : public CDialog
{
// Construction
public:
	EPTEditDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(EPTEditDlg)
	enum { IDD = IDD_EPS_INSERT };
	CButton	m_but_ok;
	CButton	m_but_cancel;
	int		basic_external_pa;
	int		basic_internal_pa;
	int		enhanced_class;
    int     no_br;
	int		enhanced_external_pa;
	int		enhanced_internal_pa;
	int		enhanced_tx_pa_attn;
	int		enhanced_tx_pre;
	int		output_power_dBm;
	int		external_PA_control_bits;
	int		basic_tx_pa_attn;
	//}}AFX_DATA

	enum {
		None,
		Insert,
		Edit
	} mode;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(EPTEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();

	// Generated message map functions
	//{{AFX_MSG(EPTEditDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EPTEDITDLG_H__456D8C0F_80B6_4EDC_8982_2C576EDE4616__INCLUDED_)
