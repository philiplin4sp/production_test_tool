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
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/phys/sockettransport.h#1 $
//
///////////////////////////////////////////////////////////////////////

#ifndef __SOCKETTRANSPORT_H__
#define __SOCKETTRANSPORT_H__

#include "common/types.h"
#include "transportimplementation.h"
#include "thread/thread.h"
#include <string>

class socket_abs;

static const int inBufferSize = 4096;

class SocketTransport : public BlueCoreTransportImplementation , Threadable
{
public:
    SocketTransport ( uint16 port , const char * host , SOCKConfiguration::direction dir , const CallBacks& );
    ~SocketTransport ();
    bool start ();
    bool ready ( uint32 timeout );
    void stop ();
    void sendpdu ( const PDU& );
    void setDataWatch ( DataWatcher * );
private:
    socket_abs * m;
    DataWatcher * dw;
    uint16 mPort;
    std::string mHostname;
    char buffer[768];
    CriticalSection bufferInUse;
    int ThreadFunc();
    SignalBox mDone;
    SignalBox::Item dead;
    void getData();
    char inBuffer[inBufferSize];
    std::string partBuffer;
    void processPacket ( const char * buffer , const char * end );
};

#endif//__SOCKETTRANSPORT_H__
