#include "btcli.h"
#include "decodecallback.h"
#include "thread/critical_section.h"
extern "C"
{
#include "print.h"
#include "globals.h"
#include "constants.h"
#include "dictionaries.h"
};

#include "aclengine/aclengine.h"
#include "scoengine/scoengine.h"

#include "fastpipe_engine_iface.h"
#include <cassert>

extern AclEngine *aclEngine;
extern ScoEngine *scoEngine;

void DecodeCallBack::onPDU ( const PDU& pdu)
{
    switch (pdu.channel())
    {
    case PDU::other:
        break;
    case PDU::le:
        break;    
    case PDU::bccmd:
        onBCCMDResponse(pdu);
        break;
    case PDU::hq:
        onHQRequest(pdu);
        break;
    case PDU::dm:
        break;
    case PDU::hciCommand:
        onHCIEvent(pdu);
        break;
    case PDU::hciACL:
        onHCIACLData(pdu);
        break;
    case PDU::hciSCO:
        onHCISCOData(pdu);
        break;
    case PDU::l2cap:
        break;
    case PDU::rfcomm:
        break;
    case PDU::sdp:
        break;
    case PDU::debug:
        onDebug(pdu);
        break;
    case PDU::upgrade:
        break;
    case PDU::vm:
        onVMData(pdu);
        break;
    case PDU::bcsp_channel_count:
        break;
    case PDU::lmp_debug:
        onLMPdebug(pdu);
        break;
    default:
        break;
    }
}

// handle the "BadPDU" responses from HCIPDUFactory decoding
void printBadPDU (BadPDUReason r, const uint8 *pdu, uint32 len)
{
    if (r == BP_HCI_DATA_RESERVED_PBF || r == BP_HCI_DATA_RESERVED_BCF)
    {
        /*
         * hci_data_wrong_size takes precedence over
         * hci_data_reserved_pbf/bcf, so if we get this
         * far we know the length is OK.
         */
        //decodeAcl ((pdu[0] | (pdu[1] << 8)) & 0x0fff, (pdu[1] >> 4) & 3, pdu[1] >> 6, pdu + 4, len - 4);
    }
    else
    {
        size_t i;

#ifdef THREADING_WORKAROUND
    CriticalSection::Lock cs_lock (cs);
#endif
        printTimestamp ();
        printlf ("bad_pdu ");
        printByValue (badPDUreason_d, (uint32) r, 8, "r");
        printlf (":");
        for (i = 0; i < len; ++i)
        {
            printlf (" 0x%02x", pdu[i]);
        }
        printlf (" [len %lu]\n", (ul) len);
    }

}

void DecodeCallBack::onBCCMDResponse(const PDU &pdu) 
{
    BadPDUReason failCode;
    uint32 length = HCIPDUFactory::decomposeBCCMD_PDU (pdu, 0, failCode);

    if (length == 0)
    {
        printBadPDU (failCode, pdu.data(), pdu.size() );
    }
    else
    {
        uint32 *pa = new uint32 [length];
    
        if (pa == 0)
        {
            return;
        }

        pa [0] = length;
        length = HCIPDUFactory::decomposeBCCMD_PDU (pdu, pa, failCode);

#ifdef THREADING_WORKAROUND
        CriticalSection::Lock lock (cs);
#endif
#if 0
        size_t i;
        ASSERT (pa[0] >= 4);

        printlf ("App: received bccmd response for cmd 0x%lx with seqno 0x%lx: status %lu, pa[] =", (ul) pa[1], (ul) pa[2], (ul) pa[3]);
        for (i = 4; i < pa[0]; ++i)
        {
            printlf (" 0x%lx", (ul) pa[i]);
        }
        printlf (" [%u args]\n", pa[0] - 4);
#else
        decodeBCCMD (pa);
#endif
        delete[] pa;
    }    
}

void DecodeCallBack::onHQRequest(const PDU &pdu) 
{
    BadPDUReason failCode;

    uint32 length = HCIPDUFactory::decomposeHQ_PDU (pdu, 0, failCode);

    if (length == 0)
    {
        printBadPDU (failCode, pdu.data(), pdu.size() );
    }
    else
    {
        uint32 *pa = new uint32 [length];

        if (pa == 0)
        {
            return;
        }

        pa [0] = length;
        length = HCIPDUFactory::decomposeHQ_PDU(pdu, pa, failCode);
    
#ifdef THREADING_WORKAROUND
        CriticalSection::Lock cs_lock (cs);
#endif
#if 0
        size_t i;
        ASSERT (pa[0] >= 4);

        printlf ("App: received hq indication; cmd 0x%lx with seqno 0x%lx, status %lu, pa[] =", (ul) pa[1], (ul) pa[2], (ul) pa[3]);
        for (i = 4; i < pa[0]; ++i)
        {
            printlf (" 0x%lx", (ul) pa[i]);
        }
        printlf (" [%u args]\n", pa[0] - 4);
#else
        decodeHQ (pa);
#endif
    }
}

void DecodeCallBack::onHCIEvent(const PDU &pdu) 
{
    HCIEventPDU eventPdu(pdu);
    BadPDUReason failCode;
    uint32 length = HCIPDUFactory::decomposeEventPDU(pdu, 0, failCode);
    
    if ( length == 0 )
    {
        printBadPDU (failCode, pdu.data(), pdu.size() );
        return;
    }

    if (eventPdu.get_event_code() == HCI_EV_NUMBER_COMPLETED_PKTS)
    {
        aclEngine->nocp(pdu);
    }
    else if (eventPdu.get_event_code() == HCI_EV_COMMAND_COMPLETE)
    {
        HCICommandCompletePDU cc_pdu(pdu);
        if (cc_pdu.get_op_code() == HCI_READ_BUFFER_SIZE)
        {
            HCI_READ_BUFFER_SIZE_RET_T_PDU rbs_pdu(cc_pdu);
            aclEngine->setBufferSizes(rbs_pdu);
            scoEngine->setBufferSizes(rbs_pdu);
        }
    }

    if (length != 0)
    {
        uint32 *pa = new uint32 [length];

        if (pa == 0)
        {
            return;
        }

        pa [0] = length;
        length = HCIPDUFactory::decomposeEventPDU(pdu, pa, failCode);

#ifdef THREADING_WORKAROUND
        CriticalSection::Lock cs_lock (cs);
#endif
#if 0
        unsigned i;

        ASSERT (pa[0] >= 2);
        ASSERT (ec == pa[1]);

        printlf ("App: received event, code 0x%02x, pa[] =", ec);
        for (i = 2; i < pa[0]; ++i)
        {
            printlf (" 0x%lx", (ul) pa[i]);
        }
        printlf (" [%u args]\n", pa[0] - 2);
#else
        decodeEvt (pa);
#endif
    }
}

#if 0
static BROADCAST_TYPE conv ( HCIACLPDU::broadcast_type x )
{
    switch (x)
    {
    case HCIACLPDU::ptp:
        return bc_type_p2p;
    case HCIACLPDU::act:
        return bc_type_active;
    case HCIACLPDU::pic:
        return bc_type_piconet;
    case HCIACLPDU::bct_reserved:
        return bc_type_unused;
    default:
        //  if we don't recognise it pass it through.
        return BROADCAST_TYPE(x);
    }
}
#endif

static PBF conv_pbf (HCIACLPDU::packet_boundary_flag x)
{
    switch ( x )
    {
    case HCIACLPDU::start:
        return pbf_start;
    case HCIACLPDU::cont:
        return pbf_continuation;
    case HCIACLPDU::start_not_auto_flush:
        return pbf_start_not_auto_flush;
    default:
        //  if we don't recognise it pass it through.
        return (PBF)x;
    };
}

void DecodeCallBack::onHCIACLData(const PDU &pdu) 
{
    HCIACLPDU aclpdu(pdu);
#if 0
    uint16 ch = pdu.get_handle ();
    bool pb = pdu.get_pbf () == HCIACLPDU::start;
    BROADCAST_TYPE bf = conv (pdu.get_bct ());
    const uint8 *data = pdu.get_dataPtr ();
    uint16 length = pdu.get_length ();
    unsigned u;

    ASSERT (bf <= 2);

    printlf ("App: received data [%s%s], data =", pb ? "start" : "contin", bf == 0 ? "" : bf == 1 ? "; active broadcast" : "; piconet broadcast");
    for (u = 0; u < length; ++u)
    {
        printlf (" %02x", data[u]);
    }
    printlf (" [len %lu]\n", (ul) length);

    pdd->sendHCIData (ch, pb, bf, data, length);
#else
    {
        
        btcli_enter_critical_section();
        if (fastpipeEngineIsConnected(aclpdu.get_handle()))
        {
            fastpipeEngineReceive(aclpdu);
            btcli_leave_critical_section();
        }
        else
        {
            // Leave critical section before entering aclEngine.
            // AclEngine isn't very hygeinic about its threads, so printlf 
            // (which needs locking) is called from various different threads.
            // CriticalSection::Lock on Linux can't be obtained recursively, so
            // just drop the lock now to make life easier.
            btcli_leave_critical_section();
            aclEngine->receive(aclpdu);
        }
    }
#endif
}

extern "C" uint32 scoch[10]; /* HERE */
//extern "C" FILE *fsco[SCO_COUNT];

void DecodeCallBack::onHCISCOData(const PDU &pdu) 
{
/*    HCISCOPDU spdu(pdu);
    const uint8 *data = spdu.get_dataPtr();
    uint32 length = spdu.get_length();

printlf("\"");
        for (int i = 0; i < length; ++i)
        {
            printlf ("%c", (char) data[i]);
        }
printlf("\"\n");*/

    scoEngine->receive(HCISCOPDU(pdu));

/*    const uint8 *data = pdu.data ();
    uint32 length = pdu.size();

    #ifdef THREADING_WORKAROUND
    CriticalSection::Lock cs_lock (cs);
#endif
    uint32 ch;
    size_t i;

    if (length < 3 || length != data[2] + 3U)
    {
        printTimestamp ();
        printlf ("sco");
        for (i = 0; i < length; ++i)
        {
            printlf (" 0x%02x", data[i]);
        }
        printlf ("\n");
    }
    else
    {
        ch = data[0] | (data[1] << 8);
		bool sent_to_file = false;
        if (ch != NOHANDLE)
        {
			if (fsco[0] != NULL && ch == scoch[0])
			{
#ifdef MGRSCOHEADERS
				fwrite (data, 1, length, fsco[0]);
#else
				fwrite (data + 3, 1, length - 3, fsco[0]);
#endif
				sent_to_file = true;
			}
			if (fsco[1] != NULL && ch == scoch[1])
			{
#ifdef MGRSCOHEADERS
				fwrite (data, 1, length, fsco[1]);
#else
				fwrite (data + 3, 1, length - 3, fsco[1]);
#endif
				sent_to_file = true;
			}
			if (fsco[2] != NULL && ch == scoch[2])
			{
#ifdef MGRSCOHEADERS
				fwrite (data, 1, length, fsco[2]);
#else
				fwrite (data + 3, 1, length - 3, fsco[2]);
#endif
				sent_to_file = true;
			}
        }
        if (!sent_to_file && showSco)
        {
            printTimestamp ();
            printlf ("sco ");
            printch (ch);
            for (i = 3; i < length; ++i)
            {
                printlf (" 0x%02x", data[i]);
            }
            printlf ("\n");
        }
    }*/
}

void DecodeCallBack::onDebug ( const PDU& pdu) 
{
    const uint8 *buf = pdu.data ();
    size_t len = pdu.size ();

#ifdef THREADING_WORKAROUND
    CriticalSection::Lock cs_lock (cs);
#endif
#if 0
    ASSERT (len != 0);

    if (buf[len - 1] == NUL)
    {
        printlf ("debug");
        printlf (" \"");
        while (len--)
        {
            printChar (*buf++);
        }
        printlf ("\"\n");
    }
    else
    {
        printlf ("debug");
        for (; len != 0; --len)
        {
            printlf (" 0x%02x", *buf++);
        }
        printlf ("\n");
    }
#else   
    decodeDebug(buf, len);
#endif
}

void DecodeCallBack::onVMData(const PDU &pdu) 
{
    VM_PDU vmPdu(pdu);
    uint32 len = vmPdu.get_words();
    uint16 *data = new uint16 [len];
    vmPdu.get_packet(data);

#ifdef THREADING_WORKAROUND
    CriticalSection::Lock cs_lock (cs);
#endif
#if 1
    size_t u;
//  ASSERT (len != 0);

    if ( data[0] == 4 && data[1] == 0x80 && data[2] < 16 )
    {
        static class LineBuff : private NoCopy
        {
            size_t held, size;
            char *data;
        public:
            LineBuff()
                : held(0), size(256), data(new char[size])
            { }
            ~LineBuff()
            { delete [] data; }

            void append(char c)
            {
                if(held >= size)
                {
                    char *fresh = new char[size *= 2];
                    memcpy(fresh, data, held);
                    delete[] data;
                    data = fresh;
                }
                data[held++] = c;
            }

            const char *string()
            { append('\0'); return data; }

            void reset()
            { held = 0; }
        } line[16];

        // It's a VmPutChar packet, collect until newline
        int chan = data[2];
        ichar c = (ichar)data[3];
        if(c == '\n')
        {
            const char *s;

            printTimestamp ();
            printlf ("vm_print ");
            if (chan != 0)
            {
                printlf ("c:0x%1x ", chan);
            }
            s = line[chan].string();
            printlf ("\"");
            while ((c = *s++) != NUL)
            {
                printChar (c);
            }
            printlf ("\"\n");
            line[chan].reset();
        }
        else
            line[chan].append(c);
    }
    else
    {
        printTimestamp ();
        printlf ("vm_data");
#ifdef MMUSE8BITHOST
        /*
         * Use the old 8-bit code (deprecated)
         */
        if (len >= 1)
        {
            printlf (" ");
            printByValue (NULL, buf[0], 8, "len");
        }
        if (len >= 2)
        {
            printlf (" ");
            printByValue (NULL, buf[1], 8, "sc");
        }
        for (u = 2; u < len; ++u)
        {
            printlf (" 0x%02x", buf[u]);
        }
        if (len != 0 && len != buf[0])
        {
            printlf (" [len %lu]", (ul) len);
        }
#else/*MMUSE8BITHOST*/
        /*
         * Use the 16-bit version
         */
        uint32 len = data[0] * 2;
        if (len >= 2)
        {
            printlf (" ");
            printByValue (NULL, data[0] , 8 , "len" );
        }
        if (len >= 4)
        {
            printlf (" ");
            printByValue (NULL, data[1] , 8 , "sc" );
        }
        for (u = 4; u < len; u+=2)
        {
            printlf ( " 0x%04x", data[u/2] );
        }
        printlf (" [len %lu]", (ul) len);
#endif/*MMUSE8BITHOST*/
        printlf ("\n");
    }
#else
    decodeVMdata (buf, len);
#endif /* 1 */
}


void DecodeCallBack::onLMPdebug(const PDU &pdu)
{
    const uint8 *buf = pdu.data ();
    size_t len = pdu.size ();

#ifdef THREADING_WORKAROUND
    CriticalSection::Lock cs_lock (cs);
#endif
#if 0
    ASSERT (len != 0);

    printlf ("lmp");
    for (; len != 0; --len)
    {
        printlf (" 0x%02x", *buf++);
    }
    printlf ("\n");
#else
    decodeLMPdebug (buf, len, NULL);
#endif
}

//    virtual void onUnknownPDU    ( const PDU& ) {};

extern "C" bool auto_rawlog;

void DecodeCallBack::onTransportFailure (FailureMode f) 
{
    {
#ifdef THREADING_WORKAROUND
        CriticalSection::Lock cs_lock (cs);
#endif
        printTimestamp ();
        iprintlf(II("*** HCI TRANSPORT FAILED ***  (failureMode: "));
        printByValue(transportErrors_d, f, 16, "Unknown transport error");
        iprintlf(II(")"));
#ifdef DONT_COMPLETELY_SCREW_UP_ANYONE_EXPECTING_NORMAL_END_OF_LINE_TERMINATORS
        printlf("\n");
#else
        printf ("\r"); printlfOnly ("\n");
        fflush (stdout);
    }
    if (auto_rawlog)
    {
        parseCmd("rawlog", true);
    }
#endif

}
