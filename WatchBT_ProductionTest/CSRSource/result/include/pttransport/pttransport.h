#ifndef PTTRANSPORT_H
#define PTTRANSPORT_H

/**********************************************************************
*
*  FILE   :  pttransport.h
*
*            Copyright (C) Cambridge Silicon Radio Ltd 2005-2009
*
*  PURPOSE:  Provides methods allowing the caller to obtain an 
*            interface to the spi library using a specific transport 
*            mechanism.
*
*  $Id: $
*
***********************************************************************/

#include "common/types.h"
#include "common/portability.h"
#include "abstract/memory_reader.h"
#include "spi/spifns.h"
#include "pttrans_types.h"
#include "chiputil/chiphelper.h"
#include "chiputil/xaphelper.h"

#ifdef __cplusplus
extern "C"
{
#endif //def __cplusplus

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TRANSPORT_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TRANSPORT_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef WIN32
#ifdef PTTRANSPORT_EXPORTS
#define PTTRANSPORT_API __declspec(dllexport)
#else
#define PTTRANSPORT_API __declspec(dllimport)
#endif
#else
#define PTTRANSPORT_API
#endif

typedef unsigned int pttrans_stream_t;
#define PTTRANS_STREAM_INVALID 0x7FFFFFFF
#define PTTRANS_STREAMS_EQUAL(stream1, stream2) ((stream1)==(stream2))


// Open a transport method using environment variables.
// Environment variables can be overridden with 'command' which is a series of space
// seperated assignments which may include those passable to pttrans_setvar() and:
// 'SPITRANS=LPT' (default) or
// 'SPITRANS=USB'
// port being anything other than -1 will add SPIPORT=<port> to list.
// mul being anything other than -1 will add SPIMUL=<port> to list.
// Function returns false on failure, call pttrans_get_last_error() for error information.
typedef int pttrans_open_t(const char*, int, int);
PTTRANSPORT_API int pttrans_open(const char *command, int port, int mul);
typedef int pttrans_stream_open_t(pttrans_stream_t, const char *);

// Stream versions of pttrans_open. These return PTTRANS_SUCCESS on success and other values
// for failure. Call pttrans_get_last_error() for error information.
PTTRANSPORT_API int pttrans_stream_open(pttrans_stream_t *p_stream, const char *trans);
PTTRANSPORT_API int pttrans_stream_open_no_detect_core(pttrans_stream_t *p_stream, const char *trans); 

// Special version of 'open' for multispi broadcast. Finds the first port with a chip connected
// If auto-detect (broadcast_mask = 0) then checks all ports, otherwise only uses ports in mask
// Function returns PTTRANS_FAILURE if no chips are found
PTTRANSPORT_API int pttrans_open_multispi_broadcast(const char *command, int port, uint16 broadcast_mask);
PTTRANSPORT_API int pttrans_stream_open_multispi_broadcast(pttrans_stream_t *p_stream, const char *command, uint16 broadcast_mask);

// Close transport method.
typedef void pttrans_close_t(void);
PTTRANSPORT_API void pttrans_close(void);
typedef void pttrans_stream_close_t(pttrans_stream_t);
PTTRANSPORT_API void pttrans_stream_close(pttrans_stream_t stream);

// Returns true if a legacy stream is open.
typedef int pttrans_isopen_t(void);
PTTRANSPORT_API int pttrans_isopen(void);

int pttrans_get_legacy_stream(pttrans_stream_t *p_stream);

// Returns the number of open streams.
PTTRANSPORT_API unsigned int pttrans_count_streams(void);

// If addr is not null, copy the address of the last error into it
// If buf is not null, copy a pointer to the last error message string into it
// These functions both act on the last error that occured in the current thread.
// Returns the error code
typedef int pttrans_get_last_error_t(uint16 *addr, const char** buf);
PTTRANSPORT_API int pttrans_get_last_error(uint16 *addr, const char** buf);

typedef void pttrans_clear_last_error_t();
PTTRANSPORT_API void pttrans_clear_last_error();

// Set and Get global flags.  See pttrans_flags in pttrans_types.h for details
typedef unsigned int pttrans_set_flags_t(unsigned int);
PTTRANSPORT_API unsigned int pttrans_set_flags(unsigned int flags);
typedef unsigned int pttrans_get_flags_t();
PTTRANSPORT_API unsigned int pttrans_get_flags();

/*************************************
 * The type of debug being output. Different debug is output under different circumstances.
 *************************************/
typedef enum
{
    PTTRANS_DEBUG_TYPE_NORMAL,
    PTTRANS_DEBUG_TYPE_FULL,
} PtTransDebugTypeEnum;

/* these debug functions always output PTTRANS_DEBUG_TYPE_NORMAL debug */
typedef void pttrans_debug_t(const char *message);
PTTRANSPORT_API void pttrans_debug(const char *message);
typedef void pttrans_stream_debug_t(pttrans_stream_t stream, const char *message);
PTTRANSPORT_API void pttrans_stream_debug(pttrans_stream_t stream, const char *message);

/* these functions output more configurable debug and can take printf style arguments */
typedef void pttrans_debugf_t(PtTransDebugTypeEnum debug_type, const char *fmt, ...);
PTTRANSPORT_API void pttrans_debugf(PtTransDebugTypeEnum debug_type, const char *fmt, ...);
typedef void pttrans_stream_debugf_t(pttrans_stream_t stream, PtTransDebugTypeEnum debug_type, const char *fmt, ...);
PTTRANSPORT_API void pttrans_stream_debugf(pttrans_stream_t stream, PtTransDebugTypeEnum debug_type, const char *fmt, ...);
typedef void pttrans_debugfv_t(PtTransDebugTypeEnum debug_type, const char *fmt, va_list ap);
PTTRANSPORT_API void pttrans_debugfv(PtTransDebugTypeEnum debug_type, const char *fmt, va_list ap);
typedef void pttrans_stream_debugfv_t(pttrans_stream_t stream, PtTransDebugTypeEnum debug_type, const char *fmt, va_list ap);
PTTRANSPORT_API void pttrans_stream_debugfv(pttrans_stream_t stream, PtTransDebugTypeEnum debug_type, const char *fmt, va_list ap);

// Enumerate the devices of the given transport type, this should be freed after use.
// This call does not change the currently open transport.
PTTRANSPORT_API int pttrans_enumerate(const char *spitrans, pttrans_device **devices, int *dev_count);
PTTRANSPORT_API void pttrans_enumerate_free(pttrans_device *devices, int dev_count);
// Enumerate all pttransport devices, providing comma separated lists of human readable port names
// (for user selection), and transport options for passing to pttrans_open.
// maxLen is an in/out parameter. Pass in the length of the allocated storage for the ports and
// trans parameters (same size). If the length is too short, the function returns error and maxLen
// is set to the size required. If any other error occurs, maxLen is set to zero.
// count is set to the number of available pttransport devices found.
PTTRANSPORT_API int pttrans_enumerate_strings(uint16* maxLen, char* ports, char* trans, uint16* count);

// Lock and unlock the device currently opened, for exclusive access.
PTTRANSPORT_API int pttrans_lock(uint32 timeout);
PTTRANSPORT_API int pttrans_stream_lock(pttrans_stream_t stream, uint32 timeout); 
PTTRANSPORT_API void pttrans_unlock(void);
PTTRANSPORT_API void pttrans_stream_unlock(pttrans_stream_t stream); 

// Pass the transport layer a command.
// Returned string valid until next call.
// SPISLOW - Slows down subsequent SPI activity to enable talking to a chip in shallow sleep.
typedef const char* pttrans_command_t(const char *);
PTTRANSPORT_API const char* pttrans_command(const char *command);
typedef const char* pttrans_stream_command_t(pttrans_stream_t, const char *);
PTTRANSPORT_API const char* pttrans_stream_command(pttrans_stream_t stream, const char *command); 

// Set a transport layer variable.
// SPIMUL - Set which multiplexed device to use.
// SPIPORT - Set which port to use (ENV will read environment variable)
// SPI_DELAY - spifns_set_shift_period()
// SPI_DELAY_MODE - spifns_set_shift_period_mode()
// SPICLOCK - sets the SPI clock speed in kHz (float)
// SPICMDBITS - command bits (set the otherwise-unused command bits in the spi_read and spi_write wire commands)
// SPIDEBUG - when set to TRUE writes debug output about SPI transactions.
// SPIRECORD - specifies a file which all SPI transactions will be written to, or STOP to finish recording.
// SPIRECORD_READS - allows whether SPI reads are recorded to be ON or OFF (default ON).
// SPIRECORD_WRITES - allows whether SPI writes are recorded to be ON or OFF (default ON).
typedef int pttrans_setvar_t(const char *, const char *);
PTTRANSPORT_API int pttrans_setvar(const char *var, const char *value);
typedef int pttrans_stream_setvar_t(pttrans_stream_t, const char *, const char *);
PTTRANSPORT_API int pttrans_stream_setvar(pttrans_stream_t stream, const char *var, const char *value); 

#define pttrans_setvarint(var, value)                  \
    do {                                               \
        char strval[16];                               \
        SNPRINTF(strval, sizeof(strval), "%d", value); \
        pttrans_setvar(var, strval);                   \
    } while (0)
#define pttrans_setstreamvarint(s, var, value)            \
    do {                                                  \
        char strval[16];                                  \
        SNPRINTF(strval, sizeof(strval), "%d", value);    \
        pttrans_stream_setvar(s, var, strval);            \
    } while (0)

// Get a transport layer variable.
// Returned string valid until next call.
// SPITRANS - Transport method (LPT,USB)
// SPIMUL - Multiplexed device in use.
// SPIMUTEX - Mutex name to use.
// SPI_DELAY - spifns_query_shift_period()
// SPI_DELAY_MODE - spifns_query_shift_period_mode()
// SPICLOCK - gets the SPI clock speed in kHz (float)
// SPICMDBITS - command bits (set the otherwise-unused command bits in the spi_read and spi_write wire commands)
typedef const char* pttrans_getvar_t(const char *);
PTTRANSPORT_API const char* pttrans_getvar(const char *var);
typedef const char* pttrans_stream_getvar_t(pttrans_stream_t, const char *);
PTTRANSPORT_API const char* pttrans_stream_getvar(pttrans_stream_t stream, const char *var); 


// Should only be called from something receiving remote messages to pass them to pttrans layer.
// String returned by 'response' should be free'd by caller.
PTTRANSPORT_API void pttrans_remote_cmd(const char   *message, uint32  messLen,
                                        const char **response, uint32 *respLen);
PTTRANSPORT_API void pttrans_stream_remote_cmd(pttrans_stream_t stream, const char *message,
                                               uint32 messLen, const char **response, uint32 *respLen); 

typedef int pttrans_read_t(uint16, uint16*);
typedef int pttrans_write_t(uint16, uint16);
typedef int pttrans_read_verify_t(uint16, uint16*);
typedef int pttrans_write_verify_t(uint16, uint16);
typedef int pttrans_read_block_t(uint16, uint32, uint16*);
typedef int pttrans_write_block_t(uint16, uint32, const uint16*);
typedef int pttrans_read_block_verify_t(uint16, uint32, uint16*); 
typedef int pttrans_write_block_verify_t(uint16, uint32, const uint16*);
typedef int pttrans_stream_read_t(pttrans_stream_t, uint16, uint16*);
typedef int pttrans_stream_write_t(pttrans_stream_t, uint16, uint16);
typedef int pttrans_stream_read_verify_t(pttrans_stream_t, uint16, uint16*);
typedef int pttrans_stream_write_verify_t(pttrans_stream_t, uint16, uint16);
typedef int pttrans_stream_read_block_t(pttrans_stream_t, uint16, uint32, uint16*);
typedef int pttrans_stream_write_block_t(pttrans_stream_t, uint16, uint32, const uint16*);
typedef int pttrans_stream_read_block_verify_t(pttrans_stream_t, uint16, uint16, uint16*); 
typedef int pttrans_stream_write_block_verify_t(pttrans_stream_t, uint16, uint16, const uint16*);
PTTRANSPORT_API int pttrans_read(uint16 addr, uint16 *value);
PTTRANSPORT_API int pttrans_write(uint16 addr, uint16 data);
PTTRANSPORT_API int pttrans_read_verify(uint16 addr, uint16 *value);
PTTRANSPORT_API int pttrans_write_verify(uint16 addr, uint16 data);
PTTRANSPORT_API int pttrans_read_block(uint16 addr, uint32 count, uint16 *data);
PTTRANSPORT_API int pttrans_write_block(uint16 addr, uint32 count, const uint16 *data);
PTTRANSPORT_API int pttrans_read_block_verify(uint16 addr, uint32 count, uint16 *data);
PTTRANSPORT_API int pttrans_write_block_verify(uint16 addr, uint32 count, const uint16 *data);
PTTRANSPORT_API int pttrans_stream_read(pttrans_stream_t stream, uint16 addr, uint16 *value); 
PTTRANSPORT_API int pttrans_stream_write(pttrans_stream_t stream, uint16 addr, uint16 data); 
PTTRANSPORT_API int pttrans_stream_read_verify(pttrans_stream_t stream, uint16 addr, uint16 *value); 
PTTRANSPORT_API int pttrans_stream_write_verify(pttrans_stream_t stream, uint16 addr, uint16 data); 
PTTRANSPORT_API int pttrans_stream_read_block(pttrans_stream_t stream, uint16 addr, uint16 count, uint16 *data); 
PTTRANSPORT_API int pttrans_stream_write_block(pttrans_stream_t stream, uint16 addr, uint16 count, const uint16 *data); 
PTTRANSPORT_API int pttrans_stream_read_block_verify(pttrans_stream_t stream, uint16 addr, uint16 count, uint16 *data); 
PTTRANSPORT_API int pttrans_stream_write_block_verify(pttrans_stream_t stream, uint16 addr, uint16 count, const uint16 *data); 



// Construct a read command in the 'SpiAction' structure ready for calling 'pttrans_sequence()'.
// Note this allocates memory in seq->readData which must be free'd after calling 'pttrans_sequence()'.
PTTRANSPORT_API void pttrans_makeread(SpiAction *seq, uint16 address, uint16 length, uint16 *data);

// Construct a write command in the 'SpiAction' structure ready for calling 'pttrans_sequence()'.
PTTRANSPORT_API void pttrans_makewrite(SpiAction *seq, uint16 address, uint16 length, const uint16 *data);

// Construct a set var command in the 'SpiAction' structure read for calling 'pttrans_sequence()'.
PTTRANSPORT_API void pttrans_makesetvar(SpiAction *seq, const char *var, const char *value);

// Execute a sequence of commands.
PTTRANSPORT_API int pttrans_sequence(SpiAction *seq, int length);
PTTRANSPORT_API int pttrans_stream_sequence(pttrans_stream_t stream, SpiAction *seq, int length); 


typedef int pttrans_xap_reset_and_stop_t(void);
typedef int pttrans_xap_reset_and_go_t(void);
typedef int pttrans_xap_go_t(void);
typedef int pttrans_xap_stop_t(void);
typedef pttrans_xap_state pttrans_xap_stopped_t(void);
typedef int pttrans_stream_xap_reset_and_stop_t(pttrans_stream_t);
typedef int pttrans_stream_xap_reset_and_go_t(pttrans_stream_t);
typedef int pttrans_stream_xap_go_t(pttrans_stream_t);
typedef int pttrans_stream_xap_stop_t(pttrans_stream_t);
typedef pttrans_xap_state pttrans_stream_xap_stopped_t(pttrans_stream_t);
PTTRANSPORT_API int pttrans_xap_reset_and_stop(void);
PTTRANSPORT_API int pttrans_xap_reset_and_go(void);
PTTRANSPORT_API int pttrans_xap_go(void);
PTTRANSPORT_API int pttrans_xap_go_irq_on_breakpoint(void);
PTTRANSPORT_API int pttrans_xap_stop(void);
PTTRANSPORT_API pttrans_xap_state pttrans_update_xap_state(void);
PTTRANSPORT_API pttrans_xap_state pttrans_xap_stopped(void);
PTTRANSPORT_API int pttrans_stream_xap_reset_and_stop(pttrans_stream_t stream); 
PTTRANSPORT_API int pttrans_stream_xap_reset_and_go(pttrans_stream_t stream); 
PTTRANSPORT_API int pttrans_stream_xap_go(pttrans_stream_t stream); 
PTTRANSPORT_API int pttrans_stream_xap_go_irq_on_breakpoint(pttrans_stream_t stream); 
PTTRANSPORT_API int pttrans_stream_xap_stop(pttrans_stream_t stream); 
PTTRANSPORT_API pttrans_xap_state pttrans_stream_update_xap_state(pttrans_stream_t stream); 
PTTRANSPORT_API pttrans_xap_state pttrans_stream_xap_stopped(pttrans_stream_t stream); 


/* Initialise BCCMD support if present */
PTTRANSPORT_API int pttrans_bccmd_init(uint16 slutid_spi_user_cmd,
                                       uint16 bccmd_spi_interface);
PTTRANSPORT_API int pttrans_bccmd_cmd(uint16 *pdu, uint16 pdulen, int wait_for_result);
PTTRANSPORT_API int pttrans_stream_bccmd_init(pttrans_stream_t stream, uint16 slutid_spi_user_cmd,
                                              uint16 bccmd_spi_interface); 
PTTRANSPORT_API int pttrans_stream_bccmd_cmd(pttrans_stream_t stream, uint16 *pdu,
                                             uint16 pdulen, int wait_for_result); 


PTTRANSPORT_API pttrans_core_type_t pttrans_get_core_type(void);
PTTRANSPORT_API void pttrans_set_core_type(pttrans_core_type_t r_core_type);
PTTRANSPORT_API pttrans_core_type_t pttrans_stream_get_core_type(pttrans_stream_t stream); 
PTTRANSPORT_API void pttrans_stream_set_core_type(pttrans_stream_t stream, pttrans_core_type_t r_core_type); 

/* Basically performs a read or write from/to the DBG_SPI_PROC_SELECT register */
PTTRANSPORT_API pttrans_uniproc_t pttrans_get_proc_type(void);
PTTRANSPORT_API int pttrans_set_proc_type(pttrans_uniproc_t proc);
PTTRANSPORT_API pttrans_uniproc_t pttrans_stream_get_proc_type(pttrans_stream_t stream); 
PTTRANSPORT_API int pttrans_stream_set_proc_type(pttrans_stream_t stream, pttrans_uniproc_t proc); 


PTTRANSPORT_API void pttrans_chip_select(int which);
PTTRANSPORT_API void pttrans_stream_chip_select(pttrans_stream_t stream, int which); 


// Retrieves the version of pttransport.dll.
typedef uint32 pttrans_get_version_t(void);
PTTRANSPORT_API uint32 pttrans_get_version(void);

PTTRANSPORT_API ChipDescript *pttrans_chip_descript(void);
PTTRANSPORT_API ChipDescript *pttrans_stream_chip_descript(pttrans_stream_t stream);

#ifdef __cplusplus
}

PTTRANSPORT_API int pttrans_readgp(GenericPointer addr, uint16 *value);
PTTRANSPORT_API int pttrans_writegp(GenericPointer addr, uint16 data);
PTTRANSPORT_API int pttrans_readgp_verify(GenericPointer addr, uint16 *value);
PTTRANSPORT_API int pttrans_writegp_verify(GenericPointer addr, uint16 data);
PTTRANSPORT_API int pttrans_readgp_block(GenericPointer addr, uint32 count, uint16 *data);
PTTRANSPORT_API int pttrans_writegp_block(GenericPointer addr, uint32 count, const uint16 *data);
PTTRANSPORT_API int pttrans_readgp_block_verify(GenericPointer addr, uint32 count, uint16 *data);
PTTRANSPORT_API int pttrans_writegp_block_verify(GenericPointer addr, uint32 count, const uint16 *data);
PTTRANSPORT_API int pttrans_stream_readgp(pttrans_stream_t stream, GenericPointer addr, uint16 *value);
PTTRANSPORT_API int pttrans_stream_writegp(pttrans_stream_t stream, GenericPointer addr, uint16 data);
PTTRANSPORT_API int pttrans_stream_readgp_verify(pttrans_stream_t stream, GenericPointer addr, uint16 *value);
PTTRANSPORT_API int pttrans_stream_writegp_verify(pttrans_stream_t stream, GenericPointer addr, uint16 data);
PTTRANSPORT_API int pttrans_stream_readgp_block(pttrans_stream_t stream, GenericPointer addr, uint32 count, uint16 *data);
PTTRANSPORT_API int pttrans_stream_writegp_block(pttrans_stream_t stream, GenericPointer addr, uint32 count, const uint16 *data);
PTTRANSPORT_API int pttrans_stream_readgp_block_verify(pttrans_stream_t stream, GenericPointer addr, uint32 count, uint16 *data);
PTTRANSPORT_API int pttrans_stream_writegp_block_verify(pttrans_stream_t stream, GenericPointer addr, uint32 count, const uint16 *data);


PTTRANSPORT_API ChipHelper &pttrans_chip_helper();
PTTRANSPORT_API ChipHelper &pttrans_stream_chip_helper(pttrans_stream_t stream);
PTTRANSPORT_API CXapHelper &pttrans_xap_helper();
PTTRANSPORT_API CXapHelper &pttrans_stream_xap_helper(pttrans_stream_t stream);

// returns the number of read, write, setvar and lock operations done on this stream.
// This is mostly useful for testing to check that the correct stream is or isn't being used.
PTTRANSPORT_API unsigned int pttrans_stream_get_num_operations(pttrans_stream_t stream);

class UniReader : public IMemoryReaderGP
{
public:
    virtual bool read_mem(GenericPointer addr, uint16 len, uint16 * data)
    {
        return (pttrans_readgp_block(addr, len, data) == PTTRANS_SUCCESS);
    }
};

class UniStreamReader : public IMemoryReaderGP
{
private:
    pttrans_stream_t stream_;
public:
    UniStreamReader(pttrans_stream_t stream) : stream_(stream) {}
    virtual bool read_mem(GenericPointer addr, uint16 len, uint16 * data)
    {
        return (pttrans_stream_readgp_block(stream_, addr, len, data) == PTTRANS_SUCCESS);
    }
};


// Simple class to acquire and release spi;
class TransAcquireLock
{
private:
    enum
    {
        INFINITE_TIMEOUT = 0xFFFFFFFF
    };

    int mAcquired;
public:
    TransAcquireLock(uint32 aTimeout = INFINITE_TIMEOUT)
    {
        mAcquired = false;
        Acquire(aTimeout);
    }

    ~TransAcquireLock()
    {
        if (mAcquired)
        {
            pttrans_unlock();
        }
    }

    //  Will return true if acquire suceeds or if already acquired.
    bool Acquire(uint32 aTimeout = INFINITE_TIMEOUT)
    {
        if (mAcquired)
        {
            return true;
        }

        mAcquired = pttrans_lock(aTimeout);
        return (mAcquired != 0);
    }
};

// Simple class to acquire and release a spi stream
class TransAcquireStreamLock
{
private:
    enum
    {
        INFINITE_TIMEOUT = 0xFFFFFFFF
    };

    bool mAcquired;
    pttrans_stream_t mStream;
public:
    TransAcquireStreamLock(pttrans_stream_t stream, uint32 aTimeout = INFINITE_TIMEOUT)
    {
        mAcquired = false;
        mStream = stream;
        Acquire(aTimeout);
    }

    ~TransAcquireStreamLock()
    {
        if (mAcquired)
        {
            pttrans_stream_unlock(mStream);
        }
    }

    //  Will return true if acquire suceeds or if already acquired.
    bool Acquire(uint32 aTimeout = INFINITE_TIMEOUT)
    {
        if (mAcquired)
        {
            return true;
        }

        mAcquired = (pttrans_stream_lock(mStream, aTimeout) == PTTRANS_SUCCESS);
        return mAcquired;
    }
};

#endif //def __cplusplus

#endif // PTTRANSPORT_H
