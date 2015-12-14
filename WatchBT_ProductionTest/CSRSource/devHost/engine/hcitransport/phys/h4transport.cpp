///////////////////////////////////////////////////////////////////////
//
//  FILE   :  H4Transport.cpp
//
//  Copyright CSR 2000-2006
//
//  AUTHOR :  Adam Hughes
//
//  CLASS  :  H4Transport
//
//  PURPOSE:  transparent class inheriting top API from the
//            HCITransport class, and owning a H4 stack which does
//            all the work.
//
//            This Class also provides BCSP-specific private channels
//            L2CAP etc and puts the debug channel messages to a file
//            in the debug build version.
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/phys/h4transport.cpp#1 $
//
//  OLD MODIFICATION HISTORY (before the move):
//  1.5   7:Sep:00  at  Added modification history.
//                      Added intercept of send callback to delete the
//                      pdu buffer for consistency with the BCSP
//                      transport.
//  1.6   16:Oct:00 at  Restored previous callback behaviour - the
//                      memory deallocation problem was in
//                      BlueCoreTransport.
//  1.7   16:Nov:00 at  Added ability to open and close COM port
//                      externally.
//  1.8	   3:Mar:01 dm  Added Windows CE functionality
//  1.9   28:Mar:01 ckl Added stdafx.h
//
//  MODIFICATION HISTORY:
//  1.14  30:jan:03 mm  Fixed setting of H4plusSixteenBitTransport to
//                      false when we are not using H4.
//
///////////////////////////////////////////////////////////////////////

#include <cassert>
#include <cstring>
#include "h4transport.h"
#define ASSERT assert

class CallBackInfo
{
public:
    CallBackInfo ( const PDU& aptr , SignalBox::Item * aSentPdu )
        : ptr ( aptr ) , sentPdu ( aSentPdu ) {}
    PDU ptr;
    SignalBox::Item * sentPdu;
};

H4Transport::H4Transport( const H4Configuration &aConfig , UARTPtr aComPort , const CallBacks& aRecv )
:
    BlueCoreTransportImplementation ( "H4Transport" ,
				      aConfig.useCSRExtensions() , aRecv ),
    mStack ( aComPort )
{
    init();
}

void H4Transport::init()
{
    //set up H4 stack and callbacks on all channels
    mCommandCallBackInfo.channel = PDU::hciCommand;
    mACLCallBackInfo.channel = PDU::hciACL;
    mSCOCallBackInfo.channel = PDU::hciSCO;
    mCommandCallBackInfo.mObject = this;
    mACLCallBackInfo.mObject = this;
    mSCOCallBackInfo.mObject = this;
    //  Commands and events to the same controller...
    //  we hope the upper layer can tell what it is expecting...
    mStack.setupRecvCommand ( &mCommandCallBackInfo , onPacketFromChip );
    mStack.setupRecvEvent   ( &mCommandCallBackInfo , onPacketFromChip );
    mStack.setupRecvACLData ( &mACLCallBackInfo , onPacketFromChip );
    mStack.setupRecvSCOData ( &mSCOCallBackInfo , onPacketFromChip );
}

H4Transport::~H4Transport()
{
    stop();
}

bool H4Transport::start( )
{
    bool ok = mStack.Start ();
    if ( ok )
    {
        mStack.setupOnSyncLoss ( onSyncLoss , this );
        mStack.setupOnUartDies ( this->onUartDies , this );
    }
    return ok ;
}

void H4Transport::onSyncLoss ( void * p )
{
    ((H4Transport*)p)->onLinkFail ( h4_sync_loss );
}

void H4Transport::onUartDies ( void * p )
{
    ((H4Transport*)p)->onLinkFail ( uart_failure );
}

void H4Transport::stop()
{
    mStack.Stop();
    // deleting the stack causes cancelled requests on all the channels.
}


bool H4Transport::ready ( uint32 timeout )
{
    return mStack.ready( timeout );
}

void H4Transport::setDataWatch ( DataWatcher * w )
{
    mStack.setDataWatch ( w );
}

void H4Transport::sendpdu ( const PDU& aPacket )
{
    CallBackInfo * lCallBackInfo = 0;
    CallBack * lCallBack = 0;
    switch ( aPacket.channel() )
    {
    case PDU::hciCommand:
    case PDU::hciACL:
    case PDU::hciSCO:
    case PDU::h4dp:
    case PDU::raw:
        lCallBackInfo = new CallBackInfo ( aPacket , &sentPdu );
        lCallBack = new CallBack;
        lCallBack->initialise ( lCallBackInfo , onPacketSent );
        break;
    default:
        sendTunnel ( aPacket );
        break;
    }
    switch ( aPacket.channel() )
    {
    case PDU::hciCommand:
        assert ( "Command longer than 65535" && aPacket.size() < 0x10000 );
        mStack.sendCommand ( aPacket.data() , (uint16) aPacket.size() , lCallBack );
        break;
    case PDU::hciACL:
        mStack.sendACLData ( aPacket.data() , aPacket.size() , lCallBack );
        break;
    case PDU::hciSCO:
        assert ( "SCO data longer than 65535" && aPacket.size() < 0x10000 );
        mStack.sendSCOData ( aPacket.data() , aPacket.size() , lCallBack );
        break;
    case PDU::h4dp:
        mStack.sendDp(aPacket.data(), aPacket.size(), lCallBack);
        break;
    case PDU::raw:
        mStack.sendRaw(aPacket.data(), aPacket.size(), lCallBack);
        break;
    default:
        break;
    }
}

void H4Transport::onPacketFromChip ( uint8 * apdu , size_t aLength , void * parameters )
{
    H4CallBackInfo * lInfo = (H4CallBackInfo*) parameters ;
    assert ( lInfo->channel );
    assert ( lInfo->mObject );
    lInfo->mObject->processPacketFromChip ( lInfo->channel , apdu , aLength );
}

void H4Transport::processPacketFromChip ( PDU::bc_channel channel , uint8 * apdu , size_t aLength )
{
    recvData ( channel , apdu , aLength );
}

void H4Transport::onPacketSent ( bool done , void * parameters )
{
    CallBackInfo * lCallBackInfo = (CallBackInfo*) parameters;
    lCallBackInfo->sentPdu->set();
    //  deleting the info object will delete the last pointer to the
    //  pdu, and so delete the pdu...  I hope.
    delete lCallBackInfo;
}

///////////////////////////////////////////////////////////////////////////////
//  Transport configuration objects
///////////////////////////////////////////////////////////////////////////////
BaseH4Configuration::BaseH4Configuration ( const UARTConfiguration & aUartConfig ,
				   bool aTunnel ) // plain H4 only
:   TransportConfiguration ( aTunnel ),
    mUartConfiguration ( aUartConfig )
{
}

H4Configuration::H4Configuration ( UARTConfiguration & aUartConfig ,
				   bool aTunnel ) // plain H4 only
:   BaseH4Configuration ( aUartConfig, aTunnel )
{
}

H4Configuration::H4Configuration ( const char * aDeviceName , int BaudRate ,
				   bool aTunnel )
:   BaseH4Configuration ( UARTConfiguration ( aDeviceName ,
                                              BaudRate ,
                                              H4_parity ,
                                              H4_stopbits ,
                                              H4_flowcontrolon ,
                                              H4_readtimeout ),
                          aTunnel )
{
}

H4Configuration::H4Configuration ( const wchar_t * aDeviceName , int BaudRate ,
				   bool aTunnel ) //  plain H4 only.
:
    BaseH4Configuration ( UARTConfiguration ( aDeviceName ,
                                              BaudRate ,
                                              H4_parity ,
                                              H4_stopbits ,
                                              H4_flowcontrolon ,
                                              H4_readtimeout ),
                          aTunnel )
{
}

TransportConfiguration * H4Configuration::clone () const
{
    return new H4Configuration ( *this );
}
BlueCoreTransportImplementation * H4Configuration::make ( const CallBacks& aRecv ) const
{
    return new H4Transport ( *this , UARTPtr ( mUartConfiguration.make() ) , aRecv );
}


