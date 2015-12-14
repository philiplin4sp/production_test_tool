#ifndef ACL_CHANNEL_H
#define ACL_CHANNEL_H

#include "acl_channel_list.h"

#include <list>
#include <map>
#include "common/types.h"
#include "aclscheduler.h"
#include "hcipacker/hcidatapdu.h"
#include "common/countedpointer.h"
#include "thread/safe_counter.h"

typedef void (* DefaultHandler)(const HCIACLPDU& pdu);
typedef std::map<uint16, CountedPointer<AclEngine::Callback> > CallbackMap;

class AclEngine::Implementation::ChannelList::Channel : public SafeCounter
{
public:
    class Send
    {
    public:
        Send(uint32 ch, uint32 lLowerThreshold, uint32 lUpperThreshold);
        void add(ExtendedHCIACLPDU_Helper pdu);
        bool isEmpty();
        bool nocp(uint16 numPackets);
        HCIACLPDU getPdu(bool &isSendablePdu);
        void clear();
        size_t sizeOfQueue();
        uint32 getTokensUsed();
        void setBufferSizes(HCI_READ_BUFFER_SIZE_RET_T_PDU pdu);
        int addSender(CountedPointer<AclEngine::Sender> &sender);
        void removeSender(int index);
    private:
        void fillQueue();
        CriticalSection mCs;
        uint32 mCh;
        std::deque<CountedPointer<AclEngine::PacketQueue> > queue;
        std::deque<ExtendedHCIACLPDU_Helper> pending_queue;
        uint32 tokensUsed;
        bool signalSet;
        uint32 lowerThreshold;
        uint32 upperThreshold;
        CallbackMap mCallbacks;
        size_t size;
        HCI_READ_BUFFER_SIZE_RET_T_PDU rbs_pdu;

        std::vector<CountedPointer<AclEngine::Sender> > senders;
        int currentSender;
    } sender;

    class Receive
    {
    public:
        Receive(uint32 ch, DefaultCallback *dcb);
        void receive(const HCIACLPDU &pdu);
        bool setCallback(uint16 l2capch, CountedPointer<Callback> callback);
        bool unsetCallback(uint16 l2capch);
        enum State {IDLE, PROCESSING};
    private:
        CriticalSection mCs;
        uint32 mCh;
        CallbackMap mCallbacks;
        DefaultCallback *defaultCallback;
        uint16 l2CapChannel;
        size_t l2CapLengthRemaining;
        State state;
        State idle(const HCIACLPDU &pdu);
        State continueProcessing(const HCIACL_L2CAP_PDU &pdu);
        State startProcessing(const HCIACLPDU &pdu);
        State callDefaultHandler(const HCIACL_L2CAP_PDU &pdu);
        State process(const HCIACL_L2CAP_PDU &pdu);
        void reportL2CapPacketEndedPrematurely();
        CountedPointer<AclEngine::Callback> getCallback();
    } receiver;

    Channel(uint32 ch, DefaultCallback *dcb, uint32 lLowerThreshold, uint32 lUpperThreshold);
    virtual ~Channel();
};

#endif //ACL_CHANNEL
