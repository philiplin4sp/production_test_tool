///////////////////////////////////////////////////////////////////////
//
//	File	: CFileX.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: J. Cooper
//
//	Class	: CFileX, CMemFileX
//
//	Purpose	: A drop in replacement for CFile and CMemFile.
//            All of the implemented methods perform the same function as their
//            counterparts in CFile.
//
//	$Id: //depot/bc/main/devHost/dfu/DFUEngine/CFileX.h#27 $
//
///////////////////////////////////////////////////////////////////////

#ifndef CFILEX_H
#define CFILEX_H

#include "CStringX.h"
#include "CTimeX.h"
#include "../SmartPtr.h"

class CFileX
{
public:
	enum SEEK {
		begin,
		end,
	};

	struct FileStatus
	{
		CTimeX m_mtime;
		unsigned long m_size;
	};

	static const unsigned modeCreate;
	static const unsigned modeNoTruncate;
	static const unsigned modeRead;
	static const unsigned modeWrite;
	static const unsigned shareExclusive;
	static const unsigned shareDenyRead;
	static const unsigned shareDenyWrite;

	CFileX();
	~CFileX();

	virtual CStringX GetFilePath() const;
	virtual void SetFilePath(const TCHAR *);
	virtual bool Open(const TCHAR *, unsigned);
	virtual void Close();
	virtual void SetLength(unsigned long);
	virtual unsigned long GetLength() const;
	virtual void Write(const void *p, unsigned int len);
	virtual unsigned int Read(void *p, unsigned int len);
	void SeekToBegin() { Seek(0, begin); }
	virtual void Seek(long pos, SEEK rel);
	virtual unsigned long GetPosition() const;
	static bool GetStatus(const TCHAR *, FileStatus &);

private:
	friend class CFileXCommonImpl;
	SmartPtr<CFileXCommonImpl> common_impl_;

	friend class CFileXSpecificImpl;
	SmartPtr<CFileXSpecificImpl> specific_impl_;
};

class CMemFileX : public CFileX
{
public:
	CMemFileX();
	~CMemFileX();

	virtual bool Open(const TCHAR *, unsigned);
	virtual void Close();

	virtual void SetLength(unsigned long);
	virtual unsigned long GetLength() const;
	virtual void Write(const void *p, unsigned int len);
	virtual unsigned int Read(void *p, unsigned int len);
	virtual void Seek(long pos, SEEK rel);
	virtual unsigned long GetPosition() const;

private:
	friend class CMemFileXImpl;
	SmartPtr<CMemFileXImpl> memfile_impl_;
};

#endif