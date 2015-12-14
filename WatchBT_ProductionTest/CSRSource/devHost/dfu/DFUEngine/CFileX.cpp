///////////////////////////////////////////////////////////////////////
//
//	File	: CFileX.cpp
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
//	$Id: //depot/bc/main/devHost/dfu/DFUEngine/CFileX.cpp#27 $
//
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CFileX.h"
#include <stdio.h>
#if defined _WINCE || defined _WIN32_WCE
#include <windows.h>
#else
#include <io.h>
#endif
#include <memory.h>
#include <malloc.h>
#include <assert.h>

// Visual C++ debugging code
#ifdef _DEBUG
#include <crtdbg.h>
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define new DEBUG_NEW
#endif

const unsigned CFileX::modeRead = 1;
const unsigned CFileX::modeWrite = 2;
const unsigned CFileX::modeCreate = 4;
const unsigned CFileX::shareDenyRead = 8;
const unsigned CFileX::shareDenyWrite = 16;
const unsigned CFileX::shareExclusive = shareDenyWrite | shareDenyRead;
const unsigned CFileX::modeNoTruncate = 32;

class CFileXCommonImpl
{
public:
	CStringX file_path;
};

class CFileXSpecificImpl
{
public:
	CFileXSpecificImpl();
	~CFileXSpecificImpl();
	FILE *file;
};

CFileX::CFileX() : common_impl_(new CFileXCommonImpl)
{
}

CFileX::~CFileX() {}

CFileXSpecificImpl::CFileXSpecificImpl() : file(NULL) {}

CFileXSpecificImpl::~CFileXSpecificImpl()
{
	if(file) fclose(file);
}

CStringX CFileX::GetFilePath() const
{
	return common_impl_->file_path;
}

void CFileX::SetFilePath(const TCHAR *new_path)
{
	common_impl_->file_path = new_path;
}

bool CFileX::Open(const TCHAR *path, unsigned mode)
{
	if(!specific_impl_) specific_impl_ = SmartPtr<CFileXSpecificImpl>(new CFileXSpecificImpl);

	SetFilePath(path);

	TCHAR flags[4];
	int flagslen = 0;

	flags[flagslen++] = _T('r');
	if(mode & modeWrite)
	{
		flags[flagslen++] = _T('+');
	}
	else if(!(mode & modeRead))
	{
		// file opened for no access. can't really do that.
		return false;
	}
	flags[flagslen++] = _T('b');
	flags[flagslen++] = _T('\0');

	if(mode & modeCreate)
	{
		// create the file and then close it for reopen.
		FILE *t = _tfopen(path, (mode & modeNoTruncate) ? _T("ab") : _T("wb"));
		if(t) fclose(t);
	}

	specific_impl_->file = _tfopen(path, flags);
	if(!specific_impl_->file) return false;
	return true;
}

void CFileX::Close()
{
	if(specific_impl_->file) fclose(specific_impl_->file);
	specific_impl_->file = NULL;
}

void CFileX::SetLength(unsigned long new_len)
{
#if defined _WINCE || defined _WIN32_WCE
	// apparently this should work on CE even though it doesn't work on normal windows.
	long pos = ftell(specific_impl_->file);
	if(fseek(specific_impl_->file, 0, SEEK_END) == 0)
	{
		long len = ftell(specific_impl_->file);
		unsigned long xlen = len;
		while(xlen++ < new_len) fputc('\0', specific_impl_->file);
		fseek(specific_impl_->file, new_len, SEEK_SET);
		fflush(specific_impl_->file);
		HANDLE hF = _fileno(specific_impl_->file);
		SetFilePointer(hF, new_len, NULL, FILE_BEGIN);
		SetEndOfFile(hF);
	}
	fseek(specific_impl_->file, pos, SEEK_SET);
#else
	fflush(specific_impl_->file);
	if(_chsize(_fileno(specific_impl_->file), new_len)!=0)
		throw CExceptionX(_T("Chsize Error"));
#endif
}

unsigned long CFileX::GetLength() const
{
	long len = -1;
	long pos = ftell(specific_impl_->file);
#if !defined _WINCE && !defined _WIN32_WCE
	if(pos == -1) throw CExceptionX(_T("Tell Error"));
#endif

#if !defined _WINCE && !defined _WIN32_WCE
	try
#endif
	{
		if(fseek(specific_impl_->file, 0, SEEK_END))
#if !defined _WINCE && !defined _WIN32_WCE
			throw CExceptionX(_T("Seek Error"));
#else
			{}
#endif
		len = ftell(specific_impl_->file);
#if !defined _WINCE && !defined _WIN32_WCE
		if(len == -1) throw CExceptionX(_T("Tell Error"));
#endif
		if(fseek(specific_impl_->file, pos, SEEK_SET))
#if !defined _WINCE && !defined _WIN32_WCE
			throw CExceptionX(_T("Seek Error"));
#else
			{}
#endif
	}
#if !defined _WINCE && !defined _WIN32_WCE
	catch(...)
	{
		if(fseek(specific_impl_->file, pos, SEEK_SET)) fclose(specific_impl_->file);
		throw;
	}
#endif
	return len;
}

void CFileX::Write(const void *p, unsigned int len)
{
	while(len > 0)
	{
		int result = fwrite(p, 1, len, specific_impl_->file);
		if(result <= 0)
		{
#if !defined _WINCE && !defined _WIN32_WCE
			throw CExceptionX(_T("Write Error"));
#endif
		}
		else
		{
			len -= result;
		}
	}
}

unsigned int CFileX::Read(void *p, unsigned int len)
{
	int read = 0;
	int result;
	while((result = fread(p, 1, len, specific_impl_->file)) > 0)
	{
		read += result;
	}
#if !defined _WINCE && !defined _WIN32_WCE
	if(result < 0) throw CExceptionX(_T("Read Error"));
#endif
	return read;
}

unsigned long CFileX::GetPosition() const
{
	long result = ftell(specific_impl_->file);
#if !defined _WINCE && !defined _WIN32_WCE
	if(result < 0) throw CExceptionX(_T("GetPosition Error"));
#endif
	return result;
}

void CFileX::Seek(long pos, SEEK rel)
{
	int origin;
	switch(rel)
	{
	case begin:
		origin = SEEK_SET;
		break;
	case end:
		origin = SEEK_END;
		break;
	}
	if(fseek(specific_impl_->file, pos, origin))
#if !defined _WINCE && !defined _WIN32_WCE
		throw CExceptionX(_T("Seek Error"));
#else
		{}
#endif
}

#if !defined _WINCE && !defined _WIN32_WCE

bool CFileX::GetStatus(const TCHAR *file_path, FileStatus &status)
{
	struct _stat stat;
	if(_tstat(file_path, &stat) != 0) return false;
	status.m_mtime = stat.st_mtime;
	status.m_size = stat.st_size;
	return true;
}

#else

bool CFileX::GetStatus(const TCHAR *file_path, FileStatus &status)
{
	static const unsigned int FT_TICKS_PER_SEC = 10000000;

	WIN32_FILE_ATTRIBUTE_DATA stat;
	if(!GetFileAttributesEx(file_path, GetFileExInfoStandard, &stat)) return false;
	ULARGE_INTEGER filetime;
	filetime.LowPart = stat.ftLastWriteTime.dwLowDateTime;
	filetime.HighPart = stat.ftLastWriteTime.dwHighDateTime;
	SYSTEMTIME st;
	FILETIME ft;
	memset(&st, 0, sizeof(st));
	st.wYear = 1970;
	st.wMonth = 1;
	st.wDay = 1;
	SystemTimeToFileTime(&st, &ft);
	ULARGE_INTEGER lt;
	lt.LowPart = ft.dwLowDateTime;
	lt.HighPart = ft.dwHighDateTime;
	filetime.QuadPart -= lt.QuadPart;
	filetime.QuadPart /= FT_TICKS_PER_SEC;
	status.m_mtime = filetime.QuadPart;
	status.m_size = stat.nFileSizeLow;
	return true;
}

#endif

class CMemFileXImpl
{
public:
	CMemFileXImpl() : buf(NULL), buflen(0), filelen(0), pos(0) {}
	~CMemFileXImpl() { free(buf); }

	void *buf;
	unsigned int filelen;
	unsigned int buflen;
	unsigned int pos;
};

CMemFileX::CMemFileX() : memfile_impl_(NULL)
{
}

CMemFileX::~CMemFileX() {}

bool CMemFileX::Open(const TCHAR *name, unsigned mode)
{
	SetFilePath(name);
	return true;
}

void CMemFileX::Close()
{
}

void CMemFileX::SetLength(unsigned long len)
{
	if(!memfile_impl_) memfile_impl_ = SmartPtr<CMemFileXImpl>(new CMemFileXImpl);
	memfile_impl_->filelen = len;
	if(memfile_impl_->buflen < len) memfile_impl_->buf = realloc(memfile_impl_->buf, (memfile_impl_->buflen = len));
}

unsigned long CMemFileX::GetLength() const
{
	return memfile_impl_ ? memfile_impl_->filelen : 0;
}

void CMemFileX::Write(const void *p, unsigned int len)
{
	if(!memfile_impl_) memfile_impl_ = SmartPtr<CMemFileXImpl>(new CMemFileXImpl);
	if(memfile_impl_->pos + len > memfile_impl_->filelen) memfile_impl_->filelen = memfile_impl_->pos + len;
	if(memfile_impl_->buflen < memfile_impl_->filelen)
	{
		if((memfile_impl_->buflen *= 2) < memfile_impl_->filelen) memfile_impl_->buflen = memfile_impl_->filelen;
		memfile_impl_->buf = realloc(memfile_impl_->buf, memfile_impl_->buflen);
	}
	assert(memfile_impl_->buf);
	memcpy((void *) ((char *) memfile_impl_->buf + memfile_impl_->pos), p, len);
	memfile_impl_->pos += len;
}

unsigned int CMemFileX::Read(void *p, unsigned int len)
{
	if(!memfile_impl_) memfile_impl_ = SmartPtr<CMemFileXImpl>(new CMemFileXImpl);
	unsigned int readlen = memfile_impl_->pos + len < memfile_impl_->filelen ? len : memfile_impl_->filelen - memfile_impl_->pos;
	memcpy(p, (void *) ((char *) memfile_impl_->buf + memfile_impl_->pos), readlen);
	memfile_impl_->pos += readlen;
	return readlen;
}

unsigned long CMemFileX::GetPosition() const
{
	return memfile_impl_ ? memfile_impl_->pos : 0;
}

void CMemFileX::Seek(long pos, SEEK rel)
{
	if(!memfile_impl_) memfile_impl_ = SmartPtr<CMemFileXImpl>(new CMemFileXImpl);
	switch(rel)
	{
	case begin:
		assert(pos >= 0);
		memfile_impl_->pos = pos;
		break;
	case end:
		memfile_impl_->pos = memfile_impl_->filelen + pos;
	}
}
