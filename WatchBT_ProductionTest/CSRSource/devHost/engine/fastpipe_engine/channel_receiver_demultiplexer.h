/*
 * Copyright CSR 2008
 *
 * $Id: //depot/bc/bluesuite_2_4/devHost/engine/fastpipe_engine/channel_receiver_demultiplexer.h#1 $
 *
 * Purpose: ChannelReceiverDemultiplexer receives data and examines the
 *          handle on the packet. The data is then sent to the correct 
 *          ChannelReceiverP (where ChannelReceiverP is a simple wrapper around
 *          a ChannelReceiver pointer).
 */

#ifndef CHANNEL_RECEIVER_DEMULTIPLEXER_H
#define CHANNEL_RECEIVER_DEMULTIPLEXER_H

#include <vector>
#include "common/types.h"

#include "channel_receiver_p.h"
#include "common/countedpointer.h"

class HCIDataPDU;
class DisconnectedReceiver;
class ConnectedReceiver;

class ChannelReceiverDemultiplexer : public Counter
{
public:
    ChannelReceiverDemultiplexer(DisconnectedReceiver &dreceiver);
    bool connect(uint16 ch, ConnectedReceiver &receiver);
    bool disconnect(uint16 ch);
    bool isConnected(uint16 ch);
    void receive(const HCIDataPDU &pdu);
private:
    std::vector<ChannelReceiverP> channelReceivers;
    bool class_invarient() const;
};

typedef CountedPointer<ChannelReceiverDemultiplexer> 
    ChannelReceiverDemultiplexerPtr;

#endif
