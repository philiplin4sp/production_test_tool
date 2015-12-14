///////////////////////////////////////////////////////////////////////////////
//
//  FILE   :  transportimplementation.h
//
//  Copyright CSR 2001-2006
//
//  AUTHOR :  Adam Hughes
//
//  CLASS  :  TransportImplementation
//
//  PURPOSE:  Provide a single standard transport object.
//            This oblject is used for pointer-to-implementation
//            in the transport apis and as the base class for
//            the specific transport types.
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/phys/transportimplementation.h#1 $
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TRANSPORTIMPLEMENTATION_H_
#define _TRANSPORTIMPLEMENTATION_H_

#include "transportapi.h"
#include "thread/signal_box.h"
#include "thread/critical_section.h"
#include <vector>
#include "csrhci/transportconfiguration.h"

class BlueCoreTransportImplementation
{
public:
    BlueCoreTransportImplementation( const char * aName , bool aTunnel , const CallBacks& aRecv );
    virtual ~BlueCoreTransportImplementation();

    //  control the underlying transport
    virtual bool start ( void ) = 0 ;
    virtual bool ready ( uint32 timeOut ) = 0 ;
    virtual void stop ( void ) = 0 ;

	virtual bool pdu_size_forbidden( const PDU& aPacket) { return false; }
    virtual void sendpdu  ( const PDU& aPacket ) = 0;
    virtual void blockUntilSentPdu( const PDU& aPacket , uint32 timeout );

    // look at the name.
    const char * getName() const;

    virtual void setDataWatch ( DataWatcher * w ) = 0;

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  SleepControl  -  Control when the transport lets the BlueCore sleep
    //
    virtual bool SleepControl(enum SleepType);

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  PingPeriod  -  Control when the transport pings the BlueCore
    //
    virtual bool PingPeriod(int millisecs);

    virtual bool set_sco_bandwidth(int bandwidth)
    { return false; }

    virtual bool reconfigure_uart_baudrate(uint32 baudrate);

private:
    const char * mName;
    CriticalSection OneBlockingCallAtATime;
    SignalBox mSignals;
    BlueCoreTransportCallBack& mReceiver;
    BlueCoreTransportFailure& mReporter;
protected:
    SignalBox::Item sentPdu;

    void recvData    ( PDU::bc_channel channel, const uint8 *apdu, size_t aLength );
    BlueCoreTransportCallBack * mCallBack;

    // tunnelling
    bool tunnel; // use CSR extensions.
    void sendTunnel ( const PDU& aPacket );
    void recvTunnel ( const uint8 *apdu, size_t aLength );
    std::vector<uint8> mRecvTunnel[PDU::csr_hci_extensions_count];
    void onLinkFail( FailureMode f );

private:
    // hide the copy constructor and assignment operator.
    BlueCoreTransportImplementation & operator= ( const BlueCoreTransportImplementation& );
    BlueCoreTransportImplementation ( const BlueCoreTransportImplementation& );
};

#endif//_TRANSPORTIMPLEMENTATION_H_
