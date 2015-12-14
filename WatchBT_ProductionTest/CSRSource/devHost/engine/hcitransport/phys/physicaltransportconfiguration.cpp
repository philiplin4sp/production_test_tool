//////////////////////////////////////////////////////////////////////////////
//
//  FILE    :   physicaltransportconfiguration.cpp
//
//  Copyright CSR 2001-2006
//
//  CLASS   :   UARTConfiguration and USBConfiguration
//
//  PURPOSE :   to implement uart and usb abstraction configuration classes
//
///////////////////////////////////////////////////////////////////////////////
#include "csrhci/physicaltransportconfiguration.h"
#include "uart.h"
#ifdef WIN32
#ifndef _WIN32_WCE
#include "usb.h"
#endif
#endif
#include "unicode/ichar.h"

///////////////////////////////////////////////////////////////////////////////
//
//   UART
//
///////////////////////////////////////////////////////////////////////////////

UARTConfiguration::UARTConfiguration( const char * const aName,
                                      uint32 aBaudRate,
                                      UARTConfiguration::parity aParity,
                                      uint8 aStopBits,
                                      bool aFlowControlOn,
                                      uint32 aReadTimeout )
:
    pimple ( 
#ifdef linux
             // on linux if the name start with a 'c', try SoE.
             *aName == 'c' ?
             (Implementation*) new SoEConfiguration( aName,
                                   strtol(aName+1,0,10),
                                   aBaudRate,
                                   aParity,
                                   aStopBits,
                                   aFlowControlOn )
             : *aName == 'd' ?
             (Implementation*) new Casirack2UARTConfiguration (aName,
                                                          aBaudRate,
                                                          aParity,
                                                          aStopBits,
                                                          aFlowControlOn,
                                                          aReadTimeout)
             :
#endif
             new RealUARTConfiguration( aName,
                                        aBaudRate,
                                        aParity,
                                        aStopBits,
                                        aFlowControlOn,
                                        aReadTimeout )
           )
{
}

UARTConfiguration::UARTConfiguration( const wchar_t * const aName,
                                      uint32 aBaudRate,
                                      UARTConfiguration::parity aParity,
                                      uint8 aStopBits,
                                      bool aFlowControlOn,
                                      uint32 aReadTimeout )
:
    pimple (
#ifdef linux
             // on linux if the name start with a 'c', try SoE.
             *aName == 'c' ?
             (Implementation*) new SoEConfiguration( aName,
                                   wcstol(aName+1,0,10),
                                   aBaudRate,
                                   aParity,
                                   aStopBits,
                                   aFlowControlOn )
             : *aName == 'd' ?
             (Implementation*) new Casirack2UARTConfiguration(aName,
                                                         aBaudRate,
                                                         aParity,
                                                         aStopBits,
                                                         aFlowControlOn,
                                                         aReadTimeout)
             :
#endif
             new RealUARTConfiguration( aName,
                                        aBaudRate,
                                        aParity,
                                        aStopBits,
                                        aFlowControlOn,
                                        aReadTimeout )
           )
{
}

#ifdef WIN32
#ifndef _WIN32_WCE
UARTConfiguration::UARTConfiguration ( const char * const aName )
:
    pimple ( new PipeConfiguration ( aName ) )
{
}

UARTConfiguration::UARTConfiguration ( const wchar_t * const aName )
:
    pimple ( new PipeConfiguration ( aName ) )
{
}
#endif

UARTConfiguration::UARTConfiguration ( const char * const aName , void * handle )
:
    pimple ( new RealUARTHandle ( aName , handle ) )
{
}

UARTConfiguration::UARTConfiguration ( const wchar_t * const aName , void * handle )
:
    pimple ( new RealUARTHandle ( aName , handle ) )
{
}
#endif

UARTAbstraction * UARTConfiguration::make () const
{
    return pimple->make();
}

UARTConfiguration::~UARTConfiguration ()
{
}

UARTConfiguration::UARTConfiguration ( const UARTConfiguration & aFrom )
:
    pimple ( aFrom.pimple ) // common counted pointer.
{
}

bool UARTConfiguration::nameIsWide () const
{
    return pimple->nameIsWide();
}

const char * UARTConfiguration::getNameA () const
{
    return pimple->getNameA();
}

const wchar_t * UARTConfiguration::getNameW () const
{
    return pimple->getNameW();
}

void UARTConfiguration::getName ( const char *& aName ) const
{
    pimple->getName(aName);
}

void UARTConfiguration::getName ( const wchar_t *& aName ) const
{
    pimple->getName(aName);
}

bool UARTConfiguration::testOpen () const
{
    return pimple->testOpen();
}

bool UARTConfiguration::getFlowControlOn() const
{
    return pimple->getFlowControlOn();
}

bool UARTConfiguration::hasPios() const
{
    return pimple->hasPios();
}

///////////////////////////////////////////////////////////////////////////////
//   UARTImplementation
///////////////////////////////////////////////////////////////////////////////

UARTConfiguration::Implementation::Implementation ( const char * const aName )
:
    mName ( aName )
{
}

UARTConfiguration::Implementation::Implementation ( const wchar_t * const aName )
:
    mName ( aName )
{
}

UARTConfiguration::Implementation::~Implementation ()
{
}

UARTConfiguration::Implementation::Implementation ( const UARTConfiguration::Implementation & aFrom )
:
    mName ( aFrom.mName )
{
}

///////////////////////////////////////////////////////////////////////////////
//   RealUART
///////////////////////////////////////////////////////////////////////////////

RealUARTConfiguration::RealUARTConfiguration ( const char * const aName , uint32 aBaudRate , UARTConfiguration::parity aParity  , uint8 aStopBits , bool aFlowControlOn , uint32 aReadTimeout )
:
    UARTConfiguration::Implementation ( aName ),
    UARTConfigurationData (aBaudRate, aParity, aStopBits, aFlowControlOn ),
    mReadTimeout ( aReadTimeout )
{
}

RealUARTConfiguration::RealUARTConfiguration ( const wchar_t * const aName , uint32 aBaudRate , UARTConfiguration::parity aParity  , uint8 aStopBits , bool aFlowControlOn , uint32 aReadTimeout )
:
    UARTConfiguration::Implementation ( aName ),
    UARTConfigurationData (aBaudRate, aParity, aStopBits, aFlowControlOn ),
    mReadTimeout ( aReadTimeout )
{
}

UARTAbstraction * RealUARTConfiguration::make () const
{
    return new UARTAbstraction ( *this );
}

#ifdef WIN32
#ifndef _WIN32_WCE
///////////////////////////////////////////////////////////////////////////////
//   Pipe
///////////////////////////////////////////////////////////////////////////////

PipeConfiguration::PipeConfiguration ( const char * const aName )
:
    UARTConfiguration::Implementation ( aName )
{
}

PipeConfiguration::PipeConfiguration ( const wchar_t * const aName )
:
    UARTConfiguration::Implementation ( aName )
{
}

UARTAbstraction * PipeConfiguration::make () const
{
    return new UARTAbstraction ( *this );
}

#endif
///////////////////////////////////////////////////////////////////////////////
//   pre-opened UART handle
///////////////////////////////////////////////////////////////////////////////

RealUARTHandle::RealUARTHandle ( const char * const aName , void * handle )
:
    UARTConfiguration::Implementation ( aName ), mHandle(handle)
{
}

RealUARTHandle::RealUARTHandle ( const wchar_t * const aName , void * handle )
:
    UARTConfiguration::Implementation ( aName ), mHandle(handle)
{
}

UARTAbstraction * RealUARTHandle::make () const
{
    return new UARTAbstraction ( *this );
}

void * RealUARTHandle::getHandle() const
{
    return mHandle;
}

#ifndef _WIN32_WCE
///////////////////////////////////////////////////////////////////////////////
//
//   USB
//
///////////////////////////////////////////////////////////////////////////////

USBConfiguration::USBConfiguration ( const char * const aName , void * handle )
:
    mName ( aName ), mHandle ( handle )
{
}

USBConfiguration::USBConfiguration ( const wchar_t * const aName , void * handle )
:
    mName ( aName ), mHandle ( handle )
{
}

template <class T> USBAbstraction * make_helper ( const T* aName , const USBConfiguration &aThis );

USBAbstraction * USBConfiguration::make () const
{
    if (mName.isA())
        return make_helper<char> ( mName.getA() , *this );
    if (mName.isW())
        return make_helper<wchar_t> ( mName.getW() , *this );
    return 0;
}
#endif
#endif



#ifdef linux
SoEConfiguration::SoEConfiguration ( const char * const aName, int soe_port , uint32 aBaudRate , UARTConfiguration::parity aParity  , uint8 aStopBits , bool aFlowControlOn )
:   UARTConfiguration::Implementation(aName),
    UARTConfigurationData (aBaudRate, aParity, aStopBits, aFlowControlOn ),
    port(soe_port)
{
}

SoEConfiguration::SoEConfiguration ( const wchar_t * const aName, int soe_port , uint32 aBaudRate , UARTConfiguration::parity aParity  , uint8 aStopBits , bool aFlowControlOn )
:   UARTConfiguration::Implementation(aName),
    UARTConfigurationData (aBaudRate, aParity, aStopBits, aFlowControlOn ),
    port(soe_port)
{
}

UARTAbstraction * SoEConfiguration::make() const
{
    return new UARTAbstraction ( *this );
}

Casirack2UARTConfiguration::Casirack2UARTConfiguration(const char *const aName,
                                             uint32 aBaudRate,
                                             UARTConfiguration::parity aParity,
                                             uint8 aStopBits,
                                             bool aFlowControlOn,
                                             uint32 aReadTimeout)
    : RealUARTConfiguration(aName, 
                            aBaudRate, 
                            aParity, 
                            aStopBits, 
                            aFlowControlOn, 
                            aReadTimeout)
{
}

Casirack2UARTConfiguration::Casirack2UARTConfiguration(const wchar_t *const aName,
                                             uint32 aBaudRate,
                                             UARTConfiguration::parity aParity,
                                             uint8 aStopBits,
                                             bool aFlowControlOn,
                                             uint32 aReadTimeout)
    : RealUARTConfiguration(aName,
                            aBaudRate,
                            aParity,
                            aStopBits,
                            aFlowControlOn,
                            aReadTimeout)
{
}

UARTAbstraction *Casirack2UARTConfiguration::make() const
{
    return new UARTAbstraction (*this);
}

int Casirack2UARTConfiguration::getPort() const
{
    if (nameIsWide())
        return getNameW()[1] - '0';
    else
        return getNameA()[1] - '0';
}

#endif


