////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) Cambridge Silicon Radio Ltd 2001-2009
//
//  FILE:    stop_watch.cpp
//
//  AUTHOR:  ajh
//
//  DATE:    3 October 2001
//
//  PURPOSE: Simple duration measuring object.
//
//  CLASS:   StopWatch
//
////////////////////////////////////////////////////////////////////////////////

extern"C"{
#include "hi_res_clock.h"
}
#include "stop_watch.h"

static bool initialised = false;

class StopWatch::Impl : public Counter
{
public:
  Impl() 
  {
    if ( !initialised )
    {
      InitHiResClock(0);
      initialised = true;
    }
    HiResClockGetNanoSec ( &start );
  }
  unsigned long millisec()
  {
    HiResClock now;
    HiResClockGetNanoSec ( &now );
    return (unsigned long)(( (now.tv_sec - start.tv_sec) * 1000 ) // 1000 ms = 1s
         + ( (now.tv_nsec - start.tv_nsec) / 1000000 )); // 1ms = 1000000ns
  }
  unsigned long microsec()
  {
    HiResClock now;
    HiResClockGetNanoSec ( &now );
    return (unsigned long)( ((now.tv_sec - start.tv_sec) * 1000000 ) // 1000000 us = 1s
         + ( (now.tv_nsec - start.tv_nsec) / 1000 ) ); // 1us = 1000ns
  }
private:
  HiResClock start;
};

StopWatch::StopWatch() // Records the time it was built
: ptr ( new Impl )
{ }

StopWatch::StopWatch(const StopWatch &x)
: ptr ( x.ptr )
{ }

StopWatch &StopWatch::operator=(const StopWatch &x)
{ ptr = x.ptr; return *this; }

StopWatch::~StopWatch()
{ }

unsigned long StopWatch::duration() // milliseconds since creation.
{ return ptr->millisec(); }

unsigned long StopWatch::uduration() // microseconds since creation.
{ return ptr->microsec(); }

