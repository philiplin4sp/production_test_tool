#ifndef SEND_TOKEN_HANDLER_H
#define SEND_TOKEN_HANDLER_H

#include "common/types.h"

class SendTokenHandler
{
public:
    uint32 getMaxTokens(uint16);
    void useTxTokens(uint16, uint32);
private:
    virtual uint32 getMaxTokens_(uint16) const = 0;
    virtual void useTxTokens_(uint16, uint32) = 0;
};

#endif
