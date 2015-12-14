#include <vector>
#include <cassert>
#include <algorithm>

#include "writer.h"

#include "../phys/uart.h"
#include "flow_control_receiver.h"
#include "random.h"
#include "time/stop_watch.h"
#include "defines.h"

extern CriticalSection cs;

Writer::Writer(UARTAbstraction *const u,
               const uint32 dl,
               const uint32 ps,
               FlowControlReceiver &r) :
    uart(u),
    data_length(dl),
    packet_size(ps),
    flowControlReceiver(r)
{   
}

static credit_t received_credits(0);

int Writer::ThreadFunc()
{
    assert(this);
    uint32 data_sent(0);
    Random random(RANDOM_SEED);

    StopWatch stopWatch;
    while (data_sent < data_length && KeepGoing())
    {
        size_t credits = flowControlReceiver.useCredits();

        received_credits += credits;

        uint32 sent(0);
        std::vector<uint8> packet(credits);
        std::generate(packet.begin(), packet.end(), random);
/*        printf("Sent:\n");
        for (int i = 0; i < credits; ++i)
        {
           printf("0x%02x ", packet[i]);
        }
        printf("\n");*/
        uart->write(&packet[0], packet.size(), &sent);
        data_sent += sent;
    }

    double duration(((double) stopWatch.duration()) / 1000);

    printf("Sent %u bytes in %f s at %f bps \n", 
           data_sent,
           duration,
           (data_sent * 8) / duration);

    return 0;
}

