/*
 * Copyright CSR 2008
 *
 * $Id: //depot/bc/bluesuite_2_4/devHost/engine/fastpipe_engine/token_util.h#1 $
 *
 * Purpose: Defines a number of things useful for handling tokens.
 *
 */

#ifndef TOKEN_UTIL_H
#define TOKEN_UTIL_H

#define TOKEN_SIZE 3
#define PIPE_OFFSET 0
#define CREDIT_OFFSET 1

#include "common/types.h"

extern uint16 composeCredits(const uint8 first, const uint8 second);
extern void decomposeCredits(const uint16 credits, uint8 &first, uint8 &second);

#endif
