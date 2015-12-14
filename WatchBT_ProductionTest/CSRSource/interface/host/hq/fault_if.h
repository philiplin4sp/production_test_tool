/*
 * fault_if.h - defines an enum of the possible fault reasons
 * 
 * Fault codes, with description strings are in the xmacro file "fault_codes.h". See
 * this file for full explanation of usage.
 */
 
#ifndef _FAULT_IF_H_
#define _FAULT_IF_H_

#define FAULT_IFACE_X(enum, hexv, sdesc, ldesc) enum = hexv
#define FAULT_IFACE_SEP  ,
enum faultid_enum{

#include "fault_codes.h"

};
#undef FAULT_IFACE_X
#undef FAULT_IFACE_SEP

typedef enum faultid_enum faultid; 

#endif /* _FAULT_IF_H_ */


