/*
 * Copyright CSR 2008
 *
 * $Id: //depot/bc/bluesuite_2_4/devHost/engine/fastpipe_engine/channel_receiver_demultiplexer.cpp#1 $
 */

#include "channel_receiver_demultiplexer.h"

#include "hcipacker/hcipacker.h"

#include <cassert>

ChannelReceiverDemultiplexer::ChannelReceiverDemultiplexer
    (DisconnectedReceiver &dreceiver) :
    channelReceivers(0xffff, ChannelReceiverP(dreceiver)) 
{
    assert(class_invarient());
}

bool ChannelReceiverDemultiplexer::connect(uint16 ch, 
                                           ConnectedReceiver &channelReceiver)
{
    assert(class_invarient());
    return channelReceivers[ch].connect(channelReceiver);
}

bool ChannelReceiverDemultiplexer::disconnect(uint16 ch)
{
    assert(class_invarient());
    return channelReceivers[ch].disconnect();
}

bool ChannelReceiverDemultiplexer::isConnected(uint16 ch)
{
    assert(class_invarient());
    return channelReceivers[ch].isConnected();
}

void ChannelReceiverDemultiplexer::receive(const HCIDataPDU &pdu)
{
    assert(class_invarient());
    channelReceivers[pdu.get_handle()].receive(pdu);
}

bool ChannelReceiverDemultiplexer::class_invarient() const
{
    return this &&
           channelReceivers.size() == 0xffff;
}
