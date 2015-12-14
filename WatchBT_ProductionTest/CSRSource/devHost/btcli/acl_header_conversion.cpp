#include "acl_header_conversion.h"

PBF conv (HCIACLPDU::packet_boundary_flag x)
{
    switch ( x )
    {
    case HCIACLPDU::start:
        return pbf_start;
    case HCIACLPDU::cont:
        return pbf_continuation;
    case HCIACLPDU::start_not_auto_flush:
        return pbf_start_not_auto_flush;
    default:
        //  if we don't recognise it pass it through.
        return (PBF)x;
    };
}

BF conv ( HCIACLPDU::broadcast_type x )
{
    switch (x)
    {
    case HCIACLPDU::ptp:
        return bf_p2p;
    case HCIACLPDU::act:
        return bf_active;
    case HCIACLPDU::pic:
        return bf_piconet;
    case HCIACLPDU::bct_reserved:
        return bf_unused;
    default:
        return (BF)x;
    }
}

HCIACLPDU::packet_boundary_flag conv (PBF x)
{
    switch ( x )
    {
    case pbf_start:
        return HCIACLPDU::start;
    case pbf_continuation:
        return HCIACLPDU::cont;
    case pbf_start_not_auto_flush:
        return HCIACLPDU::start_not_auto_flush;
    default:
        return (HCIACLPDU::packet_boundary_flag)x;
    };
}

HCIACLPDU::broadcast_type conv ( BF x )
{
    switch (x)
    {
    case bf_p2p:
        return HCIACLPDU::ptp;
    case bf_active:
        return HCIACLPDU::act;
    case bf_piconet:
        return HCIACLPDU::pic;
    case bf_unused:
        return HCIACLPDU::bct_reserved;
    default:
        return (HCIACLPDU::broadcast_type)x;
    }
}
