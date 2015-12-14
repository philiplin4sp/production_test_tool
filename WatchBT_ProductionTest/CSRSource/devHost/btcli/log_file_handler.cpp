#include "log_file_handler.h"
#include "thread/critical_section.h"
#include <cstdio>
#include <cassert>
#include <string>
#include <cstdarg>
#include <common/types.h>
extern "C" void getTimestamp (char *buf);

// If each log file had it's own CriticalSection then it would be possible for 
// each LogFile object to open the same file at once. This probably isn't what
// the user meant to do. To get around this each LogFile object shares a single
// CriticalSection. This will mean only one log operation can happen at once,
// but until someone can show this is a problem this system should suffice.
static CriticalSection cs;
static bool clash_exists(const std::string &file_name);

class LogFile
{
public:
    LogFile();
    bool is_enabled();
    log_file_status_t enable(const std::string file_name);
    log_file_status_t disable();
    bool is_clash(const std::string &file_name);
    int vprintf(const char *format, va_list argptr);
    int printf(const char *format, ...);

private:
    FILE *log_file;
    std::string file_name;
    log_file_status_t open_file(const std::string &file_name);
    log_file_status_t close_file();
} rawlog;

LogFile::LogFile() : log_file(0), file_name("") {}

bool LogFile::is_enabled()
{
    return log_file != 0;
}

log_file_status_t LogFile::enable(std::string file_name)
{
    if (log_file)
        if (file_name == this->file_name) return REOPENED_SAME_FILE_SAME_LOG;
        else return ALREADY_LOGGING;
    else
        if (clash_exists(file_name)) return CLASH_EXISTS;
        else return open_file(file_name);
}

log_file_status_t LogFile::disable()
{
    if (log_file)
        return close_file();
    else
        return NOT_LOGGING;
}

bool LogFile::is_clash(const std::string &file_name)
{
    return log_file && file_name == this->file_name;
}

int LogFile::vprintf(const char *format, va_list argptr)
{
    if (log_file) return vfprintf(log_file, format, argptr);
    else return 0;
}

int LogFile::printf(const char *format, ...)
{
    if (!log_file)
    {
        return 0;
    }

    va_list ap;
    va_start(ap, format);
    return vfprintf(log_file, format, ap);
}

log_file_status_t LogFile::open_file(const std::string &file_name)
{
    // Don't open over a already existing file pointer
    assert(!log_file);
    log_file_status_t file_status(FILE_OPEN_SUCCESSFUL);
    if ((log_file = fopen(file_name.c_str(), "a+")) != NULL)
        this->file_name = file_name;
    else
        file_status = FILE_OPEN_UNSUCCESSFUL;
    return file_status;
}

log_file_status_t LogFile::close_file()
{
    assert(log_file);
    log_file_status_t file_status(FILE_CLOSE_SUCCESSFUL);
    if (fclose(log_file) == EOF)
        file_status = FILE_CLOSE_UNSUCCESSFUL;
    // Even if fclose fails, the file stream is invalidated, don't use it again
    log_file = 0;
    return file_status;
}

static bool clash_exists(const std::string &file_name)
{
    return rawlog.is_clash(file_name);
}

bool is_rawlog_log_enabled()
{
    CriticalSection::Lock lock(cs);
    return rawlog.is_enabled();
}

log_file_status_t enable_rawlog_log(const char *const file_name)
{
    CriticalSection::Lock lock(cs);
    assert(file_name);
    return rawlog.enable(file_name);
}

log_file_status_t disable_rawlog_log()
{
    CriticalSection::Lock lock(cs);
    return rawlog.disable();
}

void rawlog_log(char type, const uint8 *d, uint32 l)
{
    CriticalSection::Lock lock(cs);

    if (!rawlog.is_enabled())
    {
        return;
    }

    char buf[16];
    getTimestamp(buf);

    rawlog.printf("%s hci_%cx ", buf, type);
    size_t i;

    for (i = 0; i < l; ++i)
    {
        if (i % 5 == 0 && i != 0)
	{
            rawlog.printf("-");
        }
        rawlog.printf("%02x", d[i]);
    }
    rawlog.printf("\n");
}

