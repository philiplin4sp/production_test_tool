///////////////////////////////////////////////////////////////////////
//
//  FILE   :  usertransport.cpp
//
//  Copyright CSR 2000-2006
//
//  AUTHOR :  Adam Hughes
//
//  CLASS  :  UserTransport
//            UserConfiguration
//
//  PURPOSE:  thick class inheriting top API from the
//            HCITransport class, and owning a UART and
//            the wherewithal to put stuff in it.
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/phys/usertransport.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

#include "usertransport.h"

UserTransport::UserTransport ( const UserConfiguration & config , UARTPtr& aUART , const CallBacks& aRecv )
: BlueCoreTransportImplementation ( "UserTransport",
                                    config.useCSRExtensions(),
                                    aRecv ),
  mUART ( aUART ),
  dead ( dying ),
  mChannel ( config.getChannel() )
{
}

UserTransport::~UserTransport()
{
    stop();
    dead.wait();
}

bool UserTransport::start ()
{
    return mUART->open() && Start();
}

bool UserTransport::ready ( uint32 timeout )
{
    return IsActive();
}

void UserTransport::stop ()
{
    mUART->abortWait();
}

void UserTransport::sendpdu ( const PDU& apdu )
{
    uint32 wrote_now;
    mUART->write ( apdu.data() , apdu.size() , &wrote_now );
}

int UserTransport::ThreadFunc()
{
    bool continuing = true;
    uint8 buffer[1024];
    int rv = 0;
    while ( continuing )
    {
        uint32 read_now = 0;
        mUART->read ( buffer , 1024 , &read_now);
        if ( read_now )
            recvData ( mChannel , buffer , read_now );
        else
        {
            switch ( mUART->wait( 1000 , &read_now ) )
            {
            case UARTAbstraction::timedOut:
            case UARTAbstraction::UART_CTSPULSED:
            case UARTAbstraction::UART_XMITDONE:
                // ignore these.
                break;
            case UARTAbstraction::CLIENT_DIEREQ:
                continuing = false;
                break;
            case UARTAbstraction::UART_RCVDONE:
                recvData ( mChannel , buffer , read_now );
                break;
            case UARTAbstraction::UART_ERROR:
                onLinkFail(uart_failure);
                break;
            default:
                rv = 0xBADC0DE;
                continuing = false;
                break;
            }
        }
    }
    dead.set();
    return rv;
}

///////////////////////////////////////////////////////////////////////
//
//  UserConfiguration Class
//
///////////////////////////////////////////////////////////////////////

UserConfiguration::UserConfiguration ( UARTConfiguration& config , PDU::bc_channel fake )
:   TransportConfiguration ( false ),
    mChannel ( fake ),
    mUartConfiguration ( config )
{}

BlueCoreTransportImplementation * UserConfiguration::make ( const CallBacks& aRecv ) const
{
    UARTPtr x ( mUartConfiguration.make() );
    return new UserTransport ( *this , x , aRecv );
}

TransportConfiguration * UserConfiguration::clone () const
{
    return new UserConfiguration ( *this );
}
