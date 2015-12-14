// EPTEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pstool.h"
#include "EPTEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// EPTEditDlg dialog


EPTEditDlg::EPTEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(EPTEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(EPTEditDlg)
	basic_internal_pa = 0;
	basic_external_pa = 0;
	basic_tx_pa_attn = 0;
	external_PA_control_bits = 0;
	enhanced_internal_pa = 0;
	enhanced_external_pa = 0;
	enhanced_class = 0;
    no_br = 0;
	enhanced_tx_pa_attn = 0;
	enhanced_tx_pre = 0;
	output_power_dBm = 0;
	//}}AFX_DATA_INIT
	mode = None;
}


void EPTEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(EPTEditDlg)
	DDX_Control(pDX, IDOK, m_but_ok);
	DDX_Control(pDX, IDCANCEL, m_but_cancel);
	DDX_Text(pDX, IDC_BR_EXTERNAL_PA, basic_external_pa);
	DDV_MinMaxInt(pDX, basic_external_pa, 0, 255);
	DDX_Text(pDX, IDC_BR_INTERNAL_PA, basic_internal_pa);
	DDV_MinMaxInt(pDX, basic_internal_pa, 0, 255);
	DDX_Text(pDX, IDC_EDR_CLASS, enhanced_class);
	DDV_MinMaxInt(pDX, enhanced_class, 0, 1);
	DDX_Text(pDX, IDC_EDR_NO_BR, no_br);
	DDV_MinMaxInt(pDX, no_br, 0, 1);
	DDX_Text(pDX, IDC_EDR_EXTERNAL_PA, enhanced_external_pa);
	DDV_MinMaxInt(pDX, enhanced_external_pa, 0, 255);
	DDX_Text(pDX, IDC_EDR_INTERNAL_PA, enhanced_internal_pa);
	DDV_MinMaxInt(pDX, enhanced_internal_pa, 0, 255);
	DDX_Text(pDX, IDC_EDR_TX_PA_ATTN, enhanced_tx_pa_attn);
	DDV_MinMaxInt(pDX, enhanced_tx_pa_attn, 0, 7);
	DDX_Text(pDX, IDC_EDR_TX_PRE, enhanced_tx_pre);
	DDV_MinMaxInt(pDX, enhanced_tx_pre, 0, 255);
	DDX_Text(pDX, IDC_OUTPUT_DBM, output_power_dBm);
	DDV_MinMaxInt(pDX, output_power_dBm, -128, 127);
	DDX_Text(pDX, IDC_PA_CTRL_BITS, external_PA_control_bits);
	DDV_MinMaxInt(pDX, external_PA_control_bits, 0, 1);
	DDX_Text(pDX, IDC_BR_TX_PA_ATTN, basic_tx_pa_attn);
	DDV_MinMaxInt(pDX, basic_tx_pa_attn, 0, 7);
	//}}AFX_DATA_MAP
}

BOOL EPTEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	switch(mode)
	{
	case Insert:
		SetWindowText(_T("Insert Power Table Value"));
		break;
	case Edit:
		SetWindowText(_T("Edit Power Table Value"));
		break;
	}

	return TRUE;
}


BEGIN_MESSAGE_MAP(EPTEditDlg, CDialog)
	//{{AFX_MSG_MAP(EPTEditDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

