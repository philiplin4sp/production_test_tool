#include "file_transfer.h"
#include "common/countedpointer.h"
#include "thread/critical_section.h"
#include "file_sender.h"
#include "file_receiver.h"
#include <cassert>
#include "file_pdus.h"
#include "control_type.h"
#include "channel_enum.h"

class FileTransfer::Implementation : public AclEngine::Callback
{
public:
    enum ControlState {IDLE, RECEIVING_START};
    Implementation(uint16 lAclChNum, 
                   AclEngine *lAclEngine, 
                   AlertCallback *lAlertCallBack,
                   L2CapSizeCallback *lL2CapSizeCallback);
    Sender mFileSender;
    uint16 mAclChNum;
    AclEngine *mAclEngine;
    bool mSendData;
    CriticalSection mCs;
    CountedPointer<Receiver> mFileReceiver;
    AlertCallback *mAlertCallback;
    L2CapSizeCallback *mL2CapSizeCallback;

    virtual void l2CapPacketEndedPrematurely(size_t defecit);
    virtual bool receive(const HCIACL_L2CAP_PDU &pdu, 
                         size_t l2CapLengthRemaining);
private:    
    ControlState mControlState;

    bool handleControl(const HCIACL_L2CAP_PDU &pdu, 
                       size_t l2CapLengthRemaining);
    bool receivingStart(const HCIACL_L2CAP_PDU &pdu);

};

FileTransfer::FileTransfer(uint16 lAclChNum, 
                           AclEngine *lAclEngine, 
                           AlertCallback *lAlertCallBack,
                           L2CapSizeCallback *lL2CapSizeCallback) :
    pImpl(new Implementation(lAclChNum, lAclEngine, lAlertCallBack, lL2CapSizeCallback))
{
    pImpl->inc();
}

FileTransfer::FileTransfer(const FileTransfer &from) :
    pImpl(from.pImpl)
{
    pImpl->inc();
}

FileTransfer &FileTransfer::operator=(const FileTransfer &from)
{
    if (from.pImpl != pImpl)
    {
        pImpl->dec();
        pImpl = from.pImpl;
        pImpl->inc();
    }
    return *this;
}

FileTransfer::~FileTransfer()
{
    pImpl->dec();
}

bool FileTransfer::startSend(const std::string &fileName, bool flushable)
{
    return pImpl->mFileSender.start(fileName, flushable);
}

bool FileTransfer::cancelSend(uint32 &fileSize, uint32 &bytesSent)
{
    return pImpl->mFileSender.localCancel(fileSize, bytesSent);
}

FileTransfer::SendSnapshot FileTransfer::getSendSnapshot()
{
    return pImpl->mFileSender.getSnapshot();
}

bool FileTransfer::startReceive(const std::string &fileName, bool flushable)
{
    if (pImpl->mFileReceiver && pImpl->mFileReceiver->isDeletable())
    {
        pImpl->mFileReceiver = 0;
    }

    if (pImpl->mFileReceiver == 0)
    {
        pImpl->mFileReceiver = Receiver::NewReceiver(pImpl->mAclChNum, 
                                                     pImpl->mAclEngine, 
                                                     fileName, 
                                                     flushable,
                                                     pImpl->mCs,
                                                     pImpl->mAlertCallback);
        return pImpl->mFileReceiver != 0;
    }
    else
    {
        return false;
    }
}

bool FileTransfer::cancelReceive(uint32 *fileSize, uint32 *bytesReceived)
{
    ReceiveSnapshot snapshot = getReceiveSnapshot();

    if (snapshot.receiveActive)
    {
        pImpl->mAclEngine->unsetCallback(pImpl->mAclChNum, RECEIVE_CHANNEL);
        *fileSize = snapshot.fileSize;
        *bytesReceived = snapshot.octetsReceived;
        pImpl->mFileReceiver->localCancel();
        pImpl->mFileReceiver = 0;
    }

    return snapshot.receiveActive;
}

FileTransfer::ReceiveSnapshot FileTransfer::getReceiveSnapshot()
{
    CriticalSection::Lock lock(pImpl->mCs);
    ReceiveSnapshot snapshot;
    snapshot.receiveActive = false;
    snapshot.ch = pImpl->mAclChNum;
    if (pImpl->mFileReceiver && !pImpl->mFileReceiver->isDeletable())
    {
        snapshot = pImpl->mFileReceiver->getSnapshot();
    }
    return snapshot;
}

void FileTransfer::disconnect()
{
    pImpl->mFileSender.disconnect();

    if (pImpl->mFileReceiver)
    {
        pImpl->mFileReceiver->disconnect();
        pImpl->mFileReceiver = 0;
    }
}

uint32 FileTransfer::getAclCh()
{
    return pImpl->mAclChNum;
}

FileTransfer::Implementation::Implementation(uint16 lAclChNum, 
                                             AclEngine *lAclEngine, 
                                             AlertCallback *lAlertCallBack,
                                             L2CapSizeCallback *lL2CapSizeCallback) :
    mFileSender(lAclChNum, lAclEngine, lAlertCallBack, lL2CapSizeCallback),
    mAclChNum(lAclChNum),
    mAclEngine(lAclEngine),
    mSendData(false),
    mAlertCallback(lAlertCallBack),
    mL2CapSizeCallback(lL2CapSizeCallback),
    mControlState(IDLE)
{
    lAclEngine->setCallback(mAclChNum, CONTROL_CHANNEL, this);
}

void FileTransfer::Implementation::l2CapPacketEndedPrematurely(size_t defecit)
{
    mFileReceiver->errorStartReceive();
    mControlState = IDLE;
}

bool FileTransfer::Implementation::receive(const HCIACL_L2CAP_PDU &pdu, 
                                           size_t l2CapLengthRemaining)
{
    assert(mControlState == IDLE ||
           mControlState == RECEIVING_START);
    switch (mControlState)
    {
    case IDLE:
        // Not expecting a cont.
        return handleControl(pdu, l2CapLengthRemaining);
    case RECEIVING_START:
        // Expecting a cont packet for C_START
        return receivingStart(pdu);
    default:    
        // State machine has gone rather wrong
        return false;
    }
}

bool FileTransfer::Implementation::handleControl(const HCIACL_L2CAP_PDU &pdu, 
                                             size_t l2CapLengthRemaining)
{
    // PDUs coming through here will always be start packets
    switch (ControlHelper(pdu).get_ControlType())
    {
    case C_START:
        if (l2CapLengthRemaining >= 5 && 
            mFileReceiver && 
            !mFileReceiver->isDeletable() && 
            mFileReceiver->beginStartReceive(pdu, l2CapLengthRemaining))
        {
            if (!mFileReceiver->isStartReceiveComplete())
            {
                mControlState = RECEIVING_START;
            }

            return true;
        }
        else
        {
            return false;
        }
        break;
#ifdef SEND_START_ACK_ENABLED
    case START_ACK:    
        if (l2CapLengthRemaining == 1 && 
            mFileSender && 
            !mFileSender->isDeletable() &&mFileSender->startAck())
        {
            return true;
        }
        else
        {
            // File Sender is not trying to start transfer
            delete mFileSender;
            mFileSender = 0;
            return false;
        }
        break;
#endif
    case FINISH:
        {
            bool accepted = l2CapLengthRemaining == 5 && 
                            mFileReceiver && 
                            !mFileReceiver->isDeletable() && 
                            mFileReceiver->finishReceive(pdu);
            mFileReceiver = 0;
            return accepted;
        }
        break;
    case FINISH_ACK:
        {
            bool accepted = l2CapLengthRemaining == 1 && 
                            mFileSender.finishAck();
            return accepted;
        }
        break;
    case CANCEL_SEND:
        {
            bool accepted = l2CapLengthRemaining == 1 && 
                            mFileSender.remoteCancel();
            return accepted;
        }
        break;
    case CANCEL_RECEIVE:
        {
            bool accepted = l2CapLengthRemaining == 1 && 
                            mFileReceiver && 
                            !mFileReceiver->isDeletable() && 
                            mFileReceiver->remoteCancel();
            if (accepted) mFileReceiver = 0;
            return accepted;
        }
        break;
    default:
        // Received gibberish. Reject it.
        return false;
        break;
    }
}

bool FileTransfer::Implementation::receivingStart(const HCIACL_L2CAP_PDU &pdu)
{
    if (mFileReceiver == 0)
    {
        return false;
    }
    mFileReceiver->continueStartReceive(pdu);
    if (mFileReceiver->isStartReceiveComplete())
    {
        mControlState = IDLE;
    }
    else
    {
        mControlState = RECEIVING_START;
    }
    return true;
}
