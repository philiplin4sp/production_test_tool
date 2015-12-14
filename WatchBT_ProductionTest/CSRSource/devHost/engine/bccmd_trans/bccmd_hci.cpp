// bccmd_hci.cpp
//
/*
 MODIFICATION HISTORY
        1.1   13:feb:01  Chris Lowe     Working
        1.4   07:mar:01  Chris Lowe     Reset closes transport, 
                                        closing transport resets.
        1.7   11:apr:01  Chris Lowe     Comments

        $Id: //depot/bc/bluesuite_2_4/devHost/engine/bccmd_trans/bccmd_hci.cpp#1 $
*/



#include "bccmd_hci.h"
// $Id: //depot/bc/bluesuite_2_4/devHost/engine/bccmd_trans/bccmd_hci.cpp#1 $

#include "csrhci/transportconfiguration.h"

#include "time/hi_res_clock.h"
#include "thread/system_wide_mutex.h"

// local class
class bccmd_transaction
{
public:
    class return_vector : public BlueCoreDeviceController_newStyle::PDUCallBack
    {
    public:
        return_vector ( SignalBox::Item& a , BCCMD_PDU*& r)
        : x(a), resp(r) {}
        void operator() ( const PDU & p)
        {
            resp = new BCCMD_PDU(p);
            x.set();
        }
        SignalBox::Item& x;
        BCCMD_PDU* &resp;
    };
    bccmd_transaction( const BCCMD_PDU& p , BlueCoreDeviceController_newStyle&a, SignalBox& aBox) 
    : item(aBox), req(p), resp(0), cb(0)
    {
        cb = new return_vector(item,resp);
        a.send(BlueCoreDeviceController_newStyle::sae(req,cb));
        if ( !aBox.wait(5000) )
            a.purge_channel(PDU::bccmd);
//        if ( !getState() )
//           aBox.wait(5000);
    }
    ~bccmd_transaction()
    {
        delete resp;
    }
    bool ok() const
    {
		if (!resp)
			return false;
		if (resp->channel()  != PDU::bccmd)
			return false;
		uint16 req_type = resp->get_req_type();
		if ( req_type != BCCMDPDU_GETRESP
				&& req_type != BCCMDPDU_GETREQ )
			return false;
        if (req.get_varid()  != resp->get_varid())
			return false;
		if (req.get_seq_no() != resp->get_seq_no())
			return false;
		return true;
    }
    const BCCMD_PDU response()
    {
        if ( resp )
            return *resp;
        else
            return BCCMD_PDU(null_pdu);
    }
private:
	SignalBox::Item item;
    BCCMD_PDU req;
    BCCMD_PDU* resp;
    BlueCoreDeviceController_newStyle::CallBackPtr cb;
};
//******************************************************************************
// bccmd_hci_base
//******************************************************************************

bccmd_hci_base::bccmd_hci_base(bccmd_trans_error_handler *handler):
        bccmd_trans(handler),transport_died(pdu_completed)
{
	clear_transport_error();
}

bccmd_hci_base::~bccmd_hci_base()
{
}

void bccmd_hci_base::clear_transport_error()
{
    transport_error_condition_ = false;
    transport_died.unset();
}

bool bccmd_hci_base::reset_bc(reset_t reset_type)
{
    if (IsConnected())
    {
        switch (reset_type)
        {
        case RESET_WARM:
            dev_con()->forceReset(2000,BlueCoreDeviceController_newStyle::bccmd_warm);
            break;
        case RESET_COLD:
            dev_con()->forceReset(2000,BlueCoreDeviceController_newStyle::bccmd_cold);
            break;
        default:
            // do nothing
            break;
        }
    }
	HiResClockSleepMilliSec(1000);
    close();
	HiResClockSleepMilliSec(1000);
    return open();
}

bool bccmd_hci_base::reset_and_close(reset_t reset_type)
{
    if (IsConnected())
    {
        switch (reset_type)
        {
        case RESET_WARM:
            dev_con()->forceReset(2000,BlueCoreDeviceController_newStyle::bccmd_warm);
            break;
        case RESET_COLD:
            dev_con()->forceReset(2000,BlueCoreDeviceController_newStyle::bccmd_cold);
            break;
        default:
            // do nothing
            break;
        }
    }
	HiResClockSleepMilliSec(1000);
    return close();
}

bccmd_trans_error_response bccmd_hci_base::run_pdu(const BCCMD_PDU& p, BCCMD_PDU& resp)
{
	if(transport_error_condition_)
	{
        return  raise_error(II("The transport has failed.\n")
            II("You may need to reset the chip, and reconnect.\n")
            II("If problems persist, confirm the Host Interface, Baud rate and UART configuration settings, using an SPI transport if neccessary."),
            II("Transport failed"), BCCMD_TRANS_ERROR_OPERATION, BCCMD_TRANS_ERRTYPE_ABORT);
	}

    ASSERT(IsConnected());
    CriticalSection::Lock here(prot_sb);

    bccmd_transaction current(p,*(dev_con()),pdu_completed);

    if ( current.ok() )
    {
        resp = current.response();
        return BCCMD_TRANS_ERROR_NONE;
    }
    else if (transport_error_condition_)
        return  raise_error(II("The transport has failed.\n")
            II("You may need to reset the chip, and reconnect.\n")
            II("If problems persist, confirm the Host Interface, Baud rate and UART configuration settings, using an SPI transport if neccessary."),
            II("Transport failed"), BCCMD_TRANS_ERROR_OPERATION, BCCMD_TRANS_ERRTYPE_ABORT);
    else
        return raise_error(II("Timed out waiting for a response from the BlueCore chip.\n")
            II("You may need to reset the chip, and reconnect.\n")
            II("If problems persist, confirm the Host Interface, Baud rate and UART configuration settings, using an SPI transport if neccessary."),
            II("Communication error"), BCCMD_TRANS_ERROR_OPERATION, BCCMD_TRANS_ERRTYPE_ABORT);
}

bccmd_trans_error_response bccmd_hci_base::send_pdu(const BCCMD_PDU &p)
{
    ASSERT(IsConnected());
    dev_con()->send(p);
    return BCCMD_TRANS_ERROR_NONE;
}

bool bccmd_hci_base::requires_permissions() 
{
    return true;
}

int bccmd_hci_base::GetNumOperationAttempts() 
{
    return 1;
}

static char * getStr ( const uint8* data , uint32 len )
{
    char * str = new char[(len*3)+1];
    uint32 i = 0;
    for (;i!=len;++i)
        sprintf(str+(i*3)," %02x",data[i]);
    return str;
}

#ifdef PRINT_DATA
void bccmd_hci_base::onSend (const uint8* data, size_t len )
{
    FILE* f = fopen ( dwfilename , "a+" );
    char * s = getStr ( data , len );
    if(NULL == s) /* extra protection */
        s = "";
    if ( f )
    {
        fprintf (f , "--> 01%s\n" , s);
        fclose (f);
    }
    delete[] s;
}

void bccmd_hci_base::onRecv (const uint8* data, size_t len )
{
    FILE* f = fopen ( dwfilename , "a+" );
    char * s = getStr ( data , len );
    if(NULL == s) /* extra protection */
        s = "";
    if ( f )
    {
        fprintf (f , "<-- 04%s\n" , s);
        fclose (f);
    }
    delete[] s;
}
#endif
//******************************************************************************
// bccmd_hci
// Creates and owns a BlueCoreDeviceController.
//******************************************************************************

bccmd_hci::bccmd_hci(bccmd_trans_error_handler* handler, TransportConfiguration *trans)
: bccmd_hci_base(handler) , trans_(trans) , dev_con_ (0)
{ 
	ASSERT(trans);
}

bccmd_hci::~bccmd_hci()
{
	ASSERT(!IsConnected());
	delete trans_;
}

bool bccmd_hci::open(BlueCoreDeviceController_newStyle::CallBackProvider* hqPduHandler)
{
    if (IsConnected())
        return true;

    dev_con_ = new BlueCoreDeviceController_newStyle ( *trans_, *this );
    dev_con_->createWorker();
    if (! (dev_con_->startTransport() && dev_con_->isTransportReady(5000)) )
    {
        delete dev_con_;
        dev_con_ = 0;
		raise_error(II("HCI communications failed.\n")
			II("Failed to connect to the chip.\n")
			II("Please check the cable and power supply, then use Reconnect to retry. "),  
			II("Communication error"), BCCMD_TRANS_ERROR_CONNECTION, BCCMD_TRANS_ERRTYPE_ABORT);
    }
#ifdef PRINT_DATA
    else
        dev_con_->setDataWatch(this);
#endif
    return IsConnected();
}

bool bccmd_hci::close()
{
	OnClose();

	if (IsConnected())
	{
		delete dev_con_;
		dev_con_ = 0;

		clear_transport_error();
		return true;
	}
	return false;
}

void bccmd_hci::onPDU ( const PDU& pdu )
{
}

void bccmd_hci::onTransportFailure( FailureMode f )
{       
    transport_error_condition_ = true;
    transport_died.set();
}

bool bccmd_hci::IsConnected() const 
{
    return dev_con_ != 0;
}

BlueCoreDeviceController_newStyle* bccmd_hci::dev_con()
{
    return dev_con_;
}

static const unsigned long MAX_USB_DEVICES = 20;
#define CSR_USB 0
#define CSR_BTW 1

#define CSR_USB_FMT "\\\\.\\csr%d"
#define CSR_BTW_FMT "\\\\.\\BTWUSB-%d"

// Preprocessor magic to safely longify strings
#define LL(x)           _LL(x)
#define _LL(x)          L ## x

unsigned long bccmd_hci::USBDeviceIterate(unsigned long prev)
{
	// split the ID into device type and device number.
	unsigned long type = prev >> 24;
	unsigned long n = prev & 0x00FFFFFF;

	do
	{
		// increment to get the next sensible ID
		switch(type)
		{
#ifdef WIN32
		case CSR_USB:
		case CSR_BTW:
			n = (n + 1) & 0xFFFFFF;
			if(n <= MAX_USB_DEVICES) break;
			// else fall through and increment type
#endif
		default:
			type = (type + 1) & 0xFF;
			// we've changed type, so rewind device number to 0 again
			n = 0;
		}

		// try to open the device ID.
		switch(type)
		{
#ifdef WIN32
		case CSR_USB:
			{
				ichar portname[100];
				isprintf(portname, 100, II(CSR_USB_FMT), n);
				HANDLE test;
				if ( (test = ::CreateFile(portname,GENERIC_READ | GENERIC_WRITE,
							0,0,OPEN_EXISTING,0,0) ) != INVALID_HANDLE_VALUE )
				{
					// opened successfully. return this ID.
					::CloseHandle(test);
					return (type << 24) | n;
				}
			}
			break;
		case CSR_BTW:
			{
				ichar portname[100];
				isprintf(portname, 100, II(CSR_BTW_FMT), n);
				HANDLE test;
				SystemWideMutex mutex(&portname[4], true);
				if (mutex.IsAcquired())
				{
					if ( (test = ::CreateFile(portname,GENERIC_READ | GENERIC_WRITE,
								0,0,OPEN_EXISTING,0,0) ) != INVALID_HANDLE_VALUE )
					{
						// opened successfully. return this ID.
						::CloseHandle(test);
						return (type << 24) | n;
					}
				}
			}
#endif
		}
		// loop until we've exhausted all types.
	} while(type != 0xFF);
	return 0xFFFFFFFF;
}

size_t bccmd_hci::USBDeviceName(unsigned long dev, char *buf, size_t buflen)
{
	switch(dev >> 24)
	{
#ifdef WIN32
	case 0:
		return _snprintf(buf, buflen, CSR_USB_FMT, dev & 0xFFFFFF);
	case 1:
		return _snprintf(buf, buflen, CSR_BTW_FMT, dev & 0xFFFFFF);
#endif
	}
	return 0;
}

size_t bccmd_hci::USBDeviceName(unsigned long dev, wchar_t *buf, size_t buflen)
{
	switch(dev >> 24)
	{
#ifdef WIN32
	case 0:
		return _snwprintf(buf, buflen, LL(CSR_USB_FMT), dev & 0xFFFFFF);
	case 1:
		return _snwprintf(buf, buflen, LL(CSR_BTW_FMT), dev & 0xFFFFFF);
#endif
	}
	return 0;    
}

///////////////////////////////////////////////////////////////////////////////
//  bccmd_hci_ref
//  Refers to a BlueCoreDeviceController. Does not create or own one.
///////////////////////////////////////////////////////////////////////////////

bccmd_hci_ref::bccmd_hci_ref(bccmd_trans_error_handler *handler, BlueCoreDeviceController_newStyle *devCon)
: bccmd_hci_base(handler), dev_con_ (devCon)
{
}

bccmd_hci_ref::~bccmd_hci_ref(void)
{
}

void bccmd_hci_ref::set_dev_con(BlueCoreDeviceController_newStyle *devCon)
{
    dev_con_ = devCon;
}

bool bccmd_hci_ref::open(BlueCoreDeviceController_newStyle::CallBackProvider* hqPduHandler)
{
    return IsConnected();
}

bool bccmd_hci_ref::close(void)
{
    OnClose();

    if(IsConnected())
    {
        dev_con_ = 0;
        return true;
    }

    return false;
}

bool bccmd_hci_ref::IsConnected() const 
{
    return dev_con_ != 0;
}

BlueCoreDeviceController_newStyle* bccmd_hci_ref::dev_con()
{
    return dev_con_;
}
