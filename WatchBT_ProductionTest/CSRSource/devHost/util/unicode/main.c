/**********************************************************************
*
*  FILE   :  main.c
*
*            Copyright (C) Cambridge Silicon Radio Ltd 2001-2009
*
*  PURPOSE:  Generic main unicode and non-unicode
*
*  $Id: //depot/bc/bluesuite_2_4/devHost/util/unicode/main.c#1 $
*
***********************************************************************/


/* Include library header file */

#include "main.h"


/* Calling convention for program entry point */

#ifdef _WINCE
#define CALLCONV _stdcall 
#else
#define CALLCONV
#endif


#ifdef UNICODE__ICHAR_WIDE


/* Windows platforms provide a wide character version of main */

#ifdef WIN32

int CALLCONV wmain(int argc, ichar *argv[])
{
    return imain(argc, argv);
}

#else

#error Wide characters are not currently supported for non-Win32 platforms
/* mbstowcs() could be used to convert the arguments */

#endif


#else


/* Simple forwarding function for single byte characters on all platforms */

int CALLCONV main(int argc, ichar *argv[])
{
    return imain(argc, argv);
}


#endif
