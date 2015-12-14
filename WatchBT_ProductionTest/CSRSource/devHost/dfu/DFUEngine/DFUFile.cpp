///////////////////////////////////////////////////////////////////////
//
//	File	: DFUFile.cpp
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
//			  suffix, is accessed via the CFileX operator which provides
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
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUFile.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUFile.h"
#include "CRC.h"
#include "../SmartPtr.h"
#include "resource.h"
#include "DFUFileCache.h"
#include <stddef.h>
#include <string.h>

// Visual C++ debugging code
#ifdef _DEBUG
#include <crtdbg.h>
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define new DEBUG_NEW
#endif

// Version string
const char *dfufile_id = "$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUFile.cpp#1 $ Copyright (C) 2000-2006 Cambridge Silicon Radio Limited";

// Standard suffix values
static const uint8 suffixSignature[3] = {'U', 'F', 'D'};
static const uint16 suffixRevision = 0x0100;

// Size of buffer to use for accessing file data
static const uint32 bufferSize = 1024;

// Constructors
DFUFile::DFUFile()
{
	// Set default values
	Reset();
}

DFUFile::DFUFile(const TCHAR *file)
{
	// Attempt to read the file, ignoring any result
	ReadFile(file);
}

// Destructor
DFUFile::~DFUFile()
{
}

// Reset the contents of the DFU file
DFUEngine::Result DFUFile::Reset()
{
#if !defined _WINCE && !defined _WIN32_WCE
	try
	{
#endif
		// Reset the suffix
		suffix.dwCRC = 0;
		suffix.bLength = sizeof(Suffix);
		memcpy(suffix.ucDfuSignature,
			   suffixSignature, sizeof(suffixSignature));
		suffix.bcdDFU = suffixRevision;
		suffix.idVendor = unused;
		suffix.idProduct = unused;
		suffix.bcdDevice = unused;

		// Empty any file data
		data.SeekToBegin();
		data.SetLength(0);
#if !defined _WINCE && !defined _WIN32_WCE
	}
	catch (...)
	{
		return DFUEngine::Result(DFUEngine::fail_exception, GetStringTableEntry(IDS_RESULT_FAIL_EXCEPTION_UNKNOWN));
	}
#endif

	return DFUEngine::success;
}

// Reading and writing of disk files
DFUEngine::Result DFUFile::WriteFile(const TCHAR *file) const
{
#if !defined _WINCE && !defined _WIN32_WCE
	try
	{
#endif
		// Create a buffer to contain the file data
		uint32 length = data.GetLength() + sizeof(Suffix);
		SmartPtr<uint8, true> buffer(new uint8[length]);

		// Write the file body to the buffer if not empty
		if (data.GetLength() != 0)
		{
			// Store the current file pointer and rewind to the start
			uint32 originalPosition = data.GetPosition();
			data.SeekToBegin();

			// Write the file body
			data.Read(buffer, length);

			// Restore the original file position
			data.Seek(originalPosition, CFileX::begin);
		}

		// Copy the DFU suffix to the buffer
		Suffix suffix;
		GetSuffix(suffix);
		memcpy(buffer + data.GetLength(), &suffix, sizeof(Suffix));

		// Attempt to create the file
		CFileX out;
		if (!out.Open(file, CFileX::modeCreate | CFileX::modeWrite | CFileX::shareExclusive))
		{
			return DFUEngine::Result(DFUEngine::fail_file_create, file);
		}

		// Write the buffer to the file
		out.Write(buffer, length);

		// Update the file path
		data.SetFilePath(out.GetFilePath());

		// Close the file before updating the cache
		out.Close();

		// Store the file data within the cache (transfers buffer ownership)
		DFUFileCache(length, buffer).WriteCache(file);
#if !defined _WINCE && !defined _WIN32_WCE
	}
	catch (...)
	{
		return DFUEngine::Result(DFUEngine::fail_exception, GetStringTableEntry(IDS_RESULT_FAIL_EXCEPTION_UNKNOWN));
	}
#endif

	// Successful if this point reached
	return DFUEngine::success;
}

DFUEngine::Result DFUFile::ReadFile(const TCHAR *file)
{
#if !defined _WINCE && !defined _WIN32_WCE
	try
	{
#endif
		// Start with default values
		DFUEngine::Result result = Reset();
		if (!result) return result;

		// Attempt to read the file from the cache
		DFUFileCache cache;
		uint32 length;
		SmartPtr<uint8, true> buffer;
		if (cache.ReadCache(file))
		{
			// Extract the data from the cache
			length = cache.GetLength();
			buffer = cache.GetData();

			// Copy the file body from the buffer
			data.Write(buffer, length - sizeof(Suffix));

			// Copy the DFU suffix from the buffer
                        Suffix suffix;
                        memcpy(&suffix,buffer + length - sizeof(Suffix),sizeof(Suffix));
			SetSuffix(suffix);

			// Set the file path and rewind to the start
			data.SetFilePath(file);
			data.SeekToBegin();
		}
		else
		{
			// Attempt to open the file
			CFileX in;
			if (!in.Open(file, CFileX::modeRead | CFileX::shareDenyWrite))
			{
				return DFUEngine::Result(DFUEngine::fail_file_open, file);
			}

			// Read and validate the suffix
			length = in.GetLength();
			if (length <= sizeof(Suffix))
			{
				return DFUEngine::Result(DFUEngine::fail_file_short, file);
			}
			in.Seek(-(int32) sizeof(Suffix), CFileX::end);
			Suffix suffix;
			in.Read(&suffix, sizeof(Suffix));
			SetSuffix(suffix);
			if ((suffix.bLength < sizeof(Suffix))
				|| (length < suffix.bLength)
				|| (memcmp(suffix.ucDfuSignature,
						   suffixSignature, sizeof(suffixSignature)) != 0)
				|| (suffix.bcdDFU != suffixRevision))
			{
				return DFUEngine::Result(DFUEngine::fail_file_suffix, file);
			}

			// Read the whole file to the buffer
			buffer = SmartPtr<uint8, true>(new uint8[length]);
			in.SeekToBegin();
			if (in.Read(buffer, length) != length)
			{
				return DFUEngine::Result(DFUEngine::fail_file_read, file);
			}

			// Copy the file body from the buffer
			data.Write(buffer, length - sizeof(Suffix));

			// Verify the CRC
			if (suffix.dwCRC != CalculateCRCDFU())
			{
				return DFUEngine::Result(DFUEngine::fail_file_crc, file);
			}

			// Set the file path and rewind to the start
			data.SetFilePath(in.GetFilePath());
			data.SeekToBegin();

			// Close the file before updating the cache
			in.Close();

			// Store the file data within the cache (transfers buffer ownership)
			DFUFileCache(length, buffer).WriteCache(file);
		}
#if !defined _WINCE && !defined _WIN32_WCE
	}
	catch (...)
	{
		return DFUEngine::Result(DFUEngine::fail_exception, GetStringTableEntry(IDS_RESULT_FAIL_EXCEPTION_UNKNOWN));
	}
#endif

	// Successful if this point reached
	return DFUEngine::success;
}

// Access to file contents
const CFileX &DFUFile::operator*() const
{
	return data;
}

CFileX &DFUFile::operator*()
{
	return data;
}

const CFileX *DFUFile::operator->() const
{
	return &data;
}

CFileX *DFUFile::operator->()
{
	return &data;
}

// Write DFU suffix
void DFUFile::SetVendor(uint16 vendor)
{
	suffix.idVendor = vendor;
}

void DFUFile::SetProduct(uint16 product)
{
	suffix.idProduct = product;
}

void DFUFile::SetDevice(uint16 device)
{
	suffix.bcdDevice = device;
}

void DFUFile::SetSuffix(const Suffix &suffix)
{
	this->suffix = suffix;
}

// Read DFU suffix
uint16 DFUFile::GetVendor() const
{
	return suffix.idVendor;
}

uint16 DFUFile::GetProduct() const
{
	return suffix.idProduct;
}

uint16 DFUFile::GetDevice() const
{
	return suffix.bcdDevice;
}

void DFUFile::GetSuffix(Suffix &suffix) const
{
	suffix = this->suffix;
	suffix.dwCRC = CalculateCRCDFU();
}

// Calculate the CRC for the file
uint32 DFUFile::CalculateCRC(uint32 start, uint32 end) const
{
	CRC crc;

	// Clip the range to the file extent
	if (data.GetLength() < end)
	{
		end = data.GetLength();
	}

	// Calculate the CRC over the specified part of the file body
	if (start < end)
	{
		// Store the current file pointer and rewind to the start
		uint32 originalPosition = data.GetPosition();
		data.Seek(start, CFileX::begin);

		// Include the data in the CRC
		SmartPtr<uint8, true> buffer(new uint8[bufferSize]);
		while (start < end)
		{
			uint32 read = data.Read(buffer, bufferSize < end-start ? bufferSize : end - start);
			crc(buffer, read);
			start += read;
		}

		// Restore the original file position
		data.Seek(originalPosition, CFileX::begin);
	}

	// Return the result
	return crc();
}

// Calculate the CRC for the file
uint32 DFUFile::CalculateCRCDFU() const
{
	// Calculate the CRC over the file body
	CRC crc(CalculateCRC(0, data.GetLength()));

	// Include the suffix, except for the CRC itself
	crc(&suffix, offsetof(Suffix, dwCRC));

	// Return the result
	return crc();
}
