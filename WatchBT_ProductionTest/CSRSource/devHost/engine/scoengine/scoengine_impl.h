#ifndef SCO_ENGINE_IMPL_H
#define SCO_ENGINE_IMPL_H

#include "scoengine.h"
#include "common/types.h"
#include "hcipacker/hcidatapdu.h"
#include "csrhci/bluecontroller.h"
#include "thread/critical_section.h"
#include "thread/signal_box.h"

class ScoEngine::Implementation
{
public:
    class ChannelList;
    class Channel;

    Implementation(BlueCoreDeviceController_newStyle *dc, DefaultCallback *defaultCallback);
    ~Implementation();
    void setBufferSizes(const HCI_READ_BUFFER_SIZE_RET_T_PDU& pdu);
    bool getBufferSizes(HCI_READ_BUFFER_SIZE_RET_T_PDU &rbs_pdu);
    void sentRbs();
    bool isConnected(uint16 ch);
    bool connect(uint16 ch);
    bool disconnect(uint16 ch);
    bool hasCallback(uint16 ch);
    bool setCallback(uint16 ch, Callback *callback);
    bool unsetCallback(uint16 ch);
    bool send(HCISCOPDU pdu);
    void receive(HCISCOPDU pdu);
    CriticalSection &getCriticalSection();
private:
    enum RBS_State {
        RBS_INVALID,
        AWAITING_REPLY,
        RBS_VALID
    } rbsState;
    
    SingleSignal rbsSignal;
    BlueCoreDeviceController_newStyle *mdc;
    ChannelList *channelList;
    DefaultCallback *mDefaultCallback;
    CriticalSection mCs;
    HCI_READ_BUFFER_SIZE_RET_T_PDU rbs_pdu;
};

#endif // SCO_ENGINE_IMPL_H
