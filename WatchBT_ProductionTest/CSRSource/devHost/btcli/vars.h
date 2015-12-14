// Copyright (C) 2000-2006 Cambridge Silicon Radio Ltd.; all rights reserved.
// Generic variable support.
// $Id: //depot/bc/bluesuite_2_4/devHost/btcli/vars.h#1 $


/*
 * A variable has a name and a value; both are strings.
 * Variable names are case-sensitive.
 * Variable values are immutable (though a variable's
 * value may be replaced, and a variable may be deleted).
 */


#ifndef VARS_H
#define VARS_H


#include <stddef.h>	/* for NULL */


/* Print the value of a variable, if it exists, else do nothing;
 * print the values of all the variables, if passed NULL */
void printVar (const char *name);

/* Delete a variable, if it exists, else do nothing */
void deleteVar (const char *name);

/* Find the value of a variable, if it exists, else return NULL */
const char *findVar (const char *name);

/* Add variable to list, if it doesn't exist, else replace its value */
void addVar (const char *name, const char *value);


#endif /* VARS_H */
