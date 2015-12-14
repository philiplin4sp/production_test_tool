#include "file_receiver.h"
#include "file_pdus.h"
#include <cstdio>

CountedPointer<FileTransfer::Receiver> FileTransfer::Receiver::NewReceiver(
                                                uint16 lAclChNum,
                                                AclEngine *lAclEngine,
                                                const std::string &lFileName, 
                                                bool flushable,
                                                CriticalSection &cs,
                                                AlertCallback *alert)
{
    FILE *file = fopen(lFileName.c_str(), "wb");
    if (0 == file)
    {
        return 0;
    }
    else
    {
        Receiver *fileReceiver = new Receiver(lAclChNum, 
                                              lAclEngine, 
                                              lFileName, 
                                              file, 
                                              flushable, 
                                              cs,
                                              alert);
            
        CountedPointer<Receiver> cp(fileReceiver);
         
        if (!lAclEngine->setCallback(lAclChNum, RECEIVE_CHANNEL, fileReceiver))
        {
            return 0;
        }
        else
        {
            return cp;
        }
    }
}

FileTransfer::Receiver::Receiver(uint16 lAclChNum,
                                 AclEngine *lAclEngine, 
                                 const std::string &lFileName, 
                                 FILE *file, 
                                 bool flushable,
                                 CriticalSection &cs,
                                 AlertCallback *alert) :
    mAclChNum(lAclChNum),
    mAclEngine(lAclEngine),
    mFileName(lFileName),
    mReceiveState(RECEIVE_STATE_IDLE),
    mFileSize(0),
    sizeReceived(0),
    mL2CapLengthRemaining(0),
    mFile(file),
    finalTimeElapsed(0),
    stopWatch(0),
    l2CapPacketsTruncated(0),
    l2CapPacketTruncationOctetsLost(0),
    mCs(cs),
    mAlert(alert)
{
    mPbf = flushable ? HCIACLPDU::start : HCIACLPDU::start_not_auto_flush;
}

FileTransfer::Receiver::~Receiver()
{

}

bool FileTransfer::Receiver::beginStartReceive(const HCIACL_L2CAP_PDU &pdu, size_t l2CapLengthRemaining)
{
    bool accepted;

    FileTransfer::ReceiveSnapshot snapshot;
    FileTransfer::AlertType alertType;

    switch (mReceiveState)
    {
    case RECEIVE_STATE_IDLE:
        mL2CapLengthRemaining = l2CapLengthRemaining;
        mReceiveState = RECEIVE_STATE_RECEIVING_START;
        mFileSize = StartStartPdu(pdu).getFileSize();
        mL2CapLengthRemaining -= (pdu.get_l2capLength());
        if (isStartReceiveComplete())
        {
            mReceiveState = RECEIVE_STATE_RECEIVED_START;
#ifdef SEND_START_ACK_ENABLED
            StartAckHelper ack;
            ack.set_header(mAclChNum, mPbf);
            ack.set_length();
            mAclEngine->send(ack);
#endif
            alertType = FileTransfer::ALERT_RECEIVE_START;
            snapshot = getSnapshot();
        }
        accepted =  true;
        break;
    case RECEIVE_STATE_RECEIVING_START:
    case RECEIVE_STATE_RECEIVED_START:
    case RECEIVE_STATE_RECEIVING_DATA:
        mReceiveState = RECEIVE_STATE_FINISHED;
        alertType = FileTransfer::ALERT_RECEIVE_UNEXPECTED_PACKET_RECEIVED;
        snapshot = getSnapshot();
        accepted = false;
    case RECEIVE_STATE_FINISHED:
        accepted =false;
        break;
    }

    alert(alertType, FileTransfer::SendSnapshot(), snapshot);

    return accepted;
}

bool FileTransfer::Receiver::continueStartReceive(const HCIACL_L2CAP_PDU &pdu)
{
    bool accepted;

    switch (mReceiveState)
    {
    case RECEIVE_STATE_IDLE:
    case RECEIVE_STATE_RECEIVING_START:
        mL2CapLengthRemaining -= pdu.get_length();
        if (isStartReceiveComplete())
        {
#ifdef SEND_START_ACK_ENABLED
            StartAckHelper ack;
            ack.set_header(mAclChNum, mPbf);
            ack.set_length();
            mAclEngine->send(ack);
#endif
            mReceiveState = RECEIVE_STATE_RECEIVED_START;
        }
        accepted = true;
        break;
    case RECEIVE_STATE_RECEIVED_START:  
    case RECEIVE_STATE_RECEIVING_DATA:
        mReceiveState = RECEIVE_STATE_FINISHED;
        accepted = false;
    case RECEIVE_STATE_FINISHED:
        accepted = false;
        break;
    }

    return accepted;
}

void FileTransfer::Receiver::errorStartReceive()
{
    StartNakHelper nak;
    nak.set_header(mAclChNum, mPbf);
    nak.set_length();
    mAclEngine->send(nak);
}

bool FileTransfer::Receiver::isStartReceiveComplete()
{
    return 0 == mL2CapLengthRemaining;
}

bool FileTransfer::Receiver::cancel(uint32 *fileSize, uint32 *bytesReceived)
{
    CancelSendHelper cancel;
    cancel.set_header(mAclChNum, mPbf);
    cancel.set_length();
    mAclEngine->send(cancel);
    *fileSize = mFileSize;
    *bytesReceived = sizeReceived;
    return true;
}

bool FileTransfer::Receiver::receive(const HCIACL_L2CAP_PDU &pdu, size_t l2CapLengthRemaining) 
{
    FileTransfer::ReceiveSnapshot snapshot;
    FileTransfer::AlertType alertType;
    bool accepted;

    {
        CriticalSection::Lock lock(mCs);

        switch (mReceiveState)
        {
        case RECEIVE_STATE_IDLE:
        case RECEIVE_STATE_RECEIVING_START:
            mReceiveState = RECEIVE_STATE_FINISHED;
            accepted = false;
            alertType = FileTransfer::ALERT_RECEIVE_UNEXPECTED_PACKET_RECEIVED;
            snapshot = getSnapshot();
            break;
        case RECEIVE_STATE_RECEIVED_START:
            {
                stopWatch = new StopWatch;

                if (fwrite(pdu.get_l2capDataPtr(), 1, pdu.get_l2capLength(), mFile) != pdu.get_l2capLength())
                {
                    CancelSendHelper cancel;
                    cancel.set_header(mAclChNum, mPbf);
                    cancel.set_length();
                    mAclEngine->send(cancel);
                    fclose(mFile);
                    mFile = 0;
                    alertType = FileTransfer::ALERT_RECEIVE_FILE_ERROR;
                    snapshot = getSnapshot();
                    mReceiveState = RECEIVE_STATE_FINISHED;
                    accepted = true;
                }
                else
                {
                    sizeReceived += pdu.get_l2capLength();
                    alertType = FileTransfer::ALERT_RECEIVE_DATA_START;
                    snapshot = getSnapshot();
                    accepted = true;
                    mReceiveState = RECEIVE_STATE_RECEIVING_DATA;
                }
            }
            break;
        case RECEIVE_STATE_RECEIVING_DATA:
            {

                if (fwrite(pdu.get_l2capDataPtr(), 1, pdu.get_l2capLength(), mFile) != pdu.get_l2capLength())
                {
                    CancelSendHelper cancel;
                    cancel.set_header(mAclChNum, mPbf);
                    cancel.set_length();
                    mAclEngine->send(cancel);
                    fclose(mFile);
                    mFile = 0;
                    alertType = FileTransfer::ALERT_RECEIVE_FILE_ERROR;
                    snapshot = getSnapshot();
                    mReceiveState = RECEIVE_STATE_FINISHED;
                }
                else
                {
                    sizeReceived += pdu.get_l2capLength();
                    alertType = FileTransfer::ALERT_RECEIVE_DATA;
                    snapshot = getSnapshot();
                    accepted = true;
                }
            }
            break;
        }
    }

    alert(alertType, FileTransfer::SendSnapshot(), snapshot);
    return accepted;
}

bool FileTransfer::Receiver::finishReceive(const ControlHelper &helper)
{
    bool accepted;

    FileTransfer::ReceiveSnapshot snapshot;
    FileTransfer::AlertType alertType;

    switch (mReceiveState)
    {
    case RECEIVE_STATE_IDLE:
    case RECEIVE_STATE_RECEIVING_START:
        mReceiveState = RECEIVE_STATE_FINISHED;
        accepted = false;
        break;
    case RECEIVE_STATE_RECEIVED_START:
        {
            // Finished with 0 bytes transferred
            mAclEngine->unsetCallback(mAclChNum, RECEIVE_CHANNEL);
            FinishAckHelper finishAck;
            finishAck.set_header(mAclChNum, mPbf);
            finishAck.set_length();
            mAclEngine->send(finishAck);
            alertType = FileTransfer::ALERT_RECEIVE_DATA_START;
            snapshot = getSnapshot();
            alert(alertType, FileTransfer::SendSnapshot(), snapshot);
            alertType = FileTransfer::ALERT_RECEIVE_FINISH;
            alert(alertType, FileTransfer::SendSnapshot(), snapshot);
            fclose(mFile);
            finalTimeElapsed = 0;
            mReceiveState = RECEIVE_STATE_FINISHED;
            accepted = true;
        }
        break;
    case RECEIVE_STATE_RECEIVING_DATA:
        {
            mAclEngine->unsetCallback(mAclChNum, RECEIVE_CHANNEL);
            FinishAckHelper finishAck;
            finishAck.set_header(mAclChNum, mPbf);
            finishAck.set_length();
            mAclEngine->send(finishAck);
            alertType = FileTransfer::ALERT_RECEIVE_FINISH;
            snapshot = getSnapshot();
            alert(alertType, FileTransfer::SendSnapshot(), snapshot);
            fclose(mFile);
            finalTimeElapsed = stopWatch->duration();
            mReceiveState = RECEIVE_STATE_FINISHED;
            delete stopWatch;
            stopWatch = 0;
            accepted = true;
        }
        break;
    case RECEIVE_STATE_FINISHED:
        accepted = false;
        break;
    }
    return accepted;
}

void FileTransfer::Receiver::l2CapPacketEndedPrematurely(size_t defecit) 
{
    l2CapPacketsTruncated++;
    l2CapPacketTruncationOctetsLost += defecit;
}

bool FileTransfer::Receiver::isDeletable()
{
    return RECEIVE_STATE_FINISHED == mReceiveState;
}

void FileTransfer::Receiver::stopSending()
{
    // Do nothing
}

void FileTransfer::Receiver::startSending()
{
    // Do nothing
}

void FileTransfer::Receiver::disconnect()
{

}

bool FileTransfer::Receiver::localCancel()
{
    bool accepted;

    switch (mReceiveState)
    {
    case RECEIVE_STATE_IDLE:
        accepted = true;
        mReceiveState = RECEIVE_STATE_FINISHED;
        break;
    case RECEIVE_STATE_RECEIVED_START:    
    case RECEIVE_STATE_RECEIVING_START:
        {
            CancelSendHelper cancelSend;
            cancelSend.set_header(mAclChNum, mPbf);
            cancelSend.set_length();
            mAclEngine->send(cancelSend);
            mReceiveState = RECEIVE_STATE_FINISHED;
            accepted = true;
        }
        break;
    case RECEIVE_STATE_RECEIVING_DATA:
        {
            CancelSendHelper cancelSend;
            cancelSend.set_header(mAclChNum, mPbf);
            cancelSend.set_length();
            mAclEngine->send(cancelSend);
            mReceiveState = RECEIVE_STATE_FINISHED;
            fclose(mFile);
            accepted = true;
        }
        break;
    case RECEIVE_STATE_FINISHED:
        accepted = false;
        break;
    }
    return accepted;
}

bool FileTransfer::Receiver::remoteCancel()
{
    bool accepted;
    FileTransfer::AlertType alertType;
    FileTransfer::ReceiveSnapshot snapshot;

    switch (mReceiveState)
    {
    case RECEIVE_STATE_IDLE:
        accepted = false;
        break;
    case RECEIVE_STATE_RECEIVING_START:
        accepted = false;
        break;
    case RECEIVE_STATE_RECEIVED_START:
        
        break;
    case RECEIVE_STATE_RECEIVING_DATA:
        accepted = true;
        fclose(mFile);
        alertType = FileTransfer::ALERT_RECEIVE_REMOTE_CANCEL;
        snapshot = getSnapshot();
        alert(alertType, FileTransfer::SendSnapshot(), snapshot);
        mAclEngine->unsetCallback(mAclChNum, RECEIVE_CHANNEL);
        break;
    case RECEIVE_STATE_FINISHED:
        accepted = false;
        break;
    }
    return accepted;
}

FileTransfer::ReceiveSnapshot FileTransfer::Receiver::getSnapshot()
{
    FileTransfer::ReceiveSnapshot snapshot;
    snapshot.receiveActive = true;
    snapshot.ch = mAclChNum;
    if (stopWatch)
    {
        snapshot.timeElapsed = stopWatch->duration();
    }
    else
    {
        snapshot.timeElapsed = 0;
    }
    snapshot.octetsReceived = sizeReceived;
    snapshot.fileSize = mFileSize;
    snapshot.l2CapPacketsTruncated = l2CapPacketsTruncated;
    snapshot.l2CapPacketTruncationOctetsLost = l2CapPacketTruncationOctetsLost;
    return snapshot;
}

void FileTransfer::Receiver::alert(
                                FileTransfer::AlertType alertType, 
                                FileTransfer::SendSnapshot sendSnapshot,
                                FileTransfer::ReceiveSnapshot receiveSnapshot)
{
    if (mAlert)
    {
        mAlert->fileTransferCallback(alertType, 
                                     sendSnapshot, 
                                     receiveSnapshot);
    }
}
