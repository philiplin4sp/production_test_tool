/*
 * Copyright CSR 2008
 *
 * $Id: //depot/bc/bluesuite_2_4/devHost/engine/fastpipe_engine/token_sender.h#1 $
 * 
 * Purpose: TokenSender asks tokenHandler_ for some tokens to send down the
 *          token pipe. If there are tokens that can be sent, they are returned
 *          so the Scheduler can send them. 
 *
 */

#ifndef TOKEN_SENDER_H
#define TOKEN_SENDER_H

#include "channel_sender.h"
#include "hcipacker/hcidatapdu.h"
#include "common/types.h"

class TokenHandler;

class TokenSender : public ChannelSender
{
public:
    TokenSender(uint16, uint16, TokenHandler &);
private:
    virtual std::pair<bool, HCIACLPDU> getPdu_(uint16);
    TokenHandler &tokenHandler_;
};

#endif

