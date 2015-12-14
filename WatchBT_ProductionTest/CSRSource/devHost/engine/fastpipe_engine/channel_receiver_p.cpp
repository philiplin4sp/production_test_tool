/*
 * Copyright CSR 2008
 *
 * $Id: //depot/bc/bluesuite_2_4/devHost/engine/fastpipe_engine/channel_receiver_p.cpp#1 $
 */

#include "channel_receiver_p.h"
#include "disconnected_receiver.h"
#include "connected_receiver.h"

#include <cassert>

ChannelReceiverP::ChannelReceiverP(DisconnectedReceiver &d) :
    dreceiver(d),
    receiver(&d)
{
    assert(class_invarient());
}

bool ChannelReceiverP::connect(ConnectedReceiver &receiver)
{
    assert(class_invarient());
    bool ok = false;
    if (!this->receiver->isConnected())
    {
        this->receiver = &receiver;
	    ok = true;
    }
    return ok;
}

bool ChannelReceiverP::disconnect()
{
    assert(class_invarient());
    bool ok = false;
    if (this->receiver->isConnected())
    {
        receiver = &dreceiver;
	    ok = true;
    }
    return ok;
}

bool ChannelReceiverP::isConnected()
{
    assert(class_invarient());
    return receiver->isConnected();
}

void ChannelReceiverP::receive(const HCIDataPDU &pdu)
{
    assert(class_invarient());
    receiver->receive(pdu);
}

bool ChannelReceiverP::class_invarient() const
{
    return this &&  
           receiver && 
           ((&dreceiver == receiver && 
             !receiver->isConnected()) ||
            (&dreceiver != receiver &&
             receiver->isConnected()));
}



