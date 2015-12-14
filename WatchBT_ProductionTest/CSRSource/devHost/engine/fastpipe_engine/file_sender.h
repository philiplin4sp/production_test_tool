/*
 * Copyright CSR 2008
 *
 * $Id: //depot/bc/bluesuite_2_4/devHost/engine/fastpipe_engine/file_sender.h#1 $
 *
 * Purpose: Starts a thread to read data from a file and puts the data in a
 *          queue to be read via getPdu.
 *
 */

#ifndef FILE_SENDER_H
#define FILE_SENDER_H

#include "thread/critical_section.h"
#include "hcipacker/hcidatapdu.h"
#include "common/countedpointer.h"
#include "common/types.h"
#include <string>
#include <vector>
#include <cstdio>

enum FileSenderResult
{
    SUCCESS,
    COULD_NOT_OPEN_FILE,
    COULD_NOT_CALCULATE_FILE_SIZE,
    FILE_SEND_STILL_RUNNING
};

class FileSender : public Counter
{
public:
    enum Status
    {
        DISABLED,
        DATA,
        ERROR,
        LAST_DATA
    };

    typedef std::pair<Status, HCIACLPDU> Data;

    FileSender(uint16 ch);
    ~FileSender();
    FileSenderResult enable(const std::string &file_name, long &file_size);
    void start();
    bool disable();
    Data getPdu(uint16 maxSize);
    bool isRunning() const;
    long getNumberOfOctetsSent() const;
private:
    CriticalSection cs_;
    FILE *file_;
    void close_file();
    uint16 ch_;
    bool running;
    long numberOfOctetsSent;
};

#endif
