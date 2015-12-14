
/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        crc.c  - cyclic redundancy check code

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating
	1.3  28-apr-00 npm  - initialised dest in rev16 to keep compiler happy

*/

#ifdef	RCS_STRINGS
static  char    crc_c_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/crc.c#1 $";
#endif

#include "common/types.h"

//table for calculating CRC for poly 0x1021, lsb processed first, initial value 0xffff,
//bits shifted in reverse order

const uint16 crcTable[] = {
	0x0000, 0x1081, 0x2102, 0x3183,
	0x4204, 0x5285, 0x6306, 0x7387,
	0x8408, 0x9489, 0xa50a, 0xb58b,
	0xc60c, 0xd68d,	0xe70e, 0xf78f
};


/****************************************************************************
NAME
	initCRC

FUNCTION
	initialises the crc (with 0xffff) 

IMPLEMENTATION NOTE

****************************************************************************/


void initCRC(uint16 * crc)
{
	*crc =  0xffff ; 
}

/****************************************************************************
NAME
	updateCRC

FUNCTION
	runs another byte through the crc by treating it as two nibbles.
	The crc is generated in reverse, ie bits are fed into the register from the top

IMPLEMENTATION NOTE

****************************************************************************/

void updateCRC(uint16 * crc,uint8 databyte)
{
	uint16 reg = *crc ;
	reg = (reg >>4) ^ crcTable[(reg ^databyte) &0xf] ;
	reg = (reg >>4) ^ crcTable[(reg ^(databyte>>4)) &0xf] ;
	*crc = reg ; ;
}


/****************************************************************************
NAME
	rev16

FUNCTION
	a convenience function to reverse the bit order of a uint16. 

IMPLEMENTATION NOTE

****************************************************************************/

uint16 rev16(uint16 src)
{
	uint16 dest=0 ; //keep compiler happy 
	int b ;
	for (b = 0 ; b < 16 ; b ++)
	{
		dest = dest << 1 ;
		dest |= (src &1) ;
		src = src >>1 ;
	}
	return dest ;
}

/****************************************************************************
NAME
	getBigEndianCRC

FUNCTION
	returns the crc as a big-endian number, regardless of what the native mode
	is. 

IMPLEMENTATION NOTE
	The crc generator creates a reversed crc so it needs to be swapped back
	before being passed on.

****************************************************************************/

uint16 getBigEndianCRC(uint16 crc)
{
	uint16 bigend ;
	uint8* bp ;
	uint16 ret = rev16(crc) ; //we've been calculating a reversed crc so turn it back the right way.
	//now make it big-endian... (this works whether our native mode is big or little
	bp = (uint8*) &bigend ;
	bp[0] = (ret >>8) & 0xff ;
	bp[1] = ret  & 0xff ;
	return bigend ;
}

