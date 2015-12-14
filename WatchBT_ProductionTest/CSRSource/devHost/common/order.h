/* $Id: //depot/bc/bluesuite_2_4/devHost/common/order.h#1 $ */

#ifndef ORDER_H

#define ORDER_H

/*
    Reorder a word to the same bit pattern it would have on a
    little-endian system.

    dest and src must be different
*/

#define NETWORK_REORDER16(dest, src) \
    do { \
        0[(uint8 *)&(dest)] = (uint8)((src) & 0xFF); \
        1[(uint8 *)&(dest)] = (uint8)(((src)>>8) & 0xFF); \
    } while(0)

#define NETWORK_REORDER32(dest, src) \
    do { \
        0[(uint8 *)&(dest)] = (uint8)((src) & 0xFF); \
        1[(uint8 *)&(dest)] = (uint8)(((src)>>8) & 0xFF); \
        2[(uint8 *)&(dest)] = (uint8)(((src)>>16) & 0xFF); \
        3[(uint8 *)&(dest)] = (uint8)(((src)>>24) & 0xFF); \
    } while(0)

#endif
