///////////////////////////////////////////////////////////////////////
//
//	File	: DFUFile2.cpp
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
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUFile2.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUFile2.h"
#include "CFileX.h"
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
const char *dfufile2_id = "$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUFile2.cpp#1 $ Copyright (C) 2000-2006 Cambridge Silicon Radio Limited";

// Standard header values
static const uint8 headerIdentifier1[] = {'C', 'S', 'R', '-', 'd', 'f', 'u', '1'};
static const uint8 headerIdentifier2[] = {'C', 'S', 'R', '-', 'd', 'f', 'u', '2'};
static const uint16 headerReadVersion = 1;
static const uint16 headerWriteVersion = 2;

// Constructors
DFUFile2::DFUFile2() : DFUFile()
{
}

DFUFile2::DFUFile2(const TCHAR *file) : DFUFile(file)
{
}

// Destructor
DFUFile2::~DFUFile2()
{
}

// Read and write comment field
bool DFUFile2::SetComment(const TCHAR *comment)
{
	CStringX comment_ = comment;

	// No action unless a version 2 file
	Header header;
	if (!IsVersion2() || !GetHeader(header))
	{
		return false;
	}

	// Set the new comment string in the header
	for (unsigned int i = 0; i < sizeof(header.file_desc); ++i)
	{
		header.file_desc[i] = (i < comment_.GetLength())
		                      && (32 <= comment_[i]) && (comment_[i] < 127)
							  ? char(comment_[i]) : ' ';
	}

	// Set the new comment string in the description area
	uint32 length = comment_.GetLength() * sizeof(uint16);
	if (!BodyWrite(comment_, DescriptionAreaOffset(header),
	               offsetof(Header, ss) <= header.file_hdr_len ? header.desc_len : 0,
	               length))
	{
		return false;
	}
	header.file_len += length - header.desc_len;
	header.desc_len = length;

	// Ensure that the header is large enough
	if (header.file_version < headerWriteVersion)
		header.file_version = headerWriteVersion;
	if (header.file_hdr_len < sizeof(Header))
	{
		header.file_len += sizeof(Header) - header.file_hdr_len;
		header.file_hdr_len = sizeof(Header);
	}

	// Write the modified header back to the file
	if (!SetHeader(header))
	{
		return false;
	}

	// Update the file CRC
	Footer footer;
	footer.file_crc = CalculateCRCBody();
	if (!SetFooter(footer))
	{
		return false;
	}

	// Successful if this point reached
	return true;
}

CStringX DFUFile2::GetComment() const
{
	// Read the file header
	Header header;
	if (!IsVersion2() || !GetHeader(header))
	{
		return CStringX();
	}

	// Check for a description area
	CStringX comment;
	if ((offsetof(Header, ss) <= header.file_hdr_len)
		&& (header.desc_len != 0))
	{
		// Extract the description area
		uint32 length = header.desc_len / sizeof(uint16);
		SmartPtr<wchar_t, true> buffer(new wchar_t[length]);
		if (BodyRead(buffer, DescriptionAreaOffset(header), header.desc_len))
		{
			comment = CStringX((wchar_t *) buffer, length);
		}
	}
	else
	{
		// Use the description field from the file header
		comment = CStringX((char *) header.file_desc, sizeof(header.file_desc));
	}

	// Strip any trailing white space
	comment.TrimRight();

	// Return the comment
	return comment;
}

// Check file version
bool DFUFile2::IsVersion2() const
{
	// Read the file header and footer
	Header header;
	Footer footer;
	if (!GetHeader(header) || !GetFooter(footer))
	{
		return false;
	}

	// Check the header identification string
	if ((memcmp(header.file_id, headerIdentifier1,
		        sizeof(headerIdentifier1)) != 0)
		&& (memcmp(header.file_id, headerIdentifier2,
		           sizeof(headerIdentifier2)) != 0))
	{
		return false;
	}

	// Check the header version and length
	if ((header.file_version < headerReadVersion)
		|| (header.file_hdr_len < offsetof(Header, file_desc)))
	{
		return false;
	}

	// Check file crc
	if (footer.file_crc != CalculateCRCBody())
	{
		return false;
	}

	// Successful if this point reached
	return true;
}

// Calculate the CRC for the file body
uint32 DFUFile2::CalculateCRCBody() const
{
	// Exclude the footer from the CRC calculation
	return CalculateCRC(0, (*this)->GetLength() - sizeof(Footer));
}

// Read and write the file header and footer
bool DFUFile2::SetHeader(const Header &header)
{
	// Read the existing header to find its length
	Header originalHeader;
	if (!GetHeader(originalHeader))
	{
		return false;
	}

	// Special case if the new header is longer than the buffer
	if (sizeof(Header) < header.file_hdr_len)
	{
		// Only possible if the header is not being grown
		if (originalHeader.file_hdr_len < header.file_hdr_len)
		{
			return false;
		}

		// Replace the initial segment of the header only
		return BodyWrite(&header, 0, sizeof(Header), sizeof(Header));
	}

	// Replace the whole of the existing header
	return BodyWrite(&header, 0, originalHeader.file_hdr_len, header.file_hdr_len);
}

bool DFUFile2::GetHeader(Header &header) const
{
	// Clear the header buffer to all zeros
	memset(&header, 0, sizeof(header));

	// Attempt to read the fixed fields at the start of the header
	if (!BodyRead(&header, 0, offsetof(Header, file_desc)))
	{
		return false;
	}

	// Read as much of the header as possible
	return BodyRead(&header, 0, header.file_hdr_len < sizeof(Header) ? header.file_hdr_len : sizeof(Header));
}

bool DFUFile2::SetFooter(const Footer &footer)
{
	const CFileX &data = **this;

	// Fail if file not long enough to contain the footer
	if (data.GetLength() < sizeof(Footer))
	{
		return false;
	}

	// Replace the footer
	return BodyWrite(&footer, data.GetLength() - sizeof(Footer),
	                 sizeof(Footer), sizeof(Footer));
}

bool DFUFile2::GetFooter(Footer &footer) const
{
	const CFileX &data = **this;

	// Fail if file not long enough to contain the footer
	if (data.GetLength() < sizeof(Footer))
	{
		return false;
	}

	// Read the footer
	return BodyRead(&footer, data.GetLength() - sizeof(Footer), sizeof(Footer));
}

// Calculate the offset of the description area

uint32 DFUFile2::DescriptionAreaOffset(const Header &header) const
{
	// Sum the size of all preceding areas
	return header.file_hdr_len
	       + header.ss_len
		   + header.as_len
		   + header.ps_len
		   + header.sps_len
		   + header.aps_len;
}

// Read and write from the file body

bool DFUFile2::BodyRead(void *buffer, uint32 position, uint32 length) const
{
#if !defined _WINCE && !defined _WIN32_WCE
	try
	{
#endif
		// File offset will be restored so losing the const qualifier is safe
		CFileX &data = const_cast<CFileX &>(**this);

		// Fail if file not long enough 
		if (data.GetLength() < position + length)
		{
			return false;
		}

		// Store the current file pointer and set the offset
		uint32 originalPosition = data.GetPosition();
		data.Seek(position, CFileX::begin);

		// Read from the file
		data.Read(buffer, length);

		// Restore the original file position
		data.Seek(originalPosition, CFileX::begin);
#if !defined _WINCE && !defined _WIN32_WCE
	}
	catch (...)
	{
		return false;
	}
#endif

	// Successful if this point reached
	return true;
}

bool DFUFile2::BodyWrite(const void *buffer, uint32 position, uint32 originalLength, uint32 bufferLength)
{
#if !defined _WINCE && !defined _WIN32_WCE
	try
	{
#endif
		CFileX &data = **this;

		// Fail if the original length does not fit within the file
		if (data.GetLength() < position + originalLength)
		{
			return false;
		}

		// Store the current file pointer
		uint32 originalPosition = data.GetPosition();

		// Shuffle any data after the insertion point
		if (originalLength != bufferLength)
		{
			// Create a buffer to contain the file data
			uint32 tailLength = data.GetLength() - position - originalLength;
			SmartPtr<uint8, true> tail(new uint8[tailLength]);

			// Read the tail of the file
			if (tailLength != 0)
			{
				data.Seek(position + originalLength, CFileX::begin);
				data.Read(tail, tailLength);
			}

			// Set the new file length
			data.SetLength(position + bufferLength + tailLength);

			// Write the tail of the file to the new position
			if (tailLength != 0)
			{
				data.Seek(position + bufferLength, CFileX::begin);
				data.Write(tail, tailLength);
			}
		}

		// Write the new file data
		data.Seek(position, CFileX::begin);
		data.Write(buffer, bufferLength);

		// Restore the original file position
		data.Seek(originalPosition, CFileX::begin);
#if !defined _WINCE && !defined _WIN32_WCE
	}
	catch (...)
	{
		return false;
	}
#endif

	// Successful if this point reached
	return true;
}