#include "aclengine.h"
#include "aclengine_impl.h"
#include "aclscheduler.h"
#include "acl_channel_list.h"
#include "acl_channel.h"
#include "hciacl_l2cap_pdu.h"
#include <cassert>

AclEngine::AclEngine(BlueCoreDeviceController_newStyle *ldc, DefaultCallback *dcb) :
    pImpl(new Implementation(ldc, dcb))
{
}

bool AclEngine::connect(uint32 chNum)
{
    return pImpl->connect(chNum);
}

bool AclEngine::disconnect(uint32 chNum)
{
    return pImpl->disconnect(chNum);
}

bool AclEngine::nocp(const HCI_EV_NUMBER_COMPLETED_PKTS_T_PDU& pdu)
{
    return pImpl->nocp(pdu);
}

bool AclEngine::setBufferSizes(const HCI_READ_BUFFER_SIZE_RET_T_PDU& pdu)
{
    return pImpl->setBufferSizes(pdu);
}

bool AclEngine::send(ExtendedHCIACLPDU_Helper helper)
{
    return pImpl->send(helper);
}

void AclEngine::receive(const HCIACLPDU &pdu)
{
    HCIACL_L2CAP_PDU l2cap_pdu(pdu);
    pImpl->receive(l2cap_pdu);
}

bool AclEngine::setCallback(uint16 aclch, uint16 l2capch, Callback *callback)
{
    return pImpl->setCallback(aclch, l2capch, callback);
}

void AclEngine::unsetCallback(uint16 aclch, uint16 l2capch)
{
    pImpl->unsetCallback(aclch, l2capch);
}

bool AclEngine::clear(uint16 aclch)
{
    return pImpl->clear(aclch);
}

bool AclEngine::sizeOfQueue(uint16 aclch, size_t *size)
{
    return pImpl->sizeOfQueue(aclch, size);
}

void AclEngine::sentRbs()
{
    pImpl->sentRbs();
}

bool AclEngine::isAclChannelOpen(uint32 ch)
{
    return pImpl->isAclChannelOpen(ch);
}

int AclEngine::addSender(uint16 ch, CountedPointer<AclEngine::Sender> sender)
{
    return pImpl->addSender(ch, sender);
}

void AclEngine::removeSender(uint16 ch, int index)
{
    pImpl->removeSender(ch, index);
}

void AclEngine::kickScheduler()
{
    pImpl->kickScheduler();
}

// Implementation

AclEngine::Implementation::Implementation(BlueCoreDeviceController_newStyle *ldc, DefaultCallback *dcb) :
    mdc(ldc)
{
    channelList = new ChannelList(dcb);
    mScheduler = new Scheduler(ldc, channelList);
    mdcb = dcb;
}

bool AclEngine::Implementation::connect(uint32 chNum)
{
    return channelList->add_channel(chNum);
}

bool AclEngine::Implementation::disconnect(uint32 chNum)
{
    CountedPointer<AclEngine::Implementation::ChannelList::Channel> channel(channelList->remove_channel(chNum));
    if (channel)
    {
        mScheduler->disconnectFreedTokens(channel->sender.getTokensUsed());
        channel = 0;
        kickScheduler();
        return true;
    }
    else
    {
        return false;    
    }
}

bool AclEngine::Implementation::nocp(const HCI_EV_NUMBER_COMPLETED_PKTS_T_PDU& pdu)
{
    return mScheduler->nocp(pdu);
}

bool AclEngine::Implementation::setBufferSizes(const HCI_READ_BUFFER_SIZE_RET_T_PDU& pdu)
{
    return mScheduler->setBufferSizes(pdu);
}

bool AclEngine::Implementation::send(ExtendedHCIACLPDU_Helper helper)
{
    return mScheduler->add(helper);
}

void AclEngine::Implementation::receive(const HCIACL_L2CAP_PDU &pdu)
{
    CountedPointer<AclEngine::Implementation::ChannelList::Channel> channel(channelList->get_channel(pdu.get_handle()));

    if (channel)
    {
        channel->receiver.receive(pdu);
    }
    else
    {
        mdcb->receive(pdu);
    }
}

bool AclEngine::Implementation::setCallback(uint16 aclch, uint16 l2capch, Callback *callback)
{
    CountedPointer<AclEngine::Implementation::ChannelList::Channel> channel(channelList->get_channel(aclch));
    CountedPointer<Callback> cp(callback);

    return channel && channel->receiver.setCallback(l2capch, callback);
}

bool AclEngine::Implementation::unsetCallback(uint16 aclch, uint16 l2capch)
{
    CountedPointer<AclEngine::Implementation::ChannelList::Channel> channel(channelList->get_channel(aclch));

    return channel && channel->receiver.unsetCallback(l2capch);
}

bool AclEngine::Implementation::clear(uint16 aclch)
{
    CountedPointer<AclEngine::Implementation::ChannelList::Channel> channel(channelList->get_channel(aclch));
    
    if (channel)
    {
        channel->sender.clear();
        return true;
    }
    else
    {
        return false;
    }
}

bool AclEngine::Implementation::sizeOfQueue(uint16 aclch, size_t *size)
{
    CountedPointer<AclEngine::Implementation::ChannelList::Channel> channel(channelList->get_channel(aclch));

    if (channel)
    {
        *size = channel->sender.sizeOfQueue();
        return true;
    }
    else
    {
        return false;
    }
}

void AclEngine::Implementation::sentRbs()
{
    mScheduler->sentRbs();
}

bool AclEngine::Implementation::isAclChannelOpen(uint16 ch)
{
    return channelList->get_channel(ch);
}

int AclEngine::Implementation::addSender(uint16 ch, CountedPointer<AclEngine::Sender> sender)
{
    return channelList->addSender(ch, sender);
}

void AclEngine::Implementation::removeSender(uint16 ch, int index)
{
    channelList->removeSender(ch, index);
}

void AclEngine::Implementation::kickScheduler()
{
    mScheduler->kick();
}
