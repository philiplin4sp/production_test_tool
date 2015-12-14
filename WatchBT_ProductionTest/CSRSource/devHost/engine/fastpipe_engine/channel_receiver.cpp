/*
 * Copyright CSR 2008
 *
 * $Id: //depot/bc/bluesuite_2_4/devHost/engine/fastpipe_engine/channel_receiver.cpp#1 $
 */

#include "channel_receiver.h"
#include "hcipacker/hcidatapdu.h"

#include <cassert>

ChannelReceiver::ChannelReceiver(bool connected)
    : connected_(connected)
{
}

bool ChannelReceiver::isConnected()
{
    assert(this);
    return connected_;
}

void ChannelReceiver::receive(const HCIDataPDU &pdu)
{
    assert(this);
    receive_(pdu);
}
