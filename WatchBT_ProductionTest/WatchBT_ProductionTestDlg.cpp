// WatchBT_ProductionTestDlg.cpp : implementation file
//

using System;
#include "stdafx.h"
#include "WatchBT_ProductionTest.h"
#include "WatchBT_ProductionTestDlg.h"
#include "ExportFunction.h"
#include "TestEngine.h"
#include "portaudio.h"
#include "usb_hid_reports.h"
#include <fstream>
#include <string>
#include <String>
#include "windows.h"
#include <iostream>
#include <vector>
/* Seavia 20150630 */
#include <cstdio>
#include <cstdlib>
#include <string.h>
/* End Seavia*/
/* John 20150824 */
#include <sstream>

#include "ExtractConfig.h"

#using <mscorlib.dll>
using namespace std;
using namespace System;
using namespace System::Globalization;
//extern "C"
//{
	#include <fftw3.h>
//}

#include <math.h>
#pragma warning(disable: 4793) // methods are compiled as native (clr warning)

extern "C" {
#include "hidsdi.h"
#include <setupapi.h>
}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define NFIX_CRASH
#define fft_write_file 1
//#define EASY_SPI_LOCK
//#define EASY_FACTORY_ENABLE
//#define debug_mode 1
//#define display_time 1
//#define test_mode
//#define sleeptime 20000
//#define NO_MM_UNIT
//#define NO_GPIO_BOARD
//#define update_color 1 
//#define test_code


/*
 * Method definition
 */
int WriteBluetoothAddress();
int WriteBlutoothAddress_And_SerialNumber();
int ReadBlueToothAddrsFile(string FileName);
unsigned long getAvailBluetoothAddr(int &success_flag);
void AddBlueToothAddrToAssignedBlueToothAddrsFile(unsigned long addr);
int WriteAssignedBlueToothAddrsFile();
int OpenLogFile(string name);
void WriteMainLogFile(const char * format, ...);
void CloseMainLogFile();
int checkRefEndPointConnection(int connection_type);
void CsrDevicesConnect();
int ReadConfigFile(char *file_name);
int WriteConfigFile(char *file_name);
int BKPrecisionMultimeter();
int CreateDirectory(CString newPath);
int CreateLogsDirectories();
int ReadSerialNumberFile(string FileName);
int WriteSerialNumberFile(string FileName);
int ReadPairingInfoFile(string FileName);
int ReadPskey(uint16 size, uint16 psKey, uint16 *data);
void check_available_ports();
int automaticPairingModeEnable(int connection_type, int warmresetFlag);
int automaticPairingModeDisable(int connection_type);
void displayInit(void);
void I2c_SendOrBufferCommand(uint32 value, uint8 length, uint8 startStopCont);
int checkLockStatus(uint32 csr_SPIDeviceHandle);
//int usbEnable();
uint8 write_bAddPlusLicenseKey(uint16 *bAddress, uint16 *licenseKey);
uint8 Write_Serial_Number(uint16 *bAddress);
int PCBTEST_accelerometerCheck();
int PCBTEST_serialFlashCheck();
int PCBTEST_spiPortLock();
int oledTest();
int bccmdDisable();
uint8 pairingInfoClear(void);
uint8 pairingInfoWrite(uint16 *data, uint16 length);


#if 1 /*Seavia 20150810 import model list from config file*/
int ReadModelFile(char *file_name);
int model_list;
int ResetConfigFile(char *modelName, char *fileName);
int ResetSNPrefix(char *strptr);
#endif

double GetConfigurationValue(string config_name);
TESTENGINE_API(int32) spSetPio32(uint32 handle, uint32 mask, uint32 direction, uint32 value, uint32* errLines);
TESTENGINE_API(int32) spGetPio32(uint32 handle, uint32* direction, uint32* value);
TESTENGINE_API(int32) spMapPio32(uint32 handle, uint32 mask, uint32 pios, uint32* errLines);
TESTENGINE_API(int32) spSetPio(uint32 handle, uint16 mask, uint16 port);
TESTENGINE_API(int32) spLedEnable(uint32 handle, uint16 led, uint16 state);
TESTENGINE_API(int32) spradiotestTxstart(uint32 handle, uint16 frequency, uint16 intPA, uint16 extPA, int16 modulation);
TESTENGINE_API(int32) spradiotestCfgPkt(uint32 handle, uint16 type, uint16 size);
TESTENGINE_API(int32) spradiotestCfgFreq(uint32 handle, uint16 txrxinterval, uint16 loopback, uint16 report);
TESTENGINE_API(int32) spradiotestRxdata1(uint32 handle, uint16 frequency, uint8 hiside, uint16 rx_attenuation);
TESTENGINE_API(int32) spradiotestPcmToneIf(uint32 handle, uint16 freq, uint16 ampl, uint16 dc, uint16 pcm_if);
TESTENGINE_API(int32) spbccmdI2CTransfer(uint32 handle, uint16 slaveAddr, uint16 txOctets, uint16 rxOctets, uint8 restart, uint8 data[], uint16 *octets);
TESTENGINE_API(int32) sppsWrite(uint32 handle, uint16 psKey, uint16 store, uint16 length, const uint16 data[]);
TESTENGINE_API(int32) sppsRead(uint32 handle, uint16 psKey, uint16 store, uint16 arrayLen, uint16 data[], uint16 * len);
TESTENGINE_API(int32) spradiotestPause(uint32 handle);

int ReadTestTypeSelectionFile();
int WriteTestTypeSelectionFile();
int ReadModelSelectionFile();
int WriteModelSelectionFile();

int VerifyDutHandle(uint32 handle);
int UsbGpioModuleInit();
int UsbMultimeterModuleInit();
int MultimeterInit(int measurement_type);
int checkDUTConnection(int connection_type);
int checkRefEndPointConnection(int connection_type);
int checkTransmitModuleConnection(int connection_type);





/*
 * Glabal variables
 */

char *product_test_routines_names[NUM_OF_PRODUCT_TEST_ROUTINES] = {
		"1) Initialization", 
		"2) RF TX Power",
		"3) RF RX Sen", 
		"4) Speaker", 
		/*"OLED", 
		"5) Peak current",*/
		"5) Microphone",
		"6) Factory Enable"
}; //ToDO Check for bccmd disable method

char *pcb_test_routines_names[NUM_OF_PCB_TEST_ROUTINES] =	{
		"1) Initialization", 
		"2) OLED",
		"3) Power Supply",
		"4) Crystal Trim", 
		"5) RF TX Power",
		"6) RF RX Sen", 
		"7) Speaker Mic", 
		"8) Accelerometer",
		"9) Push buttons",
		"10) Peak current", 
		"11) Standby current",
		"12) Serial Flash", // Magnetometer is removed from the spec
		//"Charger current",
		// "SPI Port Locking"
};


char *fftresultsStrings[16] =	{
		"", 
		"1k",
		"2k",
		"1k, 2k", 
		"3k",
		"1k, 3k", 
		"2k, 3k", 
		"1k, 2k, 3k",
		"Other", 
		"1k, Other",
		"2k, Other",
		"1k, 2k, Other", 
		"3k, Other",
		"1k, 3k, Other", 
		"2k, 3k, Other", 
		"1k, 2k, 3k, Other",
};
int g_BT_address_runout = 0;

vector <long> assignedBluetoothAddrList;
uint16 bAddress[BLUETOOTH_ADDRESS_LENGTH]; 
/*John 8/21/2015: use bAddress2 to append to log files*/
/*John 8/24/2015: use update_bool to determine if bAddress2 needs to be updated */
uint16 bAddress2[BLUETOOTH_ADDRESS_LENGTH];
bool update_bool = true;
unsigned long testSelection = 0;
uint32 serialNumber = 0, availSerialNumber = 0, startingAvailSerialNumber=0, endingAvailSerialNumber=0;
unsigned long availBluetoothAddrLap=0x0;
unsigned long availBluetoothAddrNap=0, availBluetoothAddrUap=0;
unsigned long startingAvailBluetoothAddrLap=0, endingAvailBluetoothAddrLap=0;
uint16 pairing_info_data[100], pairing_info_data_size=0;
int testType = PCB_TEST, tempTestType = PCB_TEST;
int pcb_tests_results[NUM_OF_PCB_TEST_ROUTINES];
int product_tests_results[NUM_OF_PRODUCT_TEST_ROUTINES];
char serialNumberArray[100];
int updateSerialNumberTextBox = 0;
CString currentBluetoothAddrDisplayStr;
CButton *productTestCheckBoxes[NUM_OF_PRODUCT_TEST_ROUTINES];
CStatic *productTestResultsLabels[NUM_OF_PRODUCT_TEST_ROUTINES];
CStatic *productTestResultsDetailsLabels[NUM_OF_PRODUCT_TEST_ROUTINES];
int productTestCheckBoxesIDs[NUM_OF_PRODUCT_TEST_ROUTINES], productTestResultsLabelsIDs[NUM_OF_PRODUCT_TEST_ROUTINES], productTestResultsDetailsLabelsIDs[NUM_OF_PRODUCT_TEST_ROUTINES];

CButton *pcbTestCheckBoxes[NUM_OF_PCB_TEST_ROUTINES];
CStatic *pcbTestResultsLabels[NUM_OF_PCB_TEST_ROUTINES];
CStatic *pcbTestResultsDetailsLabels[NUM_OF_PCB_TEST_ROUTINES];
int pcbTestCheckBoxesIDs[NUM_OF_PCB_TEST_ROUTINES], pcbTestResultsLabelsIDs[NUM_OF_PCB_TEST_ROUTINES], pcbTestResultsDetailsLabelsIDs[NUM_OF_PCB_TEST_ROUTINES];
uint8 cmdBuffer[64];
uint8 currentCmdBufferIdx, commandBufferComplete;
int timestampflag=1, newlineflag=1; 
HANDLE DeviceHandle;
int EnumerateDevices(wchar_t *device_path);
HANDLE OpenDevice(void);
char device_path_multibytes[sizeof(wchar_t)/sizeof(char)*MAX_PATH];

char TestStatusString[500];


char currentBluetoothAddressString[100], currentLogFileName[100];
CString currentavailBluetoothAddrStr;
int model_selection=0; 

#if 1 /*Seavia 20150810 import model list from config file*/
unsigned int NUM_OF_MODELS; // Seavia : read model list from models.txt, change NUM_OF_MODELS from #define to be a variable
char modelsStrings[MAX_OF_MODELS][15];
char *configFileNames[2][CONFIG_MODEL_QTY] = {
		{"..\\config\\config-pcba.txt", "..\\config\\config-pcba.txt", "..\\config\\config-pcba.txt", "..\\config\\config-pcba.txt", "..\\config\\config-pcba.txt"},
		{"..\\config\\config-notifier.txt", "..\\config\\config-pvc.txt", "..\\config\\config-svc.txt", "..\\config\\config-gvc41.txt", "..\\config\\config-gvc45.txt"}
};
#else
char *modelsStrings[NUM_OF_MODELS] = {"PVC", "SVC", "GVC"};
char *configFileNames[2][NUM_OF_MODELS] = {
												{"..\\config\\config-pcba.txt", "..\\config\\config-pcba.txt", "..\\config\\config-pcba.txt"},
												{"..\\config\\config-pvc.txt", "..\\config\\config-svc.txt", "..\\config\\config-gvc.txt"}
										  };

#endif

#if 1 /* Seavia 20150903, Add Project Information, store in PSKEY_USR34 */
CString projectInformationStr = "PRJ_INFO: ";;
int selectedModel;
char *projectModelInfo[MODEL_INFO_QTY] = {
		"MVS01-", 
		"MNR01-",
		"MVS(R)02-", 
		"GVS02-"
}; 
#endif
uint16 psKey15Value[6] = {0x9030, 0xac00, 0x15e5, 0x25a2, 0x4a00, 0xa80a};



/*typedef struct {
  
  char       modelName[20];
  char       fileName[50];

} configFileType;

configFileType configFiles[2][NUM_OF_MODELS];*/

char *test_typesStrings[2] = {"PCB", "PRODUCT"};




#if 1 /* Seavia, 20150612 add Serial number prefix string.
Serial number definition (20 characters): Model name (10 characters) + Factory Indicator (1 character) + 
                           Production Year (2 digits) + Production Month (1 Hex) + Sequential number ( 6 digits)
Serial number prefix (14 characters): Model name (10 characters) + Factory Indicator (1 character) + 
					   Production Year (2 digits) + Production Month (1 Hex) 

1. Read Serial number prefix (10 char) from models file
2. Serial number prefix + date information (year : 2 char + month : 1 char) + 6 hex bluetooth address = serial number

*/

char   snPrefix[MAX_PREFIX+1]; // include '\0'
uint16 snPrefix_u16[MAX_PREFIX_U16];
uint16 snPrefixLen;
uint16 snPrefixLen_u16;
string sn_prefix_string; // John Testing
char   spSerialNumber[MAX_PREFIX+4+1]; // include serialnum & '\0'
uint16 spSerialNumber_u16[MAX_PREFIX_U16+6]; //spSerialNumber = snPrefix + availableSerialNumber
string factoryIndicator;

/*Seavia, len : char array length */
void convert2U16Arr(uint16 *dst, char *src, int len) {
	uint16 first_char;
	if((len % 2) == 0) {
		for(int i = 0; i < len; i+=2) {
			first_char = (uint16)(src[i]);
			dst[i/2] = (first_char << 8) | (uint16)(src[i+1]);
		}
	}
	else {
		dst[0] = (uint16)(src[0]);
		for(int i = 1; i < len; i+=2) {
			first_char = (uint16)(src[i]);
			dst[(i+1)/2] = (first_char << 8) | (uint16)(src[i+1]);
		}
	}
}

/*Seavia, len : char array length */
void convert2CharArr(char *dst, uint16 *src, int len) {
	int index = 0;
	string recover = string("");
	if((len% 2) == 1) {
		recover += (char)(src[0]);
		index++;
	}
	for(int i = index; i < (len+1)/2; i++) {
		recover += (char)(src[i] >> 8);
		recover += (char)(src[i] & 0xFF);
	}
	dst = (char *)recover.c_str();
}

/*Seavia : convert current year / month to char array. Ex. 2015 Oct = > 15A  */
void getDateMonth(char dst[4]) {
	int year;
	time_t t = time(0);
	struct tm *now = localtime(&t);
	year = (now->tm_year) - 100;
	dst[1] = "0123456789"[year % 10];
	year /= 10;
	dst[0] = "0123456789"[year];
	dst[2] = "0123456789ABCDEF"[(now->tm_mon)+1];
	dst[3] = '\0';

	//WriteMainLogFile ("getDateMonth => year : %d, month : %d", (now->tm_year) - 100, now->tm_mon+1);
}

#endif /* Seavia 20150612 end*/



void SetStatus (const char * format, ... )
{
	va_list argptr; 
	memset(TestStatusString, 0, sizeof(TestStatusString));
    va_start(argptr, format);
    vsnprintf(TestStatusString, sizeof(TestStatusString)-1, format, argptr);
    va_end(argptr);

	//WriteMainLogFile(/*format*/TestStatusString);
	WriteLogFile(/*format*/TestStatusString);
}

int PeriphiralOpen(int perphiralList, int connection_type)
{
#ifdef PERIPHERAL_CONNECTION

	if(perphiralList & PERIPHIRAL_ID_GPIOBOARD)
	{
#ifndef NO_GPIO_BOARD 
		if(UsbGpioModuleInit() == 0)
		{
			SetStatus("Fail To Open GPIO");
			return 0;
		}
#endif
	}

	if(perphiralList & PERIPHIRAL_ID_MULTIMETER)
	{
#ifndef NO_MM_UNIT 
		if(UsbMultimeterModuleInit() == 0)
		{
			SetStatus("Fail To Open MultiMeter");
			return 0;
		}

		if(MultimeterInit(CURRENT_MEASUREMENT) == 0)
		{
			SetStatus("Fail To Initialize MultiMeter");
			return 0;
		}
#endif
	}

	if(perphiralList & PERIPHIRAL_ID_DUT)
	{
		if(checkDUTConnection(/*SPI_CONNECTION*/connection_type) == 0)
		{
			SetStatus("Fail To open DUT");
			return 0;
		}
	}

	if(perphiralList & PERIPHIRAL_ID_REFENDPOINT)
	{
		if(checkRefEndPointConnection(USB_CONNECTION) == 0)
		{
			SetStatus("Fail To open Reference EP");
			return 0;
		}
	}

	if(perphiralList & PERIPHIRAL_ID_TRANSMIT_MODULE)
	{
		if(checkTransmitModuleConnection(USB_CONNECTION) == 0)
		{
			SetStatus("Fail To open SP Transmit Module");
			return 0;
		}
	}
#endif
    return 1;
}
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CWatchBT_ProductionTestDlg dialog




CWatchBT_ProductionTestDlg::CWatchBT_ProductionTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWatchBT_ProductionTestDlg::IDD, pParent)
	, fwVer(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	brush = new CBrush(RGB( 240,240,240));
}

void CWatchBT_ProductionTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FWVER, fwVer);
}

BEGIN_MESSAGE_MAP(CWatchBT_ProductionTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
	//ON_BN_CLICKED(IDOK, &CWatchBT_ProductionTestDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_RestorePcbTest, &CWatchBT_ProductionTestDlg::OnBnClickedRestore)
	
	
//	ON_STN_CLICKED(IDC_STATIC_PCB_TEST_RESULT, &CWatchBT_ProductionTestDlg::OnStnClickedStaticPcbTestResult)

	ON_BN_CLICKED(IDC_BUTTON_StartPcbTest, &CWatchBT_ProductionTestDlg::OnBnClickedStartPcbTest)
	ON_BN_CLICKED(IDC_BUTTON_StartProductTest, &CWatchBT_ProductionTestDlg::OnBnClickedStartProductTest)
	
//	ON_CBN_SELCHANGE(IDC_COMBO_MODEL_SELECTION, &CWatchBT_ProductionTestDlg::OnCbnSelchangeComboModelSelection)
//ON_CBN_CLOSEUP(IDC_COMBO_MODEL_SELECTION, &CWatchBT_ProductionTestDlg::OnCbnSelchangeComboModelSelection)
ON_CBN_SELCHANGE(IDC_COMBO_MODEL_SELECTION, &CWatchBT_ProductionTestDlg::OnCbnSelchangeComboModelSelection)
ON_CBN_SELCHANGE(IDC_COMBO_TEST__TYPE_SELECTION, &CWatchBT_ProductionTestDlg::OnCbnSelchangeComboTestTypeSelection)
END_MESSAGE_MAP()

WNDPROC g_prevFunc = NULL;

LRESULT MyWindowHook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if ((uMsg == WM_CHAR) || (uMsg == WM_KEYDOWN) || (uMsg == WM_KEYUP) || (uMsg == WM_SYSKEYDOWN) || (uMsg == WM_SYSKEYUP))
    {
        return 0; // swallow message
    }

    return ::CallWindowProcW(g_prevFunc, hWnd, uMsg, wParam, lParam);
}


void MySubclassWindow(HWND hwnd)
{
    g_prevFunc = (WNDPROC)::SetWindowLongW(hwnd, GWL_WNDPROC, (LONG_PTR)MyWindowHook);
}


// CWatchBT_ProductionTestDlg message handlers

BOOL CWatchBT_ProductionTestDlg::OnInitDialog()
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

	// TODO: Add extra initialization here
	CString strMessage;
	//WriteConfigFile();

	/*int config_files_index, test_type_index;
	
	for (test_type_index = 0; test_type_index < 2; test_type_index++)
	{	
		for (config_files_index = 0; config_files_index < NUM_OF_MODELS; config_files_index++)
		{
			strncpy(configFiles[test_type_index][config_files_index].modelName, modelsStrings[config_files_index], sizeof(configFiles[config_files_index].modelName)-1); 
			configFiles[test_type_index][config_files_index].modelName[sizeof(configFiles[config_files_index].modelName)-1] = 0;

			strncpy(configFiles[test_type_index][config_files_index].fileName, configFileNames[config_files_index], sizeof(configFiles[config_files_index].fileName)-1);
			configFiles[test_type_index][config_files_index].fileName[sizeof(configFiles[config_files_index].fileName)-1] = 0;
		}
	}*/
#if 1 /*Seavia 20150810 import model list from config file*/
	//modelsStrings = new char *[20];
	memset(modelsStrings,0,sizeof(modelsStrings));

	ReadModelFile(MODEL_LIST_FILE_NAME);
	CComboBox * pCombo = (CComboBox *) GetDlgItem(IDC_COMBO_MODEL_SELECTION);
	for (int models_index = 0; models_index < model_list; models_index++)
	{		
		pCombo->InsertString(models_index, /*configFiles[models_index].modelName*/modelsStrings[models_index]);
	}
#else
	CComboBox * pCombo = (CComboBox *) GetDlgItem(IDC_COMBO_MODEL_SELECTION);
	for (int models_index = 0; models_index < NUM_OF_MODELS; models_index++)
	{		
		pCombo->InsertString(models_index, /*configFiles[models_index].modelName*/modelsStrings[models_index]);
		
	}
#endif	
	
	//pCombo->ShowDropDown(1);
	//IDC_COMBO_MODEL_SELECTION.

	::MySubclassWindow(pCombo->GetSafeHwnd());
	
	//int selection = pCombo->GetEditSel();
	
	ReadModelSelectionFile();
	pCombo->SetCurSel(model_selection);

	ReadTestTypeSelectionFile();
#if 1 /*Seavia to do here, modify the read config file name*/
	char fileName[50];
	memset(fileName,0,sizeof(fileName));
	ResetConfigFile(modelsStrings[pCombo->GetCurSel()], fileName);
	int result = ReadConfigFile(fileName);
	ResetSNPrefix(modelsStrings[pCombo->GetCurSel()]);
#else
	int result = ReadConfigFile(configFileNames[testType][model_selection]);
#endif
	if (result != 1)  
	//if (!ReadConfigFile())
	{	
		
		if (result == 0)  
		{	strMessage.Format("Unable to read config file");
		}
		else
		{	strMessage.Format("config file is corrupted");
		}
		AfxMessageBox(strMessage, MB_ICONINFORMATION|MB_OK); 
		exit(1);
	}
	
	if (!ReadBlueToothAddrsFile(AVAIL_BLUETOOTH_ADDR_LIST_FILENAME))
	{
		strMessage.Format("Unable to read %s", AVAIL_BLUETOOTH_ADDR_LIST_FILENAME);
		AfxMessageBox(strMessage, MB_ICONINFORMATION|MB_OK); 
		exit(1);
	}

	if (!ReadBlueToothAddrsFile(ASSIGNED_BLUETOOTH_ADDR_LIST_FILENAME))
	{
		strMessage.Format("Unable to read %s", ASSIGNED_BLUETOOTH_ADDR_LIST_FILENAME);
		AfxMessageBox(strMessage, MB_ICONINFORMATION|MB_OK); 
		exit(1);
	}	
	int success_flag=0;	
	CString availBluetoothAddrStr;

	availBluetoothAddrLap = getAvailBluetoothAddr(success_flag);
	if (success_flag)
	{	availBluetoothAddrStr.Format("AVAIL_BT_ADDR: 0x%04x%02x%06x", availBluetoothAddrNap, availBluetoothAddrUap, availBluetoothAddrLap);
	}
	else
	{
		AfxMessageBox("Talk to Manager, No More BlueToothAddress", MB_OK|MB_ICONQUESTION|MB_DEFBUTTON2);
		availBluetoothAddrStr.Format("AVAIL_BT_ADDR: NONE");
		exit(1);
	}
	GetDlgItem(IDC_STATIC_AVAIL_BT_ADDR)->SetWindowText(availBluetoothAddrStr); 

	if (!ReadSerialNumberFile(AVAIL_SERIAL_NUMBERS_FILENAME))
	{
		strMessage.Format("Unable to read %s", AVAIL_SERIAL_NUMBERS_FILENAME);
		AfxMessageBox(strMessage, MB_ICONINFORMATION|MB_OK); 
		exit(1);
	}	

	if (!ReadSerialNumberFile(ASSIGNED_SERIAL_NUMBERS_FILENAME))
	{
		strMessage.Format("Unable to read %s", ASSIGNED_SERIAL_NUMBERS_FILENAME);
		AfxMessageBox(strMessage, MB_ICONINFORMATION|MB_OK); 
		exit(1);
	}	

	if (!ReadPairingInfoFile(PAIRING_INFO_FILENAME))
	{
		strMessage.Format("Unable to read %s", PAIRING_INFO_FILENAME);
		AfxMessageBox(strMessage, MB_ICONINFORMATION|MB_OK); 
		exit(1);	
	}

	memset(currentBluetoothAddressString, 0x0, sizeof(currentBluetoothAddressString));
	memset(currentLogFileName, 0x0, sizeof(currentLogFileName));

	CWnd *pWndProductTestGroupBox;
	CWnd *pWndPcbTestGroupBox;
	CFont *m_font;
	LOGFONT lf;

	CRect pcbTestGroupBoxDimension, productTestGroupBoxDimension;
	int i, width=240, height=/*35*/45;

	//testType = GetConfigurationValue(TEST_TYPE_STR);

	CComboBox * pComboTestTypeSel = (CComboBox *) GetDlgItem(IDC_COMBO_TEST_TYPE_SELECTION);
	::MySubclassWindow(pComboTestTypeSel->GetSafeHwnd());

	for (int test_type_index = 0; test_type_index < 2; test_type_index++)
	{		
		pComboTestTypeSel->InsertString(test_type_index, test_typesStrings[test_type_index]);
		
	}
	pComboTestTypeSel->SetCurSel(testType);
	
	if (testType == PCB_TEST)
	{	
		m_font = GetDlgItem(IDC_STATIC_PCB_TEST_RESULT)->GetFont();
		if (m_font) 
		{	m_font->GetLogFont(&lf);
			lf.lfHeight = 30;
			m_font->CreateFontIndirect(&lf);    // Create the font.
			GetDlgItem(IDC_STATIC_PCB_TEST_RESULT)->SetFont(m_font);
			//GetDlgItem(IDC_STATIC_PRODUCT_TEST_RESULT)->SetFont(m_font);
			GetDlgItem(IDC_BUTTON_StartPcbTest)->SetFont(m_font);
#ifdef NFIX_CRASH
			lf.lfHeight = 20;
			m_font->CreateFontIndirect(&lf);    // Create the font.
			GetDlgItem(IDC_STATIC_AVAIL_BT_ADDR)->SetFont(m_font);
			GetDlgItem(IDC_STATIC_CURR_BT_ADDR)->SetFont(m_font);

#if 1		/*Seavia 20150903 add Project Information */
			GetDlgItem(IDC_STATIC_PRJ_INFO)->SetFont(m_font);
#endif
			GetDlgItem(IDC_STATIC_SERIAL_NUMBER)->SetFont(m_font);
			GetDlgItem(IDC_STATIC_MODEL_NUMBER)->SetFont(m_font);
			GetDlgItem(IDC_COMBO_MODEL_SELECTION)->SetFont(m_font);
			GetDlgItem(IDC_STATIC_TEST_TYPE)->SetFont(m_font);
			GetDlgItem(IDC_COMBO_TEST_TYPE_SELECTION)->SetFont(m_font);
#endif
		}
#if 1 /*Seavia add TEST SW version*/
	    CString resultstring = "PCB TEST RESULT "+TEST_SW_VERSION+" : Not started";
		GetDlgItem(IDC_STATIC_PCB_TEST_RESULT)->SetWindowText(resultstring);
#else
		GetDlgItem(IDC_STATIC_PCB_TEST_RESULT)->SetWindowText("PCB TEST RESULT: Not started");
#endif
		GetDlgItem(IDC_BUTTON_StartPcbTest)->SetFocus(); 
		
		//hide product test result
#ifndef NFIX_CRASH
		GetDlgItem(IDC_STATIC_PRODUCT_TEST_RESULT)->SetWindowText(" "/*"PRODUCT TEST RESULT: Not started"*/);
#else
		GetDlgItem(IDC_STATIC_PRODUCT_TEST_RESULT)->SetWindowPos(NULL, 0, 0, 0, 0, NULL);
#endif
		GetDlgItem(IDC_BUTTON_StartProductTest)->SetWindowPos(NULL, 0, 0, 0, 0, NULL);
		GetDlgItem(IDC_BUTTON_StartProductTest)->EnableWindow(FALSE);
#ifndef NFIX_CRASH
        GetDlgItem(IDC_BUTTON_StartPcbTest)->SetWindowPos(NULL, 1150, 100, 200, 100, NULL);
#else
		GetDlgItem(IDC_BUTTON_StartPcbTest)->SetWindowPos(NULL, 1150, 110, 200, 100, NULL);

#if 1	/* Seavia 20150903 add Project Information */
		GetDlgItem(IDC_STATIC_AVAIL_BT_ADDR)->SetWindowPos(NULL, 100, 10, /*500*/300, 30, NULL);
		GetDlgItem(IDC_STATIC_CURR_BT_ADDR)->SetWindowPos(NULL, 100, 45, /*500*/300, 30, NULL);
		GetDlgItem(IDC_STATIC_PRJ_INFO)->SetWindowPos(NULL, 100, 75, /*500*/300, 30, NULL);
#else
		GetDlgItem(IDC_STATIC_AVAIL_BT_ADDR)->SetWindowPos(NULL, 100, 10, /*500*/300, 50, NULL);
		GetDlgItem(IDC_STATIC_CURR_BT_ADDR)->SetWindowPos(NULL, 100, 60, /*500*/300, 50, NULL);
#endif
		GetDlgItem(IDC_STATIC_SERIAL_NUMBER)->SetWindowPos(NULL, /*600*/400, 10, /*400*/200, 50, NULL);
		GetDlgItem(IDC_STATIC_PCB_TEST_RESULT)->SetWindowPos(NULL, 600, 60, 1000, 50, NULL);
		//GetDlgItem(IDC_BUTTON_RestorePcbTest)->SetWindowPos(NULL, 400, 670, 100, 30, NULL);
		//hide model number label and selection box 
		GetDlgItem(IDC_STATIC_MODEL_NUMBER)->SetWindowPos(NULL, /*1000*/0, /*10*/0, /*100*/0, /*50*/0, NULL);
		GetDlgItem(IDC_COMBO_MODEL_SELECTION)->SetWindowPos(NULL, /*1150*/0, /*10*/0, /*200*/0, /*200*/0, NULL); 
		GetDlgItem(IDC_COMBO_MODEL_SELECTION)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_TEST_TYPE)->SetWindowPos(NULL, 600, 10, 100, 50, NULL); 
		GetDlgItem(IDC_COMBO_TEST_TYPE_SELECTION)->SetWindowPos(NULL, 800, 10, 200, 200, NULL); 
		
#endif
		pWndProductTestGroupBox = GetDlgItem(productTestGroupBox);
		pWndPcbTestGroupBox = GetDlgItem(pcbTestGroupBox);

		//minimize product test group box 
		pWndProductTestGroupBox->SetWindowPos(NULL, 0, 0, 0, 0, NULL);
#ifndef NFIX_CRASH
		pWndPcbTestGroupBox->SetWindowPos(NULL, 100, 75, 1000, 590, NULL);
#else

#if 1	/* Seavia 20150903 add Project Information */
		pWndPcbTestGroupBox->SetWindowPos(NULL, 100, 120, 1000, 590, NULL);
#else
		pWndPcbTestGroupBox->SetWindowPos(NULL, 100, 110, 1000, 590, NULL);
#endif  /* END Seavia 20150903 add Project Information */

#endif
		CWnd *pWnd = GetDlgItem(pcbTestGroupBox);
		pWnd->GetWindowRect(&pcbTestGroupBoxDimension);
		ScreenToClient(&pcbTestGroupBoxDimension); 
		
		   
		for (i=0; i<NUM_OF_PCB_TEST_ROUTINES; i++)
		{	
			pcbTestCheckBoxes[i] = new CButton;
			//start the ID at end of all product list controls IDs. 
			//make sure it is calculated with 'NUM_OF_PRODUCT_TEST_ROUTINES',
			//NOT 'NUM_OF_PCB_TEST_ROUTINES' !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			pcbTestCheckBoxesIDs[i] = 0x11+3*NUM_OF_PRODUCT_TEST_ROUTINES+i*3;
			pcbTestCheckBoxes[i]->Create(pcb_test_routines_names[i],
    										WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
											CRect(pcbTestGroupBoxDimension.left+10, 
											pcbTestGroupBoxDimension.top+height*i+15, 
											pcbTestGroupBoxDimension.left+width, 
											pcbTestGroupBoxDimension.top+height*(i+1)),
											this, pcbTestCheckBoxesIDs[i]);
			if(testSelection & (0x1 << i))
				CheckDlgButton(pcbTestCheckBoxesIDs[i], 1);
			pcbTestResultsLabels[i] = new CStatic;
			pcbTestResultsLabelsIDs[i] = pcbTestCheckBoxesIDs[i]+1;
			pcbTestResultsLabels[i]->Create("Not started", WS_CHILD | WS_VISIBLE,
											CRect(pcbTestGroupBoxDimension.left+width+10,
											pcbTestGroupBoxDimension.top+height*i+15, 
											pcbTestGroupBoxDimension.left+width+10+width, 
											pcbTestGroupBoxDimension.top+height*(i+1)), 
											this, pcbTestResultsLabelsIDs[i]); 

			m_font = GetDlgItem(pcbTestGroupBox)->GetFont();
			if (m_font) 
			{	m_font->GetLogFont(&lf);
				lf.lfHeight = 30;
				m_font->CreateFontIndirect(&lf);    // Create the font.
				GetDlgItem(pcbTestResultsLabelsIDs[i])->SetFont(m_font);
			}
		
			pcbTestResultsDetailsLabels[i] = new CStatic;
			pcbTestResultsDetailsLabelsIDs[i] = pcbTestCheckBoxesIDs[i]+2;
			pcbTestResultsDetailsLabels[i]->Create(" ", WS_CHILD | WS_VISIBLE,
											CRect(pcbTestGroupBoxDimension.left+10+width+width, 
											pcbTestGroupBoxDimension.top+height*i+15, 
#ifdef NFIX_CRASH
											pcbTestGroupBoxDimension.left+10+width+width+width*2, 
#else
											pcbTestGroupBoxDimension.left+10+width+width+width,													
#endif
											pcbTestGroupBoxDimension.top+height*(i+1)), 
											this, pcbTestResultsDetailsLabelsIDs[i]); 

			if (i == (NUM_OF_PCB_TEST_ROUTINES-1))
			{
#ifdef NFIX_CRASH
				GetDlgItem(IDC_BUTTON_RestorePcbTest)->SetWindowPos(NULL, pcbTestGroupBoxDimension.left+10+width+width+width*2+40, pcbTestGroupBoxDimension.top+height*i+15, 100, 30, NULL);
#else
                GetDlgItem(IDC_BUTTON_RestorePcbTest)->SetWindowPos(NULL, pcbTestGroupBoxDimension.left+10+width+width+width+20, pcbTestGroupBoxDimension.top+height*i+15, 100, 30, NULL);
#endif
			}
		}
	}
	else
	{
		
		m_font = GetDlgItem(IDC_STATIC_PRODUCT_TEST_RESULT)->GetFont();
		if (m_font) 
		{	m_font->GetLogFont(&lf);
			lf.lfHeight = 30;
			m_font->CreateFontIndirect(&lf);    // Create the font.
			GetDlgItem(IDC_STATIC_PRODUCT_TEST_RESULT)->SetFont(m_font);
			//GetDlgItem(IDC_STATIC_PCB_TEST_RESULT)->SetFont(m_font);
			GetDlgItem(IDC_BUTTON_StartProductTest)->SetFont(m_font);
#ifdef NFIX_CRASH
			lf.lfHeight = 20;
			m_font->CreateFontIndirect(&lf);    // Create the font.
			GetDlgItem(IDC_STATIC_AVAIL_BT_ADDR)->SetFont(m_font);
			GetDlgItem(IDC_STATIC_CURR_BT_ADDR)->SetFont(m_font);
			
#if 1		/*Seavia 20150903 add Project Information */
			GetDlgItem(IDC_STATIC_PRJ_INFO)->SetFont(m_font);
#endif
			GetDlgItem(IDC_STATIC_SERIAL_NUMBER)->SetFont(m_font);
			GetDlgItem(IDC_STATIC_MODEL_NUMBER)->SetFont(m_font);
			GetDlgItem(IDC_COMBO_MODEL_SELECTION)->SetFont(m_font); 
			GetDlgItem(IDC_STATIC_TEST_TYPE)->SetFont(m_font);
			GetDlgItem(IDC_COMBO_TEST_TYPE_SELECTION)->SetFont(m_font);
#endif
			GetDlgItem(IDC_FWVER)->SetFont(m_font);
		}
#if 1 /*Seavia add TEST SW version*/
	    CString resultstring = "PRODUCT TEST RESULT "+TEST_SW_VERSION+" : Not started";
		GetDlgItem(IDC_STATIC_PRODUCT_TEST_RESULT)->SetWindowText(resultstring);
#else
		GetDlgItem(IDC_STATIC_PRODUCT_TEST_RESULT)->SetWindowText("PRODUCT TEST RESULT : Not started");
#endif
		GetDlgItem(IDC_BUTTON_StartProductTest)->SetFocus(); 
		//hide pcb test result
#ifndef NFIX_CRASH
		GetDlgItem(IDC_STATIC_PCB_TEST_RESULT)->SetWindowText(" ");
#else
		GetDlgItem(IDC_STATIC_PCB_TEST_RESULT)->SetWindowPos(NULL, 0, 0, 0, 0, NULL);
#endif
		GetDlgItem(IDC_BUTTON_StartPcbTest)->SetWindowPos(NULL, 0, 0, 0, 0, NULL);
		GetDlgItem(IDC_BUTTON_StartPcbTest)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_RestorePcbTest)->SetWindowPos(NULL, 0, 0, 0, 0, NULL);
		GetDlgItem(IDC_BUTTON_RestorePcbTest)->EnableWindow(FALSE);
#ifndef NFIX_CRASH
		GetDlgItem(IDC_BUTTON_StartProductTest)->SetWindowPos(NULL, 1150, 100, 200, 100, NULL);
#else
		GetDlgItem(IDC_BUTTON_StartProductTest)->SetWindowPos(NULL, 1150, 110, 200, 100, NULL);

#if 1	/* Seavia 20150903 add Project Information */
		GetDlgItem(IDC_STATIC_AVAIL_BT_ADDR)->SetWindowPos(NULL, 100, 10, /*500*/300, 30, NULL);
		GetDlgItem(IDC_STATIC_CURR_BT_ADDR)->SetWindowPos(NULL, 100, 45, /*500*/300, 30, NULL);
		GetDlgItem(IDC_STATIC_PRJ_INFO)->SetWindowPos(NULL, 100, 75, /*500*/300, 30, NULL);
#else
		GetDlgItem(IDC_STATIC_AVAIL_BT_ADDR)->SetWindowPos(NULL, 100, 10, /*500*/300, 50, NULL);
		GetDlgItem(IDC_STATIC_CURR_BT_ADDR)->SetWindowPos(NULL, 100, 60, /*500*/300, 50, NULL);
#endif

		GetDlgItem(IDC_STATIC_SERIAL_NUMBER)->SetWindowPos(NULL, /*600*/400, 10, /*400*/200, 50, NULL);
		GetDlgItem(IDC_STATIC_PRODUCT_TEST_RESULT)->SetWindowPos(NULL, 600, 60, 1000, 50, NULL);
		//GetDlgItem(IDC_BUTTON_RestorePcbTest)->SetWindowPos(NULL, 400, 670, 100, 30, NULL);
		GetDlgItem(IDC_STATIC_MODEL_NUMBER)->SetWindowPos(NULL, 1000, 10, 100, 50, NULL); 
		GetDlgItem(IDC_COMBO_MODEL_SELECTION)->SetWindowPos(NULL, 1150, 10, 200, 200, NULL); 
		GetDlgItem(IDC_STATIC_TEST_TYPE)->SetWindowPos(NULL, 600, 10, 100, 50, NULL); 
		GetDlgItem(IDC_COMBO_TEST_TYPE_SELECTION)->SetWindowPos(NULL, 800, 10, 200, 200, NULL);
#endif
		pWndProductTestGroupBox = GetDlgItem(productTestGroupBox);
		pWndPcbTestGroupBox = GetDlgItem(pcbTestGroupBox);

		//minimize pcb test group box 
		pWndPcbTestGroupBox->SetWindowPos(NULL, 0, 0, 0, 0, NULL);
#ifndef NFIX_CRASH
		pWndProductTestGroupBox->SetWindowPos(NULL, 100, 75, 1000, 590, NULL);
#else


#if 1	/* Seavia 20150903 add Project Information */
		pWndProductTestGroupBox->SetWindowPos(NULL, 100, 120, 1000, 590, NULL);
#else
		pWndProductTestGroupBox->SetWindowPos(NULL, 100, 110, 1000, 590, NULL);
#endif

#endif
		CWnd *pWnd = GetDlgItem(productTestGroupBox);
		pWnd->GetWindowRect(&productTestGroupBoxDimension);
		ScreenToClient(&productTestGroupBoxDimension); 

		for (i=0; i<NUM_OF_PRODUCT_TEST_ROUTINES; i++)
		{
			productTestCheckBoxes[i] = new CButton;
			productTestCheckBoxesIDs[i] = 0x11+i*3;
			productTestCheckBoxes[i]->Create(product_test_routines_names[i],
    										WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
											CRect(productTestGroupBoxDimension.left+10, 
											productTestGroupBoxDimension.top+height*i+15, 
											productTestGroupBoxDimension.left+width, 
											productTestGroupBoxDimension.top+height*(i+1)),
											this, productTestCheckBoxesIDs[i]);
			if(testSelection & (0x1 << i))
				CheckDlgButton(productTestCheckBoxesIDs[i], 1);
			productTestResultsLabels[i] = new CStatic;
			productTestResultsLabelsIDs[i] = productTestCheckBoxesIDs[i]+1;
			productTestResultsLabels[i]->Create("Not started", WS_CHILD | WS_VISIBLE,
											CRect(productTestGroupBoxDimension.left+width+10, 
											productTestGroupBoxDimension.top+height*i+15, 
#ifndef NFIX_CRASH
											productTestGroupBoxDimension.left+width+10+220,
#else
											productTestGroupBoxDimension.left+width+10+width,
#endif
											productTestGroupBoxDimension.top+height*(i+1)), 
											this, productTestResultsLabelsIDs[i]); 

			m_font = GetDlgItem(productTestGroupBox)->GetFont();
			if (m_font) 
			{	m_font->GetLogFont(&lf);
				lf.lfHeight = 30;
				m_font->CreateFontIndirect(&lf);    // Create the font.
				GetDlgItem(productTestResultsLabelsIDs[i])->SetFont(m_font);
				
			}

			productTestResultsDetailsLabels[i] = new CStatic;
			productTestResultsDetailsLabelsIDs[i] = productTestCheckBoxesIDs[i]+2;
#ifndef NFIX_CRASH
			productTestResultsDetailsLabels[i]->Create(" ", WS_CHILD | WS_VISIBLE,
											CRect(productTestGroupBoxDimension.left+width+10+/*200*/140, 
											productTestGroupBoxDimension.top+height*i+15, 
											productTestGroupBoxDimension.left+width+10+width+125, 
											productTestGroupBoxDimension.top+height*(i+1)), 
											this, productTestResultsDetailsLabelsIDs[i]); 
#else
			productTestResultsDetailsLabels[i]->Create(" ", WS_CHILD | WS_VISIBLE,
											CRect(productTestGroupBoxDimension.left+width+10+width, 
											productTestGroupBoxDimension.top+height*i+15, 
											productTestGroupBoxDimension.left+width+10+width+width*2, 
											productTestGroupBoxDimension.top+height*(i+1)), 
											this, productTestResultsDetailsLabelsIDs[i]);
#endif
		}

		//display serial # 
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CWatchBT_ProductionTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CWatchBT_ProductionTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWatchBT_ProductionTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int label_text_color = GREY, label_text_id = 0;

HBRUSH CWatchBT_ProductionTestDlg::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
    // Call base class version at first. Or else it will override your changes.
    //HBRUSH hbr = CDialog::OnCtlColor( pDC, pWnd, nCtlColor );

    // Check whether which static label its.
    if( pWnd->GetDlgCtrlID() ==  label_text_id)
    {
        // Set color
		switch (label_text_color)
		{	
			case BLACK:
				pDC->SetTextColor( RGB( 0, 0, 0));
				break;

			case RED:
				pDC->SetTextColor( RGB( 255, 0, 0));  
				break;

			case GREEN:
				pDC->SetTextColor( RGB( 0, 255, 0));
				break;

			case GREY:
				pDC->SetTextColor( RGB( 240,240,240));
				break;

			default:
				pDC->SetTextColor( RGB( 255, 255, 255));
		}
		pDC->SetBkMode( RGB( 240,240,240) );
		return (HBRUSH)(brush->GetSafeHandle());
    }

    return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

CWatchBT_ProductionTestDlg::~CWatchBT_ProductionTestDlg()
{
	delete brush;
	CloseMainLogFile();
}

static const uint16 MID_FREQUENCY_MHZ = 2441;
static const uint16 LOW_FREQUENCY_MHZ = 2402;
static const uint16 HIGH_FREQUENCY_MHZ = 2480;
static const uint16 OFFSET_AVERAGES = 200;
/*float64*/double REF_END_POINT_OUTPUT_POWER = /*-70.0*/0;
int DUT_TRANSMIT_INTERNAL_PA_VALUE = 50, SP_MODULE_TRANSMIT_INTERNAL_PA_VALUE = 50, REF_ENDPOINT_TRANSMIT_INTERNAL_PA_VALUE = 50;
float64 PATH_LOSS = 71.0;
uint32 dutHandle=0, referenceEndPointHandle=0, transmitModuleHandle=0;
int dutCsrDeviceIndex = -1, refEndPointCsrDeviceIndex = -1, transmitModuleCsrDeviceIndex = -1;
unsigned long TimeElasp=0, TotalTimeElasp=0;
ofstream MainLogFileHd, IndividualLogFileHd;




HANDLE UsbMultimeterModuleConnectionHd=0, UsbGpioModuleConnectionHd=0;
int MultimeterInitFlag=0;
#define MAX_GPIO_COMMANDS_LENGTH 20

typedef struct
{
    string config_name;
    double config_value;
	double config_value_max_range;
	double config_value_min_range;
}
config_variable_type;

typedef struct
{
	unsigned long startingBluetoothAddr;
	unsigned long endingBluetoothAddr;
	uint16 licenseKey[LICENSE_KEY_LENGTH];
}
licenseKey_type;


int num_of_licenseKeys = 0;

uint16 range_1_thru_0x800_licenseKey[LICENSE_KEY_LENGTH] = {0x3465, 0x5679, 0x4B21, 0xBCB8, 0x0000}; //test key CVC Handsfree

licenseKey_type licenseKeys[MAX_NUMBER_OF_LICENSE_KEYS]  = { {0x1, 0x800, 0x3465, 0x5679, 0x4B21, 0xBCB8, 0x0000}, };

config_variable_type config_variables[NUM_OF_CONFIGURATIONS+1] = { {TEST_TYPE_STR, 0, 1, 0},
																 {CRYSTAL_TRIM_OFFSET_LIMIT_STR, 2, CRYSTAL_TRIM_OFFSET_LIMIT_MAX_RANGE, 0}, 
																 {ATTENUATION_STR, PATH_LOSS, ATTENUATION_MAX_RANGE, 0},
																 {DUT_OUTPUT_POWER_MIN_LIMIT_STR, DUT_POWER_MIN_LIMIT, /*0*/DUT_OUTPUT_POWER_MAX_LIMIT_RANGE, DUT_OUTPUT_POWER_MIN_LIMIT_RANGE},
																 {DUT_OUTPUT_POWER_MAX_LIMIT_STR, DUT_POWER_MAX_LIMIT, DUT_OUTPUT_POWER_MAX_LIMIT_RANGE, /*0*/ DUT_OUTPUT_POWER_MIN_LIMIT_RANGE},
																 {REF_END_POINT_OUTPUT_POWER_STR, REF_END_POINT_OUTPUT_POWER, REF_END_POINT_OUTPUT_POWER_MAX_LIMIT_RANGE, REF_END_POINT_OUTPUT_POWER_MIN_LIMIT_RANGE},
																 {DUT_TRANSMIT_INTERNAL_PA_VALUE_STR, DUT_TRANSMIT_INTERNAL_PA_VALUE, 63, 0},
																 {DUT_PACKETS_LOSS_PERCENTAGE_MAX_LIMIT_STR, DUT_PACKETS_LOSS_PERCENTAGE, 10},
																 {DUT_BER_PERCENTAGE_MAX_LIMIT_STR, DUT_BER_PERCENTAGE, DUT_BER_PERCENTAGE_MAX_LIMIT_RANGE, 0},
															     {GPIO_COM_NUM_STR, /*0*//*34*/44, 999, 0},
																 {MULTIMETER_COM_NUM_STR, /*0*/29, 999, 0},
															     
																 {DUT_STANDBY_CURRENT_MAX_LIMIT_STR, 1/*0.0002*/, 1/*0.001*/, 0},
																 {DUT_PEAK_CURRENT_MAX_LIMIT_STR, 0.2, 1, 0},
																 {DUT_CHARGING_CURRENT_MIN_LIMIT_STR, 0.054, 0.1, 0},
																 {DUT_CHARGING_CURRENT_MAX_LIMIT_STR, 0.066, 0.2, 0},

																 {V18_POWER_SUPPLY_MIN_LIMIT_STR, 1.7, 1.8, 0},
																 {V18_POWER_SUPPLY_MAX_LIMIT_STR, 1.9, 2.0, 0},
																 {V135_POWER_SUPPLY_MIN_LIMIT_STR, 1.3, 1.35, 0},
																 {V135_POWER_SUPPLY_MAX_LIMIT_STR, 1.4, 1.5, 0},
																 {V33_POWER_SUPPLY_MIN_LIMIT_STR, 3.2, 3.3, 0},
																 {V33_POWER_SUPPLY_MAX_LIMIT_STR, 3.4, 3.5, 0},

																 //{PRODUCT_FFT_SIGNAL_1K_FREQ_MIN_LIMIT_STR, 9*pow((double) 10,7), pow((double) 10,9), 0},
																 {PRODUCT_MICROPHONE_FFT_SIGNAL_1K_FREQ_MIN_LIMIT_STR, 7, 9, 0},
																 //{PRODUCT_FFT_NOISE_FLOOR_MAX_LIMIT_STR, pow((double) 10,6), pow((double) 10,9), 0},
																 {PRODUCT_MICROPHONE_FFT_NOISE_FLOOR_MAX_LIMIT_STR, 6, 9, 0},
																 //{PRODUCT_FFT_NOISE_FLOOR_2K_FREQ_MAX_LIMIT_STR, 4*pow((double) 10,6), pow((double) 10,9), 0},
																 {PRODUCT_MICROPHONE_FFT_NOISE_FLOOR_2K_FREQ_MAX_LIMIT_STR, 6, 9, 0},
																 //{PRODUCT_FFT_NOISE_FLOOR_3K_FREQ_MAX_LIMIT_STR, 2*pow((double) 10,6), pow((double) 10,9), 0},
																 {PRODUCT_MICROPHONE_FFT_NOISE_FLOOR_3K_FREQ_MAX_LIMIT_STR, 6, 9, 0},

																 {PRODUCT_SPEAKER_FFT_SIGNAL_1K_FREQ_MIN_LIMIT_STR, 7, 9, 0},
																 {PRODUCT_SPEAKER_FFT_NOISE_FLOOR_MAX_LIMIT_STR, 6, 9, 0},
																 {PRODUCT_SPEAKER_FFT_NOISE_FLOOR_2K_FREQ_MAX_LIMIT_STR, 6, 9, 0},
																 {PRODUCT_SPEAKER_FFT_NOISE_FLOOR_3K_FREQ_MAX_LIMIT_STR, 6, 9, 0},

																 //{PCB_FFT_SIGNAL_1K_FREQ_MIN_LIMIT_STR, 9*pow((double) 10,7), pow((double) 10,9), 0},
																 {PCB_LOOPBACK_FFT_SIGNAL_1K_FREQ_MIN_LIMIT_STR, 7, 9, 0},
																 //{PCB_FFT_NOISE_FLOOR_MAX_LIMIT_STR, pow((double) 10,6), pow((double) 10,9), 0},
																 {PCB_LOOPBACK_FFT_NOISE_FLOOR_MAX_LIMIT_STR, 6, 9, 0},
																 //{PCB_FFT_NOISE_FLOOR_2K_FREQ_MAX_LIMIT_STR, 4*pow((double) 10,6), pow((double) 10,9), 0},
																 {PCB_LOOPBACK_FFT_NOISE_FLOOR_2K_FREQ_MAX_LIMIT_STR, 6, 9, 0},
																 //{PCB_FFT_NOISE_FLOOR_3K_FREQ_MAX_LIMIT_STR, 2*pow((double) 10,6), pow((double) 10,9), 0},
																 {PCB_LOOPBACK_FFT_NOISE_FLOOR_3K_FREQ_MAX_LIMIT_STR, 6, 9, 0},

																 {PCB_SPEAKER_FFT_SIGNAL_1K_FREQ_MIN_LIMIT_STR, 7, 9, 0},
																 //{PCB_FFT_NOISE_FLOOR_MAX_LIMIT_STR, pow((double) 10,6), pow((double) 10,9), 0},
																 {PCB_SPEAKER_FFT_NOISE_FLOOR_MAX_LIMIT_STR, 6, 9, 0},
																 //{PCB_FFT_NOISE_FLOOR_2K_FREQ_MAX_LIMIT_STR, 4*pow((double) 10,6), pow((double) 10,9), 0},
																 {PCB_SPEAKER_FFT_NOISE_FLOOR_2K_FREQ_MAX_LIMIT_STR, 6, 9, 0},
																 //{PCB_FFT_NOISE_FLOOR_3K_FREQ_MAX_LIMIT_STR, 2*pow((double) 10,6), pow((double) 10,9), 0},
																 {PCB_SPEAKER_FFT_NOISE_FLOOR_3K_FREQ_MAX_LIMIT_STR, 6, 9, 0},
																 {PRODUCT_IGNORE_SERIAL_NUMBER_CHECK_STR, 0, 1, 0},
																 {PRODUCT_IGNORE_HID_CHECK_STR, 0, 1, 0},
																 {RSSI_SAMPLE_SIZE_STR, 10, MAX_RSSI_SAMPLE_SIZE, 1},
																 {NUM_OF_TIMES_CALCULATE_RSSI_VALUE_STR, 1, 50, 1},
																 {EASY_FACTORY_ENABLE_STR, 0, 1, 0},
																 {SP_MODULE_TRANSMIT_INTERNAL_PA_VALUE_STR, SP_MODULE_TRANSMIT_INTERNAL_PA_VALUE, 63, 0},
																 {FREQ_HOPPING_BER_SAMPLE_SIZE_STR, 5000, 20000, 0},
																 {REFERENCE_ENDPOINT_INTERNAL_PA_VALUE_STR, REF_ENDPOINT_TRANSMIT_INTERNAL_PA_VALUE, 63, 0},
																 {BLUETOOTH_CONNECTION_WAIT_TIME_STR, 8, 100, 0},
																 {MICROPHONE_TEST_WAIT_TIME_STR, 1, 100, 0},
																 {FFT_SIGNAL_FREQ_WINDOW_STR, 2, 20, 0},
																 {STOP_AT_FIRST_FAIL_STR, 1, 1, 0},
																 {PRODUCT_RX_RETRY_STR, 2, 20, 0},
																 {DUT_RESET_USING_RELAYS_STR, 0, 1, 0},
#if 0															 /* Seavia 20160512 add sn prefix*/
																 {PRODUCT_SERIAL_NUM_PREFIX_STR, 0, 0, 0},
#endif															 /* Seavia 20150612 end*/
#if 1															 /* Seavia 20150811 add factory indicator*/
																 {PRODUCT_FACTORY_INDICATOR_STR, 0, 0, 0},
#endif															 /* Seavia 20150811 end*/
																 {ADDRESS_LICENSEKEYINFORMATION_STR, 0, 0, 0},
																}; 

double GetConfigurationValue(string config_name)
{	
	int search_index = 0;
	
	while (search_index < NUM_OF_CONFIGURATIONS)
	{
		if (config_name == config_variables[search_index].config_name)
		{	
			if((config_variables[search_index].config_value_min_range <= config_variables[search_index].config_value) 
				&&
			   (config_variables[search_index].config_value_max_range >= config_variables[search_index].config_value)
			  )
			  return config_variables[search_index].config_value;
			else 
			  return 0.0;	
		}
		else
		{	 
		}
		search_index++;
	}

	return 0.0;
}

int WriteToUsbModule(const char * outputData, const unsigned int & sizeBuffer, unsigned long & length, HANDLE &UsbDeviceHd)
{	
	//if (length > 0)
	//{
		if (WriteFile(UsbDeviceHd, // handle to file to write to
			outputData,              // pointer to data to write to file
			sizeBuffer,              // number of bytes to write
			&length,NULL) == 0)      // pointer to number of bytes written
		{

			WriteLogFile("WriteToUsbModule:Writing of serial communication has problem.");

			return FALSE;
		}
		return TRUE;
	//}
	//return FALSE
}

int ReadFromUsbModule(char * inputData, const unsigned int sizeBuffer, unsigned long & length, HANDLE &UsbDeviceHd)
{
	Sleep(50);
	
	if (ReadFile(UsbDeviceHd,  // handle of file to read
		inputData,               // handle of file to read
		sizeBuffer,              // number of bytes to read
		&length,                 // pointer to number of bytes read
		NULL) == 0)              // pointer to structure for data
	{

		WriteLogFile("ReadFromUsbModule:Reading of serial communication has problem.");

		return FALSE;
	}
	if (length > 0)
	{
		//inputData[length] = NULL; // Assign end flag of message.
		return TRUE;  
	}  

	return /*TRUE*/FALSE;
}

int ClearGPIOCMDS()
{

#if 1 /* Seavia 20150810 , fix unreferenced local variable warning*/
	char command[MAX_GPIO_COMMANDS_LENGTH+1], resp[MAX_GPIO_COMMANDS_LENGTH+1];
#else
	char command[MAX_GPIO_COMMANDS_LENGTH+1], resp[MAX_GPIO_COMMANDS_LENGTH+1], expected_resp[MAX_GPIO_COMMANDS_LENGTH+1];
#endif
	unsigned long length_written=0, length_read=0, expected_length_read=0;
	int completedbytes, totalbytes;
	int n=0;
	memset(command,0x0,sizeof(command));
	sprintf(command, "\r");

	totalbytes = strlen(command);
	completedbytes = 0;
	n = 0;
	do
	{
		if(WriteToUsbModule(&command[completedbytes], totalbytes - completedbytes, length_written, UsbGpioModuleConnectionHd) == 1)
		{
			completedbytes += length_written;
		}
		else
		{
            break;
		}
	} while ( (totalbytes < completedbytes) && ((n++)<2));

	if (totalbytes != completedbytes) 
	{
		WriteLogFile("ClearGPIOCMDS:totalbytes %d != completedbytes %d", totalbytes, completedbytes);
		return 0;
	}

	memset(resp,0x0,sizeof(resp));
	expected_length_read = strlen("\n\r>");
    completedbytes = 0;
	n=0;
	do 
	{	
		if(ReadFromUsbModule(&resp[completedbytes], expected_length_read -completedbytes, length_read, UsbGpioModuleConnectionHd) == 1)
		{
			completedbytes += length_read;
		}
		else
		{
            break;
		}
	} while ( ((unsigned long)completedbytes < expected_length_read) && ((n++)<2));

	if (completedbytes != expected_length_read) 
	{	
		WriteLogFile("ClearGPIOCMDS:completedbytes %d != expected_length_read %d", completedbytes, expected_length_read);
	}

	if (strncmp(resp, "\n\r>", strlen("\n\r>")))
	{	
		WriteLogFile("ClearGPIOCMDS:resp %s != \n\r>", resp);
		return 0;
	}

	return 1; 
}
		
int ChangeGPIOState(int gpio_num, int state)
{
	char command[MAX_GPIO_COMMANDS_LENGTH+1], resp[MAX_GPIO_COMMANDS_LENGTH+1];
	unsigned long length_written=0, length_read=0, expected_length_read=0;
	int completedbytes, totalbytes;
	int n=0;
#ifdef NO_GPIO_BOARD 
	return 1;
#endif
	if ((gpio_num > 7) || (gpio_num < 0))
	{
		return 0;
	}

	memset(command,0x0,sizeof(command));
	if (state == HIGH)
	{	
		sprintf(command, "gpio set %d\r", gpio_num);
	}
	else if (state == LOW)
	{	
		sprintf(command, "gpio clear %d\r", gpio_num);
	}

	totalbytes  = strlen(command);
	completedbytes = 0;
	n=0;
	do 
	{
		if(WriteToUsbModule(&command[completedbytes], totalbytes - completedbytes, length_written, UsbGpioModuleConnectionHd) == 1)
		{
			completedbytes += length_written;
		}
		else
		{
            break;
		}
	} while ( (totalbytes < completedbytes) && ((n++)<2));

	if (totalbytes != completedbytes) 
	{
		WriteLogFile("ChangeGPIOState:totalbytes %d != completedbytes %d", totalbytes, completedbytes);
		return 0;
	}

	memset(resp,0x0,sizeof(resp));
	//the expected length is the length of the command minus 1 (the carriage return in command) 
	//plus line feed and carriage return and greater sign ('>')
	expected_length_read = strlen(command)-1+strlen("\n\r>");
    completedbytes = 0;
	n=0;
	do 
	{	
		if(ReadFromUsbModule(&resp[completedbytes], expected_length_read -completedbytes, length_read, UsbGpioModuleConnectionHd) == 1)
		{
			completedbytes += length_read;
		}
		else
		{
            break;
		}
	} while ( ((unsigned long)completedbytes < expected_length_read) && ((n++)<2));

	if (completedbytes != expected_length_read) 
	{
		WriteLogFile("ChangeGPIOState:completedbytes %d != expected_length_read %d", completedbytes, expected_length_read);
		//return 0;
	}

	//the resp should match the command minus 1 (the carriage return in command) 
	if (strncmp(resp, command, strlen(command)-1))
	{
		WriteLogFile("ChangeGPIOState:resp %s != command %s", resp, command);
		return 0;
	}
	return 1; 
}

int ReadGPIOValue(int gpio_num, double &adcvalue, int io_type)
{
	char command[MAX_GPIO_COMMANDS_LENGTH+1], resp[MAX_GPIO_COMMANDS_LENGTH+1], expected_resp[MAX_GPIO_COMMANDS_LENGTH+1];
	char actual_io_value_str[5];
	char *pch;
	unsigned long length_read=0, length_written=0, expected_length_read=0;
#if 1 /* Seavia 20150810 , fix unreferenced local variable*/
	int n=0;
#else
	int state, n=0;
#endif
	int completedbytes, totalbytes;
	
	if ((gpio_num > 7) || (gpio_num < 0))
	{
		return 0;
	}

	if ((io_type != DIGITAL_IO) && (io_type != ANALOG_IO))
	{
		return 0;
	}
	memset(command,0x0,sizeof(command));
	if (io_type == DIGITAL_IO)
	{	sprintf(command, "gpio read %d\r", gpio_num);
	}
	else
	{	sprintf(command, "adc read %d\r", gpio_num);
	}
    totalbytes =  strlen(command);
	completedbytes = 0;
	n=0;
	do 
	{
		if(WriteToUsbModule(&command[completedbytes], totalbytes - completedbytes, length_written, UsbGpioModuleConnectionHd) == 1)
		{
			completedbytes += length_written;
		}
		else
		{
            break;
		}
	} while ( (totalbytes < completedbytes) && ((n++)<5));
	
	if (totalbytes != completedbytes) 
	{
		WriteLogFile("ReadGPIOValue:totalbytes %d != completedbytes %d", totalbytes, completedbytes);
		return 0;
	}

	memset(resp,0x0,sizeof(resp));
	
	if (io_type == DIGITAL_IO) 
	{	
		//the expected length is the length of the command minus 1 (the carriage return in command) 
		//plus line feed and carriage return and the value length(strlen("1")
		//plus line feed and carriage return and greater sign ('>')
		expected_length_read = strlen(command) -1 + strlen("\n\r") + strlen("1") + strlen("\n\r>"); 
	}
	else
	{	//the expected length is the length of the command minus 1 (the carriage return in command) 
		//plus line feed and carriage return and the max adc value length(strlen("1024")
		//plus line feed and carriage return and greater sign ('>')
		expected_length_read = strlen(command) -1 + strlen("\n\r") + strlen("1024") + strlen("\n\r>");
	}
    
	completedbytes = 0;
	n=0;
	do 
	{	
		if(ReadFromUsbModule(&resp[completedbytes], expected_length_read -completedbytes, length_read, UsbGpioModuleConnectionHd) == 1)
		{
			completedbytes += length_read;
		}
		else
		{
            break;
		}
	} while ( ((unsigned long)completedbytes < expected_length_read) && ((n++)<5));

	if (completedbytes != expected_length_read) 
	{	
		if (completedbytes == 8)
		{
			WriteLogFile("ReadGPIOValue:completedbytes %d != expected_length_read %d", completedbytes, expected_length_read);
		}
		//return 0;
	}
		

	//the expected resp should have line feed and carriage return 
	memset(expected_resp,0x0,sizeof(expected_resp));
	strncpy(expected_resp, "\n\r", strlen("\n\r"));

	//make sure the 'resp' contains line feed and carriage return
	pch = strstr(resp, expected_resp);
	if ( pch == NULL)
	{	
		WriteLogFile("ReadGPIOValue:resp %s does not contain %s", resp, expected_resp);
		return 0;
	}
	
	pch += strlen("\n\r");
	memset(actual_io_value_str,0x0,sizeof(actual_io_value_str));
	//extract the result (either '0' or '1' for digital type, value up to '1024' for analog type) in the bytes after the carriage return
	if (io_type == DIGITAL_IO) 
	{	strncpy(actual_io_value_str, pch, 1);
	}
	else
	{	strncpy(actual_io_value_str, pch, strlen("1024"));
	}

	if (io_type == DIGITAL_IO)
	{	if ((atoi(actual_io_value_str) != LOW) && (atoi(actual_io_value_str) != HIGH))
		{
			return 0;
		}
	}
	else
	{
		if ((atoi(actual_io_value_str) < 0) || (atoi(actual_io_value_str) > 1024))
		{	
			WriteLogFile("ReadGPIOValue:adc value %d is out of range", atoi(actual_io_value_str));
			return 0;
		}
	}
	
	if (io_type == DIGITAL_IO)
	{	
		adcvalue = double(atoi(actual_io_value_str)); 
		return 1;
	}
	else
	{	adcvalue = (double(atoi(actual_io_value_str))/1023.0 * 5.0);
	    return 1;
	}

	//this should never been reached
	return 0;
}

#if 0 // seavia, move to .h
#define MAX_MULTIMETER_COMMANDS_LENGTH 30
#define MULTIMETER_CHANGE_TO_DC_CURRENT_COMMAND_STR ":func curr:dc\n"
#define MULTIMETER_CHANGE_TO_DC_VOLTAGE_COMMAND_STR ":func volt:dc\n"

#define MULTIMETER_FUNCTION_QUERY_COMMAND_STR ":func?\n"

#define MULTIMETER_CHANGE_TO_DC_CURRENT_RESP_STR "curr:dc"
#define MULTIMETER_CHANGE_TO_DC_VOLTAGE_RESP_STR "volt:dc"

#define MULTIMETER_GET_MEASUREMENT_COMMAND_STR ":fetc?\n"
#define MULTIMETER_CHANGE_DC_CURRENT_RANGE_TO_2_AMP_COMMAND_STR ":curr:dc:rang:upp 5\n" //for new multimeter
#define MULTIMETER_CHANGE_DC_VOLTAGE_RANGE_TO_2_AMP_COMMAND_STR ":volt:dc:rang:upp 5\n"
#define MULTIMETER_CHANGE_DC_CURRENT_RANGE_QUERY_COMMAND_STR ":curr:dc:rang:upp?\n"
#define MULTIMETER_CHANGE_DC_VOLTAGE_RANGE_QUERY_COMMAND_STR ":volt:dc:rang:upp?\n"
#define MULTIMETER_CHANGE_DC_CURRENT_RANGE_TO_2_AMP_RESP_STR ":curr:dc:rang:upp?\n5" //for new multimeter
#define MULTIMETER_CHANGE_DC_VOLTAGE_RANGE_TO_2_AMP_RESP_STR ":volt:dc:rang:upp?\n5"

//#define MULTIMETER_CHANGE_DC_CURRENT_RANGE_TO_AUTO_COMMAND_STR ":curr:dc:rang:auto 1\n"
//#define MULTIMETER_CHANGE_DC_VOLTAGE_RANGE_TO_AUTO_COMMAND_STR ":volt:dc:rang:auto 1\n"

//#define MULTIMETER_CHANGE_DC_CURRENT_RANGE_TO_AUTO_QUERY_COMMAND_STR ":curr:dc:rang:auto?\n"
//#define MULTIMETER_CHANGE_DC_VOLTAGE_RANGE_TO_AUTO_QUERY_COMMAND_STR ":volt:dc:rang:auto?\n"

//#define MULTIMETER_CHANGE_DC_CURRENT_RANGE_TO_AUTO_QUERY_COMMAND_STR ":curr:dc:rang:auto?\n"
//#define MULTIMETER_CHANGE_DC_VOLTAGE_RANGE_TO_AUTO_QUERY_COMMAND_STR ":volt:dc:rang:auto?\n"

//#define MULTIMETER_CHANGE_DC_CURRENT_RANGE_TO_AUTO_RESP_STR ":curr:dc:rang:auto?\n1"
//#define MULTIMETER_CHANGE_DC_VOLTAGE_RANGE_TO_AUTO_RESP_STR ":volt:dc:rang:auto?\n1"
#endif

int MultimeterInit(int measurement_type)
{
	char command[MAX_MULTIMETER_COMMANDS_LENGTH+1], resp[MAX_MULTIMETER_COMMANDS_LENGTH+1];
#if 1 /* Seavia 20150810 , fix unreferenced local variable*/
	char expected_resp[MAX_MULTIMETER_COMMANDS_LENGTH+1];
#else
	char expected_resp[MAX_MULTIMETER_COMMANDS_LENGTH+1], value_str[MAX_MULTIMETER_COMMANDS_LENGTH+1];
#endif
	unsigned long length_written=0, length_read=0, n=0, num_of_attempts=0, counter=0, write_counter=0;
	int completedbytes, totalbytes, expected_length_read;
	CString strMessage;
	
	//return if multimeter is already initialized
	if (MultimeterInitFlag)	return 1;

	memset(command,0x0,sizeof(command));
	if (measurement_type == CURRENT_MEASUREMENT)
	{	strncpy(command, MULTIMETER_CHANGE_TO_DC_CURRENT_COMMAND_STR, sizeof(command)); 
	}
	else if (measurement_type == VOLTAGE_MEASUREMENT)
	{	strncpy(command, MULTIMETER_CHANGE_TO_DC_VOLTAGE_COMMAND_STR, sizeof(command)); 
	}
	else
	{	return INVALID_MEASUREMENT;
	}
		
	command[MAX_MULTIMETER_COMMANDS_LENGTH] = 0;

	
	totalbytes = strlen(command);
	completedbytes = 0;
	n = 0;
	do
	{
		if(WriteToUsbModule(&command[completedbytes], totalbytes - completedbytes, length_written, UsbMultimeterModuleConnectionHd) == 1)
		{
			completedbytes += length_written;
		}
		else
		{
            break;
		}
	} while ( (totalbytes < completedbytes) && ((n++)<2));
	
	if (totalbytes != completedbytes) 
	{	
		WriteLogFile("MultimeterInit:totalbytes %d != completedbytes %d", totalbytes, completedbytes);
		return 0;
	}

	length_read = 0;
	memset(resp,0x0,sizeof(resp)); 
	expected_length_read = /*strlen(command)-1*/MAX_MULTIMETER_COMMANDS_LENGTH;
	
	completedbytes = 0;
	n=0;
	do 
	{	
		if(ReadFromUsbModule(&resp[completedbytes], expected_length_read -completedbytes, length_read, UsbMultimeterModuleConnectionHd) == 1)
		{
			completedbytes += length_read;
		}
		else
		{
            break;
		}
	} while ( (completedbytes < expected_length_read) && ((n++)<2));

	memset(expected_resp,0x0,sizeof(expected_resp));
	if (measurement_type == CURRENT_MEASUREMENT)
	{	strncpy(expected_resp, MULTIMETER_CHANGE_TO_DC_CURRENT_RESP_STR ,sizeof(expected_resp));
	}
	else if (measurement_type == VOLTAGE_MEASUREMENT)
	{	strncpy(expected_resp, MULTIMETER_CHANGE_TO_DC_VOLTAGE_RESP_STR, sizeof(expected_resp)); 
	}
		

	
// ------------------------------------------------------------------------------------------------------
	num_of_attempts=0;
	
	do 
	{	
		memset(command,0x0,sizeof(command));
		strncpy(command, MULTIMETER_FUNCTION_QUERY_COMMAND_STR, sizeof(command)); 
		command[MAX_MULTIMETER_COMMANDS_LENGTH] = 0;
	
		
		totalbytes =  strlen(command);
		completedbytes = 0;
		n = 0;
		do
		{
			if(WriteToUsbModule(&command[completedbytes], totalbytes - completedbytes, length_written, UsbMultimeterModuleConnectionHd) == 1)
			{
				completedbytes += length_written;
			}
			else
			{
				break;
			}
		} while ( (totalbytes < completedbytes) && ((n++)<2));
	
		if (totalbytes != completedbytes) 
		{	
			WriteLogFile("MultimeterInit:totalbytes %d != completedbytes %d", totalbytes, completedbytes);
			return 0;
		}

		length_read = 0;
		memset(resp,0x0,sizeof(resp)); 
		expected_length_read = MAX_MULTIMETER_COMMANDS_LENGTH;
		completedbytes = 0;
		n=0;
		do 
		{	
			if(ReadFromUsbModule(&resp[completedbytes], expected_length_read -completedbytes, length_read, UsbMultimeterModuleConnectionHd) == 1)
			{
				completedbytes += length_read;
			}
			else
			{
				break;
			}
		} while ( (completedbytes < expected_length_read) && ((n++)<2));

		memset(expected_resp,0x0,sizeof(expected_resp));
		if (measurement_type == CURRENT_MEASUREMENT)
		{	strncpy(expected_resp, MULTIMETER_CHANGE_TO_DC_CURRENT_RESP_STR ,sizeof(expected_resp));
		}
		else if (measurement_type == VOLTAGE_MEASUREMENT)
		{	strncpy(expected_resp, MULTIMETER_CHANGE_TO_DC_VOLTAGE_RESP_STR, sizeof(expected_resp)); 
		}
		if (strstr(resp, expected_resp) == NULL)
		{
			WriteLogFile("MultimeterInit:resp %s does not contain %s. num_of_attempts is %d", resp, expected_resp, num_of_attempts);
		}
	} while ( (strstr(resp, expected_resp) == NULL) && ((num_of_attempts++)<10));
		
	if (strstr(resp, expected_resp) == NULL)
	{	
		//success_flag = 0;
		WriteLogFile("Unable to communicate with BK PRECISION 2, please re-set it by powering it off and on");
		AfxMessageBox("Unable to communicate with BK PRECISION 2, please re-set it by powering it off and on \n", MB_ICONINFORMATION|MB_OK);
		return /*INVALID_MEASUREMENT*/0;
	}
	
//#endif

// ------------------------------------------------------------------------------------------------------
	memset(command,0x0,sizeof(command));

	if (measurement_type == CURRENT_MEASUREMENT)
	{	
		strncpy(command, MULTIMETER_CHANGE_DC_CURRENT_RANGE_TO_2_AMP_COMMAND_STR, sizeof(command));  

	}
	else if (measurement_type == VOLTAGE_MEASUREMENT)
	{	
		strncpy(command, MULTIMETER_CHANGE_DC_VOLTAGE_RANGE_TO_2_AMP_COMMAND_STR, sizeof(command));   
	}

	command[MAX_MULTIMETER_COMMANDS_LENGTH] = 0;

	
	totalbytes =  strlen(command);
	completedbytes = 0;
	n = 0;


	do
	{
		if(WriteToUsbModule(&command[completedbytes], totalbytes - completedbytes, length_written, UsbMultimeterModuleConnectionHd) == 1)
		{
			completedbytes += length_written;
		}
		else
		{
            break;
		}
	} while ( (totalbytes < completedbytes) && ((n++)<2));
	
	if (totalbytes != completedbytes) 
	{	
		WriteLogFile("MultimeterInit:totalbytes %d != completedbytes %d", totalbytes, completedbytes);
		return 0;
	}

	length_read = 0;
	memset(resp,0x0,sizeof(resp)); 
	expected_length_read = /*strlen(command)-1*/MAX_MULTIMETER_COMMANDS_LENGTH;
	
	completedbytes = 0;
	n=0;
	do 
	{	
		if(ReadFromUsbModule(&resp[completedbytes], expected_length_read -completedbytes, length_read, UsbMultimeterModuleConnectionHd) == 1)
		{
			completedbytes += length_read;
		}
		else
		{
            break;
		}
	} while ( (completedbytes < expected_length_read) && ((n++)<2));

	memset(expected_resp,0x0,sizeof(expected_resp));
	//the 'expected resp' from BK Precision is the same as the command sent minus the '\n' character
	strncpy(expected_resp, command, strlen(command)-1); 


		
// ------------------------------------------------------------------------------------------------------
	num_of_attempts=0;

	do 
	{	
		memset(command,0x0,sizeof(command));

		if (measurement_type == CURRENT_MEASUREMENT)
		{	
			strncpy(command, MULTIMETER_CHANGE_DC_CURRENT_RANGE_QUERY_COMMAND_STR, sizeof(command));  
		}
		else if (measurement_type == VOLTAGE_MEASUREMENT)
		{	
			strncpy(command, MULTIMETER_CHANGE_DC_VOLTAGE_RANGE_QUERY_COMMAND_STR, sizeof(command));   
		}
		command[MAX_MULTIMETER_COMMANDS_LENGTH] = 0;

		totalbytes = strlen(command);
		completedbytes = 0;
		n = 0;
		do
		{
			if(WriteToUsbModule(&command[completedbytes], totalbytes - completedbytes, length_written, UsbMultimeterModuleConnectionHd) == 1)
			{
				completedbytes += length_written;
			}
			else
			{
				break;
			}
		} while ( (totalbytes < completedbytes) && ((n++)<2));
	
		if (totalbytes != completedbytes) 
		{	
			WriteLogFile("MultimeterInit:totalbytes %d != completedbytes %d", totalbytes, completedbytes);
			return 0;
		}	
	
		length_read = 0;
		memset(resp,0x0,sizeof(resp)); 
		expected_length_read = MAX_MULTIMETER_COMMANDS_LENGTH;
	
		completedbytes = 0;
		n=0;
		do 
		{	
			if(ReadFromUsbModule(&resp[completedbytes], expected_length_read -completedbytes, length_read, UsbMultimeterModuleConnectionHd) == 1)
			{
				completedbytes += length_read;
			}
			else
			{
				break;
			}
		} while ( (completedbytes < expected_length_read) && ((n++)<2));



		memset(expected_resp,0x0,sizeof(expected_resp));
		
		if (measurement_type == CURRENT_MEASUREMENT)
		{	
			strncpy(expected_resp, MULTIMETER_CHANGE_DC_CURRENT_RANGE_TO_2_AMP_RESP_STR, sizeof(expected_resp)-1);  
		}
		else if (measurement_type == VOLTAGE_MEASUREMENT)
		{	
			strncpy(expected_resp, MULTIMETER_CHANGE_DC_VOLTAGE_RANGE_TO_2_AMP_RESP_STR, sizeof(expected_resp)-1);   
		}
		if (strstr(resp, expected_resp) == NULL)
		{
			WriteLogFile("MultimeterInit:resp %s does not contain %d, num_of_attempts is %d", resp, expected_resp, num_of_attempts);
		}
	} while ( (strstr(resp, expected_resp) == NULL) && ((num_of_attempts++)<10));

	if (strstr(resp, expected_resp) == NULL)
	{	WriteLogFile("resp is %s num_of_attempts %d" , resp, num_of_attempts); 
		//success_flag = 0;
		WriteLogFile("Unable to communicate with BK PRECISION 4, please re-set it by powering it off and on");
		AfxMessageBox("Unable to communicate with BK PRECISION 4, please re-set it by powering it off and on \n", MB_ICONINFORMATION|MB_OK);
		return /*INVALID_MEASUREMENT*/0;
	}
	MultimeterInitFlag = 1;
	return 1;
}

int ReadMultimeterMeasurement(double &measured_value)
{
	char command[MAX_MULTIMETER_COMMANDS_LENGTH+1], resp[MAX_MULTIMETER_COMMANDS_LENGTH+1];
#if 1 /* Seavia 20150810 , fix unreferenced local variable warning*/
	char value_str[MAX_MULTIMETER_COMMANDS_LENGTH+1];
#else
	char expected_resp[MAX_MULTIMETER_COMMANDS_LENGTH+1], value_str[MAX_MULTIMETER_COMMANDS_LENGTH+1];
#endif
	char previous_char;
	unsigned long length_written=0, length_read=0, n=0, num_of_attempts=0, counter=0, write_counter=0;
	int completedbytes, totalbytes, expected_length_read;
	CString strMessage;

	//initialize 'measured_value' to 0.0 since this variable is passed by reference
	measured_value = 0.0;
#ifdef NO_MM_UNIT
    return 1;
#endif
// ------------------------------------------------------------------------------------------------------

	num_of_attempts=0;

	do
	{	
		memset(command,0x0,sizeof(command));
		strncpy(command, MULTIMETER_GET_MEASUREMENT_COMMAND_STR, sizeof(command)); 
		command[MAX_MULTIMETER_COMMANDS_LENGTH] = 0;

		//Sleep(50);
		totalbytes =  strlen(command);
		completedbytes = 0;
		n = 0;
		do
		{
			if(WriteToUsbModule(&command[completedbytes], totalbytes - completedbytes, length_written, UsbMultimeterModuleConnectionHd) == 1)
			{
				completedbytes += length_written;
			}
			else
			{
				break;
			}
		} while ( (totalbytes < completedbytes) && ((n++)<2));

		if (totalbytes != completedbytes) 
		{
			WriteLogFile("ReadMultimeterMeasurement:totalbytes %d != completedbytes %d", totalbytes, completedbytes);
			return 0;
		}

		length_read = 0;
		memset(resp,0x0,sizeof(resp)); 
		expected_length_read = MAX_MULTIMETER_COMMANDS_LENGTH;

		completedbytes = 0;
		n=0;
		do 
		{
			if(ReadFromUsbModule(&resp[completedbytes], expected_length_read -completedbytes, length_read, UsbMultimeterModuleConnectionHd) == 1)
			{
				completedbytes += length_read;
			}
			else
			{
				break;
			}
		} while ( (completedbytes < expected_length_read) && ((n++)<2));

		if (completedbytes > 1)
		{	char *str_ptr = strstr(resp, MULTIMETER_GET_MEASUREMENT_COMMAND_STR);
			char *str_search_ptr;
			
			if ( str_ptr != NULL)
			{
				str_ptr+= sizeof(MULTIMETER_GET_MEASUREMENT_COMMAND_STR)-1;
				
				if (*str_ptr != 0)
				{	for (str_search_ptr=str_ptr; *str_search_ptr != 0; str_search_ptr++)
					{	
						if (((*str_search_ptr) == '\n') || ((*str_search_ptr) == '\r'))
						{	
							break;
						}
						previous_char = *str_search_ptr;
					}
					//make sure the character before '\n' is NOT '-' !
					if (previous_char != '-') 
					{
						memset(value_str,0x0,sizeof(value_str));
						if ((str_search_ptr-str_ptr) < sizeof(value_str))
						{	strncpy(value_str, str_ptr, str_search_ptr-str_ptr);
						}
						else
						{	strncpy(value_str, str_ptr, sizeof(value_str)-1);
						}
						value_str[sizeof(value_str)-1] = 0;

						measured_value = atof(value_str);

						//success_flag = 1;
						return /*measured_value*/ 1;
					}
					else 
					{
						WriteLogFile("ReadMultimeterMeasurement:last character is negative sign,num_of_attempts is %d ", num_of_attempts);
					}
				}
				else
				{
					WriteLogFile("ReadMultimeterMeasurement:%s does not contain any value", resp);
				}
			}
			else
			{
				WriteLogFile("ReadMultimeterMeasurement:%s does not contain %s,num_of_attempts is %d ", resp, MULTIMETER_GET_MEASUREMENT_COMMAND_STR, num_of_attempts);
			}
		}
		else
		{
			WriteLogFile("ReadMultimeterMeasurement:completedbytes is %d", completedbytes);
		}
	} while ( (num_of_attempts++)<10 );

	WriteLogFile("Unable to communicate with BK PRECISION 5, please re-set it by powering it off and on");
	AfxMessageBox("Unable to communicate with BK PRECISION 5, please re-set it by powering it off and on \n", MB_ICONINFORMATION|MB_OK);	

	//success_flag = 0;
	return /*INVALID_MEASUREMENT*/ 0;
}

int OpenUsbModule(int ComPortNumber, HANDLE &UsbDeviceHd)
{
	char portName[20];
	DCB	m_portConfig;
	CString strMessage;
	int n=0;
	
	memset(portName,0x0,sizeof(portName));
	sprintf(portName, "\\\\.\\COM%d", ComPortNumber);
	portName[sizeof(portName)-1] = '\0';

	if (UsbDeviceHd != 0)
	{
		return 1;
	}

	n=0;
	do 
	{	
		Sleep(10);
		UsbDeviceHd = CreateFile(portName,  // Specify port device: default "COM1"
			GENERIC_READ | GENERIC_WRITE,       // Specify mode that open device.
			0,                                  // the devide isn't shared.
			NULL,                               // the object gets a default security.
			OPEN_EXISTING,                      // Specify which action to take on file. 
			0,                                  // default.
			NULL);                              // default.
	}
	while ((UsbDeviceHd == INVALID_HANDLE_VALUE) && ((n++)<2));
	
	if ( UsbDeviceHd == INVALID_HANDLE_VALUE)
	{

		//strMessage.Format("unable to open comport %d", ComPortNumber);
		WriteLogFile("OpenUsbModule:unable to open comport %d", ComPortNumber); 
		UsbDeviceHd = 0;
		return(0);
	}

	 //  Initialize the DCB structure.
    SecureZeroMemory(&m_portConfig, sizeof(DCB));
    m_portConfig.DCBlength = sizeof(DCB);

	// Get current configuration of serial communication port.
	if (GetCommState(UsbDeviceHd,&m_portConfig) == 0)
	{
		WriteLogFile("OpenUsbModule:Get configuration port has problem.");
		CloseHandle(UsbDeviceHd);
		UsbDeviceHd = 0;
		return FALSE;
	}
	
	m_portConfig.BaudRate = CBR_9600;    
	m_portConfig.StopBits = ONESTOPBIT;    
	m_portConfig.Parity = NOPARITY;        
	m_portConfig.ByteSize = 8; 

	if (SetCommState(UsbDeviceHd,&m_portConfig) == 0)
	{
		strMessage.Format(_T("OpenUsbModule:Set configuration port has problem. handle is:  %d"), UsbDeviceHd);
		WriteLogFile(strMessage);
		CloseHandle(UsbDeviceHd);
		UsbDeviceHd = 0;
		return FALSE;
	}

	// instance an object of COMMTIMEOUTS.
	COMMTIMEOUTS comTimeOut;                   
	comTimeOut.ReadIntervalTimeout = 10;
	comTimeOut.ReadTotalTimeoutMultiplier = 10;
	comTimeOut.ReadTotalTimeoutConstant = 10;
	comTimeOut.WriteTotalTimeoutMultiplier = 1000;
	comTimeOut.WriteTotalTimeoutConstant = 1000;

	if (SetCommTimeouts(UsbDeviceHd,&comTimeOut) == 0)
	{	strMessage.Format(_T("OpenUsbModule:Set CommTimeouts has problem. handle is:  %d"), UsbDeviceHd);
		WriteLogFile(strMessage);
		CloseHandle(UsbDeviceHd);
		UsbDeviceHd = 0;
		return FALSE;
	}
	return 1;
}

int CloseUSBHandles(HANDLE &UsbDeviceHd)
{
	if(UsbDeviceHd != 0)
		CloseHandle(UsbDeviceHd);
	UsbDeviceHd = 0;
	return 1;
}

int DetermineCsrDeviceType(uint32 handle)
{	
	int32	iSuccess;
	uint16	data[1], len;
	int		n=0;
	uint16  nap;
	uint8   uap;
	uint32  lap;
	
	n=0;
	do 
	{
		iSuccess = psRead(handle, 702, 0, 1, data, &len); //get the vendor ID
	}
	while ((iSuccess != TE_OK) && ((n++)<5));

	if (iSuccess != TE_OK)
	{
		return UNKNOWN;
	}

	n=0;
	do 
	{
		iSuccess = psReadBdAddr(handle, (uint32 *)&lap, (uint8 *)&uap, (uint16 *)&nap);
	}
	while ((iSuccess != TE_OK) && ((n++)<5));

	if (iSuccess != TE_OK)
	{
		return UNKNOWN;
	}

	if (data[0] == 0x0a12) //csr vendor ID 
	{
		return CSR_REFERENCE_END_POINT;
	}
	/*else if ((data[0] == 0x2794) && (lap == 0x3)) //if bluetooth address is 0x600f77000003, it is the sp transmit module 
	{
		return SILVERPLUS_TRANSMIT_MODULE;
	}*/
	else if (data[0] == 0x2794) //silverpllus vendor ID
	{
		return SILVERPLUS_DUT;
	}
	return UNKNOWN;
}

int ReadDutBluetoothAddr(char *addr)
{
#ifndef DESIGN_MODE
	int32	iSuccess;
	int		n=0;
	uint16  nap;
	uint8   uap;
	uint32  lap;
	
	if (dutHandle == 0)
		return 0;

	n=0;
	do 
	{
		iSuccess = psReadBdAddr(dutHandle, (uint32 *)&lap, (uint8 *)&uap, (uint16 *)&nap);
	}
	while ((iSuccess != TE_OK) && ((n++)<5));
	
	if (iSuccess != TE_OK)
	{
		return 0;
	}

	sprintf(addr, "%04x%02x%06x", nap, uap, lap);
#endif
	return 1;
}

int VerifyDutHandle(uint32 handle)
{
	if (DetermineCsrDeviceType(handle) != SILVERPLUS_DUT)
		return TE_ERROR;
	return TE_OK;
}

int ReadPskey(uint16 size, uint16 psKey, uint16 *data){
#ifndef DESIGN_MODE
	int32	iSuccess;
	int		n=0;
	uint16  length_read;
	
	if (dutHandle == 0)
		return 0;

	n=0;
	do{	
		iSuccess = psRead(dutHandle, psKey, 0, size, data, &length_read);
	}
	while ((iSuccess != TE_OK) && ((n++)<5));

	if (iSuccess == TE_OK){
		return 1;
	}else{

		iSuccess = bccmdSetColdReset(dutHandle, /*5000*/15000);
		if (iSuccess != TE_OK)
			return 0; 
		n=0;
		do{	
			iSuccess = psRead(dutHandle, psKey, 0, size, data, &length_read);
		}
		while ((iSuccess != TE_OK) && ((n++)<5));
		if (iSuccess != TE_OK)
			return 0;
	}
#endif 
	return 1;
}

int SetPskey(uint16 size, uint16 psKey, uint16 *data){
#ifndef DESIGN_MODE
	int32	iSuccess;
	int		n=0;
	
	if (dutHandle == 0)
		return 0;

	n=0;
	do 
	{	
		iSuccess = VerifyDutHandle(dutHandle);
		if (iSuccess == TE_OK)
		{	
			iSuccess = psWrite(dutHandle, psKey, 0, size, data);
		}
	}
	while ((iSuccess != TE_OK) && ((n++)<5));
	
	if (iSuccess != TE_OK)
	{
		return 0;
	}
#endif 
	return 1;
}


int VerifyDutBluetoothAddr(/*uint32 handle, uint16 &nap, uint8 &uap, uint32 &lap*/)
{	
	int32	iSuccess;
#if 0 /* Seavia 20150810 , fix unreferenced local variable warning*/
	uint16	data[1], len;
#endif
	int		n=0;
	uint16  nap;
	uint8   uap;
	uint32  lap;
	
	//if (handle != dutHandle)
		//return 0;

    n=0;

	iSuccess = psReadBdAddr(/*handle*/dutHandle, (uint32 *)&lap, (uint8 *)&uap, (uint16 *)&nap);

	
	if (iSuccess != TE_OK)
	{
		return 0;
	}

	
	if ((nap == DUT_NAP/*0x600f*/) && (uap == /*77*/DUT_UAP) && (lap > 0x1))
	{	
		return 1;
	}

	return 0;
}


void CsrDevicesConnect(int connection_type, int device_type)
{
	char	csr_device_str[11];
	int		n=0;
	uint32	csrDeviceHandle;
	CString strMessage;
	
	if (connection_type == USB_CONNECTION)
	{	for (int csr_device_index=0; csr_device_index<MAX_CSR_DEVICES; csr_device_index++)
		{
			memset(csr_device_str,0x0,sizeof(csr_device_str));
			sprintf(csr_device_str, "\\\\.\\csr%d", csr_device_index);
			csr_device_str[sizeof(csr_device_str)-1] = '\0';

			if (((device_type == SILVERPLUS_DUT) && (csr_device_index != refEndPointCsrDeviceIndex))
				|| 
				((device_type == CSR_REFERENCE_END_POINT) && (csr_device_index != dutCsrDeviceIndex))
				||
				((device_type == SILVERPLUS_TRANSMIT_MODULE) && (csr_device_index != transmitModuleCsrDeviceIndex))
			   )
			{
				n=0;

				do 
				{
					csrDeviceHandle = openTestEngine(USB, csr_device_str, 115200, 1000, 2000);
				}
				while ((csrDeviceHandle == 0) && ((n++)<5));

				if (csrDeviceHandle != 0)
				{	
					if((DetermineCsrDeviceType(csrDeviceHandle) == SILVERPLUS_DUT) 
						 &&
					   (device_type == SILVERPLUS_DUT)
					  )
					{
						dutHandle = csrDeviceHandle;
						dutCsrDeviceIndex = csr_device_index;
						return;
					}
				
					else if((DetermineCsrDeviceType(csrDeviceHandle) == CSR_REFERENCE_END_POINT) 
						     &&
							(device_type == CSR_REFERENCE_END_POINT)
						   )
					{
						referenceEndPointHandle = csrDeviceHandle;
						refEndPointCsrDeviceIndex = csr_device_index;
						return;
					}

					else if((DetermineCsrDeviceType(csrDeviceHandle) == SILVERPLUS_TRANSMIT_MODULE) 
						     &&
							(device_type == SILVERPLUS_TRANSMIT_MODULE)
						   )
					{
						transmitModuleHandle = csrDeviceHandle;
						transmitModuleCsrDeviceIndex = csr_device_index;
						return;
					}
					
					//very important: close the handle since this handle is NOT what we are looking for
					closeTestEngine(csrDeviceHandle);
				}
			}
		}
	}
	else 
	{	
		if (device_type == SILVERPLUS_DUT)
		{
			n=0;
			do 
			{
				csrDeviceHandle = openTestEngineSpi(-1, 0, SPI_USB);
			}
			while ((csrDeviceHandle == 0) && ((n++)</*2*/5));
			
			if (csrDeviceHandle != 0)
			{
				if (DetermineCsrDeviceType(csrDeviceHandle) == SILVERPLUS_DUT)
				{
					dutHandle = csrDeviceHandle;
					return;
				}
			}
		}
		else
		{

		}
	}
}

void CloseDUT()
{
	if (dutHandle != 0)
	{
		closeTestEngine(dutHandle);
		dutHandle = 0;
		dutCsrDeviceIndex = -1;
	}
}
void CsrDevicesClose()
{
	if (dutHandle != 0)
	{
		closeTestEngine(dutHandle);
		dutHandle = 0;
		dutCsrDeviceIndex = -1;
	}

	if (referenceEndPointHandle != 0)
	{
		closeTestEngine(referenceEndPointHandle);
		referenceEndPointHandle = 0;
		refEndPointCsrDeviceIndex = -1;
	}
}

int checkDUTConnection(int connection_type)
{
	if (dutHandle == 0) 
	{	
		CsrDevicesConnect(connection_type, SILVERPLUS_DUT);
		if (dutHandle == 0) 
		{	
			WriteLogFile("DUT is not connected !!!");
			//AfxMessageBox("DUT is not connected, please disconnect and connect DUT if it is already connected\n", MB_ICONINFORMATION|MB_OK);	
			return 0;
		}
	}
	return 1;
}

int checkRefEndPointConnection(int connection_type)
{
	if (referenceEndPointHandle == 0)
	{
		CsrDevicesConnect(connection_type, CSR_REFERENCE_END_POINT);
		
		if (referenceEndPointHandle == 0)
		{	
			WriteLogFile("Ref End Point is not connected");
			AfxMessageBox("Ref End Point is not connected, please disconnect and connect DUT if it is already connected\n", MB_ICONINFORMATION|MB_OK);	
			return 0;
		}
	}
	return 1;
}

int checkTransmitModuleConnection(int connection_type)
{
	if (transmitModuleHandle == 0)
	{
		CsrDevicesConnect(connection_type, SILVERPLUS_TRANSMIT_MODULE);
		
		if (transmitModuleHandle == 0)
		{	
			WriteLogFile("SilverPlus Transmit Module is not connected");
			AfxMessageBox("SilverPlus Transmit Module is not connected, please disconnect and connect SilverPlus Transmit Module if it is already connected\n", MB_ICONINFORMATION|MB_OK);	
			return 0;
		}
	}
	return 1;
}

int changeXtalOffset(int16 offset)
{	
	int32	iSuccess;
	//int		n=0;
	//CString strMessage;

	iSuccess = psWriteXtalOffset(dutHandle, offset);
	
	if(iSuccess != TE_OK)
	{
		//strMessage.Format(_T("DUT psWriteXtalOffset %d failed"), offset);
		WriteLogFile("changeXtalOffset:DUT psWriteXtalOffset %d failed", offset);
		return 0;
	}
	
	iSuccess = bccmdSetColdReset(dutHandle, 1000);

	if(iSuccess != TE_OK)
	{
		WriteLogFile("changeXtalOffset:DUT bccmdSetColdReset failed");
		return 0;
	}

	iSuccess = spradiotestTxstart(dutHandle, MID_FREQUENCY_MHZ, 50, 255, 0);
		
	if (iSuccess =! TE_OK)
	{	
		WriteLogFile("changeXtalOffset:DUT radiotestTxstart failed");
		return 0;
	}
	
	return 1;
}

int CalibrateXtalTrim(int16 &trim)
{	
	float64	OFFSET=0.0;
	int16	new_crystal_offset=0;
	int32	iSuccess;
	int		n=0;
	CString strMessage;	
	trim = 255;

	if (!changeXtalOffset(new_crystal_offset))
	{	
		return 0;
	}
	
	Sleep(100);

	iSuccess = get_freq_offset(referenceEndPointHandle, &OFFSET, OFFSET_AVERAGES);

	if(iSuccess != TE_OK)
	{
		WriteLogFile("CalibrateXtalTrim:RefEp get_freq_offset failed");
		return 0;
	}

	double measuredFreqMHz = MID_FREQUENCY_MHZ + MID_FREQUENCY_MHZ * OFFSET/1000000;
	
	iSuccess = radiotestCalcXtalOffset(MID_FREQUENCY_MHZ, measuredFreqMHz, &new_crystal_offset);

    if(iSuccess != TE_OK)
	{
		WriteLogFile("CalibrateXtalTrim:radiotestCalcXtalOffset failed");
		return 0;
	}



	if (!changeXtalOffset(new_crystal_offset))
	{	
		return 0;
	}
	
	Sleep(100);
	iSuccess = get_freq_offset(referenceEndPointHandle, &OFFSET, OFFSET_AVERAGES);

	if(iSuccess != TE_OK)
	{	
		WriteLogFile("CalibrateXtalTrim:RefEp get_freq_offset failed");
		return 0;
	}
	
	if ((OFFSET >= GetConfigurationValue(CRYSTAL_TRIM_OFFSET_LIMIT_STR) *-1/*-2*/) && (OFFSET <= GetConfigurationValue(CRYSTAL_TRIM_OFFSET_LIMIT_STR)/*2*/))
	{	
		trim = new_crystal_offset;
		return 1;
	}

	strMessage.Format(_T("CalibrateXtalTrim:New offset is: %4.2f, limit is %f"), OFFSET, GetConfigurationValue(CRYSTAL_TRIM_OFFSET_LIMIT_STR));
	WriteLogFile(strMessage);

	return 0;
}

int UsbGpioModuleInit()
{
	int result;
	if(!UsbGpioModuleConnectionHd)
	{
#if 1 /* Seavia 20150910, fix warning, conversion from 'double' to 'int', possible losse of data */
		result = OpenUsbModule((int)GetConfigurationValue(GPIO_COM_NUM_STR), UsbGpioModuleConnectionHd);
#else
		result = OpenUsbModule(GetConfigurationValue(GPIO_COM_NUM_STR), UsbGpioModuleConnectionHd);
#endif
		if(result == 1)
		{
			if(ClearGPIOCMDS() == 1)
				return 1;
		}
	}
	else
	{
		return 1;
	}
	return 0;
}

int UsbMultimeterModuleInit()
{	
	int result=0;
	CString strMessage;

#if 1 /* Seavia 20150910, fix warning, conversion from 'double' to 'int', possible losse of data */
	result = OpenUsbModule((int)GetConfigurationValue(MULTIMETER_COM_NUM_STR), UsbMultimeterModuleConnectionHd);
#else
	result = OpenUsbModule(GetConfigurationValue(MULTIMETER_COM_NUM_STR), UsbMultimeterModuleConnectionHd);
#endif
	if (result != 1)
	{	//strMessage.Format(_T("open usb multimeter module encourters problem"));
		//return result;
	}



	return /*1*/result;
}


int automaticPairingModeDisable(int connection_type)
{
	uint16 psKey[6];
	uint16 lengthRetrieved=0;
	int32 iSuccess;
	int n=0, i=0;

	memset(psKey,0x0,sizeof(psKey));
	if (PeriphiralOpen(PERIPHIRAL_ID_DUT, connection_type))
	{
		n=0;
		do 
		{	
			iSuccess = psSize(dutHandle, PSKEY_USER15, 0 ,&lengthRetrieved);
		}
		while ((iSuccess != TE_OK) && ((n++)<5)); 

		if (iSuccess != TE_OK)
		{
			WriteLogFile("automaticPairingModeDisable: psSize for PSKEY_USER15 on DUT failed.");
			return 0;
		}
#if 0
		n=0;
		do 
		{	
			iSuccess = /*psRead*/sppsRead(dutHandle, PSKEY_USER15, 0, sizeof(psKey), psKey, &lengthRetrieved);
		}
		while ((iSuccess != TE_OK) && ((n++)<5)); 

		if (iSuccess != TE_OK)
		{
			WriteLogFile("automaticPairingModeDisable:psRead for PSKEY_USER15 on DUT failed.");
			return 0;
		}
#endif 
		for (i=0; i<sizeof(psKey)/sizeof(uint16); i++)
		{	
			psKey[i] = psKey15Value[i];
		}

		if((psKey[1] & ENABLE_AUTO_PAIRING_ON_POWER_ON)  || (psKey[2] & ENABLE_AUTO_DISCOVERABLE))
		{
			psKey[1] &= ~ENABLE_AUTO_PAIRING_ON_POWER_ON;
			psKey[2] &= ~ENABLE_AUTO_DISCOVERABLE;

			n=0;
			do 
			{	
				iSuccess = VerifyDutHandle(dutHandle);
				if (iSuccess == TE_OK)
				{	
					iSuccess = /*psWrite*/sppsWrite(dutHandle, PSKEY_USER15, 0, 6, psKey);
				}
			}
			while ((iSuccess != TE_OK) && ((n++)<5)); 
			
			if (iSuccess != TE_OK)
			{
				WriteLogFile("automaticPairingModeDisable:psWrite for PSKEY_USER15 on DUT failed.");
				return 0;
			}

			iSuccess = bccmdSetWarmReset(dutHandle, /*5000*/15000);
			if(iSuccess != TE_OK)
			{
				SetStatus("WARM RESET FAIL");
				return 0;
			}
			return 1;
		}
		return 1;
	}
	else
	{	
		WriteLogFile("automaticPairingModeDisable: open DUT failed.");
	}
	return 0;
}

int automaticPairingModeEnable(int connection_type, int warmresetFlag)
{
	uint16 psKey[6];
	uint16 lengthRetrieved=0;
	int32 iSuccess;
	int n=0, i=0;

	memset(psKey,0x0,sizeof(psKey));
	if (PeriphiralOpen(PERIPHIRAL_ID_DUT, connection_type))
	{
		n=0;
		do
		{
			iSuccess = psSize(dutHandle, PSKEY_USER15, 0 ,&lengthRetrieved);
		}
		while ((iSuccess != TE_OK) && ((n++)<5));

		if (iSuccess != TE_OK)
		{	int error = teGetLastError(dutHandle);
			WriteLogFile("automaticPairingModeEnable:psSize for PSKEY_USER15 on DUT failed.");
			return 0;
		}
#if 0
		n=0;
		do
		{
			iSuccess = /*psRead*/sppsRead(dutHandle, PSKEY_USER15, 0, sizeof(psKey), psKey, &lengthRetrieved);
		}
		while ((iSuccess != TE_OK) && ((n++)<5));

		if (iSuccess != TE_OK)
		{
			WriteLogFile("automaticPairingModeEnable:psRead for PSKEY_USER15 on DUT failed.");
			return 0;
		}
#endif 

		for (i=0; i<sizeof(psKey)/sizeof(uint16); i++)
		{	
			psKey[i] = psKey15Value[i];
		}
		
		//if((!(psKey[1] & ENABLE_AUTO_PAIRING_ON_POWER_ON))||(!(psKey[2] & ENABLE_AUTO_DISCOVERABLE)))
		//{
			//psKey[1] |= ENABLE_AUTO_PAIRING_ON_POWER_ON;
			//psKey[2] |= ENABLE_AUTO_DISCOVERABLE;
			
			n=0;
			do
			{
				iSuccess = VerifyDutHandle(dutHandle);
				if (iSuccess == TE_OK)
				{	
					iSuccess = /*psWrite*/sppsWrite(dutHandle, PSKEY_USER15, 0, 6, psKey);
				}
			} while ((iSuccess != TE_OK) && ((n++)<5));
		
			if (iSuccess != TE_OK)
			{
				WriteLogFile("automaticPairingModeEnable:psWrite for PSKEY_USER15 on DUT failed.");
				return 0;
			}


			if(warmresetFlag)
			{
				iSuccess = bccmdSetWarmReset(dutHandle, /*5000*/15000);
				if(iSuccess != TE_OK)
				{
					SetStatus("AUTOPAIRENABLE RESET FAIL");
					return 0;
				}
			}
			return 1;
		/*}
		else
		{
			return 1;
		}*/
	}
	return 0;
}

int disableEnterDFUModeAfterPowerOn()
{
#if 1 /* Seavia 20150810 , fix unreferenced local variable warning*/
	uint16 psKeyForBootMode, psKeyForUsr33;
#else
	uint16 pskey_length_read, psKeyForBootMode, psKeyForUsr33;
#endif
	int n;
	int32 iSuccess;
	uint16 length_read=0;

	if (ReadPskey(1, PSKEY_INITIAL_BOOTMODE, &psKeyForBootMode) == 1)
	{
		if (psKeyForBootMode == 0x0) //this is FW 1.60 which enter DFU mode after PowerOn
		{
			//set USR33 to '1' so we know to restore enter DFU mode after PowerOn during 'factory enable'
			psKeyForUsr33 = 0x1; 
			if (SetPskey(1, PSKEY_USER33, &psKeyForUsr33)) 
			{
				n=0;
				do 
				{	
					iSuccess = psRead(dutHandle, PSKEY_USER33, 0, 1, &psKeyForUsr33, &length_read);
				}
				while ((iSuccess != TE_OK) && ((n++)<5));

				if (iSuccess != TE_OK)
				{
					SetStatus("Read USR33 Pskey: Fail");
					return 0;
				}

				//confirm USR33 is set to '1'
				if (psKeyForUsr33 != 0x1) 
				{
					SetStatus("USR33 NOT 1");
					return 0;
				}

				//set BOOTMODE to '1' to DISABLE enter DFU mode after PowerOn
				psKeyForBootMode = 0x1;
				if (SetPskey(1, PSKEY_INITIAL_BOOTMODE, &psKeyForBootMode))
				{
					n=0;
					do 
					{	
						iSuccess = psRead(dutHandle, PSKEY_INITIAL_BOOTMODE, 0, 1, &psKeyForBootMode, &length_read);
					}
					while ((iSuccess != TE_OK) && ((n++)<5));

					if (iSuccess != TE_OK)
					{
						SetStatus("Read BootMode Pskey: Fail");
						return 0;
					}

					//confirm BOOTMODE is set to '1'
					if (psKeyForBootMode != 0x1)
					{	
						SetStatus("BootMode NOT 1");
						return 0;
					}

					return 1; 
				}
				else
				{
					SetStatus("Set BootMode Pskey: Fail");
					return 0;
				}
			}
			else
			{	
				SetStatus("Set USR33 Pskey: Fail");
				return 0;
			}
		}
		else //this is NOT FW 1.60, it doesn't enter DFU mode after PowerOn, so don't do anything
		{
			return 1; 
		}
	}
	else
	{
		SetStatus("Read BootMode Pskey: Fail");
		return 0;
	}

	//should NEVER be reached 
	return 0;
}

int enableEnterDFUModeAfterPowerOn()
{
#if 1 /* Seavia 20150810 , fix unreferenced local variable warning*/
	uint16 psKeyForBootMode, psKeyForUsr33;
#else
	uint16 pskey_length_read, psKeyForBootMode, psKeyForUsr33;
#endif
	int n; 
	int32 iSuccess;
	uint16 length_read=0;

	if (ReadPskey(1, PSKEY_USER33, &psKeyForUsr33) == 1)
	{
		//if USR33 is set to '1', need to restore (ENABLE) enter DFU mode after PowerOn during 'factory enable'
		if (psKeyForUsr33 == 0x1)
		{
			//set BOOTMODE to '0' to ENABLE enter DFU mode after PowerOn
			psKeyForBootMode = 0;
			if (SetPskey(1, PSKEY_INITIAL_BOOTMODE, &psKeyForBootMode))
			{
				n=0;
				do 
				{	
					iSuccess = psRead(dutHandle, PSKEY_INITIAL_BOOTMODE, 0, 1, &psKeyForBootMode, &length_read);
				}
				while ((iSuccess != TE_OK) && ((n++)<5));

				if (iSuccess != TE_OK)
				{
					SetStatus("Read BootMode Pskey: Fail");
					return 0;
				}

				//confirm BOOTMODE is set to '0'
				if (psKeyForBootMode != 0)
				{	
					SetStatus("BootMode NOT 0");
					return 0;
				}

				//delete USR33
				n=0;
				do 
				{
					iSuccess = VerifyDutHandle(dutHandle);
					if (iSuccess == TE_OK)
					{
						iSuccess = psClear(dutHandle, PSKEY_USER33, 0);
					}
				}
				while ((iSuccess != TE_OK) && ((n++)<5));

				if (iSuccess  != TE_OK)
				{	
					SetStatus("Clear USR33: Fail");
					return 0;
				}
				
				return 1;
			}
			else
			{	
				SetStatus("Set BootMode Pskey: Fail");
				return 0;
			}
		}

		//USR33 is NOT '1', don't do anything 
		return 1; 
	}
	else
	{
		//SetStatus("Read USR33 Pskey: Fail");
		//can NOT read USR33, don't do anything 
		return 1;
	}

	//should NEVER be reached 
	return 1;
}


int PRODUCT_Initialization(){
	uint16 psKey[2], psKeyForBootMode=1, psKeyForUsr33=1;
	int n=0;
	uint16  nap;
	uint8   uap;
	uint32  lap;
	int32	iSuccess;

	if(PeriphiralOpen(PERIPHIRAL_ID_GPIOBOARD, USB_CONNECTION)){
		for (int i=0; i<5; i++){
			if(!ChangeGPIOState(USB_ENABLE_PIN_NUM, HIGH))
			{
				SetStatus("Fail To SET USB_ENABLE_PIN_NUM to HIGH");
				WriteMainLogFile("Fail To SET USB_ENABLE_PIN_NUM to HIGH");
				return 0;
			}
			if(!ChangeGPIOState(VCHARGE_IO_PIN_NUM, HIGH))
			{
				SetStatus("Fail To SET VCHARGE_IO_PIN_NUM to HIGH");
				WriteMainLogFile("Fail To SET VCHARGE_IO_PIN_NUM to HIGH");
				return 0;
			}
			
			//vbat_state = HIGH;
			Sleep(2000); //must wait for at least 2 seconds for the chip to start up after VBAT is turned on. */

			if(PeriphiralOpen(PERIPHIRAL_ID_DUT, USB_CONNECTION)){
				n=0;
				do 
				{
					iSuccess = psReadBdAddr(/*handle*/dutHandle, (uint32 *)&lap, (uint8 *)&uap, (uint16 *)&nap);
				}
				while ((iSuccess != TE_OK) && ((n++)<5));
				
				if (iSuccess != TE_OK) 
				{	
					SetStatus("ReadBtAddr: Fail");
					WriteMainLogFile("Fail To SET VCHARGE_IO_PIN_NUM to HIGH");
					return 0;
				}
			
				if ((nap == DUT_NAP) && (uap == DUT_UAP) && (lap == 0x1))
				{	
					if (disableEnterDFUModeAfterPowerOn() != 1)
					{
						SetStatus("disableDFUModeAfterPowerOn: Fail");
						WriteMainLogFile("Fail To SET VCHARGE_IO_PIN_NUM to HIGH");
						return 0;
					}
				}

				if (automaticPairingModeDisable(USB_CONNECTION) != 1)
				{
					SetStatus("automaticPairingModeDisable: Fail");
					WriteMainLogFile("Fail To SET VCHARGE_IO_PIN_NUM to HIGH");
					return 0;
				}
#if 1 /*Seavia 20150903, add Project information, store in PSKEY_USER34 */
				uint16 prjInfo[2] = {0};
				uint16 model = 0xFFFF;
				uint16 FirmwareVersion = {0};
				CString ModelFWInformation;
				projectInformationStr = "PRJ_INFO: ";
				if (ReadPskey(2, PSKEY_USER34, prjInfo) == 1){
					model = prjInfo[0];
					FirmwareVersion = prjInfo[1];
					ModelFWInformation.Format("%sV%d", projectModelInfo[model], FirmwareVersion);
					projectInformationStr.Format("PRJ_INFO: %s", ModelFWInformation);
					WriteMainLogFile("%s: ReadPsKey(PSKEY_USER34) model:%d(%d) FW:%d", __FUNCTION__, model, selectedModel, FirmwareVersion);
					if (selectedModel != model){
						if (model == 0xFFFF){
							WriteMainLogFile("%s: The Version Before v2.23", __FUNCTION__);
						}else{
							WriteMainLogFile("%s: Wrong Verison with selected item", __FUNCTION__);
							return 0;
						}
					}
				}else{
					WriteMainLogFile("%s: ReadPsKey(PSKEY_USER34) FAIL.", __FUNCTION__);
					projectInformationStr.Format("PRJ_INFO: read pskey_user34 fail");
				}
#endif
				uint32 currentSerialNumber = 0;

				if (ReadPskey(2, PSKEY_USER40, psKey) == 1){
						currentSerialNumber = psKey[0] << 16;
						currentSerialNumber += psKey[1];
				}else{
					WriteMainLogFile("%s: ReadPsKey(PSKEY_USER40) FAIL", __FUNCTION__);
				}
				if (currentSerialNumber > 0){
						serialNumber = currentSerialNumber;
						_snprintf(serialNumberArray, sizeof(serialNumberArray)-1, "SN#: %04x %04x", ((serialNumber >> 16)&0xFFFF), serialNumber&0xFFFF);
						updateSerialNumberTextBox = 1;
						WriteMainLogFile("%s: Get New SN", __FUNCTION__);
				}else{
					if(GetConfigurationValue(PRODUCT_IGNORE_SERIAL_NUMBER_CHECK_STR) != 1){
						serialNumber = 0;
						SetStatus("Fail to Read Serial Number");
						return 0;
					}
						
				}
				return 1;
			}
			else
			{
				//SetStatus("Fail To Open DUT"); //modified for VC2
				//return 0; //modified for VC2
			}

			if(!ChangeGPIOState(VBAT_IO_PIN_NUM, LOW))
			{
				SetStatus("Fail To SET VBAT_IO_PIN_NUM to LOW");
				return 0;
			}
			if(!ChangeGPIOState(USB_ENABLE_PIN_NUM, LOW))
			{
				SetStatus("Fail To SET USB_ENABLE_PIN_NUM to LOW");
				return 0;
			}
			if(!ChangeGPIOState(VCHARGE_IO_PIN_NUM, LOW))
			{
				SetStatus("Fail To SET VCHARGE_IO_PIN_NUM to LOW");
				return 0;
			}
			
		}
	}
	SetStatus("Fail To Open GPIO Board"); //modified for VC2
	return 0;
}

int PCBTEST_Initialization()
{
#ifndef DESIGN_MODE
	int vcharge_state, vbat_state;

	if(PeriphiralOpen(PERIPHIRAL_ID_GPIOBOARD, SPI_CONNECTION))
	{
		if(!ChangeGPIOState(BUTTON_1_PIN_NUM, HIGH))
		{
			SetStatus("Fail To SET BUTTON 1 to HIGH");
			return 0;
		}
		if(!ChangeGPIOState(BUTTON_2_PIN_NUM, HIGH))
		{
			SetStatus("Fail To SET BUTTON 2 to HIGH");
			return 0;
		}

		if(!ChangeGPIOState(VCHARGE_IO_PIN_NUM, LOW))
		{
			SetStatus("Fail To SET VCHRG to LOW");
			return 0;
		}
		vcharge_state = LOW;
		if(!ChangeGPIOState(VBAT_IO_PIN_NUM, LOW))
		{
			SetStatus("Fail To SET VBAT to LOW");
			return 0;
		}
		vbat_state = LOW;
		Sleep(3000);
		if(!ChangeGPIOState(VBAT_IO_PIN_NUM, HIGH))
		{
			SetStatus("Fail To SET VCHRG to HIGH");
			return 0;
		}
		vbat_state = HIGH;
	}
	else
	{
		SetStatus("Fail To Open GPIO");
		return 0;
	}
	
	if(PeriphiralOpen(PERIPHIRAL_ID_DUT, SPI_CONNECTION))
	{
		if (automaticPairingModeDisable(SPI_CONNECTION) != 1)
		{
			SetStatus("automaticPairingModeDisable FAIL");
			return 0;
		}
	}
	else
	{
		SetStatus("Fail To Open DUT");
		return 0;
	}
#endif
#if 1 /*Seavia 20150903, add Project information, store in PSKEY_USER34 */
	uint16 psKey[2]={0};
	uint16 prjInfo[2]={0};

	uint16 model = 0;
	uint16 FirmwareVersion = 0;
	uint32 currentSerialNumber = 0;
	CString ModelFWInformation;
	projectInformationStr = "PRJ_INFO: ";

	if (ReadPskey(2, PSKEY_USER34, prjInfo) == 1){
		
		model = prjInfo[0];
		FirmwareVersion = prjInfo[1];

		ModelFWInformation.Format("%sV%d", projectModelInfo[model], FirmwareVersion);
		projectInformationStr.Format("PRJ_INFO: %s", ModelFWInformation);

		if ( 1 == MatchNameWithConfigFile(CT2A((LPCSTR)ModelFWInformation), VERSION_CONFIG_FILE_PATH)){
			WriteMainLogFile ("%s : ReadPsKey Succeed. model:%d,%d FW:%d,%d, selectedModel = %d",
				__FUNCTION__, model, prjInfo[0], FirmwareVersion, prjInfo[1], selectedModel);
		}else{
			WriteLogFile("%s is wrong fw version", ModelFWInformation);
			SetStatus("%s is wrong fw version", ModelFWInformation);
			return 0;
		}
	}else{
		SetStatus("Read pskey_user34 fail");
		WriteMainLogFile ("PCBTEST_Initialization : ReadPsKey(PSKEY_USER34) FAIL.");
		//GetDlgItem(IDC_STATIC_PRJ_INFO)->SetWindowText(projectInformationStr);
	}
#endif

	if (ReadPskey(2, PSKEY_USER40, psKey) == 1)
	{
		currentSerialNumber = psKey[0] << 16;
		currentSerialNumber += psKey[1];
	}

	//only write the serial number to PSKEY_USER40 if there is no serial number stored 
	if (currentSerialNumber <= 0)
	{	
		if ( (availSerialNumber <= 0) || 
			!((availSerialNumber >= startingAvailSerialNumber) && (availSerialNumber <= endingAvailSerialNumber))
			){	
				//serialNumber = 0;
				SetStatus("Out Of Serial Number");
				return 0;
		}
		else
		{
			psKey[0] = (availSerialNumber >> 16)&0xFFFF;
			psKey[1] = availSerialNumber&0xFFFF;
	 
			if (SetPskey(2, PSKEY_USER40, psKey))
			{	
				serialNumber = availSerialNumber;
				availSerialNumber++;
				WriteSerialNumberFile(ASSIGNED_SERIAL_NUMBERS_FILENAME);
				_snprintf(serialNumberArray, sizeof(serialNumberArray)-1, "SN#: %04x %04x", ((serialNumber >> 16)&0xFFFF), serialNumber&0xFFFF);
				updateSerialNumberTextBox = 1;
			}

			else
			{
				updateSerialNumberTextBox = 2; 
				serialNumber = 0;
				SetStatus("Serial Number Write Fail");
				return 0;
					//GetDlgItem(pcbTestResultsDetailsLabelsIDs[i])->SendMessage(WM_CTLCOLORSTATIC );
			}
		}
		
	}
	else
	{	
		serialNumber = currentSerialNumber;
		_snprintf(serialNumberArray, sizeof(serialNumberArray)-1, "SN#: %04x %04x", ((serialNumber >> 16)&0xFFFF), serialNumber&0xFFFF);
		updateSerialNumberTextBox = 3;
	
	}
	
	if (ReadDutBluetoothAddr(currentBluetoothAddressString))
	{
		currentBluetoothAddrDisplayStr = "CURR_BT_ADDR: 0x";
		currentBluetoothAddrDisplayStr += /*currentBluetoothAddrStr*/currentBluetoothAddressString;
	}
	else
	{	
		currentBluetoothAddrDisplayStr = "CURR_BT_ADDR: Unknown";
		SetStatus("Fail To Read BT_ADDR DUT");
		return 0;
	}
	
	return 1;
}

/*
int PCBTEST_Oled()
{	
	int iAnswer;
	if(PeriphiralOpen(PERIPHIRAL_ID_DUT, SPI_CONNECTION))
	{             
		iAnswer = AfxMessageBox("Is display ok?", MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2);
		if(iAnswer == IDYES) 
		{	
			return 1;
		}
		return 0;
	}
	else
	{
		SetStatus("Fail To Open DUT");
		return 0;
	}
}
*/
int PCBTEST_MeasurePowerSupply()
{
	//int result=0;
	double io_18_adc_value, io_135_adc_value, io_33_adc_value; 
	//CString strMessage;
#ifdef NO_GPIO_BOARD 
	SetStatus("NO GPIO Board");
	return 1;
#endif
	if(PeriphiralOpen(PERIPHIRAL_ID_GPIOBOARD, SPI_CONNECTION))
	{
		if((ReadGPIOValue(V18_IO_PIN_NUM, io_18_adc_value, ANALOG_IO) == 1) &&
			(ReadGPIOValue(V135_IO_PIN_NUM, io_135_adc_value, ANALOG_IO) == 1) &&
			(ReadGPIOValue(V33_IO_PIN_NUM, io_33_adc_value, ANALOG_IO) == 1) )
		{
			SetStatus("V18:%1.2f, V33:%1.2f, V135:%1.2f",io_18_adc_value,io_33_adc_value,io_135_adc_value);
			if (
				   ((io_18_adc_value >= GetConfigurationValue(V18_POWER_SUPPLY_MIN_LIMIT_STR)) 
				    && (io_18_adc_value <= GetConfigurationValue(V18_POWER_SUPPLY_MAX_LIMIT_STR)))

			    && ((io_33_adc_value >= GetConfigurationValue(V33_POWER_SUPPLY_MIN_LIMIT_STR)) 
				    && (io_33_adc_value <= GetConfigurationValue(V33_POWER_SUPPLY_MAX_LIMIT_STR)))

			    && ((io_135_adc_value >= GetConfigurationValue(V135_POWER_SUPPLY_MIN_LIMIT_STR)) 
				    && (io_135_adc_value <= GetConfigurationValue(V135_POWER_SUPPLY_MAX_LIMIT_STR)))
			   )
			{	
				return 1;
			}
		}
		else
		{	
			SetStatus("Fail To Read ADC");
		}
	}
	else
	{	
		SetStatus("Fail To Open GPIO");
	}
	return 0;
}

int PCBTEST_CalCrystalOffset()
{
	int32	iSuccess;
	int		calibrate_crystal_trim_result = 0;
	int		n=0;
	int16	trimvalue;

	if(PeriphiralOpen(PERIPHIRAL_ID_GPIOBOARD|PERIPHIRAL_ID_DUT|PERIPHIRAL_ID_REFENDPOINT, SPI_CONNECTION))
	{
		iSuccess = spradiotestRxdata1(referenceEndPointHandle, MID_FREQUENCY_MHZ, 0, 0);
		if((iSuccess != TE_OK))
		{
			iSuccess = bccmdSetColdReset(referenceEndPointHandle, 2000);
			if(iSuccess != TE_OK)
			{	
				SetStatus("REFEP CLDRST Fail");
				return 0;
			}
			iSuccess = spradiotestRxdata1(referenceEndPointHandle, MID_FREQUENCY_MHZ, 0, 0);
			if((iSuccess != TE_OK))
			{	
				SetStatus("REFEP SETRX Fail");
				return 0;
			}
		}

		n=0;
		do 
		{
			calibrate_crystal_trim_result = CalibrateXtalTrim(trimvalue);
		} while ((!calibrate_crystal_trim_result) && ((n++)<4/*2*/));

		if (!calibrate_crystal_trim_result)
		{	
			SetStatus("CRYSTAL TRIM Fail");
			return 0;
		}
		SetStatus("TRIM VAL %d",trimvalue);
		return 1;
	}
	return 0;
}

int RFTransmitPowerTestAtThisFreq(int freq, float64 &txPowervalue)
{	
	CString	strMessage;
	int32	iSuccess;
	float64 rssiChip, rssiDbm;
	uint16	rssi_samples[MAX_RSSI_SAMPLE_SIZE], sum;
	double	ave;
	int		n=0;
	
	txPowervalue = -255;
	WriteLogFile("RFTransmitPowerTestAtThisFreq %d:", freq);
#if 1 /* Seavia 20150910, fix warning, conversion from 'double' to 'int', possible losse of data */
	int rssi_sample_size = (int) GetConfigurationValue(RSSI_SAMPLE_SIZE_STR);
#endif
#if 1 /* Seavia 20150910, fix warning, conversion from 'double' to 'int', possible losse of data */
	uint16 intPA = (uint16) GetConfigurationValue(DUT_TRANSMIT_INTERNAL_PA_VALUE_STR);
	iSuccess = spradiotestTxstart(dutHandle, freq, intPA/*50*/, /*0*/255, 0); 
#else
	iSuccess = spradiotestTxstart(dutHandle, freq, GetConfigurationValue(DUT_TRANSMIT_INTERNAL_PA_VALUE_STR)/*50*/, /*0*/255, 0); 
#endif
	if(iSuccess != TE_OK)
	{	
		WriteLogFile("RFTransmitPowerTestAtThisFreq:Setting DUT to tx mode (radiotestTxstart) failed");
		return -1;
	}

	//Sleep(5000);
	Sleep(500);
	n=0;
	do 
	{	
#if 1 /* Seavia 20150910, fix warning, conversion from 'double' to 'uint16', possible losse of data */
		//uint16 sample_size = (uint16) GetConfigurationValue(RSSI_SAMPLE_SIZE_STR);
		iSuccess = radiotestRxstart2(referenceEndPointHandle, freq, 0, 0, (uint16)rssi_sample_size);
#else
		iSuccess = radiotestRxstart2(referenceEndPointHandle, freq, 0, 0, GetConfigurationValue(RSSI_SAMPLE_SIZE_STR));
#endif
	} while ((iSuccess != TE_OK) && ((n++)<5)); 

	if (iSuccess != TE_OK)
    {
		WriteLogFile("RFTransmitPowerTestAtThisFreq:Setting reference end point to rx mode failed");
		return -1;
	}

	// Approx. 10 samples per second, plus some headroom
#if 1 /* Seavia 20150910, fix warning, conversion from 'double' to 'int', possible losse of data */
	//int rssi_sample_size = (int) GetConfigurationValue(RSSI_SAMPLE_SIZE_STR);
    int timeoutMs = 500 + (100 * rssi_sample_size);
#else
    int timeoutMs = 500 + (100 * GetConfigurationValue(RSSI_SAMPLE_SIZE_STR));
#endif
	n=0;
	do 
	{
#if 1 /* Seavia 20150910, fix warning, conversion from 'double' to 'uint16', possible losse of data */
		//rssi_sample_size = (int) GetConfigurationValue(RSSI_SAMPLE_SIZE_STR);
		iSuccess = hqGetRssi(referenceEndPointHandle, /*50000*//*1500*/timeoutMs, (uint16)rssi_sample_size, rssi_samples);
#else
		iSuccess = hqGetRssi(referenceEndPointHandle, /*50000*//*1500*/timeoutMs, GetConfigurationValue(RSSI_SAMPLE_SIZE_STR), rssi_samples);
#endif
	} while ((iSuccess != TE_OK) && ((n++)<3)); 

	if(iSuccess != TE_OK)
	{
		WriteLogFile("RFTransmitPowerTestAtThisFreq %d:Getting RSSI from reference end point failed", freq);
		return -1;
	}
	
	newlineflag = 0;
	timestampflag = 1;
	WriteLogFile("%s ", "RSSI_SAMPLES: ");
		
	timestampflag = 0;
	
	sum = 0;
#if 1 /* Seavia 20150910, fix warning, conversion from 'double' to 'int', possible losse of data */
	for (int sample_index=0; sample_index<(rssi_sample_size-1); sample_index++)
#else
	for (int sample_index=0; sample_index<(GetConfigurationValue(RSSI_SAMPLE_SIZE_STR)-1); sample_index++)
#endif
	{	sum += rssi_samples[sample_index];
		WriteLogFile("%d ", rssi_samples[sample_index]);
	}
	WriteLogFile("\n");

	newlineflag = 1;
	timestampflag = 1;

	ave = sum / (GetConfigurationValue(RSSI_SAMPLE_SIZE_STR)-1);
	rssiChip = ave;
	
	n=0;
	do 
	{	iSuccess = refEpGetRssiDbm(referenceEndPointHandle, freq, rssiChip, &rssiDbm);
	} while ((iSuccess != TE_OK) && ((n++)<GetConfigurationValue(NUM_OF_TIMES_CALCULATE_RSSI_VALUE_STR))); 

	if(iSuccess != TE_OK)
	{
		WriteLogFile("RFTransmitPowerTestAtThisFreq:Getting RSSI Dbm from reference end point failed");
		return -1;
    }

	rssiDbm = rssiDbm + /*-PATH_LOSS*/GetConfigurationValue(ATTENUATION_STR);
	txPowervalue = rssiDbm;
	if ((rssiDbm >= GetConfigurationValue(DUT_OUTPUT_POWER_MIN_LIMIT_STR)) && (rssiDbm <= GetConfigurationValue(DUT_OUTPUT_POWER_MAX_LIMIT_STR)))
	{

		return 1;
	}
	return /*-1*/0;
}

int RFTransmitPowerTest()
{
	int result=0, n=0;

	float64 txpowerLow, txpowerMid, txpowerHigh;
	int resultlow, resultMid, resultHigh;
	int32	iSuccess;

	iSuccess = bccmdSetColdReset(referenceEndPointHandle, 2000);
	if (iSuccess != TE_OK)
	{	SetStatus("ColdReset REF Fail");
		WriteLogFile("RFTransmitPowerTest:bccmdSetColdReset on REF END POINT failed."); 
		return -1;
	}

	iSuccess = bccmdSetColdReset(dutHandle, /*5000*/15000);
	if (iSuccess != TE_OK)
	{	SetStatus("ColdReset DUT Fail");
		WriteLogFile("RFTransmitPowerTest:bccmdSetColdReset on DUT failed."); 
		return -1;
	}

	n=0;
	do 
	{	resultlow = RFTransmitPowerTestAtThisFreq(LOW_FREQUENCY_MHZ, txpowerLow);
	} while ((resultlow == -1) && ((n++)<2));

	n=0;
	do 
	{	resultMid = RFTransmitPowerTestAtThisFreq(MID_FREQUENCY_MHZ, txpowerMid);
	} while ((resultMid == -1) && ((n++)<2));
	
	n=0;
	do 
	{	resultHigh = RFTransmitPowerTestAtThisFreq(HIGH_FREQUENCY_MHZ, txpowerHigh);
	} while ((resultHigh == -1) && ((n++)<2));
	
	
	//pause any running radio tests, this fixes issues with Gemini
	radiotestPause(dutHandle);
	iSuccess = spradiotestPause(referenceEndPointHandle);
	if (iSuccess != TE_OK)
	{	
		iSuccess = bccmdSetColdReset(referenceEndPointHandle, 2000);
		if (iSuccess != TE_OK)
		{	SetStatus("ColdReset REF Fail");
			WriteLogFile("RFTransmitPowerTest:bccmdSetColdReset on REF END POINT failed."); 
			return -1;
		}
	}
	//bccmdSetWarmReset(dutHandle, /*5000*/15000);
	
	SetStatus("Atn:%.0f,H:%3.2f,M:%3.2f,L:%3.2f", GetConfigurationValue(ATTENUATION_STR), txpowerHigh,txpowerMid,txpowerLow);
	if((resultHigh == 1) && (resultMid == 1) && (resultlow == 1))
	{	
		return 1;
	}
	else
		return 0;
	return 0;
}

int PRODUCT_TEST_RFTransmitPowerTest()
{
	int result=0, retries=3;

	if(PeriphiralOpen(PERIPHIRAL_ID_DUT|PERIPHIRAL_ID_REFENDPOINT, USB_CONNECTION))
	{
		do 
		{	
			result = RFTransmitPowerTest();
			if (result != SUCCESS) 
			{
				WriteLogFile("PRODUCT_TEST_RFTransmitPowerTest retry %d", retries);
			}
		} while ((result != SUCCESS) && (retries-- > 0)); 
		
		return /*RFTransmitPowerTest()*/result;
	}
	return 0;
}

int PCBTEST_RFTransmitPowerTest()
{
	int result=0;

	//float64 txpowerLow, txpowerMid, txpowerHigh;
	//int resultlow, resultMid, resultHigh;

	if(PeriphiralOpen(PERIPHIRAL_ID_GPIOBOARD|PERIPHIRAL_ID_DUT|PERIPHIRAL_ID_REFENDPOINT, SPI_CONNECTION))
	{
		return RFTransmitPowerTest();
	}
	return 0;
}

int ber1, ber0;
int RFReceiveSensitivityTestAtThisFreq(/*int freq,*/ float64 &BER, double &packets_loss_percentage/*, int test_type*/)
{
	CString	strMessage;
	int32	iSuccess;
#if 0 /* Seavia 20150810 , fix unreferenced local variable warning*/
	uint16	intPaVal;
	float64 powerActual;
#endif
	int		n=0;
	uint32	BER_array[9];
	packets_loss_percentage=100;
	BER = 100;

    //configure the packet types
	//if (test_type == PCB_TEST)
	//{	radiotestCfgPkt(dutHandle, /*15*/11, /*339*/100);
		//radiotestCfgPkt(referenceEndPointHandle, /*15*/11, /*339*/100);
	//}
	//else if (test_type == PRODUCT_TEST)
	//{	radiotestCfgPkt(dutHandle, 29, 50);
		//radiotestCfgPkt(transmitModuleHandle, 29, 50);
	//}

#if 0	
	radiotestCfgPkt(dutHandle, 4/*5*//*15*//*11*/, 27/*10*//*339*//*100*/);
	radiotestCfgPkt(referenceEndPointHandle, 4/*5*//*15*//*11*/, 27/*10*//*339*//*100*/);

  
	//configure the tx/rx interval
    radiotestCfgFreq(dutHandle, 3750/*37500/12500*/, /*9375*/1875, 1);
	
	//if (test_type == PCB_TEST)
	//{	
		radiotestCfgFreq(referenceEndPointHandle, 3750/*37500/12500*/, /*9375*/1875, 1);
	//}
	//else if (test_type == PRODUCT_TEST)
	//{	radiotestCfgFreq(transmitModuleHandle, 3750/*37500/12500*/, /*9375*/1875, 1); 
	//}

	//if (test_type == PCB_TEST)
	//{	
		iSuccess = refEpGetPaLevel(referenceEndPointHandle, freq, GetConfigurationValue(REF_END_POINT_OUTPUT_POWER_STR)/*TARGET_OUTPUT_POWER - PATH_LOSS*/, &intPaVal, &powerActual);
		
		if(iSuccess != TE_OK)
		{	
			WriteLogFile("RFReceiveSensitivityTestAtThisFreq:Reference end point configure output power (refEpGetPaLevel) failed");
			return -1;
		}

		iSuccess = radiotestTxdata1(referenceEndPointHandle, freq, intPaVal, 0);
		if(iSuccess != TE_OK)
		{	
			WriteLogFile("RFReceiveSensitivityTestAtThisFreq:Reference end point set to tx mode (radiotestTxdata1) failed");
			//WriteLogFile("RFReceiveSensitivityTestAtThisFreq:transmitModule set to tx mode (radiotestTxdata1) failed");
			return -1;
		}
	/*}
	else if (test_type == PRODUCT_TEST)
	{	intPaVal = GetConfigurationValue(SP_MODULE_TRANSMIT_INTERNAL_PA_VALUE_STR);
		iSuccess = radiotestTxdata1(/*referenceEndPointHandle*//*transmitModuleHandle, freq, GetConfigurationValue(SP_MODULE_TRANSMIT_INTERNAL_PA_VALUE_STR), 0);
		if(iSuccess != TE_OK)
		{	
			//WriteLogFile("RFReceiveSensitivityTestAtThisFreq:Reference end point set to tx mode (radiotestTxdata1) failed");
			WriteLogFile("RFReceiveSensitivityTestAtThisFreq:transmitModule set to tx mode (radiotestTxdata1) failed");
			return -1;
		}
	}*/
	//Sleep(5000);

	iSuccess = radiotestBer1(dutHandle, freq, 0, 0, 10000);

	if (iSuccess != TE_OK)
    {
		WriteLogFile("RFReceiveSensitivityTestAtThisFreq:Dut set to BER receive mode (radiotestBer1) failed");
		return -1;
	}

	int count=0;
	do
    {
		iSuccess = hqGetBer(dutHandle, 2000, BER_array);
		Sleep(10);
		count++;
    } while(iSuccess == TE_ERROR && count</*20*/4);
	
	if (iSuccess != TE_OK)
    {
		WriteLogFile("RFReceiveSensitivityTestAtThisFreq:Dut get BER (hqGetBer) failed");
		return -1;
	}
#endif 

#if 0

	radiotestCfgPkt(dutHandle, 4, 27);
	radiotestCfgPkt(referenceEndPointHandle, 4, 27);

	//configure the tx/rx interval
    radiotestCfgFreq(dutHandle, 6250, 6875, 1);
	
	radiotestCfgFreq(referenceEndPointHandle, 6250, 6875, 1);
	
	iSuccess = radiotestBerLoopback(referenceEndPointHandle, freq, 50, 255, /*5000*/GetConfigurationValue(LOOPBACK_NUMBER_OF_SAMPLE_SIZE_STR));
	if (iSuccess != TE_OK)
    {
		SetStatus("Ref Endpoint:loopback failed");
		WriteLogFile("RFReceiveSensitivityTestAtThisFreq:referenceEndPointHandle set to Loopback mode (radiotestBerLoopback) failed");
		return -1;
	}

	iSuccess = radiotestLoopback(dutHandle, freq, 50, 255);
    if(iSuccess != TE_OK)
    {	
		SetStatus("Dut:loopback failed");
		WriteLogFile("RFReceiveSensitivityTestAtThisFreq:DUT set to Loopback mode (radiotestLoopback) failed");
		return -1;
	}     

	int count=0;
	do
    {
		iSuccess = hqGetBer(referenceEndPointHandle, 2000, BER_array);
		Sleep(10);
		count++;
    } while(iSuccess == TE_ERROR && count< 20 /*4*/);
	
	if (iSuccess != TE_OK)
    {
		SetStatus("Ref Endpoint:get BER failed");
		WriteLogFile("RFReceiveSensitivityTestAtThisFreq:Dut get BER (hqGetBer) failed");
		return -1;
	}

#endif

	spradiotestCfgPkt(dutHandle, 4, 27);
	spradiotestCfgPkt(referenceEndPointHandle, 4, 27);

	//configure the tx/rx interval
    spradiotestCfgFreq(dutHandle, 1250, 1875, 1);
	spradiotestCfgFreq(referenceEndPointHandle, 1250, 1875, 1);

	uint16 channels[5] = { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff };
          
	n=0;
	do
    {
		iSuccess = radiotestCfgHoppingSeq(referenceEndPointHandle, channels);
		n++;
    } while((iSuccess != TE_OK) && (n < 5));
	
	if (iSuccess != TE_OK)
	{
		SetStatus("Ref Endpoint:Config hopping freqs failed");
		WriteLogFile("RFReceiveSensitivityTestAtThisFreq:Reference Endpoint configures hopping frequencies (CfgHoppingSeq) failed");
		return -1; 
	}
	//int temp = GetConfigurationValue(REFERENCE_ENDPOINT_INTERNAL_PA_VALUE_STR);
	n=0;
	do
    {
#if 1 /* Seavia 20150910, fix warning, conversion from 'double' to 'uint16', possible losse of data */
		uint16 ref_ipa_value = (uint16) GetConfigurationValue(REFERENCE_ENDPOINT_INTERNAL_PA_VALUE_STR);
		iSuccess = radiotestTxdata2(referenceEndPointHandle, 0, ref_ipa_value, 255);
#else
		iSuccess = radiotestTxdata2(referenceEndPointHandle, 0, GetConfigurationValue(REFERENCE_ENDPOINT_INTERNAL_PA_VALUE_STR), 255);
#endif
		n++;
    } while((iSuccess != TE_OK) && (n < 5));
	
	if (iSuccess != TE_OK)
	{
		SetStatus("Ref Endpoint:set to Txdata2 failed");
		WriteLogFile("RFReceiveSensitivityTestAtThisFreq:Reference Endpoint set to Txdata2 (radiotestTxdata2) failed");
		return -1; 
	}
	//temp = GetConfigurationValue(FREQ_HOPPING_BER_SAMPLE_SIZE_STR);
	n=0;
	do
    {
#if 1 /* Seavia 20150910, fix warning, conversion from 'double' to 'uint32', possible losse of data */
		uint32 freq_hopping_ber_sample_size = (uint32) GetConfigurationValue(FREQ_HOPPING_BER_SAMPLE_SIZE_STR);
		iSuccess = radiotestBer2(dutHandle, 0, 0, 0, freq_hopping_ber_sample_size);
#else
		iSuccess = radiotestBer2(dutHandle, 0, 0, 0, GetConfigurationValue(FREQ_HOPPING_BER_SAMPLE_SIZE_STR));
#endif
		n++;
    } while((iSuccess != TE_OK) && (n < 5));
	
    if(iSuccess != TE_OK)
	{
		SetStatus("Dut:set to Ber2 failed");
		WriteLogFile("RFReceiveSensitivityTestAtThisFreq:Dut set to Ber2 (radiotestBer2) failed");
		return -1; 
	}

	int count=0;
	do
    {
		iSuccess = hqGetBer(dutHandle, 2000, BER_array);
		Sleep(10);
		count++;
    } while(iSuccess == TE_ERROR && count</*20*//*4*/10);
	
	if (iSuccess != TE_OK)
    {
		SetStatus("Dut:get BER failed");
		WriteLogFile("RFReceiveSensitivityTestAtThisFreq:Dut get BER (hqGetBer) failed");
		return -1;
	}
                        
	//float64 BER = (float64 (BER_array[4]-BER_array[3]+BER_array[7]) / float64 (BER_array[4])) * 100.0;
	packets_loss_percentage = float64 (BER_array[4]-BER_array[3]) / float64 (BER_array[4]) * 100.0; 

	BER = (float64 (BER_array[1]) / float64 (BER_array[0])) * 100.0;
	if (packets_loss_percentage > GetConfigurationValue(DUT_PACKETS_LOSS_PERCENTAGE_MAX_LIMIT_STR))
	{
		return /*-1*/0;
	}



	if (BER < GetConfigurationValue(DUT_BER_PERCENTAGE_MAX_LIMIT_STR))
	{	return 1;
	}

	return /*-1*/0;
}

int TurnOffAudioAmp()
{
	uint32 direction, value, errLines;
	int32	iSuccess;

	iSuccess = spSetPio32(dutHandle, 0x1, 0x1, 0x0, &errLines); 
	if (iSuccess != TE_OK)
	{
		//SetStatus("SETPIO[0] to LOW FAIL on DUT");
		//WriteLogFile("TurnOffAudioAmp on DUT failed."); 
		return -1;
	}
	
	iSuccess = spGetPio32(dutHandle, &direction, &value); 
	if (iSuccess != TE_OK)
	{
		//SetStatus("GETPIO[0] FAIL on DUT");
		//WriteLogFile("TurnOffAudioAmp on DUT failed."); 
		return -1;
	}

	if (value & 0x1)
	{
		//WriteLogFile("bccmdGetPio[0] output is high on DUT."); 
		//SetStatus("PIO[0] HIGH on DUT");
		return -1;
	}

	return 1;
}

int RFReceiveSensitivityTest(int test_type)
{
	int result=0, n=0;
	CString	strMessage;

	float64 berLow=100.0, berMid=100.0, berHigh=100.0, berFreqHopping=100.0;
	double perLow=100.0, perMid=100.0, perHigh=100.0, perFreqHopping=100.0;
#if 1 /* Seavia 20150810 , fix unreferenced local variable warning, initialize resultlow to 0*/
	int resultlow=0, result_freq_hopping;
#else
	int resultlow, resultMid, resultHigh, result_freq_hopping;
#endif
	int32	iSuccess;

	uint32 direction, value, errLines;

	/*if (test_type == PCB_TEST) 
	{	iSuccess = bccmdSetColdReset(referenceEndPointHandle, 2000);
		if (iSuccess != TE_OK)
		{
			WriteLogFile("RFReceiveSensitivityTest:bccmdSetColdReset on REF END POINT failed."); 
			return -1;
		}
	}
	else if (test_type == PRODUCT_TEST)
	{*/
		iSuccess = bccmdSetColdReset(dutHandle, 15000);
		if (iSuccess != TE_OK)
		{
			SetStatus("ColdReset FAIL on DUT");
			WriteLogFile("RFReceiveSensitivityTest:bccmdSetColdReset on DUT failed."); 
			return -1;
		}

		iSuccess = spSetPio32(dutHandle, 0x1, 0x1, 0x1, &errLines); 
		if (iSuccess != TE_OK)
		{
			SetStatus("SETPIO[0] to HIGH FAIL on DUT");
			WriteLogFile("RFReceiveSensitivityTest:bccmdSetPio[0] to high on DUT failed."); 
			return -1;
		}
	
		iSuccess = spGetPio32(dutHandle, &direction, &value); 
		if (iSuccess != TE_OK)
		{
			SetStatus("GETPIO[0] FAIL on DUT");
			WriteLogFile("RFReceiveSensitivityTest:spGetPio32 on DUT failed."); 
			return -1;
		}

		if (!(value & 0x1))
		{
			WriteLogFile("RFReceiveSensitivityTest:bccmdGetPio[0] output is low on DUT."); 
			SetStatus("PIO[0] LOW on DUT");
			return -1;
		}

		/*iSuccess = spradiotestPcmToneIf(dutHandle, 2, 6, 0, 0);
		if (iSuccess != TE_OK)
		{
			WriteLogFile("RFReceiveSensitivityTest:radiotestPcmToneIf on DUT failed."); 
			SetStatus("Playing 1k tone FAIL on DUT");
			return -1;
		}*/
		
		iSuccess = bccmdSetColdReset(referenceEndPointHandle/*transmitModuleHandle*/, 2000);
		if (iSuccess != TE_OK)
		{
			WriteLogFile("RFReceiveSensitivityTest:bccmdSetColdReset on REF END POINT failed."); 
			//WriteLogFile("RFReceiveSensitivityTest:bccmdSetColdReset on SILVERPLUS TRANSMIT MODULE failed."); 
			TurnOffAudioAmp();
			return -1;
		}
	/*}*/

#if 0
	n=0;
	do 
	{	resultlow = RFReceiveSensitivityTestAtThisFreq(LOW_FREQUENCY_MHZ, berLow,  perLow, test_type);
	} while ((resultlow == -1) && ((n++)<2));

	if(resultlow == 1)
	{
		//do NOT run middle frequency!!!
		//resultMid = RFReceiveSensitivityTestAtThisFreq(MID_FREQUENCY_MHZ,  berMid,  perMid);
		if (test_type == PRODUCT_TEST)
		{
			iSuccess = bccmdSetColdReset(dutHandle/*transmitModuleHandle*/, 2000);
			if (iSuccess != TE_OK)
			{
				WriteLogFile("RFReceiveSensitivityTest:bccmdSetColdReset on DUT failed."); 
				//WriteLogFile("RFReceiveSensitivityTest:bccmdSetColdReset on SILVERPLUS TRANSMIT MODULE failed."); 
				TurnOffAudioAmp();
				return -1;
			}

			iSuccess = spSetPio32(dutHandle, 0x1, 0x1, 0x1, &errLines); 
			if (iSuccess != TE_OK)
			{
				SetStatus("SETPIO[0] to HIGH FAIL on DUT");
				WriteLogFile("RFReceiveSensitivityTest:bccmdSetPio[0] to high on DUT failed."); 
				return -1;
			}
		
			iSuccess = spGetPio32(dutHandle, &direction, &value); 
			if (iSuccess != TE_OK)
			{
				SetStatus("GETPIO[0] FAIL on DUT");
				WriteLogFile("RFReceiveSensitivityTest:spGetPio32 on DUT failed."); 
				return -1;
			}

			if (!(value & 0x1))
			{
				WriteLogFile("RFReceiveSensitivityTest:bccmdGetPio[0] output is low on DUT."); 
				SetStatus("PIO[0] LOW on DUT");
				return -1;
			}
		}

		n=0;
		do 
		{
			resultHigh = RFReceiveSensitivityTestAtThisFreq(HIGH_FREQUENCY_MHZ,  berHigh,  perHigh, test_type);
		}
		while ((resultHigh == -1) && ((n++)<2));

		//SetStatus("PER:H:%3.2f, M:%3.2f, L:%3.2f BER:H:%3.2f, M:%3.2f, L:%3.2f",perHigh,perMid,perLow, berHigh, berMid, berLow);
		if ((resultHigh == 1) || (resultHigh == 0))
		{	SetStatus("PER:H:%3.2f,L:%3.2f BER:H:%3.2f,L:%3.2f",perHigh,perLow, berHigh, berLow);
		}
		
		//pause any running radio tests, this fixes issues with Gemini
		radiotestPause(dutHandle);
		radiotestPause(referenceEndPointHandle);
		//radiotestPause(transmitModuleHandle);
		bccmdSetWarmReset(dutHandle, 5000);
		
		if((resultHigh == 1) /*&& (resultMid == 1)*/ && (resultlow == 1))
		{
			if (test_type == PRODUCT_TEST)
			{	TurnOffAudioAmp();
			}
			return 1;
		}
		else
		{	
			if (test_type == PRODUCT_TEST)
			{	TurnOffAudioAmp();
			}
			return 0;
		}
	}
	else
	{
		if(resultlow == 0)
		{	SetStatus("PER:L:%3.2f BER:L:%3.2f",perLow, berLow);
		}
		
		//pause any running radio tests, this fixes issues with Gemini
		radiotestPause(dutHandle);
		radiotestPause(referenceEndPointHandle);
		//radiotestPause(transmitModuleHandle);
		bccmdSetWarmReset(dutHandle, 5000);
		if (test_type == PRODUCT_TEST)
		{	TurnOffAudioAmp();
		}
		return 0;
	}

	if (test_type == PRODUCT_TEST)
	{	TurnOffAudioAmp();
	}
	return 0;
#endif 

	n=0;
	do 
	{	result_freq_hopping = RFReceiveSensitivityTestAtThisFreq(berFreqHopping,  perFreqHopping);
#if 1 /*Seavia 20150810, check how to fix resultlow uninitialization*/
		WriteMainLogFile ("RFReceiveSensitivityTest => resultlow is not initialized : %d", resultlow);
#endif
	} while ((resultlow == -1) && ((n++)<2));

	if ((result_freq_hopping == 1) || (result_freq_hopping == 0))
	{	SetStatus("PER:%3.2f, BER:%3.2f ",perFreqHopping, berFreqHopping);
	}
		
	//pause any running radio tests, this fixes issues with Gemini
	radiotestPause(dutHandle);
	iSuccess = spradiotestPause(referenceEndPointHandle);
	if (iSuccess != TE_OK)
	{	
		iSuccess = bccmdSetColdReset(referenceEndPointHandle, 2000);
		if (iSuccess != TE_OK)
		{	SetStatus("ColdReset REF Fail");
			WriteLogFile("RFReceiveSensitivityTest:bccmdSetColdReset on REF END POINT failed."); 
			return -1;
		}
	}
	//bccmdSetWarmReset(dutHandle, /*5000*/15000);
	TurnOffAudioAmp();

	if (result_freq_hopping == 1)
		return 1;

	return 0;
}

int PCBTEST_RFReceiveSensitivityTest()
{
	if(PeriphiralOpen(PERIPHIRAL_ID_GPIOBOARD|PERIPHIRAL_ID_DUT|PERIPHIRAL_ID_REFENDPOINT/*PERIPHIRAL_ID_TRANSMIT_MODULE*/, SPI_CONNECTION))
	{
		return RFReceiveSensitivityTest(PCB_TEST);
	}
	return 0;
}

int PRODUCT_TEST_RFReceiveSensitivityTest()
{	//CsrDevicesClose();
	if(PeriphiralOpen(PERIPHIRAL_ID_DUT|PERIPHIRAL_ID_REFENDPOINT/*PERIPHIRAL_ID_TRANSMIT_MODULE*/, USB_CONNECTION))
	{
#if 1 /* Seavia 20150910, fix warning, conversion from 'double' to 'int', possible loss of data*/
		int retries = (int) GetConfigurationValue(PRODUCT_RX_RETRY_STR);
#else
		int retries = GetConfigurationValue(PRODUCT_RX_RETRY_STR);
#endif
		int result;
		do
		{
			result = RFReceiveSensitivityTest(PRODUCT_TEST);
			if (result != SUCCESS)
				WriteLogFile("PRODUCT_TEST_RFReceiveSensitivityTest retry %d", retries);
		} while ((result != SUCCESS) && (retries-- > 0));
		return result;
	}
	return 0;
}

int PCBTEST_PushButtonsTest() 
{
	int result=0, n=0;
	CString	strMessage;
	int32	iSuccess;
#if 1 /* Seavia 20150810 , fix unreferenced local variable warning*/
	uint32	direction, value;
#else
	uint32	direction, value, errLines;
#endif
	int button1_on_state=-1, button1_off_state=-1, button2_on_state=-1, button2_off_state=-1;
	
	if(PeriphiralOpen(PERIPHIRAL_ID_GPIOBOARD|PERIPHIRAL_ID_DUT, SPI_CONNECTION))
	{
		result = ChangeGPIOState(BUTTON_1_PIN_NUM, LOW);
		if (result != 1)
		{
			return -1;
		}

		iSuccess = spGetPio32(dutHandle, &direction, &value); 
		if (iSuccess != TE_OK)
		{
			WriteLogFile("PCBTEST_PushButtonsTest:spGetPio32 on DUT failed."); 
			return -1;
		}

		button1_off_state = (value & 16);
		//button1_off_state = (value & 0); //modified for new nomato
		
		result = ChangeGPIOState(BUTTON_1_PIN_NUM, HIGH);
		if (result != 1)
		{
			return -1;
		}

		iSuccess = spGetPio32(dutHandle, &direction, &value); 
		

		if (iSuccess != TE_OK)
		{
			WriteLogFile("PCBTEST_PushButtonsTest:spGetPio32 on DUT failed."); 
			return -1;
		}


		button1_on_state = (value & 16);
		

		result = ChangeGPIOState(BUTTON_2_PIN_NUM, LOW); 
		if (result != 1)
		{
			return -1;
		}

		iSuccess = spGetPio32(dutHandle, &direction, &value); 
		

		if (iSuccess != TE_OK)
		{
			WriteLogFile("PCBTEST_PushButtonsTest:spGetPio32 on DUT failed."); 
			return -1;
		}

		button2_off_state = (value & 32);
		//button2_off_state = (value & 0); //modified for new nomato
		
	
		result = ChangeGPIOState(BUTTON_2_PIN_NUM, HIGH);
		if (result != 1)
		{
			return -1;
		}

		iSuccess = spGetPio32(dutHandle, &direction, &value); 
		

		if (iSuccess != TE_OK)
		{
			WriteLogFile("PCBTEST_PushButtonsTest:spGetPio32 on DUT failed."); 
			return -1;
		}


		button2_on_state = (value & 32);
		
		SetStatus("B1(on):%d,(off):%d,B2(on):%d,(off):%d",button1_on_state,button1_off_state,button2_on_state,button2_off_state);
		if ((button1_on_state) && (!button1_off_state) && (button2_on_state) && (!button2_off_state))
		{	
			return 1; 
		}
		else
			
			return 0;
	}
	return 0;
}
#ifdef SAMPLE_SIZE_44k
#define SAMPLE_RATE  (44100) /*(8000)*/
#else
#define SAMPLE_RATE  (8000) /*(8000)*/
#endif
#define FRAMES_PER_BUFFER (512)
#define NUM_CHANNELS    (/*2*/1)

#define PA_SAMPLE_TYPE  /*paFloat32*/paInt16
typedef /*float*/short SAMPLE;
//#define FFF_SIGNAL_FREQ_WINDOW /*10*/ 1

typedef struct
{
    int          frameIndex;  /* Index into sample array. */
    int          maxFrameIndex;
    SAMPLE      *recordedSamples;
}
paTestData;

typedef struct
{
    short *buffer;
    unsigned int bufferSampleCount;
    unsigned int playbackIndex;
}
paFileData;

static paFileData	File_data;
static paTestData   data;
unsigned long		audio_file_size;

int GetBlueToothInputDevIndex()
{	char device_name[100], device_name_lower_case[100]; 
	
	for (int device_index=0; device_index < Pa_GetDeviceCount(); device_index++)
	{	
		WriteLogFile("GetBlueToothInputDevIndex %s.", Pa_GetDeviceInfo(device_index)->name);
		//if (!strcmp(Pa_GetDeviceInfo(device_index)->name, "Bluetooth Microphone (Bluetooth"))
		//if (strstr(Pa_GetDeviceInfo(device_index)->name, "Microphone (Blue") != NULL)

		memset(device_name,0x0,sizeof(device_name));
		strncpy(device_name, Pa_GetDeviceInfo(device_index)->name, sizeof(device_name)-1);

		memset(device_name_lower_case,0x0,sizeof(device_name_lower_case)); 
		for (int i=0; i < (int)strlen(device_name); i++)
		{
			device_name_lower_case[i] = tolower(device_name[i]);
		}

		//WriteLogFile("GetBlueToothInputDevIndex lowercase %s.", device_name_lower_case);
#if 1
		if (((strstr(device_name_lower_case, /*"Microphone"*/"microphone") != NULL) ||
             (strstr(device_name_lower_case, "input") != NULL))
			 &&
		    (strstr(/*Pa_GetDeviceInfo(device_index)->name*/device_name_lower_case, /*"Blue"*/"blue") != NULL)
		   )
#else
		if(strstr(Pa_GetDeviceInfo(device_index)->name, "blue") != NULL)
#endif
		{
			return device_index;
		}
	}
	return -1;
}

/*int GetBlueToothOutputDevIndex()
{	char name[100];
	for (int device_index=0; device_index < Pa_GetDeviceCount(); device_index++)
	{	strncpy(name, Pa_GetDeviceInfo(device_index)->name, sizeof(name)-1);
		WriteLogFile(name);
		if (!strncmp(Pa_GetDeviceInfo(device_index)->name, 
			"Speakers (Bluetooth Hands-free ",
			sizeof("Speakers (Bluetooth Hands-free ")))
		{
			return device_index;
		}
	}
	return -1;
}*/

/*int GetPCInputDevIndex()
{	
	for (int device_index=0; device_index < Pa_GetDeviceCount(); device_index++)
	{	
		if (!strcmp(Pa_GetDeviceInfo(device_index)->name, 
			"Microphone (Realtek High Defini"))
		{
			return device_index;
		}
	}
	return -1;
}*/

int GetPCOutputDevIndex()
{	
	for (int device_index=0; device_index < Pa_GetDeviceCount(); device_index++)
	{	WriteLogFile("GetPCOutputDevIndex %s.", Pa_GetDeviceInfo(device_index)->name);
	}
	for (int device_index=0; device_index < Pa_GetDeviceCount(); device_index++)
	{	
		/*if (!strcmp(Pa_GetDeviceInfo(device_index)->name, 
			"Speakers (Realtek High Definiti"))*/
		if (strstr(Pa_GetDeviceInfo(device_index)->name, "Speakers (") != NULL)
		{
			if (strstr(Pa_GetDeviceInfo(device_index)->name, "Blue") == NULL)
			{	WriteLogFile("Found GetPCOutputDevIndex is %d.", device_index);
				return device_index;
			}
		}
	}
	return -1;
}

static int patestCallback( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{
    paTestData *ptr_data = (paTestData*)userData;
    SAMPLE *out = (SAMPLE*)outputBuffer;

#if 1 /* Seavia 20150810 , fix unreferenced local variable warning*/
    unsigned long i;
#else
    unsigned long i,j;
#endif
	int finished;

    (void) timeInfo; /* Prevent unused variable warnings. */
    (void) statusFlags;

	if (inputBuffer == NULL) return 0;
   
    //play the known audio file
	if (File_data.buffer != NULL)
	{	for( i=0; i<framesPerBuffer; i++ )
		{
			*out++ = File_data.buffer[ File_data.playbackIndex++ ];
				
			if( File_data.playbackIndex >= audio_file_size)
					File_data.playbackIndex = 0; /* loop at end of buffer */
			
		}
	}

	const SAMPLE *rptr = (const SAMPLE*)inputBuffer;
	SAMPLE *wptr = &ptr_data->recordedSamples[ptr_data->frameIndex * 1];
	long framesToCalc;
	long framesLeft = ptr_data->maxFrameIndex - ptr_data->frameIndex;
	
	if (framesLeft >= 0)
	{	if( (unsigned long)framesLeft < framesPerBuffer )
		{
			framesToCalc = framesLeft;
			finished = paComplete;
		}
		else
		{
			framesToCalc = framesPerBuffer;
			finished = paContinue;
		}

		for( i=0; i< (unsigned long)framesToCalc; i++ )
		{
			*wptr++ = *rptr++;  /* left */
		}
		ptr_data->frameIndex += framesToCalc;
	}

    return finished;
}

int run_fft(int SIZE, int STARTING_SAMPLE, int freq_in_k, int audio_test_type, double & fft_recorded_samples_signal_magnitude_peak, double & noise_magnitude_2k, double & noise_magnitude_3k, double & noise_value_at_other_freqs, int &fft_failed_at_this_freq) 
{	
	fftw_complex    *fft_data_recorded_samples, *fft_result_recorded_samples, *ifft_result_recorded_samples;
	fftw_plan       plan_forward, plan_backward;
    int             i, j;
	double			fft_recorded_samples_magnitude;
	char			FileName[50], FileNameIndexStr[2];
	CString			strMessage;
	double			fft_signal_1k_min_limit, fft_noise_floor_max_limit, fft_noise_floor_2k_max_limit, fft_noise_floor_3k_max_limit; 
	int				fail_flag = 1;

	fft_recorded_samples_signal_magnitude_peak=0.0;
	noise_magnitude_2k = /*0.0*/1;
	noise_magnitude_3k = /*0.0*/1;
	noise_value_at_other_freqs = 1;
	fft_failed_at_this_freq = 0;

    fft_data_recorded_samples = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * SAMPLE_RATE );
	if (fft_data_recorded_samples == NULL) 
	{	return 0;
	}
	fft_result_recorded_samples = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * SAMPLE_RATE );
	if (fft_result_recorded_samples == NULL) 
	{	return 0;
	}
	ifft_result_recorded_samples = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * SAMPLE_RATE );
	if (ifft_result_recorded_samples == NULL)
	{	return 0;
	}

	//run fft on the recorded samples

	plan_forward  = fftw_plan_dft_1d( SAMPLE_RATE, fft_data_recorded_samples, fft_result_recorded_samples, FFTW_FORWARD, FFTW_ESTIMATE );
    plan_backward = fftw_plan_dft_1d( SAMPLE_RATE, fft_result_recorded_samples, ifft_result_recorded_samples, FFTW_BACKWARD, FFTW_ESTIMATE );
	
	/* populate input data */
    for( i = 0, j=SIZE-SAMPLE_RATE; i < SAMPLE_RATE; i++, j++ ) 
	{
        fft_data_recorded_samples[i][0] = data.recordedSamples[STARTING_SAMPLE+i];
        fft_data_recorded_samples[i][1] = 0.0;
    }
	
	fftw_execute( plan_forward );
	fftw_execute( plan_backward );

	if (audio_test_type == BLUETOOTH_DEVICE_MICROPHONE_TEST) 
	{
		fft_signal_1k_min_limit = pow((double) 10, GetConfigurationValue(PRODUCT_MICROPHONE_FFT_SIGNAL_1K_FREQ_MIN_LIMIT_STR)); 
		fft_noise_floor_max_limit = pow((double) 10, GetConfigurationValue(PRODUCT_MICROPHONE_FFT_NOISE_FLOOR_MAX_LIMIT_STR));
		fft_noise_floor_2k_max_limit = pow((double) 10, GetConfigurationValue(PRODUCT_MICROPHONE_FFT_NOISE_FLOOR_2K_FREQ_MAX_LIMIT_STR));
		fft_noise_floor_3k_max_limit = pow((double) 10, GetConfigurationValue(PRODUCT_MICROPHONE_FFT_NOISE_FLOOR_3K_FREQ_MAX_LIMIT_STR));
	}
	else if (audio_test_type == BLUETOOTH_DEVICE_SPEAKER_TEST)
	{
		fft_signal_1k_min_limit = pow((double) 10, GetConfigurationValue(PRODUCT_SPEAKER_FFT_SIGNAL_1K_FREQ_MIN_LIMIT_STR)); 
		fft_noise_floor_max_limit = pow((double) 10, GetConfigurationValue(PRODUCT_SPEAKER_FFT_NOISE_FLOOR_MAX_LIMIT_STR));
		fft_noise_floor_2k_max_limit = pow((double) 10, GetConfigurationValue(PRODUCT_SPEAKER_FFT_NOISE_FLOOR_2K_FREQ_MAX_LIMIT_STR));
		fft_noise_floor_3k_max_limit = pow((double) 10, GetConfigurationValue(PRODUCT_SPEAKER_FFT_NOISE_FLOOR_3K_FREQ_MAX_LIMIT_STR));
	}
	else if (audio_test_type == PCB_LOOPBACK_TEST) 
	{
		fft_signal_1k_min_limit = pow((double) 10, GetConfigurationValue(PCB_LOOPBACK_FFT_SIGNAL_1K_FREQ_MIN_LIMIT_STR));
		fft_noise_floor_max_limit = pow((double) 10, GetConfigurationValue(PCB_LOOPBACK_FFT_NOISE_FLOOR_MAX_LIMIT_STR));
		fft_noise_floor_2k_max_limit = pow((double) 10, GetConfigurationValue(PCB_LOOPBACK_FFT_NOISE_FLOOR_2K_FREQ_MAX_LIMIT_STR));
		fft_noise_floor_3k_max_limit = pow((double) 10, GetConfigurationValue(PCB_LOOPBACK_FFT_NOISE_FLOOR_3K_FREQ_MAX_LIMIT_STR));
	}
	else if (audio_test_type == PCB_SPEAKER_TEST)
	{
		fft_signal_1k_min_limit = pow((double) 10, GetConfigurationValue(PCB_SPEAKER_FFT_SIGNAL_1K_FREQ_MIN_LIMIT_STR));
		fft_noise_floor_max_limit = pow((double) 10, GetConfigurationValue(PCB_SPEAKER_FFT_NOISE_FLOOR_MAX_LIMIT_STR));
		fft_noise_floor_2k_max_limit = pow((double) 10, GetConfigurationValue(PCB_SPEAKER_FFT_NOISE_FLOOR_2K_FREQ_MAX_LIMIT_STR));
		fft_noise_floor_3k_max_limit = pow((double) 10, GetConfigurationValue(PCB_SPEAKER_FFT_NOISE_FLOOR_3K_FREQ_MAX_LIMIT_STR));
	}

	fail_flag = FFT_FAILED_AT_1K;
#if 1 /* Seavia 20150910, fix warning, conversion from 'double' to 'int', possible losse of data */
	int fft_singal_freq_window = (int) GetConfigurationValue(FFT_SIGNAL_FREQ_WINDOW_STR);
	for (i = 999 - fft_singal_freq_window ; ( i <= 999 + fft_singal_freq_window )/*&& (fail_flag == 1)*/; i++)
#else
	for (i = 999-GetConfigurationValue(FFT_SIGNAL_FREQ_WINDOW_STR); (i<=999+GetConfigurationValue(FFT_SIGNAL_FREQ_WINDOW_STR)) /*&& (fail_flag == 1)*/; i++)
#endif
	{ 
		fft_recorded_samples_magnitude = sqrt(pow(fft_result_recorded_samples[i][0],2) + pow(fft_result_recorded_samples[i][1],2));
	
		if (fft_recorded_samples_signal_magnitude_peak < fft_recorded_samples_magnitude)
		{
			fft_recorded_samples_signal_magnitude_peak = fft_recorded_samples_magnitude;
		}
		if (fft_recorded_samples_magnitude >= fft_signal_1k_min_limit)
		{	
			fail_flag &= ~FFT_FAILED_AT_1K;
			//fft_recorded_samples_signal_magnitude_peak = fft_recorded_samples_magnitude;

		}
		else
		{	

		}
	}
	
	//skip the first 3 samples
	for(i = /*0*/3; (i < SAMPLE_RATE/2) /*&& (fail_flag == 0)*/; i++ )
	{
		fft_recorded_samples_magnitude = sqrt(pow(fft_result_recorded_samples[i][0],2) + pow(fft_result_recorded_samples[i][1],2));
	
		if( ((fft_recorded_samples_magnitude <= fft_noise_floor_2k_max_limit) 
			     &&((i >= (1999-GetConfigurationValue(FFT_SIGNAL_FREQ_WINDOW_STR))) && (i <= (1999+GetConfigurationValue(FFT_SIGNAL_FREQ_WINDOW_STR)))))
		    || ((fft_recorded_samples_magnitude <= fft_noise_floor_3k_max_limit) 
			     &&((i >= (2999-GetConfigurationValue(FFT_SIGNAL_FREQ_WINDOW_STR))) && (i <= (2999+GetConfigurationValue(FFT_SIGNAL_FREQ_WINDOW_STR)))))
			||(((i >= (999-GetConfigurationValue(FFT_SIGNAL_FREQ_WINDOW_STR))) && (i <= (999+GetConfigurationValue(FFT_SIGNAL_FREQ_WINDOW_STR)))))
		    ||(fft_recorded_samples_magnitude <= fft_noise_floor_max_limit)
		  )
		{
			//so far ok, keep going
		}
		else
		{

			if ((i >= (1999-GetConfigurationValue(FFT_SIGNAL_FREQ_WINDOW_STR))) && (i <= (1999+GetConfigurationValue(FFT_SIGNAL_FREQ_WINDOW_STR))))
			{
			//failed
			fail_flag |= FFT_FAILED_AT_2K;
			}
			else if ((i >= (2999-GetConfigurationValue(FFT_SIGNAL_FREQ_WINDOW_STR))) && (i <= (2999+GetConfigurationValue(FFT_SIGNAL_FREQ_WINDOW_STR))))
			{
				fail_flag |= FFT_FAILED_AT_3K;
			}
			else
			{
				fail_flag |= FFT_FAILED_AT_OTHER_FREQ;
			}


		}

		if ( ((i >= (1999-GetConfigurationValue(FFT_SIGNAL_FREQ_WINDOW_STR))) && (i <= (1999+GetConfigurationValue(FFT_SIGNAL_FREQ_WINDOW_STR))))
			 ||
			 ((i >= (999-GetConfigurationValue(FFT_SIGNAL_FREQ_WINDOW_STR))) && (i <= (999+GetConfigurationValue(FFT_SIGNAL_FREQ_WINDOW_STR))))
			 ||
			 ((i >= (2999-GetConfigurationValue(FFT_SIGNAL_FREQ_WINDOW_STR))) && (i <= (2999+GetConfigurationValue(FFT_SIGNAL_FREQ_WINDOW_STR))))
		   )
		{	 
			if ((i >= (999-GetConfigurationValue(FFT_SIGNAL_FREQ_WINDOW_STR))) && (i <= (999+GetConfigurationValue(FFT_SIGNAL_FREQ_WINDOW_STR))))
			{

			}
			else if ((i >= (1999-GetConfigurationValue(FFT_SIGNAL_FREQ_WINDOW_STR))) && (i <= (1999+GetConfigurationValue(FFT_SIGNAL_FREQ_WINDOW_STR))))
			{

				if (fft_recorded_samples_magnitude > noise_magnitude_2k)
				{
					noise_magnitude_2k = fft_recorded_samples_magnitude;
				}
			}
			else if ((i >= (2999-GetConfigurationValue(FFT_SIGNAL_FREQ_WINDOW_STR))) && (i <= (2999+GetConfigurationValue(FFT_SIGNAL_FREQ_WINDOW_STR))))
			{

				if (fft_recorded_samples_magnitude > noise_magnitude_3k)
				{
					noise_magnitude_3k = fft_recorded_samples_magnitude;
				}
			}
			
		}
		else
		{
			if (fft_recorded_samples_magnitude > noise_value_at_other_freqs)
			{
				noise_value_at_other_freqs = fft_recorded_samples_magnitude;
			}
		}

	}


#ifdef fft_write_file

	FILE  *fid;
	
	_itoa(freq_in_k, FileNameIndexStr, 10);
	FileNameIndexStr[1] = '\0';

	memset(FileName,0x0,sizeof(FileName));
	if (audio_test_type == BLUETOOTH_DEVICE_MICROPHONE_TEST)
	{
		strncpy(FileName, "bluetooth_microphone_fft_results_", sizeof(FileName));
	}
	else if (audio_test_type == BLUETOOTH_DEVICE_SPEAKER_TEST)
	{
		strncpy(FileName, "pc_microphone_fft_results_", sizeof(FileName));
	}
	else if (audio_test_type == PCB_LOOPBACK_TEST)
	{
		strncpy(FileName, "pcb_loopback_fft_results_", sizeof(FileName));
	}
	else if (audio_test_type == PCB_SPEAKER_TEST)
	{
		strncpy(FileName, "pcb_speaker_fft_results_", sizeof(FileName));
	}
	strcat(FileName, FileNameIndexStr); 
	strcat(FileName, "k.csv");
	FileName[sizeof(FileName)-1] = '\0';
    fid = fopen(FileName, "w");
    if( fid == NULL )
    {
		//printf("Could not open file.");
		//return 0;
    }
    else
    {
		fprintf( fid, "%s, %s, %s, %s, %s, %s, %s, %s\n",
                    /*"original_input", "original_input_real", "original_input_img", 
					"fft_input_real", "fft_input_img", "inverse_fft_input_real", "inverse_fft_input_img", "magnitute_input_fft",*/
					"original_output", "original_output_real", "original_output_img", 
					"fft_output_real", "fft_output_img", "inverse_fft_output_real", "inverse_fft_output_img", "magnitute_output_fft"/*,
					"fft_recorded_samples_ave_signal_magnitude", "fft_recorded_samples_ave_noise_floor_magnitude", "fft_recorded_samples_max_noise_floor_magnitude"*/
					
					);	
		/* print fft result */
		for( i = 0 ; i < SAMPLE_RATE ; i++ ) 
		{
			//magnitute_fft_result = sqrt(pow(fft_result[i][0],2) + pow(fft_result[i][1],2));
			fft_recorded_samples_magnitude = sqrt(pow(fft_result_recorded_samples[i][0],2) + pow(fft_result_recorded_samples[i][1],2));
			fprintf( fid, "%d, %2.2f, %2.2f, %2.2f, %2.2f, %2.2f, %2.2f, %2.2f, %2.2f, %2.2f, %2.2f\n"/*"%d, %2.2f, %2.2f, %2.2f, %2.2f, %2.2f, %2.2f, %2.2f, %d, %2.2f, %2.2f, %2.2f, %2.2f, %2.2f, %2.2f, %2.2f*/,
                     /*File_data.buffer[i], fft_data[i][0], fft_data[i][1], fft_result[i][0], fft_result[i][1], ifft_result[i][0]/SIZE, ifft_result[i][1]/SIZE, magnitute_fft_result, */
					 data.recordedSamples[STARTING_SAMPLE+i], fft_data_recorded_samples[i][0], fft_data_recorded_samples[i][1], fft_result_recorded_samples[i][0], fft_result_recorded_samples[i][1], ifft_result_recorded_samples[i][0]/SAMPLE_RATE/*SIZE*/, ifft_result_recorded_samples[i][1]/SAMPLE_RATE/*SIZE*/, fft_recorded_samples_magnitude/*,
					 fft_recorded_samples_signal_magnitude_ave, fft_recorded_samples_noise_floor_magnitude_ave, fft_recorded_samples_max_noise_floor_magnitude*/
					 );
		}
		
        fclose( fid );
    }
	//fclose( fid );	
#endif


	/* free memory */
    fftw_destroy_plan( plan_forward );
    fftw_destroy_plan( plan_backward );
	
    fftw_free( fft_data_recorded_samples );
    fftw_free( fft_result_recorded_samples );
    fftw_free( ifft_result_recorded_samples );

	fft_failed_at_this_freq = fail_flag;
	if (fail_flag)
		return -1;

	return 1;
}

int RunAudioTestAtThisFreq(int freq_in_k, int audio_test_type, double &peak_value, double &noise_value_2k, double &noise_value_3k, double &noise_value_at_other_freqs, int &fft_failed_at_this_freq)
{
    PaStreamParameters inputParameters,
		               outputParameters;
    PaStream *stream;
    PaError err;

#if 1 /* Seavia 20150810 , fix unreferenced local variable warning*/
	int					i;
#else
	int					i, j, k;
    int					bufferCount;
	PaDeviceInfo		device_info;
	int32	iSuccess;
	int                 numSamples;
	int                 numBytes;
#endif
	int                 totalFrames;
	CString				strMessage;
	unsigned long		result;
	char				FileName[50], FileNameIndexStr[2];
	int		n=0;
	int startingSample;
	int recordedSamplesPerLineIndex=0, recordedSamplesLineIndex=0;
	CString heading;
	int	bluetooth_device_microphone_test_flag=0; 

	_itoa(freq_in_k, FileNameIndexStr, 10);
	FileNameIndexStr[1] = '\0';

if ((audio_test_type != PCB_SPEAKER_TEST) && (audio_test_type != BLUETOOTH_DEVICE_SPEAKER_TEST))
{	/* initialize audio file buffer */
	FILE  *pFile;

	

	memset(FileName,0x0,sizeof(FileName));
	strncpy(FileName, "input_audio_", sizeof(FileName));
	if (audio_test_type == PCB_LOOPBACK_TEST)
	{	
#ifdef SAMPLE_SIZE_44k
		strcat(FileName, "sam44k_");
#else
        strcat(FileName, "low_amp_");
#endif
		strcat(FileName, FileNameIndexStr);
		strcat(FileName, "k.raw");
		//FileName[18] = '\0';
	}
	else
	{
		strcat(FileName, FileNameIndexStr);
		strcat(FileName, "k.raw");
		FileName[18] = '\0';
	}

	pFile = fopen(/*"input_audio_1k.raw"*/FileName, "rb");
	if (pFile == NULL) 
	{	
		strMessage.Format(_T("RunAudioTestAtThisFreq:Could not open %s"), FileName);
		WriteLogFile(strMessage);
		return -1;
	}
	// obtain file size:
	fseek (pFile , 0 , SEEK_END);
	File_data.bufferSampleCount = ftell (pFile);
	audio_file_size = File_data.bufferSampleCount / /*4*/2;
	rewind (pFile);

	// allocate memory to contain the whole file:
	File_data.buffer = (/*float**/short*) malloc (File_data.bufferSampleCount);
	if (File_data.buffer == NULL) 
	{	
		WriteLogFile("Could not allocate memory for audio file.");
		return -1;
	}
	
	// copy the file into the buffer:
	result = fread (File_data.buffer,/*sizeof(float)*/2,audio_file_size,pFile);
	if (result != audio_file_size) 
	{	SetStatus("Could not read in audio file");
		WriteLogFile("RunAudioTestAtThisFreq:Could not read in audio file.");
		return -1;
	}
	fclose (pFile);

	File_data.playbackIndex = 0;
//#endif
}
else
{
	audio_file_size = SAMPLE_RATE;
	File_data.buffer = NULL;
}
	
if(audio_file_size > (2*SAMPLE_RATE))
{
	audio_file_size =  2*SAMPLE_RATE;
}

    startingSample = totalFrames = SAMPLE_RATE; 
	data.maxFrameIndex =  audio_file_size*2; 
    data.frameIndex = 0;
    data.recordedSamples = (SAMPLE *) malloc( data.maxFrameIndex * NUM_CHANNELS * sizeof(SAMPLE)); /* From now on, recordedSamples is initialised. */
    if( data.recordedSamples == NULL )
    {	SetStatus("Could not allocate record array");
        WriteLogFile("RunAudioTestAtThisFreq:Could not allocate record array.");
        goto /*done*/error;
    }
    for( i=0; i<data.maxFrameIndex; i++ ) data.recordedSamples[i] = 0;
//#endif	

	err = Pa_Initialize();
	if( err != paNoError ) { SetStatus("Pa_Initialize failed"); WriteLogFile("RunAudioTestAtThisFreq:Pa_Initialize failed.");	goto error; }

	if ((audio_test_type == BLUETOOTH_DEVICE_MICROPHONE_TEST) || (audio_test_type == PCB_LOOPBACK_TEST))
	{
		outputParameters.device = /*Pa_GetDefaultOutputDevice()*/GetPCOutputDevIndex(); /* default output device */
		if (outputParameters.device == paNoDevice) 
		{   SetStatus("Fail To Open PC's Speaker as ouput device");
			WriteLogFile("RunAudioTestAtThisFreq:Could not open PC's speaker as output device.");
			goto /*done*/error;
		}
	}

	if ((audio_test_type != PCB_SPEAKER_TEST) && (audio_test_type != BLUETOOTH_DEVICE_SPEAKER_TEST))
	{	outputParameters.channelCount = 1;       
		outputParameters.sampleFormat = PA_SAMPLE_TYPE/*paFloat32*/; 
		outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
		outputParameters.hostApiSpecificStreamInfo = NULL;
	}

	if (audio_test_type == BLUETOOTH_DEVICE_MICROPHONE_TEST)
	{
		inputParameters.device = /*Pa_GetDefaultInputDevice()*/GetBlueToothInputDevIndex();
		int temp = inputParameters.device;
		if (inputParameters.device == paNoDevice) 
		{	SetStatus("Fail To Open DUT microphone as input device via bluetooth connection");
			WriteLogFile("RunAudioTestAtThisFreq:Could not open DUT's microphone as input device.");
			bluetooth_device_microphone_test_flag = 1;
			goto /*done*/error;
		}
	}
	else if (audio_test_type == BLUETOOTH_DEVICE_SPEAKER_TEST)
	{
		inputParameters.device = Pa_GetDefaultInputDevice()/*GetPCInputDevIndex()*/;
		if (inputParameters.device == paNoDevice) 
		{	SetStatus("Fail To open PC's microphone as input device");
			WriteLogFile("RunAudioTestAtThisFreq:Could not open PC's microphone as input device.");
			goto /*done*/error;
		}
	}
	else if ((audio_test_type == PCB_LOOPBACK_TEST) || (audio_test_type == PCB_SPEAKER_TEST))
	{
		inputParameters.device = Pa_GetDefaultInputDevice() /*GetPCInputDevIndex()*/;
		if (inputParameters.device == paNoDevice) 
		{	SetStatus("Fail To open PC's microphone as input device");
			WriteLogFile("RunAudioTestAtThisFreq:Could not open PC's microphone as input device.");
			goto /*done*/error;
		}
	}
    inputParameters.channelCount = 1;                    
    inputParameters.sampleFormat = PA_SAMPLE_TYPE/*paFloat32*/;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;	

	/* Open an audio I/O stream. */
	if ((audio_test_type != PCB_SPEAKER_TEST) && (audio_test_type != BLUETOOTH_DEVICE_SPEAKER_TEST))
	{	err = Pa_OpenStream(
              &stream,
              &inputParameters, 
              &outputParameters,
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              paNoFlag,      /* paClipOff if we won't output out of range samples so don't bother clipping them */
              patestCallback, 
              &data/*NULL*/ ); 

		if( err != paNoError ) { SetStatus("Pa_OpenStream failed"); WriteLogFile("RunAudioTestAtThisFreq:Pa_OpenStream failed.");	WriteLogFile(Pa_GetErrorText( err )); goto error;	}
	}
	else
	{
		err = Pa_OpenStream(
              &stream,
              &inputParameters/*NULL*/, 
              /*&outputParameters*/NULL,
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              paNoFlag,      /*paClipOff if we won't output out of range samples so don't bother clipping them */
              patestCallback, 
              &data/*NULL*/ ); 

		if( err != paNoError ) { SetStatus("Pa_OpenStream failed"); WriteLogFile("RunAudioTestAtThisFreq:Pa_OpenStream failed.");	goto error;	}
	}
	
	err = Pa_StartStream( stream );
	if( err != paNoError ) { SetStatus("Pa_StartStream failed"); WriteLogFile("RunAudioTestAtThisFreq:Pa_StartStream failed.");  goto error; }

	while( ( err = Pa_IsStreamActive( stream ) ) == 1 )
    {
        Pa_Sleep(1000);
    }
	if( err < 0 ) { SetStatus("Pa_IsStreamActive failed"); WriteLogFile("RunAudioTestAtThisFreq:Pa_IsStreamActive failed."); goto /*done*/error; }

	err = Pa_CloseStream( stream );
	if( err != paNoError ) { SetStatus("Pa_CloseStream failed"); WriteLogFile("RunAudioTestAtThisFreq:Pa_CloseStream failed."); goto error; }
	Pa_Terminate();

	if ((audio_test_type != PCB_SPEAKER_TEST) && (audio_test_type != BLUETOOTH_DEVICE_SPEAKER_TEST))
	{
	}
	else 
	{	
	
	}

	
	for (recordedSamplesLineIndex=0; recordedSamplesLineIndex<50; recordedSamplesLineIndex++)
	{	
		if (audio_test_type == PCB_SPEAKER_TEST) 
			heading = "PCB_SPEAKER_TEST_RECORDED_SAMPLES_SET:";
		else if (audio_test_type == PCB_LOOPBACK_TEST)
			heading = "PCB_LOOPBACK_TEST_RECORDED_SAMPLES_SET: ";
		else if (audio_test_type == BLUETOOTH_DEVICE_SPEAKER_TEST)
			heading = "PRODUCT_SPEAKER_TEST_RECORDED_SAMPLES_SET: ";
		else 
			heading = "PRODUCT_MICROPHONE_TEST_RECORDED_SAMPLES_SET: ";
		
		newlineflag = 0;
		timestampflag = 1;
		WriteLogFile("%s ", heading);
		
		timestampflag = 0;
		for (recordedSamplesPerLineIndex=0; recordedSamplesPerLineIndex<16; recordedSamplesPerLineIndex++)
		{	 WriteLogFile("%d ", data.recordedSamples[startingSample+recordedSamplesLineIndex*16+recordedSamplesPerLineIndex]);
		}
		WriteLogFile("\n");
	}
	newlineflag = 1;
	timestampflag = 1;

    if (run_fft(totalFrames, startingSample, freq_in_k, audio_test_type, peak_value, noise_value_2k, noise_value_3k, noise_value_at_other_freqs, fft_failed_at_this_freq) != 1)
	{ 
		WriteLogFile("RunAudioTestAtThisFreq:run_fft failed."); 
		goto done; 
	}

	if( data.recordedSamples )       /* Sure it is NULL or valid. */
        free( data.recordedSamples );
	if (File_data.buffer )
		free (File_data.buffer);
	return 1;
error:
	Pa_Terminate();
    
	if( data.recordedSamples )       /* Sure it is NULL or valid. */
        free( data.recordedSamples );
	if (File_data.buffer )
		free (File_data.buffer);

	if (bluetooth_device_microphone_test_flag) 
		return -2;
    return -1;

done:
    Pa_Terminate();
    if( data.recordedSamples )       /* Sure it is NULL or valid. */
        free( data.recordedSamples );
	if (File_data.buffer )
		free (File_data.buffer);
    if( err != paNoError )
    {
        //fprintf( stderr, "An error occured while using the portaudio stream\n" );
        //fprintf( stderr, "Error number: %d\n", err );
        //fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
        err = 1;          /* Always return 0 or 1, but no other return codes. */
    }
	return /*-1*/0;
}

int ReadPairingInfoFile(string FileName)
{
	//CString strMessage;
	uint16 number;
	
	ifstream PairingInfoFile (FileName.c_str());
	if (PairingInfoFile.fail())
	{	
		WriteLogFile("Can not read %s.", FileName); 
		return 0;
	}
	
	pairing_info_data_size=0;
	while ( PairingInfoFile.good() && !PairingInfoFile.fail())
	{
		PairingInfoFile >> hex >> number;
		if ( /*PairingInfoFile.good() &&*/ !PairingInfoFile.fail())
		{
			pairing_info_data[pairing_info_data_size++]=number;
		}
	}
	PairingInfoFile.close();
	return 1;
} 

int PRODUCT_TEST_MicrophoneTest()
{
	int	input_audio_file_freq_in_k=1, result=0, pairing_clear_result;
	double peak_value=0.0, noise_value_2k=0.0, noise_value_3k=0.0, noise_value_at_other_freqs=0.0;
#if 1 /* Seavia 20150810 , fix unreferenced local variable warning*/
	int n=0;
#else
	uint16 count;
	int n=0, temp;
#endif
    int32 iSuccess;
	int fft_failed_at_this_freq=0;
	int bt_connection_fail_count=0, fft_fail_count=0;
	
#if 1 /* Seavia 20150910, fix warning, conversion from 'double' to 'DWORD', possible losse of data */
	uint32 mic_test_wait_time = (uint32) GetConfigurationValue(MICROPHONE_TEST_WAIT_TIME_STR);
#endif
	if(PeriphiralOpen(PERIPHIRAL_ID_DUT, USB_CONNECTION))
	{
		if (!pairingInfoWrite(pairing_info_data, pairing_info_data_size))
		{
			SetStatus("WRITE PAIRING INFO FAILED");
			return 0;
		}
		
#if 0		
		iSuccess = bccmdSetColdReset(dutHandle, /*5000*/15000);
		if(iSuccess != TE_OK)
		{
			SetStatus("COLD RESET FAIL");
			return 0;
		}
#else
        if(!ChangeGPIOState(VBAT_IO_PIN_NUM, LOW))
		{
			SetStatus("Fail To SET USB_ENABLE_PIN_NUM to HIGH");
			return 0;
		}
		if(!ChangeGPIOState(USB_ENABLE_PIN_NUM, LOW))
		{
			SetStatus("Fail To SET USB_ENABLE_PIN_NUM to HIGH");
			return 0;
		}
		Sleep(3000);
        if(!ChangeGPIOState(VBAT_IO_PIN_NUM, HIGH))
		{
			SetStatus("Fail To SET USB_ENABLE_PIN_NUM to HIGH");
			return 0;
		}
		if(!ChangeGPIOState(USB_ENABLE_PIN_NUM, HIGH))
		{
			SetStatus("Fail To SET USB_ENABLE_PIN_NUM to HIGH");
			return 0;
		}	
		CsrDevicesClose();
#endif

		n=0;
		do 
		{	
			//vbat_state = HIGH;
#if 1 /* Seavia 20150910, fix warning, conversion from 'double' to 'DWORD', possible losse of data */
			uint32 bt_conn_wait_time = (uint32) (GetConfigurationValue(BLUETOOTH_CONNECTION_WAIT_TIME_STR)*1000);
			Sleep(/*8000*/bt_conn_wait_time); //must wait for at least 1 second for the chip to start up after VBAT is turned on. 
#else
			Sleep(/*8000*/GetConfigurationValue(BLUETOOTH_CONNECTION_WAIT_TIME_STR)*1000); //must wait for at least 1 second for the chip to start up after VBAT is turned on. 
#endif
			//temp = GetConfigurationValue(BLUETOOTH_CONNECTION_WAIT_TIME_STR);
			result = RunAudioTestAtThisFreq(input_audio_file_freq_in_k, BLUETOOTH_DEVICE_MICROPHONE_TEST, peak_value, noise_value_2k, noise_value_3k, noise_value_at_other_freqs, fft_failed_at_this_freq);
			if (result == -2)
			{	bt_connection_fail_count++;
				fft_fail_count = 0;
				//Sleep(1000);
				if (GetConfigurationValue(DUT_RESET_USING_RELAYS_STR))
				{	
					CsrDevicesClose();
					
					if(!ChangeGPIOState(USB_ENABLE_PIN_NUM, LOW))
					{
						SetStatus("Fail To SET USB_ENABLE_PIN_NUM to LOW");
						return 0;
					}
					
					Sleep(3000);

					if(!ChangeGPIOState(USB_ENABLE_PIN_NUM, HIGH))
					{
						SetStatus("Fail To SET USB_ENABLE_PIN_NUM to HIGH");
						return 0;
					}
				}
				
				else
				{	
					if(PeriphiralOpen(PERIPHIRAL_ID_DUT, USB_CONNECTION))
					{	
						iSuccess = bccmdSetColdReset(dutHandle, /*5000*/15000);
						if(iSuccess != TE_OK)
						{
							SetStatus("COLD RESET FAIL");
							return 0;
						}
					}
				}
			}
			else if (result != 1) 
			{
				fft_fail_count++;
			}
			
			//for (input_audio_file_freq_in_k=1; input_audio_file_freq_in_k<=3; input_audio_file_freq_in_k++)
			//{
				//result = RunAudioTestAtThisFreq(input_audio_file_freq_in_k, BLUETOOTH_DEVICE_MICROPHONE_TEST, peak_value, noise_value_2k, noise_value_3k, noise_value_at_other_freqs, fft_failed_at_this_freq);
		} while ( ((result == -2) || (result == 0)) && ((/*n++*/bt_connection_fail_count< 2) && (fft_fail_count< 2)));	
			
			
		if (result != 1)
		{						
			if (result == 0)
			{	SetStatus("1k:%4.2f  2k:%4.2f  3k:%4.2f Other:%4.2f Failed at:%s",log10(peak_value), log10(noise_value_2k), log10(noise_value_3k), 
						log10(noise_value_at_other_freqs),fftresultsStrings[fft_failed_at_this_freq&0x0F]); 
			}
		}
		else
		{
			SetStatus("1k:%4.2f  2k:%4.2f  3k:%4.2f Other:%4.2f",log10(peak_value), log10(noise_value_2k), log10(noise_value_3k), log10(noise_value_at_other_freqs));
		}
		
		if(PeriphiralOpen(PERIPHIRAL_ID_DUT, USB_CONNECTION)==1)
		{	
#if 1 /* Seavia 20150910, fix warning, conversion from 'double' to 'DWORD', possible losse of data */
			//uint32 mic_test_wait_time = (uint32) GetConfigurationValue(MICROPHONE_TEST_WAIT_TIME_STR);
			Sleep(mic_test_wait_time*1000);
#else
			Sleep(GetConfigurationValue(MICROPHONE_TEST_WAIT_TIME_STR)*1000);
#endif
			//put this back in!!!!!!!!!!!!!!!!!!!
			pairing_clear_result = pairingInfoClear();
			
			if ((result == 1) && (pairing_clear_result == 1))
			{	
				return 1;
			}
			else
			{	
				if (result == 1) 
				{
					SetStatus("Pairing clear failed");
				}
				return 0;
			}
		}
		else
		{
#if 1 /* Seavia 20150910, fix warning, conversion from 'double' to 'DWORD', possible losse of data */
			Sleep(mic_test_wait_time*1000);
#else
			Sleep(GetConfigurationValue(MICROPHONE_TEST_WAIT_TIME_STR)*1000);
#endif
			return 0;
		}
	}
	return 0;
}

int PRODUCT_TEST_SpeakerTest()
{	
	int32	iSuccess;
	int	input_audio_file_freq_in_k=1, result;
	double peak_value=0.0, noise_value_2k=0.0, noise_value_3k=0.0, noise_value_at_other_freqs=0.0;
	uint32 direction, value, errLines;
	int fft_failed_at_this_freq=0;
	
	//for (input_audio_file_freq_in_k=1; input_audio_file_freq_in_k<=3; input_audio_file_freq_in_k++)
	if(PeriphiralOpen(PERIPHIRAL_ID_DUT, USB_CONNECTION))
	{
		
		/*if (!pairingInfoWrite(pairing_info_data, pairing_info_data_size))
		{
			SetStatus("WRITE PAIRING INFO FAILED");
			return 0;
		}*/
		
		iSuccess = bccmdSetColdReset(dutHandle, /*5000*/15000);
		if (iSuccess != TE_OK)
		{
			SetStatus("ColdReset DUT FAIL");
			WriteLogFile("PRODUCT_TEST_SpeakerTest:bccmdSetColdReset failed."); 
			return -1;
		}
		
		
		iSuccess = spSetPio32(dutHandle, 0x1, 0x1, 0x1, &errLines); 
		if (iSuccess != TE_OK)
		{
			SetStatus("SETPIO[0] FAIL");
			WriteLogFile("PRODUCT_TEST_SpeakerTest:bccmdSetPio[0] to high on DUT failed."); 
			return -1;
		}
	
		iSuccess = spGetPio32(dutHandle, &direction, &value); 
		if (iSuccess != TE_OK)
		{
			SetStatus("GETPIO[0] FAIL");
			WriteLogFile("PRODUCT_TEST_SpeakerTest:spGetPio32 on DUT failed."); 
			return -1;
		}


	
		if (!(value & 0x1))
		{
			WriteLogFile("PRODUCT_TEST_SpeakerTest:bccmdGetPio[0] output is low on DUT."); 
			SetStatus("PIO[0] LOW");
			return -1;
		}
		iSuccess = spradiotestPcmToneIf(dutHandle, 2, 6, 0, 0);
		if (iSuccess != TE_OK)
		{
			WriteLogFile("PRODUCT_TEST_SpeakerTest:radiotestPcmToneIf failed.");	
			return -1;
		}
		result = RunAudioTestAtThisFreq(input_audio_file_freq_in_k, BLUETOOTH_DEVICE_SPEAKER_TEST, peak_value, noise_value_2k, noise_value_3k, noise_value_at_other_freqs, fft_failed_at_this_freq);
		if (result != 1)
		{	
			SetStatus("1k:%4.2f  2k:%4.2f  3k:%4.2f Other:%4.2f Failed at:%s",log10(peak_value), log10(noise_value_2k), log10(noise_value_3k), 
				log10(noise_value_at_other_freqs),fftresultsStrings[fft_failed_at_this_freq&0x0F]);
		}
		else
		{
			SetStatus("1k:%4.2f  2k:%4.2f  3k:%4.2f Other:%4.2f",log10(peak_value), log10(noise_value_2k), log10(noise_value_3k), log10(noise_value_at_other_freqs));
		}

		iSuccess = bccmdSetWarmReset(dutHandle, /*5000*/15000);
		if (iSuccess != TE_OK)
		{
			WriteLogFile("PRODUCT_TEST_SpeakerTest:bccmdSetWarmReset failed."); 
		}
		return result;
	}
	return 0;
}

int PCBTEST_LoopbackAndSpeakerOnly()
{
	int32	iSuccess;
	CString strMessage;
	int n=0, result;
	uint32 direction, value, errLines;
#if 0 /* Seavia 20150810 , fix unreferenced local variable warning*/
	int num_of_tests_index;
#endif
	double speaker_peak_value=0.0, speaker_noise_value_2k=0.0, speaker_noise_value_3k=0.0, speaker_noise_value_at_other_freqs=0.0;
	double loopback_peak_value=0.0, loopback_noise_value_2k=0.0, loopback_noise_value_3k=0.0, loopback_noise_value_at_other_freqs=0.0;
	int speaker_fft_failed_at_this_freq=0, loopback_fft_failed_at_this_freq=0;

	if(PeriphiralOpen(PERIPHIRAL_ID_DUT, SPI_CONNECTION))
	{

		iSuccess = spSetPio32(dutHandle, 0x1, 0x1, 0x1, &errLines); 
		if (iSuccess != TE_OK)
		{
			SetStatus("SETPIO[0] FAIL");
			WriteLogFile("PCBTEST_LoopbackAndSpeakerOnly:bccmdSetPio[0] to high on DUT failed."); 
			return -1;
		}
		Sleep(500);
		iSuccess = spGetPio32(dutHandle, &direction, &value); 
		if (iSuccess != TE_OK)
		{
			SetStatus("GETPIO[0] FAIL");
			WriteLogFile("PCBTEST_LoopbackAndSpeakerOnly:spGetPio32 on DUT failed."); 
			return -1;
		}


	
		if (!(value & 0x1))
		{
			WriteLogFile("PCBTEST_LoopbackAndSpeakerOnly:bccmdGetPio[0] output is low on DUT."); 
			SetStatus("PIO[0] LOW");
			return -1;
		}
//#if 1
		iSuccess =  spradiotestPcmToneIf(dutHandle, 2, 6, 0, 0);
		if (iSuccess != TE_OK)
		{
			WriteLogFile("PCBTEST_LoopbackAndSpeakerOnly:radiotestPcmToneIf failed.");
			return -1;
		}

		result = RunAudioTestAtThisFreq(1, PCB_SPEAKER_TEST, speaker_peak_value, speaker_noise_value_2k, speaker_noise_value_3k, speaker_noise_value_at_other_freqs, speaker_fft_failed_at_this_freq);
		if (result != 1)
		{
            //SetStatus("RUNAUDIO FAIL");
			SetStatus("Sp:1k:%4.2f  2k:%4.2f  3k:%4.2f Other:%4.2f Speaker Test Failed at: %s",log10(speaker_peak_value), 
				log10(speaker_noise_value_2k), log10(speaker_noise_value_3k), log10(speaker_noise_value_at_other_freqs),
				fftresultsStrings[speaker_fft_failed_at_this_freq&0x0F]);
			return -1;
		}
	

		iSuccess = spSetPio32(dutHandle, 0x1, 0x1, 0x1, &errLines); 
		if (iSuccess != TE_OK)
		{
			SetStatus("SETPIO FAIL");
			WriteLogFile("PCBTEST_LoopbackAndSpeakerOnly:bccmdSetPio[0] to high on DUT failed."); 
			return -1;
		}

		iSuccess = spGetPio32(dutHandle, &direction, &value); 
		if (iSuccess != TE_OK)
		{
			SetStatus("GETPIO FAIL");
			WriteLogFile("PCBTEST_LoopbackAndSpeakerOnly:spGetPio32 on DUT failed."); 
			return -1;
		}
	
		if (!(value & 0x1))
		{
			SetStatus("PIO[0] LOW");
			WriteLogFile("PCBTEST_LoopbackAndSpeakerOnly:bccmdGetPio[0] output is low on DUT."); 
			return -1;
		}



		//put DUT in LOOPBACK mode
		n=0;
		do 
		{
			result = radiotestPcmLbIf(dutHandle, 1, 0);
		}
		while ((result != TE_OK) && ((n++)<5)); 
		
		if (result != TE_OK)
		{
			WriteLogFile("PCBTEST_LoopbackAndSpeakerOnly:Unable to set Loopback mode on DUT."); 
			return -1;
		}
 	
		result = RunAudioTestAtThisFreq(1, PCB_LOOPBACK_TEST, loopback_peak_value, loopback_noise_value_2k, loopback_noise_value_3k, loopback_noise_value_at_other_freqs, loopback_fft_failed_at_this_freq);
		iSuccess = bccmdSetWarmReset(dutHandle, 5000);
		if (iSuccess != TE_OK)
		{
			WriteLogFile("PCBTEST_LoopbackAndSpeakerOnly:bccmdSetWarmReset failed."); 
			//return -1;
		}

		
	
		if (result != 1)
		{
					SetStatus("Lb:1k:%4.2f  2k:%4.2f  3k:%4.2f Other:%4.2f Loopback Test Failed at: %s", log10(loopback_peak_value), 
						log10(loopback_noise_value_2k), log10(loopback_noise_value_3k), log10(loopback_noise_value_at_other_freqs),
						fftresultsStrings[loopback_fft_failed_at_this_freq&0x0F]);
			return -1;
		}	

		SetStatus("Sp:1k:%4.2f  2k:%4.2f  3k:%4.2f Other:%4.2f Lb:1k:%4.2f  2k:%4.2f  3k:%4.2f Other:%4.2f",log10(speaker_peak_value), log10(speaker_noise_value_2k), log10(speaker_noise_value_3k), log10(speaker_noise_value_at_other_freqs), log10(loopback_peak_value), log10(loopback_noise_value_2k), log10(loopback_noise_value_3k), log10(loopback_noise_value_at_other_freqs));
		
	
		
		return 1;
	}

	return 0;
}

int ChangeCurrent(char* tmp){
	char command[MAX_MULTIMETER_COMMANDS_LENGTH+1], resp[MAX_MULTIMETER_COMMANDS_LENGTH+1];

	char expected_resp[MAX_MULTIMETER_COMMANDS_LENGTH+1];
	unsigned long length_written=0, length_read=0, n=0, num_of_attempts=0, counter=0, write_counter=0;
	int completedbytes, totalbytes, expected_length_read;
	CString strMessage;

	memset(command,0x0,sizeof(command));
	strncpy(command, tmp, sizeof(command));  
	command[MAX_MULTIMETER_COMMANDS_LENGTH] = 0;

	totalbytes =  strlen(command);
	completedbytes = 0;
	n = 0;

	do
	{
		if(WriteToUsbModule(&command[completedbytes], totalbytes - completedbytes, length_written, UsbMultimeterModuleConnectionHd) == 1)
		{
			completedbytes += length_written;
		}
		else
		{
            break;
		}
	} while ( (totalbytes < completedbytes) && ((n++)<2));
	
	if (totalbytes != completedbytes) 
	{	
		WriteLogFile("MultimeterInit:totalbytes %d != completedbytes %d", totalbytes, completedbytes);
		return 0;
	}

	length_read = 0;
	memset(resp,0x0,sizeof(resp)); 
	expected_length_read = /*strlen(command)-1*/MAX_MULTIMETER_COMMANDS_LENGTH;
	
	completedbytes = 0;
	n=0;
	do 
	{	
		if(ReadFromUsbModule(&resp[completedbytes], expected_length_read -completedbytes, length_read, UsbMultimeterModuleConnectionHd) == 1)
		{
			completedbytes += length_read;
		}
		else
		{
            break;
		}
	} while ( (completedbytes < expected_length_read) && ((n++)<2));

	memset(expected_resp,0x0,sizeof(expected_resp));
	//the 'expected resp' from BK Precision is the same as the command sent minus the '\n' character
	strncpy(expected_resp, command, strlen(command)-1); 
	return 0;
}

int /*PCBTEST_*/MeasurePeakCurrent()
{
	int32	iSuccess;
#if 1 /* Seavia 20150810 , fix unreferenced local variable warning*/
	int		n=0;
#else
	int		n=0, result;
#endif
	double	peak_current=0.0;
	CString strMessage;
	uint32  direction, value, errLines;
	
	ChangeCurrent(MULTIMETER_CHANGE_DC_CURRENT_RANGE_TO_500_uAMP_COMMAND_STR);
	//if(PeriphiralOpen(PERIPHIRAL_ID_GPIOBOARD|PERIPHIRAL_ID_DUT|PERIPHIRAL_ID_MULTIMETER, SPI_CONNECTION))
	//{	
		iSuccess = spradiotestTxstart(dutHandle, MID_FREQUENCY_MHZ, 50, 0, 0); 
		if (iSuccess != TE_OK)
		{
			WriteLogFile("MeasurePeakCurrent:radiotestTxstart failed."); 
			return -1;
		}

		iSuccess = spSetPio32(dutHandle, 0x1, 0x1, 0x1, &errLines); 
		if (iSuccess != TE_OK)
		{
			WriteLogFile("MeasurePeakCurrent:bccmdSetPiop[0] to high on DUT failed."); 
			return -1;
		}
	
		iSuccess = spGetPio32(dutHandle, &direction, &value); 
		if (iSuccess != TE_OK)
		{
			WriteLogFile("MeasurePeakCurrent:spGetPio32 on DUT failed."); 
			return -1;
		}


	
		if (!(value & 0x1))
		{
			WriteLogFile("MeasurePeakCurrent:bccmdGetPio[0] output is low on DUT."); 
			return -1;
		}


		iSuccess = spradiotestPcmToneIf(dutHandle, 2, 8/*6*/, 0, 0);
		if (iSuccess != TE_OK)
		{
			WriteLogFile("MeasurePeakCurrent:radiotestPcmToneIf failed.");	
			return -1;
		}
#if 1 /*Seavia 20150903, per Winner request, fix peak current read error */
		Sleep(100); //modified for VC2
#else
		Sleep(1000); //modified for VC2
#endif
		if (ReadMultimeterMeasurement(peak_current) == 1)
		{	

			SetStatus("Peak:%1.3f", peak_current);
			iSuccess = bccmdSetWarmReset(dutHandle, 0);
			if (peak_current <= GetConfigurationValue(DUT_PEAK_CURRENT_MAX_LIMIT_STR))
			{
				return 1;	
			}
			else
				return -1;
		}
		else
		{
			SetStatus("Fail To Read peak current");
			return -1;
		}
	//}
	//else
	//{
		//SetStatus("Fail To Open peripheral");
		//return -1;
	//}
}

int PCBTEST_MeasurePeakCurrent()
{
	if(PeriphiralOpen(PERIPHIRAL_ID_GPIOBOARD|PERIPHIRAL_ID_DUT|PERIPHIRAL_ID_MULTIMETER, SPI_CONNECTION))
	{
		return MeasurePeakCurrent();
	}
	else
	{
		SetStatus("Fail To Open peripheral");
		return -1;
	}
}

int PRODUCT_TEST_MeasurePeakCurrent()
{
	int returnVal;
	if(PeriphiralOpen(PERIPHIRAL_ID_GPIOBOARD|PERIPHIRAL_ID_DUT|PERIPHIRAL_ID_MULTIMETER, USB_CONNECTION))
	{
		if(!ChangeGPIOState(VCHARGE_IO_PIN_NUM, LOW))
		{
			SetStatus("Fail To SET VCHARGE_IO_PIN_NUM to LOW");
			return 0;
		}
		returnVal = MeasurePeakCurrent();
		if(!ChangeGPIOState(VCHARGE_IO_PIN_NUM, HIGH))
		{
			SetStatus("Fail To SET VCHARGE_IO_PIN_NUM to HIGH");
			return 0;
		}
		CsrDevicesClose();
		return returnVal;
	}
	else
	{
		SetStatus("Fail To Open peripheral");
		return -1;
	}

}


int PCBTEST_MeasureStandbyCurrent()
{
	int32	iSuccess;

#if 1 /* Seavia 20150810 , fix unreferenced local variable warning*/
	int		n=0;
#else
	int		n=0, result;
#endif
	double	stand_by_current=0.0;
	uint32 errLines;
	CString strMessage;

	ChangeCurrent(MULTIMETER_CHANGE_DC_CURRENT_RANGE_TO_50_uAMP_COMMAND_STR);
	if(PeriphiralOpen(PERIPHIRAL_ID_GPIOBOARD|PERIPHIRAL_ID_DUT|PERIPHIRAL_ID_MULTIMETER, SPI_CONNECTION))
	{
#if 1 /*Seavia 20150903, per Winner request, fix standby current calibration issue.*/
		bccmdSetWarmReset(dutHandle, 0);
#else
		bccmdSetColdReset(dutHandle, 0);
#endif

		spMapPio32(dutHandle, 0x00040000, 0xFFFFFFFF, &errLines);
		spSetPio32(dutHandle, 0x40000, 0x40000, 
											 0x40000,  &errLines);

	    iSuccess = spSetPio32(dutHandle, 0x1, 0x1, 0x0, &errLines); 
		if (iSuccess != TE_OK)
		{
			WriteLogFile("MeasurePeakCurrent:bccmdSetPiop[0] to high on DUT failed."); 
			return -1;
		}

		Sleep(200);
		n=0;
		do 
		{
			iSuccess = radiotestDeepSleep(dutHandle);
		} 
		while ((iSuccess != TE_OK) && ((n++)<5)); 
		
		if (iSuccess != TE_OK)
		{
			WriteLogFile("PCBTEST_MeasureStandbyCurrent:Unable to put DUT into sleep mode."); 
			return -1;
		}

		CsrDevicesClose();
		Sleep(1000); //modified for VC2

		if (ReadMultimeterMeasurement(stand_by_current) == 1)
		{
#if 1 /*Seavia 20150903, per Winner request, standby current <= 0 should return fail.*/
			if (stand_by_current <= 0)
			{
				return -1;
			}
			SetStatus("standby:%1.8f", stand_by_current);

#else
			SetStatus("standby:%1.4f", stand_by_current);
#endif

			if (stand_by_current <= GetConfigurationValue(DUT_STANDBY_CURRENT_MAX_LIMIT_STR))
			{
				//strMessage.Format("stand_by_current %f is greater than limit %f \n", stand_by_current, GetConfigurationValue(DUT_STANDBY_CURRENT_MAX_LIMIT_STR));
				
				return 1;
			}
			else
				return -1;
		}
		else
		{	

			SetStatus("Fail To Read standby current");

			return -1;
		}
	}
	else
	{
		SetStatus("Fail To Open peripheral");
		return -1;
	}
}

int TurnOffVChargeTurnOnVbat()
{
	int result = ChangeGPIOState(VCHARGE_IO_PIN_NUM, LOW);
		
	if (result != 1)
	{	return result;
	}
	Sleep(1000);
	result = ChangeGPIOState(VBAT_IO_PIN_NUM, HIGH);
	if (result != 1)
	{	return result;
	}
	return 1;
}

uint8 fwVersion[2];
bool readSerial(HANDLE DeviceHandle)
{
	DWORD count=0;
	hid_command_t command_report;
	hid_status_t status_response;
	char strbda[32] = {0};

	command_report.report_id=REPORT_COMMAND_ID;
	command_report.command=COMMAND_READ_DEVICE_INFO;
	command_report.data[0]=0x00;
	if (!WriteFile(DeviceHandle,&command_report,sizeof(hid_command_t),&count,NULL))
	{
		/* cannot write */
		return(FALSE);
	}
	if (!ReadFile(DeviceHandle,&status_response,sizeof(hid_status_t),&count,NULL)) 
	{
		/* cannot read */
		return(FALSE);
    }

	fwVersion[0] = status_response.data[10];
	fwVersion[1] = status_response.data[11];
	return TRUE;
}

int PRODUCT_TEST_FactoryEnable()
{
#ifdef FACTORY_ENABLE
    //Put Back Enter Pairing mode on power On
    uint16 psKey[6];
    uint16 deviceCode[3];
    uint16 lengthRetrieved=0, productID=PRODUCT_ID, usbConfigID=USB_CONFIG_ID, usbVM=USB_VM_CONTROL_ID;
	uint16 userConfigData12[2] = {0x4010, 0001};
	int n=0, i=0;
	int32 iSuccess;

	//John Test
	CsrDevicesClose();
	Sleep(5000);

    memset(psKey,0x0,sizeof(psKey));
    memset(deviceCode,0x0,sizeof(deviceCode));
    if (PeriphiralOpen(PERIPHIRAL_ID_DUT, USB_CONNECTION)){
		if(PCBTEST_spiPortLock()){ 
			if(WriteBlutoothAddress_And_SerialNumber()) {	
				n=0;
				do{
					iSuccess = psSize(dutHandle, PSKEY_USER15, 0 ,&lengthRetrieved);
				}while ((iSuccess != TE_OK) && ((n++)<5)); 

				if (iSuccess != TE_OK){
					SetStatus("psSize: Fail");
					update_bool = false;
					WriteLogFile("PRODUCT_TEST_FactoryEnable: psSize on PSKEY_USER15 Failed");
					return 0;
				}
				for (i=0; i<sizeof(psKey)/sizeof(uint16); i++){	
					psKey[i] = psKey15Value[i];
				}

				//psKey[1] |= ENABLE_AUTO_PAIRING_ON_POWER_ON;
				//psKey[2] |= ENABLE_AUTO_DISCOVERABLE;

				n=0;
				do 
				{
					iSuccess = VerifyDutHandle(dutHandle);
					if (iSuccess == TE_OK)
					{
						iSuccess = /*psWrite*/sppsWrite(dutHandle, PSKEY_USER15, 0, 6, psKey);
					}
				}
				while ((iSuccess != TE_OK) && ((n++)<5)); 

				if(/*psWrite(dutHandle, PSKEY_USER15, 0, 6, psKey)*/iSuccess == TE_OK)
				{
					if(GetConfigurationValue(PRODUCT_IGNORE_HID_CHECK_STR) != 1) 
					{
						/*if((psWrite(dutHandle, HID_DEVICE_CLASS_CODE, 0, 3, deviceCode))
						&&(psWrite(dutHandle, HID_PRODUCT_ID, 0, 1, &productID))
						&&(psWrite(dutHandle, USB_CONFIG, 0, 1, &usbConfigID))
						&&(psWrite(dutHandle, USB_VM_CONTROL, 0, 1, &usbVM))
						&&(psClear(dutHandle, USER_CONFIGURATION_DATA_43, 0))	//disable 'Test Mode' been displayed on menu 
						&&(psClear(dutHandle, USER_CONFIGURATION_DATA_49, 0)) 
						&&(psWrite(dutHandle, USER_CONFIGURATION_DATA_12, 0, 2, userConfigData12))
						)*/

						n=0;
						do 
						{
							iSuccess = VerifyDutHandle(dutHandle);
							if (iSuccess == TE_OK)
							{
								iSuccess = /*psWrite*/sppsWrite(dutHandle, HID_DEVICE_CLASS_CODE, 0, 3, deviceCode);
							}
						}
						while ((iSuccess != TE_OK) && ((n++)<5)); 

						if (iSuccess != TE_OK)
						{
							SetStatus("psWrite HID_DEVICE_CLASS_CODE Fail");
							update_bool = false;
							WriteLogFile("PRODUCT_TEST_FactoryEnable: psWrite HID_DEVICE_CLASS_CODE Fail");
							return 0;
						}

						n=0;
						do 
						{
							iSuccess = VerifyDutHandle(dutHandle);
							if (iSuccess == TE_OK)
							{
								iSuccess = /*psWrite*/sppsWrite(dutHandle, HID_PRODUCT_ID, 0, 1, &productID);
							}
						}
						while ((iSuccess != TE_OK) && ((n++)<5)); 

						if (iSuccess != TE_OK)
						{
							SetStatus("psWrite HID_PRODUCT_ID Fail");
							update_bool = false;
							WriteLogFile("PRODUCT_TEST_FactoryEnable: psWrite HID_PRODUCT_ID Fail");
							return 0;
						}

						n=0;
						do 
						{
							iSuccess = VerifyDutHandle(dutHandle);
							if (iSuccess == TE_OK)
							{
								iSuccess = /*psWrite*/sppsWrite(dutHandle, USB_CONFIG, 0, 1, &usbConfigID);
							}
						}
						while ((iSuccess != TE_OK) && ((n++)<5)); 

						if (iSuccess != TE_OK)
						{
							SetStatus("psWrite USB_CONFIG Fail");
							update_bool = false;
							WriteLogFile("PRODUCT_TEST_FactoryEnable: psWrite USB_CONFIG Fail");
							return 0;
						}

						n=0;
						do 
						{
							iSuccess = VerifyDutHandle(dutHandle);
							if (iSuccess == TE_OK)
							{
								iSuccess = /*psWrite*/sppsWrite(dutHandle, USB_VM_CONTROL, 0, 1, &usbVM);
							}
						}
						while ((iSuccess != TE_OK) && ((n++)<5)); 

						if (iSuccess != TE_OK)
						{
							SetStatus("psWrite USB_VM_CONTROL Fail");
							update_bool = false;
							WriteLogFile("PRODUCT_TEST_FactoryEnable: psWrite USB_VM_CONTROL Fail");
							return 0;
						}

						n=0;
						do 
						{
							iSuccess = psClear(dutHandle, USER_CONFIGURATION_DATA_43, 0);
						}
						while ((iSuccess != TE_OK) && ((n++)<5)); 

						if (iSuccess != TE_OK)
						{
							SetStatus("psClear USER_CONFIGURATION_DATA_43 Fail");
							update_bool = false;
							WriteLogFile("PRODUCT_TEST_FactoryEnable: psClear USER_CONFIGURATION_DATA_43 Fail");
							return 0;
						}
					
						
						n=0;
						do 
						{
							iSuccess = psClear(dutHandle, USER_CONFIGURATION_DATA_49, 0);
						}
						while ((iSuccess != TE_OK) && ((n++)<5)); 

						if (iSuccess != TE_OK)
						{
							SetStatus("psClear USER_CONFIGURATION_DATA_49 Fail");
							update_bool = false;
							WriteLogFile("PRODUCT_TEST_FactoryEnable: psClear USER_CONFIGURATION_DATA_49 Fail");
							return 0;
						}
						
						n=0;
						do 
						{
							iSuccess = VerifyDutHandle(dutHandle);
							if (iSuccess == TE_OK)
							{
								iSuccess = /*psWrite*/sppsWrite(dutHandle, USER_CONFIGURATION_DATA_12, 0, 2, userConfigData12);
							}
						}
						while ((iSuccess != TE_OK) && ((n++)<5)); 

						if (iSuccess != TE_OK)
						{
							SetStatus("psWrite USER_CONFIGURATION_DATA_12 Fail");
							update_bool = false;
							WriteLogFile("PRODUCT_TEST_FactoryEnable: psWrite USER_CONFIGURATION_DATA_12 Fail");
							return 0;
						}

						/* John: follow version 39
						if (enableEnterDFUModeAfterPowerOn() != 1)
						{	
							SetStatus("enableEnterDFUModeAfterPowerOn: Fail");
							WriteLogFile("PRODUCT_TEST_FactoryEnable: enableEnterDFUModeAfterPowerOn Failed");
							return 0;
						}
						*/

						//{
							// ...make any more changes here if required...


						// /* John: don't try this at home...
						// I'm ignoring psfactoryset, it's nuts
							
							n=0;
							do 
							{
								iSuccess = VerifyDutHandle(dutHandle);
								if(iSuccess == TE_OK) {
								//try {
									//WriteLogFile("John is in try");
									//Sleep(5000);
									iSuccess = psFactorySet(dutHandle);

									if(iSuccess != TE_OK) {
										int error = teGetLastError(dutHandle);
										update_bool = false;
										WriteLogFile("John: psFactorySet fail");
									}
								//} catch(exception &e) {
								//	WriteLogFile(e.what());
								//}
								}
							}
							while ((iSuccess != TE_OK) && ((n++)<5));
						// */
							// John: igoring psFactorySet, don't know what are the consequences
							//if(/*psFactorySet(dutHandle)*/iSuccess == TE_OK) // Set Factory psr
							if(iSuccess == TE_OK || iSuccess != TE_OK)
							{
								bccmdSetWarmReset(dutHandle, /*1000*/15000);
								Sleep(5000);
								/* try finding the device */
								DeviceHandle = OpenDevice();
								if (DeviceHandle!=NULL) 
								{
									readSerial(DeviceHandle);
									return 1; 
								}
								else
								{	
									n=0;
									do 
									{
										Sleep(5000);
										/* try finding the device */
										DeviceHandle = OpenDevice();
									} while ( ((n++)<5) && (DeviceHandle==NULL)); 

									if (DeviceHandle!=NULL) 
									{	
										readSerial(DeviceHandle);
										return 1; 
									}
									update_bool = false;
									SetStatus("OpenDevice HID: Fail");
									update_bool = false;
									WriteLogFile("PRODUCT_TEST_FactoryEnable: OpenDevice HID Fail");
								}
							}
							else
							{
								update_bool = false;
								SetStatus("psFactorySet: Fail");
								update_bool = false;
								WriteLogFile("PRODUCT_TEST_FactoryEnable: psFactorySet Fail");
							}
						/*}
						else
						{
							SetStatus("PSWrite: USB_CONFIG Fail");
						}*/
					}
					else
					{
						return 1;
					}
				}
				else
				{
					update_bool = false;
					SetStatus("PSWrite: PSKEY_USER15 Fail");
					update_bool = false;
					WriteLogFile("PRODUCT_TEST_FactoryEnable: PSWrite PSKEY_USER15 AUTO PAIRING&DISCOVERABLE Fail");
				}
			}
		}
    }
    CsrDevicesClose();// You should close the handle to avoid false IO overlap
#endif
    return 0;
} 

/*static int usbEnable()
{
// Enable USB
#ifdef USB_ENABLE
	uint16 psKey=0;
	uint16 lengthRetrieved=0;
	if (PeriphiralOpen(PERIPHIRAL_ID_DUT, SPI_CONNECTION))
	{	
		psRead(dutHandle, HOST_INTERFACE, 0, 1, &psKey, &lengthRetrieved);
		psKey |= ENABLE_USB;
		if(psWrite(dutHandle, HOST_INTERFACE, 0, 1, &psKey))
			return 1;		
	}
	CsrDevicesClose();// You should close the handle to avoid false IO overlap
#endif
	return 0;
}*/
int PCBTEST_oledTest()
{
#ifdef OLED_TEST
	//Try setting all the pixels of OLED on to check if it is working fine
	uint32 i2ctransfer,errlines;
	uint16 count,i=0,j=0;
	uint32 errLines;
	int iAnswer;
	int32	iSuccess;

	displayInit();
	if(commandBufferComplete)
    {
		if(PeriphiralOpen(PERIPHIRAL_ID_DUT, SPI_CONNECTION))
		{			
			/*Turn V33*/
			iSuccess = spMapPio32(dutHandle, 0x00040000, 0xFFFFFFFF, &errLines);
			if (iSuccess != TE_OK)
			{	
				SetStatus("spMapPio32 Fail");
				WriteLogFile("PCBTEST_oledTest:spMapPio32 failed.");
				return 0;
			}

			iSuccess = spSetPio32(dutHandle, 0x40000, 0x40000, 0x40000,  &errlines);
			if (iSuccess != TE_OK)
			{	
				SetStatus("spSetPio32 Fail");
				WriteLogFile("PCBTEST_oledTest:spSetPio32 failed.");
				return 0; 
			}

			Sleep(100);
			iSuccess = spSetPio(dutHandle, 0x4000, 0x4000);
			if (iSuccess != TE_OK)
			{	
				SetStatus("spSetPio Fail");
				WriteLogFile("PCBTEST_oledTest:spSetPio failed.");
				return 0; 
			}

			iSuccess = spLedEnable(dutHandle, 0, 1);
			if (iSuccess != TE_OK)
			{	
				SetStatus("spLedEnable Fail");
				WriteLogFile("PCBTEST_oledTest:spLedEnable failed.");
				return 0; 
			}

			Sleep(1000);
			iSuccess = spLedEnable(dutHandle, 0, 0);
			if (iSuccess != TE_OK)
			{	
				SetStatus("spLedEnable Fail");
				WriteLogFile("PCBTEST_oledTest:spLedEnable failed.");
				return 0; 
			}

			iSuccess = spLedEnable(dutHandle, 1, 1);
			if (iSuccess != TE_OK)
			{	
				SetStatus("spLedEnable Fail");
				WriteLogFile("PCBTEST_oledTest:spLedEnable failed.");
				return 0; 
			}

			Sleep(1000);
			iSuccess = spLedEnable(dutHandle, 1, 0);
			if (iSuccess != TE_OK)
			{	
				SetStatus("spLedEnable Fail");
				WriteLogFile("PCBTEST_oledTest:spLedEnable failed.");
				return 0; 
			}

			iSuccess = spLedEnable(dutHandle, 2, 1);
			if (iSuccess != TE_OK)
			{	
				SetStatus("spLedEnable Fail");
				WriteLogFile("PCBTEST_oledTest:spLedEnable failed.");
				return 0; 
			}
			Sleep(1000);
			iSuccess = spLedEnable(dutHandle, 2, 0);
			if (iSuccess != TE_OK)
			{	
				SetStatus("spLedEnable Fail");
				WriteLogFile("PCBTEST_oledTest:spLedEnable failed.");
				return 0; 
			}
			for (i=0; i < 500; i++) j++; /* Delay */
			iSuccess = spSetPio(dutHandle, 0x4000, 0);
			if (iSuccess != TE_OK)
			{	
				SetStatus("spSetPio Fail");
				WriteLogFile("PCBTEST_oledTest:spSetPio failed.");
				return 0; 
			}

			Sleep(50);
			i2ctransfer = spbccmdI2CTransfer(dutHandle , OLED_SLAVE_ADDRESS, currentCmdBufferIdx ,
											0, 1, cmdBuffer ,
											&count);	

			if(i2ctransfer == 1)
			{
				//CsrDevicesClose();
				iAnswer = AfxMessageBox("Is display ok?", MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2);
				if(iAnswer == IDYES) 
				{	
					/*if(!ChangeGPIOState(VBAT_IO_PIN_NUM, LOW))
					{
						SetStatus("Fail To SET USB_ENABLE_PIN_NUM to HIGH");
						return 0;
					}
					Sleep(100);
					if(!ChangeGPIOState(VBAT_IO_PIN_NUM, HIGH))
					{
						SetStatus("Fail To SET USB_ENABLE_PIN_NUM to HIGH");
						return 0;
					}*/
					/*ToDo clear cmdbuff
					memset(cmdBuffer, 0, sizeof(cmdBuffer));
					I2c_SendOrBufferCommand(DISPLAY_ON_RESUME, 1,I2CSEND_LAST_COMMAND);
					I2c_SendOrBufferCommand(DISPLAY_OFF,1,0); 
					i2ctransfer = bccmdI2CTransfer(dutHandle , OLED_SLAVE_ADDRESS, currentCmdBufferIdx ,
											0, 1, cmdBuffer ,
											&count);*/
					return 1;
				}
				WriteLogFile("PCBTEST_oledTest:display NOT ok.");
				return 0;
			}
			SetStatus("spbccmdI2CTransfer Fail");
			WriteLogFile("PCBTEST_oledTest:spbccmdI2CTransfer failed.");
			return 0; 

		}
		SetStatus("DUT Open Fail");
		WriteLogFile("PCBTEST_oledTest:Dut Open failed.");
		return 0; 
	}
	SetStatus("commandBufferComplete FALSE");
	WriteLogFile("PCBTEST_oledTest:commandBufferComplete FALSE.");
	CsrDevicesClose();// You should close the handle to avoid false IO overlap
#endif	
	return 0;
}
int PRODUCT_TEST_oledTest()
{
#ifdef OLED_TEST
	//Try setting all the pixels of OLED on to check if it is working fine
	uint32 i2ctransfer,errLines;
	uint16 count,i=0,j=0;
    int iAnswer;
	displayInit();
	if(commandBufferComplete)
    {
		if(PeriphiralOpen(PERIPHIRAL_ID_DUT, USB_CONNECTION))
		{			
			/*ToDO Reset toggle*/

			spSetPio32(dutHandle, 0x4000, 0x4000, 
											 0x4000,  &errLines);
			Sleep(50);
			spSetPio32(dutHandle, 0x4000, 0x4000, 0x0000,  &errLines);
			
			Sleep(50);
			i2ctransfer = bccmdI2CTransfer(dutHandle , OLED_SLAVE_ADDRESS, currentCmdBufferIdx ,
											0, 1, cmdBuffer ,
											&count);	
			if(i2ctransfer == 1)
			{
				//CsrDevicesClose();
				iAnswer = AfxMessageBox("Is display ok?", MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2);
				if(iAnswer == IDYES) 
				{			
					return 1;
				}
				return 0;
			}
		}
	}
	CsrDevicesClose();// You should close the handle to avoid false IO overlap
#endif	
	return 0;
}

static void displayInit(void)
{    
    uint16 i = 0,j=0;
    /*Buffer*/            
    /*OLED Initialization*/
    I2c_SendOrBufferCommand(DISPLAY_OFF,1,I2CSEND_FIRST_COMMAND);
    //I2c_SendOrBufferCommand((SET_DISPLAY_CLOCK_DIV | (0x80 << 8)), 2,0);
	I2c_SendOrBufferCommand((SET_DISPLAY_CLOCK_DIV | (0xD4 << 8)), 2,0); //modified for new oled
    I2c_SendOrBufferCommand((SET_MULTIPLEX | (0x1f << 8)), 2,0);
    //I2c_SendOrBufferCommand((SET_DISPLAY_OFFSET | (0x00 << 8)), 2,0);
	I2c_SendOrBufferCommand((SET_DISPLAY_OFFSET | (0x20 << 8)), 2,0); //modified for new oled
    I2c_SendOrBufferCommand(SET_START_LINE, 1,0);
    //I2c_SendOrBufferCommand((CHARGE_PUMP | (0x14 << 8)), 2,0);
	I2c_SendOrBufferCommand((CHARGE_PUMP | (0x10 << 8)), 2,0); //modified for new oled
    I2c_SendOrBufferCommand((MEMORY_MODE | (0x00 << 8)), 2,0);
    I2c_SendOrBufferCommand(SEGMENT_RE_MAP,1,0);
    I2c_SendOrBufferCommand(COM_SCAN_DIRECTION,1,0);
    I2c_SendOrBufferCommand(SET_COM_PINS | ((0x02)<<8), 2,0);
    //I2c_SendOrBufferCommand(SET_CONTRAST | ((0x8F)<<8), 2,0);
	I2c_SendOrBufferCommand(SET_CONTRAST | ((0x9a)<<8), 2,0); //modified for new oled
    //I2c_SendOrBufferCommand(SET_PRECHARGE | ((0xF1)<<8),2,0);
	I2c_SendOrBufferCommand(SET_PRECHARGE | ((0x22)<<8),2,0); //modified for new oled
    //I2c_SendOrBufferCommand(SET_VCOM_DETECT|((0x40)<<8),2,0);
	I2c_SendOrBufferCommand(SET_VCOM_DETECT|((0x30)<<8),2,0); //modified for new oled
    I2c_SendOrBufferCommand(SET_NORMAL_DISPLAY,1,0);   
    I2c_SendOrBufferCommand(DISPLAY_ON,1,0); 
	I2c_SendOrBufferCommand(ENTIRE_DISPLAY_ON, 1,I2CSEND_LAST_COMMAND);
	//ToDo Check the time required to keep the display on before reset   

	
}

/**********************************************************************************************************
 Send/Buffer Command.
**********************************************************************************************************/
static void I2c_SendOrBufferCommand(uint32 value, uint8 length, uint8 startStopCont)
{

#if 1 /* Seavia 20150810 , fix unreferenced local variable warning*/
    int32 i;
#else
    int32 i,i2ctransfer,csrDeviceHandle,csrDeviceType;
#endif
    if(startStopCont & I2CSEND_FIRST_COMMAND) currentCmdBufferIdx = 0;
    for(i = 0; i < length; i++)
    {
        /*Buffer all control command and data*/
        if((i == (length - 1)) &&(startStopCont & I2CSEND_LAST_COMMAND) )
            cmdBuffer[(i * 2) + currentCmdBufferIdx] = CONTROL_BYTE_CMD;
        else 
            cmdBuffer[(i * 2) + currentCmdBufferIdx] = CONTROL_BYTE_CMD_CONTINUATION;
        cmdBuffer[(i * 2) + 1 + currentCmdBufferIdx] = value & 0xFF;
        value >>= 8;        
    }
    currentCmdBufferIdx += 2*length;
	if(startStopCont & I2CSEND_LAST_COMMAND)
		commandBufferComplete = 1;  
}

#ifdef TEST_CODE
uint8 testFlag=0;
#endif

static int PCBTEST_spiPortLock()
{	
#ifdef SPI_PORT_LOCK
	uint16 psKey=0;
	uint16 lengthRetrieved=0;
	// Set customer key
	uint32 custKey[4] = {0x6F6E65BB, 0x776F74AA, 0x726565BB, 0x6F7572BB}; //Remember to save the same key to unlock SPI
	
	int n=0;
	int32	iSuccess;
	int		lockStatus;
#ifdef TEST_CODE
	if(testFlag) // Lock SPI and try to read status thru USB, 
	{
		if(PeriphiralOpen(PERIPHIRAL_ID_DUT, USB_CONNECTION))
		{
			if(checkLockStatus(dutHandle)== SPI_LOCKED)
				return 2;
		}
	}
	else
	{
		if(PeriphiralOpen(PERIPHIRAL_ID_DUT))
	{	
		psRead(dutHandle, HOST_INTERFACE, 0, 1, &psKey, &lengthRetrieved);
		psKey |= ENABLE_USB;
		if(psWrite(dutHandle, HOST_INTERFACE, 0, 1, &psKey))
		{

		}
		else
		{
			SetStatus("USB Enbl Fail");
			return 0;
		}

#else
	//if(PeriphiralOpen(PERIPHIRAL_ID_DUT, SPI_CONNECTION))
	if(PeriphiralOpen(PERIPHIRAL_ID_DUT, USB_CONNECTION))
	{	
		n=0;
		do 
		{	
			iSuccess = psRead(dutHandle, HOST_INTERFACE, 0, 1, &psKey, &lengthRetrieved);
		}
		while ((iSuccess != TE_OK) && ((n++)<5));

		if (iSuccess != TE_OK)
		{
			SetStatus("psRead: HOST_INTERFACE Fail");
			WriteLogFile("PCBTEST_spiPortLock: psRead HOST_INTERFACE on DUT failed.");
			return 0;
		}

		psKey |= ENABLE_USB;

		n=0;
		do 
		{	
			iSuccess = VerifyDutHandle(dutHandle);
			if (iSuccess == TE_OK)
			{
				iSuccess = /*psWrite*/sppsWrite(dutHandle, HOST_INTERFACE, 0, 1, &psKey);
			}
		}
		while ((iSuccess != TE_OK) && ((n++)<5));

		if(iSuccess == TE_OK/*psWrite(dutHandle, HOST_INTERFACE, 0, 1, &psKey)*/)
		{

		}
		else
		{
			SetStatus("psWrite: HOST_INTERFACE Fail");
			WriteLogFile("PCBTEST_spiPortLock: psWrite HOST_INTERFACE on DUT failed.");
			return 0;
		}
#endif

		lockStatus = checkLockStatus(dutHandle); 
		if(lockStatus == SPI_LOCKED)
		{	
			SetStatus("RUN RESTORE LOCK");
			WriteLogFile("PCBTEST_spiPortLock: CHK LOCK PASS");
			return 1;
		}
		else if(lockStatus == -1)
		{
			SetStatus("SetCustomerKey Fail");
			WriteLogFile("PCBTEST_spiPortLock: SetCustomerKey Fail");
			return 0;
		}
		else if (lockStatus == SPI_UNLOCKED_CUSTOMER_KEY_NOT_SET)
		{	// set customer key
			if (!(bccmdSetSpiLockCustomerKey(dutHandle, custKey) == TE_OK))	
			{
				// customer key is already set and is stored in the flash
				/*if(!(checkLockStatus(dutHandle)== SPI_UNLOCKED_CUSTOMER_KEY_SET))
				{
					SetStatus("RUN RESTORE LOCK");
					WriteLogFile("PCBTEST_spiPortLock: RUN RESTORE LOCK");
					return 1; //erase the flash and clear the customer key stored before intiating the lock, otherwise fails
				}
				else
				{
					SetStatus("CHK LOCK read FAILED");
					WriteLogFile("PCBTEST_spiPortLock: CHK LOCK status FAILED");
					return 0;
				}*/
				
				
				
			}
		}

		n=0;
		do 
		{	
			iSuccess =  bccmdSpiLockInitiateLock(dutHandle);
		}
		while ((iSuccess != TE_OK) && ((n++)<5));

		//initiate SPI lock
		if (/*bccmdSpiLockInitiateLock(dutHandle)*/iSuccess == TE_OK)
		{	
			lockStatus = checkLockStatus(dutHandle);
			if(lockStatus == SPI_LOCKED)
			{
				//bccmdSetWarmReset(dutHandle, /*1000*/15000);
				SetStatus("CHK LOCK PASS");
				WriteLogFile("PCBTEST_spiPortLock: CHK LOCK PASS");
#ifdef TEST_CODE
			testFlag = 1;
#endif
				return 1;
			}
			else
			{
				SetStatus("CHK LOCK FAILED-2");
				WriteLogFile("PCBTEST_spiPortLock: FAILED, SPI not LOCKED.");
				return 0;
			}
		}	
		else
		{
			int error = teGetLastError(dutHandle);
			
			SetStatus("BCCMDLOCKSTATUS FAILED");
			WriteLogFile("PCBTEST_spiPortLock: bccmdSpiLockInitiateLock on DUT failed.");
			return 0;
		}
	}	
	//CsrDevicesClose();// You should close the handle to avoid false IO overlap
#endif	
	return 0;
}

static int checkLockStatus(uint32 dutHandle)
{
	uint16 spiStatus=0;
	int n=0;
	int32	iSuccess;
	
	// check lock status	
	n=0;
	do 
	{	
		iSuccess = bccmdGetSpiLockStatus(dutHandle, &spiStatus);
	}
	while ((iSuccess != TE_OK) && ((n++)<5));

	if (/*bccmdGetSpiLockStatus(dutHandle, &spiStatus)*/ iSuccess == TE_OK)
	{		
		switch(spiStatus)
		{			
			case 2:
				//SPI unlocked, customer key set								
				return SPI_UNLOCKED_CUSTOMER_KEY_SET;

			case 3:
				//SPI locked, customer key set
				return SPI_LOCKED;

			case 0:
				//SPI unlocked, customer key not set	
				return SPI_UNLOCKED_CUSTOMER_KEY_NOT_SET;
			case 1:
				//SPI locked, customer key not set									
			default:
				//unknown SPI lock status
				WriteLogFile("checkLockStatus: spiStatus is %d", spiStatus);
				WriteLogFile("if spiStatus is 0: SPI unlocked, customer key not set");
				WriteLogFile("if spiStatus is 1: SPI locked, customer key not set");
				WriteLogFile("otherwise: unknown SPI lock status");
				break;
		}			
	}
	else
	{	
		WriteLogFile("checkLockStatus: bccmdGetSpiLockStatus on DUT failed.");
	}
	return -1;
}

int StartSerialFlashScript()
{
	PROCESS_INFORMATION processInformation = {0};
	STARTUPINFO startupInfo                = {0};
	startupInfo.cb                         = sizeof(startupInfo);
	//int nStrBuffer                         = cmdLine.GetLength() + 50;
	DWORD exitCode;

	// Create the process
	BOOL result = CreateProcess(NULL, "01_serialFlash.bat"/*cmdLine.GetBuffer(nStrBuffer)*/, 
								NULL, NULL, FALSE, 
								NORMAL_PRIORITY_CLASS /*| CREATE_NO_WINDOW*/, 
								NULL, NULL, &startupInfo, &processInformation);
	//cmdLine.ReleaseBuffer();
 
	if (!result)
	{
		// CreateProcess() failed
		// Get the error from the system
		/*LPVOID lpMsgBuf;
		DWORD dw = GetLastError();
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
                    NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL);
 
		// Display the error
		CString strError = (LPTSTR) lpMsgBuf;
		TRACE(_T("::executeCommandLine() failed at CreateProcess()\nCommand=%s\nMessage=%s\n\n"), cmdLine, strError);
 
		// Free resources created by the system
		LocalFree(lpMsgBuf);*/
 
		// We failed.
		return FALSE;
	}
	else
	{
		// Successfully created the process.  Wait for it to finish.
		WaitForSingleObject( processInformation.hProcess, INFINITE );
 
		// Get the exit code.
		result = GetExitCodeProcess(processInformation.hProcess, &exitCode);
 
		// Close the handles.
		CloseHandle( processInformation.hProcess );
		CloseHandle( processInformation.hThread );
 
		if (!result)
		{
			// Could not get exit code.
			//TRACE(_T("Executed command but couldn't get exit code.\nCommand=%s\n"), cmdLine);
			return FALSE;
		}
 
		// We succeeded.
		return TRUE;
	}
	return FALSE;
}

static int PCBTEST_serialFlashCheck()
{
#ifdef FLASH_CHECK
	char str1[7];
	string Str_expected="Success";
	FILE *sFile;
	ChangeCurrent(MULTIMETER_CHANGE_DC_CURRENT_RANGE_TO_2_AMP_COMMAND_STR);
	//if(PeriphiralOpen(PERIPHIRAL_ID_DUT))
	CloseDUT(); // You should close the handle to avoid false IO overlap
	system("01_serialFlash.bat"); // Script to erase flash and burn

	/*if (StartSerialFlashScript() == FALSE)
	{	SetStatus("Unable to run Serial Flash bat");
		return 0;
	}*/

	sFile = fopen("serialFlash.txt","r+");
	if(sFile)
	{
		fseek(sFile, -9 ,SEEK_END);
		fgets(str1, 8,sFile);
		if(ferror (sFile))
		{
			SetStatus("FILE ReadError");
			return 0;
		}
		fclose(sFile);
		if(Str_expected.compare(str1)==0)
		{	
			SetStatus("WRITE SUCESS, WAITING FOR VERIFICATION");
			//return 1;
		}
		else  //modified by Ben for Flash failed
		{	
			SetStatus("FAILURE");
			return 0;
		}
	}
	else
	{
		SetStatus("FILE NOT FOUND");
	}

	fclose(sFile);

	if (MatchNameWithConfigFile("Success", "verifyFlashResult.txt")){
        SetStatus("FLASH VERIFY OK");
        return 1;
	}else{
        SetStatus("FLASH VERIFY FAIL");
        return 0;
	}


	//CsrDevicesClose();// You should close the handle to avoid false IO overlap
#endif
	return 0;	
}

static int PCBTEST_accelerometerCheck()
{
#ifdef ACCELEROMETER_TEST
	//read the device id and compare. 8451 has device id = 1A
	uint32 i2ctransfer;
	uint8 data = WHO_AM_I ; 
	uint16 count;
#if 1 /* Seavia 20150810 , fix unreferenced local variable warning*/
	uint32 errLines;
#else
	uint32 errLines, iSuccess,direction,value;
#endif

#ifdef BLUETOOTH_TESTCODE
	uint16 bAddress[BLUETOOTH_ADDRESS_LENGTH] = {0x0000, 0x0002, 0x0077, 0x600f}; //Test Address
	uint16 licenseKey[LICENSE_KEY_LENGTH] = {0x3465, 0x5679, 0x4B21, 0xBCB8, 0x0000}; //test key CVC Handsfree
#endif

#ifdef AUTO_PAIRING
automaticPairingModeEnable(USB_CONNECTION, 1);
#endif


#ifdef PAIRING_INFO_TEST_CODE
	pairingInfoWrite(&count, 1);
#endif

#ifdef PAIRING_INFO_CLEAR_TEST_CODE
	pairingInfoClear();
#endif

	if(PeriphiralOpen(PERIPHIRAL_ID_DUT, SPI_CONNECTION))
	{	

		    bccmdSetColdReset(dutHandle, 0);
			spMapPio32(dutHandle, 0x00040000, 0xFFFFFFFF, &errLines);



			spSetPio32(dutHandle, 0x40000, 0x40000, 
											 0x00000,  &errLines);

			Sleep(200);
			spSetPio32(dutHandle, 0x40000, 0x40000, 
											 0x40000,  &errLines);
			Sleep(200);


		i2ctransfer = bccmdI2CTransfer(dutHandle , ACCELEROMETER_SLAVE_ADDRESS, 1,
					   1, 1, &data,
						&count);	
		if((i2ctransfer == 1) && ((data == 26)||(data == 13)))
		{
			//CsrDevicesClose();// You should close the handle to avoid false IO overlap
#ifdef BLUETOOTH_TESTCODE
			if(write_bAddPlusLicenseKey(bAddress, licenseKey))
			return 1;
#else
			return 1;
#endif
			
		}
		else
		{
			SetStatus("I2CTRFR return=%d, data=%d", i2ctransfer, data);
			return 0;
		}
	}

#endif
	return 0;
}
	
static uint8 write_bAddPlusLicenseKey(uint16 *bAddress, uint16 *licenseKey)
{
	int32	iSuccess;
	int		n=0;
#ifdef BADDLICENSE_WRITE
	if((bAddress) && (licenseKey)) // if not NULL pointer
	{
		if (PeriphiralOpen(PERIPHIRAL_ID_DUT, USB_CONNECTION)) 
		{
			n=0;
			do 
			{
				iSuccess = VerifyDutHandle(dutHandle);
				if (iSuccess == TE_OK)
				{
					iSuccess = /*psWrite*/sppsWrite(dutHandle, BLUETOOTH_ADDRESS, 0, BLUETOOTH_ADDRESS_LENGTH , bAddress);
				}
			}
			while ((iSuccess != TE_OK) && ((n++)<5));

			if (iSuccess != TE_OK)
			{
				WriteLogFile("write_bAddPlusLicenseKey: psWrite BLUETOOTH_ADDRESS on DUT failed"); 
				return 0;
			}
			
			n=0;
			do 
			{
				iSuccess = VerifyDutHandle(dutHandle);
				if (iSuccess == TE_OK)
				{
					iSuccess = /*psWrite*/sppsWrite(dutHandle, LICENSE_KEY, 0, LICENSE_KEY_LENGTH , licenseKey);
				}
			}
			while ((iSuccess != TE_OK) && ((n++)<5));

			if (iSuccess != TE_OK)
			{
				WriteLogFile("write_bAddPlusLicenseKey: psWrite LICENSE_KEY on DUT failed"); 
				return 0;
			}

			/*if(psWrite(dutHandle, BLUETOOTH_ADDRESS, 0, BLUETOOTH_ADDRESS_LENGTH , bAddress)
				&&(psWrite(dutHandle, LICENSE_KEY, 0, LICENSE_KEY_LENGTH , licenseKey)));*/
				return 1;
		}
	}
#endif
	return 0;
}

//John 07302015: write it...Orz
static uint8 Write_Serial_Number(uint16 *bAddress) {
	int32	iSuccess;
	int n;

	if (PeriphiralOpen(PERIPHIRAL_ID_DUT, USB_CONNECTION)) {
			n=0;
			do 
			{
				iSuccess = VerifyDutHandle(dutHandle);
				if (iSuccess == TE_OK)
				{
					char hex1[5], hex2[5];
					sprintf(hex1, "%02X", bAddress[0]);
					sprintf(hex2, "%04X", bAddress[1]); 
					puts(hex1);
					puts(hex2);

					/* John 8/3/2015: add an extra '0' to hex1 if there is only one digit
						concatonate snPrefix with bluetooth address
						e.g.: 
							"MVR03COR10C157" + "00" + "684f"
					*/
					
					char snCandidate[24];
					memset(snCandidate, 0, 24);

					strcpy(snCandidate, snPrefix);
					/*
					if (bAddress[0] < 16) {
						strcat(snCandidate, "0");
					}
					*/
					strcat(snCandidate, hex1);
					strcat(snCandidate, hex2);

					/*
						John 8/18/2015 - fix serial number being wrong, don't mess with snPrefixLen, that is global
					*/
					uint16 snCandidateLen = snPrefixLen;
					snCandidateLen += 6;

					convert2U16Arr(snPrefix_u16, snCandidate, snCandidateLen);
					snPrefixLen_u16 = (snCandidateLen + 1) / 2;

					for(int i = 0; i < snPrefixLen_u16; i++) {
						spSerialNumber_u16[i] = snPrefix_u16[i];
					}
					
					iSuccess = /*psWrite*/sppsWrite(dutHandle, SERIALNUMBER_ADDRESS, 0, SERIALNUMBER_LENGTH , spSerialNumber_u16);

					WriteLogFile("WriteSerialNumber: Serial Number is %s", snCandidate);
					WriteMainLogFile("WriteSerialNumber: Serial Number is %s", snCandidate);
				}
			}
			while ((iSuccess != TE_OK) && ((n++)<5));
	}
	else {

		WriteLogFile("WriteSerialNumber Fail");
		WriteMainLogFile("WriteSerialNumber Fail");
		return 0;
	}
	return 0;
}

uint8 pairingInfoWrite(uint16 *data, uint16 length)
{
	// data : input from log file 
	//this always edits the first paired device info...device info keeps adding when more devices are paired
	//SPI used since its done during pcb test..this should be done before SPI locking 
int32	iSuccess;
int		n=0;
#ifdef PAIRING_INFO_WRITE
	uint16 pairingInfo[PAIRING_INFO_LENGTH] = {0x0001, 0x0005, 0x0005, 0x0000, 0x0000};
	uint16 firstPairingDevice[ FIRST_PAIRING_DEVICE_LENGTH ] = {0xfff0, 0xffff};
	if(PeriphiralOpen(PERIPHIRAL_ID_DUT, USB_CONNECTION))
	{
		if(automaticPairingModeEnable(USB_CONNECTION, 0) != 1) 
			return 0;
		/*if((psWrite(dutHandle, PAIRING_INFO, 0, PAIRING_INFO_LENGTH , pairingInfo))
				&&(psWrite(dutHandle, FIRST_PAIRING_DEVICE_INFO, 0, FIRST_PAIRING_DEVICE_LENGTH , firstPairingDevice))
				&&(psWrite(dutHandle, RANDOM_DATA_SAMPLE, 0, length , data)))*/

		n=0;
		do 
		{
			iSuccess = VerifyDutHandle(dutHandle);
			if (iSuccess == TE_OK)
			{
				iSuccess = /*psWrite*/sppsWrite(dutHandle, PAIRING_INFO, 0, PAIRING_INFO_LENGTH , pairingInfo);
			}
		}
		while ((iSuccess != TE_OK) && ((n++)<5));

		if (iSuccess != TE_OK)
		{
			WriteLogFile("pairingInfoWrite: psWrite PAIRING_INFO on DUT failed"); 
			return 0;
		}
			
		n=0;
		do 
		{
			iSuccess = VerifyDutHandle(dutHandle);
			if (iSuccess == TE_OK)
			{
				iSuccess = /*psWrite*/sppsWrite(dutHandle, FIRST_PAIRING_DEVICE_INFO, 0, FIRST_PAIRING_DEVICE_LENGTH , firstPairingDevice);
			}
		}
		while ((iSuccess != TE_OK) && ((n++)<5));

		if (iSuccess != TE_OK)
		{
			WriteLogFile("pairingInfoWrite: psWrite FIRST_PAIRING_DEVICE_INFO on DUT failed"); 
			return 0;
		}

		n=0;
		do 
		{
			iSuccess = VerifyDutHandle(dutHandle);
			if (iSuccess == TE_OK)
			{
				iSuccess = /*psWrite*/sppsWrite(dutHandle, RANDOM_DATA_SAMPLE, 0, length , data);
			}
		}
		while ((iSuccess != TE_OK) && ((n++)<5));
		
		if (iSuccess != TE_OK)
		{
			WriteLogFile("pairingInfoWrite: psWrite RANDOM_DATA_SAMPLE on DUT failed"); 
			return 0;
		}
		
		n=0;
		do 
		{
			iSuccess = VerifyDutHandle(dutHandle);
			if (iSuccess == TE_OK)
			{
				iSuccess = psClear(dutHandle, USER_CONFIGURATION_DATA_49, 0);
			}
		}
		while ((iSuccess != TE_OK) && ((n++)<5));
		if (iSuccess != TE_OK)
		{
			WriteLogFile("pairingInfoWrite: psClear USER_CONFIGURATION_DATA_49 on DUT failed"); 
			return 0;
		}

		//{
			iSuccess = bccmdSetWarmReset(dutHandle, /*5000*/15000);
			if(iSuccess != TE_OK)
			{
				SetStatus("AUTOPAIRENABLE RESET FAIL");
				return 0;
			}
			return 1;
		//}
	}
#endif
	return 0;
}

static uint8 pairingInfoClear(void)
{
	uint32 iSuccess;
	//erases pairing info thru USB connection before setting factory defaults
#ifdef PAIRING_INFO_CLEAR
	if(PeriphiralOpen(PERIPHIRAL_ID_DUT, USB_CONNECTION))
	{
		if (automaticPairingModeDisable(USB_CONNECTION) != 1)
		{
			SetStatus("pairingInfoClear automaticPairingModeDisable FAIL");
			return 0;
		}
		if((psClear(dutHandle, PAIRING_INFO, PS_STORES_I)) 
			&& (psClear(dutHandle, FIRST_PAIRING_DEVICE_INFO , PS_STORES_I)) 
			&& (psClear(dutHandle, RANDOM_DATA_SAMPLE, PS_STORES_I)))
		{
			
			iSuccess = bccmdSetWarmReset(dutHandle, /*5000*/15000);
			if(iSuccess != TE_OK)
			{
				SetStatus("pairingInfoClear RESET FAIL");
				return 0;
			}
			return 1;
		}

	}
#endif
	return 0;
}

int WriteBluetoothAddress()
{

#if 0 /* Seavia 20150810 , fix unreferenced local variable warning*/
	int readBtAddrResult;
	uint16	data[1], len;
#endif
	int32	iSuccess;
	int		n=0;
	uint16  nap;
	uint8   uap;
	uint32  lap;
	int success_flag=0, licenseKeysIndex, licenseKeysSegmentIndex;
	uint16 newLicenseKeys[LICENSE_KEY_LENGTH];
	int iAnswer;
	
	//if (handle != dutHandle)
		//return 0;
	if (PeriphiralOpen(PERIPHIRAL_ID_DUT, USB_CONNECTION)) 
	{
		n=0;
		do 
		{
			iSuccess = psReadBdAddr(/*handle*/dutHandle, (uint32 *)&lap, (uint8 *)&uap, (uint16 *)&nap);
		}
		while ((iSuccess != TE_OK) && ((n++)<5));
		
		if ((iSuccess == TE_OK) && (nap == DUT_NAP/*0x600f*/) && (uap == /*77*/DUT_UAP) && (lap > 0x1))
		{	
			sprintf(currentBluetoothAddressString, "%04x%02x%06x", nap, uap, lap);
			currentBluetoothAddrDisplayStr = "CURR_BT_ADDR: 0x";
			currentBluetoothAddrDisplayStr += /*currentBluetoothAddrStr*/currentBluetoothAddressString;
			WriteLogFile("WriteBluetoothAddress: Bluetooth Address is %s", currentBluetoothAddressString);
			WriteMainLogFile("WriteBluetoothAddress: Bluetooth Address is %s", currentBluetoothAddressString);
			return 1;
		}
		else if(iSuccess == TE_OK)
		{
			availBluetoothAddrLap = getAvailBluetoothAddr(success_flag);
			if (success_flag)
			{	
				bAddress[0] = (availBluetoothAddrLap >> 16)&0xFFFF;
				bAddress[1] = availBluetoothAddrLap&0xFFFF; 
				bAddress[2] = DUT_UAP;
				bAddress[3] = DUT_NAP; 

				for (licenseKeysIndex=0; licenseKeysIndex<num_of_licenseKeys; licenseKeysIndex++)
				{
					if ((availBluetoothAddrLap >= licenseKeys[licenseKeysIndex].startingBluetoothAddr) 
						&&
					   (availBluetoothAddrLap <= licenseKeys[licenseKeysIndex].endingBluetoothAddr))
					{
						for(licenseKeysSegmentIndex=0;licenseKeysSegmentIndex<LICENSE_KEY_LENGTH; licenseKeysSegmentIndex++) 
						{
							newLicenseKeys[licenseKeysSegmentIndex] = licenseKeys[licenseKeysIndex].licenseKey[licenseKeysSegmentIndex];
						}
						break;
					}
				}
				
				if (licenseKeysIndex >= num_of_licenseKeys)
				{	
					//product_tests_overall_result = NO_AVAIL_LICENSE_KEY;
					iAnswer = AfxMessageBox("Talk to Manager, No More License Keys", MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2);
					SetStatus("Talk to Manager, No More License Keys");
					WriteLogFile("WriteBluetoothAddress: Failed, No More License Keys");
					WriteMainLogFile("WriteBluetoothAddress: Failed, No More License Keys");
					return 0;
				}
				else
				{ 
					if(write_bAddPlusLicenseKey(bAddress, newLicenseKeys) == 1)
					{	
						AddBlueToothAddrToAssignedBlueToothAddrsFile(availBluetoothAddrLap);
						availBluetoothAddrLap = getAvailBluetoothAddr(success_flag);
						if (success_flag)
						{	
							currentavailBluetoothAddrStr.Format("AVAIL_BT_ADDR 0x%04x%02x%06x", availBluetoothAddrNap, availBluetoothAddrUap, availBluetoothAddrLap);
						}
						else
						{	
							iAnswer = AfxMessageBox("Talk to Manager, No More BlueToothAddress for next test1", MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2);
							currentavailBluetoothAddrStr.Format("AVAIL_BT_ADDR: NONE");
							SetStatus("AVAIL_BT_ADDR: NONE");
							return 1;
						}
						//

						if (ReadDutBluetoothAddr(currentBluetoothAddressString))
						{
							currentBluetoothAddrDisplayStr = "CURR_BT_ADDR: 0x";
							currentBluetoothAddrDisplayStr += /*currentBluetoothAddrStr*/currentBluetoothAddressString;
							WriteLogFile("WriteBluetoothAddress: Bluetooth Address is %s", currentBluetoothAddressString);
							WriteMainLogFile("WriteBluetoothAddress: Bluetooth Address is %s", currentBluetoothAddressString);
							return 1;
						}
						else
						{	
							currentBluetoothAddrDisplayStr = "CURR_BT_ADDR: Unknown";
							SetStatus("CURR_BT_ADDR: Unknown");
							WriteLogFile("WriteBluetoothAddress: CURR_BT_ADDR: Unknown");
							return 0;
						}
					}
					else
					{
						//product_tests_overall_result = WRITE_BT_ADDR_OR_LICENSE_KEY_FAILED;
						SetStatus("BT ADDR WRITE FAIL");
						WriteLogFile("WriteBluetoothAddress: BT ADDR WRITE FAIL");
						WriteMainLogFile("WriteBluetoothAddress: BT ADDR WRITE FAIL");
						return 0;
					}
				}
			}
			else
			{	 
				iAnswer = AfxMessageBox("Talk to Manager, No More BlueToothAddress", MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2);
				SetStatus("Talk to Manager, No More BlueToothAddress");
				WriteLogFile("WriteBluetoothAddress: FAILED, No More BlueToothAddress");
				WriteMainLogFile("WriteBluetoothAddress: FAILED, No More BlueToothAddress");
				return 0;
			}
		}
		else
		{
			WriteLogFile("WriteBluetoothAddress: psReadBdAddr on DUT failed");
		}
	}
	return 0;
}

//John 07302015: Testing

int WriteBlutoothAddress_And_SerialNumber()
{
#if 0 /* Seavia 20150810 , fix unreferenced local variable warning*/
	int readBtAddrResult;
	uint16	data[1], len;
#endif
	int32	iSuccess;
	int		n=0;
	uint16  nap;
	uint8   uap;
	uint32  lap;
	int success_flag=0, licenseKeysIndex, licenseKeysSegmentIndex;
	uint16 newLicenseKeys[LICENSE_KEY_LENGTH];
	int iAnswer;
	
	//if (handle != dutHandle)
		//return 0;
	if (PeriphiralOpen(PERIPHIRAL_ID_DUT, USB_CONNECTION)) 
	{
		n=0;
		do 
		{
			iSuccess = psReadBdAddr(/*handle*/dutHandle, (uint32 *)&lap, (uint8 *)&uap, (uint16 *)&nap);
		}
		while ((iSuccess != TE_OK) && ((n++)<5));
		
		if ((iSuccess == TE_OK) && (nap == DUT_NAP/*0x600f*/) && (uap == /*77*/DUT_UAP) && (lap > 0x1))
		{	
			sprintf(currentBluetoothAddressString, "%04x%02x%06x", nap, uap, lap);
			currentBluetoothAddrDisplayStr = "CURR_BT_ADDR: 0x";
			currentBluetoothAddrDisplayStr += /*currentBluetoothAddrStr*/currentBluetoothAddressString;
			/* John 8/24/2015: don't update address in log */
			update_bool = false;
			WriteLogFile("WriteBluetoothAddress: Bluetooth Address is %s", currentBluetoothAddressString);
			WriteMainLogFile("WriteBluetoothAddress: Bluetooth Address is %s", currentBluetoothAddressString);
			return 1;
		}
		else if(iSuccess == TE_OK)
		{
			availBluetoothAddrLap = getAvailBluetoothAddr(success_flag);
			if (success_flag)
			{	
				bAddress[0] = (availBluetoothAddrLap >> 16)&0xFFFF;
				bAddress[1] = availBluetoothAddrLap&0xFFFF; 
				bAddress[2] = DUT_UAP;
				bAddress[3] = DUT_NAP; 

				for (licenseKeysIndex=0; licenseKeysIndex<num_of_licenseKeys; licenseKeysIndex++)
				{
					if ((availBluetoothAddrLap >= licenseKeys[licenseKeysIndex].startingBluetoothAddr) 
						&&
					   (availBluetoothAddrLap <= licenseKeys[licenseKeysIndex].endingBluetoothAddr))
					{
						for(licenseKeysSegmentIndex=0;licenseKeysSegmentIndex<LICENSE_KEY_LENGTH; licenseKeysSegmentIndex++) 
						{
							newLicenseKeys[licenseKeysSegmentIndex] = licenseKeys[licenseKeysIndex].licenseKey[licenseKeysSegmentIndex];
						}
						break;
					}
				}
				
				if (licenseKeysIndex >= num_of_licenseKeys)
				{	
					//product_tests_overall_result = NO_AVAIL_LICENSE_KEY;
					iAnswer = AfxMessageBox("Talk to Manager, No More License Keys", MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2);
					update_bool = false;
					SetStatus("Talk to Manager, No More License Keys");
					update_bool = false;
					WriteLogFile("WriteBluetoothAddress: Failed, No More License Keys");
					WriteMainLogFile("WriteBluetoothAddress: Failed, No More License Keys");
					return 0;
				}
				else
				{ 
					if(write_bAddPlusLicenseKey(bAddress, newLicenseKeys) == 1)
					{	
						Write_Serial_Number(bAddress);

						AddBlueToothAddrToAssignedBlueToothAddrsFile(availBluetoothAddrLap);
						availBluetoothAddrLap = getAvailBluetoothAddr(success_flag);
						if (success_flag)
						{	
							currentavailBluetoothAddrStr.Format("AVAIL_BT_ADDR 0x%04x%02x%06x", availBluetoothAddrNap, availBluetoothAddrUap, availBluetoothAddrLap);
						}
						else
						{	
							currentavailBluetoothAddrStr.Format("AVAIL_BT_ADDR: NONE");
							update_bool = false;
							SetStatus("AVAIL_BT_ADDR: NONE");
							g_BT_address_runout = 1;
							return 1;
						}
						//

						if (ReadDutBluetoothAddr(currentBluetoothAddressString))
						{
							currentBluetoothAddrDisplayStr = "CURR_BT_ADDR: 0x";
							currentBluetoothAddrDisplayStr += /*currentBluetoothAddrStr*/currentBluetoothAddressString;
							update_bool = false;
							update_bool = false;
							WriteLogFile("WriteBluetoothAddress: Bluetooth Address is %s", currentBluetoothAddressString);
							WriteMainLogFile("WriteBluetoothAddress: Bluetooth Address is %s", currentBluetoothAddressString);
							return 1;
						}
						else
						{	
							currentBluetoothAddrDisplayStr = "CURR_BT_ADDR: Unknown";
							SetStatus("CURR_BT_ADDR: Unknown");
							update_bool = false;
							update_bool = false;
							WriteLogFile("WriteBluetoothAddress: CURR_BT_ADDR: Unknown");
							return 0;
						}
					}
					else
					{
						//product_tests_overall_result = WRITE_BT_ADDR_OR_LICENSE_KEY_FAILED;
						update_bool = false;
						SetStatus("BT ADDR WRITE FAIL");
						update_bool = false;
						WriteLogFile("WriteBluetoothAddress: BT ADDR WRITE FAIL");
						WriteMainLogFile("WriteBluetoothAddress: BT ADDR WRITE FAIL");
						return 0;
					}
				}
			}
			else
			{	 
				iAnswer = AfxMessageBox("Talk to Manager, No More BlueToothAddress", MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2);
				update_bool = false;
				SetStatus("Talk to Manager, No More BlueToothAddress");
				update_bool = false;
				WriteLogFile("WriteBluetoothAddress: FAILED, No More BlueToothAddress");
				WriteMainLogFile("WriteBluetoothAddress: FAILED, No More BlueToothAddress");
				return 0;
			}
		}
		else
		{
			update_bool = false;
			WriteLogFile("WriteBluetoothAddress: psReadBdAddr on DUT failed");
		}
	}
	return 0;
}

#if 1 /* Seavia 20150810 import model name from config file : models.txt*/
int ReadModelFile(char *file_name)
{
	string model_name;
	CString model_name_cstr;
	int i=0;

	model_list = 0;

	ifstream ModelFile (/*CONFIG_FILE_NAME*/file_name);
	if (ModelFile.fail())
	{
		WriteMainLogFile("ReadModelFile: %s is not found.", /*CONFIG_FILE_NAME*/file_name); 
		return 0;
    }

	if (ModelFile.is_open())
	{	
		while ( !ModelFile.eof())
		{	
			model_name = " ";
			ModelFile >> model_name;
			model_name_cstr = model_name.c_str();
			strcpy(modelsStrings[i], model_name.c_str());
			//WriteMainLogFile("Model name: %s, %s", modelsStrings[i], model_name.c_str()); 
			i++;
		}
	}
	model_list = i;
	NUM_OF_MODELS = model_list;
	return NUM_OF_MODELS;
}

int ResetConfigFile(char *modelName, char *fileName)
{
	selectedModel = -1;
	if (strstr(modelName, "MVR02"))
	{
		selectedModel = MVS02;
		strcpy(fileName, configFileNames[testType][CONFIG_PVC]);
		WriteMainLogFile("ResetConfigFile :: MVR02 matched, read config : %s", configFileNames[testType][CONFIG_PVC]);
	}
	else if (strstr(modelName, "MVS02"))
	{
		selectedModel = MVS02;
		strcpy(fileName, configFileNames[testType][CONFIG_SVC]);
		WriteMainLogFile("ResetConfigFile :: MVS02 matched, read config : %s", configFileNames[testType][CONFIG_SVC]);
	}
	else if (strstr(modelName, "MNR01")) //reserve for notifier
	{
		selectedModel = MNR01;
		strcpy(fileName, configFileNames[testType][CONFIG_NOTIFIER]);
		WriteMainLogFile("ResetConfigFile :: MNR01 matched, read config : %s", configFileNames[testType][CONFIG_NOTIFIER]);
	}
	else if (strstr(modelName, "GVS02GCM"))
	{
		selectedModel = GVS02;
		strcpy(fileName, configFileNames[testType][CONFIG_GVC41]);
		WriteMainLogFile("ResetConfigFile :: GVS02GCM matched, read config : %s", configFileNames[testType][CONFIG_GVC41]);
	}
	else if (strstr(modelName, "GVS02GCG"))
	{
		selectedModel = GVS02;
		strcpy(fileName, configFileNames[testType][CONFIG_GVC45]);
		WriteMainLogFile("ResetConfigFile :: GVS02GCG matched, read config : %s", configFileNames[testType][CONFIG_GVC45]);
	}
	else
	{
		WriteMainLogFile("ResetConfigFile :: can't recognize model name : %s", modelName);
		return 0;
	}
	return 1;
}

int ResetSNPrefix(char *strptr)
{
	char date_month[4];
	snPrefixLen =0;
	snPrefixLen_u16 =0;
	memset(snPrefix, 0, sizeof(snPrefix));
	memset(snPrefix_u16, 0, sizeof(snPrefix_u16));
	memset(spSerialNumber, 0, sizeof(spSerialNumber));
	memset(spSerialNumber_u16, 0, sizeof(spSerialNumber_u16));

	strcpy(snPrefix, strptr);
	snPrefixLen = strlen(snPrefix);
	//WriteMainLogFile("ResetSNPrefix(1) :: snPrefix : %s, snPrefixLen = %d", snPrefix, snPrefixLen);
	snPrefix[snPrefixLen++] = factoryIndicator[0];
	//WriteMainLogFile("ResetSNPrefix(2) :: snPrefix : %s, snPrefixLen = %d", snPrefix, snPrefixLen);
	getDateMonth(date_month);
	strcat(snPrefix, date_month);
	snPrefixLen += 3;
	convert2U16Arr(snPrefix_u16, snPrefix, snPrefixLen);
	snPrefixLen_u16 = (snPrefixLen+1) / 2;
	WriteMainLogFile("ResetSNPrefix :: snPrefix : %s, snPrefixLen = %d", snPrefix, snPrefixLen);
	return 1;

}
#endif 

int ReadConfigFile(char *file_name)
{

	string config_name, strInput;
	float /*double*/ config_value;
	CString strMessage, config_name_cstr;
	int licenseKeyIndex=0;
#if 0 /* Seavia 20150810 , fix unreferenced local variable warning*/
	uint16 licenseKeySegment;
#endif
	int search_index=0, sscanf_result=0;
	unsigned long startingBueToothAddrRange, endingBueToothAddrRange;
	unsigned long temp_license_keys[LICENSE_KEY_LENGTH];
	unsigned long tempTestSelection = 0; //not really been used

#if 0 /* Seavia 20150612 serial number prefix string*/
	// John: Globaling this, 
	//Globling string sn_prefix_string; (because is used in Production Test as well)
	snPrefixLen =0;
	snPrefixLen_u16 =0;
	memset(snPrefix, 0, sizeof(snPrefix));
	memset(snPrefix_u16, 0, sizeof(snPrefix_u16));
	memset(spSerialNumber, 0, sizeof(spSerialNumber));
	memset(spSerialNumber_u16, 0, sizeof(spSerialNumber_u16));
	WriteMainLogFile("ReadConfigFile: %s", /*CONFIG_FILE_NAME*/file_name); 
#endif /* Seavia end*/

	ifstream ConfigFile (/*CONFIG_FILE_NAME*/file_name);
	if (ConfigFile.fail())
	{
		WriteLogFile("ReadConfigFile: %s is not found.", /*CONFIG_FILE_NAME*/file_name); 
		WriteMainLogFile("ReadConfigFile: %s is not found.", /*CONFIG_FILE_NAME*/file_name); 
		return 0;
    }

	if (ConfigFile.is_open())
	{	
		while ( !ConfigFile.eof())
		{	

			config_name = " ";
			ConfigFile >> config_name;
			config_name_cstr = config_name.c_str();


				search_index = 0;
				while (search_index < (NUM_OF_CONFIGURATIONS + 1))
				{
					if (config_name_cstr.GetString() == config_variables[search_index].config_name)
					{	
						break;
					}
					search_index++;
				}

				if (search_index >= (NUM_OF_CONFIGURATIONS + 1)) 
				{	
					if (!ConfigFile.fail())
					{	WriteLogFile("ReadConfigFile:%s is corrupted. search_index = %d",file_name, search_index); 
						WriteMainLogFile("ReadConfigFile:%s is corrupted. search_index = %d",file_name, search_index); 
						return /*0*/-1;
					}
				}
				else if (search_index == (NUM_OF_CONFIGURATIONS))
				{
					        ConfigFile >> hex >> startingBueToothAddrRange;
					        ConfigFile >> hex >> endingBueToothAddrRange;
						    ConfigFile >> hex >> temp_license_keys[0];
							ConfigFile >> hex >> temp_license_keys[1];
							ConfigFile >> hex >> temp_license_keys[2];
							ConfigFile >> hex >> temp_license_keys[3];
							ConfigFile >> hex >> temp_license_keys[4];

						if (num_of_licenseKeys < MAX_NUMBER_OF_LICENSE_KEYS)
						{
							licenseKeys[num_of_licenseKeys].startingBluetoothAddr = startingBueToothAddrRange;
							licenseKeys[num_of_licenseKeys].endingBluetoothAddr = endingBueToothAddrRange; 
						}
					

						for (int licenseKeySegmentIndex=0; licenseKeySegmentIndex<LICENSE_KEY_LENGTH; licenseKeySegmentIndex++)
						{
								licenseKeys[num_of_licenseKeys].licenseKey[licenseKeySegmentIndex] = (uint16)temp_license_keys[licenseKeySegmentIndex];
						}
						num_of_licenseKeys++;
				}
				else
				{

					if(search_index == 0)
					{
						/*ConfigFile >> hex >> testSelection;
						if(testSelection & 0x8000)
						    config_value = 1;
						else config_value = 0;*/
						ConfigFile >> hex >> tempTestSelection;
						if(tempTestSelection >= 0x8000)
						    config_value = 1;
						else config_value = 0;
					}
#if 0 // John 07302015: Getting Serial_Num_Prefix
					else if(config_variables[search_index].config_name == PRODUCT_SERIAL_NUM_PREFIX_STR) {
						const char *strptr;
						char date_month[4];

						ConfigFile >> sn_prefix_string;

						snPrefixLen = sn_prefix_string.length();
						strptr = sn_prefix_string.c_str();
						//Eddy append year(2 char)+month(1 char) to sn_prefix_string (string) -> snPrefixString (unit 16) 
						// and snPrefix (uint)

		                //Eddy, cover sn_prefix_string to snPrefixString, snprefix is 13 character snPrefixLen should be 7 ?
						strcpy(snPrefix, strptr);
						getDateMonth(date_month);
						strcat(snPrefix, date_month);
						snPrefixLen += 3;
						// convert string to uint16 array snPrefix[]
						// snPrefixLen= length of snPrefix[], add 0 for first 8 bit, if the prefix length is not a even number
						convert2U16Arr(snPrefix_u16, snPrefix, snPrefixLen);
						snPrefixLen_u16 = (snPrefixLen+1) / 2;
						// John: ugly ugly code, fix Min/MAX value in PRODUCT_SERIAL_NUM_PREFIX_STR struct, since it's not a number value
						config_value = 0;
						WriteMainLogFile("ReadConfigFile: SN prefix : %s, snPrefixLen = %d", snPrefix, snPrefixLen); 
					
					}
#endif
#if 1 /*Seavia 20150810 get factory indicator from config file*/
					else if(config_variables[search_index].config_name == PRODUCT_FACTORY_INDICATOR_STR) {
						ConfigFile >> factoryIndicator;
						//WriteMainLogFile("ReadConfigFile: Factory Indicator : %c", factoryIndicator[0]); 
						config_value = 0;
					}
#endif
					else
					{
						ConfigFile >> config_value;
					}

					if((config_variables[search_index].config_value_min_range <= config_value) 
						&&
					   (config_variables[search_index].config_value_max_range >= config_value)
					  )
					{  
						config_variables[search_index].config_value = config_value;
					}
					else
					{	
						strMessage.Format(_T("ReadConfigFile:config_name %s 's value %f is out of range %f - %f"), 
										config_name_cstr, config_value, config_variables[search_index].config_value_min_range, config_variables[search_index].config_value_max_range);
						WriteLogFile(strMessage);
						WriteMainLogFile(strMessage);
					}
				}		
			/*(if (configurations_index > NUM_OF_CONFIGURATIONS)
			{
				return 1;
			}*/
		}
		
	}
	else
	{
		WriteLogFile("ReadConfigFile:%s not been able to be opened.",file_name); 
		WriteMainLogFile("ReadConfigFile:%s not been able to be opened.",file_name); 
		return 0;
	}
	ConfigFile.close();
	return 1;
}

int WriteConfigFile(char *file_name)
{
#if 0
	string config_name;
	double config_value;
	//CString strMessage, config_name_cstr;
	int configurations_index=0;
	unsigned long temp_value;

	ofstream ConfigFile (file_name/*CONFIG_FILE_NAME*/);
	if (ConfigFile.fail())
	{
		WriteLogFile("%s is not found.", file_name/*CONFIG_FILE_NAME*/); 
		return 0;
    }

	for(int i=0; i<NUM_OF_CONFIGURATIONS; i++)
	{
		if (i == 0) 
		{		
			if (tempTestType == PCB_TEST)
			{	
				temp_value = testSelection - 0x8000;
			}
			else
			{
				temp_value = testSelection + 0x8000;
			}
			ConfigFile << config_variables[i].config_name << " " << hex << temp_value << " " << endl;
		}
		else
		{	
			ConfigFile << config_variables[i].config_name << " " << config_variables[i].config_value << " " << endl;
		}
	}

	for (int n=0; n</*num_of_licenseKeys*/1; n++)
	{
		CString licenseKeyStr;
		
		licenseKeyStr.Format("%s 0x%06x 0x%06x %04x %04x %04x %04x %04x", ADDRESS_LICENSEKEYINFORMATION_STR,
																licenseKeys[n].startingBluetoothAddr,licenseKeys[n].endingBluetoothAddr, 
																licenseKeys[n].licenseKey[0], licenseKeys[n].licenseKey[1], 
			                                                    licenseKeys[n].licenseKey[2], licenseKeys[n].licenseKey[3],
																licenseKeys[n].licenseKey[4]);
			
		ConfigFile << licenseKeyStr << endl;	
		
		/*for (int licenseKeySegmentIndex=0; licenseKeySegmentIndex<LICENSE_KEY_LENGTH; licenseKeySegmentIndex++)
		{
			ConfigFile << hex <<  << " ";
		}*/
		//ConfigFile << " " << endl; 
	}
	ConfigFile.close();
#endif
	return 0;
}

int ReadSerialNumberFile(string FileName)
{
	CString strMessage, config_name_cstr;
	uint32 number;
	int Index=0;

	ifstream SerialNumbersFile (FileName.c_str());
	
	if (SerialNumbersFile.fail())
	{	
		WriteLogFile("Can not read %s.", FileName); 
		return 0;
	}
	
	while ( SerialNumbersFile.good() && !SerialNumbersFile.fail())
	{
		SerialNumbersFile >> hex >> number; 
		if ( /*SerialNumbersFile.good() &&*/ !SerialNumbersFile.fail())
		{
			if (FileName == ASSIGNED_SERIAL_NUMBERS_FILENAME)
			{	
				if (Index == 0)
				{	//use the next number as the available serial number
					availSerialNumber = number+1;
				}
			}
			else if (FileName == AVAIL_SERIAL_NUMBERS_FILENAME)
			{
				if (Index == 0)
				{	
					startingAvailSerialNumber = number;
					availSerialNumber = startingAvailSerialNumber;
				}
				else if (Index == 1)
				{
					endingAvailSerialNumber = number;
				}
			}
		}
		else
		{
			if (FileName == AVAIL_SERIAL_NUMBERS_FILENAME)
			{	if (Index <= 1)
				{	
					startingAvailSerialNumber = endingAvailSerialNumber = availSerialNumber = 0;
				}
			}
		}
		Index++;
	}

	SerialNumbersFile.close();
	return 1;
}

#if 1 /*John: WriteBarcode */
int WriteSerialNumberFile(string FileName)
{
	//delete the file first before creating a new one
	DeleteFile(FileName.c_str());
	
	ofstream SerialNumbersFile(FileName.c_str());
	if (SerialNumbersFile.fail())
	{	WriteLogFile("Can not create %s.", FileName); 
		return 0;
	}
	//SerialNumbersFile << snPrefixString << end;
	SerialNumbersFile << serialNumberArray << endl;
    SerialNumbersFile.close();
	return 1;
}
#endif

int CheckBluetoothAddrNotUsed(unsigned long addr)
{	
	/*int found = 0;
	for (unsigned long n=0; n<assignedBluetoothAddrList.size(); n++) 
	{
		if (addr == assignedBluetoothAddrList[n])
		{	found = 1;
		}
    }

	return (!found);*/

	if (assignedBluetoothAddrList.size() > 0)
	{	if (addr > (unsigned long)assignedBluetoothAddrList[assignedBluetoothAddrList.size()-1])
			return 1;
		else
			return 0;
	}
	return 1;
}

void AddBlueToothAddrToAssignedBlueToothAddrsFile(unsigned long addr)
{
	assignedBluetoothAddrList.push_back(addr);
	WriteAssignedBlueToothAddrsFile();
}

int WriteAssignedBlueToothAddrsFile()
{
	ofstream AssignedBlueToothAddrsFile (ASSIGNED_BLUETOOTH_ADDR_LIST_FILENAME);
	if (AssignedBlueToothAddrsFile.fail())
	{	WriteLogFile("WriteAssignedBlueToothAddrsFile:Can not create %s.", ASSIGNED_BLUETOOTH_ADDR_LIST_FILENAME); 
		return 0;
	}

	for (unsigned long n=0; n<assignedBluetoothAddrList.size(); n++) 
	{
		AssignedBlueToothAddrsFile << hex << assignedBluetoothAddrList[n] << endl;
    }
	
	AssignedBlueToothAddrsFile.close();
	return 1;
}

int ReadBlueToothAddrsFile(string FileName)
{
	unsigned long bluetoothAddrNap, bluetoothAddrUap, bluetoothAddrLap;
	CString strMessage, config_name_cstr;
	int bluetoothAddrListIndex=0;

	ifstream BlueToothAddrsFile (FileName.c_str());
	
	if (BlueToothAddrsFile.fail())
	{
		//WriteMainLogFile("ReadBlueToothAddrsFile:%s is not found.", FileName.c_str());
		return 0;
    }

	if (FileName == ASSIGNED_BLUETOOTH_ADDR_LIST_FILENAME)
	{
		assignedBluetoothAddrList.clear();
	}

	if (BlueToothAddrsFile.is_open())
	{	
		while ( BlueToothAddrsFile.good() && !BlueToothAddrsFile.fail())
		{	
			if (FileName == AVAIL_BLUETOOTH_ADDR_LIST_FILENAME)
			{	BlueToothAddrsFile >> hex >> bluetoothAddrNap;
				BlueToothAddrsFile >> hex >> bluetoothAddrUap;
				BlueToothAddrsFile >> hex >> bluetoothAddrLap;
			}
			else if (FileName == ASSIGNED_BLUETOOTH_ADDR_LIST_FILENAME)
			{
				BlueToothAddrsFile >> hex >> bluetoothAddrLap;
			}
			
			if ((!BlueToothAddrsFile.fail())/*&&(BlueToothAddrsFile.good())*/) 
			{

				if (FileName == AVAIL_BLUETOOTH_ADDR_LIST_FILENAME)
				{
					//availBluetoothAddrList.push_back(bluetoothAddr);
					//if this is the first addr in the file, assign it to the starting avail bluetooth addr
					if (bluetoothAddrListIndex == 0)
					{	
						startingAvailBluetoothAddrLap = bluetoothAddrLap;
					} //if this is the second addr in the file, assign it to the ending avail bluetooth addr
					else if (bluetoothAddrListIndex == 1)
					{
						endingAvailBluetoothAddrLap = bluetoothAddrLap;
					}

					availBluetoothAddrNap = bluetoothAddrNap;
					availBluetoothAddrUap = bluetoothAddrUap;
				}
				else
				{
					assignedBluetoothAddrList.push_back(bluetoothAddrLap);
				}
				bluetoothAddrListIndex++;
			}
			else 
			{	if (FileName == AVAIL_BLUETOOTH_ADDR_LIST_FILENAME)
				{
					//if data is corrupted this is the first entry or second entry in assignedBluetoothaddrlist.txt
					//assign both startingAvailBluetoothAddr and endingAvailBluetoothAddr to 0. 
					if (bluetoothAddrListIndex <= 1)
					{	
						startingAvailBluetoothAddrLap = endingAvailBluetoothAddrLap = 0;
						availBluetoothAddrNap = availBluetoothAddrUap = 0;
						BlueToothAddrsFile.close();
						return 0; 
					} 
				}
			}
		}
	}


	BlueToothAddrsFile.close();
	return 1;
}

int OpenMainLogFile(string name)
{
	MainLogFileHd.open (name.c_str(), ios::out | ios::app);
	if (MainLogFileHd.fail())
	{
		return 0;
    }

	return 1;
}

void CloseMainLogFile()
{
	if(MainLogFileHd.is_open())
		MainLogFileHd.close();
}


/*
	John 8/21/2015 v59: add BT address to serial # in log
*/
void WriteMainLogFile(const char * format, ...)
{
	char Message[500];
	string MainLogFileName;
    va_list argptr;
	memset(Message,0,sizeof(Message));
    va_start(argptr, format);
    vsnprintf(Message, sizeof(Message)-1, format, argptr);
    va_end(argptr);

	if (!MainLogFileHd.is_open())
	{
		if (CreateDirectory(LOG_DIRECTORY_NAME))
		{
			MainLogFileName = LOG_DIRECTORY_NAME;
			MainLogFileName += "\\";
			MainLogFileName += "MainLog.txt";
			OpenMainLogFile(MainLogFileName);
		}
	}

	
    time_t ltime; /* calendar time */
	ltime=time(NULL); /* get current cal time */
	char * time = asctime(localtime(&ltime));
	time[strlen(time)-1] = '\0';
	
    if (!MainLogFileHd.fail() && MainLogFileHd.is_open())
	{
		if (testType == PCB_TEST)
			MainLogFileHd << /*configFiles[model_selection].modelName*/"PCBA" << " " <<  time << " " << "serial #" << serialNumber << " " << Message << endl;   
		/* John: use BT address instead of serial # if available */
		else if(serialNumber != 0)
			MainLogFileHd << /*configFiles[model_selection].modelName*/modelsStrings[model_selection] << " " <<  time << " " << hex << "BT address " << bAddress2[0] << bAddress2[1] << ", " << "serial # " << serialNumber << ": "<< Message << endl;
		else
			MainLogFileHd << /*configFiles[model_selection].modelName*/modelsStrings[model_selection] << " " <<  time << " " << "serial #" << serialNumber << " " << Message << endl;   
	}
}

int OpenLogFile()
{	
	char currentSerialNumberLogFilename[100], directory_name[100], IndividualLogFileName[100];
#if 0 /*Seavia 20150910 fix warning : unreferenced local variable*/
	int directory_name_int;
#endif
	CString directory_name_str;

	if (serialNumber == 0)	
		return 0;

	/* John 8/21/2015: Initialize bAddress2 to be used by log files */
	int success_flag = 0;
	availBluetoothAddrLap = getAvailBluetoothAddr(success_flag);
	if (success_flag && update_bool) {	
		bAddress2[0] = (availBluetoothAddrLap >> 16)&0xFFFF;
		bAddress2[1] = availBluetoothAddrLap&0xFFFF; 
		bAddress2[2] = DUT_UAP;
		bAddress2[3] = DUT_NAP; 
	} else {
		update_bool = false;
	}

	/* John 8/27/2015: add bAddress2[0] and serial number to the log name*/
	char temp1[100], temp2[100];
	memset(temp1, 0x0, sizeof(temp1));
	memset(temp2, 0x0, sizeof(temp2));
	memset(currentSerialNumberLogFilename, 0x0, sizeof(currentSerialNumberLogFilename));
	/* John 8/21/2015: Use bAddress2 as file name */
	sprintf(currentSerialNumberLogFilename, "%02x", bAddress2[0]);
	sprintf(temp1, "%04x", bAddress2[1]);
	sprintf(temp2, "- %x", serialNumber);
	//sprintf(currentSerialNumberLogFilename, "%04x", bAddress2[1]);
	/* John 8/21/2015: Mark off for now */
	//sprintf(currentSerialNumberLogFilename, "%d", serialNumber);
	strcat(currentSerialNumberLogFilename, temp1);
	strcat(currentSerialNumberLogFilename, temp2);
	strcat(currentSerialNumberLogFilename, LOG_FILE_EXT);

	memset(directory_name, 0x0, sizeof(directory_name));
	strncpy(directory_name, LOG_DIRECTORY_NAME, sizeof(directory_name));
	/* John 8/21/2015: Mark off, just in one layer */
	/* John 8/24/2015: Use date as log file directory name */
	strncat(directory_name, "\\", 1); 
	//directory_name_int = serialNumber/256;
	
	/* John 8/24/2015: Get current date */
	time_t now = time(0);
	tm *ltm = localtime(&now);

	string dateString = "", date = "", month = "", year = "";
	stringstream ss;
	ss << (ltm->tm_mday);
	ss >> date;
	ss.str("");
	ss.clear();
	if(date.length() == 1) {
		date.insert(0, "0");
	}
	dateString += date;
	dateString += "-";
	//tmp = numToString(1 + ltm->tm_mon);
	ss << (1 + ltm->tm_mon);
	ss >> month;
	ss.str("");
	ss.clear();
	if(month.length() == 1) {
		month.insert(0, "0");
	}
	dateString += month;
	dateString += "-";
	ss << (1900 + ltm->tm_year);
	ss >> year;
	ss.str("");
	ss.clear();
	dateString += year;

	//directory_name_str.Format("serial#%d-%d", directory_name_int*256, (directory_name_int+1)*256-1);
	directory_name_str = dateString.c_str();
	////strncat(directory_name, currentBluetoothAddressString, strlen(currentBluetoothAddressString)-2);
	strcat(directory_name, directory_name_str);

	if (strcmp(currentLogFileName, currentSerialNumberLogFilename))
	{	
		if (IndividualLogFileHd.is_open())
			IndividualLogFileHd.close();

		CreateDirectory(LOG_DIRECTORY_NAME);
		CreateDirectory(directory_name);

		strncpy(currentLogFileName, currentSerialNumberLogFilename, sizeof(currentLogFileName));
		
		memset(IndividualLogFileName, 0x0, sizeof(IndividualLogFileName)); 
		strncpy(IndividualLogFileName, directory_name, sizeof(IndividualLogFileName));
		strncat(IndividualLogFileName, "\\", 1);
		strncat(IndividualLogFileName, currentLogFileName, sizeof(IndividualLogFileName));

		IndividualLogFileHd.open (IndividualLogFileName, ios::out|ios::app);
		if (IndividualLogFileHd.fail())
		{
			return 0;
		}
		//WriteMainLogFile("log file %s is opened", IndividualLogFileName );
	}
	else
	{
		if (!IndividualLogFileHd.is_open())
		{	
			CreateDirectory(LOG_DIRECTORY_NAME);
			CreateDirectory(directory_name);

			memset(IndividualLogFileName, 0x0, sizeof(IndividualLogFileName)); 
			strncpy(IndividualLogFileName, directory_name, sizeof(IndividualLogFileName));
			strncat(IndividualLogFileName, "\\", 1);
			strncat(IndividualLogFileName, currentLogFileName, sizeof(IndividualLogFileName));
		
			IndividualLogFileHd.open (IndividualLogFileName, ios::out|ios::app);
			if (IndividualLogFileHd.fail())
			{
				return 0;
			}
			//WriteMainLogFile("log file %s is opened", IndividualLogFileName );
		}
	}

	return 1;
}

void WriteLogFile(const char * format, ... )
{
	char Message[500];
    va_list argptr;
	memset(Message,0,500);
    va_start(argptr, format);
    vsnprintf(Message, sizeof(Message)-1, format, argptr);
    va_end(argptr);

	time_t ltime; /* calendar time */
	ltime=time(NULL); /* get current cal time */
	char * time = asctime(localtime(&ltime));
	time[strlen(time)-1] = '\0';

	if (OpenLogFile() == 1)
	{	if (!IndividualLogFileHd.fail() && IndividualLogFileHd.is_open())
		{
			if ((timestampflag) && (newlineflag))
			{	
				if (testType == PCB_TEST)
				{	IndividualLogFileHd << /*configFiles[model_selection].modelName*/ "PCBA" << " " << time << " " << Message << endl;
				}
				else
				{	IndividualLogFileHd << /*configFiles[model_selection].modelName*/ modelsStrings[model_selection] << " " << time << " " << Message << endl;
				}
			}
			else if (timestampflag)
			{	
				if (testType == PCB_TEST)
				{	IndividualLogFileHd << /*configFiles[model_selection].modelName modelsStrings[model_selection]*/ "PCBA" << " " << time << " " << Message;
				}
				else
				{	IndividualLogFileHd << /*configFiles[model_selection].modelName modelsStrings[model_selection]*/ modelsStrings[model_selection] << " " << time << " " << Message;
				}
			}
			else
			{	
				IndividualLogFileHd << /*configFiles[model_selection].modelName modelsStrings[model_selection] << */" " << Message;
			}
		}
		else
		{
			return;
		}
	}
}

void CloseLogFile()
{
	IndividualLogFileHd.close();
}

unsigned long getAvailBluetoothAddr(int &success_flag)
{	
	for (unsigned long currentAvailBluetoothAddrLap=startingAvailBluetoothAddrLap; currentAvailBluetoothAddrLap <=endingAvailBluetoothAddrLap; currentAvailBluetoothAddrLap++)
	{
		if (CheckBluetoothAddrNotUsed(currentAvailBluetoothAddrLap))
		{
			success_flag = 1;
			return currentAvailBluetoothAddrLap;
		}
	}
	success_flag = 0;
	return 0;
}

int CreateLogsDirectories()
{
	string MainLogFileName, LogDirectoryNameStringFormat;
#if 0 /* Seavia 20150810 , fix unreferenced local variable warning*/
	char IndividualLogFilesDirectoryName[1000];
	char IndividualLogFilesName[1000];
#endif

	CString strMessage;

	if (CreateDirectory(LOG_DIRECTORY_NAME))
	{
		MainLogFileName = LOG_DIRECTORY_NAME;
		MainLogFileName += "\\";
		MainLogFileName += "MainLog.txt";
		OpenMainLogFile(MainLogFileName);
	} 

	return 1;
}

int CreateDirectory(CString newPath) 
{	
	if (!CreateDirectory(newPath,NULL)) 
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS) 
		{
			// directory already exists
			return 1;
		} 
		else 
		{
			// creation failed due to some other reason
			return 0;
		}
	}
	return 1;
}



int EnumerateDevices(wchar_t *device_path) {
	SP_DEVICE_INTERFACE_DATA devInfoData;
	int MemberIndex;
	ULONG Length;
	GUID HidGuid;
	HANDLE hDevInfo;
	HANDLE LocDevHandle;
	HIDD_ATTRIBUTES Attributes;
	PSP_DEVICE_INTERFACE_DETAIL_DATA detailData;
	PHIDP_PREPARSED_DATA PreparsedData;
	HIDP_CAPS Capabilities;
	int result=0;

	/* get HID GUID */
	HidD_GetHidGuid(&HidGuid);
	/* get pointer to the device information */
	hDevInfo = SetupDiGetClassDevs(&HidGuid,
			  					   NULL, 
								   NULL, 
								   DIGCF_PRESENT|DIGCF_DEVICEINTERFACE);
	/* go through all the device infos and find devices we are interested in */
	devInfoData.cbSize = sizeof(devInfoData);
	MemberIndex = 0;
	while((SetupDiEnumDeviceInterfaces(hDevInfo, 
								      0, 
									  &HidGuid, 
									  MemberIndex, 
									  &devInfoData))&&(result==0)) {
		/* first get the size of memory needed to hold the device interface info */
		SetupDiGetDeviceInterfaceDetail(hDevInfo, 
		   							    &devInfoData, 
										NULL, 
										0, 
										&Length, 
										NULL);
		/* allocate memory */
		detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(Length);
		/* and set the size in the structure */
		detailData -> cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		/* now get the actual device interface info */
		SetupDiGetDeviceInterfaceDetail(hDevInfo, 
										&devInfoData, 
										detailData, 
										Length, 
										NULL, 
										NULL);
#ifdef DEBUG
		wprintf(L"%s\n",detailData->DevicePath);
#endif
		/* create handle to the device */
		LocDevHandle=CreateFile(detailData->DevicePath, 
				 				GENERIC_READ, 
								FILE_SHARE_READ, 
								(LPSECURITY_ATTRIBUTES)NULL,
								OPEN_EXISTING, 
								FILE_ATTRIBUTE_NORMAL, 
								NULL);
		/* set the size in the structure */
		Attributes.Size = sizeof(Attributes);
		/* get and test the VID and PID */
		if ((LocDevHandle != INVALID_HANDLE_VALUE) &&
			(HidD_GetAttributes(LocDevHandle,&Attributes))&&
			(Attributes.ProductID == 0x0002) &&
			(Attributes.VendorID == 0x2794)) {
			/* found the right device */
			/* is it the right HID collection? */
			HidD_GetPreparsedData(LocDevHandle, &PreparsedData);
			HidP_GetCaps(PreparsedData, &Capabilities);
#if 0
			wprintf(L"%04x %04x\n",Capabilities.UsagePage,Capabilities.Usage);
#endif
			if ((Capabilities.UsagePage == 0xFF00) &&
				(Capabilities.Usage == 0x0001)) {
					/* this is the correct HID collection */
				if (device_path!=NULL) {
					//wcscpy(device_path,detailData->DevicePath);
					strcpy ( device_path_multibytes, detailData->DevicePath );
				}
#ifdef DEBUG				
				wprintf(L"Device Found\n");
#endif
				result=1;
			}
		}
		/* close the device handle again */
		if (LocDevHandle != INVALID_HANDLE_VALUE)
			CloseHandle(LocDevHandle);
		/* and free the memory used to hold device info */
		free(detailData);
		/* try the next device */
		MemberIndex++;
	}
	/* free memory used for the device information set */
	SetupDiDestroyDeviceInfoList(hDevInfo);
	return result;
}

/* returns handle when device found or NULL when not found */
HANDLE OpenDevice(void) {
	wchar_t device_path[MAX_PATH];
	HANDLE DeviceHandle;
	if (EnumerateDevices(device_path)) {
		mbstowcs(device_path, device_path_multibytes, sizeof(device_path_multibytes));
		/* create handle to the device */
		DeviceHandle=CreateFileW(device_path, 
				 				GENERIC_READ | GENERIC_WRITE, 
								FILE_SHARE_READ|FILE_SHARE_WRITE, 
								(LPSECURITY_ATTRIBUTES)NULL,
								OPEN_EXISTING, 
								FILE_ATTRIBUTE_NORMAL, 
								NULL);
		if (DeviceHandle!=INVALID_HANDLE_VALUE) {
			return(DeviceHandle);
		}
	}
	return(NULL);
}

typedef int (*function_ptr)(void);
function_ptr product_tests[NUM_OF_PRODUCT_TEST_ROUTINES] = { &PRODUCT_Initialization, 
															 &PRODUCT_TEST_RFTransmitPowerTest, 
															 &PRODUCT_TEST_RFReceiveSensitivityTest,
															 &PRODUCT_TEST_SpeakerTest,
															 /*&PRODUCT_TEST_oledTest,*/
															 /*&PRODUCT_TEST_MeasurePeakCurrent,*/
															 &PRODUCT_TEST_MicrophoneTest,
															 &PRODUCT_TEST_FactoryEnable	
														   }; 

function_ptr pcb_tests[NUM_OF_PCB_TEST_ROUTINES] = {	&PCBTEST_Initialization,
														&PCBTEST_oledTest,
                                                        &PCBTEST_MeasurePowerSupply, 
														&PCBTEST_CalCrystalOffset,
														&PCBTEST_RFTransmitPowerTest,
														&PCBTEST_RFReceiveSensitivityTest,
														&PCBTEST_LoopbackAndSpeakerOnly,
														&PCBTEST_accelerometerCheck,
														&PCBTEST_PushButtonsTest,
														&PCBTEST_MeasurePeakCurrent,
														&PCBTEST_MeasureStandbyCurrent,
														&PCBTEST_serialFlashCheck
														//&PCBTEST_MeasureChargerCurrent,
														//&PCBTEST_spiPortLock
												   }; 

void CWatchBT_ProductionTestDlg::OnBnClickedStartPcbTest(){	
	
	CString	strMessage, currentBluetoothAddrStr, logfileName, resultstring;
	int  pcb_tests_overall_result=SUCCESS;
	unsigned int Begin = 0, Last = 0;
	unsigned int Now =0;

	int i=0, success_flag=0;
	int pcb_tests_not_complete = 0;
	int abort = 0;
	
	GetDlgItem(IDC_BUTTON_StartPcbTest)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_RestorePcbTest)->EnableWindow(FALSE);

	memset(serialNumberArray, 0x0, sizeof(serialNumberArray));
	memset(TestStatusString, 0, sizeof(TestStatusString));
	// TODO: Add your control notification handler code here
	//OnOK();
	Begin = Last = GetTickCount()/1000;
	MultimeterInitFlag = 0;

	//Routine initialize status 
	for (i=0; i<NUM_OF_PCB_TEST_ROUTINES; i++)
	{	
		GetDlgItem(pcbTestResultsLabelsIDs[i])->SetWindowText("Not started"); 
		GetDlgItem(pcbTestResultsDetailsLabelsIDs[i])->SetWindowText(" ");
		label_text_color = GREY;
		label_text_id = pcbTestResultsLabelsIDs[i];
#ifdef update_color
		GetDlgItem(pcbTestResultsLabelsIDs[i])->SendMessage(WM_CTLCOLORSTATIC);
#endif
		pcb_tests_results[i] = NOT_STARTED;
	}
	label_text_color = GREY;
	label_text_id = IDC_STATIC_PCB_TEST_RESULT;
	GetDlgItem(IDC_STATIC_PCB_TEST_RESULT)->SetWindowText(" ");
	for (i=0; i<NUM_OF_PCB_TEST_ROUTINES/*-1*/; i++){
		if (abort)
			break;

		if (IsDlgButtonChecked(pcbTestCheckBoxesIDs[i]) || (i==0)){
			int retries = (int) GetConfigurationValue(PRODUCT_RX_RETRY_STR);
			if (pcb_tests[i] != NULL){	
				SetStatus("");
				GetDlgItem(pcbTestResultsLabelsIDs[i])->SetWindowText("Progress"); 
#ifndef test_mode
				//int retries = (int) GetConfigurationValue(PRODUCT_RX_RETRY_STR);
				pcb_tests_results[i] = pcb_tests[i]();
				if (pcb_tests_results[i] != SUCCESS){
					if (i == 0)
					{
						//retry init once
						SetStatus("");
						label_text_color = GREY;
						GetDlgItem(pcbTestResultsLabelsIDs[i])->SetWindowText("RETRY");
						GetDlgItem(IDC_STATIC_PRJ_INFO)->SetWindowText(projectInformationStr);
						CsrDevicesClose();
						CloseUSBHandles(UsbGpioModuleConnectionHd);
						CloseUSBHandles(UsbMultimeterModuleConnectionHd);
						Sleep(5000);
						pcb_tests_results[0] = pcb_tests[0]();
					}
					else if ((retries > 0) && (pcb_tests[i] != PCBTEST_serialFlashCheck))//((i >= 3) && (i <= 6))
					{
						label_text_color = GREY;
						GetDlgItem(pcbTestResultsLabelsIDs[i])->SetWindowText("RETRY");
						strMessage.Format("%s", TestStatusString);
						GetDlgItem(pcbTestResultsDetailsLabelsIDs[i])->SetWindowText(strMessage);
						if(bccmdSetColdReset(dutHandle, 5000) != TE_OK)
							WriteLogFile("Retry:DUT bccmdSetColdReset failed");

						//retry all tests except serial flash based on config
						for (; retries > 0; retries--)
						{
							int init = 0;
							for (; init < 2; init++)
							{
								CsrDevicesClose();
								CloseUSBHandles(UsbGpioModuleConnectionHd);
								CloseUSBHandles(UsbMultimeterModuleConnectionHd);
								Sleep(3000);
								if (PCBTEST_Initialization() == SUCCESS)
									break;
								WriteLogFile("PCB retry init %d", init);
							}
							if (init == 2)
								break;

							WriteLogFile("PCB retry %s", pcb_test_routines_names[i]);
							SetStatus("");
							pcb_tests_results[i] = pcb_tests[i]();
							if (pcb_tests_results[i] == SUCCESS)
								break;
						}
					}
				}
#else
				//if (i == 0) pcb_tests_results[i] = 1; /*< NUM_OF_PCB_TEST_ROUTINES-2) pcb_tests_results[i] = SUCCESS;*/ else pcb_tests_results[i] = SUCCESS;
#endif
				WriteLogFile ("%s: result is %d", pcb_test_routines_names[i], pcb_tests_results[i]); 
				if (pcb_tests_results[i] == 1)
				{
					WriteMainLogFile("%s: SUCCESS", pcb_test_routines_names[i]);
				}
				else
				{
					WriteMainLogFile("%s: FAILURE", pcb_test_routines_names[i]);
				}
				
				label_text_id = pcbTestResultsLabelsIDs[i];
				//GetDlgItem(pcbTestResultsLabelsIDs[i])->SendMessage(WM_CTLCOLORSTATIC );

				if (pcb_tests_results[i] == SUCCESS)
				{	
					label_text_color = GREEN;
					GetDlgItem(pcbTestResultsLabelsIDs[i])->SetWindowText("PASS"); 
				}	
				else 
				{	
					label_text_color = RED;
					GetDlgItem(pcbTestResultsLabelsIDs[i])->SetWindowText("FAILURE"); 

					if (GetConfigurationValue(STOP_AT_FIRST_FAIL_STR))
					{
						abort = 1;
						//break;
					}
				}
				//GetDlgItem(pcbTestResultsLabelsIDs[i])->SendMessage(WM_CTLCOLORSTATIC );
				//InvalidateRect(NULL, false);
				
				

#ifdef display_time					
				Now = GetTickCount()/1000;
				TimeElasp = Now-Last; 
				TotalTimeElasp = Now-Begin;
				Last = Now;
#endif		
				//if (pcb_tests_results[i] != 1)
				//{
#ifndef display_time
					strMessage.Format("%s", TestStatusString);
#else
					strMessage.Format("%d, %s", TotalTimeElasp, TestStatusString);
#endif
				//}
				//else
				//{
#ifndef display_time
					//strMessage.Format("Passed: %s", TestStatusString);
#else
					//strMessage.Format("%d, %d, %s", TimeElasp, TotalTimeElasp, TestStatusString);
#endif
				//}
				GetDlgItem(pcbTestResultsDetailsLabelsIDs[i])->SetWindowText(strMessage); 

				if(i == 0)
				{

					if(updateSerialNumberTextBox == 1)
						GetDlgItem(IDC_STATIC_SERIAL_NUMBER)->SetWindowText(serialNumberArray);
					else if(updateSerialNumberTextBox == 2)
					{
									//label_text_color = RED;
									GetDlgItem(pcbTestResultsLabelsIDs[i])->SetWindowText("FAILURE"); 
									//GetDlgItem(pcbTestResultsLabelsIDs[i])->SendMessage(WM_CTLCOLORSTATIC );
									GetDlgItem(pcbTestResultsDetailsLabelsIDs[i])->SetWindowText("write serial number failed.");
					}
					else if(updateSerialNumberTextBox == 3)
					{
						GetDlgItem(IDC_STATIC_SERIAL_NUMBER)->SetWindowText(serialNumberArray);
					}
					updateSerialNumberTextBox = 0;

					if(pcb_tests_results[i] == SUCCESS) 
					{
						GetDlgItem(IDC_STATIC_CURR_BT_ADDR)->SetWindowText(currentBluetoothAddrDisplayStr);

						//Seavia 20150903
						GetDlgItem(IDC_STATIC_PRJ_INFO)->SetWindowText(projectInformationStr);

					}
					else
					{
						break;
					}
				}

			}
			else
			{	
			}
		}
		else
		{
			//strMessage.Format(_T("BER is: %.6f%% \n"), BER);
			
		}
	}

	for (i=/*0*/1; i<NUM_OF_PCB_TEST_ROUTINES/*-1*/; i++)
	{
		if (pcb_tests_results[i] != NOT_STARTED)  
		{	
			pcb_tests_overall_result = pcb_tests_results[i];
			if (pcb_tests_overall_result != SUCCESS) 
			{	
				break;
			}
		}
		else 
		{	
			pcb_tests_not_complete = 1;
		}
	}

	if (pcb_tests_overall_result == SUCCESS) 
	{
		pcb_tests_overall_result = pcb_tests_results[0];
	}

	if (pcb_tests_overall_result == SUCCESS) 
	{	if (pcb_tests_not_complete)
		{
			pcb_tests_overall_result = NOT_COMPLETE;
		}
	}
	
	if(!ChangeGPIOState(VBAT_IO_PIN_NUM, LOW))
	{
		//AfxMessageBox("Unable to turn VBAT IO PIN  output off !!!", MB_OK);
		WriteLogFile("Unable to turn VBAT IO PIN  output off");
	}

	if(!ChangeGPIOState(VCHARGE_IO_PIN_NUM, LOW))
	{
		//AfxMessageBox("Unable to turn VCHARGE IO PIN output off !!!", MB_OK);
		WriteLogFile("Unable to turn VCHARGE IO PIN  output off");
	}
		
	switch(pcb_tests_overall_result)
	{
		case SUCCESS:
	
			label_text_color = GREEN;
			label_text_id = IDC_STATIC_PCB_TEST_RESULT;
			resultstring = "PCB TEST RESULT"+TEST_SW_VERSION+": PASS";
			GetDlgItem(IDC_STATIC_PCB_TEST_RESULT)->SetWindowText(resultstring); 
		break;

		case NOT_STARTED:

			label_text_color = BLACK;
			label_text_id = IDC_STATIC_PCB_TEST_RESULT;
			resultstring = "PCB TEST RESULT"+TEST_SW_VERSION+": Not started";
			GetDlgItem(IDC_STATIC_PCB_TEST_RESULT)->SetWindowText(resultstring); 
		break;
		
		case WRITE_PAIRING_INFO_FAILED:

			label_text_color = RED;
			label_text_id = IDC_STATIC_PCB_TEST_RESULT;
			resultstring = "PCB TEST RESULT"+TEST_SW_VERSION+": FAILURE Write PairingInfo Failed";
			GetDlgItem(IDC_STATIC_PCB_TEST_RESULT)->SetWindowText(resultstring); 
		break;

		case NO_AVAIL_SERIAL_NUMBER:

			label_text_color = RED;
			label_text_id = IDC_STATIC_PCB_TEST_RESULT;
			resultstring = "PCB TEST RESULT"+TEST_SW_VERSION+": FAILURE No Avail SN";
			GetDlgItem(IDC_STATIC_PCB_TEST_RESULT)->SetWindowText(resultstring); 
		break;

		case WRITE_SERIAL_NUMBER_FAILED:

			label_text_color = RED;
			label_text_id = IDC_STATIC_PCB_TEST_RESULT;
			resultstring = "PCB TEST RESULT"+TEST_SW_VERSION+": FAILURE Write SN Failed";
			GetDlgItem(IDC_STATIC_PCB_TEST_RESULT)->SetWindowText(resultstring); 
		break;

		case NOT_COMPLETE:

			label_text_color = RED;
			label_text_id = IDC_STATIC_PCB_TEST_RESULT;
			resultstring = "PCB TEST RESULT"+TEST_SW_VERSION+": Not Complete";
			GetDlgItem(IDC_STATIC_PCB_TEST_RESULT)->SetWindowText(resultstring); 
		break;

		default:
		
			label_text_color = RED;
			label_text_id = IDC_STATIC_PCB_TEST_RESULT;
			resultstring = "PCB TEST RESULT"+TEST_SW_VERSION+": FAILURE";
			GetDlgItem(IDC_STATIC_PCB_TEST_RESULT)->SetWindowText(resultstring); 
	}
#ifdef update_color
	GetDlgItem(IDC_STATIC_PCB_TEST_RESULT)->SendMessage(WM_CTLCOLORSTATIC );
#endif

	//logstring.Format("%s%s", serialNumberArray, resultstring);
	if (strlen(serialNumberArray) != 0)
	{	//WriteMainLogFile("%s %s", serialNumberArray, resultstring);
	}
	else
	{	//WriteMainLogFile("SerialNumber#(Unknown) %s", resultstring);
	}
	WriteLogFile ("PCB OVERALL RESULT is %d\n", pcb_tests_overall_result);
	WriteMainLogFile("PCB OVERALL RESULT is %d\n", pcb_tests_overall_result);

	CsrDevicesClose();
	CloseUSBHandles(UsbGpioModuleConnectionHd);
	CloseUSBHandles(UsbMultimeterModuleConnectionHd);
	if (IndividualLogFileHd.is_open())
		IndividualLogFileHd.close();
	GetDlgItem(IDC_BUTTON_StartPcbTest)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_RestorePcbTest)->EnableWindow(TRUE);
}

void CWatchBT_ProductionTestDlg::OnBnClickedStartProductTest()
{
	// TODO: Add your control notification handler code here
	int product_tests_overall_result = SUCCESS, product_tests_not_complete = 0;
	int oled_test_result = 0;
	unsigned int Begin = 0, Last = 0;
	unsigned int Now =0;
	CString	strMessage, emptyMessage;
#if 1 /* Seavia 20150810 , fix unreferenced local variable warning*/
	int i=0, success_flag=0;
#else
	int i=0, success_flag=0, licenseKeysIndex, licenseKeysSegmentIndex;
	uint16 psKey[2], pskey_length_read;
	int iAnswer;
	uint16 newLicenseKeys[LICENSE_KEY_LENGTH];
#endif
	CString availBluetoothAddrStr, resultstring;
	int abort = 0;

	fwVer = "";
	fwVersion[0] = 0;
	fwVersion[1] = 0;
	UpdateData(FALSE);

	GetDlgItem(IDC_BUTTON_StartProductTest)->EnableWindow(FALSE);

	Begin = Last = GetTickCount()/1000;
    memset(TestStatusString, 0, sizeof(TestStatusString));
	MultimeterInitFlag = 0;
	for (i=0; i<NUM_OF_PRODUCT_TEST_ROUTINES; i++)
	{
		GetDlgItem(productTestResultsLabelsIDs[i])->SetWindowText("Not started"); 
		GetDlgItem(productTestResultsDetailsLabelsIDs[i])->SetWindowText(" "); 
		label_text_color = GREY;
		label_text_id = productTestResultsLabelsIDs[i];
#ifdef update_color
		GetDlgItem(productTestResultsLabelsIDs[i])->SendMessage(WM_CTLCOLORSTATIC);
#endif
		product_tests_results[i] = NOT_STARTED;
	}
	//label_text_color = GREY;
	//label_text_id = IDC_STATIC_PRODUCT_TEST_RESULT;
	GetDlgItem(IDC_STATIC_PRODUCT_TEST_RESULT)->SetWindowText(" ");
	WriteLogFile("beginning");
	for (int i=0; i<NUM_OF_PRODUCT_TEST_ROUTINES-1; i++)
	{
		if (abort)
			break;
		//CheckDlgButton
		if (IsDlgButtonChecked(productTestCheckBoxesIDs[i]) || (i==0)) 
		{
			SetStatus("");
			if (product_tests[i] != NULL)
			{	

#if 1 /*Seavia 20150903 add project information */
				GetDlgItem(IDC_STATIC_PRJ_INFO)->SetWindowText(projectInformationStr);
#endif
				GetDlgItem(productTestResultsLabelsIDs[i])->SetWindowText("Progress"); 
				//InvalidateRect(NULL, false);
				product_tests_results[i] = product_tests[i]();
				if(i == NUM_OF_PRODUCT_TEST_ROUTINES - 2) {
					update_bool = false;
				}
				WriteLogFile ("%s: result is %d", product_test_routines_names[i], product_tests_results[i]);  
				if (product_tests_results[i] == 1)
				{	
					if(i == NUM_OF_PRODUCT_TEST_ROUTINES - 2) {
						update_bool = false;
					}
					WriteMainLogFile("%s: SUCCESS", product_test_routines_names[i]);
				}
				else
				{
					if(i == NUM_OF_PRODUCT_TEST_ROUTINES - 2) {
						update_bool = false;
					}
					WriteMainLogFile("%s: FAILURE", product_test_routines_names[i]);
				}
#ifdef display_time					
				Now = GetTickCount()/1000;
				TimeElasp = Now-Last; 
				TotalTimeElasp = Now-Begin;
				Last = Now;
#endif		

				label_text_id = productTestResultsLabelsIDs[i];
				//GetDlgItem(productTestResultsLabelsIDs[i])->SendMessage(WM_CTLCOLORSTATIC );

				if (product_tests_results[i] == SUCCESS)
				{	
					label_text_color = GREEN;
					GetDlgItem(productTestResultsLabelsIDs[i])->SetWindowText("PASS"); 
				}	
				else 
				{	
					label_text_color = RED;
					GetDlgItem(productTestResultsLabelsIDs[i])->SetWindowText("FAILURE"); 

					if (GetConfigurationValue(STOP_AT_FIRST_FAIL_STR))
					{
						//break;
						abort = 1;
					}
				}
				//InvalidateRect(NULL, false);
#ifdef update_color
				GetDlgItem(productTestResultsLabelsIDs[i])->SendMessage(WM_CTLCOLORSTATIC );
#endif

#ifndef display_time
				strMessage.Format("%s", TestStatusString);
#else
				strMessage.Format("%d, %s", TotalTimeElasp, TestStatusString);
#endif
				GetDlgItem(productTestResultsDetailsLabelsIDs[i])->SetWindowText(strMessage); 	
				if(i == 0)
				{
					if(updateSerialNumberTextBox == 1)
						GetDlgItem(IDC_STATIC_SERIAL_NUMBER)->SetWindowText(serialNumberArray);
					updateSerialNumberTextBox = 0;
					if (product_tests_results[i] != SUCCESS)
					{
						break;

					}

				}
			}
			else
			{	
			}
		}
		else
		{
		}
	}


	for (i=0; i<NUM_OF_PRODUCT_TEST_ROUTINES-1; i++)
	{	
		if (product_tests_results[i] == NOT_STARTED)  
		{		
			product_tests_not_complete = 1;
		}
		else if(product_tests_results[i] != SUCCESS) 
		{	
			product_tests_overall_result = 0;
			break;
		}
	}	 
	if (product_tests_overall_result == SUCCESS)
	{	if (product_tests_not_complete)
		{	
			product_tests_overall_result = NOT_COMPLETE;
		}
	}

//#ifndef EASY_FACTORY_ENABLE	
	if ( (GetConfigurationValue(EASY_FACTORY_ENABLE_STR) == 1) || 
		 (product_tests_overall_result == SUCCESS)
	   )
//#endif
	{
		if(IsDlgButtonChecked(productTestCheckBoxesIDs[NUM_OF_PRODUCT_TEST_ROUTINES-1]))
		{
#ifndef test_mode
			product_tests_results[NUM_OF_PRODUCT_TEST_ROUTINES-1] = product_tests[NUM_OF_PRODUCT_TEST_ROUTINES-1]();
#else
		//product_tests_results[NUM_OF_PRODUCT_TEST_ROUTINES-1] = 1;
#endif
			product_tests_overall_result = product_tests_results[NUM_OF_PRODUCT_TEST_ROUTINES-1];
			label_text_id = productTestResultsLabelsIDs[NUM_OF_PRODUCT_TEST_ROUTINES-1];
			//GetDlgItem(productTestResultsLabelsIDs[NUM_OF_PRODUCT_TEST_ROUTINES-1])->SendMessage(WM_CTLCOLORSTATIC );

			if (product_tests_results[NUM_OF_PRODUCT_TEST_ROUTINES-1] == SUCCESS)
			{	
				label_text_color = GREEN;
				GetDlgItem(productTestResultsLabelsIDs[NUM_OF_PRODUCT_TEST_ROUTINES-1])->SetWindowText("PASS"); 
			}	
			else 
			{	
				label_text_color = RED;
				GetDlgItem(productTestResultsLabelsIDs[NUM_OF_PRODUCT_TEST_ROUTINES-1])->SetWindowText("FAILURE"); 
			}
			GetDlgItem(IDC_STATIC_AVAIL_BT_ADDR)->SetWindowText(currentavailBluetoothAddrStr);
			GetDlgItem(IDC_STATIC_CURR_BT_ADDR)->SetWindowText(currentBluetoothAddrDisplayStr);
			//Seavia 20150903
			GetDlgItem(IDC_STATIC_PRJ_INFO)->SetWindowText(projectInformationStr);
			strMessage.Format("%s", TestStatusString);
			GetDlgItem(productTestResultsDetailsLabelsIDs[NUM_OF_PRODUCT_TEST_ROUTINES-1])->SetWindowText(strMessage); 
#ifdef update_color
			GetDlgItem(productTestResultsLabelsIDs[NUM_OF_PRODUCT_TEST_ROUTINES-1])->SendMessage(WM_CTLCOLORSTATIC );
#endif
			update_bool = false;
			WriteLogFile ("%s: result is %d", product_test_routines_names[NUM_OF_PRODUCT_TEST_ROUTINES-1], product_tests_results[NUM_OF_PRODUCT_TEST_ROUTINES-1]); 
			if (product_tests_results[NUM_OF_PRODUCT_TEST_ROUTINES-1] == 1)
			{
				WriteMainLogFile("%s: SUCCESS", product_test_routines_names[NUM_OF_PRODUCT_TEST_ROUTINES-1]);
			}
			else
			{
				WriteMainLogFile("%s: FAILURE", product_test_routines_names[NUM_OF_PRODUCT_TEST_ROUTINES-1]);
			}
		}
	}

	switch(product_tests_overall_result)
	{
		case SUCCESS:
			if (fwVersion[0])
			{
				fwVer.Format("FW=%X.%02X", fwVersion[0], fwVersion[1]);
				UpdateData(FALSE);
			}
			label_text_color = GREEN;
			label_text_id = IDC_STATIC_PRODUCT_TEST_RESULT;
			resultstring = "PRODUCT TEST RESULT "+TEST_SW_VERSION+" : PASS";
			GetDlgItem(IDC_STATIC_PRODUCT_TEST_RESULT)->SetWindowText(resultstring); 
		break;
			
		case NOT_STARTED:

			label_text_color = BLACK;
			label_text_id = IDC_STATIC_PRODUCT_TEST_RESULT;
			resultstring = "PRODUCT TEST RESULT "+TEST_SW_VERSION+": Not started";
			GetDlgItem(IDC_STATIC_PRODUCT_TEST_RESULT)->SetWindowText(resultstring); 
		break;

		case WRITE_BT_ADDR_OR_LICENSE_KEY_FAILED:

			label_text_color = RED;
			label_text_id = IDC_STATIC_PRODUCT_TEST_RESULT;
			resultstring = "PRODUCT TEST RESULT "+TEST_SW_VERSION+": FAILURE Write Bt Addr or License Failed";
			GetDlgItem(IDC_STATIC_PRODUCT_TEST_RESULT)->SetWindowText(resultstring); 
		break;

		case NO_AVAIL_BT_ADDR:
			label_text_color = RED;
			label_text_id = IDC_STATIC_PRODUCT_TEST_RESULT;
			resultstring = "PRODUCT TEST RESULT "+TEST_SW_VERSION+": FAILURE No Avail BT Addr";
			GetDlgItem(IDC_STATIC_PRODUCT_TEST_RESULT)->SetWindowText(resultstring); 
		break;

		case NOT_COMPLETE:
			label_text_color = RED;
			label_text_id = IDC_STATIC_PRODUCT_TEST_RESULT;
			resultstring = "PRODUCT TEST RESULT "+TEST_SW_VERSION+": Not Complete";
			GetDlgItem(IDC_STATIC_PRODUCT_TEST_RESULT)->SetWindowText(resultstring); 
		break;

		case NO_AVAIL_LICENSE_KEY:
			label_text_color = RED;
			label_text_id = IDC_STATIC_PRODUCT_TEST_RESULT;
			resultstring = "PRODUCT TEST RESULT "+TEST_SW_VERSION+": FAILURE No License Key";
			GetDlgItem(IDC_STATIC_PRODUCT_TEST_RESULT)->SetWindowText(resultstring); 
		break;

		default:
			label_text_color = RED;
			label_text_id = IDC_STATIC_PRODUCT_TEST_RESULT;
			resultstring = "PRODUCT TEST RESULT "+TEST_SW_VERSION+": FAILURE";
			GetDlgItem(IDC_STATIC_PRODUCT_TEST_RESULT)->SetWindowText(resultstring); 
	}
#ifdef update_color	
	GetDlgItem(IDC_STATIC_PRODUCT_TEST_RESULT)->SendMessage(WM_CTLCOLORSTATIC ); 
#endif
	if(!ChangeGPIOState(VCHARGE_IO_PIN_NUM, LOW))
	{
		WriteLogFile("Fail To SET VCHARGE_IO_PIN_NUM to LOW");

	}

	if(!ChangeGPIOState(USB_ENABLE_PIN_NUM, LOW))
	{
		//AfxMessageBox("Unable to turn USB_ENABLE output off !!!", MB_OK);
		WriteLogFile("Unable to turn USB_ENABLE output off");
	}
	if(!ChangeGPIOState(VBAT_IO_PIN_NUM, LOW))
	{
		//AfxMessageBox("Unable to turn USB_ENABLE output off !!!", MB_OK);
		WriteLogFile("Unable to turn VBAT_IO_PIN_NUM output off");
	}
	
	update_bool = false;
	WriteLogFile ("PRODUCT OVERALL RESULT is %d\n", product_tests_overall_result);
	WriteMainLogFile("PRODUCT OVERALL RESULT is %d\n", product_tests_overall_result);
	update_bool = true;
	CsrDevicesClose();
	CloseUSBHandles(UsbGpioModuleConnectionHd);
	CloseUSBHandles(UsbMultimeterModuleConnectionHd);
	if (IndividualLogFileHd.is_open())
		IndividualLogFileHd.close();
	GetDlgItem(IDC_BUTTON_StartProductTest)->EnableWindow(TRUE);
	//I don't have choice
	if (g_BT_address_runout){
		AfxMessageBox("Talk to Manager, No More Bluetooth address", MB_OK|MB_ICONQUESTION|MB_DEFBUTTON2);
		exit(1);
	}
}



void CWatchBT_ProductionTestDlg::OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

int CheckRestoreResult()
{

	char str1[7];
    string Str_expected="Success";
    FILE *sFile;
 //   CsrDevicesClose();
  //  system("02_Restore.bat"); // Script to unlock SPI; erase CSR flash and burn bin file
    sFile = fopen("restore_result.txt","r+");
    if(sFile)
    {
        fseek(sFile, -9 ,SEEK_END);
        fgets(str1, 8,sFile);
        if(ferror (sFile))
        {
			SetStatus("SPI-UNLOCK: FILE ReadError");
			return 0;
        }
        fclose(sFile);
        if(Str_expected.compare(str1)==0)
        {
			SetStatus("SPI-UNLOCK: SUCCESS");
			return 1;
            // set the text into restored or done here
        }
		else
		{
			SetStatus("SPI-UNLOCK:  FAIL");
			return 0;
		}
    }
    else
    {
		SetStatus("SPI-UNLOCK: FILE NOT FOUND");
		return 0;
    }
}

void CWatchBT_ProductionTestDlg::OnBnClickedRestore()
{
#if 1
	// disable start button function and enable restore, otherwise there would be overlapping of IO operations which would crash
	GetDlgItem(IDC_BUTTON_StartPcbTest)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_RestorePcbTest)->EnableWindow(FALSE);
	if(PeriphiralOpen(PERIPHIRAL_ID_GPIOBOARD, SPI_CONNECTION))
	{

		system("02_Restore.bat"); // Script to unlock SPI; erase CSR flash and burn bin file
		//ToDo...Make Sure restore has completed successfully ab


		Sleep(1000);
		if(!ChangeGPIOState(VCHARGE_IO_PIN_NUM, LOW))
		{
			AfxMessageBox("Unable to turn VCHARGE_IO_PIN_NUM output off", MB_OK);
			WriteLogFile("Unable to turn VCHARGE_IO_PIN_NUM output off");
		}

		if (CheckRestoreResult())
		{
			label_text_color = GREEN;
			label_text_id = IDC_STATIC_PCB_TEST_RESULT;
			GetDlgItem(IDC_STATIC_PCB_TEST_RESULT)->SetWindowText(TestStatusString); 
			GetDlgItem(IDC_BUTTON_StartPcbTest)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_RestorePcbTest)->EnableWindow(TRUE);
			return ;
		}
	}
	else
	{
		SetStatus("SPI-UNLOCK: FAIL TO OPEN GPIO");
	}
	label_text_color = RED;
	label_text_id = IDC_STATIC_PCB_TEST_RESULT;
	GetDlgItem(IDC_STATIC_PCB_TEST_RESULT)->SetWindowText(TestStatusString); 
	GetDlgItem(IDC_BUTTON_StartPcbTest)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_RestorePcbTest)->EnableWindow(TRUE);
#endif
}

TESTENGINE_API(int32) spSetPio32(uint32 handle, uint32 mask, uint32 direction, 
                                     uint32 value, uint32* errLines)
{
	int32	iSuccess;
	int retries = 0;

	for (; retries < 5; retries++)
	{
		iSuccess = bccmdSetPio32(handle, mask, direction, value, errLines);
		if (iSuccess == TE_OK)
			return TE_OK;
	}
	return iSuccess;
}

TESTENGINE_API(int32) spGetPio32(uint32 handle, uint32* direction, uint32* value)
{
	int32	iSuccess;
	int retries = 0;

	for (; retries < 5; retries++)
	{
		iSuccess = bccmdGetPio32(handle, direction, value);
		if (iSuccess == TE_OK)
			return TE_OK;
	}
	return iSuccess;
}
///////////////////////////////////////////////////////
TESTENGINE_API(int32) spMapPio32(uint32 handle, uint32 mask, uint32 pios, 
                                     uint32* errLines)
{
	int32	iSuccess;
	int retries = 0;

	for (; retries < 5; retries++)
	{
		iSuccess = bccmdMapPio32(handle, mask, pios, errLines);
		if (iSuccess == TE_OK)
			return TE_OK;
	}
	return iSuccess;
}

TESTENGINE_API(int32) spSetPio(uint32 handle, uint16 mask, uint16 port)
{
	int32	iSuccess;
	int retries = 0;

	for (; retries < 5; retries++)
	{
		iSuccess = bccmdSetPio(handle, mask, port);
		if (iSuccess == TE_OK)
			return TE_OK;
	}
	return iSuccess;
}

TESTENGINE_API(int32) spLedEnable(uint32 handle, uint16 led, uint16 state)
{
	int32	iSuccess;
	int retries = 0;

	for (; retries < 5; retries++)
	{
		iSuccess = bccmdLedEnable(handle, led, state);
		if (iSuccess == TE_OK)
			return TE_OK;
	}
	return iSuccess;
}

TESTENGINE_API(int32) spradiotestTxstart(uint32 handle, uint16 frequency, uint16 intPA, 
                                        uint16 extPA, int16 modulation)
{
	int32	iSuccess;
	int retries = 0;

	for (; retries < 5; retries++)
	{
		iSuccess = radiotestTxstart(handle, frequency, intPA, extPA, modulation);
		if (iSuccess == TE_OK)
			return TE_OK;
	}
	return iSuccess;
}

TESTENGINE_API(int32) spradiotestCfgPkt(uint32 handle, uint16 type, uint16 size)
{
	int32	iSuccess;
	int retries = 0;

	for (; retries < 5; retries++)
	{
		iSuccess = radiotestCfgPkt(handle, type, size);
		if (iSuccess == TE_OK)
			return TE_OK;
	}
	return iSuccess;
}

TESTENGINE_API(int32) spradiotestCfgFreq(uint32 handle, uint16 txrxinterval, 
                                        uint16 loopback, uint16 report)
{
	int32	iSuccess;
	int retries = 0;

	for (; retries < 5; retries++)
	{
		iSuccess = radiotestCfgFreq(handle, txrxinterval, loopback, report);
		if (iSuccess == TE_OK)
			return TE_OK;
	}
	return iSuccess;
}

TESTENGINE_API(int32) spradiotestRxdata1(uint32 handle, uint16 frequency, uint8 hiside, 
                                        uint16 rx_attenuation)
{
	int32	iSuccess;
	int retries = 0;

	for (; retries < 5; retries++)
	{
		iSuccess = radiotestRxdata1(handle, frequency, hiside, rx_attenuation);
		if (iSuccess == TE_OK)
			return TE_OK;
	}
	return iSuccess;
}

TESTENGINE_API(int32) spradiotestPcmToneIf(uint32 handle, uint16 freq, uint16 ampl, 
                                            uint16 dc, uint16 pcm_if)
{
	int32	iSuccess;
	int retries = 0;

	for (; retries < 5; retries++)
	{
		iSuccess = radiotestPcmToneIf(handle, freq, ampl, dc, pcm_if);
		if (iSuccess == TE_OK)
			return TE_OK;
	}
	return iSuccess;
}

TESTENGINE_API(int32) spbccmdI2CTransfer(uint32 handle, uint16 slaveAddr, uint16 txOctets,
                                        uint16 rxOctets, uint8 restart, uint8 data[],
                                        uint16 *octets)
{
	int32	iSuccess;
	int retries = 0;

	for (; retries < 5; retries++)
	{
		iSuccess = bccmdI2CTransfer(handle, slaveAddr, txOctets, rxOctets, restart, data, octets);
		if (iSuccess == TE_OK)
			return TE_OK;
	}
	return iSuccess;
}

TESTENGINE_API(int32) sppsWrite(uint32 handle, uint16 psKey, uint16 store, 
                                    uint16 length, const uint16 data[])
{
	int32	iSuccess;
	int n = 0;
	uint16  psKeyData[100]; 
	uint16	length_read;
	

	n=0;
	do 
	{
		iSuccess = psWrite(handle, psKey, store, length, data);
	}
	while ((iSuccess != TE_OK) && ((n++)<5)); 
	
	if (iSuccess != TE_OK)
	{
		return iSuccess;
	}

	n=0;
	do 
	{
		iSuccess = psRead(handle, psKey, store, length, psKeyData, &length_read);
	}
	while ((iSuccess != TE_OK) && ((n++)<5)); 

	if (iSuccess != TE_OK)
	{
		return iSuccess;
	}

	for (n=0; n<length; n++)
	{
		if (data[n] != psKeyData[n])
		{	
			
			return 0;
		}
	}

	return 1; 
}

TESTENGINE_API(int32) sppsRead(uint32 handle, uint16 psKey, uint16 store, 
                                uint16 arrayLen, uint16 data[], uint16 * len)
{
	int32	iSuccess, data_consistent_flag=0;
	int n = 0, m = 0, i = 0;
	uint16  psKeyDataArray[3][100];

	memset(psKeyDataArray, 0, sizeof(psKeyDataArray));
	
	do 
	{	
		n=0;
		do 
		{
			iSuccess = psRead(handle, psKey, store, arrayLen, psKeyDataArray[n], len);
			if (iSuccess == TE_OK)
			{
				n++;
			}
		}
		while (n<3);

		for (i=0; (uint16)i < arrayLen/sizeof(uint16); i++)
		{	
			if ((psKeyDataArray[0][i] == psKeyDataArray[1][i]) && (psKeyDataArray[1][i] == psKeyDataArray[2][i]))
			{
			}
			else
			{
				break; 
			}
		}

		if ((uint16)i >= arrayLen/sizeof(uint16))
		{	
			data_consistent_flag = 1;
		}
	}
	while ((data_consistent_flag == 0) && ((m++)<8)); 

	if (data_consistent_flag == 1)
	{
		for (i=0; (uint16)i < arrayLen/sizeof(uint16); i++)
		{
			data[i] = psKeyDataArray[0][i];
		}
	}

	return data_consistent_flag;
}

TESTENGINE_API(int32) spradiotestPause(uint32 handle)
{
	int32	iSuccess;
	int retries = 0;

	for (; retries < 5; retries++)
	{
		iSuccess = radiotestPause(handle);
		if (iSuccess == TE_OK)
			return TE_OK;
	}
	return iSuccess;
}

int ReadModelSelectionFile()
{	
	int temp_model_selection; 
	ifstream modelSelectionFile (MODEL_SELECTION_FILE_NAME);
	if (modelSelectionFile.fail())
	{
		WriteLogFile("ReadModelSelectionFile: %s is not found.", MODEL_SELECTION_FILE_NAME); 
		return 0;
    }

	if (modelSelectionFile.is_open())
	{	
		modelSelectionFile >> temp_model_selection;
		if ((temp_model_selection >= 0) && ((uint16)temp_model_selection < NUM_OF_MODELS))
		{
			model_selection = temp_model_selection;
		}
	}
	else
	{
		WriteLogFile("ReadModelSelectionFile:%s not been able to be opened.", MODEL_SELECTION_FILE_NAME); 
		return 0;
	}
	modelSelectionFile.close();
	return 1;
}

int WriteModelSelectionFile()
{
	ofstream modelSelectionFile (MODEL_SELECTION_FILE_NAME);
	if (modelSelectionFile.fail())
	{
		WriteLogFile("%s can not opened.", MODEL_SELECTION_FILE_NAME); 
		return 0;
    }

	modelSelectionFile << model_selection << endl;
	modelSelectionFile.close();
#if 1 /* Seavia 20150810, fix warning, not all control paths return a value*/
	return 1;
#endif
} 

void CWatchBT_ProductionTestDlg::OnCbnSelchangeComboModelSelection()
{
	// TODO: Add your control notification handler code here
	int iAnswer;
	CString strMessage;
	
	CComboBox * pCombo = (CComboBox *) GetDlgItem(IDC_COMBO_MODEL_SELECTION);
		
	if (model_selection != pCombo->GetCurSel())
	{	iAnswer = pCombo->GetCurSel();
		strMessage.Format("Are you sure you want to change the configuration to model %s?", /*configFiles[pCombo->GetCurSel()].modelName*/modelsStrings[pCombo->GetCurSel()]);
		iAnswer = AfxMessageBox(strMessage, MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2);
		if(iAnswer == IDNO) 
		{	
			pCombo->SetCurSel(model_selection);
			if (testType == PCB_TEST)
			{
				GetDlgItem(IDC_BUTTON_StartPcbTest)->SetFocus(); 
			}
			else 
			{
				GetDlgItem(IDC_BUTTON_StartProductTest)->SetFocus();
			}
			return;
		}
#if 1 /*Seavia */
		char fileName[50];
		memset(fileName,0,sizeof(fileName));
		ResetConfigFile(modelsStrings[pCombo->GetCurSel()], fileName);
		if (!ReadConfigFile(fileName)/*configFiles[pCombo->GetCurSel()].fileName)*/)
#else
		if (!ReadConfigFile(configFileNames[testType][pCombo->GetCurSel()])/*configFiles[pCombo->GetCurSel()].fileName)*/)
#endif
		{	
			strMessage.Format("Unable to load the model %s's configuration file", /*configFiles[pCombo->GetCurSel()].modelName*/modelsStrings[pCombo->GetCurSel()]);
			AfxMessageBox(strMessage, MB_ICONINFORMATION|MB_OK); 
			pCombo->SetCurSel(model_selection);
			if (testType == PCB_TEST)
			{
				GetDlgItem(IDC_BUTTON_StartPcbTest)->SetFocus(); 
			}
			else 
			{
				GetDlgItem(IDC_BUTTON_StartProductTest)->SetFocus();
			}
			return;
		}
#if 1 /*Seavia 20150810, SN prefix is selected from dialog*/
		ResetSNPrefix(modelsStrings[pCombo->GetCurSel()]);
#endif
		model_selection = pCombo->GetCurSel();
		WriteModelSelectionFile();
		if (testType == PCB_TEST)
		{
			GetDlgItem(IDC_BUTTON_StartPcbTest)->SetFocus(); 
		}
		else 
		{
			GetDlgItem(IDC_BUTTON_StartProductTest)->SetFocus();
		}
	}
}

void CWatchBT_ProductionTestDlg::OnCbnSelchangeComboTestTypeSelection()
{
	// TODO: Add your control notification handler code here
	int iAnswer;
	CString strMessage;
	char  szPath[MAX_PATH];
#if 0 /* Seavia 20150810 , fix unreferenced local variable warning*/
	PSTR  lpFilename;
#endif
	HINSTANCE hinst;
	
	CComboBox * pCombo = (CComboBox *) GetDlgItem(IDC_COMBO_TEST_TYPE_SELECTION);
	
	if (testType != pCombo->GetCurSel())
	{	
		strMessage.Format("Are you sure you want to change the test type to %s?", test_typesStrings[pCombo->GetCurSel()]); 
		iAnswer = AfxMessageBox(strMessage, MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2);
		if(iAnswer == IDNO) 
		{	
			pCombo->SetCurSel(testType);
			if (testType == PCB_TEST)
			{
				GetDlgItem(IDC_BUTTON_StartPcbTest)->SetFocus(); 
			}
			else 
			{
				GetDlgItem(IDC_BUTTON_StartProductTest)->SetFocus();
			}
			return;
		}
		
		tempTestType = pCombo->GetCurSel(); 
		
		// Retrieve the full path for the current module.
		if ( GetModuleFileName( NULL, szPath, sizeof (szPath) ) != 0 )
		{	
			WriteTestTypeSelectionFile();
			hinst = ShellExecute(0, "open", szPath, NULL, NULL, SW_SHOWNORMAL);
			
			/*ASSERT(AfxGetMainWnd() != NULL);
			AfxGetMainWnd()->SendMessage(WM_CLOSE);*/
			exit (1); 
			return; 
		}
			
		//restore test type selection combo box 
		pCombo->SetCurSel(testType);
		tempTestType = testType;
		if (testType == PCB_TEST)
		{
			GetDlgItem(IDC_BUTTON_StartPcbTest)->SetFocus(); 
		}
		else 
		{
			GetDlgItem(IDC_BUTTON_StartProductTest)->SetFocus();
		}	
		strMessage.Format("Unable to change test type !");
		AfxMessageBox(strMessage, MB_ICONSTOP|MB_OK); 

		return; 
	}
}

int ReadTestTypeSelectionFile()
{	
	ifstream testTypeSelectionFile (TEST_TYPE_SELECTION_FILE_NAME);
	if (testTypeSelectionFile.fail())
	{
		WriteLogFile("ReadTestTypeSelectionFile: %s is not found.", TEST_TYPE_SELECTION_FILE_NAME); 
		return 0;
    }

	if (testTypeSelectionFile.is_open())
	{	
		testTypeSelectionFile >> hex >> testSelection;
		if(testSelection & 0x8000)
			testType = 1; 
		else testType = 0;
	}
	else
	{
		WriteLogFile("ReadTestTypeSelectionFile:%s not been able to be opened.", TEST_TYPE_SELECTION_FILE_NAME); 
		return 0;
	}
	testTypeSelectionFile.close();
	return 1;
}

int WriteTestTypeSelectionFile()
{
	ofstream testTypeSelectionFile (TEST_TYPE_SELECTION_FILE_NAME);
	unsigned long temp_value;

	if (testTypeSelectionFile.fail())
	{
		WriteLogFile("%s can not opened.", TEST_TYPE_SELECTION_FILE_NAME); 
		return 0;
    }

	if (tempTestType == PCB_TEST)
	{	
		temp_value = testSelection - 0x8000;
	}
	else
	{
		temp_value = testSelection + 0x8000;
	}

	testTypeSelectionFile << hex << temp_value << endl;
	testTypeSelectionFile.close();
#if 1 /* Seavia 20150810, fix warning, not all control paths return a value*/
	return 1;
#endif
} 
