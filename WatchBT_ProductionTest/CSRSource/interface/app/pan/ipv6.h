/******************************************************************************

    file ipv6.h  --  Internet Protocol, Version 6

    Copyright (c) 2002-2005 CSR Ltd.

    $Log: ipv6.h,v $
    Revision 1.2.24.1.22.1  2003/05/27 17:55:52  mgr
    Merge in the latest and greatest PAN stuff.

    Revision 1.2.24.1  2003/02/20 17:02:01  cph
    merged vm from rfc-16p4-branch. merged Bluestack from head 20/2/2003

    Revision 1.2.22.1  2002/12/10 11:23:29  bs01
    Update rfc-16p4-branch to the VM needed for BlueLab 2.7.
    Add VmRebootForDfu, PioGetStrongBias, PioSetStrongBias, StreamConnectDispose, PioSetAuxDac, FileType and FileIndex. (Last two irrelevant on 4Mbit builds.)
    Also merge changes to support usrpio_aux_dac into pio, usrpio, and lc (H17.3).

    Revision 1.3  2002/10/18 16:48:12  mgr
    Implement new NAP/GN-supporting BNEP API.

    Revision 1.2  2002/09/10 15:47:22  bs01
    Merge changes between jbs-pan-branch-start and jbs-pan-branch-mark-a onto the main line. Move external pan headers to interface/app/pan directory.

    Revision 1.1.2.2  2002/09/02 15:02:49  bs01
    Pull all external headers used by pan into a single pantypes.h.

    Revision 1.1.2.1  2002/09/02 12:37:27  bs01
    Move pan headers and support from interface/app/bluestack and dev/src/csr/pan into interface/app/pan. Update dev side to cope.

    Revision 1.5  2002/08/30 18:01:01  mgr
    Rename IP_ADDRs' s6_addr[] to w[].
    Rename PSKEY_IPVn_FIXED_* to PSKEY_IPVn_STATIC_*.
    Rename PSKEY_IPVn_ROUTER_ADDR to PSKEY_IPVn_STATIC_ROUTER_ADDR.

    Revision 1.4  2002/08/30 18:01:00  mgr
    Rename IP_ADDRs' s6_addr[] to w[].
    Rename PSKEY_IPVn_FIXED_* to PSKEY_IPVn_STATIC_*.
    Rename PSKEY_IPVn_ROUTER_ADDR to PSKEY_IPVn_STATIC_ROUTER_ADDR.

    Revision 1.3  2002/08/20 17:40:56  mgr
    BlueLabify.

    Revision 1.2  2002/07/18 14:05:25  mgr
    Merge from chef-1-branch.

    Revision 1.1.2.2  2002/04/05 18:39:33  mgr
    Make the TCP primitives public.
    Improve the public/private header split.
    Some lint pacification.

    Revision 1.1.2.1  2002/03/18 15:19:09  mgr
    Initial revision.


******************************************************************************/


#ifndef IPV6_H
#define IPV6_H


#include "pantypes.h"


/******************************************************************************

    struct IPV6_ADDR  --  an IPv6 address

******************************************************************************/

typedef struct IPV6_ADDR
{
    uint16 w[8];
} IPV6_ADDR;


/******************************************************************************

    #defines and macros for IPv6 addressing

Note that reserved addresses include IPv4-mapped, IPv4-compatible,
loopback and unspecified addresses.

******************************************************************************/

#define IPV6_ADDR_LEN 16 /* in octets */
#define IPV6_PREFIX_RESERVED 0 /* First 8 bits */
#define IPV6_PREFIX_LINK_LOCAL_UNICAST 0xfe80U /* First 10 bits */
#define IPV6_PREFIX_SITE_LOCAL_UNICAST 0xfec0U /* First 10 bits */
#define IPV6_PREFIX_MULTICAST 0xffU /* First 8 bits */
#define IPV6_MULTICAST_SCOPE_NODE 1 /* Fourth 4 bits */
#define IPV6_MULTICAST_SCOPE_LINK 2 /* Fourth 4 bits */
#define IPV6_MULTICAST_SCOPE_SITE 5 /* Fourth 4 bits */
#define IPV6_MULTICAST_SCOPE_ORGANIZATION 8 /* Fourth 4 bits */
#define IPV6_MULTICAST_SCOPE_GLOBAL 14 /* Fourth 4 bits */
#define IPV6_MULTICAST_GROUP_ALL_NODES 1 /* Last 112 bits */
#define IPV6_MULTICAST_GROUP_ALL_ROUTERS 2 /* Last 112 bits */
#define IPV6_MULTICAST_GROUP_MDNS 0xfb /* Last 112 bits */
#define IPV6_MULTICAST_GROUP_SOLICITED 0x01ffU /* Last-but-24 88 bits */
#define IPV6_IS_LINK_LOCAL_UNICAST(x) ((x)->w[0] >> 6 == \
        /* fe80::/10 */          IPV6_PREFIX_LINK_LOCAL_UNICAST >> 6)
#define IPV6_IS_MULTICAST(x) ((x)->w[0] >> 8 == \
        /* ff00::/8 */           IPV6_PREFIX_MULTICAST)
#define IPV6_IS_RESERVED(x) ((x)->w[0] >> 8 == \
        /* ::/8 */               IPV6_PREFIX_RESERVED)
#define IPV6_IS_UNSPECIFIED(x) (( \
        /* :: */                 (x)->w[0] | \
                                 (x)->w[1] | \
                                 (x)->w[2] | \
                                 (x)->w[3] | \
                                 (x)->w[4] | \
                                 (x)->w[5] | \
                                 (x)->w[6] | \
                                 (x)->w[7] \
                               ) == 0)
#define IPV6_IS_LOOPBACK(x) (( \
        /* ::1 */                (x)->w[0] | \
                                 (x)->w[1] | \
                                 (x)->w[2] | \
                                 (x)->w[3] | \
                                 (x)->w[4] | \
                                 (x)->w[5] | \
                                 (x)->w[6] | \
                                 ((x)->w[7] ^ 1) \
                               ) == 0)
#define IPV6_IS_LINK_LOCAL_ALL_NODES_MULTICAST(x) (( \
        /* ff02::1 */            ((x)->w[0] ^ (IPV6_PREFIX_MULTICAST << 8 | IPV6_MULTICAST_SCOPE_LINK)) | \
                                 (x)->w[1] | \
                                 (x)->w[2] | \
                                 (x)->w[3] | \
                                 (x)->w[4] | \
                                 (x)->w[5] | \
                                 (x)->w[6] | \
                                 ((x)->w[7] ^ IPV6_MULTICAST_GROUP_ALL_NODES) \
                               ) == 0)
#define IPV6_IS_SOLICITED_MULTICAST(x,y) (( \
       /* ff02::1:ffyy:yyyy */  ((x)->w[0] ^ ((IPV6_PREFIX_MULTICAST << 8) | IPV6_MULTICAST_SCOPE_LINK)) | \
                                 (x)->w[1] | \
                                 (x)->w[2] | \
                                 (x)->w[3] | \
                                 (x)->w[4] | \
                                 ((x)->w[5] ^ (IPV6_MULTICAST_GROUP_SOLICITED >> 8)) | \
                                 ((x)->w[6] ^ ((((IPV6_MULTICAST_GROUP_SOLICITED & 0xff) << 8) & 0xff00) | ((y)->w[6] & 0xff))) | \
                                 ((x)->w[7] ^ (y)->w[7]) \
                               ) == 0)
#define IPV6_IS_IPV4MAPPED(x) (( \
        /* ::ffff:x.y.z.w */     (x)->w[0] | \
                                 (x)->w[1] | \
                                 (x)->w[2] | \
                                 (x)->w[3] | \
                                 (x)->w[4] | \
                                 ~(x)->w[5] \
                               ) == 0)
#define IPV6_IS_IPV4MAPPED_CLASSDORE(x) (( \
        /* ::ffff:x.y.z.w */     (x)->w[0] | \
        /* where x starts 111 */ (x)->w[1] | \
                                 (x)->w[2] | \
                                 (x)->w[3] | \
                                 (x)->w[4] | \
                                 ~(x)->w[5] \
                               ) == 0 && \
                                 (x)->w[6] >> 13 == 7) /* yeah, I know */
#define IPV6_IS_IPV4COMPATIBLE(x) (( \
        /* ::x.y.z.w */          (x)->w[0] | \
                                 (x)->w[1] | \
                                 (x)->w[2] | \
                                 (x)->w[3] | \
                                 (x)->w[4] | \
                                 (x)->w[5] \
                               ) == 0)
#define IPV6_IS_SAME_ADDR(x,y) (( \
                                 ((x)->w[0] ^ (y)->w[0]) | \
                                 ((x)->w[1] ^ (y)->w[1]) | \
                                 ((x)->w[2] ^ (y)->w[2]) | \
                                 ((x)->w[3] ^ (y)->w[3]) | \
                                 ((x)->w[4] ^ (y)->w[4]) | \
                                 ((x)->w[5] ^ (y)->w[5]) | \
                                 ((x)->w[6] ^ (y)->w[6]) | \
                                 ((x)->w[7] ^ (y)->w[7]) \
                               ) == 0)


#endif /* IPV6_H */
