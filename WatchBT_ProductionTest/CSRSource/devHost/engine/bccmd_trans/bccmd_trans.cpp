// bccmd_trans.cpp 
//

// This file contains the BCCMD functions pertaining to the persistent store.  
// They rely on implementations of run_pdu and send_pdu.

/*
 MODIFICATION HISTORY
        1.1   13:feb:01  Chris Lowe     Working
        1.2   23:feb:01  Chris Lowe     Now supports firmware versions.
        1.3   07:mar:01  Chris Lowe     Reset closes transport, 
                                        closing transport resets.
        1.4   28:mar:01  Chris Lowe     #inc's changed for Result dir
        1.5 04:Jul:01  Chris Lowe   Uses Stores instead of Dummy.
        #12  06:Oct:04 Simon Walker   Added extra check to run_pdu_report_errs.
        
        $Id: //depot/bc/bluesuite_2_4/devHost/engine/bccmd_trans/bccmd_trans.cpp#1 $
*/

#include "bccmd_trans.h"
#include "bccmd_spi.h"
#include "bccmd_hci.h"
#include "csrhci/transportconfiguration.h"
#include "spi/spi_common.h"  // for chip identifier constants.
#include "chiputil/xaphelper.h"
#include "chiputil/sluttable.h"
#include <malloc.h>
#include "spi/spi_common.h"
#include "bccmdtransdebugsentry.h"
#include "bccmdtransdebugsentrywithreturn.h"

#ifdef _WIN32_WCE
#ifndef MB_TASKMODAL
#define MB_TASKMODAL 0
#endif
#endif

const ichar* UNKNOWN_ERR_MSG = II("The chip reported an unknown error.");
const ichar* NO_SUCH_VARID_ERR_MSG = II("The chip reports 'No such BCCMD VarID'");

static char *findvariable(const char *trans, const char *var)
{
    char *value;
	bool finished = false;
	char *mytrans = STRDUP(trans);
	try
	{
		char *v, *end = mytrans - 1;

		while (!finished)
		{
			v = ++end;

			// Find equals dividing viable and value.
			while (*end != '=' && *end != 0)
				end++;

			if (*end != 0) *end = 0;
			else           break;

			value = ++end;

			// Find end of value.
			while (*end != ' ' && *end != 0)
				end++;

			if (*end != '\0') *end = '\0';
			else finished = true;

			if(STRICMP(v, var) == 0)
			{
				char *r = (char*)malloc(strlen(value) + 1);
				if (r != NULL)
				{
					// Take copy of the value.
					strcpy(r, value);
					return r;
				}
			}
		}
	}
	catch(...)
	{
		free(mytrans);
		throw;
	}
	free(mytrans);

#if !defined(_WINCE) & !defined(_WIN32_WCE)
    // If we still don't have a value get the environment variable.
    if((value = getenv(var))) return STRDUP(value);
#endif

    return NULL;
}

static const char TRANS_VAR_NAME[] = "CSRTRANS";

static const char TRANS_SPI[] = "SPI";
static const char TRANS_BCSP[] = "BCSP";
static const char TRANS_H4[] = "H4";
static const char TRANS_H5[] = "H5";
static const char TRANS_USB[] = "USB";

static const char HCIPORT_VAR_NAME[] = "HCIPORT";
static const char HCIBAUD_VAR_NAME[] = "HCIBAUD";

bccmd_trans *bccmd_trans::create_transport(const char *trans, bccmd_trans_error_handler *error_handler, int mul)
{
	static const char DEFAULT_BCSP_PORT[] = "com1";
	static const char DEFAULT_H4_PORT[] = "com1";
	static const char DEFAULT_H5_PORT[] = "com1";

	static const uint32 DEFAULT_BCSP_BAUD = 115200;
	static const uint32 DEFAULT_H4_BAUD = 115200;
	static const uint32 DEFAULT_H5_BAUD = 115200;

	bccmd_trans *r;
	char *trans_var = findvariable(trans, TRANS_VAR_NAME);
	try
	{
		if(!trans_var) trans_var = STRDUP(TRANS_SPI);
		if(STRICMP(trans_var, TRANS_SPI)==0)
		{
			r = new bccmd_spi(error_handler, trans, -1, mul);
		}
		else if(STRICMP(trans_var, TRANS_BCSP)==0)
		{
			char *port = findvariable(trans, HCIPORT_VAR_NAME);
			if(!port) port = STRDUP(DEFAULT_BCSP_PORT);
			try
			{
#ifdef WIN32
				if(port[0] != '\\')
				{
					char *port2 = (char *) malloc(strlen(port) + 5);
					sprintf(port2, "\\\\.\\%s", port);
					free(port);
					port = port2;
				}
#endif
				char *baudstr = findvariable(trans, HCIBAUD_VAR_NAME);
				uint32 baud = 0;
				if(baudstr) baud = atoi(baudstr);
				free(baudstr);
				if(baud == 0) baud = DEFAULT_BCSP_BAUD;
				r = new bccmd_hci(error_handler, new BCSPConfiguration(UARTConfiguration(port, baud, BCSP_parity , BCSP_stopbits , true , BCSP_readtimeout)));
			}
			catch(...)
			{
				free(port);
				throw;
			}
			free(port);
		}
		else if(STRICMP(trans_var, TRANS_H4)==0)
		{
			char *port = findvariable(trans, HCIPORT_VAR_NAME);
			if(!port) port = STRDUP(DEFAULT_H4_PORT);
			try
			{
#ifdef WIN32
				if(port[0] != '\\')
				{
					char *port2 = (char *) malloc(strlen(port) + 5);
					sprintf(port2, "\\\\.\\%s", port);
					free(port);
					port = port2;
				}
#endif
				char *baudstr = findvariable(trans, HCIBAUD_VAR_NAME);
				uint32 baud = 0;
				if(baudstr) baud = atoi(baudstr);
				free(baudstr);
				if(baud == 0) baud = DEFAULT_H4_BAUD;
				r = new bccmd_hci(error_handler, new H4Configuration(port, baud));
			}
			catch(...)
			{
				free(port);
				throw;
			}
			free(port);
		}
		else if(STRICMP(trans_var, TRANS_H5)==0)
		{
			char *port = findvariable(trans, HCIPORT_VAR_NAME);
			if(!port) port = STRDUP(DEFAULT_H5_PORT);
			try
			{
#ifdef WIN32
				if(port[0] != '\\')
				{
					char *port2 = (char *) malloc(strlen(port) + 5);
					sprintf(port2, "\\\\.\\%s", port);
					free(port);
					port = port2;
				}
#endif
				char *baudstr = findvariable(trans, HCIBAUD_VAR_NAME);
				uint32 baud = 0;
				if(baudstr) baud = atoi(baudstr);
				free(baudstr);
				if(baud == 0) baud = DEFAULT_H5_BAUD;
				r = new bccmd_hci(error_handler, new H5Configuration(port, baud));
			}
			catch(...)
			{
				free(port);
				throw;
			}
			free(port);
		}
#ifdef WIN32
#ifndef _WIN32_WCE
		else if(STRICMP(trans_var, TRANS_USB)==0)
		{
			char *port = findvariable(trans, HCIPORT_VAR_NAME);
			if(!port)
			{
				ichar portname[100];
				unsigned long dev = bccmd_hci::USBDeviceIterate();
				if(bccmd_hci::USBDeviceName(dev, portname, sizeof(portname)/sizeof(*portname)) != 0)
				{
					port = STRDUP(inarrow(portname).c_str());
				}
				else
				{
					error_handler->on_error(II("unable to find any HCI USB devices"), II("Transport Error"), BCCMD_TRANS_ERROR_CONNECTION, BCCMD_TRANS_ERRTYPE_ABORT);
				}
			}
			if(port && port[0] != '\\')
			{
				char *port2 = (char *) malloc(strlen(port) + 5);
				sprintf(port2, "\\\\.\\%s", port);
				free(port);
				port = port2;
			}
			if(port)
			{
				try
				{
					r = new bccmd_hci(error_handler, new H2Configuration(USBConfiguration(port)));
				}
				catch(...)
				{
					free(port);
					throw;
				}
				free(port);
			}
			else
			{
				r = NULL;
			}
		}
#endif
#endif
		else
		{
			error_handler->on_error(II("unknown transport"), II("Transport Error"), BCCMD_TRANS_ERROR_CONNECTION, BCCMD_TRANS_ERRTYPE_ABORT);
			r = NULL;
		}
	}
	catch(...)
	{
		free(trans_var);
		throw;
	}
	free(trans_var);
	return r;
}

// returns suggestions for the next variable
bccmd_trans::trans_suggestion *bccmd_trans::suggest_var(const char *trans)
{
	trans_suggestion *r = new trans_suggestion;
	r->value = STRDUP("");
	r->description = NULL;
	return r;
}

void bccmd_trans::suggest_var_free(trans_suggestion *mem)
{
	free(mem->value);
	free(mem->description);
	delete mem;
}

// returns suggestions for values for a given variable
bccmd_trans::trans_suggestion **bccmd_trans::suggest_value(const char *trans, const char *var)
{
	trans_suggestion **r = NULL;
	if(STRICMP(var, TRANS_VAR_NAME)==0)
	{
		r = new trans_suggestion*[6];
		r[0] = new trans_suggestion;
		r[0]->value = STRDUP(TRANS_SPI);
		r[0]->description = NULL;
		r[1] = new trans_suggestion;
		r[1]->value = STRDUP(TRANS_BCSP);
		r[1]->description = NULL;
		r[2] = new trans_suggestion;
		r[2]->value = STRDUP(TRANS_H4);
		r[2]->description = NULL;
		r[3] = new trans_suggestion;
		r[3]->value = STRDUP(TRANS_H5);
		r[3]->description = NULL;
		r[4] = new trans_suggestion;
		r[4]->value = STRDUP(TRANS_USB);
		r[4]->description = NULL;
		r[5] = NULL;
	}
	else
	{
		char *trans_var = findvariable(trans, TRANS_VAR_NAME);
		try
		{
			if(!trans_var) trans_var = STRDUP(TRANS_SPI);
			if(STRICMP(trans_var, TRANS_SPI)==0)
			{
				pttrans_device *devices;
				int devcount;
				if(pttrans_enumerate(NULL, &devices, &devcount) == PTTRANS_SUCCESS)
				{
					struct transport
					{
						char *name;
						transport *next;
					} *transports = NULL;
					int transcount = 0;
					try
					{
						if(STRICMP(var, "SPITRANS")==0)
						{
							for(int i = 0; i < devcount; ++i)
							{
								transport **p = &transports;
								while(*p && STRICMP(devices[i].transport, (*p)->name) != 0) p = &(*p)->next;
								if(!*p)
								{
									*p = new transport;
									(*p)->name = STRDUP(devices[i].transport);
									(*p)->next = NULL;
									++transcount;
								}
							}
							r = new trans_suggestion*[transcount+1];
							int j = 0;
							for(transport *t = transports; t; t = t->next)
							{
								r[j] = new trans_suggestion;
								r[j]->value = STRDUP(t->name);
								r[j]->description = NULL;
								++j;
							}
							r[j] = NULL;
						}
						else if(STRICMP(var, SPIPORT)==0)
						{
							char *spitrans_var = findvariable(trans, "SPITRANS");
							if(spitrans_var)
							{
								try
								{
									int mydevcount = 0;
									int i;
									for(i = 0; i < devcount; ++i) if(STRICMP(devices[i].transport, spitrans_var)==0) ++mydevcount;
									r = new trans_suggestion*[mydevcount+1];
									int j = 0;
									for(i = 0; i < devcount; ++i)
									{
										if(STRICMP(devices[i].transport, spitrans_var)==0)
										{
											char tmp[64];
											SNPRINTF(tmp, sizeof(tmp), "%d", devices[i].port);
											r[j] = new trans_suggestion;
											r[j]->value = STRDUP(tmp);
											r[j]->description = STRDUP(devices[i].name);
											++j;
										}
									}
									r[j] = NULL;
								}
								catch(...)
								{
									free(spitrans_var);
									throw;
								}
								free(spitrans_var);
							}
						}
					}
					catch(...)
					{
						while(transports)
						{
							transport *t = transports;
							transports = t->next;
							free(t->name);
							delete t;
						}
						pttrans_enumerate_free(devices, devcount);
						throw;
					}
					while(transports)
					{
						transport *t = transports;
						transports = t->next;
						free(t->name);
						delete t;
					}
					pttrans_enumerate_free(devices, devcount);
				}
			}
#ifdef WIN32
#ifndef _WIN32_WCE
			else if(STRICMP(trans_var, TRANS_USB)==0)
			{
				if(STRICMP(var, HCIPORT_VAR_NAME)==0)
				{
					struct device
					{
						char *name;
						device *next;
					} *devices = NULL;
					int devcount = 0;
					try
					{
						device **pnext = &devices;
						char portname[100];
						for(unsigned long dev = bccmd_hci::USBDeviceIterate(); bccmd_hci::USBDeviceName(dev, portname, sizeof(portname)/sizeof(*portname)) != 0; dev = bccmd_hci::USBDeviceIterate(dev))
						{
							device *d = new device;
							d->name = STRDUP(portname);
							d->next = NULL;
							*pnext = d;
							pnext = &d->next;
							++devcount;
						}
						r = new trans_suggestion*[devcount+1];
						int i = 0;
						for(device *d = devices; d; d = d->next)
						{
							r[i] = new trans_suggestion;
							r[i]->value = d->name;
							d->name = NULL;
							r[i]->description = NULL;
							++i;
						}
						r[i] = NULL;
					}
					catch(...)
					{
						while(devices)
						{
							device *d = devices;
							devices = d->next;
							free(d->name);
							free(d);
						}
						throw;
					}
					while(devices)
					{
						device *d = devices;
						devices = d->next;
						free(d->name);
						free(d);
					}
				}
			}
#endif
#endif
		}
		catch(...)
		{
			free(trans_var);
			throw;
		}
		free(trans_var);
	}
	if(!r)
	{
		r = new trans_suggestion*[1];
		r[0] = NULL;
	}
	return r;
}

void bccmd_trans::suggest_value_free(trans_suggestion **mem)
{
	for(trans_suggestion **p = mem; *p; ++p)
	{
		free((*p)->value);
		free((*p)->description);
		delete *p;
	}
	delete [] mem;
}

// Error handlers
//

bccmd_trans_exception::bccmd_trans_exception(const ichar *error_str, const ichar *caption_str, const int code,const bccmd_trans_error_type error_type): 
	m_code(code), 
	m_type(error_type) 
{ 
	if (error_str) 
		bccmd_trans_exception::error_str_ = istrdup(error_str);
	else
		bccmd_trans_exception::error_str_ = 0;
	if (caption_str) 
		bccmd_trans_exception::caption_str_ = istrdup(caption_str); 
	else
		bccmd_trans_exception::caption_str_ = 0;
}

bccmd_trans_exception::~bccmd_trans_exception()
{
	free(error_str_); free(caption_str_);
}

const ichar *bccmd_trans_exception::error_str() 
{
	return error_str_;
}

const ichar *bccmd_trans_exception::caption_str() 
{
	return caption_str_;
}

bccmd_trans_error_handler::bccmd_trans_error_handler() : 
    last_error_string_(NULL)
{
    clear_errors();
}

const int bccmd_trans_error_handler::get_last_error_code() const 
{
	return last_error_code_;
}

const ichar* bccmd_trans_error_handler::get_last_error_string() const
{
	return last_error_string_;
}

bccmd_trans_error_type bccmd_trans_error_handler::get_last_error_type() const
{
    return last_error_type_;
}

bccmd_trans_error_response bccmd_trans_error_handler::get_last_response() const
{
    return last_response_;
}

bccmd_trans_error_response bccmd_trans_error_handler::get_overall_response() const
{
    return overall_response_;
}

void bccmd_trans_error_handler::clear_errors()
{
	free(last_error_string_); 
	last_error_string_ = NULL; 
	last_error_code_ = BCCMD_TRANS_ERROR_NONE;
	last_response_ = BCCMD_TRANS_ERROR_NONE;
	overall_response_ = BCCMD_TRANS_ERROR_NONE;
}

void bccmd_trans_error_handler::store_last_error(const ichar *error_string, const int error_code, bccmd_trans_error_type error_type) 
{
	free(last_error_string_);
	last_error_string_ = istrdup(error_string);
	last_error_code_ = error_code;
	last_error_type_ = error_type;
}

//*******************************************
// update the overall response if this response is more important.
//*******************************************
void bccmd_trans_error_handler::add_to_overall_response(bccmd_trans_error_response resp)
{
    static const unsigned int priorities[] = { 0, 3, 1, 0, 2 };

    assert(overall_response_ >= 0 && overall_response_ < sizeof(priorities)/sizeof(*priorities));
    assert(resp >= 0 && resp < sizeof(priorities)/sizeof(*priorities));

    if (overall_response_ >= 0 && overall_response_ < sizeof(priorities)/sizeof(*priorities)
        && resp >= 0 && resp < sizeof(priorities)/sizeof(*priorities))
    {
        if (priorities[resp] >= priorities[overall_response_])
        {
            overall_response_ = resp;
        }
    }
}

bccmd_trans_error_handler::~bccmd_trans_error_handler() 
{
	free(last_error_string_);
}

#ifdef WIN32
bccmd_trans_error_response bccmd_trans_win_mbox_error_handler::on_error_extra(const ichar *error_string, const ichar * caption_string, const int error_code, const bccmd_trans_error_type error_type)
{
	switch (error_type)
	{
	case BCCMD_TRANS_ERRTYPE_ARI:
		{
   			int result = MessageBox(parent_hwnd_, error_string,	caption_string, BCCMD_TRANS_ERRTYPE_ARI | MB_ICONWARNING | MB_TASKMODAL);
			switch (result)
			{
			case IDABORT:
				return BCCMD_TRANS_ERROR_ABORT;
			case IDRETRY:
				return BCCMD_TRANS_ERROR_RETRY;
			case IDIGNORE:
				return BCCMD_TRANS_ERROR_IGNORE;
			default:
				ASSERT(0);
			}
		}
		break;
	case BCCMD_TRANS_ERRTYPE_OK_CANCEL:
		{
   			int result = MessageBox(parent_hwnd_, error_string,	caption_string, BCCMD_TRANS_ERRTYPE_OK_CANCEL | MB_ICONWARNING| MB_TASKMODAL);
			switch (result)
			{
			case IDOK:
				return BCCMD_TRANS_ERROR_ABORT;
			case IDCANCEL:
				return BCCMD_TRANS_ERROR_IGNORE;
			default:
				ASSERT(0);
			}
		}
		break;
	case BCCMD_TRANS_ERRTYPE_MESSAGE:
		// MessageBox(parent_hwnd_, error_string,	caption_string, BCCMD_TRANS_ERRTYPE_ABORT | MB_TASKMODAL);
		return BCCMD_TRANS_ERROR_CONTINUE;
		break;
	default:
		MessageBox(parent_hwnd_, error_string,	caption_string, BCCMD_TRANS_ERRTYPE_ABORT | MB_ICONSTOP | MB_TASKMODAL);
		return BCCMD_TRANS_ERROR_ABORT;
		break;
	}	
	ASSERT(0);
	return BCCMD_TRANS_ERROR_ABORT;
}
#endif

bccmd_trans_error_response bccmd_trans_throw_error_handler::on_error_extra(const ichar *error_string, const ichar * caption, const int error_code, const bccmd_trans_error_type error_type)
{
	if (error_type == BCCMD_TRANS_ERRTYPE_MESSAGE)
	{
		return BCCMD_TRANS_ERROR_CONTINUE;
	}
	else
	{    
		throw new bccmd_trans_exception(error_string, caption, error_code, error_type);
		// never reaches:
		return BCCMD_TRANS_ERROR_ABORT;
	}
}

bccmd_trans::bccmd_trans(bccmd_trans_error_handler *handler)
{
    mSecurityEnabled = SECURITY_UNKNOWN;
    set_error_handler(handler);
}

bccmd_trans::~bccmd_trans() 
{
}

//****************************************
// Do anything that should be done when we close a connection, such as invalidating
// cached data.
//****************************************
void bccmd_trans::OnClose()
{
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY;
    mSecurityEnabled = SECURITY_UNKNOWN;
    mp_slut.reset();
    mp_xap_helper.reset();
}

void bccmd_trans::set_error_handler(bccmd_trans_error_handler *error_handler)
{
	error_handler_ = error_handler ? error_handler : &m_default_error_handler;
}

bccmd_trans_error_handler *bccmd_trans::get_error_handler(void)
{
	return error_handler_;
}

bccmd_trans_error_response bccmd_trans::raise_error(const ichar *error_string, const ichar * caption, const int error_code, const bccmd_trans_error_type error_type)
{
	BCCMDTRANS_FUNCTION_DEBUG_SENTRY;
	ASSERT(get_error_handler());
	return get_error_handler()->on_error(error_string, caption, error_code, error_type);
}

#ifdef WIN32
bccmd_trans_win_mbox_error_handler::bccmd_trans_win_mbox_error_handler(HWND parent_hwnd) :
		bccmd_trans_error_handler()
{
	parent_hwnd_ = parent_hwnd;
}
#endif

//****************************************
// Try to determine whether security is enabled by doing a memget bccmd.
//****************************************
bccmd_trans::SecurityStateEnum bccmd_trans::detectSecurity()
{
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY;
    if (IsConnected())
    {
        if (mSecurityEnabled == SECURITY_UNKNOWN)
        {
            static const uint16 test_address = 0xFF9A;
            BCCMD_MEMORY_PDU p;
            p.set_req_type(BCCMDPDU_GETREQ);
            p.set_base_addr(test_address);
            p.set_len(1);
            p.set_length();
            BCCMD_MEMORY_PDU rv (run_pdu_report_errs(p));
            if ( rv.channel() != PDU::zero && rv.get_req_type() == BCCMDPDU_GETRESP && 
                 rv.get_status() == BCCMDPDU_STAT_OK )
            {
                mSecurityEnabled = SECURITY_DISABLED;
            }
            else if (rv.channel() != PDU::zero && rv.get_req_type() == BCCMDPDU_GETRESP && 
                 rv.get_status() == BCCMDPDU_STAT_NO_ACCESS )
            {
                mSecurityEnabled = SECURITY_ENABLED;
            }
        }
    }
    return mSecurityEnabled;
}

bool bccmd_trans::get_firmware_id(int *const id, uint16 *nameBuffer, const size_t sNameBuffer)
{
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY;
    if (IsConnected())
    {
        BCCMD_BUILDID_PDU p;
        p.set_req_type(BCCMDPDU_GETREQ);
        p.set_length();
        BCCMD_BUILDID_PDU rv (run_pdu_report_errs(p));
        *nameBuffer = II('\0');
        if ( rv.channel() != PDU::zero && rv.get_req_type() == BCCMDPDU_GETRESP && 
             rv.get_status() == BCCMDPDU_STAT_OK )
        {
            *id = rv.get_data();
            return true;
        }
    }
    return false;
}

bool bccmd_trans::read_mem(GenericPointer addr, uint16 len, uint16 * data) 
{
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY;
    // Can only read from raw data here
    if(addr.Mem() != Mem_RawData)
    {
        return false;
    }

    if (IsConnected())
    {
	    ASSERT(len > 0 && len <= 64);

        BCCMD_MEMORY_PDU p;
        p.set_req_type(BCCMDPDU_GETREQ);
	    p.set_base_addr(addr);
	    p.set_len(len);
        p.set_length();
        BCCMD_MEMORY_PDU rv (run_pdu_report_errs(p));
        if ( rv.channel() != PDU::zero && rv.get_req_type() == BCCMDPDU_GETRESP && 
             rv.get_status() == BCCMDPDU_STAT_OK )
        {
            rv.get_mem(data,len);
            return true;
        }
    }
    return false;
}

//==============================================================================
// Get or create a CXapHelper object.
//
// This version always returns the SlutTable owned by the bccmd_trans object.
// It will create a new one if we don't have one already, or if force_reread is true.
//==============================================================================
bool                                                    // returns whether an object was successfully obtained.
bccmd_trans::get_xap_helper(CXapHelper *&apXapHelper,   // [out] this is set to point to the object
                            bool force_reread)          // [in] if true then discard a cached version and always make a new one
{
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY;
    if(!mp_xap_helper.get() || force_reread)
    {
        mp_xap_helper.reset(new CXapHelper);

        // as this is bccmd_trans we're only interested in bluecores, so no need to check BFT_IDENT
        uint16 reg_ff9a, reg_fe81;
        if (read_mem(GenericPointer(Mem_RawData, GBL_CHIP_VERSION_GEN1_ADDR), 1, &reg_ff9a))
        {
            mp_xap_helper->SetFF9A(reg_ff9a);

            // either it's a pre-bc7 or we successfully read fe81 and give it to xaphelper.
            if (mp_xap_helper->IsPreBc7() ||
                 (read_mem(GenericPointer(Mem_RawData, GBL_CHIP_VERSION_GEN2_ADDR), 1, &reg_fe81) &&
                   (mp_xap_helper->SetFE81(reg_fe81), true)))
            {
                assert(mp_xap_helper->IsReadyForUse());
            }
        }
    }
    apXapHelper = mp_xap_helper.get();
    return apXapHelper != NULL;
}

//**********************************************
// Get or create a SlutTable object.
//
// This function always returns the SlutTable owned by the bccmd_trans object.
// It will create a new one if we don't have one already, or if force_reread is true.
//**********************************************
bool                                          // returns whether an object was successfully obtained.
bccmd_trans::get_slut(SlutTable *&apSlut,     // [out] this is set to point to the object
                      bool force_reread)      // [in] if true then discard a cached version and always make a new one
{
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY;
    if(!mp_slut.get() || force_reread)
    {
        CXapHelper *p_xap_helper;
        if(get_xap_helper(p_xap_helper, force_reread))
        {
            mp_slut.reset(new SlutTable(this, p_xap_helper));
        }
    }
    apSlut = mp_slut.get();
    return apSlut != NULL;
}

//******************************************************************************
// run_pdu_report_errs - Write the given PDU to the device, reporting errors.
//
// RETURNS
//    Returns true if a valid GET-RESPONSE PDU is received, otherwise false.
//
bool bccmd_trans::run_pdu_report_errs(BCCMDPDU*p)
{
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY;
    BCCMD_PDU req ( (uint8*)p , p->pdulen*BCCMD_PDU::byte_word_size_ratio );
    BCCMD_PDU resp ( run_pdu_report_errs (req) );
    if ( resp.channel() )
    {
        if(resp.get_req_type() == BCCMDPDU_GETRESP)
        {
            memcpy (p,resp.data(),resp.size());
            return true;
        }
    }
    return false;
}

bccmd_trans_error_response bccmd_trans::do_request(const BCCMD_PDU&p, BCCMD_PDU &resp, bccmd_trans_error_handler *error_handler)
{
    bccmd_trans_error_response cmd_was_run;
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY_R(bccmd_trans_error_response, cmd_was_run);

    for (int i = GetNumOperationAttempts(); i--; )
    {
        if (error_handler)
        {
            error_handler->clear_errors();
        }
        cmd_was_run = run_pdu(p,resp);
        if ((cmd_was_run == BCCMD_TRANS_ERROR_NONE) && (resp.get_status() != BCCMDPDU_STAT_BAD_PDU))
            break; // it was run uncorrupted.
        if (cmd_was_run == BCCMD_TRANS_ERROR_ABORT)
            break; // it failed.
    }   
    if (cmd_was_run != BCCMD_TRANS_ERROR_NONE)
    {  
        // Only raise an error if there isn't one already
        if (!error_handler || error_handler->get_last_error_code() == 0)
        {
            raise_error(II("Failed to send or process command"), II("Communication error"), BCCMD_TRANS_ERROR_CONNECTION, BCCMD_TRANS_ERRTYPE_ABORT);
        }
    }
    return cmd_was_run;
}

bool bccmd_trans::check_status(BCCMD_PDU &resp)
{
    bool do_retry = false;
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY_R(bool, do_retry);

    // Check the object for error in the status work.
    Debug("check response\n");
    switch (resp.get_status())
    {
        case BCCMDPDU_STAT_OK:                  /* Default.  No problem found. */
            break;
        case BCCMDPDU_STAT_NO_SUCH_VARID:       /* varid not recognised. */
            do_retry = true;
            Debug("WARNING: Got \"No such BCCMD VarID\", retry\n");
            if (!m_error_message)
            {
                m_error_message = NO_SUCH_VARID_ERR_MSG;
            }
            break;
        case BCCMDPDU_STAT_TOO_BIG:             /* Data exceeded pdu/mem capacity. */
            raise_error(II("The chip reports 'BCCMD too big'."), II("Communication error"), BCCMD_TRANS_ERROR_OPERATION, BCCMD_TRANS_ERRTYPE_ABORT);
            break;
        case BCCMDPDU_STAT_NO_VALUE:            /* Variable has no value. */
            raise_error(II("The chip reports 'No value found'."), II("Communication error"), BCCMD_TRANS_ERROR_OPERATION, BCCMD_TRANS_ERRTYPE_ABORT);
            break;
        case BCCMDPDU_STAT_BAD_PDU:             /* Incorrect value(s) in PDU. */
            raise_error(II("The chip reports 'Bad PDU'."), II("Communication error"), BCCMD_TRANS_ERROR_CONNECTION, BCCMD_TRANS_ERRTYPE_ABORT);
            break;
        case BCCMDPDU_STAT_NO_ACCESS:           /* Value of varid inaccessible. */
            do_retry = true;
            raise_error(II("The chip reports 'Access denied'.\nKey may not exist for this version of the firmware."), II("Communication error"), BCCMD_TRANS_ERROR_OPERATION, BCCMD_TRANS_ERRTYPE_ABORT);
            break;
        case BCCMDPDU_STAT_READ_ONLY:           /* Value of varid unwritable. */
            raise_error(II("The chip reports 'Read only'."), II("Communication error"), BCCMD_TRANS_ERROR_PERMISSIONS, BCCMD_TRANS_ERRTYPE_ABORT);
            break;
        case BCCMDPDU_STAT_WRITE_ONLY:          /* Value of varid unreadable. */
            raise_error(II("The chip reports 'Write only'."), II("Communication error"), BCCMD_TRANS_ERROR_PERMISSIONS, BCCMD_TRANS_ERRTYPE_ABORT);
            break;
        case BCCMDPDU_STAT_ERROR:               /* The useless catch-all error. */
            if ( (
                     (resp.get_req_type() == BCCMDPDU_GETREQ) 
                  || (resp.get_req_type() == BCCMDPDU_GETRESP)
                 ) 
                && 
                 (
                     (resp.get_varid() == BCCMDVARID_PS) 
                  || (resp.get_varid() == BCCMDVARID_PSSIZE)  
                  || (resp.get_varid() == BCCMDVARID_PSCLR)
               ) ) 
            { // The key is not present
            }
            else
            {
                raise_error(II("The chip reported a problem processing a command."), II("Chip BCCMD error"), BCCMD_TRANS_ERROR_PERMISSIONS, BCCMD_TRANS_ERRTYPE_ABORT);
            }
            break;
        case BCCMDPDU_STAT_PERMISSION_DENIED:   /* Request not allowed */
            raise_error(II("The chip reports 'Permission denied'."), II("Communication error"), BCCMD_TRANS_ERROR_PERMISSIONS, BCCMD_TRANS_ERRTYPE_ABORT);
            break;
        default: 
            Debug("WARNING: Got \"Unknown error\", retry\n");
            if (!m_error_message)
            {
                m_error_message = UNKNOWN_ERR_MSG;
            }
            do_retry = true;
            break;
    }
    return do_retry;
}
//******************************************************************************
// run_pdu_report_errs - Write the given PDU to the device, reporting errors.
//
// RETURNS
//    The PDU received from the device.
//
BCCMD_PDU bccmd_trans::run_pdu_report_errs(const BCCMD_PDU&p)
{
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY;
    BCCMD_PDU resp(null_pdu);
    bccmd_trans_error_handler *error_handler = get_error_handler();
    int retries = 0;
    bool do_retry = false;

    m_error_message = NULL;

    // Due to the danger of repeating a set request (we don't know what actually happened) a retry will only be performed
    // on a "get"
    bool doing_getreq = (p.get_req_type() == BCCMDPDU_GETREQ);   
    bccmd_trans_error_response cmd_was_run;
    do
    {
        Debug("Send request\n");
        cmd_was_run = do_request(p, resp, error_handler);
        if (cmd_was_run == BCCMD_TRANS_ERROR_NONE)
        {
            do_retry = (check_status(resp) && (doing_getreq));
        }  
    }
    while ((do_retry) && (++retries < 3));

    // If a retry would still be required then a failure needs to be reported. Use the error string from the
    // first error
    if (cmd_was_run != BCCMD_TRANS_ERROR_NONE)
    {
        if (m_error_message)
        {
            raise_error(m_error_message, II("Communication error"), BCCMD_TRANS_ERROR_PERMISSIONS, BCCMD_TRANS_ERRTYPE_ABORT);
        }
    }

    return resp;       
}
