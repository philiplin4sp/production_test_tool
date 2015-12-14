/*
 * Copyright CSR 2008
 *
 * $Id: //depot/bc/bluesuite_2_4/devHost/engine/fastpipe_engine/flow_control_pdu.cpp#1 $
 *
 */

#include "flow_control_pdu.h"
#include "token_util.h"

FlowControlPdu::FlowControlPdu()
    : HCIACLPDU(TOKEN_SIZE)
{
}

FlowControlPdu::FlowControlPdu(const PDU &pdu)
    : HCIACLPDU(pdu)
{
}

void FlowControlPdu::set_pipe(uint8 pipe)
{
    set_uint8(HCIACLPDU::header_size + PIPE_OFFSET, pipe);
}

uint8 FlowControlPdu::get_pipe(void)
{
    return get_uint8(HCIACLPDU::header_size + PIPE_OFFSET);
}

void FlowControlPdu::set_token_size(uint16 size)
{
    uint8 first, second;
    decomposeCredits(size, first, second);
    set_uint8(HCIACLPDU::header_size + CREDIT_OFFSET, first);
    set_uint8(HCIACLPDU::header_size + CREDIT_OFFSET + 1, second);
}

uint16 FlowControlPdu::get_token_size(void)
{
    return composeCredits(
               get_uint8(HCIACLPDU::header_size + CREDIT_OFFSET), 
               get_uint8(HCIACLPDU::header_size + CREDIT_OFFSET + 1));
}
