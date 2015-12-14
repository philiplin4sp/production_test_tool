#ifndef FLOW_CONTROL_SENDER_H
#define FLOW_CONTROL_SENDER_H

#include <cassert>

#include "defines.h"

class FlowControlSender
{
public:
    virtual ~FlowControlSender() {}
    void addCredits(credit_t credits)
    {
        assert(this);
        doAddCredits(credits);
    }
private:
    virtual void doAddCredits(credit_t credits) = 0;
};

#endif

