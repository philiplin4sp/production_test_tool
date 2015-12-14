/*
 * Copyright CSR 2008
 *
 * $Id: //depot/bc/bluesuite_2_4/devHost/engine/fastpipe_engine/token_handler.cpp#1 $
 *
 */

#include "token_handler.h"
#include "time/time_stamp.h"
#include <cassert>
#include <cstdio>

static uint32 minval(uint32 a, uint32 b)
{
    return a < b ? a : b;
}

ChannelTokens::ChannelTokens(const uint16 ph,
                             const uint32 overh,
                             const uint32 capacityh,
                             const uint32 overhc,
                             const uint32 capacityhc,
                             const uint16 ch) :
    ph_(ph),
    overh_(overh),
    capacityh_(capacityh),
    overhc_(overhc),
    capacityhc_(capacityhc),
    ch_(ch)
{

}

uint16 ChannelTokens::getPh() const
{
    assert(this);
    return ph_;
}

uint32 ChannelTokens::getOverh() const
{
    assert(this);
    return overh_;
}

uint32 ChannelTokens::getCapacityh() const
{
    assert(this);
    return capacityh_;
}

uint32 ChannelTokens::getOverhc() const
{
    assert(this);
    return overhc_;
}

uint32 ChannelTokens::getCapacityhc() const
{
    assert(this);
    return capacityhc_;
}

uint16 ChannelTokens::getCh() const
{
    assert(this);
    return ch_;
}

TokenHandler::TokenHandler(const uint32 limith) :
    limith_(limith),
    limithc_(0),
    reducing_limitc_(false),
    new_limitc_(0),
    usageh_(0),
    usagehc_(0),
    overh_(0),
    overhc_(0)
{
}

bool TokenHandler::isLimithViolated(const uint32 tokens_added) const
{
    return limith_ != 0 && usageh_ + overh_ + tokens_added > limith_;
}

bool TokenHandler::isLimithcViolated(const uint32 tokens_added) const
{
    return usagehc_ + overhc_ + tokens_added > limithc_;
}

static uint32 getMaxUseableTokens(const uint32 limit, const uint32 over)
{
    return limit - over;
}

uint32 TokenHandler::getMaxUseableRxTokens() const
{
    return getMaxUseableTokens(limith_, overh_);
}

uint32 TokenHandler::getMaxUseableTxTokens() const
{
    return getMaxUseableTokens(limithc_, overhc_);
}

TokenHandler::OperationStatus TokenHandler::create
    (const ChannelTokens &channelTokens)
{
    assert(this);
    const uint16 ph(channelTokens.getPh());
    const uint32 capacityh(channelTokens.getCapacityh());
    const uint32 capacityhc(channelTokens.getCapacityhc());
    const uint32 overh(channelTokens.getOverh());
    const uint32 overhc(channelTokens.getOverhc());

    if (pipes.find(ph) != pipes.end())
    {
        return RECYCLED_PIPE_ID;
    }

    if (isLimithViolated(overh))
    {
        return VIOLATES_LIMITH;
    }

    if (isLimithcViolated(overhc))
    {
        return VIOLATES_LIMITC;
    }

    overh_ += overh;
    overhc_ += overhc;
    pipes.insert(std::pair<uint16, Pipe>(ph, Pipe(capacityh,
                                                  overh,
                                                  capacityhc,
                                                  overhc)));
    return OK;
}

bool TokenHandler::destroy(const uint16 ph)
{
    assert(this);
    std::map<uint16, Pipe>::iterator pipe(pipes.find(ph));
    if (pipe == pipes.end()) return false;
    if (pipe->second.getState() != Pipe::CONNECTED) return false;
    pipe->second.setState(Pipe::DISCONNECT_SEND_TOKEN);
    return true;
}

bool TokenHandler::isConnected(const uint16 ph) const
{
    assert(this);
    return pipes.find(ph) != pipes.end();
}

bool TokenHandler::freeRxTokens(uint16 &ph, uint16 &tokens)
{
    assert(this);
    bool found_sendable(false);

    for (std::map<uint16, Pipe>::iterator i(pipes.begin());
         !found_sendable && i != pipes.end();
         ++i)
    {
        const Pipe::State state(i->second.getState());

        // Do not attempt to send tokens if on second stage of pipe 
        // destruction receiving tokens.
        if (state < Pipe::DISCONNECT_RECEIVE_TOKEN)
        {
            const uint16 size(i->second.freeRxTokens());
            usageh_ -= minval(size, usageh_);
            ph = i->first;
            tokens = size;

            if (size == 0)
            {
                // Only send 0 tokens if disconnecting.
                if (state == Pipe::DISCONNECT_SEND_TOKEN)
                {
                    found_sendable = true;
                    i->second.setState(Pipe::DISCONNECT_RECEIVE_TOKEN);
                }
            }
            else
            {
                found_sendable = true;
            }      
        }
    }

    return found_sendable;
}

FastPipeEngineUseRxTokensStatus TokenHandler::useRxTokens(const uint16 ph, 
                                                          const uint16 tokens)
{
    assert(this);

    if (isLimithViolated(tokens))
    {
        usageh_ = getMaxUseableRxTokens();
        if (pipes.find(ph)->second.useRxTokens(tokens) == 
                FASTPIPE_ENGINE_USE_RX_TOKENS_STATUS_ERROR_TOO_MANY_RX_TOKENS_USED_CHANNEL)
        {
            return FASTPIPE_ENGINE_USE_RX_TOKENS_STATUS_ERROR_TOO_MANY_RX_TOKENS_USED_BOTH;
        }
        else
        {
            return FASTPIPE_ENGINE_USE_RX_TOKENS_STATUS_ERROR_TOO_MANY_RX_TOKENS_USED_TOTAL;
        }
    }
    else
    {
        usageh_ += tokens;
        return pipes.find(ph)->second.useRxTokens(tokens);
    }
}

uint16 TokenHandler::getMaxTxTokens(const uint16 ph) const
{
    assert(this);
    if (reducing_limitc_ && ph != 0)
    {
        // If we're reducing limitc, only allow tokens to be sent.
        return 0;
    }

    assert(usagehc_ <= getMaxUseableTxTokens());
    const std::map<uint16, Pipe>::const_iterator pipe = pipes.find(ph);
    assert(pipe != pipes.end());
    uint32 useableTxTokens = getMaxUseableTxTokens() - usagehc_;
    return (uint16) minval(minval(0xffffu, useableTxTokens),
                           pipe->second.getMaxTxTokens());
}

void TokenHandler::useTxTokens(const uint16 ph, const uint16 tokens)
{
    assert(this);
    std::map<uint16, Pipe>::iterator pipe(pipes.find(ph));
    assert(pipe != pipes.end());
    assert(pipe->second.getState() == Pipe::CONNECTED);
    assert(!isLimithcViolated(tokens));
    usagehc_ += tokens;
    pipe->second.useTxTokens(tokens);
}

TokenHandler::OperationStatus TokenHandler::freeTxTokens(const uint16 ph,
                                                         const uint16 tokens)
{
    assert(this);
    std::map<uint16, Pipe>::iterator pipe(pipes.find(ph));
    if (pipe == pipes.end())
    {
        return PIPE_DOES_NOT_EXIST;
    }
    
    if (pipe->second.getState() == Pipe::DISCONNECT_RECEIVE_TOKEN &&
        tokens == 0)
    {
        overh_ -= pipe->second.getOverh();
        overhc_ -= pipe->second.getOverhc();
        pipes.erase(pipe);
        return PIPE_DISCONNECTED;
    }

    if (tokens > usagehc_)
    {
        usagehc_ = 0;
        if (reducing_limitc_)
        {
            // Mark a limitc reduction as complete
            reducing_limitc_ = false;
            limithc_ = new_limitc_;
        }
        if (pipe->second.freeTxTokens(tokens) == TOO_MANY_TX_TOKENS_RETURNED_CHANNEL)
        {
            return TOO_MANY_TX_TOKENS_RETURNED_BOTH;
        }
        else
        {
            return TOO_MANY_TX_TOKENS_RETURNED_TOTAL;
        }
    }
    else
    {
        usagehc_ -= tokens;

        if (reducing_limitc_ && usagehc_ + overhc_ < new_limitc_)
        {
            // Mark a limitc reduction as complete
            reducing_limitc_ = false;
            limithc_ = new_limitc_;
        }

        return pipe->second.freeTxTokens(tokens);
    }
}

bool TokenHandler::getPipeInfo(const uint16 ph, uint32 &rxUsed, uint32 &txUsed)
{
    assert(this);
    std::map<uint16, Pipe>::iterator pipe(pipes.find(ph));
    if (pipe == pipes.end()) return false;

    pipe->second.getInfo(rxUsed, txUsed);

    return true;
}

void TokenHandler::getTotalInfo(uint32 &rxUsed, uint32 &txUsed)
{
    assert(this);
    rxUsed = usageh_;
    txUsed = usagehc_;
}

void TokenHandler::getEveryPipeInfoAndTotalInfo(std::deque<PipeInfo> &pipeInfos, TokenCount &totalUsage)
{
    assert(this);

    for (std::map<uint16, Pipe>::iterator i(pipes.begin());
         i != pipes.end();
         ++i)
    {
        PipeInfo pipeInfo;
        pipeInfo.ph = i->first;
        i->second.getInfo(pipeInfo.usage.rx, pipeInfo.usage.tx);
        pipeInfos.push_back(pipeInfo);
    }
    totalUsage.rx = usageh_;
    totalUsage.tx = usagehc_;
}

TokenHandler::OperationStatus TokenHandler::reduceLimitc(const uint32 reduction)
{
    assert(this);

    if (reducing_limitc_)
    {
        // Only allow one round of reducing limitc at a time.
        return CURRENTLY_REDUCING_LIMITC;
    }

    if (limithc_ - reduction >= overhc_)
    {
        if (usagehc_ + overhc_ < limithc_ - reduction)
        {
            // Usage already below the new limit. Can set to new limitc without
            // any issue.
            limithc_ -= reduction;
            return REDUCED_LIMITC;
        }
        else
        {
            // Usage is greater than the new limit. Don't allow any data to be
            // sent, only tokens. Tokens will be allowed to be sent so that the
            // controller can still send the host tokens. If tokens weren't
            // allowed to be sent, the controller could keep sending the host
            // data until all rx tokens were used, resulting in the receive
            // path being blocked, meaning no tokens could be received, so the
            // usage would never drop below the new limit.
            reducing_limitc_ = true;
            new_limitc_ = limithc_ - reduction;
            return REDUCING_LIMITC;
        }
    }
    else
    {
        // Reducing limitc by this much will cause limitc to be violated by
        // overhc.
        return REDUCTION_VIOLATES_LIMITC;
    }
}

TokenHandler::OperationStatus TokenHandler::setLimitc(const uint32 newLimitc)
{
    assert(this);

    if (newLimitc < (usagehc_ + overhc_))
    {
        return VIOLATES_FLOWCONTROL;
    }

    limithc_ = newLimitc;
    return OK;
}

TokenHandler::Pipe::Pipe(uint32 capacityh, uint32 overh, uint32 capacityhc, uint32 overhc) :
    state_(CONNECTED),
    capacityh_(capacityh),
    overh_(overh),
    usageh_(0),
    capacityhc_(capacityhc),
    overhc_(overhc),
    usagehc_(0)
{
    assert(state_ < STATE_MAX);
}

void TokenHandler::Pipe::setState(const TokenHandler::Pipe::State state)
{
    assert(this);
    assert(state < STATE_MAX);
    state_ = state;
    assert(state < STATE_MAX);
}

TokenHandler::Pipe::State TokenHandler::Pipe::getState() const
{
    assert(this);
    assert(state_ < STATE_MAX);
    return state_;
}

void TokenHandler::Pipe::useTxTokens(uint16 tokens)
{
    assert(this);
    assert((usagehc_ + tokens) <= capacityhc_);
    usagehc_ += tokens;
}

uint16 TokenHandler::Pipe::freeRxTokens()
{
    assert(this);
    const uint16 freeing = (uint16) minval(0xffffu, usageh_);
    usageh_ -= freeing;
    return freeing;
}

FastPipeEngineUseRxTokensStatus TokenHandler::Pipe::useRxTokens(uint16 tokens)
{
    assert(this);

    if (capacityh_ == 0 ||
        usageh_ + tokens <= capacityh_)
    {
        usageh_ += tokens;
        return FASTPIPE_ENGINE_USE_RX_TOKENS_STATUS_SUCCESS;
    }
    else
    {
        usageh_ = capacityh_;
        return FASTPIPE_ENGINE_USE_RX_TOKENS_STATUS_ERROR_TOO_MANY_RX_TOKENS_USED_CHANNEL;
    }
}

uint16 TokenHandler::Pipe::getMaxTxTokens() const
{
    assert(this);
    assert(usagehc_ <= capacityhc_);
    return (uint16) minval(0xffffu, capacityhc_ - usagehc_);
}

uint32 TokenHandler::Pipe::getOverh() const
{
    assert(this);
    return overh_;
}

uint32 TokenHandler::Pipe::getOverhc() const
{
    assert(this);
    return overhc_;
}

TokenHandler::OperationStatus TokenHandler::Pipe::freeTxTokens(uint16 tokens)
{
    assert(this);
    if (tokens > usagehc_)
    {
        usagehc_ = 0;
        return TOO_MANY_TX_TOKENS_RETURNED_CHANNEL;
    }
    else
    {
        usagehc_ -= tokens;
        return OK;
    }
}

void TokenHandler::Pipe::getInfo(uint32 &rxUsed, uint32 &txUsed)
{
    assert(this);
    rxUsed = usageh_;
    txUsed = usagehc_;
}
