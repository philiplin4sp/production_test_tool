///////////////////////////////////////////////////////////////////////
//
//  FILE   :  BCSPTransport.cpp
//
//  Copyright CSR 2000-2006
//
//  AUTHOR :  Adam Hughes
//
//  CLASS  :  BCSPTransport
//
//  PURPOSE:  transparent class inheriting top API from the
//            HCITransport class, and owning a BCSPStack which does
//            all the work.
//
//            This Class also provides BCSP-specific private channels
//            L2CAP etc and puts the debug channel messages to a file
//            in the debug build version.
//
//            Start-up sequence:
//            new BCSPTransport;
//            setupCallBacks (HCITransport functions);
//            start stack;
//            
//            If the stack is started with out the callbacks in place,
//            then data arriving in the intervening time will be lost.
//            Since this is likely to be the HCI initialisation packet,
//            that would be bad...
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/phys/bcsptransport.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

#include <cassert>
#include "bcsptransport.h"
#include "bcspimplementation.h"

class BCSPCallBackInfo
{
public:
    BCSPCallBackInfo ( PDU::bc_channel channel ) : packet ( channel ) {}
    BCSPCallBackInfo ( const PDU & pdu ) : packet ( pdu ) {}
    BCTransferRequest req;
    BCSPTransport * object;
    PDU packet;
    SignalBox::Item * sentPdu;
};

BCSPTransport::BCSPTransport( const BCSPConfiguration &aConfig , UARTPtr comPort , const CallBacks& aRecv )
:
    BlueCoreTransportImplementation ( "BCSPTransport" , aConfig.useCSRExtensions() , aRecv ), // tunnelling
    mBufferSize (4096),
    mTunnelOver ( aConfig.getTunnelOver() ),
    mTunnelBelow ( aConfig.getTunnelBelow() ),
    mStack ( comPort )
{
    assert ( mTunnelBelow >= PDU::zero );
    assert ( mTunnelOver  <= PDU::csr_hci_extensions_count );
    //BTRACELOG ( "E:\\Work\\bcsplog.txt" );
    BTRACEDISABLE ( ( ( 0xFFFFFFFF ) ) );//^ IO ) ^ LINKEST ) ^ USER0 );

    mStackConfiguration.resendTimeout = aConfig.getResendTimeout();
    mStackConfiguration.retryLimit = aConfig.getRetryLimit();
    mStackConfiguration.TShy = aConfig.getTShy();
    mStackConfiguration.TConf = aConfig.getTConf();
    mStackConfiguration.confLimit = aConfig.getConfLimit();
    mStackConfiguration.windowSize = aConfig.getWindowSize();
    mStackConfiguration.useCRC = aConfig.getUseCRC();
    mStackConfiguration.useLinkEstablishmentProtocol = aConfig.getUseLinkEstablishmentProtocol();

    BCSPStack * stack = mStack.getBCSPStack() ;
    ::setStackConfiguration ( stack, &mStackConfiguration) ;
    mStack.setOnLinkFailSync(onSyncPacket,(void*) this) ;
    mStack.setOnLinkFailRetries(onRetriesMax,(void*) this) ;
    mStack.setOnLinkFailUart(onUartDied,(void*) this) ;
    //  Set up all the channels to receive.
    for (uint8 i = 0 ; i < PDU::bcsp_channel_count ; i ++ )
    {
        buffers[i] = new uint8[mBufferSize];
        if ( i == PDU::hciSCO )
            BCSPopenChannel ( &channels[i] , stack , i , BCSPChannelUnreliable , BCSPCallbackDeferred ) ;
        else
            BCSPopenChannel ( &channels[i] , stack , i , BCSPChannelReliable , BCSPCallbackDeferred ) ;
        BCSPCallBackInfo *info = new BCSPCallBackInfo ( PDU::bc_channel(i) );
        info->object = this;
        BCinitTransferRequest( &(info->req), buffers[i], mBufferSize, onPacketFromChip , info );
        BCSPreadPacket( &channels[i] , &(info->req) ) ;
    }
}

BCSPTransport::~BCSPTransport()
{
    // don't call stop - the deletion of the stack does that.
}

bool BCSPTransport::start()
{
    //set up bcsp stack and callbacks on all channels
    return mStack.Start() ;
}

void BCSPTransport::stop()
{
    // deleting the stack causes cancelled requests on all the channels.
    mStack.Stop();
}

bool BCSPTransport::ready ( uint32 timeout )
{
    //  if the link has failed return false
    return ( mStack.connect( timeout ) );
}

void BCSPTransport::setDataWatch ( DataWatcher * w )
{
    mStack.setDataWatch ( w );
}

void BCSPTransport::onSyncPacket( void * p )
{
    ((BCSPTransport*)p)->onLinkFail ( bcsp_sync_recvd );
}

void BCSPTransport::onRetriesMax( void * p )
{
    ((BCSPTransport*)p)->onLinkFail ( bcsp_retry_limit );
}

void BCSPTransport::onUartDied( void * p )
{
    ((BCSPTransport*)p)->onLinkFail ( uart_failure );
}

void BCSPTransport::getStackConfiguration(StackConfiguration &cfg)
{
    cfg = mStackConfiguration;
}

void BCSPTransport::setStackConfiguration(const StackConfiguration &cfg)
{
    mStackConfiguration = cfg;
}

void BCSPTransport::sendpdu ( const PDU& aPacket )
{
    if ( aPacket.channel() < mTunnelOver && aPacket.channel() > mTunnelBelow )
    {
        assert ( "pdu bigger than uint16" && aPacket.size() < 65536 );
        BCTransferRequest *req = new BCTransferRequest;
        BCSPCallBackInfo *info = new BCSPCallBackInfo ( aPacket );
        info->object = 0;
        info->sentPdu = &sentPdu;
        BCinitTransferRequest( req , (uint8*)aPacket.data() , (uint16)aPacket.size() , onPacketToChip , info );
        BCSPwritePacket ( &channels[aPacket.channel()], req );
    }
    else
    {
        if ( tunnel )
            sendTunnel ( aPacket );
        else
            assert ( "Can't send a tunnelled packet when not tunnelling." );
    }
}

void BCSPTransport::onPacketToChip ( BCTransferRequest * req, BCTransferStatus status, void * info )
{
    BCSPCallBackInfo* lInfo = (BCSPCallBackInfo*)info;
    assert ( lInfo->object == 0 );
    delete req;
    lInfo->sentPdu->set();
    delete lInfo;
}

void BCSPTransport::onPacketFromChip (BCTransferRequest *req , BCTransferStatus status, void *info)
{
    BCSPCallBackInfo* lInfo = (BCSPCallBackInfo*)info;
    assert ( lInfo->object );
    switch (status)
    {
    case transferCancelled:
        // delete the buffer
        delete[] req->data;
        delete lInfo;
        break;
    case transferBufferTooSmall:
        assert ( 0 );
        break;
    default:
        lInfo->object->processDataFromChip( req , lInfo->packet.channel() );
        break;
    }
    return;
}

void BCSPTransport::processDataFromChip ( BCTransferRequest * req, PDU::bc_channel ch )
{
    //  collect the data.
    //  expose the data received
    recvData (ch, req->data, req->length);
    //  reset the transfer request to the right size.
    BCreloadTransferRequest( req , req->data , mBufferSize) ;
    BCSPreadPacket( &channels[ch] , req );
}

///////////////////////////////////////////////////////////////////////////////
//  Transport configuration objects
///////////////////////////////////////////////////////////////////////////////

BCSPConfiguration::BCSPConfiguration ( const UARTConfiguration & aUartConfig ,
                                       bool aUseCRC ,
                                       bool aTunnel ,
                                       PDU::bc_channel aTunnel_over ,
                                       PDU::bc_channel aTunnel_below )
:
    TransportConfiguration ( aTunnel ),
    tunnel_below ( aTunnel_below ),
    tunnel_over ( aTunnel_over ),
    // getStackConfiguration returns a pointer to a const.
    mStackConfiguration ( new StackConfiguration(*(::getDefaultStackConfiguration())) ),
    mUartConfiguration ( aUartConfig )
{
    mStackConfiguration->useCRC = aUseCRC;
}

BCSPConfiguration::BCSPConfiguration ( const char * aDeviceName , int BaudRate ,
                                       bool aUseCRC ,
                                       bool aTunnel ,
                                       PDU::bc_channel aTunnel_over ,
                                       PDU::bc_channel aTunnel_below )
:
    TransportConfiguration ( aTunnel ),
    tunnel_below ( aTunnel_below ),
    tunnel_over ( aTunnel_over ),
    // getStackConfiguration returns a pointer to a const.
    mStackConfiguration ( new StackConfiguration(*(::getDefaultStackConfiguration())) ),
    mUartConfiguration ( aDeviceName , BaudRate , BCSP_parity , BCSP_stopbits , BCSP_flowcontrolon , BCSP_readtimeout )
{
    mStackConfiguration->useCRC = aUseCRC;
}

BCSPConfiguration::BCSPConfiguration ( const wchar_t * aDeviceName , int BaudRate ,
                                       bool aUseCRC ,
                                       bool aTunnel ,
                                       PDU::bc_channel aTunnel_over ,
                                       PDU::bc_channel aTunnel_below )
:
    TransportConfiguration ( aTunnel ),
    tunnel_below ( aTunnel_below ),
    tunnel_over ( aTunnel_over ),
    // getStackConfiguration returns a pointer to a const.
    mStackConfiguration ( new StackConfiguration (*(::getDefaultStackConfiguration())) ),
    mUartConfiguration ( aDeviceName , BaudRate , BCSP_parity , BCSP_stopbits , BCSP_flowcontrolon , BCSP_readtimeout )
{
    mStackConfiguration->useCRC = aUseCRC;
}

BCSPConfiguration::BCSPConfiguration ( const BCSPConfiguration& from )
:
    TransportConfiguration ( from ),
    tunnel_below ( from.tunnel_below ),
    tunnel_over ( from.tunnel_over ),
    mStackConfiguration ( new StackConfiguration ( *from.mStackConfiguration ) ),
    mUartConfiguration ( from.mUartConfiguration )
{
}

BCSPConfiguration::~BCSPConfiguration ()
{
    delete mStackConfiguration;
}

TransportConfiguration * BCSPConfiguration::clone () const
{
    return new BCSPConfiguration ( *this );
}

BlueCoreTransportImplementation * BCSPConfiguration::make ( const CallBacks& aRecv ) const
{
    return new BCSPTransport ( *this , UARTPtr ( mUartConfiguration.make() ) , aRecv );
}

void BCSPConfiguration::setConfig ( uint32 aResendTimeout , uint32 aRetryLimit , uint32 aTShy , uint32 aTConf , uint16 aConfLimit , uint8 aWindowSize , bool aUseLinkEstablishmentProtocol )
{
    mStackConfiguration->resendTimeout = aResendTimeout;
    mStackConfiguration->retryLimit = aRetryLimit;
    mStackConfiguration->TShy = aTShy;
    mStackConfiguration->TConf = aTConf;
    mStackConfiguration->confLimit = aConfLimit;
    mStackConfiguration->windowSize = aWindowSize;
    mStackConfiguration->useLinkEstablishmentProtocol = aUseLinkEstablishmentProtocol;
}

PDU::bc_channel BCSPConfiguration::getTunnelOver() const
{
    return tunnel_over;
}

PDU::bc_channel BCSPConfiguration::getTunnelBelow() const
{
    return tunnel_below;
}

uint32 BCSPConfiguration::getResendTimeout() const
{
    return mStackConfiguration->resendTimeout;
}

uint32 BCSPConfiguration::getRetryLimit() const
{
    return mStackConfiguration->retryLimit;
}

uint32 BCSPConfiguration::getTShy() const
{
    return mStackConfiguration->TShy;
}

uint32 BCSPConfiguration::getTConf() const
{
    return mStackConfiguration->TConf;
}

uint16 BCSPConfiguration::getConfLimit() const
{
    return mStackConfiguration->confLimit;
}

uint8 BCSPConfiguration::getWindowSize() const
{
    return mStackConfiguration->windowSize;
}

bool BCSPConfiguration::getUseCRC() const
{
    return mStackConfiguration->useCRC;
}

bool BCSPConfiguration::getUseLinkEstablishmentProtocol() const
{
    return mStackConfiguration->useLinkEstablishmentProtocol;
}

