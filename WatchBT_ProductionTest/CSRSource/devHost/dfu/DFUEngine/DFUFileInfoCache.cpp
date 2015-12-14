///////////////////////////////////////////////////////////////////////
//
//	File	: DFUFileInfoCache.cpp
//		  Copyright (C) 2001-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFUFileInfoCache
//
//	Purpose	: Cached access to DFU file information functions.
//
//			  The constructor takes the name of a file as parameter.
//			  The resulting object provides access to the file's
//			  details, obtained via a cache if possible.
//
//			  The following member functions allow different
//			  information to be retrieved:
//				IsValid		- Is it a valid DFU file.
//				IsVersion2	- Is it a version 2 DFU file.
//				GetVendor	- The vendor ID.
//				GetProduct	- The product ID.
//				GetDevice	- The device ID.
//				GetComment	- The comment string.
//
//	Modification history:
//	
//		1.1		23:May:01	at	File created.
//		1.2		24:May:01	at	Made thread safe.
//		1.3		24:May:01	ckl	Changed map to use LPCTSTR as key.
//		1.4		23:Jul:01	at	Added version string.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUFileInfoCache.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUFileInfoCache.h"
#include "DFUFileCache.h"
#include "DFUFile2.h"
#include "thread/critical_section.h"
#include "thread/atomic_counter.h"
#include "CMapX.h"

// Visual C++ debugging code
#ifdef _DEBUG
#include <crtdbg.h>
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define new DEBUG_NEW
#endif

// Version string
const char *dfufileinfocache_id = "$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUFileInfoCache.cpp#1 $ Copyright (C) 2000-2006 Cambridge Silicon Radio Limited";

// Information stored in the cache
struct DFUFileInfoCache::Cache
{
	// Details used to detect changes
	DFUFileCache::Info info;

	// The file type
	DFUFileInfoCache::filetype type;

	// Internal file details
	uint16 idVendor;
	uint16 idProduct;
	uint16 bcdDevice;
	CStringX comment;
};

// Map of filenames to cached data
static CMapX<CStringX, const TCHAR *, DFUFileInfoCache::Cache, DFUFileInfoCache::Cache &> *map = NULL;
static CriticalSection *lockMap = NULL;

AtomicCounter init_counter(0);

void DFUFileInfoCache::init()
{
	if(init_counter.inc() == 1)
	{
		map = new CMapX<CStringX, const TCHAR *, DFUFileInfoCache::Cache, DFUFileInfoCache::Cache &>;
		lockMap = new CriticalSection;
	}
	DFUFileCache::init();
}

void DFUFileInfoCache::deinit()
{
	DFUFileCache::deinit();
	if(init_counter.dec() == 0)
	{
		delete map;
		map = NULL;
		delete lockMap;
		lockMap = NULL;
	}
}

// Constructor
DFUFileInfoCache::DFUFileInfoCache(const TCHAR *file)
{
	// Read the file information
	DFUFileCache::Info info;
	info = DFUFileCache::ReadInfo(file);

	// Check if the details already exist
	Cache cache;
	bool found;
	{
		CriticalSection::Lock lock(lockMap);
		found = map->Lookup(file, cache) ? true : false;
	}
	if (found && (cache.info == info))
	{
		// Use the cached data
		type = cache.type;
		idVendor = cache.idVendor;
		idProduct = cache.idProduct;
		bcdDevice = cache.bcdDevice;
		comment = cache.comment;
	}
	else
	{
		// Read the file from disk
		DFUFile2 dfu;
		if (dfu.ReadFile(file))
		{
			// Fill in the file details
			type = dfu.IsVersion2() ? version2 : version1;
			idVendor = dfu.GetVendor();
			idProduct = dfu.GetProduct();
			bcdDevice = dfu.GetDevice();
			comment = dfu.GetComment();

			// Update the cache
			cache.info = info;
			cache.type = type;
			cache.idVendor = idVendor;
			cache.idProduct = idProduct;
			cache.bcdDevice = bcdDevice;
			cache.comment = comment;
			{
				CriticalSection::Lock lock(lockMap);
				map->SetAt(file, cache);
			}
		}
		else
		{
			// Not a valid file
			type = invalid;
			idVendor = cache.idProduct = cache.bcdDevice = DFUFile::unused;
			comment.Empty();

			// Update the cache
			{
				CriticalSection::Lock lock(lockMap);
				map->RemoveKey(file);
			}
		}
	}
}

// Check file type
bool DFUFileInfoCache::IsValid() const
{
	return type != invalid;
}

bool DFUFileInfoCache::IsVersion2() const
{
	return type == version2;
}

// Read DFU suffix
uint16 DFUFileInfoCache::GetVendor() const
{
	return idVendor;
}

uint16 DFUFileInfoCache::GetProduct() const
{
	return idProduct;
}

uint16 DFUFileInfoCache::GetDevice() const
{
	return bcdDevice;
}

// Read comment field
CStringX DFUFileInfoCache::GetComment() const
{
	return comment;
}
