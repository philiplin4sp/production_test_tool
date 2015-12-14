///////////////////////////////////////////////////////////////////////
//
//  FILE   :  Win32\critical_section.cpp
//
//            Copyright (C) Cambridge Silicon Radio Ltd 2000-2009
//
//  AUTHOR :  Adam Hughes
//
//  PURPOSE:  Hold machine dependent definitions of Critical
//            sections as standard functions which are used in
//            the rest of the code.
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/util/thread/win32/critical_section.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

#include "thread/critical_section.h"

#include "../error_msg.h"

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#include <windows.h>

void *CriticalSection::InitialiseCS(bool recursive)
{
    (void) recursive; // all win32 critical sections are recursive.
    LPCRITICAL_SECTION section = new CRITICAL_SECTION;
    ::InitializeCriticalSection(section);
    OUTPUT_HANDLE_CREATE(section);
    return section;
}

void CriticalSection::EnterCS(void *section)
{
    ::EnterCriticalSection ((LPCRITICAL_SECTION)section) ;
}

void CriticalSection::LeaveCS(void *section)
{
    ::LeaveCriticalSection((LPCRITICAL_SECTION)section);
}

void CriticalSection::DestroyCS(void *section)
{
    ::DeleteCriticalSection((LPCRITICAL_SECTION)section);
    OUTPUT_HANDLE_CLOSE(section);
    delete (LPCRITICAL_SECTION)section;
}
