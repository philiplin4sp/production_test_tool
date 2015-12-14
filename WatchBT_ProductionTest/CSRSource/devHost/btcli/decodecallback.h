#include "csrhci/bluecontroller.h"

    //  all PDUs from the chip which don't correspond to a sent
    //  PDU go through here.
class DecodeCallBack : public BlueCoreDeviceController_newStyle::CallBackProvider
{
public:
    virtual void onPDU ( const PDU& );

    //  and "extra" callbacks.
    virtual void onTransportFailure (FailureMode);
private:
    virtual void onBCCMDResponse ( const PDU &);
    virtual void onHQRequest     ( const PDU &);
    virtual void onHCIEvent      ( const PDU &);
    virtual void onHCIACLData    ( const PDU &);
    virtual void onHCISCOData    ( const PDU &);
    virtual void onDebug         ( const PDU &);
    virtual void onVMData        ( const PDU &);
    virtual void onLMPdebug      (const PDU &);
};
