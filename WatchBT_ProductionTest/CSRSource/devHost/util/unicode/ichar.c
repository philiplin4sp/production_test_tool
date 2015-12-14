/**********************************************************************
*
*  FILE   :  ichar.c
*
*            Copyright (C) Cambridge Silicon Radio Ltd 2001-2009
*
*  PURPOSE:  ichar functions.
*
*  $Id: //depot/bc/bluesuite_2_4/devHost/util/unicode/ichar.c#1 $
*
***********************************************************************/


/* Include library header file */

#include "ichar.h"
#if (defined(WIN32) && !defined(_WINCE)) || !defined(_POSIX_C_SOURCE)
#include "common/portability.h"
#endif

/* Include system header files */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>


/* Needed to work out whether we're running on W98 */
#if defined (_WIN32) && defined (UNICODE__ICHAR_WIDE)
#include <windows.h>
#endif


/* String manipulation */

ichar *istrchr(const ichar *string, iint c)
{
    return ICHAR_SELECT(strchr, wcschr)(string, c);
}

size_t istrcspn(const ichar *string, const ichar *strCharSet)
{
    return ICHAR_SELECT(strcspn, wcscspn)(string, strCharSet);
}

ichar *istrcat(ichar *strDestination, const ichar *strSource)
{
    return ICHAR_SELECT(strcat, wcscat)(strDestination, strSource);
}

ichar *istrncat(ichar *strDest, const ichar *strSource, size_t count)
{
    return ICHAR_SELECT(strncat, wcsncat)(strDest, strSource, count);
}

ichar *istrcpy(ichar *strDestination, const ichar *strSource)
{
    return ICHAR_SELECT(strcpy, wcscpy)(strDestination, strSource);
}

ichar *istrncpy(ichar *strDest, const ichar *strSource, size_t count)
{
    return ICHAR_SELECT(strncpy, wcsncpy)(strDest, strSource, count);
}

ichar *istrpbrk(const ichar *string, const ichar *strCharSet)
{
    return ICHAR_SELECT(strpbrk, wcspbrk)(string, strCharSet);
}

ichar *istrrchr(const ichar *string, iint c)
{
#if !defined _WINCE && !defined _WIN32_WCE
    return ICHAR_SELECT(strrchr, wcsrchr)(string, c);
#else
    const ichar *last = NULL;
    while (*string)
    {
        if (*string == c)
            last = string;
        ++string;
    }
    return (ichar *)last;
#endif
}

size_t istrspn(const ichar *string, const ichar *strCharSet)
{
    return ICHAR_SELECT(strspn, wcsspn)(string, strCharSet);
}

ichar *istrstr(const ichar *string, const ichar *strCharSet)
{
    return ICHAR_SELECT(strstr, wcsstr)(string, strCharSet);
}

ichar *istrtok(ichar *strToken, const ichar *strDelimit)
{
    return ICHAR_SELECT(strtok, wcstok)(strToken, strDelimit);
}

int istrcmp(const ichar *string1, const ichar *string2)
{
#if !defined _WINCE && !defined _WIN32_WCE
    return ICHAR_SELECT(strcmp, wcscmp)(string1, string2);
#else
    while (*string1 && *string2 && (*string1 == *string2))
    {
        string1++; 
        string2++;
    }
    return (*string1 - *string2);
#endif
}

int istricmp(const ichar *string1, const ichar *string2)
{
#if defined(WIN32) && !defined(_WINCE)
    return ICHAR_SELECT(STRICMP, _wcsicmp)(string1, string2);
#else
    while (*string1 && *string2 && (itolower(*string1) == itolower(*string2)))
    {
        string1++;
        string2++;
    }
    return itolower(*string1) - itolower(*string2);
#endif
}

int istrncmp(const ichar *string1, const ichar *string2, size_t count)
{
    return ICHAR_SELECT(strncmp, wcsncmp)(string1, string2, count);
}

size_t istrlen(const ichar *string)
{
    return ICHAR_SELECT(strlen, wcslen)(string);
}

ichar *istrdup(const ichar *string)
{
#ifndef _POSIX_C_SOURCE
    return ICHAR_SELECT(STRDUP, _wcsdup)(string);
#else
    ichar* s = NULL;
    if ( string )
    {
        s = calloc(strlen(string)+1,sizeof(ichar));
        strcpy(s,string);
    }
    return s;
#endif
}

#if !defined(_WINCE) && !defined(_WIN32_WCE)
int istrcoll(const ichar *string1, const ichar *string2)
{
    return ICHAR_SELECT(strcoll, wcscoll)(string1, string2);
}
#endif

#if !defined(_WINCE) && !defined(_WIN32_WCE)
size_t istrxfrm(ichar *strDest, const ichar *strSource, size_t count)
{
    return ICHAR_SELECT(strxfrm, wcsxfrm)(strDest, strSource, count);
}
#endif

/* Character manipulation */

int iisalnum(iint c)
{
#if !defined(_WINCE) && !defined(_WIN32_WCE)
    return ICHAR_SELECT(isalnum, iswalnum)(c);
#else
    return (  ((c >= 'A') && (c <= 'Z'))
           || ((c >= '0') && (c <= '9'))
           || ((c >= 'a') && (c <= 'z')) );
#endif
}

int iisalpha(iint c)
{
    return ICHAR_SELECT(isalpha, iswalpha)(c);
}

int iisdigit(iint c)
{
#if !defined(_WINCE) && !defined(_WIN32_WCE)
    return ICHAR_SELECT(isdigit, iswdigit)(c);
#else
    return ((c >= II('0')) && (c <= II('9')));
#endif
}

int iisgraph(iint c)
{
    return ICHAR_SELECT(isgraph, iswgraph)(c);
}

int iislower(iint c)
{
    return ICHAR_SELECT(islower, iswlower)(c);
}

int iisprint(iint c)
{
    return ICHAR_SELECT(isprint, iswprint)(c);
}

int iispunct(iint c)
{
    return ICHAR_SELECT(ispunct, iswpunct)(c);
}

int iisspace(iint c)
{
#if !defined(_WINCE) && !defined(_WIN32_WCE)
    return ICHAR_SELECT(isspace, iswspace)(c);
#else
    return ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r'));
#endif
}

int iisupper(iint c)
{
    return ICHAR_SELECT(isupper, iswupper)(c);
}

#ifndef _POSIX_C_SOURCE
int iisascii(iint c)
{
    return ICHAR_SELECT(isascii, iswascii)(c);
}
#endif

int iiscntrl(iint c)
{
    return ICHAR_SELECT(iscntrl, iswcntrl)(c);
}

int iisxdigit(iint c)
{
#if !defined(_WINCE) && !defined(_WIN32_WCE)
    return ICHAR_SELECT(isxdigit, iswxdigit)(c);
#else
    return ( ((c >= II('A')) && (c <= II('F')))
           || ((c >= II('a')) && (c <= II('f')))
           || ((c >= II('0')) && (c <= II('9'))) );
#endif
}

int itoupper(iint c)
{
    return ICHAR_SELECT(toupper, towupper)(c);
}

int itolower(iint c)
{
    return ICHAR_SELECT(tolower, towlower)(c);
}


/* Formatted I/O */

int iprintf(const ichar *format, ...)
{
    int count;
    va_list argptr;
    va_start(argptr, format);
    count = ICHAR_SELECT(vprintf, vwprintf)(format, argptr);
    va_end(argptr);
    return count;
}

int ifprintf(FILE *stream, const ichar *format, ...)
{
    int count;
    va_list argptr;
    va_start(argptr, format);
    count = ICHAR_SELECT(vfprintf, vfwprintf)(stream, format, argptr);
    va_end(argptr);
    return count;
}

int isprintf(ichar *buffer, size_t max_size, const ichar *format, ...)
{
	int count;
	va_list argptr;
	va_start(argptr, format);
#ifdef UNICODE__ICHAR_WIDE
#ifdef _INC_SWPRINTF_INL_
	count = vswprintf(buffer, max_size, format, argptr);
#else
	count = vswprintf(buffer, format, argptr);
#endif
#else
	count = vsprintf(buffer, format, argptr);
#endif	
	va_end(argptr);
	return count;
}

int ivprintf(const ichar *format, va_list argptr)
{
    return ICHAR_SELECT(vprintf, vwprintf)(format, argptr);
}

int ivfprintf(FILE *stream, const ichar *format, va_list argptr)
{
    return ICHAR_SELECT(vfprintf, vfwprintf)(stream, format, argptr);
}

int ivsprintf(ichar *buffer, size_t max_size, const ichar *format, va_list argptr)
{
#ifdef UNICODE__ICHAR_WIDE
#ifdef _INC_SWPRINTF_INL_
	return  vswprintf(buffer, max_size, format, argptr);
#else
	return  vswprintf(buffer, format, argptr);
#endif
#else
	return  vsprintf(buffer, format, argptr);
#endif	
}


/* Unformatted I/O */

iint ifgetc(FILE *stream)
{
    return ICHAR_SELECT(fgetc, fgetwc)(stream);
}

ichar *ifgets(ichar *string, int n, FILE *stream)
{
    return ICHAR_SELECT(fgets, fgetws)(string, n, stream);
}

iint ifputc(iint c, FILE *stream)
{
    return ICHAR_SELECT(fputc, fputwc)(c, stream);
}

iint ifputs(const ichar *string, FILE *stream)
{
    return ICHAR_SELECT(fputs, fputws)(string, stream);
}

iint igetc(FILE *stream)
{
    return ICHAR_SELECT(getc, getwc)(stream);
}

iint igetchar()
{
    return ICHAR_SELECT(getchar, getwchar)();
}

/* Don't use this function, use ifgets instead
ichar *igets(ichar *buffer)
{
    return ICHAR_SELECT(gets, _getws)(buffer);
}
*/

iint iputc(iint c, FILE *stream)
{
    return ICHAR_SELECT(putc, putwc)(c, stream);
}

iint iputchar(iint c)
{
    return ICHAR_SELECT(putchar, putwchar)(c);
}

iint iputs(const ichar *string)
{
    return ICHAR_SELECT(puts, _putws)(string);
}

iint iungetc(iint c, FILE *stream)
{
    return ICHAR_SELECT(putc, putwc)(c, stream);
}

/* String conversion */

double istrtod(const ichar *nptr, ichar **endptr)
{
    return ICHAR_SELECT(strtod, wcstod)(nptr, endptr);
}

long istrtol(const ichar *nptr, ichar **endptr, int base)
{
#if !defined _WINCE && !defined _WIN32_WCE
    return ICHAR_SELECT(strtol, wcstol)(nptr, endptr, base);
#else
//  ASSERT(base == 10);
  const ichar *p = nptr;
  if (*p == II('-'))
  {
      p++;
      return 0L - (long)istrtoul(p, endptr, base);
  }
  if (*p == II('+'))
      p++;
  return (long) istrtoul(p, endptr, base);
#endif
}

unsigned long istrtoul(const ichar *nptr, ichar **endptr, int base)
{
#if !defined _WINCE && !defined _WIN32_WCE
    return ICHAR_SELECT(strtoul, wcstoul)(nptr, endptr, base);
#else
    const ichar *p = nptr;
    unsigned long val = 0;
    // ASSERT(base == 10);
    while (iisdigit(*p))
    {
        val = val * 10;
        val = val + ((*p) - II('0'));
        p++;
    }
    if (endptr)
        *endptr = (ichar *)p;
    return val;
#endif
}

int iatoi(const ichar *string)
{
    return ICHAR_SELECT(atoi, _wtoi)(string);
}

long iatol(const ichar *string)
{
    return ICHAR_SELECT(atol, _wtol)(string);
}

#if !defined _WINCE && !defined _WIN32_WCE
double iatof(const ichar *string)
{
    double value;
#ifdef UNICODE__ICHAR_WIDE
    /*
        **  using swscanf fails... dunno why, but work around it (AJH)
        **  if (swscanf(string, L"%f", &value) != 1) value = 0.0;
        */
        char * mbc_string;
        size_t need = wcstombs(0, string, istrlen(string));
        /* allocate on the stack */
        mbc_string = malloc ( need );
        if ( mbc_string )
        {
            wcstombs(mbc_string, string, istrlen(string));
            value = atof(mbc_string);
            free ( mbc_string );
        }
        else
            value = 0.0;
#else
    value = atof(string);
#endif
    return value;
}

#endif

/* Command execution */
#if !defined(_WINCE) && !defined(_WIN32_WCE)
int isystem(const ichar *command)
{
    return ICHAR_SELECT(system, _wsystem)(command);
}
#endif

ichar *igetenv(const ichar *varname)
{
#if !defined(_WINCE) && !defined(_WIN32_WCE)
    return ICHAR_SELECT(getenv, _wgetenv)(varname);
#else
    return 0;
#endif
}

/* Time processing */
#if !defined(_WINCE) && !defined(_WIN32_WCE)
ichar *iasctime(const struct tm *timeptr)
{
    return ICHAR_SELECT(asctime, _wasctime)(timeptr);
}
#endif

#if !defined(_WINCE) && !defined(_WIN32_WCE)
ichar *ictime(const time_t *timer)
{
    return ICHAR_SELECT(ctime, _wctime)(timer);
}
#endif

#if !defined(_WINCE) && !defined(_WIN32_WCE)
size_t istrftime(ichar *strDest, size_t maxsize, const ichar *format, const struct tm *timeptr)
{
    return ICHAR_SELECT(strftime, wcsftime)(strDest, maxsize, format, timeptr);
}
#endif


/* File handling */

FILE *ifopen(const ichar *filename, const ichar *mode)
{
#if defined (_WIN32) && defined (UNICODE__ICHAR_WIDE)
    /*
     * _wfopen() appears to be broken under Windows98
     * (you get errno 9 "Bad file descriptor"!).
     * Work around this by converting to multibyte character strings;
     * there's no loss of generality because W98 only supports FAT
     * and this only supports 8-bit characters in filenames.
     */
    OSVERSIONINFO osvi;
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx (&osvi);
    if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) /* "better" versions of Windows return _NT */
    {
        size_t needf, needm;
        char *mbfilename, *mbmode;
        FILE *f;

        needf = wcstombs (NULL, filename, 0); /* This appears to be a M$ extension */
        needm = wcstombs (NULL, mode, 0);
        if (needf == -1 || needm == -1)
        {
            fprintf (stderr, "Couldn't handle wchars in ifopen()\n");
            exit (1);
        }

        mbfilename = malloc (needf + 1); /* + 1 "just in case" */
        mbmode = malloc (needm + 1); /* + 1 "just in case" */
        if (mbfilename == NULL || mbmode == NULL)
        {
            fprintf (stderr, "Couldn't allocate memory in ifopen()\n");
            exit (1);
        }

        if (wcstombs (mbfilename, filename, needf) == -1 ||
            wcstombs (mbmode, mode, needm) == -1)
        {
            fprintf (stderr, "Couldn't convert wchars in ifopen()\n");
            exit (1);
        }
        mbfilename[needf] = 0; /* "just in case" */
        mbmode[needm] = 0;

        f = fopen (mbfilename, mbmode);
        free (mbfilename);
        free (mbmode);

        return f;
    }
#endif
    return ICHAR_SELECT(fopen, _wfopen)(filename, mode);
}

FILE *ifreopen(const ichar *path, const ichar *mode, FILE *stream)
{
    return ICHAR_SELECT(freopen, _wfreopen)(path, mode, stream);
}

#if !defined(_WINCE) && !defined(_WIN32_WCE)
void iperror(const ichar *string)
{
    ICHAR_SELECT(perror, _wperror)(string);
}
#endif

/* don't use this function, use mkstemp instead
#if !defined(_WINCE) && !defined(_WIN32_WCE)
ichar *itmpnam(ichar *string)
{
    return ICHAR_SELECT(tmpnam, _wtmpnam)(string);
}
#endif
*/

#if !defined(_WINCE) && !defined(_WIN32_WCE)
int iremove(const ichar *path)
{
    return ICHAR_SELECT(remove, _wremove)(path);
}
#endif

#if !defined(_WINCE) && !defined(_WIN32_WCE)
int irename(const ichar *oldname, const ichar *newname)
{
    return ICHAR_SELECT(rename, _wrename)(oldname, newname);
}
#endif

/* Locale handling */

#if !defined(_WINCE) && !defined(_WIN32_WCE)
ichar *isetlocale(int category, const ichar *locale)
{
    return ICHAR_SELECT(setlocale, _wsetlocale)(category, locale);
}
#endif
