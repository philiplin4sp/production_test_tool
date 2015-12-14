/*++

Copyright (c) 1997-1998  Microsoft Corporation

Module Name:

    CSRBC01.h

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

#ifndef HEADER_CSR_USB_H
#define HEADER_CSR_USB_H

#include "../../../usbDeviceDriver/sys/csrbc_usb.h"

// CSR unicode macros.
#include "common/nocopy.h"
#include "usb.h"
#include <map>
#include <list>

struct data_list
{
public:
    size_t append(const unsigned char* begin, const unsigned char* end)
    {
        block b;
        size_t size = end - begin;
        b.buffer = b.read = new uint8[size];
        if ( b.buffer )
        {
            memcpy(b.buffer,begin,size);
            b.end = b.buffer + size;
            mlist.push_back(b);
            return size;
        }
        else
            return 0;
    }
    size_t grab(unsigned char* toFill, size_t size)
    {
        size_t grabbed = 0;
        while ( grabbed < size && !mlist.empty() )
        {
            block& b = mlist.front();
            size_t to_grab_from_this_block = size - grabbed;
            size_t block_size = b.end - b.read;
            if ( to_grab_from_this_block > block_size )
                to_grab_from_this_block = block_size;
            memcpy(toFill+grabbed,b.read,to_grab_from_this_block);
            b.read += to_grab_from_this_block;
            if ( b.read == b.end )
            {
                delete[] b.buffer;
                mlist.pop_front();
            }
            grabbed += to_grab_from_this_block;
        }
        return grabbed;
    }
private:
    struct block
    {
        unsigned char* buffer;
        unsigned char* end;
        unsigned char* read;
    };
    std::list<block> mlist;
};

static const int SCO_RECV_BUFFER_COUNT = 3;
static const int SCO_SEND_BUFFER_COUNT = 5;

class USBStack : public USBAbstraction
{
private:
    HANDLE	handle;
    unsigned int driver_version;
    HANDLE	wait_for_event;
    HANDLE	wait_for_data;
    HANDLE	wait_for_acl;
    HANDLE	wait_for_start_sco;
    HANDLE	wait_for_send_sco; 
    HANDLE	wait_for_recv_sco[SCO_RECV_BUFFER_COUNT];
    HANDLE  wait_for_sco_paused;
    OVERLAPPED sco_overlaps[SCO_RECV_BUFFER_COUNT];

    HANDLE	wait_for_command;
    HANDLE	kill_threads_event;

    CRITICAL_SECTION close_handle_crit;

    bool monitor_events_thread_terminate;
    bool monitor_data_thread_terminate;
    bool monitor_sco_thread_terminate;
    bool monitor_sco_thread_pausing;
    bool monitor_sco_enabled;

    int	event_paused;
    int	data_paused;

	UCHAR max_packet_size;

    uint32 sco_packet_size;

    unsigned char *event_buffer;
    unsigned char *data_buffer;
    unsigned char *sco_buffer[SCO_RECV_BUFFER_COUNT];
    unsigned char *sco_holding;
    UINT_PTR offset_into_sco_holding;

    HANDLE monitor_events_thread_handle;
    HANDLE monitor_data_thread_handle;
    HANDLE monitor_sco_thread_handle;
    HANDLE send_sco_thread_handle;

    void (*event_func) (void *data, size_t length, void *context);
    void (*data_func) (void *data, size_t length, void *context);
    void (*sco_func) (void *data, size_t length, void *context);

    void *event_func_context;
    void *data_func_context;
    void *sco_func_context;

    int get_hci_event(void *buffer, int length);
    static DWORD WINAPI USBStack::monitor_events_thread(LPVOID arg);

    int get_hci_data(void *buffer, int length);
    static DWORD WINAPI USBStack::monitor_data_thread(LPVOID arg);

    //  SCO functions:
    enum { no_sco, full_speed_sco, high_speed_sco } sco_status;
    int get_sco_data(void *buffer, unsigned long length);
    static DWORD WINAPI USBStack::monitor_sco_thread(LPVOID arg);
    //  called by monitor_sco_thread to use multiple buffers.
    int monitor_sco_round_robin();
    void wait_for_sco_start();
    int process_sco_buffer( int buffer_number );
    int request_sco_buffer( int buffer_number );

    static DWORD WINAPI USBStack::send_sco_thread(LPVOID arg);
    int send_sco_round_robin();
    void do_send();

    //  A circular buffer of buffers
    class sco_send_buffer_t
    {
    public:
        sco_send_buffer_t();
        ~sco_send_buffer_t();

        //  queue data to send
        size_t add_packet( const uint8* buffer, size_t length );

        //  initialise a buffer to send
        //  (returns zero if there is no more data to send).
        size_t fill_next();
        //  which one was it?
        int next_send();

        //  tell the state machine that buffers are queued or sent
        void have_queued( int buffer_number );
        void have_sent( int buffer_number );

        //  keep the buffer filling code up to date with the latest
        //  alternate interface and packet size.  This is atomic
        void set_sco_packet_size(uint32 size);
		void set_packets_per_header(unsigned int num);
        
        //  access to data I want to hold in this object, but which
        //  are conceptually part of the parent.
        const uint8 * get_data(int buffer);
		unsigned int get_data_len(int buffer);
        OVERLAPPED overlaps[SCO_SEND_BUFFER_COUNT];
	    HANDLE wait_for_sent[SCO_SEND_BUFFER_COUNT];
        HANDLE wait_to_send;
    private:
        uint32 sco_packet_size;
		unsigned int packets_per_header;
        //  buffers to be offered to the driver
        uint8 * data[SCO_SEND_BUFFER_COUNT];
        size_t fill( int buffer_number );
        //  tell the state machine that the buffer is full of data
        //  waiting to be sent
        void have_written( int buffer_number );
        std::list<int> writable_buffers;
        std::list<int> queueable_buffers;
        std::list<int> queued_buffers;

        //  data queuing to be sent
        std::map<int,data_list> data_queues;
        CRITICAL_SECTION protect_queues;
    } sco_send_buffer;


    void monitor_events(void);
    void monitor_data(void);
    void monitor_sco(void);
    void close_handle(void);
    void close_connection_on_error(void);
    void set_alternate_setting(int num);

public:

    USBStack ( const USBConfiguration & aConfig );
    ~USBStack ();

    bool open_connection ();
    void close_connection (void);

    bool is_open (void);

	bool size_forbidden (unsigned long);
    int send_cmd (void *data, size_t length);
    int send_acl (void *data, size_t length);
    int send_sco (void *buffer, size_t length);

    void set_evt_callback (void (*func) (void *, size_t, void *), void *context);
    void set_acl_callback (void (*func) (void *, size_t, void *), void *context);
    void set_sco_callback (void (*func)(void *, size_t, void *), void *context);

    bool set_sco_bandwidth(int bytes_per_second);
};



#endif // HEADER_CSR_USB_H

