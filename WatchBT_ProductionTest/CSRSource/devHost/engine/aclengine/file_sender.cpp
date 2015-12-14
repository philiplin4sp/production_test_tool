#include "file_sender.h"
#include "file_pdus.h"
#include <vector>
#include <cstdio>

#define ACL_HEADER_SIZE 4
#define MAX_L2CAP_SIZE (0xffff - ACL_HEADER_SIZE) /* strip off 4 for acl header */
#define L2CAP_HEADER_SIZE 4

FileTransfer::Sender::Sender(uint16 ch, AclEngine *aclEngine, AlertCallback *alert, L2CapSizeCallback *l2CapSizeCallback) :
    signal(),
    stateMachine(new StateMachine(ch, aclEngine, alert, l2CapSizeCallback, signal)),
    thread(signal, stateMachine)
{

}

bool FileTransfer::Sender::start(const std::string &fileName, bool flushable) 
{
    return stateMachine->start(fileName, flushable) &&
           thread.Start();
}

void FileTransfer::Sender::waitForStop()
{
    signal.set();
    thread.WaitForStop(0);
}

bool FileTransfer::Sender::localCancel(uint32 &fileSize, uint32 &bytesSent)
{
    if (!stateMachine->localCancel())
    {
        return false;
    }

    waitForStop();
    fileSize = stateMachine->snapshot.fileSize;
    bytesSent = stateMachine->snapshot.octetsSent;
    return true;
}

bool FileTransfer::Sender::remoteCancel()
{
    if (!stateMachine->remoteCancel())
    {
        return false;
    }

    waitForStop();
    return true;
}

bool FileTransfer::Sender::finishAck()
{
    if (!stateMachine->finishAck())
    {
        return false;
    }

    waitForStop();
    return true;
}

void FileTransfer::Sender::disconnect()
{
    if (!stateMachine->disconnect())
    {
        return;
    }

    waitForStop();
}

FileTransfer::SendSnapshot FileTransfer::Sender::getSnapshot() 
{
    return stateMachine->getSnapshot();
}


FileTransfer::Sender::Thread::Thread(SingleSignal &signal, CountedPointer<FileTransfer::Sender::StateMachine> stateMachine) :
    mSignal(signal),
    mStateMachine(stateMachine)
{

}

FileTransfer::Sender::Thread::~Thread()
{
    Kill();
}

int FileTransfer::Sender::Thread::ThreadFunc()
{
    while (KeepGoing())
    {
        mSignal.wait();
        bool callAlert;
        FileTransfer::AlertType alertType;
        FileTransfer::SendSnapshot snapshot;

        bool dontexit = mStateMachine->action(callAlert, alertType, snapshot);

        if (callAlert)
        {
            mStateMachine->alert(alertType, snapshot);
        }

        if (!dontexit)
        {
            break;
        }
        else
        {
            // We usually want to send something. Nothing is broken by calling this function redundantly.
            // It does tend to simplify things a bit.
            mStateMachine->mAclEngine->kickScheduler();
        }
    }

    return 0;
}

class FileTransfer::Sender::StateMachine::State
{
public:
    // We can only start a file transfer in one state. In every other state we want to reject it.
    virtual bool start(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine, const std::string &fileName, bool flushable) {return false;}
    virtual bool localCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine) = 0;
    virtual bool remoteCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine) = 0;
    // A finish ack is only accepted in one state. In every other state we want to reject it.
    virtual bool finishAck(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine) {return false;}
    // In every state but idle, we need to move to the Idle state and stop the thread.
    virtual bool disconnect(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine) {stateMachine->changeToIdle(); return true;}
    virtual bool action(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine, bool &callAlert, FileTransfer::AlertType &alertType, FileTransfer::SendSnapshot &s) = 0;
};

class Idle : public FileTransfer::Sender::StateMachine::State
{
    static Idle *instance;
public:
    static FileTransfer::Sender::StateMachine::State* getInstance();
    virtual bool start(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine, const std::string &fileName, bool flushable);
    virtual bool localCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine);
    virtual bool remoteCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine);
    virtual bool disconnect(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine);
    virtual bool action(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine, bool &callAlert, FileTransfer::AlertType &alertType, FileTransfer::SendSnapshot &s);
};

class Starting : public FileTransfer::Sender::StateMachine::State
{
    static Starting *instance;
public:
    static FileTransfer::Sender::StateMachine::State* getInstance();
    virtual bool localCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine);
    virtual bool remoteCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine);
    virtual bool action(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine, bool &callAlert, FileTransfer::AlertType &alertType, FileTransfer::SendSnapshot &s);
};

class SendingData : public FileTransfer::Sender::StateMachine::State
{
    static SendingData *instance;
public:
    static FileTransfer::Sender::StateMachine::State* getInstance();
    virtual bool localCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine);
    virtual bool remoteCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine); 
    virtual bool action(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine, bool &callAlert, FileTransfer::AlertType &alertType, FileTransfer::SendSnapshot &s);
};

class SendingFinish : public FileTransfer::Sender::StateMachine::State
{
    static SendingFinish *instance;
public:
    static FileTransfer::Sender::StateMachine::State* getInstance();
    virtual bool localCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine);
    virtual bool remoteCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine);
    virtual bool action(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine, bool &callAlert, FileTransfer::AlertType &alertType, FileTransfer::SendSnapshot &s);
};

class WaitingForFinishAck : public FileTransfer::Sender::StateMachine::State
{
    static WaitingForFinishAck *instance;
public:
    static FileTransfer::Sender::StateMachine::State* getInstance();
    virtual bool localCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine);
    virtual bool remoteCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine);
    virtual bool finishAck(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine); 
    virtual bool action(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine, bool &callAlert, FileTransfer::AlertType &alertType, FileTransfer::SendSnapshot &s);
};

class Finished : public FileTransfer::Sender::StateMachine::State
{
    static Finished *instance;
public:
    static FileTransfer::Sender::StateMachine::State* getInstance();
    virtual bool localCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine);
    virtual bool remoteCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine);
    virtual bool action(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine, bool &callAlert, FileTransfer::AlertType &alertType, FileTransfer::SendSnapshot &s);
};

class Cancelling : public FileTransfer::Sender::StateMachine::State
{
    static Cancelling *instance;
public:
    static FileTransfer::Sender::StateMachine::State* getInstance();
    virtual bool localCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine);
    virtual bool remoteCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine);
    virtual bool action(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine, bool &callAlert, FileTransfer::AlertType &alertType, FileTransfer::SendSnapshot &s);
};

class Cancelled : public FileTransfer::Sender::StateMachine::State
{
    static Cancelled *instance;
public:
    static FileTransfer::Sender::StateMachine::State* getInstance();
    virtual bool localCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine);
    virtual bool remoteCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine);
    virtual bool action(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine, bool &callAlert, FileTransfer::AlertType &alertType, FileTransfer::SendSnapshot &s);
};

class RemoteCancelling : public FileTransfer::Sender::StateMachine::State
{
    static RemoteCancelling *instance;
public:
    static FileTransfer::Sender::StateMachine::State* getInstance();
    virtual bool localCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine);
    virtual bool remoteCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine);
    virtual bool action(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine, bool &callAlert, FileTransfer::AlertType &alertType, FileTransfer::SendSnapshot &s);
};

Idle *Idle::instance = new Idle;

FileTransfer::Sender::StateMachine::State* Idle::getInstance()
{
    return instance;
}

bool Idle::start(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine, const std::string &fileName, const bool flushable)
{
    stateMachine->mFileName = fileName;
    stateMachine->mState = Starting::getInstance();
    stateMachine->snapshot = FileTransfer::SendSnapshot(true, stateMachine->snapshot.ch, 0, 0, 0, stateMachine->getL2CapDataSize());
    stateMachine->mPbf = flushable ? HCIACLPDU::start : HCIACLPDU::start_not_auto_flush;    
    stateMachine->mSignal.set();

    return true;
}

bool Idle::localCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine)
{
    // No file send to cancel
    return false;
}

bool Idle::remoteCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine)
{
    // The other end shouldn't be in a state to send a cancel.
    return false;
}

bool Idle::disconnect(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine)
{
    // Nothing to be done
    return false;
}

bool Idle::action(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine, bool &callAlert, FileTransfer::AlertType &alertType, FileTransfer::SendSnapshot &s)
{
    callAlert = false;
    return false;
}

Starting *Starting::instance = new Starting;

FileTransfer::Sender::StateMachine::State* Starting::getInstance()
{
    return instance;
}

bool Starting::localCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine)
{
    stateMachine->changeToIdle();
    return true;
}

bool Starting::remoteCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine)
{
    // Haven't sent anything yet, so the remote end is in error.
    return false;
}

bool Starting::action(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine, bool &callAlert, FileTransfer::AlertType &alertType, FileTransfer::SendSnapshot &s)
{
    callAlert = true;
    bool retval = true;

    if (0 == (stateMachine->mFile = fopen(stateMachine->mFileName.c_str(), "rb")))
    {
        // Couldn't open file
        alertType = FileTransfer::ALERT_SEND_FILE_ERROR;
        stateMachine->changeToIdle();
        retval = false;
    }
    else if ((0 == fseek(stateMachine->mFile, 0L, SEEK_END)) && 
             ((uint32) -1 != (stateMachine->snapshot.fileSize = ftell(stateMachine->mFile))) && 
             (0 == fseek(stateMachine->mFile, 0L, SEEK_SET)))
    {
        // Opened file successfully
        // Data send thread started successfully
        StartHelper startHelper(stateMachine->snapshot.fileSize, stateMachine->mFileName);
        startHelper.set_header(stateMachine->snapshot.ch, stateMachine->mPbf);
        startHelper.set_length();
        stateMachine->putPdu(startHelper);
#ifdef SEND_START_ACK_ENABLED
        mSendState = SEND_STATE_WAIT_FOR_START_ACK;    
#else
        alertType = FileTransfer::ALERT_SEND_START;
        stateMachine->mState = SendingData::getInstance();
#endif
    }
    else
    {
        // Error occurred
        alertType = FileTransfer::ALERT_SEND_FILE_ERROR;
        stateMachine->changeToIdle();
        retval = false;
    }

    s = stateMachine->snapshot;
    return retval;
}

SendingData *SendingData::instance = new SendingData;

FileTransfer::Sender::StateMachine::State* SendingData::getInstance()
{
    return instance;
}

bool SendingData::localCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine)
{
    stateMachine->mState = Cancelling::getInstance();
    return true;
}

bool SendingData::remoteCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine)
{
    stateMachine->mState = RemoteCancelling::getInstance();
    return true;
}

bool SendingData::action(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine, bool &callAlert, FileTransfer::AlertType &alertType, FileTransfer::SendSnapshot &s)
{
    bool retval = true;
    callAlert = true;
    std::vector<uint8> buf(stateMachine->getL2CapDataSize());

    size_t n = fread(&buf[0], 1, buf.size(), stateMachine->mFile);

    if (ferror(stateMachine->mFile) != 0)
    {
        // Error occured with reading the file. Cancel transfer.
        CancelReceiveHelper cancelReceiveHelper;
        cancelReceiveHelper.set_header(stateMachine->snapshot.ch, stateMachine->mPbf);
        cancelReceiveHelper.set_length();
        stateMachine->putPdu(cancelReceiveHelper);
        alertType = FileTransfer::ALERT_SEND_FILE_ERROR;
        stateMachine->changeToIdle();
    }
    else 
    {
        if (0 < n && n <= buf.size())
        {
            HCIACL_L2CAP_PDU_Helper fileFragment(n);
            fileFragment.set_header(stateMachine->snapshot.ch, stateMachine->mPbf);
            fileFragment.set_l2capData(&buf[0], n);
            fileFragment.set_l2capChNum(0x42);
            fileFragment.set_length();
            stateMachine->putPdu(fileFragment, true);
            stateMachine->snapshot.octetsQueued += n;
        }
         
        if (feof(stateMachine->mFile))
        {
            stateMachine->mState = SendingFinish::getInstance();
        }
        else
        {
            alertType = FileTransfer::ALERT_SEND_QUEUING_DATA;
        }
    }

    s = stateMachine->snapshot;
    return retval;
}

SendingFinish *SendingFinish::instance = new SendingFinish;

FileTransfer::Sender::StateMachine::State* SendingFinish::getInstance()
{
    return instance;
}

bool SendingFinish::localCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine)
{
    stateMachine->mState = Cancelling::getInstance();
    return true;
}

bool SendingFinish::remoteCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine)
{
    // The we've sent all the data, but haven't officially finished the transfer yet, so this is still valid.
    stateMachine->mState = RemoteCancelling::getInstance();
    return true;
}

bool SendingFinish::action(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine, bool &callAlert, FileTransfer::AlertType &alertType, FileTransfer::SendSnapshot &s)
{
    callAlert = true;
    FinishHelper finishHelper(stateMachine->snapshot.fileSize);
    finishHelper.set_header(stateMachine->snapshot.ch, stateMachine->mPbf);
    finishHelper.set_length();
    stateMachine->putPdu(finishHelper);
    alertType = FileTransfer::ALERT_SEND_QUEUED_ALL_DATA;
    stateMachine->mState = WaitingForFinishAck::getInstance();
    s = stateMachine->snapshot;
    return true;
}

WaitingForFinishAck *WaitingForFinishAck::instance = new WaitingForFinishAck;

FileTransfer::Sender::StateMachine::State* WaitingForFinishAck::getInstance()
{
    return instance;
}

bool WaitingForFinishAck::localCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine)
{
    stateMachine->mState = Cancelling::getInstance();
    return true;
}

bool WaitingForFinishAck::remoteCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine)
{
    // The remote end may not have received all the data or the finish packet, so this is valid.
    stateMachine->mState = RemoteCancelling::getInstance();
    return true;
}

bool WaitingForFinishAck::finishAck(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine)
{
    stateMachine->mState = Finished::getInstance();
    return true;
}

bool WaitingForFinishAck::action(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine, bool &callAlert, FileTransfer::AlertType &alertType, FileTransfer::SendSnapshot &s)
{
    callAlert = false;
    return true;
}

Finished *Finished::instance = new Finished;

FileTransfer::Sender::StateMachine::State* Finished::getInstance()
{
    return instance;
}

bool Finished::localCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine)
{
    // Too late to cancel. There is a small race condition here, but it shouldn't matter.
    // There might be a failure to cancel message before the transfer completion message.
    return false;
}

bool Finished::remoteCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine)
{
    // The remote end has received all data, and sent us a finish ack. As such, the the remote end is in error.
    return false;
}

bool Finished::action(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine, bool &callAlert, FileTransfer::AlertType &alertType, FileTransfer::SendSnapshot &s)
{
    callAlert = true;
    alertType = FileTransfer::ALERT_SEND_FINISHED;
    s = stateMachine->snapshot;
    stateMachine->changeToIdle();
    // Kill the thread
    return false;
}

Cancelling *Cancelling::instance = new Cancelling;

FileTransfer::Sender::StateMachine::State* Cancelling::getInstance()
{
    return instance;
}

bool Cancelling::localCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine)
{
    // Already in the middle of a cancel.
    return false;
}

bool Cancelling::remoteCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine)
{
    // Technically, the remote end has won this race. It is easier to ignore the request, and makes little real difference.
    return false;
}

bool Cancelling::action(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine, bool &callAlert, FileTransfer::AlertType &alertType, FileTransfer::SendSnapshot &s)
{
    callAlert = false;
    CancelReceiveHelper cancelReceiveHelper;
    cancelReceiveHelper.set_header(stateMachine->snapshot.ch, stateMachine->mPbf);
    cancelReceiveHelper.set_length();
    stateMachine->putPdu(cancelReceiveHelper);
    stateMachine->mState = Cancelled::getInstance();
    return true;
}

Cancelled *Cancelled::instance = new Cancelled;

FileTransfer::Sender::StateMachine::State* Cancelled::getInstance()
{
    return instance;
}

bool Cancelled::localCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine)
{
    // Already cancelled.
    return false;
}

bool Cancelled::remoteCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine)
{
    // Already cancelled
    return false;
}

bool Cancelled::action(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine, bool &callAlert, FileTransfer::AlertType &alertType, FileTransfer::SendSnapshot &s)
{
    callAlert = false;
    stateMachine->changeToIdle();
    return false;
}

RemoteCancelling *RemoteCancelling::instance = new RemoteCancelling;

FileTransfer::Sender::StateMachine::State* RemoteCancelling::getInstance()
{
    return instance;
}

bool RemoteCancelling::localCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine)
{
    // Already cancelling.
    return false;
}

bool RemoteCancelling::remoteCancel(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine)
{
    // Already cancelling.
    return false;
}

bool RemoteCancelling::action(CountedPointer<FileTransfer::Sender::StateMachine> stateMachine, bool &callAlert, FileTransfer::AlertType &alertType, FileTransfer::SendSnapshot &s)
{
    callAlert = true;
    alertType = FileTransfer::ALERT_SEND_REMOTE_CANCEL;
    s = stateMachine->snapshot;
    stateMachine->changeToIdle();
    return false;
}

FileTransfer::Sender::StateMachine::StateMachine(uint16 ch, AclEngine *aclEngine, AlertCallback *alert, L2CapSizeCallback *l2CapSizeCallback, SingleSignal &signal) :
    mSignal(signal),
    mFileName(""),
    mAclEngine(aclEngine),
    mL2CapSizeCallback(l2CapSizeCallback),
    mAlert(alert),
    mFile(0),
    mPbf(HCIACLPDU::start),
    snapshot(false, ch, 0, 0, 0, getL2CapDataSize()),
    mCs(),
    mState(Idle::getInstance()),
    senderIndex(0),
    waitingpdu(null_pdu),
    firstSent(false)
{
    mSignal.unset();
    senderIndex = mAclEngine->addSender(snapshot.ch, this);
}

bool FileTransfer::Sender::StateMachine::action(bool &callAlert, FileTransfer::AlertType &alertType, FileTransfer::SendSnapshot &s)
{
    CriticalSection::Lock lock(mCs);
    mSignal.unset();
    return mState->action(this, callAlert, alertType, s);
}

bool FileTransfer::Sender::StateMachine::start(const std::string &fileName, bool flushable)
{
    CriticalSection::Lock lock(mCs);
    return mState->start(this, fileName, flushable);
}

FileTransfer::SendSnapshot FileTransfer::Sender::StateMachine::getSnapshot() 
{
    CriticalSection::Lock lock(mCs);
    return snapshot;
}

bool FileTransfer::Sender::StateMachine::localCancel()
{
    CriticalSection::Lock lock(mCs);
    clear();
    return mState->localCancel(this);
}

bool FileTransfer::Sender::StateMachine::remoteCancel()
{
    CriticalSection::Lock lock(mCs);
    clear();
    return mState->remoteCancel(this);
}

bool FileTransfer::Sender::StateMachine::finishAck()
{
    CriticalSection::Lock lock(mCs);
    return mState->finishAck(this);
}

bool FileTransfer::Sender::StateMachine::disconnect()
{
    CriticalSection::Lock lock(mCs);
    return mState->disconnect(this);
}

CountedPointer<AclEngine::PacketQueue> FileTransfer::Sender::StateMachine::getPacketQueue(const uint16 splitSize)
{
    CriticalSection::Lock lock(mCs);
    if (waitingpdu.size() == 0)
    {
        return 0;
    }

    // Mark the first acl packet from the l2cap packet, so that we don't count the l2cap header.
    firstSent = true;
    splitPdu = waitingpdu.split(splitSize);
    splitData = waitingData;
    waitingpdu = null_pdu;
    mSignal.set();
    return new Queue(this);
}

bool FileTransfer::Sender::StateMachine::getPdu(HCIACLPDU &pdu)
{
    CriticalSection::Lock lock(mCs);
    if (splitPdu.empty())
    {
        return false;
    }

    pdu = splitPdu.front();
    splitPdu.pop_front();
    if (splitData)
    {
        snapshot.octetsSent += pdu.get_length() - (firstSent? L2CAP_HEADER_SIZE : 0);
    }
    firstSent = false;
    return true;
}

void FileTransfer::Sender::StateMachine::putPdu(const ExtendedHCIACLPDU_Helper &pdu, bool isData)
{
    waitingpdu = pdu;
    waitingData = isData;
}

uint16 FileTransfer::Sender::StateMachine::getL2CapDataSize()
{
    uint16 size(MAX_L2CAP_SIZE);

    if (mL2CapSizeCallback)
    {
        size = mL2CapSizeCallback->getL2CapSize();
        if (size < 5) size = 5;
        else if (size > MAX_L2CAP_SIZE) size = MAX_L2CAP_SIZE;
    }
    return size - L2CAP_HEADER_SIZE;
}

void FileTransfer::Sender::StateMachine::alert(const FileTransfer::AlertType alertType, const FileTransfer::SendSnapshot &snapshot)
{
    if (mAlert)
    {
        mAlert->fileTransferCallback(alertType, snapshot, FileTransfer::ReceiveSnapshot());
    }
}

void FileTransfer::Sender::StateMachine::changeToIdle()
{
    if (mFile)
    {
        fclose(mFile);
        mFile = 0;
    }
    snapshot.sendActive = false;
    mState = Idle::getInstance();
    mSignal.unset();
}

void FileTransfer::Sender::StateMachine::clear()
{
    waitingpdu = null_pdu;
    splitPdu.clear();
    mSignal.set();
}

FileTransfer::Sender::Queue::Queue(CountedPointer<StateMachine> stateMachine) :
    mStateMachine(stateMachine)
{
    
}

bool FileTransfer::Sender::Queue::getPdu(HCIACLPDU &pdu)
{
    return mStateMachine->getPdu(pdu);
}

