// Copyright (C) 2000-2006 Cambridge Silicon Radio Ltd.; all rights reserved.
// btcli symbolic constants.
// $Id: //depot/bc/bluesuite_2_4/devHost/btcli/constants.h#1 $


#ifndef CONSTANTS_H
#define CONSTANTS_H


#define NOTHCICOMMAND	0xffff	/* Make sure this isn't a valid HCI command opcode! */
#define NOHANDLE	0xffff
#ifndef FTPCID
#define FTPCID		0xdada
#endif
#define RESET		0x0c03
#define HNOCP		0x0c35
#define TUNNEL		0xfc00
#define BUFFERSIZE	512		/* HERE it would be nice to avoid using all these large but fixed-size buffers all over the place */
#define NUL		0		/* C string terminator */
#ifndef SENDHWM
#define SENDHWM		(256 * 1024)	/* Note that even at 723k2 less than 100 K is being sent per second */
#endif
#ifndef SENDL2CAPSIZE
#define SENDL2CAPSIZE	(339 * 193)	/* Shouldn't exceed 0x10003 */
#endif
#define MAXSENDS	8
#define MAXRECVS	8

#ifndef FRAGMENT_START
#define FRAGMENT_START (0x40)
#endif
#ifndef FRAGMENT_END
#define FRAGMENT_END (0x80)
#endif
#define FRAGMENT_COMPLETE (FRAGMENT_START | FRAGMENT_END)
#define CHANNEL_ID_LMP_DEBUG 20
#define CHANNEL_ID_VM_DATA 13


#endif /* CONSTANTS_H */
