#include "send_token_handler.h"

#include <cassert>

uint32 SendTokenHandler::getMaxTokens(uint16 ch)
{
    assert(this);
    return getMaxTokens_(ch);
}

void SendTokenHandler::useTxTokens(uint16 ch, uint32 used)
{
    assert(this);
    useTxTokens_(ch, used);
}
