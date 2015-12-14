///////////////////////////////////////////////////////////////////////////////
//
//  FILE: algorithm.h
//
//  PURPOSE: A filter for <algorithm>, to correct windows-isms on WIN32
//
///////////////////////////////////////////////////////////////////////////////

#ifndef COMMON_ALGORITHM_HEADER_FILE
#define COMMON_ALGORITHM_HEADER_FILE
#ifdef __cplusplus

#include <algorithm>

#if defined(WIN32) || defined(_WIN32_WCE)
# ifdef min
#  undef min
# endif
# ifdef max
#  undef max
# endif
# ifndef NOMINMAX
#  define NOMINMAX
# endif
# if (_MSC_VER <=1300)
#  define min(a,b) _cpp_min(a,b)
#  define max(a,b) _cpp_max(a,b)
# endif
#endif

#endif
#endif

