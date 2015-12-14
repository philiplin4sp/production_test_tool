///////////////////////////////////////////////////////////////////////
//
//  FILE   :  usertransport.h
//
//  Copyright CSR 2000-2006
//
//  AUTHOR :  Adam Hughes
//
//  CLASS  :  UserTransport
//
//  PURPOSE:  Declare a class inheriting top API from the
//            HCITransport class, and owning a UART and
//            the wherewithal to put stuff in it.
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/phys/usertransport.h#1 $
//
///////////////////////////////////////////////////////////////////////

#ifndef _USERTRANSPORT_H_
#define _USERTRANSPORT_H_

#include "transportimplementation.h"
#include "uart.h"
#include "thread/thread.h"
#include "thread/signal_box.h"

class UserTransport : public BlueCoreTransportImplementation, Threadable
{
public:
    UserTransport ( const UserConfiguration& , UARTPtr& , const CallBacks& );
    ~UserTransport ();
    bool start ();
    bool ready ( uint32 timeout );
    void stop ();
    void sendpdu ( const PDU& );
    void setDataWatch ( DataWatcher * aDW ) { mUART->setDataWatch( aDW ); }
private:
    int ThreadFunc();
    UARTPtr mUART;
    SignalBox dying;
    SignalBox::Item dead;
    PDU::bc_channel mChannel;
};

#endif//_USERTRANSPORT_H_
