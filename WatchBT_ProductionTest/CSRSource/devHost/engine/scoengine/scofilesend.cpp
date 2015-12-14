#include "scofilesend.h"
#include "scofilesend_impl.h"
#include <vector>
#include "hcipacker/hcidatapdu.h"
#include "time/stop_watch.h"
#include <common/algorithm.h>
#include <cassert>
#include <cmath>

ScoFileSend::ScoFileSend(uint16 ch, 
                         AlertCallback &alertCallback, 
                         ScoEngine &scoEngine)
    : pImpl(new Implementation(ch, alertCallback, scoEngine))
{

}

ScoFileSend::ScoFileSend(const ScoFileSend &from)
    : pImpl(from.pImpl)
{
    pImpl->inc();
}

ScoFileSend &ScoFileSend::operator=(const ScoFileSend &from)
{
    this->pImpl = from.pImpl;
    this->pImpl->inc();
    return *this;
}

ScoFileSend::~ScoFileSend()
{
    if (!pImpl->dec())
    {
        delete pImpl;
        pImpl = 0;
    }
}

bool ScoFileSend::enable(const std::string &fileName,  
                         uint32 rate, 
                         uint32 sampleSize,
                         bool overridePacketSize,
                         uint16 packetSize,
                         bool overrideNumPackets,
                         uint16 numPackets)
{
    return pImpl->enable(fileName, 
                         rate,
                         sampleSize,
                         overridePacketSize,
                         packetSize,
                         overrideNumPackets,
                         numPackets);
}

bool ScoFileSend::disable()
{
    //return pImpl->disable();
    return true;
}

ScoFileSend::Implementation::Implementation(uint16 ch, 
                                            AlertCallback &alertCallback,
                                            ScoEngine &scoEngine)
    : smartc(1),
      mCh(ch),
      mAlert(alertCallback),
      mScoEngine(scoEngine)
{

}

ScoFileSend::Implementation::~Implementation()
{

}

void ScoFileSend::Implementation::inc()
{
    CriticalSection::Lock lock(prot_smartc);
    ++smartc;
}

int ScoFileSend::Implementation::dec()
{
    CriticalSection::Lock lock(prot_smartc);
    return --smartc;
}


bool ScoFileSend::Implementation::enable(const std::string &fileName, 
                                         uint32 rate, 
                                         uint32 sampleSize,
                                         bool overridePacketSize,
                                         uint16 packetSize,
                                         bool overrideNumPackets,
                                         uint16 numPackets)
{
    HCI_READ_BUFFER_SIZE_RET_T_PDU rbs_pdu;

    if (!overridePacketSize || !overrideNumPackets)
    {
        // Either the packet size or number of packets hasn't been overriden.
        // This means we still need rbs to supply us with data.
        if (!mScoEngine.getBufferSizes(rbs_pdu)) return false;
    }
    if (!overridePacketSize) packetSize = rbs_pdu.get_sco_data_pkt_length();
    if (!overrideNumPackets) numPackets = rbs_pdu.get_total_sco_data_pkts();
    Snapshot snapshot(mCh, rate, sampleSize, packetSize, numPackets);
    Instance instance(fileName, snapshot, mAlert, mScoEngine);
    return instance.start();
}

ScoFileSend::Implementation::Instance::Instance(
                                        const std::string &fileName,
                                        ScoFileSend::Snapshot &snapshot,
                                        ScoFileSend::AlertCallback &alert,
                                        ScoEngine &scoEngine) :
    mAlert(alert),
    mSnapshot(snapshot),
    mFileName(fileName),
    mScoEngine(scoEngine),
    dataOnChip(0),
    sendState(IDLE),
    ttime(0),
    mFile(alert, snapshot),   
    mTnext(0)
{

}

bool ScoFileSend::Implementation::Instance::start()
{
    return init()       &&
           startClock() &&
           sendFile()   &&
           stopClock()  &&
           report();
}

bool ScoFileSend::Implementation::Instance::init()
{
    if (!mFile.open(mFileName.c_str(), "rb"))
    {
        return false;
    }
    mAlert.callback(STARTING_SEND, mSnapshot);
    return true;
}

bool ScoFileSend::Implementation::Instance::startClock()
{
    InitHiResClock(&clock);
    t0 = HiResClockGetSec();
    return true;
}


bool ScoFileSend::Implementation::Instance::sendFile()
{
    std::vector<uint8> data(mSnapshot.scoPktSize);
    size_t n;

    while (sendState != FINISHED)
    {
        switch(sendState)
        {
        case IDLE:
            n = mFile.read(&data[0], mSnapshot.scoPktSize);
            if (!n)
            {
                sendState = NO_DATA_TO_SEND;
            }
            else
            {
                sendData(&data[0], n);
                ttime = HiResClockGetMilliSec();
                sendState = READ_DATA;
            }
            break;
        case NO_DATA_TO_SEND:
            if (!mFile.isEof()) mAlert.callback(FILE_ERROR, mSnapshot);
            sendState = FINISHED;
            break;
        case READ_DATA:
            n = mFile.read(&data[0], mSnapshot.scoPktSize);
            sendState = n != 0 ? MAKE_SENDABLE : NO_DATA_TO_SEND;
            break;
        case MAKE_SENDABLE:
            {
                HiResClockMilliSec new_time = HiResClockGetMilliSec();
                HiResClockMilliSec td = new_time - ttime;
                size_t dataSent = (mSnapshot.scoRate / 1000) * 
                                  (mSnapshot.scoSampleSize / 8) * td;
                ttime = new_time;
                
                if (dataSent > dataOnChip)
                {
                    updateStatistics(dataSent - dataOnChip);
                    dataOnChip = 0;
                    sendState = SEND_DATA;
                }
                else 
                {
                    dataOnChip -= dataSent;
                    // Check if sending data will overflow the buffer
                    if ((((dataOnChip + mSnapshot.scoPktSize) - 1) / mSnapshot.scoPktSize) == mSnapshot.scoNumPackets) 
                        HiResClockSleepMilliSec(mSnapshot.interval / 1000);
                    else
                        sendState = SEND_DATA;
                }
            }
            break;
        case SEND_DATA:
            sendData(&data[0], n);
            sendState = READ_DATA;
            break;
        default:
            // Oops
            assert(0);
            break;
        }
    }
    return true;
}

void ScoFileSend::Implementation::Instance::updateStatistics(size_t overrun_size)
{
    using namespace std;
    HiResClockMilliSec overrun_time = (HiResClockMilliSec) ceil(overrun_size / ((mSnapshot.scoRate / 1000.0) / (mSnapshot.scoSampleSize / 8.0)));
    mSnapshot.worstOver = 
                
        
        max(overrun_time,(unsigned long) mSnapshot.worstOver);   /* Linux64 expects it to be unsigned long*/
    mSnapshot.over += overrun_time;
}

void ScoFileSend::Implementation::Instance::sendData(uint8 *data, size_t length)
{
    HCISCOPDU pdu(length);
    pdu.set_data(data, length);
    pdu.set_handle(mSnapshot.scoch);
    mScoEngine.send(pdu);
    dataOnChip += length;
    mSnapshot.total += length;
}

bool ScoFileSend::Implementation::Instance::stopClock()
{
    time_t endtime = HiResClockGetSec();
    mSnapshot.timeElapsed = endtime - t0;
    if (0 == mSnapshot.timeElapsed)
    {
        mSnapshot.timeElapsed = 1;
    }
    return true;
}

bool ScoFileSend::Implementation::Instance::report()
{
    mAlert.callback(FINISHED_SEND, mSnapshot);
    return true;
}
