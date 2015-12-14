#ifndef _USB_REPORTS_H_
#define _USB_REPORTS_H_
typedef unsigned char uint8;
typedef struct {
    uint8 report_id;
    uint8 command;
    uint8 data[1021];
} hid_command_t;

typedef struct {
    uint8 report_id;
    uint8 last_command;        
    uint8 last_command_status;
	uint8 data[12];
} hid_status_t;

#define REPORT_COMMAND_ID       1
#define REPORT_COMMAND_SIZE     ((sizeof(hid_command_t)/sizeof(uint8))-1)
#define REPORT_STATUS_ID        2
#define REPORT_STATUS_SIZE      ((sizeof(hid_status_t)/sizeof(uint8))-1)

typedef enum {
    STATUS_CMD_COMPLETED_OK,
    STATUS_CMD_IN_PROGRESS,
    STATUS_CMD_FAILED
} hid_status_e;

typedef enum {
    COMMAND_NOP,                /* no operation */    
    COMMAND_SET_BOOTMODE,       /* data[0] = new bootmode to set, returns STATUS_CMD_COMPLETED_OK is already in that bootmode or STATUS_CMD_IN_PROGRESS if reboot will follow */
    COMMAND_ERASE_SQIF,         /* data[0] = partition to erase */
    COMMAND_WRITE_SQIF,         /* data[0..1] = number of bytes to follow, data[2]..data[data[0..1]-2] = data to be written, the data count must be even */
    COMMAND_WRITE_SIGNATURE,    /* data[0..1] = size of data to follow, data[2]..data[data[0..1]-2] = signature data; data[0..1] must be even */
    COMMAND_VALIDATE_SQIF  ,/* data[0] = validation type (1=DFU, 2=CRC, 3=NONE) */
    COMMAND_READ_DEVICE_INFO, /* reads bluetooth add and sequence number and version info and sends to host app */
    COMMAND_TEST_ON,
    COMMAND_TEST_OFF,
    COMMAND_BURN_TEST_INIT,
    COMMAND_BURN_TEST_END,
    COMMAND_UPDATE_RESET,
    COMMAND_SET_EVENT,
    COMMAND_PTS_TRIGGER,
	COMMAND_BATTERY,
    COMMAND_READ_SERIAL1, /* reads first 12 bytes of serial number */
    COMMAND_READ_SERIAL2 /* reads last 6 bytes of serial number */
} hid_commands_e;
#endif /* _USB_REPORTS_H_ */
