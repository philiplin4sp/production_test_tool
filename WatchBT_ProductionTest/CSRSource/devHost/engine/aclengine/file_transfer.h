#ifndef FILE_TRANSFER_H
#define FILE_TRANSFER_H

#include "common/types.h"
#include "aclengine.h"
#include <string>

class FileTransfer
{
public:
    class Sender;
    class Receiver;

    struct SendSnapshot
    {
        // Don't care what data, as long as we mark that a send isn't running.
        SendSnapshot() : sendActive(false) {}

        SendSnapshot(bool sa,
                     uint32 c,
                     uint32 os,
                     uint32 oq,
                     uint32 fs,
                     uint32 ls) :
            sendActive(sa),
            ch(c),
            octetsSent(os),
            octetsQueued(oq),
            fileSize(fs),
            l2CapSize(ls) {}
        bool sendActive;
        uint32 ch;
        uint32 octetsSent;
        uint32 octetsQueued;
        uint32 fileSize;
        uint32 l2CapSize;
    };

    struct ReceiveSnapshot
    {
        bool receiveActive;
        uint32 ch;
        uint32 timeElapsed;
        uint32 octetsReceived;
        uint32 fileSize;
        uint32 l2CapPacketsTruncated; 
        uint32 l2CapPacketTruncationOctetsLost;
    };

    enum AlertType
    {
        ALERT_RECEIVE_START,
        ALERT_RECEIVE_FINISH,
        ALERT_RECEIVE_DATA,
        ALERT_RECEIVE_DATA_START,
        ALERT_RECEIVE_FILE_ERROR,
        ALERT_RECEIVE_REMOTE_CANCEL,
        ALERT_RECEIVE_UNEXPECTED_PACKET_RECEIVED,
        ALERT_SEND_START,
        ALERT_SEND_REMOTE_CANCEL,
        ALERT_SEND_FILE_ERROR,
        ALERT_SEND_QUEUING_DATA,
        ALERT_SEND_QUEUED_ALL_DATA,
        ALERT_SEND_FINISHED,
        ALERT_SEND_UNEXPECTED_PACKET_RECEIVED
    };
   
    class AlertCallback
    {
    public:
        virtual void fileTransferCallback(AlertType,
                                          SendSnapshot,
                                          ReceiveSnapshot) = 0;
    };

    class L2CapSizeCallback
    {
    public:
        virtual uint16 getL2CapSize() = 0;
    };
 
    FileTransfer(uint16 lAclChNum, 
                 AclEngine *lAclEngine, 
                 AlertCallback *lAlertCallBack,
                 L2CapSizeCallback *lL2CapSizeCallback = 0);
    FileTransfer(const FileTransfer &from);
    FileTransfer &operator=(const FileTransfer &from);
    ~FileTransfer();

    // Interface to file send
    bool startSend(const std::string &fileName, bool flushable);
    bool cancelSend(uint32 &fileSize, uint32 &bytesSent);
    SendSnapshot getSendSnapshot();

    // Interface to file receive
    bool startReceive(const std::string &fileName, bool flushable);
    bool cancelReceive(uint32 *fileSize, uint32 *bytesReceived);
    ReceiveSnapshot getReceiveSnapshot();

    void disconnect();
    uint32 getAclCh();
private:
    class Implementation;
    Implementation *pImpl;
};

#endif
