// PSToolDlg.H : 

/*
MODIFICATION HISTORY
		1.14   16:jan:01  Chris Lowe 	Introduced PsBufSize
		1.17   23:feb:01  Chris Lowe 	Added Firmware version
		1.19   28:mar:01  Chris Lowe 	#inc's changed for Result dir
		#9     18:mar:04  sjw         B-671: Can now apply canned key operations.
        #10    23:mar:04  sjw      B-802: User now has to press button to edit/add a key that is not present

RCS IDENTIFIER
  $Id: //depot/bc/bluesuite_2_4/devHost/PersistentStore/PSTool/PSToolDlg.h#1 $

*/
//
//{{AFX_INCLUDES()
#include "msflexgrid.h"
//}}AFX_INCLUDES

#if !defined(AFX_PSTOOLDLG_H__30EA23CC_A6B1_11D4_8FE6_00B0D065A91C__INCLUDED_)
#define AFX_PSTOOLDLG_H__30EA23CC_A6B1_11D4_8FE6_00B0D065A91C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ps/pshelp.h"
#include "ProgressWnd.h"
#include "ServerThread.h"

const int MAX_DESCRIPTION_LENGT = 10000;
const int PROGRESS_BAR_MAX = 20000;

/////////////////////////////////////////////////////////////////////////////
// CPSToolDlg dialog

class CPSToolDlg : public CDialog
{
private:

	enum enable_keys {
		ENABLE_KEYS_NONE, 
		ENABLE_KEYS_ENUM,
		ENABLE_KEYS_VAL, 
		ENABLE_KEYS_STRING,
		ENABLE_KEYS_POWER_TABLE,
		ENABLE_KEYS_BD_ADDR
	} ;

	enum enhanced_power_table_field_posns {
		EPT_BASIC_INT_PA = 0,
		EPT_BASIC_EXT_PA = 1,
		EPT_BASIC_TX_PA_ATTN = 2,
		EPT_BASIC_CONTROL_BIT = 3,
		EPT_ENH_INT_PA = 4,
		EPT_ENH_EXT_PA = 5,
		EPT_ENH_CLASS = 6,
		EPT_ENH_TX_PA_ATTN = 7,
		EPT_TX_PRE = 8,
		EPT_OUTPUT_DBM = 9,
        EPT_NO_BR = 10,
        EPT_NUM_FIELDS
	};

   	enum bluetooth_power_table_v0_field_posns {
		PTV0_BASIC_INT_PA,
		PTV0_BASIC_TX_MIX_LEVEL,
		PTV0_BASIC_TX_MIX_OFFSET,
		PTV0_BASIC_TX_PA_ATTN,
		PTV0_ENH_INT_PA,
		PTV0_ENH_TX_MIX_LEVEL,
		PTV0_ENH_TX_MIX_OFFSET,
		PTV0_ENH_CLASS,
		PTV0_ENH_TX_PA_ATTN,
		PTV0_OUTPUT_DBM,
        PTV0_NO_BR,
        PTV0_NUM_FIELDS
	};

	enum {
		PsBufSize = 256,
		PsDefaultArrayLength = 8
	};

	enum edit_format_type {  // represented by formatStrings
		edit_format_none,
		edit_format_decimal,
		edit_format_hex,
		edit_format_hex_words,
		edit_format_text,
		edit_format_enum,
		edit_format_bd_addr
	};
	CPSToolApp *m_app;
// Construction
public:
	CPSToolDlg(CWnd* pParent = NULL, CPSToolApp *app = NULL);	// standard constructor
    ~CPSToolDlg();
	CPSHelper *GetHelper();
	void SetHelper(CPSHelper *a_helper);

// Dialog Data
	//{{AFX_DATA(CPSToolDlg)
	enum { IDD = IDD_PSTOOL_DIALOG };
	CButton	m_but_set_key;
	CButton	m_but_notpresent;
	CButton	m_but_del_key;
	CStatic	m_static_uap;
	CStatic	m_static_nap;
	CStatic	m_static_lap;
	CEdit	m_ed_uap;
	CEdit	m_ed_nap;
	CEdit	m_ed_lap;
	CEdit	m_ed_filter;
	CStatic	m_st_edit_format;
	CButton	m_but_ps_edit;
	CButton	m_but_ps_insert;
	CButton	m_but_ps_delete;
	CStatic	m_key_label;
	CStatic	m_caption_present;
	CSpinButtonCtrl	m_spin_val;
	CListBox	m_list_keys;
	CEdit	m_ed_val;
	CEdit	m_ed_str;
	CComboBox	m_combo_enum;
	CMSFlexGrid	m_grid_ps;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPSToolDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPSHelper *m_helper;
	bool m_show_only_overrides;
	bool m_show_only_set;
	const pskey_entry *m_entry;
	uint16 m_raw_len;
	bool m_present;
	HICON m_hIcon;
	CPowerTable m_power_table;
	CEdit m_ed_grid_ps;
    bccmd_trans_error_handler *m_error_handler;
    
    bool m_initial_connect;
    bool m_first_dlg_display;
	bool m_changed;
	bool m_attempt_edit;
    bool CheckToSaveCommon(int keys);
	bool CheckToSave();
	bool CheckToSaveOnExit();
    void DoConnect();

	enum {SORT_UNSORTED, SORT_BY_NAME, SORT_BY_ORDINAL} m_sort_method;
	enum {SHOW_FRIENDLY, SHOW_ID} m_show_method;
	
    HACCEL m_hAccel;                     // dialog accelerators
   

	// Generated message map functions
	//{{AFX_MSG(CPSToolDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeListKeys();
	afx_msg void OnSet();
	afx_msg void OnRead();
	afx_msg void OnButClose();
	afx_msg void OnButResetClose();
	afx_msg void OnButPsInsert();
	afx_msg void OnButPsDelete();
	afx_msg void OnButPsEdit();
	afx_msg void OnDescribe();
	afx_msg void OnButReset();
	afx_msg void OnSelchangeComboEnum();
	afx_msg void OnChangeEdStr();
	afx_msg void OnChangeEdVal();
	afx_msg void OnChangeEdUap();
	afx_msg void OnChangeEdNap();
	afx_msg void OnChangeEdLap();
	afx_msg void OnChangeEdFilter();
	afx_msg void OnClose();
	afx_msg void OnButDelKey();
	afx_msg void OnFileExit();
	afx_msg void OnFactoryFactoryrestore();
	afx_msg void OnFactoryFactoryrestoreall();
	afx_msg void OnFactoryFactoryset();
	afx_msg void OnHelpAbout();
	afx_msg void OnCanned(UINT nID);
	afx_msg void OnViewFriendlynames();
	afx_msg void OnViewProgrammerids();
	afx_msg void OnViewSortbyname();
	afx_msg void OnViewSortbyordinal();
	afx_msg void OnViewShowOnlySet();
	afx_msg void OnUpdateDescribe(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewFriendlyNames(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewProgrammerIds(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewSortByName(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewSortByOrdinal(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewShowOnlySet(CCmdUI* pCmdUI);
	afx_msg void OnViewUnsorted();
	afx_msg void OnUpdateViewUnsorted(CCmdUI* pCmdUI);
	afx_msg void OnKeyEditraw();
	afx_msg void OnButReconnect();
	afx_msg void OnHelpFirmware();
	afx_msg void OnFileDump();
	afx_msg void OnFileMerge();
	afx_msg void OnFileRunquery();
	afx_msg void OnStoresAll();
	afx_msg void OnStoresImplementation();
	afx_msg void OnStoresRAMonly();
	afx_msg void OnStoresROMonly();
	afx_msg void OnStoresFactoryOnly();
	afx_msg void OnStoresNotRAM();
	afx_msg void OnUpdateStoresAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStoresImplementation(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStoresRAMonly(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStoresROMonly(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStoresFactoryOnly(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStoresNotRAM(CCmdUI* pCmdUI);
	afx_msg void OnButNotPresent();
	afx_msg void OnBootmodeNone();
	afx_msg void OnBootmode0();
	afx_msg void OnBootmode1();
	afx_msg void OnBootmode2();
	afx_msg void OnBootmode3();
	afx_msg void OnBootmode4();
	afx_msg void OnBootmode5();
	afx_msg void OnBootmode6();
	afx_msg void OnBootmode7();
	afx_msg void OnUpdateBootmodeNone(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBootmode0(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBootmode1(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBootmode2(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBootmode3(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBootmode4(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBootmode5(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBootmode6(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBootmode7(CCmdUI* pCmdUI);
	afx_msg void OnBootmodeKeyDelete();
	afx_msg void OnBootmodeShowOnlyOverrides();
	afx_msg void OnUpdateBootmodeShowOnlyOverrides(CCmdUI* pCmdUI);
    afx_msg void OnUpdateDeleteButton(CCmdUI* pCmdUI);
    afx_msg void OnUpdateSetButton(CCmdUI* pCmdUI);
    afx_msg void OnUpdateReadButton(CCmdUI* pCmdUI);
    afx_msg void OnUpdateResetCloseButton(CCmdUI* pCmdUI);
    afx_msg void OnUpdateResetButton(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFileMerge(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFileQuery(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFileDump(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFirmwareId(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditRaw(CCmdUI* pCmdUI);
    afx_msg void OnUpdateBootmodeDeleteKey(CCmdUI* pCmdUI);
    afx_msg void OnUpdateBootmodeDeleteOverride(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFactorySet(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFactoryRestore(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFactoryRestoreAll(CCmdUI* pCmdUI);
    afx_msg LRESULT OnDialogDisplayed(WPARAM wParam, LPARAM lParam);

    DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu);

	void SetBootmode(uint16 bootmode);
	void SetStores(uint16 stores);
	bool SetKey(bool &proceed);
	void ReadKey();
	bool DoSet();
	void RefreshKeyList(void);
	void EnableControls(enable_keys keys);
	bool SetRawData(const uint16 key, const uint16 len, bool &proceed);
	void DisplayRawData(const uint16 *data, const uint16 len);
	void DisplayWarningIfNotPresent(bool present);
	void SetFormat(edit_format_type formatType);

    void PrepareServerThread(CServerThread &thread);
    bool DoBasicServerOperation(TServerOperationEnum operation, bool &cancelled);
    bool DoFileServerOperation(TServerOperationEnum operation, bool &cancelled, LPCTSTR filename1, LPCTSTR filename2);
    bool RunOperation(CServerThread &thread, TServerOperationEnum operation, bool &cancelled);
    void CommonProgressBarSetup(CProgressWnd &progressWnd, bool enable_cancel);
    bool CommonRunProgressBar(CServerThread &thread, bool enable_cancel, LPCTSTR text);

    int CommonHandleRetry(ichar *title, bool success);
    int ResetCommon();
    void Enable_Menus_Reconnect(bool successful_reconnect);
    void CheckBootMode(CCmdUI* pCmdUI, uint16 required_boot_mode);
    void CheckSetStores(CCmdUI* pCmdUI, uint16 required_store);

    LRESULT OnKickIdle(WPARAM wp, LPARAM lCount);
    bool IsConnected();

private:
#ifndef _WINCE
	bool SetPowerTableKey();
	bool SetEnhPowerTableKey();
	void ReadPowerTableKey();
	void ReadEnhPowerTableKey();
    void AutofitColumns();
#endif
    template <typename T> T KeyMaximum();
    template <typename T> T KeyMinimum();
	template <typename T> bool SetIntegerKey();
	template <typename T> bool StringToInteger(T &);
	template <typename T> void ReadIntegerKey();
	template <typename T> bool ReadIntegerValue(T &t);
public:
    afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
protected:
    virtual void OnOK();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PSTOOLDLG_H__30EA23CC_A6B1_11D4_8FE6_00B0D065A91C__INCLUDED_)
