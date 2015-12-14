// Copyright (C) 2000-2006 Cambridge Silicon Radio Ltd.; all rights reserved.
// C++ 'main' file for btcli application.
// Includes callbacks for transports (isolates btcli from the underlying C++).
// $Id: //depot/bc/bluesuite_2_4/devHost/btcli/btcli.cpp#1 $
//  MODIFICATION HISTORY
//     #159   20:feb:04 pws    B-2032: Add -U option.
//      #166   14:jun:04 mm     B-3048: Added BlueZ.
//      #167   15:jun:04 mm     B-3048: Added BlueZ (fixed).
//      #168   15:jun:04 mm     B-3048: AutoUp Added (fixed.
//    #170   10:nov:04 mm    B-4709: Allow bdaddr to name bluez device
//    #171   08:apr:05 sms    B-6261: Ressurect raw ACL command.

#include "btcli.h"

#include <stdlib.h>
#include <stdio.h> // HERE for FILE *fsco -- remove when moved
#include <ctype.h>

#if !defined(_WINCE) && !defined(_WIN32_WCE)
#include <assert.h>
#define ASSERT assert
#else
#include <windef.h>
#include <dbgapi.h>
#include "wince/wincefuncs.h"
#endif

#ifndef _WIN32
#include <signal.h>
#endif

#ifdef MGRWIN32ALLOCDEBUG
#include "crtdbg.h"
#endif
#ifdef MMWIN32ALLOCDEBUG
#include "crtdbg.h"
#endif

#include "common/nocopy.h"

#include "constants.h"

#include "aclengine/aclengine.h"
#include "scoengine/scoengine.h"

#ifndef BLUECONTROLLER_MODEL
#error multiple dll btcli has gone bye-bye
#else  /* BLUECONTROLLER_MODEL */
#include <string.h>
#include "unicode/ichar.h"

#include "csrhci/bluecontroller.h"
#include "csrhci/transportconfiguration.h"
#endif /* BLUECONTROLLER_MODEL */

#ifdef LATENCY_TRACKING
#include "soe/EventTracker.h"
#endif

#ifndef _WIN32_WCE
#include <errno.h>
#endif

#include <cassert>
#include "common/globalversioninfo.h"

bool tunnel = false;
ichar* submode = NULL;
//  BCSP defaults.  These override the defaults in the BCSP stackconfiguration
bool bcspUseCRCs = true;
uint32 bcspResendTimeout = 250;
uint32 bcspRetryLimit = 20;
uint8 bcspWindowSize = 4;
bool bcspUseLE = true;

int h4ds_wakeup_len = 8;
#ifdef linux
static bool autoup = false;
#endif
#include "decodecallback.h"
DecodeCallBack decodeCallBack;
BlueCoreDeviceController_newStyle *pdd;
AclEngine *aclEngine;
ScoEngine *scoEngine;

#define NOTRANSPORT 0
#define BCSP 1
#define USB 2
#define H4 3
#define H5 5
#define H4DS 6
#define SDIO 8
#define SOCK 9
#ifndef NO_BLUEZ
#define BLUEZ 11
#endif//NO_BLUEZ
#ifdef MM_BCSP_H5
#define BCSP_H5 10
#endif

#ifdef _WIN32
#if !defined(_WINCE) && !defined(_WIN32_WCE)
#define NUM_COM_PORTS 20
#define COM_PORT_GEN II("COM%d"), i + 1
#define COM_PORT_LEN (10)
#else
#define NUM_COM_PORTS 20
#define COM_PORT_GEN II("COM%d:"), i + 1
#define COM_PORT_LEN (10)
#endif
#define USB_PORT_DEF II("\\\\.\\csr0")
#else
#ifdef linux
#define NUM_COM_PORTS 2
#define COM_PORT_GEN II("/dev/ttyS%d"), i
#define COM_PORT_LEN (15)
#define USB_PORT_DEF II("hci0")
#ifndef NO_BLUEZ
#define BLUEZ_PORT_DEF II("hci0")
#endif//NO_BLUEZ
#else
#define NUM_COM_PORTS 2
#define COM_PORT_GEN II("/dev/term/%c"), i + 'a'
#define COM_PORT_LEN (15)
#endif
#endif

#define RESTART_TRANSPORT_RESTART_TIMEOUT       2000
#define RESTART_COMMS_ESTABLISH_TIMEOUT         100


#include "btcli.h"

extern "C"
{
#include "vars.h"
#include "print.h" /* For log file */
#include "dictionaries.h"
#include "globals.h"
#undef bool
}

#ifdef THREADING_WORKAROUND
//#include "thread/critical_section.h"
CriticalSection cs;
#endif /* THREADING_WORKAROUND */

extern "C"
{


#ifdef THREADING_WORKAROUND
void btcli_enter_critical_section(void)
{
    cs.Enter();
}
void btcli_leave_critical_section(void)
{
    cs.Leave();
}
#endif /* THREADING_WORKAROUND */

bool trans_sleep_now()
{
    return pdd->SleepControl(SLEEP_TYPE_NOW);
}

void trans_sleep_always()
{
    pdd->SleepControl(SLEEP_TYPE_NOW);
}

bool trans_sleep_never(void)
{
    return pdd->SleepControl(SLEEP_TYPE_NEVER);
}

void sendCmd (PA pa)
{
    bool ok = false;
#if 0
    size_t u;
    printlf ("[Sending cmd:");
    for (u = 0; u < pa[0]; ++u)
    {
        printlf (" 0x%lx", (ul) pa[u]);
    }
    printlf (" [len %lu]]\n", (ul) pa[0]);
#endif
    HCIPDU * pdu = HCIPDUFactory::createCommandPDU(pa);
    if (pdu != 0)
    {
        HCICommandPDU cmd (*pdu);
        delete pdu;
        pdu = NULL;

        if (cmd.get_op_code() == HCI_READ_BUFFER_SIZE)
        {
            aclEngine->sentRbs();
            scoEngine->sentRbs();
        }

        if (cmd.get_op_code() == HCI_RESET)
        {
            ok = pdd->forceReset(1000, BlueCoreDeviceController::hci);
        }
        else
        {
            ok = pdd->send (cmd);
        }
    }
    //ok = pdd->sendHCICommand ((uint32 *) pa); /* HERE get rid of the cast! */
    if (!ok)
    {
        printlf ("rejected by DeviceController\n");
    }
}

void sendCmdRaw (uint8 *pkt, uint16 pktlen)
{
    //pdd->sendRawHCICommand (pkt, pktlen, false);

    pdd->send (HCICommandPDU (pkt, pktlen), false);
}


int sendVMdata (const uint8 *data, u32 len)
{
#if 0
{
    size_t u;
    printlf ("[Sending VM data:");
    for (u = 0; u < len; ++u)
    {
        printlf (" 0x%lx", (ul) data[u]);
    }
    printlf (" [len %lu]]\n", (ul) len);
}
#endif
    //return pdd->sendRawVMdata ( data , len );

    return pdd->send (VM_PDU (data, len));
}

HCIACLPDU::packet_boundary_flag conv_pbf(uint8 x)
{
    switch(x)
    {
    case START:
        return HCIACLPDU::start;
    case CONTIN:
        return HCIACLPDU::cont;
    default:
        return (HCIACLPDU::packet_boundary_flag)x;
    }
}

HCIACLPDU::broadcast_type conv_bf(uint8 x)
{
    switch (x)
    {
    case bc_type_p2p:
        return HCIACLPDU::ptp;
    case bc_type_active:
        return HCIACLPDU::act;
    case bc_type_piconet:
        return HCIACLPDU::pic;
    default:
        return (HCIACLPDU::broadcast_type) x;
    }
}

extern "C"
{
extern uint16 aclPacketLength;
extern uint8 syncDataPacketLength;
extern uint16 aclPacketNumber;
extern uint16 syncDataPacketNumber;
}

#include <list>

int sendAcl (uint16 ch, uint16 cid, uint8 pbf, uint8 abf, const uint8 *data, uint32 datalen, bool l2capheader_enabled, u16 l2caplen)
{
    bool ok = true;

#if 0
    BROADCAST_TYPE bf = (BROADCAST_TYPE) abf;
    size_t u;
    printlf ("[Sending ACL data: ch:0x%04x, pbf:%u, bf:%u, data:", ch, pbf, bf);
    for (u = 0; u < datalen; ++u)
    {
        printlf ("%02x", data[u]);
    }
    printlf (" [len %x]]\n", (ul) datalen);
#endif
    ASSERT (datalen < 0x10004);

    if ((pbf == START || pbf == START_NAF) && l2capheader_enabled)
    {
        HCIACL_L2CAP_PDU_Helper pdu((uint16)datalen);
        pdu.set_header(ch, conv_pbf(pbf), conv_bf(abf));
        pdu.set_l2capData(data, (uint16)datalen);
        pdu.set_l2capChNum(cid);
        pdu.set_l2caplength(l2caplen);
        pdu.set_length();
        ok = aclEngine->send(pdu);
    }
    else
    {
        ExtendedHCIACLPDU_Helper pdu((uint16)datalen);
        pdu.set_header(ch, conv_pbf(pbf), conv_bf(abf));
        pdu.set_data(data, (uint16)datalen);
        pdu.set_length();
        ok = aclEngine->send(pdu);
    }


    if (!ok)
    {
        printlf ("rejected by DeviceController\n");
    }

    return ok;
}

int sendAclRaw (uint16 ch, uint16 cid, uint8 pbf, uint8 abf, const uint8 *data, uint32 datalen, u16 l2caplen)
{

    bool ok = true;

#if 0
    BROADCAST_TYPE bf = (BROADCAST_TYPE) abf;
    size_t u;
    printlf ("[Sending ACL data: ch:0x%04x, pbf:%u, bf:%u, data:", ch, pbf, bf);
    for (u = 0; u < datalen; ++u)
    {
        printlf ("%02x", data[u]);
    }
    printlf (" [len %x]]\n", (ul) datalen);
#endif
    ASSERT (datalen < 0x10004);

    if (pbf == START || pbf == START_NAF)
    {
        HCIACL_L2CAP_PDU_Helper pdu((uint16)datalen);
        pdu.set_header(ch, conv_pbf(pbf), conv_bf(abf));
        pdu.set_l2capData(data, (uint16)datalen);
        pdu.set_l2capChNum(cid);
        pdu.set_l2caplength(l2caplen);
        pdu.set_length();
        ok = pdd->send(pdu, false);
    }
    else
    {
        ExtendedHCIACLPDU_Helper pdu((uint16)datalen);
        pdu.set_header(ch, conv_pbf(pbf), conv_bf(abf));
        pdu.set_data(data, (uint16)datalen);
        pdu.set_length();
        ok = pdd->send(pdu, false);
    }

    if (!ok)
    {
        printlf ("rejected by DeviceController\n");
    }

    return ok;
}


int sendSco (uint16 ch, const uint8 *data, uint8 datalen)
{
    uint8 *pkt = (uint8 *) malloc (datalen);

    ASSERT (pkt != NULL);
    memcpy (pkt, data, datalen);

    return sendScoNoCopy (ch, pkt, datalen);
}

int sendScoNoCopy (uint16 ch, uint8 *pkt, uint8 datalen)
{
    HCISCOPDU pdu(datalen);
    pdu.set_data(pkt, datalen);
    pdu.set_handle(ch);

    bool ok(scoEngine->send(pdu));

    if (!ok)
    {
        printlf ("rejected by DeviceController\n");
    }

    return ok;
}

/***************************************************
 * sends bccmd warm or cold reset command with 
 * or without flow control
 ***************************************************/
bool sendWarmOrColdReset(BCCMD_PDU bccmdResetPdu, BlueCoreDeviceController::ResetType resetType)
{
    bool ok = false;

    if (!gBccmdResetFlowControlled)
    {
        ok = pdd->forceReset(1000, resetType);
    }
    else
    {
        ok = pdd->send (bccmdResetPdu);
    }

    return ok;
}

void sendBCCMD (const PA pa)
{
    bool ok;
#if 0
    size_t u;
    printlf ("[Sending bccmd:");
    for (u = 0; u < pa[0]; ++u)
    {
        printlf (" 0x%lx", (ul) pa[u]);
    }
    printlf (" [len %lu]]\n", (ul) pa[0]);
#endif
    //ok = pdd->sendChipMgtHostToChip ((uint32 *) pa); /* HERE get rid of the cast! */

    BCCMD_PDU bccmd(HCIPDUFactory::createBCCMD_PDU (pa));

    // Intercept reset commands
    switch (bccmd.get_varid())
    {
        case BCCMDVARID_COLD_RESET:
            ok = sendWarmOrColdReset(bccmd, BlueCoreDeviceController::bccmd_cold);
            break;
        case BCCMDVARID_WARM_RESET:
            ok = sendWarmOrColdReset(bccmd, BlueCoreDeviceController::bccmd_warm);
            break;
        default:
            ok = pdd->send (bccmd);
            break;
    };

    if (!ok)
    {
        printlf ("rejected by DeviceController\n");
    }
}

extern "C" void print_bccmdgetrespfields(uint16 sn, uint16 vi, uint16 s);

void sendRawBCCMD (uint16 req_type, uint16 varid, uint16 seq_no, const uint16 *payload, size_t payload_len)
{
    class RawBccmdResponse : public BlueCoreDeviceController_newStyle::PDUCallBack
    {
        virtual void operator() ( const PDU & pdu)
        {
            BCCMD_PDU bccmd(pdu);
            print_bccmdgetrespfields(bccmd.get_seq_no(), bccmd.get_varid(), bccmd.get_status());
            std::vector<uint16> buf(bccmd.get_payload_length());
            bccmd.get_payload(&buf[0], buf.size());
            for (std::vector<uint16>::const_iterator i = buf.begin();
                 i != buf.end();
                 ++i)
            {
                printlf(" 0x%04x", *i);
            }
            printlf("\n");
        }
    };

	BCCMD_PDU pdu(varid, payload_len);
	pdu.set_req_type(req_type);
	pdu.set_payload(payload, payload_len);
	pdu.set_seq_no(seq_no);
	pdu.set_length();
	bool ok = pdd->send(BlueCoreDeviceController_newStyle::sae(pdu, new RawBccmdResponse));

	if (!ok)
	{
		printlf ("rejected by DeviceController\n");
	}
}

bool sendFp(uint16 fpch, uint16 length, const uint8 *data)
{
    bool ok = true;
    HCIACLPDU pdu(length);

    pdu.set_handle(fpch);
    pdu.set_data(data, length);
    pdu.set_length();

    if (!(ok = fastpipeEngineSend(pdu)))
    {
        printlf ("rejected by DeviceController\n");
    }

    return ok;
}


size_t getSizeQueued (uint16 ch)
{
    size_t n = pdd->getSizeOfDataWaitingOnHandle (ch);

    if (n == 0)
    {
        /*
         * If there's nothing waiting, then return the
         * number of outstanding packets: all we really
         * care about in this situation is whether there
         * is any outstanding data.
         */
//        n = pdd->getNumberOfPacketsOutstandingOnHandle (ch);
    }

    return n;
}

size_t getTotalSizeQueuedInAllChannels ()
{
    size_t totalSize = 0;

    for ( uint16 i = 0 ; i < PDU::csr_hci_extensions_count ; i++ )
    {
        totalSize += getSizeQueued(i);
    }

    return totalSize;
}

size_t flushQueued (uint16 ch)
{
//    return pdd->flushHCIData (ch);

    return true;
}

size_t pendingItemsOnRegulator()
{
    return pdd->pendingItemCountOnRegulator();
}

#ifdef MGRDIAG
int ttransport;

void getSomeBCSPdiagnostics (void)
{
    if (ttransport == BCSP)
        ((Win32BCSPTransport *)pt)->printAndClearRetryLog ();
    pdd->printQueueStatus ();
}
#endif


#ifndef _WIN32
void controlC (int sig)
{
    /*
     * On some systems, including Solaris, a process
     * will not die if there is any serial data waiting
     * to be transmitted; one ends up with a 'defunct'
     * process and it is necessary to reboot the system
     * to get rid of it!
     *
     * To attempt to avoid this, termination signals
     * are captured here.  The signal causes fgets ()
     * to exit as if EOF had been read, which in turn
     * causes btcli to exit cleanly as if 'quit' had
     * been used.
     */
    printf (" ...\n");
    /*
     * Except that this doesn't work if the active thread
     * happens not to be the btcli thread, in which case
     * the problem above can still occur!
     * HERE
     */
}
#endif


} /* extern "C" */


extern "C" uint32 scoch[10]; /* HERE */

UARTConfiguration::parity get_parity ( int transport , const ichar comparity )
{
    switch ( comparity )
    {
    case 'e':
        return UARTConfiguration::even;
    case 'n':
        return UARTConfiguration::none;
    case 'o':
        return UARTConfiguration::odd;
    default:
        switch ( transport )
        {
        case BCSP:
        case H5:
#ifdef MM_BCSP_H5
    case BCSP_H5:
#endif
            return BCSP_parity;
        case H4:
        case H4DS:
            return H4_parity;
        default:
            return UARTConfiguration::none;
        }
    }
}

uint8 get_stopbits ( int transport , const ichar comstopbits )
{
    switch ( comstopbits )
    {
    case '0':
    case '1':
    case '2':
        return comstopbits - '0';
    default:
        switch ( transport )
        {
        case BCSP:
        case H5:
#ifdef MM_BCSP_H5
        case BCSP_H5:
#endif
            return BCSP_stopbits;
        case H4:
        case H4DS:
            return H4_stopbits;
        default:
            return 0;
        }
    }
}

bool get_flowcontrolon ( int transport )
{
    switch ( transport )
    {
    case BCSP:
    case H5:
#ifdef MM_BCSP_H5
case BCSP_H5:
#endif
        return BCSP_flowcontrolon;
    case H4:
    case H4DS:
        return H4_flowcontrolon;
    default:
        return false;
    }
}

uint32 get_readtimeout ( int transport )
{
    switch ( transport )
    {
    case BCSP:
#ifdef _WIN32_WCE
        return MAXDWORD ;
#endif
    case H5:
#ifdef MM_BCSP_H5
case BCSP_H5:
#endif
        return BCSP_readtimeout;
    case H4:
    case H4DS:
        return H4_readtimeout;
    default:
        return 1;
    }
}

#include "acl_header_conversion.h"

class AclCallback : public AclEngine::DefaultCallback
{
public:
    virtual void receive(const HCIACLPDU &pdu);
    virtual ~AclCallback() {};
} aclCallback;

void AclCallback::receive(const HCIACLPDU &pdu)
{
    if (showAcl || showGps)
    {
        CriticalSection::Lock lock(cs);

            uint16 datalen = pdu.get_length();
            const uint8 *data = pdu.get_dataPtr();
            
            //GPS Channel
            if ( 0x711 == pdu.get_handle() && ! showGps ) {
                // Just ignore the data as we do NOT want to display
                // it.  We return out of the function as we do NOT want
                // to carry on doing the other default stuff
                return;
            }
  	    printTimestamp ();
	    printlf ("acl ");
            //GPS Channel
            if ( 0x711 == pdu.get_handle() ) {
                int lines;
                // print it nicely because it is large
                printch (pdu.get_handle());
                printlf( ":GPS msg len %4d: ", datalen );
                if ( 0 == data[12] && 'C' == data[13] && 'S' == data[14] && 'R' == data[15] ) {
                    // Start of a sample
                    uint32 timestamp, ts80ths, flags, captured;
                    timestamp = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
                    ts80ths = data[4];
                    flags = data[5];
                    captured = data[6] | (data[7] << 8);
                    printlf( "timestamp %10u:%2u  flags 0x%02x, data %4d bytes, captured %4d bytes\n",
                             timestamp, ts80ths, flags, datalen-16, captured );
                    if ( 16 >= datalen || (7 & datalen) ) {
                        printlf( "***Nonaligned/short packet, aborting print\n" );
                        return;
                    }
                    data += 16;             // Header size is 16 bytes
                    datalen -= 16;
                }
                else {
                    printlf( "continuation packet, data %4d bytes\n", datalen );
                }
                if ( 8 > datalen || (7 & datalen) ) {
                    printlf( "***Nonaligned/short packet, aborting print\n" );
                    return;
                }
                lines = 0;
                printlf( "%5d: ", 32 * lines );
                while ( datalen ) {
                    uint32 t1;
                    t1 = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
                    data += 4;
                    datalen -= 4;
                    if ( datalen && (7 & ++lines) )
                        printlf( "%08x ", t1 );
                    else {
                        printlf( "%08x\n", t1 );
                        if ( datalen ) {
                            printlf( "%5d: ", 4 * lines );
                        }
                    }
                }
                return;
            }
            if ( ! showAcl )
                return;
            //Coaster Channel
            if ( 0x713 == pdu.get_handle() ) {
              // Then it's an indication from the LC7 GPS Coaster world...
              // See BCCMD_VARIDs coaster_start, coaster_indications et al.

#define READ16( dp ) ((dp)[0] | ((dp)[1] << 8))
#define READ32( dp ) ((READ16(dp)<<16) | READ16(dp+2))

              unsigned id = READ16(data);
              printch (pdu.get_handle());
              printlf( ":GPS Coaster indication len %3d: ", datalen );

              if ( 0xaa55 == id ) {
                printlf( "Timestamp tick\n" );
              }
              else if ( 0xbb55 == id ) {
                datalen -= 2 + 5;       // two id bytes plus 5-byte timestamp
                unsigned svs = datalen / 18;

                unsigned ts = READ32( data + 2 );
                unsigned ts80ths = data[6];

                data += 7;

                printlf( "Tracking parameters, TS %d:%d, with %d entries:\n", ts, ts80ths, svs );

                // typedef struct sventry713 {
                //     uint8  satid  ;
                //     uint8  prn  ;
                //     uint8  lockStatusUnion_lowReg16_hi8  ; /* 3 */
                //     uint8  dataTransitionTimer  ; /* 4 */
                //     uint16 SNR                  ; /* 6 */
                //     uint32 codeDoppler          ; /* 10, int32 */
                //     uint32 codeShift            ; /* 14, uint32 */
                //     uint32 carrierDoppler       ; /* 18, int32 */

                while ( svs > 0 ) {
                  uint8  satid                        = data[0];
                  uint8  prn                          = data[1];
                  uint8  lockStatusUnion_lowReg16_hi8 = data[2]; /* 3 */
                  /*uint8  dataTransitionTimer          = data[3];* 4 */
                  uint16 SNR                          = READ16( data + 4); /* 6 */
                  int32 codeDoppler                   = READ32( data + 6); /* 10, int32 */
                  uint32 codeShift                    = READ32( data + 10); /* 14, uint32 */
                  int32 carrierDoppler                = READ32( data + 14); /* 18, int32 */

                  printlf(
                    "SV: id %2d, PRN %2d: lock %02x SNR %7.2f code %8.3f dopps %7.3f %9.2f\n",
                    satid, prn, lockStatusUnion_lowReg16_hi8, ((double)SNR)/8.0,
                    (double)codeShift   / 65536.0 ,
                    (double)codeDoppler / 65536.0 ,
                    (double)carrierDoppler / 65536.0 );

                  data += 18;
                  svs--;
                }
              }
              else if ( 0xcc55 == id ) {
                datalen -= 2 + 5;       // two id bytes plus 5-byte timestamp
                unsigned svs = datalen / 36;

                unsigned ts = READ32( data + 2 );
                unsigned ts80ths = data[6];

                data += 7;

                printlf( "Databits, TS %d:%d, with %d entries:\n", ts, ts80ths, svs );

                //typedef struct databits713 {
                //    uint8  satid  ;
                //    uint8  prn  ;
                //    uint16 index;
                //    uint16 databits[8];
                //    uint16 quality[8];
                //} __attribute__((packed)) DATABITS713;

                while ( svs ) {
                  int i;
                  uint8  satid      = data[0];
                  uint8  prn        = data[1];
                  uint16 index      = READ16( data + 2 ); /* 4 */
                  uint16 *databits  = (uint16 *)(data +  4); /* size 16 bytes => 20 */
                  uint16 *quality   = (uint16 *)(data + 20); /* size 16 bytes => 36 */

                  printlf( "SV: id %2d, PRN %2d: bit %3d, data ",
                           satid, prn, index );

                  for ( i = 0; i < 8; i++ )
                    printlf( "%04x ", databits[i] );
                  printlf( "  Qual " );
                  for ( i = 0; i < 8; i++ )
                    printlf( "%04x ", quality[i] );
                  printlf("\n");

                  svs--;
                  data += 36;
                }
              }
              else {
                printlf( "type %04x\n", id );
              }
#undef READ16
#undef READ32
              return;
            }
	    {
		    printch (pdu.get_handle());
		    printlf (" ");
	    }

	    printByValue (pbf_d, conv(pdu.get_pbf()), 2, "pbf");
	    printlf (" ");
	    printByValue (bf_d, conv(pdu.get_bct()), 2, "bf");

            if ((pdu.get_pbf() == HCIACLPDU::start ||
                 pdu.get_pbf() == HCIACLPDU::start_not_auto_flush) &&
                datalen >= 4)
	    {
		    unsigned l2caplen, cid;
		    l2caplen = data[0] | (data[1] << 8);
		    cid = data[2] | (data[3] << 8);
		    printlf (" len:0x%04x ", l2caplen);
		    printByValue (cid_d, cid, 16, "cid");
		    data += 4;
		    datalen -= 4;
	    }
	    printlf (" \"");
	    while (datalen--)
	    {
		    printChar (*data++);
	    }
	    printlf ("\"\n");
    }
}

class ScoCallback : public ScoEngine::DefaultCallback
{
    virtual void receive(ScoEngine::DefaultCallback::PduStatus status,
                         HCISCOPDU pdu)
    {
        switch (status)
        {
        case LENGTH_DISCREPANT:
            {
                // Length field does not match the length of the pdu.
                // Print everything
                const uint8 *data = pdu.data();
                size_t length = pdu.size();

                printTimestamp ();
                printlf ("sco");
                for (size_t i = 0; i < length; ++i)
                {
                    printlf (" 0x%02x", data[i]);
                }
                printlf ("\n");
            }
            break;
        default:
            if (showSco)
            {
                const uint8 *data = pdu.get_dataPtr();
                uint8 length = pdu.get_length();

                printTimestamp ();
                printlf ("sco ");
                printch (pdu.get_handle());
                for (uint8 i = 0; i < length; ++i)
                {
                    printlf (" 0x%02x", data[i]);
                }
                printlf ("\n");

            }
            break;
        }
    }
} scoCallback;

static istring comport_ammended;

extern "C"
{
bool reconfigured_baudrate = false;
uint32 baudrate;
};

bool start (int transport, const ichar *comport, const ichar *comrate, const ichar comparity, const ichar comstopbits, bool autoProtocolDetect )
{
    comport_ammended = comport;
    long comrate_num = reconfigured_baudrate ? baudrate : iatol(comrate);

    // Prefix comport above com9 with \\.\ as windows can't cope with unprefixed comports above com9.

    if (istrncmp(comport, II("com"), 3) == 0)
    {
        if (istrlen(comport) > 4)
        {
            if (iisdigit(comport[3]) && iisdigit(comport[4]))
            {
                comport_ammended = II("\\\\.\\") + comport_ammended;
            }
        }
    }

    UARTConfiguration conf( comport_ammended.c_str(),
                comrate_num,
                get_parity(transport,comparity),
                get_stopbits(transport,comstopbits),
                get_flowcontrolon(transport),
                get_readtimeout(transport)
              );
    switch ( transport )
    {
    case BCSP:
        {
            BCSPConfiguration bcsp( conf,
                                    bcspUseCRCs != 0,
                                    csr_extensions != 0,
                                    tunnel ? PDU::l2cap : PDU::bcsp_channel_count,
                                    tunnel ? PDU::dm : PDU::zero );
            if ( bcsp.getResendTimeout() != bcspResendTimeout
              || bcsp.getRetryLimit() != bcspRetryLimit
              || bcsp.getWindowSize() != bcspWindowSize
              || bcsp.getUseLinkEstablishmentProtocol() != bcspUseLE )
                bcsp.setConfig(bcspResendTimeout,
                               bcspRetryLimit,
                               bcsp.getTShy(),
                               bcsp.getTConf(),
                               bcsp.getConfLimit(),
                               bcspWindowSize,
                               bcspUseLE);
            pdd = new BlueCoreDeviceController_newStyle(bcsp, decodeCallBack);
        }
        break;

#ifdef MM_BCSP_H5
    case BCSP_H5:
        pdd = new BlueCoreDeviceController_newStyle (
            BCSPH5Configuration (
                comport , comrate_num ,
                useCRCs != 0 , csr_extensions != 0 ), decodeCallBack );
        break;
#endif /* MM_BCSP_H5 */

#ifdef linux
#ifndef NO_BLUEZ
    case BLUEZ:
    {
        BlueZConfiguration x(comport, autoup);
    pdd = new BlueCoreDeviceController_newStyle(x, decodeCallBack);
    }
        break;
#endif /* NO_BLUEZ */
#endif /* linux */

    case H5:
        // With H5 we must tunnel all BlueCore channels
    {
        H5Configuration x(conf, bcspUseCRCs != 0);
        pdd = new BlueCoreDeviceController_newStyle(x, decodeCallBack);
    }
        break;

    case H4:
        pdd = new BlueCoreDeviceController_newStyle (
            H4Configuration ( conf, csr_extensions != 0 ), decodeCallBack );
        break;

#ifndef _WIN32_WCE
    case H4DS:
        pdd = new BlueCoreDeviceController_newStyle(
            H4DSConfiguration( conf,
                               csr_extensions != 0,
                               h4ds_wakeup_len), decodeCallBack);
        break;

#ifdef _WIN32
/* Neither of these are supported under Linux yet */
    case USB:
        pdd = new BlueCoreDeviceController_newStyle (
        H2Configuration ( USBConfiguration ( comport ),
                  csr_extensions != 0 ), decodeCallBack );
        break;
    case SOCK:
        pdd = new BlueCoreDeviceController_newStyle (
        SOCKConfiguration ( comport , (uint16)comrate_num ,
                SOCKConfiguration::toChip ), decodeCallBack );
        break;
#endif /*WIN32*/
#endif /*!_WIN32_WICE*/
#ifndef _WIN32_WCE
    case SDIO:
        pdd = new BlueCoreDeviceController_newStyle (
        SDIOConfiguration(SDIOPConfiguration(comport)), decodeCallBack );
        break;
#endif /* !_WIN32_WCE */
    default:
        return false;
        break;
    }

    aclEngine = new AclEngine(pdd, &aclCallback);
    scoEngine = new ScoEngine(pdd, &scoCallback);
    btcli_enter_critical_section();
    fastpipeEngineInit();
    btcli_leave_critical_section();

    setDataWatch();

    if ( !pdd->startTransport() )
    {
        delete pdd;
        return false;
    }
    else
    {
        // for H4 and auto protocol detect we need to send this sequence
        // so the chip/firmware receives some data to determine the protocol.
        // without this no data are send to the chip/firmware at connection hence
        // auto detection is not possible. It also needs an 'r' (RESET_HCI) after.
        if (transport == H4 && autoProtocolDetect)
        {
            sendH4dp();
            sleep_ms(50);
            sendH4dp();
        }
        return true;
    }
}

ichar *logname = NULL;
char *sconame[SCO_COUNT] = {NULL,NULL,NULL};

#ifdef LATENCY_TRACKING
EventTracker& eventTracker = EventTracker::getInstance();
#endif

extern "C" int imain (int argc, ichar *argv[])
{
#define PARITYSTUFF II(" [-pe|-po|-pn]")
#define STOPBITSSTUFF II(" [-B0|-B1|-B2]")
#ifdef _WIN32
    ichar comstring[256] = II("parity=e data=8 stop=1 dtr=on rts=on baud=");
#else
#endif

    const ichar *progname = argv[0], *transportname;
    const ichar *comrate = II("38400"), *comsettings = NULL, *comport = NULL;
    const ichar *execname = NULL;
    const ichar *fsmdir = NULL, *sympath = NULL;
    ichar comparity = NUL;
    ichar comstopbits = NUL;
    int transport = NOTRANSPORT;
    bool autoProtocolDetect = false;
    bool quiet = false;
    int oargc = argc;
    ichar **oargv = argv;

#ifdef MMWIN32ALLOCDEBUG
    _crtDbgFlag = _CRTDBG_ALLOC_MEM_DF|_CRTDBG_DELAY_FREE_MEM_DF|_CRTDBG_CHECK_ALWAYS_DF|_CRTDBG_CHECK_CRT_DF|_CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_DELAY_FREE_MEM_DF|_CRTDBG_CHECK_ALWAYS_DF|_CRTDBG_CHECK_CRT_DF|_CRTDBG_LEAK_CHECK_DF);
    _CrtCheckMemory();
#endif

#ifndef _WIN32
    signal (SIGINT, controlC);
    signal (SIGQUIT, controlC);
    signal (SIGABRT, controlC);
    signal (SIGTERM, controlC);
#endif
    print_init();

    while (--argc)
    {
        ++argv;
#ifdef _WIN32
        if (istrcmp (*argv, II("/?")) == 0)
        {
            goto badsyntax;
        }
#endif
        if (**argv == '-')
        {
            switch ((*argv)[1])
            {
            case 'e':
                showExpansions = false;
                break;
            case 't':
                printTimestamps (1);
                break;
            case 'w':
                autoWlps = true;
                break;
            case 'X':
                quiet = true;
                break;
            case 'T':
                tunnel = true;
                break;
            case 'C':
                csr_extensions = false;
                break;
            case 'B':
                comstopbits = (*argv)[2];
                break;
            case 'c':
                bcspUseCRCs = false;
                break;
            case '1':
                bcspWindowSize = 1;
                break;
            case '2':
                bcspWindowSize = 2;
                break;
            case '3':
                bcspWindowSize = 3;
                break;
            case '4':
                bcspWindowSize = 4;
                break;
            case '5':
                bcspWindowSize = 5;
                break;
            case '6':
                bcspWindowSize = 6;
                break;
            case '7':
                bcspWindowSize = 7;
                break;
            case 'r':
                if (  (*argv)[2] == 't' )
                {
                    bcspResendTimeout = iatoi((*argv)+3);
                    //  don't let it be zero!
                    if ( bcspResendTimeout == 0 )
                        goto badsyntax;
                }
                else if ( (*argv)[2] == 'l')
                {
                    bcspRetryLimit = iatoi((*argv)+3);
                }
                else
                    goto badsyntax;
                break;
            case 'D':
                if (  (*argv)[2] == 'P' )
                {
                    autoProtocolDetect = true;
                }
                else
                    goto badsyntax;
                break;
            case 'x':
                execname = &(*argv)[2];
                break;
            case 'l':
                logname = &(*argv)[2];
                break;
            case 'A':
                showAscii = true;
                break;
            case 'S':
                sympath = &(*argv)[2];
                break;
            case 'F':
                fsmdir = &(*argv)[2];
                break;
            case 'H':
                if ((*argv)[2] == 'W' && iisdigit((*argv)[3]))
                {
                    h4ds_wakeup_len = iatol(&(*argv)[3]);
                }
                break;
            case 'p':
                if (  !(((*argv)[2] == 'e' ||
                     (*argv)[2] == 'o' ||
                     (*argv)[2] == 'n') &&
                    (*argv)[3] == NUL))
                {
                    goto badsyntax;
                }
                comparity = (*argv)[2];
                break;
            case 'a':
                showAcl = false;
                break;
            case 'g':
                showGps = true;
                break;
            case 's':
                switch ((*argv)[2])
                {
                case NUL:
                    showSco = false;
                    break;

                case '0':
                    sconame[0] = iinarrowdup(&(*argv)[3]);
                    break;

                case '1':
                    sconame[1] = iinarrowdup(&(*argv)[3]);
                    break;

                case '2':
                    sconame[2] = iinarrowdup(&(*argv)[3]);
                    break;

                default:
                    sconame[0] = iinarrowdup(&(*argv)[2]);
                    break;
                }
                break;
#ifdef linux
            case 'U':
                autoup = true;
                break;
#endif
            case 'm':
                submode = *argv + 2;
                break;
            default:
            badsyntax:
                ifprintf (stderr,
                    II("usage: %s [<transport>] [<port>] [<bps>]") PARITYSTUFF STOPBITSSTUFF II(" [-x<file>] [-l[<file>]] [-s[<n>]<file>] [-s] [-a] [-g] [-t] [-w] [-c] [-A] [-DP] [-1|-2|-3|-4|-5|-6|-7] [-rt<timeout>] [-rl<n>] [-S<file>] [-F<path>] [-T] [-m<submode>] [-HW<waketime>]")
#ifdef linux
                    II(" [-U]")
#endif
                    II("\n"), progname);
                exit (1);
            }
        } /* if (**argv == '-')*/
        else if (istricmp (*argv, II("h4")) == 0)
        {
            transportname = *argv;
            transport = H4;
        }
        else if (istricmp (*argv, II("h4ds")) == 0)
        {
            transportname = *argv;
            transport = H4DS;
        }
        else if (istricmp (*argv, II("h5")) == 0)
        {
                transportname = *argv;
                transport = H5;
        }
        else if (istricmp (*argv, II("bcsp")) == 0)
        {
            transportname = *argv;
            transport = BCSP;
        }
#ifdef MM_BCSP_H5
        else if (istricmp (*argv, II("bcsph5")) == 0)
        {
            transportname = *argv;
            transport = BCSP_H5;
        }
#endif /* MM_BCSP_H5 */
        else if (istricmp (*argv, II("usb")) == 0)
        {
            transportname = *argv;
            transport = USB;
        }
#ifndef NO_BLUEZ
        else if (istricmp (*argv, II("bluez")) == 0)
        {
            transportname = *argv;
            transport = BLUEZ;
        }

#endif//NO_BLUEZ
        else if (istricmp (*argv, II("SOCK")) == 0)
        {
            transportname = *argv;
            transport = SOCK;
        }
        else if (istricmp (*argv, II("sdio")) == 0)
        {
#ifndef _WIN32_WCE
            transportname = *argv;
            transport = SDIO;
#else
            printlf("SDIO currently not supported\n");
            exit(1);
#endif //_WIN32_WCE
        }
        else if (iisdigit (**argv))
        {
            if (istrchr(*argv, ':'))
                comport = *argv;
            else
                comrate = *argv;
        }
        else
        {
            comport = *argv;
        }
    }

    if (logname != NULL)
    {
        if (*logname == NUL)
        {
            logname = II("btcli.log");
        }
        if (!printlOpen (logname))
        {
            ifprintf (stderr, II("Couldn't open %s\n"), logname);
            exit (1);
        }
        while (oargc--)
        {
            iprintlfOnly (*oargv++);
            if (oargc != 0)
            {
                printlfOnly (" ");
            }
        }
        printlfOnly ("\n");
    }

    if (!quiet)
    {
        iprintlf (II("btcli, version %s\n"), II(VERSION_BLUESUITE_STR));

        // If the special build string is set then show the date of the build
        if (strcmp (VERSION_SPECIAL_BUILD, "") != 0)
        {
	        iprintlf (II("%s\n"), II(VERSION_SPECIAL_BUILD));
		}
	    const istring copyright = icoerce(VERSION_COPYRIGHT_START_YEAR("2001"));
	    iprintlf (II("%s\n"), copyright.c_str());

    }

    loadFSMdata (fsmdir);
    (void) loadSymbolData (sympath, NULL);

	for( int i = 0 ; i < SCO_COUNT ; ++i )
	{
		if (sconame[i] != NULL)
		{
            if (!enableScoLogging(i, sconame[i]))
            {
                fprintf (stderr, "Couldn't open %s\n", sconame[i]);
				exit (1);
            }
		}
	}

#ifdef _WIN32
    ASSERT (istrlen (comrate) < 10);
    istrcat (comstring, comrate);
    if (comparity != NUL)
    {
        comstring[7] = comparity;
    }
    comsettings = comstring;
#else
    comsettings = comrate;
#endif
    if (transport == NOTRANSPORT)
    {
        transportname = II("BCSP");
        transport = BCSP;
    }
    if (transport == USB)
    {
#if !defined(_WIN32)
        ifprintf (stderr, II("USB currently not supported\n"));
        exit (1);
#else
        comrate = II("12000000");
        if (comport == NULL)
        {
            comport = USB_PORT_DEF;
        }
#endif
    }
#ifndef NO_BLUEZ
    if (transport == BLUEZ)
    {
#if defined(linux)
        comrate = II("<unknown>");
        if (comport == NULL)
        {
            comport = BLUEZ_PORT_DEF;
        }
#else /* linux */
        ifprintf (stderr, II("BlueZ currently not supported\n"));
        exit (1);
#endif /* linux */
    }
#endif//NO_BLUEZ
    if (transport != BCSP && transport != H5 && transport != H4DS && 
        transport != H4 && comparity != NUL)
    {
        if (!quiet)
        {
            ifprintf (stderr, II("Parity is only selectable for UART transports\n"));
        }
        else
        {
            ifprintf (stderr, II("Parity is not selectable for this transport\n"));
        }
        exit (1);
    }

    do 
    {
    #ifdef MGRDIAG
        ttransport = transport;
    #endif
    
    #ifdef LATENCY_TRACKING
        eventTracker.setLabel(comport);
    #endif
    
        if (comport != NULL)
        {
            if (start (transport, comport, comrate, comparity, comstopbits, autoProtocolDetect))
            {
                if (!quiet)
                {
                    if (reconfigured_baudrate)
                    {
                        iprintlf (II("%s on %s at %d bps\n"), transportname, comport, baudrate);
                    }
                    else
                    {
                        iprintlf (II("%s on %s at %s bps\n"), transportname, comport, comrate);
                    }
                }
            }
            else
            {
                if (!quiet)
                {
                    if (reconfigured_baudrate)
                    {
                        ifprintf (stderr, II("Couldn't open %s at %d bps (%s)\n"), comport, baudrate, transportname);
                    }
                    else
                    {
                        ifprintf (stderr, II("Couldn't open %s at %s bps (%s)\n"), comport, comrate, transportname);
                    }
                }
                else
                {
                    if (reconfigured_baudrate)
                    {
                        ifprintf (stderr, II("Couldn't open %s at %d bps\n"), comport, baudrate);
                    }
                    else
                    {
                        ifprintf (stderr, II("Couldn't open %s at %s bps\n"), comport, comrate);
                    }
                }
                exit (1);
            }
        }
        else
        {
            int i;
            ichar comport[COM_PORT_LEN];
    
            for (i = 0; i < NUM_COM_PORTS; i++)
            {
                isprintf (comport, COM_PORT_LEN, COM_PORT_GEN);
    
                if (start (transport, comport, comrate, comparity, comstopbits, autoProtocolDetect))
                {
                    if (!quiet)
                    {
                        if (reconfigured_baudrate)
                        {
                            iprintlf (II("%s on %s at %d bps\n"), transportname, comport, baudrate);
                        }
                        else
                        {
                            iprintlf (II("%s on %s at %s bps\n"), transportname, comport, comrate);
                        }
                    }
                    break;
                }
            }
    
            if (i >= NUM_COM_PORTS)
            {
                if (!quiet)
                {
                    if (reconfigured_baudrate)
                    {
                        ifprintf (stderr, II("Couldn't open any ports at %d bps (%s)\n"), baudrate, transportname);
                    }
                    else
                    {
                        ifprintf (stderr, II("Couldn't open any ports at %s bps (%s)\n"), comrate, transportname);
                    }
                }
                else
                {
                    if (reconfigured_baudrate)
                    {
                        ifprintf (stderr, II("Couldn't open any ports at %d bps\n"), baudrate);
                    }
                    else
                    {
                        ifprintf (stderr, II("Couldn't open any ports at %s bps\n"), comrate);
                    }
                }
                exit (1);
            }
        }
    
    #ifdef MGRWIN32ALLOCDEBUG
        _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
        _CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
        _CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
        _CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
        _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
        _CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );
    #endif
        {
            CriticalSection::Lock lock(cs);
            parseInit ();
        }
        pdd->createWorker ();

        if (transport == H4 && autoProtocolDetect)
        {
            // send 'r' command: reset, HCI_RESET
            pdd->forceReset(1000, BlueCoreDeviceController::hci);
        }

        /* We are about to run the command parser.
         * Check if we need to feed in a script first, either because we are
         * restarting - or a start up script
         */
        if (restart_after_commands == RESTART_NEW_SCRIPT)
        {
            /* Reset restart before running script as may be set by the 
             * script being called.
             */
            restart_after_commands = RESTART_NONE;

            /* Give transport a chance to start-up */ 
            if (pdd->isTransportReady(RESTART_TRANSPORT_RESTART_TIMEOUT))
            {
                /* and get the NOP back */
                sleep_ms(RESTART_COMMS_ESTABLISH_TIMEOUT);
            }
            
            exec_do (restart_filename);
        }
        else 
        {
            /* Reset as we may have restarted & don't want to restart yet again */
            restart_after_commands = RESTART_NONE;

            if (execname)
            {
                char *execname8 = (char *) malloc (istrlen (execname) + 1), *ptr = execname8;
                const ichar *iptr = execname;
        
                ASSERT (execname8 != NULL);
                while ((*ptr++ = (char)(*iptr++)) != 0)
                    ;
                exec_do (execname8);
                free (execname8);
                execname = NULL;    /* We don't care if we loop later */
            }
        }
    
    #if defined(linux)
    // This is needed by the test scripts on linux. Other wise the 
        // version/connection info does not display properly, and the script
        // has no way of knowing if btcli has started or not.
        fflush(stdout);
    #endif

        /* A script may have asked for another restart, in which case entering 
         * the parser doesn't work
         */
        if (restart_after_commands == RESTART_NONE)
        {
            btcli ();
        }
    
        delete pdd;
        
    } while (restart_after_commands != RESTART_NONE);
    
#ifdef MGRWIN32ALLOCDEBUG
    Sleep (2000); /* To allow other threads to die cleanly */
    _CrtDumpMemoryLeaks ();
#endif

    iprintlf (II("Adieu!\n"));
    printlClose ();
    return 0;
}

#ifdef RAW_QUEUE
#include <list>
#endif


#ifdef LATENCY_TRACKING
extern "C" void showEventTracking(void) {
    eventTracker.print();
}
#endif

#include "log_file_handler.h"
#include "time/time_stamp.h"

#include <vector>
#include <list>

class btcliDataWatcher : public DataWatcher
{
    class RawChunk
    {
        const char mType;
        TimeStamp mTimeStamp;
        const std::vector<uint8> mBuf;

    public:
        RawChunk(const char type, const uint8 *d, const uint32 length) :
            mType(type),
            mBuf(d, d + length)
        {
        }

        RawChunk operator=(const RawChunk &rc)
        {
            return RawChunk(rc);
        }

        char getType() const
        {
            return mType;
        }

        std::string getTimeStamp() const
        {
            return mTimeStamp.hms();
        }

        const u8 *getBuf() const
        {
            return &mBuf[0];
        }

        uint32 getLength() const
        {
            return mBuf.size();
        }
    };

    #define NRAWCHUNKS 1024
    typedef std::deque<RawChunk> RawChunks;
    RawChunks rawChunks;
    CriticalSection protect_rawChunks;

#ifdef RAW_QUEUE
    class el
    {
    public:
        el ( const uint8*data , uint32 size ) : d ( data , size )
        {
			getTimestamp ( timestamp );
		}
        PDU d;
        char timestamp[16];
    };
    std::list<el> outbound;
    std::list<el> inbound;
#endif
    void printHCI (const char type, const uint8 *d, const uint32 l)
    {
        CriticalSection::Lock lock(cs);
        printTimestamp ();
        printlf ("hci_%cx ", type);
        printQuintets (d, l);
        printlf ("\n");
    }

    void storeHCI (const char type, const uint8 *d, const uint32 length)
    {
        CriticalSection::Lock lock(protect_rawChunks);
        rawChunks.push_back(RawChunk(type, d, length));

        if (rawChunks.size() > NRAWCHUNKS)
        {
            rawChunks.pop_front();
        }
    }

    RawChunks move_rawChunks()
    {
        CriticalSection::Lock lock(protect_rawChunks);
        RawChunks rawChunksCopy(rawChunks);
        rawChunks.clear();
        return rawChunksCopy;
    }

public:
#define TX 't'
#define RX 'r'

    void onData (const char type, const uint8 *d, const uint32 length)
    {
        rawlog_log(type, d, length);
        if (showRaw)
        {
            printHCI (type, d, length);
        }
        else
        {
            storeHCI (type, d, length);
        }
    }

    void onSend ( const uint8 * d , size_t length )
    {
        onData(TX, d, length);        
#ifdef RAW_QUEUE
        outbound.push_back ( el ( d , length ) );
#endif
    }
    
    void onRecv ( const uint8 * d , size_t length )
    {
        onData(RX, d, length);
#ifdef RAW_QUEUE
        inbound.push_back ( el ( d , length ) );
#endif
    }

    void showWatchedData(void)
    {
        const RawChunks &rawChunksCopy = move_rawChunks();
        CriticalSection::Lock lock(cs);

        for (RawChunks::const_iterator i = rawChunksCopy.begin();
             i != rawChunksCopy.end();
             ++i)
        {
            printTag();
            printlf("%s hci_%cx ", i->getTimeStamp().c_str(), i->getType());
            printQuintets(i->getBuf(), i->getLength());
            printlf("\n");
        }
    }
} hwDataView;

extern "C" void showWatchedData (void)
{
	hwDataView.showWatchedData();
}

extern "C" void setDataWatch (void)
{
    pdd->setDataWatch (&hwDataView);
}

extern "C" void unsetDataWatch (void)
{
    pdd->setDataWatch (0);
}

#include <map>
#include "aclengine/file_transfer.h"
typedef std::map<uint16, FileTransfer> FileTransfers;

FileTransfers fileTransfers;

class FileTransferAlertCallback : public FileTransfer::AlertCallback
{
public:
    void fileTransferCallback(FileTransfer::AlertType alertType,
                              FileTransfer::SendSnapshot sendSnapShot,
                              FileTransfer::ReceiveSnapshot receiveSnapShot);
} fileAlert;

void FileTransferAlertCallback::fileTransferCallback(
                                 FileTransfer::AlertType alertType,
                                 FileTransfer::SendSnapshot sendSnapShot,
                                 FileTransfer::ReceiveSnapshot receiveSnapShot)
{
    CriticalSection::Lock lock(cs);
    switch(alertType)
    {
    case FileTransfer::ALERT_RECEIVE_START:
        break;

    case FileTransfer::ALERT_RECEIVE_FINISH:
        printTimestamp();
        printlf ("recv: %lu octets received in %lu.%03lu s (%lu bps)",
                 (ul) receiveSnapShot.octetsReceived,
                 (ul) receiveSnapShot.timeElapsed / 1000,
                 (ul) receiveSnapShot.timeElapsed % 1000,
                 (ul) (receiveSnapShot.timeElapsed == 0 ? 0 :
                         receiveSnapShot.octetsReceived * 8000.0 /
                           receiveSnapShot.timeElapsed));
        if (receiveSnapShot.octetsReceived < receiveSnapShot.fileSize)
        {
            printlf(" with %lu octets missing", (ul) receiveSnapShot.fileSize -
                                                receiveSnapShot.octetsReceived);
        }
        else if (receiveSnapShot.octetsReceived > receiveSnapShot.fileSize)
        {
            printlf(" with %lu octets extra", (ul) receiveSnapShot.octetsReceived -
                                              receiveSnapShot.fileSize);
        }

        if (0 != receiveSnapShot.l2CapPacketsTruncated)
        {
            printlf(" with %lu L2CAP PDUs truncated losing %lu octets",
                    (ul) receiveSnapShot.l2CapPacketsTruncated,
                    (ul) receiveSnapShot.l2CapPacketTruncationOctetsLost);
        }

        printlf(" on ");
        printch(receiveSnapShot.ch);
        printlf("\n");
        break;

    case FileTransfer::ALERT_RECEIVE_DATA:
        break;

    case FileTransfer::ALERT_RECEIVE_DATA_START:
        printTimestamp();
        printlf("recv: receiving %lu octets on ", (ul) receiveSnapShot.fileSize);
        printch(receiveSnapShot.ch);
        printlf("\n");
        break;

    case FileTransfer::ALERT_RECEIVE_FILE_ERROR:
        printTimestamp();
        printlf("recv: \"%s\" on ", strerror(errno));
        printch(receiveSnapShot.ch);
        printlf("\n");
        break;

    case FileTransfer::ALERT_RECEIVE_REMOTE_CANCEL:
        printTimestamp();
        if (0 == receiveSnapShot.fileSize)
        {
	    printlf ("recv: aborted remotely on ");
        }
        else
        {
            printlf (
                "recv: aborted remotely after %lu of %lu octets received on ",
                (ul) receiveSnapShot.octetsReceived,
                (ul) receiveSnapShot.fileSize);
        }
        printch(receiveSnapShot.ch);
        printlf("\n");
        break;

    case FileTransfer::ALERT_RECEIVE_UNEXPECTED_PACKET_RECEIVED:
        break;

    case FileTransfer::ALERT_SEND_START:
        printTimestamp();
        printlf("send: queueing %lu octets in %lu-octet L2CAP PDUs on ",
                (ul) sendSnapShot.fileSize,
                (ul) sendSnapShot.l2CapSize);
        printch(sendSnapShot.ch);
        printlf("\n");
        break;

    case FileTransfer::ALERT_SEND_REMOTE_CANCEL:
        printTimestamp();
        printlf("send: aborted remotely after %lu of %lu octets sent on ",
                (ul) sendSnapShot.octetsSent,
                (ul) sendSnapShot.fileSize);
        printch(sendSnapShot.ch);
        printlf("\n");
        break;

    case FileTransfer::ALERT_SEND_FILE_ERROR:
        printTimestamp();
        printlf("send: \"%s\" on ", strerror(errno));
        printch(sendSnapShot.ch);
        printlf("\n");
        break;

    case FileTransfer::ALERT_SEND_QUEUING_DATA:
        break;

    case FileTransfer::ALERT_SEND_QUEUED_ALL_DATA:
        printTimestamp();
        printlf ("send: %lu octets queued on ", (ul) sendSnapShot.octetsQueued);
	printch (sendSnapShot.ch);
	printlf ("\n");
        break;

    case FileTransfer::ALERT_SEND_FINISHED:
        printTimestamp();
        printlf ("send: send complete on ");
        printch (sendSnapShot.ch);
        printlf ("\n");
        break;

    case FileTransfer::ALERT_SEND_UNEXPECTED_PACKET_RECEIVED:
        break;

    default:
        break;
    }
}

class FileTransferL2CapSizeCallback : public FileTransfer::L2CapSizeCallback
{
public:
    virtual uint16 getL2CapSize()
    {
        uint16 n = 0xffff;

        if (findVar ("SENDL2CAPSIZE"))
        {
            n = (uint16)strtol (findVar ("SENDL2CAPSIZE"), NULL, 0);
        }

        return n;
    }
} l2CapSize;

extern "C" bool isAclChannelOpen(uint16 ch)
{
    return aclEngine->isAclChannelOpen(ch);
}

extern "C" bool openAclChannel (uint32 chNum)
{
    if (aclEngine->connect(chNum))
    {
        fileTransfers.insert(
            FileTransfers::value_type((uint16)chNum, FileTransfer((uint16)chNum, 
                                                          aclEngine,
                                                          &fileAlert,
                                                          &l2CapSize)));
        return true;
    }
    else
    {
        return false;
    }
}

extern "C" bool closeAclChannel (uint32 chNum)
{
    fileTransfers.find((uint16)chNum)->second.disconnect();
    fileTransfers.erase((uint16)chNum);
    return aclEngine->disconnect((uint16)chNum);
}

extern "C" bool sendFile(uint16 ch, const char *fileName, bool flushable)
{
    FileTransfers::iterator it(fileTransfers.find(ch));
    return it != fileTransfers.end() &&
           it->second.startSend(fileName, flushable);
}

extern "C" bool recvFile(uint16 ch, const char *fileName, bool flushable)
{
    FileTransfers::iterator it(fileTransfers.find(ch));
    return it != fileTransfers.end() &&
           it->second.startReceive(fileName, flushable);
}

extern "C" bool isSendFileActive(uint16 ch)
{
    FileTransfers::iterator it(fileTransfers.find(ch));
    return it != fileTransfers.end() &&
           it->second.getSendSnapshot().sendActive;
}

extern "C" bool isRecvFileActive(uint16 ch)
{
    FileTransfers::iterator it(fileTransfers.find(ch));
    return it != fileTransfers.end() &&
           it->second.getReceiveSnapshot().receiveActive;
}

extern "C" bool cancelFileSend(uint16 ch, uint32 *fileSize, uint32 *bytesSent)
{
    assert(fileSize);
    assert(bytesSent);
    FileTransfers::iterator it(fileTransfers.find(ch));
    return it != fileTransfers.end() &&
           it->second.cancelSend(*fileSize, *bytesSent);
}

extern "C" bool cancelFileRecv(uint16 ch, uint32 *fileSize, uint32 *bytesReceived)
{
    FileTransfers::iterator it(fileTransfers.find(ch));
    return it != fileTransfers.end() &&
           it->second.cancelReceive(fileSize, bytesReceived) && 0xff;
}

extern "C" void aclEngineReInit()
{
    // It is best to not have a lock while dealing with AclEngine.
    CriticalSection::AntiLock antilock(cs);
    fileTransfers.clear();
    delete aclEngine;
    aclEngine = new AclEngine(pdd, &aclCallback);

    int j;
    for (j = 0; j < 18; ++j)
	{
		aclch[j] = NOHANDLE;
	}
	for (j = 0; j < 10; ++j)
	{
		scoch[j] = NOHANDLE;
	}
	lastaclch = lastscoch = NOHANDLE;
	abch = 0xbac;
	pbch = 0xbec;
}

extern "C" void aclFstat(uint32 ch)
{
    FileTransfer::SendSnapshot sendSnapshot(
        fileTransfers.find((uint16)ch)->second.getSendSnapshot());

    if (sendSnapshot.sendActive)
    {
        CriticalSection::Lock lock(cs);
        printTimestamp();
    	printlf("send: approximately %lu of %lu octets sent on ",
                (ul) sendSnapshot.octetsSent, (ul) sendSnapshot.fileSize);
	printch(ch);
	printlf("\n");
    }

    FileTransfer::ReceiveSnapshot receiveSnapshot(
        fileTransfers.find((uint16)ch)->second.getReceiveSnapshot());

    if (receiveSnapshot.receiveActive)
    {
        CriticalSection::Lock lock(cs);
        printTimestamp();
        if (0 == receiveSnapshot.octetsReceived)
        {
            printlf("recv: not started ");
        }
        else
        {
    	    printlf (
                "recv: %lu of %lu octets received in %lu.%03lu s (%lu bps) ",
                (ul) receiveSnapshot.octetsReceived,
                (ul) receiveSnapshot.fileSize,
                (ul) receiveSnapshot.timeElapsed / 1000,
                (ul) receiveSnapshot.timeElapsed % 1000,
                (ul (receiveSnapshot.timeElapsed == 0 ? 0 :
                       receiveSnapshot.octetsReceived * 8000.0 /
                         receiveSnapshot.timeElapsed)));
        }

        if (0 != receiveSnapshot.l2CapPacketsTruncated)
        {
            printlf ("with %lu L2CAP PDUs truncated losing %lu octets ",
                     (ul) receiveSnapshot.l2CapPacketsTruncated,
                     (ul) receiveSnapshot.l2CapPacketTruncationOctetsLost);
        }

        printlf("on ");
        printch(ch);
        printlf("\n");
    }
}

extern "C" void aclFstatAll()
{
    for (FileTransfers::const_iterator i(fileTransfers.begin());
         i != fileTransfers.end();
         ++i)
    {
        aclFstat(i->first);
    }
}

#include "scoengine/scologger.h"
ScoLogger scoLoggers[SCO_COUNT];
#include "scoengine/scofilesend.h"
std::map<uint16, ScoFileSend> scoFileSenders;

class ScoFileSendAlert : public ScoFileSend::AlertCallback
{
public:
    virtual void callback(ScoFileSend::AlertType alertType,
                          ScoFileSend::Snapshot snapshot);
} scoFileSendAlert;

void ScoFileSendAlert::callback(ScoFileSend::AlertType alertType,
                                ScoFileSend::Snapshot snapshot)
{
    CriticalSection::Lock lock(cs);

    switch (alertType)
    {
    case ScoFileSend::NO_HIGH_PERFORMANCE_CLOCK:
        printTimestamp();
		printlf ("sends: system does not support high-performance clock\n");
        break;
    case ScoFileSend::CAN_NOT_OPEN_FILE:
        printTimestamp();
        printlf ("sends: %s!\n", strerror (errno));
        break;
    case ScoFileSend::TIME_GET_DEV_CAPS_FAILED:
        printTimestamp ();
        printlf ("sends: timeGetDevCaps failed!\n");
        break;
    case ScoFileSend::TIME_GET_DEV_CAPS:
        printTimestamp();
        printlf ("sends: timeGetDevCaps reports that minimum timer "
                 "resolution is %lu ms!\n", (ul) snapshot.periodMin);
        break;
    case ScoFileSend::TIME_BEGIN_PERIOD_FAILED:
        printTimestamp();
        printlf("sends: timeBeginPeriod (1) failed!\n");
        break;
    case ScoFileSend::STARTING_SEND:
        printTimestamp ();
	    printlf ("sends: sending %lu octets every %lu us (%lu bps) with %i"
                 " samples (%li ms) prequeued on ",
                (ul) snapshot.scoPktSize,
                (ul) snapshot.interval,
                (ul) 1000000 * snapshot.scoPktSize * 8 / snapshot.interval,
                snapshot.samps, /* HERE: why is this an int? */
                (long) snapshot.samps / (snapshot.scoRate / 1000));
	    printch (snapshot.scoch);
	    printlf ("\n");
        break;
    case ScoFileSend::FINISHED_SEND:
        printTimestamp ();

        printlf ("sends: %lu octets queued in %lu s (%lu kbps) on ",
                 (ul) snapshot.total,
                 (ul) snapshot.timeElapsed,
                 (ul) snapshot.total / ((1000 / 8) * snapshot.timeElapsed));
        printch (snapshot.scoch);
        printlf (" (%lu ms overruns -- worst %lu ms)\n",
                 (ul) snapshot.over, (ul) snapshot.worstOver);
        break;
    case ScoFileSend::TIME_END_PERIOD_FAILED:
        printTimestamp ();
        printlf ("sends: timeEndPeriod (1) failed!\n");
        break;
    /*
    case ScoFileSend::COULD_NOT_READ_CLOCK_RESOLUTION:
        printTimestamp ();
        printlf ("sends: couldn't read realtime clock resolution (%s)!\n", strerror (errno));
        break;
    case ScoFileSend::READ_CLOCK_RESOLUTION:
        printTimestamp();
        printlf ("sends: clock_getres reports that timer resolution is %lu ms!\n", snapshot.resolution);
        break;
    case ScoFileSend::FILE_ERROR:
        printTimestamp();
        printlf ("sends: %s!\n", strerror (errno));
        break;
    */ /* HERE: Why is FILE_ERROR not being handled? */
    }
}

extern "C" bool isScoChannelOpen(uint16 ch)
{
    return scoEngine->isConnected(ch);
}

extern "C" bool openScoChannel (uint16 sco_num, uint16 ch)
{
    scoch[sco_num] = ch;
    if (scoEngine->connect(ch))
    {
        scoLoggers[sco_num].setCh(ch, scoEngine);
        scoFileSenders.insert(
            std::pair<uint16, ScoFileSend>(ch, ScoFileSend(ch,
                                                           scoFileSendAlert, *scoEngine)));
        return true;
    }
    else
    {
        return false;
    }
}

extern "C" bool closeScoChannel (uint16 sco_num, uint16 ch)
{
    scoch[sco_num] = NOHANDLE;
    scoLoggers[sco_num].disconnect();

    std::map<uint16, ScoFileSend>::iterator fileSender(scoFileSenders.find(ch));
    if (fileSender != scoFileSenders.end())
    {
        scoFileSenders.erase(ch);
    }

    return scoEngine->disconnect(ch);
}

extern "C" void scoEngineReInit()
{
    int i;

    for (i = 0; i < SCO_COUNT; ++i)
    {
        scoLoggers[i].restart();
    }

    for (i = 0; i < 10; ++i)
	{
		scoch[i] = NOHANDLE;

	}

    scoFileSenders.clear();

    delete scoEngine;
    scoEngine = new ScoEngine(pdd, &scoCallback);
}

extern "C" bool enableScoLogging(uint16 sco_num, const char *fileName)
{
    return scoLoggers[sco_num].enable(fileName);
}

extern "C" void disableScoLogging(uint16 sco_num)
{
    scoLoggers[sco_num].disable();
}

extern "C" bool enableScoFileSend(uint16 scoch,
                                  const char *fileName,
                                  uint32 rate,
                                  uint32 sampleSize,
                                  bool overridePacketSize,
                                  uint16 packetSize,
                                  bool overrideNumPackets,
                                  uint16 numPackets)
{
    std::map<uint16, ScoFileSend>::iterator
        fileSender(scoFileSenders.find(scoch));

    if (scoFileSenders.end() != fileSender)
        return fileSender->second.enable(fileName,
                                         rate,
                                         sampleSize,
                                         overridePacketSize,
                                         packetSize,
                                         overrideNumPackets,
                                         numPackets);
    else
        return false;
}

extern "C" bool disable(uint16 scoch)
{
    std::map<uint16, ScoFileSend>::iterator
        fileSender(scoFileSenders.find(scoch));

    if (scoFileSenders.end() != fileSender)
    {
        return fileSender->second.disable();
    }
    else
    {
        return false;
    }
}

extern "C" bool warm_reset()
{
    return pdd->forceReset(20, BlueCoreDeviceController::bccmd_warm);
}

extern "C" bool reconfigure_uart_baudrate(uint32 baudrate)
{
    return pdd->reconfigure_uart_baudrate(baudrate);;
}

extern "C" void sendH4dp()
{
    // Treat the first byte as a packet type, which will be added later.
    uint8 h4dpdata[] = {0x81, 0x70, 0x51, 0x1f, 0x68, 0x34, 0x91, 0x80, 0xd9, 0x8f, 0x48, 0x34};
    PDU pdu(PDU::h4dp, h4dpdata, sizeof(h4dpdata));

    if (!pdd->send(pdu, false))
    {
        printf("rejected by device controller\n");
    }
}

extern "C" void sendH4Raw(const uint8 *data, size_t len)
{
    PDU pdu(PDU::raw, data, len);
    if (!pdd->send(pdu, false))
    {
        printf("rejected by device controller\n");
    }
}
