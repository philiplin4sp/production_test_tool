// PowerTableV0Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "pstool.h"
#include "PowerTableV0Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PowerTableV0Dlg dialog


PowerTableV0Dlg::PowerTableV0Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(PowerTableV0Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(PowerTableV0Dlg)
	basic_internal_pa = 0;
	basic_tx_mix_offset = 0;
	basic_tx_mix_level = 0;
	basic_tx_pa_attn = 0;
	enhanced_internal_pa = 0;
	enhanced_tx_mix_level = 0;
	enhanced_tx_mix_offset = 0;
	enhanced_class = 0;
    no_br = 0;
	enhanced_tx_pa_attn = 0;
	output_power_dBm = 0;
	//}}AFX_DATA_INIT
	mode = None;
}


void PowerTableV0Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PowerTableV0Dlg)
	DDX_Control(pDX, IDOK, m_but_ok);
	DDX_Control(pDX, IDCANCEL, m_but_cancel);
	DDX_Text(pDX, IDC_BR_INTERNAL_PA, basic_internal_pa);
	DDV_MinMaxInt(pDX, basic_internal_pa, 0, 255);
	DDX_Text(pDX, IDC_BR_MIX_OFFSET, basic_tx_mix_offset);
	DDV_MinMaxInt(pDX, basic_tx_mix_offset, 0, 15);
	DDX_Text(pDX, IDC_BR_MIX_LEVEL, basic_tx_mix_level);
	DDV_MinMaxInt(pDX, basic_tx_mix_level, 0, 15);
	DDX_Text(pDX, IDC_EDR_CLASS, enhanced_class);
	DDV_MinMaxInt(pDX, enhanced_class, 0, 1);
	DDX_Text(pDX, IDC_NO_BR, no_br);
	DDV_MinMaxInt(pDX, no_br, 0, 1);
	DDX_Text(pDX, IDC_EDR_MIX_OFFSET, enhanced_tx_mix_offset);
	DDV_MinMaxInt(pDX, enhanced_tx_mix_offset, 0, 15);
	DDX_Text(pDX, IDC_EDR_MIX_LEVEL, enhanced_tx_mix_level);
	DDV_MinMaxInt(pDX, enhanced_tx_mix_level, 0, 15);
    DDX_Text(pDX, IDC_EDR_INTERNAL_PA, enhanced_internal_pa);
	DDV_MinMaxInt(pDX, enhanced_internal_pa, 0, 255);
	DDX_Text(pDX, IDC_EDR_TX_PA_ATTN, enhanced_tx_pa_attn);
	DDV_MinMaxInt(pDX, enhanced_tx_pa_attn, 0, 7);
	DDX_Text(pDX, IDC_OUTPUT_DBM, output_power_dBm);
	DDV_MinMaxInt(pDX, output_power_dBm, -128, 127);
	DDX_Text(pDX, IDC_BR_TX_PA_ATTN, basic_tx_pa_attn);
	DDV_MinMaxInt(pDX, basic_tx_pa_attn, 0, 7);
	//}}AFX_DATA_MAP
}

BOOL PowerTableV0Dlg::OnInitDialog()
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


BEGIN_MESSAGE_MAP(PowerTableV0Dlg, CDialog)
	//{{AFX_MSG_MAP(PowerTableV0Dlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PowerTableV0Dlg message handlers
