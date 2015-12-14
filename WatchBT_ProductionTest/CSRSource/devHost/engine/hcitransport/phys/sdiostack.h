///////////////////////////////////////////////////////////////////////
//
//  FILE   :  sdiostack.h
//
//  AUTHOR :  Adam Hughes
//
//  CLASS  :  SDIOStack
//
//  PURPOSE:  Declare an implmentation of the SDIO type A transport
//
///////////////////////////////////////////////////////////////////////

#ifndef HEADER_SDIOSTACK_H
#define HEADER_SDIOSTACK_H

#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

#include "common/types.h"
#include "thread/thread.h"
#include "csrhci/physicaltransportconfiguration.h"

class SDIOStackCallBacks
{
public:
    virtual ~SDIOStackCallBacks(){};
    virtual void onEvent( uint8 * data, uint32 length ) = 0;
    virtual void onACL( uint8 * data, uint32 length ) = 0;
    virtual void onSCO( uint8 * data, uint32 length ) = 0;
    virtual void onVendor(uint8 channel, uint8 *pdu, uint32 len) = 0;
    virtual void onTransportFailure() = 0;
};

class SDIOStack: public Watched
{
public:
    SDIOStack(const name_store &lDevice);
    ~SDIOStack();

    bool start();
    bool ready( uint32 timeout );
    void stop();

    void sendCommand( const uint8* data, size_t length );
    void sendACL( const uint8* data, size_t length );
    void sendSCO( const uint8* data, size_t length );
    void sendVendor(uint8 channel, const uint8 *data, size_t len);

    void setCallBacks(SDIOStackCallBacks*);
    void setDataWatch(DataWatcher*);

private:
    void sendPacket(uint8 service_id, const uint8 *data, size_t len, uint8 channel = 0);
    bool receivePackets(uint8 *packet, size_t len);

    bool sdioWrite(uint8 *packet, size_t packet_len);
    void sdioClose();
    
    class reader : public Threadable {
    public:
        reader(SDIOStack& s);
        virtual ~reader();

        int ThreadFunc();
        void Stop();
        enum ReadStatus
        {
            SUCCESS,
            FAILURE,
            NO_ACTION
        };
    private:
        ReadStatus sdioRead(uint8 *packet, uint32 packet_len, uint32 *length_read);
        SDIOStack& stack;
#ifdef _WIN32
        HANDLE wait_for_read;
        HANDLE thread_dying;
        HANDLE ioctl_signal;
#endif
    };
    friend class reader;

    SDIOStackCallBacks *callbacks;

    const name_store device;
#ifdef _WIN32
    HANDLE handle;
#else
    int fd;
#endif
    bool opened;
    reader *read_thread;
};

#endif//HEADER_SDIOSTACK_H

