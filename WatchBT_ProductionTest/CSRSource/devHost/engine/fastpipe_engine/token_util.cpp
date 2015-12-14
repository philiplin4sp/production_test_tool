/*
 * Copyright CSR 2009
 *
 * $Id: //depot/bc/main/devHost/engine/fastpipe_engine/token_util.h#1 $
 *
 * Purpose: Defines a number of things useful for handling tokens.
 *
 */

#include "token_util.h"

uint16 composeCredits(const uint8 first, const uint8 second)
{
    // The MSB of size must come first in the PDU. 
    return (((uint16) first) << 8) + second;
}

void decomposeCredits(const uint16 credits, uint8 &first, uint8 &second)
{
    first = (uint8)((credits >> 8) & 0xFF);
    second = (uint8)(credits & 0xFF);
}
