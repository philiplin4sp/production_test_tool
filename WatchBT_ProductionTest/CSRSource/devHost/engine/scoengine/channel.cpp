#include "channel.h"

ScoEngine::Implementation::Channel::Channel(uint16 ch, DefaultCallback *defaultCallback)
    : ch(ch),
      mDefaultCallback(defaultCallback),
      callback(0)
{

}

ScoEngine::Implementation::Channel::~Channel()
{
    mDefaultCallback = 0;
    callback = 0;
}

bool ScoEngine::Implementation::Channel::hasCallback()
{
    CriticalSection::Lock lock(mCs);
    
    return callback;
}

bool ScoEngine::Implementation::Channel::setCallback(Callback *callback)
{
    CriticalSection::Lock lock(mCs);
    
    if (this->callback)
    {
        // Callback already exists. Must unset callback before a new one can be set.
        return false;
    }

    this->callback = callback;
    return true;
}

bool ScoEngine::Implementation::Channel::unsetCallback()
{
    CriticalSection::Lock lock(mCs);

    if (!this->callback)
    {
        // Callback does not exist. Cannot unset unknown callback.
        return false;
    }

    callback = 0;
    return true;
}

bool ScoEngine::Implementation::Channel::useCallback(HCISCOPDU pdu)
{    
    CountedPointer<Callback> lCallback(getLocalCallbackPointer());

    return lCallback && lCallback->receive(pdu);
}

void ScoEngine::Implementation::Channel::receive(HCISCOPDU pdu)
{
    if (!useCallback(pdu) &&
        mDefaultCallback)
    {
        // Using Callback failed, but there is a default callback.
        mDefaultCallback->receive(ScoEngine::DefaultCallback::OK, pdu);
    }
}

CountedPointer<ScoEngine::Callback> ScoEngine::Implementation::Channel::
                                        getLocalCallbackPointer()
{
    CriticalSection::Lock lock(mCs);
    return callback;
}
