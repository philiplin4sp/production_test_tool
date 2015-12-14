/******************************************************************************

    file tcp.h  --  Transmission Control Protocol

    Copyright (c) 2002-2005 CSR Ltd.

    $Log: tcp_hacked.h,v $
    Revision 1.3  2003/01/08 15:44:19  mgr
    Align with tcp.h.

    Revision 1.2  2002/09/10 15:47:23  bs01
    Merge changes between jbs-pan-branch-start and jbs-pan-branch-mark-a onto the main line. Move external pan headers to interface/app/pan directory.

    Revision 1.1.2.4  2002/09/10 14:06:16  bs01
    Move from IP_ADDR_T to IP_ADDR

    Revision 1.1.2.3  2002/09/10 14:06:02  bs01
    Move from IP_ADDR_T to IP_ADDR

    Revision 1.1.2.2  2002/09/04 09:59:14  bs01
    tcp_hacked and bnep_hacked moving closer to tcp and bnep headers. Autogen stuff updated to cope.

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

#define TCP_PRIM_BASE 0x0200 /*HERETODO*/
enum TCP_MSG_TYPE
{
    TCP_OPEN_REQ = TCP_PRIM_BASE + 1,
    TCP_OPEN_CFM,
    TCP_OPEN_IND,
    TCP_OPEN_RSP,
    TCP_DATA_REQ,
    TCP_DATA_CFM,
    TCP_DATA_IND,
    TCP_DATA_RSP,
    TCP_CLOSE_REQ,
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
    uint16 loc_port; /* May not be zero (IN_PORT_ANY) */
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
    uint16 status;
    BUFFER_HANDLE b_in;
    BUFFER_HANDLE b_out;
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
    BUFFER_HANDLE b_out;
    uint16 status;
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
    BUFFER_HANDLE b;
    uint16 status;
} TCP_OPEN_RSP_T;

typedef struct TCP_DATA_REQ_T
{
    uint16 type;
    BUFFER_HANDLE b_out;
    uint16 len;
    uint8 *data;
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
    BUFFER_HANDLE b_out;
    uint16 len;
    uint8 *data;
} TCP_DATA_IND_T;

typedef struct TCP_DATA_RSP_T
{
    uint16 type;
    BUFFER_HANDLE b;
    uint16 len;
} TCP_DATA_RSP_T;

typedef struct TCP_CLOSE_REQ_T
{
    uint16 type;
    BUFFER_HANDLE b;
} TCP_CLOSE_REQ_T;

typedef struct TCP_CLOSE_IND_T
{
    uint16 type;
    phandle_t phandle;
    BUFFER_HANDLE b_out;
} TCP_CLOSE_IND_T;

typedef struct TCP_CLOSED_IND_T
{
    uint16 type;
    phandle_t phandle;
    BUFFER_HANDLE b_out;
} TCP_CLOSED_IND_T;

typedef struct TCP_CLOSED_RSP_T
{
    uint16 type;
    BUFFER_HANDLE b;
} TCP_CLOSED_RSP_T;

typedef struct TCP_ABORT_REQ_T
{
    uint16 type;
    BUFFER_HANDLE b;
} TCP_ABORT_REQ_T;

typedef struct TCP_ABORT_IND_T
{
    uint16 type;
    phandle_t phandle;
    BUFFER_HANDLE b_out;
} TCP_ABORT_IND_T;

enum TCP_ABORT_RSP_FLAGS
{
    TCP_ABORT_RSP_FLAGS_COMPLETE,
    TCP_ABORT_RSP_FLAGS_BACKTOLISTEN
};
typedef struct TCP_ABORT_RSP_T
{
    uint16 type;
    BUFFER_HANDLE b;
    uint16 flags;
} TCP_ABORT_RSP_T;


#endif /* TCP_H */
