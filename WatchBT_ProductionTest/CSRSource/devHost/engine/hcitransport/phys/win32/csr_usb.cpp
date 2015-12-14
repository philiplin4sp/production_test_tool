// Module to handle USB transport.
// Copyright (c) 2000-2006 Cambridge Silicon Radio.  All Rights Reserved.
//
// $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/phys/win32/csr_usb.cpp#1 $

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#include <assert.h>
#include "../csr_usb.h"
#include "thread/error_msg.h"

// declarations from the DDK
#define STATUS_BUFFER_TOO_SMALL          (0xC0000023L)
typedef struct _USB_DEVICE_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType;
    USHORT bcdUSB;
    UCHAR bDeviceClass;
    UCHAR bDeviceSubClass;
    UCHAR bDeviceProtocol;
    UCHAR bMaxPacketSize0;
    USHORT idVendor;
    USHORT idProduct;
    USHORT bcdDevice;
    UCHAR iManufacturer;
    UCHAR iProduct;
    UCHAR iSerialNumber;
    UCHAR bNumConfigurations;
} USB_DEVICE_DESCRIPTOR, *PUSB_DEVICE_DESCRIPTOR;

// DFU mode device identification
static const uint8 dfuDeviceClass = 0xFE;
static const uint8 dfuDeviceSubClass = 0x01;
static const uint8 dfuDeviceProtocol = 0x00;

// dumps out all sco data sent to the device driver to a file (excluding sco headers)
//#define DUMP_SCO_WRITES_TO_FILE

// 
#define REMOVE_ASSERTS

// instead of using the samples passed to us, generate our own samples
// (FOR TESTING PURPOSES)
//#define GENERATE_SAMPLES

// defines what type of sample to generate if GENERATE_SAMPLES is defined
#define SAMPLERATE_8BIT
//#define SAMPLERATE_16BIT

#ifdef DUMP_SCO_WRITES_TO_FILE
HANDLE sco_handle;
#endif

#define MAX_INTERRUPT_PACKET_SIZE 64
#define MAX_BULK_PACKET_SIZE 64

//  32 packets = service every 32ms seems a reasonable compromise
//  between latency and buffering.
#define SCO_PACKETS_PER_BUFFER (36)
#define SCO_BUFFER_SIZE (sco_packet_size * SCO_PACKETS_PER_BUFFER)
//  the send buffer size is actually the maximum it can be.  51 = max(sco_packet_size)
#define SCO_SEND_BUFFER_SIZE (51 * SCO_PACKETS_PER_BUFFER)
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USBStack::USBStack ( const USBConfiguration & aConfig )
:
    USBAbstraction ( aConfig ),
    driver_version(0),
    sco_status(no_sco),
    monitor_sco_enabled(false),
    monitor_sco_thread_pausing(false),

    event_func(0),
    event_func_context(0),

    data_func(0),
    data_func_context(0),

    sco_func(0),
    sco_func_context(0),

    sco_holding(0),
    offset_into_sco_holding(0),

	max_packet_size(0)
{
    handle = INVALID_HANDLE_VALUE;
    monitor_events_thread_handle = INVALID_HANDLE_VALUE;
    monitor_data_thread_handle = INVALID_HANDLE_VALUE;
    monitor_sco_thread_handle = INVALID_HANDLE_VALUE;
    send_sco_thread_handle = INVALID_HANDLE_VALUE;
    wait_for_event = CreateEvent (0, 0, 0, 0);
    wait_for_data = CreateEvent (0, 0, 0, 0);
    wait_for_acl = CreateEvent (0, 0, 0, 0);
    wait_for_command = CreateEvent (0, 0, 0, 0);
    wait_for_send_sco = CreateEvent (0, 0, 0, 0);
    wait_for_start_sco =  CreateEvent (0, TRUE, 0, 0);
    wait_for_sco_paused = CreateEvent (0, 0, 0, 0);
    kill_threads_event = CreateEvent (0, TRUE, 0, 0);

    InitializeCriticalSection(&close_handle_crit);

    event_paused = 0;
    data_paused = 0;

    // > event size + interrupt data payload size
    event_buffer = new unsigned char[256 + MAX_INTERRUPT_PACKET_SIZE];

    // > ACL data size + bulk data payload size
    data_buffer = new unsigned char[65536 + MAX_BULK_PACKET_SIZE];

    memset(sco_overlaps,0,sizeof(sco_overlaps));
    //  there are several small buffers for reading sco
    //  allocated in wait_for_sco_start()
    for ( int i = 0 ; i < SCO_RECV_BUFFER_COUNT; ++i )
    {
        wait_for_recv_sco[i] = CreateEvent (0, TRUE, 0, 0);
		sco_buffer[i] = 0;
        sco_overlaps[i].hEvent = wait_for_recv_sco[i];
    }

    sco_packet_size = 0;

#ifdef DUMP_SCO_WRITES_TO_FILE
    sco_handle = CreateFile(II("sco.raw"),
                            GENERIC_WRITE,
                            0,
                            0,
                            CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                            0);
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USBStack::~USBStack ()
{
    close_connection();

    delete[] event_buffer;
    delete[] data_buffer;
    for ( int i = 0 ; i < SCO_RECV_BUFFER_COUNT ; ++i )
    {
        delete[] sco_buffer[i];
        CloseHandle (wait_for_recv_sco[i]);
    }

    CloseHandle (wait_for_event);
    CloseHandle (wait_for_data);

    CloseHandle (wait_for_acl);
    CloseHandle (wait_for_command);
    CloseHandle (kill_threads_event);
    CloseHandle (wait_for_send_sco);
    CloseHandle (wait_for_start_sco);
    CloseHandle (wait_for_sco_paused);

    DeleteCriticalSection(&close_handle_crit);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool USBStack::set_sco_bandwidth(int bytes_per_second)
{
    if ( sco_status == no_sco )
        return false;

    if(bytes_per_second == 0)
    {
        set_alternate_setting(0);
        return true;
    }

    if(bytes_per_second <= 8000)
    {
        set_alternate_setting(1);
        return true;
    }

    if(bytes_per_second <= 16000)
    {
        set_alternate_setting(2);
        return true;
    }
    
    if(bytes_per_second <= 24000)
    {
        set_alternate_setting(3);
        return true;
    }
    
    if(bytes_per_second <= 32000)
    {
        set_alternate_setting(4);
        return true;
    }

    if(bytes_per_second <= 48000)
    {
        set_alternate_setting(5);
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// valid num values:
//  - 0 = No active voice channels (for USB compliance)
//  - 1 = One voice channel with 8-bit encoding
//  - 2 = Two voice channels with 8-bit encoding & One voice channel with 16-bit encoding
//  - 3 = Three voice channels with 8-bit encoding
//  - 4 = Two voice channels with 16-bit encoding
//  - 5 = Three voice channels with 16-bit encoding

void USBStack::set_alternate_setting(int num)
{
    static OVERLAPPED overlapped = { 0, 0, 0, 0, 0 };
    unsigned long written;
    ULONG buffer;
    int status = 0;

    // validate alternate setting number
    if((num < 0) || (num > 5))
    {
        return;
    }
    
    // only start monitoring the sco pipe if sco is enabled and we have the bandwidth
    if((monitor_sco_enabled == FALSE) && (num != 0))
    {
        //  START
        monitor_sco ();
        Sleep (1000);
    }
    else if ((monitor_sco_enabled == TRUE) && (num != 0))
    {
        //  PAUSE
        ::ResetEvent(wait_for_start_sco);
        monitor_sco_thread_pausing = true;
        WaitForSingleObject(wait_for_sco_paused,INFINITE);
        monitor_sco_thread_pausing = false;
        //  sco should start again when "wait_for_start_sco" is set in the driver
    }
    else if ((monitor_sco_enabled == TRUE) && (num == 0))
    {
        //  STOP
        monitor_sco_thread_terminate = true;
        
        if (WaitForSingleObject(monitor_sco_thread_handle, 300)  == WAIT_TIMEOUT)
            TerminateThread (monitor_sco_thread_handle, 0);
        CloseHandle(monitor_sco_thread_handle);
        monitor_sco_thread_handle = INVALID_HANDLE_VALUE;

        if (WaitForSingleObject(send_sco_thread_handle, 300)  == WAIT_TIMEOUT)
            TerminateThread (send_sco_thread_handle, 0);
        CloseHandle(send_sco_thread_handle);
        send_sco_thread_handle = INVALID_HANDLE_VALUE;

        monitor_sco_enabled = FALSE;
    }


    if ( sco_status == high_speed_sco )
    {
        switch(num)
        {
        case 0:
            sco_packet_size = 0;
            break;
        case 1:
            sco_packet_size = 11;
            break;
        case 2:
            sco_packet_size = 19;
            break;
        case 3:
            sco_packet_size = 27;
            break;
        case 4:
            sco_packet_size = 35;
            break;
        case 5:
            sco_packet_size = 51;
            break;
        }
		sco_send_buffer.set_packets_per_header(1);
    }
    else if ( sco_status == full_speed_sco )
    {
        switch(num)
        {
        case 0: sco_packet_size = 0;
                break;
        case 1: sco_packet_size = 9;
                break;
        case 2: sco_packet_size = 17;
                break;
        case 3: sco_packet_size = 25;
                break;
        case 4: sco_packet_size = 33;
                break;
        case 5: sco_packet_size = 49;
                break;
        }
		sco_send_buffer.set_packets_per_header(3);
    }
    sco_send_buffer.set_sco_packet_size(sco_packet_size);
    // set up the buffer.
    buffer = num;

    overlapped.hEvent = wait_for_start_sco;

    status = DeviceIoControl(handle,
                             IOCTL_CSRBC_START_SCO_DATA,
                             &buffer,
                             sizeof(buffer),
                             0,
                             0,
                             &written,
                             &overlapped);


    if(!status)
    {
        printf("Failed to issue START_SCO_DATA IoControlCode (err=%08x)\n", GetLastError() );
    }
/*
    if((!status) && ((err = GetLastError () == ERROR_IO_PENDING))
    {
        status = GetOverlappedResult(handle, &overlapped, &written, TRUE);
    }
*/            
#ifdef DEBUG
    printf("Set Alternate Setting %d : %08x\n", num, status);
#endif

    if ( num == 0 )
    {
        ::ResetEvent(wait_for_start_sco);
    }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool USBStack::open_connection ()
{
#ifndef _WIN32_WCE
    //  only create a handle if we weren't passed one.
    if ( mConfig.get_handle() )
    {
        handle = mConfig.get_handle();
    }
    else
    {
        if ( mConfig.nameIsWide() )
        {
            handle = CreateFileW
                (
                mConfig.getNameW(),
                GENERIC_READ | GENERIC_WRITE,
                0,
                0,
                OPEN_EXISTING,
                FILE_FLAG_OVERLAPPED,
                0
                );
        }
        else
        {
            handle = CreateFileA
                (
                mConfig.getNameA(),
                GENERIC_READ | GENERIC_WRITE,
                0,
                0,
                OPEN_EXISTING,
                FILE_FLAG_OVERLAPPED,
                0
                );
        }
    }
#else
    handle = INVALID_HANDLE_VALUE;
#endif
    if (handle == INVALID_HANDLE_VALUE)
        return false;

    //  get version
    unsigned long written = 0;
    BOOL status = DeviceIoControl
    (
        handle,
        IOCTL_CSRBC_GET_VERSION,
        0,
        0,
        &driver_version,
        4,
        &written,
        0
    );
    //  the old driver fails to set the return value.
    OSVERSIONINFO ver;
    ver.dwOSVersionInfoSize = sizeof OSVERSIONINFO;
    status = status && GetVersionEx(&ver);
    if ( status )
    {
		USB_DEVICE_DESCRIPTOR device_descriptor;
		unsigned long written = 0;
		status = DeviceIoControl(handle, IOCTL_CSRBC_GET_DEVICE_DESCRIPTOR, 0, 0, &device_descriptor, sizeof(device_descriptor), &written, 0);
		if(status && written == sizeof(device_descriptor))
		{
			if (device_descriptor.bDeviceClass == dfuDeviceClass
				&& device_descriptor.bDeviceSubClass == dfuDeviceSubClass
				&& device_descriptor.bDeviceProtocol == dfuDeviceProtocol)
			{
				// DFU mode.
				return false;
			}
			else if (device_descriptor.bDeviceClass == 0
			         && device_descriptor.bDeviceSubClass == 0
			         && device_descriptor.bDeviceProtocol == 0)
			{
				// probably DFU mode. Need to read the interface descriptor to be sure,
				// but for our purposes we'll just assume it is. if it's not in DFU mode
				// then it's probably an error anyway.
				return false;
			}

			max_packet_size = device_descriptor.bMaxPacketSize0;
		}

        if ( (ver.dwMajorVersion > 5) || ( (ver.dwMajorVersion == 5) && (ver.dwMinorVersion >= 1) ) )
        {
            switch(driver_version)
            {
            case 1:
                // new driver, full speed usb 
                sco_status = full_speed_sco;
                break;
            case 2:
                // new driver, high speed usb 
                sco_status = high_speed_sco;
                break;
            default:
                // old version
                sco_status = no_sco;
                break;
            }
        }
        else
            sco_status = no_sco;

        ::ResetEvent(kill_threads_event);
        monitor_events ();
        monitor_data ();

        return true;
    }
    else
        return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool USBStack::is_open (void)
{
    return (handle != INVALID_HANDLE_VALUE);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void USBStack::close_connection (void)
{

    close_connection_on_error();
    if (monitor_events_thread_handle != INVALID_HANDLE_VALUE) 
    {
      if (WaitForSingleObject(monitor_events_thread_handle, 300) == WAIT_TIMEOUT)
        TerminateThread (monitor_events_thread_handle, 0);
      CloseHandle(monitor_events_thread_handle);
      monitor_events_thread_handle = INVALID_HANDLE_VALUE;
    }

    if (monitor_data_thread_handle != INVALID_HANDLE_VALUE) 
    {
      if (WaitForSingleObject(monitor_data_thread_handle, 300)  == WAIT_TIMEOUT)
        TerminateThread (monitor_data_thread_handle, 0);
      CloseHandle(monitor_data_thread_handle);
      monitor_data_thread_handle = INVALID_HANDLE_VALUE;
    }

    if (monitor_sco_thread_handle != INVALID_HANDLE_VALUE) 
    {
      if (WaitForSingleObject(monitor_sco_thread_handle, 300)  == WAIT_TIMEOUT)
        TerminateThread (monitor_sco_thread_handle, 0);
      CloseHandle(monitor_sco_thread_handle);
      monitor_sco_thread_handle = INVALID_HANDLE_VALUE;
    }

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void USBStack::close_handle(void)
{
    // only close the handle if we created it.
    if ( !mConfig.get_handle() )
    {
        EnterCriticalSection(&close_handle_crit);
        if (handle != INVALID_HANDLE_VALUE)
        {
            CloseHandle (handle);
        }
        handle = INVALID_HANDLE_VALUE;
        LeaveCriticalSection(&close_handle_crit);
    }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void USBStack::close_connection_on_error (void)
{
    DWORD err = GetLastError();
    ERROR_MSG(("USBStack::close_connection_on_error = %x\n", err));
    if ( err )
        onFailure(usb_handle_loss);
    
    if ((monitor_events_thread_handle != INVALID_HANDLE_VALUE) 
        ||  (monitor_data_thread_handle != INVALID_HANDLE_VALUE)
        ||  (monitor_sco_thread_handle != INVALID_HANDLE_VALUE))
    {
        monitor_events_thread_terminate = true;
        monitor_data_thread_terminate = true;
        monitor_sco_thread_terminate = true;
        SetEvent(kill_threads_event);
    }
    Sleep(0);
    close_handle();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool USBStack::size_forbidden (unsigned long length)
{
	return max_packet_size ? (length % max_packet_size == 0) : false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int USBStack::send_cmd (void *buffer, size_t length)
{
    sent ( (uint8*) buffer , length ); 
    int
        status;

    unsigned long
        written;

    static OVERLAPPED
        overlapped = { 0, 0, 0, 0, 0 };
    HANDLE handles[2] = {wait_for_command, kill_threads_event};
    DWORD err;

    if (handle != INVALID_HANDLE_VALUE)
    {
        overlapped.hEvent = wait_for_command;

        status = DeviceIoControl
        (
            handle,
            IOCTL_CSRBC_SEND_HCI_COMMAND,
            buffer,
            length,
            0,
            0,
            &written,
            &overlapped
        );

        if ((!status) && ((err = GetLastError ()) == ERROR_IO_PENDING))
        {
            if (WaitForMultipleObjects(2, handles, false, -1) == WAIT_OBJECT_0 + 1)
                return 0;           
            // WaitForSingleObject (wait_for_command, -1);
            written = 0;
        }
        else if (!status)
        {
            close_connection_on_error  ();
        }
#ifndef _WIN32_WCE
        status = GetOverlappedResult (handle, &overlapped, &written, FALSE);
#endif
        if (status == 0)
        {
            written = 0;
        }
    }
    else
    {
        written = 0;
    }

    return written;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int USBStack::send_acl (void *data, size_t length)
{
    sent ( (uint8*) data , length ); 
    unsigned int
        status;

    unsigned long
        written;

    static OVERLAPPED
        overlapped = { 0, 0, 0, 0, 0 };
    HANDLE handles[2] = {wait_for_acl, kill_threads_event};
    DWORD err;

    if (handle != INVALID_HANDLE_VALUE)
    {
        overlapped.hEvent = wait_for_acl;

        status = WriteFile (handle, data, length, &written, &overlapped);

        if ((!status) && ((err = GetLastError ()) == ERROR_IO_PENDING))
        {
            if (WaitForMultipleObjects(2, handles, false, -1) == WAIT_OBJECT_0 + 1)
                return 0;           

            written = 0;
        }
        else if (!status)
        {
            close_connection_on_error  ();
        }
#ifndef _WIN32_WCE
        status = GetOverlappedResult (handle, &overlapped, &written, FALSE);
#endif
        if (status == 0)
        {
            written = 0;
        }
    }
    else
    {
        written = 0;
    }

    return written;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int USBStack::get_hci_event (void *buffer, int length)
{
    int
        status;

    unsigned long
        written;

    static OVERLAPPED
        overlapped = { 0, 0, 0, 0, 0 };
    HANDLE handles[2] = {wait_for_event, kill_threads_event};
    DWORD err;

    if (handle != INVALID_HANDLE_VALUE)
    {
        if (event_paused > 0)
        {
            event_paused = 1;

            overlapped.hEvent = wait_for_event;

            status = DeviceIoControl
            (
                handle,
                IOCTL_CSRBC_BLOCK_HCI_EVENT,
                0,
                0,
                0,
                0,
                &written,
                &overlapped
            );

            if ((!status) && ((err = GetLastError ()) == ERROR_IO_PENDING))
            {
                if (WaitForMultipleObjects(2, handles, false, -1) == WAIT_OBJECT_0 + 1)
                {
                    ERROR_MSG(("CsrUsb::GetHCIEvent - killed\n"));
                    return 0;
                }
                written = 0;
            }
            else if (!status)
            {
                ERROR_MSG(("CsrUsb::GetHCIEvent - failed %x\n", GetLastError()));
                close_connection_on_error();
                return 0;
            }
        }

        overlapped.hEvent = wait_for_event;

        status = DeviceIoControl
        (
            handle,
            IOCTL_CSRBC_GET_HCI_EVENT,
            0,
            0,
            buffer,
            length,
            &written,
            &overlapped
        );

        if ((!status) && (length != 16) && (GetLastError() == ERROR_INVALID_PARAMETER))
        {
            //  the old driver required it to be 16.
            status = DeviceIoControl
            (
                handle,
                IOCTL_CSRBC_GET_HCI_EVENT,
                0,
                0,
                buffer,
                16,
                &written,
                &overlapped
            );
        }

        if ((!status) && ((err = GetLastError ()) == ERROR_IO_PENDING))
        {
            if (WaitForMultipleObjects(2, handles, false, -1) == WAIT_OBJECT_0 + 1)
            {
                ERROR_MSG(("CsrUsb::GetHCIEvent - killed\n"));
                return 0;
            }
#ifndef _WIN32_WCE
            status = GetOverlappedResult (handle, &overlapped, &written, FALSE);
#endif
        }
        else if (!status)
        {
            ERROR_MSG(("CsrUsb::GetHCIEvent - failed %x\n", GetLastError()));
            close_connection_on_error();
            return 0;
        }

        if (status == 0)
        {
            written = 0;
        }
    }
    else
    {
        written = 0;
    }

    if (written == 0)
    {
        event_paused ++;
    }
    else
    {
        event_paused = 0;
    }

    return written;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DWORD WINAPI USBStack::monitor_events_thread (LPVOID arg)
{
    USBStack
        *stack;

    int
        offset,
        length,
        size;

    stack = (USBStack *) arg;
    stack->monitor_events_thread_terminate = false;

    // ''offset'' is the number of bytes in the buffer (always)
    // ''length'' is the length of the next event
    // ''size''   is the length of the last read from the driver

    size = 0;

    while (!stack->monitor_events_thread_terminate)
    {
        offset = size;

        // If we only have a small amount of data in the buffer, then
        // we need to read in another chunk before we know the length
        while (offset < 2 && !stack->monitor_events_thread_terminate)
        {
            size = stack->get_hci_event (stack->event_buffer + offset, MAX_INTERRUPT_PACKET_SIZE);
            offset += size;
        }

        // We now know the length of the current packet
        length = (unsigned char) stack->event_buffer[1] + 2;

        // Read in the entire packett (plus perhaps the start of the next one)
        while (offset < length && !stack->monitor_events_thread_terminate)
        {
            size = stack->get_hci_event (stack->event_buffer + offset, MAX_INTERRUPT_PACKET_SIZE);
            offset += size;
        }

        // Send the packet up
        if (stack->event_func && !stack->monitor_events_thread_terminate)
        {
            stack->recv ((uint8*)stack->event_buffer, length);
            stack->event_func (stack->event_buffer, length, stack->event_func_context);
        }

        // If there is more data in the buffer that we have not read,
        // then copy it down, update the remaining size, and loop
        if(offset > length)
        {
            memmove(stack->event_buffer, stack->event_buffer + length, offset - length);
        }
        size = offset - length;
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void USBStack::monitor_events (void)
{
    DWORD
        thread_id;
    // ASSERT(monitor_events_thread_handle == INVALID_HANDLE_VALUE);
    // This should perhaps close down the thread if one already exists
    monitor_events_thread_handle = CreateThread (0, 0, monitor_events_thread, this, 0, &thread_id);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int USBStack::get_hci_data (void *buffer, int length)
{
    int
        status;

    unsigned long
        written;

    static OVERLAPPED
        overlapped = { 0, 0, 0, 0, 0 };
    HANDLE handles[2] = {wait_for_data, kill_threads_event};
    DWORD err;

    if (handle != INVALID_HANDLE_VALUE)
    {
        if (data_paused > 0)
        {
            data_paused = 1;

            overlapped.hEvent = wait_for_data;

            status = DeviceIoControl
            (
                handle,
                IOCTL_CSRBC_BLOCK_HCI_DATA,
                0,
                0,
                0,
                0,
                &written,
                &overlapped
            );

            if ((!status) && ((err = GetLastError ()) == ERROR_IO_PENDING))
            {
                if (WaitForMultipleObjects(2, handles, false, -1) == WAIT_OBJECT_0 + 1)
                {
                    ERROR_MSG(("CsrUsb::GetData - killed %x\n", GetLastError()));
                    return 0;
                }

            }
            else if (!status)
            {
                ERROR_MSG(("CsrUsb::GetData - failed %x\n", GetLastError()));
                close_connection_on_error  ();
                return 0;
            }
        }

        overlapped.hEvent = wait_for_data;

        status = ReadFile (handle, buffer, length, &written, &overlapped);

        if ((!status) && ((err = GetLastError ()) == ERROR_IO_PENDING))
        {
            if (WaitForMultipleObjects(2, handles, false, -1) == WAIT_OBJECT_0 + 1)
            {
                ERROR_MSG(("CsrUsb::GetData - killed %x\n", GetLastError()));
                return 0;           
            }
#ifndef _WIN32_WCE
            status = GetOverlappedResult (handle, &overlapped, &written, FALSE);
#endif
        }
        else if (!status)
        {
            ERROR_MSG(("CsrUsb::GetData - failed %x\n", GetLastError()));
            close_connection_on_error  ();
            return 0;
        }

        if (status == 0)
        {
            written = 0;
        }
    }
    else
    {
        written = 0;
    }

    if (written == 0)
    {
        data_paused ++;
    }
    else
    {
        data_paused = 0;
    }

    return written;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DWORD WINAPI USBStack::monitor_data_thread (LPVOID arg)
{
    USBStack
        *stack;

    int
        offset,
        length,
        size;

    stack = (USBStack *) arg;

    // ''offset'' is the number of bytes in the buffer (always)
    // ''length'' is the length of the next event
    // ''size''   is the length of the last read from the driver

    size = 0;
    stack->monitor_data_thread_terminate = false;

    while (!stack->monitor_data_thread_terminate)
    {
        offset = size;      

        // If we have read less than 4 bytes then the length is not
        // yet valid.  It is a good idea to read this much before we
        // try to use the length
        while (offset < 4 && !stack->monitor_data_thread_terminate)
            {
            size = stack->get_hci_data (stack->data_buffer + offset, MAX_BULK_PACKET_SIZE);
            offset += size;
        }

        // Get the length of the next block (+4)
        length = (unsigned char) stack->data_buffer[2] + ((unsigned short) stack->data_buffer[3] << 8) + 4;

        // Now we read in the body of the data, we keep going until
        // we have read at least ''length'' bytes.
        while (offset < length && !stack->monitor_data_thread_terminate)
        {
            size = stack->get_hci_data (stack->data_buffer + offset, MAX_BULK_PACKET_SIZE);
            offset += size;
        }

        // send the data up the stack
        if (stack->data_func && !stack->monitor_data_thread_terminate)
        {
            stack->recv ((uint8*)stack->data_buffer, length);
            stack->data_func (stack->data_buffer, length, stack->data_func_context);
        }

        // If there is more data in the buffer that we have not read,
        // then copy it down and loop
        if(offset > length)
        {
            memmove(stack->data_buffer, stack->data_buffer + length, offset - length);
        }

        // Set the size of what is left in the buffer
        size = offset - length;
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void USBStack::monitor_data (void)
{
    DWORD
        thread_id;

    // assert(monitor_data_thread_handle == INVALID_HANDLE_VALUE);
    // This should perhaps close down the thread if one already exists
    monitor_data_thread_handle = CreateThread (0, 0, USBStack::monitor_data_thread, this, 0, &thread_id);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void USBStack::set_evt_callback (void (*func)(void *, size_t, void *), void *context)
{
    event_func = func;
    event_func_context = context;
}

void USBStack::set_acl_callback (void (*func)(void *, size_t, void *), void *context)
{
    data_func = func;
    data_func_context = context;
}

void USBStack::set_sco_callback (void (*func)(void *, size_t, void *), void *context)
{
    sco_func = func;
    sco_func_context = context;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

//#define SCO_CACHE_BUFFER_SIZE     ( 4096 )
#define SCO_REFORMAT_BUFFER_SIZE    ( 4096 )
#define SCO_WRITEIOCTL_THRESHOLD    ( (17*3) + 3 )
//unsigned char out_buffer[SCO_CACHE_BUFFER_SIZE];
//int out_buffer_offset = 0;
unsigned char fragment_buffer[SCO_REFORMAT_BUFFER_SIZE];
int fragment_buffer_offset = 0;
int fragment_buffer_good_offset = 0;
int fragmentation = 0;
unsigned char sample = 0;
#include <math.h>
unsigned char GenerateSample()
{
    static unsigned int offset = 0;
    static int direction = 0;
    static int byte = 0;
    unsigned char samplebyte;
//  unsigned int x;
//  float x;
#define PI 3.1415927

#ifdef SAMPLWRATE_16BIT
    if(byte == 0)
    {
    if(direction == 0)
    {
        offset++;
        if(offset == 12)
        {
            direction = 1;
        }
    }
    else
    {
        offset--;
        if(offset == 0)
        {
            direction = 0;
        }
    }
    }
    x = offset * 2000;
#endif

#ifdef SAMPLERATE_8BIT
    if(direction == 0)
    {
        offset++;
        if(offset == 12)
        {
            direction = 1;
        }
    }
    else
    {
        offset--;
        if(offset == 0)
        {
            direction = 0;
        }
    }
    samplebyte = offset * 20;
#endif
    //x = ((float) offset / 0xffff) * 2.00 * PI;

    //x = sin(x);

//  x = ((float)offset) * (500*2*PI/8000);
//  int amp = (int)((float)(1<<15) * sin(x));
#ifdef SAMPLERATE_16BIT
    if(byte == 0)
    {
        samplebyte = (((unsigned int)(x * 0xffff)) >> 8) & 0xff;
//      samplebyte = (((unsigned int)amp) >> 8) & 0xff;
        byte = 1;
    }
    else
    {
        samplebyte = ((unsigned int)(x * 0xffff)) & 0xff;
//      samplebyte = ((unsigned int)amp)  & 0xff;

        byte = 0;
//      offset++;
    }
#endif  

    return(samplebyte);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static inline uint16 get_handle(const uint8 * from)
{ return from[0] + (unsigned(from[1])<<8); }

static inline void set_handle(uint8 * to, uint16 handle)
{ to[0] = handle & 0xFF; to[1] = (handle >> 8) & 0xFF; }

int USBStack::send_sco(void *b, size_t length)
{
    if ( sco_status != no_sco )
	{
		size_t wrote = 0;
		uint8* buffer = (uint8*)b;
		if (handle != INVALID_HANDLE_VALUE)
            wrote = sco_send_buffer.add_packet(buffer,length);
		return wrote;
	}
    else
        return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


DWORD WINAPI USBStack::send_sco_thread (LPVOID arg)
{
    USBStack *stack = (USBStack *) arg;
    return stack->send_sco_round_robin();
}

void USBStack::do_send()
{
    //  send to do.
    bool data_to_send = (sco_send_buffer.fill_next() > 0);
    int buffer = -1;
    if ( data_to_send )
        buffer = sco_send_buffer.next_send();
    while( data_to_send && buffer != -1 && handle != INVALID_HANDLE_VALUE )
    {
        DWORD err;
        unsigned long wrote = 0;

        BOOL status = DeviceIoControl(// hDevice 
                                 handle,                
                                 // dwIoControlCode
                                 IOCTL_CSRBC_SEND_SCO_DATA,
                                 // lpInBuffer
                                 (void*)sco_send_buffer.get_data(buffer),
                                 // nInBufferSize
                                 sco_send_buffer.get_data_len(buffer),
                                 // lpOutBuffer
                                 0,
                                 // nOutBufferSize
                                 0,
                                 // lpBytesReturned
                                 &wrote,
                                 // lpOverlapped
                                 &sco_send_buffer.overlaps[buffer]);

        // its most likely the overlapped i/o operation is in progress
        if((!status) && ((err=GetLastError() != ERROR_IO_PENDING)))
        {
            printf("BAD STATUS in send_sco_round_robin (err=%08x)\n",err);
            close_connection_on_error();
        }
        else
        {
            sco_send_buffer.have_queued(buffer);
        }

        // try again
        data_to_send = (sco_send_buffer.fill_next() > 0);
        if ( data_to_send )
            buffer = sco_send_buffer.next_send();
    }
}

int USBStack::send_sco_round_robin()
{
    WaitForSingleObject(wait_for_start_sco,INFINITE);

    static const int wait_for_send_index = SCO_SEND_BUFFER_COUNT;
    static const int kill_thread_index = SCO_SEND_BUFFER_COUNT + 1;
    static const int handle_count = SCO_SEND_BUFFER_COUNT + 2;
    HANDLE handles[handle_count];
    memset(handles,0, sizeof handles);
    for( int j = 0 ; j < SCO_SEND_BUFFER_COUNT ; ++j )
        handles[j] = sco_send_buffer.wait_for_sent[j];
    handles[wait_for_send_index] = sco_send_buffer.wait_to_send;
    handles[kill_thread_index] = kill_threads_event;

    while (!monitor_sco_thread_terminate)
    {
        int rv = WaitForMultipleObjects(sizeof(handles)/sizeof(*handles),handles,FALSE,1000);
        if ( WAIT_OBJECT_0 <= rv && rv < WAIT_OBJECT_0 + handle_count )
        {
            //  react to kill_threads_event
            if ( rv == WAIT_OBJECT_0 + kill_thread_index )
                return 0x0abb;

            if ( !monitor_sco_thread_terminate )
            {
                //  figure out which sco channel, and which buffer is asking
                int event = (rv-WAIT_OBJECT_0);
                if ( event == wait_for_send_index )
                    do_send();
                else
                {
                    //  sent done
                    int buffer_number = event;
                    int status = 0;
                    unsigned long wrote = 0;
#if !defined _WINCE && !defined _WIN32_WCE
                    status = GetOverlappedResult(handle, &(sco_send_buffer.overlaps[buffer_number]), &wrote, TRUE);
#endif
                    if ( status )
                    {
                        sent(sco_send_buffer.get_data(buffer_number),wrote);
                        sco_send_buffer.have_sent(buffer_number);
                        do_send();
                    }
                    else
                    {
                        printf("BAD GetOverlappedResult STATUS in send_sco_round_robin (err=%08x)\n",GetLastError());
                        close_connection_on_error();
                    }
                }
            }
        }
        else if ( WAIT_FAILED == rv )
        {
            printf("WAIT_FAILED in send_sco_round_robin (err=%08x)\n", GetLastError());
            break;
        }
        else if ( WAIT_TIMEOUT != rv )
        {
            printf("WAIT_ABANDONED in send_sco_round_robin (event=%d, err=%08x)\n",rv - WAIT_ABANDONED_0, GetLastError());
        }
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void USBStack::monitor_sco (void)
{
    if ( sco_status != no_sco )
    {
        DWORD thread_id;
        monitor_sco_enabled = TRUE;
        monitor_sco_thread_terminate = false;
        monitor_sco_thread_handle = CreateThread (0, 0, USBStack::monitor_sco_thread, this, 0, &thread_id);
        send_sco_thread_handle = CreateThread (0, 0, USBStack::send_sco_thread, this, 0, &thread_id);
    }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#if !defined _WINCE && !defined _WIN32_WCE

DWORD WINAPI USBStack::monitor_sco_thread (LPVOID arg)
{
#if !defined _WINCE && !defined _WIN32_WCE
    USBStack *stack = (USBStack *) arg;
    if ( stack->sco_status != no_sco )
        return stack->monitor_sco_round_robin();
    else
#endif
        return 0;
}

int USBStack::monitor_sco_round_robin()
{
    unsigned paused_reads = 0;
    wait_for_sco_start();

    while ( !monitor_sco_thread_terminate )
    {
        // do nothing unless a calling app is interested in the data and an alternate interface
        // has been selected
        if( !sco_func || (sco_packet_size == 0) )
        {
            Sleep(1000);
            continue;
        }

        DWORD rv = WaitForMultipleObjects(SCO_RECV_BUFFER_COUNT,wait_for_recv_sco,FALSE,100);
        if ( rv >= WAIT_OBJECT_0 && rv < WAIT_OBJECT_0 + SCO_RECV_BUFFER_COUNT )
        {
            int buffer_number = rv - WAIT_OBJECT_0;
            process_sco_buffer(buffer_number);
			::ResetEvent(sco_overlaps[buffer_number].hEvent);
            if ( !monitor_sco_thread_terminate )
            {
                if ( !monitor_sco_thread_pausing )
                    request_sco_buffer(buffer_number);
                else
                    paused_reads++;
                if ( paused_reads == SCO_RECV_BUFFER_COUNT )
                {
                    paused_reads = 0;
                    ::SetEvent(wait_for_sco_paused);
                    for ( int i = 0 ; i < SCO_RECV_BUFFER_COUNT ; ++i )
                    {
                        delete[] sco_buffer[i];
                    }
                    wait_for_sco_start();
                }
            }
        }
        else if( rv != WAIT_TIMEOUT )
        {
            printf ("sco_round_robin: something terrible has happened %08x\n",rv);
            break;
        }
    }

    return 0;
}

void USBStack::wait_for_sco_start()
{
    WaitForSingleObject(wait_for_start_sco,INFINITE);
    // initialise the reads
    for ( int i = 0 ; i < SCO_RECV_BUFFER_COUNT ; ++i )
    {
        ::ResetEvent(sco_overlaps[i].hEvent);
        sco_buffer[i] = new unsigned char[SCO_BUFFER_SIZE];
        request_sco_buffer(i);
    }
}

int USBStack::request_sco_buffer( int buffer_number )
{
    int status;
    unsigned long read = 0;
    DWORD err;
    memset (sco_buffer[buffer_number],0,SCO_BUFFER_SIZE);
#ifdef DEBUG
    printf ("Requesting buffer %d\n",buffer_number);
#endif

    if(handle != INVALID_HANDLE_VALUE)
    {
		assert(WaitForSingleObject(sco_overlaps[buffer_number].hEvent,0) == WAIT_TIMEOUT);
        status = DeviceIoControl(// hDevice
                                 handle,
                                 // dwIoControlCode
                                 IOCTL_CSRBC_RECV_SCO_DATA_MD,
                                 // lpInBuffer
                                 0,
                                 // nInBufferSize
                                 0,
                                 // lpOutBuffer
                                 sco_buffer[buffer_number],
                                 // nOutBufferSize
                                 SCO_BUFFER_SIZE,
                                 // lpBytesReturned
                                 &read,
                                 // lpOverlapped
                                 &sco_overlaps[buffer_number]);

        // its possible the request has not completed yet so check if it is in progress
        if ((!status) && ((err = GetLastError ()) != ERROR_IO_PENDING))
        {
            printf("*** BAD STATUS in request_sco_buffer(%d) (err=%08x)!\n",buffer_number,err);
            close_connection_on_error();
        }
    }

    return read;
}

int USBStack::process_sco_buffer( int buffer_number )
{
    unsigned long read = 0;
    if( GetOverlappedResult(handle, &sco_overlaps[buffer_number], &read, TRUE) )
    {
        unsigned char * data = sco_buffer[buffer_number];
        unsigned char * end_of_buffer = data + read;
        while (  data < end_of_buffer  )
        {
            if ( sco_status == high_speed_sco )
            {
                // if the low word of the hci_handle is non-zero
                if ( *data )
                {
                    uint32 logical_len = data[2] + 3;
                    if ( logical_len > sco_packet_size )
                        logical_len = sco_packet_size;
                    sco_func(data,logical_len,sco_func_context);
                }
				data += sco_packet_size;
            }
            else
            {
                if ( sco_holding )
                {
                    uint32 logical_len = sco_holding[2] + 3;
					if ( logical_len == (sco_packet_size * 3) )
					{
						memcpy(sco_holding+offset_into_sco_holding,data,logical_len-offset_into_sco_holding);
						data += logical_len-offset_into_sco_holding;
						sco_func(sco_holding,logical_len,sco_func_context);
					}
                    delete[] sco_holding;
                    sco_holding = 0;
                    offset_into_sco_holding = 0;
                    //  should really be a "continue;" here, but it is pointless.
                }

                // check the length field.
                uint32 logical_len = data[2] + 3;
                if ( logical_len == (sco_packet_size * 3) )
                {
                    if ( data + logical_len < end_of_buffer )
                    {
                        sco_func(data,logical_len,sco_func_context);
                        data += logical_len;
                    }
                    else
                    {
                        assert(sco_holding==0);
                        sco_holding = new unsigned char[logical_len];
						offset_into_sco_holding = end_of_buffer-data;
                        memcpy(sco_holding,data,offset_into_sco_holding);
                        data = end_of_buffer;
                    }
                }
				else
				{
					//  skip forwards
					data += sco_packet_size;
				}
            }
        }
		recv(sco_buffer[buffer_number],read);
    }
    else
    {
        printf("GetOverlappedResult failed (err = %08x)\n",GetLastError());
    }
    return read;
}

#endif
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/**********************************************************************
**
**  sco_send_buffer_t
**
**********************************************************************/

USBStack::sco_send_buffer_t::sco_send_buffer_t()
:   sco_packet_size(0), packets_per_header(1)
{
    for( int i = 0; i < SCO_SEND_BUFFER_COUNT; ++i)
    {
		//  each buffer should be largest size we could need
        data[i] = new uint8[SCO_SEND_BUFFER_SIZE];
		assert(data[i]);
        wait_for_sent[i] = CreateEvent(0,0,0,0);
        overlaps[i].hEvent = wait_for_sent[i];
        writable_buffers.push_back(i);
    }
    wait_to_send = CreateEvent(0,0,0,0);
    InitializeCriticalSection(&protect_queues);
}

USBStack::sco_send_buffer_t::~sco_send_buffer_t()
{
    for( int i = 0; i < SCO_SEND_BUFFER_COUNT; ++i)
    {
        CloseHandle(wait_for_sent[i]);
        delete[] data[i];
    }
    CloseHandle(wait_to_send);
    DeleteCriticalSection(&protect_queues);
}

void USBStack::sco_send_buffer_t::set_sco_packet_size(uint32 size)
{
    ::EnterCriticalSection(&protect_queues);
    sco_packet_size = size;
    ::LeaveCriticalSection(&protect_queues);
}

void USBStack::sco_send_buffer_t::set_packets_per_header(unsigned int num)
{
    ::EnterCriticalSection(&protect_queues);
    packets_per_header = num;
    ::LeaveCriticalSection(&protect_queues);
}

// inc ntw
size_t USBStack::sco_send_buffer_t::add_packet( const uint8* buffer, size_t length )
{
    size_t wrote = 0;
    ::EnterCriticalSection(&protect_queues);
    wrote = data_queues[get_handle(buffer)].append(buffer+3,buffer+length);
    ::LeaveCriticalSection(&protect_queues);
    if ( wrote )
        SetEvent(wait_to_send);
    return wrote;
}

const uint8 * USBStack::sco_send_buffer_t::get_data(int buffer)
{
    return data[buffer];
}

unsigned int USBStack::sco_send_buffer_t::get_data_len(int buffer)
{
	assert(SCO_PACKETS_PER_BUFFER % packets_per_header == 0);
	return sco_packet_size * SCO_PACKETS_PER_BUFFER;
}

size_t USBStack::sco_send_buffer_t::fill( int buffer_to_fill )
{
    int i = 0 ;
    ::EnterCriticalSection(&protect_queues);
    if ( !data_queues.empty() )
    {
        std::map<int,data_list>::iterator m = data_queues.begin();
		assert(SCO_PACKETS_PER_BUFFER % packets_per_header == 0);
        while( m != data_queues.end() && i*packets_per_header != SCO_PACKETS_PER_BUFFER )
        {
			uint16 handle = m->first;
			data_list& source = m->second;
            uint8 * start = data[buffer_to_fill]+(i * sco_packet_size * packets_per_header);
            set_handle(start,handle);
            start[2] = uint8(source.grab(start+3,sco_packet_size * packets_per_header - 3));
			if(start[2] == 0)
			{
				data_queues.erase(m);
				m = data_queues.begin();
            }
			else
			{
				++m;
				if( m == data_queues.end() )
					m = data_queues.begin();
			}
            ++i;
        }
        while( i * packets_per_header != SCO_PACKETS_PER_BUFFER )
		{
            uint8 * start = data[buffer_to_fill]+(i * sco_packet_size * packets_per_header);
			memset(start,0, sco_packet_size * packets_per_header);
			++i;
		}
//        printf("filled %d\n",buffer_to_fill);
    }
    else
    {
//        printf("no data for %d\n",buffer_to_fill);
    }
    ::LeaveCriticalSection(&protect_queues);
    return i;
}

size_t USBStack::sco_send_buffer_t::fill_next()
{
    int wrote = -1;
    if ( !writable_buffers.empty() )
    {
        wrote = fill(writable_buffers.front());
        if( wrote )
            have_written(writable_buffers.front());
    }
    return wrote;
}

int USBStack::sco_send_buffer_t::next_send()
{
    int rv = queueable_buffers.empty() ? -1 : queueable_buffers.front();
//    printf("next_send() = %d\n",rv);
    return rv;
}

void move(std::list<int>& l1 , std::list<int>& l2 , int i )
{
    if (l1.front() != i)
	{
		int first = l1.front();
//		printf("%d:",i);
		do
		{
			l1.push_back(l1.front());
			l1.pop_front();

			if ( l1.front() == first )
				abort();
		}
		while ( l1.front() != i );
	}
	l1.pop_front();
    l2.push_back(i);
}

void USBStack::sco_send_buffer_t::have_written( int buffer_number )
{
//    printf("have_written(%d)\n",buffer_number);
	move(writable_buffers,queueable_buffers,buffer_number);
}

void USBStack::sco_send_buffer_t::have_sent( int buffer_number )
{
 //   printf("have_sent(%d)\n",buffer_number);
	move(queued_buffers,writable_buffers,buffer_number);
}

void USBStack::sco_send_buffer_t::have_queued( int buffer_number )
{
 //   printf("have_queued(%d)\n",buffer_number);
	move(queueable_buffers,queued_buffers,buffer_number);
}
