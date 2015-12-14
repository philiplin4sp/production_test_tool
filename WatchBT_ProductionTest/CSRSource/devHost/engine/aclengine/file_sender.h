#ifndef FILE_SENDER_H
#define FILE_SENDER_H

#include "thread/thread.h"
#include "thread/signal_box.h"
#include "thread/critical_section.h"
#include "file_transfer.h"

class FileTransfer::Sender
{
public:
    class StateMachine : public AclEngine::Sender
    {
    public:
        class State;
        StateMachine(uint16 ch, AclEngine *aclEngine, AlertCallback *alert, L2CapSizeCallback *l2CapSizeCallback, SingleSignal &signal);
        bool start(const std::string &fileName, bool flushable);
        bool localCancel();
        bool remoteCancel();
        bool finishAck();
        bool disconnect();
        FileTransfer::SendSnapshot getSnapshot();
        bool action(bool &callAlert, FileTransfer::AlertType &alertType, FileTransfer::SendSnapshot &s);
        virtual CountedPointer<AclEngine::PacketQueue> getPacketQueue(uint16 splitSize);

        // Utility functions used internally by StateMachine's states.
        void putPdu(const ExtendedHCIACLPDU_Helper &pdu, bool isData = false);
        bool getPdu(HCIACLPDU &pdu);
        uint16 getL2CapDataSize();
        void alert(const FileTransfer::AlertType alertType, const FileTransfer::SendSnapshot &snapshot);
        void changeToIdle();
        void clear();

        SingleSignal &mSignal;
        std::string mFileName;

        AclEngine *mAclEngine;
    private:
        L2CapSizeCallback *mL2CapSizeCallback;
        AlertCallback *mAlert;
    public:
        FILE *mFile;
        HCIACLPDU::packet_boundary_flag mPbf;
        FileTransfer::SendSnapshot snapshot;
        CriticalSection mCs;
        State *mState;
        int senderIndex;
        ExtendedHCIACLPDU_Helper waitingpdu;
        bool waitingData;
        std::deque<HCIACLPDU> splitPdu;
        bool splitData;
        bool firstSent;
    };

    class Thread : public Threadable
    {
    public:
        Thread(SingleSignal &signal, CountedPointer<StateMachine> stateMachine);
        ~Thread();
        virtual int ThreadFunc();
    private:
        CountedPointer<StateMachine> mStateMachine;
        SingleSignal &mSignal;
    };

    class Queue : public AclEngine::PacketQueue
    {
    public:
        Queue(CountedPointer<StateMachine> stateMachine);
        virtual bool getPdu(HCIACLPDU &pdu) ;
    private:
        CountedPointer<StateMachine> mStateMachine;
    };

    Sender(uint16 ch, AclEngine *aclEngine, AlertCallback *alert, L2CapSizeCallback *l2CapSizeCallback);
    bool start(const std::string &fileName, bool flushable);
    bool localCancel(uint32 &fileSize, uint32 &bytesSent);
    bool remoteCancel();
    bool finishAck();
    void disconnect();
    FileTransfer::SendSnapshot getSnapshot();

private:
    void waitForStop();
    SingleSignal signal;
    CountedPointer<StateMachine> stateMachine;
    Thread thread;
};

#endif
