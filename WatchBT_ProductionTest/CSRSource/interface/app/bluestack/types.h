/*!
\author Copyright (C) Cambridge Consultants Ltd 1999
\author Copyright (C) CSR plc 2009

        All rights reserved

\file   types.h

\brief This file contains the type definitions.
*/

#ifndef BLUESTACK__TYPES_H
#define BLUESTACK__TYPES_H

#ifndef BLUELAB
#ifndef NULL
#define NULL    0
#endif /* ndef NULL */

#ifndef FALSE
#define FALSE   0
#endif /* ndef  FALSE */

#ifndef TRUE
#define TRUE    1
#endif /* ndef  TRUE */
#endif /* ndef BLUELAB */

#ifdef BLUESTACK_TYPES_INT_TYPE_FILE
#define BLUESTACK_TYPES_DONE
#include BLUESTACK_TYPES_INT_TYPE_FILE
#endif /* BLUESTACK_TYPES_INT_TYPE_FILE */

#ifdef BLUESTACK_TYPES_INT_TYPE_DEFS
#define BLUESTACK_TYPES_DONE
BLUESTACK_TYPES_INT_TYPE_DEFS
#endif /* BLUESTACK_TYPES_INT_TYPE_DEFS */

#ifndef BLUESTACK_TYPES_DONE

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int int16_t;
typedef unsigned short int uint16_t;

#ifndef DONT_HAVE_LIMITS_H
#include <limits.h>
#endif /* ndef DONT_HAVE_LIMITS_H */

#if defined(USHRT_MAX) && (USHRT_MAX >= 0xFFFFFFFFUL)
typedef signed short int int32_t;
typedef unsigned short int uint32_t;
#elif defined(UINT_MAX) && (UINT_MAX >= 0xFFFFFFFFUL)
typedef signed int int32_t;
typedef unsigned int uint32_t;
#else
typedef signed long int int32_t;
typedef unsigned long int uint32_t;
#endif

typedef uint8_t bool_t;
typedef uint32_t uint24_t;

#endif /* ndef BLUESTACK_TYPES_DONE */
#undef BLUESTACK_TYPES_DONE

typedef uint16_t phandle_t;
#define INVALID_PHANDLE ((phandle_t)0xFFFF)

#endif /* ndef BLUESTACK__TYPES_H */
