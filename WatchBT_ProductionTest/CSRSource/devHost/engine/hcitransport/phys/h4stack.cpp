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

#include "h4stack.h"

#include <cstring>
#include <cassert>
#define ASSERT assert
#include <stdio.h>

#undef LINKED_TO_TIME_LIBRARY

#ifdef LINKED_TO_TIME_LIBRARY
#include "time/time_stamp.h"
#endif

#ifndef NDEBUG
#define DEBUG_PRINTING_H4
#endif

#ifdef DEBUG_PRINTING_H4
#define DB_PRINTF(x) printf x
#define DB_DUMP(X) dump_packet X

static void dump_packet(const char *txt, const uint8 *data, uint32 len)
{
    uint32 u;
    DB_PRINTF (("%s", txt));
    for (u = 0; u < len; ++u)
    {
        DB_PRINTF ((" %02x", data[u]));
    }
    DB_PRINTF ((" [len %lu]\n", (unsigned long) len));
}
#else
#define DB_PRINTF(x)
#define DB_DUMP(X)
#endif


void CallBack::initialise (void * aObject , void (*aCallBack)( bool , void *) )
{
    mObject = aObject;
    mCallBack = aCallBack;
}

void CallBack::execute ()
{
    mCallBack (true , mObject);
    //  delete this;
}

void CallBack::diedIncomplete ()
{
    mCallBack (false , mObject);
    //  delete this;
}

H4Stack::~H4Stack ()
{
    Stop();
    finalQueues();
    ASSERT (!IsActive());
}


H4Stack::H4Stack ( UARTPtr aFile )
:
    direction ( UartGoesToChip ),
    mFile ( aFile ),
    rxPktType ( 0 ),
    mThreadDead ( mThreadSignal ),
    RX_CHUNK_SIZE ( 256 )
{
    mComCallBackObject = 0 ;
    mComCallBackFunction = 0 ;
    mACLCallBackObject = 0 ;
    mACLCallBackFunction = 0 ;
    mSCOCallBackObject = 0 ;
    mSCOCallBackFunction = 0 ;
    initStateMachines ();
    initQueues();
}

void H4Stack::setDataWatch ( DataWatcher * w )
{
    mFile->setDataWatch ( w );
}

void H4Stack::setupRecvCommand ( void * aObject,
    void (*aRecvCallBack) ( uint8 * , size_t , void * ) )
{
    mComCallBackObject = aObject ;
    mComCallBackFunction = aRecvCallBack ;
}

void H4Stack::setupRecvACLData ( void * aObject, 
    void (*aRecvCallBack) ( uint8 * , size_t , void * ) )
{
    mACLCallBackObject = aObject ;
    mACLCallBackFunction = aRecvCallBack ;
}

void H4Stack::setupRecvSCOData ( void * aObject,
    void (*aRecvCallBack) ( uint8 * , size_t , void * ) )
{
    mSCOCallBackObject = aObject ;
    mSCOCallBackFunction = aRecvCallBack ;
}

void H4Stack::setupRecvEvent ( void * aObject,
    void (*aRecvCallBack) ( uint8 * , size_t , void * ) )
{
    mEvtCallBackObject = aObject ;
    mEvtCallBackFunction = aRecvCallBack ;
}

void H4Stack::recvCommand ( uint8 *apdu, size_t aLength )
{
    if ( mComCallBackFunction != NULL )
        mComCallBackFunction ( apdu , aLength , mComCallBackObject ) ;
}

void H4Stack::recvACLData ( uint8 *apdu, size_t aLength )
{
    if ( mACLCallBackFunction != NULL )
        mACLCallBackFunction ( apdu , aLength , mACLCallBackObject ) ;
}

void H4Stack::recvSCOData ( uint8 *apdu, size_t aLength )
{
    if ( mSCOCallBackFunction != NULL )
        mSCOCallBackFunction ( apdu , aLength , mSCOCallBackObject ) ;
}

void H4Stack::recvEvent ( uint8 *apdu, size_t aLength )
{
    if ( mEvtCallBackFunction != NULL )
        mEvtCallBackFunction ( apdu , aLength , mEvtCallBackObject ) ;
}

void H4Stack::setUartDirection ( bool aUartGoesToChip )
{
    direction = aUartGoesToChip ? UartGoesToChip : UartGoesToHost;
}

bool H4Stack::initQueues (void)
{
#ifdef H4_MULTIQUEUE
    int i;

    for (i = 0; i < H4_NUMQUEUES; ++i)
    {
        queue[i].head = NULL;
    }

    queue[H4_QUEUE_CMD].pktType = H4_PKTTYPE_CMD;
    queue[H4_QUEUE_ACL].pktType = H4_PKTTYPE_ACL;
    queue[H4_QUEUE_SCO].pktType = H4_PKTTYPE_SCO;
#else
    queue.head = NULL;
#endif

    return true;
}


void H4Stack::finalQueues (void)
{
#ifdef H4_MULTIQUEUE
    int i;

    for (i = 0; i < H4_NUMQUEUES; ++i)
    {
        deleteQueue (queue[i].head);
    }
#else
    deleteQueue (queue.head);
    queue.head = 0;
#endif
}


void H4Stack::deleteQueue (H4_PDULINK *pduLink)
{
    H4_PDULINK *nextPduLink;

    while (pduLink != NULL)
    {
        DB_PRINTF (("Tx: deleting a pending PDU\n"));
        nextPduLink = pduLink->next;
        pduLink->callback->diedIncomplete ();
        delete pduLink->PDU;
        delete pduLink->callback;
        delete pduLink;
        pduLink = nextPduLink;
    }
}


bool H4Stack::initStateMachines (void)
{
    txState = TXIDLE;
    rxState = RXIDLE;
#ifdef H4_SIMPLERESYNC
    resyncing = false;
    onSyncLoss = NULL;
#endif
    onUartDies = NULL;

    return true;
}

bool H4Stack::Start ()
{
    return mFile->open()
        && Threadable::Start()
        && ( Threadable::HighPriority() , true );
}

void H4Stack::Stop ()
{
    if (IsActive())
    {
        Threadable::Stop();
        DB_PRINTF (("H4: asking serial daemon to die\n"));
        mFile->abortWait();
        mThreadDead.wait();
    }
}

int H4Stack::ThreadFunc ()
{
    DB_PRINTF (("H4: client side initialised\n"));
    serialDaemon();
    mThreadDead.set();
    return 0;
}


void H4Stack::sendCommand (const uint8 *apdu, size_t aLength, CallBack *aCallBack)
{
    if ( direction == UartGoesToChip )
#ifdef H4_MULTIQUEUE
        sendPDU (H4_QUEUE_CMD, apdu, aLength, aCallBack);
#else
        sendPDU (H4_PKTTYPE_CMD, apdu, aLength, aCallBack);
#endif
}

void H4Stack::sendACLData (const uint8 *apdu, size_t aLength, CallBack *aCallBack)
{
#ifdef H4_MULTIQUEUE
    sendPDU (H4_QUEUE_ACL, apdu, aLength, aCallBack);
#else
    sendPDU (H4_PKTTYPE_ACL, apdu, aLength, aCallBack);
#endif
}

void H4Stack::sendSCOData (const uint8 *apdu, size_t aLength, CallBack *aCallBack)
{
#ifdef H4_MULTIQUEUE
    sendPDU (H4_QUEUE_SCO, apdu, aLength, aCallBack);
#else
    sendPDU (H4_PKTTYPE_SCO, apdu, aLength, aCallBack);
#endif
}

void H4Stack::sendEvent (const uint8 *apdu, size_t aLength, CallBack *aCallBack)
{
    if ( direction == UartGoesToHost )
#ifdef H4_MULTIQUEUE
        sendPDU (H4_QUEUE_EVT, apdu, aLength, aCallBack);
#else
        sendPDU (H4_PKTTYPE_EVT, apdu, aLength, aCallBack);
#endif
}

void H4Stack::sendDp (const uint8 *apdu, size_t aLength, CallBack *aCallBack)
{
#ifdef H4_MULTIQUEUE
    sendPDU (H4_QUEUE_DP, apdu, aLength, aCallBack);
#else
    sendPDU (H4_PKTTYPE_DP, apdu, aLength, aCallBack);
#endif 
}

void H4Stack::sendRaw (const uint8 *apdu, size_t aLength, CallBack *aCallBack)
{
#ifdef H4_MULTIQUEUE
    ASSERT("cannot send raw with H4_MULTIQUEUE" && false);
#else
    sendPDU (H4_PKTTYPE_RAW, apdu, aLength, aCallBack);
#endif
}

#ifdef H4_MULTIQUEUE
void H4Stack::sendPDU (H4_QUEUENUM queuenum, const uint8 *contents, uint32 length, CallBack *callback)
#else
void H4Stack::sendPDU (H4_PKTTYPE pktType, const uint8 *contents, size_t length, CallBack *callback)
#endif
{
    H4_PDU *pdu;
    H4_PDULINK *pduLink;

    ASSERT (IsActive());
#ifdef H4_MULTIQUEUE
    ASSERT (queuenum >= 0 && queuenum < H4_NUMQUEUES);
#else
    ASSERT (pktType >= H4_PKTTYPE_MIN && pktType < H4_PKTTYPE_MAX );
#endif
    ASSERT (contents != NULL);
    ASSERT (length >= 3 || pktType == H4_PKTTYPE_RAW); // No-parameter command or zero-length SCO packet, unless it's a raw send in which case there are no rules.
    ASSERT (callback != NULL);

    pdu = new H4_PDU;
    ASSERT (pdu != NULL);
    pdu->length = length;
    pdu->contents = contents;

    pduLink = new H4_PDULINK;
    ASSERT (pduLink != NULL);
    pduLink->next = NULL;
    pduLink->PDU = pdu;
#ifndef H4_MULTIQUEUE
    pduLink->pktType = pktType;
#endif
    pduLink->callback = callback;

    CriticalSection::Lock here ( cs );

#ifdef H4_MULTIQUEUE
    if (queue[queuenum].head == NULL)
    {
        DB_PRINTF (("Tx: pkttype %u length %lu added to empty queue %u\n", queue[queuenum].pktType, (unsigned long) length, queuenum));
        queue[queuenum].head = queue[queuenum].tail = pduLink;
    }
    else
    {
        DB_PRINTF (("Tx: pkttype %u length %lu added to tail of queue %u\n", queue[queuenum].pktType, (unsigned long) length, queuenum));
        queue[queuenum].tail = queue[queuenum].tail->next = pduLink;
    }
#else
    if (queue.head == NULL)
    {
        DB_PRINTF (("Tx: pkttype %u length %lu added to empty queue\n", pktType, (unsigned long) length));
        queue.head = queue.tail = pduLink;
    }
    else
    {
        DB_PRINTF (("Tx: pkttype %u length %lu added to tail of queue\n", pktType, (unsigned long) length));
        queue.tail = queue.tail->next = pduLink;
    }
#endif

    mFile->reqXmit();
}


bool H4Stack::ready (uint32 timeout)
{
    return IsActive() ;
}


void H4Stack::serialDaemon (void)
{
    bool dead = false;
    /*bool packet_waiting = false;*/

    DB_PRINTF (("H4: serial daemon started\n"));

    rxStateMachine (); /* Prime rx */

#ifdef MM01_DUMP_STATS
        mFile->DumpStats();
#endif /* def MM01_DUMP_STATS */

    while (!dead)
    {
        UARTAbstraction::Event event = mFile->wait ( 1000 , &rxPktRead );

        switch (event)
        {
        case UARTAbstraction::UART_RCVDONE:

#ifdef MM01_DUMP_STATS
            mFile->DumpStats();
#endif /* def MM01_DUMP_STATS */

            DB_PRINTF (("RXDONE.\n"));
            /* HERE need to check nbytes read as could be less than asked */
            /* for in case of comms error (see on-line doc, "Communications Errors") */
            rxStateMachine ();
            break;

        case UARTAbstraction::UART_XMITDONE:
            DB_PRINTF (("TXDONE.\n"));
            txStateMachine ();
            break;

        case UARTAbstraction::CLIENT_TXREQ:

            DB_PRINTF (("TXREQ.\n"));
            if ( txState == TXIDLE )
            {
                txStateMachine ();
            }
            else
            {
                                DB_PRINTF(("txstate = %d\n", txState));
            }
            break;

        case UARTAbstraction::UART_CTSPULSED:
            break;

        case UARTAbstraction::CLIENT_DIEREQ:

            finalQueues ();
            if (rxPkt)
            {
                delete[] rxPkt;
            }
            dead = true;
            break;

        case UARTAbstraction::UART_ERROR:
            if ( onUartDies )
            {
                onUartDies(onUartDiesArg);
            }
            dead = true;
            break;

        case UARTAbstraction::timedOut:
            //  if the timeout has occured and RTS not in the active state,
            //  then there is probably a problem.
            if ( rxState != RXPKTTYPE )
            {
                DB_PRINTF (( "RX has been waiting for data for a second.\n" ));
                DB_PRINTF (( "rxstate = %d; pkt type = %d" , rxState , rxPktType ));
                if ( rxPkt )
                {
                    DB_PRINTF (( "; data so far =" ));
                    for ( unsigned int i = 0 ; i < rxPktHdrLen ; ++i )
                    {
                        DB_PRINTF ((" %02x" , rxPkt[i] ));
                    }
                }
                DB_PRINTF (( ".\n" ));
#ifdef MM01_DUMP_STATS
                mFile->DumpStats();
#endif /* def MM01_DUMP_STATS */
            }
            break;
        default:
            break;
        }
    }

    DB_PRINTF (("H4: serial daemon terminated\n"));
}


void H4Stack::txStateMachine (void)
{
    bool retrigger;

    do
    {
        switch (txState)
        {
        case TXIDLE:
            {
                CriticalSection::Lock here (cs);
                retrigger = false;
#ifdef H4_MULTIQUEUE
                for (txActiveQueue = 0; txActiveQueue < H4_NUMQUEUES; ++txActiveQueue)
                {
                    if (queue[txActiveQueue].head != NULL)
                    {
                        DB_PRINTF (("Tx: sending from head of queue %u (pkttype %u)\n", txActiveQueue, queue[txActiveQueue].pktType));
                        retrigger = txData (1, &queue[txActiveQueue].pktType);
                        txState = TXPKTTYPE;
                        break;
                    }
                }
#else
                if (queue.head != NULL)
                {
                    // Make sure protocol detection packets are sent in one lump and don't send packet type if raw.
                    if (queue.head->pktType != H4_PKTTYPE_DP && queue.head->pktType != H4_PKTTYPE_RAW)
                    {
                        DB_PRINTF (("Tx: sending from head of queue (pkttype %u)\n", queue.head->pktType));
                        retrigger = txData (&queue.head->pktType,1);
                    }
                    else
                    {
                        retrigger = true;
                    }
                    txState = TXPKTTYPE;
                }
#endif
            }
            break;
        case TXPKTTYPE:

#ifdef H4_MULTIQUEUE
            DB_DUMP(("Tx: sending pkt",
            queue[txActiveQueue].head->PDU->contents,
            queue[txActiveQueue].head->PDU->length));
            retrigger = txData(queue[txActiveQueue].head->PDU->contents,
                               queue[txActiveQueue].head->PDU->length);
#else
            DB_DUMP(("Tx: sending pkt",
            queue.head->PDU->contents,
            queue.head->PDU->length));
            retrigger = txData(queue.head->PDU->contents,
                               queue.head->PDU->length);
#endif
            {
                txState = TXPKT;
            }

            break;

        case TXPKT:
        {
            H4_PDULINK *pduLink;

            {
                CriticalSection::Lock here ( cs );
#ifdef H4_MULTIQUEUE
                pduLink = queue[txActiveQueue].head;
                queue[txActiveQueue].head = pduLink->next;
#else
                pduLink = queue.head;
                queue.head = pduLink->next;
#endif
            }

            pduLink->callback->execute ();
            delete pduLink->PDU;
            delete pduLink->callback;
            delete pduLink;

            DB_PRINTF (("Tx: pkt sent\n"));
            retrigger = true;
            txState = TXIDLE;
            break;
        }

        default:

            ASSERT (txState && !"has unexpected value");

            break;
        }

    } while (retrigger);
}


#ifdef H4_SAS
static uint8 *rxPktPrev = NULL;
static uint32 rxPktPrevLen;
static bool rxPktShow = false;
#endif

void H4Stack::rxStateMachine (void)
{
#ifdef H4_SAS
    static uint32 badcount = 0;
#endif
    uint32 rxAmount;

    bool retrigger;

    do
    {
        switch (rxState)
        {
        case RXIDLE:
            DB_PRINTF (("Rx: waiting for pkt\n"));
            rxPkt = NULL;
            retrigger = rxData ( &rxPktType , 1 , &rxPktRead );
            rxState = RXPKTTYPE;
            break;

        case RXPKTTYPE:

            if ( rxPktRead != 1 )
            {
                DB_PRINTF (( "Rx: returned early.\n"));
                assert ( rxPktRead == 0 );
                retrigger = true;
                rxState = RXIDLE;
                break;
            }

            switch (rxPktType)
            {
            case H4_PKTTYPE_CMD:
                if ( direction == UartGoesToHost )
                {
                    DB_PRINTF (("Rx: receiving command pkt\n"));
                    rxPktHdrLen = 3;
                }
                else
                    rxPktHdrLen = 0;
                break;

            case H4_PKTTYPE_ACL:
                DB_PRINTF (("Rx: receiving ACL pkt\n"));
                rxPktHdrLen = 4;
                break;

            case H4_PKTTYPE_SCO:
                DB_PRINTF (("Rx: receiving SCO pkt\n"));
                rxPktHdrLen = 3;
                break;

            case H4_PKTTYPE_EVT:
                if ( direction == UartGoesToChip )
                {
                    DB_PRINTF (("Rx: receiving event pkt\n"));
                    rxPktHdrLen = 2;
                }
                else
                    rxPktHdrLen = 0;
                break;

            default:

                rxPktHdrLen = 0;
                break;
            }

            if ( rxPktHdrLen == 0 )
            {
#ifdef H4_SIMPLERESYNC
#ifdef H4_SAS
                if (badcount++ == 0)
                {
                    size_t i;
                    fprintf (stderr, "Rx: lost sync\n");
                    printf ("Rx: prev:");
                    if (rxPktPrev)
                    {
                        for (i = 0; i < rxPktPrevLen; ++i)
                        {
                            printf (" %02x", rxPktPrev[i]);
                        }
                    }
                    else
                    {
                        printf (" none");
                    }
                    printf ("\nRx: this:");
                }
                fprintf (stderr, " %02x", rxPktType);
#endif
                if (!resyncing)
                {
                    resyncing = true;
                    if (onSyncLoss != NULL)
                    {
                        onSyncLoss (onSyncLossArg);
                    }
                    else
                    {
                        fprintf (stderr, "H4Stack: sync lost\n");
                    }
                }
                DB_PRINTF (("Rx: bad pktType (0x%02x) -- discarding and waiting again\n", rxPktType));
                rxPktHdrLen = 0;
#else
                ASSERT ((rxPktType, false)); /* HERE Should somehow notify upper layers so they can do the official recovery procedure */
                /* HERE for fancy resync scan for 04 0e 04 xx 03 0c 00 */
#endif
            }

#ifdef H4_SIMPLERESYNC
            if (rxPktHdrLen == 0)
            {
                rxState = RXIDLE;
                retrigger = true;
                break;
            }
            resyncing = false;
#ifdef H4_SAS
            if (badcount)
            {
                fprintf (stderr, "\nRx: resynchronised after %lu bad octets\n", (unsigned long) badcount);
                badcount = 0;
                rxPktShow = true;
            }
#endif
#endif
            rxPktIndex = 0;
            retrigger = rxData ( rxPktHdr , rxPktHdrLen , &rxPktRead );
            rxState = RXPKTHDR;

            break;

        case RXPKTHDR:

            rxPktIndex += rxPktRead;
            if ( rxPktIndex != rxPktHdrLen )
            {
                retrigger = rxData ( rxPktHdr + rxPktIndex , rxPktHdrLen - rxPktIndex , &rxPktRead );
                break;
            }

            switch (rxPktType)
            {
            case H4_PKTTYPE_CMD:
                rxPktPayloadLen = rxPktHdr[2];
                break;

            case H4_PKTTYPE_ACL:
                rxPktPayloadLen = rxPktHdr[2] | (rxPktHdr[3] << 8);
                break;

            case H4_PKTTYPE_SCO:
                rxPktPayloadLen = rxPktHdr[2];
                break;

            case H4_PKTTYPE_EVT:
                rxPktPayloadLen = rxPktHdr[1];
                break;

            }

            rxPktSize = rxPktHdrLen + (uint32) rxPktPayloadLen;
            rxPktTotalLen = rxPktSize;
            DB_PRINTF (("Rx: pkt total length is %lu:%lu\n", (unsigned long) rxPktSize, (unsigned long) rxPktTotalLen));
            {
                rxPkt = new uint8[rxPktTotalLen];
            }
            ASSERT (rxPkt != NULL);
            memcpy (rxPkt, rxPktHdr, rxPktHdrLen);
            {
                rxAmount = rxPktTotalLen;
                if (rxAmount > RX_CHUNK_SIZE)
                    rxAmount = RX_CHUNK_SIZE;
            }
            rxAmount -= rxPktIndex;
            retrigger = rxData ( rxPkt + rxPktIndex , rxAmount , &rxPktRead);
            rxState = RXPKT;
            break;

        case RXPKT:
            rxPktIndex += rxPktRead;
            if ( rxPktIndex < rxPktTotalLen )
            {
                DB_PRINTF (("Returned early. Got %d so far.\n" , rxPktIndex ));
                {
                    rxAmount = rxPktTotalLen - rxPktIndex;
                    if (rxAmount > RX_CHUNK_SIZE)
                        rxAmount = RX_CHUNK_SIZE;
                }
                retrigger = rxData(rxPkt + rxPktIndex, rxAmount, &rxPktRead);
                break;
            }
            DB_DUMP(("Rx: received packet", rxPkt, rxPktTotalLen));
            DB_PRINTF(("Actually read %lu bytes\n", (unsigned long) rxPktRead));
#ifdef H4_SAS
            if (rxPktPrev)
            {
                free (rxPktPrev);
            }
            rxPktPrev = malloc (rxPktTotalLen);
            memcpy (rxPktPrev, rxPkt, rxPktTotalLen);
            rxPktPrevLen = rxPktTotalLen;
            if ( false && rxPktShow)
            {
                size_t i;
                printf ("Rx: next:");
                for (i = 0; i < rxPktTotalLen; ++i)
                {
                    printf (" %02x", rxPkt[i]);
                }
                printf ("\n");
                rxPktShow = false;
            }
#endif
            switch (rxPktType)
            {
            case H4_PKTTYPE_CMD:

                recvCommand (rxPkt, rxPktSize);
                break;

            case H4_PKTTYPE_ACL:

                recvACLData (rxPkt, rxPktSize);
                break;

            case H4_PKTTYPE_SCO:

                recvSCOData (rxPkt, (uint16 /* Always safe */) rxPktSize);
                break;

            case H4_PKTTYPE_EVT:

                recvEvent (rxPkt, (uint16 /* Always safe */) rxPktSize); /* Note use recvCommand for events */
                break;
            }

            delete[] rxPkt;
            retrigger = true;
            rxState = RXIDLE;
            break;

        default:
            ASSERT (rxState && !"has unexpected value");
            break;
        }

    } while (retrigger);
}


#ifdef H4_SIMPLERESYNC
void H4Stack::setupOnSyncLoss (void (*onSyncLoss) (void *), void *onSyncLossArg)
{
    this->onSyncLoss = onSyncLoss;
    this->onSyncLossArg = onSyncLossArg;
}
#endif

void H4Stack::setupOnUartDies (void (*fn) (void *), void *param)
{
    this->onUartDies = fn;
    this->onUartDiesArg = param;
}

//  HERE: Think about these!
bool H4Stack::txData ( const uint8 * buf , size_t len )
{
    uint32 wrote;
    return ( mFile->write ( buf , len , &wrote ) );
}

bool H4Stack::rxData ( uint8 * buf , size_t len , uint32 *read )
{
    uint32 lread;
    if ( read == 0 )
        read = &lread;
    return mFile->read ( buf , len , read );
}

