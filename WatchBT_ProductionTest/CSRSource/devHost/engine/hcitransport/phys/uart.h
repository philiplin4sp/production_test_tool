///////////////////////////////////////////////////////////////////////////////
//
//  FILE    :   uart.h
//
//  Copyright CSR 2001-2006
//
//  CLASS   :   UARTAbstraction
//
//  PURPOSE :   to provide a uart abstraction class for use with BCSP and H4
//              uart transport types.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CSR_UART_ABSTRACTION_H_
#define _CSR_UART_ABSTRACTION_H_

#include "common/types.h"
#include "common/countedpointer.h"
#include "csrhci/physicaltransportconfiguration.h"

class RealUARTConfiguration;
#ifdef WIN32
class PipeConfiguration;
class RealUARTHandle;
#endif
#ifdef linux
class SoEConfiguration;
class Casirack2UARTConfiguration;
#endif

class UARTAbstraction : public Counter
{
public:
    UARTAbstraction( const RealUARTConfiguration & aConfig );
#ifdef WIN32
#ifndef _WIN32_WCE
    UARTAbstraction( const PipeConfiguration & aConfig );
#endif
    UARTAbstraction( const RealUARTHandle & aConfig );
#endif
#ifdef linux
    UARTAbstraction( const SoEConfiguration & aConfig );
    UARTAbstraction( const Casirack2UARTConfiguration & aConfig );
#endif
    virtual ~UARTAbstraction();
    bool open ();
    void reopen();
    const char * getName() const;
    bool write( const uint8 * aBuffer , size_t aToWrite , uint32 * aWrote ) const;
    bool read ( uint8 * aBuffer , size_t aBufferSize , uint32 * aRead ) const;
    bool readCTS();
    void setRTS( bool active );
    bool readRTS();
    bool setBaudRate ( uint32 baudrate );
    uint32 getBaudRate();
    bool setHWFC ( bool active );

    enum fcmode
    {
        off,
        normal
    };
    bool setHWFC ( fcmode mode );

    bool setReadTimeout( uint32 timeout );
    void expectPulse();
#ifdef MM01_DUMP_STATS
	void DumpStats();
#endif /* def MM01_DUMP_STATS */
    //enums for events
    enum Event
    {
        UART_XMITDONE =0,
        UART_RCVDONE=1,
        CLIENT_TXREQ=2,
        CLIENT_DIEREQ=3,
        UART_CTSPULSED=4,
	UART_DATAWAITING,
        UART_ERROR,
        timedOut
    } ;
    Event wait ( uint32 timeout , uint32 * aReadableByteCount ) const;
    void abortWait() const;
    void reqXmit() const;
    int LastError() const;
    // class is public, but you can't see it so it is still "safe".
    class implementation;
    void setDataWatch ( DataWatcher * w );
    bool setPioDrives(uint32 mask, uint32 drives);
    bool setPios(uint32 mask, uint32 pios);
    bool getPios(uint32 *pios);
    bool reset();
private:
    mutable implementation * mUart;
};

bool prepareForH4 ( UARTAbstraction& );

bool prepareForBCSP ( UARTAbstraction& );

typedef CountedPointer<UARTAbstraction> UARTPtr;

///////////////////////////////////////////////////////////////////////////////
//
//   UARTConfiguration Implementations
//
///////////////////////////////////////////////////////////////////////////////

class UARTConfiguration::Implementation : public Counter
{
public:
    Implementation ( const char * const aName );
    Implementation ( const wchar_t * const aName );
    //  Rule of three (requires copy constructor)
    virtual ~Implementation ();
    Implementation ( const Implementation & );
    Implementation & operator= ( const Implementation & );

    bool nameIsWide () const { return mName.isW(); }
    const char * getNameA () const { return mName.getA(); }
    const wchar_t * getNameW () const { return mName.getW(); }
    void getName ( const char *& aName ) const { mName.get(aName); }
    void getName ( const wchar_t *& aName ) const { mName.get(aName); }
    virtual bool getFlowControlOn() const = 0;

    virtual UARTAbstraction * make () const = 0;
    virtual bool testOpen() const = 0;

    // Most uarts don't have pios.
    virtual bool hasPios() const {return false;}
private:
    name_store mName;
};

///////////////////////////////////////////////////////////////////////////////
//   RealUART
///////////////////////////////////////////////////////////////////////////////

class UARTConfigurationData
{
public:
    UARTConfigurationData ( uint32 aBaudRate,
			    UARTConfiguration::parity aParity,
			    uint8 aStopBits,
			    bool aFlowControlOn )
    : mBaudRate(aBaudRate),
      mParity(aParity),
      mStopBits(aStopBits),
      mFlowControlOn(aFlowControlOn)
    {}

//    UARTConfigurationData (); // set all zero
//    void setBaudRate (uint32 x) { mBaudRate = x; }
//    void setParity (UARTConfiguration::parity x) { mParity = x; }
//    void setStopBits (uint8 x) { mStopBits = x; }
//    void setFlowControlOn (bool x) { mFlowControlOn = x; }
//    void setReadTimeout (uint32 x) { mReadTimeout = x; }

    uint32 getBaudRate () const { return mBaudRate; }
    UARTConfiguration::parity getParity () const { return mParity; }
    uint8 getStopBits () const { return mStopBits; }
    bool getFlowControlOn () const { return mFlowControlOn; }
private:
    uint32 mBaudRate;
    UARTConfiguration::parity mParity;
    uint8 mStopBits;
    bool mFlowControlOn;
};

class RealUARTConfiguration
: public UARTConfiguration::Implementation,
  public UARTConfigurationData
{
public:
    RealUARTConfiguration ( const char * const aName , uint32 aBaudRate , UARTConfiguration::parity aParity  , uint8 aStopBits , bool aFlowControlOn , uint32 aReadTimeout );
    RealUARTConfiguration ( const wchar_t * const aName , uint32 aBaudRate , UARTConfiguration::parity aParity , uint8 aStopBits , bool aFlowControlOn , uint32 aReadTimeout );

    UARTAbstraction * make () const;
    bool testOpen() const;
    uint32 getReadTimeout () const { return mReadTimeout; }
    bool getFlowControlOn() const
    { return UARTConfigurationData::getFlowControlOn(); }
    bool hasPios() const;
private:
    uint32 mReadTimeout;
};

#ifdef WIN32
///////////////////////////////////////////////////////////////////////////////
//   Pre-opened HANDLE
///////////////////////////////////////////////////////////////////////////////

class RealUARTHandle : public UARTConfiguration::Implementation
{
public:
    RealUARTHandle ( const char * const aName , void * aHandle );
    RealUARTHandle ( const wchar_t * const aName , void * aHandle );

    UARTAbstraction * make () const;
    bool testOpen() const { return true; }
    bool getFlowControlOn () const;
    void * getHandle() const;
private:
    void * mHandle;
};

#ifndef _WIN32_WCE
///////////////////////////////////////////////////////////////////////////////
//   Pipe
///////////////////////////////////////////////////////////////////////////////

class PipeConfiguration : public UARTConfiguration::Implementation
{
public:
    PipeConfiguration ( const char * const aName );
    PipeConfiguration ( const wchar_t * const aName );

    UARTAbstraction * make () const;
    bool testOpen() const;
    bool getFlowControlOn () const { return false; }
};
#endif
#endif

#ifdef linux
class SoEConfiguration
: public UARTConfiguration::Implementation,
  public UARTConfigurationData
{
public:
    SoEConfiguration ( const char * const aName , int soe_port , uint32 aBaudRate , UARTConfiguration::parity aParity  , uint8 aStopBits , bool aFlowControlOn );
    SoEConfiguration ( const wchar_t * const aName , int soe_port , uint32 aBaudRate , UARTConfiguration::parity aParity , uint8 aStopBits , bool aFlowControlOn );

    UARTAbstraction * make() const;
    bool testOpen() const;
    bool getFlowControlOn () const { return UARTConfigurationData::getFlowControlOn(); }
    int getPort() const { return port; }
    bool hasPios() const {return true;}
private:
    int port;
};

class Casirack2UARTConfiguration : public RealUARTConfiguration 
{
public:
    Casirack2UARTConfiguration(const char * const aName,
                          uint32 aBaudRate,
                          UARTConfiguration::parity aParity,
                          uint8 aStopBits,
                          bool aFlowControlOn,
                          uint32 aReadTimeout);
    
    Casirack2UARTConfiguration(const wchar_t * const aName, 
                          uint32 aBaudRate, 
                          UARTConfiguration::parity aParity, 
                          uint8 aStopBits, 
                          bool aFlowControlOn,
                          uint32 aReadTimeout);

    virtual UARTAbstraction *make() const;
    virtual bool testOpen() const;
    int getPort() const; 
    bool hasPios() const {return true;}
};

#endif

#endif

