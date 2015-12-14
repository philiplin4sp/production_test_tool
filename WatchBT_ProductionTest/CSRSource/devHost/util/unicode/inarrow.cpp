/**********************************************************************
*
*  FILE   :  inarrow.cpp
*
*            Copyright (C) Cambridge Silicon Radio Ltd 2001-2009
*
*  PURPOSE:  unicode widening and narrowing functions.
*
*  $Id: //depot/bc/bluesuite_2_4/devHost/util/unicode/inarrow.cpp#1 $
*
***********************************************************************/

/* Include library header file */

#include "ichar.h"
#if defined(WIN32) || defined(_WIN32_WCE)  
#define strdup _strdup
#endif

/* Include system header files */


/*
    C callable equivalent functions
*/
char *iinarrowdup(const ichar *string)
{
	return strdup(inarrow(string).c_str());
}

ichar *iccoercedup(const char *string)
{
    return istrdup(icoerce(string).c_str());
}

/*
    String conversions.
*/

std::string inarrow(const std::string &string)
{
    return string;
}

istring icoerce(const std::string &string)
{
#ifdef UNICODE__ICHAR_WIDE
    return iwiden(string);
#else
    return string;
#endif
}

#ifdef UNICODE__ICHAR_SUPPORT_WIDE
std::wstring iwiden(const std::string &string)
{
    const char *a = string.data();
    size_t need = mbstowcs(0, a, string.size());
    if ( need != (size_t) -1 )
    {
		struct Buff { wchar_t *buff; explicit Buff(size_t n) : buff(new wchar_t[n]) {} ~Buff() { delete [] buff; } } buff(need);
        mbstowcs(buff.buff, a, string.size());
        return std::wstring(buff.buff, need);
    }
    else
    {
        // the api call has failed.  Lets do something stright forward.
        std::wstring rv;
        for ( size_t i = 0 ; i < string.size() ; ++i )
            rv += (wchar_t)string[i];
        return rv;
    }
}

std::string inarrow(const std::wstring &string)
{
    const wchar_t *w = string.data();
    size_t need = wcstombs(0, w, string.size());
    if ( need != (size_t) -1 )
    {
		struct Buff { char *buff; explicit Buff(size_t n) : buff(new char[n]) {} ~Buff() { delete [] buff; } } buff(need);
        wcstombs(buff.buff, w, string.size());
        return std::string(buff.buff, need);
    }
    else
    {
        // the api call has failed.  Lets do something stright forward.
        std::string rv;
        for ( size_t i = 0 ; i < string.size() ; ++i )
            rv += (char)(string[i] & 0xFF); 
        // this may result in unprintable characters being generated.  Tough.
        return rv;
    }
}

istring icoerce(const std::wstring &string)
{
#ifdef UNICODE__ICHAR_WIDE
    return string;
#else
    return inarrow(string);
#endif
}

std::wstring iwiden(const std::wstring &string)
{
    return string;
}
#endif


/*
    Filename conversions to allow use with fstream constructors etc.
    This may need to be platform dependent, but currently just use the
    platform independent conversions defined above. If this does need
    to be done then only two functions (marked below) need changing.
*/

std::string inarrow_filename(const std::string &filename)
{
    return filename;
}

istring icoerce_filename(const std::string &filename)
{
#ifdef UNICODE__ICHAR_WIDE
    return iwiden_filename(filename);
#else
    return filename;
#endif
}

#ifdef UNICODE__ICHAR_SUPPORT_WIDE
std::wstring iwiden_filename(const std::string &filename)
{
    // Platform specific widening
    return iwiden(filename);
}

std::string inarrow_filename(const std::wstring &filename)
{
    // Platform specific narrowing
    return inarrow(filename);
}

istring icoerce_filename(const std::wstring &filename)
{
#ifdef UNICODE__ICHAR_WIDE
    return filename;
#else
    return inarrow_filename(filename);
#endif
}

std::wstring iwiden_filename(const std::wstring &filename)
{
    return filename;
}
#endif
