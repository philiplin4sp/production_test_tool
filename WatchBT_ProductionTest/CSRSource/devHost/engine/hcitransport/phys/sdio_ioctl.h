
#ifndef _UNIRAW_IOCTL_H_
#define _UNIRAW_IOCTL_H_

#include <winioctl.h>

#define UNIRAW_IOCTL_INDEX  0x1872
#define UNIRAW_BLOCK_TRANSFER 0x08000000

#define UNIRAW_DRIVER_VERSION CTL_CODE(FILE_DEVICE_UNKNOWN, UNIRAW_IOCTL_INDEX, METHOD_BUFFERED, FILE_READ_DATA)
#define UNIRAW_READ_CCCR CTL_CODE(FILE_DEVICE_UNKNOWN, UNIRAW_IOCTL_INDEX+1, METHOD_BUFFERED, FILE_READ_DATA)
#define UNIRAW_READ_FBR CTL_CODE(FILE_DEVICE_UNKNOWN, UNIRAW_IOCTL_INDEX+2, METHOD_BUFFERED, FILE_READ_DATA)
#define UNIRAW_GET_BUS_WIDTH CTL_CODE(FILE_DEVICE_UNKNOWN, UNIRAW_IOCTL_INDEX+3, METHOD_BUFFERED, FILE_READ_DATA)
#define UNIRAW_SET_BUS_WIDTH CTL_CODE(FILE_DEVICE_UNKNOWN, UNIRAW_IOCTL_INDEX+4, METHOD_BUFFERED, FILE_WRITE_DATA)
#define UNIRAW_GET_BUS_CLOCK CTL_CODE(FILE_DEVICE_UNKNOWN, UNIRAW_IOCTL_INDEX+5, METHOD_BUFFERED, FILE_READ_DATA)
#define UNIRAW_SET_BUS_CLOCK CTL_CODE(FILE_DEVICE_UNKNOWN, UNIRAW_IOCTL_INDEX+6, METHOD_BUFFERED, FILE_WRITE_DATA)
#define UNIRAW_GET_FUNCTION_BLOCK_LENGTH CTL_CODE(FILE_DEVICE_UNKNOWN, UNIRAW_IOCTL_INDEX+7, METHOD_BUFFERED, FILE_READ_DATA)
#define UNIRAW_SET_FUNCTION_BLOCK_LENGTH CTL_CODE(FILE_DEVICE_UNKNOWN, UNIRAW_IOCTL_INDEX+8, METHOD_BUFFERED, FILE_WRITE_DATA)
#define UNIRAW_GET_DEV_TUPLE CTL_CODE(FILE_DEVICE_UNKNOWN, UNIRAW_IOCTL_INDEX+9, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)
#define UNIRAW_GET_FUNC_TUPLE CTL_CODE(FILE_DEVICE_UNKNOWN, UNIRAW_IOCTL_INDEX+10, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)
#define UNIRAW_READ_EER CTL_CODE(FILE_DEVICE_UNKNOWN, UNIRAW_IOCTL_INDEX+11, METHOD_BUFFERED, FILE_READ_DATA)

#endif 

typedef struct _CCCR
{
    unsigned char CccrSdioRevision;
    unsigned char SDRevision;
    unsigned char IOEnable;
    unsigned char IOReady;
    unsigned char INTEnable;
    unsigned char INTPending;
    unsigned char IOAbort;
    unsigned char BusInterfaceControl;
    unsigned char CardCapability;
    unsigned char CommonCISPointer[3];
    unsigned char BusSuspend;
    unsigned char FunctionSelect;
    unsigned char ExecFlags;
    unsigned char ReadyFlags;
    unsigned char FN0BlockSize[2];
    unsigned char VendorUnique[16];
}
CCCR, *PCCCR;

typedef struct _FBR
{
    unsigned char FunctionInterfaceCode;    // 0x00n00
    unsigned char ExtendedInterfaceCode;    // 0x00n01
    unsigned char EPS_SPS;                  // 0x00n02
    unsigned char RFU03_8[6];               // 0x00n03 - 0x00n08
    unsigned char CISPointer[3];            // 0x00n09 - 0x00n0b
    unsigned char CSAPointer[3];            // 0x00n0c - 0x00n0e
    unsigned char DataAccessWindowToCSA;    // 0x00n0f
    unsigned char IOBlockSize[2];           // 0x00n10 - 0x00n11
    unsigned char RFU012_FF[237];           // 0x00n12 - 0x00nff
}
FBR, *PFBR;




























