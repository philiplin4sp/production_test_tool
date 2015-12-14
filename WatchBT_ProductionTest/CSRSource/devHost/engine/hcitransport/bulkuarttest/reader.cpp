#include <cstdio>
#include <vector>
#include <algorithm>
#include "reader.h"

#include "../phys/uart.h"
#include "time/stop_watch.h"

#include "flow_control_sender.h"
#include "random.h"

Reader::Reader(UARTAbstraction *const u,
               const uint32 dl,
               const uint32 ps,
               FlowControlSender &s) :
    uart(u),
    data_length(dl),
    packet_size(ps),
    flowControlSender(s)
{
}

#define PACKET_SIZE 200

CriticalSection cs;

static void processData(const uint8 *const packet, const uint32 r,
                        Random &random, uint32 &data_received, 
                        FlowControlSender &fcs)
{
    data_received += r;
    // Generate data to check against
    uint8 expected[r];
    std::generate(expected, expected + r, random);
    // Check it's valid
    if (!std::equal(packet, packet + r, expected))
    {
        printf("Data received incorrect after %d bytes (packet end).\n", 
               data_received);
        printf("Data:\n");
        for (int i = 0; i < r; ++i)
        {
            printf("0x%02x ", packet[i]);
        }
        printf("\n");
        printf("Expected:\n");
        for (int i = 0; i < r; ++i)
        {
            printf("0x%02x ", expected[i]);
        }
        printf("\n");
        //random = Random(packet[r - 1]);
    }
    fcs.addCredits(r);
}

int Reader::ThreadFunc()
{
    assert(this);
    uint32 data_received(0);
    Random random(RANDOM_SEED);

    StopWatch stopWatch;

    enum {rread, rwait} state = rread;
    while (data_received < data_length && KeepGoing())
    {
        uint32 r(0);
        uint8 data[PACKET_SIZE];

        switch (state)
        {
        case rread:
            {
            if (uart->read(&data[0], PACKET_SIZE, &r))
            {
                processData(&data[0], r, random, data_received, 
                            flowControlSender);
            }
            else
            {
                state = rwait;
            }
            }
            break;
        case rwait:
            {
                UARTAbstraction::Event x = uart->wait(1000, &r);
                if (x == UARTAbstraction::UART_RCVDONE)
                {
                    processData(&data[0], r, random, data_received, 
                                flowControlSender);
                }
                else if (x == UARTAbstraction::UART_DATAWAITING)
                {
                    state = rread;
                }
                else
                {
                    printf("other event 0x%x\n", x);
                    state = rread;
                }
                state = rread;
                break;
            }
        }
    }

    double duration(((double) stopWatch.duration()) / 1000);

    printf("Received %u bytes in %f s at %f bps\n", 
           data_received,
           duration, 
           (data_received * 8) / duration);

    return 0;
}

