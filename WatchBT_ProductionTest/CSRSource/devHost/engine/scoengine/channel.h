#ifndef CHANNEL_H
#define CHANNEL_H

#include "scoengine_impl.h"
#include "hcipacker/hcidatapdu.h"
#include "common/countedpointer.h"

class ScoEngine::Implementation::Channel : public Counter
{
public:
    Channel(uint16 ch, DefaultCallback *defaultCallback);
    ~Channel();
    bool hasCallback();
    bool setCallback(Callback *callback);
    bool unsetCallback();
    void receive(HCISCOPDU pdu);

private:
    uint16 ch;
    DefaultCallback *mDefaultCallback;
    CountedPointer<Callback> callback;
    
    // Used to control access to the callback pointer.
    // It does not protect the object pointed at by
    // callback which can protect itself.
    CriticalSection mCs;

    // Get a local (on the stack) copy of callback.
    // Used to neaten the code a little.
    CountedPointer<Callback> getLocalCallbackPointer();

    bool useCallback(HCISCOPDU pdu);
};

#endif // CHANNEL_H
