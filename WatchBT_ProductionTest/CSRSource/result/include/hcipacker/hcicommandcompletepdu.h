#ifndef _HCICOMMANDCOMPLETE_H
#define _HCICOMMANDCOMPLETE_H

#include "hcieventpdu.h"
#include "legacy.h"

/* 
	an HCICommandCompletePDU corresponds to an command complete.
	These are further subclassed into particular kinds
*/

class HCICommandCompletePDU : public HCIEventPDU
{
public:
    HCICommandCompletePDU ( const PDU& );
    HCICommandCompletePDU ( const uint8 * data , uint32 len );
    uint8 get_num_hci_command_pkts () const;
    uint16 get_op_code () const;
    uint8 get_status () const;
    virtual bool decompose(uint32 * toFill, BadPDUReason& why_failed, uint32 &length) const;
    
protected:
    HCICommandCompletePDU ( uint16 opcode , uint16 pduSize );
    void set_op_code ( uint16 opcode );
} ;

#include "hcipacker/gen_cmd_cmplt.h"
#include "hcipacker/xtracommandcompletepdus.h" 

#endif /* _HCICOMMANDCOMPLETE_H */
