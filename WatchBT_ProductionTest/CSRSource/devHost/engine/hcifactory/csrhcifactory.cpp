///////////////////////////////////////////////////////////////////////
//
//  FILE    :  csrhcifactory.cpp
//
//  Copyright CSR 2002-2006
//
//  AUTHOR  :  Adam Hughes
//
//  TEMPLATE:  CSRHCIFactory
//
//  PURPOSE :  instantiate bluecontrollers.
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcifactory/csrhcifactory.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

#define DECLARE_CSRHCIFACTORY
#include "csrhcifactory.h"

//class BlueCoreDeviceController;
#include "csrhci/bluecontroller.h"
//class TransportConfiguration
#include "csrhci/transportconfiguration.h"

class CSRHCIFactory::implementation
{
public:
    implementation ( TransportConfigurationPtr aConfig )
    : mConfig ( aConfig ) {}
    BlueCoreDeviceController_newStyle * spawn ( BlueCoreDeviceController_newStyle::CallBackProvider&cb) const
    {
        return new BlueCoreDeviceController_newStyle ( *mConfig , cb );
    }
    void getName ( const char *& a ) const    { mConfig->getName ( a ); }
    void getName ( const wchar_t *& a ) const { mConfig->getName ( a ); }
private:
    TransportConfigurationPtr mConfig;
};


CSRHCIFactory::CSRHCIFactory ( const TransportConfigurationPtr& aConfig )
:pimple(0)
{
    if ( aConfig )
        pimple = new implementation ( aConfig );
}

CSRHCIFactory::~CSRHCIFactory ()
{
    delete pimple;
}

bool CSRHCIFactory::isReady() const
{
    return ( pimple != 0 );
}

BlueCoreDeviceController_newStyle * CSRHCIFactory::spawn( BlueCoreDeviceController_newStyle::CallBackProvider& cb ) const
{
    BlueCoreDeviceController_newStyle * result = 0;
    if ( pimple )
        result = pimple->spawn( cb );
    return result;
}

void CSRHCIFactory::kill ( BlueCoreDeviceController*tbd ) const
{
    delete tbd;
}

void CSRHCIFactory::getName ( const wchar_t *& aName ) const
{
    if ( pimple )
        pimple->getName( aName );
}

void CSRHCIFactory::getName ( const char *& aName ) const
{
    if ( pimple )
        pimple->getName( aName );
}

