///////////////////////////////////////////////////////////////////////
//
//	File	: StringTable.cpp
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: J. Cooper
//
//	Class	: 
//
//	Purpose	: A windows resource implementation of an abstract wrapper around a String Table.
//
//	$Id: //depot/bc/main/devHost/dfu/DFUEngine/StringTable.cpp#27 $
//
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StringTable.h"
#include <windows.h>

// Visual C++ debugging code
#ifdef _DEBUG
#include <crtdbg.h>
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define new DEBUG_NEW
#endif

int GetStringTableEntry(unsigned int ID, TCHAR *buf, unsigned int len)
{
	return LoadString(GetModuleHandle(_T("DFUEngine.dll")), ID, buf, len);
}

CStringX GetStringTableEntry(unsigned int ID)
{
	CStringX ret;
	int trylen = 255;
	bool done = false;
	while(!done)
	{
		TCHAR *pbuf = ret.GetBuffer(trylen+1);
		int result = GetStringTableEntry(ID, pbuf, trylen+1);
		if(result < 0)
		{
			ret.ReleaseBuffer(0);
			done = true;
		}
		else
		{
			ret.ReleaseBuffer(result);
			if (result < trylen)
			{
				done = true;
			}
			else
			{
				trylen = (trylen+1)*2 - 1;
			}
		}
	}
	return ret;
}
