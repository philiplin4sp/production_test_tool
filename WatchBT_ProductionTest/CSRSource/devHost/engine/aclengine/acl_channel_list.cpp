#include "acl_channel_list.h"
#include "acl_channel.h"
#include <cmath>

static const uint16 INVALID_CHANNEL = 0;

AclEngine::Implementation::ChannelList::ChannelList(DefaultCallback *dcb) :
    currentChannel(channels.end()),
    mdcb(dcb),
    mAclPacketNumber(0),
    actChValid(false),
    actCh(0),
    picChValid(false),
    picCh(0),
    rbsState(RBS_INVALID),
    upperThreshold(2000),
    lowerThreshold(1000)
{
    
}

CountedPointer<AclEngine::Implementation::ChannelList::Channel> AclEngine::Implementation::ChannelList::add_channel_internal(uint16 ch)
{
    CountedPointer<Channel> channel;

    if (channels.find(ch) == channels.end())
    {
        channels[ch] = channel = CountedPointer<Channel>(new Channel(ch, mdcb, lowerThreshold, upperThreshold));
    }

    if (channels.size() == 1)
    {
        currentChannel = channels.begin();
    }

    if (RBS_VALID == rbsState)
    {
        channels[ch]->sender.setBufferSizes(rbs_pdu);
    }

    return channel;
}

CountedPointer<AclEngine::Implementation::ChannelList::Channel> AclEngine::Implementation::ChannelList::add_channel(uint16 ch)
{
    CriticalSection::Lock lock(mCs);
    return add_channel_internal(ch);
}

CountedPointer<AclEngine::Implementation::ChannelList::Channel> AclEngine::Implementation::ChannelList::add_act_channel(uint16 ch)
{
    CriticalSection::Lock lock(mCs);
    actChValid = true;
    actCh = ch;
    return add_channel_internal(ch);
}

CountedPointer<AclEngine::Implementation::ChannelList::Channel> AclEngine::Implementation::ChannelList::add_pic_channel(uint16 ch)
{
    CriticalSection::Lock lock(mCs);
    picChValid = true;
    picCh = ch;
    return add_channel_internal(ch);
}

CountedPointer<AclEngine::Implementation::ChannelList::Channel> AclEngine::Implementation::ChannelList::remove_channel(uint16 ch)
{
    CriticalSection::Lock lock(mCs);
    CountedPointer<Channel> channel;
    std::map<uint16, CountedPointer<Channel> >::iterator channel_to_remove = channels.find(ch);

    if (channel_to_remove == channels.end())
    {
        return 0;
    }

    channel = channel_to_remove->second;    
    
    if (channel_to_remove != channels.end())
    {
        if (channel_to_remove == currentChannel)
        {
            advanceChannel();
        }
        channels.erase(channel_to_remove);
    }

    return channel;
}

void AclEngine::Implementation::ChannelList::setAclPacketNumber(uint16 aclPacketNumber)
{
    CriticalSection::Lock lock(mCs);
    mAclPacketNumber = aclPacketNumber;
}

bool AclEngine::Implementation::ChannelList::isActChValid()
{
    CriticalSection::Lock lock(mCs);
    return actChValid;
}

uint16 AclEngine::Implementation::ChannelList::getActCh()
{
    CriticalSection::Lock lock(mCs);
    return actCh;
}

bool AclEngine::Implementation::ChannelList::isPicChValid()
{
    CriticalSection::Lock lock(mCs);
    return picChValid;
}

uint16 AclEngine::Implementation::ChannelList::getPicCh()
{
    CriticalSection::Lock lock(mCs);
    return picCh;
}


CountedPointer<AclEngine::Implementation::ChannelList::Channel> AclEngine::Implementation::ChannelList::get_channel(uint16 ch)
{
    CriticalSection::Lock lock(mCs);
    CountedPointer<Channel> channel;

    if (channels.find(ch) != channels.end())
    {
        channel = channels[ch];
    }

    return channel;
}

void AclEngine::Implementation::ChannelList::setBufferSizes(HCI_READ_BUFFER_SIZE_RET_T_PDU pdu)
{
    CriticalSection::Lock lock(mCs);
    rbs_pdu = pdu;

    for (std::map<uint16, CountedPointer<Channel> >::iterator i(channels.begin()); channels.end() != i; ++i)
    {
        i->second->sender.setBufferSizes(pdu);
    }
    rbsState = RBS_VALID;
}

void AclEngine::Implementation::ChannelList::advanceChannel()
{
    ++currentChannel;
    if (currentChannel == channels.end()) 
    {
        // Hit the end of the map. Loop back to the beginning.
        currentChannel = channels.begin();
    }
}

int AclEngine::Implementation::ChannelList::addSender(uint16 ch, CountedPointer<AclEngine::Sender> sender)
{
    CriticalSection::Lock lock(mCs);
    return channels.find(ch)->second->sender.addSender(sender);
}

void AclEngine::Implementation::ChannelList::removeSender(uint16 ch, int index)
{
    CriticalSection::Lock lock(mCs);
    channels.find(ch)->second->sender.removeSender(index);
}

bool AclEngine::Implementation::ChannelList::getPdu(HCIACLPDU &pdu)
{
    CriticalSection::Lock lock(mCs);
    bool found = false;
    for (size_t i = 0; i < channels.size() && !found; ++i, advanceChannel())
    {
        pdu = currentChannel->second->sender.getPdu(found);
    }
    return found;
}

