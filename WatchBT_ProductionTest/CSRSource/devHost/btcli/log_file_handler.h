#ifndef LOG_FILE_HANDLER_H
#define LOG_FILE_HANDLER_H

#include "common/types.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum 
{
    FILE_OPEN_SUCCESSFUL,
    FILE_OPEN_UNSUCCESSFUL,
    REOPENED_SAME_FILE_SAME_LOG,
    ALREADY_LOGGING,
    FILE_CLOSE_SUCCESSFUL,
    FILE_CLOSE_UNSUCCESSFUL,
    NOT_LOGGING,
    CLASH_EXISTS
} log_file_status_t;

bool is_rawlog_log_enabled();
log_file_status_t enable_rawlog_log(const char *file_name);
log_file_status_t disable_rawlog_log();
void rawlog_log(char type, const uint8 *d, uint32 l);

#ifdef __cplusplus
};
#endif

#endif
