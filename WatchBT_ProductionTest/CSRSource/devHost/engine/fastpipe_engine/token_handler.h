/*
 * Copyright CSR 2008
 *
 * $Id: //depot/bc/bluesuite_2_4/devHost/engine/fastpipe_engine/token_handler.h#1 $
 *
 * Purpose: TokenHandler keeps track of flow control tokens, both tx and rx. 
 *          ChannelTokens contains the parameters needed to set up a pipe in 
 *          TokenHandler.
 *          TokenHandler::Pipe contains the information for each individual
 *          pipe.
 *
 */

#ifndef TOKEN_HANDLER_H
#define TOKEN_HANDLER_H

#include "common/types.h"
#include "status_codes.h"
#include <map>
#include <deque>

class ChannelTokens
{
public:
    ChannelTokens(uint16 ph,
                  uint32 overh,
                  uint32 capacityh,
                  uint32 overhc,
                  uint32 capacityhc,
                  uint16 ch);
    uint16 getPh() const;
    uint32 getOverh() const;
    uint32 getCapacityh() const;
    uint32 getOverhc() const;
    uint32 getCapacityhc() const;
    uint16 getCh() const;
private:
    const uint16 ph_;
    const uint32 overh_;
    const uint32 capacityh_;
    const uint32 overhc_;
    const uint32 capacityhc_;
    const uint16 ch_;
    ChannelTokens &operator=(const ChannelTokens &);
};

class TokenHandler
{
public:
    struct TokenCount
    {
        uint32 rx;
        uint32 tx;
    };

    struct PipeInfo
    {
        uint16 ch;
        uint16 ph;
        TokenCount usage;
    };

    enum OperationStatus
    {
        OK,
        TOO_MANY_RX_TOKENS_USED_CHANNEL,
        TOO_MANY_RX_TOKENS_USED_TOTAL,
        TOO_MANY_RX_TOKENS_USED_BOTH,
        TOO_MANY_TX_TOKENS_RETURNED_CHANNEL,
        TOO_MANY_TX_TOKENS_RETURNED_TOTAL,
        TOO_MANY_TX_TOKENS_RETURNED_BOTH,
        PIPE_DOES_NOT_EXIST,
        CHANNEL_DOES_NOT_EXIST,
        PIPE_DISCONNECTED,
        VIOLATES_LIMITH,
        VIOLATES_LIMITC,
        RECYCLED_PIPE_ID,
        CURRENTLY_REDUCING_LIMITC,
        REDUCED_LIMITC,
        REDUCING_LIMITC,
        REDUCTION_VIOLATES_LIMITC,
        VIOLATES_FLOWCONTROL,
        GENERIC_FAILURE
    };

    TokenHandler(uint32);
    OperationStatus create(const ChannelTokens &channelTokens);
    bool destroy(uint16);
    bool isConnected(uint16 ph) const;

    // Get some tokens to return back to the bluecore
    bool freeRxTokens(uint16 &, uint16 &);
    // Received a packet, so some return tokens are used.
    FastPipeEngineUseRxTokensStatus useRxTokens(uint16, uint16);
    // About to send some data. Find out the maximum sendable.
    uint16 getMaxTxTokens(uint16) const;
    // Just sent an amount of data.
    void useTxTokens(uint16, uint16);
    // Received some tokens from the bluecore.
    OperationStatus freeTxTokens(uint16, uint16);
    // Get information for a pipe
    bool getPipeInfo(uint16 ph, uint32 &rxUsed, uint32 &txUsed);
    void getEveryPipeInfoAndTotalInfo(std::deque<PipeInfo> &pipes, TokenCount &total);
    void getTotalInfo(uint32 &rxUsed, uint32 &txUsed);
    OperationStatus reduceLimitc(uint32 reduction);
    OperationStatus setLimitc(uint32 newLimitc);

    class Pipe
    {
    public:
        enum State
        {
            CONNECTED,
            DISCONNECT_SEND_TOKEN,
            DISCONNECT_RECEIVE_TOKEN,
            STATE_MAX
        };
        Pipe(uint32 capacityh, uint32 overh, uint32 capacityhc, uint32 overhc);
        void setState(State);
        State getState() const;
        uint16 getMaxTxTokens() const;
        void useTxTokens(uint16);
        uint16 freeRxTokens();
        FastPipeEngineUseRxTokensStatus useRxTokens(uint16);
        OperationStatus freeTxTokens(uint16);
        void getInfo(uint32 &rxUsed, uint32 &txUsed);
        uint32 getOverh() const;
        uint32 getOverhc() const;
    private:
        State state_;
        const uint32 capacityh_;
        const uint32 overh_;
        uint32 usageh_;
        const uint32 capacityhc_;
        const uint32 overhc_;
        uint32 usagehc_;
        Pipe &operator=(const Pipe &);
    };

private:
    const uint32 limith_;
    uint32 limithc_;
    bool reducing_limitc_;
    uint32 new_limitc_;
    uint32 usageh_;
    uint32 usagehc_;
    uint32 overh_;
    uint32 overhc_;
    std::map<uint16, Pipe> pipes;
    TokenHandler &operator=(const TokenHandler &);
    bool isLimithViolated(const uint32 tokens_added) const;
    bool isLimithcViolated(const uint32 tokens_added) const;
    uint32 getMaxUseableRxTokens() const;
    uint32 getMaxUseableTxTokens() const;
};

#endif
