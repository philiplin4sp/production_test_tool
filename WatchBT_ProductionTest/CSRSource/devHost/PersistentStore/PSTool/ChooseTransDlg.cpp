// ChooseTransDlg.cpp : implementation file
//

/*
 MODIFICATION HISTORY
		1.1   12:feb:01  Chris Lowe 	Working
		1.6   21:feb:01  Chris Lowe 	Added registry read/write (not working
		1.8   23:feb:01  Chris Lowe 	Added Firmware version
		1.10   28:mar:01  Chris Lowe 	#inc's changed for Result dir
		1.11   02:may:01  Chris Lowe 	Added 14k4 baud
		1.12   24:sep:01  Chris Lowe 	Added support for Widcomm USB device driver.
		1.15   09:oct:01  Mark Marshall Added a command line option to select the multi-SPI
										device.  The BAUD combo changes into a port selection
										combo when the option is given, and an LPT transport is chosen

		$Id: //depot/bc/bluesuite_2_4/devHost/PersistentStore/PSTool/ChooseTransDlg.cpp#1 $
*/


#include "stdafx.h"
#include "PSTool.h"
#include "ChooseTransDlg.h"
#include "unicode/ichar.h"
#include "csrhci/bccmd_spi.h"
#include "csrhci/bccmd_hci.h"
#include "csrhci/bccmd_trans.h"
#include "csrhci/bluecontroller.h"
#include "csrhci/transportconfiguration.h"
#include "ps/psserver.h"
#include "ps/pscache.h"
#include "thread/system_wide_mutex.h"
#include "pttransport/pttransport.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChooseTransDlg dialog



const ichar * const RegSectSpiBCCMD = II("SPI BCCMD Configuration");
const ichar * const RegSectBCSP		= II("BCSP Configuration");
const ichar * const RegSectH4		= II("H4 Configuration");
const ichar * const RegSectH5		= II("H5 Configuration");
const ichar * const RegSectUSB		= II("USB Configuration");
const ichar * const RegSectMRU		= II("MRU");

const ichar * const RegEntryBaud	= II("Data Rate for Device");
const ichar * const RegEntryMRU		= II("PSTool");
const ichar * const RegEntryPort	= II("Name of Device");
const ichar * const RegEntryMultiSPI	= II("Multi SPI Port");
const ichar * const RegEntryStopSpi = II("Stop on SPI Access");

// This maps from enum transChoice
const ichar *const RegSects[] = { 
	RegSectSpiBCCMD, 
	RegSectBCSP, 
	RegSectH4, 
	RegSectH5, 
	RegSectUSB
};


struct BaudPair
{
	const ichar *name;
	int rate;
};

const int cBaudRates = 10;
const BaudPair BaudRates[cBaudRates] =
{
	{II("9.6 kbaud"), 9600}, 
	{II("14.4 kbaud"), 14400},
	{II("19.2 kbaud"), 19200},
	{II("38.4 kbaud"), 38400},
	{II("57.6 kbaud"), 57600},
	{II("115.2 kbaud"), 115200},
	{II("230.4 kbaud"), 230400},
	{II("460.8 kbaud"), 460800},
	{II("921.6 kbaud"), 921600},
	{II("1382.4 kbaud"), 1382400}
};

CChooseTransDlg::CChooseTransDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChooseTransDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChooseTransDlg)
	m_radio_val = -1;
	m_use_cache = false;
	//}}AFX_DATA_INIT
	m_radio_val = AfxGetApp()->GetProfileInt(RegSectMRU, RegEntryMRU, -1);
    devs = NULL;
    devcount = 0;
}

CChooseTransDlg::~CChooseTransDlg()
{
    pttrans_enumerate_free(devs, devcount);
}
	

void CChooseTransDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChooseTransDlg)
	DDX_Control(pDX, IDC_CB_PORT, m_cb_port);
	DDX_Control(pDX, IDC_CB_BAUD, m_cb_baud);
	DDX_Control(pDX, IDC_RAD_SPI_BCCMD, m_rad_control);
	DDX_Radio(pDX, IDC_RAD_SPI_BCCMD, m_radio_val);
	DDX_Check(pDX, IDC_CACHE, m_use_cache);
	//}}AFX_DATA_MAP
	// Even though we set the radio cal, the corresponding function doesn't get called.  MFC's a bit crap.
	if (pDX->m_bSaveAndValidate == false)
	{
		switch (m_radio_val)
		{
		case 0: 
			OnRadSpiBccmd();
			break;
		case 1: 
			OnRadBcsp();
			break;
		case 2: 
			OnRadH4();
			break;
		case 3: 
			OnRadH5();
			break;
		case 4: 
			OnRadUsb();
			break;
		}
	}
}


BEGIN_MESSAGE_MAP(CChooseTransDlg, CDialog)
	//{{AFX_MSG_MAP(CChooseTransDlg)
	ON_BN_CLICKED(IDC_RAD_BCSP, OnRadBcsp)
	ON_BN_CLICKED(IDC_RAD_H4, OnRadH4)
	ON_BN_CLICKED(IDC_RAD_H5, OnRadH5)
	ON_BN_CLICKED(IDC_RAD_SPI_BCCMD, OnRadSpiBccmd)
	ON_BN_CLICKED(IDC_RAD_USB, OnRadUsb)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

	

CChooseTransDlg::transChoice CChooseTransDlg::GetTrans()
{	
	return m_trans;
}


/////////////////////////////////////////////////////////////////////////////
// CChooseTransDlg message handlers

void CChooseTransDlg::OnRadBcsp() 
{
	// Enable baud rates, select port to be comm port.
	m_trans = TransBCSP;
	SetPortBehaviour(portComm);
}

void CChooseTransDlg::OnRadH4() 
{
	// Enable baud rates, select port to be comm port.
	m_trans = TransH4;
	SetPortBehaviour(portComm);
}

void CChooseTransDlg::OnRadH5() 
{
	// Enable baud rates, select port to be comm port.
	m_trans = TransH5;
	SetPortBehaviour(portComm);
}

void CChooseTransDlg::OnRadSpiBccmd() 
{
	// Disable baud rates, select port to be LPT port.
	m_trans = TransSpiBCCMD;
	SetPortBehaviour(portLpt);
}

void CChooseTransDlg::OnRadUsb() 
{
	// Disable baud rates, Disable port.
	m_trans = TransUSB;
	SetPortBehaviour(portUSB);
}


// This function also reads values from the registry.
void CChooseTransDlg::SetPortBehaviour(CChooseTransDlg::portBehaviour behaviour)
{
	CWinApp *app = AfxGetApp();
	m_behaviour = behaviour;

	if (m_behaviour == portDisable)
	{
		m_cb_port.EnableWindow(false);
		EnableBaud(false);
		return;
	}
	else
		m_cb_port.EnableWindow(true);

	if (m_behaviour == portComm)
	{	
		int i;
		m_cb_port.ResetContent();
		for (int p = 1; p <= LAST_COMM_PORT; p++)
		{
			ichar buffer[10];
			isprintf(buffer, 10, II("COM%d"), p);
			m_cb_port.SetItemData(m_cb_port.AddString(buffer), p);
		}
		i = m_cb_port.FindString(-1, app->GetProfileString(RegSects[GetTrans()], RegEntryPort, II("COM1"))) ;
		if (i == CB_ERR)
			i = 0;
		m_cb_port.SetCurSel(i);
		EnableBaud(true);
		m_cb_baud.ResetContent();
		for (i = 0; i < cBaudRates; i++)
		{
			m_cb_baud.SetItemData(m_cb_baud.AddString(BaudRates[i].name), BaudRates[i].rate);
		}
		GetDlgItem(IDC_BAUD_STATIC)->CWnd::SetWindowText(II("B&aud"));

		DWORD userBaudRate = app->GetProfileInt(RegSects[GetTrans()], RegEntryBaud, 38400);
		for (i = 0; i < cBaudRates; i++) 
		{
			if( m_cb_baud.GetItemData(i) == userBaudRate)
				break;
		}
		if (i == cBaudRates)
			m_cb_baud.SetCurSel(2);
		else
			m_cb_baud.SetCurSel(i);
			
	}
	else if (m_behaviour == portLpt)
	{ 
		m_cb_port.ResetContent();

        pttrans_enumerate_free(devs, devcount);
        
        int dev, index;
        pttrans_enumerate(NULL, &devs, &devcount);

        for (dev = 0; dev < devcount; dev++)
        {
            index = m_cb_port.AddString((CString)devs[dev].name);
            m_cb_port.SetItemData(index, (DWORD)&devs[dev]);
        }

        // Find if we've saved what was chosen previously in the registry.
        CString profileStr = app->GetProfileString(RegSects[GetTrans()], RegEntryPort, II("LPT1"));
		m_cb_port.SetCurSel(m_cb_port.FindString(-1, profileStr) );

		if(m_bUseMultiSpi)
		{
		    EnableBaud(true);
		    m_cb_baud.ResetContent();
		    for (int i = 0; i < 16; i++)
		    {
				CString str;
				str.Format(II("SPI %d"), i);
				m_cb_baud.AddString(str);
		    }

		    DWORD multiSpiNum = app->GetProfileInt(RegSects[GetTrans()], RegEntryMultiSPI, 0);
		    if(multiSpiNum >= 0 && multiSpiNum < 16)
			m_cb_baud.SetCurSel(multiSpiNum);
		    else
			m_cb_baud.SetCurSel(0);

		    GetDlgItem(IDC_BAUD_STATIC)->CWnd::SetWindowText(II("&Multi Spi"));
		}
		else
		{
		    EnableBaud(false);
		}
	} 
	else  if (m_behaviour == portUSB)
	{
		m_cb_port.ResetContent();
		ichar portname[100];
		for(unsigned long dev = bccmd_hci::USBDeviceIterate();
			bccmd_hci::USBDeviceName(dev, portname, sizeof(portname)/sizeof(*portname)) != 0;
			dev = bccmd_hci::USBDeviceIterate(dev))
		{
			m_cb_port.AddString(portname);
		}
		// m_cb_port.SetCurSel(0);
		m_cb_port.SetCurSel(0);
		EnableBaud(false);
	}
}


void CChooseTransDlg::EnableBaud(bool enable)
{
	m_cb_baud.EnableWindow(enable);
}


CString CChooseTransDlg::GetPortString()
{
	if (m_port_str[0] != II('\\'))
		return II("\\\\.\\") + m_port_str;
	else
		return m_port_str;
}


int CChooseTransDlg::GetPortNum()
{
	return m_port_num;
}


int CChooseTransDlg::GetBaud()
{
	return m_baud;
}


PsServer *CChooseTransDlg::GetNewTransport(bccmd_trans_error_handler *handler)
{
    pttrans_device *dev;
    const char *transType = NULL;
	bccmd_trans *trans;
	// Create a server based on the transport.
	switch(GetTrans())
	{
	case TransSpiBCCMD:
        // Find if the user selected a USB or LPT port.
        dev = (pttrans_device*)GetPortNum();
		trans = new bccmd_spi(handler, dev->trans, NULL, GetBaud());
		break;
	case TransH4:				
	case TransH5:				
	case TransBCSP:
	case TransUSB:
		{
			TransportConfiguration * bctrans;
			UARTConfiguration UConf ( GetPortString().LockBuffer() , GetBaud() , BCSP_parity , BCSP_stopbits , true , BCSP_readtimeout );
			switch (GetTrans())
			{
				case TransBCSP:
					bctrans = new BCSPConfiguration( UConf );
					break;
				case TransH4:
					bctrans = new H4Configuration( GetPortString().LockBuffer()  , GetBaud() );
					break;
				case TransH5:
					bctrans = new H5Configuration( GetPortString().LockBuffer()  , GetBaud() );
					break;
				case TransUSB:
					bctrans = new H2Configuration(USBConfiguration(m_port_str.LockBuffer()));
					break;
				default:
					exit(-1);
			}

			trans = new bccmd_hci(handler, bctrans);
			break;
		}
	}
	PsCache *psc = new PsCache;
	psc->SetServer(new PsBccmd(trans));
	psc->delete_child_on_destruct(true);
	psc->UseCache(m_use_cache);
	return psc;
}


void CChooseTransDlg::WriteRegistry()
{
	CWinApp *app = AfxGetApp();
	switch(GetTrans())
	{
	case TransSpiBCCMD:
		app->WriteProfileString(RegSectSpiBCCMD, RegEntryPort, m_port_str);
		if(m_bUseMultiSpi)
		    app->WriteProfileInt(RegSectSpiBCCMD, RegEntryMultiSPI, m_baud);
		break;
	case TransBCSP:
		app->WriteProfileString(RegSectBCSP, RegEntryPort, m_port_str);
		app->WriteProfileInt(RegSectBCSP, RegEntryBaud, m_baud);
		break;
	case TransH4:				
		app->WriteProfileString(RegSectH4, RegEntryPort, m_port_str);
		app->WriteProfileInt(RegSectH4, RegEntryBaud, m_baud);
		break;
	case TransH5:				
		app->WriteProfileString(RegSectH5, RegEntryPort, m_port_str);
		app->WriteProfileInt(RegSectH5, RegEntryBaud, m_baud);
		break;
	case TransUSB:
		app->WriteProfileString(RegSectUSB, RegEntryPort, m_port_str);
		break;
	}
	// Write the MRU settings.
	app->WriteProfileInt(RegSectMRU, RegEntryMRU, (int)GetTrans());
}


void CChooseTransDlg::OnOK() 
{
	int sel;
	UpdateData(true);
	// Get Transport
	if (m_radio_val == -1)
	{
		MessageBox(II("You need to select a transport to continue."), II("No transport selected"), MB_OK);
		return;
	}
	m_trans = (transChoice)(m_radio_val + TransSpiBCCMD);
	// Get the port
	if ((m_behaviour == portComm) ||  (m_behaviour == portLpt) ||  (m_behaviour == portUSB))
	{
		sel = m_cb_port.GetCurSel();
		if (sel == -1)
		{
			MessageBox(II("You need to select a port for this transport"), II("No port selected"), MB_OK);
			return;
		}
		m_cb_port.GetLBText(m_cb_port.GetCurSel(), m_port_str);
		m_port_num = m_cb_port.GetItemData(m_cb_port.GetCurSel());
		if (m_behaviour == portComm)
		{
			// get the baud rate
			sel = m_cb_baud.GetCurSel();
			if (sel == -1)
			{
				MessageBox(II("You need to select a baud rate for this port"), II("No baud rate selected"), MB_OK);
				return;
			}
			m_baud = m_cb_baud.GetItemData(sel);
		}
		else if (m_behaviour == portLpt)
		{
			m_baud = -1;
			if(m_bUseMultiSpi)
			{
				// get the MultSpi port number
				m_baud = m_cb_baud.GetCurSel();
				if (m_baud == -1)
				{
					MessageBox(II("Please specify a port from the Multi-SPI device."), II("No Multi-SPI port selected"), MB_OK);
					return;
				}
			}
		}
	}
	WriteRegistry();
	CDialog::OnOK();  // Let the dialog close.
}

bool CChooseTransDlg::m_bUseMultiSpi = false;

// This is only used to set the multi spi config at the moment.
// It could be expanded to allow more options later?
void CChooseTransDlg::CommandLineOptions(PSCmdLineInfo& cli)
{
    m_bUseMultiSpi = cli.m_bUseMultiSpi;
}

