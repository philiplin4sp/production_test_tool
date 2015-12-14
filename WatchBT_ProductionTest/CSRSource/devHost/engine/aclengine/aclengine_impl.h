#ifndef ACL_ENGINE_IMPL_H
#define ACL_ENGINE_IMPL_H

#include "aclengine.h"
#include "hciacl_l2cap_pdu.h"
#include "thread/critical_section.h"

class AclEngine::Implementation
{
public:
    class Scheduler;
    class ChannelList;
    Implementation(BlueCoreDeviceController_newStyle *ldc, DefaultCallback *dcb);
    bool connect(uint32 chNum);
    bool disconnect(uint32 chNum);
    bool nocp(const HCI_EV_NUMBER_COMPLETED_PKTS_T_PDU& pdu);
    bool setBufferSizes(const HCI_READ_BUFFER_SIZE_RET_T_PDU& pdu);
    bool send(ExtendedHCIACLPDU_Helper helper);
    void receive(const HCIACL_L2CAP_PDU &pdu);
    bool setCallback(uint16 aclch, uint16 l2capch, Callback *callback);
    bool unsetCallback(uint16 aclch, uint16 l2capch);
    bool clear(uint16 aclch);
    bool sizeOfQueue(uint16 aclch, size_t *size);
    void sentRbs();
    uint32 getQueueUpperThreshold();
    uint32 getQueueLowerThreshold();
    bool isAclChannelOpen(uint16 ch);
    int addSender(uint16 ch, CountedPointer<AclEngine::Sender> sender);
    void removeSender(uint16 ch, int index);
    void kickScheduler();

private:
    BlueCoreDeviceController_newStyle *mdc;
    Scheduler *mScheduler;
    ChannelList *channelList;
    DefaultCallback *mdcb;
    CriticalSection p;
};

#endif //ACL_ENGINE_IMPL_H
