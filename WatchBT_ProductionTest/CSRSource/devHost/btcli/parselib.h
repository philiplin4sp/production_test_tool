// Copyright (C) 2000-2006 Cambridge Silicon Radio Ltd.; all rights reserved.
// Generic command line parsing support.
// $Id: //depot/bc/bluesuite_2_4/devHost/btcli/parselib.h#1 $


/*
 * Read-only lines are passed in.  Lines are expected to
 * start with a command, followed by whitespace-separated tokens.
 * Parsing works by attempting to match the current token
 * against an expected value (which may be textual, numeric or string),
 * preserving state if no match is found and moving to the start
 * of the next token (or end-of-line) if a match is found.
 */


#ifndef PARSELIB_H
#define PARSELIB_H


#include <stddef.h>	/* for size_t */

#include "typedefs.h"	/* for u32 */
#include "dictionary.h"


#define BADSIZE ((size_t) -1)

/*
 * Start the parser on a line.
 * The line may contain \r and/or \n.
 */
void startParse (const char *s);

/*
 * Is cursor at end of line?
 */
bool getDone (void);

/*
 * Try to match the current token to the literal word given.
 */
bool getKeyword(const char *word);

/*
 * Try to match current token in list of commands (abbreviated or full);
 * return the command's index in the list.
 */
bool getCommandIndex (size_t *index);

/*
 * Try to match current token to something which might be a variable
 * returning the result in a malloced buffer.
 * The buffer is NUL-terminated.
 */
char *getVar (void);

/*
 * Return the rest of line, excluding trailing \r or \n, in a malloced buffer.
 * The buffer is NUL-terminated.
 */
char *getRestOfLine (void);

/*
 * Are we at an equals sign?
 * Note this does NOT move on to the next token if we are;
 * it simply skips the equals sign and any following \r or \n.
 */
bool getEquals (void);

/*
 * Try to match current token to "-delimited string of given maximum size.
 * Return length or BADSIZE if error.
 * The buffer is NOT NUL-terminated.
 */
size_t getString (char *buf, size_t n);

/*
 * Try to match current token to "-delimited string of given maximum size.
 * Return length excluding terminaing NUL or BADSIZE if error.
 * The buffer IS NUL-terminated.
 * Slash escapes other than \\, \" and \^ are not treated as escapes.
 */
size_t getStringZ (char *buf, size_t n);

/*
 * Try to match current token to long hex number or one-digit
 * decimal number, of given maximum size (in octets).
 * Return length or 0 if error.
 * The buffer is NOT NUL-terminated.
 */
size_t getLongHex (u8 *buf, size_t n);

/*
 * Try to match current token in dictionary (if not NULL),
 * then try to match current token to hex, binary, octal or decimal value
 * of given maximum size (in bits, if not zero
 * (*result is not modified if no dictionary-only match is found)).
 */
bool get (int nbits, u32 *result, Dictionary dictionary);

/*
 * Try to match tokens with N or N * M, where N is an nbit size numeric and
 * M is a 32-bit multiplicity,
 */
bool getNumericMultiple(int nbits, u32 *result, u32 *multiplicity);

/*
 * Try to match tokens with a buffer 

/*
 * Try to match current token to BD_ADDR;
 * return last BD_ADDR if requested.
 */
bool getba (u32 *lap, u32 *uap, u32 *nap);

/*
 * Try to match current token to connection handle;
 * return last ACL connection handle if requested and valid.
 */
bool getch (u32 *ch);

/*
 * Similar to getch, but will return true even if the connection handle on the
 * command line is NOHANDLE. Basically detects if a connection handle was given
 * on the command line.
 */
bool getch_includeNOHANDLE(u32 *ch);

/*
 * Try to match current token to connection handle;
 * return last SCO connection handle or ACL connection handle
 * if requested and valid.
 */
bool getsch (u32 *ch);

/*
 * Substitute variable values for variables; return false on buffer overflow.
 */
bool substituteVars (char *sout, const char *sin, size_t n);

/*
 * Dynamic length string type
 */
struct dynstr {
	size_t capacity;
	size_t length;
	char *data;
};

/*
 * configure a dynstr
 */
void new_dynstr(struct dynstr *str, int start_length);

/*
 * deconfigure a dynstr
 */
void delete_dynstr(struct dynstr *str);

/*
 * Version of fgets that works on dynamically allocated strings.
 * Resizes string if capacity outgrown.
 */
struct dynstr *fgetsDynamic (struct dynstr* str, FILE *stream);

/* 
 * getStringZ that deals with dynamic length strings
 */
size_t getStringDynamicZ (struct dynstr *str);

/* 
 * getString that deals with dynamic length strings
 */
size_t getStringDynamic (struct dynstr *str);

/* 
 * substituteVars that deals with dynamic length strings
 */
bool substituteVarsDynamic (struct dynstr *sout, const char *sin);

/* 
 * gets a variable length list of uint8s off the command line, 
 * and puts them in a dynstr.
 */
bool get_uint8_list_Dynamic(struct dynstr *list);

/*
 * appends count chars from a vanilla C-string to a dynstr
 */
size_t appendToDynamic (struct dynstr *str, char *chars, int count);


#endif /* PARSELIB_H */
