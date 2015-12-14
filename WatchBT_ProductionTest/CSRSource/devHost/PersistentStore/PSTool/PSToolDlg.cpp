// PSToolDlg.cpp : implementation file
//  Access keys are 1066 and 1812

/*
MODIFICATION HISTORY
		1.10   30:nov:00  Chris Lowe 	Changes to accept named struct and union in psbc_entry
		1.13   04:dec:00  Chris Lowe 	Hourglass, Raw edit.
		1.14   06:dec:00  Chris Lowe 	Fixed bug on uint32 set raw - size was wrong.
		1.17   12:dec:00  Chris Lowe 	Fixed bug with TIME, and with trailing space.
		1.18   13:dec:00  Chris Lowe 	Fixed another bug with TIME.
		1.19   13:dec:00  Chris Lowe 	Fixed bug with uint16 and large PSKEY_MAX
		1.22   16:jan:01  Chris Lowe 	Introduced PsBufSize
		1.25   08:feb:01  Chris Lowe 	removed int8
		1.27   16:feb:01  Chris Lowe 	Now re-reads after set and delete.
		1.27   23:feb:01  Chris Lowe 	Now supports firmware versions.
		1.32   13:mar:01  Chris Lowe 	USB support bugfixed
		1.33   28:mar:01  Chris Lowe 	#inc's changed for Result dir
		1.34   04:apr:01  Chris Lowe 	EditRaw now uses key.
		1.35   06:jun:01  Chris Lowe 	New stern message in OnAccessAll
		1.36   08:jun:01  Chris Lowe 	Tweaked sternness.
										Added signed string formatting
		1.37	 04:jul:01	Chris Lowe	Added version info.
		1.38	 06:jul:01	Chris Lowe	Default view set to Develper keys.
		1.39	 06:jul:01	Chris Lowe	Typo in length mismatch dlg
		#29    16:mar:04  Simon Walker  B-2052: Processes new structure type for UTF-8 encoding/decoding.
		#30    16:mar:04  Simon Walker  B-2275: Now ensures that data is NULL terminated. Also fixed careless min/max bug
		#31    18:mar:04  Simon Walker  B-671: Can now apply canned key operations.
        #32    23:mar:04  Simon Walker  B-802: User now has to press button to edit/add a key that is not present

RCS IDENTIFIER
  $Id: //depot/bc/bluesuite_2_4/devHost/PersistentStore/PSTool/PSToolDlg.cpp#1 $

*/

#include "stdafx.h"
#include "PSTool.h"
#include "PSToolDlg.h"
#include "ps/psserver.h"
#include "PSEditDlg.h"
#include "EPTEditDlg.h"
#include "PowerTableV0Dlg.h"
#include "PSDescrDlg.h"
#include "PSRawDlg.h"
#include "ChooseTransDlg.h"
#include "versioninfo\VersionInfo.h"
#include <stdlib.h>
#include <afxpriv.h>
#include <limits>

#include "ps/psbc_keys.h"	// for PSKEY_BD_ADDR

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static const char THIS_FILE[] = "PSToolDlg.cpp";
#endif

// Max number of uint16's the SetKey method allocates
#define _MAX_SETKEY_DATA 56

// Maximum number of canned operations allowed on menu
const int _MAX_CANNED_OPS = (ID_KEY_CANNED_MAX - ID_KEY_CANNED_MIN);
// Maximum size for a temporary character buffer
const int _MAX_BUF_SIZE = 1024;
// Position of the Entry submenu
const int _ENTRY_MENU_POS = 1;

#define WM_DIALOG_DISPLAYED WM_APP  // Message used to indicate dialog is being displayed for first time

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	VersionInfo<istring> versionInfo;
	SetDlgItemText(IDC_ABOUT_TITLE, versionInfo.GetProductName().c_str());
	CString build = versionInfo.GetFileVersion().c_str();
	build += CString(" ");
	build += versionInfo.GetSpecialBuild().c_str();
	SetDlgItemText(IDC_ABOUT_BUILD, build);
	SetDlgItemText(IDC_ABOUT_COPYRIGHT, versionInfo.GetLegalCopyright().c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}





/////////////////////////////////////////////////////////////////////////////
// CPSToolDlg dialog


BEGIN_EVENTSINK_MAP(CPSToolDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(CPSToolDlg)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


static const ichar *formatStrings[] = // indexed by edit_format_type enum.
{
	II(""),					// edit_format_none
	II("&decimal:"),			// edit_format_decimal
	II("h&ex:"),				// edit_format_hex
	II("h&ex: xxxx xxxx ..."),	// edit_format_hex_words
	II("te&xt:"),			// edit_format_text
	II("select from &list:"),	// edit_format_enum
	II("&Bluetooth Address:")	// edit_format_bd_addr
};

CPSHelper *CPSToolDlg::GetHelper()
{
	return m_helper;
}

void CPSToolDlg::SetHelper(CPSHelper *a_helper)
{
	m_helper = a_helper;
}


CPSToolDlg::CPSToolDlg(CWnd* pParent /*=NULL*/, CPSToolApp *app /* = NULL*/)
	: CDialog(CPSToolDlg::IDD, pParent),
	 m_app(app)
{
	//{{AFX_DATA_INIT(CPSToolDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_changed = false;
	m_entry = NULL;
	m_show_method = SHOW_FRIENDLY;
	m_sort_method = SORT_BY_NAME;
	m_show_only_overrides = false;
	m_show_only_set = false;
    m_initial_connect = false;
    m_first_dlg_display = true;
    
    m_error_handler = new bccmd_trans_error_handler;

}

CPSToolDlg::~CPSToolDlg()
{
    delete m_error_handler;
    m_error_handler = NULL;
}

void CPSToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPSToolDlg)
	DDX_Control(pDX, IDC_SET, m_but_set_key);
	DDX_Control(pDX, IDC_BUT_NOTPRESENT, m_but_notpresent);
	DDX_Control(pDX, IDC_BUT_DEL_KEY, m_but_del_key);
	DDX_Control(pDX, IDC_STATIC_UAP, m_static_uap);
	DDX_Control(pDX, IDC_STATIC_NAP, m_static_nap);
	DDX_Control(pDX, IDC_STATIC_LAP, m_static_lap);
	DDX_Control(pDX, IDC_ED_UAP, m_ed_uap);
	DDX_Control(pDX, IDC_ED_NAP, m_ed_nap);
	DDX_Control(pDX, IDC_ED_LAP, m_ed_lap);
	DDX_Control(pDX, IDC_ED_FILTER, m_ed_filter);
	DDX_Control(pDX, IDC_EDIT_FORMAT, m_st_edit_format);
	DDX_Control(pDX, IDC_BUT_PS_EDIT, m_but_ps_edit);
	DDX_Control(pDX, IDC_BUT_PS_INSERT, m_but_ps_insert);
	DDX_Control(pDX, IDC_BUT_PS_DELETE, m_but_ps_delete);
	DDX_Control(pDX, IDC_KEY_LABEL, m_key_label);
	DDX_Control(pDX, IDC_CAPTION_PRESENT, m_caption_present);
	DDX_Control(pDX, IDC_SPIN_VAL, m_spin_val);
	DDX_Control(pDX, IDC_LIST_KEYS, m_list_keys);
	DDX_Control(pDX, IDC_ED_VAL, m_ed_val);
	DDX_Control(pDX, IDC_ED_STR, m_ed_str);
	DDX_Control(pDX, IDC_COMBO_ENUM, m_combo_enum);
	DDX_Control(pDX, IDC_GRID_PS, m_grid_ps);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPSToolDlg, CDialog)
	//{{AFX_MSG_MAP(CPSToolDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_LBN_SELCHANGE(IDC_LIST_KEYS, OnSelchangeListKeys)
	ON_BN_CLICKED(IDC_SET, OnSet)
	ON_BN_CLICKED(IDC_READ, OnRead)
	ON_BN_CLICKED(IDC_CLOSE, OnButClose)
	ON_BN_CLICKED(IDC_RESETCLOSE, OnButResetClose)
	ON_BN_CLICKED(IDC_BUT_PS_INSERT, OnButPsInsert)
	ON_BN_CLICKED(IDC_BUT_PS_DELETE, OnButPsDelete)
	ON_BN_CLICKED(IDC_BUT_PS_EDIT, OnButPsEdit)
	ON_BN_CLICKED(IDC_DESCRIBE, OnDescribe)
	ON_BN_CLICKED(IDC_BUT_RESET, OnButReset)
	ON_CBN_SELCHANGE(IDC_COMBO_ENUM, OnSelchangeComboEnum)
	ON_EN_CHANGE(IDC_ED_STR, OnChangeEdStr)
	ON_EN_CHANGE(IDC_ED_VAL, OnChangeEdVal)

	ON_EN_CHANGE(IDC_ED_LAP, OnChangeEdLap)
	ON_EN_CHANGE(IDC_ED_UAP, OnChangeEdUap)
	ON_EN_CHANGE(IDC_ED_NAP, OnChangeEdNap)
    
    ON_EN_CHANGE(IDC_ED_FILTER, OnChangeEdFilter)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUT_DEL_KEY, OnButDelKey)
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_COMMAND(ID_FACTORY_FACTORYRESTORE, OnFactoryFactoryrestore)
	ON_COMMAND(ID_FACTORY_FACTORYRESTOREALL, OnFactoryFactoryrestoreall)
	ON_COMMAND(ID_FACTORY_FACTORYSET, OnFactoryFactoryset)
	ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
	ON_COMMAND_RANGE(ID_KEY_CANNED_MIN, ID_KEY_CANNED_MAX, OnCanned)
	ON_COMMAND(ID_VIEW_FRIENDLYNAMES, OnViewFriendlynames)
	ON_COMMAND(ID_VIEW_PROGRAMMERIDS, OnViewProgrammerids)
	ON_COMMAND(ID_VIEW_SORTBYNAME, OnViewSortbyname)
	ON_COMMAND(ID_VIEW_SORTBYORDINAL, OnViewSortbyordinal)
	ON_COMMAND(ID_VIEW_SHOWONLYSET, OnViewShowOnlySet)
	ON_UPDATE_COMMAND_UI(IDC_DESCRIBE, OnUpdateDescribe)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FRIENDLYNAMES, OnUpdateViewFriendlyNames)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROGRAMMERIDS, OnUpdateViewProgrammerIds)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SORTBYNAME, OnUpdateViewSortByName)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SORTBYORDINAL, OnUpdateViewSortByOrdinal)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOWONLYSET, OnUpdateViewShowOnlySet)
	ON_COMMAND(ID_KEY_EDITRAW, OnKeyEditraw)
	ON_BN_CLICKED(IDC_BUT_RECONNECCT, OnButReconnect)
	ON_COMMAND(ID_HELP_FIRMWARE, OnHelpFirmware)
	ON_COMMAND(ID_FILE_DUMP, OnFileDump)
	ON_COMMAND(ID_FILE_MERGE, OnFileMerge)
	ON_COMMAND(ID_FILE_RUNQUERY, OnFileRunquery)
	ON_COMMAND(ID_STORES_ALL, OnStoresAll)
	ON_COMMAND(ID_STORES_IMPLEMENTATION, OnStoresImplementation)
	ON_COMMAND(ID_STORES_RAMONLY, OnStoresRAMonly)
	ON_COMMAND(ID_STORES_ROMONLY, OnStoresROMonly)
	ON_COMMAND(ID_STORES_FACTORYONLY, OnStoresFactoryOnly)
	ON_COMMAND(ID_STORES_NOTRAM, OnStoresNotRAM)
	ON_UPDATE_COMMAND_UI(ID_STORES_ALL, OnUpdateStoresAll)
	ON_UPDATE_COMMAND_UI(ID_STORES_IMPLEMENTATION, OnUpdateStoresImplementation)
	ON_UPDATE_COMMAND_UI(ID_STORES_RAMONLY, OnUpdateStoresRAMonly)
	ON_UPDATE_COMMAND_UI(ID_STORES_ROMONLY, OnUpdateStoresROMonly)
	ON_UPDATE_COMMAND_UI(ID_STORES_FACTORYONLY, OnUpdateStoresFactoryOnly)
	ON_UPDATE_COMMAND_UI(ID_STORES_NOTRAM, OnUpdateStoresNotRAM)
	ON_BN_CLICKED(IDC_BUT_NOTPRESENT, OnButNotPresent)
	ON_COMMAND(ID_BOOTMODE_NONE, OnBootmodeNone)
	ON_COMMAND(ID_BOOTMODE_0, OnBootmode0)
	ON_COMMAND(ID_BOOTMODE_1, OnBootmode1)
	ON_COMMAND(ID_BOOTMODE_2, OnBootmode2)
	ON_COMMAND(ID_BOOTMODE_3, OnBootmode3)
	ON_COMMAND(ID_BOOTMODE_4, OnBootmode4)
	ON_COMMAND(ID_BOOTMODE_5, OnBootmode5)
	ON_COMMAND(ID_BOOTMODE_6, OnBootmode6)
	ON_COMMAND(ID_BOOTMODE_7, OnBootmode7)
	ON_UPDATE_COMMAND_UI(ID_BOOTMODE_NONE, OnUpdateBootmodeNone)
	ON_UPDATE_COMMAND_UI(ID_BOOTMODE_0, OnUpdateBootmode0)
	ON_UPDATE_COMMAND_UI(ID_BOOTMODE_1, OnUpdateBootmode1)
	ON_UPDATE_COMMAND_UI(ID_BOOTMODE_2, OnUpdateBootmode2)
	ON_UPDATE_COMMAND_UI(ID_BOOTMODE_3, OnUpdateBootmode3)
	ON_UPDATE_COMMAND_UI(ID_BOOTMODE_4, OnUpdateBootmode4)
	ON_UPDATE_COMMAND_UI(ID_BOOTMODE_5, OnUpdateBootmode5)
	ON_UPDATE_COMMAND_UI(ID_BOOTMODE_6, OnUpdateBootmode6)
	ON_UPDATE_COMMAND_UI(ID_BOOTMODE_7, OnUpdateBootmode7)
	ON_COMMAND(ID_BOOTMODE_DELETE_KEY, OnBootmodeKeyDelete)
	ON_UPDATE_COMMAND_UI(ID_BOOTMODE_DELETE_KEY, OnUpdateBootmodeDeleteKey)
	ON_COMMAND(ID_BOOTMODE_SHOWONLYOVERRIDES, OnBootmodeShowOnlyOverrides)
	ON_UPDATE_COMMAND_UI(ID_BOOTMODE_SHOWONLYOVERRIDES, OnUpdateBootmodeShowOnlyOverrides)
	//}}AFX_MSG_MAP

    ON_UPDATE_COMMAND_UI(IDC_BUT_DEL_KEY, OnUpdateDeleteButton)
    ON_UPDATE_COMMAND_UI(IDC_SET, OnUpdateSetButton)
    ON_UPDATE_COMMAND_UI(IDC_READ, OnUpdateReadButton)
    ON_UPDATE_COMMAND_UI(IDC_RESETCLOSE, OnUpdateResetCloseButton)
    ON_UPDATE_COMMAND_UI(IDC_BUT_RESET, OnUpdateResetButton)

    ON_UPDATE_COMMAND_UI(ID_FILE_MERGE, OnUpdateFileMerge)
    ON_UPDATE_COMMAND_UI(ID_FILE_RUNQUERY, OnUpdateFileQuery)
    ON_UPDATE_COMMAND_UI(ID_FILE_DUMP, OnUpdateFileDump)

    ON_UPDATE_COMMAND_UI(ID_HELP_FIRMWARE, OnUpdateFirmwareId)
    ON_UPDATE_COMMAND_UI(ID_KEY_EDITRAW, OnUpdateEditRaw)
	ON_WM_INITMENUPOPUP()
    ON_MESSAGE(WM_KICKIDLE, OnKickIdle) 
    ON_MESSAGE (WM_DIALOG_DISPLAYED, OnDialogDisplayed)

    ON_UPDATE_COMMAND_UI(ID_FACTORY_FACTORYSET, OnUpdateFactorySet)
    ON_UPDATE_COMMAND_UI(ID_FACTORY_FACTORYRESTORE, OnUpdateFactoryRestore)
    ON_UPDATE_COMMAND_UI(ID_FACTORY_FACTORYRESTOREALL, OnUpdateFactoryRestoreAll)
    ON_WM_WINDOWPOSCHANGED()
    END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPSToolDlg message handlers

BOOL CPSToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString str;
		str.LoadString(IDS_ABOUTBOX);
		if (!str.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, str);
		}
	}

	// Create popup with canned operation entries
	const CPSHelper::canned_entry* entry = canned;

	if(entry->contents != 0)
	{
		CMenu pCannedPopup;
		
		if(pCannedPopup.CreatePopupMenu())
		{
			int i = 0;

			while(entry->contents != 0)
			{
				if(i >= _MAX_CANNED_OPS)
				{
					break;
				}

				pCannedPopup.InsertMenu(-1, MF_BYPOSITION, ID_KEY_CANNED_MIN + i, entry->name);

				entry++;
				i++;
			}
		}

		CMenu* pMainMenu = GetMenu();

		if(pMainMenu != NULL)
		{
			CMenu* pEntryMenu = pMainMenu->GetSubMenu(_ENTRY_MENU_POS);

			if(pEntryMenu != NULL)
			{
				pEntryMenu->InsertMenu(-1, MF_BYPOSITION|MF_POPUP, (UINT)pCannedPopup.Detach(), II("Canned Operations"));
			}
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	EnableControls(ENABLE_KEYS_NONE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CPSToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}

	{
		CDialog::OnSysCommand(nID, lParam);
	}
}


BOOL CPSToolDlg::PreTranslateMessage(MSG* pMsg) 
{
	return CDialog::PreTranslateMessage(pMsg);
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPSToolDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPSToolDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


bool CPSToolDlg::CheckToSaveCommon(int keys)
{
    bool do_continue = true;
	if (m_entry && m_changed)
	{
        int ret = MessageBox(II("The value has changed but has not yet been written.  Do you want to write the value?"), II("Value has not been written"), keys | MB_ICONWARNING);
		if (ret == IDYES)
        {
            // If set fails don't carry on with subsequent action (like reset)
			do_continue = DoSet();
        }
		else if (ret == IDNO)
        {
            m_changed = false;
        }
        else if (ret == IDCANCEL)
        {
            do_continue = false;
        }
	}
    return do_continue;
}

bool CPSToolDlg::CheckToSave()
{
    return CheckToSaveCommon(MB_YESNO);
}

bool CPSToolDlg::CheckToSaveOnExit()
{
    return CheckToSaveCommon(MB_YESNOCANCEL);
}

void CPSToolDlg::OnSelchangeListKeys() 
{
	ichar s[1000];
	CheckToSave();
	EnableControls(ENABLE_KEYS_NONE);
	if (m_list_keys.GetCurSel() == -1)
	{
		m_entry = NULL;
		return;
	}

	// A key has been selected, so let's display it's contents...
	pskey key =  (pskey)(m_list_keys.GetItemData(m_list_keys.GetCurSel()));
	m_entry = m_helper->GetKeyDetails(key);
	if (m_helper->HasPresentationKeys(key) > 0)
	{
		// Recommend the use of other keys.
	}
	isprintf(s, 1000, II("%d (0x%04x) %s"), m_entry->key_number, m_entry->key_number, 
		(m_show_method == SHOW_FRIENDLY)? m_entry->id : m_entry->label);
	m_key_label.SetWindowText(s);

	OnRead();
}


int HexCharToInt(ichar c)
{	
	if ((c >= II('0')) && (c <= II('9')))
		return c - II('0');
	if ((c >= II('a')) && (c <= II('f')))
		return c - II('a') + 10;
	if ((c >= II('A')) && (c <= II('F')))
		return c - II('A') + 10;
	return -1;
}

bool CPSToolDlg::SetRawData
(
    const uint16 key, 
    const uint16 len, 
    bool &proceed           // Flag used to indicate that user code not to proceed on length error
)
{
    int usedLen = 0;
	CString str;
    bool ret = false;
    uint16 data[MAX_HEX_DATA];
        
	m_ed_str.GetWindowText(str);

    if (GetHexData<uint16>(str, data, MAX_HEX_DATA, &usedLen, 4))
    {
	    if ((len != (uint16)(-1)) && (len != 0) && (usedLen != len))
	    {
		    if (MessageBox(II("The length of the data you have entered does not match the typical length in the persistent store.  Proceed with write?"), II("Mismatch in entry length"), MB_YESNO | MB_ICONWARNING) == IDNO)
            {
                proceed = false;
			    return false;
            }
	    }
        ret = m_helper->psbc_set_pres_or_raw(key, data, usedLen);
    }
    else
    {
        MessageBox(II("The value is not valid. Enter a hexadecimal string"));
        proceed = false;
    }

    return ret;
}

#ifndef _WINCE
bool CPSToolDlg::SetPowerTableKey()
{
    CString str;
    CPowerTable pt;
    ASSERT(m_entry->key_constraint == pskey_cons_none);
    pt.element_count = m_grid_ps.GetRows() - 2;

    for (int i = 0; i < pt.element_count; i++)
    {
        str = m_grid_ps.GetTextMatrix(i + 1, 0);
        pt.elements[i].internal_pa = iatoi(str);
        str = m_grid_ps.GetTextMatrix(i + 1, 1);
        pt.elements[i].external_pa = iatoi(str);
        str = m_grid_ps.GetTextMatrix(i + 1, 2);
        pt.elements[i].dBm = iatoi(str);
    }
    pt.Sort();
    return m_helper->psbc_set_power_table(m_entry->key_number, pt);
}

bool CPSToolDlg::SetEnhPowerTableKey()
{
    CString str;
    if (m_entry->key_number == PSKEY_LC_ENHANCED_POWER_TABLE)
    {
        // OLD POWER TABLE
        // Extract from the grid.
        CEnhancedPowerTable pt;
        ASSERT(m_entry->key_constraint == pskey_cons_none);
        pt.element_count = m_grid_ps.GetRows() - 2;

        for (int i = 0; i < pt.element_count; i++)
        {
            str = m_grid_ps.GetTextMatrix(i + 1, EPT_BASIC_INT_PA);
            pt.elements[i].basic_internal_pa = iatoi(str);
            str = m_grid_ps.GetTextMatrix(i + 1, EPT_BASIC_EXT_PA);
            pt.elements[i].basic_external_pa = iatoi(str);
            str = m_grid_ps.GetTextMatrix(i + 1, EPT_BASIC_TX_PA_ATTN);
            pt.elements[i].basic_tx_pa_attn = iatoi(str);
            str = m_grid_ps.GetTextMatrix(i + 1, EPT_BASIC_CONTROL_BIT);
            pt.elements[i].external_PA_control_bits = iatoi(str);
            str = m_grid_ps.GetTextMatrix(i + 1, EPT_ENH_INT_PA);
            pt.elements[i].enhanced_internal_pa = iatoi(str);
            str = m_grid_ps.GetTextMatrix(i + 1, EPT_ENH_EXT_PA);
            pt.elements[i].enhanced_external_pa = iatoi(str);
            str = m_grid_ps.GetTextMatrix(i + 1, EPT_ENH_CLASS);
            pt.elements[i].enhanced_class = iatoi(str);
            str = m_grid_ps.GetTextMatrix(i + 1, EPT_ENH_TX_PA_ATTN);
            pt.elements[i].enhanced_tx_pa_attn = iatoi(str);
            str = m_grid_ps.GetTextMatrix(i + 1, EPT_TX_PRE);
            pt.elements[i].enhanced_tx_pre = iatoi(str);
            str = m_grid_ps.GetTextMatrix(i + 1, EPT_OUTPUT_DBM);
            pt.elements[i].output_power_dBm = iatoi(str);
            str = m_grid_ps.GetTextMatrix(i + 1, EPT_NO_BR);
            pt.elements[i].no_br = iatoi(str);
        }
        pt.Sort();
        return m_helper->psbc_set_enhanced_power_table(m_entry->key_number, pt);
    }
    else
    {
        // NEW POWER TABLE
        // Extract from the grid.
        CBluetoothPowerTableV0 pt;
        ASSERT(m_entry->key_constraint == pskey_cons_none);
        pt.element_count = m_grid_ps.GetRows() - 2;

        for (int i = 0; i < pt.element_count; i++)
        {
            str = m_grid_ps.GetTextMatrix(i + 1, PTV0_BASIC_INT_PA);
            pt.elements[i].basic_internal_pa = iatoi(str);
            str = m_grid_ps.GetTextMatrix(i + 1, PTV0_BASIC_TX_MIX_LEVEL);
            pt.elements[i].basic_tx_mix_level = iatoi(str);
            str = m_grid_ps.GetTextMatrix(i + 1, PTV0_BASIC_TX_MIX_OFFSET);
            pt.elements[i].basic_tx_mix_offset = iatoi(str);
            str = m_grid_ps.GetTextMatrix(i + 1, PTV0_BASIC_TX_PA_ATTN);
            pt.elements[i].basic_tx_pa_attn = iatoi(str);
            str = m_grid_ps.GetTextMatrix(i + 1, PTV0_ENH_INT_PA);
            pt.elements[i].enhanced_internal_pa = iatoi(str);
            str = m_grid_ps.GetTextMatrix(i + 1, PTV0_ENH_CLASS);
            pt.elements[i].enhanced_class = iatoi(str);
            str = m_grid_ps.GetTextMatrix(i + 1, PTV0_ENH_TX_MIX_LEVEL);
            pt.elements[i].enhanced_tx_mix_level = iatoi(str);
            str = m_grid_ps.GetTextMatrix(i + 1, PTV0_ENH_TX_MIX_OFFSET);
            pt.elements[i].enhanced_tx_mix_offset = iatoi(str);
            str = m_grid_ps.GetTextMatrix(i + 1, PTV0_ENH_TX_PA_ATTN);
            pt.elements[i].enhanced_tx_pa_attn = iatoi(str);
            str = m_grid_ps.GetTextMatrix(i + 1, PTV0_OUTPUT_DBM);
            pt.elements[i].output_power_dBm = iatoi(str);
            str = m_grid_ps.GetTextMatrix(i + 1, PTV0_NO_BR);
            pt.elements[i].no_br = iatoi(str);
        }
        pt.Sort();
        return m_helper->psbc_set_bluetooth_power_table_v0(m_entry->key_number, pt);
    }
}
#endif // _WINCE

template <typename T> bool CPSToolDlg::StringToInteger(T& t)
{
    ASSERT(sizeof(T) <= sizeof(long));
    CString str;
    m_ed_val.GetWindowText(str);
        // for some reason only uint32s can be hex. everything handled here is always decimal.
    long l = iatol(str);
    if (str.GetLength() == 0)
        return false;
    if(l > (numeric_limits<T>::max)() || l < (numeric_limits<T>::min)())
        return false;
    t = (T) l;
    return true;
}

template <> bool CPSToolDlg::StringToInteger(unsigned long &t)
{
    CString str;
    m_ed_val.GetWindowText(str);
    t = istrtoul(str, NULL, m_entry->format == pskey_format_dec ? 10 : 16);
    return true;
}

template <typename T> bool CPSToolDlg::SetIntegerKey()
{
    T t;
    if( !StringToInteger(t)
        || ((m_entry->key_constraint == pskey_cons_min_max || m_entry->key_constraint == pskey_cons_min)
           && KeyMinimum<T>() > t)
        || ((m_entry->key_constraint == pskey_cons_min_max || m_entry->key_constraint == pskey_cons_max)
           && KeyMaximum<T>() < t))
    {
        ichar err[256];
        isprintf(err, 256, II("The value entered is out of the range %d to %d."), KeyMinimum<T>(), KeyMaximum<T>());
        m_error_handler->on_error(err, II("Invalid Value"), BCCMD_TRANS_ERROR_OPERATION, BCCMD_TRANS_ERRTYPE_ABORT);
        return false;
    }
    else
    {
        ASSERT(sizeof(uint16)==2);
        uint16 data[sizeof(T)/2];
        switch(sizeof(T))
        {
            case 4:
// we know it's 4 bytes in size so quiet the warning.
#pragma warning( push )
#pragma warning( disable: 4333 )
                data[0] = (uint16)((t >> 16) & 0xffff);
#pragma warning( pop )
                data[1] = (uint16)(t & 0xffff);
                return m_helper->psbc_set_raw(m_entry->key_number, data, 2);
            default:
                ASSERT(false);	// deliberate fall through to 16-bit.
            case 2:
                data[0] = (uint16) t;
                return m_helper->psbc_set_uint16(m_entry->key_number, data[0]);
        }
    }
}

// *****************************************************************************************************
// This function sets the persistent store based on m_entry with the data from the correct edit field .
// *****************************************************************************************************
bool CPSToolDlg::SetKey
(
    bool &proceed   // Flag used to indicate that user chose not to proceed on length error
) 
{
	uint16 data[_MAX_SETKEY_DATA];
	uint16 len;
	CString str;
	CWaitCursor hourglass;
    bool ret = false;

	if (!m_entry)
    {
		return false;  // Quit if we have nothing set.
    }
	m_error_handler->clear_errors();

	if(m_entry->key_constraint == pskey_cons_enum)
	{
		switch(m_entry->structure_type)
		{
		case	pskey_struct_uint32:
		case	pskey_struct_bdcod:
			ret = m_helper->psbc_set_uint32(m_entry->key_number, m_combo_enum.GetItemData(m_combo_enum.GetCurSel()));
			break;
		default:
			ret = m_helper->psbc_set_uint16(m_entry->key_number, (uint16) m_combo_enum.GetItemData(m_combo_enum.GetCurSel()));
		}
	}
	else
	{
		switch(m_entry->structure_type)
		{
			default:
				// not a type we know. assert and fall through to raw.
			case pskey_struct_deep_sleep_state:
			case pskey_struct_edr_switch_mode:
			case pskey_struct_hwakeup_state:
			case pskey_struct_iq_trim_enable_flag:
			case pskey_struct_phys_bus:
				// enumerated type should have been handled earlier. assert and fall through to raw.
				ASSERT(false);
			case pskey_struct_BCSP_LM_PS_BLOCK:
			case pskey_struct_HOSTIO_FC_PS_BLOCK:
			case pskey_struct_HOSTIO_UART_PS_BLOCK:
			case pskey_struct_LM_LINK_KEY_BD_ADDR_T:
			case pskey_struct_PROTOCOL_INFO:
			case pskey_struct_TIME_array:
			case pskey_struct_agc_hyst_config:
			case pskey_struct_bc5_gc_trim_bias_control:
			case pskey_struct_clkgen_active_rate:
			case pskey_struct_cpu_idle_mode:
			case pskey_struct_lc_fc_lwm:
			case pskey_struct_mmu_rate_jemima:
			case pskey_struct_temperature_calibration:
			case pskey_struct_temperature_calibration_10bit:
			case pskey_struct_txpre_compensation:
			case pskey_struct_usbclass:
			case pskey_struct_wideband_rssi_config:
				// Write it from raw
				{
					ASSERT(m_entry->key_constraint == pskey_cons_none);
					// try to get the size of a value on the chip first. if not then use the default.
					if(!m_helper->psbc_size(m_entry->key_number, &len))
						len = m_helper->StructureLen(m_entry->structure_type);
					ret = SetRawData(m_entry->key_number, len, proceed);
				}
				break;
			case pskey_struct_BT_TIMER_SLOT_DATA_array:
			case pskey_struct_BT_TIMER_TRIGGER_DATA_array:
			case pskey_struct_IQ_LVL_VAL_array:
			case pskey_struct_golden_curve_freq_steps_array:
			case pskey_struct_int16_array:
			case pskey_struct_temperature_calibration_array:
			case pskey_struct_uint16_array:
			case pskey_struct_uint32_array:
			case pskey_struct_uint8_array:
			case pskey_struct_vm_log_entry_array:
				// Write it from raw
				{
					ASSERT(m_entry->key_constraint == pskey_cons_none);
					// try to get the size of a value on the chip first. if not then use the default.
					if(!m_helper->psbc_size(m_entry->key_number, &len))
						len = PsDefaultArrayLength;
					ret = SetRawData(m_entry->key_number, len, proceed);
				}
				break;

			case pskey_struct_char_array:
			case pskey_struct_local_name_section:
			case pskey_struct_unicodestring:
				{
					m_ed_str.GetWindowText(str);
					len = str.GetLength();
					if(len <= _MAX_SETKEY_DATA)
					{
					    if (m_entry->structure_type == pskey_struct_local_name_section && len > PSKEY_STRING_FRAGMENT_LENGTH)
					    {
						    ASSERT(PSKEY_STRING_FRAGMENT_LENGTH == 14); // check caption below
						    MessageBox(II("Fragmented string sections should be less than 14 characters.  Refer to the Description for Local Name 0"), II("Truncated to 14 characters"), MB_OK);
						    len = PSKEY_STRING_FRAGMENT_LENGTH;
					    }
					    for (int i = 0; i < len; i++)
					    {
						    data[i] = str[i];
					    }
					    ret = m_helper->psbc_set_raw(m_entry->key_number, data, len);
					}
                    else
                    {
						MessageBox(II("String too long"));
                        proceed = false;
                    }
				}
				break;

			case pskey_struct_utf8_array:
				{
					ASSERT(m_entry->key_constraint == pskey_cons_none);
					m_ed_str.GetWindowText(str);

					// First find the size
					int size = WideCharToMultiByte(CP_UTF8, 0, (LPCTSTR)str, str.GetLength(), NULL, 0, NULL, NULL);
					
					if(size < _MAX_SETKEY_DATA * sizeof(data[0]))
					{
						// Just reset the data so that the string is NULL terminated
						memset(data, 0, sizeof(uint16) * _MAX_SETKEY_DATA);

						size = WideCharToMultiByte(CP_UTF8, 0, (LPCTSTR)str, str.GetLength(), (char*)data, (_MAX_SETKEY_DATA * sizeof(data[0])) - 1, NULL, NULL);

						char* p = (char*)data;

						if(size & 0x01)
						{
							p[size++] = 0;
						}

						len = size / 2;
						ret = m_helper->psbc_set_raw(m_entry->key_number, data, len);
					}
                    else
                    {
						MessageBox(II("String too long"));
                        proceed = false;
                    }
				}
				break;

#ifndef _WINCE
			case	pskey_struct_power_setting_array:
				// Extract from the grid.
				ret = SetPowerTableKey();
				break;

			case pskey_struct_enhanced_power_setting_array:
				ret = SetEnhPowerTableKey();
				break;
#endif

			case	pskey_struct_local_name_complete:
				{
					ASSERT(m_entry->key_constraint == pskey_cons_none);
					m_ed_str.GetWindowText(str);
					for (int i = 0; i < str.GetLength(); i++)
					{
						data[i] = str[i];
					}

					ret = m_helper->set_fragmented_string(m_entry->base_key, data, str.GetLength());
				}
				break;

			case	pskey_struct_uint32:
			case	pskey_struct_bdcod:
#ifndef NO_PSDB_TIME_TYPES
			case	pskey_struct_TIME:
#endif
			case	pskey_struct_int32:
			case	pskey_struct_uint16:
			case	pskey_struct_uint8:
			case	pskey_struct_int16:
			case	pskey_struct_int8:
			case	pskey_struct_bool:
				{
					switch(m_entry->key_constraint)
					{
						case pskey_cons_none:	// Display it raw. sometimes.
							if(m_entry->structure_type == pskey_struct_uint16
							   || m_entry->structure_type == pskey_struct_uint8
							   || m_entry->structure_type == pskey_struct_int16
							   || m_entry->structure_type == pskey_struct_int8
							   || m_entry->structure_type == pskey_struct_bool)
							{
								ret = SetRawData(m_entry->key_number, 1, proceed);
								break;
							}
							// else fall through.
						case pskey_cons_min_max:
						case pskey_cons_min:
						case pskey_cons_max:
							switch(m_entry->structure_type)
							{
							case	pskey_struct_uint32:
							case	pskey_struct_bdcod:
								ret = SetIntegerKey<uint32>();
								break;
#ifndef NO_PSDB_TIME_TYPES
							case	pskey_struct_TIME:
#endif
							case	pskey_struct_int32:
								ret = SetIntegerKey<int32>();
								break;
							default:
								ASSERT(false);
							case	pskey_struct_uint16:
							case	pskey_struct_uint8:
								ret = SetIntegerKey<uint16>();
								break;
							case	pskey_struct_int16:
							case	pskey_struct_int8:
							case	pskey_struct_bool:
								ret = SetIntegerKey<int16>();
								break;
							}
							break;
						case pskey_cons_str_len:
							ASSERT(false);  // We're dealing with numbers here...  
							break;
						default:
							ASSERT(false);  // We're dealing with numbers here...  
							break; 
					}

				}
				break;
			case	pskey_struct_bdaddr:
				{
					uint32 lap, uap, nap;
                    int len;

					CString strlap, struap, strnap;

					ASSERT(m_entry->key_constraint == pskey_cons_none);

					m_ed_lap.GetWindowText(strlap);
					m_ed_uap.GetWindowText(struap);
					m_ed_nap.GetWindowText(strnap);

                    if (GetHexData<uint32>(strlap, &lap, 1, &len, 6) &&
                        GetHexData<uint32>(struap, &uap, 1, &len, 2) &&
                        GetHexData<uint32>(strnap, &nap, 1, &len, 4))
                    {
					    lap = istrtoul(strlap, NULL, 16);
					    uap = istrtoul(struap, NULL, 16);
					    nap = istrtoul(strnap, NULL, 16);
		
    					uint16 buf[4];

					    buf[0] = (uint16) (lap >> 16) & 0x000000ff;
					    buf[1] = (uint16) lap & 0x0000ffff; 
					    buf[2] = (uint16) uap &  0x00ff;
					    buf[3] = (uint16) nap;

    					ret = m_helper->psbc_set_raw(m_entry->key_number, buf, 4);
                    }
                    else
                    {
                        proceed = false;
                        MessageBox(II("Bluetooth address is invalid"));
                    }
				}
				break;
		}
	}

    // If there is an error returned from sending the BCCMD, tell the user and don't read back the key
    // otherwise changes to the key are lost
    if (m_error_handler->get_last_error_code() != 0)
    {
        ret = false;
    }
    else if (ret)
    {
	    m_changed = false;
    }

    return ret;
}


void CPSToolDlg::DisplayRawData(const uint16 *data, const uint16 len)
{
	ichar *str = (ichar*)malloc((len * 5 + 1) * sizeof(ichar));
    ichar *strptr = str;

    if (str == NULL)
        return;

	for (int i = 0; i < len; i++)
	{
        isprintf(strptr, 5, II("%04x"), data[i]);
		if (i < len - 1)
			isprintf(strptr + 4, 2, II(" "));
        strptr += 5;
	}
    *strptr = 0;
	m_ed_str.SetWindowText(str);
	SetFormat(edit_format_hex_words);
	m_changed = false;
	m_raw_len = len;
	EnableControls(ENABLE_KEYS_STRING);

    free(str);
}


void CPSToolDlg::DisplayWarningIfNotPresent(bool present)
{
	if (m_error_handler->get_last_error_code() != 0)
    {
		m_caption_present.SetWindowText(II("* ERROR *"));
    }
	else
	{
		if(m_attempt_edit)
		{
			m_caption_present.SetWindowText(II("ENTRY IS NOT PRESENT"));
		}
		else
		{
			m_but_notpresent.ShowWindow(SW_SHOW);
			if (m_helper->key_known(m_entry->key_number))
			{
				m_caption_present.SetWindowText(II("")); // Blanked off 'cos the button displays the text
			}
			else
			{
				m_caption_present.SetWindowText(II("ENTRY IS NOT PRESENT IN THIS BOOT MODE"));
			}
		}
	}
	m_caption_present.ShowWindow(!present);
	m_present = present;
}

#ifndef _WINCE
void CPSToolDlg::ReadPowerTableKey()
{
    CPowerTable pt;
    ASSERT(m_entry->key_constraint == pskey_cons_none);
    if (!m_helper->psbc_get_power_table(m_entry->key_number, &pt))
    {
        DisplayWarningIfNotPresent(false);
        pt.element_count = 0;
        if(!m_attempt_edit)
        {
            return;
        }
    }
    EnableControls(ENABLE_KEYS_POWER_TABLE);
    m_grid_ps.Clear();
    m_grid_ps.SetCols(3);
    m_grid_ps.SetRows(pt.element_count + 2);
    // Titles
    m_grid_ps.SetTextMatrix(0, 0, II("Internal PA"));
    m_grid_ps.SetTextMatrix(0, 1, II("External PA"));
    m_grid_ps.SetTextMatrix(0, 2, II("dBm"));

    for (int i = 0; i < pt.element_count; i++)
    {
        ichar s[20];
        isprintf(s, 20, II("%d"), pt.elements[i].internal_pa);
        m_grid_ps.SetTextMatrix(i + 1, 0, s);
        isprintf(s, 20, II("%d"), pt.elements[i].external_pa);
        m_grid_ps.SetTextMatrix(i + 1, 1, s);
        isprintf(s, 20, II("%d"), pt.elements[i].dBm);
        m_grid_ps.SetTextMatrix(i + 1, 2, s);
    }
    SetFormat(edit_format_none);
}

void CPSToolDlg::AutofitColumns()
{
    // need to convert to twips so calculate conversion factor
    CDC* pDC = GetDC();
    int TwipsPerPixel = 1440 / pDC->GetDeviceCaps(LOGPIXELSX);

    CRect rect;
    m_grid_ps.GetClientRect(&rect);
    int colWidth = rect.Width() / m_grid_ps.GetCols() * TwipsPerPixel ;

    // setup column widths and word wrap header text
    m_grid_ps.SetWordWrap(TRUE);
    for (int x = 0; x < m_grid_ps.GetCols(); x++)
    {
        m_grid_ps.SetColAlignment(x, 0);
        m_grid_ps.SetColWidth(x, colWidth);
    }
    m_grid_ps.SetRowHeight(0, 650);
}

void CPSToolDlg::ReadEnhPowerTableKey()
{
    if (m_entry->key_number == PSKEY_LC_ENHANCED_POWER_TABLE)
    {
        CEnhancedPowerTable pt;
        ASSERT(m_entry->key_constraint == pskey_cons_none);
        if (!m_helper->psbc_get_enhanced_power_table(m_entry->key_number, &pt))
        {
                DisplayWarningIfNotPresent(false);
            pt.element_count = 0;
            if(!m_attempt_edit)
            {
                return;
            }
        }
        EnableControls(ENABLE_KEYS_POWER_TABLE);
        m_grid_ps.Clear();
        m_grid_ps.SetCols(EPT_NUM_FIELDS);
        m_grid_ps.SetRows(pt.element_count + 2);

        AutofitColumns();

        // Titles
        m_grid_ps.SetTextMatrix(0, EPT_BASIC_INT_PA,     II("Basic Int PA"));
        m_grid_ps.SetTextMatrix(0, EPT_BASIC_EXT_PA,     II("Basic Ext PA"));
        m_grid_ps.SetTextMatrix(0, EPT_BASIC_TX_PA_ATTN, II("Basic TX PA Attn"));
	    m_grid_ps.SetTextMatrix(0, EPT_BASIC_CONTROL_BIT,II("Ctrl Bit"));
        m_grid_ps.SetTextMatrix(0, EPT_ENH_INT_PA,       II("Enh Int PA"));
        m_grid_ps.SetTextMatrix(0, EPT_ENH_EXT_PA,       II("Enh Ext PA"));
        m_grid_ps.SetTextMatrix(0, EPT_ENH_CLASS,        II("Enh Class"));
        m_grid_ps.SetTextMatrix(0, EPT_ENH_TX_PA_ATTN,   II("Enh TX PA Attn"));
        m_grid_ps.SetTextMatrix(0, EPT_TX_PRE,           II("Enh TX Pre"));
        m_grid_ps.SetTextMatrix(0, EPT_OUTPUT_DBM,       II("O/p dBm"));
        m_grid_ps.SetTextMatrix(0, EPT_NO_BR,            II("No-BR"));

        for (int i = 0; i < pt.element_count; i++)
        {
            ichar s[20];
            isprintf(s, 20, II("%d"), pt.elements[i].basic_internal_pa);
            m_grid_ps.SetTextMatrix(i + 1, EPT_BASIC_INT_PA, s);
            isprintf(s, 20, II("%d"), pt.elements[i].basic_external_pa);
            m_grid_ps.SetTextMatrix(i + 1, EPT_BASIC_EXT_PA, s);
            isprintf(s, 20, II("%d"), pt.elements[i].basic_tx_pa_attn);
            m_grid_ps.SetTextMatrix(i + 1, EPT_BASIC_TX_PA_ATTN, s);
            isprintf(s, 20, II("%d"), pt.elements[i].external_PA_control_bits);
            m_grid_ps.SetTextMatrix(i + 1, EPT_BASIC_CONTROL_BIT, s);
            isprintf(s, 20, II("%d"), pt.elements[i].enhanced_internal_pa);
            m_grid_ps.SetTextMatrix(i + 1, EPT_ENH_INT_PA, s);
            isprintf(s, 20, II("%d"), pt.elements[i].enhanced_external_pa);
            m_grid_ps.SetTextMatrix(i + 1, EPT_ENH_EXT_PA, s);
            isprintf(s, 20, II("%d"), pt.elements[i].enhanced_class);
            m_grid_ps.SetTextMatrix(i + 1, EPT_ENH_CLASS, s);
            isprintf(s, 20, II("%d"), pt.elements[i].enhanced_tx_pa_attn);
            m_grid_ps.SetTextMatrix(i + 1, EPT_ENH_TX_PA_ATTN, s);
            isprintf(s, 20, II("%d"), pt.elements[i].enhanced_tx_pre);
            m_grid_ps.SetTextMatrix(i + 1, EPT_TX_PRE, s);
            isprintf(s, 20, II("%d"), pt.elements[i].output_power_dBm);
            m_grid_ps.SetTextMatrix(i + 1, EPT_OUTPUT_DBM, s);
            isprintf(s, 20, II("%d"), pt.elements[i].no_br);
            m_grid_ps.SetTextMatrix(i + 1, EPT_NO_BR, s);
        }
        SetFormat(edit_format_none);
    }
    else
    {
        CBluetoothPowerTableV0 pt;
        ASSERT(m_entry->key_constraint == pskey_cons_none);
        if (!m_helper->psbc_get_bluetooth_power_table_v0(m_entry->key_number, &pt))
        {
            DisplayWarningIfNotPresent(false);
            pt.element_count = 0;
            if(!m_attempt_edit)
            {
                return;
            }
        }
        EnableControls(ENABLE_KEYS_POWER_TABLE);
        m_grid_ps.Clear();
        m_grid_ps.SetCols(PTV0_NUM_FIELDS);
        m_grid_ps.SetRows(pt.element_count + 2);

        AutofitColumns();

        // Titles
        m_grid_ps.SetTextMatrix(0, PTV0_BASIC_INT_PA,     II("Basic Int PA"));
        m_grid_ps.SetTextMatrix(0, PTV0_BASIC_TX_MIX_LEVEL,     II("Basic Tx Mix Level"));
        m_grid_ps.SetTextMatrix(0, PTV0_BASIC_TX_MIX_OFFSET,     II("Basic Tx Mix Offset"));
        m_grid_ps.SetTextMatrix(0, PTV0_BASIC_TX_PA_ATTN, II("Basic TX PA Attn"));

        m_grid_ps.SetTextMatrix(0, PTV0_ENH_INT_PA,       II("Enh Int PA"));
        m_grid_ps.SetTextMatrix(0, PTV0_ENH_TX_MIX_LEVEL,       II("Enh Tx Mix Level"));
        m_grid_ps.SetTextMatrix(0, PTV0_ENH_TX_MIX_OFFSET,   II("Enh TX Mix Offset"));
        m_grid_ps.SetTextMatrix(0, PTV0_ENH_CLASS,        II("Enh Class"));
        m_grid_ps.SetTextMatrix(0, PTV0_ENH_TX_PA_ATTN, II("Enh TX PA Attn"));

        m_grid_ps.SetTextMatrix(0, PTV0_OUTPUT_DBM,       II("O/p dBm"));
        m_grid_ps.SetTextMatrix(0, PTV0_NO_BR,            II("No BR"));

        for (int i = 0; i < pt.element_count; i++)
        {
            ichar s[20];
            isprintf(s, 20, II("%d"), pt.elements[i].basic_internal_pa);
            m_grid_ps.SetTextMatrix(i + 1, PTV0_BASIC_INT_PA, s);

            isprintf(s, 20, II("%d"), pt.elements[i].basic_tx_mix_level);
            m_grid_ps.SetTextMatrix(i + 1, PTV0_BASIC_TX_MIX_LEVEL, s);

            isprintf(s, 20, II("%d"), pt.elements[i].basic_tx_mix_offset);
            m_grid_ps.SetTextMatrix(i + 1, PTV0_BASIC_TX_MIX_OFFSET, s);

            isprintf(s, 20, II("%d"), pt.elements[i].basic_tx_pa_attn);
            m_grid_ps.SetTextMatrix(i + 1, PTV0_BASIC_TX_PA_ATTN, s);

            isprintf(s, 20, II("%d"), pt.elements[i].enhanced_internal_pa);
            m_grid_ps.SetTextMatrix(i + 1, PTV0_ENH_INT_PA, s);

            isprintf(s, 20, II("%d"), pt.elements[i].enhanced_tx_mix_level);
            m_grid_ps.SetTextMatrix(i + 1, PTV0_ENH_TX_MIX_LEVEL, s);

            isprintf(s, 20, II("%d"), pt.elements[i].enhanced_tx_mix_offset);
            m_grid_ps.SetTextMatrix(i + 1, PTV0_ENH_TX_MIX_OFFSET, s);

            isprintf(s, 20, II("%d"), pt.elements[i].enhanced_tx_pa_attn);
            m_grid_ps.SetTextMatrix(i + 1, PTV0_ENH_TX_PA_ATTN, s);

            isprintf(s, 20, II("%d"), pt.elements[i].enhanced_class);
            m_grid_ps.SetTextMatrix(i + 1, PTV0_ENH_CLASS, s);

            isprintf(s, 20, II("%d"), pt.elements[i].enhanced_tx_pa_attn);
            m_grid_ps.SetTextMatrix(i + 1, PTV0_ENH_TX_PA_ATTN, s);

            isprintf(s, 20, II("%d"), pt.elements[i].output_power_dBm);
            m_grid_ps.SetTextMatrix(i + 1, PTV0_OUTPUT_DBM, s);

            isprintf(s, 20, II("%d"), pt.elements[i].no_br);
            m_grid_ps.SetTextMatrix(i + 1, PTV0_NO_BR, s);
        }
        SetFormat(edit_format_none);
    }
}
#endif	// _WINCE

template <typename T> bool CPSToolDlg::ReadIntegerValue(T &t)
{
	uint16 *buf = NULL;
	uint16 len;
	if (!m_helper->psbc_get_raw(m_entry->key_number, &buf, 0, &len))
	{
		DisplayWarningIfNotPresent(false);
		len = m_helper->StructureLen(m_entry->structure_type);
		buf = (uint16 *)malloc(len * sizeof(uint16));
		memset(buf, 0, len * sizeof(uint16));
		if(!m_attempt_edit)
		{
			free(buf);
			return false;
		}
	}
	if(len != m_helper->StructureLen(m_entry->structure_type))
	{
		MessageBox(II("The key cannot be displayed because the size of the data did not match the expected size. Use Edit Raw to edit this key safely."));
		len = m_helper->StructureLen(m_entry->structure_type);
		free(buf);
		buf = (uint16 *)malloc(len * sizeof(uint16));
		memset(buf, 0, len * sizeof(uint16));
	}
	// negate the top bit of word0 for signed types. the mechanics of shifting do the rest for us.
	t = ((numeric_limits<T>::min)() < 0) ? buf[0] - (buf[0] & 0x8000)*2 : buf[0];
	for(int i=1; i<len; ++i)
	{
		t <<= 16;
		t |= buf[i];
	}
	free(buf);
	return true;
}

// special handling of 16-bit types. all presentation keys are 16-bits and we need to do
// this to make sure we handle them properly.
template <> bool CPSToolDlg::ReadIntegerValue(uint16 &t)
{
	if(!m_helper->psbc_get_uint16(m_entry->key_number, &t))
	{
		DisplayWarningIfNotPresent(false);
		t = 0;
		if(!m_attempt_edit)
		{
			return false;
		}
	}
    if (m_error_handler->get_last_error_code() != 0)
    {
        MessageBox(m_error_handler->get_last_error_string(), II("Error"), MB_OK | MB_ICONWARNING);
    }

	return true;
}

template <typename T> void CPSToolDlg::ReadIntegerKey()
{
    T value;
    ichar str[1024];
    ASSERT(m_helper->StructureLen(m_entry->structure_type) <= sizeof(value));
    // for some reason only uint32s and bdcods use the format field. all other types are always decimal.
    bool decimal = m_entry->format == pskey_format_dec
               || (m_entry->structure_type != pskey_struct_uint32 && m_entry->structure_type != pskey_struct_bdcod);

    if(!ReadIntegerValue(value)) return;

    ASSERT(m_entry->key_constraint == pskey_cons_none
           || m_entry->key_constraint == pskey_cons_min_max
           || m_entry->key_constraint == pskey_cons_min
           || m_entry->key_constraint == pskey_cons_max);
    if (!m_present)  // set up default if not there.
    {
        value = 0;
        if (m_entry->key_constraint != pskey_cons_none)
        {
            value = (T) m_entry->constraints.constraint_min_max.min_val;
        }
    }
    str[0] = '\0';
    if(decimal)
    {
		// print differently depending on whether T is signed.
        if((numeric_limits<T>::min)() < 0)
        {
            isprintf(str, 1024, II("%ld"), (long) value);
        }
        else
        {
            isprintf(str, 1024, II("%lu"), (unsigned long) value);
        }
    }
    else
    {
		// clever way of saying isprintf("%08x").
        ichar fmt[] = II("0x%0_lx");
        ASSERT(sizeof(T)*2 < 10);
        fmt[4] = II('0') + (sizeof(T)*2);
        isprintf(str, 1024, fmt, (unsigned long) value);
    }
    EnableControls(ENABLE_KEYS_VAL);

    int spin_min = (m_entry->key_constraint == pskey_cons_min || m_entry->key_constraint == pskey_cons_min_max)
        ? m_entry->constraints.constraint_min_max.min_val
        : KeyMinimum<T>();
    int spin_max = (m_entry->key_constraint == pskey_cons_max || m_entry->key_constraint == pskey_cons_min_max)
        ? m_entry->constraints.constraint_min_max.max_val
        : KeyMaximum<T>();

    if(decimal)
        m_spin_val.SetBase(10);
    else
        m_spin_val.SetBase(16);
    m_spin_val.SetRange32(spin_min, spin_max);

    m_ed_val.SetWindowText(str);
    m_changed = false;

    if(decimal)
        SetFormat(edit_format_decimal);
    else
        SetFormat(edit_format_hex);
}


// return the uintxx key's maximum value
template <typename T> T CPSToolDlg::KeyMaximum()
{
    T max;

    // if a maximum has not been set for uintxx type then
    // set the maximum allowable depending on size of data
    if (m_entry->key_constraint == pskey_cons_min ||
        m_entry->key_constraint == pskey_cons_none)
    {
        if (m_entry->structure_type == pskey_struct_uint8)
        {
            max = UCHAR_MAX;
        }
        else if (m_entry->structure_type == pskey_struct_int8)
        {
            max = CHAR_MAX;
        }
        else
        {
            max = (numeric_limits<T>::max)();
        }
    }
    else
    {
        max = (T)m_entry->constraints.constraint_min_max.max_val;
    }

    return max;
}

// return the uintxx key's maximum value
template <typename T> T CPSToolDlg::KeyMinimum()
{
    T min;

    // if a minimum has not been set for uintxx type then
    // set the minimum to zero
    if (m_entry->key_constraint == pskey_cons_max || 
        m_entry->key_constraint == pskey_cons_none)
    {
        min = (numeric_limits<T>::min)();
    }
    else
    {
        min = (T)m_entry->constraints.constraint_min_max.min_val;
    }

    return min;
}

void CPSToolDlg::ReadKey() // This function populates the correct edit field with the data from m_entry.
{
 	uint16 *buf = NULL;
	uint16 len;
	ichar str[1024];
	CWaitCursor hourglass;
	m_changed = false;
	if (!m_entry) 
    {
        return;  // Quit if we have nothing set.
    }
    if (IsConnected())
    {
    	DisplayWarningIfNotPresent(true); // innocent until proven guilty
    	m_spin_val.SetBuddy(&m_ed_val);
	    ASSERT(m_app);
	    m_error_handler->clear_errors();

	    m_but_notpresent.ShowWindow(SW_HIDE);
    }

	EnableControls(ENABLE_KEYS_NONE);
	SetFormat(edit_format_none);

    // If not connected to chip do nothing
    if (!IsConnected())
    {
        return;
    }

	if(m_entry->key_constraint == pskey_cons_enum)
	{
		uint32 value;
		// special treatment of enumerated types.
		if (!m_helper->psbc_get_pres_or_raw(m_entry->key_number, &buf, 0, &len))
		{
			DisplayWarningIfNotPresent(false);
			value = m_helper->get_enum_list(m_entry->constraints.contraint_enum.enum_list_num)[0].val;
			if(!m_attempt_edit)
			{
				free(buf);
				return;
			}
		}
		else
		{
			switch(len)
			{
				case 2:
					value = (buf[0] << 16) | buf[1];
					break;
				default:
					ASSERT(false);	//deliberate fall-through to 16-bit.
				case 1:
					value = buf[0];
			}
		}

		int i;
		const string_val_pair *s;
		m_combo_enum.ResetContent();
		s = m_helper->get_enum_list(m_entry->constraints.contraint_enum.enum_list_num);
		while (s->str)
		{
			m_combo_enum.SetItemData(m_combo_enum.AddString(s->str), s->val);
			s++;
		}
		for (i = 0; i < m_combo_enum.GetCount(); i++)
		{
			if (value == m_combo_enum.GetItemData(i))
			{
				m_combo_enum.SetCurSel(i);
				break;
			}
		}
		if (i == m_combo_enum.GetCount())
		{
			isprintf(str, 1024, II("Unrecognised value 0x%x"), value);
			i = m_combo_enum.AddString(str);
			m_combo_enum.SetItemData(i, value);
			m_combo_enum.SetCurSel(i);
		}
		EnableControls(ENABLE_KEYS_ENUM);
		SetFormat(edit_format_enum);

		free(buf);
	}
	else
	{
		switch(m_entry->structure_type)
		{
			default:
				ASSERT(false);
			case pskey_struct_BCSP_LM_PS_BLOCK:
			case pskey_struct_HOSTIO_FC_PS_BLOCK:
			case pskey_struct_HOSTIO_UART_PS_BLOCK:
			case pskey_struct_IQ_LVL_VAL_array:
			case pskey_struct_LM_LINK_KEY_BD_ADDR_T:
			case pskey_struct_PROTOCOL_INFO:
			case pskey_struct_agc_hyst_config:
			case pskey_struct_bc5_gc_trim_bias_control:
			case pskey_struct_clkgen_active_rate:
			case pskey_struct_cpu_idle_mode:
			case pskey_struct_deep_sleep_state:
			case pskey_struct_edr_switch_mode:
			case pskey_struct_golden_curve_freq_steps_array:
			case pskey_struct_hwakeup_state:
			case pskey_struct_iq_trim_enable_flag:
			case pskey_struct_lc_fc_lwm:
			case pskey_struct_mmu_rate_jemima:
			case pskey_struct_phys_bus:
			case pskey_struct_temperature_calibration:
			case pskey_struct_temperature_calibration_10bit:
			case pskey_struct_txpre_compensation:
			case pskey_struct_usbclass:
			case pskey_struct_wideband_rssi_config:
				// Display it raw.
				{
					ASSERT(m_entry->key_constraint == pskey_cons_none);
					if (!m_helper->psbc_get_raw(m_entry->key_number, &buf, 0, &len))
					{
						DisplayWarningIfNotPresent(false);
						len = m_helper->StructureLen(m_entry->structure_type);
						if(len == (uint16)(-1)) len = PsDefaultArrayLength;
						buf = (uint16 *)malloc(len * sizeof(uint16));
						memset(buf, 0, len * sizeof(uint16));
						if(!m_attempt_edit)
						{
							break;
						}
					}
					DisplayRawData(buf, len);
					free(buf);
				}
				break;
			case pskey_struct_BT_TIMER_SLOT_DATA_array:
			case pskey_struct_BT_TIMER_TRIGGER_DATA_array:
			case pskey_struct_TIME_array:
			case pskey_struct_int16_array:
			case pskey_struct_temperature_calibration_array:
			case pskey_struct_uint16_array:
			case pskey_struct_uint32_array:
			case pskey_struct_uint8_array:
			case pskey_struct_vm_log_entry_array:
				// Display array types raw.
				{
					ASSERT(m_entry->key_constraint == pskey_cons_none);
					if (!m_helper->psbc_get_raw(m_entry->key_number, &buf, 0, &len))
					{
						DisplayWarningIfNotPresent(false);
						len = PsDefaultArrayLength;
						buf = (uint16 *)malloc(len * sizeof(uint16));
						memset(buf, 0, len * sizeof(uint16));
						if(!m_attempt_edit)
						{
							break;
						}
					}
					DisplayRawData(buf, len);
					free(buf);
				}
				break;
			case pskey_struct_char_array:
			case pskey_struct_local_name_section:
			case pskey_struct_unicodestring:
				{
					if (!m_helper->psbc_get_raw(m_entry->key_number, &buf, 0, &len))
					{
						DisplayWarningIfNotPresent(false);
						len = m_helper->StructureLen(m_entry->structure_type);
						buf = (uint16 *)malloc(len * sizeof(uint16));
						memset(buf, 0, len * sizeof(uint16));
						if(!m_attempt_edit)
						{
							break;
						}
					}
					int i;
					for (i = 0; i < len; i++)
						str[i] = (ichar)(buf[i]);
					str[i] = II('\0');
					m_ed_str.SetWindowText(str);
					m_changed = false;
					EnableControls(ENABLE_KEYS_STRING);
					SetFormat(edit_format_text);

				}
				break;

			case	pskey_struct_utf8_array:
				{
					ASSERT(m_entry->key_constraint == pskey_cons_none);
					if (!m_helper->psbc_get_raw(m_entry->key_number, &buf, 0, &len))
					{
						DisplayWarningIfNotPresent(false);
						len = PsDefaultArrayLength;
						buf = (uint16 *)malloc(len * sizeof(uint16));
						memset(buf, 0, len * sizeof(uint16));
						if(!m_attempt_edit)
						{
							break;
						}
					}

					int size;
					if(len > 0 && (size = MultiByteToWideChar(CP_UTF8, 0, (char*)buf, len * 2, NULL, 0)) > 0)
					{
						ichar* str = new ichar[size + 1];

						MultiByteToWideChar(CP_UTF8, 0, (char*)buf, len * 2, str, size);

						str[size] = II('\0');

						m_ed_str.SetWindowText(str);
					}
					else
					{
						m_ed_str.SetWindowText(_T(""));
					}
					m_changed = false;
					EnableControls(ENABLE_KEYS_STRING);
					SetFormat(edit_format_text);
				}
				break;
#ifndef _WINCE
			case	pskey_struct_power_setting_array:
				// Display it in a power table
				ReadPowerTableKey();
				break;

			case	pskey_struct_enhanced_power_setting_array:
				ReadEnhPowerTableKey();
				break;
#endif
			case	pskey_struct_local_name_complete:
				// Display it raw.
				{
					uint16 data[256];
					ASSERT(m_entry->key_constraint == pskey_cons_none);
					if (!m_helper->get_fragmented_string(m_entry->base_key, data, &len))
					{
						DisplayWarningIfNotPresent(false);
						memset(data, 0, 256);
						len = PsDefaultArrayLength;
						if(!m_attempt_edit)
						{
							break;
						}
					}
					int i;
					for (i = 0; i < len; i++)
						str[i] = (ichar)(data[i]);
					str[i] = II('\0');
					m_ed_str.SetWindowText(str);
					m_changed = false;
					EnableControls(ENABLE_KEYS_STRING);
					SetFormat(edit_format_text);
				}
				break;
			case	pskey_struct_int32:
				ReadIntegerKey<int32>();
				break;
			case	pskey_struct_uint32:
			case	pskey_struct_bdcod:
#ifndef NO_PSDB_TIME_TYPES
			case	pskey_struct_TIME:
#endif
				ReadIntegerKey<uint32>();
				break;
			case	pskey_struct_uint16:
			case	pskey_struct_uint8:
			case	pskey_struct_int16:
			case	pskey_struct_int8:
			case	pskey_struct_bool:
				switch(m_entry->key_constraint)
				{
					case pskey_cons_none:	// Display it raw.
						uint16 data;
						if (!m_helper->psbc_get_uint16(m_entry->key_number, &data))
						{
							DisplayWarningIfNotPresent(false);
							data = 0;
							m_raw_len = 1;
							if(!m_attempt_edit)
							{
								break;
							}
						}
                        if (m_error_handler->get_last_error_code() != 0)
                        {
                            MessageBox(m_error_handler->get_last_error_string(), II("Error"), MB_OK | MB_ICONWARNING);
                        }
						DisplayRawData(&data, 1);
						break;
					case pskey_cons_min_max:
					case pskey_cons_min:
					case pskey_cons_max:
						if(m_entry->structure_type == pskey_struct_uint8 || m_entry->structure_type == pskey_struct_uint16)
						{
							ReadIntegerKey<uint16>();
						}
						else
						{
							ReadIntegerKey<int16>();
						}
						break;
					case pskey_cons_str_len:
						ASSERT(false);  // We're dealing with numbers here...  
						break;
					default:
						ASSERT(false);  // We're dealing with numbers here...  
						break;
				}
				break;

			case	pskey_struct_bdaddr:
				{
					uint32 lap, uap, nap;
					ASSERT(m_entry->key_constraint == pskey_cons_none);
					if (!m_helper->psbc_get_raw(m_entry->key_number, &buf, 0, &len))
					{
						DisplayWarningIfNotPresent(false);
						lap = uap = nap = 0;
						if(!m_attempt_edit)
						{
							break;
						}
					}
					else
					{
                        if (len == m_helper->StructureLen(m_entry->structure_type))
                        {
						    lap = ((buf[0] & 0x00ff) << 16) + buf[1]; 
						    uap = (buf[2] & 0x00ff);
						    nap = buf[3];
                        }
                        else
                        {
    						lap = uap = nap = 0;
                            m_caption_present.SetWindowText(II("The key cannot be displayed because the size of the data did not match the expected size. Use Edit Raw to edit this key safely."));
                        	m_caption_present.ShowWindow(true);
                        }
					}
					ichar strlap[7], struap[3], strnap[5];

					isprintf(strlap, 7, II("%06x") , lap);
					isprintf(struap, 3, II("%02x") , uap);
					isprintf(strnap, 5, II("%04x") , nap);

					m_ed_lap.SetWindowText(strlap);
					m_ed_uap.SetWindowText(struap);
					m_ed_nap.SetWindowText(strnap);

					m_changed = false;
					EnableControls(ENABLE_KEYS_BD_ADDR);
					SetFormat(edit_format_bd_addr);
					
					free(buf);
				}
				break;
		}
	}
}

// Convert the specified string to lower case
void ToLower(ichar *s)
{
	while (*s != NULL)
	{
		*s = itolower(*s);
		s++;
	}
}

void CPSToolDlg::RefreshKeyList(void)
{	
	ichar str[500];
	const ichar *label;
	ichar label_lc[500]; // Lower-case version of label (only used when filtering the contents of the key list box)
	ASSERT(m_helper);
	CheckToSave();
	m_list_keys.ResetContent();

    // Exit if not connected
    if (!IsConnected())
    {
        return;
    }

	// Get the contents of the filter edit box and make it lower case
	TCHAR filter[64];
	m_ed_filter.GetWindowText(filter, sizeof(filter)/sizeof(*filter) - 1);
	ToLower(filter);

	for (int i = 0; i < CPSHelper::pskey_max; i++)
	{
		const pskey_entry *key = m_helper->GetKeyDetails(i);
		if (!key) continue;

		uint16 bm;
		if(!m_show_only_overrides)
		{
			bm = m_helper->GetBootmode();
			m_helper->SetBootmode(PsServer::NoBootmode());
		}

		bool known = m_helper->key_known(key->key_number);

		if(!m_show_only_overrides)
			m_helper->SetBootmode(bm);

		if (!known) continue;

		switch (m_show_method)
		{
		case SHOW_FRIENDLY:
			label = key->label;
			break;
		case SHOW_ID:
			label = key->id;
			if (istrncmp(label, II("PSKEY_"), 6) == 0)
				label += 6;
			break;
		}
	
		// If the user has specified a filter, then apply it here
		if (filter[0])
		{
			istrncpy(label_lc, label, 499);
			ToLower(label_lc);
			if (!istrstr(label_lc, filter)) continue;
		}

		if(m_show_only_set)
		{
			uint16 len;
			if(!m_helper->psbc_size(key->key_number, &len)) continue;
		}

		switch (m_sort_method)
		{
		case SORT_UNSORTED:
			isprintf(str, 500, II("%04X\t%s"), i, label);
			break;
		case SORT_BY_NAME:
			isprintf(str, 500, II("%s"), label);
			break;
		case SORT_BY_ORDINAL:
			isprintf(str, 500, II("%04X\t%s"), key->key_number, label);
			break;
		}

		// add the key label (and number) to the list.
		m_list_keys.SetItemData(m_list_keys.AddString(str), i);
	}	
	m_entry = NULL;
	m_but_notpresent.ShowWindow(SW_SHOW);
	m_st_edit_format.SetWindowText(_T(""));
	EnableControls(ENABLE_KEYS_NONE);
}

void CPSToolDlg::EnableControls(enable_keys keys)
{
	m_combo_enum.ShowWindow(keys == ENABLE_KEYS_ENUM);
	m_ed_val.ShowWindow(keys == ENABLE_KEYS_VAL);
	m_spin_val.ShowWindow(keys == ENABLE_KEYS_VAL);
	m_ed_str.ShowWindow(keys == ENABLE_KEYS_STRING);

	m_but_ps_insert.ShowWindow(keys == ENABLE_KEYS_POWER_TABLE);
	m_but_ps_edit.ShowWindow(keys == ENABLE_KEYS_POWER_TABLE);
	m_but_ps_delete.ShowWindow(keys == ENABLE_KEYS_POWER_TABLE);

    // Prevent assertion     
    if (::IsWindow(m_grid_ps.m_hWnd))
    {
        m_grid_ps.ShowWindow(keys == ENABLE_KEYS_POWER_TABLE);
    }

    m_ed_lap.ShowWindow(keys == ENABLE_KEYS_BD_ADDR);
    m_ed_uap.ShowWindow(keys == ENABLE_KEYS_BD_ADDR);
    m_ed_nap.ShowWindow(keys == ENABLE_KEYS_BD_ADDR);
    m_static_lap.ShowWindow(keys == ENABLE_KEYS_BD_ADDR);
    m_static_uap.ShowWindow(keys == ENABLE_KEYS_BD_ADDR);
    m_static_nap.ShowWindow(keys == ENABLE_KEYS_BD_ADDR);

}

void CPSToolDlg::OnButNotPresent(void)
{
	m_attempt_edit = true;
	ReadKey();
}

int CPSToolDlg::CommonHandleRetry(ichar *title, bool success)
{
    int action = IDOK;
    if (!success)
    {
        // Display the error with a retry option
        const ichar *err = m_error_handler->get_last_error_string();
        const ichar *err_to_display = err ? err : II("Unknown error");

        UINT message_type = MB_ICONWARNING;
        // If the error type is abort then don't display a retry option
        if (m_error_handler->get_last_error_type() == BCCMD_TRANS_ERRTYPE_ABORT)
        {
            message_type |= MB_OK;
        }
        else
        {
            message_type |= MB_RETRYCANCEL;
        }
        action = MessageBox(err_to_display, title, message_type);
    }

    return action;
}

bool CPSToolDlg::DoSet()
{
    bool ret = false;
    bool retry = false;
    bool proceed = true;
    do
    {
        ret = SetKey(proceed);
	    if (ret)
        {
	        OnRead();
        }

        // If the user didn't proceed on a length error then don't display an error
        if (proceed)
        {
            retry = (CommonHandleRetry(II("Set failed"), ret) == IDRETRY);
        }
    }
    while (retry);

    return ret;
}

void CPSToolDlg::OnSet()
{
    (void) DoSet();
}

void CPSToolDlg::OnRead()
{
	m_attempt_edit = false;
	ReadKey();
}

void CPSToolDlg::OnButPsInsert() 
{

    if (m_grid_ps.GetRows() == MAX_POWER_TABLE_ELEMS)
    {
        MessageBox(II("The power table already contains the maximum number of entries"), II("Error"), MB_OK | MB_ICONWARNING);

        return;
    }
	int row = m_grid_ps.GetRow();

	ichar rowch[100];
	CString s;

	*rowch = II('\0');
	switch (m_entry->structure_type)
	{
	case pskey_struct_enhanced_power_setting_array:
		{
            if (m_entry->key_number == PSKEY_LC_ENHANCED_POWER_TABLE)
            {
			    EPTEditDlg dlg;
			    dlg.mode = EPTEditDlg::Insert;

			    if (dlg.DoModal() == IDOK) {
				    isprintf(rowch, 100, II("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d"), 
						    dlg.basic_internal_pa, 
						    dlg.basic_external_pa, 
						    dlg.basic_tx_pa_attn,
						    dlg.external_PA_control_bits, 
						    dlg.enhanced_internal_pa, 
						    dlg.enhanced_external_pa, 
						    dlg.enhanced_class, 
						    dlg.enhanced_tx_pa_attn, 
						    dlg.enhanced_tx_pre, 
						    dlg.output_power_dBm,
                            dlg.no_br);
			    }		
            }
            else
            {
			    PowerTableV0Dlg dlg;
			    dlg.mode = PowerTableV0Dlg::Insert;


			    if (dlg.DoModal() == IDOK) {
				    isprintf(rowch, 100, II("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d"), 
						    dlg.basic_internal_pa, 
						    dlg.basic_tx_mix_level, 
						    dlg.basic_tx_mix_offset, 
						    dlg.basic_tx_pa_attn, 
						    dlg.enhanced_internal_pa, 
						    dlg.enhanced_tx_mix_level, 
						    dlg.enhanced_tx_mix_offset, 
						    dlg.enhanced_class, 
						    dlg.enhanced_tx_pa_attn, 
						    dlg.output_power_dBm,
                            dlg.no_br);
                }
            }
		}
		break;
	
	case pskey_struct_power_setting_array:
		{
			CPSEditDlg *dlg = new CPSEditDlg;

			if (dlg->DoModal() == IDOK) {
				isprintf(rowch, 100, II("%d\t%d\t%d"), dlg->m_butval_internal_pa, dlg->m_butval_external_pa, dlg->m_butval_dbm);
			}
			delete dlg;
		}
		break;
	}
	if (istrlen(rowch))
	{
		VARIANT v;
		v.vt = VT_I4;
		v.lVal = row;
		m_grid_ps.AddItem(rowch, v);
		m_changed = true;
        AutofitColumns();
    }
}

void CPSToolDlg::OnButPsEdit() 
{
	int row = m_grid_ps.GetRow();
	if (row == m_grid_ps.GetRows() - 1 )
	{
		OnButPsInsert();
		return;
	}
	else if (row > m_grid_ps.GetRows() - 1 )
	{
		return;
	}

	ichar rowch[100];
	CString s;

	*rowch = II('\0');
	switch (m_entry->structure_type)
	{
	    case pskey_struct_enhanced_power_setting_array:
		{
            if (m_entry->key_number == PSKEY_LC_ENHANCED_POWER_TABLE)
            {
			    EPTEditDlg dlg;
			    dlg.mode = EPTEditDlg::Edit;

			    s = m_grid_ps.GetTextMatrix(row, EPT_BASIC_INT_PA);
			    dlg.basic_internal_pa = iatoi(s);
			    s = m_grid_ps.GetTextMatrix(row, EPT_BASIC_EXT_PA);
			    dlg.basic_external_pa = iatoi(s);
			    s = m_grid_ps.GetTextMatrix(row, EPT_BASIC_TX_PA_ATTN);
			    dlg.basic_tx_pa_attn = iatoi(s);
			    s = m_grid_ps.GetTextMatrix(row, EPT_BASIC_CONTROL_BIT);
			    dlg.external_PA_control_bits = iatoi(s);
			    s = m_grid_ps.GetTextMatrix(row, EPT_ENH_INT_PA);
			    dlg.enhanced_internal_pa = iatoi(s);
			    s = m_grid_ps.GetTextMatrix(row, EPT_ENH_EXT_PA);
			    dlg.enhanced_external_pa = iatoi(s);
			    s = m_grid_ps.GetTextMatrix(row, EPT_ENH_CLASS);
			    dlg.enhanced_class = iatoi(s);
			    s = m_grid_ps.GetTextMatrix(row, EPT_ENH_TX_PA_ATTN);
			    dlg.enhanced_tx_pa_attn = iatoi(s);
			    s = m_grid_ps.GetTextMatrix(row, EPT_TX_PRE);
			    dlg.enhanced_tx_pre = iatoi(s);
			    s = m_grid_ps.GetTextMatrix(row, EPT_OUTPUT_DBM);
			    dlg.output_power_dBm = iatoi(s);
			    s = m_grid_ps.GetTextMatrix(row, EPT_NO_BR);
			    dlg.no_br = iatoi(s);

			    if (dlg.DoModal() == IDOK) {
				    if (m_grid_ps.GetRow() < m_grid_ps.GetRows() )
					    m_grid_ps.RemoveItem(m_grid_ps.GetRow());
				    isprintf(rowch, 100, II("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d"), 
						    dlg.basic_internal_pa, 
						    dlg.basic_external_pa, 
						    dlg.basic_tx_pa_attn, 
						    dlg.external_PA_control_bits, 
						    dlg.enhanced_internal_pa, 
						    dlg.enhanced_external_pa, 
						    dlg.enhanced_class, 
						    dlg.enhanced_tx_pa_attn, 
						    dlg.enhanced_tx_pre, 
						    dlg.output_power_dBm,
                            dlg.no_br);
			    }		
            }
            else
            {
			    PowerTableV0Dlg dlg;
			    dlg.mode = PowerTableV0Dlg::Edit;

			    s = m_grid_ps.GetTextMatrix(row, PTV0_BASIC_INT_PA);
			    dlg.basic_internal_pa = iatoi(s);
			    s = m_grid_ps.GetTextMatrix(row, PTV0_BASIC_TX_MIX_LEVEL);
			    dlg.basic_tx_mix_level = iatoi(s);
			    s = m_grid_ps.GetTextMatrix(row, PTV0_BASIC_TX_MIX_OFFSET);
			    dlg.basic_tx_mix_offset = iatoi(s);
			    s = m_grid_ps.GetTextMatrix(row, PTV0_BASIC_TX_PA_ATTN);
			    dlg.basic_tx_pa_attn = iatoi(s);
			    s = m_grid_ps.GetTextMatrix(row, PTV0_ENH_INT_PA);
			    dlg.enhanced_internal_pa = iatoi(s);
                s = m_grid_ps.GetTextMatrix(row, PTV0_ENH_TX_MIX_LEVEL);
			    dlg.enhanced_tx_mix_level = iatoi(s);
			    s = m_grid_ps.GetTextMatrix(row, PTV0_ENH_TX_MIX_OFFSET);
			    dlg.enhanced_tx_mix_offset = iatoi(s);
			    s = m_grid_ps.GetTextMatrix(row, PTV0_ENH_CLASS);
			    dlg.enhanced_class = iatoi(s);
			    s = m_grid_ps.GetTextMatrix(row, PTV0_ENH_TX_PA_ATTN);
			    dlg.enhanced_tx_pa_attn = iatoi(s);
			    s = m_grid_ps.GetTextMatrix(row, PTV0_OUTPUT_DBM);
			    dlg.output_power_dBm = iatoi(s);
			    s = m_grid_ps.GetTextMatrix(row, PTV0_NO_BR);
			    dlg.no_br = iatoi(s);

			    if (dlg.DoModal() == IDOK) {
				    if (m_grid_ps.GetRow() < m_grid_ps.GetRows() )
					    m_grid_ps.RemoveItem(m_grid_ps.GetRow());
				    isprintf(rowch, 100, II("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d"), 
						    dlg.basic_internal_pa, 
						    dlg.basic_tx_mix_level, 
						    dlg.basic_tx_mix_offset, 
						    dlg.basic_tx_pa_attn, 
						    dlg.enhanced_internal_pa, 
						    dlg.enhanced_tx_mix_level, 
						    dlg.enhanced_tx_mix_offset, 
						    dlg.enhanced_class, 
						    dlg.enhanced_tx_pa_attn, 
						    dlg.output_power_dBm,
                            dlg.no_br);
			    }		
            }
		}
		break;
	
	case pskey_struct_power_setting_array:
		{
			CPSEditDlg *dlg = new CPSEditDlg;
			s = m_grid_ps.GetTextMatrix(row, 0);
			dlg->m_butval_internal_pa = iatoi(s);
			s = m_grid_ps.GetTextMatrix(row, 1);
			dlg->m_butval_external_pa = iatoi(s);
			s = m_grid_ps.GetTextMatrix(row, 2);
			dlg->m_butval_dbm = iatoi(s);

		//	dlg->SetWindowText("Edit Power Table Value");  Can't do this - there is no hWnd for the dialog yet.

			if (dlg->DoModal() == IDOK) {
				if (m_grid_ps.GetRow() < m_grid_ps.GetRows() )
					m_grid_ps.RemoveItem(m_grid_ps.GetRow());
				isprintf(rowch, 100, II("%d\t%d\t%d"), dlg->m_butval_internal_pa, dlg->m_butval_external_pa, dlg->m_butval_dbm);
			}
			delete dlg;
		}
		break;

	}
	if (istrlen(rowch))
	{
		VARIANT v;
		v.vt = VT_I4;
		v.lVal = row;
		m_grid_ps.AddItem(rowch, v);
		m_changed = true;
	}
}

void CPSToolDlg::OnButPsDelete() 
{
	if (m_grid_ps.GetRow() < m_grid_ps.GetRows() - 1 )
	{
		m_grid_ps.RemoveItem(m_grid_ps.GetRow());
        AutofitColumns();
        m_changed = true;
	}
}


void CPSToolDlg::OnDescribe() 
{
	if (!m_entry)
		return;

	CPSDescrDlg *dlg = new CPSDescrDlg(m_entry, m_helper);

	dlg->DoModal();

	delete dlg;
}

int CPSToolDlg::ResetCommon()
{
    bool retry = false;
    int action = IDOK;
    bool cancelled = false;
    do
    {
        bool ret = DoBasicServerOperation(SERVER_OPERATION_RESET, cancelled);
        if (!cancelled)
        {
            action = CommonHandleRetry(II("Reset failed"), ret);
        }
    }
    while ((action == IDRETRY) && (!cancelled));

    if (cancelled)
    {
        action = IDCANCEL;
    }
    return action;

}

void CPSToolDlg::OnButReset() 
{
    if (CheckToSave())
    {
        (void) ResetCommon();
    }

}

// ===============================================================================
// If the user "cancels" an unsuccessful reconnect whole menus need to be disabled
// ===============================================================================
void CPSToolDlg::Enable_Menus_Reconnect(bool successful_reconnect)
{
    UINT menu_state = successful_reconnect ? MF_BYPOSITION | MF_ENABLED : MF_BYPOSITION | MF_GRAYED;
	CMenu* pMainMenu = GetMenu();

	if(pMainMenu != NULL)
	{
        // Canned operations - assume alst entry on "entry" menu
	    CMenu* pEntryMenu = pMainMenu->GetSubMenu(_ENTRY_MENU_POS);
        int count = pEntryMenu->GetMenuItemCount();
        pEntryMenu->EnableMenuItem(count - 1, menu_state);
    }
}

void CPSToolDlg::DoConnect() 
{
    PSCmdLineInfo cmdLineInfo;
    m_app->ParseCommandLine(cmdLineInfo);

    if (!CheckToSave())
    {
        // If failed to save change then do not continue
        return;
    }
	bool selected = m_list_keys.GetCurSel() != -1;
	pskey key;
	if(selected) key = (pskey)(m_list_keys.GetItemData(m_list_keys.GetCurSel()));

	PsServer *psc = NULL;

	// Open up the Choose Transport dialog.
    CChooseTransDlg transDlg;
    transDlg.CommandLineOptions(cmdLineInfo);
	int nResponse = transDlg.DoModal();
    if (nResponse == IDCANCEL)
	{
        // if this is the "connect" when the app is first run then exit the app
        if (m_initial_connect)
        {
            OnButClose();
        }
		return;
	}
    CWaitCursor hourglass;
    if (IsConnected())
    {
	    m_helper->GetServer()->close();
        delete m_helper->GetServer();
        m_helper->ClearServer();
    }
    bool retry = false;
    bool successful_reconnect = false;
    bool cancelled = false;
    do
    {
        retry = false;

        m_helper->ClearServer();
        delete psc;
        psc = transDlg.GetNewTransport(m_error_handler);
        m_helper->SetServer(psc);

        successful_reconnect = DoBasicServerOperation(SERVER_OPERATION_CONNECT, cancelled);
        if (successful_reconnect)
        {
           successful_reconnect = m_helper->CheckValid();
        }
        if ((!successful_reconnect) && (!cancelled))
        {
           int ret = MessageBox(m_error_handler->get_last_error_string(), II("Error"), MB_RETRYCANCEL | MB_ICONWARNING);
           retry = (ret == IDRETRY);

        }
    }
    while ((retry) && (!cancelled));
        
    Enable_Menus_Reconnect(successful_reconnect);
    if (successful_reconnect)
    {
	    RefreshKeyList();

        // If doing the first connect then select "bluetooth address" otherwise select
        // key that was previously being viewed
	    if(selected || m_initial_connect)
	    {
		    int i=0;
            pskey key_to_match = m_initial_connect ? PSKEY_BDADDR : key;
		    while(i<m_list_keys.GetCount() && (pskey) m_list_keys.GetItemData(i) != key_to_match) ++i;
		    if(i<m_list_keys.GetCount())
		    {
			    m_list_keys.SetCurSel(i);
			    OnSelchangeListKeys();
		    }
	    }
    }
    else
    {
        m_changed = false;                  /* Clear the changed flag, we're not connected */
        EnableControls(ENABLE_KEYS_NONE);   /* Hide the edit box (whichever is in use) */
        SetFormat(edit_format_none);        /* Clear the PSkey label*/
        m_helper->GetServer()->close();
        delete m_helper->GetServer();
        m_helper->ClearServer();
    }
}

void CPSToolDlg::OnButReconnect() 
{
    m_initial_connect = false;
    DoConnect();
}

void CPSToolDlg::OnSelchangeComboEnum() 
{
	m_changed = true;
}

void CPSToolDlg::OnChangeEdStr() 
{
	m_changed = true;	
}

void CPSToolDlg::OnChangeEdVal() 
{
	m_changed = true;		
}

void CPSToolDlg::OnChangeEdFilter()
{
	RefreshKeyList();
}

void CPSToolDlg::OnChangeEdUap() 
{
	m_changed = true;	
}

void CPSToolDlg::OnChangeEdLap() 
{
	m_changed = true;	
}

void CPSToolDlg::OnChangeEdNap() 
{
	m_changed = true;	
}

void CPSToolDlg::OnClose() 
{
	if (CheckToSaveOnExit())
    {
	    ::DestroyWindow(m_grid_ps.m_hWnd);
	    CDialog::OnClose();
    }
}

void CPSToolDlg::OnFileExit() 
{
	OnButClose();
}

void CPSToolDlg::OnButClose() 
{
	if (CheckToSaveOnExit())
    {
    	::DestroyWindow(m_grid_ps.m_hWnd);
	    EndDialog(IDOK);
    }
}

void CPSToolDlg::OnButResetClose() 
{
	if (CheckToSaveOnExit())
    {
        if (ResetCommon() != IDCANCEL)
        {
            // If user selects "Cancel" if get error then do not exit
        	::DestroyWindow(m_grid_ps.m_hWnd);
	        EndDialog(IDOK);
        }
    }
}

void CPSToolDlg::OnButDelKey() 
{
	if (m_entry)
    {
	    ASSERT(m_helper);
        int action = IDOK;
        do
        {
    	    bool ret = m_helper->psbc_clr(m_entry->key_number);
            action = CommonHandleRetry(II("Delete failed"), ret);
        }   
        while (action == IDRETRY);

	    OnRead();
    }
}

void CPSToolDlg::OnBootmodeKeyDelete()
{
	if (m_entry)
    {
	    ASSERT(m_helper);
	    m_helper->psbc_clrb(m_entry->key_number);
	    OnRead();
    }
}

void CPSToolDlg::OnFactoryFactoryrestore() 
{
	ASSERT(m_helper);
	m_helper->psbc_factory_restore(false);
	OnRead();
}

void CPSToolDlg::OnFactoryFactoryrestoreall() 
{
	ASSERT(m_helper);
	m_helper->psbc_factory_restore(true);
	OnRead();
}

void CPSToolDlg::OnFactoryFactoryset() 
{
	ASSERT(m_helper);
	m_helper->psbc_factory_set();
	OnRead();
}

void CPSToolDlg::OnHelpAbout() 
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

static void cannedOp_formatValue(ichar *pisz, size_t size, const CPSHelper::canned_operation* op, const pskey_entry *key)
{
	if(op != NULL && key != NULL) // are we safe?
    {
	    switch(key->structure_type)
	    {
	    default:
	    case pskey_struct_uint16:
		    {
			    uint16 v = LOWORD(op->value);
			    isprintf(pisz, size, II(" 0x%04X"), v);
		    }
		    break;
	    case pskey_struct_uint32:
		    {
			    uint32 v = op->value;
			    if(key->format == pskey_format_dec)
				    isprintf(pisz, size, II(" %lu"), v);
			    else
				    isprintf(pisz, size, II(" 0x%08lX"), v);
		    }
		    break;
	    }
    }
}

void CPSToolDlg::OnCanned(UINT nID) 
{
	ASSERT(m_helper != NULL);

	int index = nID - ID_KEY_CANNED_MIN;

	CString description;

	ichar* buf = description.GetBuffer(_MAX_BUF_SIZE);
	
	const CPSHelper::canned_operation* op = canned[index].contents;

	isprintf(buf, _MAX_BUF_SIZE, II("Apply canned operation '%s'? This will effect the following keys:\r\n\r\n"), canned[index].name);

	bool flip = false;
    ichar iszCantDelete[_MAX_BUF_SIZE] = II("Can't delete the following (they're not set):\r\n");
	bool flipCantDelete = false;
	unsigned cnCantDelete = 0;
    PsServer *pss = m_helper->GetServer();

	unsigned actions = 0;

    BeginWaitCursor();
	while(op->key != 0)
	{
		const pskey_entry *key = m_helper->GetKeyDetails(op->key);
		ichar isz[_MAX_BUF_SIZE] = II("");
		bool display_key = false;
        bool exists = pss->key_exists(op->key);

        if(op->operation == CPSHelper::ps_file_op_delete)
        {
            if(m_helper->psbc_size(op->key, NULL))
            {
                if((istrlen(buf) + istrlen(II("delete ")) + istrlen(key->id) + istrlen(II("\r\n"))) >= _MAX_BUF_SIZE)
                    break;
                istrcat(buf, II("delete "));
                istrcat(buf, key->id);
                display_key = true;
            }
            else
            {
                if((istrlen(iszCantDelete) + istrlen(key->id) + istrlen(II("\r\n"))) >= _MAX_BUF_SIZE)
                    break;
                istrcat(iszCantDelete, key->id);
                ++cnCantDelete;
                if(flipCantDelete)
                    istrcat(iszCantDelete, II("\r\n"));
                else
                    istrcat(iszCantDelete, II(" \t"));
                flipCantDelete = !flipCantDelete;
            }
        }
        else
        {   
            if(exists)
            {
                switch(op->operation)
                {
                case CPSHelper::ps_file_op_null:
                    // do nothing: list terminator
                    break;
                case CPSHelper::ps_file_op_set:
                    cannedOp_formatValue(isz, _MAX_BUF_SIZE, op, key);
                    if((istrlen(buf) + istrlen(II("set ")) + istrlen(key->id) + istrlen(isz) + istrlen(II("\r\n"))) >= _MAX_BUF_SIZE)
                        break;
                    istrcat(buf, II("set "));
                    istrcat(buf, key->id);
                    istrcat(buf, isz);
                    display_key = true;
                    break;
                case CPSHelper::ps_file_op_set_if_new:
                    if(!m_helper->psbc_size(op->key, NULL))
                    {
                        cannedOp_formatValue(isz, _MAX_BUF_SIZE, op, key);
                        if((istrlen(buf) + istrlen(II("set ")) + istrlen(key->id) + istrlen(isz) + istrlen(II("\r\n"))) >= _MAX_BUF_SIZE)
                            break;
                        istrcat(buf, II("set "));
                        istrcat(buf, key->id);
                        istrcat(buf, isz);
                        display_key = true;
                    }
                    break;
                case CPSHelper::ps_file_op_replace:
                    if(m_helper->psbc_size(op->key, NULL))
                    {
                        cannedOp_formatValue(isz, _MAX_BUF_SIZE, op, key);
                        if((istrlen(buf) + istrlen(II("set ")) + istrlen(key->id) + istrlen(isz) + istrlen(II("\r\n"))) >= _MAX_BUF_SIZE)
                            break;
                        istrcat(buf, II("set "));
                        istrcat(buf, key->id);
                        istrcat(buf, isz);
                        display_key = true;
                    }
                    break;
                case CPSHelper::ps_file_op_delete:
                    // Already done above
                    break;
                }
            }
        }
		if(display_key)
		{
//			istrcat(buf, key->id);
			++actions;
			if(flip)
			{
				istrcat(buf, II("\r\n"));
			}
			else
			{
				istrcat(buf, II(" \t"));
			}
			
			flip = !flip;
		}

		op++;
	}
    EndWaitCursor();
    if(actions > 0)
    {
        if(cnCantDelete > 0)
        {
            size_t l = istrlen(buf);
            if(flip && l > 2) // remove the space and tab
            {    
                buf[l-2] = '\0';
                istrcat(buf, II("\r\n"));
            }
            istrcat(buf, II("\r\n"));
            if((istrlen(buf) + istrlen(iszCantDelete)) < _MAX_BUF_SIZE)
                istrcat(buf, iszCantDelete);
        }
        int response = AfxMessageBox(buf, MB_OKCANCEL|MB_ICONQUESTION, 0);
        if(response == IDOK)
        {
            BeginWaitCursor();

            m_helper->apply_canned(index);
            OnRead();

            EndWaitCursor();

        	if (m_error_handler->get_last_error_code() != 0)
            {
                MessageBox(m_error_handler->get_last_error_string(), II("Canned operation failed"), MB_OK | MB_ICONWARNING);
            }
            else
            {
                AfxMessageBox(II("Completed canned operation."), MB_OK|MB_ICONINFORMATION, 0);
            }
        }
    }
    else
    {
        if(cnCantDelete > 0)
            isprintf(buf, _MAX_BUF_SIZE, II("Canned operation '%s'.\r\n\r\n%s"), canned[index].name, iszCantDelete);
        else
            isprintf(buf, _MAX_BUF_SIZE, II("Canned operation '%s'. No applicable operations to do. No keys will be changed.\r\n\r\n"), canned[index].name);
        AfxMessageBox(buf, MB_OK|MB_ICONEXCLAMATION, 0);
    }

	description.ReleaseBuffer();
}

void CPSToolDlg::OnViewFriendlynames() 
{	
	m_show_method = SHOW_FRIENDLY;	
	RefreshKeyList();
}

void CPSToolDlg::OnViewProgrammerids() 
{
	m_show_method = SHOW_ID;	
	RefreshKeyList();
}

void CPSToolDlg::OnViewUnsorted() 
{
	m_sort_method = SORT_UNSORTED;
	RefreshKeyList();
}

void CPSToolDlg::OnViewSortbyname() 
{
	m_sort_method = SORT_BY_NAME;
	RefreshKeyList();
}

void CPSToolDlg::OnViewSortbyordinal() 
{
	m_sort_method = SORT_BY_ORDINAL;
	RefreshKeyList();
}

void CPSToolDlg::OnViewShowOnlySet()
{
	m_show_only_set = !m_show_only_set;
	RefreshKeyList();
}

void CPSToolDlg::OnUpdateDescribe(CCmdUI* pCmdUI) 
{
    // enable describe button if a pskey is selected 
    pCmdUI->Enable(m_list_keys.GetCurSel() != -1);
}

void CPSToolDlg::OnUpdateViewFriendlyNames(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable(IsConnected());
	pCmdUI->SetRadio(m_show_method == SHOW_FRIENDLY);	
}

void CPSToolDlg::OnUpdateViewProgrammerIds(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable(IsConnected());
	pCmdUI->SetRadio(m_show_method == SHOW_ID);	
}

void CPSToolDlg::OnUpdateViewUnsorted(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable(IsConnected());
	pCmdUI->SetRadio(m_sort_method == SORT_UNSORTED);	
}

void CPSToolDlg::OnUpdateViewSortByName(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable(IsConnected());
	pCmdUI->SetRadio(m_sort_method == SORT_BY_NAME);		
}

void CPSToolDlg::OnUpdateViewSortByOrdinal(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable(IsConnected());
	pCmdUI->SetRadio(m_sort_method == SORT_BY_ORDINAL);		
}

void CPSToolDlg::OnUpdateViewShowOnlySet(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(IsConnected());
	pCmdUI->SetRadio(m_show_only_set);
}

void CPSToolDlg::OnKeyEditraw() 
{
	CPSRawDlg dlg;
	ASSERT(m_helper);
	dlg.m_helper = m_helper;
	if (m_entry)
		dlg.m_startkeynum = m_entry->key_number;
	else
		dlg.m_startkeynum = 1;
	dlg.DoModal();
	OnRead();
}

void CPSToolDlg::SetFormat(edit_format_type formatType)
{
	m_st_edit_format.SetWindowText(formatStrings[int(formatType)]);
}

void CPSToolDlg::OnHelpFirmware() 
{
	const size_t sNameBuffer = 512;
	uint16 nameBuffer[sNameBuffer];
	int ver;
	if (m_helper->GetFirmwareId(&ver, nameBuffer, sNameBuffer))
	{
		ichar copyNameBuffer[100];
		uint16 *p = nameBuffer;
		ichar *d = copyNameBuffer;
		while (*d++ = *p++)
			;
		ichar s[sNameBuffer + 100];
		if (istrlen(copyNameBuffer))
		{
			isprintf(s, sNameBuffer + 100, II("The chip reports version %d (0x%04x): \n  \"%s\""), ver, ver, copyNameBuffer);
		}
		else
		{
			isprintf(s, sNameBuffer + 100, II("The chip reports version %d (0x%04x)."), ver, ver);
		}
		MessageBox(s, II("Firmware version"), MB_OK);			
	}
	else
	{
		MessageBox(II("The firmware version could not be read. Either\n\
			 the build was before Beta 10, or an internal (DEV) build before Dec 19th 2000\n\
			or the chip is not responding"), II("Unknown firmware version"), MB_OK);			
	}
}

void CPSToolDlg::OnFileDump() 
{
	// Display trhe file Save as diaolg
	CFileDialog *fd = new CFileDialog(
			FALSE, 
			II(".psr"), 
			II("dump"),
			OFN_HIDEREADONLY|OFN_LONGNAMES|OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT,
			II("Persistent Store Files|*.psr||"), 
			this
			);
	fd->m_ofn.lpstrTitle = II("Save Persistent Store File As");
	if (fd->DoModal() == IDOK)
	{
        bool cancelled;
        DoFileServerOperation(SERVER_OPERATION_DUMP, cancelled, fd->GetFileName(), (LPCTSTR) "");
        if (!cancelled && m_error_handler->get_last_error_code() != 0)
        {
            MessageBox(m_error_handler->get_last_error_string(), II("Dump failed"), MB_OK | MB_ICONWARNING);
        }
	}
	delete fd;
}

void CPSToolDlg::OnFileMerge() 
{
	// Display dialog for open pst file
	CFileDialog *fd = new CFileDialog(TRUE, II(".psr"), NULL,
								  OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_LONGNAMES|OFN_PATHMUSTEXIST,
								  II("Persistent Store Files|*.psr||"), this);

    fd->m_ofn.nMaxFileTitle = 256;
	fd->m_ofn.lpstrTitle = II("Open Persistent Store File");
	if (fd->DoModal() == IDOK)
	{		
        bool cancelled;
        bool ret = DoFileServerOperation(SERVER_OPERATION_MERGE, cancelled, fd->GetFileName(), (LPCTSTR)"");
        if (!cancelled && m_error_handler->get_last_error_code() != 0)
        {
            MessageBox(m_error_handler->get_last_error_string(), II("Merge failed"), MB_OK | MB_ICONWARNING);
        }
	}
	delete fd;
}

void CPSToolDlg::OnFileRunquery() 
{
	// Display dialog for Query file
	CFileDialog *fqd = new CFileDialog(TRUE, II(".psq"), NULL,
								  OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_LONGNAMES|OFN_PATHMUSTEXIST,
								  II("Persistent Store Queries|*.psq||"), this);
	fqd->m_ofn.lpstrTitle = II("Run PS Query File");
	if (fqd->DoModal() == IDOK)
	{
		// Display dialog for Results file
		CFileDialog *fsd = new CFileDialog(FALSE, II(".psr"), II("store"),
								  OFN_HIDEREADONLY|OFN_LONGNAMES|OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT,
								  II("Persistent Store Files|*.psr||"), this);
		fsd->m_ofn.lpstrTitle = II("Save Persistent Store File As");
		if (fsd->DoModal() == IDOK)
		{
            bool cancelled;
            DoFileServerOperation(SERVER_OPERATION_QUERY, cancelled, fsd->GetPathName(), fqd->GetPathName());
            if (!cancelled && m_error_handler->get_last_error_code() != 0)
            {
                MessageBox(m_error_handler->get_last_error_string(), II("Query failed"), MB_OK | MB_ICONWARNING);
            }
		}
		delete fsd;
	}
	delete fqd;
}

void CPSToolDlg::SetStores(uint16 stores)
{
    if (m_helper->GetServer())
    {
	    m_helper->SetStores(stores);
	    if(m_show_only_set) RefreshKeyList();
	    else ReadKey();
    }
}

void CPSToolDlg::OnStoresAll() 
{
	SetStores(PS_STORES_DEFAULT);
}

void CPSToolDlg::OnStoresImplementation() 
{
	SetStores(PS_STORES_I);
}

void CPSToolDlg::OnStoresRAMonly() 
{
	SetStores(PS_STORES_T);
}

void CPSToolDlg::OnStoresROMonly() 
{
	SetStores(PS_STORES_R);
}

void CPSToolDlg::OnStoresFactoryOnly() 
{
	m_helper->SetStores(PS_STORES_F);
	ReadKey();
}

void CPSToolDlg::OnStoresNotRAM() 
{
	m_helper->SetStores(PS_STORES_IFR);
	ReadKey();
}


void CPSToolDlg::CheckSetStores(CCmdUI* pCmdUI, uint16 required_store)
{
    bool enable = m_helper->GetServer();
    pCmdUI->Enable(enable);
    if (enable)
    {
	    pCmdUI->SetRadio(m_helper->GetStores() ==  required_store);	
    }
}

void CPSToolDlg::OnUpdateStoresAll(CCmdUI* pCmdUI) 
{
    CheckSetStores(pCmdUI, PS_STORES_DEFAULT);
}

void CPSToolDlg::OnUpdateStoresImplementation(CCmdUI* pCmdUI) 
{
    CheckSetStores(pCmdUI, PS_STORES_I);
}

void CPSToolDlg::OnUpdateStoresRAMonly(CCmdUI* pCmdUI) 
{
    CheckSetStores(pCmdUI, PS_STORES_T);
}

void CPSToolDlg::OnUpdateStoresROMonly(CCmdUI* pCmdUI) 
{
    CheckSetStores(pCmdUI, PS_STORES_R);
}

void CPSToolDlg::OnUpdateStoresFactoryOnly(CCmdUI* pCmdUI) 
{
    CheckSetStores(pCmdUI, PS_STORES_F);
}

void CPSToolDlg::OnUpdateStoresNotRAM(CCmdUI* pCmdUI) 
{
    CheckSetStores(pCmdUI, PS_STORES_IFR);
}

void CPSToolDlg::OnBootmodeNone() { SetBootmode(PsServer::NoBootmode()); }
void CPSToolDlg::OnBootmode0() { SetBootmode(0); }
void CPSToolDlg::OnBootmode1() { SetBootmode(1); }
void CPSToolDlg::OnBootmode2() { SetBootmode(2); }
void CPSToolDlg::OnBootmode3() { SetBootmode(3); }
void CPSToolDlg::OnBootmode4() { SetBootmode(4); }
void CPSToolDlg::OnBootmode5() { SetBootmode(5); }
void CPSToolDlg::OnBootmode6() { SetBootmode(6); }
void CPSToolDlg::OnBootmode7() { SetBootmode(7); }

void CPSToolDlg::SetBootmode(uint16 bootmode)
{
	m_helper->SetBootmode(bootmode);
	if(m_show_only_overrides || m_show_only_set) RefreshKeyList();
	ReadKey();
}

void CPSToolDlg::CheckBootMode(CCmdUI* pCmdUI, uint16 required_boot_mode) 
{
    bool enable = m_helper->GetServer();
    pCmdUI->Enable(enable);
    if (enable)
    {
	    pCmdUI->SetRadio(m_helper->GetBootmode() == required_boot_mode);	
    }
}


void CPSToolDlg::OnUpdateBootmodeNone(CCmdUI* pCmdUI) 
{
    CheckBootMode(pCmdUI, PsServer::NoBootmode());
}

void CPSToolDlg::OnUpdateBootmode0(CCmdUI* pCmdUI) 
{
    CheckBootMode(pCmdUI, 0);
}

void CPSToolDlg::OnUpdateBootmode1(CCmdUI* pCmdUI) 
{
    CheckBootMode(pCmdUI, 1);
}

void CPSToolDlg::OnUpdateBootmode2(CCmdUI* pCmdUI) 
{
    CheckBootMode(pCmdUI, 2);
}

void CPSToolDlg::OnUpdateBootmode3(CCmdUI* pCmdUI) 
{
    CheckBootMode(pCmdUI, 3);
}

void CPSToolDlg::OnUpdateBootmode4(CCmdUI* pCmdUI) 
{
    CheckBootMode(pCmdUI, 4);
}

void CPSToolDlg::OnUpdateBootmode5(CCmdUI* pCmdUI) 
{
    CheckBootMode(pCmdUI, 5);
}

void CPSToolDlg::OnUpdateBootmode6(CCmdUI* pCmdUI) 
{
    CheckBootMode(pCmdUI, 6);
}

void CPSToolDlg::OnUpdateBootmode7(CCmdUI* pCmdUI) 
{
    CheckBootMode(pCmdUI, 7);
}

void CPSToolDlg::OnUpdateBootmodeShowOnlyOverrides(CCmdUI* pCmdUI)
{
    bool enable = m_helper->GetServer();
    pCmdUI->Enable(enable);
    if (enable)
    {
    	pCmdUI->SetRadio(m_show_only_overrides);
    }
}

void CPSToolDlg::OnBootmodeShowOnlyOverrides()
{
  	m_show_only_overrides = !m_show_only_overrides;
   RefreshKeyList();
}

// This abomination is to overcome the "by design" bug in Q242577 
void CPSToolDlg::OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu)
{
    ASSERT(pPopupMenu != NULL);
    // Check the enabled state of various menu items.

    CCmdUI state;
    state.m_pMenu = pPopupMenu;
    ASSERT(state.m_pOther == NULL);
    ASSERT(state.m_pParentMenu == NULL);

    // Determine if menu is popup in top-level menu and set m_pOther to
    // it if so (m_pParentMenu == NULL indicates that it is secondary popup).
    HMENU hParentMenu;
    if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
        state.m_pParentMenu = pPopupMenu;    // Parent == child for tracking popup.
    else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
    {
        CWnd* pParent = this;
           // Child windows don't have menus--need to go to the top!
        if (pParent != NULL &&
           (hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
        {
           int nIndexMax = ::GetMenuItemCount(hParentMenu);
           for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
           {
            if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
            {
                // When popup is found, m_pParentMenu is containing menu.
                state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
                break;
            }
           }
        }
    }

    state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
    for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
      state.m_nIndex++)
    {
        state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
        if (state.m_nID == 0)
           continue; // Menu separator or invalid cmd - ignore it.

        ASSERT(state.m_pOther == NULL);
        ASSERT(state.m_pMenu != NULL);
        if (state.m_nID == (UINT)-1)
        {
           // Possibly a popup menu, route to first item of that popup.
           state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
           if (state.m_pSubMenu == NULL ||
            (state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
            state.m_nID == (UINT)-1)
           {
            continue;       // First item of popup can't be routed to.
           }
           state.DoUpdate(this, TRUE);   // Popups are never auto disabled.
        }
        else
        {
           // Normal menu item.
           // Auto enable/disable if frame window has m_bAutoMenuEnable
           // set and command is _not_ a system command.
           state.m_pSubMenu = NULL;
           state.DoUpdate(this, FALSE);
        }

        // Adjust for menu deletions and additions.
        UINT nCount = pPopupMenu->GetMenuItemCount();
        if (nCount < state.m_nIndexMax)
        {
           state.m_nIndex -= (state.m_nIndexMax - nCount);
           while (state.m_nIndex < nCount &&
            pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
           {
            state.m_nIndex++;
           }
        }
        state.m_nIndexMax = nCount;
    }
} 

//==================================================================
// Set enable/disable state of "delete" button
//==================================================================
afx_msg void CPSToolDlg::OnUpdateDeleteButton(CCmdUI* pCmdUI)
{
    pCmdUI->Enable((IsConnected()) && (m_entry && m_entry->base_key == 0) && (m_present));
}

//==================================================================
// Set enable/disable state of "set" button
//==================================================================
afx_msg void CPSToolDlg::OnUpdateSetButton(CCmdUI* pCmdUI)
{
    pCmdUI->Enable((IsConnected()) && (m_present || m_attempt_edit));
}

//==================================================================
// Set enable/disable state of "read" button
//==================================================================
afx_msg void CPSToolDlg::OnUpdateReadButton(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(IsConnected());
}

//==================================================================
// Set enable/disable state of "resetclose" button
//==================================================================
afx_msg void CPSToolDlg::OnUpdateResetCloseButton(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(IsConnected());
}

//==================================================================
// Set enable/disable state of "reset" button
//==================================================================
afx_msg void CPSToolDlg::OnUpdateResetButton(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(IsConnected());
}

//==================================================================
// Set enable/disable state of "merge" 
//==================================================================
afx_msg void CPSToolDlg::OnUpdateFileMerge(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(IsConnected());
}

//==================================================================
// Set enable/disable state of "dump" 
//==================================================================
afx_msg void CPSToolDlg::OnUpdateFileDump(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(IsConnected());
}

//==================================================================
// Set enable/disable state of "query" 
//==================================================================
afx_msg void CPSToolDlg::OnUpdateFileQuery(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(IsConnected());
}

//==================================================================
// Set enable/disable state of "firmware id" 
//==================================================================
afx_msg void CPSToolDlg::OnUpdateFirmwareId(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(IsConnected());
}

//==================================================================
// Set enable/disable state of "edit raw" 
//==================================================================
afx_msg void CPSToolDlg::OnUpdateEditRaw(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(IsConnected());
}

//==================================================================
// Set enable/disable state of "factory set" 
//==================================================================
afx_msg void CPSToolDlg::OnUpdateFactorySet(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(IsConnected());
}

//==================================================================
// Set enable/disable state of "factory restore" 
//==================================================================
afx_msg void CPSToolDlg::OnUpdateFactoryRestore(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(IsConnected());
}

//==================================================================
// Set enable/disable state of "factory restore all" 
//==================================================================
afx_msg void CPSToolDlg::OnUpdateFactoryRestoreAll(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(IsConnected());
}

//==================================================================
// Set enable/disable state of "bootmode deleete override" 
//==================================================================
afx_msg void CPSToolDlg::OnUpdateBootmodeDeleteKey(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(IsConnected());
}

//==================================================================
// "true" if connected successfully
//==================================================================
bool CPSToolDlg::IsConnected()
{
    return (m_helper->GetServer() != NULL);
}
//==================================================================
// Indicates that the dialog is idle. Used to call UpdateUI handlers
// for buttons
//==================================================================
LRESULT CPSToolDlg::OnKickIdle(WPARAM wp, LPARAM lCount)
{
     UpdateDialogControls(this, TRUE);
     return FALSE;
}

//==================================================================
// Combine this indication with a global flag to detect the first
// time the dialog is displayed
//==================================================================
void CPSToolDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
    CDialog::OnWindowPosChanged(lpwndpos);

    // send message to ourselves if this is the first display of dialog
    if ((lpwndpos->flags & SWP_SHOWWINDOW) && m_first_dlg_display) 
    {
        PostMessage(WM_DIALOG_DISPLAYED, 0, 0);
        m_first_dlg_display = false;
    }
}

//==================================================================
// First time dialog has been displayed. Force a "connect"
//==================================================================
afx_msg LRESULT CPSToolDlg::OnDialogDisplayed(WPARAM wParam, LPARAM lParam)
{
    m_initial_connect = true;

    // Do an initial "connect" - which is the same as a "reconnect"
    DoConnect();

    return TRUE;
}

//==================================================================
// Setup the progress bar with anything always required
//==================================================================
void CPSToolDlg::CommonProgressBarSetup(CProgressWnd &progressWnd, bool enable_cancel)
{
    progressWnd.Create(this, II("Waiting for chip..."), enable_cancel);
    progressWnd.SetRange(0, PROGRESS_BAR_MAX);
    progressWnd.GoModal();
    progressWnd.SetWindowSize(1);
}

//==================================================================
// Get the server thread ready to run
//==================================================================
void CPSToolDlg::PrepareServerThread(CServerThread &thread)
{
    thread.SetHelper(m_helper);
    thread.Start();
}

//====================================================================
// do a server operation that doesn't take any parameters e.g. connect
//====================================================================
bool CPSToolDlg::DoBasicServerOperation(TServerOperationEnum operation, bool &cancelled)
{
    CServerThread thread(operation, m_helper);
    PrepareServerThread(thread);
    return RunOperation(thread, operation, cancelled);
}

//====================================================================
// do a server operation that takes one or two filenames e.g. merge
//====================================================================
bool CPSToolDlg::DoFileServerOperation(TServerOperationEnum operation, bool &cancelled, LPCTSTR filename1, LPCTSTR filename2)
{
    CServerThread thread(operation, m_helper);
    thread.SetFilename1(filename1);
    thread.SetFilename2(filename2);
    PrepareServerThread(thread);
    return RunOperation(thread, operation, cancelled);
}

//====================================================================
// Show and update a progress bar either for the operation itself or
// for the cancel
//====================================================================
bool CPSToolDlg::CommonRunProgressBar(CServerThread &thread, bool enable_cancel, LPCTSTR text)
{
    CProgressWnd progress_wnd;
    bool cancelled = false;

    CommonProgressBarSetup(progress_wnd, enable_cancel);
    progress_wnd.SetText(text);

    // Loop until thread completes or progress bar is cancelled
    while (!progress_wnd.Cancelled() && thread.IsActive())
    {
        if (progress_wnd)
        {
            progress_wnd.PeekAndPump();
            progress_wnd.StepIt();
            // Reset when full
            if (progress_wnd.GetPos() >= PROGRESS_BAR_MAX)
            {
                progress_wnd.SetPos(0);
            }
            if (progress_wnd.Cancelled())
            {
                break;
            }
        }
    }
    cancelled = progress_wnd.Cancelled();

    if (!cancelled)
    {
        // make sure progress bar is full
        int pos = progress_wnd.GetPos();
        while (pos < PROGRESS_BAR_MAX)
        {
            progress_wnd.StepIt();
            pos++;
        }
    }
    return cancelled;
}

//====================================================================
// Run an operation in the background with a progress bar
//====================================================================
bool CPSToolDlg::RunOperation(CServerThread &thread, TServerOperationEnum operation, bool &cancelled)
{
    bool ret = false;

    // no "cancel" on a merge
    bool enable_cancel = (operation != SERVER_OPERATION_MERGE);

    m_helper->GetServer()->clear_cancel();

    cancelled = CommonRunProgressBar(thread, enable_cancel, II(""));
    if (cancelled)
    {
        // Show another progress bar while cancelling is taking place
        CWaitCursor wait_cursor;
        m_helper->GetServer()->do_cancel();
        (void) CommonRunProgressBar(thread, false, II("Cancelling"));

    }
    ret = (!cancelled) && (thread.SuccessfulOperation());
    return ret;
}

//====================================================================
// User has pressed "enter" on keyboard
//====================================================================
void CPSToolDlg::OnOK()
{
    // If the user presses "enter" this function is called (no default button). 
    // This should be ignored - the dialog shouldn't exit B-67960
}

