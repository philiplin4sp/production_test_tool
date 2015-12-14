///////////////////////////////////////////////////////////////////////
//
//  FILE   :  h5configuration.cpp
//
//  Copyright CSR 2000-2006
//
//  AUTHOR :  Mark Marshall
//
//  CLASS  :  H5Configuration
//
//  PURPOSE:  Configuration object for H5
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/phys/h5configuration.cpp#1 $
//
//  MODIFICATION HISTORY:
//      1.1     03:jan:02   mm01    Created.
//      1.2     29:jan:03   ajh     Added new headers - at top level,
//                                  Knock on moved data out of low level
//                                  headers, and forced rename of
//                                  BlueCoreTransport::Implementation to
//                                  BlueCoreTransportImplementation... grrr.
//      1.3     30:jan:03   mm01    CVS log entries & fixed for GCC.
//
///////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

#include "csrhci/transportconfiguration.h"
#include "h5transport.h"
#include "bcspimplementation.h"

static const UARTConfiguration::parity H5_parity = UARTConfiguration::even;
static const uint8  H5_stopbits = 1;
static const bool   H5_flowcontrolon = false;
static const uint32 H5_readtimeout = 1; // no timeout

class H5Configuration::Impl : public Counter
{
public:
    UARTConfiguration	mUartConfiguration;
    bool		mUseCRCs;
    uint32		mResendTimeout;
    uint32		mTShy;
    uint32		mTConf;
    uint8		mWindowSize;
    bool		mMuzzleAtStart;
    uint32		mTWakeup;

    Impl(UARTConfiguration aUartConfig, bool useCRCs)
	:	mUartConfiguration(aUartConfig), mUseCRCs(useCRCs),
		mResendTimeout(250), mTShy(250), mTConf(250),
		mWindowSize(4), mMuzzleAtStart(false), mTWakeup(50)
    { /*EMPTY*/ }
};

H5Configuration::H5Configuration(
    UARTConfiguration &aUartConfig,
    bool aUseCRC /*= true*/)
 :	TransportConfiguration(true),
    m_impl(new Impl(aUartConfig, aUseCRC))
{
}

H5Configuration::H5Configuration(
    const char *aDeviceName, int BaudRate,
    bool aUseCRC /*= true*/)
 :  TransportConfiguration(true),
    m_impl(new Impl(
	       UARTConfiguration(aDeviceName, BaudRate, H5_parity,
				 H5_stopbits, H5_flowcontrolon,
				 H5_readtimeout),
	       aUseCRC))
{
}

H5Configuration::H5Configuration(
    const wchar_t *aDeviceName, int BaudRate,
    bool aUseCRC /*= true*/)
 : TransportConfiguration(true),
    m_impl(new Impl(
	       UARTConfiguration(aDeviceName, BaudRate, H5_parity,
				 H5_stopbits, H5_flowcontrolon,
				 H5_readtimeout),
	       aUseCRC))
{
}

/*virtual*/ H5Configuration::~H5Configuration()
{
}

/*virtual*/ H5Configuration::operator TransportConfiguration::Protocol () const
{
    return TransportConfiguration::H5;
}

/*virtual*/ TransportConfiguration *H5Configuration::clone () const
{
    return new H5Configuration(*this);
}

/*virtual*/ BlueCoreTransportImplementation *H5Configuration::make ( const CallBacks& aRecv ) const
{
    return new H5Transport(*this, UARTPtr(m_impl->mUartConfiguration.make()), aRecv );
}

/*virtual*/ bool H5Configuration::nameIsWide() const
{
	return m_impl->mUartConfiguration.nameIsWide();
}

/*virtual*/ const char *H5Configuration::getNameA() const
{
	return m_impl->mUartConfiguration.getNameA();
}

/*virtual*/ const wchar_t *H5Configuration::getNameW() const
{
	return m_impl->mUartConfiguration.getNameW();
}

/*virtual*/ void H5Configuration::getName(const char *&aName) const
{
	m_impl->mUartConfiguration.getName(aName);
}

/*virtual*/ void H5Configuration::getName(const wchar_t *&aName) const
{
	m_impl->mUartConfiguration.getName(aName);
}

void H5Configuration::setConfig(
    uint32 aResendTimeout, uint32 aTShy, uint32 aTConf,
    uint8 aWindowSize /*= 4*/, uint32 aTWakeup /*= 50*/,
    bool aMuzzleAtStart /*= false*/)
{
    m_impl->mResendTimeout = aResendTimeout;
    m_impl->mTShy = aTShy;
    m_impl->mTConf = aTConf;
    m_impl->mWindowSize = aWindowSize;
    m_impl->mTWakeup = aTWakeup;
    m_impl->mMuzzleAtStart = aMuzzleAtStart;
}

bool H5Configuration::getUseCRC() const
{
    return m_impl->mUseCRCs;
}

uint32 H5Configuration::getResendTimeout() const
{
    return m_impl->mResendTimeout;
}

uint32 H5Configuration::getTShy() const
{
    return m_impl->mTShy;
}

uint32 H5Configuration::getTConf() const
{
    return m_impl->mTConf;
}

uint8 H5Configuration::getWindowSize() const
{
    return m_impl->mWindowSize;
}

uint32 H5Configuration::getTWakeup() const
{
    return m_impl->mTWakeup;
}

bool H5Configuration::getMuzzleAtStart() const
{
    return m_impl->mMuzzleAtStart;
}

