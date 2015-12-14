/* psenum.h

 * This file appears to actually do some of what PSKEY_ENUM_INCLUDE should do.
 * (B-12576)

// Replacement  header file to #define the stuff pertinent to the persistent store project without 
// needing to put the whole of the bluetooth source with the DLL.

// The problem with the existing DLL's is their interdependancy - you #include one and you end up 
// pulling in everything.

// A more sensible approach is to treat the headers as interfaces to modules, such that any .c file
// that requires the services of two other modules will #include those two.

// often .h files are dependant on other .h files, and this should be implemented by #includes within the headers.
// But dependancy of code on base services should be left to the .c files, not the .h files.  Let the .c file #include multiple files.
// In this manner the interfaces to particular modules are encapsulated within the header file
// and extraneous #includes are kept to a minimum.

*/

/*
MODIFICATION HISTORY
		1.10   13:dec:00  Chris Lowe 	Working
		1.7   28:mar:01  Chris Lowe 	#inc's changed for Result dir
                ...
                #4    07:mar:06  jn01           Add "fm_api.h".

RCS IDENTIFIER
  $Id: //depot/bc/bluesuite_2_4/devHost/PersistentStore/PSHelp/psenum.h#1 $

*/

#ifndef	PSENUM_INCLUDED
#define	PSENUM_INCLUDED

#include "host/io/io_digital.h"
#include "host/io/io_bt.h"
#include "host/fm/fm_api.h"

#ifndef TRUE
#define TRUE (true)
#define FALSE (false)
#endif


/* from psbc_private.h */

/* Access controls for bccmd - implemented as pskey bitmask/offsets. */
#define  PS_NOACCESS             ((pskey)(0))
#define  PS_READONLY             ((pskey)(0x0800))
#define  PS_WRITEONLY            ((pskey)(0x1000))
#define  PS_READWRITE            ((pskey)(0x1800))

/* Marginally more legible aliases for two of the above. */
#define  PS_READABLE             (PS_READONLY)
#define  PS_WRITABLE             (PS_WRITEONLY)

/* Bitmask of the two above access control bits. */
#define  PS_ACCESS_BITS          (0x1800)

/* Bit indicating data is held in ps without its access control bits. */
#define  PS_NO_PERMS             ((pskey)(0x2000))


/* Clear the two permissions bits from a pskey. */
#define basekey(k)              ((k)&(~PS_ACCESS_BITS))

/* Clear the two permissions bits and set the "fixed" bit in a pskey. */
#define fixkey(k)               (basekey(k)|PS_NO_PERMS)


#ifndef NULL
//#define NULL ((void *)(0))
#define NULL (0)
#endif


/* from rtime.h */

/* TIME constants. */
typedef uint32 TIME;

#define	MILLISECOND	((TIME)(1000))
#define	SECOND		((TIME)(1000 * MILLISECOND))
#define	MINUTE		((TIME)(60 * SECOND))

/* these definitions are required by the enumeration pairs. */

/* constants required by default values in psbc_data. */
/* these are dummies and not very sensible and are here just to make it compile */
#define MAX_NUM_ACLS 0
#define MAX_NUM_SCOS 0

#endif	/* PSENUM_INCLUDED */
