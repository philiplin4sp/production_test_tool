// Copyright (C) 2000-2006 Cambridge Silicon Radio Ltd.; all rights reserved.
// The external interface to btcli.
// $Id: //depot/bc/bluesuite_2_4/devHost/btcli/btcli.h#1 $

#ifndef BTCLI_H
#define BTCLI_H


#include "types.h"
#include "typedefs.h"

#define SCO_COUNT 3

#ifdef __cplusplus
extern "C"
{
#endif
    extern ichar *logname;
    extern char *sconame[SCO_COUNT];
#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
extern "C"
{
#endif

#include "fmdecode.h"

/* Call this before calling any other function */
void parseInit (void);

/* Start the default CLI */
void btcli (void);

/* Parse a single command (option to force echo) */
void parseCmd (const char *s, bool force);

/* Execute a batch file */
void exec_do (const char *filename);

/* Decode an HCI event */
void decodeEvt (const PA pa);

/* Decode an HCI ACL data packet */
void decodeAcl (u16 ch, u8 pbf, u8 bf, const u8 *data, u32 datalen);


    
    
/* Decode a bccmd response */
void decodeBCCMD (const PA pa);

/* Decode an hq indication */
void decodeHQ (const PA pa);
//#ifdef __cplusplus
//}
//#endif

/* Decode a debug packet */
void decodeDebug (const u8 *buf, u32 len);

/* Decode an LMP debug packet */
void decodeLMPdebug (const u8 *buf, u32 len, char *whoami);


/* Send an HCI command (to be provided) */
void sendCmd (PA pa);

/* Send an HCI command packet straight to transport layer (to be provided) */
/* The packet buffer must have been malloced and will be auto-freed. */
void sendCmdRaw (u8 *pkt, u16 pktlen);

/* Send an HCI ACL data packet (to be provided) */
int sendAcl (u16 ch, u16 cid, u8 pbf, u8 bf, const u8 *data, u32 datalen, bool l2capheader_enabled, u16 l2caplen);

/* Send an HCI ACL data packet straight to transport layer (to be provided);
 * any resulting nocp event will confuse the DeviceController */
int sendAclRaw (u16 ch, u16 cid, u8 pbf, u8 bf, const u8 *data, u32 datalen, u16 l2caplen);

/* Send an HCI SCO data packet (to be provided) */
int sendSco (u16 ch, const u8 *data, u8 datalen);

/* Send an HCI SCO data packet (to be provided) */
/* The packet buffer must have been malloced and will be auto-freed,
 * and the SCO data must start at offset 3 so that the SCO header
 * can be written in front (the datalen is still the SCO data length) */
int sendScoNoCopy (u16 ch, u8 *pkt, u8 datalen);

/* Send a bccmd command (to be provided) */
void sendBCCMD (const PA pa);

/* Send a raw BCCMD */
void sendRawBCCMD (uint16 req_type, uint16 varid, uint16 seq_no, const uint16 *payload, size_t payload_len);

/* Send VM data (to be provided) */
int sendVMdata (const u8 *data, u32 len);

/* Send FastPipe data */
bool sendFp(uint16 fpch, uint16 length, const uint8 *data);

bool sendFpTokens(uint8 ph, uint16 size);

/* How much ACL data is currently queued? */
size_t getSizeQueued (u16 ch);

/* How much ACL total data is currently queued for all channels? */
size_t getTotalSizeQueuedInAllChannels ();

/* Flush ACL data currently queued, return how much was flushed */
size_t flushQueued (u16 ch);

/* check if there are any pending items on the Regulator*/
size_t pendingItemsOnRegulator();

/* Load the FSM data (state and event names) */
void loadFSMdata (const ichar *fsmdir);

/*
 * Load the symbol data (symbol names and corresponding addresses).
 * The arguments mean the same but I don't know what the hell to do
 * with the unicode stuff.
 */
bool loadSymbolData (const ichar *sympath, const char *csympath);

/* These two commands support  */
bool trans_sleep_now(void);
void trans_sleep_always(void);
bool trans_sleep_never(void);

/* Block callbacks */
#define block_callbacks() //HERE

/* Unblock callbacks */
#define unblock_callbacks() //HERE

/* Whether to snarf raw HCI traffic */
void setDataWatch (void);
void unsetDataWatch (void);

/* To display snarfed and hoarded raw HCI traffic */
void showWatchedData (void);

bool isAclChannelOpen(uint16 ch);
bool openAclChannel (uint32 chNum);
bool closeAclChannel (uint32 chNum);
bool sendFile(uint16 ch, const char *fileName, bool flushable);
bool recvFile(uint16 ch, const char *fileName, bool flushable);
bool isSendFileActive(uint16 ch);
bool isRecvFileActive(uint16 ch);
bool cancelFileSend(uint16 ch, uint32 *fileSize, uint32 *bytesSent);
bool cancelFileRecv(uint16 ch, uint32 *fileSize, uint32 *bytesReceived);
void aclEngineReInit();
void aclFstat(uint32 ch);
void aclFstatAll();

void setAclQueueUpperThreshold(uint32 upperThreshold);
void setAclQueueLowerThreshold(uint32 lowerThreshold);
uint32 getAclQueueUpperThreshold();
uint32 getAclQueueLowerThreshold();

bool openScoChannel (uint16 sco_num, uint16 ch);
bool closeScoChannel (uint16 sco_num, uint16 ch);
bool enableScoLogging(uint16 sco_num, const char* fileName);
bool wEnableScoLogging(uint16 sco_num, const wchar_t *fileName);
bool iEnableScoLogging(uint16 sco_num, const ichar *fileName);
void disableScoLogging(uint16 sco_num);
bool enableScoFileSend(uint16 scoch, 
                       const char *fileName,  
                       uint32 rate, 
                       uint32 sampleSize,
                       bool overridePacketSize,
                       uint16 packetSize,
                       bool overrideNumPackets,
                       uint16 numPackets);
bool disableScoFileSend(uint16 scoch);
bool warm_reset();
bool reconfigure_uart_baudrate(uint32 baudrate);
void scoEngineReInit();

/* Sleep command for use on POSIX and WIN32 platforms */
void sleep_ms(int ms);


#define THREADING_WORKAROUND
#ifdef THREADING_WORKAROUND
/* keep the printfs safe from over writing eachother */
void btcli_enter_critical_section(void);
void btcli_leave_critical_section(void);


void sendH4dp();
void sendH4Raw(const uint8 *data, size_t len);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include "thread/critical_section.h"
extern CriticalSection cs;
#endif

#endif /* THREADING_WORKAROUND */

#include "fastpipe_engine_iface.h"


#ifdef __cplusplus
#endif

#endif /* BTCLI_H */
