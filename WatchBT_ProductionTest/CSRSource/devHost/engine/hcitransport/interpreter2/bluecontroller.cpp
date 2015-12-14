////////////////////////////////////////////////////////////////////////////////
//
//  FILE   :    bluecontroller.cpp
//
//  PURPOSE:    code behind the API to the "new" HCI.DLL.
//              Pointer to implementation for BlueCoreDeviceController
//              and HCIDeviceController classes
//
//  CLASS  :    BlueCoreDeviceController, HCIDeviceController 
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32_WCE
#pragma warning(disable:4786)
#endif

#include "../bluecontroller.h"
#include "../interpreter2/implementation.h"
#include "../transportconfiguration.h"

//  Constructor
BlueCoreDeviceController::BlueCoreDeviceController ()
{
}

//  Destructor
BlueCoreDeviceController::~BlueCoreDeviceController ()
{
}

//  Transport related functions
bool BlueCoreDeviceController::isTransportReady ( uint32 timeout )
{
    return getImplementation()->isTransportReady( timeout );
}
bool BlueCoreDeviceController::startTransport ( void )
{
    return getImplementation()->startTransport();
}
bool BlueCoreDeviceController::forceReset ( uint32 timeout , ResetType type )
{
    return getImplementation()->forceReset ( timeout , type ) ;
}

void BlueCoreDeviceController::setDataWatch ( DataWatcher * w )
{
    getImplementation()->setDataWatch ( w );
}

//  However the threading is implemented, calling "doWork" gives
//  processor time to the object to allow it to send, receive etc.
//
//  doWork will return when it has done any pending work it can do.
bool BlueCoreDeviceController::doWork( void )
{
    return getImplementation()->doWork();
}

//  Wait for something to happen and then do it and wait some more...
//  createWorker will create a worker thread which calls workerThread
//  which calls waitForWork, which will return only when the object
//  is deleted, but it will do all background processing that would
//  have to be implemented by repeated calls to doWork otherwise.
void BlueCoreDeviceController::createWorker ( void )
{
    getImplementation()->createWorker();
}
//  or just give wait for work a thread...
void BlueCoreDeviceController::waitForWork( void )
{
    getImplementation()->waitForWork();
}
/////////////////////////////////////////////////////////////////////////
// NAME
//  SleepControl  -  Control when the transport lets the BlueCore sleep
//
bool BlueCoreDeviceController::SleepControl(enum SleepType type)
{
	return getImplementation()->SleepControl(type);
}

/////////////////////////////////////////////////////////////////////////
// NAME
//  PingPeriod  -  Control when the transport pings the BlueCore
//
bool BlueCoreDeviceController::PingPeriod(int millisecs)
{
	return getImplementation()->PingPeriod(millisecs);
}

bool BlueCoreDeviceController::reconfigure_uart_baudrate(uint32 baudrate)
{
    return getImplementation()->reconfigure_uart_baudrate(baudrate);
}

bool BlueCoreDeviceController::sendRawAny ( PDU::bc_channel channel , const uint8 * aBuffer , uint32 aLength , bool withFlowControl )
{
    return getImplementation()->sendRawAny ( channel , aBuffer , aLength , withFlowControl );
}

////////////////////////////////////////////////////////////////////////////////
//  Constructor
BlueCoreDeviceController_newStyle::BlueCoreDeviceController_newStyle
    ( const TransportConfiguration & aConfig ,
      CallBackProvider& aReceiver )
:
    implementation ( new Implementation ( aConfig , aReceiver ) )
{
}

//  Destructor
BlueCoreDeviceController_newStyle::~BlueCoreDeviceController_newStyle ()
{
    delete implementation;
}

bool BlueCoreDeviceController_newStyle::offerConnection ( const TransportConfiguration& aConfig )
{
    return implementation->offerConnection ( aConfig );
}

bool BlueCoreDeviceController_newStyle::send ( const sae& pdu , bool withFlowControl )
{
    return implementation->send ( pdu , withFlowControl );
}

bool BlueCoreDeviceController_newStyle::send ( const PDU& pdu , bool withFlowControl )
{
    return implementation->send ( sae(pdu,CallBackPtr(0)) , withFlowControl );
}

bool BlueCoreDeviceController_newStyle::purge_channel ( PDU::bc_channel c )
{
    return implementation->purge_channel(c);
}

bool BlueCoreDeviceController_newStyle::sendUp ( const PDU& pdu )
{
    return implementation->sendUp ( pdu );
}

//  additional call which returns the amount of data in bytes currently
//  awaiting transmission in the ACL queues.
size_t BlueCoreDeviceController_newStyle::getSizeOfDataWaitingOnHandle ( uint16 connectionHandle )
{
    return implementation->getSizeOfDataWaitingOnHandle 
                                           ( connectionHandle );
}

size_t BlueCoreDeviceController_newStyle::pendingItemCountOnRegulator()
{
    return implementation->pendingItemCountOnRegulator();
}

BlueCoreDeviceController::Implementation * BlueCoreDeviceController_newStyle::getImplementation() const
{
    return implementation;
}

void InterpretedCallBackProvider::onPDU ( const PDU& pdu )
{
    switch ( pdu.channel() )
    {
    case PDU::bccmd:
        onBCCMDResponse ( BCCMD_PDU(pdu) );
        break;
    case PDU::hq:
        onHQRequest ( HQ_PDU(pdu) );
        break;
    case PDU::dm:
//        onDMPrim ( DM_PDU(pdu) );
        break;
    case PDU::hciCommand:
        onHCIEvent ( HCIEventPDU(pdu) );
        break;
    case PDU::hciACL:
        onHCIACLData ( HCIACLPDU(pdu) );
        break;
    case PDU::hciSCO:
        onHCISCOData ( HCISCOPDU(pdu) );
        break;
    case PDU::l2cap:
//        onL2CAPPrim ( L2CAP_PDU(pdu) );
        break;
    case PDU::rfcomm:
//        onRFCOMMPrim ( RFCOMM_PDU(pdu) );
        break;
    case PDU::sdp:
//        onSDPPrim ( SDP_PDU(pdu) );
        break;
    case PDU::debug:
        onDebug ( pdu );
        break;
    case PDU::vm:
        onVMData ( VM_PDU ( pdu ) );
        break;
    default:
//        onUnknownPDU ( pdu );
        break;
    }
}


TCIphys::TCIphys ( const H4Configuration& c ,
          BlueCoreDeviceController_newStyle::CallBackProvider& p )
: implementation ( new BlueCoreDeviceController_newStyle::Implementation ( c , p ) )
{
    // HERE: Must reverse H4 flow direction.
}

TCIphys::~TCIphys ()
{
    delete implementation;
}
bool TCIphys::sendCommand ( const PDU& a )
{
    return implementation->send ( sae(a,BlueCoreDeviceController_newStyle::CallBackPtr(0)) , true );
}

bool TCIphys::sendEvent ( const PDU& a )
{
    return implementation->send ( sae(a,BlueCoreDeviceController_newStyle::CallBackPtr(0)) , true );
}

bool TCIphys::sendACL ( const PDU& a )
{
    return implementation->send ( sae(a,BlueCoreDeviceController_newStyle::CallBackPtr(0)) , true );
}

bool TCIphys::sendSCO ( const PDU& a )
{
    return implementation->send ( sae(a,BlueCoreDeviceController_newStyle::CallBackPtr(0)) , true );
}

BlueCoreDeviceController::Implementation * TCIphys::getImplementation() const
{
    return implementation;
}


