/*
 * Copyright CSR 2008
 *
 * $Id: //depot/bc/bluesuite_2_4/devHost/engine/fastpipe_engine/pdu_sender.h#1 $
 *
 * Purpose: PduSender is a base class for sending data somewhere e.g to a 
 *          Bluecore via BlueCoreDeviceController or to a test harness.
 *
 */

#ifndef PDU_SENDER_H
#define PDU_SENDER_H

class HCIDataPDU;

class PduSender
{
public:
    bool send(const HCIDataPDU &);
private:
    virtual bool send_(const HCIDataPDU &) = 0;
};

#endif
