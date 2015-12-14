// BtCliCtrlDlg.h : header file
//

/* 

  $Log:$

*/


#if !defined(AFX_BTCLICTRLDLG_H__1673DE70_38BA_11D5_90DC_0010B5BCCAFF__INCLUDED_)
#define AFX_BTCLICTRLDLG_H__1673DE70_38BA_11D5_90DC_0010B5BCCAFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CBtCliCtrlDlg dialog

class CBtCliCtrlDlg : public CDialog
{
// Construction
public:
	CBtCliCtrlDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CBtCliCtrlDlg)
	enum { IDD = IDD_BTCLICTRL_DIALOG };
	CComboBox	m_Transport;
	CComboBox	m_Comport;
	CComboBox	m_Baudrate;
	BOOL	m_Times;
	BOOL	m_Log;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBtCliCtrlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

    void SetOkButtonState(BOOL state);
    void SetDropDownSelection();
    void EnableBaudRate();
    void DisableBaudRate();

	// Generated message map functions
	//{{AFX_MSG(CBtCliCtrlDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnSelchangeComboTransport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BTCLICTRLDLG_H__1673DE70_38BA_11D5_90DC_0010B5BCCAFF__INCLUDED_)
