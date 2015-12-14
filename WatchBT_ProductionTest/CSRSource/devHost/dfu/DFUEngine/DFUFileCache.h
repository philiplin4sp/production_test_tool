///////////////////////////////////////////////////////////////////////
//
//	File	: DFUFileCache.h
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
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUFileCache.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef DFUFILECACHE_H
#define DFUFILECACHE_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "types.h"
#include "CTimeX.h"
#include "../SmartPtr.h"

// DFUFileCache class
class DFUFileCache
{
public:

	// Constructors
	DFUFileCache();
	DFUFileCache(uint32 length, SmartPtr<uint8, true> data);

	// Destructor
	~DFUFileCache();

	// Cache access
	void WriteCache(const TCHAR *file) const;
	bool ReadCache(const TCHAR *file);

	// File data access
	uint32 GetLength() const;
	SmartPtr<uint8, true> GetData() const;
	
	// Information used to detect changes on disk
	struct Info
	{
		// Constructor
		Info();

		// File information
		uint32 length;		// File length in bytes (zero if does not exist)
		CTimeX modified;		// Date of last modification (only if exists)
	};

	// Read information about a specific file
	static Info ReadInfo(const TCHAR *file);

	static void init();
	static void deinit();

private:

	// The file data
	uint32 length;
	SmartPtr<uint8, true> data;
};

// Comparison of file information
bool operator==(const DFUFileCache::Info &a, const DFUFileCache::Info &b);

#endif
