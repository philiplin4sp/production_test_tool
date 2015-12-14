// Copyright (C) 2000-2006 Cambridge Silicon Radio Ltd.; all rights reserved.
// Generic dictionary support.
// $Id: //depot/bc/bluesuite_2_4/devHost/btcli/dictionary.h#1 $


/*
 * Dictionaries are names with corresponding values.
 *
 * A name is a case-sensitive string, a value is a 32-bit unsigned integer;
 * both are immutable.
 *
 * A Dictionary consists of a sequence of DictionaryPairs
 * terminated by a DictionaryPair with a NULL name.
 * Note that when a Dictionary is searched by value the
 * first matching name in the sequence is returned.
 */


#ifndef DICTIONARY_H
#define DICTIONARY_H


#include <stddef.h>	/* for NULL */

#include "typedefs.h"	/* for u32 */


typedef struct DictionaryPair
{
	const u32 value;
	const char *name;
} DictionaryPair;

typedef DictionaryPair Dictionary[];


/* Look value up in dictionary; return NULL if not found */
const char *lookupByValue (const Dictionary dictionary, u32 value);

/* return largest value defined in the dictionary */
u32 getMax (const Dictionary dictionary);

/* Look name up in dictionary; return false if not found, preserving *value */
bool lookupByName (const Dictionary dictionary, const char *name, u32 *value);

/* Look part of name up in dictionary; print matches; return false if not found */
bool printMatchByName (const Dictionary dictionary, const char *name);

#endif /* DICTIONARY_H */
