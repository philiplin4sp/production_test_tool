/**********************************************************************
*
*  FILE   :  nocopy.h
*
*            Copyright (C) Cambridge Silicon Radio Ltd 2001-2009
*
*  PURPOSE:  A simple base class which says "this class can't be 
*            copied". Just like boost::nocopyable, but boost has 
*            problems with WinCE.
*
*  $Id: //depot/bc/bluesuite_2_4/devHost/common/nocopy.h#1 $
*
***********************************************************************/

#ifndef COMMON__NOCOPY_H

#define COMMON__NOCOPY_H

class NoCopy
{
private:
    NoCopy(const NoCopy &);
    const NoCopy &operator=(const NoCopy &);
protected:
    NoCopy() { }
    ~NoCopy() { }
};

#endif
