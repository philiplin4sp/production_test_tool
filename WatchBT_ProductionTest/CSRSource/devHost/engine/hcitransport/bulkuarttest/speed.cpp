#include "unicode/ichar.h"
#include "common/types.h"
#include "../phys/uart.h"
#include "../physicaltransportconfiguration.h"
#include <string>
#include "unicode/main.h"

#define STR(x) #x
#define XSTR(x) STR(x)

#define DEFAULT_PORT 20000

#include "defines.h"
#include "reader.h"
#include "writer.h"
#include "local_flow_control.h"
#include "network_flow_control_receiver.h"
#include "network_flow_control_sender.h"
#include "raw_checker.h"

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

struct parameters
{
    parameters() :
        read_name(""),
        write_name(""),
        data_length(1000000),
        packet_size(1000),
        baudrate(4000000),
        sport(XSTR(DEFAULT_PORT)),
        port(DEFAULT_PORT),
        recv_fc_over_network(false),
        send_fc_over_network(false)
    {
    }

    std::string read_name;
    std::string write_name;
    uint32 data_length;
    uint32 packet_size;
    uint32 baudrate;
    char *sport;
    short port;
    bool recv_fc_over_network;
    bool send_fc_over_network;
    char *send_name;

};

// Abstract away argument structure a bit
ichar get_opt(int argc, ichar **argv, int i)
{
    if (i < argc)
    {
        return argv[i][0];
    }
    else
    {
        return 0;
    }
}

ichar *get_arg(int argc, ichar **argv, int i)
{
    if (i < argc)
    {
        return argv[i] + 1;
    }
    else
    {
        return 0;
    }
} 

void process_args(int argc,
                  ichar **argv,
                  parameters &p)
{
    for (int i = 1; i < argc; ++i)
    {
        switch(get_opt(argc, argv, i))
        {
        case 'u':
            p.write_name = get_arg(argc, argv, i);
            break;
        case 'v':
            p.read_name = get_arg(argc, argv, i);
            break;
        case 'l':
            if (!get_uint32(get_arg(argc, argv, i), p.data_length))
            {
                iprintf(II("Ignoring %s\n"), argv[i]);
            }
            break;
        case 's':
            if (!get_uint32(get_arg(argc, argv, i), p.packet_size))
            {
                iprintf(II("Ignoring %s\n"), argv[i]) ;
            }
            break;
        case 'p':
            {
                char *got = 0;
                short port = istrtol(get_arg(argc, argv, i), &got, 0);
                if (got != get_arg(argc, argv, i) && got && *got == 0)
                {
                    p.sport = get_arg(argc, argv, i);
                    p.port = port;
                }
                else
                {
                    iprintf(II("Ignoring %s\n"), argv[i]);
                }
            }
            break;
        case 'n':
            {
                // get flow control tokens over the network on a port
                p.recv_fc_over_network = true;
            }
            break;
        case 'm':
            {
                // send flow control tokens to a network address.
                p.send_fc_over_network = true;
                p.send_name = get_arg(argc, argv, i);
            }
            break;
        default:
            iprintf(II("Ignoring %s\n"), argv[i]);
            break;
        }
    }
}

void open_uart(const std::string &name, UARTAbstraction *uart)
{
    if (!uart->open())
    {
        ifprintf(stderr, "Unable to open %s\n", name.c_str());
        delete uart;
        exit(EXIT_FAILURE);
    }
}

void create_uarts(const parameters &p,
                  UARTAbstraction **uw,
                  UARTAbstraction **ur)
{
    DataWatcher *dw(new RawChecker());
    if (p.write_name != "")
    {
        *uw = UARTConfiguration(p.write_name.c_str(),
				p.baudrate,
				UARTConfiguration::none,
				1,
				false,
				0).make();
        (*uw)->setDataWatch(dw);
        open_uart(p.write_name, *uw);
    }

    if (p.read_name != "")
    {
        if (p.read_name != p.write_name)
        {
            *ur = UARTConfiguration(p.read_name.c_str(),
				    p.baudrate,
				    UARTConfiguration::none,
				    1,
				    false,
				    0).make();
            (*ur)->setDataWatch(dw);
            open_uart(p.read_name, *ur);
        }
        else
        {
            *ur = *uw;
        }
    }
}

void clean_uarts(UARTAbstraction *w, UARTAbstraction *r)
{
    delete w;

    if (w != r)
    {
        delete r;
    }
}

int imain(int argc, ichar **argv)
{
    parameters p;
    process_args(argc, argv, p);
    UARTAbstraction *uw = 0;
    UARTAbstraction *ur = 0;
    create_uarts(p, &uw, &ur);

    Writer *writer(0);

    LocalFlowControl localFlowControl;
    NetworkFlowControlReceiver *nr(0);
    NetworkFlowControlSender *ns(0);
    
    FlowControlReceiver *r(0);
    FlowControlSender *s(0);

    if (p.recv_fc_over_network)
    {
        nr = new NetworkFlowControlReceiver(p.port, localFlowControl);
        if (!nr->isConnected())
        {
            printf("poop\n");
        }
        else
        {
            nr->Start();
        }
        r = nr;
    }
    else
    {
        r = &localFlowControl;
    }

    if (p.send_fc_over_network)
    {
        ns = new NetworkFlowControlSender(p.send_name, p.sport);
        s = ns;
    }
    else
    {
        s = &localFlowControl;
    }

    if (uw)
    {
        
        writer = new Writer(uw, p.data_length, p.packet_size, *r);
        writer->Start();
    }

    Reader *reader(0);

    if (ur)
    {
        reader = new Reader(ur, p.data_length, p.packet_size, *s);
        reader->Start();
    }

    if (writer)
    {
        writer->WaitForStop(0);
    }

    if (reader)
    {
        reader->WaitForStop(0);
    }

    clean_uarts(uw, ur);
    uw = 0;
    ur = 0;

    delete nr;
    nr = 0;
    delete ns;
    ns = 0;

    return 0;
}
