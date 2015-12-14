// PSEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PSTool.h"
#include "PSEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPSEditDlg dialog


CPSEditDlg::CPSEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPSEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPSEditDlg)
	m_butval_dbm = 0;
	m_butval_external_pa = 0;
	m_butval_internal_pa = 0;
	//}}AFX_DATA_INIT
}


void CPSEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPSEditDlg)
	DDX_Control(pDX, IDOK, m_but_ok);
	DDX_Control(pDX, IDCANCEL, m_but_cancel);
	DDX_Text(pDX, IDC_ED_PS_DBM, m_butval_dbm);
	DDV_MinMaxInt(pDX, m_butval_dbm, -128, 127);
	DDX_Text(pDX, IDC_ED_PS_EXTERNAL_PA, m_butval_external_pa);
	DDV_MinMaxInt(pDX, m_butval_external_pa, 0, 255);
	DDX_Text(pDX, IDC_ED_PS_INTERNAL_PA, m_butval_internal_pa);
	DDV_MinMaxInt(pDX, m_butval_internal_pa, 0, 255);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPSEditDlg, CDialog)
	//{{AFX_MSG_MAP(CPSEditDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPSEditDlg message handlers
