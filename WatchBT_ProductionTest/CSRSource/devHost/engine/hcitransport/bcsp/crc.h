/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
    CRC.h  -  cyclic redundancy check  functionality

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating

*/





#ifndef _CRC_H_
#define _CRC_H_

#ifdef	RCS_STRINGS
static  char    crc_h_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/crc.h#1 $";
#endif




extern void initCRC(uint16 *crc) ;
extern void updateCRC(uint16* crc,uint8 data) ;
extern uint16 getBigEndianCRC(uint16 crc) ;

#endif
