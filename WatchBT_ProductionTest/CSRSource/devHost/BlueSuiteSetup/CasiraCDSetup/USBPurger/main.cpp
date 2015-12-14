// main.c
// Command line interface for the USB purger
//
// CVS ID
//     $Id: //depot/bc/bluesuite_2_4/devHost/BlueSuiteSetup/CasiraCDSetup/USBPurger/main.cpp#1 $
//
// CSS LOG
//
//     $Log: main.cpp,v $
//     Revision 1.3  2002/05/10 13:08:11  cl01
//     Mods for Microstar
//     Removerd MBCS / Unicode version diagnostic
//     Fixed potential bug in _tprintf call
//
//     Revision 1.2  2002/05/09 16:23:42  cl01
//     Added Microstar device
//
//     Revision 1.1  2002/04/17 14:44:31  cl01
//     Created
//
//
//

#include <stdio.h>
#include <tchar.h>
#include "USBPurge.h"
#include "common/globalversioninfo.h"


class CCmdPurgeReporter: public CPurgeReporter
{
public:
	void report(const int progress, const int max_progress, const TCHAR *text, const CPurgeReporter::State state)
	{
		int op = _tprintf(_T("%02d%% %s"), (progress * 100) / (max_progress + 1), text);
		// erase the rest of the line
		for (; op < 79; op++)
			_tprintf(_T(" "));
		if (state == CPurgeReporter::Busy)
			_tprintf(_T("\r"));
//			for (int i = 1; i < op; i++) _puttchar('\b');
		else
			_tprintf(_T("\n"));
	};
};

void die_usage();


int _tmain(int argc, _TCHAR **argv, _TCHAR **envp)
{
	_tprintf (_T("usbpurger, version %s\n"), VERSION_BLUESUITE_STR);

	// If the special build string is set then show the date of the build
	if (strcmp (VERSION_SPECIAL_BUILD, "") != 0)
	{
		_tprintf (_T("%s\n"), VERSION_SPECIAL_BUILD);
	}
	_tprintf (_T("%s\n"), VERSION_COPYRIGHT_START_YEAR("2002"));

	CCmdPurgeReporter reporter;

    if ((argc != 1) &&  (argc != 3))
	{
		die_usage();
	}
	else
	if (argc == 3)
	{
        DeviceInfo dev;
		TCHAR * format = _T("USB\\VID_%04x&PID_%04x");
		TCHAR * buf = (TCHAR * )malloc((_tcsclen(format) + 1) * sizeof(TCHAR));  // "%04x" is four characters long, so the resultant string is the same length as the format string.
		_stprintf(buf, format, _tcstoul(argv[1], NULL, 16), _tcstoul(argv[2], NULL, 16));
		_tprintf("Purging on %s", buf);
        dev.dev = buf;
        dev.drivername = NULL;
		CUSBPurge purger(1, &dev, &reporter);
		purger.do_purge();
		free(buf);
	}
	else
		{
		static const DeviceInfo dev_ids[] = {
            {_T("USB\\VID_044E&PID_3001"), NULL}, //  COMPAQ ALPS based VID&PID
            {_T("USB\\VID_044E&PID_3007"), NULL}, //  ALPS Alternative VID&PID
			{_T("USB\\VID_049F&PID_0027"), NULL}, //  COMPAQ CSR based VID&PID
			{_T("USB\\VID_04BF&PID_030A"), NULL}, //  IBM CSR based VID&PID
			{_T("USB\\VID_04BF&PID_0309"), NULL}, //  TDK CSR based VID&PID
			{_T("USB\\VID_04BF&PID_0310"), NULL}, //  IBM BDC CSR based VID&PID
			{_T("USB\\VID_0A12&PID_0001"), NULL}, //  CSR VID&PID
			{_T("USB\\VID_03EE&PID_641F"), NULL}, //  Mitsumi Dongle VID&PID
			{_T("USB\\VID_0DB0&PID_1967"), NULL}, //  MicroStar Bluetooth card
			{_T("USB\\VID_0A12&PID_FFFF"), NULL}, //  CSR DFU VID&PID
			{_T("USB\\VID_04BF&PID_030B"), NULL}, //  TDK CSR VID&PID DFU
			{_T("USB\\VID_04BF&PID_030C"), NULL}, //  IBM CSR VID&PID DFU
			{_T("USB\\VID_04BF&PID_0311"), NULL}, //  IBM BDC VID&PID DFU
			{_T("USB\\VID_049F&PID_0036"), NULL}, //  COMPAQ CSR VID&PID DFU
			{_T("USB\\VID_044E&PID_FFFF"), NULL}, //  ALPS DFU VID&PID
			{_T("USB\\VID_03EE&PID_FFFF"), NULL}, //  Mitsumi Dongle DFU VID&PID
			{_T("USB\\VID_0DB0&PID_FFFF"), NULL}, //  MicroStar Bluetooth card DFU ?
			{_T("USB\\VID_0C24&PID_0001"), NULL}, //  Taiyo Yuden
			{_T("USB\\VID_0C24&PID_FFFF"), NULL}, //  Taiyo Yuden DFU
			{_T("USB\\VID_05ac&PID_8203"), NULL}, 
			{_T("USB\\VID_05ac&PID_ffff"), NULL}, 
			{_T("USB\\VID_1668&PID_2441"), NULL}, //  IBM
			{_T("USB\\VID_1668&PID_3441"), NULL}, //  IBM DFU
			{_T("USB\\VID_049F&PID_0086"), NULL}, //  BLUETHUMB
			{_T("USB\\VID_049F&PID_0036"), NULL}, //  BLUETHUMB DFU
			{_T("USB\\VID_413c&PID_8000"), NULL}, //  DELL
			{_T("USB\\VID_413c&PID_8103"), NULL}, //  DELL U2
			{_T("USB\\VID_413c&PID_8010"), NULL}, //  DELL DFU
			{_T("USB\\VID_413c&PID_8106"), NULL}, //  DELL U2
			{_T("USB\\VID_413c&PID_8120"), NULL}, //  DELL EASTFOLD
			{_T("USB\\VID_413c&PID_8122"), NULL}, //  DELL EASTFOLD DFU
			{_T("USB\\VID_10ab&PID_1002"), NULL}, //  MOTION BC02
			{_T("USB\\VID_10ab&PID_1003"), NULL}, //  MOTION BC02 DFU
			{_T("USB\\VID_10ab&PID_1005"), NULL}, //  MOTION BC04
			{_T("USB\\VID_10ab&PID_1006"), NULL}, //  MOTION BC04 DFU
			{_T("USB\\VID_054c&PID_0267"), NULL}, 
			{_T("USB\\VID_054c&PID_0268"), NULL}, 

            {_T("USB\\VID_0A12&PID_0042"), NULL}, // CSR USB<>SPI (babel)
            {_T("USB\\VID_0A12&PID_1004"), "CSR Babel"}  // CSR USB<>SPI (wrong babel)
		};
		CUSBPurge purger(sizeof(dev_ids) / sizeof(dev_ids[0]) , dev_ids, &reporter);
		purger.do_purge();
	}
	return true;
}

void die_usage()
{
   printf("USB Purger\n");
   printf("Removes references to device drivers from the registry and .inf files, \n");
   printf("which has the effect of making an attached device unknown at the time of next entry.\n");
   printf("This doesn't actually delete the driver - only the OS's reference to it.\n");
   printf("\n");
   printf("Can specify a known vid+pid on the command line, vid then pid:\n");
   printf("  USBPurger 0a12 0001\n");
   exit(-1);
}

