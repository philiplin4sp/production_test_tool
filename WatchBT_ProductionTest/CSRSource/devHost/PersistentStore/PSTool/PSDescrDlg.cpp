// PSDescrDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PSTool.h"
#include "PSDescrDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int MAX_DESCRIPTION_LENGT = 10000;

// Position and dimensions of OK button and main dialog windows when
// no presentation key info is to be displayed
#define _BT_OK_Y 302
#define _DLG_WIDTH 486
#define _DLG_HEIGHT 360

/////////////////////////////////////////////////////////////////////////////
// CPSDescrDlg dialog


CPSDescrDlg::CPSDescrDlg(const pskey_entry *entry, CPSHelper *helper, CWnd* pParent /*=NULL*/)
	: CDialog(CPSDescrDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPSDescrDlg)
	m_descr = _T("");
	//}}AFX_DATA_INIT

	m_entry = entry;
	m_helper = helper;
}


void CPSDescrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPSDescrDlg)
	DDX_Control(pDX, IDOK, m_bt_ok);
	DDX_Control(pDX, IDC_STATIC_PRESREL, m_st_preskeys);
	DDX_Control(pDX, IDC_ED_PRESKEYS, m_ed_preskeys);
	DDX_Control(pDX, IDC_ED_DESCR, m_ed_descr);
	DDX_Text(pDX, IDC_ED_DESCR, m_descr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPSDescrDlg, CDialog)
	//{{AFX_MSG_MAP(CPSDescrDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Initialise the dialog with the information about the entry
// returns      TRUE
BOOL CPSDescrDlg::OnInitDialog(void)
{
	ASSERT(m_entry);

	CDialog::OnInitDialog();

	// Set the title bar text

	ichar s_pres_key[MAX_DESCRIPTION_LENGT];
	isprintf(s_pres_key, MAX_DESCRIPTION_LENGT, II("%d - %s - %s"), m_entry->key_number, m_entry->id, m_entry->label);

	SetWindowText(s_pres_key);

	// Fill the description text edit

	CString s;
	const ichar *p;
	const ichar *const *pline;
	ichar *q;
	q = s.GetBuffer(MAX_DESCRIPTION_LENGT);
	for (pline = m_entry->description; *pline; pline++)
	{
		for (p = *pline; *p; p++)
		{
			if (*p == II('\n'))
			{
				*q++ = II('\r');
				*q++ = II('\n');
			}
			else
			   *q++ = *p;
		}
	}
	*q = II('\0');
	s.ReleaseBuffer();

	m_descr = s;

	// Display presentation key information if applicable

	int num_keys;

    bool adjust_y_pos_ok = false;
	if (m_entry->base_key > 0)
	{
		const pskey_entry *base_entry = m_helper->GetKeyDetails(m_entry->base_key);

		// Is a presentation key, so display base key
		isprintf(s_pres_key, MAX_DESCRIPTION_LENGT, II("%d - %s - %s"), base_entry->key_number, base_entry->label, base_entry->id);

		m_st_preskeys.SetWindowText(II("Is a presentation key for the following base key:"));
		m_ed_preskeys.SetWindowText(s_pres_key);
	}
	else if ((num_keys = m_helper->HasPresentationKeys(m_entry->key_number)) > 0)
	{
		// We're assuming that there is no hierarchy
		// i.e. a presentation key cannot be a base key for another

		// List presentation keys
		const pskey_entry **entries = new const pskey_entry*[num_keys];

		m_helper->HasPresentationKeys(m_entry->key_number, entries, num_keys);

		s_pres_key[0] = II('\0');

		for(int i = 0; i < num_keys; i++)
		{
			isprintf(s_pres_key, MAX_DESCRIPTION_LENGT, II("%d - %s - %s\r\n"), entries[i]->key_number, entries[i]->id, entries[i]->label);
			m_ed_preskeys.SetSel(-1, -1);
			m_ed_preskeys.ReplaceSel(s_pres_key);
		}

		delete [] entries;

		m_st_preskeys.SetWindowText(II("Is a base key for the following presentation keys:"));
	}
	else
	{
		m_st_preskeys.ShowWindow(SW_HIDE);
		m_ed_preskeys.ShowWindow(SW_HIDE);

		SetWindowPos(0, 0, 0, _DLG_WIDTH, _DLG_HEIGHT, SWP_NOZORDER|SWP_NOMOVE);
        adjust_y_pos_ok = true;
	}

    // work out where the "ok" button needs to be drawn
    RECT rect;
    RECT pos_rect;
    RECT dlg_rect;
    m_bt_ok.GetClientRect(&rect);   // Get the size of the ok button
    m_bt_ok.GetWindowRect(&pos_rect); // Get its position
    GetWindowRect(&dlg_rect);

    // If not a presentation key then need to adjust height of ok button
    int ypos = adjust_y_pos_ok ? _BT_OK_Y : pos_rect.top - dlg_rect.top - rect.bottom;

    // Centralise ok button at appropriate height
    m_bt_ok.SetWindowPos(0, _DLG_WIDTH/2 - rect.right/2, ypos, 0, 0, SWP_NOZORDER|SWP_NOSIZE);

	UpdateData(FALSE);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPSDescrDlg message handlers
