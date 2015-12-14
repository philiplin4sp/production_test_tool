/******************************************************************************

    file in.h  --  generic Internet support

    Copyright (c) 2002-2005 CSR Ltd.

    $Log: in.h,v $
    Revision 1.1.58.1.22.1  2003/05/27 17:55:52  mgr
    Merge in the latest and greatest PAN stuff.

    Revision 1.1.58.1  2003/02/20 17:02:01  cph
    merged vm from rfc-16p4-branch. merged Bluestack from head 20/2/2003

    Revision 1.1.14.1  2002/12/10 11:23:28  bs01
    Update rfc-16p4-branch to the VM needed for BlueLab 2.7.
    Add VmRebootForDfu, PioGetStrongBias, PioSetStrongBias, StreamConnectDispose, PioSetAuxDac, FileType and FileIndex. (Last two irrelevant on 4Mbit builds.)
    Also merge changes to support usrpio_aux_dac into pio, usrpio, and lc (H17.3).

    Revision 1.1  2002/11/13 16:55:16  bs01
    Get sdklib pan library building; move in.h and ipv4.h from dev/src/csr/pan into interface/app/pan. Fixup includes and other mistakes in devHost/sdklib/pan

    Revision 1.4  2002/11/12 19:34:31  mgr
    Move the IP protocol code definitions into ip_private.h .

    Revision 1.3  2002/08/20 17:40:06  mgr
    Add some more port definitions.
    Rename QOTD to QUOTE.

    Revision 1.2  2002/07/18 14:05:24  mgr
    Merge from chef-1-branch.

    Revision 1.1.2.6  2002/06/28 19:07:34  mgr
    Split out UDP layer.
    Add mDNS server.
    Make radioio cope with odd-length packed pmalloc ('isIN') mblks.
    Add ability to perform ping and mDNS tests, and cause link-local jams.

    Revision 1.1.2.5  2002/06/05 17:49:08  mgr
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
    TCP - try to work around delayed ack/small tx buffer infelicity.
    Fix TCP TIME-WAIT recycling when inbound not logically empty.
    Suppress useless TCP window updates and expedite them when becoming not-full.
    Add support for IPv4 link-local addresses.
    Add support for IPv4 default router.

    Revision 1.1.2.4  2002/04/15 18:03:02  mgr
    Add IPv4 support.
    Add QUOTE server.
    Add sanity-checking for addresses received in ARP/ND.

    Revision 1.1.2.3  2002/04/05 18:39:33  mgr
    Make the TCP primitives public.
    Improve the public/private header split.
    Some lint pacification.

    Revision 1.1.2.2  2002/03/28 19:26:36  mgr
    Tighten up state checking.
    Implement T_crt (and T_setup for the responder).
    Add BNEP_DISCONNECT_REQ and implement BNEP_DOWN_IND.
    Set the default link policy to allow switch, hold, sniff and park.
    Fix maximum tohost packet size to cope with maximum-size Ethernet packets.
    Improve config process (pay attention to more_flag and handle collisions).
    Don't ignore Ethernet packets whose source is apparently us (for DAD).

    Revision 1.1.2.1  2002/03/18 15:19:09  mgr
    Initial revision.


******************************************************************************/


#ifndef IN_H
#define IN_H


/******************************************************************************

    #defines for Internet well-known port codes

See http://www.iana.org/assignments/port-numbers .

******************************************************************************/

#define IN_PORT_ANY 0 /* Reserved */
#define IN_PORT_ECHO 7 /* RFC862 */
#define IN_PORT_DISCARD 9 /* RFC863 */
#define IN_PORT_DAYTIME 13 /* RFC867 */
#define IN_PORT_QUOTE 17 /* RFC865 */
#define IN_PORT_CHARGEN 19 /* RFC864 */
#define IN_PORT_TELNET 23 /* RFC854 */
#define IN_PORT_SMTP 25 /* RFC821 */
#define IN_PORT_DOMAIN 53 /* RFC1034, RFC1035 */
#define IN_PORT_BOOTPS 67 /* RFC951 */
#define IN_PORT_BOOTPC 68 /* RFC951 */
#define IN_PORT_TFTP 69 /* RFC1350 */
#define IN_PORT_HTTP 80 /* RFC2616 */
#define IN_PORT_POP3 110 /* RFC1939 */
#define IN_PORT_NNTP 119 /* RFC977 */
#define IN_PORT_ZERO 1023 /* Reserved */
#define IN_PORT_MDNS 5353 /* draft-ietf-dnsext-mdns-10 */


#endif /* IN_H */
