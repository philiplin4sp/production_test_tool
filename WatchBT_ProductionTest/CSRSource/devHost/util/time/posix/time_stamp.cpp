// Copyright (C) Cambridge Silicon Radio Ltd 2001-2009
//
// Simple time stamp
// JBS 3 October 2001
// Introduced for marking packets in WinPrimDebug

#include "time/time_stamp.h"
#include "time/hi_res_clock.h"

#include <time.h>
#include <stdio.h>
#include <string.h>

class TimeStamp::Impl : public Counter
{
    HiResClock created;

public:
    Impl()
    {
        InitHiResClock(0);
        HiResClockGetNanoSec(&created);
    }

    std::string hms() const
    {
        char buf[16];
        HiResClockHMSFromPreset(buf, created);
        return std::string(buf);
    }

    std::string hmsu() const
    {
        char buf[19];
	char buf2[19];
	strftime(buf2, sizeof(buf2), "%H:%M:%S", localtime(&created.tv_sec));
	sprintf(buf, "%s.%06li", buf2, created.tv_nsec / 1000);
	return std::string(buf);
    }

    std::string ymdhm() const
    {
        char buf[32];
        strftime(buf, sizeof(buf), "%Y-%b-%d %H:%M",
                    localtime(&created.tv_sec));
        return std::string(buf);
    }

    void advance(unsigned int msec)
    {
        created.tv_sec  += msec / 1000;
        created.tv_nsec += (msec % 1000) * 1000000L;
        /* Normalise */
        created.tv_sec  += created.tv_nsec / 1000000000L;
        created.tv_nsec %= 1000000000L;
    }
};


TimeStamp::TimeStamp()
    : ptr(new Impl)
{ }

TimeStamp::TimeStamp(const TimeStamp &x)
    : ptr(x.ptr)
{ }

TimeStamp &TimeStamp::operator=(const TimeStamp &x)
{ ptr = x.ptr; return *this; }

TimeStamp::~TimeStamp()
{ }

std::string TimeStamp::hms() const
{ return ptr->hms(); }

std::string TimeStamp::hmsu() const
{ return ptr->hmsu(); }

std::string TimeStamp::ymdhm() const
{ return ptr->ymdhm(); }

TimeStamp TimeStamp::add_milliseconds(unsigned int msec) const
{
    Impl *new_impl = new Impl(*ptr);
    new_impl->advance(msec);
    return TimeStamp(new_impl);
}

TimeStamp::TimeStamp(Impl *p)
    : ptr(p)
{ }
