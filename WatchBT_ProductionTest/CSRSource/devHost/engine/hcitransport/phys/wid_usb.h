/*++

Copyright (c) 1997-1998  Microsoft Corporation

Module Name:

    wid_usb.h

Abstract:

    Driver-defined special IOCTL's    

Environment:

    Kernel & user mode

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1997-1998 Microsoft Corporation.  All Rights Reserved.
Revision History:

	11/17/97: created

--*/


// Module to handle USB transport.
// Copyright (c) 2000-2006 Cambridge Silicon Radio.  All Rights Reserved.

// $Log: wid_usb.h,v $
// Revision 1.1  2001/12/10 22:26:49  ajh
// Commited changes to single dll which separate transport protocols
// from transport devices.
//
// Revision 1.7  2001/11/06 18:59:13  mm01
// The class contained static functions, that are called as threads.  I made these stubs that called proper member functions, also added volatile to some bools
//
// Revision 1.6  2001/10/11 16:52:03  bs01
// Make the widcomm usb wrapper non-copyable. (It has raw pointers in it.)
//
// Revision 1.5  2001/10/11 16:44:54  bs01
// Rename from mutex to system_wide_mutex (the shorter name could have cause people to use it inappropriately and get unintended interactions between separate processes.)
//
// Revision 1.4  2001/10/09 17:02:57  cl01
// Added Exclusive Access for Widcomm USB driver.
//
// Revision 1.3  2001/09/26 11:32:33  cl01
// Comments
//
// Revision 1.2  2001/09/25 14:09:06  cl01
// Seems to be working.  Mods to read only full buffers.
//
// Revision 1.1  2001/09/24 15:30:41  cl01
// Added support for Widcomm USB device driver
//



#ifndef CSRBC01WIDH_INC
#define CSRBC01WIDH_INC

#include "thread/system_wide_mutex.h"
#include "usb.h"


// The base of the IOCTL control codes.
#define BTWUSB_IOCTL_INDEX  0x089a

#define IOCTL_BTWUSB_GET_PIPE_INFO     CTL_CODE(FILE_DEVICE_UNKNOWN,  \
    BTWUSB_IOCTL_INDEX+0,METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_BTWUSB_GET_DEVICE_DESCRIPTOR CTL_CODE(FILE_DEVICE_UNKNOWN,  \
    BTWUSB_IOCTL_INDEX+1,METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_BTWUSB_GET_CONFIGURATION_DESCRIPTOR CTL_CODE(FILE_DEVICE_UNKNOWN,  \
    BTWUSB_IOCTL_INDEX+2,METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_BTWUSB_PUT CTL_CODE(FILE_DEVICE_UNKNOWN,  \
    BTWUSB_IOCTL_INDEX+3,METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_BTWUSB_PUT_CMD CTL_CODE(FILE_DEVICE_UNKNOWN,  \
    BTWUSB_IOCTL_INDEX+4,METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_BTWUSB_GET CTL_CODE(FILE_DEVICE_UNKNOWN,  \
    BTWUSB_IOCTL_INDEX+5,METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_BTWUSB_GET_EVENT CTL_CODE(FILE_DEVICE_UNKNOWN,  \
    BTWUSB_IOCTL_INDEX+6,METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_BTWUSB_GET_STATS CTL_CODE(FILE_DEVICE_UNKNOWN,  \
    BTWUSB_IOCTL_INDEX+7,METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_BTWUSB_CLEAR_STATS CTL_CODE(FILE_DEVICE_UNKNOWN,  \
    BTWUSB_IOCTL_INDEX+8,METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_BTWUSB_GET_VERSION CTL_CODE(FILE_DEVICE_UNKNOWN,  \
    BTWUSB_IOCTL_INDEX+9,METHOD_BUFFERED, FILE_ANY_ACCESS)

// TMP!!
#define IOCTL_BTWUSB_CANCEL_READ CTL_CODE(FILE_DEVICE_UNKNOWN,  \
    BTWUSB_IOCTL_INDEX+0xA,METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_BTWUSB_SUBMIT_READ CTL_CODE(FILE_DEVICE_UNKNOWN,  \
    BTWUSB_IOCTL_INDEX+0xB,METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_BTWUSB_RESET_PORT CTL_CODE(FILE_DEVICE_UNKNOWN,  \
    BTWUSB_IOCTL_INDEX+0xC,METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_BTWUSB_GET_IDS CTL_CODE(FILE_DEVICE_UNKNOWN,  \
    BTWUSB_IOCTL_INDEX+0xD,METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_BTWUSB_GO_DFU CTL_CODE(FILE_DEVICE_UNKNOWN,  \
    BTWUSB_IOCTL_INDEX+0xE,METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_BTWUSB_UNCOND_SUSPEND CTL_CODE(FILE_DEVICE_UNKNOWN,  \
    BTWUSB_IOCTL_INDEX+0xF,METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_BTWUSB_COND_SUSPEND CTL_CODE(FILE_DEVICE_UNKNOWN,  \
    BTWUSB_IOCTL_INDEX+0x10,METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_BTWUSB_GET_HARDWARE_ID CTL_CODE(FILE_DEVICE_UNKNOWN,  \
    BTWUSB_IOCTL_INDEX+0x11,METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_BTWUSB_GET_DEVICE_DESC CTL_CODE(FILE_DEVICE_UNKNOWN,  \
    BTWUSB_IOCTL_INDEX+0x12,METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_BTWUSB_SET_VOICE CTL_CODE(FILE_DEVICE_UNKNOWN,  \
    BTWUSB_IOCTL_INDEX+0x13,METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_BTWUSB_PUT_VOICE CTL_CODE(FILE_DEVICE_UNKNOWN,  \
    BTWUSB_IOCTL_INDEX+0x14,METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_BTWUSB_GET_VOICE CTL_CODE(FILE_DEVICE_UNKNOWN,  \
    BTWUSB_IOCTL_INDEX+0x15,METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_BTWUSB_SET_EXCLUSIVE_ACCESS CTL_CODE(FILE_DEVICE_UNKNOWN,  \
    BTWUSB_IOCTL_INDEX+0x16,METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_BTWUSB_RESET_EXCLUSIVE_ACCESS CTL_CODE(FILE_DEVICE_UNKNOWN,  \
    BTWUSB_IOCTL_INDEX+0x17,METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// BTWUSB Stats
//
typedef struct BTWUSBStats
{
    // Number of Read IRPs submitted to the USB software stack
    unsigned long reads_submitted;

    // Number of Read IRP completions from the USB software stack
    unsigned long reads_completed;

    // Number of Read IRP completions in error
    unsigned long reads_completed_error;

    // Read frames discarded due to no buffers
    unsigned long reads_disc_nobuf;

    // Bytes received
    unsigned long reads_bytes;

    // Read buffers filled
    unsigned long reads_filled;

    // Number of Event IRPs submitted to the USB software stack
    unsigned long events_submitted;

    // Number of Event IRP completions from the USB software stack
    unsigned long events_completed;

    // Number of Event IRP completions in error
    unsigned long events_completed_error;

    // Event frames discarded due to no buffers
    unsigned long events_disc_nobuf;

    // Bytes received
    unsigned long events_bytes;

    // Event buffers filled
    unsigned long events_filled;

    // Number of Write IRPs submitted
    unsigned long writes_submitted;

    // Number of Write IRPs completed
    unsigned long writes_completed;

    // Number of Write IRPs completed in error
    unsigned long writes_completed_error;

    // Number of Write time-outs
    unsigned long writes_timeout;

    // Number of Writes not submitted due to no room on the tx queue
    unsigned long writes_disc_nobuf;

    // Number of Writes not submitted due to too long data
    unsigned long writes_disc_toolong;

    // Number of Command IRPs submitted
    unsigned long commands_submitted;

    // Number of Command IRPs completed
    unsigned long commands_completed;

    // Number of Command IRPs completed in error
    unsigned long commands_completed_error;

    // Number of Command time-outs
    unsigned long commands_timeout;

    // Number of Commands not submitted due to no room on the tx queue
    unsigned long commands_disc_nobuf;

    // Number of Commands not submitted due to too long data
    unsigned long commands_disc_toolong;

    // Number of configuration request time-outs
    unsigned long configs_timeout;

    // Number of voice IRPs submitted to the USB software stack
    unsigned long voicerx_submitted;

    // Number of voice IRP completions from the USB software stack
    unsigned long voicerx_completed;

    // Number of Event IRP completions in error
    unsigned long voicerx_completed_error;

    // Event frames discarded due to no buffers
    unsigned long voicerx_disc_nobuf;

    // Bytes received
    unsigned long voicerx_bytes;

    // Event buffers filled
    unsigned long voicerx_filled;

    // Number of Voice Tx IRPs submitted
    unsigned long voicetx_submitted;

    // Number of Voice Tx IRPs completed
    unsigned long voicetx_completed;

    // Number of Voice Tx IRPs completed in error
    unsigned long voicetx_completed_error;

    // Number of Voice Tx time-outs
    unsigned long voicetx_timeout;

    // Number of Voice Tx not submitted due to no room on the tx queue
    unsigned long voicetx_disc_nobuf;

    // Number of Writes not submitted due to too long data
    unsigned long voicetx_disc_toolong;

    // Last error reported by IRP
    unsigned long last_irp_error;

    // Last error reported by URB
    unsigned long last_urb_error;

} BTWUSBStats, *pBTWUSBStats;

//
// Device USB Parameters
//
typedef struct _USB_IDS
{
    unsigned short idVendor;
    unsigned short idProduct;
} USB_IDS, *PUSB_IDS;


class USBWidStack : public USBAbstraction
{

private:

	HANDLE	cmd_handle; 
	HANDLE	evt_handle; 
	HANDLE	acl_handle;
	HANDLE	sco_handle;
	HANDLE	wait_for_evt;
	HANDLE	wait_for_acl;
	HANDLE	wait_for_sco;
	HANDLE	wait_for_send_acl;
	HANDLE	wait_for_send_cmd;
	HANDLE	wait_for_send_sco;
	HANDLE	kill_threads_event;

	CRITICAL_SECTION close_handle_crit;

	volatile bool monitor_evt_thread_terminate;
	volatile bool monitor_acl_thread_terminate;
	volatile bool monitor_sco_thread_terminate;

	int		evt_paused;
	int		acl_paused;
	int		sco_paused;

	char	*evt_buffer;
	char	*acl_buffer;
	char	*sco_buffer;
	
	enum {StreamPollTime = 100};
	enum {
		Max_evt_Packet = 256+16+4,
		Max_acl_Packet = 65536+16+4,
		Max_sco_Packet = 65536+16+4,
	};

	long flushing_streams;

	SystemWideMutex *mutex_;

	HANDLE	monitor_evt_thread_handle;
	HANDLE	monitor_acl_thread_handle;
	HANDLE	monitor_sco_thread_handle;

	void	(*evt_func) (void *data, size_t length, void *context);
	void	(*acl_func) (void *data, size_t length, void *context);
	void	(*sco_func) (void *data, size_t length, void *context);

	void	*evt_func_context;
	void	*acl_func_context;
	void	*sco_func_context;

	int USBWidStack::get_evt (void *buffer, int length);
	int USBWidStack::get_acl (void *buffer, int length);
	int USBWidStack::get_sco (void *buffer, int length);

	static DWORD WINAPI __monitor_evt_thread (LPVOID arg);
	static DWORD WINAPI __monitor_acl_thread (LPVOID arg);
	static DWORD WINAPI __monitor_sco_thread (LPVOID arg);
	DWORD monitor_evt_thread (void);
	DWORD monitor_acl_thread (void);
	DWORD monitor_sco_thread (void);

	void monitor_evt (void);
	void monitor_acl (void);
	void monitor_sco (void);

	void close_handles(void);
	void close_connection_on_error (void);

public:

	USBWidStack ( const USBConfiguration & aConfig );
	~USBWidStack ();

	bool open_connection ();
	void close_connection (void);

	bool is_open (void);

	int send_cmd (void *data, size_t length);
	int send_acl (void *data, size_t length);
	int send_sco (void *data, size_t length);
  
	void set_evt_callback (void (*func) (void *, size_t, void *), void *context);
	void set_acl_callback (void (*func) (void *, size_t, void *), void *context);
	void set_sco_callback (void (*func) (void *, size_t, void *), void *context);
};



#endif // end, #ifndef CSRBC01WIDH_INC

