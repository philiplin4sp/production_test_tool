#ifndef FASTPIPE_ENGINE_IFACE_H
#define FASTPIPE_ENGINE_IFACE_H

#include "common/types.h"
#include "fastpipe_engine/status_codes.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern bool showFp;

enum FastPipeEngineResult
{
    FASTPIPE_ENGINE_SUCCESS,
    FASTPIPE_ENGINE_ERROR_NOT_ENABLED,
    FASTPIPE_ENGINE_ERROR_INCORRECT_TOKEN_PIPE_ID,
    FASTPIPE_ENGINE_ERROR_RECYCLED_PIPE_ID,
    FASTPIPE_ENGINE_ERROR_RECYCLED_HANDLE,
    FASTPIPE_ENGINE_ERROR_UNCONNECTED_PIPE_ID,
    FASTPIPE_ENGINE_ERROR_DESTROYING_TOKEN_PIPE,
    FASTPIPE_ENGINE_ERROR_UNREQUESTED_DESTROY,
    FASTPIPE_ENGINE_ERROR_VIOLATES_LIMITH,
    FASTPIPE_ENGINE_ERROR_VIOLATES_LIMITC,
    FASTPIPE_ENGINE_ERROR_UNCONNECTED_HANDLE,
    FASTPIPE_ENGINE_ERROR_ALREADY_RECEIVING,
    FASTPIPE_ENGINE_ERROR_NOT_RECEIVING,
    FASTPIPE_ENGINE_ERROR_COULD_NOT_OPEN_FILE,
    FASTPIPE_ENGINE_ERROR_CURRENTLY_REDUCING_LIMITC,
    FASTPIPE_ENGINE_SUCCESS_REDUCED_LIMITC,
    FASTPIPE_ENGINE_SUCCESS_REDUCING_LIMITC,
    FASTPIPE_ENGINE_ERROR_REDUCTION_VIOLATES_LIMITC,
    FASTPIPE_ENGINE_ERROR_NEW_LIMITC_VIOLATES_FLOWCONTROL,
    FASTPIPE_ENGINE_ERROR_UNRECOGNISED
};

void fastpipeEngineInit(void);
void fastpipeEngineDeInit(void);
void fastpipeEngineReInit(void);
void fastpipeEngineEnable(uint32 limith);
enum FastPipeEngineResult fastpipeEngineDestroyChannelSender(uint16 ph);
enum FastPipeEngineResult fastpipeEngineConnect(uint16 p,
                                                uint32 overh,
                                                uint32 capacityh,
                                                uint32 overhc,
                                                uint32 capacitytxhc,
                                                uint32 capacityrxhc,
                                                uint16 handle);
enum FastPipeEngineResult fastpipeEngineDisconnect(uint16 p);
void fastpipeEngineDisplayInfo(void);
bool fastpipeEngineIsConnected(uint16 ch);
bool fastpipeFileSendEnable(const uint16 ch, const char *const fileName, long *file_size);
bool fastpipeFileSendDisable(const uint16 ch);
bool fastpipeFileSendStart(const uint16 ch);
enum FastPipeEngineResult fastpipeEngineFileReceiveEnable(uint16 ch, const char *const fileName);
enum FastPipeEngineResult fastpipeEngineFileReceiveDisable(uint16 ch);
enum FastPipeEngineResult fastpipeEngineReduceLimitc(uint32 reduction);
enum FastPipeEngineResult fastpipeEngineResize(uint32 limitc);
void fastpipeFstat(uint16 ch);
void fastpipeFstatAll(void);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

// This function is only accessible from C++

class HCIDataPDU;

void fastpipeEngineReceive(const HCIDataPDU &);
bool fastpipeEngineSend(const HCIDataPDU &);

#endif

#endif // FASTPIPE_ENGINE_IFACE_H
