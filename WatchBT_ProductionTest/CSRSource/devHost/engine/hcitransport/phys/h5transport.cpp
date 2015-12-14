///////////////////////////////////////////////////////////////////////
//
//  FILE   :  h5transport.cpp
//
//  Copyright CSR 2002-2006
//
//  AUTHOR :  Mark Marshall
//
//  CLASS  :  H5Transport
//
//  PURPOSE:  transparent class inheriting top API from the
//            HCITransport class, and owning a H5Stack which does
//            all the work.
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/phys/h5transport.cpp#1 $
//
//  MODIFICATION HISTORY:
//
///////////////////////////////////////////////////////////////////////

#include <cassert>
#include "h5transport.h"
#include "h5stack.h"

H5Transport::H5Transport(const H5Configuration &aConfig, UARTPtr comPort, const CallBacks& aRecv ) :
    BlueCoreTransportImplementation("H5Transport", true, aRecv),
    mStack(0)
{
	mStack = new H5Stack(comPort, *this, aConfig.getResendTimeout(),
		aConfig.getTShy(), aConfig.getTConf(), aConfig.getUseCRC(),
		false, aConfig.getWindowSize(), aConfig.getTWakeup(),
		aConfig.getMuzzleAtStart());
}

H5Transport::~H5Transport()
{
	delete mStack;
}

/*virtual*/ bool H5Transport::start()
{
    // set up H5 stack and callbacks on all channels
    return mStack->Start();
}

/*virtual*/ bool H5Transport::ready(uint32 timeout)
{
    // if the link has failed return false
    return mStack->Ready(timeout);
}

/*virtual*/ void H5Transport::stop()
{
    // deleting the stack causes cancelled requests on all the channels.
    mStack->Stop();
}

void H5Transport::setDataWatch(DataWatcher *w)
{
    mStack->setDataWatch(w);
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  SleepControl  -  Control when the transport lets the BlueCore sleep

/*virtual*/ bool H5Transport::SleepControl(enum SleepType type)
{
	switch (type)
	{
	default:
	case SLEEP_TYPE_NOW:
	case SLEEP_TYPE_ALWAYS:
		mStack->HostSleep();
		break;

	case SLEEP_TYPE_NEVER:
		mStack->HostWakeup();
		break;
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  PingPeriod  -  Control when the transport pings the BlueCore

/*virtual*/ bool H5Transport::PingPeriod(int millisecs)
{
	return true;
}


/*virtual*/ void H5Transport::sendpdu(const PDU &aPacket)
{
	switch (aPacket.channel())
	{
	case PDU::hciCommand:
	case PDU::hciACL:
	case PDU::hciSCO:
		mStack->SendPDU(aPacket);
		break;

	default:
		sendTunnel(aPacket);
		break;
	}
}

/*virtual*/ void H5Transport::sentCommand(bool sentOk)
{
	sentPdu.set();
}

/*virtual*/ void H5Transport::sentACLData(bool sentOk)
{
	sentPdu.set();
}

/*virtual*/ void H5Transport::sentSCOData(bool sentOk)
{
	sentPdu.set();
}

/*virtual*/ void H5Transport::sentEvent(bool sentOk)
{
	sentPdu.set();
}

/*virtual*/ /*void H5Transport::sentOther(bool sentOk)
{
	sentPdu.set();
}*/

/*virtual*/ void H5Transport::recvCommand(const PDU &aPacket)
{
	recvData(aPacket.channel(), aPacket.data(), aPacket.size());
}

/*virtual*/ void H5Transport::recvACLData(const PDU &aPacket)
{
	recvData(aPacket.channel(), aPacket.data(), aPacket.size());
}

/*virtual*/ void H5Transport::recvSCOData(const PDU &aPacket)
{
	recvData(aPacket.channel(), aPacket.data(), aPacket.size());
}

/*virtual*/ void H5Transport::recvEvent(const PDU &aPacket)
{
	recvData(aPacket.channel(), aPacket.data(), aPacket.size());
}

/*virtual*/ /*void H5Transport::recvOther(const PDU &aPacket)
{
	recvData(aPacket.channel(), aPacket.data(), aPacket.size());
}*/

/*virtual*/ void H5Transport::linkFailed(FailureMode mode)
{
	onLinkFail(mode);
}
