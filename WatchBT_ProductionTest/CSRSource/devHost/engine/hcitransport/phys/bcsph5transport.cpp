///////////////////////////////////////////////////////////////////////
//
//  FILE   :  BCSPH5Transport.cpp
//
//  Copyright CSR 2002-2006
//
//  AUTHOR :  Mark Marshall
//
//  CLASS  :  BCSPH5Transport
//
//  PURPOSE:  transparent class inheriting top API from the
//            HCITransport class, and owning a H5Stack which does
//            all the work.
//
//            This Class also provides BCSP-specific private channels
//            L2CAP etc and puts the debug channel messages to a file
//            in the debug build version.
//
//            Start-up sequence:
//            new BCSPH5Transport;
//            setupCallBacks (HCITransport functions);
//            start stack;
//
//	      This class is only supplied as an example at the moment.
//	      It shows hoe to extend the H5StackBase class.
//            
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/phys/bcsph5transport.cpp#1 $
//
//  MODIFICATION HISTORY:
//  1.1   ??:jan:03  mm01  First version
//  1.2   29:jan:03  ajh   Added new headers - at top level,
//                         Knock on moved data out of low level headers,
//                         and forced rename of 
//                         BlueCoreTransport::Implementation to
//                         BlueCoreTransportImplementation... grrr.
//  1.3   30:jan:01  mm01  Added CVS log.
//
///////////////////////////////////////////////////////////////////////

#include <cassert>
#include "bcsph5transport.h"
#include "bcspimplementation.h"
#include "bcsph5stack.h"


BlueCoreTransportImplementation * BCSPH5Configuration::make ( const CallBacks& aRecv ) const
{
    return new BCSPH5Transport ( *this ,
                                 UARTPtr ( mUartConfiguration.make() ),
                                 aRecv );
}


BCSPH5Transport::BCSPH5Transport ( const BCSPConfiguration &aConfig,
				                   UARTPtr comPort ,
                                   const CallBacks& aRecv ) :
    BlueCoreTransportImplementation("BCSPH5Transport", true, aRecv),
    mStack(0)
{
    mStack = new BCSPH5Stack( comPort,
                              *this,
                              aConfig.getResendTimeout(),
                              aConfig.getTShy(),
                              aConfig.getTConf(),
                              aConfig.getUseCRC(),
                              aConfig.getWindowSize(),
                              false);
    BTRACEDISABLE(((0xFFFFFFFF)));//^ IO ) ^ LINKEST ) ^ USER0 );
}

BCSPH5Transport::~BCSPH5Transport()
{
    delete mStack;
}

/*virtual*/ bool BCSPH5Transport::start()
{
    // start the BCSP stack
    return mStack->Start();
}

/*virtual*/ bool BCSPH5Transport::ready(uint32 timeout)
{
    // if the link has failed return false
    return mStack->Ready(timeout);
}

/*virtual*/ void BCSPH5Transport::stop()
{
    // deleting the stack causes cancelled requests on all the channels.
    mStack->Stop();
}

void BCSPH5Transport::setDataWatch(DataWatcher *w)
{
    mStack->setDataWatch(w);
}


/////////////////////////////////////////////////////////////////////////
// NAME
//  SleepControl  -  Control when the transport lets the BlueCore sleep

/*virtual*/ /*bool BCSPH5Transport::SleepControl(enum SleepType type)
{
	return false;
}*/


/////////////////////////////////////////////////////////////////////////
// NAME
//  PingPeriod  -  Control when the transport pings the BlueCore

/*virtual*/ /*bool BCSPH5Transport::PingPeriod(int millisecs)
{
	return false;
}*/


/*virtual*/ void BCSPH5Transport::sendpdu(const PDU &aPacket)
{
	mStack->SendPDU(aPacket);
}

/*virtual*/ void BCSPH5Transport::sentCommand(bool sentOk)
{
	sentPdu.set();
}

/*virtual*/ void BCSPH5Transport::sentACLData(bool sentOk)
{
	sentPdu.set();
}

/*virtual*/ void BCSPH5Transport::sentSCOData(bool sentOk)
{
	sentPdu.set();
}

/*virtual*/ void BCSPH5Transport::sentEvent(bool sentOk)
{
	sentPdu.set();
}

/*virtual*/ void BCSPH5Transport::sentOther(bool sentOk)
{
	sentPdu.set();
}

/*virtual*/ void BCSPH5Transport::recvCommand(const PDU &aPacket)
{
	recvData(aPacket.channel(), aPacket.data(), aPacket.size());
}

/*virtual*/ void BCSPH5Transport::recvACLData(const PDU &aPacket)
{
	recvData(aPacket.channel(), aPacket.data(), aPacket.size());
}

/*virtual*/ void BCSPH5Transport::recvSCOData(const PDU &aPacket)
{
	recvData(aPacket.channel(), aPacket.data(), aPacket.size());
}

/*virtual*/ void BCSPH5Transport::recvEvent(const PDU &aPacket)
{
	recvData(aPacket.channel(), aPacket.data(), aPacket.size());
}

/*virtual*/ void BCSPH5Transport::recvOther(const PDU &aPacket)
{
	recvData(aPacket.channel(), aPacket.data(), aPacket.size());
}

/*virtual*/ void BCSPH5Transport::linkFailed(FailureMode mode)
{
	onLinkFail(mode);
}
