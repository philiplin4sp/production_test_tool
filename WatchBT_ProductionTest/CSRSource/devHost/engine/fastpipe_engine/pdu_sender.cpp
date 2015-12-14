/*
 * Copyright CSR 2008
 *
 * $Id: //depot/bc/bluesuite_2_4/devHost/engine/fastpipe_engine/pdu_sender.cpp#1 $
 *
 */

#include "pdu_sender.h"

bool PduSender::send(const HCIDataPDU &pdu)
{
    return send_(pdu);
}
