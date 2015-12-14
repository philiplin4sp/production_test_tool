/*
 * Copyright CSR 2008
 *
 * $Id: //depot/bc/bluesuite_2_4/devHost/engine/fastpipe_engine/channel_receiver.h#1 $
 *
 * Purpose: ChannelReceiver is a base class for receiving data from
 *          ChannelReceiverDemultiplexer.
 */

#ifndef CHANNEL_RECEIVER_H
#define CHANNEL_RECEIVER_H

class HCIDataPDU;

class ChannelReceiver
{
public:
    ChannelReceiver(bool connected);
    bool isConnected();
    void receive(const HCIDataPDU &);
private:
    const bool connected_;
    virtual void receive_(const HCIDataPDU &) = 0;
};

#endif
