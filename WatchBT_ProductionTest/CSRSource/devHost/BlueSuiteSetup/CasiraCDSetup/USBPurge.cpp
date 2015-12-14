// USBPurge.cpp

// Copyright (c) 2002 CSR Ltd.  All rights reserved.

// $Id: //depot/bc/bluesuite_2_4/devHost/BlueSuiteSetup/CasiraCDSetup/USBPurge.cpp#1 $

// $Log: USBPurge.cpp,v $
// Revision 1.1  2002/03/28 17:01:50  cl01
// refactored Purge stuff into a non-windows class
//

#include "USBPurge.h"
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <shfolder.h>
#include <shlwapi.h>
#include <assert.h>
#include <setupapi.h> // for SetupDiXxx functions.


CUSBPurge::CUSBPurge(int devc, const DeviceInfo *devlist, CPurgeReporter *reporter) :
		devc_(devc),
		devlist(devlist),
		reporter_(reporter)
{
	assert(reporter_);
	heap_ = HeapCreate(0, 0, 0);
	assert(heap_ != INVALID_HANDLE_VALUE);
	thread_handle_ = INVALID_HANDLE_VALUE;
	progress_ = 0;
	thread_can_run_ = true;
}

CUSBPurge::~CUSBPurge()
{
	thread_can_run_= false;
	if (heap_)
	{
		HeapDestroy(heap_);
		heap_ = NULL;
	}
}


void CUSBPurge::Start(void)
{
	// Kick off the purge thread.
	DWORD thread_id;
	thread_handle_= CreateThread(NULL, 0, static_do_purge, (LPVOID)this, 0, &thread_id);
}

void CUSBPurge::Terminate(void)
{
	thread_can_run_= false;
	if (thread_handle_ != INVALID_HANDLE_VALUE)
	{
		WaitForSingleObject(thread_handle_, INFINITE);
		CloseHandle(thread_handle_);
		thread_handle_ = INVALID_HANDLE_VALUE;
	}
	if (heap_)
	{
		HeapDestroy(heap_);
		heap_ = NULL;
	}
}

void CUSBPurge::report(int progress, const TCHAR *s, CPurgeReporter::State state)
{	
	assert(reporter_);
	reporter_->report(progress, MAX_PROGRESS, s, state);
}


unsigned long __stdcall CUSBPurge::static_do_purge(LPVOID f)
{
	CUSBPurge *p = (CUSBPurge *)f;
	assert(p);
	return p->do_purge();
}



UINT CUSBPurge::purge_dir(TCHAR *folderBuf, TCHAR *ext)
{
	TCHAR filespecBuf[MAX_PATH + 1];
	TCHAR filenameBuf[MAX_PATH + 1];
	TCHAR tmpbuf[256];

	_tcsncpy(filespecBuf, folderBuf, MAX_PATH);
	PathAppend(filespecBuf, _T("*.inf"));
	// Search out all .inf files 
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFile(filespecBuf, &FindFileData);
	while ((hFind != INVALID_HANDLE_VALUE) && (GetLastError() != ERROR_NO_MORE_FILES) && thread_can_run_)
	{	
		// open the file
		_tcsncpy(filenameBuf, folderBuf, MAX_PATH);
		PathAppend(filenameBuf, FindFileData.cFileName);		
		inc_progress(MAX_PROGRESS - 1);
		_stprintf(tmpbuf, _T("Scanning file %s"), FindFileData.cFileName);
		report(progress_, tmpbuf, CPurgeReporter::Busy);
		bool found = false;
		for (int i = 0; i < devc_; i++)
		{
			if (match_string_in_file(filenameBuf, devlist[i].dev))
			{
                if (devlist[i].drivername == NULL ||
                    match_string_in_file(filenameBuf, devlist[i].drivername))
                {
				    found = true;
				    break;
                }
			}
		}
		if (found)
		{
			int fileappend = 0;
			_stprintf(tmpbuf, _T("Purging file %s"), FindFileData.cFileName);
			report(progress_, tmpbuf, CPurgeReporter::Tasked);
			rename_unique(filenameBuf);
			PathRemoveExtension(filenameBuf);
			PathAddExtension(filenameBuf, _T(".pnf"));
			rename_unique(filenameBuf);
			Sleep(DISPLAY_SLEEP);
		}
		//next file
		FindNextFile(hFind, &FindFileData);
	}
	switch (GetLastError())
	{
	case NO_ERROR:
	case ERROR_NO_MORE_FILES:
	case ERROR_PATH_NOT_FOUND:
		break;
	default:
		do_error();
		return false;
	}
	return true;
}


UINT CUSBPurge::do_purge()
{

	TCHAR tmpbuf[256];
	TCHAR folderBuf[MAX_PATH + 1];
	_stprintf(tmpbuf, _T("Scanning .inf files for CSR chip id"));

	// Go to the windows/inf directory
	SHGetFolderPath(NULL, CSIDL_WINDOWS, NULL, 0, folderBuf);
	PathAppend(folderBuf, _T("inf"));
	if (!purge_dir(folderBuf, _T("*.inf")))
		return false;

	SHGetFolderPath(NULL, CSIDL_WINDOWS, NULL, 0, folderBuf);
	PathAppend(folderBuf, _T("inf\\other"));
	if (!purge_dir(folderBuf, _T("*.inf")))
		return false;

	report(progress_, _T("purging registry"), CPurgeReporter::Busy);
	for (int i = 0; i < devc_; i++)
	{
		purge_reg(&devlist[i]);
	}
	report(MAX_PROGRESS, _T("Finished purging."), CPurgeReporter::Completed);
	return true;
}

void CUSBPurge::do_error(void)
{
	TCHAR tmpbuf[4096];
	LPVOID lpMsgBuf;
    DWORD Err = GetLastError();
	if (Err == 0) // No error
		return;

    if (FormatMessage( 
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, 
        Err,  
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,  
        0,  
        NULL ))
        _stprintf(tmpbuf, _T("Error encountered: %s\n"), (TCHAR *)lpMsgBuf);
    else 
        _stprintf(tmpbuf, _T("Error encountered: (0x%08x)\n"), Err);
    
    if (lpMsgBuf) 
		LocalFree(lpMsgBuf); // Free system buffer 
    
    SetLastError(Err);
	MessageBox(NULL, tmpbuf, _T("Error"), MB_OK | MB_ICONINFORMATION );
	report(MAX_PROGRESS, tmpbuf, CPurgeReporter::Failed);
}


// This reads the whole file into memory, then scans memory for the string.
bool CUSBPurge::match_string_in_file(const TCHAR * filename, const TCHAR * string)
{
	bool found = false;
	HANDLE file = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (file == INVALID_HANDLE_VALUE)
	{
		return found;
	}
	// read the whole file into memory.
	DWORD filesize = GetFileSize(file, NULL);
	DWORD numRead;
	DWORD stringlen = _tcsclen(string);
	if (filesize == INVALID_FILE_SIZE)
		return found;
	void *data = HeapAlloc(heap_, 0, filesize);
	if (ReadFile(file, data, filesize, &numRead, NULL) == 0)
		return found;

	if (filesize < stringlen)
		return found;
	for (DWORD i = 0; i <= filesize - stringlen; i++)
	{
		if (_tcsncicmp(((TCHAR *)data) + i, string, stringlen) == 0)
			found = true;
	}

	HeapFree(heap_, 0, data);
	data = NULL;
	CloseHandle(file);
	return found;
}

int CUSBPurge::inc_progress(int limit)
{
	if (progress_ < limit)
		progress_ ++;
	return progress_;
}



void CUSBPurge::rename_unique(const TCHAR *filename)
{
	int fileIndex = 0;
	TCHAR newFilename[MAX_PATH];
	TCHAR newExtension[10];
//	TCHAR message[2 * MAX_PATH + 100];
	do {
		fileIndex++;
		_tcscpy(newFilename, filename);
		_stprintf(newExtension, _T(".%04x"), fileIndex);
		_tcscat(newFilename, newExtension);
//		_stprintf(message, _T("Renaming %s to %s"), filename, newFilename);
//		MessageBox(message, _T("Renaming file"), MB_OK);
	} while ((GetFileAttributes(filename) != -1) && (MoveFile(filename, newFilename) == 0));
//	MessageBox(_T("Renamed OK"), _T("Renamed OK"), MB_OK);
}



bool CUSBPurge::purge_reg(const DeviceInfo *hardware_id)
{
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx (&osvi);
	if (   (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) 
		|| (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) )
	{
		if (osvi.dwMajorVersion >= 5)
			return purge_reg_2k(hardware_id);
		else if (osvi.dwMajorVersion == 4)
			return purge_reg_9x(hardware_id);  // removes the USB/
	}
	MessageBox(NULL, _T("Unable to purge registry: unrecognosed operating system."), _T("Error purging registry"), MB_ICONEXCLAMATION | MB_OK);
	return false;
}


bool CUSBPurge::purge_reg_9x(const DeviceInfo *hardware_id)
{
	DWORD i, retCode;
	TCHAR  subkey[MAX_PATH]; 
	FILETIME ftLastWriteTime;
	HKEY hKey;
	DWORD errval;
	
	SetLastError(NO_ERROR);
	// Open the USB Enum key.
	if ((errval = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Enum\\USB"),0, KEY_ALL_ACCESS, &hKey)) != ERROR_SUCCESS)
	{
		SetLastError(errval);
        do_error();
		return false;
	}
    for (i = 0, retCode = ERROR_SUCCESS; retCode == ERROR_SUCCESS; i++) 
	{
		unsigned long subkey_size = MAX_PATH;
		retCode = RegEnumKeyEx(hKey, i, subkey, &subkey_size, NULL, NULL, NULL, &ftLastWriteTime);
		//TCHAR buf[1000];
		//_stprintf(buf, "comparing \"%s\" with \"%s\".", subkey, &hardware_id->dev[4]);
		//MessageBox(buf, "Comparison:", MB_OK);
		if (!_tcsicmp(&hardware_id->dev[4],subkey))
		{
			report(progress_, _T("Found instance in registry"), CPurgeReporter::Tasked);
			Sleep(DISPLAY_SLEEP);
			if ((errval = SHDeleteKey(hKey, subkey)) != ERROR_SUCCESS)
			{
				SetLastError(errval);
				do_error();
				goto cleanup_Registry;
			}
			// Start from beginning again
			i = -1;
		}
    }



cleanup_Registry:
	RegCloseKey(hKey);
    errval = GetLastError();
    return (errval == NO_ERROR); 
}


bool CUSBPurge::purge_reg_2k(const DeviceInfo *hardware_id)
// taken from remove.c, part of the win2k ddk
{
    HDEVINFO DeviceInfoSet;
    SP_DEVINFO_DATA DeviceInfoData;
    DWORD i,err;
    // Create a Device Information Set with all present devices.
    DeviceInfoSet = SetupDiGetClassDevs(NULL, // All Classes
        0,
        0, 
        DIGCF_ALLCLASSES ); // All devices on system
    if (DeviceInfoSet == INVALID_HANDLE_VALUE)
    {
        do_error();        
        return false;
    }
    
    //  Enumerate through all Devices.
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    for (i=0; thread_can_run_ && SetupDiEnumDeviceInfo(DeviceInfoSet,i,&DeviceInfoData); i++)
    {
        DWORD DataT;
        LPTSTR p,buffer = NULL;
        DWORD buffersize = 0;
        
        // We won't know the size of the HardwareID buffer until we call
        // this function. So call it with a null to begin with, and then 
        // use the required buffer size to Alloc the nessicary space.
        // Keep calling we have success or an unknown failure.
        while (!SetupDiGetDeviceRegistryProperty(
            DeviceInfoSet,
            &DeviceInfoData,
            SPDRP_HARDWAREID,
            &DataT,
            (PBYTE)buffer,
            buffersize,
            &buffersize))
        {
            if (GetLastError() == ERROR_INVALID_DATA)
            {
                // May be a Legacy Device with no HardwareID. Continue.
                break;
            }
            else if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                // We need to change the buffer size.
                if (buffer) 
                    LocalFree(buffer);
                buffer = (char *)LocalAlloc(LPTR,buffersize);
            }
            else
            {
                // Unknown Failure.
                do_error();        
                goto cleanup_DeviceInfo;
            }            
        }

        if (GetLastError() == ERROR_INVALID_DATA) 
            continue;
        
        //
        // Compare each entry in the buffer multi-sz list with our HardwareID.
        //
        for (p=buffer;*p&&(p<&buffer[buffersize]);p+=lstrlen(p)+sizeof(TCHAR))
        {	
			//OutputDebugString(_T("Dev hardware_id: "));
			//OutputDebugString(p);
			//OutputDebugString(_T("\n"));
            if (!_tcsicmp(hardware_id->dev,p))
            {
                if (hardware_id->drivername != NULL)
                {
                    // Check we have the correct driver name.
                    if (SetupDiBuildDriverInfoList(DeviceInfoSet, &DeviceInfoData, SPDIT_COMPATDRIVER))
                    {
                        SP_DRVINFO_DATA DriverInfoData;
                        DriverInfoData.cbSize = sizeof(SP_DRVINFO_DATA);
                        bool found = false;
                        int driver = 0;

                        while (SetupDiEnumDriverInfo(DeviceInfoSet, &DeviceInfoData,
                                                     SPDIT_COMPATDRIVER, driver++, &DriverInfoData))
                        {
                            if (_tcsicmp(hardware_id->drivername, DriverInfoData.Description) == 0)
                                found = true;
                        }

                        SetupDiDestroyDriverInfoList(DeviceInfoSet, &DeviceInfoData, SPDIT_COMPATDRIVER);

                        if (!found)
                            break;
                    }
                }

				report(progress_, _T("Found instance in registry"), CPurgeReporter::Tasked);
				Sleep(DISPLAY_SLEEP);

                //
                // Worker function to remove device.
                //
                if (!SetupDiCallClassInstaller(DIF_REMOVE,
	                    DeviceInfoSet,
		                &DeviceInfoData))
					do_error();        
            }
        }

        if (buffer) 
			LocalFree(buffer);
    }

    if ((GetLastError()!=NO_ERROR)&&(GetLastError()!=ERROR_NO_MORE_ITEMS))
       do_error();       
    
    //
    //  Cleanup.
    //    
cleanup_DeviceInfo:
    err = GetLastError();
    SetupDiDestroyDeviceInfoList(DeviceInfoSet);   
    return (err == NO_ERROR); 
}


