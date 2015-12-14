///////////////////////////////////////////////////////////////////////
//
//  FILE    :  csrhcifactory_dlg.cpp
//
//  Copyright CSR 2002-2006
//
//  AUTHOR  :  Adam Hughes (ckl)
//
//  TEMPLATE:  CSRHCIFactory
//
//  PURPOSE :  instantiate bluecontrollers with default dialog
//    Since this is a separate module it is anticipated that 
//    instances that don't speciifically innvoke the devault constructor
//    (and therefore don;t need Dialog support) 
//    won't link to this module and therfore won't
//    drag in tons of MFC cruft.
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcifactory/csrhcifactory_dlg.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

#include "options/optiondlg.h"

#define DECLARE_CSRHCIFACTORY
#include "factorydlg.h"

#include "question/question.h"

#include "csrhci/csrhcitransportenumeration.h"


static const std::string cancelled[1] = { "cancelled" };
extern const Answer DlgCancelled ( cancelled , cancelled + 1 );
extern const std::string transportRegistryPath[4] =
{
    "Software",
    "Cambridge Silicon Radio",
    "Development Kit",
    "ProtocolDialogue"
};

class CSRHCIFactory::implementation;

Answer invokeDialogueToChooseATransport
     ( const Question &choices ,
       const std::vector<std::string> &HistoryLocation
       = std::vector<std::string> ( transportRegistryPath , transportRegistryPath + 4 ) );

Answer invokeDialogueToChooseATransport( const Question &choices , const std::vector<std::string> &HistoryLocation )
{
    COptionDlg lDlg ( choices , HistoryLocation , II("Choose a protocol") );
    if ( lDlg.DoModal() == IDOK )
        return lDlg.getResult();
    else
        return DlgCancelled;
}
           
CSRHCIFactory useDlgGetFactory() //  invoke dialog.
{
    Question transports = enumerateTransports();
    Answer answer = ::invokeDialogueToChooseATransport ( transports );
    if ( !( answer == DlgCancelled ) )
    {
        std::pair < bool , TransportConfigurationPtr > x = createTransportConfig ( answer );
        if ( x.first )
            return x.second;
    }
    return 0;
}

BlueCoreDeviceController_newStyle * useDlgGetBCDC ( BlueCoreDeviceController_newStyle::CallBackProvider& cb )
{
    CSRHCIFactory x( useDlgGetFactory() );
    if ( x.isReady() )
        return x.spawn(cb);
    else
        return 0;
}

BlueCoreDeviceController_newStyle * useDlgGetBCDC ( BlueCoreDeviceController_newStyle::CallBackProvider& cb , std::string& aString )
{
    CSRHCIFactory x( useDlgGetFactory() );
    if ( x.isReady() )
    {
        char * a;
        x.getName( (const char*&)a );
        aString = a;
        return x.spawn(cb);
    }
    else
        return 0;
}

BlueCoreDeviceController_newStyle * useDlgGetBCDC ( BlueCoreDeviceController_newStyle::CallBackProvider& cb , std::wstring& aString )
{
    CSRHCIFactory x( useDlgGetFactory() );
    if ( x.isReady() )
    {
        wchar_t * a;
        x.getName( (const wchar_t*&)a );
        aString = a;
        return x.spawn(cb);
    }
    else
        return 0;
}


