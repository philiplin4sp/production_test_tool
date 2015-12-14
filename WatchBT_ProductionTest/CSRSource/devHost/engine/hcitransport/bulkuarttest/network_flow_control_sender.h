#ifndef NETWORK_FLOW_CONTROL_SENDER_H
#define NETWORK_FLOW_CONTROL_SENDER_H

#include "defines.h"
#include "flow_control_sender.h"

class NetworkFlowControlSender : public FlowControlSender
{   
public:
    NetworkFlowControlSender(const char *name, const char *port);
    bool isConnected();
private:
    virtual void doAddCredits(credit_t credits);
    int sockfd;
};

#endif

