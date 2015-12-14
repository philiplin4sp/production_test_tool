///////////////////////////////////////////////////////////////////////
//
//	File	: CStringX.cpp
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
//	$Id: //depot/bc/main/devHost/dfu/DFUEngine/CStringX.cpp#27 $
//
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CStringX.h"
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>

// Visual C++ debugging code
#ifdef _DEBUG
#include <crtdbg.h>
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define new DEBUG_NEW
#endif

static TCHAR empty[] = _T("");

CStringX::CStringX() : len_(0), sz_(empty) {}

CStringX::CStringX(const TCHAR *sz) : len_(0)
{
	assign(sz, _tcslen(sz));
}

CStringX::CStringX(const TCHAR *str, unsigned int len) : len_(0)
{
	assign(str, len);
}

CStringX::CStringX(const CStringX &b) : len_(0)
{
	assign(b.sz_, b.len_);
}

CStringX::~CStringX()
{
	Empty();
}

CStringX &CStringX::operator=(const TCHAR *sz)
{
	assign(sz, _tcslen(sz));
	return *this;
}

CStringX &CStringX::operator=(const CStringX &b)
{
	assign(b.sz_, b.len_);
	return *this;
}

CStringX &CStringX::operator+=(const TCHAR *sz)
{
	append(sz, _tcslen(sz));
	return *this;
}

CStringX &CStringX::operator+=(const CStringX &b)
{
	append(b.sz_, b.len_);
	return *this;
}

CStringX &CStringX::operator*=(unsigned N)
{
	TCHAR *sz = sz_;
	unsigned int len = len_;
	len_ = 0; sz_ = empty;
	while(N-- > 0) append(sz, len);
	if(len) free(sz);
	return *this;
}

CStringX &CStringX::operator+=(TCHAR c)
{
	append(&c, 1);
	return *this;
}

bool CStringX::operator<(const CStringX &b) const
{
	return _tcscmp(*this, b)<0;
}

bool CStringX::operator<=(const CStringX &b) const
{
	return _tcscmp(*this, b)<=0;
}

bool CStringX::operator>(const CStringX &b) const
{
	return !(b <= *this);
}

bool CStringX::operator>=(const CStringX &b) const
{
	return !(b < *this);
}

bool CStringX::operator==(const CStringX &b) const
{
	return _tcscmp(*this, b)==0;
}

bool CStringX::operator!=(const CStringX &b) const
{
	return !(*this == b);
}

CStringX operator+(const CStringX &a, const CStringX &b)
{
	return CStringX::combine(a.sz_, a.len_, b.sz_, b.len_);
}

CStringX operator+(const CStringX &a, const TCHAR *sz)
{
	return CStringX::combine(a.sz_, a.len_, sz, _tcslen(sz));
}

CStringX operator+(const TCHAR *sz, const CStringX &b)
{
	return CStringX::combine(sz, _tcslen(sz), b.sz_, b.len_);
}

CStringX operator+(const CStringX &a, TCHAR b)
{
	return CStringX::combine(a.sz_, a.len_, &b, 1);
}

#ifdef _UNICODE
CStringX::CStringX(const char *sz) : len_(0), sz_(empty)
{
	assign(sz, strlen(sz));
}

CStringX::CStringX(const char *str, unsigned int len) : len_(0), sz_(empty)
{
	assign(str, len);
}

CStringX &CStringX::operator=(const char *sz)
{
	assign(sz, strlen(sz));
	return *this;
}

CStringX &CStringX::operator+=(const char *sz)
{
	append(sz, strlen(sz));
	return *this;
}

CStringX &CStringX::operator+=(char c)
{
	append(&c, 1);
	return *this;
}

CStringX operator+(const CStringX &a, const char *sz)
{
	return CStringX::combine(a.sz_, a.len_, sz, strlen(sz));
}

CStringX operator+(const char *sz, const CStringX &b)
{
	return CStringX::combine(sz, strlen(sz), b.sz_, b.len_);
}

CStringX operator+(const CStringX &a, char b)
{
	return CStringX::combine(a.sz_, a.len_, &b, 1);
}
#endif

void CStringX::Empty()
{
	if(len_)
	{
		free(sz_);
		len_ = 0;
		sz_ = empty;
	}
}

CStringX::operator const TCHAR *() const
{
	return sz_;
}

TCHAR *CStringX::GetBuffer(unsigned int len)
{
	if(!len_)
	{
		sz_ = (TCHAR *) malloc((buflen_ = 1 + (len_ = len)) * sizeof(*sz_));
	}
	else if(buflen_ < len)
	{
		sz_ = (TCHAR *) realloc(sz_, (buflen_ = 1 + (len_ = len)) * sizeof(*sz_));
	}
	assert(sz_);

	return sz_;
}

void CStringX::ReleaseBuffer(unsigned int len)
{
	if((len_ = len) == 0)
	{
		sz_ = empty;
	}
	else
	{
		if(buflen_ < len_+1)
		{
			sz_ = (TCHAR *) realloc(sz_, (buflen_ = 1 + len_) * sizeof(*sz_));
			assert(sz_);
		}
		sz_[len_] = _T('\0');
	}
}

void CStringX::ReleaseBuffer()
{
	ReleaseBuffer(_tcslen(sz_));
}

bool CStringX::IsEmpty() const
{
	return len_ == 0;
}

void CStringX::Format(const TCHAR *fmtstring, ...)
{
	bool done = false;
	TCHAR *oldsz_ = len_ ? sz_ : NULL;
	sz_ = NULL;
	buflen_ = 256;
	while (!done)
	{
		sz_ = (TCHAR *) realloc(sz_, buflen_ * sizeof(*sz_));
		assert(sz_);
		va_list v;
		va_start(v, fmtstring);
		int result = _vsntprintf(sz_, buflen_, fmtstring, v);
		va_end(v);
		if(result < 0)
		{
			buflen_ *= 2;
		}
		else if(result == 0)
		{
			free(sz_);
			len_ = 0;
			sz_ = empty;
			done = true;
		}
		else
		{
			if ((unsigned int) result == buflen_)	// need to add a null terminator.
			{
				sz_ = (TCHAR *) realloc(sz_, ++buflen_);
				sz_[buflen_] = _T('\0');
			}
			len_ = result;
			done = true;
		}
	}
	free(oldsz_);
}

unsigned int CStringX::GetLength() const { return len_; }

void CStringX::Delete(unsigned int pos, unsigned int len)
{
	if(pos < len_ && len != 0)
	{
		if(pos == 0 && len >= len_)
		{
			free(sz_);
			len_ = 0;
			sz_ = empty;
		}
		else
		{
			if(pos + len < len_)
			{
				for(unsigned int i = 0; i < len; ++i)
				{
					sz_[pos + i] = sz_[pos + len + i];
				}
				len_ -= len;
			}
			else
			{
				len_ = pos;
			}
			sz_[len_] = _T('\0');
		}
	}
}

int CStringX::Find(const TCHAR *sz, unsigned int start) const
{
	const TCHAR *result = _tcsstr(&(*this)[start], sz);
	return result ? result - sz_ : -1;
}

int CStringX::Find(const TCHAR c, unsigned int start) const
{
	TCHAR str[2] = { c, _T('\0') };
	return Find(str, start);
}

int CStringX::FindOneOf(const TCHAR *charset, unsigned int start) const
{
	const TCHAR *result = _tcspbrk(&(*this)[start], charset);
	return result ? result - sz_ : -1;
}

CStringX CStringX::Left(unsigned int nCount) const
{
	return nCount >= len_ ? *this : CStringX(sz_, nCount);
}

CStringX CStringX::Right(unsigned int nCount) const 
{
	return nCount >= len_ ? *this : CStringX(&sz_[len_ - nCount], nCount);
}

CStringX CStringX::Mid(unsigned int nBegin) const
{
	if (nBegin >= len_) return _T("");
	else return CStringX(&sz_[nBegin]);
}

CStringX CStringX::Mid(unsigned int nBegin, unsigned int nCount) const
{
	if (nBegin >= len_) return _T("");
	else if (nBegin + nCount >= len_) return CStringX(&sz_[nBegin], len_ - nBegin);
	else return CStringX(&sz_[nBegin], nCount);
}

void CStringX::MakeUpper()
{
	if(len_) _tcsupr(sz_);
}

void CStringX::MakeLower()
{
	if(len_) _tcslwr(sz_);
}

void CStringX::TrimRight()
{
	if(len_ > 0)
	{
		while(len_ > 0 && _istspace(sz_[len_-1])) --len_;
		if(len_ == 0)
		{
			free(sz_);
			sz_ = empty;
		}
	}
}

void CStringX::assign(const TCHAR *str, unsigned int len)
{
	TCHAR *oldsz_ = len_ ? sz_ : NULL;
	len_ = len;
	if(len_ > 0)
	{
		sz_ = (TCHAR *) malloc((buflen_ = 1 + len_) * sizeof(*sz_));
		assert(sz_);
		_tcsncpy(sz_, str, len);
		sz_[len_] = _T('\0');
	}
	else
	{
		sz_ = empty;
	}
	free(oldsz_);
}

void CStringX::assign(const char *str, unsigned int len)
{
	TCHAR *oldsz_ = len_ ? sz_ : NULL;
	len_ = len;
	if(len_ > 0)
	{
		sz_ = (TCHAR *) malloc((buflen_ = 1 + len_) * sizeof(*sz_));
		assert(sz_);
		for(unsigned int i=0; i<len; ++i) _stprintf(&sz_[i], _T("%hc"), str[i]);
	}
	else
	{
		sz_ = empty;
	}
	free(oldsz_);
}

void CStringX::append(const TCHAR *str, unsigned int len)
{
	if(len > 0)
	{
		sz_ = (TCHAR *) realloc(len_ ? sz_ : NULL, (buflen_ = 1 + (len_ + len)) * sizeof(*sz_));
		_tcsncpy(&sz_[len_], str, len);
		len_ += len;
		sz_[len_] = _T('\0');
	}
}

void CStringX::append(const char *str, unsigned int len)
{
	CStringX other(str, len);
	*this += other;
}

CStringX CStringX::combine(const TCHAR *a, unsigned int a_len, const TCHAR *b, unsigned int b_len)
{
	CStringX r;
	if(a_len > 0 || b_len > 0)
	{
		r.sz_ = (TCHAR *) malloc((r.buflen_ = 1 + (r.len_ = a_len + b_len)) * sizeof(*r.sz_));
		_tcsncpy(r.sz_, a, a_len);
		_tcsncpy(&r.sz_[a_len], b, b_len);
		r.sz_[r.len_] = _T('\0');
	}
	return r;
}

CStringX CStringX::combine(const TCHAR *a, unsigned int a_len, const char *b, unsigned int b_len)
{
	CStringX r2(b, b_len);
	return combine(a, a_len, r2.sz_, r2.len_);
}

CStringX CStringX::combine(const char *a, unsigned int a_len, const TCHAR *b, unsigned int b_len)
{
	CStringX r(a, a_len);
	r.append(b, b_len);
	return r;
}

CStringListX::CStringListX()
{
	count = 0;
	head = new Node;
#if !defined _WINCE && !defined _WIN32_WCE
	try
#endif
	{
		tail = new Node;
	}
#if !defined _WINCE && !defined _WIN32_WCE
	catch(...)
	{
		delete head;
		throw;
	}
#endif

	head->next = tail;
	tail->prev = head;
	head->prev = tail->next = NULL;
}

CStringListX::~CStringListX()
{
	RemoveAll();
	delete head;
	delete tail;
}

CStringX &CStringListX::GetHead() const
{
	return head->next->value;
}

CStringX &CStringListX::GetTail() const
{
	return tail->prev->value;
}

bool CStringListX::IsEmpty() const
{
	return head->next == NULL;
}

unsigned int CStringListX::GetCount() const
{
	return count;
}

CStringX CStringListX::RemoveHead()
{
	CStringX r = GetHead();
	RemoveAt(GetHeadPosition());
	return r;
}

CStringX CStringListX::RemoveTail()
{
	CStringX r = GetTail();
	RemoveAt(GetTailPosition());
	return r;
}

CStringListX::POSITION CStringListX::AddHead(const TCHAR *value)
{
	return InsertBefore(GetHeadPosition(), value);
}

CStringListX::POSITION CStringListX::AddTail(const TCHAR *value)
{
	return InsertAfter(GetTailPosition(), value);
}

void CStringListX::AddHead(const CStringListX *list)
{
	POSITION writepos = GetHeadPosition(), readpos = list->GetHeadPosition();
	while(readpos)
	{
		InsertBefore(writepos, list->GetNext(readpos));
	}
}

void CStringListX::AddTail(const CStringListX *list)
{
	POSITION readpos = list->GetHeadPosition();
	while(readpos)
	{
		AddTail(list->GetNext(readpos));
	}
}

void CStringListX::RemoveAll()
{
	while(head->next != tail)
	{
		Node *node = head->next;
		head->next = node->next;
		delete node;
	}
	tail->prev = head;
	count = 0;
}

CStringListX::POSITION CStringListX::GetHeadPosition() const
{
	return POSITION(head->next);
}

CStringListX::POSITION CStringListX::GetTailPosition() const
{
	return POSITION(tail->prev);
}

CStringX &CStringListX::GetNext(POSITION &pos) const
{
	CStringX &r = *pos;
	pos.node_ = pos.node_->next;
	return r;
}

CStringListX::POSITION CStringListX::Find(const TCHAR *value) const
{
	POSITION pos = GetHeadPosition();
	while(pos && _tcscmp(*pos, value)!=0) GetNext(pos);
	return pos;
}

CStringListX::POSITION::operator CStringX *()
{
	return node_->next && node_->prev ? &node_->value : NULL;
}

void CStringListX::RemoveAt(POSITION pos)
{
	pos.node_->prev->next = pos.node_->next;
	pos.node_->next->prev = pos.node_->prev;
	delete pos.node_;
	--count;
}

CStringListX::POSITION CStringListX::InsertBefore(POSITION pos, const TCHAR *value)
{
	Node *node = new Node;
	node->prev = pos.node_->prev;
	node->next = pos.node_;
	pos.node_->prev = node;
	node->prev->next = node;
	node->value = value;
	++count;
	return POSITION(node);
}

CStringListX::POSITION CStringListX::InsertAfter(POSITION pos, const TCHAR *value)
{
	Node *node = new Node;
	node->prev = pos.node_;
	node->next = pos.node_->next;
	pos.node_->next = node;
	node->next->prev = node;
	node->value = value;
	++count;
	return POSITION(node);
}

CStringListX::POSITION::POSITION(Node *node) : node_(node) {}
