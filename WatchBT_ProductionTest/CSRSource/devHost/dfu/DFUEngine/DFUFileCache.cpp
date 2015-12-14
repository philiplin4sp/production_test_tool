///////////////////////////////////////////////////////////////////////
//
//	File	: DFUFileCache.cpp
//		  Copyright (C) 2001-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFUFileCache
//
//	Purpose	: Cached access to DFU file data.
//
//			  Two constructors are provided. Use without parameters
//			  to construct without file data for subsequent cache
//			  lookup. Alternatively, supply the file data to store
//			  in the cache. Note, however, that the cache is not
//			  actually updated until the WriteCache member function
//			  is called.
//
//			  The following member functions are provided to access
//			  the cache:
//				WriteCache	- Write the file data to the cache. This
//							  should be called after the file has been
//							  written to disk since the file's last
//							  modification date is used to monitor
//							  subsequent changes.
//				ReadCache	- Attempt to find specified file within
//							  the cache.
//
//			  A pair of member functions provide access to the file
//			  data:
//				GetLength	- Obtain the length of the data in bytes.
//				GetData		- Obtain a pointer to the data.
//
//			  A static member function allows changes to files on disk
//			  to be detected. This is used internally to maintain the
//			  cache, but it may also be useful to support additional
//			  cache style functionality:
//				ReadInfo	- Read information about a file that
//							  can be used to detect changes.
//
//	Modification history:
//	
//		1.1		23:May:01	at	File created but incomplete.
//		1.2		24:May:01	at	Implemented file cacheing.
//		1.3		24:May:01	at	Added description of functionality
//								and fixed ordering of discards.
//		1.4		24:May:01	ckl	Changed map to use LPCTSTR as key.
//		1.5		23:Jul:01	at	Added version string.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUFileCache.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUFileCache.h"
#include "CMapX.h"
#include "CFileX.h"
#include "thread/critical_section.h"
#include "thread/atomic_counter.h"
#include <time.h>

// Visual C++ debugging code
#ifdef _DEBUG
#include <crtdbg.h>
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define new DEBUG_NEW
#endif

// Version string
const char *dfufilecache_id = "$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUFileCache.cpp#1 $ Copyright (C) 2000-2006 Cambridge Silicon Radio Limited";

// Maximum number of files to cache in memory
#if !defined _WINCE && !defined _WIN32_WCE
static const int max_cache = 20;
#else
static const int max_cache = 2;
#endif

// Information stored in the cache
struct Cache
{
	// Details used to detect changes
	DFUFileCache::Info info;

	// The file data
	uint32 length;
	SmartPtr<uint8, true> data;

	// Last access to the cached data
	CTimeX accessed;
};

// Map of filenames to cached data
typedef CMapX<CStringX, const TCHAR *, Cache, Cache &> MapStringToCache;
static MapStringToCache *map = NULL;
static CriticalSection *lockMap = NULL;

static AtomicCounter init_counter(0);

void DFUFileCache::init()
{
	if(init_counter.inc() == 1)
	{
		map = new MapStringToCache;
		lockMap = new CriticalSection;
	}
}

void DFUFileCache::deinit()
{
	if(init_counter.dec() == 0)
	{
		delete map;
		map = NULL;
		delete lockMap;
		lockMap = NULL;
	}
}

// Constructors
DFUFileCache::DFUFileCache()
{
	// No associated file initially
	length = 0;
}

DFUFileCache::DFUFileCache(uint32 length, SmartPtr<uint8, true> data)
: length(length), data(data)
{
	// Initialised with the specified data
}

// Destructor
DFUFileCache::~DFUFileCache()
{
	// Thread safe access to the smart pointer
	CriticalSection::Lock lock(lockMap);
	data = SmartPtr<uint8, true>(0);
}

// Cache access
void DFUFileCache::WriteCache(const TCHAR *file) const
{
	// Read the file information and set the access time
	Cache cache;
	cache.info = ReadInfo(file);
	cache.accessed = CTimeX::GetCurrentTime();

	// Copy the file details
	cache.length = length;
	cache.data = data;

	// Start of access to shared data
	{
		CriticalSection::Lock lock(lockMap);

		// Delete any existing cache entry for this file
		map->RemoveKey(file);

		// Limit the maximum cache size
		if (max_cache <= map->GetCount())
		{
			// Find the least recently accessed cache entry
			MapStringToCache::POSITION pos = map->GetStartPosition();
			CStringX oldestFile;
			Cache oldestCache;
			map->GetNextAssoc(pos, oldestFile, oldestCache);
			while (pos)
			{
				CStringX thisFile;
				Cache thisCache;
				map->GetNextAssoc(pos, thisFile, thisCache);
				if (thisCache.accessed < oldestCache.accessed)
				{
					oldestFile = thisFile;
					oldestCache = thisCache;
				}
			}

			// Delete the oldest entry
			map->RemoveKey(oldestFile);
		}

		// Update the cache
		map->SetAt(file, cache);
	}
}

bool DFUFileCache::ReadCache(const TCHAR *file)
{
	// Read the file information
	Info info;
	info = DFUFileCache::ReadInfo(file);

	{
		CriticalSection::Lock lock(lockMap);

		// Search the cache for this file
		Cache cache;
		bool found;
		if (map->Lookup(file, cache) && (cache.info == info))
		{
			// Update the time of last access
			cache.accessed = CTimeX::GetCurrentTime();
			map->SetAt(file, cache);

			// Copy the file data
			found = true;
			length = cache.length;
			data = cache.data;
		}
		else
		{
			// Delete any cache entry for this file
			map->RemoveKey(file);

			// Reset any file data
			found = false;
			length = 0;
			data = SmartPtr<uint8, true>(0);
		}

		// Return whether the file was found in the cache
		return found;
	}
}

// File data access
uint32 DFUFileCache::GetLength() const
{
	return length;
}

SmartPtr<uint8, true> DFUFileCache::GetData() const
{
	return data;
}

// Constructor
DFUFileCache::Info::Info()
{
	length = 0;
}

// Read information about a specific file
DFUFileCache::Info DFUFileCache::ReadInfo(const TCHAR *file)
{
	Info info;

	// Read the file information
	CFileX::FileStatus status;
	if (CFileX::GetStatus(file, status))
	{
		info.length = status.m_size;
		info.modified = status.m_mtime;
	}

	// Return the information
	return info;
}

// Comparison of file information
bool operator==(const DFUFileCache::Info &a, const DFUFileCache::Info &b)
{
	return (a.length == b.length) && (a.modified == b.modified);
}