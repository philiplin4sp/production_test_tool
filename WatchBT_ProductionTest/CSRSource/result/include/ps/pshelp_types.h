/* pshelp_types.h

 Contains the fundamental types used across the pshelp system

*/

/*
MODIFICATION HISTORY
  $Log:$

RCS IDENTIFIER
  $Id: //depot/bc/bluesuite_2_4/devHost/PersistentStore/PSHelp/pshelp_types.h#1 $

*/


#ifndef PSHELP_TYPES_INCLUDED
#define PSHELP_TYPES_INCLUDED

#include "unicode/ichar.h"
#include "common/types.h"


typedef uint16 pskey;
/* List of known pskeys *with* their perms bits.  Terminated by PSKEY_NULL. */
extern const pskey psbckeys[];


#endif/*PSHELP_TYPES_INCLUDED*/

