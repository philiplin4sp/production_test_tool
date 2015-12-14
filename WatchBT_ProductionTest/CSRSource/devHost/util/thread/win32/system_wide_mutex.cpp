// Copyright (C) Cambridge Silicon Radio Ltd 2001-2009
//
// Win32 implememtation of the Mutex thread.  Uses the CreateMutex function
// 
// Modification History
// $Log: system_wide_mutex.cpp,v $
// Revision 1.11  2003/06/05 20:19:29  cl01
// Vain attampt to fix problem with mutexes
//
// Revision 1.10  2003/06/05 17:22:51  cl01
// Added extra checks.
//
// Revision 1.9  2003/06/05 15:41:10  cl01
// Tidied up interface
// Fixed potential race hazard over acquired var in threads.
//
// Revision 1.8  2002/09/23 16:07:14  cl01
// Changed actual mutex object from Semaphore to Mutex
//
// Revision 1.7  2002/06/27 14:31:59  cl01
// Relaxed rules for printing filenames in Mutex Logging
//
// Revision 1.6  2002/06/27 13:40:10  cl01
// Added debug log (to diagnose spi mutex deadlocks)
//
// Revision 1.5  2002/03/13 18:17:43  bcsw
// Changed non-Unicode support
//
// Revision 1.4  2001/12/19 17:37:25  ajh
// Internationalisation route 2:
// Dual interface, without ichar.
//
// Revision 1.3  2001/11/27 09:53:02  at01
// Corrected size of allocated buffer for Unicode strings.
//
// Revision 1.2  2001/11/19 15:14:27  at01
// HostStack and DFUWizard updated to use Unicode characters under Win32 via the unicode/ichar.h support. This will use the Microsoft Layer for Unicode under Windows 9x/ME (hopefully). This initial set of changes is likely to break everything other than DFUWizard until suitable changes are made elsewhere.
//
// Revision 1.1  2001/10/11 16:44:56  bs01
// Rename from mutex to system_wide_mutex (the shorter name could have cause people to use it inappropriately and get unintended interactions between separate processes.)
//
// Revision 1.5  2001/10/11 10:13:52  cl01
// Added code to prefix any mutex name.
//
// Revision 1.4  2001/10/10 17:26:17  cl01
// Added code to prefix any mutex name.
//
// Revision 1.3  2001/10/10 11:53:47  cl01
// Uses Semaphore as underlying sync obj.  Mutex had problems when moved between threads.
//
// Revision 1.2  2001/10/09 18:03:47  cl01
// Added Exclusive Access for Widcomm USB driver.
//
//
// $Id: //depot/bc/bluesuite_2_4/devHost/util/thread/win32/system_wide_mutex.cpp#1 $

#include "thread/system_wide_mutex.h"
#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <tchar.h>

#include "thread/error_msg.h"

#if defined(MUTEX_LOG) && defined(EXTRA_DEBUG)
#define DATA_LOG_MSG(x) WriteLog(sprintf_static(x))
#define LOG_MSG(x) data_->DATA_LOG_MSG(x)
#else
#define DATA_LOG_MSG(x) do { } while(0)
#define LOG_MSG(x) DATA_LOG_MSG(x)
#endif 

class SystemWideMutex::MutexData
{
public:
    MutexData(const char * mutex_name);
    MutexData(const wchar_t * mutex_name);
    ~MutexData();
    void Release_(bool ignore_error);

    HANDLE handle;
    unsigned m_acquired;

#ifdef MUTEX_LOG
    void WriteLog(const char *string);
    bool enable_logging_;
#endif

private:
    void commonCreation ( const wchar_t *mutex_name );
};

SystemWideMutex::MutexData::MutexData(const char *mutex_name)
    : m_acquired(0)
{
    static const char prefix[] = "csr_mutex_";
    //  this call previously used _alloca.  It is probably better to do so,
    //  but I'm a coward when it comes to new things...  this string gets
    //  deleted at the end of the function.

#ifdef MUTEX_LOG
    enable_logging_ = (strcmp(mutex_name, "spi_app_mutex" ) == 0);
#endif

    TCHAR * string = new TCHAR [ strlen(mutex_name) + strlen(prefix) + 1 ];
    _stprintf(string, TEXT("%hs%hs"), prefix, mutex_name);

    TCHAR *sp = string;
    while (*sp)
    {
        if (_tcschr(TEXT(".\\/\"*"), *sp))
            *sp = '-';
        sp++;
    }
    handle = CreateMutex(NULL, 0, string);
    OUTPUT_HANDLE_CREATE(handle);

    // OutputDebugString("Mutex created:");
    // OutputDebugString(string);
    // OutputDebugString("\n");
    // int lasterr = GetLastError();
    // assert(handle);
    delete string;
}

SystemWideMutex::MutexData::MutexData(const wchar_t *mutex_name)
    : m_acquired(0)
{
    static const wchar_t prefix[] = L"csr_mutex_";
    //  this call previously used _alloca.  It is probably better to do so,
    //  but I'm a coward when it comes to new things...  this string gets
    //  deleted at the end of the function.

#ifdef MUTEX_LOG
    enable_logging_ = (wcscmp(mutex_name, L"spi_app_mutex" ) == 0);
#endif

    wchar_t * string = new wchar_t [ wcslen(mutex_name) + wcslen(prefix) + 1 ];
    wcscpy(string, prefix);
    wcscat(string, mutex_name);

    wchar_t *sp = string;
    while (*sp)
    {
        if (wcschr(L".\\/\"*", *sp))
            *sp = L'-';
        sp++;
    }
    handle = CreateMutexW(NULL, 0, string);
    OUTPUT_HANDLE_CREATE(handle);

    // OutputDebugString("Mutex created:");
    // OutputDebugString(string);
    // OutputDebugString("\n");
    // int lasterr = GetLastError();
    // assert(handle);
    delete string;
}

SystemWideMutex::MutexData::~MutexData()
{
    CloseHandle(handle);
    OUTPUT_HANDLE_CLOSE(handle);

}


#ifdef MUTEX_LOG
void SystemWideMutex::MutexData::WriteLog(const char *string)
{
    FILE *file;
    char buf[128]; 
    if (enable_logging_)
    {
        file = fopen("c:\\spi_app_mutex.log", "ab");
        _strtime(buf);
        fputs(buf, file);
        fputs(" ", file);
        // Prepare a string containing 30 characters containing the end of the exe name + the beginning of the arguments.
        {
            const char *cmd = GetCommandLineA();
            int pos;
            int len = strlen(cmd);
            const char * path_break = strstr(cmd,".exe\"");
            if (!path_break) 
                path_break = strstr(cmd,".EXE\"");
            if (!path_break) 
                path_break = strstr(cmd,"\" ");
            if (!path_break) 
                path_break = strstr(cmd," ");
            if (path_break)
                pos = path_break - cmd - 14;
            else
                pos = len;
            
            if (pos < 0)
                pos = 0;
            char *bufptr = buf;
            for (int i = pos; i < pos + 30; i++)
            {
                if (i < len)
                    *bufptr++ = cmd[i];
                else
                    *bufptr++ = ' ';
            }
            *bufptr = '\0';
                    
            fputs(buf, file);
            fputs(" ", file);
        }
        fputs(string, file);
        fputs("\r\n", file);
        fclose(file);
    }
}
#endif

SystemWideMutex::SystemWideMutex(const char *mutex_name, bool AcquireImmed)
    : data_(new MutexData(mutex_name))
{
    assert(data_);
    if (AcquireImmed) Acquire(0);
}

SystemWideMutex::SystemWideMutex(const wchar_t *mutex_name, bool AcquireImmed)
    : data_(new MutexData(mutex_name))
{
    assert(data_);
    if (AcquireImmed) Acquire(0);
}


SystemWideMutex::~SystemWideMutex()
{
    assert(data_);
    while(IsAcquired()) data_->Release_(true);
    delete data_;
}

bool SystemWideMutex::Acquire(uint32 dwMilliseconds)
{
    assert(data_);
    assert(data_->handle != INVALID_HANDLE_VALUE);

    if(data_->m_acquired)
    {
        ++data_->m_acquired;
    }
    else
    {
        if(NO_TIMEOUT != INFINITE && dwMilliseconds == NO_TIMEOUT) dwMilliseconds = INFINITE;

        DWORD wait_result = WaitForSingleObject(data_->handle, dwMilliseconds);
        switch (wait_result)
        {
        case WAIT_ABANDONED:
            // Another thread died while holding the mutex, we now have it.
        case WAIT_OBJECT_0:
            // We have acquired the mutex
            data_->m_acquired = 1;
            break;

        case WAIT_TIMEOUT:
            // We failed to get the mutex
        default:
            // We should never get here?
            break;
        }

#ifdef MUTEX_LOG
        if (data_->enable_logging_)
        {
            if (data_->m_acquired)
                LOG_MSG(("Acquired mutex"));
            else
                LOG_MSG(("  Failed to acquire mutex"));
        }
#endif
    }

    return data_->m_acquired ? true : false;
}

void SystemWideMutex::Release()
{
    assert(data_);
    data_->Release_(false);
}

void SystemWideMutex::MutexData::Release_(bool ignore_error)
{
    if(m_acquired)
    {
        if(--m_acquired == 0)
        {
            BOOL release_result = ReleaseMutex(handle);
            if(!release_result && !ignore_error)
            {
                ++m_acquired;
            }
#ifdef MUTEX_LOG
            else
            {
                if (enable_logging_)
                {
                    DATA_LOG_MSG(("Released mutex"));
                }
            }
#endif
        }
    }
}

bool SystemWideMutex::IsAcquired() const
{
    assert(data_);
    return data_->m_acquired ? true : false;
}

