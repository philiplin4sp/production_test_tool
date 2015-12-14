///////////////////////////////////////////////////////////////////////
//
//  FILE   :  USBTransport.cpp
//
//  Copyright CSR 2000-2006
//
//  AUTHOR :  Adam Hughes
//
//  CLASS  :  USBTransport
//
//  PURPOSE:  transparent class inheriting top API from the
//            HCITransport class, and owning a usbStack which does
//            all the work.
//
//            Start-up sequence:
//            new USBTransport;
//            setupCallBacks (HCITransport functions);
//            start stack;
//            
//            If the stack is started without the callbacks in place,
//            then data arriving in the intervening time will be lost.
//            Since this is likely to be the HCI initialisation packet,
//            that would be bad...
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/phys/usbtransport.cpp#1 $
//
//  MODIFICATION HISTORY:
//  1.12  7:Sep:00  at  Added modification history.
//                      Removed the inappropriate duplication of
//                      BlueCoreTransport::recvData().
//	1.7   28:Mar:01 ckl Added stdafx.h
//	1.8	   3:Mar:01	dm  Added Windows CE functionality
//
///////////////////////////////////////////////////////////////////////

#ifndef _WIN32_WCE
#include "cassert"
#include <windows.h>
#else
#include <windef.h>
#include <winbase.h>
#include <dbgapi.h>
#endif
#include "usbtransport.h"
#include "usb.h"

USBTransport::USBTransport( const H2Configuration & aConfig , USBPtr aDevice , const CallBacks& aRecv )
:
    BlueCoreTransportImplementation ( "USBTransport" , aConfig.useCSRExtensions() , aRecv ),
    mConfig ( aConfig ),
    mStack ( aDevice )
{
    if ( mStack )
    {
        mStack->set_acl_callback ( onDataPacketFromChip , this );
        mStack->set_evt_callback ( onEventPacketFromChip , this );
        mStack->set_sco_callback ( onSCOPacketFromChip , this );
        mStack->set_fail_callback( onFail , this );
    }
}

USBTransport::~USBTransport()
{
    stop();
}

bool USBTransport::start()
{
    return mStack && mStack->open_connection();
}

void USBTransport::stop()
{
    if ( mStack )
        mStack->close_connection ();
}

bool USBTransport::set_sco_bandwidth(int bandwidth)
{
    return mStack->set_sco_bandwidth(bandwidth);
}

// Are we ready yet
// 
//	if we are not open yet - then try and open us again :-)
//  otherwise its all down to mStack.

bool USBTransport::ready ( uint32 timeout )
{
    return mStack && mStack->is_open ();
}

void USBTransport::setDataWatch ( DataWatcher * w )
{
    if ( mStack )
        mStack->set ( w );
}

bool USBTransport::pdu_size_forbidden( const PDU& aPacket )
{
	if (mStack)
	{
		switch ( aPacket.channel() )
		{
		case PDU::hciCommand:
		case PDU::hciACL:
		case PDU::hciSCO:
			return mStack->size_forbidden(aPacket.size());
		default:
			// would be tunneled. let tunnelling code handle it.
			return false;
		}
	}
	// with no stack it still won't work if you send it, but we'll handle that later.
	return false;
}

void USBTransport::sendpdu ( const PDU& aPacket )
{
    if ( mStack )
    {
        switch ( aPacket.channel() )
        {
        case PDU::hciCommand:
            mStack->send_cmd( (uint8*)aPacket.data() , aPacket.size() );
            break;
        case PDU::hciACL:
            mStack->send_acl( (uint8*)aPacket.data() , aPacket.size() );
            break;
        case PDU::hciSCO:
            mStack->send_sco( (uint8*)aPacket.data() , aPacket.size() );
            break;
        default:
            sendTunnel ( aPacket );
            break;
        }
        sentPdu.set();
    }
}

void USBTransport::onDataPacketFromChip ( void * aData , size_t aLength, void *info)
{
    ((USBTransport *) info)->processDataFromChip ( PDU::hciACL , aData , aLength );
}

void USBTransport::onSCOPacketFromChip ( void * aData , size_t aLength, void *info)
{
    ((USBTransport *) info)->processDataFromChip ( PDU::hciSCO , aData , aLength );
}

void USBTransport::onEventPacketFromChip ( void * aData , size_t aLength, void *info)
{
    ((USBTransport *) info)->processDataFromChip ( PDU::hciCommand , aData , aLength );
}

void USBTransport::processDataFromChip ( PDU::bc_channel channel , void * aData , size_t aLength )
{
    //  collect the data.
    recvData (channel, (uint8*)aData, aLength);
}

void USBTransport::onFail( FailureMode f, void * aObject)
{
    ((USBTransport*)aObject)->onLinkFail(f);
}

///////////////////////////////////////////////////////////////////////////////
//  Transport configuration objects
///////////////////////////////////////////////////////////////////////////////

H2Configuration::H2Configuration ( USBConfiguration & aUsbConfig , bool aTunnel )
:
    TransportConfiguration ( aTunnel ),
    mUSBConfiguration ( aUsbConfig )
{
}

TransportConfiguration * H2Configuration::clone () const
{
    return new H2Configuration ( *this );
}

BlueCoreTransportImplementation * H2Configuration::make ( const CallBacks& aRecv ) const
{
    return new USBTransport ( *this , USBPtr ( mUSBConfiguration.make() ) , aRecv );
}

