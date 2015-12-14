#ifndef FLOW_CONTROL_H
#define FLOW_CONTROL_H

#include "flow_control_receiver.h"
#include "flow_control_sender.h"

class FlowControl : public FlowControlReceiver, public FlowControlSender
{

};

#endif

