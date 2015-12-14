#ifndef FLOW_CONTROL_RECEIVER_H
#define FLOW_CONTROL_RECEIVER_H

#include <cassert>

#include "defines.h"

class FlowControlReceiver
{
public:
    virtual ~FlowControlReceiver() {};
    credit_t useCredits()
    {
        assert(this);
        return doUseCredits();
    }
private:
    virtual credit_t doUseCredits() = 0;
};

#endif

