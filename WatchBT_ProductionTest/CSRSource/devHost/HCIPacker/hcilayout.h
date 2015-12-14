///////////////////////////////////////////////////////////////////////////////
//  Define layout format for Bluetooth HCI messages.
///////////////////////////////////////////////////////////////////////////////

#ifndef __HCILAYOUT_H__
#define __HCILAYOUT_H__

#define OFFSET_uint8(name) name
#define OFFSET_int8(name) name
#define OFFSET_uint16(name) name,name##pad0
#define OFFSET_uint24(name) name,name##pad0,name##pad1
#define OFFSET_uint32(name) name,name##pad0,name##pad1,name##pad2
#define OFFSET_BluetoothDeviceAddress(name) name,name##pad0,name##pad1,name##pad2,name##pad3,name##pad4
#define OFFSET_EventMask(name) name,name##pad0,name##pad1,name##pad2,name##pad3,name##pad4,name##pad5,name##pad6

// there are no pointers in HCI, but...
#define OFFSET_PtrArray(name,ptrcount) name,name##_end=name+(2*ptrcount)-1
#define OFFSET_uint8_array(name,count) name,name##_end=(name + count - 1)

#endif//__HCILAYOUT_H__
