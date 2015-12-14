/****************************************************************************

	Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
	BTRACE.h  -  debug tracing functionality

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating
	1.3  27-apr-00 npm  - added user bit defs
	1.4  28-apr-00 npm  - added data integrity bit def
	1.5	 8-may-00 npm	- fixed problem with PLAINBTRACE definition in release mode
	1.6	 22-june-00 npm	- added WINERR trace bit
*/

#ifndef _BTRACE_H_
#define _BTRACE_H_

#ifdef	RCS_STRINGS
static  char    btrace_h_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/btrace.h#1 $";
#endif

#include "common/types.h"
#include "common/linkage.h"

#ifdef _DEBUG

BCSP_API char * BCSPDebugID ;
BCSP_API uint32 BCSPdebugMask ;

BCSP_API void btrace(uint32 bit,char * fmt,...) ;
BCSP_API void plainbtrace(uint32 bit,char * fmt,...) ;
BCSP_API void openDebugLog(char * filename) ;

#define BTRACELOG(filename) openDebugLog(filename) 
#define BTRACEENABLE(bits) BCSPdebugMask |= (bits)
#define BTRACEDISABLE(bits) {BCSPdebugMask |= (bits) ; BCSPdebugMask ^=(bits) ;}
#define BTRACEID(str) BCSPDebugID = str 

#define BTRACE0(bit,str) btrace(bit,str) 
#define BTRACE1(bit,str,arg0) btrace(bit,str,arg0) 
#define BTRACE2(bit,str,arg0,arg1) btrace(bit,str,arg0,arg1) 
#define BTRACE3(bit,str,arg0,arg1,arg2) btrace(bit,str,arg0,arg1,arg2) 
#define BTRACE4(bit,str,arg0,arg1,arg2,arg3) btrace(bit,str,arg0,arg1,arg2,arg3) 
#define BTRACE5(bit,str,arg0,arg1,arg2,arg3,arg4) btrace(bit,str,arg0,arg1,arg2,arg3,arg4) 
#define BTRACE6(bit,str,arg0,arg1,arg2,arg3,arg4,arg5) btrace(bit,str,arg0,arg1,arg2,arg3,arg4,arg5) 
#define BTRACE7(bit,str,arg0,arg1,arg2,arg3,arg4,arg5,arg6) btrace(bit,str,arg0,arg1,arg2,arg3,arg4,arg5,arg6) 
#define BTRACE8(bit,str,arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7) btrace(bit,str,arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7) 
#define PLAINBTRACE0(bit,str) plainbtrace(bit,str) ;
#define PLAINBTRACE1(bit,str,arg0) plainbtrace(bit,str,arg0) ;

#else

#define BTRACELOG(filename)
#define BTRACEENABLE(bits) 
#define BTRACEDISABLE(bits)
#define BTRACEID(str) 

#define BTRACE0(bit,str) 
#define BTRACE1(bit,str,arg0)
#define BTRACE2(bit,str,arg0,arg1) 
#define BTRACE3(bit,str,arg0,arg1,arg2)  
#define BTRACE4(bit,str,arg0,arg1,arg2,arg3)
#define BTRACE5(bit,str,arg0,arg1,arg2,arg3,arg4)
#define BTRACE6(bit,str,arg0,arg1,arg2,arg3,arg4,arg5) 
#define BTRACE7(bit,str,arg0,arg1,arg2,arg3,arg4,arg5,arg6) 
#define BTRACE8(bit,str,arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7) 
#define PLAINBTRACE0(bit,str) 
#define PLAINBTRACE1(bit,str,arg0) 

#endif

//bits for enabling tracing 


#define SCHEDULER (1<<31) 
#define SLIP (1<<0) 
#define REQROUTER (1<<1) 
#define SEQ (1<<2) 
#define MUX (1<<3) 
#define SLIPSND (1<<4)
#define SLIPSNDRAW (1<<5)
#define IO (1<<6)
#define SLIPRCV (1<<7)
#define RCV (1<<8)
#define LINKEST (1<<9)
#define PKTMNG (1<<10)
#define PKTDLVR (1<<11)
#define DATAINTEGRITY (1<<12) 
#define WINERR (1<<13) 


#define USER0 (1<<16) 
#define USER1 (1<<17) 
#define USER2 (1<<18) 
#define USER3 (1<<19) 
#define USER4 (1<<20) 
#define USER5 (1<<21) 
#define USER6 (1<<22) 
#define USER7 (1<<23) 




#endif
