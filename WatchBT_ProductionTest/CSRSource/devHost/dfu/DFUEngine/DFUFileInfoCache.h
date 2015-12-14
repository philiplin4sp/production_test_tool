///////////////////////////////////////////////////////////////////////
//
//	File	: DFUFileInfoCache.h
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
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUFileInfoCache.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef DFUFILEINFOCACHE_H
#define DFUFILEINFOCACHE_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "types.h"

// DFUFileInfoCache class
class DFUFileInfoCache
{
public:
	static void init();
	static void deinit();

	// Constructor
	DFUFileInfoCache(const TCHAR *file);

	// Check file type
	bool IsValid() const;
	bool IsVersion2() const;

	// Read DFU suffix
	uint16 GetVendor() const;
	uint16 GetProduct() const;
	uint16 GetDevice() const;

	// Read comment field
	CStringX GetComment() const;

private:

	// The file type
	enum filetype
	{
		invalid,
		version1,
		version2
	};

	// The file type
	filetype type;

	// Internal file details
	uint16 idVendor;
	uint16 idProduct;
	uint16 bcdDevice;
	CStringX comment;

public:
	// Type used to implement the cache
	struct Cache;
};

#endif
