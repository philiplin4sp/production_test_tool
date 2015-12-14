#ifndef FILE_RECEIVER_H
#define FILE_RECEIVER_H

#include "thread/critical_section.h"
#include "time/stop_watch.h"
#include "file_transfer.h"

class ControlHelper;

class FileTransfer::Receiver : public AclEngine::Callback
{
public:
    static CountedPointer<Receiver> NewReceiver(uint16 lAclChNum,
                                                AclEngine *lAclEngine, 
                                                const std::string &lFileName,
                                                bool flushable,
                                                CriticalSection &cs,
                                                AlertCallback *alert);
    ~Receiver();
    bool cancel(uint32 *fileSize, uint32 *bytesSent);
    void l2CapPacketEndedPrematurely(size_t defecit);
    bool receive(const HCIACL_L2CAP_PDU &pdu, size_t l2CapLengthRemaining);
    virtual void stopSending();
    virtual void startSending();
    bool beginStartReceive(const HCIACL_L2CAP_PDU &pdu, size_t l2CapLengthRemaining);
    bool continueStartReceive(const HCIACL_L2CAP_PDU &pdu);
    void errorStartReceive();
    bool isStartReceiveComplete();
    bool finishReceive(const ControlHelper &pdu);
    bool isDeletable();
    bool sizeChangedReceive(const ControlHelper &helper);
    const std::string& getFileName();
    void disconnect();
    bool localCancel();
    bool remoteCancel();
    FileTransfer::ReceiveSnapshot getSnapshot();

    enum ReceiveState 
    {
        RECEIVE_STATE_IDLE,
        RECEIVE_STATE_RECEIVING_START,
        RECEIVE_STATE_RECEIVED_START,
        RECEIVE_STATE_RECEIVING_DATA, 
        RECEIVE_STATE_FINISHED
    };
protected:
    Receiver(uint16 lAclChNum,
             AclEngine *lAclEngine,
             const std::string &lFileName,
             FILE *file,
             bool flushable,
             CriticalSection &cs,
             AlertCallback *alert);
    uint16 mAclChNum;
    AclEngine * const mAclEngine;
    const std::string mFileName;
    ReceiveState mReceiveState;
    size_t mFileSize;
    uint32 sizeReceived;
    size_t mL2CapLengthRemaining;
    FILE *mFile;
    uint32 finalTimeElapsed;
    StopWatch *stopWatch;
    HCIACLPDU::packet_boundary_flag mPbf;
    uint32 l2CapPacketsTruncated;
    uint32 l2CapPacketTruncationOctetsLost;
    CriticalSection &mCs;
    AlertCallback *mAlert;
    void alert(FileTransfer::AlertType alertType, 
               FileTransfer::SendSnapshot sendSnapshot,
               FileTransfer::ReceiveSnapshot receiveSnapshot);
};

#endif
