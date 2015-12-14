// Copyright (C) 2001-2006 CSR Ltd.  All rights reserved.
// Support for standard C functions which Windows CE fails to support.
// $Id: //depot/bc/bluesuite_2_4/devHost/util/wince/wincefuncs.c#1 $


#if (!defined(_WINCE) && !defined(_WIN32_WCE))
#error This file is only needed for Windows CE
#endif

#include "unicode/ichar.h"
#include "unicode/main.h"
#include <string.h>
#include <stdio.h>
#include <winbase.h>
#include <windef.h>
#include <dbgapi.h>
#include <ctype.h>
#include "types.h"
#include "wincefuncs.h"


/* errno is not supported under Windows CE */
/* There is no DOS shell to run batch files or have environment variables etc */

int errno = 0;

int system (const char *cmd)
{
	PROCESS_INFORMATION pi;
	wchar_t wcmd[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, cmd, -1, wcmd, 199);
	if (CreateProcessW(wcmd, wcmd, NULL, NULL, 0, 0, NULL, NULL, NULL, &pi))
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		return 1;
	}
	return 0;
}

char *strerror (int dummy)
{
	static wchar_t buf[200];
	static char cbuf[200];
	FormatMessageW(0, NULL, GetLastError(), 0, buf, 200, NULL);
	WideCharToMultiByte(CP_ACP, 0, buf, -1, cbuf, 199, NULL, NULL);
	return cbuf;
}

const char *getenv (const char *varname)
{
	return 0;
}


#ifndef isspace

int isspace(int c)
{
	return ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r'));
}

int isalpha(int c)
{
	return (  ((c >= 'A') && (c <= 'Z'))
		   || ((c >= 'a') && (c <= 'z')) );
}

int isalnum(int c)
{
	return (  ((c >= 'A') && (c <= 'Z'))
		   || ((c >= '0') && (c <= '9'))
		   || ((c >= 'a') && (c <= 'z')) );
}

int isdigit(int c)
{
	return ((c >= '0') && (c <= '9'));
}

int isxdigit(int c)
{
	return (  ((c >= 'A') && (c <= 'F'))
		   || ((c >= 'a') && (c <= 'f'))
		   || ((c >= '0') && (c <= '9')) );
}

int isprint(int c)
{
	return (c >= ' ') && (c <= 255);
}

int isascii(int c)
{
	return (c >= 0x20 && c <0xff && c != 0x7f) ;
}

int ispunct(int c)
{
	return (c >= 0x20 && c <0x30) || (c >= 0x38 && c <0x41) || (c >= 0x5b && c <0x61) || (c >= 0x7b && c <0x7f) ;
}

size_t strspn( const char *string, const char *strCharSet )
{
	const char *d = string;
	const char *p = strCharSet;
	while (*p && *d)
	{
		if (*d == *p)
		{
			d++;
			p = strCharSet;
		}
		else
			p++;
	}
	return d - string;
}

#endif

time_t time(time_t *timer) 
{
	
  SYSTEMTIME t;
  FILETIME f;
  __int64 i;
  GetLocalTime(&t);
  SystemTimeToFileTime(&t, &f);
  i = *((__int64*)(&f));
  i /= 10000000L; // devide by 1 / 100us, giving value in seconds.
  if (timer)
	  *timer = (time_t)(i);
  return (time_t)(i);  
}


uint32 mtime(void) 
{
	
  SYSTEMTIME t;
  FILETIME f;
  __int64 i;
  GetLocalTime(&t);
  SystemTimeToFileTime(&t, &f);
  i = *((__int64*)(&f));
  i /= 10000L; // devide by 1000 / 100us, giving value in miliseconds.
  return (uint32)(i);  
}





#ifdef _WINCE_CONSOLE

// now uses ichar...
// extern int _cdecl main(int argc, char *argv[]);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd )
{
	ichar *argv[100];
	int argc = 1;
	int j;
	int iscomport = FALSE;
	wchar_t *p = lpCmdLine;
	wchar_t *wordstart = lpCmdLine;
	argv[0] = II("btcli.exe");
	while (*p)
	{
		while (*p && *p != ' ')
		{
			p++;
		}
		if (p != wordstart)
		{  // Append a colon to the com port name if it doesn't have it
			if (   (itoupper((ichar)(wordstart[0])) == 'C') 
				&& (itoupper((ichar)(wordstart[1])) == 'O') 
				&& (itoupper((ichar)(wordstart[2])) == 'M')
				&& (iisdigit((ichar)(wordstart[3])))
			   )
			{
				argv[argc] = malloc((p - wordstart + 2) * sizeof(ichar));
				iscomport = TRUE;
			}
			else
			{
				argv[argc] = malloc((p - wordstart + 1) * sizeof(ichar));
				iscomport = FALSE;
			}
			j = 0;
			while (wordstart != p)
			{
				argv[argc][j] = (ichar)(*wordstart);
				wordstart++;
				j++;
			}
			if (iscomport && argv[argc][j - 1] != II(':'))
				argv[argc][j++] = II(':');
			argv[argc][j] = II('\0');
			argc++;
			wordstart++;
		}
		if (*p)
			p++;
	}
	return imain(argc, argv);
}

#endif // _WINCE_CONSOLE