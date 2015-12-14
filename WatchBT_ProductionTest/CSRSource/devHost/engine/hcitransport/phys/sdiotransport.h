///////////////////////////////////////////////////////////////////////
//
//  FILE   :  sdiotransport.h
//
//  Copyright CSR 2006
//
//  AUTHOR :  Adam Hughes
//
//  CLASS  :  SDIOTransport
//
//  PURPOSE:  transparent class inheriting top API from the
//            BlueCoreTransport class, and owning a SDIOStack which
//            does all the work.
//
//            BlueCoreTransport also provides BCSP-specific private
//            channels L2CAP etc which are tunneled through HCI vendor
//            specific commands and puts the debug channel messages
//            to a file in the debug build version.
//
//  $Id:$
//
///////////////////////////////////////////////////////////////////////

#ifndef HEADER_SDIOTRANSPORT_H
#define HEADER_SDIOTRANSPORT_H

#include "transportimplementation.h"
#include "sdiostack.h"

class SDIOTransport : public BlueCoreTransportImplementation,
                      public SDIOStackCallBacks
{
public:
    SDIOTransport ( const SDIOConfiguration& , const CallBacks& );
    virtual ~SDIOTransport();

    bool start( void );
    bool ready( uint32 timeout );
    void stop();

    //  SDIO Stack call back object virtual functions
    void onTransportFailure ();
    void onEvent ( uint8 * apdu , uint32 aLength );
    void onACL ( uint8 * apdu , uint32 aLength );
    void onSCO ( uint8 * apdu , uint32 aLength );
    void onVendor(uint8 channel, uint8 *pdu, uint32 len);

    static void onPacketSent ( bool done , void * parameters );

    void setDataWatch ( DataWatcher * w );
private:
    void sendpdu  ( const PDU& aPacket ) ;

    SDIOStack mStack;
};

#endif //HEADER_SDIOTRANSPORT_H

