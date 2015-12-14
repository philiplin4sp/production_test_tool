#ifndef FILE_WRAPPER_H
#define FILE_WRAPPER_H

#include "scofilesend.h"
#include "thread/safe_counter.h"
#include <cstdio>

class FileWrapper
{
public:
    FileWrapper(ScoFileSend::AlertCallback &alert,
                ScoFileSend::Snapshot &snapshot) :
        mAlert(alert),
        mSnapshot(snapshot),
        file(0)
    {

    }

    bool open(const char *fileName, const char *mode)
    {
        using namespace std;
        if (!(file = fopen(fileName, mode)))
        {
            mAlert.callback(ScoFileSend::CAN_NOT_OPEN_FILE, mSnapshot);
            return false;
        }
        return true;
    }

    bool isOpen()
    {
        return file;
    }

    ~FileWrapper()
    {
        using namespace std;
        // Some fcloses are bad and don't like 0.
        if (file)
        {
            fclose(file);
        }
    }

    size_t read(uint8 *buf, size_t length)
    {
        return fread(buf, sizeof(uint8), length / sizeof(uint8), file); 
    }

    bool isEof()
    {
        return feof(file);
    }

private:
    ScoFileSend::AlertCallback &mAlert;
    ScoFileSend::Snapshot &mSnapshot;
    FILE *file;
};

#endif
