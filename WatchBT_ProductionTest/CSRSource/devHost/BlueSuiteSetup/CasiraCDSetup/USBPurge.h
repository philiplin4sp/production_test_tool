// USBPurge.h
// Will remove specified vid+pid from registry and driver database.
// Copyright (c) 2002 CSR Ltd.  All rights reserved.

// $Id: //depot/bc/bluesuite_2_4/devHost/BlueSuiteSetup/CasiraCDSetup/USBPurge.h#1 $

// $Log: USBPurge.h,v $
// Revision 1.1  2002/03/28 17:01:50  cl01
// refactored Purge stuff into a non-windows class
//


#ifndef USB_PURGE_INCLUDED
#define USB_PURGE_INCLUDED


#include <tchar.h>
#include <windows.h>


#define MAX_PROGRESS 600

#define DISPLAY_SLEEP 1000


struct DeviceInfo
{
    const TCHAR *dev;           // The device name to search for.
    const TCHAR *drivername;    // Ignored if NULL, otherwise only drivers
                                // matching this name will be purged.
};


class CPurgeReporter
{
public:
	enum State {Idle, Busy, Tasked, Completed, Failed};
	virtual void report(const int progress, const int max_progress, const TCHAR *text, const State state) = 0;
};

class CUSBPurge
{
public:
	CUSBPurge(int devc, const DeviceInfo *devlist, CPurgeReporter *reporter); 
	~CUSBPurge();
	void Start(void);  // Starts a thread which in turn calls do_purge
	void Terminate(void); // Kills the thread
	UINT do_purge();  // Run the purge without threading
private:
	virtual void report(const int progress, const TCHAR *s, const CPurgeReporter::State state);
	static unsigned long __stdcall static_do_purge(LPVOID f);
	UINT purge_dir(TCHAR *folderBuf, TCHAR *ext);
	void do_error();
	bool match_string_in_file(const TCHAR * filename, const TCHAR * string);
	int inc_progress(int limit);
	void rename_unique(const TCHAR *filename);
	bool purge_reg(const DeviceInfo *hardware_id);
	bool purge_reg_2k(const DeviceInfo *hardware_id);
	bool purge_reg_9x(const DeviceInfo *hardware_id);
	bool thread_can_run_;
	int progress_;
	int devc_;
    const DeviceInfo *devlist;
	HANDLE heap_;
	HANDLE thread_handle_;
	CPurgeReporter *reporter_;
};




#endif // USB_PURGE_INCLUDED