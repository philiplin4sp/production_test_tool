///////////////////////////////////////////////////////////////////////
//
//	File	: StringTable.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: J. Cooper
//
//	Class	: 
//
//	Purpose	: An abstract wrapper around a String Table.
//
//	$Id: //depot/bc/main/devHost/dfu/DFUEngine/StringTable.h#27 $
//
///////////////////////////////////////////////////////////////////////

#ifndef STRINGTABLE_H
#define STRINGTABLE_H

#include "CStringX.h"

// get string table entry. returns characters copied if found or -1 if not a valid ID.
int GetStringTableEntry(unsigned int ID, TCHAR *buf, unsigned int len);
CStringX GetStringTableEntry(unsigned int ID);

#endif
