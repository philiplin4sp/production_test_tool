/*
MODIFICATION HISTORY
		1.1   04:feb:00  Chris Lowe 	Working
		1.6   28:mar:01  Chris Lowe 	#inc's changed for Result dir
		1.8   09:oct:01  Mark Marshall	Added a command line option to select the multi-SPI
										device.  The BAUD combo changes into a port selection
										combo when the option is given, and an LPT transport is chosen

RCS IDENTIFIER
  $Id: //depot/bc/bluesuite_2_4/devHost/PersistentStore/PSTool/ChooseTransDlg.h#1 $

*/


	#if !defined(AFX_CHOOSETRANSDLG_H__15A31FCA_F8F7_11D4_9032_00B0D065A91C__INCLUDED_)
#define AFX_CHOOSETRANSDLG_H__15A31FCA_F8F7_11D4_9032_00B0D065A91C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChooseTransDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChooseTransDlg dialog


#include "PSCmdLineInfo.h"
#include "unicode/ichar.h"
#include "ps/psserver.h"
#include "pttransport/pttransport.h"



class CChooseTransDlg : public CDialog
{
public:
enum transChoice {
	TransNone = -1,
	TransSpiBCCMD = 0,
	TransBCSP,
	TransH4,
	TransH5,
	TransUSB
};

enum portBehaviour {
    portComm,
	portLpt,
	portUSB,
	portDisable
};

enum {
	MAX_USB_DEVICES = 20,
	LAST_COMM_PORT  = 50,
	LAST_LPT_PORT	= 20
};




// Construction
public:
	CChooseTransDlg(CWnd* pParent = NULL);   // standard constructor
	~CChooseTransDlg();
	transChoice GetTrans();
	void WriteRegistry();

	PsServer *GetNewTransport(bccmd_trans_error_handler *handler);

	void SetTrans(const transChoice trans);
	void SetPortBehaviour(portBehaviour behaviour);
	void EnableBaud(bool enable);
	CString GetPortString();
	void SetPortString(const CString port);
	int GetPortNum();
	int GetBaud();
	void SetBaud(int baud);

	void CommandLineOptions(PSCmdLineInfo& cli);

// Dialog Data
	//{{AFX_DATA(CChooseTransDlg)
	enum { IDD = IDD_CHOOSETRANS };
	CButton	m_ck_spi_stop;
	CComboBox	m_cb_port;
	CComboBox	m_cb_baud;
	CButton	m_rad_control;
	int		m_radio_val;
	int		m_use_cache;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChooseTransDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChooseTransDlg)
	afx_msg void OnRadBcsp();
	afx_msg void OnRadH4();
	afx_msg void OnRadH5();
	afx_msg void OnRadSpiBccmd();
	afx_msg void OnRadUsb();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
    pttrans_device *devs;
    int devcount;
	portBehaviour m_behaviour;
	transChoice m_trans;
	int m_baud;
	CString m_port_str;
	int m_port_num;

	// set from the command line
	static bool m_bUseMultiSpi;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHOOSETRANSDLG_H__15A31FCA_F8F7_11D4_9032_00B0D065A91C__INCLUDED_)
