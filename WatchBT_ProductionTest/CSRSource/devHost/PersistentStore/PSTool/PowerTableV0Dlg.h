#if !defined(AFX_PowerTableV0Dlg_H__456D8C0F_80B6_4EDC_8982_2C576EDE4616__INCLUDED_)
#define AFX_PowerTableV0Dlg_H__456D8C0F_80B6_4EDC_8982_2C576EDE4616__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PowerTableV0Dlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PowerTableV0Dlg dialog

class PowerTableV0Dlg : public CDialog
{
// Construction
public:
	PowerTableV0Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(PowerTableV0Dlg)
	enum { IDD = IDD_POWER_TABLE_V0_INSERT };
	CButton	m_but_ok;
	CButton	m_but_cancel;
	int		basic_internal_pa;
	int		basic_tx_mix_level;
	int		basic_tx_mix_offset;
	int		basic_tx_pa_attn;
	int		enhanced_class;
	int		no_br;
	int		enhanced_tx_mix_level;
	int		enhanced_tx_mix_offset;
	int		enhanced_internal_pa;
	int		enhanced_tx_pa_attn;
	int		output_power_dBm;
	//}}AFX_DATA

	enum {
		None,
		Insert,
		Edit
	} mode;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PowerTableV0Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();

	// Generated message map functions
	//{{AFX_MSG(PowerTableV0Dlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PowerTableV0Dlg_H__456D8C0F_80B6_4EDC_8982_2C576EDE4616__INCLUDED_)
