/*
 * Copyright CSR 2008
 * 
 * $Id: //depot/bc/bluesuite_2_4/devHost/engine/fastpipe_engine/channel_sender.h#1 $
 *
 * Purpose: ChannelSender is a base class used for returning data via getPdu to 
 *          the Scheduler. It is a base class so that the ChannelSender can
 *          have multiple queues if necessary, and keep track of data that has
 *          actually been scheduled easily.
 *
 */

#ifndef CHANNEL_SENDER_H
#define CHANNEL_SENDER_H

#include "common/types.h"
#include "hcipacker/hcidatapdu.h"
#include <utility>

class ChannelSender
{
public:
    ChannelSender(uint16, uint16);
    uint16 getPh();
    uint16 getCh();
    std::pair<bool, HCIACLPDU> getPdu(uint16);
private:
    virtual std::pair<bool, HCIACLPDU> getPdu_(uint16) = 0;
    uint16 ch_;
    uint16 ph_;

};

#endif

