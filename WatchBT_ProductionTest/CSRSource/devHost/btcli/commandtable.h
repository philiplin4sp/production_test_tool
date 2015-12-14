// Copyright (C) 2000-2001 Cambridge Silicon Radio Ltd.; all rights reserved.
// Functions to access the command table.
// $Id: //depot/bc/bluesuite_2_4/devHost/btcli/commandtable.h#1 $


#ifndef COMMANDTABLE_H
#define COMMANDTABLE_H


#include <stddef.h>	/* for size_t */

#include "typedefs.h"	/* for PA */


size_t getNumCommands (void);
unsigned getCommandOpcode (size_t i);
unsigned getCurrentCommandOpcode (void);
//(void (*) (const PA)) *getCommandCompleteDecoder (size_t i)
typedef void (*DECLWORKAROUND) (const PA pa); /* gcc chokes on the direct form */
DECLWORKAROUND getCommandCompleteDecoder (size_t i);
const char *getCommandAbbNam (size_t i);
const char *getCommandFullName (size_t i);
const char *getCurrentCommandName (void);


#endif /* COMMANDTABLE_H */
