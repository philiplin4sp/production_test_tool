///////////////////////////////////////////////////////////////////////////////
//
//  FILE   :  transportapi.cpp
//
//  Copyright CSR 2001-2006
//
//  AUTHOR :  Adam Hughes
//
//  CLASS  :  BlueCoreTransport
//
//  PURPOSE:  Implement two standard APIs to transport layer.
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/phys/transportapi.cpp#1 $
//
///////////////////////////////////////////////////////////////////////////////

#include "transportapi.h"
#include "transportimplementation.h"

#ifdef WIN32
#pragma warning(disable:4786)
#endif
#include <vector>

///////////////////////////////////////////////////////////////////////////////
//  BlueCoreTransport
///////////////////////////////////////////////////////////////////////////////

BlueCoreTransport::BlueCoreTransport( const TransportConfiguration &config , const CallBacks& aRecv )
:
    mpimple ( config.make(aRecv) )
{
}

BlueCoreTransport::~BlueCoreTransport()
{
    delete mpimple;
}

//  control the underlying transport
bool BlueCoreTransport::start ()
{
    return mpimple->start();
}

bool BlueCoreTransport::ready ( uint32 timeOut )
{
    return mpimple->ready ( timeOut );
}

void BlueCoreTransport::stop ()
{
    mpimple->stop();
}

// look at the name.
const char * BlueCoreTransport::getName() const
{
    return mpimple->getName();
}

void BlueCoreTransport::setDataWatch ( DataWatcher * w )
{
    mpimple->setDataWatch ( w );
}

//  PDUs come and go...
void BlueCoreTransport::sendpdu          ( const PDU& aPacket )
{
    mpimple->sendpdu ( aPacket );
}

void BlueCoreTransport::blockUntilSentPdu( const PDU& aPacket , uint32 timeout )
{
    mpimple->blockUntilSentPdu ( aPacket , timeout );
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  SleepControl  -  Control when the transport lets the BlueCore sleep
//
bool BlueCoreTransport::SleepControl(enum SleepType type)
{
    return mpimple->SleepControl(type);
}

/////////////////////////////////////////////////////////////////////////
// NAME
//  PingPeriod  -  Control when the transport pings the BlueCore
//
bool BlueCoreTransport::PingPeriod(int millisecs)
{
    return mpimple->PingPeriod(millisecs);
}

bool BlueCoreTransport::set_sco_bandwidth(int bandwidth)
{
    return mpimple->set_sco_bandwidth(bandwidth);
}

bool BlueCoreTransport::reconfigure_uart_baudrate(uint32 baudrate)
{
    return mpimple->reconfigure_uart_baudrate(baudrate);
}
