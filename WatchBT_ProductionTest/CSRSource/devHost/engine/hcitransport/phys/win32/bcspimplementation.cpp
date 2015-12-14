///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) Cambridge Silicon Radio April 2000-2006
//
//  FILE    :   bcspiplementation.cpp  -  win32 wrapper for BCSP stack
//
///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <cassert>

#include "../../bcsp/bcspstack.h"
#include "../bcspimplementation.h"

void BCSPImplementation::sleep ( uint32 duration )
{
    Sleep ( duration );
}

uint32 BCSPImplementation::ms_clock ()
{
    return GetTickCount();
}
