///////////////////////////////////////////////////////////////////////
//
//  FILE   :  safe_counted.h
//
//            Copyright (C) Cambridge Silicon Radio Ltd 2004-2009
//
//  AUTHOR :  Adam Hughes
//
//  PURPOSE:  To declare a thread safe "counted" base class for
//            pointers from common/counter_pointer.h
//
//  WARNING:  This class hass a ridiculously high overhead.
//
//  $Id: //depot/bc/babel-1/devHost/util/thread/signal_box.h#1 $
//
///////////////////////////////////////////////////////////////////////

#include "thread/critical_section.h"
#include "common/countedpointer.h"

#ifndef SAFE_COUNTER_H
#define SAFE_COUNTER_H

class SafeCounter
{
  mutable int count;
  mutable CriticalSection p;

public:
  SafeCounter()
  { count = 0; }
  SafeCounter(const SafeCounter &)
  { }
  virtual ~SafeCounter()
  { }
  SafeCounter &operator=(const SafeCounter &)
  { return *this; }

  void inc() const
  { CriticalSection::Lock here(p); ++count; }
  void dec() const
  {
    {
      CriticalSection::Lock here(p);
      if(--count)
          return;
    }
    delete this;
  }
};

#endif
