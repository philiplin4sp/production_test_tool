/****************************************************************************
FILE
        slut_fingerprint.h  -  identifies the pointer to slut[]

DESCRIPTION
        This file contains the word used to prefix the pointer to the
        slut[] symbol look up table.

        This file deliberately does not #include bt.h so as to facilitate
        its use in host code.

MODIFICATION HISTORY
        1.1   14:mar:01  ajh    Split out of slut.h
        1.2   13:nov:01  cjo    Typos.
        ------------------------------------------------------------------
        --- This modification history is now closed. Do not update it. ---
        ------------------------------------------------------------------
*/

#ifndef __SLUT_FINGERPRINT_H__
#define __SLUT_FINGERPRINT_H__


/* We expect the fingerprint to appear at address 0x0100 and the pointer
   at address 0x0101 */

/* Arbitrary magic number. */
#define SLUT_FINGERPRINT        (0xD397)


#endif  /* __SLUT_FINGERPRINT_H__ */
