///////////////////////////////////////////////////////////////////////
//
//  FILE   :  inplementation.h
//
//  Copyright CSR 2000-2008
//
//  AUTHOR :  Adam Hughes
//
//  CLASS  :  BlueCoreDeviceController::Implementation
//
//  PURPOSE:  
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/interpreter2/implementation.h#1 $
//
///////////////////////////////////////////////////////////////////////

#include "../bluecontroller.h"
#include "../phys/transportapi.h"
#include "thread/critical_section.h"
#include "thread/thread.h"
#include "thread/signal_box.h"
#include <deque>
#include <list>
#include <map>

typedef BlueCoreDeviceController_newStyle::sae sae;
typedef BlueCoreDeviceController_newStyle::CallBackPtr cbp;

class BlueCoreDeviceController::Implementation
{
public:
    //  Constructor.
    //  A Transport given to the controller at this stage
    //  will be deleted by the controller in its destuctor.
    Implementation ();

    //  Destructor
    virtual ~Implementation () = 0;
 
    ///////////////////////////////////////////////////////////////////////
    //  Transport Control.
    ///////////////////////////////////////////////////////////////////////
    bool startTransport ();
    bool isTransportReady ( uint32 timeout );

    //  Force a reset through the stack, and only return when
    //  we are sure it has escaped the host, or the timeout lapses.
    virtual bool forceReset ( uint32 timeout , ResetType type ) = 0; // milliseconds.


    void setDataWatch ( DataWatcher * w );

    //  However the threading is implemented, calling "doWork" gives
    //  processor time to the object to allow it to send, receive etc.
    //
    //  doWork will return true when it has done any pending work it can do,
    //  false when the object is being deleted...
    bool doWork ();

    //  Wait for something to happen and then do it and wait some more...
    //  createWorker will create a worker thread which calls doWork()
    //  which calls waitForWork, which will return only when stopWork()
    //  is called, but it will do all background processing that would
    //  have to be implemented by repeated calls to doWork otherwise.
    void createWorker ();
    void stopWork ();
    //  or just give wait for work a thread...
    void waitForWork ();

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  SleepControl  -  Control when the transport lets the BlueCore sleep
    //
    bool SleepControl(enum SleepType);

    /////////////////////////////////////////////////////////////////////////
    // NAME
    //  PingPeriod  -  Control when the transport pings the BlueCore
    //
    bool PingPeriod(int millisecs);

    virtual bool sendRawAny ( PDU::bc_channel channel,
                              const uint8 * aBuffer,
                              uint32 aLength,
                              bool withFlowControl ) = 0;

    virtual bool reconfigure_uart_baudrate(uint32 baudrate);

protected:
    SignalBox mSignals ;
    SignalBox::Item mBeingDeletedSignal ;

private:
    bool mTransportReady;
    virtual BlueCoreTransport * getTransport() = 0;

    SignalBox mThreadCloseDown ;
    SignalBox::Item mThreadExitedSignal ;
    virtual void checkQueues() = 0;

    //  Thread object
    class DeviceControllerImplementationThread : public Threadable
    {
    public:
        DeviceControllerImplementationThread ( Implementation * aParent )
            : mParent ( aParent ) {}
        int ThreadFunc () { mParent->waitForWork(); return 0; }
        virtual ~DeviceControllerImplementationThread () {Kill();}
    private:
        Implementation * mParent;
    } * mThread;
};

// tracks ACl data in queues
class AclDataTracker
{
public:
    // Construction/Destruction
    AclDataTracker();
    ~AclDataTracker();

    // Add/remove data
    void addData(const PDU& pdu);
    void removeData(PDU& pdu);

    // get data waiting on guiven handle
    size_t getDataWaiting(uint16 handle);
    
    // add.remove connections
    void addConnection(uint16 handle);
    void removeConnection(uint16 handle);
    bool usingConnection(uint16 handle);
    
private:
    CriticalSection protect_list;
    std::map<uint16,size_t> connections;
};

#ifdef _DEBUG
#define BANDWIDTH_DEBUG(a) printf( a " BD: r:%d, t:%d\n",requested_bandwidth,total_bandwidth)
#else
#define BANDWIDTH_DEBUG(a)
#endif

class ScoBandwidthTracker
{
public:
    ScoBandwidthTracker()
    : audio_sample_size(16), requested_bandwidth(0), total_bandwidth(0)
    {}
    void set_audio_sample_size(uint32 val) {audio_sample_size = val;}
    uint32 increase()
    {
        requested_bandwidth += 1000 * audio_sample_size;
        BANDWIDTH_DEBUG("I");
        return requested_bandwidth;
    }
    uint32 consolidate(uint16 handle)
    {
        uint32 potential_bandwidth = 1000 * audio_sample_size;
        bandwidth_per_connection[handle] = potential_bandwidth;
        if ( total_bandwidth == requested_bandwidth ) // the other side initiated the connection so didn't know about it till now.
            requested_bandwidth += potential_bandwidth;
        total_bandwidth += potential_bandwidth;
        BANDWIDTH_DEBUG("C");
        return total_bandwidth;
    }
    uint32 decrease(uint16 handle = 0)
    {
        if ( handle )
        {
            //  this was consolidated once.
            total_bandwidth -= bandwidth_per_connection[handle];
            requested_bandwidth -= bandwidth_per_connection[handle];
            bandwidth_per_connection.erase(handle);
        }
        else
        {
            requested_bandwidth -= 1000 * audio_sample_size;
        }
        BANDWIDTH_DEBUG("D");
        return total_bandwidth;
    }
private:
    uint32 audio_sample_size;
    std::map<uint16,uint32> bandwidth_per_connection;
    uint32 requested_bandwidth;
    uint32 total_bandwidth;
};

//  controls the flow
class Regulator
{
public:
    enum special_timeout { never = 0 };
    enum special_TokenValue { infinite = -10 };
    Regulator ( SignalBox& sb ,
                int        initialTokens   = infinite ,
                uint32     timeout         = never ,
                int        tokensOnTimeout = 1 )
    :   tokens ( initialTokens ),
        waitFor ( timeout ),
        payback ( tokensOnTimeout ),
        inList ( 0 ),
        active ( sb )
    {}
    bool getNextIfAvailable ( PDU& pdu )
    {
        bool given = false;
        CriticalSection::Lock here ( protection );
        if ( inList && tokens )
        {
            sae next = list.front();
            list.pop_front();
            given = true;
            inList--;
            if ( tokens > 0 )
                tokens--;
            if ( !tokens || !inList )
                (void)active.unset();
            pdu = next.packet;
            ReturnRule = next.responseChannel;
        }
        return given;
    }
    void add ( const sae& pdu )
    {
        CriticalSection::Lock here ( protection );
        list.push_back ( pdu );
        inList ++;
        if ( tokens )
            active.set();
    }
    void incToken ( int tokenCount = 1 )
    {
        CriticalSection::Lock here ( protection );
        tokens += tokenCount;
        if ( tokens && inList )
            active.set();
    }
    void setToken ( int tokenCount = 1 )
    {
        CriticalSection::Lock here ( protection );
        tokens = tokenCount;
        if ( tokens && inList )
            active.set();
    }
    void overwrite_return ( const cbp& rule )
    {
        CriticalSection::Lock here ( protection );
        ReturnRule = rule;
    }
    bool purge()
    {
        CriticalSection::Lock here ( protection );
        bool rv = !list.empty();
        list.clear();
        inList = 0;
        (void)active.unset();
        ReturnRule = 0;
        return rv;
    }
    bool run_callback(PDU&p)
    {
        CriticalSection::Lock here ( protection );
        if (ReturnRule)
        {
            (*ReturnRule)(p);
            ReturnRule = 0;
            return true;
        }
        return false;
    }
    uint32 inListCount()
    {
        CriticalSection::Lock here ( protection );
        return inList;
    }
private:
    int tokens;
    uint32 waitFor;
    int payback;
    uint32 inList;
    std::deque<sae> list;
    CriticalSection protection;
    SignalBox::Item active;
    cbp ReturnRule;
};

//  deals with upstreams.
class Collector : public BlueCoreTransportCallBack
{
public:
    Collector ( SignalBox& sb ) : inList (0) , active ( sb ) {}
    void onPDU ( PDU::bc_channel channel , const uint8 * buffer , size_t length )
    {
        CriticalSection::Lock here ( protection );
        list.push_back(PDU(channel,buffer,length));
        ++inList;
        active.set();
    }
    bool get ( PDU& pdu )
    {
        CriticalSection::Lock here ( protection );
        if ( inList )
        {
            pdu = list.front();
            list.pop_front();
            if ( !(--inList) )
                (void)active.unset();
            return true;
        }
        else
            return false;
    }
    size_t queueSize() {return inList;}

private:
    size_t inList;
    std::deque<PDU> list;
    CriticalSection protection;
    SignalBox::Item active;
};

class pass_up : public BlueCoreDeviceController_newStyle::PDUCallBack
{
public:
    pass_up ( BlueCoreTransport& where ) : mT ( where ) {}
    void operator () ( const PDU& a )
    {
        mT.sendpdu(a);
    }
private:
    BlueCoreTransport& mT;
};


class BlueCoreDeviceController_newStyle::Implementation : public BlueCoreDeviceController::Implementation
{
public:
    //  Constructor.
    Implementation ( const TransportConfiguration& aConfig ,
                     CallBackProvider& aReceiver );

    //  Destructor
    virtual ~Implementation ();

    bool offerConnection ( const TransportConfiguration& aConfig );

    bool send ( const sae& s , bool withFlowControl );
    //  Raw send to any channel
    bool sendRawAny ( PDU::bc_channel channel , const uint8 * aBuffer ,
                      uint32 aLength , bool withFlowControl );

    //  purge all unsent prims and callbacks from a channel.
    //  returns false if channel was empty.
    bool purge_channel ( PDU::bc_channel c );
    //  Send to offered connection
    bool sendUp ( const PDU& s );

    bool forceReset ( uint32 timeout , ResetType type ); // milliseconds.

    void checkQueues ();

    size_t getSizeOfDataWaitingOnHandle ( uint16 connectionHandle );

    bool reconfigure_uart_baudrate(uint32 baudrate);

    size_t pendingItemCountOnRegulator();

private:
    CallBackProvider& mReceiver;
    //  queues.
    Collector upstream;
    Collector downstream;
    Regulator * pending[PDU::csr_hci_extensions_count];
    AclDataTracker aclDataTracker;
    ScoBandwidthTracker scoBandwidthTracker;
    bool initialised_ACL_flow_control;
    cbp perm_pass_up;
    cbp perm_no_pass;
    class TransportFail : public BlueCoreTransportFailure
    {
    public:
        TransportFail ( BlueCoreDeviceController_newStyle::CallBackProvider& x )
        : r(x) {}
        void onFail( FailureMode f )
        { r.onTransportFailure(f); }
    private:
        BlueCoreDeviceController_newStyle::CallBackProvider& r;
    };
    TransportFail chip;
    TransportFail* offered;

    BlueCoreTransport * mConnectionToChip;
    BlueCoreTransport * mConnectionOnOffer;
    BlueCoreTransport * getTransport() { return mConnectionToChip; }
};

