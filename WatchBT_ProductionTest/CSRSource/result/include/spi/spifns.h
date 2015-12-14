#ifndef SPIFNS_H_INCLUDED
#define SPIFNS_H_INCLUDED

/**********************************************************************
*
*  FILE   :  spifns.h
*
*            Copyright (C) Cambridge Silicon Radio Ltd 2001-2010
*
*  PURPOSE:  Defines the interface to SPI implementations.
*
*  $Id: //depot/bc/bluesuite_2_4/devHost/spi/spilpt/spifns.h#1 $
*
***********************************************************************/

#ifndef LINT
static const char SPIFNS_h_vss_id[] = "$Id: //depot/bc/bluesuite_2_4/devHost/spi/spilpt/spifns.h#1 $";
#endif

#include "common/portability.h"
#include "common/types.h"

#define PTTRANS_VERSION 0x0103

#ifdef __cplusplus
extern "C" {
#endif

#if !defined (WIN32) || defined (SPI_LIB)
#define SPIFNSDLL_API
#else
#ifdef SPIFNS_EXPORTS
#define SPIFNSDLL_API __declspec(dllexport)
#else
#define SPIFNSDLL_API __declspec(dllimport)
#endif
#endif

#define SPIFNS_SUCCESS (0)
#define SPIFNS_FAILURE (1)

/* these are for backward compatibility only: */
#define SUCCESS (0)
#define FAILURE (1)

/* The opaque type that externally identifies a spifns stream */
typedef unsigned int spifns_stream_t;
#define SPIFNS_STREAM_INVALID 0x7FFFFFFF
#define SPIFNS_STREAMS_EQUAL(stream1, stream2) ((stream1)==(stream2))

enum spifns_xap_state
{
    SPIFNS_XAP_STOPPED = 1,
    SPIFNS_XAP_RUNNING = 0,
    SPIFNS_XAP_NO_REPLY = -1,
    SPIFNS_XAP_SPI_LOCKED = -2,
    SPIFNS_XAP_NOT_IMPL = -3,
};


enum spifns_spi_errors
{	
    SPIFNS_ERROR_NO_ERROR  = 0x000, /* tests equal to SPIFNS_SUCCESS and PTTRANS_SUCCESS */
    SPIFNS_ERROR_GENERAL_FAILURE = SPIFNS_FAILURE, /* tests equal to SPIFNS_FAILURE and PTTRANS_FAILURE */
    /* the spifns error range is 0x100-0x1FF */
    SPIFNS_ERROR_MEMORY    = 0x101,
    SPIFNS_ERROR_CONNECT   = 0x102,
    SPIFNS_ERROR_READ_FAIL = 0x103, /* The checksum read from MISO after a read command was incorrect. */
    SPIFNS_ERROR_OS_ERROR  = 0x104,
    SPIFNS_ERROR_TOO_MANY_STREAMS = 0x105,
    SPIFNS_ERROR_INVALID_STREAM = 0x106,
    SPIFNS_ERROR_ALREADY_OPEN = 0x107,
    SPIFNS_ERROR_WRITE_FAIL = 0x108,
};

typedef enum SpiActionType
{
    SpiAction_Read = 0,
    SpiAction_Write,
    SpiAction_SetVar
} SpiActionType;

typedef struct SpiAction
{
    SpiActionType type;
    union
    {
        struct
        {
            uint16 address;
            uint16 length;
            union
            {
                const uint16 *writeData;
                uint16 *readData;
            }data;
        } rw;

        struct
        {
            const char *var;
            const char *value;
        } setvar;
    }u;
} SpiAction;

typedef enum SpiVarFlags
{
    SpiVarFlag_None               = 0x0000,
        /* If a variable is Required and no value is specified then explicitly set this var to the default value. */
    SpiVarFlag_Required           = 0x0001,
        /* StreamList variables should contain a list of streams.
           '_STREAMS' is stripped from the end of the variable name and then
           for each stream specified the resulting variable is set to "ON" and for streams not specified set to "OFF". */
    SpiVarFlag_StreamList         = 0x0002,
        /* For Append* type variables the '_PREFIX' is stripped from the end of the variable name and then
           the Id(s) are appended to the end of the value. If the value looks like a filename and has a suffix then
           the suffix is preserved. e.g. 'c:\spi.txt' -> 'c:\spi_123.txt' */
    SpiVarFlag_AppendDeviceId     = 0x0004,
    SpiVarFlag_AppendPid          = 0x0008,
    SpiVarFlag_AppendStreamId     = 0x0010,
} SpiVarFlags;

typedef struct SpiVariable
{
    const char *name;
    const char *defaultValue;
    int flags;
} SpiVariable;

typedef void (*SpiDebugFn)(const char *);
typedef void (*SpiStreamDebugFn)(const char *, void *);

SPIFNSDLL_API int spifns_init(void);
SPIFNSDLL_API int spifns_stream_init(spifns_stream_t *p_stream);
SPIFNSDLL_API void spifns_close(void);
SPIFNSDLL_API void spifns_stream_close(spifns_stream_t stream);
SPIFNSDLL_API unsigned int spifns_count_streams(void);

SPIFNSDLL_API void spifns_getvarlist(const SpiVariable **vars, int *count);

SPIFNSDLL_API int spifns_sequence(const SpiAction *seq, int length);
SPIFNSDLL_API int spifns_stream_sequence(spifns_stream_t stream, const SpiAction *seq, int length);

SPIFNSDLL_API int spifns_bccmd_init(uint16 slutid_spi_user_cmd,
                                    uint16 bccmd_spi_interface);
SPIFNSDLL_API int spifns_bccmd_cmd(uint16 *pdu, uint16 pdulen, int wait_for_result);
SPIFNSDLL_API int spifns_stream_bccmd_init(spifns_stream_t stream, uint16 slutid_spi_user_cmd,
                                           uint16 bccmd_spi_interface);
SPIFNSDLL_API int spifns_stream_bccmd_cmd(spifns_stream_t stream, uint16 *pdu, uint16 pdulen, int wait_for_result);

SPIFNSDLL_API void spifns_enumerate_ports(void (*callback)(int port, const char *name, void *callback_data), void *callback_data);

SPIFNSDLL_API const char* spifns_command(const char *command);
SPIFNSDLL_API const char* spifns_stream_command(spifns_stream_t stream, const char *command);
SPIFNSDLL_API const char* spifns_getvar(const char *var);
SPIFNSDLL_API const char* spifns_stream_getvar(spifns_stream_t stream, const char *var);

/* Returns an indication of SPI reliablity (0x0000 - Totally unreliable, 0x10000 - Totally reliable) */
SPIFNSDLL_API void spifns_get_reliablity(int *reliability);
SPIFNSDLL_API void spifns_stream_get_reliability(spifns_stream_t stream, int *reliability);


#define CHIP_SELECT_XILINX      (0)
#define CHIP_SELECT_SPARTAN     (1)
#define CHIP_SELECT_NONE        (-1)

/* Replacement for the spifns_config argument to xap_ functions */
SPIFNSDLL_API void spifns_chip_select(int which);
SPIFNSDLL_API void spifns_stream_chip_select(spifns_stream_t stream, int which);

SPIFNSDLL_API enum spifns_xap_state spifns_bluecore_xap_stopped(void);
SPIFNSDLL_API enum spifns_xap_state spifns_stream_bluecore_xap_stopped(spifns_stream_t stream);

/* returns the last error code, and if a pointer is passed in, the problematic address.*/
/* get_last_error and clear_last_error both deal with the error that occurred in the current thread */
SPIFNSDLL_API int spifns_get_last_error(unsigned short *addr, const char ** buf);
SPIFNSDLL_API void spifns_clear_last_error(void);
SPIFNSDLL_API void spifns_set_debug_callback(SpiDebugFn fn);
SPIFNSDLL_API void spifns_stream_set_debug_callback(spifns_stream_t stream, SpiStreamDebugFn fn, void *pvcontext);

SPIFNSDLL_API uint32 spifns_get_version(void);

#define SPIFNS_MAX_DEVICE_ID_LEN 255
SPIFNSDLL_API int spifns_stream_get_device_id(spifns_stream_t stream, char *buf, size_t length);

SPIFNSDLL_API int spifns_stream_lock(spifns_stream_t stream, uint32 timeout);
SPIFNSDLL_API void spifns_stream_unlock(spifns_stream_t stream);

#ifdef __cplusplus
}

class ChipHelper;
/* Implement this if the pttransport plugin needs to use ChipHelper. The instance is 
   passed from pttransport, which should have initialised it correctly after detecting
   the chip type */
SPIFNSDLL_API void spifns_set_chip_helper(const ChipHelper& chip_helper);
SPIFNSDLL_API void spifns_stream_set_chip_helper(spifns_stream_t stream, const ChipHelper& chip_helper);

#endif /* #ifdef __cplusplus */

#endif /* SPIFNS_H_INCLUDED */
