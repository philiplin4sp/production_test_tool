#ifndef SCO_FILE_SEND_H
#define SCO_FILE_SEND_H

#include <string>
#include "common/types.h"
#include "scoengine.h"

class ScoFileSend
{
public:
    class Implementation;

    enum AlertType
    {
        NO_HIGH_PERFORMANCE_CLOCK,
        CAN_NOT_OPEN_FILE,
        STARTING_SEND,
        TIME_GET_DEV_CAPS_FAILED,
        TIME_GET_DEV_CAPS,
        TIME_BEGIN_PERIOD_FAILED,
        FILE_ERROR,
        FINISHED_SEND,
        TIME_END_PERIOD_FAILED
    };

    struct Snapshot
    {
        Snapshot(uint16 ch, 
                 uint32 rate, 
                 uint32 sampleSize,
                 uint32 packetSize,
                 uint32 numPackets) :
            scoch(ch),
            scoPktSize(packetSize),
            scoRate(rate),
            scoNumPackets(numPackets),
            scoSampleSize(sampleSize),
            samps((packetSize / (sampleSize / 8)) * numPackets),
            interval((1000000 * packetSize) / ((sampleSize / 8) * rate)),
            worstOver(0),
            over(0),
            total(0),
            timeElapsed(0) {}
        uint16 scoch;
        uint32 scoPktSize;
        uint32 scoRate;
        uint32 scoNumPackets;
        uint32 scoSampleSize;
        int samps;
        uint32 interval;
        uint32 periodMin;
        uint32 resolution;
        uint32 worstOver;
        uint32 over;
        size_t total;
        time_t timeElapsed;
    };

    class AlertCallback
    {
    public:
        virtual void callback(AlertType alertType, Snapshot snapshot) = 0;
    };

    ScoFileSend(uint16 ch, AlertCallback &alertCallback, ScoEngine &scoEngine);
    ScoFileSend(const ScoFileSend &from);
    ScoFileSend &operator=(const ScoFileSend &from);
    ~ScoFileSend();

    bool enable(const std::string &fileName,
                uint32 rate, 
                uint32 sampleSize,
                bool overridePacketSize,
                uint16 packetSize,
                bool overrideNumPackets,
                uint16 numPackets);

    bool disable();

private:
    Implementation *pImpl;
};

#endif // SCO_FILE_SEND_H
