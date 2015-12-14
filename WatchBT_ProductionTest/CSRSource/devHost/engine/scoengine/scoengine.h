#ifndef SCO_ENGINE_H
#define SCO_ENGINE_H

#include "common/types.h"
#include "hcipacker/hcidatapdu.h"
#include "csrhci/bluecontroller.h"
#include "thread/critical_section.h"
#include "thread/safe_counter.h"

class ScoEngine
{
public:
    class Implementation;

    // Why two callback classes?
    //
    // Callback objects are allowed to have a lifetime that is shorter than
    // that of the ScoEngine object. For example one may wish to send a file
    // and log the received data to a file, ignoring any other data. This
    // Callback object will then usually need to be deleted after it
    // has been removed from the ScoEngine object. To avoid race conditions
    // while being deleted Callback inherits from SafeCounter so that it can be
    // stored in a CountedPointer.
    //
    // DefaultCallback is meant to be used as a fallback when no other 
    // receive operation is in progress. It should have a lifetime that is
    // greater than or equal to the ScoEngine object.

    class DefaultCallback
    {
    public:
        enum PduStatus
        {
            OK,
            LENGTH_DISCREPANT
        };
        virtual void receive(PduStatus status, HCISCOPDU pdu) = 0;
    };

    class Callback : public SafeCounter
    {
    public:
        virtual bool receive(HCISCOPDU pdu) = 0;
    };

    ScoEngine(BlueCoreDeviceController_newStyle *dc, 
              DefaultCallback *defaultCallback = 0);
    ~ScoEngine();
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
    Implementation *pImpl;
};

#endif // SCO_ENGINE
