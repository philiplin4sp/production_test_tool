// Copyright (C) 2001-2006 CSR Ltd.  All rights reserved.
// Support for standard C functions which Windows CE fails to support.
// $Id: //depot/bc/bluesuite_2_4/devHost/util/wince/wincefuncs.h#1 $


#ifndef WINCE_FUNCS_INCLUDED
#define WINCE_FUNCS_INCLUDED

#include "common/types.h"

#include <windef.h>
#include <winbase.h>
#include <dbgapi.h>
#include <stdlib.h>


#ifdef __cplusplus
extern "C" {
#endif

#if (!defined(_WINCE) && !defined(_WIN32_WCE))
#error This file is only needed for Windoes CE
#endif

extern int errno;

extern int system (const char *);
extern char *strerror (int);
extern const char *getenv (const char *);
// typedef long time_t;
time_t time(time_t *timer);
// m32 mtime(void); // defined in globals.h


#ifndef isspace
int isxdigit(int c);
int isdigit(int c);
int isspace(int c);
int isalpha(int c);
int isalnum(int c);
int isprint(int c);
int isascii(int c);
int ispunct(int c);
char *strrchr(char *str, const char c);
size_t strspn( const char *string, const char *strCharSet );


#endif


#ifdef __cplusplus
} // extern "C" 
#endif


#endif // WINCE_FUNCS_INCLUDED
