///////////////////////////////////////////////////////////////////////
//
//	File	: CExceptionX.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: J. Cooper
//
//	Class	: CExceptionX
//
//	Purpose	: A drop in replacement for CException.
//            All implemented methods perform the same functions as those in CException.
//
//	$Id: //depot/bc/main/devHost/dfu/DFUEngine/CExceptionX.h#27 $
//
///////////////////////////////////////////////////////////////////////


#ifndef CEXCEPTIONX_H
#define CEXCEPTIONX_H

#include <tchar.h>

class CExceptionX
{
protected:
	const TCHAR *msg_;
public:
	CExceptionX() : msg_(NULL) {}
	CExceptionX(const TCHAR *msg) : msg_(msg) {}
	bool GetErrorMessage(TCHAR *buf, unsigned int len) const
	{
		if(!msg_) return false;
		unsigned int i;
		for(i=0; msg_[i] && i<len-1; ++i) buf[i]=msg_[i];
		buf[i]=_T('\0');
		return true;
	}
};

#endif