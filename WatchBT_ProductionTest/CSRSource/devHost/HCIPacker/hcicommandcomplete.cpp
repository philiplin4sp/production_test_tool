#include "hcicommandcomplete.h"
#include "bluestackheaders.h" 

HCICommandComplete::HCICommandCompletePDU(uint16 opcode,uint16 pduSize) :HCIEventPDU(HCI_EV_COMMAND_COMPLETE,pduSize)
{
	setOpcode(opcode) ;
}
		
uint16 HCICommandComplete::getOpcode(void)
{
	return get_uint16_t(3) ;
} 

void HCICommandComplete::setOpcode(uint16_t opcode)
{
	set_uint16_t(3,opcode) ;
} 

