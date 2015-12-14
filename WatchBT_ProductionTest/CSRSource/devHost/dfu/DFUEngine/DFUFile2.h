///////////////////////////////////////////////////////////////////////
//
//	File	: DFUFile2.h
//		  Copyright (C) 2001-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFUFile2
//
//	Purpose	: Manipulation of version 2 DFU files. This supports both
//			  the reading of existing files, and the creation of new
//			  ones.
//
//			  This class extends the functionality provided by the
//			  DFUFile class to enable manipulation of the comment field
//			  within version 2 file headers. Two member functions are
//			  provided for this purpose:
//				SetComment	- Write the comment text.
//				GetComment	- Read the comment text.
//
//			  Writing the comment text truncates or pads the supplied
//			  string as appropriate to fill the field. Any trailing
//			  spaces used for padding are removed when the comment is
//			  read.
//
//			  An additional member function returns whether the file
//			  is a valid version 2 DFU file:
//				IsVersion2	- Check file version.
//
//			  If the file is not a version 2 DFU file then SetComment
//			  fails silently, and GetComment returns an empty string.
//			  All other methods operate normally; no error is returned
//			  during reading or writing (as long as it is a valid DFU
//			  file).
//
//			  Other member functions are the same as for DFUFile; the
//			  documentation for that class should be consulted for
//			  details.
//
//	Modification history:
//	
//		1.1		26:Feb:01	at	File created.
//		1.2		30:Apr:01	ckl	Added Windows CE support.
//		1.3		23:May:01	at	Tidied up Windows CE support.
//		1.4		23:Jul:01	at	Added version string.
//		1.5		14:Nov:01	at	Added basic Unicode support.
//		1.6		07:Jan:02	at	Added support for BC2 format DFU files.
//		1.7		09:Nov:02	at	Adjust file length in header.
//		1.8		06:Dec:02	at	Removed use of SEH on Windows CE.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUFile2.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef DFUFILE2_H
#define DFUFILE2_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "DFUFile.h"
#include "../SmartPtr.h"

// DFUFile2 class
class DFUFile2 : public DFUFile
{
public:

	// Constructors
	DFUFile2();
	DFUFile2(const TCHAR *file);

	// Destructor
	virtual ~DFUFile2();

	// Read and write comment field
	bool SetComment(const TCHAR *comment);
	CStringX GetComment() const;

	// Check file version
	bool IsVersion2() const;

private:

#pragma pack(push, 1)

	// The file header
	typedef unsigned __int64 uint64;
	struct Header
	{
		uint8 file_id[8];
		uint16 file_version;
		uint32 file_len;
		uint16 file_hdr_len;
		uint8 file_desc[64];
		uint32 ss_len;
		uint32 as_len;
		uint32 ps_len;
		uint32 sps_len;
		uint32 aps_len;
		// The following fields only exist if file_version >= 2
		uint32 desc_len;
		struct
		{
			uint64 bc;
			uint64 fg;
			uint32 ss_len;
		} ss[16];
		struct
		{
			uint16 vm;
			uint64 ts;
			uint32 as_len;
		} as[16];
		struct
		{
			uint64 bc;
			uint32 ps_len;
			uint32 sps_len;
			uint32 aps_len;
		} ps[16];
	};

	// The file footer
	struct Footer
	{
		uint32 file_crc;
	};

#pragma pack(pop)

	// Calculate the CRC for the file body
	uint32 CalculateCRCBody() const;

	// Read and write the file header and footer
	bool SetHeader(const Header &header);
	bool GetHeader(Header &header) const;
	bool SetFooter(const Footer &footer);
	bool GetFooter(Footer &footer) const;

	// Calculate the offset of the description area
	uint32 DescriptionAreaOffset(const Header &header) const;

	// Read and write from the file body
	bool BodyRead(void *buffer, uint32 position, uint32 length) const;
	bool BodyWrite(const void *buffer, uint32 position,
	               uint32 originalLength, uint32 bufferLength);
};

#endif
