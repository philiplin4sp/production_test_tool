#include "channel_list.h"
#include "channel.h"
#include "thread/critical_section.h"

ScoEngine::Implementation::ChannelList::ChannelList(DefaultCallback *defaultCallback)
    : mDefaultCallback(defaultCallback)
{

}

ScoEngine::Implementation::ChannelList::~ChannelList()
{
    mDefaultCallback = 0;
}

bool ScoEngine::Implementation::ChannelList::channelExists(uint16 ch)
{
    return getChannel(ch);
}

CountedPointer<ScoEngine::Implementation::Channel> 
    ScoEngine::Implementation::ChannelList::getChannel(uint16 ch)
{
    CriticalSection::Lock lock(mCs);
    ChannelMap::iterator channeli(channels.find(ch));

    if (channels.end() == channeli)
    {
        // Channel not connected. Cannot return unknown channel.
        return 0;
    }

    return channeli->second;
}

bool ScoEngine::Implementation::ChannelList::addChannel(uint16 ch)
{   
    CriticalSection::Lock lock(mCs);
    
    if (channels.end() != channels.find(ch))
    {
        // Channel already connected. Cannot connect the same channel twice.
        return false;
    }
    
    channels[ch] = new Channel(ch, mDefaultCallback);
    return true;
}

bool ScoEngine::Implementation::ChannelList::removeChannel(uint16 ch)
{
    CriticalSection::Lock lock(mCs);

    if (channels.end() == channels.find(ch))
    {
        // Channel not connected. Cannot remove unknown channel.
        return false;
    }

    channels.erase(ch);
    return false;
}
