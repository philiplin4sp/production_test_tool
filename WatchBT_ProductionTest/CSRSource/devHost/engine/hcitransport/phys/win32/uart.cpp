///////////////////////////////////////////////////////////////////////////////
//
//  FILE    :   uart.cpp
//
//  Copyright CSR 2001-2006
//
//  CLASS   :   UARTAbstraction
//
//  PURPOSE :   to implement a uart abstraction class for use with BCSP and H4
//              uart transport types.
//
///////////////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
#pragma warning (disable:4786)
#endif

#include <windows.h>

#if !( (defined _WIN32_WCE) || (defined DONT_USE_SETUPDI) )
#include <setupapi.h>
#include <Devguid.h>
#endif

extern "C" 
{
#include "sonic/encapsulated_commands.h"
};

#include "../uart.h"
#include <cassert>
#include "unicode/ichar.h"
#include "thread/system_wide_mutex.h"
#ifndef _WIN32_WCE
#include "pipe.h"

#else
#include "thread/thread.h"
#include "thread/signal_box.h"
#endif
#include "common/algorithm.h"

#undef INITIALISE_WITH_FAKE_DATA
#ifdef INITIALISE_WITH_FAKE_DATA
const static uint8 fake_packet[] =
{
#if 0
    8, 3 , 0x42 , 0, 0, 0,
#endif
#if 1
    #define FAKE_DEVICE_ADDRESS 0xdd, 0xda, 0x34, 0x12, 0x3f, 0xcd
    0xA, 1, 16,
        2, 0, FAKE_DEVICE_ADDRESS,
        1, 0x6f, 0x11, 0xac, 0x11, 0x33, 0x55, 0x77,
                                                     0xFF,
    0xA, 2, 16,
        FAKE_DEVICE_ADDRESS, 0x42, 0x69,
        0x88, 0x87, 0x86, 0x55, 0x99, 0xBB, 0xDD, 0xEE,
                                                     0xFE,
    0xA, 3,  5,  0x10, 0x20, 0x30, 0x40, 0x50,
    0xA, 4,  4,  0x1A, 0x2A, 0x3A, 0x4A, 0x5A,
    #undef FAKE_DEVICE_ADDRESS
#endif
};
#endif


///////////////////////////////////////////////////////////////////////////////
//
//   Test Openers
//
///////////////////////////////////////////////////////////////////////////////
const int MAX_COMM_PORT = 50;

#if !( (defined _WIN32_WCE) || (defined DONT_USE_SETUPDI) )
static istring getFriendlyName(HDEVINFO hDevInfo, SP_DEVINFO_DATA *DeviceInfoData)
{
    DWORD DataT;
    DWORD buffersize = 1;
    std::vector<ichar> buffer(buffersize);

    while (!SetupDiGetDeviceRegistryProperty(hDevInfo,
                                             DeviceInfoData,
                                             SPDRP_FRIENDLYNAME,
                                             &DataT,
                                             (PBYTE)&buffer[0],
                                             buffersize,
                                             &buffersize))
    {
        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {
            buffer.resize(buffersize);
        }
        else
        {
            break;
        }
    }
    return istring(buffer.begin(), buffer.end());
}

static bool nameMatches(istring candidate, istring actual)
{
    if ( actual.substr(0,4) == istring(II("\\\\.\\")) )
        actual = actual.substr(4,actual.size());
    for ( int i = 0; i < actual.size(); ++i )
        actual[i] = toupper(actual[i]);
    return istrstr( candidate.c_str() , actual.c_str() ) != NULL;
}

static bool findMatchingComPort(HDEVINFO hDevInfo, istring actual, SP_DEVINFO_DATA *deviceInfo)
{
    SP_DEVINFO_DATA DeviceInfoData;
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
    {
        istring friendlyName = getFriendlyName(hDevInfo, &DeviceInfoData);
        if (nameMatches(friendlyName, actual))
        {
            *deviceInfo = DeviceInfoData;
            return true;
        }
    }

    return false;
}

static bool hasProperty(const RealUARTConfiguration &config, 
                        bool (*pred)(HDEVINFO hDevInfo, SP_DEVINFO_DATA *DeviceInfoData))
{
    HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES | DIGCF_DEVICEINTERFACE);

    if (hDevInfo == INVALID_HANDLE_VALUE) return false;

    SP_DEVINFO_DATA DeviceInfoData;
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    bool property = findMatchingComPort(hDevInfo, 
                                              config.nameIsWide() ? icoerce(config.getNameW()) : icoerce(config.getNameA()),
                                              &DeviceInfoData) &&
                             pred(hDevInfo, &DeviceInfoData);
    SetupDiDestroyDeviceInfoList(hDevInfo);
    return property;
}

// We only care that the uart exists.
static bool exists(HDEVINFO, SP_DEVINFO_DATA *)
{
    return true;
}
#endif

bool RealUARTConfiguration::testOpen() const
{
#if ( (defined _WIN32_WCE) || (defined DONT_USE_SETUPDI) )
    // wince use old fashioned method.
    HANDLE lComport = INVALID_HANDLE_VALUE;
    if ( nameIsWide() )
        lComport = CreateFileW( getNameW() ,
		    GENERIC_READ|GENERIC_WRITE,
		    0, //exclusive access
		    0, //security
		    OPEN_EXISTING,
		    0,
		    0
		    ) ;
    else
#ifdef _WIN32_WCE
        exit(1);
#else /* defined DONT_USE_SETUPDI */
        lComport = CreateFileA( getNameA() ,
		    GENERIC_READ|GENERIC_WRITE,
		    0, //exclusive access
		    0, //security
		    OPEN_EXISTING,
		    0,
		    0
		    ) ;
#endif
    DWORD i;
    if ( lComport != INVALID_HANDLE_VALUE )
        ::CloseHandle ( lComport );
    else
        i = ::GetLastError();
    return ( lComport != INVALID_HANDLE_VALUE );
#else
    return hasProperty(*this, exists);
#endif
}

#if !( (defined _WIN32_WCE) || (defined DONT_USE_SETUPDI) )
// Checks the vid and pid of a uart to discover if it's a Sonic.
static bool isSonic(HDEVINFO hDevInfo, SP_DEVINFO_DATA *DeviceInfoData)
{
    const int DEVPATH_SIZE = 2000;
    SP_DEVICE_INTERFACE_DATA DeviceInterfaceData;
    struct
    {
        SP_DEVICE_INTERFACE_DETAIL_DATA DeviceInterfaceDetailData;
        TCHAR buf[DEVPATH_SIZE];
    } detail;

    DeviceInterfaceData.cbSize = sizeof(DeviceInterfaceData);
    detail.DeviceInterfaceDetailData.cbSize = sizeof(detail.DeviceInterfaceDetailData);

    for (DWORD j = 0; 
         SetupDiEnumDeviceInterfaces(hDevInfo, 
                                     DeviceInfoData, 
                                     &GUID_DEVINTERFACE_COMPORT,
                                     j, 
                                     &DeviceInterfaceData);
         ++j)
    {
        if (!SetupDiGetDeviceInterfaceDetail(hDevInfo, 
                                             &DeviceInterfaceData,
                                             &detail.DeviceInterfaceDetailData,
                                             (offsetof(SP_DEVICE_INTERFACE_DETAIL_DATA, DevicePath) + 
                                                (sizeof(TCHAR) *  DEVPATH_SIZE)), 
                                             NULL,
                                             NULL))
        {
            return false;
        }

        if (istrstr(detail.DeviceInterfaceDetailData.DevicePath, II("vid_0a12&pid_2009")) != NULL)
        {
            return true;
        }
    }

    return false;
}
#endif

bool RealUARTConfiguration::hasPios() const
{
#if ( (defined _WIN32_WCE) || (defined DONT_USE_SETUPDI) )
    // Impossible to detect safely. Return false so as not to potentially 
    // invoke undefined behaviour.
    return false;
#else
    return hasProperty(*this, isSonic);
#endif
}

#ifndef _WIN32_WCE
const wchar_t * pipeInUseSuffix  = L"-in-use";
const wchar_t * pipeExistsSuffix = L"-exists";

bool PipeConfiguration::testOpen() const
{
    OSVERSIONINFO a;
    a.dwOSVersionInfoSize = sizeof ( a );
    if (::GetVersionEx ( &a ) )
    {
        if ( a.dwPlatformId == VER_PLATFORM_WIN32_NT )
        {
            std::wstring name;
            if ( nameIsWide() )
                name = getNameW();
            else
                name = iwiden ( std::string ( getNameA() ) );
            SystemWideMutex x ( (name + pipeInUseSuffix ).c_str() , true );
            SystemWideMutex y ( (name + pipeExistsSuffix ).c_str() , true );
            //  y must be in use and x not in use for the test to return true.
            return ( !y.IsAcquired() && x.IsAcquired() );
        }
    }
    else
    {
        DWORD lastError = ::GetLastError();
        int i = 10;
    }
    return false;
}
#endif /*_WIN32_WCE*/

static std::string prependSystemName (const std::string &s)
{
    return "\\\\.\\" + s;
}

//  Pipe names
static std::string pipeName(int port)
{
    char buff[32];
    sprintf(buff, "pipe\\csr%d", port);
    return buff;
}

#ifdef DONT_USE_SETUPDI
static std::string comPortName(int port)
{
    char buff[32];
    sprintf(buff, "com%d", port);
    return buff;
}
#endif


static bool strip_name(istring& name )
{
    for ( const ichar * c = name.c_str(); *c != 0 ; c++ )
    {
        if ( *c == 'c' )
        {
            istring test = c;
            if ( test.substr(0,3) == istring(II("com")))
            {
                ichar x = test[3];
                if ( isdigit(x) )
                {
                    int i = 3 + test.substr(3).find_first_not_of(II("0123456789"));
                    name = test.substr(0,i);
                    return true;
                }
            }
        }
    }
    return false;
}

class alphabetise
{
public:
    bool operator() ( std::string& a, std::string& b )
    {
        return (strcmp(a.c_str(),b.c_str()) < 0 );
    }
} alphabetise;

std::vector<std::string> UARTConfiguration::getListOfUarts()
{
    std::vector<std::string> ports;
#ifndef _WIN32_WCE
    // detect pipes pretending to be uarts.
    int fails = 0;
    for(int pipe = 0 ; fails < 16 && pipe < 128 ; ++pipe)
    {
        std::string port = pipeName ( pipe );
        if ( UARTConfiguration ( prependSystemName(port).c_str() ).testOpen() )
        {
            ports.push_back(port);
            fails = 0;
        }
        else
            fails ++;
    }
    // add real uarts.
#ifdef DONT_USE_SETUPDI
    fails = 0;
    for(int com = 0; fails < 16 && com < 128 ; ++com)
    {
        std::string port = comPortName(com);
        UARTConfiguration lBaseConfig ( prependSystemName(port).c_str() , 0 , UARTConfiguration::none , 0 , false , 0 );
        if ( lBaseConfig.testOpen() )
        {
            ports.push_back(port);
            fails = 0;
        }
        else
            fails ++;
    }
    std::sort(ports.begin(),ports.end(),alphabetise);
    return ports;
#else
    HDEVINFO hDevInfo;

    // Create a HDEVINFO with all present devices.
    hDevInfo = SetupDiGetClassDevs(NULL,
        0, // Enumerator
        0,
        DIGCF_PRESENT | DIGCF_ALLCLASSES );

    if (hDevInfo != INVALID_HANDLE_VALUE)
    {
        SP_DEVINFO_DATA DeviceInfoData;
        DWORD i;
        bool found = false;

        // Enumerate through all devices in Set.
        DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
        for (i=0;SetupDiEnumDeviceInfo(hDevInfo,i,
           &DeviceInfoData);i++)
        {
            DWORD DataT;
            LPTSTR buffer = 0;
            DWORD buffersize = 0;

            // 
            // Call function with null to begin with, 
            // then use the returned buffer size 
            // to Alloc the buffer. Keep calling until
            // success or an unknown failure.
            // 
            while (!SetupDiGetDeviceRegistryProperty(
                    hDevInfo,
                    &DeviceInfoData,
                    12,
                    &DataT,
                    (PBYTE)buffer,
                    buffersize,
                    &buffersize))
            {
                if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
                {
                    // Change the buffer size.
                    delete[] buffer;
                    buffer = new TCHAR[buffersize];
                }
                else
                {
                   // Insert error handling here.
                   break;
                }
            }
            if (buffer)
            {
                int k;
                istring isbuffer = buffer;
                for (k = 0; k < isbuffer.size(); ++k)
                {
                    isbuffer[k] = itolower(isbuffer[k]);
                }
                if ( strip_name(isbuffer))
                {
                    std::string sbuffer = inarrow(isbuffer);
                    ports.push_back(sbuffer);
                }
                delete[] buffer;
            }
        }

        //  Cleanup
        SetupDiDestroyDeviceInfoList(hDevInfo);

		if ( GetLastError()==NO_ERROR ||
             GetLastError()==ERROR_NO_MORE_ITEMS )
        {
            std::sort(ports.begin(),ports.end(),alphabetise);
            return ports;
        }
    }
#endif /* DONT_USE_SETUPDI */
#else
	int c;
	int found = 0;
	char portname[32];
	TCHAR ws[200];
	HANDLE test;
	for ( c = 1 ; c < MAX_COMM_PORT ; c++ )
	{
		sprintf(portname, "com%d:", c);
		MultiByteToWideChar(CP_ACP,0,portname,-1,ws,32);
		if ( (test = ::CreateFile(ws, GENERIC_READ | GENERIC_WRITE,
										0,0,OPEN_EXISTING,0,0) ) != INVALID_HANDLE_VALUE )
		{
			::CloseHandle(test);
			ports.push_back(portname);
		}
	}
	return ports;
#endif
	ports.empty();
    return ports;
}
//#endif

///////////////////////////////////////////////////////////////////////////////
//
//   Debug printf class
//
///////////////////////////////////////////////////////////////////////////////
#if (defined _DEBUG) && (!defined _WIN32_WCE)
//#define UART_TRACING
static const int trace_style =
1;
#endif

#ifdef UART_TRACING
#include <sstream>
#include <stdio.h>
#endif

class debugtracer
{
#ifdef UART_TRACING
    FILE * trace;
    LARGE_INTEGER freq;
#endif
public:
#ifdef UART_TRACING
    debugtracer( const char * name )
    {
        if ( !::QueryPerformanceFrequency ( &freq ) )
            freq.QuadPart = 1;
        switch ( trace_style )
        {
        case 1:
            {
				// fixme: memory leak in std::stringstream
//                std::stringstream s ;
//                s << "C:/";
//                s < "uart";
//                s << name;
//                s << "log";
//                s << (GetTickCount() & 0xFFF) << ".txt";
//                trace = fopen ( s.str().c_str() , "w" );
                assert (name);
				char s[64];
				_snprintf(s, sizeof(s), "C:/uart%slog%d.txt", name, (GetTickCount() & 0xFFF));
				trace = fopen(s, "w");
            }
            break;
        case 2:
            trace = stdout;
            break;
        default:
            trace = 0;
            break;
        }
    }
#else
    debugtracer(){}
#endif
    ~debugtracer()
    {
#ifdef UART_TRACING
    if ( (trace_style == 1) && trace )
        fclose ( trace );
#endif
    }
    inline void UTRACE0()
    {
#ifdef UART_TRACING
        if ( trace )
            fprintf ( trace , "In %s at %s\n" , __FILE__ , __LINE__ );
#endif
    }
    inline void UTRACE1S ( const char * string )
    {
#ifdef UART_TRACING
        if ( trace )
        {
            LARGE_INTEGER time;
            if (::QueryPerformanceCounter( &time ) )
            {
                double t2 = ((double)time.QuadPart)/freq.QuadPart;
                fprintf ( trace , "%f: %s\n" , t2 , string );
            }
            else
                fprintf ( trace , "%s\n" , string );
        }
#endif
    }
    inline void UTRACE3SDSx ( const char * string1 , int number , const char * string2 )
    {
#ifdef UART_TRACING
        if ( trace )
            fprintf ( trace , "%s%3d%s" , string1 , number , string2 );
#endif
    }
    inline void UTRACE3SDS ( const char * string1 , int number , const char * string2 )
    {
#ifdef UART_TRACING
        if ( trace )
        {
            LARGE_INTEGER time;
            if (::QueryPerformanceCounter( &time ) )
                fprintf ( trace , "%f: %s%d%s\n" , ((double)time.QuadPart)/freq.QuadPart , string1 , number , string2 );
            else
                fprintf ( trace , "%s%d%s\n" , string1 , number , string2 );
        }
#endif
    }
    inline void UTRACE2SHx ( const char * string , int number )
    {
#ifdef UART_TRACING
        if ( trace )
            fprintf ( trace , "%s%02x" , string , number );
#endif
    }
};

///////////////////////////////////////////////////////////////////////////////
//
//   Real Openers
//
///////////////////////////////////////////////////////////////////////////////

class PortOpener
{
public:
    PortOpener () {}
    virtual ~PortOpener () {}
    virtual bool open( HANDLE& port , HANDLE& cts_evt ) = 0;
    virtual void close( HANDLE& port ) = 0;
    virtual std::string getName() = 0;
    virtual void setRTS ( bool active , HANDLE& h ) = 0;
    virtual bool readCTS ( HANDLE& h ) = 0;
    virtual bool readRTS () = 0;
    virtual bool setCTSTimer ( HANDLE& aH ) = 0;
    virtual bool setBaudRate ( HANDLE& aH , uint32 baudrate ) = 0;
    virtual uint32 getBaudRate ( HANDLE& aH ) = 0;
    virtual bool setHWFC ( HANDLE& aH , bool active ) = 0;
    virtual bool setHWFC ( HANDLE& aH , UARTAbstraction::fcmode mode)
    {
        if (mode == UARTAbstraction::off ||
            mode == UARTAbstraction::normal)
        {
            return setHWFC(aH, mode == UARTAbstraction::normal);
        }
        else
        {
            // some other flow control mode not supported on this uart.
            return false;
        }
    }
    virtual bool setTimeouts( HANDLE &mComport , uint32 readTimeout ) = 0;
    // Most uarts don't have pios or reset, so simply fail.
    virtual bool setPioDrives(HANDLE &aH, uint32 mask, uint32 drives) {return false;}
    virtual bool setPios(HANDLE &aH, uint32 mask, uint32 pios) {return false;}
    virtual bool getPios(HANDLE &aH, uint32 *pios) {return false;}
    virtual bool reset(HANDLE &aH) {return false;}
private:
    PortOpener ( const PortOpener & );
};

class UARTOpener : public PortOpener
{
public:
    bool readRTS ();
    void setRTS ( bool active , HANDLE& h );
    bool readCTS ( HANDLE& h );
    bool setCTSTimer ( HANDLE& h );
    bool setBaudRate ( HANDLE& aH , uint32 baudrate );
    uint32 getBaudRate ( HANDLE& aH );
    bool setHWFC ( HANDLE& ah , bool active );
    bool setTimeouts( HANDLE &mComport , uint32 readTimeout ) ;
protected:
    DCB dcb;
    virtual bool ConfigurePort( HANDLE& aH ) = 0;
    OVERLAPPED cts_overlapped ;
};

class UARTCreator : public UARTOpener
{
public:
    UARTCreator ( const RealUARTConfiguration & aConfig );
    bool open( HANDLE& aH , HANDLE& aE );
    void close( HANDLE& port );
    std::string getName();
private:
    bool ConfigurePort( HANDLE& aH ) ;
    const RealUARTConfiguration mConfig ;
};

class SonicUARTCreator : public UARTCreator
{
public:
    SonicUARTCreator ( const RealUARTConfiguration & aConfig);
    virtual bool setHWFC ( HANDLE& aH , bool active );
    virtual bool setHWFC ( HANDLE& aH , UARTAbstraction::fcmode mode);
    virtual bool setPioDrives(HANDLE &aH, uint32 mask, uint32 drives);
    virtual bool setPios(HANDLE &aH, uint32 mask, uint32 pios);
    virtual bool getPios(HANDLE &aH, uint32 *pios);
    virtual bool reset(HANDLE &aH);
    virtual void setRTS(bool active, HANDLE &aH);
    virtual bool readCTS ( HANDLE& h );
};

class UARTPasser : public UARTOpener
{
public:
    UARTPasser ( HANDLE aHandle , const std::string& name );
    bool open( HANDLE& aH , HANDLE& aE );
    void close( HANDLE& port );
    std::string getName();
private:
    bool ConfigurePort( HANDLE& aH ) ;
    std::string name;
    HANDLE handle;
};

#ifndef _WIN32_WCE
class PipeOpener : public PortOpener
{
public:
    PipeOpener ( const PipeConfiguration & aConfig ) : mConfig ( aConfig ), pSema (0) {}
    ~PipeOpener () { delete pSema; }
    bool open( HANDLE& aH , HANDLE& aE );
    void close( HANDLE& port );
    std::string getName();
    void setRTS ( bool active , HANDLE& h ) {}
    bool readRTS ();
    bool readCTS ( HANDLE& h ) { return false; }
    bool setCTSTimer ( HANDLE& h );
    bool setBaudRate ( HANDLE& aH , uint32 baudrate ) { return false; }
    uint32 getBaudRate ( HANDLE& aH ) { return 1000000; }
    bool setHWFC ( HANDLE& ah , bool active ) { return false; }
    bool setTimeouts( HANDLE &mComport , uint32 readTimeout ) ;
private:
    const PipeConfiguration mConfig ;
    SystemWideMutex * pSema;
};
#endif


///////////////////////////////////////////////////////////////////////////////
//
//   UARTAbstraction::implementation
//
///////////////////////////////////////////////////////////////////////////////

class UARTAbstraction::implementation : private debugtracer, public Watched
#ifdef _WIN32_WCE
, private Threadable
#endif
{
public:
    implementation( PortOpener * aPO );
    virtual ~implementation();
    bool open();
    void reopen();
    bool write( const uint8 * aBuffer , size_t aToWrite , uint32 * aWrote );
    bool read( uint8 * aBuffer , size_t aBufferSize , uint32 * aRead );
    Event wait ( uint32 timeout , uint32 * aReadableByteCount );
    bool readCTS()
    {
        bool rv = mPO->readCTS( mComport ) ;
        if ( rv )
            UTRACE1S("CTS: active.");
        else
            UTRACE1S("CTS: inactive.");
        return rv;
    }
    void setRTS( bool raise )
    {
        if ( raise )
            UTRACE1S("RTS: active.");
        else
            UTRACE1S("RTS: inactive.");
        mPO->setRTS( raise , mComport ) ;
    }
#ifdef MM01_DUMP_STATS
	void DumpStats();
#endif /* def MM01_DUMP_STATS */
    bool readRTS () { return mPO->readRTS(); }
    bool setBaudRate ( uint32 baudrate )
    { return mPO->setBaudRate ( mComport , baudrate ); }
    uint32 getBaudRate()
    { return mPO->getBaudRate(mComport); }
    bool setHWFC ( bool active )
    { return mPO->setHWFC ( mComport , active ); }
    bool setHWFC ( fcmode mode )
    { //fcmode = mode;
      return mPO->setHWFC ( mComport , mode ); }
    void expectPulse()
    { expecting_CTS_pulse = true; mPO->setCTSTimer(mComport); }
	bool setReadTimeout(uint32 t)
	{ return mPO->setTimeouts(mComport, t); }
    void abortWait() const;
    void reqXmit() const;
    bool setPioDrives(uint32 mask, uint32 drives)
    { return mPO->setPioDrives(mComport, mask, drives); }
    bool setPios(uint32 mask, uint32 pios)
    { return mPO->setPios(mComport, mask, pios); }
    bool getPios(uint32 *pios)
    { return mPO->getPios(mComport, pios); }
    bool reset()
    { return mPO->reset(mComport); }
private:
    PortOpener * mPO;
    //  Com port related members/methods of BCSPImplementation
    //  friend UINT callRunStack(LPVOID * p) ;
    HANDLE mComport ;
#ifndef _WIN32_WCE
    OVERLAPPED mRCVOverlapped ;
    OVERLAPPED mXMITOverlapped ;
#endif
#define mEventCount (5)
	HANDLE mEvents[mEventCount] ;
    uint8 * mReadBuffer;
    bool expecting_CTS_pulse;
#ifdef _WIN32_WCE
    SignalBox mDieNow;
    SignalBox mDead;
    SignalBox::Item mDieNowI;
    SignalBox::Item mDeadI;
    void createRxWorker(void)
    {
        bool retval = Start();
    }
    int ThreadFunc()
    {
    /*
	    Waitformultipleobjects on comm events. The book talks of 
    using setcommmask and then using waitcommevent to block until any of the events 
     you set up
    */
	    
	    DWORD dwMask;
	    UTRACE1S ("Start the wait for rx");
	    while(!mDieNow.wait(0))
	    {
		    SetCommMask(mComport, EV_RXCHAR | EV_ERR);
			if (WaitCommEvent(mComport,&dwMask, 0))
		    {
				// use the flags returned in dwmask to determine the reason for returning */
			    switch(dwMask) 
			    {
				    case EV_RXCHAR:
		 			    /* set the flags */
					    SetEvent(mEvents[UART_RCVDONE]);
					    break;
			    }
		    }
	    }
	    UTRACE1S ("End the wait for rx");
		mDeadI.set();
        return 0;
    }
#endif
};

#ifdef UART_TRACING
static std::string getnum( const std::string& s )
{
    const char * p = s.c_str();
    std::string rv;
    while ( p && *p )
    {
        if ( isdigit(*p) )
            rv += *p;
        p++;
    }
    return rv;
}
#endif

UARTAbstraction::implementation::implementation ( PortOpener * aPO )
:
    expecting_CTS_pulse ( false ),
    mPO ( aPO ),
#ifdef UART_TRACING
    debugtracer( getnum(aPO->getName()).c_str() ),
#endif
#ifdef _WIN32_WCE
    mDeadI(mDead),
    mDieNowI(mDieNow),
#endif
    mComport ( INVALID_HANDLE_VALUE )
{
    UTRACE1S ( "Creating UART" );

    // arrange events
    mEvents[UART_XMITDONE] = CreateEvent(0,1,0,0) ;
    mEvents[UART_RCVDONE] = CreateEvent(0,1,0,0) ;
    mEvents[CLIENT_TXREQ] = CreateEvent(0,0,0,0) ;
    mEvents[CLIENT_DIEREQ] = CreateEvent(0,0,0,0) ;
    mEvents[UART_CTSPULSED] = CreateEvent(0,1,0,0) ;
    assert(mEventCount == 5);


#ifndef _WIN32_WCE
    // arrange Overlapped IO.
    mXMITOverlapped.Offset = 0;
    mXMITOverlapped.OffsetHigh = 0;
    mXMITOverlapped.hEvent = mEvents[UART_XMITDONE];

    mRCVOverlapped.Offset = 0;
    mRCVOverlapped.OffsetHigh = 0;
    mRCVOverlapped.hEvent = mEvents[UART_RCVDONE];
#endif
}

UARTAbstraction::implementation::~implementation ()
{
#ifdef _WIN32_WCE
    FlushFileBuffers(mComport);
	mDieNowI.set();
    SetCommMask(mComport, 0);
    mDead.wait();
#endif

    if (mComport != INVALID_HANDLE_VALUE) 
    {
	//free com port 
#ifndef _WIN32_WCE
	PurgeComm(mComport, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
#else
	FlushFileBuffers(mComport);
	PurgeComm(mComport, PURGE_TXCLEAR | PURGE_RXCLEAR);
	Sleep(10);
#endif
    }
    mPO->close(mComport) ;
    delete mPO;
    
    //free events
    for (int c = 0 ; c < sizeof(mEvents)/sizeof(HANDLE) ; c++)
	    CloseHandle(mEvents[c]) ;
    UTRACE1S ( "Destroying UART." );
}

bool UARTAbstraction::implementation::write( const uint8 * aBuffer , size_t aToWrite , uint32 * aWrote )
{
    /*uint8 *bodged;
    if (fcmode == h4p_packet) 
    {
        
    }*/

    DWORD lWrote = 0;
    UTRACE3SDSx ( "TX:  (" , aToWrite , ") " );
    for ( int i = 0 ; i < aToWrite ; ++i )
        UTRACE2SHx ( " " , aBuffer[i] );
    UTRACE1S ( " End." );
    bool rv = WriteFile ( mComport,aBuffer,aToWrite,&lWrote,
#ifndef _WIN32_WCE
						  &mXMITOverlapped
#else
						  NULL
#endif
						  ) != 0 ;
    sent ( aBuffer , aToWrite );
    *aWrote = lWrote;
    if ( !rv )
    {
        int error = GetLastError();
        if ( error != 997 )
            UTRACE3SDS ( "TX: An error has occured (", error , ")." );
    }
    return rv;
}

bool UARTAbstraction::implementation::read( uint8 * aBuffer , size_t aBufferSize , uint32 * aRead )
{
    DWORD lRead;
    UTRACE3SDS ( "Initiating Read (" , aBufferSize , ")." );
    mReadBuffer = aBuffer;
    bool rv;
#ifdef INITIALISE_WITH_FAKE_DATA
    static int fake_index = 0;
    if ( fake_index < sizeof(fake_packet) )
    {
        rv = true;
        lRead = std::min(size_t(aBufferSize), sizeof(fake_packet) - fake_index);
        memcpy(aBuffer,fake_packet + fake_index,lRead);
        fake_index += lRead;
    }
    else
#endif
        rv = ReadFile ( mComport,aBuffer,aBufferSize,&lRead,
#ifndef _WIN32_WCE
						&mRCVOverlapped
#else
						NULL
#endif
						) != 0 ;
    *aRead = lRead;
    if ( lRead && rv )
    {
        recv ( aBuffer , aBufferSize );
        UTRACE3SDSx ( "RXI: (" , lRead , ") " );
        for ( int i = 0 ; i < lRead ; ++i )
            UTRACE2SHx ( " " , mReadBuffer[i] );
        UTRACE1S ( " End." );
    }
    return rv;
}

void UARTAbstraction::implementation::reopen()
{
    UTRACE1S ( "Reopening." );
    PurgeComm(mComport, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
    mPO->close(mComport) ;
    open();
}

UARTAbstraction::Event UARTAbstraction::implementation::wait ( uint32 timeout , uint32 * aReadableByteCount )
{
    UTRACE1S ("Entering wait.");
    DWORD ret = WaitForMultipleObjects(mEventCount,mEvents,false,timeout) ;
    UTRACE1S ("Done Waiting...");
    if (ret !=WAIT_TIMEOUT)
    {
        DWORD i = ret-WAIT_OBJECT_0;
        switch ( i )
        {
        case UART_XMITDONE : //This is the transmit-finished - all we need to do is reset the event
            ResetEvent(mEvents[UART_XMITDONE]) ; 
            UTRACE1S("Transmit done");
            break ; //allow xmit to go ahead 
        case UART_RCVDONE: //This is a receive from the uart 
#ifndef _WIN32_WCE
            /* A seperate thread which just waits for receive data is set up */
            /* which generates a WINCE_RECV event */
            {
                UTRACE1S("Received.");
                DWORD lReadableByteCount =0;
                BOOL res = GetOverlappedResult(mComport,&mRCVOverlapped,&lReadableByteCount,false) ;
                *aReadableByteCount = lReadableByteCount;
                if (!res) 
                {
                    *aReadableByteCount = 0 ; //oops - this was a timeout!
                    UTRACE1S ( "Rcv Timeout." );
                }
                else if (0 != *aReadableByteCount)
                {
                    recv ( mReadBuffer , lReadableByteCount );
                    UTRACE3SDSx ( "RXD: (" , *aReadableByteCount , ") " );
                    for ( int i = 0 ; i < *aReadableByteCount ; ++i )
                        UTRACE2SHx ( " " , mReadBuffer[i] );
                    UTRACE1S ( " End." );
                }
                else
                {
                    // the driver hasn't really returned any data
                }
            }
#endif
            ResetEvent(mEvents[UART_RCVDONE]) ; 
            break ;
        case CLIENT_TXREQ:
            UTRACE1S("Transmit Requested");
            break ;//write req - no action necessary
        case CLIENT_DIEREQ: //the user has requested that we shutdown
            UTRACE1S("Die request.");
            PurgeComm(mComport, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
            break ;
        case UART_CTSPULSED:
            UTRACE1S("CTS edge...");
            if ( expecting_CTS_pulse )
            {
                for ( int i = 0 ; i < 1000 && !mPO->readCTS( mComport ) ; i++ )
                {
                    UTRACE1S("Polling on CTS...");
                    ::Sleep(0);
                }
                UTRACE1S("CTS active again.");
                expecting_CTS_pulse = false;
                ::SetCommMask ( mComport , 0 );
            }
            ResetEvent(mEvents[UART_CTSPULSED]) ;
            break;
        default :
            UTRACE1S("Other response.");
            break ;//error!
        }
        return (UARTAbstraction::Event) i;
    }
    else
        return timedOut;
}

void UARTAbstraction::implementation::abortWait() const
{
    SetEvent(mEvents[CLIENT_DIEREQ]) ;
}

void UARTAbstraction::implementation::reqXmit() const
{
    SetEvent(mEvents[CLIENT_TXREQ]) ;
}

bool UARTAbstraction::implementation::open() 
{
    std::string text = "Opening " + mPO->getName();
    UTRACE1S ( text.c_str() );
    bool ok = mPO->open ( mComport , mEvents[UART_CTSPULSED] );
#ifdef _WIN32_WCE
    if ( ok )
        createRxWorker();	// create the thread that just blocks on waiting for receive
	else
		// the thread is dead if never created!
		mDeadI.set();
#endif
    return ok;
}

//make the com error character equal to a SLIP frame marker
#define DCB_ERROR_CHAR ((char)0xc0)

UARTCreator::UARTCreator ( const RealUARTConfiguration & aConfig )
: mConfig ( aConfig )
{
    memset(&dcb,0,sizeof(DCB)) ;
    dcb.DCBlength = sizeof(DCB) ;
    dcb.BaudRate = mConfig.getBaudRate();
    //  windows admits parity e,o,n,m,s.  BCSP allows none, odd or even.
    switch ( mConfig.getParity() )
    {
    case UARTConfiguration::none:
        dcb.Parity = NOPARITY;
        break;
    case UARTConfiguration::odd:
        dcb.Parity = ODDPARITY;
        break;
    case UARTConfiguration::even:
    default:
        dcb.Parity = EVENPARITY;
        break;
    }
    // BCSP and h4 define an 8 bit wide data stream.
    dcb.ByteSize = 8;
    //  How many stop bits?  What about 1.5?
    if ( mConfig.getStopBits() == 2 )
        dcb.StopBits = TWOSTOPBITS;
    else
        dcb.StopBits = ONESTOPBIT;
    //  switch on error checking - and set the error character.
    dcb.fErrorChar = 1 ;
    dcb.ErrorChar = DCB_ERROR_CHAR ;
    //  must be true.
    dcb.fBinary = 1 ;
    //  parity checking on.
    dcb.fParity = 1 ; 
    if ( mConfig.getFlowControlOn() )
    {
        dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
		dcb.fOutxCtsFlow = 1;
    }
    else
    {
        dcb.fRtsControl = RTS_CONTROL_DISABLE;
        dcb.fOutxCtsFlow = 0;
    }
}

#include "time/hi_res_clock.h"

bool UARTCreator::open ( HANDLE& aH , HANDLE& aE )
{
    //  attempt to open a com port - returns true if successful, false if not
    //  the portname is expected to be of the form "comN" where N=1,2,3 etc

    //  See docs for BuildCommDCB for details of parameters
    bool ok = true;
    if ( mConfig.nameIsWide() )
        aH = CreateFileW( mConfig.getNameW() ,
	                  GENERIC_READ|GENERIC_WRITE,
	                  0, //exclusive access
	                  0, //security
	                  OPEN_EXISTING,
#ifndef _WIN32_WCE
	                  FILE_FLAG_OVERLAPPED,
#else
					  0,
#endif
	                  0
	                ) ;
    else
#ifndef _WIN32_WCE
        aH = CreateFileA( mConfig.getNameA() ,
	                  GENERIC_READ|GENERIC_WRITE,
		          0, //exclusive access
		          0, //security
		          OPEN_EXISTING,
		          FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL,
		          0
		        ) ;
#else
        exit(1);
#endif

    cts_overlapped.Offset = 0;
    cts_overlapped.OffsetHigh = 0;
    cts_overlapped.hEvent = aE;
    //set up the port parameters - baud rate etc
    ok = (aH != INVALID_HANDLE_VALUE) && ConfigurePort( aH );

    return ok;
}

void UARTCreator::close ( HANDLE& aH )
{
    CloseHandle(aH);
}

bool UARTOpener::setTimeouts( HANDLE &mComport , uint32 readTimeout )
{
    //set the timeouts for the com port. 
    COMMTIMEOUTS sCT ;
    //  All zeros say never timeout.  Ever.
    //  Unless there is an over riding timeout...
    memset(&sCT,0,sizeof(COMMTIMEOUTS)) ;
    sCT.ReadIntervalTimeout = readTimeout;
    return ( SetCommTimeouts ( mComport, &sCT ) != 0 );
}

#ifndef _WIN32_WCE
bool PipeOpener::setTimeouts( HANDLE &mComport , uint32 readTimeout )
{
    return true;
}
#endif

bool UARTCreator::ConfigurePort ( HANDLE &mComport )
{
    return SetCommState(mComport,&dcb)
        && setTimeouts( mComport , mConfig.getReadTimeout() )
#ifndef _WIN32_WCE
        && SetupComm(mComport,1000,1000);
#else
        && SetupComm(mComport,16,16);
#endif
}

static UART_flowctrl_e uart_to_sonic_fcmode(UARTAbstraction::fcmode mode)
{
    UART_flowctrl_e smode = UART_flowctrl_noFC;

    switch (mode)
    {
    case UARTAbstraction::off:
        smode = UART_flowctrl_noFC;
        break;
    case UARTAbstraction::normal:
        smode = UART_flowctrl_hardware;
        break;
    }
    return smode;
}

SonicUARTCreator::SonicUARTCreator( const RealUARTConfiguration & aConfig ) :
    UARTCreator(aConfig)
{
}

bool SonicUARTCreator::setHWFC( HANDLE& aH , bool active )
{
    return setHWFC(aH, active ? UARTAbstraction::off : UARTAbstraction::off);
}

bool SonicUARTCreator::setHWFC ( HANDLE& aH , UARTAbstraction::fcmode mode)
{
    return enc_UART_flowctrl_set(aH, uart_to_sonic_fcmode(mode)) == 0;
}

bool SonicUARTCreator::setPioDrives(HANDLE &aH, uint32 mask, uint32 drives) 
{
    uint32 shifted_mask = mask;
    uint32 shifted_drives = drives;

    for (int i = 0x307; i >= 0x304; --i)
    {
        uint8 current_mask_octet = shifted_mask & 0xff;
        // Avoid touching any registers we're not interested in.
        if (current_mask_octet != 0)
        {
            uint8 current_drive_octet = shifted_drives & 0xff;
            uint8 current_register_contents = 0;
            if (read_fpga_register(aH, i, &current_register_contents) != 0)
            {
                return false;
            }
            uint8 new_register_contents = (current_register_contents & ~current_mask_octet) | current_drive_octet;
            if (write_fpga_register(aH, i, new_register_contents) != 0)
            {
                return false;
            }
        }
        shifted_mask >>= 8;
        shifted_drives >>= 8;
    }
    return true;
}

bool SonicUARTCreator::setPios(HANDLE &aH, uint32 mask, uint32 pios)
{
    return enc_pio_set(aH, mask, pios) == 0;
}

bool SonicUARTCreator::getPios(HANDLE &aH, uint32 *pios)
{
    uint32 tmp;
    if (enc_pio_get(aH, &tmp) != 0)
    {
        return false;
    }
    *pios = tmp;
    return true;
}

bool SonicUARTCreator::reset(HANDLE &aH)
{
    return enc_DUT_reset(aH, 100) == 0;
}

void SonicUARTCreator::setRTS(bool active, HANDLE &aH)
{
    enc_UART_flowctrl_set(aH, active? UART_flowctrl_CTSnRTSa : UART_flowctrl_CTSnRTSd);
}

bool SonicUARTCreator::readCTS(HANDLE &h)
{
    BOOL result;
    enc_UART_CTS_get(h, &result);
    return result;
}

#ifndef _WIN32_WCE
bool PipeOpener::open ( HANDLE& aH , HANDLE& aE )
{
    std::wstring name;
    if ( mConfig.nameIsWide() )
        name = mConfig.getNameW();
    else
        name = iwiden ( std::string ( mConfig.getNameA() ) );
    pSema = new SystemWideMutex ( (name + pipeInUseSuffix).c_str() , true );
    if ( pSema->IsAcquired() )
    {
        if ( mConfig.nameIsWide() )
        //  try to open an existing pipe first...
            aH = CreateFileW( mConfig.getNameW() ,
		        GENERIC_READ|GENERIC_WRITE,
		        0, //exclusive access
		        0, //security
		        OPEN_EXISTING,
		        FILE_FLAG_OVERLAPPED,
		        0
		        ) ;
        else
            aH = CreateFileA( mConfig.getNameA() ,
		        GENERIC_READ|GENERIC_WRITE,
		        0, //exclusive access
		        0, //security
		        OPEN_EXISTING,
		        FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL,
		        0
		        ) ;
        //  did it work?
        DWORD i;
        if ( aH == INVALID_HANDLE_VALUE )
            i = ::GetLastError();
        if (aH == INVALID_HANDLE_VALUE)
        {
            delete pSema;
            pSema = new SystemWideMutex ( (name + pipeExistsSuffix).c_str() , true );
            //  no, so create the pipe.
            if ( pSema->IsAcquired() )
            {
                if ( mConfig.nameIsWide() )
                    aH = CreateNamedPipeW ( mConfig.getNameW(),
                                PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED ,
                                PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,
                                1,
                                1000,
                                1000,
                                0,
                                0
                                );
                else
                    aH = CreateNamedPipeA ( mConfig.getNameA(),
                                PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED ,
                                PIPE_TYPE_BYTE | PIPE_READMODE_BYTE ,
                                1,
                                1000,
                                1000,
                                0,
                                0
                                );
            }
        }
        if ( aH == INVALID_HANDLE_VALUE )
            i = ::GetLastError();
    }
    return (aH != INVALID_HANDLE_VALUE);
}

void PipeOpener::close ( HANDLE& aH )
{
    CloseHandle(aH);
}

std::string PipeOpener::getName()
{
    if ( mConfig.nameIsWide() )
        return inarrow ( std::basic_string<wchar_t>(mConfig.getNameW()) );
    else
        return mConfig.getNameA();
}
#endif

std::string UARTCreator::getName()
{
    if ( mConfig.nameIsWide() )
        return inarrow ( std::basic_string<wchar_t>(mConfig.getNameW()) );
    else
        return mConfig.getNameA();
}

#ifdef MM01_DUMP_STATS
void UARTAbstraction::implementation::DumpStats()
{
	COMSTAT stat;
	DWORD errs;

	ClearCommError(mComport, &errs, &stat);

	if (errs != 0x0)
	{
		printf("Errors 0x%X:", errs);
		if (errs & CE_RXOVER)	printf(" CE_RXOVER");
		if (errs & CE_OVERRUN)	printf(" CE_OVERRUN");
		if (errs & CE_RXPARITY)	printf(" CE_RXPARITY");
		if (errs & CE_FRAME)	printf(" CE_FRAME");
		if (errs & CE_BREAK)	printf(" CE_BREAK");
		if (errs & CE_TXFULL)	printf(" CE_TXFULL");
		if (errs & CE_PTO)		printf(" CE_PTO");
		if (errs & CE_IOE)		printf(" CE_IOE");
		if (errs & CE_DNS)		printf(" CE_DNS");
		if (errs & CE_OOP)		printf(" CE_OOP");
		if (errs & CE_MODE)		printf(" CE_MODE");
		printf("\n");

		printf("CtsHold %d  DsrHold %d  RlsdHold %d  XoffHold %d  XoffSent %d  Eof %d  Txim %d\n",
			stat.fCtsHold, stat.fDsrHold, stat.fRlsdHold, stat.fXoffHold,
			stat.fXoffSent, stat.fEof, stat.fTxim);
		printf("InQue %d  OutQue %d\n", stat.cbInQue, stat.cbOutQue);
	}
}
#endif /* def MM01_DUMP_STATS */

bool UARTAbstraction::readRTS ()
{
    return mUart->readRTS();
}

#ifndef _WIN32_WCE
bool PipeOpener::readRTS()
{
    return true;
}
#endif

bool UARTOpener::readRTS()
{
    return (dcb.fRtsControl == RTS_CONTROL_ENABLE);
}

void UARTOpener::setRTS( bool active , HANDLE& h )
{
    //  only meaningful if there is no Hardware flow control.
    if ( dcb.fRtsControl != RTS_CONTROL_HANDSHAKE )
    {
        dcb.fRtsControl = active ? RTS_CONTROL_ENABLE : RTS_CONTROL_DISABLE;

		// HERE:MM 16:sep:02
		// I have chnaged this to use the 'EscapeCommState' function instead
		// of 'SetCommState'.  The later performs a reinitialisation of the
		// hardware and can cause us to lose a byte or two.  This was 
		// particuarly bad with the connect tech BlueHeat cards.
		//
		// It apears that we we wrap this function call with two calls to
		// GetCommState the fRtsControl member does not change.  I am not
		// sure that this is a problem?  The local copy should be accurate.
		//
		// After reading the documetation for DCB again I get the impression
		// that this field only sets the state of the rts line when the device
		// is initialised?
		// BOOL SetRTSresult = SetCommState(h, &dcb);
		BOOL SetRTSresult = EscapeCommFunction(h, active ? SETRTS : CLRRTS);
        assert(SetRTSresult);
    }
}

bool UARTOpener::readCTS( HANDLE& h )
{
    DWORD stat = 0;
    if ( h != INVALID_HANDLE_VALUE )
    {
        int readCTSFailure = ::GetCommModemStatus ( h , &stat );
        assert ( readCTSFailure );
    }
    return ((stat & MS_CTS_ON) == MS_CTS_ON);
}

bool UARTOpener::setCTSTimer ( HANDLE& h )
{
    DWORD ModemEvents = 0;
    return SetCommMask ( h , EV_CTS )
      && ( ::WaitCommEvent ( h , &ModemEvents , &cts_overlapped )
        || ::GetLastError() == ERROR_IO_PENDING );
}

#ifndef _WIN32_WCE
bool PipeOpener::setCTSTimer ( HANDLE& h )
{
    return false;
}
#endif

bool UARTOpener::setBaudRate ( HANDLE& h , uint32 baudrate )
{
    dcb.BaudRate = baudrate;
    return (SetCommState ( h , &dcb ) != 0);
}

uint32 UARTOpener::getBaudRate ( HANDLE& h )
{
    return dcb.BaudRate;
}

bool UARTOpener::setHWFC ( HANDLE& h , bool active )
{
    if ( active )
    {
        dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
		dcb.fOutxCtsFlow = 1;
    }
    else
    {
        dcb.fRtsControl = RTS_CONTROL_DISABLE;
        dcb.fOutxCtsFlow = 0;
    }
    return SetCommState ( h , &dcb ) != 0;
}

bool RealUARTHandle::getFlowControlOn() const
{
    DCB a;
    GetCommState(mHandle,&a);
    return a.fRtsControl == RTS_CONTROL_HANDSHAKE;
}

UARTPasser::UARTPasser(HANDLE aHandle , const std::string& aName )
: UARTOpener(), name(aName), handle(aHandle)
{
}

bool UARTPasser::open(HANDLE& aH , HANDLE& aE )
{
    cts_overlapped.Offset = 0;
    cts_overlapped.OffsetHigh = 0;
    cts_overlapped.hEvent = aE;
    aH = handle;
    return handle != INVALID_HANDLE_VALUE;
}

void UARTPasser::close( HANDLE& port )
{
    //  don't close it!
}

bool UARTPasser::ConfigurePort( HANDLE& aH )
{
    // someone else did this before they passed it in.
    return true;
}

std::string UARTPasser::getName()
{
    return name;
}

///////////////////////////////////////////////////////////////////////////////
//
//   UARTAbstraction wrapper functions
//
///////////////////////////////////////////////////////////////////////////////

UARTAbstraction::UARTAbstraction ( const RealUARTConfiguration & aConfig )
: mUart ( new implementation ( aConfig.hasPios() ? 
                                static_cast<UARTCreator *>(new SonicUARTCreator(aConfig)) : 
                                new UARTCreator ( aConfig ) ) )
{
}

#ifndef _WIN32_WCE
UARTAbstraction::UARTAbstraction ( const PipeConfiguration & aConfig )
: mUart ( new implementation ( new PipeOpener ( aConfig ) ) )
{
}

#endif

UARTAbstraction::UARTAbstraction ( const RealUARTHandle & aConfig )
: mUart ( new implementation ( new UARTPasser ( aConfig.getHandle(), aConfig.nameIsWide() ? inarrow ( std::basic_string<wchar_t>(aConfig.getNameW()) ).c_str() :aConfig.getNameA() ) ) )
{
}

UARTAbstraction::~UARTAbstraction ()
{
    delete mUart;
}

bool UARTAbstraction::write( const uint8 * aBuffer , size_t aToWrite , uint32 * aWrote ) const
{
    return mUart->write ( aBuffer , aToWrite , aWrote );
}

bool UARTAbstraction::read( uint8 * aBuffer , size_t aBufferSize , uint32 * aRead ) const
{
    return mUart->read ( aBuffer , aBufferSize , aRead );
}

bool UARTAbstraction::readCTS()
{
    return mUart->readCTS();
}

void UARTAbstraction::setRTS( bool raise )
{
    mUart->setRTS( raise );
}

void UARTAbstraction::reopen()
{
    mUart->reopen();
}

UARTAbstraction::Event UARTAbstraction::wait ( uint32 timeout , uint32 * aReadableByteCount ) const
{
    return mUart->wait ( timeout , aReadableByteCount );
}

void UARTAbstraction::abortWait() const
{
    mUart->abortWait();
}

void UARTAbstraction::reqXmit() const
{
    mUart->reqXmit();
}

bool UARTAbstraction::open()
{
    return mUart->open ();
}

int UARTAbstraction::LastError() const
{
    int err = GetLastError();
    if ( err == ERROR_IO_PENDING )
        err = 0;
    return err;
}

bool UARTAbstraction::setBaudRate ( uint32 baudrate )
{
    return mUart->setBaudRate ( baudrate );
}

uint32 UARTAbstraction::getBaudRate()
{
    return mUart->getBaudRate();
}

bool UARTAbstraction::setHWFC ( bool active )
{
    return mUart->setHWFC ( active );
}

bool UARTAbstraction::setHWFC ( fcmode mode )
{
    return mUart->setHWFC ( mode );
}

void UARTAbstraction::expectPulse()
{
    mUart->expectPulse();
}

void UARTAbstraction::setDataWatch ( DataWatcher * w )
{
    mUart->set ( w );
}

bool UARTAbstraction::setReadTimeout(uint32 t)
{
    return mUart->setReadTimeout(t);
}

bool UARTAbstraction::setPioDrives(uint32 mask, uint32 drives) 
{
    return mUart->setPioDrives(mask, drives);
}

bool UARTAbstraction::setPios(uint32 mask, uint32 pios)
{
    return mUart->setPios(mask, pios);
}

bool UARTAbstraction::getPios(uint32 *pios)
{
    return mUart->getPios(pios);
}

bool UARTAbstraction::reset() 
{
    return mUart->reset();
}

#ifdef MM01_DUMP_STATS
void UARTAbstraction::DumpStats()
{
	mUart->DumpStats();
}
#endif /* def MM01_DUMP_STATS */
