// Module to handle USB transport.
// Copyright (c) 2000-2006 Cambridge Silicon Radio.  All Rights Reserved.

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <winioctl.h>
#include <stdio.h>

#include "../wid_usb.h"

// The widcomm driver doesn't support overlapped IO, so 
// event, sco and acl reads will have to be in threads.

// There apears to me symetry between the routines for acl, sco and evt.
// They are NOT identicle (the length is different).

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USBWidStack::USBWidStack ( const USBConfiguration & aConfig )
:
    USBAbstraction ( aConfig )
{
	evt_handle = INVALID_HANDLE_VALUE;
	acl_handle = INVALID_HANDLE_VALUE;
	sco_handle = INVALID_HANDLE_VALUE;
	cmd_handle = INVALID_HANDLE_VALUE;

	monitor_evt_thread_handle = INVALID_HANDLE_VALUE;
	monitor_acl_thread_handle = INVALID_HANDLE_VALUE;
	monitor_sco_thread_handle = INVALID_HANDLE_VALUE;

	wait_for_evt = CreateEvent (0, 0, 0, 0);
	wait_for_sco = CreateEvent (0, 0, 0, 0);
	wait_for_acl = CreateEvent (0, 0, 0, 0);

	wait_for_send_acl = CreateEvent (0, 0, 0, 0);
	wait_for_send_sco = CreateEvent (0, 0, 0, 0);
	wait_for_send_cmd = CreateEvent (0, 0, 0, 0);

	kill_threads_event = CreateEvent (0, 0, 0, 0);

	InitializeCriticalSection(&close_handle_crit);

	// > event size + interrupt data payload size
	evt_buffer = new char[2 * Max_evt_Packet];
	// > ACL data size + bulk data payload size
	acl_buffer = new char[2 * Max_acl_Packet];
	// > SCO data size + bulk data payload size
	sco_buffer = new char[2 * Max_sco_Packet];

	evt_func = 0;
	acl_func = 0;
	sco_func = 0;

	evt_func_context = 0;
	acl_func_context = 0;
	sco_func_context = 0;

	flushing_streams = 3;

    mutex_ = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USBWidStack::~USBWidStack ()
{
    close_connection();

	delete evt_buffer;
	delete acl_buffer;
	delete sco_buffer;

	CloseHandle (wait_for_evt);
	CloseHandle (wait_for_acl);
	CloseHandle (wait_for_sco);
	CloseHandle (wait_for_send_acl);
	CloseHandle (wait_for_send_cmd);
	CloseHandle (wait_for_send_sco);
	CloseHandle (kill_threads_event);

	DeleteCriticalSection(&close_handle_crit);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool USBWidStack::open_connection ()
{
    unsigned long	written;
    BOOL status;

    // Mutex made redundant by version 1.5.7 of Widcomm USB driver, kept for compatibility with previous versions.
    if ( mConfig.nameIsWide() )
    {
        mutex_ = new SystemWideMutex(&mConfig.getNameW()[4], true);
        if (!mutex_->IsAcquired())
        {
	        delete mutex_;
	        mutex_ = 0;
	        return false;
        }
    }


    if (!mConfig.nameIsWide())
    {
        const char * lName = mConfig.getNameA();
        evt_handle = CreateFileA
        (
            lName,
            GENERIC_READ,
            0,
            0,
            OPEN_EXISTING,
            0,
            0
        );
        if (evt_handle == INVALID_HANDLE_VALUE)
            return false;

        acl_handle = CreateFileA
        (
            lName,
            GENERIC_READ | GENERIC_WRITE,
            0,
            0,
            OPEN_EXISTING,
            0,
            0
        );

        sco_handle = CreateFileA
        (
            lName,
            GENERIC_READ | GENERIC_WRITE,
            0,
            0,
            OPEN_EXISTING,
            0,
            0
        );

        cmd_handle = CreateFileA
        (
            lName,
            GENERIC_WRITE,
            0,
            0,
            OPEN_EXISTING,
            0,
            0
        );
    }
    else
    {
        const wchar_t * lName = mConfig.getNameW();
        evt_handle = CreateFileW
        (
            lName,
            GENERIC_READ,
            0,
            0,
            OPEN_EXISTING,
            0,
            0
        );
        if (evt_handle == INVALID_HANDLE_VALUE)
            return false;

        acl_handle = CreateFileW
        (
            lName,
            GENERIC_READ | GENERIC_WRITE,
            0,
            0,
            OPEN_EXISTING,
            0,
            0
        );

        sco_handle = CreateFileW
        (
            lName,
            GENERIC_READ | GENERIC_WRITE,
            0,
            0,
            OPEN_EXISTING,
            0,
            0
        );

        cmd_handle = CreateFileW
        (
            lName,
            GENERIC_WRITE,
            0,
            0,
            OPEN_EXISTING,
            0,
            0
        );
    }

    if (  (acl_handle == INVALID_HANDLE_VALUE)
       || (sco_handle == INVALID_HANDLE_VALUE)
       || (cmd_handle == INVALID_HANDLE_VALUE) )
	    return false;

    // Kick Widcomm stack off the device driver.  We want it for ourselves.
    status = DeviceIoControl (
                evt_handle,
                IOCTL_BTWUSB_SET_EXCLUSIVE_ACCESS,
                0,
                0,
                0,
                0,
                &written,
                0);
    if (!status) // Failed to get exclusive access on device.  
        return false;

    monitor_evt();
    monitor_acl();
    monitor_sco();

    return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool USBWidStack::is_open (void)
{
	return (acl_handle != INVALID_HANDLE_VALUE);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void USBWidStack::close_connection (void)
{

	close_connection_on_error();

	if (monitor_evt_thread_handle != INVALID_HANDLE_VALUE) 
	{
  	  if (WaitForSingleObject(monitor_evt_thread_handle, 2000) == WAIT_TIMEOUT)
	    TerminateThread (monitor_evt_thread_handle, 0);
	  CloseHandle(monitor_evt_thread_handle);
	  monitor_evt_thread_handle = INVALID_HANDLE_VALUE;
	}

    if (monitor_acl_thread_handle != INVALID_HANDLE_VALUE) 
	{
  	  if (WaitForSingleObject(monitor_acl_thread_handle, 2000)  == WAIT_TIMEOUT)
	    TerminateThread (monitor_acl_thread_handle, 0);
	  CloseHandle(monitor_acl_thread_handle);
 	  monitor_acl_thread_handle = INVALID_HANDLE_VALUE;
	}

	if (monitor_sco_thread_handle != INVALID_HANDLE_VALUE) 
	{
  	  if (WaitForSingleObject(monitor_sco_thread_handle, 2000) == WAIT_TIMEOUT)
	    TerminateThread (monitor_sco_thread_handle, 0);
	  CloseHandle(monitor_sco_thread_handle);
      monitor_sco_thread_handle = INVALID_HANDLE_VALUE;
	}
}



void USBWidStack::close_handles(void)
{
	EnterCriticalSection(&close_handle_crit);

	BOOL status;
	unsigned long written = 0;
	if (evt_handle != INVALID_HANDLE_VALUE)
	{
    // permit the Widcomm stack th use the USB driver again.
	status = DeviceIoControl (
		evt_handle,
		IOCTL_BTWUSB_RESET_EXCLUSIVE_ACCESS,
		0,
		0,
		0,
		0,
		&written,
		0);

		CloseHandle (evt_handle);
		evt_handle = INVALID_HANDLE_VALUE;
	}
	if (acl_handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle (acl_handle );
		acl_handle = INVALID_HANDLE_VALUE;
	}
	if (sco_handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle (sco_handle);
		sco_handle = INVALID_HANDLE_VALUE;
	}
	if (cmd_handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle (cmd_handle);
		cmd_handle = INVALID_HANDLE_VALUE;
	}
	delete mutex_;
	mutex_ = 0;
	LeaveCriticalSection(&close_handle_crit);
}

void USBWidStack::close_connection_on_error (void)
{
	if (    (monitor_evt_thread_handle != INVALID_HANDLE_VALUE) 
		||  (monitor_acl_thread_handle != INVALID_HANDLE_VALUE)
		||  (monitor_sco_thread_handle != INVALID_HANDLE_VALUE))
	{
	    monitor_evt_thread_terminate = true;
	    monitor_acl_thread_terminate = true;
	    monitor_sco_thread_terminate = true;
		SetEvent(kill_threads_event);
	}
	close_handles();
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int USBWidStack::send_cmd (void *buffer, size_t length)
{
    sent ( (uint8*) buffer , length ); 
	int
		status;
//	static ichar debug_buf[100];

	unsigned long
		written = 0;

	while (flushing_streams > 0)
	{
		Sleep(10);
	}

	if (evt_handle != INVALID_HANDLE_VALUE)
	{
		status = DeviceIoControl
		(
			cmd_handle,
			IOCTL_BTWUSB_PUT_CMD,
			buffer,
			length,
			0,
			0,
			&written,
			0
		);

		if (status == 0)
		{
			written = 0;
			// OutputDebugString(II("send_cmd : Written Nowt\n"));
		}
#ifdef DEBUG_WID_USB
		else
		{
			if (length > 0)
			{
				isprintf(debug_buf, 100, II("send_cmd : Written %d bytes\n"), written);
				OutputDebugString(debug_buf);
				for (unsigned long i = 0; i < length; i++)
				{
					isprintf(debug_buf, 100, II("%02x "), ((char *)buffer)[i] & 0xff );
					OutputDebugString(debug_buf);
				}
				OutputDebugString(II("\n"));
			}
		}
		
#endif
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

int USBWidStack::send_acl (void *data, size_t length)
{
    sent ( (uint8*) data , length ); 
	int
		status;

	unsigned long
		written;

	while (flushing_streams > 0)
	{
		Sleep(10);
	}

	if (evt_handle != INVALID_HANDLE_VALUE)
	{
		status = DeviceIoControl
		(
			acl_handle,
			IOCTL_BTWUSB_PUT,
			data,
			length,
			0,
			0,
			&written,
			0
		);

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

int USBWidStack::send_sco (void *data, size_t length)
{
    sent ( (uint8*) data , length ); 
	int
		status;

	unsigned long
		written;

	while (flushing_streams > 0)
	{
		Sleep(10);
	}

	if (evt_handle != INVALID_HANDLE_VALUE)
	{
		status = DeviceIoControl
		(
			sco_handle,
			IOCTL_BTWUSB_PUT_VOICE,
			data,
			length,
			0,
			0,
			&written,
			0
		);

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

int USBWidStack::get_evt (void *buffer, int length)
{
	int
		status;
//	static ichar debug_buf[100];

	unsigned long
		num_read = 0;

	int timeout = StreamPollTime;

	status = DeviceIoControl
	(
		evt_handle,
		IOCTL_BTWUSB_GET_EVENT,
		&timeout,
		sizeof (timeout),
		buffer,
		length,
		&num_read,
		0
	);
	if (!status)
	{
		// OutputDebugString(II("get_evt : bad status\n"));
		num_read = 0;
	}
#ifdef DEBUG_WID_USB
	if (num_read > 0)
	{
		isprintf(debug_buf, 100, II("get_evt : Read %d bytes\n"), num_read);
		OutputDebugString(debug_buf);
		for (unsigned long i = 0; i < num_read; i++)
		{
			isprintf(debug_buf, 100, II("%02x "), ((char *)buffer)[i] & 0xff );
			OutputDebugString(debug_buf);
		}
		OutputDebugString(II("\n"));

	}
#endif
	return num_read;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int USBWidStack::get_acl (void *buffer, int length)
{
	int
		status;

	unsigned long
		num_read = 0;

	int timeout = StreamPollTime;

	status = DeviceIoControl
	(
		acl_handle,
		IOCTL_BTWUSB_GET,
		&timeout,
		sizeof (timeout),
		buffer,
		length,
		&num_read,
		0
	);
	if (!status)
		num_read = 0;

	return num_read;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int USBWidStack::get_sco (void *buffer, int length)
{
	int
		status;

	unsigned long
		num_read = 0;

	int timeout = StreamPollTime;

	status = DeviceIoControl
	(
		sco_handle,
		IOCTL_BTWUSB_GET_VOICE,
		&timeout,
		sizeof (timeout),
		buffer,
		length,
		&num_read,
		0
	);
	if (!status)
		num_read = 0;

	return num_read;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DWORD USBWidStack::monitor_evt_thread (void)
{
//	static ichar debug_buf[100];

	int
		offset,
		length;

	monitor_evt_thread_terminate = false;

	// ''offset'' is the number of bytes in the buffer (always)
	// ''length'' is the length of the next event
	// ''size''   is the length of the last read from the driver

	// The ''flushing'' is now handled with two loops.  The first loop
	// is when we are flushing.  We keep reading from the driver until we
	// read a zero length packet.  This indicates that we should stop
	// flushing.

	// We dont want any of the data in this loop!
	while (!monitor_evt_thread_terminate)
	{
		int size = get_evt (evt_buffer, Max_evt_Packet);
		if (size == 0)
			break;
	}

	InterlockedDecrement(&flushing_streams);
	// OutputDebugString(II("Flushed evt\n"));

	offset = 0;

	while (!monitor_evt_thread_terminate)
	{
		// If we only have a small amount of data in the buffer, then
		// we need to read in another chunk before we know the length
		while (offset < 2 && !monitor_evt_thread_terminate)
		{
			int size = get_evt (evt_buffer + offset, Max_evt_Packet);
			offset += size;
		}

		// We now know the length of the current packet
		length = (unsigned char) evt_buffer[1] + 2;

		// Read in the entire packett (plus perhaps the start of the next one)
		while (offset < length && !monitor_evt_thread_terminate)
		{
			int size = get_evt (evt_buffer + offset, Max_evt_Packet);
			offset += size;
		}

		// Send the packet up
		if (evt_func && !monitor_evt_thread_terminate)
		{
            recv ( (uint8*)evt_buffer , length );
			evt_func (evt_buffer, length, evt_func_context);
		}
#ifdef DEBUG_WID_USB
		else
		{
			isprintf(debug_buf, 100, II("monitor_evt_thread : Read and discarded %d bytes\n"), length);
			OutputDebugString(debug_buf);
			for (int i = 0; i < length; i++)
			{
				isprintf(debug_buf, 100, II("%02x "), ((char *)evt_buffer)[i] & 0xff );
				OutputDebugString(debug_buf);
			}
			OutputDebugString(II("\n"));		
		}
#endif
		// If there is more data in the buffer that we have not read,
		// then copy it down, update the remaining size, and loop
		if(offset > length)
		{
			memmove(evt_buffer, evt_buffer + length, offset - length);
		}
		offset -= length;
	}

	return 0;
}

DWORD WINAPI USBWidStack::__monitor_evt_thread (LPVOID arg)
{
	USBWidStack *stack = (USBWidStack *) arg;
	return stack->monitor_evt_thread();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DWORD USBWidStack::monitor_acl_thread (void)
{
	int
		offset,
		length;

	monitor_acl_thread_terminate = false;

	// ''offset'' is the number of bytes in the buffer (always)
	// ''length'' is the length of the next event
	// ''size''   is the length of the last read from the driver

	// We dont want any of the data in this loop!
	while (!monitor_acl_thread_terminate)
	{
		int size = get_acl (acl_buffer, Max_acl_Packet);
		if (size == 0)
			break;
	}

	InterlockedDecrement(&flushing_streams);
	// OutputDebugString(II("Flushed acl\n"));

	offset = 0;

	while (!monitor_acl_thread_terminate)
	{
		// If we only have a small amount of data in the buffer, then
		// we need to read in another chunk before we know the length
		while (offset < 4 && !monitor_acl_thread_terminate)
		{
			int size = get_acl (acl_buffer + offset, Max_acl_Packet);
			offset += size;
		}

		// We now know the length of the current packet
		length = (unsigned char) acl_buffer[2] + ((unsigned short) acl_buffer[3] << 8) + 4;

		// Read in the entire packet (plus perhaps the start of the next one)
		while (offset < length && !monitor_acl_thread_terminate)
		{
			int size = get_acl (acl_buffer + offset, Max_acl_Packet);
			offset += size;
		}

		// Send the packet up
		if (acl_func && !monitor_acl_thread_terminate)
		{
            recv ( (uint8*)acl_buffer , length );
			acl_func (acl_buffer, length, acl_func_context);
		}

		// If there is more data in the buffer that we have not read,
		// then copy it down, update the remaining offset, and loop
		if(offset > length)
		{
			memmove(acl_buffer, acl_buffer + length, offset - length);
		}
		offset -= length;
	}

	return 0;
}

DWORD WINAPI USBWidStack::__monitor_acl_thread (LPVOID arg)
{
	USBWidStack *stack = (USBWidStack *) arg;
	return stack->monitor_acl_thread();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DWORD USBWidStack::monitor_sco_thread (void)
{
	int
		offset,
		length;

	monitor_sco_thread_terminate = false;

	// ''offset'' is the number of bytes in the buffer (always)
	// ''length'' is the length of the next event
	// ''size''   is the length of the last read from the driver

	// We dont want any of the data in this loop!
	while (!monitor_sco_thread_terminate)
	{
		int size = get_sco (sco_buffer, Max_sco_Packet);
		if (size == 0)
			break;
	}

	InterlockedDecrement(&flushing_streams);
	// OutputDebugString(II("Flushed sco\n"));

	offset = 0;

	while (!monitor_sco_thread_terminate)
	{
		// If we only have a small amount of data in the buffer, then
		// we need to read in another chunk before we know the length
		while (offset < 3 && !monitor_sco_thread_terminate)
		{
			int size = get_sco (sco_buffer + offset, Max_sco_Packet);
			offset += size;
		}

		// We now know the length of the current packet
		length = (unsigned char)sco_buffer[2] + 3;

		// Read in the entire packett (plus perhaps the start of the next one)
		while (offset < length && !monitor_sco_thread_terminate)
		{
			int size = get_sco (sco_buffer + offset, Max_sco_Packet);
			offset += size;
		}

		// Send the packet up
		if (sco_func && !monitor_sco_thread_terminate)
		{
            recv ( (uint8*)sco_buffer, length);
			sco_func (sco_buffer, length, sco_func_context);
		}

		// If there is more data in the buffer that we have not read,
		// then copy it down, update the remaining size, and loop
		if(offset > length)
		{
			memmove(sco_buffer, sco_buffer + length, offset - length);
		}
		offset -= length;
	}

	return 0;
}

DWORD WINAPI USBWidStack::__monitor_sco_thread (LPVOID arg)
{
	USBWidStack *stack = (USBWidStack *) arg;
	return stack->monitor_sco_thread();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void USBWidStack::monitor_evt (void)
{
	DWORD
		thread_id;

	// assert(monitor_data_thread_handle == INVALID_HANDLE_VALUE);
	// This should perhaps close down the thread if one already exists
	monitor_evt_thread_handle = CreateThread (0, 0, USBWidStack::__monitor_evt_thread, this, 0, &thread_id);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void USBWidStack::monitor_acl (void)
{
	DWORD
		thread_id;

	// assert(monitor_data_thread_handle == INVALID_HANDLE_VALUE);
	// This should perhaps close down the thread if one already exists
	monitor_acl_thread_handle = CreateThread (0, 0, USBWidStack::__monitor_acl_thread, this, 0, &thread_id);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void USBWidStack::monitor_sco (void)
{
	DWORD
		thread_id;

	// assert(monitor_data_thread_handle == INVALID_HANDLE_VALUE);
	// This should perhaps close down the thread if one already exists
	monitor_sco_thread_handle = CreateThread (0, 0, USBWidStack::__monitor_sco_thread, this, 0, &thread_id);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


void USBWidStack::set_evt_callback (void (*func)(void *, size_t, void *), void *context)
{
	evt_func = func;
	evt_func_context = context;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void USBWidStack::set_acl_callback (void (*func)(void *, size_t, void *), void *context)
{
	acl_func = func;
	acl_func_context = context;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void USBWidStack::set_sco_callback (void (*func)(void *, size_t, void *), void *context)
{
	sco_func = func;
	sco_func_context = context;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
