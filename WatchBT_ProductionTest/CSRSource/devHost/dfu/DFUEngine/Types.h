///////////////////////////////////////////////////////////////////////
//
//	File	: Types.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: n/a
//
//	Purpose	: General purpose data types.
//
//	Modification history:
//	
//		1.1		27:Sep:00	at	File created.
//		1.2		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.3		17:Mar:01	jbs	Global type definitions used.
//		1.4		23:May:01	at	Updated change history.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/Types.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef TYPES_H
#define TYPES_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include project header files
#ifndef ARMV4
#include "common/types.h"
#else
#include <types.h>
#endif

#endif
