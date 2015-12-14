#ifndef ACL_CHANNEL_LIST_H
#define ACL_CHANNEL_LIST_H

#include "aclengine_impl.h"
#include "csrhci/bluecontroller.h"
#include <map>
#include <list>

class AclEngine::Implementation::ChannelList
{
public:
    class Channel;
    ChannelList(DefaultCallback *mdcb);
    CountedPointer<Channel> add_channel_internal(uint16 ch);
    CountedPointer<Channel> add_channel(uint16 ch);
    CountedPointer<Channel> add_act_channel(uint16 ch);
    CountedPointer<Channel> add_pic_channel(uint16 ch);
    CountedPointer<Channel> remove_channel(uint16 ch);
    CountedPointer<Channel> get_channel(uint16 ch);
    void setAclPacketNumber(uint16 aclPacketNumber);
    bool isActChValid();
    uint16 getActCh();
    bool isPicChValid();
    uint16 getPicCh();
    void setBufferSizes(HCI_READ_BUFFER_SIZE_RET_T_PDU pdu);
    int addSender(uint16 ch, CountedPointer<AclEngine::Sender> sender);
    void removeSender(uint16 ch, int index);
    bool getPdu(HCIACLPDU &pdu);

private:
    CriticalSection mCs;
    typedef std::map<uint16, CountedPointer<Channel> > Channels;
    Channels channels;
    // when there is data, this is set to the next channel with data.
    Channels::iterator currentChannel;

    DefaultCallback *mdcb;
    uint16 mAclPacketNumber;
    void advanceChannel();
    bool actChValid;
    uint16 actCh;
    bool picChValid;
    uint16 picCh;
    HCI_READ_BUFFER_SIZE_RET_T_PDU rbs_pdu;
    enum 
    {
        RBS_INVALID,
        RBS_VALID
    } rbsState;
    uint32 upperThreshold;
    uint32 lowerThreshold;
};

#endif
