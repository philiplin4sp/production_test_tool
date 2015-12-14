//////////////////////////////////////////////////////////////////////////////
//
//  FILE   :    bluecontroller.h
//
//  Copyright CSR 2001-2008
//
//  PURPOSE:    Provide API to the "new" HCI.DLL.
//              Pointer to implementation for BlueCoreDeviceController
//
//  CLASS  :    BlueCoreDeviceController
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __BLUECONTROLLER_H__
#define __BLUECONTROLLER_H__

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <stdlib.h> // size_t
#include "csrhci/bluecontroller_types.h"
#include "hcipacker/hcipacker.h"
#include "thread/safe_counter.h"
#include "csrhci/transportapi.h"

class TransportConfiguration;

#ifndef DATAWATCHER
#define DATAWATCHER
class DataWatcher
{
public:
    virtual void onSend ( const uint8* , size_t ) = 0;
    virtual void onRecv ( const uint8* , size_t ) = 0;
    virtual ~DataWatcher(){}
    virtual void operator= (const DataWatcher&){}
    // no need for a copy contructor
};
#endif

class BlueCoreDeviceController
{
public:
    class Implementation;
    //  Destructor
    virtual ~BlueCoreDeviceController ();

    //  Transport related functions
    bool isTransportReady( uint32 timeout = 0 );
    bool startTransport( void );
    enum ResetType { hci , bccmd_cold , bccmd_warm };
    bool forceReset ( uint32 timeout , ResetType type = bccmd_cold ) ; // millisec

    void setDataWatch ( DataWatcher * w );

    //  However the threading is implemented, calling "doWork" gives
    //  processor time to the object to allow it to send, receive etc.
    //
    //  doWork will return when it has done any pending work it can do.
    //  false means the call was made while the object was being deleted.
    bool doWork( void );

    //  Wait for something to happen and then do it and wait some more...
    //  createWorker will create a worker thread which calls workerThread
    //  which calls waitForWork, which will return only when the object
    //  is deleted, but it will do all background processing that would
    //  have to be implemented by repeated calls to doWork otherwise.
    void createWorker ( void );
    //  or just give wait for work a thread...
    void waitForWork( void );

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
    //  NEVER (default) The BlueCore is never allowed to go to sleep.
    //  ALWAYS          The BlueCore is set to sleep whenever possible.  It
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
    //  see if the BlueCore is still ok (H5 / H4+).  This function sets the
    //  delay between each ping.
    //
    // RETURNS
    //  True if the ping period was changed.  False if there was a problem,
    //  or this function is not supported on a given transport.
    //
    bool PingPeriod(int millisecs);

    //  Send a command on any channel.
    bool sendRawAny ( PDU::bc_channel channel ,
                      const uint8 * aBuffer ,
                      uint32 aLength ,
                      bool withFlowControl = true );

    bool reconfigure_uart_baudrate(uint32 baudrate);

protected:
    //  Constructor
    BlueCoreDeviceController ();

private:
    virtual Implementation * getImplementation() const = 0;
    BlueCoreDeviceController ( const BlueCoreDeviceController& );
    BlueCoreDeviceController& operator= ( const BlueCoreDeviceController& );
};

///////////////////////////////////////////////////////////////////////////////

class BlueCoreDeviceController_newStyle : public BlueCoreDeviceController
{
public:
    ////////////////////////////////////////////////////////////////////////
    //  Helper Classes
    ////////////////////////////////////////////////////////////////////////

    //  per PDU call backs
    class PDUCallBack : public SafeCounter
    {
    public:
        virtual void operator() ( const PDU & ) = 0;
    };
    typedef CountedPointer<PDUCallBack> CallBackPtr;

    class sae // self-addressed-envelope
    {
    public:
        sae ( const PDU & p , const CallBackPtr& c )
        : packet ( p ) , responseChannel ( c ) {}
        PDU packet;
        CallBackPtr responseChannel;
    };

    //  all PDUs from the chip which don't correspond to a sent
    //  PDU go through here.
    class CallBackProvider
    {
    public:
        virtual void onPDU ( const PDU& ) = 0;

        //  and "extra" callbacks.
        virtual void onTransportFailure (FailureMode) = 0;
    };

    ////////////////////////////////////////////////////////////////////////
    //  Public Methods
    ////////////////////////////////////////////////////////////////////////
    //  Constructor
    BlueCoreDeviceController_newStyle (
        const TransportConfiguration & aConfig ,
        CallBackProvider& aReceiver );
    //  Destructor
    ~BlueCoreDeviceController_newStyle ();

    bool offerConnection ( const TransportConfiguration& aConfig );

    bool send ( const sae& , bool withFlowControl = true );
    //  defaults a sae which sends the pdu through the CallBackProvider
    bool send ( const PDU& , bool withFlowControl = true );
    bool purge_channel ( PDU::bc_channel c );

    //  Send to offered connection
    bool sendUp ( const PDU& s );

    uint16 getMaxACLPacketSize();
    
	//  additional call which returns the amount of data in bytes currently
    //  awaiting transmission in the ACL queues.
    size_t getSizeOfDataWaitingOnHandle ( uint16 connectionHandle );

    size_t pendingItemCountOnRegulator();

    ////////////////////////////////////////////////////////////////////////
    //  Private(ish) stuff
    ////////////////////////////////////////////////////////////////////////
    class Implementation;
private:
    Implementation * implementation;
    BlueCoreDeviceController::Implementation * getImplementation() const;
};

class InterpretedCallBackProvider : public BlueCoreDeviceController_newStyle::CallBackProvider
{
public:
    void onPDU ( const PDU& );
private:
    virtual void onBCCMDResponse ( const BCCMD_PDU& ) = 0;
    virtual void onHQRequest     ( const HQ_PDU& ) = 0;
    virtual void onHCIEvent      ( const HCIEventPDU& ) = 0;
    virtual void onHCIACLData    ( const HCIACLPDU& ) = 0;
    virtual void onHCISCOData    ( const HCISCOPDU& ) = 0;
//    virtual void onDMPrim        ( const DM_PDU& ) = 0;
//    virtual void onSDPPrim       ( const SDP_PDU& ) = 0;
//    virtual void onL2CAPPrim     ( const L2CAP_PDU& ) = 0;
//    virtual void onRFCOMMPrim    ( const RFCOMM_PDU& ) = 0;
    virtual void onDebug         ( const PDU& ) = 0;
    virtual void onVMData        ( const VM_PDU& ) = 0;
//    virtual void onUnknownPDU    ( const PDU& ) = 0;
};

///////////////////////////////////////////////////////////////////////////
//  TCIphys
//  Transport for TCI (upside down h4).
///////////////////////////////////////////////////////////////////////////

class H4Configuration;

class TCIphys : public BlueCoreDeviceController
{
    // implements upside down H4.
public:
    TCIphys ( const H4Configuration& c ,
              BlueCoreDeviceController_newStyle::CallBackProvider& p );
    ~TCIphys ();
    bool sendCommand ( const PDU& );
    bool sendEvent ( const PDU& );
    bool sendACL ( const PDU& );
    bool sendSCO ( const PDU& );
private:
    BlueCoreDeviceController::Implementation * getImplementation() const;
    BlueCoreDeviceController_newStyle::Implementation * implementation;
};

#endif
