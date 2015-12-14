/* Store some useful, canned PSR files directly in the application */

// $Id: //depot/bc/bluesuite_2_4/devHost/PersistentStore/PSHelp/canned.h#1 $
// $Log: canned.h,v $
// Revision 1.2  2002/09/19 17:45:27  cl01
// Added Operation functionality - can now delete keys as well.
//
// Revision 1.1  2002/09/19 15:05:08  cl01
// Moved from pshelp.cpp
//


#ifndef CANNED_H_INCLUDED
#define CANNED_H_INCLUDED

#include "pshelp.h"

struct canned_operation {
    CPSHelper::ps_file_op operation;
    int key;
    uint16 value;
};



struct canned_entry {
     ichar *name;
	 ichar *description;
     const canned_operation *contents;
};

extern PSHELP_API const canned_entry canned[];

#endif // CANNED_H_INCLUDED
