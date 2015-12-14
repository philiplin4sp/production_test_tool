/*
    File:       encapsulated_commands.c
    Copyright:  CSR (C) 2009
    Purpose:    Module abstracting Encapsulated Commands for Sonic with the Jungo Drivers
*/
#include <windows.h>
#include <stdio.h>

#include "cdc_api.h" /* The definitions of IOCTL's */

#pragma pack(1)

#include "regs.h"
#include "encapsulated_commands.h"

/* Uncomment this to provide debug output */
#if !defined(_WIN32_WCE) && !defined(_WINCE) && _MSC_VER >= 1400
#define DEBUG_OUTPUT
#endif

/* Encapsulated Commands */
typedef enum
{
    CMD_version_get,
    CMD_FPGA_reg_get,
    CMD_FPGA_reg_set,
    CMD_ONE_get,
    CMD_ONE_set,
    CMD_DUT_cfg_element_get,
    CMD_DUT_cfg_element_set,
    CMD_DUT_reset_set,
    CMD_UART_stat_get,
    CMD_UART_flowControl_get,
    CMD_UART_flowControl_set,
    CMD_UART_RTS_get,
    CMD_UART_RTS_set,
    CMD_UART_CTS_get,
    CMD_PIO_get,
    CMD_PIO_set,
    CMD_PIOCDNE_set,
    CMD_PIOCDNE_get,
    CMD_PIOCDNP_get,
    CMD_PIO_notification_get,
    CMD_PIO_notification_set,
    CMD_DUT_ADC_get,
    CMD_DUT_cfg_enable_set,
    CMD_DUT_status_get,
} CIC_encapsulatedCommand_e;



int cdc_port_open(HANDLE *com_h, const char *port, BOOL overlapped)
{
    *com_h = CreateFile(port, GENERIC_READ | GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, (overlapped ? FILE_FLAG_OVERLAPPED : 0), NULL);

    if (*com_h == INVALID_HANDLE_VALUE)
    {
#ifdef DEBUG_OUTPUT
        fprintf(stderr, "%s: Error opening COM port \"%s\", error %x\n",
            __FUNCTION__, port, GetLastError());
#endif /* DEBUG_OUTPUT */
        return -1;
    }

    return 0;
}

int cdc_port_close(HANDLE com_h)
{
    if (!CloseHandle(com_h))
    {
#ifdef DEBUG_OUTPUT
        DWORD err = GetLastError();
        fprintf(stderr, "%s: Failed closing COM handle %lx\n", __FUNCTION__,
            err);
#endif /* DEBUG_OUTPUT */
        return -1;
    }

    return 0;
}

int cdc_encapsulated_send(HANDLE com_h, void *buffer, DWORD length, PDWORD bytes_transferred)
{
    if (!DeviceIoControl(
        com_h,
        IOCTL_ACM_SEND_ENCAPSULATED_COMMAND,
        buffer,   /* in buffer */
        length,   /* in buffer length */
        NULL,     /* out buffer */
        0,        /* out buffer length */
        bytes_transferred,
        NULL))
    {
#ifdef DEBUG_OUTPUT
        DWORD last_err = GetLastError();
        fprintf(stderr, "%s: DeviceIoControl failed with error %lx\n",
            __FUNCTION__, last_err);
#endif /* DEBUG_OUTPUT */
        return -1;
    }

    if (*bytes_transferred != length)
    {
#ifdef DEBUG_OUTPUT
        DWORD last_err = GetLastError();
        fprintf(stderr, "%s: ERROR bytes_transferred %ld, length %ld, "
            "error %lx\n", __FUNCTION__, *bytes_transferred, length, last_err);
#endif /* DEBUG_OUTPUT */
        return -1;
    }

    return 0; /* success */
}

int cdc_encapsulated_get(HANDLE com_h, void *buffer, DWORD length, PDWORD bytes_transferred)
{
    if (!DeviceIoControl(
        com_h,
        IOCTL_ACM_GET_ENCAPSULATED_RESPONSE,
        NULL,   /* in buffer */
        0,      /* in buffer length */
        buffer, /* out buffer */
        length, /* out buffer length */
        bytes_transferred,
        NULL))
    {
#ifdef DEBUG_OUTPUT
        DWORD last_err = GetLastError();
        fprintf(stderr, "%s: DeviceIoControl failed with error %lx\n",
            __FUNCTION__, last_err);
#endif /* DEBUG_OUTPUT */
        return -1;
    }

    return 0; /* success */
}

int write_fpga_register(HANDLE com_h, uint16 offset, unsigned char value)
{
    unsigned char buffer[5];  
    DWORD bytes_transferred;

    buffer[0] = CMD_FPGA_reg_set;	// Write data to FPGA registers
    buffer[1] = 1;                  // Write n bytes
    buffer[2] = LOBYTE(offset);     // offset low byte
    buffer[3] = HIBYTE(offset);     // offset high byte
    buffer[4] = value;              // value

    /* Send encapsulated command */
    return cdc_encapsulated_send(com_h, buffer, sizeof(buffer), &bytes_transferred);
}

int read_fpga_register(HANDLE com_h, uint16 offset, unsigned char* value)
{
    unsigned char buffer[4];  
    DWORD bytes_transferred;
    int err;

    buffer[0] = CMD_FPGA_reg_get;	// Read bytes from FPGA registers
    buffer[1] = 1;                  // Read n bytes
    buffer[2] = LOBYTE(offset);     // offset low byte
    buffer[3] = HIBYTE(offset);     // offset high byte

    /* Send encapsulated command */
    err = cdc_encapsulated_send(com_h, buffer, sizeof(buffer), &bytes_transferred);
    if (err)
        return err;

    /* Get encapsulated response */
    err = cdc_encapsulated_get(com_h, buffer, 1, &bytes_transferred);
    if (err)
        return err;

    *value = buffer[0];

    return 0;
}

int enc_fpga_registers_read(HANDLE com_h, uint16 offset, unsigned char cbSize, unsigned char* value)
{
    unsigned char buffer[4];  
    DWORD bytes_transferred;
    int err;

    if(cbSize > 32)
        cbSize = 32;
    if(NULL == value)
        return 2;
    buffer[0] = CMD_FPGA_reg_get;	// Read bytes from FPGA registers
    buffer[1] = cbSize;             // Read n bytes
    buffer[2] = LOBYTE(offset);     // offset low byte
    buffer[3] = HIBYTE(offset);     // offset high byte

    /* Send encapsulated command */
    err = cdc_encapsulated_send(com_h, buffer, sizeof(buffer), &bytes_transferred);
    if (err)
        return err;

    /* Get encapsulated response */
    err = cdc_encapsulated_get(com_h, value, cbSize, &bytes_transferred);
    if (err)
        return err;

    if(bytes_transferred != cbSize)
        return 3;
    return 0;
}

/* PIO get */
typedef struct
{
	uint8 cmd;
} enc_pio_get_req_t;
typedef struct
{
	uint32 input;
} enc_pio_get_reply_t;
int
enc_pio_get(HANDLE com_h, uint32 *pio)
{
	enc_pio_get_req_t bufout;
	enc_pio_get_reply_t bufin;
    DWORD bytes_transferred;
    int err;

	bufout.cmd = CMD_PIO_get;
    /* Send encapsulated command */
    err = cdc_encapsulated_send(com_h, &bufout, sizeof(bufout), &bytes_transferred);
    if (err)
        return err;

    /* Get encapsulated response */
    err = cdc_encapsulated_get(com_h, &bufin, sizeof(bufin), &bytes_transferred);
    if (err)
        return err;

	if(pio)
		*pio = bufin.input;

    return 0;
}

/* PIO set */
typedef struct
{
	uint8 cmd;
    uint32 mask;
    uint32 output;
} enc_pio_set_t;
int
enc_pio_set(HANDLE com_h, uint32 mask, uint32 pio)
{
    DWORD bytes_transferred;
	enc_pio_set_t buffer;

	buffer.cmd = CMD_PIO_set;
	buffer.mask = mask;
	buffer.output = pio;
    /* Send encapsulated command */
    return cdc_encapsulated_send(com_h, &buffer, sizeof(buffer), &bytes_transferred);
}

/* PIO Change Detection Notification Enable get */
typedef struct
{
	uint8 cmd;
} enc_piocdne_get_req_t;
typedef struct
{
	uint32 rise;
	uint32 fall;
} enc_piocdne_get_reply_t;
int
enc_piocdne_get(HANDLE com_h, uint32 *rise, uint32 *fall)
{
	enc_piocdne_get_req_t bufout;
	enc_piocdne_get_reply_t bufin;
    DWORD bytes_transferred;
    int err;

	bufout.cmd = CMD_PIOCDNE_get;
    /* Send encapsulated command */
    err = cdc_encapsulated_send(com_h, &bufout, sizeof(bufout), &bytes_transferred);
    if (err)
        return err;

    /* Get encapsulated response */
    err = cdc_encapsulated_get(com_h, &bufin, sizeof(bufin), &bytes_transferred);
    if (err)
        return err;

	if(rise)
		*rise = bufin.rise;
	if(fall)
		*fall = bufin.fall;

    return 0;
}

/* PIO Change Detection Notification Enable set */
typedef struct
{
	uint8 cmd;
    uint32 rise_mask;
    uint32 rise;
    uint32 fall_mask;
    uint32 fall;
} enc_piocdne_set_t;
int
enc_piocdne_set(HANDLE com_h, uint32 mask_rise, uint32 rise, uint32 mask_fall, uint32 fall)
{
    DWORD bytes_transferred;
	enc_piocdne_set_t buffer;

	buffer.cmd = CMD_PIOCDNE_set;
	buffer.rise_mask = mask_rise;
	buffer.rise = rise;
	buffer.fall = fall;
	buffer.fall_mask = mask_fall;
    /* Send encapsulated command */
    return cdc_encapsulated_send(com_h, &buffer, sizeof(buffer), &bytes_transferred);
}

/* PIO Change Detection Notification (Poll) get */
typedef struct
{
	uint8 cmd;
} enc_piocdnp_get_req_t;
typedef struct
{
	uint32 rise;
	uint32 fall;
} enc_piocdnp_get_reply_t;
int
enc_piocdnp_get(HANDLE com_h, uint32 *rise, uint32 *fall)
{
	enc_piocdnp_get_req_t bufout;
	enc_piocdnp_get_reply_t bufin;
    DWORD bytes_transferred;
    int err;

	bufout.cmd = CMD_PIOCDNP_get;
    /* Send encapsulated command */
    err = cdc_encapsulated_send(com_h, &bufout, sizeof(bufout), &bytes_transferred);
    if (err)
        return err;

    /* Get encapsulated response */
    err = cdc_encapsulated_get(com_h, &bufin, sizeof(bufin), &bytes_transferred);
    if (err)
        return err;

	if(rise)
		*rise = bufin.rise;
	if(fall)
		*fall = bufin.fall;

    return 0;
}

/* UART flow control set */
typedef struct
{
	uint8 cmd;
    uint8 flowctrl;
} enc_UART_flowctrl_set_t;
int
enc_UART_flowctrl_set(HANDLE com_h, UART_flowctrl_e flowctrl)
{
    DWORD bytes_transferred;
	enc_UART_flowctrl_set_t buffer;

	buffer.cmd = CMD_UART_flowControl_set;
	buffer.flowctrl = flowctrl;
    /* Send encapsulated command */
    return cdc_encapsulated_send(com_h, &buffer, sizeof(buffer), &bytes_transferred);
}

/* UART flow control get */
typedef struct
{
	uint8 cmd;
} enc_UART_flowctrl_get_req_t;
typedef struct
{
    uint8 flowctrl;
} enc_UART_flowctrl_get_reply_t;
int
enc_UART_flowctrl_get(HANDLE com_h, UART_flowctrl_e *flowctrl)
{
	enc_UART_flowctrl_get_req_t bufout;
	enc_UART_flowctrl_get_reply_t bufin;
    DWORD bytes_transferred;
    int err;

	bufout.cmd = CMD_UART_flowControl_get;
    /* Send encapsulated command */
    err = cdc_encapsulated_send(com_h, &bufout, sizeof(bufout), &bytes_transferred);
    if (err)
        return err;

    /* Get encapsulated response */
    err = cdc_encapsulated_get(com_h, &bufin, sizeof(bufin), &bytes_transferred);
    if (err)
        return err;

	if(flowctrl)
		*flowctrl = bufin.flowctrl;

    return 0;
}

/* UART RTS assert set */
typedef struct
{
	uint8 cmd;
    uint8 rts_assert;
} enc_UART_RTS_set_req_t;
int
enc_UART_RTS_set(HANDLE com_h, BOOL rts_assert)
{
    DWORD bytes_transferred;
	enc_UART_RTS_set_req_t buffer;

	buffer.cmd = CMD_UART_RTS_set;
	buffer.rts_assert = rts_assert;
    /* Send encapsulated command */
    return cdc_encapsulated_send(com_h, &buffer, sizeof(buffer), &bytes_transferred);
}

/* UART RTS assert get */
typedef struct
{
	uint8 cmd;
} enc_UART_RTS_get_req_t;
typedef struct
{
    uint8 rts_assert;
} enc_UART_RTS_get_reply_t;
int
enc_UART_RTS_get(HANDLE com_h, BOOL *rts_assert)
{
	enc_UART_RTS_get_req_t bufout;
	enc_UART_RTS_get_reply_t bufin;
    DWORD bytes_transferred;
    int err;

	bufout.cmd = CMD_UART_RTS_get;
    /* Send encapsulated command */
    err = cdc_encapsulated_send(com_h, &bufout, sizeof(bufout), &bytes_transferred);
    if (err)
        return err;

    /* Get encapsulated response */
    err = cdc_encapsulated_get(com_h, &bufin, sizeof(bufin), &bytes_transferred);
    if (err)
        return err;

	if(rts_assert)
		*rts_assert = bufin.rts_assert;

    return 0;
}

/* UART CTS get */
typedef struct
{
	uint8 cmd;
} enc_UART_CTS_get_req_t;
typedef struct
{
    uint8 cts;
} enc_UART_CTS_get_reply_t;
int
enc_UART_CTS_get(HANDLE com_h, BOOL *cts)
{
	enc_UART_CTS_get_req_t bufout;
	enc_UART_CTS_get_reply_t bufin;
    DWORD bytes_transferred;
    int err;

	bufout.cmd = CMD_UART_CTS_get;
    /* Send encapsulated command */
    err = cdc_encapsulated_send(com_h, &bufout, sizeof(bufout), &bytes_transferred);
    if (err)
        return err;

    /* Get encapsulated response */
    err = cdc_encapsulated_get(com_h, &bufin, sizeof(bufin), &bytes_transferred);
    if (err)
        return err;

	if(cts)
		*cts = bufin.cts;

    return 0;
}


/* UART RTS assert set */
typedef struct
{
	uint8 cmd;
    uint8 rst_assert;
} enc_DUT_reset_set_req_t;
int
enc_DUT_reset_set(HANDLE com_h, BOOL rst_assert)
{
    DWORD bytes_transferred;
	enc_DUT_reset_set_req_t buffer;

	buffer.cmd = CMD_DUT_reset_set;
	buffer.rst_assert = rst_assert;
    /* Send encapsulated command */
    return cdc_encapsulated_send(com_h, &buffer, sizeof(buffer), &bytes_transferred);
}

/* DUT status get */
typedef struct
{
	uint8 cmd;
} enc_DUT_status_get_req_t;
typedef struct
{
    uint8 DUT_status;
} enc_DUT_status_get_reply_t;
int
enc_DUT_status_get(HANDLE com_h, uint8 *DUT_status)
{
	enc_DUT_status_get_req_t bufout;
	enc_DUT_status_get_reply_t bufin;
    DWORD bytes_transferred;
    int err;

	bufout.cmd = CMD_DUT_status_get;
    /* Send encapsulated command */
    err = cdc_encapsulated_send(com_h, &bufout, sizeof(bufout), &bytes_transferred);
    if (err)
        return err;

    /* Get encapsulated response */
    err = cdc_encapsulated_get(com_h, &bufin, sizeof(bufin), &bytes_transferred);
    if (err)
        return err;

	if(DUT_status)
		*DUT_status = bufin.DUT_status;

    return 0;
}

/* DUT Configuration Enable Set */
typedef struct
{
	uint8 cmd;
    uint8 DUT_cfg_enable;
} enc_DUT_cfg_enable_set_req_t;
typedef struct
{
    uint8 err;          /* Defined in DUT_cfg_err_t */
} enc_DUT_cfg_enable_set_reply_t;
int
enc_DUT_cfg_enable_set(HANDLE com_h, BOOL DUT_cfg_enable, uint8 *error)
{
    DWORD bytes_transferred;
	enc_DUT_cfg_enable_set_req_t bufout;
	enc_DUT_cfg_enable_set_reply_t bufin;
    int err;

	bufout.cmd = CMD_DUT_cfg_enable_set;
	bufout.DUT_cfg_enable = DUT_cfg_enable;
    /* Send encapsulated command */
    err = cdc_encapsulated_send(com_h, &bufout, sizeof(bufout), &bytes_transferred);
    if (err)
        return err;

    /* Get encapsulated response */
    err = cdc_encapsulated_get(com_h, &bufin, sizeof(bufin), &bytes_transferred);
    if (err)
        return err;

	if(error)
		*error = bufin.err;

    return 0;
}

/* DUT Configuration Element Set */
typedef struct
{
	uint8 cmd;
    uint16 id;
    uint32 value;
} enc_DUT_cfg_element_set_req_t;
typedef struct
{
    uint8 err;          /* Defined in DUT_cfg_err_t */
} enc_DUT_cfg_element_set_reply_t;
int
enc_DUT_cfg_element_set(HANDLE com_h, uint16 id, uint32 value, uint8 *error)
{
    DWORD bytes_transferred;
	enc_DUT_cfg_element_set_req_t bufout;
	enc_DUT_cfg_element_set_reply_t bufin;
    int err;

	bufout.cmd = CMD_DUT_cfg_element_set;
	bufout.id = id;
	bufout.value = value;
    /* Send encapsulated command */
    err = cdc_encapsulated_send(com_h, &bufout, sizeof(bufout), &bytes_transferred);
    if (err)
        return err;

    /* Get encapsulated response */
    err = cdc_encapsulated_get(com_h, &bufin, sizeof(bufin), &bytes_transferred);
    if (err)
        return err;

	if(error)
		*error = bufin.err;

    return 0;
}

/* DUT Configuration Element Get */
typedef struct
{
	uint8 cmd;
    uint16 id;
} enc_DUT_cfg_element_get_req_t;
typedef struct
{
    uint8 err;          /* Defined in DUT_cfg_err_t */
    uint32 value;
} enc_DUT_cfg_element_get_reply_t;
int
enc_DUT_cfg_element_get(HANDLE com_h, uint16 id, uint32 *value, uint8 *error)
{
	enc_DUT_cfg_element_get_req_t bufout;
	enc_DUT_cfg_element_get_reply_t bufin;
    DWORD bytes_transferred;
    int err;

	bufout.cmd = CMD_DUT_cfg_element_get;
	bufout.id = id;
    /* Send encapsulated command */
    err = cdc_encapsulated_send(com_h, &bufout, sizeof(bufout), &bytes_transferred);
    if (err)
        return err;

    /* Get encapsulated response */
    err = cdc_encapsulated_get(com_h, &bufin, sizeof(bufin), &bytes_transferred);
    if (err)
        return err;

	if(value)
		*value = bufin.value;
	if(error)
		*error = bufin.err;

    return 0;
}

/* DUT ADC Get */
typedef struct
{
	uint8 cmd;
    uint8 channel;
} enc_DUT_ADC_get_req_t;
typedef struct
{
    uint16 value;
} enc_DUT_ADC_get_reply_t;
int
enc_DUT_ADC_get(HANDLE com_h, adc_channel_e channel, uint16 *value)
{
	enc_DUT_ADC_get_req_t bufout;
	enc_DUT_ADC_get_reply_t bufin;
    DWORD bytes_transferred;
    int err;

	bufout.cmd = CMD_DUT_ADC_get;
	bufout.channel = channel;
    /* Send encapsulated command */
    err = cdc_encapsulated_send(com_h, &bufout, sizeof(bufout), &bytes_transferred);
    if (err)
        return err;

    /* Get encapsulated response */
    err = cdc_encapsulated_get(com_h, &bufin, sizeof(bufin), &bytes_transferred);
    if (err)
        return err;

	if(value)
		*value = bufin.value;

    return 0;
}

/* Version numbers Get */
typedef struct
{
	uint8 cmd;
    uint8 verID;
} enc_version_get_req_t;
typedef struct
{
    uint32 value;
} enc_version_get_reply_t;
int
enc_version_get(HANDLE com_h, versionID_e verID, uint32 *value)
{
	enc_version_get_req_t bufout;
	enc_version_get_reply_t bufin;
    DWORD bytes_transferred;
    int err;

	bufout.cmd = CMD_version_get;
	bufout.verID = verID;
    /* Send encapsulated command */
    err = cdc_encapsulated_send(com_h, &bufout, sizeof(bufout), &bytes_transferred);
    if (err)
        return err;

    /* Get encapsulated response */
    err = cdc_encapsulated_get(com_h, &bufin, sizeof(bufin), &bytes_transferred);
    if (err)
        return err;

	if(value)
		*value = bufin.value;

    return 0;
}


#pragma pack()

int
enc_DUT_reset(HANDLE com_h, DWORD dwMilliseconds)
{
    int err;
    err = enc_DUT_reset_set(com_h, TRUE);
    if(!err)
    {
        Sleep(dwMilliseconds);
        err = enc_DUT_reset_set(com_h, FALSE);
    }
    return err;
}
