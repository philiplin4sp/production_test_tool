#include <cassert>
#include "hcicommandpdu.h"


/*
	The constructor requires us to specify the opcode and pdusize
	We set up the opcode and parameter length correctly from these
*/

HCICommandPDU::HCICommandPDU(uint16 opcode,uint16 pduSize)
:   HCIPDU(PDU::hciCommand,pduSize) 
{
    set_op_code(opcode) ;
    set_parameter_length() ;
}

HCICommandPDU::HCICommandPDU( const PDU& from )
:   HCIPDU( from )
{
}
		
HCICommandPDU::HCICommandPDU( const uint8 * data , uint32 len )
:   HCIPDU( PDU::hciCommand , data , len )
{
}
		
/*
	read the HCI opcode field
*/
uint16 HCICommandPDU::get_op_code(void) const
{
    return get_uint16(0) ;
}

void HCICommandPDU::set_op_code(uint16 opcode)
{
    set_uint16(0,opcode) ;
}

/*
	Convenience function - read the OGF part of the opcode
*/

uint8 HCICommandPDU::get_OGF(void) const
{
    uint16 opcode = get_op_code() ;
    return uint8(opcode >> 10) ;
}

/*
	Convenience function - read the OCF part of the opcode
*/
uint16 HCICommandPDU::get_OCF(void) const
{
    uint16 opcode = get_op_code() ;
    return (opcode & uint16((1<<10)-1)) ;
}


/*
	read the parameterlength field of the HCI command 
*/

uint8 HCICommandPDU::get_parameter_length(void) const
{
    return get_uint8(2) ;
}

/*
	set the parameter length
*/
void HCICommandPDU::set_parameter_length()
{
    set_uint8(2,size()-3) ;
}

/*
	resize the pdu and make sure that the
	parameter length is set correctly
*/

void HCICommandPDU::resize(uint16 len)
{
    HCIPDU::resize(len) ;
    set_parameter_length() ;
}

HCICommandPDU * recast ( const HCIPDU*from , const HCICommandPDU*dummy )
{
    return 0;
}

bool HCICommandPDU::build(const uint32 * const parameters)
{
    return true;
}
