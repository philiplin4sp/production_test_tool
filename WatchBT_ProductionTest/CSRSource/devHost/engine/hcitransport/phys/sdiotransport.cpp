///////////////////////////////////////////////////////////////////////
//
//  FILE   :  sdiotransport.cpp
//
//  Copyright CSR 2006
//
//  AUTHOR :  Adam Hughes
//
//  CLASS  :  SDIOTransport
//
//  PURPOSE:  transparent class inheriting top API from the
//            BlueCoreTransport class, and owning a SDIOStack which
//            does all the work.
//
//            BlueCoreTransport also provides BCSP-specific private
//            channels L2CAP etc which are tunneled through HCI vendor
//            specific commands and puts the debug channel messages
//            to a file in the debug build version.
//
//  $Id:$
//
///////////////////////////////////////////////////////////////////////

#include <cassert>
#include <cstring>
#include "sdiotransport.h"
#include "csrhci/name_store.h"
#define ASSERT assert

static name_store build_name_store(const SDIOConfiguration &aConfig)
{
    if (aConfig.nameIsWide())
    {
        return name_store(aConfig.getNameW());
    }
    else
    {
        return name_store(aConfig.getNameA());
    }
}

SDIOTransport::SDIOTransport( const SDIOConfiguration &aConfig , const CallBacks& aRecv )
:
    BlueCoreTransportImplementation ( "SDIOTransport",
                                      aConfig.useCSRExtensions(),
                                      aRecv ),
    mStack(build_name_store(aConfig))  
{
    mStack.setCallBacks(this);
}

SDIOTransport::~SDIOTransport()
{
    stop();
}

bool SDIOTransport::start( )
{
    return mStack.start ();
}

void SDIOTransport::onTransportFailure()
{
    BlueCoreTransportImplementation::onLinkFail ( sdio_transport_fail );
}

void SDIOTransport::stop()
{
    mStack.stop();
}

bool SDIOTransport::ready ( uint32 timeout )
{
    return mStack.ready( timeout );
}

void SDIOTransport::setDataWatch ( DataWatcher * w )
{
    mStack.setDataWatch ( w );
}

void SDIOTransport::sendpdu ( const PDU& aPacket )
{
    switch ( aPacket.channel() )
    {
    case PDU::hciCommand:
        mStack.sendCommand ( aPacket.data() , aPacket.size() );
        break;
    case PDU::hciACL:
        mStack.sendACL ( aPacket.data() , aPacket.size() );
        break;
    case PDU::hciSCO:
        mStack.sendSCO ( aPacket.data() , aPacket.size() );
        break;
    default:
        mStack.sendVendor(aPacket.channel(), aPacket.data(), aPacket.size());
        break;
    }
}

void SDIOTransport::onEvent ( uint8 * apdu , uint32 aLength )
{
    recvData ( PDU::hciCommand , apdu , aLength );
}

void SDIOTransport::onACL ( uint8 * apdu , uint32 aLength )
{
    recvData ( PDU::hciACL , apdu , aLength );
}

void SDIOTransport::onSCO ( uint8 * apdu , uint32 aLength )
{
    recvData ( PDU::hciSCO , apdu , aLength );
}

void SDIOTransport::onVendor(uint8 channel, uint8 *pdu, uint32 len)
{
    recvData(PDU::bc_channel(channel), pdu, len);
}

///////////////////////////////////////////////////////////////////////
//  Transport configuration objects
///////////////////////////////////////////////////////////////////////

SDIOConfiguration::SDIOConfiguration ( const SDIOPConfiguration & aConfig ,
                           bool aTunnel )
:   TransportConfiguration(aTunnel),
    config(aConfig)
{
}

TransportConfiguration * SDIOConfiguration::clone () const
{
    return new SDIOConfiguration ( *this );
}

BlueCoreTransportImplementation * SDIOConfiguration::make ( const CallBacks& aRecv ) const
{
    return new SDIOTransport ( *this , aRecv );
}


