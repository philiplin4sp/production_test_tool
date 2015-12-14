///////////////////////////////////////////////////////////////////////
//
//  FILE    :  csrhcifactory.h
//
//  Copyright CSR 2002-2006
//
//  AUTHOR  :  Adam Hughes
//
//  CLASS   :  CSRHCIFactory
//
//  PURPOSE :  declare factory class to instantiate bluecontrollers.
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcifactory/factory.h#1 $
//
///////////////////////////////////////////////////////////////////////

#ifndef CSRHCI_FACTORY_H_INCLUDED
#define CSRHCI_FACTORY_H_INCLUDED

#include "csrhci/bluecontroller.h"
typedef CountedPointer<TransportConfiguration> TransportConfigurationPtr;
#include <cstdlib> // need wchar_t

class CSRHCIFactory
{
public:
    CSRHCIFactory ( const TransportConfigurationPtr& );
    ~CSRHCIFactory ();
    //  create one
    BlueCoreDeviceController_newStyle * spawn( BlueCoreDeviceController_newStyle::CallBackProvider& cb ) const;
    void kill ( BlueCoreDeviceController* ) const;
    bool isReady() const;
    void getName ( const char *& ) const;
    void getName ( const wchar_t *& ) const;
private:
    class implementation;
    implementation * pimple;
};

#endif/*CSRHCI_FACTORY_H_INCLUDED*/
