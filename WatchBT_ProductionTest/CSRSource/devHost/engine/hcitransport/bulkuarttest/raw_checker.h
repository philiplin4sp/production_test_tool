#ifndef RAW_CHECKER_H
#define RAW_CHECKER_H

#include "../bluecontroller.h"
#include "random.h"

// Use to check that data is getting through the Uart unmolested.

class RawChecker : public DataWatcher
{
public:
    RawChecker();
    virtual void onSend ( const uint8* , uint32 );
    virtual void onRecv ( const uint8* , uint32 );
private:
    Random mreceive_random;
    Random msend_random;
    uint32 mbytes_received;
    uint32 mbytes_sent;
};

#endif
