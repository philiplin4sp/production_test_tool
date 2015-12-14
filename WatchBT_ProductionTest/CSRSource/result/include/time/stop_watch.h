////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) Cambridge Silicon Radio Ltd 2001-2009
//
//  FILE:    stop_watch.h
//
//  AUTHOR:  ajh
//
//  DATE:    3 October 2001
//
//  PURPOSE: Simple duration measuring object.
//
//  USES:    hi_res_timer.h
//
//  CLASS:   StopWatch
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __STOP_WATCH_H__
#define __STOP_WATCH_H__

#include "common/countedpointer.h"
#include <string>

class StopWatch
{
public:
    StopWatch(); // Records the time it was built
    StopWatch(const StopWatch &);
    StopWatch &operator=(const StopWatch &);
    ~StopWatch();

    unsigned long duration(); // milliseconds since creation.
    unsigned long uduration();// microseconds since creation.

private:
    class Impl;
    CountedPointer<Impl> ptr;
};

#endif

