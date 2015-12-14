#ifndef __HCICOMMAND_PDU
#define __HCICOMMAND_PDU

#include "hcipdu.h"

/* 
	An HCICommandPDU represents an HCI Command 
	In practice, this is further subclassed into 
	particular commands but there is some common
	functionality (opcode and length fields).
*/

class HCICommandPDU : public HCIPDU
{
public :
    HCICommandPDU ( const PDU& ) ;
    HCICommandPDU ( const uint8 * data,uint32 length ) ;
    /* 
	    we want to be able to read the opcode in a couple of 
	    ways.  Reading the parameter length is useful too
    */
    uint16 get_op_code(void) const;
    uint8 get_OGF(void) const;
    uint16 get_OCF(void) const;
    uint8 get_parameter_length(void) const;
    /* legacy api build method */
    virtual bool build(const uint32 * const parameters);

protected :
    HCICommandPDU ( uint16 opcode , uint16 pduSize ) ;
    void set_op_code(uint16 opcode) ;
    void set_parameter_length() ;
    void resize(uint16 len) ;
} ;

#include "hcipacker/gen_cmd.h"
#include "hcipacker/gen_xtrahcicommand.h"
#include "hcipacker/xtracommandpdus.h" 

#endif
