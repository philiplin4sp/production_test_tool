/**********************************************************************
*
*  FILE   :  portability.h
*
*            Copyright (C) Cambridge Silicon Radio Ltd 2004-2009
*
*  PURPOSE:  Common declarations with platform conditionals to 
*            emeliorate portability headaches
*
*  $Id: $
*
***********************************************************************/

#ifndef COMMON_PORTABILITY_H
#define COMMON_PORTABILITY_H

#if !(defined WIN32 || defined _WIN32_WCE)
#include "common/types.h"
typedef          long long LONGLONG;
typedef unsigned long long ULONGLONG;
typedef uint32 DWORD;
typedef uint32 UINT;
#ifdef __cplusplus
typedef bool BOOL;
#endif
static const uint32 INFINITE = ~((uint32)0);
#define MAX_PATH 65536
#endif

/* Visual Studio 2005 deprecates various POSIX and other fairly standard functions, 
   most of them simply require an underscore prefix to remove the warning... 
   _MSC_VER is 1400 for Visual Studio 2005 */
#if _MSC_VER >= 1400
# define STRICMP _stricmp
# define STRNICMP _strnicmp
# define ISATTY _isatty
# define STRDUP _strdup
# include <stdio.h>
# include <stdarg.h>
  __inline int csrvsnprintf(char *dest, size_t n, const char *fmt, va_list args)
  {
      int r = _vsnprintf(dest, n, fmt, args);
      if(dest && n > 0) dest[n-1]='\0';
      // if we replaced the last character with \0 then reduce r because we just truncated the output by 1.
      if(r > 0 && (size_t)r == n)
      {
          --r;
      }
      return r;
  }
  __inline int csrsnprintf(char *dest, size_t n, const char *fmt, ...)
  {
      int r;
      va_list args;
      va_start(args, fmt);
      r = csrvsnprintf(dest, n, fmt, args);
      va_end(args);
      return r;
  }
# define SNPRINTF csrsnprintf
# define VSNPRINTF csrvsnprintf
# define FILENO _fileno
# define CHDIR _chdir
# define STRLWR _strlwr

#else
# ifdef _MSC_VER
#  define STRICMP stricmp
#  include <stdio.h>
#  include <stdarg.h>
  __inline int csrvsnprintf(char *dest, size_t n, const char *fmt, va_list args)
  {
      int r = _vsnprintf(dest, n, fmt, args);
      if(dest && n > 0) dest[n-1]='\0';
      // if we replaced the last character with \0 then reduce r because we just truncated the output by 1.
      if(r > 0 && (size_t)r == n)
      {
          --r;
      }
      return r;
  }
  __inline int csrsnprintf(char *dest, size_t n, const char *fmt, ...)
  {
      int r;
      va_list args;
      va_start(args, fmt);
      r = csrvsnprintf(dest, n, fmt, args);
      va_end(args);
      return r;
  }
# define SNPRINTF csrsnprintf
# define VSNPRINTF csrvsnprintf
# else
#  include <stdio.h>
#  include <strings.h>
#  define STRICMP strcasecmp
#  define STRNICMP strncasecmp
#  define SNPRINTF snprintf
#  define VSNPRINTF vsnprintf
# endif
# define ISATTY isatty
# define STRDUP strdup
# define FILENO fileno
# define CHDIR chdir
# define STRLWR strlwr
#endif /* _MSC_VER >= 1400 */


#endif
