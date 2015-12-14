///////////////////////////////////////////////////////////////////////
//
//	File	: CStringX.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: J. Cooper
//
//	Class	: CStringX, CStringListX
//
//	Purpose	: A drop in replacement for the CString and CStringList objects.
//            All implemented methods perform the same function as those in CString and CStringX.
//            The iterator type is CStringListX::POSITION rather than simply ::POSITION.
//
//	$Id: //depot/bc/main/devHost/dfu/DFUEngine/CStringX.h#27 $
//
///////////////////////////////////////////////////////////////////////


// Only include header file once
#ifndef CSTRINGX_H
#define CSTRINGX_H

#ifndef DFUENGINE_DLLSPEC
#ifdef DFUENGINE_EXPORT
#define DFUENGINE_DLLSPEC __declspec(dllexport)
#else
#define DFUENGINE_DLLSPEC __declspec(dllimport)
#endif
#endif

#include <tchar.h>

class DFUENGINE_DLLSPEC CStringX
{
protected:
	TCHAR *sz_;
	unsigned int buflen_;
	unsigned int len_;
public:
	CStringX();
	CStringX(const TCHAR *);
	CStringX(const TCHAR *, unsigned int len);
	CStringX(const CStringX &);
	~CStringX();
	CStringX &operator=(const TCHAR *);
	CStringX &operator=(const CStringX &);
	CStringX &operator+=(const TCHAR *);
	CStringX &operator+=(const CStringX &);
	CStringX &operator*=(unsigned);
	CStringX &operator+=(TCHAR);
	bool operator<(const CStringX &) const;
	bool operator<=(const CStringX &) const;
	bool operator>(const CStringX &) const;
	bool operator>=(const CStringX &) const;
	bool operator==(const CStringX &) const;
	bool operator!=(const CStringX &) const;
	DFUENGINE_DLLSPEC friend CStringX operator+(const CStringX &, const CStringX &);
	DFUENGINE_DLLSPEC friend CStringX operator+(const CStringX &, const TCHAR *);
	DFUENGINE_DLLSPEC friend CStringX operator+(const TCHAR *, const CStringX &);
	DFUENGINE_DLLSPEC friend CStringX operator+(const CStringX &, TCHAR);
	operator const TCHAR*() const;
	TCHAR *GetBuffer(unsigned int len);
	void ReleaseBuffer();
	void ReleaseBuffer(unsigned int);
	bool IsEmpty() const;
	void Format(const TCHAR *fmtstring, ...);
	unsigned int GetLength() const;
	void Empty();
	void Delete(unsigned int pos, unsigned int len = 1);
	int Find(const TCHAR *sz, unsigned int start = 0) const;
	int Find(const TCHAR c, unsigned int start = 0) const;
	int FindOneOf(const TCHAR *charset, unsigned int start = 0) const;
	CStringX Left(unsigned int nCount) const;
	CStringX Right(unsigned int nCount) const;
	CStringX Mid(unsigned int nBegin) const;
	CStringX Mid(unsigned int nBegin, unsigned int nCount) const;
	void MakeUpper();
	void MakeLower();
	void TrimRight();

#ifdef _UNICODE
	CStringX(const char *);
	CStringX(const char *, unsigned int len);
	CStringX &operator=(const char *);
	CStringX &operator+=(const char *);
	CStringX &operator+=(char);
	DFUENGINE_DLLSPEC friend CStringX operator+(const CStringX &, const char *);
	DFUENGINE_DLLSPEC friend CStringX operator+(const char *, const CStringX &);
	DFUENGINE_DLLSPEC friend CStringX operator+(const CStringX &, char);
#endif

private:
	void assign(const TCHAR *, unsigned int);
	void append(const TCHAR *, unsigned int);
	static CStringX combine(const TCHAR *, unsigned int, const TCHAR *, unsigned int);

#ifdef _UNICODE
	void assign(const char *, unsigned int);
	void append(const char *, unsigned int);
	static CStringX combine(const TCHAR *, unsigned int, const char *, unsigned int);
	static CStringX combine(const char *, unsigned int, const TCHAR *, unsigned int);
#endif
};

class DFUENGINE_DLLSPEC CStringListX {
protected:
	unsigned int count;
	struct Node
	{
		CStringX value;
		Node *next;
		Node *prev;
	} *head, *tail;
public:
	CStringListX();
	~CStringListX();
	class DFUENGINE_DLLSPEC POSITION
	{
		friend class CStringListX;
	public:
		operator CStringX*();
	protected:
		Node *node_;
		POSITION(Node *);
	};
	friend class POSITION;
	CStringX &GetHead() const;
	CStringX &GetTail() const;
	bool IsEmpty() const;
	unsigned int GetCount() const;
	CStringX RemoveHead();
	CStringX RemoveTail();
	POSITION AddHead(const TCHAR *);
	POSITION AddTail(const TCHAR *);
	void AddHead(const CStringListX *);
	void AddTail(const CStringListX *);
	void RemoveAll();
	POSITION GetHeadPosition() const;
	POSITION GetTailPosition() const;
	CStringX &GetNext(POSITION&) const;
	POSITION Find(const TCHAR *) const;
	void RemoveAt(POSITION);
	POSITION InsertBefore(POSITION, const TCHAR *);
	POSITION InsertAfter(POSITION, const TCHAR *);
};

#ifdef _MFC_VER
inline void Convert(const CStringList &from, CStringListX &to)
{
	::POSITION p = from.GetHeadPosition();
	while(p) to.AddTail(from.GetNext(p));
}

inline void Convert(const CStringListX &from, CStringList &to)
{
	to.RemoveAll();
	CStringListX::POSITION p = from.GetHeadPosition();
	while(p) to.AddTail(from.GetNext(p));
}
#endif

#endif
