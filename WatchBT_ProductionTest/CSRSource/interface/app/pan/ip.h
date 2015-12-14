/******************************************************************************

    file ip.h  --  generic Internet Protocol support

    Copyright (c) 2002-2005 CSR Ltd.

    $Log: ip.h,v $
    Revision 1.2.24.1.22.1  2003/05/27 17:55:52  mgr
    Merge in the latest and greatest PAN stuff.

    Revision 1.2.24.1  2003/02/20 17:02:01  cph
    merged vm from rfc-16p4-branch. merged Bluestack from head 20/2/2003

    Revision 1.2.22.1  2002/12/10 11:23:28  bs01
    Update rfc-16p4-branch to the VM needed for BlueLab 2.7.
    Add VmRebootForDfu, PioGetStrongBias, PioSetStrongBias, StreamConnectDispose, PioSetAuxDac, FileType and FileIndex. (Last two irrelevant on 4Mbit builds.)
    Also merge changes to support usrpio_aux_dac into pio, usrpio, and lc (H17.3).

    Revision 1.3  2002/10/18 16:48:12  mgr
    Implement new NAP/GN-supporting BNEP API.

    Revision 1.2  2002/09/10 15:47:22  bs01
    Merge changes between jbs-pan-branch-start and jbs-pan-branch-mark-a onto the main line. Move external pan headers to interface/app/pan directory.

    Revision 1.1.2.3  2002/09/10 14:06:35  bs01
    Make IP_ADDR a typedef for IPV6_ADDR

    Revision 1.1.2.2  2002/09/04 09:59:14  bs01
    tcp_hcked and bnep_hcked moving closer to tcp and bnep headers. Autogen stuff updated to cope.

    Revision 1.1.2.1  2002/09/02 12:37:27  bs01
    Move pan headers and support from interface/app/bluestack and dev/src/csr/pan into interface/app/pan. Update dev side to cope.

    Revision 1.4  2002/08/30 18:01:01  mgr
    Rename IP_ADDRs' s6_addr[] to w[].
    Rename PSKEY_IPVn_FIXED_* to PSKEY_IPVn_STATIC_*.
    Rename PSKEY_IPVn_ROUTER_ADDR to PSKEY_IPVn_STATIC_ROUTER_ADDR.

    Revision 1.3  2002/08/30 18:01:00  mgr
    Rename IP_ADDRs' s6_addr[] to w[].
    Rename PSKEY_IPVn_FIXED_* to PSKEY_IPVn_STATIC_*.
    Rename PSKEY_IPVn_ROUTER_ADDR to PSKEY_IPVn_STATIC_ROUTER_ADDR.

    Revision 1.2  2002/07/18 14:05:24  mgr
    Merge from chef-1-branch.

    Revision 1.1.2.4  2002/06/28 19:07:34  mgr
    Split out UDP layer.
    Add mDNS server.
    Make radioio cope with odd-length packed pmalloc ('isIN') mblks.
    Add ability to perform ping and mDNS tests, and cause link-local jams.

    Revision 1.1.2.3  2002/04/15 18:03:02  mgr
    Add IPv4 support.
    Add QUOTE server.
    Add sanity-checking for addresses received in ARP/ND.

    Revision 1.1.2.2  2002/04/05 18:39:33  mgr
    Make the TCP primitives public.
    Improve the public/private header split.
    Some lint pacification.

    Revision 1.1.2.1  2002/03/18 15:19:09  mgr
    Initial revision.


******************************************************************************/


#ifndef IP_H
#define IP_H


#include "ipv6.h"


/******************************************************************************

    typedef IP_ADDR  --  an IP address

******************************************************************************/

typedef IPV6_ADDR IP_ADDR;


/******************************************************************************

    #defines and macros for IP addressing

HERE: We're not dealing with directed (IPv4) broadcasts here.

******************************************************************************/

#define IP_IS_MULTICAST(x) (IPV6_IS_MULTICAST (x) || \
                            IPV6_IS_IPV4MAPPED_CLASSDORE (x))
#define IP_IS_UNSPECIFIED(x) (( \
        /* :: or 0.0.0.0 */    (x)->w[0] | \
                               (x)->w[1] | \
                               (x)->w[2] | \
                               (x)->w[3] | \
                               (x)->w[4] | \
                               (x)->w[6] | \
                               (x)->w[7] \
                             ) == 0 && \
                             ((x)->w[5] == 0 || \
                              (x)->w[5] == 0xffff))
#define IP_IS_SAME_ADDR IPV6_IS_SAME_ADDR
#define IP_IS_IPV4MAPPED IPV6_IS_IPV4MAPPED


/******************************************************************************

    macro IP_TO_IPV4  --  convert from IP_ADDR to IPV4_ADDR

The IP_ADDR is assumed to be IPv4-mapped.

******************************************************************************/

#define IP_TO_IPV4(x) ((IPV4_ADDR) (x)->w[6] << 16 | (x)->w[7])


/******************************************************************************

    variable IP_ADDR_ANY  --  the wildcard address

******************************************************************************/

extern const IP_ADDR IP_ADDR_ANY;


#endif /* IP_H */
