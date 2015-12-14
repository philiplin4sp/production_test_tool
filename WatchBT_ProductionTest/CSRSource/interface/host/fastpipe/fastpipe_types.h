/****************************************************************************
FILE
    fastpipe_types.h - FastPipe information shared between host and firmware
*/

#ifndef __FASTPIPE_TYPES_H__
#define __FASTPIPE_TYPES_H__


typedef enum fastpipe_result
{
    FASTPIPE_CANNOT_BE_ENABLED,
    FASTPIPE_ALREADY_ENABLED,
    FASTPIPE_NOT_ENABLED,
    FASTPIPE_CREDIT_PIPE_NOT_CREATED,
    FASTPIPE_ID_IN_USE,
    FASTPIPE_TX_CAPACITY_REFUSED,
    FASTPIPE_RX_CAPACITY_REFUSED,
    FASTPIPE_TOO_MANY_PIPES_ACTIVE,
    FASTPIPE_WOULD_EXCEED_CONTROLLER_LIMIT,
    FASTPIPE_INVALID_PARAMETERS,
    FASTPIPE_INVALID_PIPE_ID,
    FASTPIPE_IS_CREDIT_PIPE,
    FASTPIPE_SUCCESS,
    FASTPIPE_WOULD_EXCEED_HOST_LIMIT
}fastpipe_result;


#endif /* __FASTPIPE_TYPES_H___ */
