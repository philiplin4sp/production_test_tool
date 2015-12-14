/******************************************************************************

    file ipv4.h  --  Internet Protocol, Version 4

    Copyright (c) 2002-2005 CSR Ltd.

    $Log: ipv4.h,v $
    Revision 1.1.14.1  2002/12/10 11:23:28  bs01
    Update rfc-16p4-branch to the VM needed for BlueLab 2.7.
    Add VmRebootForDfu, PioGetStrongBias, PioSetStrongBias, StreamConnectDispose, PioSetAuxDac, FileType and FileIndex. (Last two irrelevant on 4Mbit builds.)
    Also merge changes to support usrpio_aux_dac into pio, usrpio, and lc (H17.3).

    Revision 1.1  2002/11/13 16:55:16  bs01
    Get sdklib pan library building; move in.h and ipv4.h from dev/src/csr/pan into interface/app/pan. Fixup includes and other mistakes in devHost/sdklib/pan

    Revision 1.8  2002/11/13 12:49:25  bs01
    Fix includes of pan headers.

    Revision 1.7  2002/11/12 19:32:34  mgr
    Remove BlueLab-specific stuff.

    Revision 1.6  2002/09/10 15:47:22  bs01
    Merge changes between jbs-pan-branch-start and jbs-pan-branch-mark-a onto the main line. Move external pan headers to interface/app/pan directory.

    Revision 1.5.2.1  2002/09/02 12:37:24  bs01
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

    Revision 1.1.2.3  2002/06/28 19:07:34  mgr
    Split out UDP layer.
    Add mDNS server.
    Make radioio cope with odd-length packed pmalloc ('isIN') mblks.
    Add ability to perform ping and mDNS tests, and cause link-local jams.

    Revision 1.1.2.2  2002/06/05 17:49:09  mgr
    Add HTTP server and DVD demo code.
    Manually apply patches from trunk for hostio_usr.
    Add beta TCP injection helper code.
    Add beta flash filing system.
    Improve BNEP compression if destined for remote device.
    Add support for acting as tester in BNEP conformance tests.
    Add support for acting as single-user GN.
    Tweak BNEP API.
    Add BNEP (L2CAP) channel flow control mechanism.
    Add support for BNEP extension headers.
    TCP send to try to work around delayed ack/small tx buffer infelicity.
    Fix TCP TIME-WAIT recycling when inbound not logically empty.
    Suppress useless TCP window updates and expedite them when becoming not-full.
    Add support for IPv4 link-local addresses.
    Add support for IPv4 default router.

    Revision 1.1.2.1  2002/04/15 18:03:02  mgr
    Add IPv4 support.
    Add QUOTE server.
    Add sanity-checking for addresses received in ARP/ND.


******************************************************************************/


#ifndef IPV4_H
#define IPV4_H


#include "app/pan/pantypes.h"


/******************************************************************************

    typedef IPV4_ADDR  --  an IPv4 address

******************************************************************************/

typedef uint32 IPV4_ADDR;


/******************************************************************************

    #defines and macros for IPv4 addressing

Note that reserved addresses include the unspecified address,
and class E addresses include the broadcast address.

******************************************************************************/

#define IPV4_ADDR_LEN 4 /* in octets */
#define IPV4_ADDR_ANY 0 /* 0.0.0.0 */
#define IPV4_ADDR_ALL_SYSTEMS 0xe0000001 /* 224.0.0.1 */
#define IPV4_ADDR_ALL_ROUTERS 0xe0000002 /* 224.0.0.2 */
#define IPV4_ADDR_MDNS 0xe00000fb /* 224.0.0.251 */
#define IPV4_ADDR_BROADCAST 0xffffffff /* 255.255.255.255 */
#define IPV4_PREFIX_CLASSD 0xe /* 224.0.0.0/4 */
#define IPV4_PREFIX_CLASSDANDE 7 /* 224.0.0.0/3 */
#define IPV4_PREFIX_LOOPBACK 127 /* 127.0.0.0/8 */
#define IPV4_PREFIX_LINK_LOCAL 0xa9fe /* 169.254.0.0/16 */
#define IPV4_IS_CLASSD(x) ((unsigned) ((x) >> 28) == IPV4_PREFIX_CLASSD)
#define IPV4_IS_CLASSDORE(x) ((unsigned) ((x) >> 29) == IPV4_PREFIX_CLASSDANDE)
#define IPV4_IS_UNSPECIFIED(x) ((x) == IPV4_ADDR_ANY)
#define IPV4_IS_LOOPBACK(x) ((unsigned) ((x) >> 24) == IPV4_PREFIX_LOOPBACK)
#define IPV4_IS_SAME_ADDR(x,y) ((x) == (y))
#define IPV4_IS_BROADCAST(x) ((x) == IPV4_ADDR_BROADCAST)
#define IPV4_IS_LINK_LOCAL(x) ((unsigned) ((x) >> 16) == IPV4_PREFIX_LINK_LOCAL)


#endif /* IPV4_H */
