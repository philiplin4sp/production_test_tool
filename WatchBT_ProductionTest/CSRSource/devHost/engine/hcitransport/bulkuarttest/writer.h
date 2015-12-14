#ifndef WRITER_H
#define WRITER_H

#include "thread/thread.h"
#include "common/types.h"

class FlowControlReceiver;
class UARTAbstraction;

class Writer : public Threadable
{   
public:
    Writer(UARTAbstraction *u, uint32 dl, uint32 ps, FlowControlReceiver &r);
    virtual int ThreadFunc();
private:
    UARTAbstraction *uart;
    const uint32 data_length;
    const uint32 packet_size;
    FlowControlReceiver &flowControlReceiver;
};  

#endif

