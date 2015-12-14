/*
 * Copyright CSR 2008
 *
 * $Id: //depot/bc/bluesuite_2_4/devHost/engine/fastpipe_engine/channel_receiver_p.h#1 $
 *
 * Purpose: A simple wrapper around a pointer to ChannelReceiver. Its role
 *          is to ensure that a ConnectedReceiver* is not overwritten by another
 *          ConnectedReceiver*. The check that a pipe is connected is now done 
 *          at a higher level, so this class can be factored out at some point.
 * 
 */

#ifndef CHANNEL_RECEIVER_P_H
#define CHANNEL_RECEIVER_P_H

class HCIDataPDU;
class DisconnectedReceiver;
class ConnectedReceiver;
class ChannelReceiver;

class ChannelReceiverP
{
public:
    ChannelReceiverP(DisconnectedReceiver &);
    bool connect(ConnectedReceiver &);
    bool disconnect();
    bool isConnected();
    void receive(const HCIDataPDU &);
private:
    DisconnectedReceiver &dreceiver;
    ChannelReceiver *receiver;
    bool class_invarient() const;
};

#endif

