#include "hciacl_l2cap_pdu.h"

HCIACL_L2CAP_PDU::HCIACL_L2CAP_PDU(const PDU &from) :
    HCIACLPDU(from),
    L2CAP_HEADER_SIZE(4)
{

}

const uint8 *HCIACL_L2CAP_PDU::get_l2capDataPtr() const
{
    if (get_pbf() == HCIACLPDU::cont || get_pbf() == HCIACLPDU::pbf_reserved1)
    {
        return get_dataPtr();
    }
    else
    {
        return get_dataPtr() + L2CAP_HEADER_SIZE;
    }
}

uint16 HCIACL_L2CAP_PDU::get_l2capLength() const
{
    if (get_pbf() == HCIACLPDU::cont || get_pbf() == HCIACLPDU::pbf_reserved1)
    {
        return get_length();
    }
    else
    {
        return get_length() - L2CAP_HEADER_SIZE;
    }
}

ExtendedHCIACLPDU_Helper::ExtendedHCIACLPDU_Helper(uint16 length) :
    HCIACLPDU(length)
{
}

ExtendedHCIACLPDU_Helper::ExtendedHCIACLPDU_Helper(const PDU &from) :
    HCIACLPDU(from)
{
}

std::deque<HCIACLPDU> ExtendedHCIACLPDU_Helper::split(uint16 splitSize) const
{
    uint16 lLength = get_length();
    uint16 packetSize;
    std::deque<HCIACLPDU> pdus;
    const uint8* dataIndex = get_dataPtr();
    HCIACLPDU::packet_boundary_flag flag = get_pbf();

    while ( lLength != 0)
    {
        //  For ACL fill in the boundary and broadcast info.
        if (lLength > splitSize)
            packetSize = splitSize;
        else
            packetSize = lLength;
        HCIACLPDU pdu( packetSize );
        pdu.set_header(get_handle(), flag, get_bct());
        pdu.set_data(dataIndex, packetSize);
        pdu.set_length();
        lLength -= packetSize;
        dataIndex += packetSize;
        pdus.push_back(pdu);
        //  After the first (sub-)packet, the rest aren't boundaries any more...
        flag = HCIACLPDU::cont;
    }
    return pdus;
}

HCIACL_L2CAP_PDU_Helper::HCIACL_L2CAP_PDU_Helper(uint16 length) :
    ExtendedHCIACLPDU_Helper(length + 4)
{
    set_uint16(4, length);
}

HCIACL_L2CAP_PDU_Helper::HCIACL_L2CAP_PDU_Helper(const PDU &from) :
    ExtendedHCIACLPDU_Helper(from)
{

}

void HCIACL_L2CAP_PDU_Helper::set_l2capChNum(uint16 l2capChNum)
{
    set_uint16(6, l2capChNum);
}

uint16 HCIACL_L2CAP_PDU_Helper::get_l2capChNum() const
{
    return get_uint16(6);
}

uint16 HCIACL_L2CAP_PDU_Helper::get_l2capLength() const
{
    return get_uint16(4);
}

void HCIACL_L2CAP_PDU_Helper::set_l2caplength(uint16 l2caplen)
{
    set_uint16(4, l2caplen);
}

void HCIACL_L2CAP_PDU_Helper::set_l2capData(const uint8 *from, uint16 size)
{
    set_uint8Array(from, 8, size);
}
