/*
 * Copyright CSR 2008
 *
 * $Id: //depot/bc/bluesuite_2_4/devHost/engine/fastpipe_engine/scheduler.h#1 $
 *
 * Purpose: Scheduler decides which packets to send based on number of tokens
 *          and data available. It ensures the host does not violate flow
 *          control.
 *          Any time users of Scheduler believe the state has changed in a way
 *          that would affect the state, the user should call kick.
 *
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "common/types.h"
#include <map>

class TokenHandler;
class PduSender;
class ChannelSender;

class Scheduler
{
public:
    Scheduler(TokenHandler &, PduSender &);
    bool create(ChannelSender &channelSender);
    bool destroy(uint16);
    void kick();
private:
    std::map<uint16, ChannelSender *> phToChannelSender;
    TokenHandler &tokenHandler_;
    PduSender &pduSender_;
};

#endif
