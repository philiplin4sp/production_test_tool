///////////////////////////////////////////////////////////////////////////////
//
//  FILE   :  transportapi.h
//
//  Copyright CSR 2001-2006
//
//  AUTHOR :  Adam Hughes
//
//  CLASS  :  HCITransport
//            BlueCoreTransport
//
//  PURPOSE:  Provide two standard APIs to transport layer.
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/phys/transportapi.h#1 $
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _PHYS_TRANSPORTAPI_H_
#define _PHYS_TRANSPORTAPI_H_

#include "common/types.h"
#include "common/nocopy.h"
#include "common/countedpointer.h"
#include "hcipacker/pdu.h"
#include "csrhci/transportapi.h"

class DataWatcher;

class TransportConfiguration;
//  a receiver base class.
class BlueCoreTransportCallBack : public NoCopy
{
public:
    virtual ~BlueCoreTransportCallBack() {}
    virtual void onPDU ( PDU::bc_channel channel , const uint8 * buffer , size_t length ) = 0;
};
// a base class to be told if the transport fails
class BlueCoreTransportFailure : public NoCopy
{
public:
    virtual ~BlueCoreTransportFailure() {}
    virtual void onFail( FailureMode ) = 0;
};

struct CallBacks
{
    CallBacks ( BlueCoreTransportCallBack& x , BlueCoreTransportFailure& y ) : c ( x ) , f ( y ) {}
    BlueCoreTransportCallBack& c;
    BlueCoreTransportFailure& f;
};

///////////////////////////////////////////////////////////////////////////////
//  Interface to use full BlueCore functionality - either through BCSP or H5
//  channels or tunneled through HCI manufacturer extensions.
///////////////////////////////////////////////////////////////////////////////
class BlueCoreTransportImplementation;
class BlueCoreTransport : public NoCopy
{
public:
    typedef CountedPointer<BlueCoreTransport> TransportPtr;
    BlueCoreTransport( const TransportConfiguration &config , const CallBacks &recvr );
    ~BlueCoreTransport();

    //  control the underlying transport
    bool start ();
    bool ready ( uint32 timeOut );
    void stop ();
    void sendAndWaitHCIReset();

    void setDataWatch ( DataWatcher * w );
    // look at the name.
    const char * getName() const;

    void sendpdu          ( const PDU& aPacket );
    void blockUntilSentPdu( const PDU& aPacket , uint32 timeout );

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  SleepControl  -  Control when the transport lets the BlueCore sleep
    //
    // FUNCTION
    //  This function controls when the tranpsort allows the BlueCore to 
    //  sleep.  For H4plus and H5 there is explicit out of band signalling
    //  that controls when the BlueCore sleeps.  There are three values that
    //  can be given for the sleep control parameter:
    //
    //  NEVER (default)	The BlueCore is never allowed to go to sleep.
    //  ALWAYS	        The BlueCore is set to sleep whenever possible.  It
    //                  is woken up if we need to talk to it, but then it 
    //                  is put back to sleep,
    //  NOW             The BlueCore is set to sleep now, but if we need to
    //                  talk to it again, we wake it up,  It then remains 
    //                  awake until further processing.
    //
    // RETURNS
    //  True if the sleep mode has been changed, false if the sleep control 
    //  functionis not supported on this transport, or there was a problem 
    //  with setting the mode.
    //
    bool SleepControl(enum SleepType);

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  PingPeriod  -  Control when the transport pings the BlueCore
    //
    // FUNCTION
    //  Some transports allow the host to ping the bluecore periodically to
    //	see if the BlueCore is still ok (H5 / H4+).  This function sets the
    //	delay between each ping.
    //
    // RETURNS
    //	True if the ping period was changed.  False if there was a problem,
    //	or this function is not supported on a given transport.
    //
    bool PingPeriod(int millisecs);
    bool set_sco_bandwidth(int bytes_per_second);

    bool reconfigure_uart_baudrate(uint32 baudrate);

private:
    BlueCoreTransportImplementation * mpimple;
};

#endif//_PHYS_TRANSPORTAPI_H_
