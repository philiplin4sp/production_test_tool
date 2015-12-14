///////////////////////////////////////////////////////////////////////
//
//  FILE   :  BCSPTransport.h
//
//  Copyright CSR 2000-2006
//
//  AUTHOR :  Adam Hughes
//
//  CLASS  :  BCSPTransport
//
//  PURPOSE:  transparent class inheriting top API from the
//            HCITransport class, and owning a BCSPStack which does
//            all the work.
//
//            This Class also provides BCSP-specific private channels
//            L2CAP etc and puts the debug channel messages to a file
//            in the debug build version.
//
//            Start-up sequence:
//            new BCSPTransport;
//            setupCallBacks (HCITransport functions);
//            start stack;
//            
//            If the stack is started with out the callbacks in place,
//            then data arriving in the intervening time will be lost.
//            Since this is likely to be the HCI initialisation packet,
//            that would be bad...
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/phys/bcsptransport.h#1 $
//
///////////////////////////////////////////////////////////////////////

#ifndef __BCSPTransport_H__
#define __BCSPTransport_H__

#include "transportimplementation.h"
#include "uart.h"
#include "bcspimplementation.h"
#include "../bcsp/stackconfiguration.h"

class BCSPImplementation;

class BCSPTransport : public BlueCoreTransportImplementation
{
public:
    BCSPTransport( const BCSPConfiguration& , UARTPtr , const CallBacks& );
    virtual ~BCSPTransport();

    static void onPacketFromChip ( BCTransferRequest * req, BCTransferStatus status, void * info );
    static void onPacketToChip ( BCTransferRequest * req, BCTransferStatus status, void * info );
    void processDataFromChip ( BCTransferRequest * req, PDU::bc_channel ch );

    bool start( void );
    bool ready ( uint32 timeout );
    void setTunnel( bool aTunnel);
    bool getTunnel();
    void stop();

    static void onSyncPacket ( void * );
    static void onRetriesMax ( void * );
    static void onUartDied ( void * );
    void getStackConfiguration(StackConfiguration &cfg);
    void setStackConfiguration(const StackConfiguration &cfg);

    void setDataWatch ( DataWatcher * w );

private:
    const uint16 mBufferSize;
    PDU::bc_channel mTunnelOver;
    PDU::bc_channel mTunnelBelow;

    BCSPImplementation       mStack;
    StackConfiguration       mStackConfiguration;
    BCSPChannel              channels[PDU::bcsp_channel_count];
    uint8*                   buffers [PDU::bcsp_channel_count];
    void sendpdu           ( const PDU& aPacket ) ;
};

#endif // __BCSPTransport_H__
