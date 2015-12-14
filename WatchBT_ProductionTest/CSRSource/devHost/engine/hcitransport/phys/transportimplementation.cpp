///////////////////////////////////////////////////////////////////////////////
//
//  FILE   :  transportimplementation.cpp
//
//  Copyright CSR 2001-2006
//
//  AUTHOR :  Adam Hughes
//
//  CLASS  :  BlueCoreTransportImplementation
//
//  PURPOSE:  Implement the single standard implementation.
//            If that makes any sense at all...
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/phys/transportimplementation.cpp#1 $
//
///////////////////////////////////////////////////////////////////////////////

#include "transportimplementation.h"
#include "tunnel.h"

#include <string.h>

BlueCoreTransportImplementation::BlueCoreTransportImplementation( const char * aName , bool aTunnel , const CallBacks& aRecv )
:
    mName ( aName ),
    mReceiver ( aRecv.c ),
    mReporter ( aRecv.f ),
    sentPdu ( mSignals ),
    mCallBack (0),
    tunnel ( aTunnel )
{
}

const char * BlueCoreTransportImplementation::getName() const
{
    return mName;
}

BlueCoreTransportImplementation::~BlueCoreTransportImplementation()
{
}

void BlueCoreTransportImplementation::blockUntilSentPdu( const PDU& aPacket , uint32 timeout )
{
    CriticalSection::Lock here ( OneBlockingCallAtATime );
    sentPdu.unset();
    sendpdu ( aPacket );
    mSignals.wait( timeout );
}

///////////////////////////////////////////////////////////////////////////////
//  What to do when data passes through our hands
///////////////////////////////////////////////////////////////////////////////
void BlueCoreTransportImplementation::recvData ( PDU::bc_channel channel, const uint8 *apdu, size_t aLength )
{
    if ( tunnel && channel == PDU::hciCommand )
        recvTunnel ( apdu , aLength );
    else
        //  we used to check what channel it came in on, but now we are opene to data on all
        //  channels.
        //  if ( channel >= 0 && channel < PDU::csr_hci_extensions_count )
        mReceiver.onPDU ( channel , apdu , aLength );
}

void BlueCoreTransportImplementation::onLinkFail( FailureMode f )
{
    //  gibber and panic.
    mReporter.onFail( f );
}

// HCI tunnelling command and event codes
const uint8 HCI_TUNNEL_COMPLETE_EVENT = 0x0E;

// HCI tunnelling payload descriptor flags and masks
const uint8 HCI_TUNNEL_CHANNEL_MASK = 0x3F;
const uint8 HCI_TUNNEL_FIRST        = 1 << 6;
const uint8 HCI_TUNNEL_LAST         = 1 << 7;
const uint8 HCI_TUNNEL_MAX_PAYLOAD  = 254;
const uint8 HCI_TUNNEL_HEADER_SIZE  = 4;

// Send a PDU that requires HCI tunnelling for manufacturer extensions
void BlueCoreTransportImplementation::sendTunnel ( const PDU& aPacket )
{
    if (tunnel)
    {
        uint8 data[HCI_TUNNEL_HEADER_SIZE+HCI_TUNNEL_MAX_PAYLOAD];
        size_t offset = 0;
        while ( offset < aPacket.size() )
        {
			size_t lLength = aPacket.size() - offset;
			while (1)
			{
				// Calculate the payload length and descriptor
				uint8 descriptor = aPacket.channel();
				if (offset == 0)
					descriptor |= HCI_TUNNEL_FIRST;

				if (HCI_TUNNEL_MAX_PAYLOAD < lLength)
					lLength = HCI_TUNNEL_MAX_PAYLOAD;

				if (offset + lLength == aPacket.size())
					descriptor |= HCI_TUNNEL_LAST;

				// Build and send the HCI command for this fragment
				data[0] = uint8(HCI_TUNNEL_COMMAND & 0xff);
				data[1] = uint8(HCI_TUNNEL_COMMAND >> 8);
				data[2] = uint8(lLength + 1);
				data[3] = descriptor;
				memcpy(data + HCI_TUNNEL_HEADER_SIZE, aPacket.data() + offset, lLength);
				PDU p(PDU::hciCommand, data, HCI_TUNNEL_HEADER_SIZE + lLength);
				if (pdu_size_forbidden(p))
				{
					// can't send pdu of this length. try one size smaller.
					--lLength;
				}
				else
				{
					sendpdu(p);
					break;
				}
			}

            offset += lLength;
        }
    }
}

// Check received PDUs for HCI tunnelling or dummy events
void BlueCoreTransportImplementation::recvTunnel ( const uint8 *apdu, size_t aLength )
{
    //  first three bytes contain the header for the tunnel data packet
    //  (2 Byte HCI Header + 1 Tunnel byte).
    if (   ( 3 <= aLength )
        && ( apdu[0] == HCI_TUNNEL_EVENT )
        && ( apdu[1] == aLength - 2 ) )
    {
        // Reassemble HCI tunnelling packet
        PDU::bc_channel channel = PDU::bc_channel(apdu[2] & HCI_TUNNEL_CHANNEL_MASK);
        if ((PDU::zero <= channel) && (channel < PDU::csr_hci_extensions_count))
        {
            bool first = (apdu[2] & HCI_TUNNEL_FIRST) != 0;
            bool last = (apdu[2] & HCI_TUNNEL_LAST) != 0;

            if ( first && last )
            {
                //  ignore the building vector, and just send up.
                recvData ( channel , apdu + 3 , aLength - 3 );
            }
            else
            {
                if (first)
                    mRecvTunnel[channel].clear();
                // Add the new fragment to the buffer
                mRecvTunnel[channel].insert( mRecvTunnel[channel].end() , apdu + 3 , apdu + aLength );
                if ( last )
                    recvData ( channel , &mRecvTunnel[channel][0] , mRecvTunnel[channel].size() );
            }
        }
    }
    else
    {
        //  If tunneling generates dummy command completes, they'll be sent up too.
        mReceiver.onPDU ( PDU::hciCommand , apdu , aLength );
    }
}

/////////////////////////////////////////////////////////////////////////
// NAME
//  SleepControl  -  Control when the transport lets the BlueCore sleep

/*virtual*/ bool BlueCoreTransportImplementation::SleepControl(enum SleepType)
{
	return false;
}

/////////////////////////////////////////////////////////////////////////
// NAME
//  PingPeriod  -  Control when the transport pings the BlueCore

/*virtual*/ bool BlueCoreTransportImplementation::PingPeriod(int millisecs)
{
	return false;
}

bool BlueCoreTransportImplementation::reconfigure_uart_baudrate(uint32 baudrate)
{
    return false;
}
