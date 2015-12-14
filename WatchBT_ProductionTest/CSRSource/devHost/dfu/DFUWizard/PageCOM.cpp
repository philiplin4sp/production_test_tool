///////////////////////////////////////////////////////////////////////
//
//	File	: PageCOM.cpp
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: PageCOM
//
//	Purpose	: DFU wizard page to allow COM port settings to be
//			  selected. The following member variables contain the
//			  selection:
//				valueCOMPort	- Name of COM port.
//				valueCOMAuto	- Should the baud rate and protocol
//								  be detected automatically.
//				valueBaud		- Baud rate to use if not automatic.
//				valueProtocol	- Protocol to use if not automatic.
//				valueLink		- Link establishment mode for BCSP.
//				valueTunnel		- Should tunnelling be used via BCSP.
//
//	Modification history:
//	
//		1.1		18:Sep:00	at	File created.
//		1.2		29:Sep:00	at	Added tunnelling option for BCSP, and
//								ensured that controls are correctly
//								enabled or disabled initially.
//		1.3		24:Oct:00	at	Corrected debugging macros.
//		1.4		26:Oct:00	at	Only baud rates appropriate to the
//								selected port and offered.
//		1.5		27:Oct:00	at	Moved initial COM port enumeration to
//								a separate page.
//		1.6		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.7		24:Nov:00	at	Baud rate list only updated if COM
//								port changed.
//		1.8		08:Dec:00	at	Added link establishment modes.
//		1.9		23:Mar:01	at	Added directory to DFUEngine include.
//		1.10	17:May:01	at	Added automatic behaviour.
//		1.11	25:May:01	at	Improved automatic behaviour.
//		1.12	19:Feb:02	at	Transport settings hidden if auto.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard/PageCOM.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUWizard.h"
#include "PageCOM.h"
#include "dfu\DFUEngine.h"

// Visual C++ debugging code
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(PageCOM, DFUPage)

// Constructor
PageCOM::PageCOM() : DFUPage(PageCOM::IDD)
{
	//{{AFX_DATA_INIT(PageCOM)
	valueCOMPort = _T("");
	valueCOMAuto = 0;
	valueBaud = _T("");
	valueProtocol = -1;
	valueTunnel = FALSE;
	valueLink = -1;
	//}}AFX_DATA_INIT
}

// Data exchange
void PageCOM::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PageCOM)
	DDX_Control(pDX, IDC_PAGE_COM_LINK_STATIC, staticLink);
	DDX_Control(pDX, IDC_PAGE_COM_DISABLED_RADIO, radioDisabled);
	DDX_Control(pDX, IDC_PAGE_COM_ENABLED_RADIO, radioEnabled);
	DDX_Control(pDX, IDC_PAGE_COM_PASSIVE_RADIO, radioPassive);
	DDX_Control(pDX, IDC_PAGE_COM_TUNNEL_CHECK, buttonTunnel);
	DDX_Control(pDX, IDC_PAGE_COM_COMPORT_COMBO, comboCOMPort);
	DDX_CBString(pDX, IDC_PAGE_COM_COMPORT_COMBO, valueCOMPort);
	DDX_Check(pDX, IDC_PAGE_COM_COMAUTO_CHECK, valueCOMAuto);
	DDX_Control(pDX, IDC_PAGE_COM_BAUD_STATIC, staticBaud);
	DDX_Control(pDX, IDC_PAGE_COM_BAUD_COMBO, comboBaud);
	DDX_CBString(pDX, IDC_PAGE_COM_BAUD_COMBO, valueBaud);
	DDX_Control(pDX, IDC_PAGE_COM_PROTOCOL_STATIC, staticProtocol);
	DDX_Control(pDX, IDC_PAGE_COM_BCSP_RADIO, radioBCSP);
	DDX_Control(pDX, IDC_PAGE_COM_H4_RADIO, radioH4);
	DDX_Radio(pDX, IDC_PAGE_COM_BCSP_RADIO, valueProtocol);
	DDX_Check(pDX, IDC_PAGE_COM_TUNNEL_CHECK, valueTunnel);
	DDX_Control(pDX, IDC_PAGE_COM_TITLE, staticTitle);
	DDX_Radio(pDX, IDC_PAGE_COM_DISABLED_RADIO, valueLink);
	//}}AFX_DATA_MAP
}

// Message map
BEGIN_MESSAGE_MAP(PageCOM, DFUPage)
	//{{AFX_MSG_MAP(PageCOM)
	ON_CBN_SELCHANGE(IDC_PAGE_COM_COMPORT_COMBO, OnPortChanged)
	ON_BN_CLICKED(IDC_PAGE_COM_COMAUTO_CHECK, OnPageChanged)
	ON_BN_CLICKED(IDC_PAGE_COM_BCSP_RADIO, OnPageChanged)
	ON_BN_CLICKED(IDC_PAGE_COM_H4_RADIO, OnPageChanged)
	ON_CBN_SELCHANGE(IDC_PAGE_COM_BAUD_COMBO, OnPageChanged)
	ON_BN_CLICKED(IDC_PAGE_COM_DISABLED_RADIO, OnPageChanged)
	ON_BN_CLICKED(IDC_PAGE_COM_ENABLED_RADIO, OnPageChanged)
	ON_BN_CLICKED(IDC_PAGE_COM_PASSIVE_RADIO, OnPageChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Initialise the page when displayed
BOOL PageCOM::OnSetActive()
{
	// Build the list of COM ports
	comboCOMPort.ResetContent();
	for (POSITION port = listPorts.GetHeadPosition(); port;)
	{
		comboCOMPort.AddString(listPorts.GetNext(port));
	}

	// Modify the current selection if appropriate
	valueCOMPort = FindUnique(listPorts, valueCOMPort, !GetSheet()->automatic);

	// Pretent that the currently selected baud rate is available
	comboBaud.ResetContent();
	comboBaud.AddString(valueBaud);

	// Ensure that the controls reflect the current selection
	UpdateData(false);

	// Enable the appropriate controls
	OnPortChanged();
	
	// Perform any other initialisation required
	return DFUPage::OnSetActive();
}

// Delayed initialisation
void PageCOM::OnSetActiveDelayed()
{
	// Perform any other initialisation required
	DFUPage::OnSetActiveDelayed();

	// Advance to the next page is automatic behaviour requested
	if (GetSheet()->automatic) SetNext();
}

// Update the controls on COM port change
void PageCOM::OnPortChanged()
{
	// Validate the controls
	UpdateData();

	// Update the list of baud rates
	comboBaud.ResetContent();
	CStringListX listBaudRates;
	DFUEngine::GetCOMBaudRates(listBaudRates, valueCOMPort);
	for (CStringListX::POSITION baud = listBaudRates.GetHeadPosition(); baud;)
	{
		comboBaud.AddString(listBaudRates.GetNext(baud));
	}
	if (listBaudRates.GetCount() == 1) valueBaud = listBaudRates.GetHead();

	// Ensure that the controls reflect the current selection
	UpdateData(false);

	// Update the controls
	OnPageChanged();
}

// Update the controls on any change
void PageCOM::OnPageChanged()
{
	// Validate the controls
	UpdateData();

	// Enable the appropriate controls
	staticBaud.ShowWindow(!valueCOMAuto);
	comboBaud.ShowWindow(!valueCOMAuto);
	staticProtocol.ShowWindow(!valueCOMAuto);
	radioBCSP.ShowWindow(!valueCOMAuto);
	radioH4.ShowWindow(!valueCOMAuto);
	staticLink.ShowWindow(!valueCOMAuto);
	staticLink.EnableWindow(valueProtocol == bcsp);
	radioDisabled.ShowWindow(!valueCOMAuto);
	radioDisabled.EnableWindow(valueProtocol == bcsp);
	radioEnabled.ShowWindow(!valueCOMAuto);
	radioEnabled.EnableWindow(valueProtocol == bcsp);
	radioPassive.ShowWindow(!valueCOMAuto);
	radioPassive.EnableWindow(valueProtocol == bcsp);
	buttonTunnel.ShowWindow(!valueCOMAuto);
	buttonTunnel.EnableWindow(valueProtocol == bcsp);

	// Update the buttons
	SetButtons();
}

// Button management
LRESULT PageCOM::GetPageBack()
{
	return IDD_PAGE_CONNECTION;
}

LRESULT PageCOM::GetPageNext()
{
	return !valueCOMPort.IsEmpty()
	       && (valueCOMAuto
		       || (!valueBaud.IsEmpty() && (0 <= valueProtocol)
			       && ((valueProtocol != bcsp) || (0 <= valueLink))))
	       ? IDD_PAGE_ACTION : disable;
}
