/******************************************************************************

    file bnep.h  --  Bluetooth Network Encapsulation Protocol

    Copyright (c) 2002-2005 CSR Ltd.

    $Log: bnep.h,v $
    Revision 1.7  2003/02/24 16:38:38  bs01
    Merge from gccsdk-2-7-branch.
    Add PAN_FLOW_* aliases for BNEP_FLOW_*
    Expose BNEP_REGISTER_REQ_FLAG_DISABLE_STACK to VM.
    802.1p is actually 802.1Q.

    Revision 1.6.10.1  2003/01/27 16:40:21  mgr
    Add PAN_FLOW_* aliases for BNEP_FLOW_*
    Expose BNEP_REGISTER_REQ_FLAG_DISABLE_STACK to VM.
    802.1p is actually 802.1Q.

    Revision 1.6  2003/01/08 15:43:20  mgr
    Remove BNEP_REGISTER_REQ flag definitions for BlueLab builds.
    Add more BNEP_CONNECT_CFM failure codes.

    Revision 1.5  2002/11/13 16:55:16  bs01
    Get sdklib pan library building; move in.h and ipv4.h from dev/src/csr/pan into interface/app/pan. Fixup includes and other mistakes in devHost/sdklib/pan

    Revision 1.4  2002/11/12 19:44:10  mgr
    Add BNEP_HCI_SWITCH_ROLE_RSP and BNEP_DISCONNECT_IND_REASON.
    Use #defines rather than bitfields if not in firmware.

    Revision 1.3  2002/10/18 16:48:12  mgr
    Implement new NAP/GN-supporting BNEP API.

    Revision 1.2  2002/09/10 15:47:22  bs01
    Merge changes between jbs-pan-branch-start and jbs-pan-branch-mark-a onto the main line. Move external pan headers to interface/app/pan directory.

    Revision 1.1.2.3  2002/09/02 15:02:49  bs01
    Pull all external headers used by pan into a single pantypes.h.

    Revision 1.1.2.2  2002/09/02 13:56:46  bs01
    Fold bnep_prim.h and tcp_prim.h into bnep.h and tcp.h respectively; they weren't different enough to be worth keeping.

    Revision 1.1.2.1  2002/09/02 12:37:26  bs01
    Move pan headers and support from interface/app/bluestack and dev/src/csr/pan into interface/app/pan. Update dev side to cope.

    Revision 1.5  2002/08/30 18:01:00  mgr
    Rename IP_ADDRs' s6_addr[] to w[].
    Rename PSKEY_IPVn_FIXED_* to PSKEY_IPVn_STATIC_*.
    Rename PSKEY_IPVn_ROUTER_ADDR to PSKEY_IPVn_STATIC_ROUTER_ADDR.

    Revision 1.4  2002/08/30 18:01:00  mgr
    Rename IP_ADDRs' s6_addr[] to w[].
    Rename PSKEY_IPVn_FIXED_* to PSKEY_IPVn_STATIC_*.
    Rename PSKEY_IPVn_ROUTER_ADDR to PSKEY_IPVn_STATIC_ROUTER_ADDR.

    Revision 1.3  2002/08/20 17:39:44  mgr
    Move flags in BNEP_CONNECT_REQ to after phandle.

    Revision 1.2  2002/07/18 14:05:24  mgr
    Merge from chef-1-branch.

    Revision 1.1.2.6  2002/06/05 17:49:08  mgr
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
    Try to work around TCP delayed ack/small tx buffer infelicity.
    Fix TCP TIME-WAIT recycling when inbound not logically empty.
    Suppress useless TCP window updates and expedite them when becoming not-full.
    Add support for IPv4 link-local addresses.
    Add support for IPv4 default router.

    Revision 1.1.2.5  2002/04/15 18:03:02  mgr
    Add IPv4 support.
    Add QUOTE server.
    Add sanity-checking for addresses received in ARP/ND.

    Revision 1.1.2.4  2002/04/05 18:39:33  mgr
    Make the TCP primitives public.
    Improve the public/private header split.
    Some lint pacification.

    Revision 1.1.2.3  2002/03/28 19:26:36  mgr
    Tighten up state checking.
    Implement T_crt (and T_setup for the responder).
    Add BNEP_DISCONNECT_REQ and implement BNEP_DOWN_IND.
    Set the default link policy to allow switch, hold, sniff and park.
    Fix maximum tohost packet size to cope with maximum-size Ethernet packets.
    Improve config process (pay attention to more_flag and handle collisions).
    Don't ignore Ethernet packets whose source is apparently us (for DAD).

    Revision 1.1.2.2  2002/03/22 13:03:08  mgr
    Changes made at UPF7,5.

    Revision 1.1.2.1  2002/03/18 15:19:07  mgr
    Initial revision.


******************************************************************************/


#ifndef BNEP_H
#define BNEP_H


#include "pantypes.h"
#include "app/bluestack/hci.h" /* for SIZE_LINK_KEY */


/******************************************************************************

    #defines for BNEP magic numbers

******************************************************************************/

#define BNEP_UUID_PANU 0x1115 /* http://www.bluetooth.org/assigned-numbers/sdp.htm */
#define BNEP_UUID_NAP 0x1116 /* http://www.bluetooth.org/assigned-numbers/sdp.htm */
#define BNEP_UUID_GN 0x1117 /* http://www.bluetooth.org/assigned-numbers/sdp.htm */


/******************************************************************************

    struct ETHER_ADDR  --  an Ethernet address

******************************************************************************/

typedef struct ETHER_ADDR
{
    uint16 w[3]; /* Big-endian */
} ETHER_ADDR;


/******************************************************************************

    enum BNEP_MSG_TYPE  --  BNEP task message types
    typedefs for BNEP task messages
    enums for BNEP task message parameters

******************************************************************************/

#define BNEP_PRIM_BASE 0x00fe

#define ENUM_SEP_BNEP_FIRST_PRIM BNEP_PRIM_BASE
#define ENUM_SEP_BNEP_LAST_PRIM (BNEP_PRIM_BASE + 25)

enum BNEP_MSG_TYPE
{
#ifndef BLUELAB
    BNEP_FLOW_REQ = BNEP_PRIM_BASE + 1,
    BNEP_FLOW_IND, /* HERE: rename to PAN_FLOW_* */
#endif
    BNEP_REGISTER_REQ = BNEP_PRIM_BASE + 3,
    BNEP_CONNECT_REQ,
    BNEP_CONNECT_CFM,
    BNEP_CONNECT_IND,
#ifndef BLUELAB
    BNEP_LISTEN_REQ,
    BNEP_IGNORE_REQ,
    BNEP_UP_IND,
    BNEP_DOWN_IND,
    BNEP_DATA_REQ,
    BNEP_EXTENDED_DATA_REQ,
    BNEP_EXTENDED_MULTICAST_DATA_REQ,
    BNEP_DATA_IND,
    BNEP_EXTENDED_DATA_IND,
    BNEP_MULTICAST_DATA_IND,
    BNEP_LOOPBACK_DATA_IND,
#endif
    BNEP_DISCONNECT_REQ = BNEP_PRIM_BASE + 18,
    BNEP_DISCONNECT_IND,
#ifndef BLUELAB
    BNEP_DEBUG_REQ,
    BNEP_DEBUG_IND,
#endif
    BNEP_HCI_SWITCH_ROLE_RSP = BNEP_PRIM_BASE + 22,
    BNEP_ROLE_CHANGE_IND,
    BNEP_BM_IND
};
#define PAN_FLOW_REQ BNEP_FLOW_REQ
#define PAN_FLOW_IND BNEP_FLOW_IND

#ifndef BTCHIP
#ifndef BLUELAB
#define BNEP_REGISTER_REQ_FLAG_DISABLE_EXTENDED 0x0004
#define BNEP_REGISTER_REQ_FLAG_MANUAL_BRIDGE 0x0002 /* reserved for future use */
#endif
#define BNEP_REGISTER_REQ_FLAG_DISABLE_STACK 0x0001
#endif

typedef struct BNEP_REGISTER_REQ_T
{
    uint16 type;
#ifdef BTCHIP
    unsigned int : 13;
    unsigned int disable_extended : 1;
    unsigned int manual_bridge : 1; /* reserved for future use */
    unsigned int disable_stack : 1;
#else
    uint16 flags;
#endif
    phandle_t phandle;
} BNEP_REGISTER_REQ_T;

#ifndef BTCHIP
#define BNEP_CONNECT_REQ_FLAG_RNAP 0x4000
#define BNEP_CONNECT_REQ_FLAG_RGN 0x2000
#define BNEP_CONNECT_REQ_FLAG_RPANU 0x1000
#define BNEP_CONNECT_REQ_FLAG_LNAP 0x0400
#define BNEP_CONNECT_REQ_FLAG_LGN 0x0200
#define BNEP_CONNECT_REQ_FLAG_LPANU 0x0100
#define BNEP_CONNECT_REQ_FLAG_NO_SWITCH 0x0004
#define BNEP_CONNECT_REQ_FLAG_SINGLE_USER 0x0002
#define BNEP_CONNECT_REQ_FLAG_ACCEPT_IN 0x0001
#else
typedef struct BNEP_CONNECT_REQ_FLAGS
{
    unsigned int reserved15 : 1;
    unsigned int rnap : 1;
    unsigned int rgn : 1;
    unsigned int rpanu : 1;
    unsigned int reserved11 : 1;
    unsigned int lnap : 1;
    unsigned int lgn : 1;
    unsigned int lpanu : 1;
    /* Not all combinations are valid! */
    unsigned int reserved7 : 1;
    unsigned int reserved6 : 1;
    unsigned int reserved5 : 1;
    unsigned int persist : 1; /* reserved for future use */
    unsigned int on_demand : 1; /* reserved for future use */
    unsigned int no_switch : 1;
    unsigned int single_user : 1;
    unsigned int accept_in : 1; /* only for passive */
} BNEP_CONNECT_REQ_FLAGS;
#endif
typedef struct BNEP_CONNECT_REQ_T
{
    uint16 type;
#ifdef BTCHIP
    BNEP_CONNECT_REQ_FLAGS flags;
#else
    uint16 flags;
#endif
    ETHER_ADDR rem_addr; /* set msw to ETHER_UNKNOWN for passive */
} BNEP_CONNECT_REQ_T;

enum BNEP_CONNECT_CFM_STATUS
{
    BNEP_CONNECT_CFM_STATUS_SUCCESS,
    BNEP_CONNECT_CFM_STATUS_BADARG,
    BNEP_CONNECT_CFM_STATUS_NOMEM,
    BNEP_CONNECT_CFM_STATUS_CONFLICT,
    BNEP_CONNECT_CFM_STATUS_TRYLATER
};
typedef struct BNEP_CONNECT_CFM_T
{
    uint16 type;
    enum BNEP_CONNECT_CFM_STATUS status;
} BNEP_CONNECT_CFM_T;

enum BNEP_CONNECT_IND_STATUS
{
    /*
     * Some of these must match the L2CA_CONNECTION codes in l2cap_prim.h.
     */
    BNEP_CONNECT_IND_STATUS_SUCCESSFUL_OUT = 0x0000, /* must match */
    BNEP_CONNECT_IND_STATUS_REJ_PSM = 0x0002, /* must match */
    BNEP_CONNECT_IND_STATUS_REJ_SECURITY = 0x0003, /* must match */
    BNEP_CONNECT_IND_STATUS_REJ_RESOURCES = 0x0004, /* must match */
    BNEP_CONNECT_IND_STATUS_NOT_MASTER = (int) 0xdda5,
    BNEP_CONNECT_IND_STATUS_ABEND = (int) 0xddab,
    BNEP_CONNECT_IND_STATUS_REJ_ROLES = (int) 0xddb2, /* HERE: pack in reason? */
    BNEP_CONNECT_IND_STATUS_REJ_NOT_READY = (int) 0xdddd, /* must match */
    BNEP_CONNECT_IND_STATUS_FAILED = (int) 0xddde, /* must match */
    BNEP_CONNECT_IND_STATUS_TIMEOUT = (int) 0xeeee, /* must match */
    BNEP_CONNECT_IND_STATUS_SUCCESSFUL_IN = (int) 0xffff
};
typedef struct BNEP_CONNECT_IND_T
{
    uint16 type;
    enum BNEP_CONNECT_IND_STATUS status;
    uint16 id;
    ETHER_ADDR rem_addr;
    uint16 rem_uuid;
    uint16 loc_uuid;
} BNEP_CONNECT_IND_T;

#ifndef BLUELAB
typedef struct BNEP_LISTEN_REQ_T
{
    uint16 type;
    phandle_t phandle;
    uint16 ether_type;
    ETHER_ADDR addr;
} BNEP_LISTEN_REQ_T;
#endif

#ifndef BLUELAB
typedef struct BNEP_IGNORE_REQ_T
{
    uint16 type;
    uint16 ether_type;
    ETHER_ADDR addr;
} BNEP_IGNORE_REQ_T;
#endif

#ifndef BLUELAB
typedef struct BNEP_DATA_REQ_T
{
    uint16 type;
    uint16 ether_type;
    ETHER_ADDR rem_addr; /* HERE: this could be multicast -- probably get BNEP to transform into a BNEP_EXTENDED_DATA_REQ if not PANU */
    MBLK_T *mblk;
} BNEP_DATA_REQ_T;
#endif

#ifndef BLUELAB
typedef struct BNEP_EXTENDED_DATA_REQ_T
{
    uint16 type;
    uint16 id; /* HERE: if 0 then to local stack, 1 to go to dst_addr */
    uint16 ether_type;
    ETHER_ADDR dst_addr; /* typically a PANU's address, but allow future exp */
    ETHER_ADDR src_addr;
    MBLK_T *mblk;
} BNEP_EXTENDED_DATA_REQ_T;
#endif

#ifndef BLUELAB
typedef struct BNEP_EXTENDED_MULTICAST_DATA_REQ_T
{
    uint16 type;
    uint16 id_not; /* HERE: if 0 then not to local stack, 1 to go to all */
    uint16 ether_type;
    ETHER_ADDR dst_addr;
    ETHER_ADDR src_addr;
    MBLK_T *mblk;
} BNEP_EXTENDED_MULTICAST_DATA_REQ_T;
#endif

#ifndef BLUELAB
typedef struct BNEP_DATA_IND_T
{
    uint16 type;
    phandle_t phandle;
    MBLK_T *mblk;
} BNEP_DATA_IND_T;
typedef BNEP_DATA_IND_T BNEP_MULTICAST_DATA_IND_T;
typedef BNEP_DATA_IND_T BNEP_LOOPBACK_DATA_IND_T;
#endif

#ifndef BLUELAB
typedef struct BNEP_EXTENDED_DATA_IND_T
{
    uint16 type;
    uint16 id; /* HERE: if 0 then from local stack */
    uint16 ether_type;
    ETHER_ADDR dst_addr; /* note may be multicast */
    ETHER_ADDR src_addr; /* should be a PANU's address, but who knows? */
    MBLK_T *mblk;
} BNEP_EXTENDED_DATA_IND_T;
#endif

#ifndef BLUELAB
typedef struct BNEP_UP_IND_T
{
    uint16 type;
    phandle_t phandle;
    uint16 rem_uuid;
    uint16 loc_uuid;
} BNEP_UP_IND_T;
#endif

#ifndef BLUELAB
typedef struct BNEP_DOWN_IND_T /* This must be compatible with BNEP_UP_IND_T */
{
    uint16 type;
    phandle_t phandle;
} BNEP_DOWN_IND_T;
#endif

typedef struct BNEP_DISCONNECT_REQ_T
{
    uint16 type;
    uint16 flags;
    uint16 id;
} BNEP_DISCONNECT_REQ_T;

enum BNEP_DISCONNECT_IND_REASON
{
    /* 0-255 reserved in case one day we get HCI reason codes from L2CAP */
    BNEP_DISCONNECT_IND_REASON_ABEND = 0x0100,
    BNEP_DISCONNECT_IND_REASON_DONE = 0x0101,
    BNEP_DISCONNECT_IND_REASON_NOT_MASTER = 0x0102
};
typedef struct BNEP_DISCONNECT_IND_T
{
    uint16 type;
    uint16 id;
    enum BNEP_DISCONNECT_IND_REASON reason;
} BNEP_DISCONNECT_IND_T;

#ifndef BLUELAB
typedef struct BNEP_DEBUG_REQ_T
{
    uint16 type;
    uint16 id;
    uint16 test;
    uint16 args[18];
} BNEP_DEBUG_REQ_T;
#endif

#ifndef BLUELAB
typedef struct BNEP_DEBUG_IND_T
{
    uint16 type;
    uint16 id;
    uint16 test;
    uint16 args[18];
} BNEP_DEBUG_IND_T;
#endif

#ifndef BLUELAB
typedef struct BNEP_FLOW_REQ_T
{
    uint16 type;
} BNEP_FLOW_REQ_T;
#define PAN_FLOW_REQ_T BNEP_FLOW_REQ_T
#endif

#ifndef BLUELAB
typedef struct BNEP_FLOW_IND_T
{
    uint16 type;
    uint16 free;
} BNEP_FLOW_IND_T;
#define PAN_FLOW_IND_T BNEP_FLOW_IND_T
#endif

typedef struct BNEP_HCI_SWITCH_ROLE_RSP_T /* DM_HCI_SWITCH_ROLE_COMPLETE_T */
{
    uint16 type;
    phandle_t phandle; /* unused */
    BD_ADDR_T bd_addr;
    uint8 status;
    uint8 role;
} BNEP_HCI_SWITCH_ROLE_RSP_T;

typedef struct BNEP_ROLE_CHANGE_IND_T
{
    uint16 type;
    uint16 rem_uuid;
    uint16 loc_uuid;
} BNEP_ROLE_CHANGE_IND_T;

#define BNEP_BM_IND_MAX_NONI 6
typedef struct BNEP_BM_IND_NODE
{
    unsigned int is_bm : 1;
    unsigned int has_link_key : 1;
    ETHER_ADDR rem_addr;
    uint8 link_key[SIZE_LINK_KEY]; /* only present if has_link_key */
} BNEP_BM_IND_NODE;
typedef struct BNEP_BM_IND_T
{
    uint16 type;
    BNEP_BM_IND_NODE *node[BNEP_BM_IND_MAX_NONI]; /* 1st, if present, is primary BM; if not present it's a session end */
} BNEP_BM_IND_T;

typedef union BNEP_UPRIM_T
{
    uint16 type;
    /* HERE: put all the primitives in here */
} BNEP_UPRIM_T;


/******************************************************************************

    #defines and macros for Ethernet addressing

******************************************************************************/

#define ETHER_ADDR_LEN 6 /* in octets */
#define ETHER_MTU 1500 /* excluding header, CRC and any 802.1Q header */
#define ETHER_LOOPBACK 0x1bac /* First 16 bits -- note this is locally-assigned and multicast to ensure it won't be in use on the network */
#define ETHER_UNKNOWN 0x0b0f /* Ditto */
#define ETHER_MULTICAST_IPV4 0x01005e /* First 24 bits, and then a zero bit */
#define ETHER_MULTICAST_IPV6 0x3333 /* First 16 bits */
#define ETHER_BROADCAST_IPV4 0xffff /* Replicated across all 48 bits */
#define ETHER_TYPE_IPV4 0x0800
#define ETHER_TYPE_ARP 0x0806
#define ETHER_TYPE_8021Q 0x8100
#define ETHER_TYPE_IPV6 0x86dd
#define ETHER_IS_LOOPBACK(x) ((x)->w[0] == ETHER_LOOPBACK)
#define ETHER_IS_UNKNOWN(x) ((x)->w[0] == ETHER_UNKNOWN)
#define ETHER_IS_BROADCAST(x) (( \
                                ~(x)->w[0] | \
                                ~(x)->w[1] | \
                                ~(x)->w[2] \
                               ) == 0)
#define ETHER_IS_MULTICAST(x) (((x)->w[0] & 0x0100) != 0)
#define ETHER_IS_SAME_ADDR(x,y) (( \
                                 ((x)->w[0] ^ (y)->w[0]) | \
                                 ((x)->w[1] ^ (y)->w[1]) | \
                                 ((x)->w[2] ^ (y)->w[2]) \
                               ) == 0)


#endif /* BNEP_H */
