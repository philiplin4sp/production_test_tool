#include <iostream>
#include <string>
#include <memory>
#include <sstream>
using namespace std;
#include "csrhci/bccmd_spi.h"
#include "csrhci/bccmd_hci.h"
#include "csrhci/transportconfiguration.h"
#include "pttransport/pttransport.h"
#include "time/hi_res_clock.h"

static const char *gComPort = "COM1";
static const char *gUsbPort = "\\\\.\\csr0";
static uint32 gBaud = 38400;
static const char *gSpiTransport = "USB";
static int gSpiPort = 0;

#define PRINT(x) (cout << gIndent << x << endl)
#define RESULT(success) (success ? "SUCCESS" : "FAILURE")

class CIndent
{
public:
    CIndent() : mIndent(0) {}
    operator int&() { return mIndent; }
private:
    int mIndent;
} gIndent;
ostream &operator<<(ostream &aOs, CIndent &aI)
{
    for(int i = 0; i < aI; ++i)
    {
        aOs << "    ";
    }
    return aOs;
}

static bool Test_Basic(bccmd_trans *engine, bool do_open, bool print = true)
{
    bool success = true;
    if (do_open)
    {
        if (print)
        {
            PRINT("opening bccmd_trans");
        }
        success = engine->open();
        if (print)
        {
            PRINT(RESULT(success));
        }
    }
    if (success)
    {
        // do a PSNEXTALL with the first send method.
        uint16 psnextall_len;
        uint16 value1;
        if (success)
        {
            if (print)
            {
                PRINT("attempting PSNEXTALL formatted BCCMD");
            }
            BCCMD_PSNEXT_ALL_PDU pdu;
            pdu.set_req_type(BCCMDPDU_GETREQ);
            pdu.set_id(0);
            pdu.set_length();
            psnextall_len = pdu.get_length();
            BCCMD_PSNEXT_ALL_PDU resp(engine->run_pdu_report_errs(pdu));
            value1 = resp.get_nextid();
            success = resp.channel() != PDU::zero && resp.get_status() == BCCMDPDU_STAT_OK;
            if (print)
            {
                PRINT(RESULT(success));
            }
        }
        // do the same with the second send method
        uint16 value2;
        if (success)
        {
            if (print)
            {
                PRINT("attempting PSNEXTALL raw BCCMD");
            }
            BCCMDPDU pdu;
            pdu.type = BCCMDPDU_GETREQ;
            pdu.seqno = 0;
            pdu.pdulen = psnextall_len;
            pdu.varid = BCCMDVARID_PSNEXT_ALL;
            pdu.status = BCCMDPDU_STAT_OK;
            pdu.d.psna.nextid = 0;
            pdu.d.psna.id = 0;
            success = engine->run_pdu_report_errs(&pdu) && pdu.status == BCCMDPDU_STAT_OK;
            value2 = pdu.d.psna.nextid;
            if (print)
            {
                PRINT(RESULT(success));
            }
        }
        if (success)
        {
            if (print)
            {
                PRINT("comparing results");
            }
            // and check they gave the same result.
            success = value1 == value2;
            if (print)
            {
                PRINT(RESULT(success));
            }
        }

        // then close.
        if (do_open)
        {
            if (print)
            {
                PRINT("closing bccmd_trans");
            }
            if(!engine->close())
            {
                if (print)
                {
                    PRINT(RESULT(false));
                }
                success = false;
            }
        }
    }
    return success;
}

static bool Test_Basic_Spi()
{
    ostringstream trans;
    trans << "SPITRANS=" << gSpiTransport << " SPIPORT=" << gSpiPort;
    PRINT("Starting basic bccmd_spi tests. trans = \"" << trans.str() << "\"");
    ++gIndent;
    unsigned int before = pttrans_count_streams();
    bool r = Test_Basic(&bccmd_spi(&bccmd_trans_error_handler(), trans.str().c_str()), true);
    if (r)
    {
        PRINT("counting streams");
        r = (pttrans_count_streams() == before);
        PRINT(RESULT(r));
    }
    --gIndent;
    PRINT(RESULT(r));
    return r;
}

static bool Test_Basic_BCSP()
{
    PRINT("Doing basic bccmd_hci BCSP tests. port=" << gComPort << ", baud=" << gBaud);
    ++gIndent;
    bool r = Test_Basic(&bccmd_hci(&bccmd_trans_error_handler(), new BCSPConfiguration(UARTConfiguration(gComPort, gBaud, BCSP_parity , BCSP_stopbits , true , BCSP_readtimeout))), true);
    --gIndent;
    PRINT(RESULT(r));
    return r;

}

static bool Test_Basic_H4()
{
    PRINT("Doing basic bccmd_hci H4 tests. port=" << gComPort << ", baud=" << gBaud);
    ++gIndent;
    bool r = Test_Basic(&bccmd_hci(&bccmd_trans_error_handler(), new H4Configuration(gComPort, gBaud)), true);
    --gIndent;
    PRINT(RESULT(r));
    return r;
}

static bool Test_Basic_USB()
{
    PRINT("Starting basic bccmd_hci USB tests. port=" << gUsbPort);
    ++gIndent;
    bool r = Test_Basic(&bccmd_hci(&bccmd_trans_error_handler(), new H2Configuration(USBConfiguration(gUsbPort))), true);
    --gIndent;
    PRINT(RESULT(r));
    return r;
}

static bool Test_PtStream(pttrans_stream_t stream, bool print = true)
{
    bool success = false;
    if (print)
    {
        PRINT("Testing pttransport stream");
    }

    uint16 ff9a, fe81;
    if (pttrans_stream_read_verify(stream, 0xFF9A, &ff9a) == PTTRANS_SUCCESS
     && pttrans_stream_read_verify(stream, 0xFE81, &fe81) == PTTRANS_SUCCESS)
    {
        CXapHelper tempXapHelper;
        tempXapHelper.AllowUnknownChipType();
        tempXapHelper.SetFF9A(ff9a);
        tempXapHelper.SetFE81(fe81);
        success = tempXapHelper.IsReadyForUse();
    }

    if (print)
    {
        PRINT(RESULT(success));
    }

    return success;
}

class CPtStream
{
public:
    CPtStream() : mStream(PTTRANS_STREAM_INVALID) {}
    ~CPtStream()
    {
        if (!PTTRANS_STREAMS_EQUAL(mStream, PTTRANS_STREAM_INVALID))
        {
            pttrans_stream_close(mStream);
        }
    }

    bool open(const char *aTrans)
    {
        bool r = pttrans_stream_open(&mStream, aTrans) == PTTRANS_SUCCESS;
        if (!r)
        {
            mStream = PTTRANS_STREAM_INVALID;
        }
        return r;
    }

    operator pttrans_stream_t() const { return mStream; }

private:
    pttrans_stream_t mStream;
};

class CPtThread : public Threadable
{
protected:
    int ThreadFunc()
    {
        mSuccess = true;
        while(mContinue && mSuccess)
        {
            CPtStream stream;
            mSuccess = stream.open(mTrans) && Test_PtStream(stream, false);
        }
        return 0;
    }

private:
    bool mSuccess;
    bool mContinue;
    const char *mTrans;

public:
    CPtThread(const char *aTrans) : mSuccess(false), mContinue(true), mTrans(aTrans) {}
    bool success() const { return mSuccess; }
    void stop() { mContinue = false; }
};

class CBccmdSpiThread : public Threadable
{
protected:
    int ThreadFunc()
    {
        mSuccess = true;
        while(mContinue && mSuccess)
        {
            auto_ptr<bccmd_spi> bc;
            bccmd_trans_error_handler errhandler;
            errhandler.clear_errors();
            bc.reset(new bccmd_spi(&errhandler, mTrans));
            mSuccess = errhandler.get_last_error_code() == 0 && Test_Basic(bc.get(), true, false);
        }
        return 0;
    }

private:
    bool mSuccess;
    bool mContinue;
    const char *mTrans;

public:
    CBccmdSpiThread(const char *aTrans) : mSuccess(false), mContinue(true), mTrans(aTrans) {}
    bool success() const { return mSuccess; }
    void stop() { mContinue = false; }
};

static bool Test_Spi_Streams()
{
    bool success = true;
    string trans1;
    string trans2;
    trans1 = "SPITRANS=";
    trans1 += gSpiTransport;
    trans1 += " SPIPORT=0";
    trans2 = "SPITRANS=";
    trans2 += gSpiTransport;
    trans2 += " SPIPORT=1";

    PRINT("Doing SPI stream tests");
    ++gIndent;

    unsigned int streams = pttrans_count_streams();

    if (success)
    {
        // T1_20A
        CPtStream stream;
        PRINT("opening pttransport stream. trans = \"" << trans1 << "\"");
        success = stream.open(trans1.c_str());
        PRINT(RESULT(success));

        unsigned int before = pttrans_stream_get_num_operations(stream);

        bccmd_trans_error_handler errhandler;
        errhandler.clear_errors();
        auto_ptr<bccmd_spi> bc;
        PRINT("opening bccmd_spi. trans = \"" << trans2 << "\"");
        bc.reset(new bccmd_spi(&errhandler, trans2.c_str()));
        success = errhandler.get_last_error_code() == 0;
        PRINT(RESULT(success));

        if (success)
        {
            PRINT("testing bccmd_spi");
            ++gIndent;
            success = Test_Basic(bc.get(), true);
            --gIndent;
        }

        if (success)
        {
            PRINT("checking operations on other stream");
            success = pttrans_stream_get_num_operations(stream) == before;
        }
        PRINT(RESULT(success));
    }
    if(success)
    {
        // T1_20B
        bccmd_trans_error_handler errhandler;
        auto_ptr<bccmd_spi> bc;
        errhandler.clear_errors();
        PRINT("opening bccmd_spi. trans = \"" << trans1 << "\"");
        bc.reset(new bccmd_spi(&errhandler, trans1.c_str()));
        success = errhandler.get_last_error_code() == 0;
        PRINT(RESULT(success));

        CPtStream stream;
        if (success)
        {
            PRINT("opening pttransport stream. trans = \"" << trans2 << "\"");
            success = stream.open(trans2.c_str());
            PRINT(RESULT(success));
        }

        unsigned int before = pttrans_stream_get_num_operations(stream);

        success = success && Test_Basic(bc.get(), true);

        if (success)
        {
            PRINT("checking operations on other stream");
            success = pttrans_stream_get_num_operations(stream) == before;
        }
        PRINT(RESULT(success));
    }
    if (success)
    {
        // T1_20C
        CBccmdSpiThread bccmdThread(trans1.c_str());
        CPtThread ptThread(trans2.c_str());

        PRINT("running mixed multithread test on multiple devices");
        ++gIndent;
        PRINT("starting threads");
        bccmdThread.Start();
        ptThread.Start();
        PRINT("waiting...");
        HiResClockSleepMilliSec(10000);
        bccmdThread.stop();
        ptThread.stop();
        PRINT("waiting for bccmd thread");
        bccmdThread.WaitForStop(0);
        PRINT("waiting for pttransport thread");
        ptThread.WaitForStop(0);

        PRINT("checking bccmd thread result");
        success = bccmdThread.success();
        PRINT(RESULT(success));
        PRINT("checking pttransport thread result");
        bool ptsuccess = ptThread.success();
        PRINT(RESULT(ptsuccess));
        success = success && ptsuccess;
        --gIndent;
        PRINT(RESULT(success));
    }
    if (success)
    {
        // T1_20D
        CBccmdSpiThread bccmdThread(trans1.c_str());
        CPtThread ptThread(trans1.c_str());

        PRINT("running mixed multithread test on single device");
        ++gIndent;
        PRINT("starting threads");
        bccmdThread.Start();
        ptThread.Start();
        PRINT("waiting...");
        HiResClockSleepMilliSec(10000);
        bccmdThread.stop();
        ptThread.stop();
        PRINT("waiting for bccmd thread");
        bccmdThread.WaitForStop(0);
        PRINT("waiting for pttransport thread");
        ptThread.WaitForStop(0);

        PRINT("checking bccmd thread result");
        success = bccmdThread.success();
        PRINT(RESULT(success));
        PRINT("checking pttransport thread result");
        bool ptsuccess = ptThread.success();
        PRINT(RESULT(ptsuccess));
        success = success && ptsuccess;
        --gIndent;
        PRINT(RESULT(success));
    }
    if (success)
    {
        // T1_20E
        std::auto_ptr<CPtStream> pStream;
        pStream.reset(new CPtStream);
        PRINT("opening pttransport stream. trans = \"" << trans1 << "\"");
        success = pStream->open(trans1.c_str());
        PRINT(RESULT(success));

        bccmd_trans_error_handler errhandler;
        auto_ptr<bccmd_spi> bc;
        errhandler.clear_errors();
        PRINT("opening bccmd_spi. trans = \"" << trans1 << "\"");
        bc.reset(new bccmd_spi(&errhandler, trans1.c_str()));
        success = errhandler.get_last_error_code() == 0;
        success = success && bc->open();
        PRINT(RESULT(success));

        if (success)
        {
            PRINT("closing pttransport stream");
            pStream.reset();

            success = Test_Basic(bc.get(), false);
        }

        PRINT(RESULT(success));
    }
    if (success)
    {
        // T1_20F
        std::auto_ptr<CPtStream> pStream;
        pStream.reset(new CPtStream);
        PRINT("opening pttransport stream. trans = \"" << trans1 << "\"");
        success = pStream->open(trans1.c_str());
        PRINT(RESULT(success));

        bccmd_trans_error_handler errhandler;
        auto_ptr<bccmd_spi> bc;
        errhandler.clear_errors();
        PRINT("opening bccmd_spi. trans = \"" << trans1 << "\"");
        bc.reset(new bccmd_spi(&errhandler, trans1.c_str()));
        success = errhandler.get_last_error_code() == 0;
        success = success && bc->open();
        PRINT(RESULT(success));

        if (success)
        {
            PRINT("closing bccmd_spi");
            success = bc->close();
        }
        if (success)
        {
            bc.reset();
            PRINT("testing remaining stream");
            ++gIndent;
            success = Test_PtStream(*pStream);
            --gIndent;
        }
        PRINT(RESULT(success));
    }
    if (success)
    {
        //T1_30A
        bccmd_trans_error_handler errhandler1;
        auto_ptr<bccmd_spi> bc1;
        errhandler1.clear_errors();
        PRINT("opening bccmd_spi 1. trans = \"" << trans1 << "\"");
        bc1.reset(new bccmd_spi(&errhandler1, trans1.c_str()));
        success = errhandler1.get_last_error_code() == 0;
        success = success && bc1->open();
        PRINT(RESULT(success));

        bccmd_trans_error_handler errhandler2;
        auto_ptr<bccmd_spi> bc2;
        errhandler2.clear_errors();
        PRINT("opening bccmd_spi 2. trans = \"" << trans2 << "\"");
        bc2.reset(new bccmd_spi(&errhandler2, trans2.c_str()));
        success = errhandler2.get_last_error_code() == 0;
        success = success && bc2->open();
        PRINT(RESULT(success));

        pttrans_stream_t stream1 = bc1->PtStream();
        unsigned int before = pttrans_stream_get_num_operations(stream1);

        if (success)
        {
            PRINT("Testing bccmd_spi 2");
            ++gIndent;
            success = Test_Basic(bc2.get(), true);
            --gIndent;
        }
        if (success)
        {
            PRINT("counting operations on stream 1");
            success = PTTRANS_STREAMS_EQUAL(stream1, bc1->PtStream()) && pttrans_stream_get_num_operations(stream1) == before;
        }
        PRINT(RESULT(success));
    }
    if (success)
    {
        //T1_30B
        bccmd_trans_error_handler errhandler1;
        auto_ptr<bccmd_spi> bc1;
        errhandler1.clear_errors();
        PRINT("opening bccmd_spi 1. trans = \"" << trans1 << "\"");
        bc1.reset(new bccmd_spi(&errhandler1, trans1.c_str()));
        success = errhandler1.get_last_error_code() == 0;
        success = success && bc1->open();
        PRINT(RESULT(success));

        bccmd_trans_error_handler errhandler2;
        auto_ptr<bccmd_spi> bc2;
        errhandler2.clear_errors();
        PRINT("opening bccmd_spi 2. trans = \"" << trans2 << "\"");
        bc2.reset(new bccmd_spi(&errhandler2, trans2.c_str()));
        success = errhandler2.get_last_error_code() == 0;
        success = success && bc2->open();
        PRINT(RESULT(success));

        pttrans_stream_t stream2 = bc2->PtStream();
        unsigned int before = pttrans_stream_get_num_operations(stream2);

        if (success)
        {
            PRINT("Testing bccmd_spi 1");
            ++gIndent;
            success = Test_Basic(bc1.get(), true);
            --gIndent;
        }
        if (success)
        {
            PRINT("counting operations on stream 2");
            success = PTTRANS_STREAMS_EQUAL(stream2, bc2->PtStream()) && pttrans_stream_get_num_operations(stream2) == before;
        }
        PRINT(RESULT(success));
    }
    if (success)
    {
        // T1_30C
        PRINT("Running bccmd_spi multithread test on two devices");
        ++gIndent;
        CBccmdSpiThread thread1(trans1.c_str());
        CBccmdSpiThread thread2(trans2.c_str());
        PRINT("starting threads");
        thread1.Start();
        thread2.Start();
        PRINT("waiting...");
        HiResClockSleepMilliSec(10000);
        thread1.stop();
        thread2.stop();
        PRINT("waiting for thread 1");
        thread1.WaitForStop(0);
        PRINT("waiting for thread 2");
        thread2.WaitForStop(0);
        PRINT("checking thread 1 result");
        success = thread1.success();
        PRINT(RESULT(success));
        PRINT("checking thread 2 result");
        bool success2 = thread2.success();
        PRINT(RESULT(success2));
        success = success && success2;
        --gIndent;
        PRINT(RESULT(success));
    }
    if (success)
    {
        // T1_30D
        PRINT("Running bccmd_spi multithread test on single device");
        ++gIndent;
        CBccmdSpiThread thread1(trans1.c_str());
        CBccmdSpiThread thread2(trans1.c_str());
        PRINT("starting threads");
        thread1.Start();
        thread2.Start();
        PRINT("waiting...");
        HiResClockSleepMilliSec(10000);
        thread1.stop();
        thread2.stop();
        PRINT("waiting for thread 1");
        thread1.WaitForStop(0);
        PRINT("waiting for thread 2");
        thread2.WaitForStop(0);
        PRINT("checking thread 1 result");
        success = thread1.success();
        PRINT(RESULT(success));
        PRINT("checking thread 2 result");
        bool success2 = thread2.success();
        PRINT(RESULT(success2));
        success = success && success2;
        --gIndent;
        PRINT(RESULT(success));
    }
    if (success)
    {
        // T1_30E
        bccmd_trans_error_handler errhandler1;
        auto_ptr<bccmd_spi> bc1;
        errhandler1.clear_errors();
        PRINT("opening bccmd_spi 1. trans = \"" << trans1 << "\"");
        bc1.reset(new bccmd_spi(&errhandler1, trans1.c_str()));
        success = errhandler1.get_last_error_code() == 0;
        success = success && bc1->open();
        PRINT(RESULT(success));

        bccmd_trans_error_handler errhandler2;
        auto_ptr<bccmd_spi> bc2;
        errhandler2.clear_errors();
        PRINT("opening bccmd_spi 2. trans = \"" << trans2 << "\"");
        bc2.reset(new bccmd_spi(&errhandler2, trans2.c_str()));
        success = errhandler2.get_last_error_code() == 0;
        success = success && bc2->open();
        PRINT(RESULT(success));

        PRINT("closing bccmd spi 1");
        success = bc1->close();
        bc1.reset();

        if (success)
        {
            PRINT("testing bccmd spi 2");
            ++gIndent;
            success = Test_Basic(bc2.get(), false);
            --gIndent;
        }
        PRINT(RESULT(success));
    }
    if (success)
    {
        // T1_30F
        bccmd_trans_error_handler errhandler1;
        auto_ptr<bccmd_spi> bc1;
        errhandler1.clear_errors();
        PRINT("opening bccmd_spi 1. trans = \"" << trans1 << "\"");
        bc1.reset(new bccmd_spi(&errhandler1, trans1.c_str()));
        success = errhandler1.get_last_error_code() == 0;
        success = success && bc1->open();
        PRINT(RESULT(success));

        bccmd_trans_error_handler errhandler2;
        auto_ptr<bccmd_spi> bc2;
        errhandler2.clear_errors();
        PRINT("opening bccmd_spi 2. trans = \"" << trans2 << "\"");
        bc2.reset(new bccmd_spi(&errhandler2, trans2.c_str()));
        success = errhandler2.get_last_error_code() == 0;
        success = success && bc2->open();
        PRINT(RESULT(success));

        PRINT("closing bccmd spi 2");
        success = bc2->close();
        bc2.reset();

        if (success)
        {
            PRINT("testing bccmd spi 1");
            ++gIndent;
            success = Test_Basic(bc1.get(), false);
            --gIndent;
        }
        PRINT(RESULT(success));
    }
    if (success)
    {
        PRINT("counting streams");
        success = streams == pttrans_count_streams();
        PRINT(RESULT(success));
    }
    --gIndent;
    PRINT(RESULT(success));
    return success;
}

int main(int argc, char *argv[])
{
    bool setSpiTransport = false;
    bool setSpiPort = false;
    bool setUsbPort = false;
    bool setComPort = false;
    bool setBaud = false;
    const char *test = "spi";
    bool setTest = false;
    struct {
        const char *name;
        bool (*func)();
    } const tests[] = {
        { "spi", Test_Basic_Spi },
        { "bcsp", Test_Basic_USB },
        { "h4", Test_Basic_H4 },
        { "usb", Test_Basic_USB },
        { "streams", Test_Spi_Streams }
    };

    bool argsOk = true;
    for(int i = 1; i < argc && argsOk; ++i)
    {
        if (STRICMP(argv[i], "-spitransport") == 0)
        {
            if (!setSpiTransport && i + 1 < argc)
            {
                gSpiTransport = argv[++i];
                setSpiTransport = true;
            }
            else
            {
                argsOk = false;
            }
        }
        else if (STRICMP(argv[i], "-spiport") == 0)
        {
            if (!setSpiPort && i + 1 < argc)
            {
                gSpiPort = atoi(argv[++i]);
                setSpiPort = true;
            }
            else
            {
                argsOk = false;
            }
        }
        else if (STRICMP(argv[i], "-usbport") == 0)
        {
            if (!setUsbPort && i + 1 < argc)
            {
                gUsbPort = argv[++i];
                setUsbPort = true;
            }
            else
            {
                argsOk = false;
            }
        }
        else if (STRICMP(argv[i], "-comport") == 0)
        {
            if (!setComPort && i + 1 < argc)
            {
                gComPort = argv[++i];
                setComPort = true;
            }
            else
            {
                argsOk = false;
            }
        }
        else if (STRICMP(argv[i], "-baud") == 0)
        {
            if (!setBaud && i + 1 < argc && (gBaud = atoi(argv[i+1])) != 0)
            {
                ++i;
                setBaud = true;
            }
            else
            {
                argsOk = false;
            }
        }
        else if (!setTest)
        {
            test = argv[i];
            setTest = true;
        }
        else
        {
            argsOk = false;
        }
    }
    bool success = false;
    if (argsOk)
    {
        bool found = false;
        for(int i = 0; i < sizeof(tests)/sizeof(*tests) && !found; ++i)
        {
            if (STRICMP(tests[i].name, test) == 0)
            {
                found = true;
                PRINT("running test plan \"" << test << "\"");
                ++gIndent;
                success = (tests[i].func)();
                --gIndent;
                PRINT(RESULT(success));
            }
        }
        argsOk = found;
    }
    if (!argsOk)
    {
        cout << argv[0] << "[-spitransport USB|LPT] [-spiport port]" << endl;
        cout << "    [-usbport port] [-comport port] [-baud baud]" << endl;
        cout << "    { " << tests[0].name;
        for(int i = 1; i < sizeof(tests)/sizeof(*tests); ++i)
        {
            cout << " | " << tests[i].name;
        }
        cout << " }" << endl;
    }
    return success ? 0 : 1;
}
