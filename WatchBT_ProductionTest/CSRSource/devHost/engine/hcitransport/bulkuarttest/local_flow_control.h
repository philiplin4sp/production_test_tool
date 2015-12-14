#ifndef LOCAL_FLOW_CONTROL_H
#define LOCAL_FLOW_CONTROL_H

#include "thread/critical_section.h"
#include "thread/signal_box.h"

#include "defines.h"
#include "flow_control.h"

class LocalFlowControl : public FlowControl
{
public:
    LocalFlowControl();
private:
    virtual void doAddCredits(credit_t credits);
    virtual credit_t doUseCredits();
    CriticalSection cs;
    credit_t credits;
    SingleSignal signal; 
};

#endif

