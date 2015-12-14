///////////////////////////////////////////////////////////////////////
//
//	File	: CRC.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: CRC
//
//	Purpose	: Calculate ANSI X3.66 32-bit cyclic redundancy check
//			  (CRC) values, as used within DFU files.
//
//			  The constructor can optionally be passed an
//			  initialisation value, otherwise the default of
//			  0xFFFFFFFF is used (correct for DFU files).
//
//			  Operator () should be called for each block of data to
//			  be included in the CRC. It returns the result up to that
//			  point. To just read the current value without affecting
//			  the CRC use operator () without any parameters.
//
//	Modification history:
//	
//		1.1		27:Sep:00	at	File created.
//		1.2		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.3		23:Jul:01	at	Added version string.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/CRC.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef CRC_H
#define CRC_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#include "types.h"

// CRC class
class CRC  
{
public:

	// Constructor
	CRC(uint32 initCRC = 0xFFFFFFFF);

	// Update the CRC and return the current value
	uint32 operator()(const void *buffer = 0,
	                  uint32 bufferLength = 0);

private:

	// The accumulator
	uint32 crc;
};

#endif
