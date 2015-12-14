/*
 * Copyright CSR 2008
 * 
 * $Id: //depot/bc/bluesuite_2_4/devHost/engine/fastpipe_engine/scheduler.cpp#1 $
 *
 */

#include "scheduler.h"
#include "channel_sender.h"
#include "pdu_sender.h"
#include "token_handler.h"

#include <cassert>

Scheduler::Scheduler(TokenHandler &tokenHandler, PduSender &pduSender) :
    tokenHandler_(tokenHandler),
    pduSender_(pduSender)
{

}

bool Scheduler::create(ChannelSender &channelSender)
{
    assert(this);
    uint16 ph(channelSender.getPh());
    if (phToChannelSender.find(ph) != phToChannelSender.end()) return false;
    phToChannelSender.insert(std::pair<uint16, 
                                       ChannelSender *>(ph, &channelSender));
    return true;
}

bool Scheduler::destroy(uint16 ph)
{
    assert(this);
    if (phToChannelSender.find(ph) == phToChannelSender.end()) return false;
    phToChannelSender.erase(ph);
    return true;
}

/* 
 * Scheduler::kick searches for a pipe with tokens available and data waiting
 * to be sent and then sends the data and uses the tokens. It repeats this
 * until no more data can be sent.
 */
void Scheduler::kick()
{
    assert(this);
    uint16 packets_sent;

    do
    {
        packets_sent = 0;
        for (std::map<uint16, ChannelSender *>::iterator 
                i(phToChannelSender.begin());
             i != phToChannelSender.end();
             ++i)
        {
            uint32 maxTokens(tokenHandler_.getMaxTxTokens(i->first));
            if (maxTokens != 0)
            {
                std::pair<bool, HCIACLPDU> 
                    packetpair(i->second->getPdu(maxTokens));
                if (packetpair.first && pduSender_.send(packetpair.second))
                {
                    tokenHandler_.useTxTokens(i->first, packetpair.second.get_length());
                    ++packets_sent;
                }
            }
        }

    }
    while (packets_sent > 0);
}
