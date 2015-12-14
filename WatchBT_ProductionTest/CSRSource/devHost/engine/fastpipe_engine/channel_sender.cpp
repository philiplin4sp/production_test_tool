/*
 * Copyright CSR 2008
 *
 * $Id: //depot/bc/bluesuite_2_4/devHost/engine/fastpipe_engine/channel_sender.cpp#1 $
 *
 */

#include "channel_sender.h"
#include <cassert>

ChannelSender::ChannelSender(uint16 ph, uint16 ch) :
    ch_(ch),
    ph_(ph)
{

}

uint16 ChannelSender::getPh()
{
    assert(this);
    return ph_;
}

uint16 ChannelSender::getCh()
{
    assert(this);
    return ch_;
}

std::pair<bool, HCIACLPDU> ChannelSender::getPdu(uint16 size)
{
    assert(this);
    return getPdu_(size);
}
