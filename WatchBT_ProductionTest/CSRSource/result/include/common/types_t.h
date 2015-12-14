/* $Id: //depot/bc/bluesuite_2_4/devHost/common/types_t.h#1 $ */

#ifdef __linux__
#include <sys/types.h>
#endif

#ifndef COMMON__TYPES_T_H
#define COMMON__TYPES_T_H

#include "common/types.h"

typedef uint8     bool_t;

#if defined(__sun__) || defined(__APPLE_CC__) || defined(__linux__)
#include <inttypes.h> /* C9X header */
#else
typedef uint8     uint8_t;
typedef uint16    uint16_t;
typedef uint32    uint32_t;
typedef int8      int8_t;
typedef int16     int16_t;
typedef int32     int32_t;
#endif

typedef uint32_t    uint24_t;
typedef int32_t     int24_t;

typedef uint16_t    phandle_t;

#endif /* COMMON__TYPES_T_H */
