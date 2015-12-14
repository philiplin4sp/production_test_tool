/*
 * Copyright CSR 2008
 *
 * $Id: //depot/bc/bluesuite_2_4/devHost/engine/fastpipe_engine/token_sender.cpp#1 $
 *
 */

#include "token_sender.h"
#include "token_handler.h"
#include "token_util.h"
#include <vector>
#include <stdio.h>

TokenSender::TokenSender(uint16 ph, uint16 ch, TokenHandler &tokenHandler) :
    ChannelSender(ph, ch),
    tokenHandler_(tokenHandler)
{

}

std::pair<bool, HCIACLPDU> TokenSender::getPdu_(const uint16 maxSize)
{
    std::pair<bool, HCIACLPDU> packetpair(false, HCIACLPDU(0));

    uint16 ph;
    uint16 credits;
    std::vector<uint8> tokens;

    while ((maxSize - tokens.size()) >= TOKEN_SIZE &&
           tokenHandler_.freeRxTokens(ph, credits))
    {
        uint8 first, second;
        decomposeCredits(credits, first, second);
        uint16 oldSize = tokens.size();
        tokens.resize(tokens.size() + TOKEN_SIZE);
        tokens[oldSize + PIPE_OFFSET] = ph;
        tokens[oldSize + CREDIT_OFFSET] = first;
        tokens[oldSize + CREDIT_OFFSET + 1] = second;
    }

    if (!tokens.empty())
    {
        if (tokens.size() > TOKEN_SIZE)
            printf("Yay, useful\n");

        HCIACLPDU pdu(tokens.size());
        pdu.set_handle(getCh());
        pdu.set_data(&tokens[0], tokens.size());
        pdu.set_length();
        packetpair.first = true;
        packetpair.second = pdu;
    }

    return packetpair;
}
