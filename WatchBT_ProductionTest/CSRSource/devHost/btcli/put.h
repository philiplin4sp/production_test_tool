// Copyright (C) 2000-2006 Cambridge Silicon Radio Ltd.; all rights reserved.
// Building and sending HCI commands.
// $Id: //depot/bc/bluesuite_2_4/devHost/btcli/put.h#1 $


#ifndef PUT_H
#define PUT_H


#include <stddef.h>	/* for size_t */

#include "typedefs.h"	/* for u32 */


/*
 * Start a command PA which will contain n parameters
 * (excluding the length and opcode parameters)
 */
void putCmd (size_t n);

/*
 * Put a single parameter.
 */
void put (u32 n);

/*
 * Put an array.
 */
void putArray (u8 a[], size_t n);

/*
 * Dispatch a command PA.
 */
void putDone (void);


#endif /* PUT_H */
