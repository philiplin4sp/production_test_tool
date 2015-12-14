/**********************************************************************
*
*  FILE   :  main.h
*
*            Copyright (C) Cambridge Silicon Radio Ltd 2001-2009
*
*  PURPOSE:  Generic main unicode and non-unicode
*
*  $Id: //depot/bc/bluesuite_2_4/devHost/util/unicode/main.h#1 $
*
***********************************************************************/

#ifndef UNICODE__MAIN_H
#define UNICODE__MAIN_H

/* Include header files */
#include "unicode/ichar.h"


#ifdef  __cplusplus
extern "C" {
#endif

/* The program using this library should define the following function */
int imain(int argc, ichar **argv);

#ifdef  __cplusplus
}
#endif


#endif
