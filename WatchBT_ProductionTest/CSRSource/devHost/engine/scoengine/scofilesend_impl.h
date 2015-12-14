#ifndef SCO_FILE_SEND_IMPL_H
#define SCO_FILE_SEND_IMPL_H

#include "scofilesend.h"
#include "common/types.h"
#include "thread/critical_section.h"
#include "file_wrapper.h"
#include "time/hi_res_clock.h"

class ScoFileSend::Implementation
{
public:
    class Instance;
    Implementation(uint16 ch,
                   AlertCallback &alertCallback,
                   ScoEngine &scoEngine);
    ~Implementation();

    void inc();
    int dec();

    bool enable(const std::string &fileName,   
                uint32 rate, 
                uint32 sampleSize,
                bool overridePacketSize,
                uint16 packetSize,
                bool overrideNumPackets,
                uint16 numPackets);

    bool disable();

private:
    CriticalSection prot_smartc;
    int smartc;

    uint16 mCh;
    AlertCallback &mAlert;
    ScoEngine &mScoEngine;
};

class ScoFileSend::Implementation::Instance
{
public:
    Instance(const std::string &fileName, 
             Snapshot &snapshot,
             AlertCallback &alert,
             ScoEngine &scoEngine);
    bool start();
private:
    bool init();
    bool startClock();
    bool sendFile();
    void sendData(uint8 *data, size_t length);
    void updateStatistics(size_t overrun_size);
    bool stopClock();
    bool report();

    AlertCallback &mAlert;
    Snapshot &mSnapshot;
    std::string mFileName;
    ScoEngine &mScoEngine;

    size_t dataOnChip;

    enum SendState
    {
        IDLE,
        NO_DATA_TO_SEND,
        READ_DATA,
        MAKE_SENDABLE,
        SEND_DATA,
        FINISHED
    } sendState;

    HiResClock clock;
    time_t t0;
    HiResClockMilliSec ttime;

    FileWrapper mFile;
    uint32 mTnext;
};

#endif // SCO_FILE_SEND_IMPL_H
