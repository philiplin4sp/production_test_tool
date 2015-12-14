///////////////////////////////////////////////////////////////////////
//
//  FILE   :  USBTransport.h
//
//  Copyright CSR 2000-2006
//
//  AUTHOR :  Adam Hughes
//
//  CLASS  :  USBTransport
//
//  PURPOSE:  transparent class inheriting top API from the
//            HCITransport class, and owning a USBStack which does
//            all the work.
//
//            Start-up sequence:
//            new USBTransport;
//            setupCallBacks (HCITransport functions);
//            start stack;
//            
//            If the stack is started with out the callbacks in place,
//            then data arriving in the intervening time will be lost.
//            Since this is likely to be the HCI initialisation packet,
//            that would be bad...
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/phys/usbtransport.h#1 $
//
//  MODIFICATION HISTORY:
//  1.7   7:Sep:00  at  Added modification history.
//                      Removed the inappropriate duplication of
//                      BlueCoreTransport::recvData().
//	1.3  28:Mar:01 ckl Removed stdafx.h
//
///////////////////////////////////////////////////////////////////////

#ifndef __USBTransport_H__
#define __USBTransport_H__

#include "transportimplementation.h"
#include "usb.h"

class USBTransport : public BlueCoreTransportImplementation
{
public:
    USBTransport( const H2Configuration& , USBPtr , const CallBacks& );
    virtual ~USBTransport();

    static void onFail( FailureMode f, void * aObject);

    static void onEventPacketFromChip ( void * aData , size_t aLength , void * info );
    static void onDataPacketFromChip  ( void * aData , size_t aLength , void * info );
	static void onSCOPacketFromChip ( void * aData , size_t aLength, void *info);
    void processDataFromChip ( PDU::bc_channel channel , void * aData, size_t aLength );

    bool start( void );
    bool ready ( uint32 timeout );
    void stop();
	bool set_sco_bandwidth(int bandwidth);

    void setDataWatch ( DataWatcher * w );
private:
    const H2Configuration & mConfig;
    USBPtr mStack;
    
	bool pdu_size_forbidden( const PDU& aPacket ) ;
    void sendpdu ( const PDU& aPacket ) ;
};

class USBCallBackInfo
{
    PDU::bc_channel channelNumber;
    USBTransport * object;
    PDU packet;
    SignalBox::Item * sentPdu;
};

#endif // __USBTransport_H__
