/*
 * Copyright CSR 2008
 *
 * $Id: //depot/bc/bluesuite_2_4/devHost/engine/fastpipe_engine/disconnected_receiver.h#1 $
 *
 * Purpose: DisconnectedReceiver receives data on disconnected pipes. Thanks to
 *          changes in how a pipe is determined to be connected this class is
 *          no longer needed and can be removed at some point.
 */

#ifndef DISCONNECTED_RECEIVER_H
#define DISCONNECTED_RECEIVER_H

#include "channel_receiver.h"

class HCIDataPDU;

class DisconnectedReceiver : public ChannelReceiver
{
public:
    DisconnectedReceiver();
private:
    virtual void receive_(const HCIDataPDU &) = 0;
};

#endif
