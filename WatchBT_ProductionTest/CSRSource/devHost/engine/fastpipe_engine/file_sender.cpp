/*
 * Copyright CSR 2008
 *
 * $Id: //depot/bc/bluesuite_2_4/devHost/engine/fastpipe_engine/file_sender.cpp#1 $
 *
 */

#include "file_sender.h"
#include <cassert>

static const size_t MAX_BUFFER_SIZE(1000000);

FileSender::FileSender(const uint16 ch) :
    file_(0),
    ch_(ch),
    running(false),
    numberOfOctetsSent(0)
{

}

FileSender::~FileSender()
{
    disable();
}

FileSenderResult FileSender::enable(const std::string &file_name, long &file_size)
{
    assert(this);
    CriticalSection::Lock lock(cs_);
    if (file_ != 0)
    {
        // Already running a file transfer
        return FILE_SEND_STILL_RUNNING;
    }

    file_ = fopen(file_name.c_str(), "rb");

    if (0 == file_)
    {
        return COULD_NOT_OPEN_FILE;
    }
    else
    {
        if ((0 != fseek(file_, 0L, SEEK_END)) ||
            (-1 == (file_size = ftell(file_))) ||
            (0 != fseek(file_, 0L, SEEK_SET)))
        {
            fclose(file_);\
            file_ = 0;
            return COULD_NOT_CALCULATE_FILE_SIZE;
        }
        else
        {
            return SUCCESS;
        }
    }
}

void FileSender::start()
{
    assert(this);
    CriticalSection::Lock lock(cs_);
    running = true;
    numberOfOctetsSent = 0;
}

void FileSender::close_file()
{
    assert(this);
    assert(file_);
    running = false;
    fclose(file_);
    file_ = 0;
}

bool FileSender::disable()
{
    assert(this);
    CriticalSection::Lock lock(cs_);

    if (file_ == 0)
    {
        return false;
    }

    close_file();
    return true;
}

bool FileSender::isRunning() const
{
    return running;
}

long FileSender::getNumberOfOctetsSent() const
{
    return numberOfOctetsSent;
}

static void buildPdu(const uint16 ch, 
                     const std::vector<uint8> &data,
                     const uint16 size,
                     HCIACLPDU &pdu)
{
    pdu.set_handle(ch);
    pdu.set_data(&data[0], size);
    pdu.set_length();
}

// Return any data that might be available. Also give an update on the state
// of the transfer.
FileSender::Data FileSender::getPdu(const uint16 maxSize)
{
    assert(this);
    CriticalSection::Lock lock(cs_);

    Status status(DISABLED);
    HCIACLPDU pdu(0);
    if (running)
    {
        std::vector<uint8> buffer(maxSize);
        const size_t numberOfBytesRead(fread(&buffer[0], 1, maxSize, file_));
        if (ferror(file_))
        {
            status = ERROR;
            close_file();
        }
        else 
        {
            buildPdu(ch_, buffer, numberOfBytesRead, pdu);
            numberOfOctetsSent += numberOfBytesRead;

            if (feof(file_))
            {
                status = LAST_DATA;
                close_file();
            }
            else
            {
                status = DATA;
            }
        }
    }
 
    return Data(status, pdu);
}

