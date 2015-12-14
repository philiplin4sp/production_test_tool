/*
 * Copyright CSR 2008
 *
 * $Id: //depot/bc/bluesuite_2_4/devHost/engine/fastpipe_engine/flow_control_pdu.h#1 $
 *
 * Purpose: FlowControlPdu packs and unpacks a FastPipe flow control pdu, with
 *          token_size stored MSB first.
 *
 */

#ifndef FLOW_CONTROL_PDU_H
#define FLOW_CONTROL_PDU_H

#include "hcipacker/hcidatapdu.h"
#include "common/types.h"

class PDU;

class FlowControlPdu : public HCIACLPDU
{
public:
    FlowControlPdu();
    FlowControlPdu(const PDU &);
    void set_pipe(uint8);
    uint8 get_pipe(void);
    void set_token_size(uint16);
    uint16 get_token_size(void);
};

#endif

