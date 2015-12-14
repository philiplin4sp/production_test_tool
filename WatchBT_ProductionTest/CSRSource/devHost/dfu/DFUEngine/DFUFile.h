///////////////////////////////////////////////////////////////////////
//
//	File	: DFUFile.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFUFile
//
//	Purpose	: Manipulation of DFU files. This supports both the
//			  reading of existing files, and the creation of new ones.
//
//			  The body of the file, i.e. everything excluding the DFU
//			  suffix, is accessed via the CFile operator which provides
//			  access to the underlying CMemFile object. All member
//			  methods that access the file data preserve its current
//			  file pointer.
//
//			  Two constructors are provided. If no arguments are
//			  supplied then an empty DFU file object is created.
//			  If a filename is supplied then that file is read.
//			  However, since no status can be returned by the
//			  constructor, it is recommended that the default
//			  constructor is used and then ReadFile called to read the
//			  file.
//
//			  A pair of complementary functions are provided to read
//			  and write DFU files on disk:
//				WriteFile		- Create a new file or overwrite an
//								  existing one.
//				ReadFile		- Read an existing file.
//
//			  These both return a result code indicating whether the
//			  the operation was successful. For a read this includes
//			  a check that the DFU suffix is valid, and that the CRC
//			  is correct.
//
//			  A single member function is provided to reset the DFU
//			  file to defaults, with an empty data body:
//				Reset			- Reset to defaults.
//
//			  A series of member functions are provided to allow the
//			  values in the DFU suffix to be modified:
//				SetVendor		- Write the vendor ID.
//				SetProduct		- Write the product ID.
//				SetDevice		- Write the device ID.
//				SetSuffix		- Replace the whole suffix.
//
//			  A similar series of member functions allows the values
//			  in the DFU suffix to be read:
//				GetVendor		- Read the vendor ID.
//				GetProduct		- Read the product ID.
//				GetDevice		- Read the device ID.
//				GetSuffix		- Read the whole suffix.
//
//			  The special value DFUFile::unused may be used or returned
//			  by any of these functions if the corresponding field is
//			  not used, i.e. if the file is suitable for use with any
//			  device.
//
//			  A single member function calculates the CRC over a
//			  specified region of the file body (inclusive of lower
//			  bound and exclusive of upper bound):
//				CalculateCRC	- Calculate CRC.
//
//	Modification history:
//	
//		1.1		27:Sep:00	at	File created.
//		1.2		20:Nov:00	at	File sharing options specified.
//		1.3		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.4		27:Feb:01	at	Added support for calculating CRC over
//								arbitrary portions of the file. Added
//								const accessor operators.
//		1.5		30:Apr:01	ckl	Added Windows CE support.
//		1.6		23:May:01	at	Cacheing used where possible.
//		1.7		24:May:01	at	Cacheing really used where possible.
//		1.8		23:Jul:01	at	Added version string.
//		1.9		06:Oct:01	at	Added access to whole file suffix.
//		1.10	06:Dec:02	at	Removed use of SEH on Windows CE.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUFile.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef DFUFILE_H
#define DFUFILE_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "DFUEngine.h"
#include "types.h"
#include "CFileX.h"

// DFUFile class
class DFUFile
{
public:

	// The file suffix
	struct Suffix
	{
		uint16 bcdDevice;
		uint16 idProduct;
		uint16 idVendor;
		uint16 bcdDFU;
		uint8 ucDfuSignature[3];
		uint8 bLength;
		uint32 dwCRC;
	};

	// Special value to indicate an unused field
	enum { unused = 0xFFFF };

	// Constructors
	DFUFile();
	DFUFile(const TCHAR *file);

	// Destructor
	virtual ~DFUFile();

	// Reset the contents of the DFU file
	DFUEngine::Result Reset();

	// Reading and writing of disk files
	DFUEngine::Result WriteFile(const TCHAR *file) const;
	DFUEngine::Result ReadFile(const TCHAR *file);

	// Access to file contents
	const CFileX &operator*() const;
	CFileX &operator*();
	const CFileX *operator->() const;
	CFileX *operator->();

	// Write DFU suffix
	void SetVendor(uint16 vendor);
	void SetProduct(uint16 product);
	void SetDevice(uint16 device);
	void SetSuffix(const Suffix &suffix);

	// Read DFU suffix
	uint16 GetVendor() const;
	uint16 GetProduct() const;
	uint16 GetDevice() const;
	void GetSuffix(Suffix &suffix) const;

	// Calculate the CRC for part of the file
	uint32 CalculateCRC(uint32 start, uint32 end) const;

private:

	// The file contents
	mutable CMemFileX data;

	// The file suffix
	Suffix suffix;

	// Calculate the CRC to be stored in the DFU file suffix
	uint32 CalculateCRCDFU() const;
};

#endif
