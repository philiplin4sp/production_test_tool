#include <cassert>
#include "hcieventpdu.h"

HCIEventPDU::HCIEventPDU(uint8 eventcode,uint16 pduSize) : HCIPDU(PDU::hciCommand,pduSize) 
{
    set_event_code(eventcode) ;
    set_parameter_length() ;
}

HCIEventPDU::HCIEventPDU( const uint8 * data , uint32 len ) : HCIPDU( PDU::hciCommand , data , len )
{
}

HCIEventPDU::HCIEventPDU ( const PDU& a ) : HCIPDU ( a )
{
}
		
uint8 HCIEventPDU::get_event_code() const
{
    /* the event code is the first thing in the pdu */
    return get_uint8(0) ;
}

uint8 HCIEventPDU::get_parameter_length() const
{
    return get_uint8(1) ;
}

bool HCIEventPDU::size_ok() const
{
    /* sanity check the parameter length */
    return size() == 2u + get_parameter_length();
}

void HCIEventPDU::set_event_code(uint8 code)
{
    set_uint8(0,code) ;	
}

void HCIEventPDU::set_parameter_length()
{
    /* the first 2 bytes in an event are header */
    set_uint8(1,size()-2) ;	
}

