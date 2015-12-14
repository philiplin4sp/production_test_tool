////////////////////////////////////////////////////////////////////////
//
//  FILE: speed.cpp
//
//  PURPOSE: implementation file for latency and throughput test
//           harness for profiling UartAbstraction implementations.
//
//  commandline:
//      uaspeed mN1 bN2 uX rN3 [q]
//      checks and times packets from 1 byte to N1 bytes,
//      looping back through uart X at baudrate N2.  Repeats N3 times.
//      if q, less output is produced.
//
//      uaspeed mN1 bN2 uX vY rN3 [q]
//      checks and times packets from 1 byte to N1 bytes,
//      sent from uart X to uart Y at baudrate N2.  Repeats N3 times.
//      if q, less output is produced.
//
////////////////////////////////////////////////////////////////////////

#include "../physicaltransportconfiguration.h"
#include "../phys/uart.h"
#include "unicode/main.h"
#include "unicode/ichar.h"
#include "time/stop_watch.h"

#define FINISHED_OK (0)
#define UART_UNMADE (1)

static void run_test_packet_sizes( UARTAbstraction * uw, UARTAbstraction *ur, bool printing , uint32 max_size );
static void run_test ( UARTAbstraction * uw, UARTAbstraction *ur, uint32 test_len , bool print_results );

void printEvent( UARTAbstraction::Event x)
{
    printf ("Event %d\n",x);
}

void pattern ( uint8 * start , uint32 length )
{
    uint32 val = 1;
    for ( unsigned int i = 0; i < length ; ++i )
    {
        start[i] = uint8(val&0xFF);
        val = val * (val+1); // Coveyou alg (Knuth).
    }
}

bool get_uint32 ( ichar*string , uint32& val)
{
    char * got = 0;
    uint32 lval = istrtoul(string, &got, 0);
    if ( got != string && got && *got == 0 )
    {
        val = lval;
        return true;
    }
    else
    {
        return false;
    }
}

int imain(int argc,ichar** argv)
{
    bool print_results = true;
    uint32 repeats = 1;
    uint32 max_size = 1000;
    ichar * lWriteName = II("c4");
    ichar * lReadName = NULL;
    uint32 lBaudrate = 4000000;
    UARTConfiguration::parity lParity = UARTConfiguration::none;
    uint8 lStopBits = 1;
    bool lFlowControlOn = false;
    uint32 lReadTimeout = 0;
#define BPB (10)

    //  check commandline
    for ( int i = 1 ; i < argc ; ++i )
    {
        switch (*argv[i])
        {
        case 'm':
            if ( !get_uint32(argv[i]+1,max_size) )
                iprintf(II("Ignoring %s\n"),argv[i]);
            break;
        case 'r':
            if ( !get_uint32(argv[i]+1,repeats) )
                iprintf(II("Ignoring %s\n"),argv[i]);
            break;
        case 'b':
            if ( !get_uint32(argv[i]+1,lBaudrate) )
                iprintf(II("Ignoring %s\n"),argv[i]);
            break;
        case 'u':
            lWriteName = argv[i]+1;
            break;
        case 'v':
            lReadName = argv[i]+1;
            break;
        case 'q':
            print_results = false;
            break;
        default:
            iprintf(II("Ignoring %s\n"),argv[i]);
            break;
        }
    }

    // construct the UARTAbstraction object
    UARTConfiguration wconf(lWriteName,lBaudrate,lParity,lStopBits,lFlowControlOn,lReadTimeout);

    bool opened = false;
    UARTAbstraction * uw = wconf.make();
    UARTAbstraction * ur = NULL;

    if (uw)
        opened = uw->open();
    iprintf(II("%s %s\n"),opened?II("Opened"):II("Failed"),lWriteName);

    if (!uw || !opened)
    {
        return UART_UNMADE;
    }

    //  construct the second uart
    if ( lReadName )
    {
        UARTConfiguration rconf(lReadName,lBaudrate,lParity,lStopBits,lFlowControlOn,lReadTimeout);
        ur = rconf.make();

        if (ur)
            opened = ur->open();
        iprintf(II("%s %s\n"),opened?II("Opened"):II("Failed"),lReadName);

        if (!ur || !opened)
        {
            return UART_UNMADE;
        }
    }

    if ( !ur )
        ur = uw;

    // run tests
    for ( unsigned int j = 0 ; j < repeats ; ++j )
        run_test_packet_sizes(uw,ur,print_results,max_size);

    // tidy up
    iprintf(II("Bye-"));
    fflush(stdout);
    delete uw;
    if ( uw != ur )
        delete ur;
    iprintf(II("bye.\n"));
    return FINISHED_OK;
}


// test assumes simplex packet transfer from uw to ur
// (or loopback if uw == ur)
struct packet
{
    uint32 time;  // arrival time (since write)
    uint32 bytes; // number of bytes in packet;
};

void print_results(uint8 * from, uint8 * to, uint32 test_len, bool write_done, uint32 w, uint32 totalRead, packet * result, uint32 pkt );

void run_test_packet_sizes( UARTAbstraction * uw, UARTAbstraction *ur, bool printing , uint32 max_size )
{
    for ( unsigned int i = 1 ; i < 1+max_size ; ++i )
        run_test(uw,ur,i,printing);
}

void run_test ( UARTAbstraction * uw, UARTAbstraction *ur, uint32 test_len , bool printing )
{
    // resulting latency and duration calculations
    packet result[test_len]; // data for each arriving packet
    uint32 pkt = 0; // packet counter

    // create and queue data.
    uint8* from = new uint8[test_len];
    pattern(from,test_len);
    uint32 w = 0;
    StopWatch s;
    bool write_done = uw->write(from, test_len, &w);
    //printf("write: %s:%lu\n",write_done?"W":"X",w);

    // gather returning data
    uint8* to = new uint8[test_len];
    uint32 totalRead = 0;
    
    enum { rread, rwait , rstop } state = rread;
    while ( state != rstop && totalRead < test_len )
    {
        uint32 r = 0;
        switch (state)
        {
        case rread:
            if (ur->read(to + totalRead,test_len - totalRead, &r))
            {
                result[pkt].time = s.uduration();
                result[pkt].bytes = r;
                pkt ++;
                totalRead += r;
                //printf("read R:%lu\n",r);
            }
            else
            {
                state = rwait;
                //printf("read X:%lu\n",r);
            }
            break;
        case rwait:
            {
            // reading paused; wait for next kick
            UARTAbstraction::Event x = ur->wait(1000,&r);
            if ( x== UARTAbstraction::UART_RCVDONE )
            {
                result[pkt].time = s.uduration();
                result[pkt].bytes = r;
                pkt ++;
                totalRead += r;
                state = rread;
                //printf("recv R:%lu\n",r);
            }
            else if ( x == UARTAbstraction::UART_DATAWAITING )
            {
                //printf("data X:%lu\n",r);
                state = rread;
            }
            else
            {
                printEvent(x);
                state = rstop;
            }
            break;
            }
        case rstop:
            break;
        }
    }
    if ( printing )
        print_results(from,to,test_len,write_done,w,totalRead,result,pkt);

    // clean up
    delete[] to;
    delete[] from;
}

void print_results(uint8 * from, uint8 * to, uint32 test_len, bool write_done, uint32 w, uint32 totalRead, packet * result, uint32 pkt )
{
    // check bytes match
    bool match = memcmp(from,to,test_len) == 0;
    // display results
    iprintf(II("test: %s:%d,R:%d %s\n"),write_done?"W":"X",w,totalRead,match?"pass":"fail");
    if ( match )
    {
        for ( unsigned int i = 0 ; i < pkt ; ++i )
        {
            printf("Packet %u\n  size: %lu\n  time: %lu\n", i, result[i].bytes, result[i].time);
            if ( i > 0 )
                printf("  ptime: %lu\n  speed: %.3fMbps\n", result[i].time - result[i-1].time,((double)result[i].bytes * BPB)/(result[i].time - result[i-1].time) );
        }
        printf("latency: %lu\nenvelop: %lu\nthruput: %.3fMbps\n", result[0].time, result[pkt-1].time, (double(test_len) * BPB)/result[pkt-1].time);
        if ( pkt > 1 )
            printf("speed: %.3fMbps\n", ((double)test_len * BPB)/(result[pkt-1].time - result[0].time) );
    }
    else
    {
        char * sep = "";
        printf("from:\n");
        for ( unsigned int i = 0 ; i < test_len ; ++i )
        {
            printf("%s%02x",sep,from[i]);
            if ((i+1)%8) sep = " "; else sep = "\n";
        }
        sep = "";
        printf("\nto:\n");
        for ( unsigned int i = 0 ; i < totalRead ; ++i )
        {
            printf("%s%02x",sep,to[i]);
            if ((i+1)%8) sep = " "; else sep = "\n";
        }
        printf("\n");
    }
    printf("\n\n");
}
