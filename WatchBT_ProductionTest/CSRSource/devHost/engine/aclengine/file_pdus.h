#ifndef FILE_PDUS_H
#define FILE_PDUS_H

#include "hciacl_l2cap_pdu.h"
#include "control_type.h"
#include "channel_enum.h"

class StartStartPdu : public HCIACL_L2CAP_PDU
{
public:
    StartStartPdu(const PDU &pdu) : HCIACL_L2CAP_PDU(pdu) {}
    virtual ~StartStartPdu(){}

    uint32 getFileSize()
    {
        return get_uint32(9);
    }

    void setFileSize(uint32 fileSize)
    {
        set_uint32(9, fileSize);
    }

    std::string getFileName()
    {
        return std::string((const char *) get_l2capDataPtr() + 4, get_l2capLength() - 4);
    }
};

class StartContPdu : public HCIACL_L2CAP_PDU
{
public:
    StartContPdu(const PDU &pdu) : HCIACL_L2CAP_PDU(pdu) {}
    virtual ~StartContPdu() {}

    std::string getFileName()
    {
        return std::string((const char *) get_l2capDataPtr(), get_l2capLength());
    }
};

class ControlHelper : public HCIACL_L2CAP_PDU_Helper
{
public:
    ControlHelper(const PDU &from) : HCIACL_L2CAP_PDU_Helper(from) {}
    ControlHelper(ControlType controlType, uint16 length) :
        HCIACL_L2CAP_PDU_Helper(length + 1)
    {
        set_l2capChNum(CONTROL_CHANNEL);
        set_uint8(8, controlType);
    }

    ControlType get_ControlType()
    {
        return (ControlType) get_uint8(8);
    }
};

class StartHelper : public ControlHelper
{
public:
    StartHelper(uint32 fileSize, const std::string &fileName) :
        ControlHelper(C_START, 4 + fileName.length())  
    {
        set_uint32(9, fileSize);
        set_uint8Array((uint8 *) fileName.c_str(), 13, fileName.length());
    }
};

#ifdef SEND_START_ACK_ENABLED
class StartAckHelper : public ControlHelper
{
public:
    StartAckHelper() : ControlHelper(START_ACK, 0) {}
    StartAckHelper(const PDU &from) : ControlHelper(from) {}
};
#endif

class StartNakHelper : public ControlHelper
{
public:
    StartNakHelper() : ControlHelper(START_NAK, 0) {}
    StartNakHelper(const PDU &from) : ControlHelper(from) {}
};

class FinishHelper : public ControlHelper
{
public:
    FinishHelper(uint32 fileSize) : ControlHelper(FINISH, 4) {set_uint32(9, fileSize);}
    FinishHelper(const PDU &from) : ControlHelper(from) {}
    uint32 getFileSize() {return get_uint32(9);}
};

class FinishAckHelper : public ControlHelper
{
public:
    FinishAckHelper() : ControlHelper(FINISH_ACK, 0) {}
    FinishAckHelper(const PDU &from) : ControlHelper(from) {}
};

class CancelReceiveHelper : public ControlHelper
{
public:
    CancelReceiveHelper() : ControlHelper(CANCEL_RECEIVE, 0) {}
    CancelReceiveHelper(const PDU &from) : ControlHelper(from) {}
};

class CancelSendHelper : public ControlHelper
{
public:
    CancelSendHelper() : ControlHelper(CANCEL_SEND, 0) {}
    CancelSendHelper(const PDU &from) : ControlHelper(from) {}
};

#endif
