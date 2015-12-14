// Copyright (C) 2000-2006 Cambridge Silicon Radio Ltd.; all rights reserved.
// Asserting on sizes.
// $Id: //depot/bc/bluesuite_2_4/devHost/btcli/assertn.h#1 $


#ifndef ASSERTN_H
#define ASSERTN_H


#define ASSERTN(x,n)	ASSERT ((n) == 32 || ((x) >> (n)) == 0)
#define ASSERT8(x)	ASSERTN (x, 8)
#define ASSERT16(x)	ASSERTN (x, 16)
#define ASSERT24(x)	ASSERTN (x, 24)


#endif /* ASSERTN_H */
