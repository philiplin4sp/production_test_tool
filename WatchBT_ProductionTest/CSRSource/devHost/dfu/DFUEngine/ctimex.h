///////////////////////////////////////////////////////////////////////
//
//	File	: CTimeX.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: J. Cooper
//
//	Class	: CTimeX
//
//	Purpose	: A drop in replacement for the CTime object.
//            All implemented methods perform the same function as those in CTime.
//
//	$Id: //depot/bc/main/devHost/dfu/DFUEngine/CTimeX.h#27 $
//
///////////////////////////////////////////////////////////////////////

#ifndef CTIMEX_H
#define CTIMEX_H

#include <time.h>

class CTimeX
{
	friend class CTimeSpanX;
public:
	CTimeX();
	CTimeX(const CTimeX &);
	CTimeX(time_t);
	~CTimeX();

	CTimeX &operator=(const CTimeX &);
	CTimeX &operator=(time_t);

	time_t GetTime() const;
	static CTimeX GetCurrentTime();

	bool operator<(const CTimeX &) const;
	bool operator<=(const CTimeX &) const;
	bool operator>(const CTimeX &) const;
	bool operator>=(const CTimeX &) const;
	bool operator==(const CTimeX &) const;
	bool operator!=(const CTimeX &) const;

	CTimeX &operator+=(const CTimeSpanX &);
	CTimeX &operator-=(const CTimeSpanX &);
	CTimeSpanX operator-(const CTimeX &) const;
	CTimeX operator+(const CTimeSpanX &) const;
	CTimeX operator-(const CTimeSpanX &) const;

private:
	struct CTimeXImpl *impl_;
};

class CTimeSpanX
{
	friend class CTimeX;
public:
	CTimeSpanX();
	CTimeSpanX(const CTimeSpanX &);
	~CTimeSpanX();

	CTimeX &operator=(const CTimeSpanX &);

	CTimeSpanX operator-() const;
	CTimeSpanX &operator+=(const CTimeSpanX &);
	CTimeSpanX &operator-=(const CTimeSpanX &);
	CTimeSpanX operator+(const CTimeSpanX &) const;
	CTimeSpanX operator-(const CTimeSpanX &) const;
	CTimeX operator+(const CTimeX &) const;
	CTimeSpanX &operator*=(int);
	CTimeSpanX operator*(int) const;

private:
	struct CTimeSpanXImpl *impl_;
};

#endif
