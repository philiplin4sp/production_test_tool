// BtCliCtrlDlg.cpp : implementation file

#include "stdafx.h"
#include "BtCliCtrl.h"
#include "BtCliCtrlDlg.h"
#include "thread/system_wide_mutex.h"
#include "unicode/ichar.h"
#include "versioninfo/VersionInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define APP_NAME II("BtCliCtrl")

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

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	VersionInfo<istring> versionInfo;
	SetDlgItemText(IDC_ABOUT_TITLE, versionInfo.GetProductName().c_str());
	CString build = versionInfo.GetFileVersion().c_str();
	build += CString(II(" "));
	build += versionInfo.GetSpecialBuild().c_str();
	SetDlgItemText(IDC_ABOUT_BUILD, build);
	SetDlgItemText(IDC_ABOUT_COPYRIGHT, versionInfo.GetLegalCopyright().c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBtCliCtrlDlg dialog

CBtCliCtrlDlg::CBtCliCtrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBtCliCtrlDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBtCliCtrlDlg)
	m_Times = FALSE;
	m_Log = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBtCliCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBtCliCtrlDlg)
	DDX_Control(pDX, IDC_COMBO_TRANSPORT, m_Transport);
	DDX_Control(pDX, IDC_COMBO_COMPORT, m_Comport);
	DDX_Control(pDX, IDC_COMBO_BAUDRATE, m_Baudrate);
	DDX_Check(pDX, IDC_CHECK_TIMES, m_Times);
	DDX_Check(pDX, IDC_CHECK_LOG, m_Log);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBtCliCtrlDlg, CDialog)
	//{{AFX_MSG_MAP(CBtCliCtrlDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO_TRANSPORT, OnSelchangeComboTransport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBtCliCtrlDlg message handlers

BOOL CBtCliCtrlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

    // Read in the previous settings.
    m_Transport.SetCurSel (AfxGetApp ()->GetProfileInt (APP_NAME, II("Transport"), 0));
	OnSelchangeComboTransport();
    m_Comport.SetCurSel   (AfxGetApp ()->GetProfileInt (APP_NAME, II("Comport"),   0));
    m_Baudrate.SetCurSel  (AfxGetApp ()->GetProfileInt (APP_NAME, II("Baudrate"),  0));

    m_Log   = AfxGetApp ()->GetProfileInt (APP_NAME, II("Log"),  0) == 1 ? true : false;
    m_Times = AfxGetApp ()->GetProfileInt (APP_NAME, II("Times"),0) == 1 ? true : false;

    UpdateData (false);	
	
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBtCliCtrlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBtCliCtrlDlg::OnPaint() 
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
HCURSOR CBtCliCtrlDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CBtCliCtrlDlg::OnOK() 
{
    // Hide us till the command is done, then quit.
    ShowWindow (SW_HIDE);

    CString cmd;
    CString bRate, cPort, transport;

    // Get latest data.
    UpdateData ();

    m_Transport.GetLBText (m_Transport.GetCurSel (), transport);

    int baudrate_selection = m_Baudrate.GetCurSel();
    if ((baudrate_selection != -1) && (m_Baudrate.IsWindowEnabled()))
    {
        m_Baudrate.GetLBText  (baudrate_selection, bRate);
    }

    int comport_selection = m_Comport.GetCurSel();
    if (comport_selection != -1)
    {
        m_Comport.GetLBText  (comport_selection, cPort);
    }

    // Set the previous settings.
    AfxGetApp ()->WriteProfileInt (APP_NAME, II("Transport"), m_Transport.GetCurSel ());
    AfxGetApp ()->WriteProfileInt (APP_NAME, II("Comport"),   m_Comport.GetCurSel   ());
    AfxGetApp ()->WriteProfileInt (APP_NAME, II("Baudrate"),  m_Baudrate.GetCurSel  ());
    AfxGetApp ()->WriteProfileInt (APP_NAME, II("Log"), m_Log     ? 1 : 0);
    AfxGetApp ()->WriteProfileInt (APP_NAME, II("Times"), m_Times ? 1 : 0);
        
    // Setup the btcli command.
    cmd.Format (II("btcli.exe %s %s %s %s %s"),
                transport,
                bRate,
                cPort,
                m_Log   ? II("-l") : II(""),
                m_Times ? II("-t") : II(""));

    // Call the command
	PROCESS_INFORMATION pinf;
	ZeroMemory(&pinf, sizeof(pinf));
	STARTUPINFO stinf = {
		sizeof(STARTUPINFO),	// cb; 
		NULL,					// lpReserved; 
		NULL,					// lpDesktop; 
		II("BTCLI"),			// lpTitle;  
		0,						// dwX; 
		0,						// dwY; 
		0,						// dwXSize; 
		0,						// dwYSize; 
		0,						// dwXCountChars; 
		0,	 					// dwYCountChars; 
		0,						// dwFillAttribute; 
		STARTF_USESHOWWINDOW,	// dwFlags; 
		SW_SHOWDEFAULT,			// wShowWindow; 
		0,						// cbReserved2; 
		0,						// lpReserved2; 
		0,						// hStdInput; 
		0,						// hStdOutput;  
		0 						// hStdError; 
	};


	BOOL process_created = CreateProcess(
		NULL,				// name of executable module
		cmd.LockBuffer(), 	// command line string
		NULL, 				// SD
		NULL, 				// TSD
		FALSE,				// handle inheritance option
		CREATE_NEW_CONSOLE,	// creation flags
		NULL, 				// new environment block
		NULL, 				// current directory name
		&stinf,				// startup information
		&pinf				// process information
	);
	cmd.UnlockBuffer();

    if (!process_created)
	{
		LPVOID lpMsgBuf;
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
		);
		// Display the string.
		MessageBox( (LPCTSTR)lpMsgBuf, II("Error"), MB_OK | MB_ICONINFORMATION );
		// Free the buffer.
		LocalFree( lpMsgBuf );
	}

    // Were done so quit.
	CDialog::OnOK();
}

//**********************************************
//Enable/Disable "OK"
//**********************************************
void CBtCliCtrlDlg::SetOkButtonState(BOOL state)
{
    CButton *ok = (CButton *)GetDlgItem(IDOK);
    ok->EnableWindow(state);
}

//**********************************************
// Enable/disable drop down and set selection
//**********************************************
void CBtCliCtrlDlg::SetDropDownSelection()
{
    // Use previous selection or select first in list
    int comportSel = m_Comport.GetCurSel ();
    int selection = (comportSel == -1) ? 0 : comportSel;
    m_Comport.SetCurSel (selection);

    int baudSel = m_Baudrate.GetCurSel ();
    selection = (baudSel == -1) ? 0 : baudSel;
    m_Baudrate.SetCurSel (selection);

    if (m_Comport.GetCount() == 0)
    {
        m_Comport.AddString(II("<No ports available>"));
        m_Comport.SetCurSel (0);
        m_Comport.EnableWindow(FALSE);

        // dont allow to continue if no ports available
        SetOkButtonState(FALSE);
    }
}

//**********************************************
// Populate baud rate drop down
//**********************************************
void CBtCliCtrlDlg::EnableBaudRate()
{
    m_Baudrate.EnableWindow(TRUE);
    m_Baudrate.AddString(II("9600"));
    m_Baudrate.AddString(II("19200"));
    m_Baudrate.AddString(II("38400"));
    m_Baudrate.AddString(II("57600"));
    m_Baudrate.AddString(II("115200"));
    m_Baudrate.AddString(II("230400"));
    m_Baudrate.AddString(II("460800"));
    m_Baudrate.AddString(II("921600"));
    m_Baudrate.AddString(II("1382400"));

}

//**********************************************
// Clear and disable baud rate selection
//**********************************************
void CBtCliCtrlDlg::DisableBaudRate()
{
	m_Baudrate.ResetContent ();
    m_Baudrate.EnableWindow(FALSE);
}


//**********************************************
// User has selected different transport option
//**********************************************
void CBtCliCtrlDlg::OnSelchangeComboTransport() 
{
    int curSel = m_Comport.GetCurSel ();

	ichar devname[100];
	HANDLE test;
	int c;

	m_Comport.ResetContent ();

    SetOkButtonState(TRUE); // Enable "OK" until it is seen if any ports are available

    CString str;
    int index = m_Transport.GetCurSel();
    int len = m_Transport.GetLBTextLen(index);
    m_Transport.GetLBText( index, str.GetBuffer(len) );
    str.ReleaseBuffer();

    m_Comport.EnableWindow(TRUE);

    if (str == "USB")
    {
        DisableBaudRate();

		for ( c = 0 ; c < 20 ; c++ )
		{
			isprintf(devname, 100, II("\\\\.\\CSR%d"), c);

			if ( (test = ::CreateFile(devname,GENERIC_READ | GENERIC_WRITE,
											0,0,OPEN_EXISTING,0,0) ) != INVALID_HANDLE_VALUE )
			{
				m_Comport.AddString(devname);
				::CloseHandle(test);
			}
		}
		// Add Widcomm USB devices to USBport combo box
		for ( c = 0 ; c < 20 ; c++ )
		{
			isprintf(devname, 100, II("\\\\.\\BTWUSB-%d"), c);
			SystemWideMutex m(&devname[4], true);
			if (m.IsAcquired())
			{
				if ( (test = ::CreateFile(devname,GENERIC_READ | GENERIC_WRITE,
										0,0,OPEN_EXISTING,0,0) ) != INVALID_HANDLE_VALUE )
				{
					m_Comport.AddString(devname);
					::CloseHandle(test);
				}
			}
		}
	}
    else if (str == "SDIO")
    {
        // No com port or baud
        DisableBaudRate();

		for ( c = 0 ; c < 4 ; c++ )
		{
			isprintf(devname, 100, II("\\\\.\\UniRawsdioTypeA%d"), c);
			if ( (test = ::CreateFile(devname,GENERIC_READ | GENERIC_WRITE,
											0,0,OPEN_EXISTING,0,0) ) != INVALID_HANDLE_VALUE )
			{
                // btcli expects "sdio<num>", not full device name
                ichar tmpStr[10];
                isprintf(tmpStr, 10, II("sdio%d"), c);
				m_Comport.AddString(tmpStr);
				::CloseHandle(test);
			}
        }
    }
    else
    {
        EnableBaudRate();

		for ( c = 1 ; c < 21 ; c++ )
		{
			isprintf(devname, 100, II("COM%d"), c);

			if ( (test = ::CreateFile(devname,GENERIC_READ | GENERIC_WRITE,
											0,0,OPEN_EXISTING,0,0) ) != INVALID_HANDLE_VALUE )
			{
				m_Comport.AddString(devname);
				::CloseHandle(test);
			}
		}
    }
    SetDropDownSelection();
}
