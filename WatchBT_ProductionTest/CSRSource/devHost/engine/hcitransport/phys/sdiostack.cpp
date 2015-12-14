///////////////////////////////////////////////////////////////////////
//
//  FILE   :  sdiostack.cpp
//
//  AUTHOR :  Adam Hughes
//
//  CLASS  :  SDIOStack
//
//  PURPOSE:  Implement the SDIO type A transport spec.
//
///////////////////////////////////////////////////////////////////////

#ifdef _WIN32_WCE
#error SDIO not supported on CE
#endif

#include <cassert>

#ifdef _WIN32
#include <windows.h>
#include "sdio_ioctl.h"
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#endif

#include "sdiostack.h"
#include "unicode/ichar.h"

#ifdef _WIN32
#define UNIRAW_BLOCK_TRANSFER 0x08000000
#endif

SDIOStack::SDIOStack(const name_store& lDevice)
    : callbacks(0), device(lDevice),
#ifdef _WIN32
      handle(NULL),
#else
      fd(-1),
#endif
      opened(false), read_thread(0)
{
}

SDIOStack::~SDIOStack()
{
    stop();
}


bool SDIOStack::start()
{
    if (!opened) 
    {
#ifdef _WIN32    
        if (device.isW())
        {
            if (wcslen(device.getW()) < 5)
            {
                return false;
            }
            const std::wstring driverDeviceName(L"\\\\.\\UniRawsdioTypeA" + std::wstring(device.getW() + 4));
            handle = CreateFileW(driverDeviceName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
        }
        else
        {
            if (strlen(device.getA()) < 5)
            {
                return false;
            }
            const std::string driverDeviceName("\\\\.\\UniRawsdioTypeA" + std::string(device.getA() + 4));
            handle = CreateFileA(driverDeviceName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
        }
        if (handle == INVALID_HANDLE_VALUE) {
            return false;
        }
#else
        const char *deviceStr(device.isA() ? device.getA() : inarrow(std::wstring(device.getW())).c_str());
        fd = open(deviceStr, O_RDWR);
        if (fd < 0) {
            perror("open");
            return false;
        }
#endif
        read_thread = new reader(*this);
        if (read_thread->Start())
        {
            opened = true;
            return true;
        }
        else
        {
            // Failed to start thread
            opened = false;
            delete read_thread;
            read_thread = 0;
            return false;
        }
    }
    else
    {
        return true;
    }
}

bool SDIOStack::ready( uint32 timeout )
{
    return opened;
}

void SDIOStack::stop()
{
    if (!opened)
        return;

    delete read_thread;
#ifdef _WIN32
    CloseHandle(handle);
    handle = 0;
#else
    close(fd);
#endif
    opened = false;
}

#define SDIO_BT_TYPE_A_HEADER_LEN 4
#define SDIO_BT_TYPE_A_PACKET_LEN_MAX 65543

#define SDIO_BT_TYPE_A_SID_HCI_COMMAND 0x01
#define SDIO_BT_TYPE_A_SID_ACL         0x02
#define SDIO_BT_TYPE_A_SID_SCO         0x03
#define SDIO_BT_TYPE_A_SID_HCI_EVENT   0x04
#define SDIO_BT_TYPE_A_SID_VENDOR      0xfe

void SDIOStack::sendCommand( const uint8* data, size_t length )
{
    sendPacket(SDIO_BT_TYPE_A_SID_HCI_COMMAND, data, length);
}

void SDIOStack::sendACL( const uint8* data, size_t length )
{
    sendPacket(SDIO_BT_TYPE_A_SID_ACL, data, length);
}

void SDIOStack::sendSCO( const uint8* data, size_t length )
{
    sendPacket(SDIO_BT_TYPE_A_SID_SCO, data, length);
}

void SDIOStack::sendVendor(uint8 channel, const uint8 *data, size_t len)
{
    sendPacket(SDIO_BT_TYPE_A_SID_VENDOR, data, len, channel);
}

void SDIOStack::sendPacket(uint8 service_id, const uint8 *data, size_t len, uint8 channel)
{
    uint8 packet[SDIO_BT_TYPE_A_PACKET_LEN_MAX];
    size_t header_len = SDIO_BT_TYPE_A_HEADER_LEN;
    size_t packet_len = len + header_len;

    if (service_id == SDIO_BT_TYPE_A_SID_VENDOR) 
    {
        packet[4] = channel;
        header_len++;
        packet_len++;
    }

    packet[0] = packet_len;
    packet[1] = packet_len >> 8;
    packet[2] = packet_len >> 16;
    packet[3] = service_id;
    memcpy(packet + header_len, data, len);

    if (opened)
    {
        if (!sdioWrite(packet, packet_len)) 
        {
            callbacks->onTransportFailure();
            sdioClose();
        }
        else 
        {
            sent(packet, packet_len); /* pass to watcher */
        }
    }
}

bool SDIOStack::sdioWrite(uint8 *packet, size_t packet_len)
{
#ifdef _WIN32
    unsigned long written;
    OVERLAPPED overlapped;

    memset(&overlapped, 0x00, sizeof(overlapped));
    overlapped.Offset = 0;
    overlapped.OffsetHigh |= UNIRAW_BLOCK_TRANSFER;
    return WriteFile(handle, packet, packet_len, &written, &overlapped);
#else
    int ret = write(fd, packet, packet_len);
    return ret >= 0;
#endif
}

bool SDIOStack::receivePackets(uint8 *packet, size_t packet_len)
{
    /* uint32 data_len = packet_len - SDIO_BT_TYPE_A_HEADER_LEN; */

    while ( packet_len > 0 ) {
        uint32 data_len = (uint32)packet[0] + (((uint32)packet[1])<<8) + (((uint32)packet[2])<<16);
        int service_id = packet[3];

        if ( data_len > packet_len || SDIO_BT_TYPE_A_HEADER_LEN >= data_len )
        {
            /* shouldn't happen, but best to defend against */
            break;
        }

        data_len -= SDIO_BT_TYPE_A_HEADER_LEN;

        switch (service_id) {
        case SDIO_BT_TYPE_A_SID_ACL:
            callbacks->onACL(packet + SDIO_BT_TYPE_A_HEADER_LEN, data_len);
            break;
        case SDIO_BT_TYPE_A_SID_SCO:
            callbacks->onSCO(packet + SDIO_BT_TYPE_A_HEADER_LEN, data_len);
            break;
        case SDIO_BT_TYPE_A_SID_HCI_EVENT:
            callbacks->onEvent(packet + SDIO_BT_TYPE_A_HEADER_LEN, data_len);
            break;
        case SDIO_BT_TYPE_A_SID_VENDOR:
            callbacks->onVendor(packet[4], packet + SDIO_BT_TYPE_A_HEADER_LEN + 1, data_len - 1);
            break;
        default:
            /* shouldn't be getting these! */
            break;
        }

        data_len += SDIO_BT_TYPE_A_HEADER_LEN;
        recv(packet, data_len); /* pass to watcher */

        packet_len -= data_len;
        packet += data_len;
    }
    return 0 == packet_len; /* did we consume all data correctly? */
}

void SDIOStack::setCallBacks(SDIOStackCallBacks*x)
{
    callbacks = x;
}

void SDIOStack::setDataWatch(DataWatcher*x)
{
    Watched::set(x);
}

void SDIOStack::sdioClose()
{
#if _WIN32
    CloseHandle(handle);
    handle = NULL;
#else
    close(fd);
    fd = -1;
#endif
    opened = false;
}

SDIOStack::reader::reader(SDIOStack& s)
    : stack(s)
{
#ifdef _WIN32
    wait_for_read = CreateEvent(0, TRUE, TRUE, 0);
    thread_dying = CreateEvent(0, TRUE, FALSE, 0);
    ioctl_signal = CreateEvent(0, TRUE, FALSE, 0);
#endif
}

SDIOStack::reader::~reader()
{
    Kill();
}

void SDIOStack::reader::Stop()
{
#ifdef _WIN32
    SetEvent(thread_dying);
#endif
    Threadable::Stop();
}

int SDIOStack::reader::ThreadFunc()
{
    while (KeepGoing()) 
    {
        uint8 packet[SDIO_BT_TYPE_A_PACKET_LEN_MAX];
        uint32 length_read;

        ReadStatus status = sdioRead(packet, SDIO_BT_TYPE_A_PACKET_LEN_MAX, &length_read);
        assert(SUCCESS == status || FAILURE == status || NO_ACTION == status);
        if (SUCCESS == status)
        {
            if ( ! stack.receivePackets(packet, length_read) )
            {
                stack.callbacks->onTransportFailure();
                stack.sdioClose();
                return -1;
            }
        }
        else if (FAILURE == status)
        {
            stack.callbacks->onTransportFailure();
            stack.sdioClose();
            return -1;
        }
        else // NO_ACTION
        {
            continue;
        }
    }
    return 0;
}

#define WAIT_FOR_READ_SIGNALLED WAIT_OBJECT_0
#define THREAD_DYING_SIGNALLED (WAIT_OBJECT_0 + 1)

SDIOStack::reader::ReadStatus SDIOStack::reader::sdioRead(uint8 *packet, uint32 packet_len, uint32 *length_read)
{
    assert(packet);
    assert(length_read);
#ifdef _WIN32
    OVERLAPPED overlapped;
    SDIOStack::reader::ReadStatus readStatus;
    HANDLE signals[] = {wait_for_read, thread_dying};

    memset(&overlapped, 0x00, sizeof(overlapped));
    overlapped.Offset = 0;
    overlapped.OffsetHigh = UNIRAW_BLOCK_TRANSFER;
    overlapped.hEvent = wait_for_read;
    //printf("Readfile\n");
    if (!ReadFile(stack.handle, packet, packet_len,(LPDWORD) length_read, &overlapped))
    {
        //printf("Readfile false\n");
        DWORD errorCode = GetLastError();
        DWORD wait_status;
        if (ERROR_IO_PENDING == errorCode)
        {
            //printf("IO pending\n");
            while (true)
            {
                //printf("Waiting 1 sec\n");
                wait_status = WaitForMultipleObjects(2, signals, 0, 1000);
            
                if (WAIT_TIMEOUT == wait_status)
                {
                    //printf("Wait timed out\n");
                    DWORD chip_status;
                    DWORD bytes_returned;
                
                    OVERLAPPED ioctl_overlapped;
                    memset(&ioctl_overlapped, 0x00, sizeof(ioctl_overlapped));
                    ioctl_overlapped.Offset = 0;
                    ioctl_overlapped.OffsetHigh = UNIRAW_BLOCK_TRANSFER;
                    ioctl_overlapped.hEvent = ioctl_signal;

                    if (DeviceIoControl(stack.handle, UNIRAW_READ_EER, NULL, 0, &chip_status, sizeof(chip_status), &bytes_returned, &ioctl_overlapped))
                    {
                        //printf("DeviceIOcontrol true\n");    
                        // Check if chip / driver can still service reads
                        if (0 == chip_status)
                        {
                            //printf("chip_status 0\n");
                            // Chip has been reset
                            CancelIo(stack.handle);
                            return FAILURE;
                        }
                        //printf("chip_status !0\n");

                    }
                    else
                    {
                        //printf("DeviceIOcontrol false\n");
                        // Couldn't use ioctl
                        CancelIo(stack.handle);
                        return FAILURE;
                    }
                }
                else
                {
                    //printf("Wait signalled\n");
                    break;
                }
            }

            if (WAIT_FOR_READ_SIGNALLED == wait_status && 
                GetOverlappedResult(stack.handle, &overlapped, (LPDWORD)length_read, false))
            {
                //printf("Read signalled\n");
                return SUCCESS;
            }
            else if (THREAD_DYING_SIGNALLED == wait_status)
            {
                //printf("thread dying signalled\n");
                CancelIo(stack.handle);
                // Thread is in the prcess of being killed, return NO_ACTION and let the thread fall out of the loop.
                return NO_ACTION;
            }
            else
            {
                //printf("Some other wait proplem\n");
                return FAILURE;
            }
        }
        else
        {
            //printf("Generic failure\n");
            return FAILURE;
        }
    }
    else
    {
        //printf("Readfile true\n");
        return SUCCESS;
    }
#else
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(stack.fd, &fds);

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;

    int ret = select(stack.fd+1, &fds, NULL, NULL, &tv);
    if (ret <= 0)
        return NO_ACTION;

    int r = read(stack.fd, packet, packet_len);
    if (r < 0)
    {
        return FAILURE;
    }
    else
    {
        *length_read = r;
        return SUCCESS;
    }
#endif // _WIN32
}

SDIOPConfiguration::SDIOPConfiguration(const char * const aName)
    : mName(aName)
{
}

SDIOPConfiguration::SDIOPConfiguration(const wchar_t * const aName)
    : mName(aName)
{
}

SDIOStack * SDIOPConfiguration::make() const
{
    return new SDIOStack(mName.getA());
}
