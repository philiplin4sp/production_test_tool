#ifndef READER_H
#define READER_H

#include "common/types.h"
#include "thread/thread.h"

class FlowControlSender;
class UARTAbstraction;

class Reader : public Threadable
{
public:
    Reader(UARTAbstraction *u, uint32 dl, uint32 ps, FlowControlSender &s);
    virtual int ThreadFunc();
private:
    UARTAbstraction *uart;
    const uint32 data_length;
    const uint32 packet_size;
    FlowControlSender &flowControlSender;
};

#endif

