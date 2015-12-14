#ifndef HCIACL_L2CAP_PDU_H
#define HCIACL_L2CAP_PDU_H

#include "hcipacker/hcidatapdu.h"
#include <deque>

class HCIACL_L2CAP_PDU : public HCIACLPDU
{
public:
    HCIACL_L2CAP_PDU(const PDU &from);
    const uint8 *get_l2capDataPtr() const;
    uint16 get_l2capLength() const;
protected:
    const uint8 L2CAP_HEADER_SIZE;
};

class ExtendedHCIACLPDU_Helper : public HCIACLPDU
{
public:
    ExtendedHCIACLPDU_Helper(uint16 length);
    ExtendedHCIACLPDU_Helper(const PDU &from);
    std::deque<HCIACLPDU> split(uint16 splitSize) const;
};

class HCIACL_L2CAP_PDU_Helper : public ExtendedHCIACLPDU_Helper
{
public:
    HCIACL_L2CAP_PDU_Helper(uint16 length);
    HCIACL_L2CAP_PDU_Helper(const PDU &from);
    void set_l2capChNum(uint16 l2capChNum);
    uint16 get_l2capChNum() const;
    uint16 get_l2capLength() const;
    void set_l2caplength(uint16 l2caplen);
    void set_l2capData(const uint8 *from, uint16 size);
};

#endif
