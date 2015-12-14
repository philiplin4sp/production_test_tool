/****************************************************************************

	Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
	SlipSender.h  -  exports slip abort functionality

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating

*/

#ifndef _SLIPSENDER_H_
#define _SLIPSENDER_H_

#ifdef	RCS_STRINGS
static  char    slipsender_h_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/slipsender.h#1 $";
#endif




struct _BCSPStack ;
extern void abortSLIPSend(struct _BCSPStack * stack) ;

#endif
