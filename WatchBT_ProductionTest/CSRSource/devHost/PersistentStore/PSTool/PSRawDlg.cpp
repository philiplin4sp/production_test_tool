// PSRawDlg.cpp : implementation file
//

/*
MODIFICATION HISTORY
		1.3    12:dec:00  Chris Lowe 	Fixed bug with trailing space.
		1.4    04:apr:01  Chris Lowe 	EditRaw now uses key.

RCS IDENTIFIER
  $Id: //depot/bc/bluesuite_2_4/devHost/PersistentStore/PSTool/PSRawDlg.cpp#1 $

*/



#include "unicode/ichar.h"
#include "stdafx.h"
#include "PSTool.h"
#include "PSRawDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPSRawDlg dialog


CPSRawDlg::CPSRawDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPSRawDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPSRawDlg)
	m_ed_val_str = _T("");
	//}}AFX_DATA_INIT
	m_startkeynum = -1;
}


void CPSRawDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPSRawDlg)
	DDX_Control(pDX, IDC_CAPTION_PRESENT, m_caption_present);
	DDX_Control(pDX, IDC_ED_VAL, m_ed_str);
	DDX_Control(pDX, IDC_SPIN_KEY, m_spin_key);
	DDX_Text(pDX, IDC_ED_VAL, m_ed_val_str);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPSRawDlg, CDialog)
	//{{AFX_MSG_MAP(CPSRawDlg)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	ON_BN_CLICKED(IDC_READ, OnRead)
	ON_BN_CLICKED(IDC_SET, OnSet)
	ON_BN_CLICKED(IDC_RAD_DEC_KEY, OnRadDecKey)
	ON_BN_CLICKED(IDC_RAD_HEX_KEY, OnRadHexKey)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_RAD_STR_VAL, OnRadStrVal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPSRawDlg message handlers

void CPSRawDlg::OnClose() 
{
	CDialog::OnCancel();	
}

void CPSRawDlg::OnRead() 
{
	uint16 data[256];
	uint16 len;
	uint16 key_num;
	key_num = m_spin_key.GetPos();
	if (m_helper->psbc_get_pres_or_raw(key_num, data, 256, &len))
	{
		ichar str[1024];
		int i;
		str[0] =II('\0');
		ASSERT(len < 100);
		for (i = 0; i < len; i++)
		{
			isprintf(str + istrlen(str), 1024 - istrlen(str), II("%04x") , data[i]);
			if (i < len - 1)
	  		    isprintf(str + istrlen(str), 1024 - istrlen(str), II(" "));

		}
		m_ed_str.SetWindowText(str);
		m_changed = false;
		m_raw_len = len;
		m_present = true;
	}
	else
	{
		m_present = false;
		m_ed_str.SetWindowText(II("0000"));

	}
	m_caption_present.ShowWindow(!m_present);
		
}		

void CPSRawDlg::OnSet() 
{
	CString str;
    int usedLen;
    uint16 data[MAX_HEX_DATA];
	uint16 key_num = m_spin_key.GetPos();
	m_ed_str.GetWindowText(str);
    if (GetHexData<uint16>(str, data, MAX_HEX_DATA, &usedLen, 4))
    {
	    if ((m_raw_len != -1) && (usedLen != m_raw_len))
	    {
		    if (MessageBox(II("The length of the data you have entered does not match the length previously read.  Proceed with write?"), II("Mismatch in entry length"), MB_YESNO | MB_ICONWARNING) == IDNO)
			    return;
	    }
	    if (m_helper->psbc_set_pres_or_raw_if_different(key_num, data, usedLen))
        {
            OnRead();
        }
        else
        {
            MessageBox(II("Set failed"));
        }
    }
    else
    {
        MessageBox(II("The value is not valid. Enter a hexadecimal string"));
    }
	m_changed = false;

}

void CPSRawDlg::OnRadDecKey() 
{
    m_spin_key.SetPos32(m_spin_key.GetPos32());     // enforce updating of the buddy control
	m_spin_key.SetBase(10);	
}

void CPSRawDlg::OnRadHexKey() 
{
    m_spin_key.SetPos32(m_spin_key.GetPos32());     // enforce updating of the buddy control
	m_spin_key.SetBase(16);	
}

BOOL CPSRawDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_spin_key.SetBase(10);
	m_spin_key.SetRange32(0, 0xFFFF);
	((CButton *)GetDlgItem(IDC_RAD_DEC))->SetCheck(1);
	if (m_startkeynum != -1)
	{
		m_spin_key.SetPos(m_startkeynum);
		OnRead();
	}
	else
		m_spin_key.SetPos(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CPSRawDlg::OnDelete() 
{
	if (MessageBox(II("Are you sure you want to clear this key?"), II("Confirm delete key"), MB_ICONWARNING | MB_YESNO) == IDYES)
	{
		uint16 key_num = m_spin_key.GetPos();
		if (!m_helper->psbc_clr(key_num))
        {
            MessageBox(II("Delete failed"));
        }
	}
}

void CPSRawDlg::OnRadDecVal() 
{
	
}

void CPSRawDlg::OnRadHexVal() 
{
	
}

void CPSRawDlg::OnRadStrVal() 
{
	
}
