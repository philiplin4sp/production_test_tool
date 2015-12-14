///////////////////////////////////////////////////////////////////////
//
//  FILE   :  implementation.cpp
//
//  Copyright CSR 2000-2008
//
//  AUTHOR :  Adam Hughes
//
//  CLASS  :  BlueCoreDeviceController::Implementation
//
//  PURPOSE:  
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/interpreter2/implementation.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

#ifdef _WIN32_WCE
#pragma warning(disable:4786)
#endif

#include "implementation.h"
#include "app/bluestack/bluetooth.h"
#include "app/bluestack/hci.h"

static const cbp no_call_back(0);

static bool ignoresFlowControl ( const PDU& pdu )
{
    switch ( pdu.channel() )
    {
    case PDU::hciCommand :
        {
        HCICommandPDU cmd ( pdu.data() , pdu.size() );
        switch ( cmd.get_op_code() )
        {
		case HCI_ACCEPT_CONNECTION_REQ:
		case HCI_REJECT_CONNECTION_REQ:
        case HCI_LINK_KEY_REQ_REPLY:
        case HCI_LINK_KEY_REQ_NEG_REPLY:
        case HCI_PIN_CODE_REQ_REPLY:
        case HCI_PIN_CODE_REQ_NEG_REPLY:
		case HCI_ACCEPT_SYNCHRONOUS_CONN_REQ:
		case HCI_REJECT_SYNCHRONOUS_CONN_REQ:
		case HCI_IO_CAPABILITY_RESPONSE:
		case HCI_USER_CONFIRMATION_REQUEST_REPLY:
		case HCI_USER_CONFIRMATION_REQUEST_NEG_REPLY:
		case HCI_USER_PASSKEY_REQUEST_REPLY:
		case HCI_USER_PASSKEY_REQUEST_NEG_REPLY:
		case HCI_REMOTE_OOB_DATA_REQUEST_REPLY:
		case HCI_REMOTE_OOB_DATA_REQUEST_NEG_REPLY:
		case HCI_IO_CAPABILITY_REQUEST_NEG_REPLY:
        case HCI_RESET:
        case HCI_HOST_NUM_COMPLETED_PACKETS:
        case HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY:
        case HCI_ULP_LONG_TERM_KEY_REQUESTED_NEGATIVE_REPLY:
        case 0xFC00:
            return true;
            break;
        default:
            break;
        }
        break;
        }
    default:
        break;
    };
    return false;
}

BlueCoreDeviceController::Implementation::Implementation ()
:
    mBeingDeletedSignal ( mSignals ),
    mTransportReady ( false ),
    mThreadExitedSignal ( mThreadCloseDown ),
    mThread ( 0 )
{
    mThreadExitedSignal.set();
}

BlueCoreDeviceController::Implementation::~Implementation( )
{
    stopWork();
    delete mThread;
}

bool BlueCoreDeviceController::Implementation::startTransport()
{
    return getTransport()->start();
}

bool BlueCoreDeviceController::Implementation::isTransportReady( uint32 timeout )
{
    //  Are we connected at this instant?
    if ( !mTransportReady )
        mTransportReady = getTransport()->ready(timeout);

    return mTransportReady ;
}

void BlueCoreDeviceController::Implementation::setDataWatch ( DataWatcher * w )
{
    getTransport()->setDataWatch ( w );
}

void BlueCoreDeviceController::Implementation::createWorker ()
{
    if ( !mThread )
        mThread = new DeviceControllerImplementationThread ( this );
    if ( !mThread->IsActive() )
        (void)mThread->Start();
}

void BlueCoreDeviceController::Implementation::stopWork ()
{
    mBeingDeletedSignal.set();

    while ( mThreadExitedSignal.getState() == false )
        (void)mThreadCloseDown.wait();
}

void BlueCoreDeviceController::Implementation::waitForWork ()
{
    bool continuing = true;
    (void)mThreadExitedSignal.unset();

    while (continuing)
    {
        if ( mSignals.wait(1000) && !doWork() )
            continuing = false;
    }

    mThreadExitedSignal.set();
}

bool BlueCoreDeviceController::Implementation::doWork ()
{
    if ( mBeingDeletedSignal.getState() )
        return false;
    checkQueues();
    return true;
}

/////////////////////////////////////////////////////////////////////////
// NAME
//  SleepControl  -  Control when the transport lets the BlueCore sleep
//
bool BlueCoreDeviceController::Implementation::SleepControl(enum SleepType type)
{
    return getTransport()->SleepControl(type);
}

/////////////////////////////////////////////////////////////////////////
// NAME
//  PingPeriod  -  Control when the transport pings the BlueCore
//
bool BlueCoreDeviceController::Implementation::PingPeriod(int millisecs)
{
    return getTransport()->PingPeriod(millisecs);
}

bool BlueCoreDeviceController::Implementation::reconfigure_uart_baudrate(uint32 baudrate)
{
    return false;
}

/////////////////////////////////////////////////////////////////////////
//  NewStyle Implementation
/////////////////////////////////////////////////////////////////////////

BlueCoreDeviceController_newStyle::Implementation::Implementation
    ( const TransportConfiguration & aConfig,
      CallBackProvider& aReceiver )
:
    BlueCoreDeviceController::Implementation (),
    mReceiver ( aReceiver ),
    upstream ( mSignals ),
    downstream ( mSignals ),
    aclDataTracker(),
    initialised_ACL_flow_control ( false ),
    perm_pass_up ( 0 ),
    perm_no_pass ( 0 ),
    chip(aReceiver),
    offered (0),
    mConnectionToChip ( new BlueCoreTransport ( aConfig , CallBacks ( upstream , chip ) ) ),
    mConnectionOnOffer ( 0 )
{
    for ( int i = 0 ; i < PDU::csr_hci_extensions_count ; i++ )
    {
        if ( i == PDU::bccmd || i == PDU::hciCommand )
            pending[i] = new Regulator ( mSignals , 1 , 1000 , 1 );
        else if ( i == PDU::hciACL )
            pending[i] = new Regulator ( mSignals , 0 );
        else
            pending[i] = new Regulator ( mSignals );
    }
}

BlueCoreDeviceController_newStyle::Implementation::~Implementation( )
{
    //  stop the world, I want to get off.
    if ( mConnectionToChip )
        mConnectionToChip->stop();
    if ( mConnectionOnOffer )
        mConnectionOnOffer->stop();
    stopWork();

    //  now the world is stopped, delete it all...
    for ( int i = 0 ; i < PDU::csr_hci_extensions_count ; i++ )
        delete pending[i];
    delete offered;
    delete mConnectionToChip;
    delete mConnectionOnOffer;
}

bool BlueCoreDeviceController_newStyle::Implementation::offerConnection ( const TransportConfiguration& aConfig )
{
    if ( !mConnectionOnOffer )
    {
        offered = new TransportFail ( mReceiver );
        mConnectionOnOffer = new BlueCoreTransport ( aConfig , CallBacks ( downstream , *offered ) );
        if ( mConnectionOnOffer )
        {
            perm_pass_up = new pass_up ( *mConnectionOnOffer );
            return mConnectionOnOffer->start();
        }
    }
    return false;
}

bool BlueCoreDeviceController_newStyle::Implementation::sendRawAny ( PDU::bc_channel channel , const uint8 *aBuffer , uint32 aLength , bool withFlowControl )
{
    return send ( sae ( PDU ( channel , aBuffer , aLength ) , no_call_back ) , withFlowControl );
}

bool BlueCoreDeviceController_newStyle::Implementation::send ( const sae& pdu , bool withFlowControl )
{
    if ( pdu.packet.channel() == PDU::hciCommand && HCICommandPDU(pdu.packet).get_op_code() == HCI_ADD_SCO_CONNECTION)
    {
        scoBandwidthTracker.increase();
    }
    if ( pdu.packet.channel() < PDU::csr_hci_extensions_count )
    {
        if ( withFlowControl && !ignoresFlowControl( pdu.packet ) )
        {
            pending[pdu.packet.channel()]->add(pdu);
            if (pdu.packet.channel() == PDU::hciACL)
            {
                aclDataTracker.addData((PDU)pdu.packet);
            }
        }
        else
        {
            pending[pdu.packet.channel()]->overwrite_return(pdu.responseChannel);
            mConnectionToChip->sendpdu ( pdu.packet );
        }
        return true;
    }
    else
    {
        pending[PDU::other]->overwrite_return(pdu.responseChannel);
        mConnectionToChip->sendpdu ( pdu.packet );
        return true;
    }
}

bool BlueCoreDeviceController_newStyle::Implementation::purge_channel ( PDU::bc_channel channel )
{
    return pending[channel]->purge();
}

bool BlueCoreDeviceController_newStyle::Implementation::sendUp ( const PDU& pdu )
{
    if ( mConnectionOnOffer )
    {
        mConnectionOnOffer->sendpdu ( pdu );
        return true;
    }
    else
        return false;
}

void BlueCoreDeviceController_newStyle::Implementation::checkQueues ()
{
    PDU pdu(null_pdu);
    for ( int i = 0 ; i < PDU::csr_hci_extensions_count ; ++i )
    {
        for ( int j = 0 ; j < 8 && pending[i]->getNextIfAvailable(pdu) ; ++j )
        {
            mConnectionToChip->sendpdu ( pdu );
            if (pdu.channel() == PDU::hciACL)
                aclDataTracker.removeData(pdu);
        }
    }
    // deal with packets coming up...
    for ( int j = 0 ; j < 8 && upstream.get( pdu ) ; ++j )
    {
        PDU::bc_channel ch = pdu.channel();

        switch ( ch )
        {
        case PDU::bccmd:
            pending[PDU::bccmd]->setToken();
            break;
        case PDU::hq :
        {
            if ( HQ_PDU(pdu).get_req_type() == HQPDU_SETREQ )
            {
                HQ_PDU ret ( pdu.clone () );
                ret.set_req_type ( HQPDU_GETRESP );
                pending[PDU::hq]->add(sae(ret,no_call_back));
            }
            break;
        }
        case PDU::hciCommand:
        {
            HCIEventPDU ev ( pdu );
            switch ( ev.get_event_code() )
            {
            case HCI_EV_NUMBER_COMPLETED_PKTS:
            {
                HCI_EV_NUMBER_COMPLETED_PKTS_T_PDU ncp( pdu );
                uint8 count = ncp.get_num_handles();
                for ( uint8 i = 0 ; i < count ; ++i )
                {
                    uint16 handle;
                    uint16 tokens;
                    ncp.get_num_completed_pkts(i,handle,tokens);
                    pending[PDU::hciACL]->incToken(tokens);
                }
            }
                break;
            case HCI_EV_COMMAND_COMPLETE:
            {
                HCICommandCompletePDU cc (pdu);
                pending[PDU::hciCommand]->setToken(cc.get_num_hci_command_pkts() );
                switch(cc.get_op_code())
                {
                case HCI_READ_VOICE_SETTING:
                {
                    HCI_READ_VOICE_SETTING_RET_T_PDU rvs(pdu);
                    if ( rvs.get_status() == HCI_SUCCESS )
                    {
                        bool is_it_16 = (rvs.get_voice_setting()
                                       & HCI_VOICE_SAMP_SIZE_MASK)
                                      == HCI_VOICE_SAMP_SIZE_16BIT;
                        scoBandwidthTracker.set_audio_sample_size ( is_it_16 ? 16 : 8 );
                    }
                }
                    break;
                case HCI_READ_BUFFER_SIZE:
                    if ( !initialised_ACL_flow_control )
                    {
                        HCI_READ_BUFFER_SIZE_RET_T_PDU rbs ( pdu );
                        if ( rbs.get_status() == HCI_SUCCESS )
                        {
                            initialised_ACL_flow_control = true;
                            pending[PDU::hciACL]->setToken ( rbs.get_total_acl_data_pkts() );
                        }
                    }
                    break;
                }
            }
                break;
            case HCI_EV_COMMAND_STATUS:
            {
                HCI_EV_COMMAND_STATUS_T_PDU cc (pdu);
                pending[PDU::hciCommand]->setToken(cc.get_num_hci_command_pkts() );
                if (cc.get_op_code() == HCI_ADD_SCO_CONNECTION
                 && cc.get_status() != HCI_COMMAND_CURRENTLY_PENDING )
                {
                    getTransport()->set_sco_bandwidth(scoBandwidthTracker.decrease());
                }
            }
                break;
            case HCI_EV_CONN_COMPLETE:
            {
                HCI_EV_CONN_COMPLETE_T_PDU cc(pdu);
                if (cc.get_status() == HCI_SUCCESS)
                {
                    if (cc.get_link_type() == HCI_LINK_TYPE_ACL)
                        aclDataTracker.addConnection(cc.get_handle());
                    else
                    {
                        // sco or esco.  This will glitch if we were the initiating end.
                        getTransport()->set_sco_bandwidth(scoBandwidthTracker.consolidate(cc.get_handle()));
                    }
                }
                else
                {
                    if (cc.get_link_type() == HCI_LINK_TYPE_SCO)
                    {
                        getTransport()->set_sco_bandwidth(scoBandwidthTracker.decrease());
                    }
                }
            }
                break;
            case HCI_EV_DISCONNECT_COMPLETE:
            {
                HCI_EV_DISCONNECT_COMPLETE_T_PDU dc(pdu);
                if ( dc.get_status() == HCI_SUCCESS )
                {
                    uint16 h = dc.get_handle();
                    if (aclDataTracker.usingConnection(h))
                        aclDataTracker.removeConnection(h);
                    else
                    {
                        // sco or esco
                        getTransport()->set_sco_bandwidth(scoBandwidthTracker.decrease(h));
                    }
                }
            }
                break;
            case HCI_EV_SYNC_CONN_COMPLETE:
            {
                HCI_EV_SYNC_CONN_COMPLETE_T_PDU scc(pdu);
                if ( scc.get_status() == HCI_SUCCESS )
                {
                    // sco or esco.  This will glitch if we were the initiating end.
                    getTransport()->set_sco_bandwidth(scoBandwidthTracker.consolidate(scc.get_handle()));
                }
                else
                    scoBandwidthTracker.decrease();
            }
                break;
            case HCI_EV_LOOPBACK_COMMAND:
                // we just had a command loop back, so allow us to send another.
                pending[PDU::hciCommand]->setToken(1);
                break;
            default:
                break;
            }
            break;
        }
        default:
            break;
        }

        if ( !pending[ch]->run_callback(pdu) )
            mReceiver.onPDU ( pdu );
        //  some stuff should be passed up... HQ and HCI only?
        if ( mConnectionOnOffer )
        {
            switch ( ch )
            {
            case PDU::hq:
            case PDU::hciCommand:
            case PDU::hciACL:
            case PDU::hciSCO:
                (void)sendUp ( pdu );
                break;
            default:
                // do nothing
                break;
            }
        }
    }
    //  accept requests from higher layers on all channels.
    for ( int k = 0 ; k < 8 && downstream.get( pdu ) ; ++k )
    {
        switch ( pdu.channel() )
        {
        case PDU::hciCommand:
        case PDU::hciACL:
        case PDU::hciSCO:
            (void)send ( sae ( pdu , perm_no_pass ) , true );
            break;
        default:
            (void)send ( sae ( pdu , perm_pass_up ) , true );
            break;
        }
    }
}

bool BlueCoreDeviceController_newStyle::Implementation::forceReset ( uint32 timeout , ResetType type )
{
    bool ok = true;
    switch ( type )
    {
    case hci:
        {
        HCI_RESET_T_PDU reset;
        mConnectionToChip->blockUntilSentPdu( reset , timeout );
        }
        break;
    case bccmd_cold:
        {
        BCCMD_COLD_RESET_PDU reset;
        reset.set_req_type ( BCCMDPDU_SETREQ );
        reset.set_seq_no ( 0xE4D );
        mConnectionToChip->blockUntilSentPdu( reset , timeout );
        }
        break;
    case bccmd_warm:
        {
        BCCMD_WARM_RESET_PDU reset;
        reset.set_req_type ( BCCMDPDU_SETREQ );
        reset.set_seq_no ( 0xE4D );
        mConnectionToChip->blockUntilSentPdu( reset , timeout );
        }
        break;
    default:
        ok = false;
        break;
    };
    return ok;
}

size_t BlueCoreDeviceController_newStyle::Implementation::getSizeOfDataWaitingOnHandle ( uint16 connectionHandle )
{
    return aclDataTracker.getDataWaiting(connectionHandle);
}

size_t BlueCoreDeviceController_newStyle::Implementation::pendingItemCountOnRegulator()
{
    // check regulator (flow controller) item size
    size_t totalSize = 0;

    for (uint16 i = 0; i < PDU::csr_hci_extensions_count ; i++)
    {
        totalSize += pending[i]->inListCount();
    }

    return totalSize;
}


bool BlueCoreDeviceController_newStyle::Implementation::reconfigure_uart_baudrate(uint32 baudrate)
{
    return getTransport()->reconfigure_uart_baudrate(baudrate);
}

//////////////////////////////////////////////////////////////////////////////////////
// AclDataTracker : tracks ACL data in queues
// 
// Construction/Destruction
AclDataTracker::AclDataTracker()
{
}

AclDataTracker::~AclDataTracker()
{
    //  Lock critical section for duration of this scope
    CriticalSection::Lock here(protect_list);
    //  erase map under Critical section,
    //  before calling destructor (outside CS's scope)
    connections.clear();
}

// Add/remove data
void AclDataTracker::addData(const PDU& pdu)
{
    // Lock critical section for duration of this scope
    CriticalSection::Lock here(protect_list);
    
    // cast pdu to an hciaclpdu - be safer here
    HCIACLPDU acl_pdu = (HCIACLPDU)pdu;
    
    // find the appropriate connection
    if ( connections.find(acl_pdu.get_handle()) != connections.end() )
    // add data to this connection (if present)
        connections[acl_pdu.get_handle()] += acl_pdu.get_length();
}

void AclDataTracker::removeData(PDU& pdu)
{
    // Lock critical section for duration of this scope
    CriticalSection::Lock here(protect_list);
    // cast pdu to an hciaclpdu - be safer here
    HCIACLPDU acl_pdu = (HCIACLPDU)pdu;

    // find the appropriate connection
    if ( connections.find(acl_pdu.get_handle()) != connections.end() )
        // remove data from this connection (if present)
        connections[acl_pdu.get_handle()] -= acl_pdu.get_length();
}

// get data waiting on guiven handle
size_t AclDataTracker::getDataWaiting(uint16 handle)
{
    // Lock critical section for duration of this scope
    CriticalSection::Lock here(protect_list);
    // find the appropriate connection
    if ( connections.find(handle) != connections.end() )
        // return data from this connection (if present)
        return connections[handle];
    else
        return 0;
}

//  not really necessary with a map, but we may want to
//  change the underlying impl.
void AclDataTracker::addConnection(uint16 handle)
{
    // Lock critical section for duration of this scope
    CriticalSection::Lock here(protect_list);
    // don't add a connection that is already there
    if ( connections.find(handle) == connections.end() )
        connections[handle] = 0;
}
void AclDataTracker::removeConnection(uint16 handle)
{
    // Lock critical section for duration of this scope
    CriticalSection::Lock here(protect_list);
    // don't remove a connection that is not there
    if ( connections.find(handle) != connections.end() )
        connections.erase(connections.find(handle));
}

// is the handle being tracked?
bool AclDataTracker::usingConnection(uint16 handle)
{
    return (connections.find(handle) != connections.end());
}
