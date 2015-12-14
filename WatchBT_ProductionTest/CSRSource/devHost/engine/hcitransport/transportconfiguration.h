//////////////////////////////////////////////////////////////////////////////
//
//  FILE   :    transportconfiguration.h
//
//  Copyright CSR 2001-2006
//
//  PURPOSE:    Declare classes used for configuring tranports.
//              These are an alternative to using the factory classes
//              (and are used by them).
//
//  CLASS  :    TransportConfiguration, H2Configuration, H4Configuration
//              BCSPConfiguration, H5Configuration
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/transportconfiguration.h#1 $
//
//////////////////////////////////////////////////////////////////////////////

#ifndef TRANSPORTCONFIGURATION_H__
#define TRANSPORTCONFIGURATION_H__

#include "csrhci/name_store.h"
#include "csrhci/physicaltransportconfiguration.h"
#include "hcipacker/pdu.h"
#include "common/countedpointer.h"

class BlueCoreTransportImplementation;
struct CallBacks;

///////////////////////////////////////////////////////////////////////////////
//  BASE CLASS : TransportConfiguration
///////////////////////////////////////////////////////////////////////////////

class TransportConfiguration : public Counted<TransportConfiguration>
{
public:
    //  enumerate the variations available
    enum Protocol
    {
        //  not allowed in host code, since it mean "no host".
        NONE = 0,
        //  CSR proprietary protocol:
        BCSP = 1,
        //  Bluetooth standard protocols:
        H2 = 2, USB = 2,
        //  H3 ,  unsupported.
        H4 = 3,
        H5 = 6,
        H4DS = 7,
        SDIO = 8,
        //  Raw access to uart.
        User = 4,
        //  Host only protocol
        SOCK = 50,
#ifndef NO_BLUEZ
	BlueZ = 51
#endif//NO_BLUEZ
    };
    // enumerate the manufacturer specific extensions supported
    enum Extensions
    {
        ignore = 0,
        CSR = 1
    };
    //  constructor
    TransportConfiguration ( bool useCSRExtensions ) : tunnel_type ( useCSRExtensions ? CSR : ignore ) {}
    //  destructor
    virtual ~TransportConfiguration() {}
    //  allow implicit cast to "TransportConfiguration::Protocol"
    virtual operator Protocol () const = 0;
    virtual TransportConfiguration * clone() const = 0;
    virtual BlueCoreTransportImplementation * make ( const CallBacks& aRecv ) const = 0;
    virtual bool nameIsWide () const = 0;
    virtual const char * getNameA () const = 0;
    virtual const wchar_t * getNameW () const = 0;
    virtual void getName ( const char *& aName ) const = 0;
    virtual void getName ( const wchar_t *& aName ) const = 0;
    bool useCSRExtensions() const { return (tunnel_type == CSR); };
private:
    Extensions tunnel_type;
};

typedef CountedPointer<TransportConfiguration> TransportConfigurationPtr;

#ifdef WIN32
#ifndef _WIN32_WCE
///////////////////////////////////////////////////////////////////////////////
//  HCI specifies H2 = Bluetooth USB
///////////////////////////////////////////////////////////////////////////////

class H2Configuration : public TransportConfiguration
{
public:
    H2Configuration ( USBConfiguration & aUSBConfig , bool aTunnel = true );
    operator Protocol () const { return USB; }
    BlueCoreTransportImplementation * make ( const CallBacks& aRecv ) const;
    TransportConfiguration * clone () const;
    bool nameIsWide () const { return mUSBConfiguration.nameIsWide(); }
    const char * getNameA () const { return mUSBConfiguration.getNameA(); }
    const wchar_t * getNameW () const { return mUSBConfiguration.getNameW(); }
    void getName ( const char *& aName ) const { mUSBConfiguration.getName(aName); }
    void getName ( const wchar_t *& aName ) const { mUSBConfiguration.getName(aName); }
private:
    USBConfiguration mUSBConfiguration;
};
#endif
#endif

#ifndef _WIN32_WCE

////////////////////////////////////////////////////////////////////////
//  SDIO Card Type-A Specification for Bluetooth (v1.00)
////////////////////////////////////////////////////////////////////////

class SDIOConfiguration : public TransportConfiguration
{
public:
    SDIOConfiguration ( const SDIOPConfiguration& aConfig, bool aTunnel = true );
    operator Protocol () const { return SDIO; }
    BlueCoreTransportImplementation * make ( const CallBacks& aRecv ) const;
    TransportConfiguration * clone () const;
    bool nameIsWide () const { return config.nameIsWide(); }
    const char * getNameA () const { return config.getNameA(); }
    const wchar_t * getNameW () const { return config.getNameW(); }
    void getName ( const char *& aName ) const { config.getName(aName); }
    void getName ( const wchar_t *& aName ) const { config.getName(aName); }
private:
    SDIOPConfiguration config;
};
#endif

///////////////////////////////////////////////////////////////////////////////
//  HCI specifies H4 - Bluetooth UART (demands UART is reliable,
//                                     no error detection or retries)
///////////////////////////////////////////////////////////////////////////////

class BaseH4Configuration : public TransportConfiguration
{
public:
    BaseH4Configuration ( const UARTConfiguration & aUartConfig , bool aTunnel = true );
    bool nameIsWide () const { return mUartConfiguration.nameIsWide(); }
    const char * getNameA () const { return mUartConfiguration.getNameA(); }
    const wchar_t * getNameW () const { return mUartConfiguration.getNameW(); }
    void getName ( const char *& aName ) const { mUartConfiguration.getName(aName); }
    void getName ( const wchar_t *& aName ) const { mUartConfiguration.getName(aName); }
    bool getFlowControlOn() const { return mUartConfiguration.getFlowControlOn(); }
protected:
    UARTConfiguration mUartConfiguration;
};

class H4Configuration : public BaseH4Configuration
{
public:
    H4Configuration ( UARTConfiguration & aUartConfig,
                      bool aTunnel = true );
    H4Configuration ( const char * aDeviceName,
                      int BaudRate,
                      bool aTunnel = true );
    H4Configuration ( const wchar_t * aDeviceName,
                      int BaudRate,
                      bool aTunnel = true );

    operator Protocol () const { return H4; }
    TransportConfiguration * clone () const;
    BlueCoreTransportImplementation * make ( const CallBacks& aRecv ) const;
};

class H4DSConfiguration : public BaseH4Configuration
{
public:
    H4DSConfiguration ( UARTConfiguration & aUartConfig,
                        bool aTunnel = true, uint32 wakeup_length_ms = 10 );
    H4DSConfiguration ( const char * aDeviceName,
                        int BaudRate,
                        bool aTunnel = true, uint32 wakeup_length_ms = 10 );
    H4DSConfiguration ( const wchar_t * aDeviceName,
                        int BaudRate,
                        bool aTunnel = true, uint32 wakeup_length_ms = 10 );

    operator Protocol () const { return H4DS; }
    TransportConfiguration * clone () const;
    BlueCoreTransportImplementation * make ( const CallBacks& aRecv ) const;

	uint32 getWakeupMessageLength() const { return mWakeMessageLength; }

protected:
	// Length in milliseconds of the wakeup message (number of 0's)
	uint32 mWakeMessageLength;
};


///////////////////////////////////////////////////////////////////////////////
//  HCI specifies H5 - Bluetooth reliable UART
//                     (with error detection and retries)
///////////////////////////////////////////////////////////////////////////////

class H5Configuration : public TransportConfiguration
{
public:
    H5Configuration(UARTConfiguration &aUartConfig,
					bool aUseCRC = true);

    H5Configuration(const char *aDeviceName, int BaudRate,
                    bool aUseCRC = true);

    H5Configuration(const wchar_t *aDeviceName, int BaudRate,
                    bool aUseCRC = true);

    virtual ~H5Configuration ();

    virtual operator Protocol () const;
    virtual TransportConfiguration * clone () const;
    virtual BlueCoreTransportImplementation * make ( const CallBacks& aRecv ) const;
    virtual bool nameIsWide() const;
    virtual const char *getNameA () const;
    virtual const wchar_t *getNameW () const;
    virtual void getName(const char * &aName) const;
    virtual void getName(const wchar_t * &aName) const;

    void setConfig( uint32 aResendTimeout, uint32 aTShy, uint32 aTConf,
                    uint8 aWindowSize = 4, uint32 aTWakeup = 50,
                    bool aMuzzleAtStart = false);

    bool getUseCRC() const;
    uint32 getResendTimeout() const;
    uint32 getTShy() const;
    uint32 getTConf() const;
    uint8 getWindowSize() const;
    uint32 getTWakeup() const;
    bool getMuzzleAtStart() const;

private:
    class Impl;
    CountedPointer<Impl> m_impl;
};

///////////////////////////////////////////////////////////////////////////////
//  CSR Proprietary BCSP = reliable UART, with error detection and retries
///////////////////////////////////////////////////////////////////////////////

typedef struct _StackConfiguration StackConfiguration;

class BCSPConfiguration : public TransportConfiguration
{
public:
    BCSPConfiguration ( const UARTConfiguration & aUartConfig ,
                        bool aUseCRC = true ,
                        bool aTunnel = true ,
                        PDU::bc_channel aTunnel_over = PDU::bcsp_channel_count ,
                        PDU::bc_channel aTunnel_below = PDU::zero );
    BCSPConfiguration ( const char * aDeviceName , int BaudRate ,
                        bool aUseCRC = true ,
                        bool aTunnel = true ,
                        PDU::bc_channel aTunnel_over = PDU::bcsp_channel_count ,
                        PDU::bc_channel aTunnel_below = PDU::zero );
    BCSPConfiguration ( const wchar_t * aDeviceName , int BaudRate ,
                        bool aUseCRC = true ,
                        bool aTunnel = true ,
                        PDU::bc_channel aTunnel_over = PDU::bcsp_channel_count ,
                        PDU::bc_channel aTunnel_below = PDU::zero );
    BCSPConfiguration ( const BCSPConfiguration& );
    ~BCSPConfiguration ();
    operator Protocol () const { return BCSP; }
    TransportConfiguration * clone () const;
    BlueCoreTransportImplementation * make ( const CallBacks& aRecv ) const;
    void setConfig ( uint32 aResendTimeout , uint32 aRetryLimit , uint32 aTShy , 
		uint32 aTConf , uint16 aConfLimit , uint8 aWindowSize , bool useLinkEstablishmentProtocol );
    bool nameIsWide () const { return mUartConfiguration.nameIsWide(); }
    const char * getNameA () const { return mUartConfiguration.getNameA(); }
    const wchar_t * getNameW () const { return mUartConfiguration.getNameW(); }
    void getName ( const char *& aName ) const { mUartConfiguration.getName(aName); }
    void getName ( const wchar_t *& aName ) const { mUartConfiguration.getName(aName); }
    PDU::bc_channel getTunnelOver() const;
    PDU::bc_channel getTunnelBelow() const;
    uint32 getResendTimeout() const;
    uint32 getRetryLimit() const;
    uint32 getTShy() const;
    uint32 getTConf() const;
    uint16 getConfLimit() const;
    uint8 getWindowSize() const;
    bool getUseCRC() const;
    bool getUseLinkEstablishmentProtocol() const;
protected:
    PDU::bc_channel tunnel_below;
    PDU::bc_channel tunnel_over;
    StackConfiguration * mStackConfiguration;
    UARTConfiguration mUartConfiguration;
private:
    //  hide the assignment operator
    BCSPConfiguration& operator = ( const BCSPConfiguration& );
};

///////////////////////////////////////////////////////////////////////////////
//  CSR Proprietary BCSP = implemented by small changes to the H5 protocol.
///////////////////////////////////////////////////////////////////////////////

class BCSPH5Configuration : public BCSPConfiguration
{
public:
    BCSPH5Configuration ( const char * aDeviceName , int BaudRate ,
                          bool aUseCRC = true ,
                          bool aTunnel = true ,
                          PDU::bc_channel aTunnel_over = PDU::bcsp_channel_count ,
                          PDU::bc_channel aTunnel_below = PDU::zero )
    : BCSPConfiguration(aDeviceName, BaudRate, aUseCRC, aTunnel, aTunnel_over, aTunnel_below)
    {}

    BCSPH5Configuration ( const wchar_t * aDeviceName , int BaudRate ,
                          bool aUseCRC = true ,
                          bool aTunnel = true ,
                          PDU::bc_channel aTunnel_over = PDU::bcsp_channel_count ,
                          PDU::bc_channel aTunnel_below = PDU::zero )
    : BCSPConfiguration(aDeviceName, BaudRate, aUseCRC, aTunnel, aTunnel_over, aTunnel_below)
    {}

    BlueCoreTransportImplementation * make ( const CallBacks& aRecv ) const;
};

///////////////////////////////////////////////////////////////////////////////
//  User transport sends raw PDUs to the uart
///////////////////////////////////////////////////////////////////////////////

class UserConfiguration : public TransportConfiguration
{
public:
    UserConfiguration ( UARTConfiguration& , PDU::bc_channel returnChannel = PDU::zero );
    PDU::bc_channel getChannel() const { return mChannel; }
    operator Protocol () const { return User; }
    TransportConfiguration * clone () const;
    BlueCoreTransportImplementation * make ( const CallBacks& aRecv ) const;
    bool nameIsWide () const { return mUartConfiguration.nameIsWide(); }
    const char * getNameA () const { return mUartConfiguration.getNameA(); }
    const wchar_t * getNameW () const { return mUartConfiguration.getNameW(); }
    void getName ( const char *& aName ) const { mUartConfiguration.getName(aName); }
    void getName ( const wchar_t *& aName ) const { mUartConfiguration.getName(aName); }
private:
    //  all upward bound pdus on this channel.
    PDU::bc_channel mChannel;
    UARTConfiguration mUartConfiguration;
};

///////////////////////////////////////////////////////////////////////////////
//  CSR Proprietary Socket Transport.
///////////////////////////////////////////////////////////////////////////////

class SOCKConfiguration : public TransportConfiguration
{
public:
    enum direction { fromChip , toChip };
    SOCKConfiguration ( uint16 port , direction dir = fromChip ); // local port
    SOCKConfiguration ( const char * host , uint16 port , direction dir = toChip ); // remote port
    SOCKConfiguration ( const wchar_t * host , uint16 port , direction dir = toChip ); // remote port
    operator Protocol() const { return SOCK; }
    TransportConfiguration * clone () const;
    BlueCoreTransportImplementation * make ( const CallBacks& aRecv ) const;
private:
    const char * getNameA () const { return hostname.getA(); }
    const wchar_t * getNameW () const { return hostname.getW(); }
    void getName ( const char *& aName ) const { hostname.get(aName); }
    void getName ( const wchar_t *& aName ) const { hostname.get(aName); }
    bool nameIsWide () const { return hostname.isW(); }
    const name_store hostname;
    const uint16 port;
    const direction type;
};

#ifndef NO_BLUEZ
///////////////////////////////////////////////////////////////////////////////
//  Special BlueZ - Use BlueZ raw socket interface (linux/USB)
///////////////////////////////////////////////////////////////////////////////

class BlueZConfiguration : public TransportConfiguration
{
public:
    BlueZConfiguration(const char *name, bool autoup = false);

    virtual ~BlueZConfiguration();

    virtual operator Protocol () const;
    virtual TransportConfiguration * clone () const;
    virtual BlueCoreTransportImplementation * make ( const CallBacks& aRecv ) const;
    virtual bool nameIsWide() const;
    virtual const char *getNameA () const;
    virtual const wchar_t *getNameW () const;
    virtual void getName(const char * &aName) const;
    virtual void getName(const wchar_t * &aName) const;

    int getDeviceNumber() const;

private:
    class Impl;
    CountedPointer<Impl> m_impl;
};
#endif//NO_BLUEZ

#endif /* TRANSPORTCONFIGURATION_H__ */
