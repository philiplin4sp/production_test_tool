#ifndef FASTPIPE_ENGINE_STATUS_CODES_H
#define FASTPIPE_ENGINE_STATUS_CODES_H

typedef enum
{
    FASTPIPE_ENGINE_USE_RX_TOKENS_STATUS_SUCCESS,
    FASTPIPE_ENGINE_USE_RX_TOKENS_STATUS_ERROR_TOO_MANY_RX_TOKENS_USED_CHANNEL,
    FASTPIPE_ENGINE_USE_RX_TOKENS_STATUS_ERROR_TOO_MANY_RX_TOKENS_USED_BOTH,
    FASTPIPE_ENGINE_USE_RX_TOKENS_STATUS_ERROR_TOO_MANY_RX_TOKENS_USED_TOTAL,
    FASTPIPE_ENGINE_USE_RX_TOKENS_STATUS_MAX
} FastPipeEngineUseRxTokensStatus;

#endif
