#ifndef ACL_HEADER_CONVERSION_H
#define ACL_HEADER_CONVERSION_H

#include "dictionaries.h"
#include "hcipacker/hcidatapdu.h"

PBF conv (HCIACLPDU::packet_boundary_flag x);
BF conv ( HCIACLPDU::broadcast_type x );
HCIACLPDU::packet_boundary_flag conv (PBF x);
HCIACLPDU::broadcast_type conv ( BF x );

#endif
