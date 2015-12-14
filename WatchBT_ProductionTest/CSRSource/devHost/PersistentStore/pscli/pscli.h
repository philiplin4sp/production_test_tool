// pscli.h
// Command line interface to the persistent store DLL.

// $Log: pscli.h,v $
// Revision 1.2  2001/11/26 16:11:53  cl01
// Unicode changes
//
// Revision 1.1  2001/08/21 17:37:11  cl01
// Added pscli.  Early version, untested, primitive.  Only implements file commands
//


#include "unicode/ichar.h"
#include "csrhci/bccmd_trans.h"

class CPSCLIErrorHandler: public bccmd_trans_error_handler
{
public:
  bccmd_trans_error_response on_error_extra(const ichar *error_string, const ichar * caption, const int error_code, const bccmd_trans_error_type error_type);
};


void do_usage(void);

