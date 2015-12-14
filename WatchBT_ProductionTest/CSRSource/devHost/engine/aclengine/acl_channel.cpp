#include "acl_channel.h"
#include <cassert>
#include <cmath>

AclEngine::Implementation::ChannelList::Channel::Channel(uint32 lChNum, DefaultCallback *dcb, uint32 lLowerThreshold, uint32 lUpperThreshold) :
    sender(lChNum, lLowerThreshold, lUpperThreshold),
    receiver(lChNum, dcb)
{   
}

AclEngine::Implementation::ChannelList::Channel::~Channel()
{
}


AclEngine::Implementation::ChannelList::Channel::Send::Send(uint32 ch, uint32 lLowerThreshold, uint32 lUpperThreshold) :
    mCh(ch),
    tokensUsed(0),
    signalSet(false),
    lowerThreshold(lLowerThreshold),
    upperThreshold(lUpperThreshold),
    size(0),
    currentSender(0)
{    
}

int AclEngine::Implementation::ChannelList::Channel::Send::addSender(CountedPointer<AclEngine::Sender> &sender)
{
    CriticalSection::Lock lock(mCs);
    for (int i = 0; i < senders.size(); ++i)
    {
        if (!senders[i])
        {
            senders[i] = sender;
            return i;
        }
    }
    
    senders.push_back(sender);
    return senders.size() - 1;
}

void AclEngine::Implementation::ChannelList::Channel::Send::removeSender(int index)
{
    CriticalSection::Lock lock(mCs);
    senders[index] = 0;
}

bool AclEngine::Implementation::ChannelList::Channel::Send::nocp(uint16 numPackets)
{
    CriticalSection::Lock lock(mCs);
    if (tokensUsed < numPackets)
    {
        // Completed more packets on this channel than we sent out. Set to 0 and alert application.
        tokensUsed = 0;
        return false;
    }
    else
    {
        tokensUsed -= numPackets;
        return true;
    }
}

void AclEngine::Implementation::ChannelList::Channel::Send::add(ExtendedHCIACLPDU_Helper pdu)
{
    CriticalSection::Lock lock(mCs);

    // Just tack it on the end of the queue.
    pending_queue.push_back(pdu);
}

bool AclEngine::Implementation::ChannelList::Channel::Send::isEmpty()
{
    CriticalSection::Lock lock(mCs);
    return 0 == size;
}

// Wrapper for packets to be sent that are just individual packet (such as 
// when sent with the acl command in btcli.
class IndividualPacketQueue : public AclEngine::PacketQueue
{
public:
    IndividualPacketQueue(const ExtendedHCIACLPDU_Helper &pdu, uint16 size);
    virtual bool getPdu(HCIACLPDU &pdu);
private:
    std::deque<HCIACLPDU> queue;
};

IndividualPacketQueue::IndividualPacketQueue(const ExtendedHCIACLPDU_Helper &pdu, const uint16 size) :
    queue(pdu.split(size))
{
}

bool IndividualPacketQueue::getPdu(HCIACLPDU &pdu)
{
    if (queue.empty()) return false;

    pdu = queue.front();
    queue.pop_front();
    return true;
}

void AclEngine::Implementation::ChannelList::Channel::Send::fillQueue()
{
    if (queue.empty())
    {
        if (!pending_queue.empty())
        {
            // Try to clear out any individual packets first, as they will likely be small.
            queue.push_front(new IndividualPacketQueue(pending_queue.front(), rbs_pdu.get_acl_data_pkt_length()));
            pending_queue.pop_front();
        }
        else if (!senders.empty())
        {
            int originalSender = currentSender;
            bool found = false;

            do {
                CountedPointer<AclEngine::PacketQueue> pq;

                if (senders[currentSender] && (pq = senders[currentSender]->getPacketQueue(rbs_pdu.get_acl_data_pkt_length())))
                {
                    queue.push_back(pq);
                }
                currentSender = (currentSender + 1) % senders.size();
            } while (currentSender != originalSender && queue.empty());
        }
    }
}

HCIACLPDU AclEngine::Implementation::ChannelList::Channel::Send::getPdu(bool &isSendablePdu)
{
    CriticalSection::Lock lock(mCs);
    HCIACLPDU pdu(null_pdu);

    while (true)
    {
        fillQueue();
        if (queue.empty())
        {
            isSendablePdu = false;
            return pdu;
        }
        else if (queue.front()->getPdu(pdu))
        {
            ++tokensUsed;
            isSendablePdu = true;
            return pdu;
        }
        else
        {
            queue.pop_front();
        }
    }
}

void AclEngine::Implementation::ChannelList::Channel::Send::clear()
{
    CriticalSection::Lock lock(mCs);
    queue.clear();
    pending_queue.clear();
    size = 0;
}

size_t AclEngine::Implementation::ChannelList::Channel::Send::sizeOfQueue()
{
    CriticalSection::Lock lock(mCs);
    return size;
}

uint32 AclEngine::Implementation::ChannelList::Channel::Send::getTokensUsed()
{
    CriticalSection::Lock lock(mCs);
    return tokensUsed;
}

void AclEngine::Implementation::ChannelList::Channel::Send::setBufferSizes(HCI_READ_BUFFER_SIZE_RET_T_PDU pdu)
{
    CriticalSection::Lock lock(mCs);
    rbs_pdu = pdu;
}

AclEngine::Implementation::ChannelList::Channel::Receive::Receive(uint32 ch, DefaultCallback *dcb) :
    mCh(ch),
    defaultCallback(dcb),
    state(IDLE)
{
}

void AclEngine::Implementation::ChannelList::Channel::Receive::receive(const HCIACLPDU &pdu)
{
    switch (state)
    {
    case IDLE:
        // Not waiting for any more acl packets to form an l2cap packet
        state = idle(pdu); break;
        break;
    case PROCESSING:
        // Part way through receiving an l2cap packet
        state = continueProcessing(pdu); break;
        break;
    default:
        // The state machine has become corrupted
        assert(0);
        break;
    }
}

AclEngine::Implementation::ChannelList::Channel::Receive::State AclEngine::Implementation::ChannelList::Channel::Receive::idle(const HCIACLPDU &pdu)
{
    switch (pdu.get_pbf())
    {
    case HCIACLPDU::start:
    case HCIACLPDU::start_not_auto_flush:
        return startProcessing(pdu);
    default:
        // Received what appears to be an isolated acl packet, not part of an l2cap packet.
        return callDefaultHandler(pdu);
    }
}

AclEngine::Implementation::ChannelList::Channel::Receive::State AclEngine::Implementation::ChannelList::Channel::Receive::startProcessing(const HCIACLPDU &pdu)
{
    // pdu is a start/non flush packet
    const uint8 *data = pdu.get_dataPtr();

    if (pdu.get_length() < 4)
    {
        // Received a non l2cap packet. Dump to the default handler
        return callDefaultHandler(pdu);
    }
    // Should be interpreted from now on as an l2cap packet
    l2CapLengthRemaining = data[0] | (((uint16) data[1]) << 8); 
    l2CapChannel = data[2] | (((uint16) data[3]) <<8);
    return process(pdu);
}

AclEngine::Implementation::ChannelList::Channel::Receive::State AclEngine::Implementation::ChannelList::Channel::Receive::continueProcessing(const HCIACL_L2CAP_PDU &pdu)
{
    switch(pdu.get_pbf())
    {
    case HCIACLPDU::start:
    case HCIACLPDU::start_not_auto_flush:
        // Previous l2cap packet ended prematurely
        reportL2CapPacketEndedPrematurely();
        return startProcessing(pdu);
    case HCIACLPDU::cont:
        return process(pdu);
    default:
        // Received a pdu with an invalid pbf
        reportL2CapPacketEndedPrematurely();
        return callDefaultHandler(pdu);
    }
}

CountedPointer<AclEngine::Callback> AclEngine::Implementation::ChannelList::Channel::Receive::getCallback()
{
    CriticalSection::Lock lock(mCs);

    if (mCallbacks.find(l2CapChannel) != mCallbacks.end())
    {
        return mCallbacks[l2CapChannel];
    }
    else
    {
        return 0;
    }
}

void AclEngine::Implementation::ChannelList::Channel::Receive::reportL2CapPacketEndedPrematurely()
{
    CountedPointer<AclEngine::Callback> callback(getCallback());
    
    if (callback)
    {
        callback->l2CapPacketEndedPrematurely(l2CapLengthRemaining);
    }
}

AclEngine::Implementation::ChannelList::Channel::Receive::State AclEngine::Implementation::ChannelList::Channel::Receive::process(const HCIACL_L2CAP_PDU &pdu)
{
    CountedPointer<AclEngine::Callback> callback(getCallback());

    if (callback)
    {
        if (pdu.get_l2capLength() <= l2CapLengthRemaining)
        {
            if (!callback->receive(pdu, l2CapLengthRemaining))
            {
                return callDefaultHandler(pdu);
            }
            l2CapLengthRemaining -= pdu.get_l2capLength();
            if (l2CapLengthRemaining == 0)
            {
                return IDLE;
            }
            else
            {
                return PROCESSING;
            }
        }
        else
        {
            // Packet is too long. Split it in two, send first half to callback and second half to default handler.
            uint16 firstPacketLength;
            uint16 secondPacketLength;

            // Packet needs to be split in correct place, accounting for presence of l2cap header.
            switch(pdu.get_pbf())
            {
            case HCIACLPDU::start:
            case HCIACLPDU::start_not_auto_flush:
                firstPacketLength = l2CapLengthRemaining + 4;
                break;
            default:
                firstPacketLength = l2CapLengthRemaining;
                break;
            }
            
            secondPacketLength = pdu.get_length() - firstPacketLength;

            const HCIACLPDU needed(pdu.get_dataPtr(), firstPacketLength, pdu.get_handle(), pdu.get_pbf(), pdu.get_bct());
            const HCIACLPDU extra(pdu.get_dataPtr() + firstPacketLength, secondPacketLength, pdu.get_handle(), HCIACLPDU::cont, pdu.get_bct());

            if (!callback->receive(needed, l2CapLengthRemaining))
            {
                // Packet was rejected any way so send entire pdu to default handler
                l2CapLengthRemaining = 0;
                return callDefaultHandler(pdu);
            }
            else
            {
                l2CapLengthRemaining = 0;
                return callDefaultHandler(extra);
            }
        }
    }
    else
    {
        // Callback has been removed. Perhaps it was canceled
        return callDefaultHandler(pdu);
    }
}

AclEngine::Implementation::ChannelList::Channel::Receive::State AclEngine::Implementation::ChannelList::Channel::Receive::callDefaultHandler(const HCIACL_L2CAP_PDU &pdu)
{
    defaultCallback->receive(pdu);
    return IDLE;

}

bool AclEngine::Implementation::ChannelList::Channel::Receive::setCallback(uint16 l2capch, CountedPointer<Callback> callback)
{
    CriticalSection::Lock lock(mCs);
    if (mCallbacks.find(l2capch) == mCallbacks.end())
    {
        mCallbacks[l2capch] = callback;
        return true;
    }
    else
    {
        return false;
    }   
}

bool AclEngine::Implementation::ChannelList::Channel::Receive::unsetCallback(uint16 l2capch)
{
    CriticalSection::Lock lock(mCs);
    if (mCallbacks.find(l2capch) == mCallbacks.end())
    {
        return false;
    }
    else
    {
        mCallbacks.erase(l2capch);
        return true;
    }
}



