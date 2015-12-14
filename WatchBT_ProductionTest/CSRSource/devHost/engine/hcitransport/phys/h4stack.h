///////////////////////////////////////////////////////////////////////
//
//  FILE   :  H4Stack
//
//  AUTHOR :  Mark RISON
//
//  CLASS  :  H4Stack
//
//  PURPOSE:  Generic portion of H4 implementation of HCITransport
//
///////////////////////////////////////////////////////////////////////

#ifndef __H4STACK_H__
#define __H4STACK_H__

#include "common/types.h"
#include "uart.h"
#include "thread/critical_section.h"
#include "thread/thread.h"
#include "thread/signal_box.h"

class CallBack
{
public:
    void initialise (void * aObject , void (*aCallBack)( bool , void *) );
    void execute ();
    void diedIncomplete();
private:
    void *mObject;
    void (*mCallBack) ( bool , void *);
};

#define H4_SIMPLERESYNC /* Don't die if bad pkttype; just wait until good pkttype comes along */
#define H4_DEFAULT_DATARATE 38400
//#define H4_MULTIQUEUE /* Have separate and prioritised queues for each pkttype */
//#define H4_SAS /* Special high-power debugging for Steve and Steve -- needs H4_SIMPLERESYNC */

class H4Stack : public Threadable
{
public:
    enum padding { no_padding , even_padding , max_padding };
    H4Stack ( UARTPtr aFile );
    virtual ~H4Stack ();

    bool ready (uint32 timeout);
    bool Start ();
    void Stop ();

    void sendCommand (const uint8 *apdu, size_t aLength, CallBack *aCallBack);
    void sendACLData (const uint8 *apdu, size_t aLength, CallBack *aCallBack);
    void sendSCOData (const uint8 *apdu, size_t aLength, CallBack *aCallBack);
    void sendEvent   (const uint8 *apdu, size_t aLength, CallBack *aCallBack);
    void sendDp      (const uint8 *apdu, size_t aLength, CallBack *aCallBack);
    void sendRaw     (const uint8 *apdu, size_t aLength, CallBack *aCallBack);

    void setupRecvCommand ( void * aObject,
                            void (*aRecvCallBack) ( uint8 * , size_t , void * ) ); // HCI Event
    void setupRecvACLData ( void * aObject, 
                            void (*aRecvCallBack) ( uint8 * , size_t , void * ) ); // ACL
    void setupRecvSCOData ( void * aObject,
                            void (*aRecvCallBack) ( uint8 * , size_t , void * ) ); // SCO
    void setupRecvEvent   ( void * aObject,
                            void (*aRecvCallBack) ( uint8 * , size_t , void * ) ); // HCI Command

#ifdef H4_SIMPLERESYNC
    void setupOnSyncLoss (void (*onSyncLoss) (void *), void *onSyncLossArg);
#endif
    void setupOnUartDies (void (*onUartDies) (void *), void *onUartDiesArg);

    void setUartDirection ( bool aUartGoesToChip );

    void setDataWatch ( DataWatcher * w );
protected:
    void recvCommand ( uint8 *apdu, size_t aLength );
    void recvACLData ( uint8 *apdu, size_t aLength );
    void recvSCOData ( uint8 *apdu, size_t aLength );
    void recvEvent   ( uint8 *apdu, size_t aLength );

private:
    enum { UartGoesToChip , UartGoesToHost } direction;


    /* Generic */
    int ThreadFunc();

    enum H4_PKTTYPE        /* See BT1.0B/H:4;2 */
    {
        H4_PKTTYPE_MIN = 1,
        H4_PKTTYPE_CMD = 1,
        H4_PKTTYPE_ACL = 2,
        H4_PKTTYPE_SCO = 3,
        H4_PKTTYPE_EVT = 4,
        H4_PKTTYPE_DP  = 5,
        H4_PKTTYPE_RAW = 6,
        H4_PKTTYPE_MAX
    };

    #ifdef H4_MULTIQUEUE
    enum H4_QUEUENUM    /* Highest-priority is lowest-numbered */
    {
        H4_QUEUE_SCO = 0,        /* Always number the first one zero */
        H4_QUEUE_CMD,            /* Always increment by one */
        H4_QUEUE_ACL
    };
    #define H4_NUMQUEUES 3
    #endif

    struct H4_PDU
    {
        size_t length;
        const uint8 *contents;
    };

    struct H4_PDULINK
    {
        H4_PDULINK *next;        /* Points to NULL if last */
        H4_PDU *PDU;
        #ifndef H4_MULTIQUEUE
        uint8 pktType;            /* Must be of size 8 bits, so can't use H4_PKTTYPE */
        #endif
        CallBack *callback;
    };

    struct H4_QUEUE
    {
        H4_PDULINK *head;        /* Points to NULL if the queue is empty */
        H4_PDULINK *tail;        /* Undefined if the queue is empty (see head) */
        #ifdef H4_MULTIQUEUE
        uint8 pktType;            /* Must be of size 8 bits, so can't use H4_PKTTYPE */
        #endif
    };
    #ifdef H4_MULTIQUEUE
    H4_QUEUE queue[H4_NUMQUEUES];
    #else
    H4_QUEUE queue;
    #endif

    enum H4_TXSTATE
    {
        TXIDLE,
        TXPKTTYPE,
        TXPKT
    };
    H4_TXSTATE txState;

    #ifdef H4_MULTIQUEUE
    int txActiveQueue;
    #endif

    enum H4_RXSTATE
    {
        RXIDLE,
        RXPKTTYPE,
        RXPKTHDR,
        RXPKT
    };
    H4_RXSTATE rxState;

    UARTPtr mFile;
    uint8 rxPktType, rxPktHdr[4], *rxPkt;
    unsigned rxPktHdrLen, rxPktPayloadLen, rxPktIndex;
    uint32 rxPktTotalLen, rxPktRead, rxPktSize;
#ifdef H4_SIMPLERESYNC
    bool resyncing;
    void (*onSyncLoss) (void *p);
    void *onSyncLossArg;
#endif
    void (*onUartDies) (void *p);
    void *onUartDiesArg;

    SignalBox mThreadSignal;
    SignalBox::Item mThreadDead;

    bool initQueues (void);
    void finalQueues (void);
    void deleteQueue (H4_PDULINK *pduLink);

    bool initStateMachines (void);

#ifdef H4_MULTIQUEUE
    void sendPDU (H4_QUEUENUM queuenum, const uint8 *PDU, size_t length, CallBack *callback);
#else
    void sendPDU (H4_PKTTYPE pktType, const uint8 *PDU, size_t length, CallBack *callback);
#endif

    void serialDaemon (void);
    void txStateMachine (void);
    void rxStateMachine (void);

    bool txData ( const uint8 * buf , size_t len );
    bool rxData ( uint8 * buf , size_t len , uint32 * read);
    //  RX_CHUNK_SIZE is the maximum amount of data that we read
    //  from the UART in one go
    const uint32 RX_CHUNK_SIZE;
    CriticalSection cs;

    //  Pointers to objects and callbacks.
    //  The callbacks must be declared static in the object,
    void *mComCallBackObject;
    void (*mComCallBackFunction) ( uint8 *, size_t , void * );
    void *mEvtCallBackObject;
    void (*mEvtCallBackFunction) ( uint8 *, size_t , void * );
    void *mACLCallBackObject;
    void (*mACLCallBackFunction) ( uint8 *, size_t , void * );
    void *mSCOCallBackObject;
    void (*mSCOCallBackFunction) ( uint8 *, size_t , void * );

};

#endif // __H4STACK_H__

