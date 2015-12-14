///////////////////////////////////////////////////////////////////////
//
//	File	: CTimeX.cpp
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: J. Cooper
//
//	Class	: CTimeX
//
//	Purpose	: A drop in replacement for the CTime object.
//            All implemented methods perform the same function as those in CTime.
//
//	$Id: //depot/bc/main/devHost/dfu/DFUEngine/CTimecpp.h#27 $
//
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CTimeX.h"

#if defined _WINCE || defined _WIN32_WCE
#include <windows.h>
#endif

struct CTimeXImpl
{
#if defined _WINCE || defined _WIN32_WCE
	ULARGE_INTEGER t;
	CTimeXImpl() { t.QuadPart=0; }
#else
	time_t t;
	CTimeXImpl() : t(0) {}
#endif
};

struct CTimeSpanXImpl
{
	bool direction;	// true = forwards
#if defined _WINCE || defined _WIN32_WCE
	ULARGE_INTEGER t;
#else
	unsigned long t;
#endif
};

CTimeX::CTimeX() : impl_(new CTimeXImpl)
{
}

CTimeX::CTimeX(time_t t) : impl_(new CTimeXImpl)
{
	*this = t;
}

CTimeX::CTimeX(const CTimeX &t) : impl_(new CTimeXImpl)
{
	*impl_ = *t.impl_;
}

CTimeX::~CTimeX()
{
	delete impl_;
}

CTimeSpanX::CTimeSpanX() : impl_(new CTimeSpanXImpl)
{
}

CTimeSpanX::CTimeSpanX(const CTimeSpanX &t) : impl_(new CTimeSpanXImpl)
{
	*impl_ = *t.impl_;
}

CTimeSpanX::~CTimeSpanX()
{
	delete impl_;
}

CTimeX &CTimeX::operator=(const CTimeX &t)
{
	*impl_ = *t.impl_;
	return *this;
}

CTimeX &CTimeX::operator=(time_t t)
{
#if defined _WINCE || defined _WIN32_WCE
	static const unsigned int FT_TICKS_PER_SEC = 10000000;
	SYSTEMTIME st;
	FILETIME ft;
	memset(&st, 0, sizeof(st));
	st.wYear = 1970;
	st.wMonth = 1;
	st.wDay = 1;
	SystemTimeToFileTime(&st, &ft);
	impl_->t.LowPart = ft.dwLowDateTime;
	impl_->t.HighPart = ft.dwHighDateTime;
	ULARGE_INTEGER ul;
	ul.QuadPart = t;
	ul.QuadPart *= FT_TICKS_PER_SEC;
	impl_->t.QuadPart += ul.QuadPart;
#else
	impl_->t = t;
#endif
	return *this;
}

time_t CTimeX::GetTime() const
{
#if defined _WINCE || defined _WIN32_WCE
	static const unsigned int FT_TICKS_PER_SEC = 10000000;
	SYSTEMTIME st;
	FILETIME ft;
	memset(&st, 0, sizeof(st));
	st.wYear = 1970;
	st.wMonth = 1;
	st.wDay = 1;
	SystemTimeToFileTime(&st, &ft);
	ULARGE_INTEGER ul1;
	ul1.LowPart = ft.dwLowDateTime;
	ul1.HighPart = ft.dwHighDateTime;
	ULARGE_INTEGER ul2;
	ul2 = impl_->t;
	ul2.QuadPart -= ul1.QuadPart;
	ul2.QuadPart /= FT_TICKS_PER_SEC;
	return (time_t) ul2.QuadPart;
#else
	return impl_->t;
#endif
}

#if defined _WINCE || defined _WIN32_WCE
#define NUMERIC(t) ((t).QuadPart)
#else
#define NUMERIC(t) t
#endif

bool CTimeX::operator<(const CTimeX &t) const
{
	return NUMERIC(impl_->t) < NUMERIC(t.impl_->t);
}

bool CTimeX::operator<=(const CTimeX &t) const
{
	return !(t < *this);
}

bool CTimeX::operator==(const CTimeX &t) const
{
	return NUMERIC(impl_->t) == NUMERIC(t.impl_->t);
}

bool CTimeX::operator>(const CTimeX &t) const
{
	return !(*this <= t);
}

bool CTimeX::operator>=(const CTimeX &t) const
{
	return !(*this < t);
}

bool CTimeX::operator!=(const CTimeX &t) const
{
	return !(*this == t);
}

CTimeX &CTimeX::operator+=(const CTimeSpanX &d)
{
	if(d.impl_->direction) NUMERIC(impl_->t) += NUMERIC(d.impl_->t);
	else NUMERIC(impl_->t) -= NUMERIC(d.impl_->t);
	return *this;
}

CTimeX &CTimeX::operator-=(const CTimeSpanX &d)
{
	return *this += -d;
}

CTimeSpanX CTimeX::operator-(const CTimeX &t) const
{
	CTimeSpanX d;
	if(*this < t)
	{
		d.impl_->direction = true;
		NUMERIC(d.impl_->t) = NUMERIC(t.impl_->t) - NUMERIC(impl_->t);
	}
	else
	{
		d.impl_->direction = false;
		NUMERIC(d.impl_->t) = NUMERIC(impl_->t) - NUMERIC(t.impl_->t);
	}
	return d;
}

CTimeX CTimeX::operator+(const CTimeSpanX &d) const
{
	return CTimeX(*this) += d;
}

CTimeX CTimeX::operator-(const CTimeSpanX &d) const
{
	return CTimeX(*this) -= d;
}

CTimeSpanX CTimeSpanX::operator-() const
{
	CTimeSpanX r;
	r.impl_->direction = !r.impl_->direction;
	return r;
}

CTimeSpanX &CTimeSpanX::operator+=(const CTimeSpanX &d)
{
	if(d.impl_->direction == d.impl_->direction)
	{
		NUMERIC(d.impl_->t) += NUMERIC(d.impl_->t);
	}
	else
	{
		if(NUMERIC(d.impl_->t) > NUMERIC(impl_->t))
		{
			impl_->direction = !impl_->direction;
			NUMERIC(impl_->t) = NUMERIC(d.impl_->t) - NUMERIC(impl_->t);
		}
		else
		{
			NUMERIC(impl_->t) -= NUMERIC(d.impl_->t);
		}
	}
	return *this;
}

CTimeSpanX &CTimeSpanX::operator-=(const CTimeSpanX &d)
{
	return *this += -d;
}

CTimeSpanX CTimeSpanX::operator+(const CTimeSpanX &d) const
{
	return CTimeSpanX(*this) += d;
}

CTimeSpanX CTimeSpanX::operator-(const CTimeSpanX &d) const
{
	return CTimeSpanX(*this) -= d;
}

CTimeX CTimeSpanX::operator+(const CTimeX &t) const
{
	return t + *this;
}

CTimeSpanX &CTimeSpanX::operator*=(int n)
{
	NUMERIC(impl_->t) *= n;
	return *this;
}

CTimeSpanX CTimeSpanX::operator*(int n) const
{
	return CTimeSpanX(*this) *= n;
}

CTimeX CTimeX::GetCurrentTime()
{
	CTimeX r;
#if defined _WINCE || defined _WIN32_WCE
	SYSTEMTIME st;
	GetSystemTime(&st);
	FILETIME ft;
	SystemTimeToFileTime(&st, &ft);
	r.impl_->t.LowPart = ft.dwLowDateTime;
	r.impl_->t.HighPart = ft.dwHighDateTime;
#else
	time(&r.impl_->t);
#endif
	return r;
}