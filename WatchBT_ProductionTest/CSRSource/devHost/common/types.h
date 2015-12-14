/* $Id: //depot/bc/bluesuite_2_4/devHost/common/types.h#1 $ */

#ifdef __linux__
#include <sys/types.h>
#endif

/*********Data types are defined for 64 bit compatibility:*********/
/*   
 *  Data types are differentiated according to 64 bit data models.
 *  64 bit data model is explained below
 *
 * Datatype  LP64 ILP64 LLP64 ILP32 LP32 16bit
 * char      8    8     8     8     8    8
 * short     16   16    16    16    16   16
 * int       32   64    32    32    16   16
 * long      64   64    32    32    32   32
 * long long 64         64    64
 * pointer   64   64    64    32    32   32
 *
 *  There is another option of differentiating according to compilers.
 */
#ifndef COMMON__TYPES_H
#define COMMON__TYPES_H

typedef unsigned char uint8;
typedef signed char int8;
typedef unsigned short uint16;
typedef signed short int16;
#ifdef __LP64__                   /*This flag is defined in Linux 64*/
typedef unsigned int uint32;
typedef signed int int32;
#else /*assume ILP32LL*/
typedef unsigned long uint32;
typedef signed long int32;
#endif
/*
 * [u]int24 is defined as [u]int32. This is the only data type which is not
 * "exactly the size" that it represents.
 */

typedef unsigned long long uint64;
typedef signed long long int64;
typedef uint32 uint24;           
typedef int32 int24;

typedef double float64;         /*used in test engine apis*/

#ifdef __linux__
typedef int SOCKET;
#endif

#endif /* COMMON__TYPES_H */
