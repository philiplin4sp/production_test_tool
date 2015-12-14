// Copyright (C) 2000-2006 Cambridge Silicon Radio Ltd.; all rights reserved.
// Various console output functions.
// $Id: //depot/bc/bluesuite_2_4/devHost/btcli/print.h#1 $


#ifndef PRINT_H
#define PRINT_H


//#include <stddef.h>	/* for NULL */

#include "typedefs.h"	/* for u32 and PA */
#include "dictionary.h"

#ifdef __cplusplus
extern "C"
{
#endif

void print_init();

/*
 * Look value up in dictionary (if not NULL) and print if found,
 * otherwise print as hex value of given size (in bits), with prefix.
 */
void printByValue (Dictionary dictionary, u32 value, int size, const char *prefix);

/* Print BD_ADDR */
void printba (u32 lap, u32 uap, u32 nap);

/* Print connection handle */
void printch (u32 ch);

/* Print string of given maximum size (not necessarily NUL-terminated) */
void printString (const PA pa, size_t n);

/* Print data block of given size as string if only consists of
   ASCII printables or CR/LF/NUL, else print as hex */
void printStringOrHexes (const u8 *data, size_t n);

/* Print data block of given size as hex quintets, separated by hyphens */
void printQuintets (const u8 *data, size_t n);


/* Print value as HEX and decimal */

void printHexAndDec(u32 value, int hexsize, int digits, const char *prefix);



/* Print Extended Inquiry Response Data */

void printEIRD (const u32 *data, u32 len);


/* Print LMP features */
void printFeaturesPageX (int page, const PA pa, const char *prefix);

/* Print LE Features */
void printLEFeatures(const PA pa, const char *prefix);

/* Print LE Supported States */
void printLESupportedStates(const PA pa, const char *prefix);

/* Print a character, appropriately escaped */
void printChar (char c);

/* Print the syntax of the current command (simple case) */
void printSyntax (const char *s);

/* Print syntax and name of current command (complex case) */
void printSyntaxCommand (void);

/* Finalise syntax line (complex case)*/
void printSyntaxEnd (void);

/* Print the syntax of the current command (start of normal complex case) */
void printSyntaxStart (void);

/* Print the syntax of the current command (continuation of complex case) */
void printSyntaxContin (const char *s);

/* Print status */
void printStatus (u32 statusCode);

void printStatus_ulp (u32 statusCode);

void printReason (u32 reasonCode);

void printAddressType (u32 addressType);

void printAddressUlp (u32 o1, u32 o2, u32 o3, u32 o4, u32 o5, u32 o6);

void printMoreProfiles (u32 moreProfiles);

void printEncRequired (u32 encRequired);

void printEventType (u32 eventType);

/* Set whether to enable printing of timestamps */
void printTimestamps (int enable);

/* Set whether to enable printing of tags */
void printTags (const char *tag);

/* Return the current time in buf, which must be (at least) 13 chars long */
void getTimestamp (char *buf);

/* Print tag and/or timestamp, if enabled */
void printTimestamp (void);

/* Print tag, if enabled */
void printTag (void);

/* Open log file (return non-zero if success) */
int printlOpen (const ichar *s);

/* Is a log file open? */
bool printLog (void);

/* Are we using timestamps or tags? */
bool printPrefix (void);

/* Close log file, if open */
void printlClose (void);

/* Print to log file, if open */
void printlfOnly (const char *s);
void iprintlfOnly (const ichar *s);

/* Print to stdout, and to log file, if open */
int printlf (const char *s, ...);
int iprintlf (const ichar *s, ...);

/* Print supported HCI commands */
void printCommands (const PA pa);

#ifdef __cplusplus
}
#endif

#endif /* PRINT_H */
