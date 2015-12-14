#ifndef ACL_ENGINE_H
#define ACL_ENGINE_H

#include "common/types.h"
#include "common/countedpointer.h"
#include "hciacl_l2cap_pdu.h"
class BlueCoreDeviceController_newStyle;
class HCI_EV_NUMBER_COMPLETED_PKTS_T_PDU;
class HCI_READ_BUFFER_SIZE_RET_T_PDU;
#include <list>

class AclEngine
{
public:
    class Callback : public Counter
    {
    public:
        virtual void l2CapPacketEndedPrematurely(size_t defecit) = 0;
        virtual bool receive(const HCIACL_L2CAP_PDU &pdu, size_t l2CapLengthRemaining) = 0;
    };

    class PacketQueue : public Counter
    {
    public:
        virtual bool getPdu(HCIACLPDU &pdu) = 0;
    };

    class Sender : public Counter
    {
    public:
        virtual CountedPointer<PacketQueue> getPacketQueue(uint16 splitSize) = 0;
    };

    class DefaultCallback
    {
    public:
        virtual void receive(const HCIACLPDU &pdu) = 0;
    };

    AclEngine(BlueCoreDeviceController_newStyle *ldc, DefaultCallback *dcb);
    bool connect(uint32 chNum);
    bool disconnect(uint32 chNum);
    bool nocp(const HCI_EV_NUMBER_COMPLETED_PKTS_T_PDU& pdu);
    bool setBufferSizes(const HCI_READ_BUFFER_SIZE_RET_T_PDU& pdu);
    bool send(ExtendedHCIACLPDU_Helper helper);
    void receive(const HCIACLPDU &pdu);
    bool setCallback(uint16 aclch, uint16 l2capch, Callback *callback);
    void unsetCallback(uint16 aclch, uint16 l2capch);
    bool clear(uint16 aclch);
    bool sizeOfQueue(uint16 aclch, size_t *size);
    void sentRbs();
    void setQueueUpperThreshold(uint32 upperThreshold);
    void setQueueLowerThreshold(uint32 lowerThreshold);
    uint32 getQueueUpperThreshold();
    uint32 getQueueLowerThreshold();
    bool isAclChannelOpen(uint32 ch);
    int addSender(uint16 ch, CountedPointer<Sender> sender);
    void removeSender(uint16 ch, int index);
    void kickScheduler();
private: 
    class Implementation;   
    Implementation *pImpl;
};

#endif /* ACL_ENGINE_H*/
