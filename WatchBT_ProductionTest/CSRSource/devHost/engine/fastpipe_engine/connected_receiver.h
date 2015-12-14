/*
 * Copyright CSR 2008
 *
 * $Id: //depot/bc/bluesuite_2_4/devHost/engine/fastpipe_engine/connected_receiver.h#1 $
 *
 * Purpose: ConnectedReceiver receives data on connected pipes. Thanks to
 *          changes in how a pipe is determined to be connected this class is
 *          no longer needed and can be removed at some point. 
 *
 */

#ifndef CONNECTED_RECEIVER_H
#define CONNECTED_RECEIVER_H

#include "channel_receiver.h"

class ConnectedReceiver : public ChannelReceiver
{
public:
    ConnectedReceiver();
private:
    virtual void receive_(const HCIDataPDU &) = 0;
};

#endif

