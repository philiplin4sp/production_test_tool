/******************************************************************************

    file tcp.h  --  Transmission Control Protocol

    Copyright (c) 2002-2005 CSR Ltd.

    $Log: tcp.h,v $
    Revision 1.8  2003/01/08 15:44:04  mgr
    Add out_size to TCP_OPEN_CFM.
    Remove len from on-chip TCP_DATA_REQ.
    Add TCP_VMDATA_IND_T.

    Revision 1.7  2003/01/07 18:11:56  bs01
    Merge changes from jbs-pan-buffer-branch back to main line. Eliminate use of BUFFER_HANDLE in VM pan interface in favour of sources and sinks.

    Revision 1.6.24.1  2002/12/20 15:48:00  bs01
    First attempt at eliminating BUFFER_HANDLE from tcp.h

    Revision 1.6  2002/11/13 16:55:16  bs01
    Get sdklib pan library building; move in.h and ipv4.h from dev/src/csr/pan into interface/app/pan. Fixup includes and other mistakes in devHost/sdklib/pan

    Revision 1.5  2002/11/04 14:46:37  bs01
    Add off-chip variants of primitives controlled by ifdefs. Extend script which produces BlueLab version of the header to understand nested ifdefs.

    Revision 1.4  2002/10/30 15:03:58  bs01
    Drop TCP DATA primitives in the BlueLab version of tcp.h

    Revision 1.3  2002/10/18 16:48:12  mgr
    Implement new NAP/GN-supporting BNEP API.

    Revision 1.2  2002/09/10 15:47:23  bs01
    Merge changes between jbs-pan-branch-start and jbs-pan-branch-mark-a onto the main line. Move external pan headers to interface/app/pan directory.

    Revision 1.1.2.4  2002/09/04 09:59:14  bs01
    tcp_hcked and bnep_hcked moving closer to tcp and bnep headers. Autogen stuff updated to cope.

    Revision 1.1.2.3  2002/09/02 15:02:50  bs01
    Pull all external headers used by pan into a single pantypes.h.

    Revision 1.1.2.2  2002/09/02 13:56:46  bs01
    Fold bnep_prim.h and tcp_prim.h into bnep.h and tcp.h respectively; they weren't different enough to be worth keeping.

    Revision 1.1.2.1  2002/09/02 12:37:27  bs01
    Move pan headers and support from interface/app/bluestack and dev/src/csr/pan into interface/app/pan. Update dev side to cope.

    Revision 1.3  2002/08/20 17:43:21  mgr
    Add flags to TCP_OPEN_REQ.
    TCP_CLOSED_IND now uses b_out rather than b_in as the identifier.
    Allow fromapp primitives (other than TCP_DATA_REQ when from off-chip)
    to use either b_in or b_out as the identifier.
    Move TCP_ABORT_RSP flags to increase consistency.
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


#ifndef TCP_H
#define TCP_H


#include "pantypes.h"
#include "ip.h"


/******************************************************************************

    enum TCP_MSG_TYPE  --  TCP task message types
    typedefs for TCP task messages
    enums for TCP task message parameters

******************************************************************************/

#define TCP_PRIM_BASE 0x0200

#define ENUM_SEP_TCP_FIRST_PRIM TCP_PRIM_BASE
#define ENUM_SEP_TCP_LAST_PRIM (TCP_PRIM_BASE + 16)

enum TCP_MSG_TYPE
{
    TCP_OPEN_REQ = TCP_PRIM_BASE + 1,
    TCP_OPEN_CFM,
    TCP_OPEN_IND,
    TCP_OPEN_RSP,
#ifndef BLUELAB
    TCP_DATA_REQ,
    TCP_DATA_CFM,
    TCP_DATA_IND,
    TCP_DATA_RSP,
#endif
    TCP_CLOSE_REQ = TCP_PRIM_BASE + 9,
    TCP_CLOSE_IND,
    TCP_CLOSED_IND,
    TCP_CLOSED_RSP,
    TCP_ABORT_REQ,
    TCP_ABORT_IND,
    TCP_ABORT_RSP
};

typedef struct TCP_OPEN_REQ_T
{
    uint16 type;
    phandle_t phandle;
    uint16 flags; /* auto-accept, no b_in, don't recycle, do/don't timeout */
    IP_ADDR rem_addr; /* May be unspecified */
    uint16 rem_port; /* May be IN_PORT_ANY */
    uint16 out_size; /* May not be zero */
    uint16 loc_port; /* May be IN_PORT_ANY */
    uint16 in_size; /* May not be zero */
} TCP_OPEN_REQ_T;

enum TCP_OPEN_CFM_STATUS
{
    TCP_OPEN_CFM_STATUS_SUCCESS,
    TCP_OPEN_CFM_STATUS_BADARG,
    TCP_OPEN_CFM_STATUS_NOMEM,
    TCP_OPEN_CFM_STATUS_NOLOCADDR
    /* Note that bit 15 is reserved for covert VM signalling */
};
typedef struct TCP_OPEN_CFM_T
{
    uint16 type;
    phandle_t phandle;
    enum TCP_OPEN_CFM_STATUS status;
#ifdef BLUELAB
    Source source;
    Sink sink;
#else
    BUFFER_HANDLE b_in;
    BUFFER_HANDLE b_out;
#endif
    uint16 out_size;
} TCP_OPEN_CFM_T;

enum TCP_OPEN_IND_STATUS
{
    TCP_OPEN_IND_STATUS_ACCEPT,
    TCP_OPEN_IND_STATUS_REJECT
};
typedef struct TCP_OPEN_IND_T
{
    uint16 type;
    phandle_t phandle;
#ifdef BLUELAB
    Sink sink;
#else
    BUFFER_HANDLE b_out;
#endif
    enum TCP_OPEN_IND_STATUS status;
    IP_ADDR rem_addr;
    uint16 rem_port;
} TCP_OPEN_IND_T;

enum TCP_OPEN_RSP_STATUS
{
    TCP_OPEN_RSP_STATUS_ACCEPT,
    TCP_OPEN_RSP_STATUS_REJECT
};
typedef struct TCP_OPEN_RSP_T
{
    uint16 type;
#ifdef BLUELAB
    Sink sink;
#else
    BUFFER_HANDLE b;
#endif
    enum TCP_OPEN_RSP_STATUS status;
} TCP_OPEN_RSP_T;

#ifndef BLUELAB
typedef struct TCP_DATA_REQ_T
{
    uint16 type;
#ifdef __XAP__
    BUFFER_HANDLE b;
#else
    BUFFER_HANDLE b_out;
    uint16 len;
    uint8 *data;
#endif
} TCP_DATA_REQ_T;

typedef struct TCP_DATA_CFM_T
{
    uint16 type;
    phandle_t phandle;
    BUFFER_HANDLE b_out;
    uint16 len;
} TCP_DATA_CFM_T;

typedef struct TCP_DATA_IND_T
{
    uint16 type;
    phandle_t phandle;
#ifdef __XAP__
    MBLK_T *mblk; 
#else
    BUFFER_HANDLE b_out;
    uint16 len;
    uint8 *data;
#endif
} TCP_DATA_IND_T;

#ifdef __XAP__
typedef struct TCP_VMDATA_IND_T /* see tcp.c for more details... */
{
    uint16 type;
    BUFFER_HANDLE b_in;
} TCP_VMDATA_IND_T;
#endif

typedef struct TCP_DATA_RSP_T
{
    uint16 type;
    BUFFER_HANDLE b;
    uint16 len;
} TCP_DATA_RSP_T;
#endif /* !BLUELAB */

typedef struct TCP_CLOSE_REQ_T
{
    uint16 type;
#ifdef BLUELAB
    Sink sink;
#else
    BUFFER_HANDLE b;
#endif
} TCP_CLOSE_REQ_T;

typedef struct TCP_CLOSE_IND_T
{
    uint16 type;
    phandle_t phandle;
#ifdef BLUELAB
    Sink sink;
#else
    BUFFER_HANDLE b_out;
#endif
} TCP_CLOSE_IND_T;

typedef struct TCP_CLOSED_IND_T
{
    uint16 type;
    phandle_t phandle;
#ifdef BLUELAB
    Sink sink;
#else
    BUFFER_HANDLE b_out;
#endif
} TCP_CLOSED_IND_T;

typedef struct TCP_CLOSED_RSP_T
{
    uint16 type;
#ifdef BLUELAB
    Sink sink;
#else
    BUFFER_HANDLE b;
#endif
} TCP_CLOSED_RSP_T;

typedef struct TCP_ABORT_REQ_T
{
    uint16 type;
#ifdef BLUELAB
    Sink sink;
#else
    BUFFER_HANDLE b;
#endif
} TCP_ABORT_REQ_T;

typedef struct TCP_ABORT_IND_T
{
    uint16 type;
    phandle_t phandle;
#ifdef BLUELAB
    Sink sink;
#else
    BUFFER_HANDLE b_out;
#endif
} TCP_ABORT_IND_T;

enum TCP_ABORT_RSP_FLAGS
{
    TCP_ABORT_RSP_FLAGS_COMPLETE,
    TCP_ABORT_RSP_FLAGS_BACKTOLISTEN
};
typedef struct TCP_ABORT_RSP_T
{
    uint16 type;
#ifdef BLUELAB
    Sink sink;
#else
    BUFFER_HANDLE b;
#endif
    enum TCP_ABORT_RSP_FLAGS flags;
} TCP_ABORT_RSP_T;


#endif /* TCP_H */
