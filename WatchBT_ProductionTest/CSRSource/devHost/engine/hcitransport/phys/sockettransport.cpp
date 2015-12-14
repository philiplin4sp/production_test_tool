///////////////////////////////////////////////////////////////////////
//
//  FILE   :  sockettransport.h
//
//  Copyright CSR 2002-2006
//
//  AUTHOR :  Adam Hughes
//
//  CLASS  :  sockettransport
//
//  PURPOSE:  class to send HCI packets over a socket, inheriting top
//            API from the BlueCoreTransport class, and owning a socket
//            abstraction which does all the work.
//
//            BlueCoreTransport provides "private channel" tunneling
//            which is all the protocol we get here!
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/phys/sockettransport.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

#include "sockettransport.h"
#include "thread/thread.h"
#include "thread/critical_section.h"
#include "thread/signal_box.h"

#ifdef _WIN32
#pragma warning (disable:4800)
#endif

#ifdef _DEBUG
#include <stdio.h>
#include <iostream>
using std::cout;
using std::endl;
#endif


static const unsigned int INVALID_HANDLE = (unsigned int)-1;

#ifdef WIN32
///////////////////////////////////////////////////////////////////////
//  GLOBAL DATA FOR WINDOWS SOCKETS
///////////////////////////////////////////////////////////////////////

#include <winsock2.h>
#include <windows.h>
#define BROKEN (char*)
#define perror(a) cout << a << endl
typedef HOSTENT hostent;
typedef TIMEVAL timeval;
#define SHUT_RDWR SD_BOTH
#define BROKEN_close closesocket

class global_data// : private Threadable
{
public:
    global_data () : err ( -1 ) {}
    ~global_data ()
    {
        CriticalSection::Lock a ( cs );
        if ( err == 0 )
            WSACleanup();
    }
    bool isOK ()
    {
        CriticalSection::Lock a ( cs );
        if ( err != 0 )
        {
            WSADATA wsaData;
            err = WSAStartup( 0x101, &wsaData );
#ifdef _DEBUG
            if ( err )
                cout << "Startup Failed: " << err << endl;
#endif
        }
        return ( err == 0 );
    }
    int get_err() { return err; }
private:
    int err;
    CriticalSection cs;
};

static global_data g_info;
#else
///////////////////////////////////////////////////////////////////////
//  UNIX SOCKETS (solaris atm...)
///////////////////////////////////////////////////////////////////////
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#define BROKEN
#define BROKEN_close close
#endif

///////////////////////////////////////////////////////////////////////
//  Socket abstraction layer.
///////////////////////////////////////////////////////////////////////

class socket_abs : public Threadable
{
public:
    socket_abs ( uint16 port )
#ifdef WIN32
    :   ok ( g_info.isOK() ) ,
#else
    :   ok ( true ) ,
#endif
        mConnected ( false ),
        s ( INVALID_HANDLE ) ,
        ws ( INVALID_HANDLE ) ,
        err ( 0 ),
        dying ( mSignals ) ,
        connected ( mSignals ),
        closed ( mSignals ),
        dead ( mDeathKnell ) ,
        mPort ( port ) , 
        mHost ( 0 )
    {
        if ( ok )
        {
            //  The WinSock DLL is acceptable.
            sockaddr_in address;
            address.sin_addr.S_un.S_addr = 0;
            address.sin_family = AF_INET;
            address.sin_port = port;

            int bool_on = 1;

            s = socket ( AF_INET , SOCK_STREAM , 0 );
            if ( s == INVALID_HANDLE )
            {
#ifdef WIN32
                err = WSAGetLastError ();
#else
                err = errno;
#endif
#ifdef _DEBUG
                cout << "err: " << ( err ) << endl;
                perror ( "Can't create socket" );
#endif
            }
            else if ( setsockopt ( s , SOL_SOCKET , SO_KEEPALIVE , BROKEN &bool_on , sizeof(bool_on) ) )
            {
#ifdef WIN32
                err = WSAGetLastError ();
#else
                err = errno;
#endif
#ifdef _DEBUG
                cout << "err: " << ( err ) << endl;
                perror ( "Can't set keepalive" );
#endif
            }
            else if ( bind ( s , (sockaddr*)&address , sizeof (address) ) )
            {
#ifdef WIN32
                err = WSAGetLastError ();
#else
                err = errno;
#endif
#ifdef _DEBUG
                cout << "err: " << ( err ) << endl;
                perror ( "Can't bind" );
#endif
            }
            else
            {
#ifdef _DEBUG
                struct linger linger_p;
                int len = sizeof(linger_p);
                getsockopt( s , SOL_SOCKET , SO_LINGER , BROKEN &linger_p , &len );
                if ( len )
                {
                    cout << "Lingering " << linger_p.l_onoff << endl;
                    cout << "length " << linger_p.l_linger << endl;
                }
                else
                   cout << "len was zero." << endl;
#endif
                if ( listen( s , 0 ) ) // 0=sockets attempting to
                                       //   connect can't queue
                {
#ifdef WIN32
                    err = WSAGetLastError ();
#else
                    err = errno;
#endif
#ifdef _DEBUG
                    cout << "err: " << err << endl;
                    perror ( "Can't listen" );
#endif
                }
#ifdef _DEBUG
                cout << "socket = " << getPort() << endl;
#endif
            }
        }
        if ( err )
        {
            ok = false;
            dead.set();
        }
        else
            Start();
    }
    socket_abs ( const char * name , uint16 port )
#ifdef WIN32
    :   ok ( g_info.isOK() ) ,
#else
    :   ok ( true ) ,
#endif
        mConnected ( false ),
        s ( INVALID_HANDLE ) ,
        ws ( INVALID_HANDLE ) ,
        err ( 0 ) ,
        dying ( mSignals ) ,
        connected ( mSignals ),
        closed ( mSignals ),
        dead ( mDeathKnell ) ,
        mPort ( port ) , 
        mHost ( 0 )
    {
        if ( ok && name )
        {
            //  The WinSock DLL is acceptable, and the name isn't null.
            s = socket ( AF_INET , SOCK_STREAM , 0 );
            if ( s == INVALID_HANDLE )
            {
#ifdef WIN32
                err = WSAGetLastError ();
#else
                err = errno;
#endif
#ifdef _DEBUG
                cout << "err: " << err << endl;
                perror ( "Can't create socket" );
#endif
            }
            mHost = gethostbyname(name);
            if ( !mHost )
            {
#ifdef WIN32
                err = WSAGetLastError ();
#else
                err = errno;
#endif
#ifdef _DEBUG
                cout << "err: " << err << endl;
                perror ( "Can't resolve host" );
#endif
            }
        }
        else
            err = ( ( name == 0 ) ? -1 : -2 );
        if ( err )
        {
            ok = false;
            dead.set();
        }
        else
        {
            ws = s;
            Start();
        }
    }
    ~socket_abs ()
    {
        if ( !dying.getState() )
            kill();
        dead.wait();
    }

    operator bool () const
    {
        return ok;
    }

    bool isReady( uint32 timeout )
    {
        return mConnected || ( mSignals.wait ( timeout ) && mConnected );
    }

    int get_err () { return err; }

    uint16 getPort()
    {
        if (!mPort)
        {
	    sockaddr laddr;
	    int laddr_size = sizeof(laddr);
            if ( getsockname ( s , &laddr , &laddr_size ) )
            {
#ifdef WIN32
                err = WSAGetLastError ();
#else
                err = errno;
#endif
#ifdef _DEBUG
                cout << "err: " << err << endl;
                perror ( "Can't get name" );
#endif
            }
            else
                mPort = *((uint16*)(laddr.sa_data));
        }
        return mPort;
    }

    void kill ()
    {
        dying.set();
        if ( ws != s && ws != INVALID_HANDLE )
        {
            shutdown ( ws , SHUT_RDWR );
            BROKEN_close ( ws );
        }
        if ( s != INVALID_HANDLE )
        {
            shutdown ( ws , SHUT_RDWR );
            BROKEN_close ( s );
        }
    }

    void close_n_accept()
    {
        mConnected = false;
        if ( ws != INVALID_HANDLE )
        {
            if ( shutdown ( ws , SD_BOTH ) )
            {
#ifdef WIN32
                err = WSAGetLastError ();
#else
                err = errno;
#endif
#ifdef _DEBUG
                cout << "err: " << err << endl;
                perror ( "Can't shutdown" );
#endif
            }
            else if ( BROKEN_close ( ws ) )
            {
#ifdef WIN32
                err = WSAGetLastError ();
#else
                err = errno;
#endif
#ifdef _DEBUG
                cout << "err: " << err << endl;
                perror ( "Can't close" );
#endif
            }
            else
                closed.set();
        }
        ok = (err == 0);
    }

    bool send ( const char * buf , size_t len )
    {
        if ( ok )
        {
            ::send ( ws , buf , len , 0 );
        }
        return ok;
    }

    uint32 recv ( char * buf , size_t max_len )
    {
        uint32 len = 0;
        if ( ok )
        {
            len = ::recv ( ws , buf , max_len , 0 );
            if ( len == INVALID_HANDLE )
            {
#ifdef WIN32
                err = WSAGetLastError ();
#else
                err = errno;
#endif
#ifdef _DEBUG
                cout << "err: " << err << endl;
                perror ( "Recv error" );
#endif
            }
            else if ( !len )
            {
                mConnected = false;
#ifdef _DEBUG
                perror ( "No Data" );
#endif
            }
        }
        return len;
    }

    enum events
    {
        TimeOut = 0,
        RecvData = 1,
        Connected = 2,
        Dying = 3,
	Disconnected = 4
    };

    events wait( uint32 timeout )
    {
        if ( dying.getState() )
            return Dying;

        if ( !mConnected )
        {
            if ( mSignals.wait(timeout) && connected.getState() )
            {
                connected.unset();
                return Connected;
            }
            else
                return TimeOut;
        }
        else
        {
            timeval t = { timeout / 1000 , ( timeout % 1000 ) * 1000 };
            fd_set readers;
            FD_ZERO(&readers);
            FD_SET(ws,&readers);
            fd_set errors;
            FD_ZERO(&errors);
            FD_SET(ws,&errors);
            if ( select ( (int)ws + 1 , &readers , 0 , &errors , &t ) )
            {
                bool data = FD_ISSET(ws,&readers);
                bool disc = FD_ISSET(ws,&errors);
                if ( dying.getState() )
                {
                    return Dying;
                }
                else if ( (data && disc) || closed.getState() )
                {
                    mConnected = false;
                    return Disconnected;
                }
                else
                    return RecvData;
            }
            else
                return TimeOut;
        }
    }

    int ThreadFunc ()
    {
        // accept/connect are blocking calls which make ws an active socket.
        if ( mHost )
        {
            sockaddr_in address;
            address.sin_addr.S_un.S_addr = *((unsigned int*)(mHost->h_addr_list[0]));
            address.sin_family = AF_INET;
            address.sin_port = mPort;
            if ( connect ( ws , (sockaddr*)&address , sizeof (address) ) )
            {
#ifdef WIN32
                err = WSAGetLastError ();
#else
                err = errno;
#endif
#ifdef _DEBUG
                cout << "err: " << err << endl;
                perror ( "Can't connect" );
#endif
            }
            else
            {
                mConnected = true;
                connected.set();
            }
            ok = ( err == 0 );

            while ( ok && mConnected && !dying.getState() )
            {
                mSignals.wait();
                if ( dying.getState() )
                    break;
                if ( closed.getState() )
                {
                    closed.unset();
                    break;
                }
            }
        }
        else
        {
            while ( ok && !dying.getState() )
            {
                ws = accept ( s , 0 , 0 );

                if ( ws != INVALID_HANDLE && !dying.getState() )
                {
                    mConnected = true;
                    connected.set();
                }

                while ( ok && mConnected && !dying.getState() )
                {
                    mSignals.wait();
                    if ( dying.getState() )
                        break;
                    if ( closed.getState() )
                    {
                        closed.unset();
                        break;
                    }
                }
                if ( dying.getState() )
                    break;
            }
        }
        dead.set();
        return 0;
    }
private:
    bool ok;
    bool mConnected;
    SOCKET s; // base socket
    SOCKET ws; // working socket
    int err;
    SignalBox mSignals;
    SignalBox::Item dying;
    SignalBox::Item connected;
    SignalBox::Item closed;
    SignalBox mDeathKnell;
    SignalBox::Item dead;

    uint16 mPort;
    hostent * mHost;
};

///////////////////////////////////////////////////////////////////////
//
//  TRANSPORT OBJECT
//
///////////////////////////////////////////////////////////////////////
static std::string getDec ( uint16 a )
{
    char number[10];
    sprintf( number , "%d" , a );
    return number;
}
#include "tunnel.h"

SocketTransport::SocketTransport ( uint16 port , const char * host , SOCKConfiguration::direction dir , const CallBacks& aRecv )
: dw ( 0 ),
  m ( ( dir == SOCKConfiguration::toChip ) ? 0 : new socket_abs ( port ) ),
  mHostname ( host ),
  mPort ( port ),
  BlueCoreTransportImplementation ( ( std::string(host) + ":" + getDec(port) ).c_str() , true , aRecv ),
  dead ( mDone )
{
    if ( m && ::gethostbyname(mHostname.c_str()) != ::gethostbyname ( "localhost" ) )
    {
        delete m;
        m = 0;
    }
}

SocketTransport::~SocketTransport ()
{
    stop ();
    if ( m )
    {
        mDone.wait();
        delete m;
    }
}

bool SocketTransport::start ()
{
    if ( !m )
        m = new socket_abs ( mHostname.c_str() , mPort );
    return *m && Start();
}

bool SocketTransport::ready ( uint32 timeout )
{
    return ( m && m->isReady(timeout) );
}

void SocketTransport::stop ()
{
    if ( m )
        m->kill();
}

void SocketTransport::sendpdu ( const PDU& pdu )
{
    if ( m )
    {
        if (   pdu.channel()                             == PDU::hciCommand
          && ( pdu.data()[0] + ( 0x100 * pdu.data()[1] ) == HCI_TUNNEL_COMMAND ) )
        {
            CriticalSection::Lock here (bufferInUse);
            const uint8 * r = pdu.data() + 3;
            size_t l = pdu.size() - 3;
            for ( int i = 0 ; i < l ; ++i )
                sprintf ( buffer + (3*i) , "%02x " , r[i] );
            buffer[3*l-1] = '\n';
            m->send ( buffer , 3 * l );
            if ( dw )
                dw->onSend ( r , l );
        }
        else
            sendTunnel ( pdu );
    }
}

void SocketTransport::setDataWatch ( DataWatcher * w )
{
    dw = w;
}

int SocketTransport::ThreadFunc()
{
    bool done = false;
    while ( m )
    {
        switch ( m->wait(1000) )
        {
        case socket_abs::TimeOut:
        case socket_abs::Connected:
        case socket_abs::Disconnected:
            //  nothing to do...
            break;
        case socket_abs::Dying:
            done = true;
            break;
        case socket_abs::RecvData:
            getData();
            break;
        default:
            break;
        }
        if ( done )
            break;
    }
    dead.set();
    return 0;
}

void SocketTransport::processPacket ( const char * buffer , const char * end )
{
    uint8 * pkt = new uint8[buffer - end];
    pkt[0] = HCI_TUNNEL_EVENT;
    uint32 pktlen = 2;
    char * stopped = 0;
    while ( buffer < end )
    {
        unsigned long i = strtoul ( buffer , &stopped , 16 );
        if ( buffer == stopped )
            buffer ++;
        else
        {
            buffer = stopped;
            pkt[pktlen++] = (uint8)(i & 0xFF);
        }
    }
    if ( pktlen > 2 && pktlen < 256 )
    {
        pkt[1] = pktlen;
        recvData ( PDU::hciCommand , pkt , pktlen );
    }
}

void SocketTransport::getData ()
{
    uint32 count = m->recv ( inBuffer , inBufferSize );
    char * eot = inBuffer;
    if ( !partBuffer.empty() )
    {
        while ( eot - inBuffer < count && *eot++ != '\n' )
            ;
        partBuffer.append(inBuffer,eot);
        //  if we found a \n, process the packet.
        if ( eot - inBuffer < count )
        {
            processPacket ( partBuffer.data() , partBuffer.data() + partBuffer.size() );
            partBuffer.erase();
        }
    }
    while ( eot - inBuffer < count )
    {
        char * sot = eot;
        while ( eot - inBuffer < count )
        {
            if ( *eot == '\n' )
            {
                processPacket ( sot , eot );
                break;
            }
        }
        if ( eot - inBuffer < count )
            partBuffer.append ( sot , eot );
    }
}

///////////////////////////////////////////////////////////////////////
//
//  TRANSPORT OBJECT
//
///////////////////////////////////////////////////////////////////////

#include "transportapi.h"
#include "unicode/ichar.h"

SOCKConfiguration::SOCKConfiguration ( uint16 p , direction dir )
: port ( p ), hostname ( (char*)0 ) , type ( dir ), TransportConfiguration ( true )
{
}

SOCKConfiguration::SOCKConfiguration ( const char * host , uint16 p , direction dir )
: port ( p ), hostname ( host ) , type ( dir ), TransportConfiguration ( true )
{
}

SOCKConfiguration::SOCKConfiguration ( const wchar_t * host , uint16 p , direction dir )
: port ( p ), hostname ( host ) , type ( dir ), TransportConfiguration ( true )
{
}

BlueCoreTransportImplementation * SOCKConfiguration::make ( const CallBacks& aRecv ) const
{
    std::string name;
    if ( hostname.isW() )
        name = inarrow ( hostname.getW() );
    else
        name = hostname.getA();
    return new SocketTransport ( port , name.c_str() , type , aRecv );
}

TransportConfiguration * SOCKConfiguration::clone () const
{
    return new SOCKConfiguration ( *this );
}
