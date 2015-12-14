///////////////////////////////////////////////////////////////////////////////
//  Define layout format for Bluecore Friendly messages.
///////////////////////////////////////////////////////////////////////////////

#ifndef __BCFLAYOUT_H__
#define __BCFLAYOUT_H__

//  All BCF pdus start at the start.
#define OFFSET_START(name)
//  Execpt VM ones
#define OFFSET_VM_START(name) name##_endofheader = 3,

//  uint16 and uint32, plus BDAddr and EventMask.
#define OFFSET_uint16(name) name,name##pad1
#define OFFSET_uint32(name) name,name##pad1,name##pad2,name##pad3
#define OFFSET_BluetoothDeviceAddress(name) name,name##pad1,name##pad2,name##pad3,name##pad4,name##pad5,name##pad6,name##pad7
#define OFFSET_BluetoothDeviceAddressWithType(name) OFFSET_uint16(name),OFFSET_BluetoothDeviceAddress(name##pad_1)
#define OFFSET_EventMask(name) name,name##pad0,name##pad1,name##pad2,name##pad3,name##pad4,name##pad5,name##pad6
#define OFFSET_IPAddress(name) OFFSET_uint16(name),OFFSET_uint16(name##pad_1),OFFSET_uint16(name##pad2),OFFSET_uint16(name##pad3),OFFSET_uint16(name##pad4),OFFSET_uint16(name##pad5),OFFSET_uint16(name##pad6),OFFSET_uint16(name##pad7)
#define OFFSET_EtherNetAddress(name) OFFSET_uint16(name),OFFSET_uint16(name##pad_1),OFFSET_uint16(name##pad2)
#define OFFSET_STREAM_BUFFER_SIZES_T(name) OFFSET_uint16(name),OFFSET_uint16(name##pad_1)
#define OFFSET_DM_SM_SERVICE_T(name) OFFSET_uint16(name),OFFSET_uint16(name##pad_1)
#define OFFSET_SMKeyState(name) OFFSET_uint16_array(name,8), OFFSET_uint16_array(name##pad1,8)

//  All other types are really one of the above.
#define OFFSET_int16(name) OFFSET_uint16(name)
#define OFFSET_uint8(name) OFFSET_uint16(name)
#define OFFSET_int8(name) OFFSET_uint8(name)
#define OFFSET_bool(name) OFFSET_uint8(name)
#define OFFSET_int32(name) OFFSET_uint32(name)
#define OFFSET_uint24(name) OFFSET_uint32(name)
#define OFFSET_l2ca_conn_result_t(name) OFFSET_uint16(name)
#define OFFSET_l2ca_move_result_t(name) OFFSET_uint16(name)
#define OFFSET_l2ca_disc_result_t(name) OFFSET_uint16(name)
#define OFFSET_l2ca_conf_result_t(name) OFFSET_uint16(name)
#define OFFSET_l2ca_info_result_t(name) OFFSET_uint16(name)
#define OFFSET_l2ca_data_result_t(name) OFFSET_uint16(name)
#define OFFSET_l2ca_misc_result_t(name) OFFSET_uint16(name)
#define OFFSET_DM_SM_TRUST_T(name) OFFSET_uint16(name)
#define OFFSET_DM_SM_KEY_TYPE_T(name) OFFSET_uint16(name)
#define OFFSET_DM_SM_PERMANENT_ADDRESS_T(name) OFFSET_uint16(name)

#define OFFSET_PtrArray(name,ptrcount) name,name##_end=name+(2*ptrcount)-1

#define OFFSET_uint8_array(name,count) OFFSET_uint16_array(name,count)
#define OFFSET_uint16_array(name,count) name,name##_end=(name + 2*(count) - 1)
#define OFFSET_uint32_array(name,count) name,name##_end=(name + 4*(count) - 1)
#endif//__BCFLAYOUT_H__
