#ifndef _HCICOMMANDCOMPLETE_H
#define _HCICOMMANDCOMPLETE_H

#error DEAD FILE

#include "hcieventpdu.h"

/* 
	an HCICommandCompletePDU corresponds to an command complete.
	These are further subclassed into particular kinds
*/

class HCICommandCompletePDU : public HCIEventPDU
{
	public:
		HCICommandCompletePDU(uint16 opcode,uint16 pduSize) ;
		uint16 getOpcode(void) ;
		
	protected:
		void setOpcode(uint16 opcode) ;
} ;

#include "hcipacker/gen_cmd_cmplt.h"
#include "handwritten.h" 

#endif /* _HCICOMMANDCOMPLETE_H */

