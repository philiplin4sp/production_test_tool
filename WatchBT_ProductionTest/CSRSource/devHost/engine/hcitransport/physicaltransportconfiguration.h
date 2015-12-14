///////////////////////////////////////////////////////////////////////////////
//
//  FILE    :   physicaltransportconfiguration.h
//
//  Copyright CSR 2001-2006
//
//  CLASS   :   UARTConfiguration and USBConfiguration
//
//  PURPOSE :   to declare uart and usb abstraction configuration classes
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _PHYSICALTRANSPORTCONFIGURATION_H_
#define _PHYSICALTRANSPORTCONFIGURATION_H_
#ifdef _MSC_VER
#pragma warning (disable:4786)
#endif

#include "common/types.h"
#include "common/countedpointer.h"
#include "csrhci/name_store.h"
#include "thread/critical_section.h"
#include <vector>
#include <string>

#ifndef DATAWATCHER
#define DATAWATCHER
class DataWatcher
{
public:
    virtual void onSend ( const uint8* , size_t ) = 0;
    virtual void onRecv ( const uint8* , size_t ) = 0;
    virtual ~DataWatcher(){}
    virtual void operator= (const DataWatcher&){}
    // no need for a copy contructor
};

class Watched
{
public:
    void set ( DataWatcher * aW )
    {
        CriticalSection::Lock here(m);
        w = aW;
    }
protected:
    Watched () : w (0) {}
    void sent ( const uint8 * d , size_t l )
    {
        CriticalSection::Lock here(m);
        if ( w )
            w->onSend ( d , l );
    }
    void recv ( const uint8 * d , size_t l )
    {
        CriticalSection::Lock here(m);
        if ( w )
            w->onRecv ( d , l );
    }
private:
    CriticalSection m;
    DataWatcher * w;
};
#endif

///////////////////////////////////////////////////////////////////////////////
//
//   UART
//
///////////////////////////////////////////////////////////////////////////////

class UARTAbstraction;

class UARTConfiguration
{
public:
    enum parity
    {
        none,
        odd,
        even
    };
    //  Real Uarts and SoE Uarts (on linux, called cN):
    UARTConfiguration ( const char * const aName , uint32 aBaudRate , parity aParity  , uint8 aStopBits , bool aFlowControlOn , uint32 aReadTimeout );
    UARTConfiguration ( const wchar_t * const aName , uint32 aBaudRate , parity aParity , uint8 aStopBits , bool aFlowControlOn , uint32 aReadTimeout );
    //  Fake Uarts:
    UARTConfiguration ( const char * const aName );
    UARTConfiguration ( const wchar_t * const aName );
    //  Pre-opened Uarts:
    UARTConfiguration ( const char * const aName , void * handle );
    UARTConfiguration ( const wchar_t * const aName , void * handle );
    //  Rule of three requires destructor and copy constructor.  Assignment disallowed.
    //  Beware - copy contructor keeps the same implementation.
    virtual ~UARTConfiguration ();
    UARTConfiguration ( const UARTConfiguration & );

    UARTAbstraction * make () const;
    //  returns true if the port or pipe is available to be opened.
    bool testOpen () const;
    static std::vector<std::string> getListOfUarts();

    bool nameIsWide () const;
    const char * getNameA () const;
    const wchar_t * getNameW () const;
    void getName ( const char *& aName ) const;
    void getName ( const wchar_t *& aName ) const;
    bool getFlowControlOn() const;
    bool hasPios() const;
    class Implementation;
private:
    UARTConfiguration & operator= ( const UARTConfiguration & );
    CountedPointer<Implementation> pimple;
};


//  defaults
static const UARTConfiguration::parity BCSP_parity = UARTConfiguration::even;
static const uint8  BCSP_stopbits = 1;
static const bool   BCSP_flowcontrolon = false;
static const uint32 BCSP_readtimeout = 1;//0xFFFFFFFF; // effectively infinite
static const UARTConfiguration::parity H4_parity = UARTConfiguration::none;
static const uint8  H4_stopbits = 1;
static const bool   H4_flowcontrolon = true;
static const uint32 H4_readtimeout = 1; // no timeout

#ifdef WIN32
#ifndef _WIN32_WCE
///////////////////////////////////////////////////////////////////////////////
//
//   USB
//
///////////////////////////////////////////////////////////////////////////////

class USBAbstraction;

class USBConfiguration
{
public:
    USBConfiguration ( const char * const aName , void * aHandle = 0 );
    USBConfiguration ( const wchar_t * const aName , void * aHandle = 0 );

    USBAbstraction * make () const;
    //  attempts to open the USB object without upsetting it.
    bool testOpen () const;

    bool nameIsWide () const { return mName.isW(); }
    const char * getNameA () const { return mName.getA(); }
    const wchar_t * getNameW () const { return mName.getW(); }
    void getName ( const char *& aName ) const { mName.get(aName); }
    void getName ( const wchar_t *& aName ) const { mName.get(aName); }
    void * get_handle() const { return mHandle; }
private:
    name_store mName;
    void * mHandle;
};

#endif//_WIN32_WCE
#endif//WIN32

#ifndef _WIN32_WCE
///////////////////////////////////////////////////////////////////////////////
//
//   SDIO
//
///////////////////////////////////////////////////////////////////////////////

class SDIOStack;

class SDIOPConfiguration
{
public:
    SDIOPConfiguration ( const char * const aName );
    SDIOPConfiguration ( const wchar_t * const aName );

    SDIOStack * make () const;
    //  attempts to open the SDIO port without upsetting it
    bool testOpen () const;

    bool nameIsWide () const { return mName.isW(); }
    const char * getNameA () const { return mName.getA(); }
    const wchar_t * getNameW () const { return mName.getW(); }
    void getName ( const char *& aName ) const { mName.get(aName); }
    void getName ( const wchar_t *& aName ) const { mName.get(aName); }
private:
    name_store mName;
};
#endif // !_WIN32_WCE

#endif//_PHYSICALTRANSPORTCONFIGURATION_H_
