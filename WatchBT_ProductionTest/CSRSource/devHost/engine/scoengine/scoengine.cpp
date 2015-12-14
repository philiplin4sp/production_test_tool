#include "scoengine.h"
#include "scoengine_impl.h"
#include "channel_list.h"
#include <cassert>

ScoEngine::ScoEngine(BlueCoreDeviceController_newStyle *dc, ScoEngine::DefaultCallback *defaultCallback)
    : pImpl(new Implementation(dc, defaultCallback))
{
}

ScoEngine::~ScoEngine()
{
    delete pImpl;
    pImpl = 0;
}

void ScoEngine::setBufferSizes(const HCI_READ_BUFFER_SIZE_RET_T_PDU &pdu)
{
    pImpl->setBufferSizes(pdu);
}

bool ScoEngine::getBufferSizes(HCI_READ_BUFFER_SIZE_RET_T_PDU &rbs_pdu)
{
    return pImpl->getBufferSizes(rbs_pdu);
}

void ScoEngine::sentRbs()
{
    pImpl->sentRbs();
}

bool ScoEngine::isConnected(uint16 ch)
{
    return pImpl->isConnected(ch);
}

bool ScoEngine::connect(uint16 ch)
{
    return pImpl->connect(ch);
}

bool ScoEngine::disconnect(uint16 ch)
{
    return pImpl->disconnect(ch);
}

bool ScoEngine::hasCallback(uint16 ch)
{
    return pImpl->hasCallback(ch);
}

bool ScoEngine::setCallback(uint16 ch, Callback *callback)
{
    return pImpl->setCallback(ch, callback);
}

bool ScoEngine::unsetCallback(uint16 ch)
{
    return pImpl->unsetCallback(ch);
}

bool ScoEngine::send(HCISCOPDU pdu)
{
    return pImpl->send(pdu);
}

void ScoEngine::receive(HCISCOPDU pdu)
{
    pImpl->receive(pdu);
}

ScoEngine::Implementation::Implementation(
                                BlueCoreDeviceController_newStyle *dc,
                                DefaultCallback *defaultCallback)
    : rbsState(RBS_INVALID),
      mdc(dc),
      channelList(new ChannelList(defaultCallback)),
      mDefaultCallback(defaultCallback)
{

}
ScoEngine::Implementation::~Implementation()
{
    mdc = 0;
    delete channelList;
    channelList = 0;
}

void ScoEngine::Implementation::setBufferSizes(
                                    const HCI_READ_BUFFER_SIZE_RET_T_PDU& pdu)
{
    rbs_pdu = pdu;
    rbsState = RBS_VALID;
    rbsSignal.set();
}

bool ScoEngine::Implementation::getBufferSizes(
                                    HCI_READ_BUFFER_SIZE_RET_T_PDU& rbs_pdu)
{
    switch (rbsState)
    {
    case RBS_INVALID:
        mdc->send(HCI_READ_BUFFER_SIZE_T_PDU());
        // fall through
    case AWAITING_REPLY:
        rbsSignal.wait(1000);
        if (!rbsSignal.get()) return false;
        // fall through
    case RBS_VALID:
        rbs_pdu = this->rbs_pdu;
        return true;
    default:
        // Probably not initialised properly
        assert(0);
    }
    return false;
}

void ScoEngine::Implementation::sentRbs()
{
    if (RBS_INVALID == rbsState) rbsState = AWAITING_REPLY;
}

bool ScoEngine::Implementation::isConnected(uint16 ch)
{
    return channelList->channelExists(ch);
}

bool ScoEngine::Implementation::connect(uint16 ch)
{
    return channelList->addChannel(ch);
}

bool ScoEngine::Implementation::disconnect(uint16 ch)
{
    return channelList->removeChannel(ch);
}

bool ScoEngine::Implementation::hasCallback(uint16 ch)
{
    CountedPointer<Channel> channel(channelList->getChannel(ch));
        
    return channel && channel->hasCallback();
}

bool ScoEngine::Implementation::setCallback(uint16 ch, Callback *callback)
{
    CountedPointer<Channel> channel(channelList->getChannel(ch));

    return channel && channel->setCallback(callback);
}

bool ScoEngine::Implementation::unsetCallback(uint16 ch)
{
    CountedPointer<Channel> channel(channelList->getChannel(ch));

    return channel && channel->unsetCallback();
}

bool ScoEngine::Implementation::send(HCISCOPDU pdu)
{
    if (!channelList->channelExists(pdu.get_handle()))
    {
        return false;  
    }
    
    return mdc->send(pdu, false);
}

void ScoEngine::Implementation::receive(HCISCOPDU pdu)
{
    CountedPointer<Channel> channel(channelList->getChannel(pdu.get_handle()));
    const uint8 *data = pdu.data();
    size_t length(pdu.size());

    if (length < 3 || length != data[2] + 3U)
    {
        if (mDefaultCallback)
        {
            mDefaultCallback->receive(ScoEngine::DefaultCallback::LENGTH_DISCREPANT, pdu);        
        }
    }
    else if (channel)
    {
        channel->receive(pdu);
    }
    else if (mDefaultCallback)
    {
        mDefaultCallback->receive(ScoEngine::DefaultCallback::OK, pdu);
    }
}

CriticalSection &ScoEngine::Implementation::getCriticalSection()
{
    return mCs;
}
