///////////////////////////////////////////////////////////////////////
//
//  FILE   :  H4Transport.h
//
//  Copyright CSR 2000-2006
//
//  AUTHOR :  Adam Hughes
//
//  CLASS  :  H4Transport
//
//  PURPOSE:  transparent class inheriting top API from the
//            BlueCoreTransport class, and owning a H4Stack which does
//            all the work.
//
//            BlueCoreTransport also provides BCSP-specific private channels
//            L2CAP etc which are tunneled through HCI vendor specific
//            commands and puts the debug channel messages to a file
//            in the debug build version.
//
//  MODIFICATION HISTORY:
//  1.5   7:Oct:00  at  Added modification history.
//                      Added intercept of send callback to delete the
//                      pdu buffer for consistency with the BCSP
//                      transport.
//  1.6   16:Oct:00 at  Restored previous callback behaviour - the
//                      memory deallocation problem was in
//                      BlueCoreTransport.
//  1.8   16:Nov:00 at  Added ability to open and close COM port
//                      externally.
//	1.10  28:Mar:01 ckl Removed stdafx.h
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/phys/h4transport.h#1 $
//
///////////////////////////////////////////////////////////////////////

#ifndef __H4Transport_H__
#define __H4Transport_H__

#include "transportimplementation.h"
#include "h4stack.h"

class H4Transport : public BlueCoreTransportImplementation
{
public:
    H4Transport ( const H4Configuration& , UARTPtr , const CallBacks& );
    virtual ~H4Transport();

    bool start( void );
    bool ready ( uint32 timeout );
    void stop();

    static void onSyncLoss ( void * );
    static void onUartDies ( void * );
    static void onPacketFromChip ( uint8 * apdu , size_t aLength , void * parameters );
    static void onPacketSent ( bool done , void * parameters );
    void processPacketFromChip ( PDU::bc_channel channel , uint8 * apdu , size_t aLength );

    void setDataWatch ( DataWatcher * w );
private:
    void sendpdu  ( const PDU& aPacket ) ;
    void init();

    H4Stack mStack;

    class H4CallBackInfo
    {
    public:
        H4Transport * mObject;
        PDU::bc_channel channel;
    };

    H4CallBackInfo mCommandCallBackInfo;
    H4CallBackInfo mACLCallBackInfo;
    H4CallBackInfo mSCOCallBackInfo;
};

#endif // __H4TRANSPORT_H__
