///////////////////////////////////////////////////////////////////////
//
//  FILE    :  factorydlg.h
//
//  Copyright CSR 2004-2006
//
//  AUTHOR  :  Adam Hughes
//
//  PURPOSE :  instantiate bluecontrollers using a dialog (MFC).
//
///////////////////////////////////////////////////////////////////////

#ifndef CSRHCI_FACTORYDLG_H_INCLUDED
#define CSRHCI_FACTORYDLG_H_INCLUDED

#include <string>
#include "csrhci/bluecontroller.h"

BlueCoreDeviceController_newStyle * useDlgGetBCDC ( BlueCoreDeviceController_newStyle::CallBackProvider& );
//  return the name of the physical device used.
BlueCoreDeviceController_newStyle * useDlgGetBCDC ( BlueCoreDeviceController_newStyle::CallBackProvider& , std::string& aString );
BlueCoreDeviceController_newStyle * useDlgGetBCDC ( BlueCoreDeviceController_newStyle::CallBackProvider& , std::wstring& aString );

#ifdef DECLARE_CSRHCIFACTORY
#include "csrhci/factory.h"
CSRHCIFactory useDlgGetFactory(); //  invoke dialog.
#endif

#endif/*CSRHCI_FACTORYDLG_H_INCLUDED*/
