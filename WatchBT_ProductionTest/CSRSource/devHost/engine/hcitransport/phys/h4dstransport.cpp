///////////////////////////////////////////////////////////////////////
//
//  FILE   :  H4DSTransport.cpp
//
//  Copyright CSR 2003-2006
//
//  AUTHOR :  Mark Marshall
//
//  CLASS  :  H4DSTransport
//
//  PURPOSE:  transparent class inheriting top API from the
//            HCITransport class, and owning a H4DS stack which does
//            all the work.
//
//            This Class also provides BCSP-specific private channels
//            L2CAP etc and puts the debug channel messages to a file
//            in the debug build version.
//
//  $Id:$
//
//  MODIFICATION HISTORY:
//
///////////////////////////////////////////////////////////////////////

#include <cassert>
#include <cstring>
#include "h4dstransport.h"
#include "h4dsstack.h"

H4DSTransport::H4DSTransport(const H4DSConfiguration &aConfig, UARTPtr aComPort, const CallBacks& aRecv)
:
    BlueCoreTransportImplementation("H4DSTransport",
				      aConfig.useCSRExtensions(), aRecv),
    mStack(0)
{
    mStack = new H4DSStack(aComPort, *this, aConfig.getWakeupMessageLength());
}

H4DSTransport::~H4DSTransport()
{
    stop();
	delete mStack;
}

bool H4DSTransport::start()
{
    return mStack->Start();
}

bool H4DSTransport::ready(uint32 timeout)
{
    return mStack && mStack->ready(timeout);
}

void H4DSTransport::stop()
{
    mStack->Stop();
    // deleting the stack causes cancelled requests on all the channels.
}

void H4DSTransport::setDataWatch(DataWatcher *w)
{
    mStack->setDataWatch(w);
}

void H4DSTransport::sendpdu(const PDU &aPacket)
{
    switch (aPacket.channel())
    {
    case PDU::hciCommand:
        assert("Command longer than 256+3" && aPacket.size() < 0x103);
        mStack->sendCommand(aPacket);
        break;
    case PDU::hciACL:
        assert("ACL data longer than 65536+4" && aPacket.size() < 0x10004);
        mStack->sendACLData(aPacket);
        break;
    case PDU::hciSCO:
        assert("SCO data longer than 65536+4" && aPacket.size() < 0x10004);
        mStack->sendSCOData(aPacket);
        break;
    case PDU::raw:
        mStack->sendRawData(aPacket);
        break;
    default:
        sendTunnel(aPacket);
        break;
    }
}

/////////////////////////////////////////////////////////////////////////////
// NAME
//  H4DSStack::CallBackInterface::sentPacket  -  Packet has been sent

/*virtual*/ void H4DSTransport::sentPacket(bool sentOk)
{
	sentPdu.set();
}

/////////////////////////////////////////////////////////////////////////////
// NAME
//  H4DSStack::CallBackInterface::recvPacket  -  Packet was recieved

/*virtual*/ void H4DSTransport::recvPacket(const PDU &aPacket)
{
	recvData(aPacket.channel(), aPacket.data(), aPacket.size());
}

/////////////////////////////////////////////////////////////////////
// NAME
//  H4DSStack::CallBackInterface::syncLost  -  The transport has crashed

/*virtual*/ void H4DSTransport::syncLost(FailureMode mode)
{
    onLinkFail(mode);
}

///////////////////////////////////////////////////////////////////////////////
//  Transport configuration objects
///////////////////////////////////////////////////////////////////////////////

static const UARTConfiguration::parity H4DS_parity = UARTConfiguration::none;
static const uint8  H4DS_stopbits = 1;
static const bool   H4DS_flowcontrolon = true;
static const uint32 H4DS_readtimeout = 1; // no timeout

H4DSConfiguration::H4DSConfiguration ( UARTConfiguration & aUartConfig ,
				   bool aTunnel, uint32 wakeup_length_ms )
:   BaseH4Configuration ( aUartConfig, aTunnel ), mWakeMessageLength(wakeup_length_ms)
{
}

H4DSConfiguration::H4DSConfiguration ( const char * aDeviceName , int BaudRate ,
				   bool aTunnel, uint32 wakeup_length_ms )
:   BaseH4Configuration ( UARTConfiguration ( aDeviceName ,
                                              BaudRate ,
                                              H4DS_parity ,
			                                  H4DS_stopbits ,
                                              H4DS_flowcontrolon ,
                                              H4DS_readtimeout ),
                          aTunnel ), mWakeMessageLength(wakeup_length_ms)
{
}

H4DSConfiguration::H4DSConfiguration(const wchar_t *aDeviceName, int BaudRate,
				   bool aTunnel, uint32 wakeup_length_ms)
:
    BaseH4Configuration(
		UARTConfiguration(
			aDeviceName,
			BaudRate,
			H4DS_parity,
			H4DS_stopbits,
			H4DS_flowcontrolon,
			H4DS_readtimeout),
		aTunnel), mWakeMessageLength(wakeup_length_ms)
{
}

TransportConfiguration *H4DSConfiguration::clone() const
{
    return new H4DSConfiguration(*this);
}

BlueCoreTransportImplementation *H4DSConfiguration::make( const CallBacks& aRecv) const
{
    return new H4DSTransport(*this, UARTPtr(mUartConfiguration.make()), aRecv);
}


