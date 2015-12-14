#ifndef _HCI_EVENTPDU_H
#define _HCI_EVENTPDU_H

#include "hcipdu.h"

/*
	An HCIEventPDU represents an HCI Event
	In practice, this is further subclassed but
	all events have an eventcode and a parameter length
*/

class HCIEventPDU : public HCIPDU
{
public:
    HCIEventPDU ( const PDU& ); // smart pointer cast
    HCIEventPDU( const uint8 * data ,uint32 len ) ;
    uint8 get_event_code() const;
    uint8 get_parameter_length() const;
    bool size_ok() const;  // size() == get_parameter_length() + 2
protected:
    HCIEventPDU(uint8 eventcode,uint16 pduSize) ;
    void set_event_code(uint8 code) ;
    void set_parameter_length() ;
} ;

#include "hcipacker/gen_evt.h" 
#include "hcipacker/xtraeventpdus.h" 
#endif
