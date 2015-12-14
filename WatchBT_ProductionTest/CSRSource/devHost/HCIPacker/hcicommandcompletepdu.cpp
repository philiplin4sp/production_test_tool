///////////////////////////////////////////////////////////////////////////////
//
//  NAME:     hcicommandcompletepdu.cpp
//
//  PURPOSE:  define HCICommandCompletePDU clas.
//
///////////////////////////////////////////////////////////////////////////////

#include "hcicommandcompletepdu.h"
#include "app/bluestack/bluetooth.h"
#include "app/bluestack/hci.h"
#include <cassert>

#ifdef _WIN32
#include <malloc.h>
#endif

HCICommandCompletePDU::HCICommandCompletePDU(uint16 opcode,uint16 pduSize) :HCIEventPDU(HCI_EV_COMMAND_COMPLETE,pduSize)
{
    set_op_code(opcode) ;
}

HCICommandCompletePDU::HCICommandCompletePDU ( const uint8 * data , uint32 len ) : HCIEventPDU ( data , len )
{
}
		
HCICommandCompletePDU::HCICommandCompletePDU ( const PDU& s ) : HCIEventPDU ( s )
{
}

uint8 HCICommandCompletePDU::get_num_hci_command_pkts () const
{
    return get_uint8 ( 2 );
}

uint16 HCICommandCompletePDU::get_op_code () const
{
    return get_uint16(3) ;
} 

void HCICommandCompletePDU::set_op_code ( uint16 opcode )
{
    set_uint16(3,opcode) ;
} 

uint8 HCICommandCompletePDU::get_status () const
{
    return get_uint8(5) ;
}

bool HCICommandCompletePDU::decompose(uint32 * toFill, BadPDUReason& why_failed, uint32 &length) const
{
#ifdef _WIN32
    /*
     * WARNING: _msize is only documentented to work using malloc, calloc, and realloc. It is not documented to work on new or new[] 
     *          but still works on Visual Studio 6. Linux should use valgrind if there are problems.
     *
     * Check that toFill[0] <= size of the allocated block.
	 */
	assert(toFill == 0 || _msize(toFill) >= toFill[0]);       
#endif

    if ( toFill )
    {
        toFill[1] = this->get_event_code();
        toFill[2] = this->get_num_hci_command_pkts();
        toFill[3] = this->get_op_code();
        toFill[4] = this->get_status();
    }
    //  start after the length field, event code, status and opcode.
    length = 5;
    
    return true;
}

