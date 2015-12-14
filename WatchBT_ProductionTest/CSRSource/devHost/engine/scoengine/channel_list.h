#ifndef CHANNEL_LIST_H
#define CHANNEL_LIST_H

#include "scoengine_impl.h"
#include "common/countedpointer.h"
#include "channel.h"
#include <map>
#include "thread/critical_section.h"

class ScoEngine::Implementation::ChannelList
{
public:
    ChannelList(DefaultCallback *defaultCallback);
    ~ChannelList();
    bool channelExists(uint16 ch);
    CountedPointer<Channel> getChannel(uint16 ch);
    bool addChannel(uint16 ch);
    bool removeChannel(uint16 ch);
private:
    typedef std::map<uint16, CountedPointer<Channel> > ChannelMap;
    ChannelMap channels;
    DefaultCallback *mDefaultCallback;
    CriticalSection mCs;
};

#endif // CHANNEL_LIST_H
