// bccmd_spi.cpp : Defines the entry point for the DLL application.
//

/* $Id: //depot/bc/bluesuite_2_4/devHost/engine/bccmd_trans/bccmd_spi.cpp#3 $ */

#include "common/portability.h"
#include "bccmd_spi.h"
#include <limits.h>

// Only pack the header files from the stack
#include "host/io/io_digital.h"
#include "host/slut/slutids.h"
#include "time/hi_res_clock.h"
#include "time/stop_watch.h"
#include <malloc.h>
#include <stdexcept>
#include "thread/system_wide_mutex.h"
#include "pttransport/io_map_public_bc.h"
#include "pttransport/pttransport.h"
#include "host/slut/slutids.h"
#include "chiputil/sluttable.h"
#include "spi/spi_common.h"
#include "bccmdtransdebugsentry.h"
#include "bccmdtransdebugsentrywithreturn.h"

#pragma pack(push, header_files_bccmd, 2)
#include "host/bccmd/bccmdpdu.h"
#include "host/bccmd/bccmd_spi_common.h"
#pragma pack(pop, header_files_bccmd)

#define _MAX_ERROR_MSG_SIZE 255
#define _MAX_DEBUG_MSG_SIZE 255

enum {
    XAP_BRK_INDEX = XAP_RSVD_13
};

bccmd_spi::~bccmd_spi()
{
    if (IsConnected())
        close();
    
    ASSERT(!IsConnected());

    if (!PTTRANS_STREAMS_EQUAL(mPtStream, PTTRANS_STREAM_INVALID))
    {
        pttrans_stream_close(mPtStream);
    }
}


bccmd_spi::bccmd_spi(bccmd_trans_error_handler *handler, const char *trans,
                     int port, int multispi) 
                     : bccmd_trans(handler)
{
    connected_ = false;
    mPtStream = PTTRANS_STREAM_INVALID;
    hw_bps_enabled = 0;

    mSupportsHQ = false;
    mpHQThread = 0;
    mpHqPduHandler = 0;
	
	slutid_force_fast_clock_ = 0;
    slutid_hq_scraping_list_head_ = 0;

    std::string realTrans = trans;
    if (port != -1)
    {
        if (!realTrans.empty())
        {
            realTrans += " ";
        }
        realTrans += "SPIPORT=";
        char portstr[16];
        sprintf(portstr, "%d", port);
        realTrans += portstr;
    }
    if (multispi != -1)
    {
        if (!realTrans.empty())
        {
            realTrans += " ";
        }
        realTrans += "SPIMUL=";
        char mulstr[16];
        sprintf(mulstr, "%d", multispi);
        realTrans += mulstr;
    }

    if (pttrans_stream_open(&mPtStream, realTrans.c_str()) != PTTRANS_SUCCESS)
    {
        mPtStream = PTTRANS_STREAM_INVALID;
        wrap_raise_error("Pttransport failed to open",
                         II("Communications Error"),
                         BCCMD_TRANS_ERROR_CONNECTION,
                         BCCMD_TRANS_ERRTYPE_ARI);
    }

    // SPI always has access to memory, so security is effectively always disabled when using SPI.
    mSecurityEnabled = SECURITY_DISABLED;
}

//================================================================================================================
// Writes a value to the spi log
//================================================================================================================
void bccmd_spi::output_hex_value_to_spi_log(const char *message, uint16 value)
{
    Debug("%s = 0x%X\n", message, value);
}

//================================================================================================================
// Writes an error to the SPI log
//================================================================================================================
void bccmd_spi::output_error_to_spi_log(const char *default_message)
{
    if (get_error_handler()->get_last_error_code() != 0)
    {
        char *message = iinarrowdup(get_error_handler()->get_last_error_string());
        if (message)
        {
            Debug("%s\n", message);
        }
        delete message;
    }
    else
    {
        Debug("%s\n", default_message);
    }
}

//================================================================================================================
// Checks to see if pttransport has found an error and uses this in preference to the general one that would
// be raised
//================================================================================================================
bccmd_trans_error_response bccmd_spi::wrap_raise_error(const char *error_string, const ichar * caption, const int error_code, const bccmd_trans_error_type error_type)
{
	bccmd_trans_error_response err;
	BCCMDTRANS_FUNCTION_DEBUG_SENTRY_R(bccmd_trans_error_response, err);
	uint16 errAddr;
	istring istr = icoerce(error_string);
    const char *errStr;
	
    int last_error = pttrans_get_last_error(&errAddr, &errStr);
    
	if ((last_error != PTTRANS_ERROR_NO_ERROR) && (last_error != PTTRANS_ERROR_PLUGIN_FN))
	{
    	istr = icoerce(errStr);
    }
	err = raise_error((const ichar *)istr.c_str(),
					  II("Communications Error"),
					  BCCMD_TRANS_ERROR_CONNECTION,
					  BCCMD_TRANS_ERRTYPE_ARI);
    return err;
}

//================================================================================================================
// Read the slut
// Returns true if the slut was read successfully and passed all sanity checks
//================================================================================================================
bool bccmd_spi::read_slut()
{
    bool ret = false;
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY_R(bool, ret);

	slutid_id_string_location_ = 0;
	slutid_id_integer_location_ = 0;
	slutid_force_fast_clock_ = 0;
	slutid_uart_auto_baud_rate_ = 0;
	slutid_uart_auto_protocol_ = 0;
	slutid_spi_user_cmd_ = SPI_USER_CMD;
    slutid_hq_scraping_list_head_ = 0;

    const ichar *error_string, *caption;
    int error_code;

    SlutTable *p_slut_table;

    // Try and read the SLUT
    int tries = 0;
    do
    {
        if (!get_slut(p_slut_table, tries > 0))
        {
            error_string = II("The application was unable to determine the lookup table location on the chip.\n")
                           II("Please check the chip is running.  You may need to reset the chip, then\n")
                           II("reconnect.");
            caption = II("Error reading lookup table");
            error_code = BCCMD_TRANS_ERROR_OPERATION;
        }
        else if (!p_slut_table->read_slut(SlutType_Raw, tries > 0))
        {
            error_string = II("The application was unable to find a look-up table on the chip.\n")
                           II("Please check the chip is running.  You may need to reset the chip, then\n")
                           II("attempt to reconnect.");
            caption = II("Unable to find look-up table fingerprint.");
            error_code = BCCMD_TRANS_ERROR_CONNECTION;
        }
        else if (!p_slut_table->entry_exists(SLUTID_BCCMD_INTERFACE))
        {
            error_string = II("The application was unable to find entries in the look-up table on chip.\n")
                           II("Please use an alternative transport, update the firmware to a version that supports BCCMD/SPI, ")
                           II("or, if using a version of the firmware significantly newer than this software, update this software.");
            caption = II("Unable to find look-up table entry");
            error_code = BCCMD_TRANS_ERROR_CONNECTION;
        }
        else
        {
            ret = true;
        }
    } while(!ret && ++tries < 2);

    if (ret)
    {
        // Read the SLUT information we are interested in.
        bccmd_spi_interface_            = (uint16)p_slut_table->getValue(SLUTID_BCCMD_INTERFACE);
        output_hex_value_to_spi_log("bccmd_spi_interface          ", bccmd_spi_interface_);

        slutid_id_integer_location_     = (uint16)p_slut_table->getValue(SLUTID_ID_INTEGER);
        output_hex_value_to_spi_log("slutid_id_integer_location   ", slutid_id_integer_location_);

        slutid_spi_user_cmd_            = (uint16)p_slut_table->getValue(SLUTID_SPI_USER_CMD);
        output_hex_value_to_spi_log("slutid_spi_user_cmd          ", slutid_spi_user_cmd_);

        slutid_uart_auto_baud_rate_     = (uint16)p_slut_table->getValue(SLUTID_UART_AUTO_BAUD_RATE);
        output_hex_value_to_spi_log("slutid_uart_auto_baud_rate   ", slutid_uart_auto_baud_rate_);

        slutid_uart_auto_protocol_       = (uint16)p_slut_table->getValue(SLUTID_UART_AUTO_PROTOCOL);
        output_hex_value_to_spi_log("slutid_uart_auto_protocol    ", slutid_uart_auto_protocol_);

        slutid_force_fast_clock_        = (uint16)p_slut_table->getValue(SLUTID_FORCE_FAST_CLOCK);
        output_hex_value_to_spi_log("slutid_force_fast_clock      ", slutid_force_fast_clock_);

        slutid_hq_scraping_list_head_   = (uint16)p_slut_table->getValue(SLUTID_HQ_SCRAPING);
        output_hex_value_to_spi_log("slutid_hq_scraping_list_head ", slutid_hq_scraping_list_head_);

        slutid_id_string_location_      = (uint16)p_slut_table->getValue(SLUTID_ID_STRING_FULL);
        if (slutid_id_string_location_ == 0)
        {
            slutid_id_string_location_  = (uint16)p_slut_table->getValue(SLUTID_ID_STRING);
        }
        output_hex_value_to_spi_log("slutid_id_string_location    ", slutid_id_string_location_);
    }
    else
    {
        raise_error(error_string, caption, error_code, BCCMD_TRANS_ERRTYPE_ABORT);
    }

    return ret;
}

//================================================================================================================
// Slow the SPI clock down to the slowest speed and then read the slut.
// follows the return convention of read_slut.
//================================================================================================================
bool bccmd_spi::read_slut_slowly()
{
    bool ret;
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY_R(bool, ret);

    std::string prev_max_clock = pttrans_stream_getvar(mPtStream, SPIMAXCLOCK);
    std::string prev_clock = pttrans_stream_getvar(mPtStream, SPICLOCK);
    pttrans_stream_command(mPtStream, "SPISLOW");
    pttrans_stream_setvar(mPtStream, SPIMAXCLOCK, pttrans_stream_getvar(mPtStream, SPICLOCK));

    ret = read_slut();

    pttrans_stream_setvar(mPtStream, SPIMAXCLOCK, prev_max_clock.c_str());
    pttrans_stream_setvar(mPtStream, SPICLOCK, prev_clock.c_str());

    return ret;
}

//================================================================================================================
// Retry reads of the slut
//================================================================================================================
bool bccmd_spi::retry_slut()
{
    bool ret;
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY_R(bool, ret);
    int retries = 0;
    do
    {
        get_error_handler()->clear_errors();
        Debug("Retry slut read\n");
        ret = read_slut();
        if (!ret)
        {
            HiResClockSleepMilliSec(100);
        }
        retries++;
    }
    while ((!ret) && (retries < 10));

    return ret;
}

//================================================================================================================
// Setup chip to allow access to the SLUT
//================================================================================================================
bool bccmd_spi::setup_access_to_chip()
{
    bool ret = true;
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY_R(bool, ret);

    const uint16 MMU_SPI_GW1_CONFIG = 0xF8F9;
    const uint16 MMU_SPI_GW2_CONFIG = 0xF8FA;
    const uint16 MMU_BT_GW1_CONFIG = 0xF8DD;
    const uint16 MMU_BT_GW2_CONFIG = 0xF8DE;
    
    if (pttrans_stream_xap_helper(mPtStream).IsBc7OrLater())
    {
        // We're speaking to a chip with generic windows, set up GW1 so we can
        // access the stack's SLUT.
        uint16 gw1, gw2;
        bool spi_success = (spi_read(MMU_BT_GW1_CONFIG, gw1) && spi_read(MMU_BT_GW2_CONFIG, gw2) && spi_write(MMU_SPI_GW1_CONFIG, gw1) && spi_write(MMU_SPI_GW2_CONFIG, gw2));
        if (!spi_success)
        {
            raise_error(II("Unable to setup access to look-up table"), II("Unable to access look-up table."), BCCMD_TRANS_ERROR_CONNECTION, BCCMD_TRANS_ERRTYPE_ABORT);
            ret = false;
        }
    }
    return ret;
}


//================================================================================================================
// This function reads the addresses of the spi BCCMD variables from the SLUT,
//================================================================================================================
bool bccmd_spi::open(BlueCoreDeviceController_newStyle::CallBackProvider* hqPduHandler)
{
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY;

    if (PTTRANS_STREAMS_EQUAL(mPtStream, PTTRANS_STREAM_INVALID))
    {
        return false;
    }

    // Keep a copy of this for the internal call to open after a reset
    mpHqPduHandler = hqPduHandler;
	// Lock the SPI mutex.  This will be released on function return.
    TransAcquireStreamLock lock(mPtStream);

    // Start chip
    bool done_reset = false;
    pttrans_stream_xap_go(mPtStream);
    bool is_running = (pttrans_stream_xap_stopped(mPtStream) == PTTRANS_XAP_RUNNING);
    Debug("Check for running\n");
    if (!is_running)
    {
        // The chip isn't running. This has been seen to happen with an oddjob via a babel
        // because the writes to start the chip fail due to spi unreliability. Changing the writes
        // to "write_verify" make it work but concerns about the effect this would have on timing
        // mean the best we can do is to slow the clock right down and retry
        const char *save_clock = pttrans_stream_getvar(mPtStream, SPICLOCK);
        pttrans_stream_command(mPtStream, "SPISLOW");

        int retries = 0;    
        do
        {
            Debug("Not running after start - retry start\n");
            pttrans_stream_xap_go(mPtStream);
            is_running = (pttrans_stream_xap_stopped(mPtStream) == PTTRANS_XAP_RUNNING);
        }
        while (!is_running && ++retries < 5);

        // Restore clock
        pttrans_stream_setvar(mPtStream, SPICLOCK, save_clock);

	    if (pttrans_stream_xap_stopped(mPtStream) != PTTRANS_XAP_RUNNING)
	    {
	        // The chip is still stopped after a xap_go, so make the rather rash assumption that boot_prog is running (and halting).  A reset would be useful.
	        pttrans_stream_xap_reset_and_go(mPtStream);
            done_reset = true;
            if (pttrans_stream_xap_stopped(mPtStream) != PTTRANS_XAP_RUNNING)
            {
                raise_error(II("SPI communications failed.\n")
                            II("Failed to unpause the chip after reading the look-up table\n")
                            II("Please check the cable and power supply, then attempt to reconnect. "),  
                            II("Communication error"), BCCMD_TRANS_ERROR_CONNECTION, BCCMD_TRANS_ERRTYPE_ABORT);
                return false;
            }
        }
    }

    connected_ = setup_access_to_chip();

    if (IsConnected())
    {
        connected_ = read_slut();

        // if the first attempt didn't work, try again at slow speed.
        if(!IsConnected())
        {
            get_error_handler()->clear_errors();
            connected_ = read_slut_slowly();
        }

        // If a reset was performed the chip is likely to take some time to settle down. Read the slut
        // until it succeeds
        if ((done_reset) && (!IsConnected()))
        {
            connected_ = retry_slut();
        }
    }

    if (IsConnected())
    {
        disable_sleep_and_speed_up();

        pttrans_stream_bccmd_init(mPtStream, slutid_spi_user_cmd_, bccmd_spi_interface_);

        escape_auto_baud_and_protocol_detection();

        enable_hq_scraping();
   }
    Debug("bccmd_spi::open end\n");
	return IsConnected();
}

// ========================================================================================
// Write the auto-baud and auto-protocol detection escape hatches
// Required when first connecting and on every reset to make sure the chip boots.
// ========================================================================================
void bccmd_spi::escape_auto_baud_and_protocol_detection()
{
    Debug("Write auto baud rate\n");
    if (slutid_uart_auto_baud_rate_ != 0)
    {
        static const unsigned int auto_baud_value = 115200;
        // Write the baud rate to the address referenced to 
        // break the chip out of auto-baud-rate selection.
        spi_write(slutid_uart_auto_baud_rate_,      auto_baud_value >> 16);
        spi_write(slutid_uart_auto_baud_rate_ + 1 , auto_baud_value & 0xffff);
    }

    Debug("Write auto protocol detect\n");
    if (slutid_uart_auto_protocol_ != 0)
    {
        // Write a protocol to the address referenced to 
        // break the chip out of auto protocol detection.
        // we write 0 which means no uart protocol.
        spi_write(slutid_uart_auto_protocol_, 0);
    }
}

// ========================================================================================
// Return whether we're running HQ scraping.
// ========================================================================================
bool bccmd_spi::hq_scraping()
{
    return mpHQThread != NULL;
}

// ========================================================================================
// Try to enable HQ scraping.
// ========================================================================================
void bccmd_spi::enable_hq_scraping()
{
    Debug("Enable HQ scraping\n");
    // We need a handler object and a pointer to the list head to check for HQ PDUs
    if( IsConnected() && mpHqPduHandler != 0 && slutid_hq_scraping_list_head_ != 0 ) 
    {
        // Attempt to enable BCCMD scraping - will fail if firmware does not support it
        if( set_hq_scraping_enable(true) )
        {
            // Start the scraping thread - default polling interval
            mpHQThread = new HQScraper(this, mpHqPduHandler, slutid_hq_scraping_list_head_);
            mpHQThread->Start();

            mSupportsHQ = true;
        }
    }
}

// ========================================================================================
// Stop HQ scraping.
// ========================================================================================
void bccmd_spi::disable_hq_scraping()
{
    Debug("Disable HQ scraping\n");
    if(mpHQThread != 0)
    {
        delete mpHQThread, mpHQThread = 0;
    }
}

// ========================================================================================
// Indicate failure to disable shallow sleep
// ========================================================================================
void bccmd_spi::raise_spi_error(const ichar *error_message)
{
	BCCMDTRANS_FUNCTION_DEBUG_SENTRY;
	ichar msg[_MAX_ERROR_MSG_SIZE];
	isprintf(msg, _MAX_ERROR_MSG_SIZE, II("SPI communications failed.\n%s.\nPlease check the cable and power supply, then attempt to reconnect."), error_message);

	raise_error(msg,  II("Communication error"), BCCMD_TRANS_ERROR_CONNECTION, BCCMD_TRANS_ERRTYPE_ABORT);
		
	connected_ = false;
}

// ========================================================================================
// Raise a comms error 
// ========================================================================================
void bccmd_spi::raise_comms_error(const ichar *error_message)
{
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY;
    char *message = iinarrowdup(error_message);
    if (message)
    {
        Debug("%s\n", message);
        free(message);
    }

	raise_error(error_message,  II("Communication error"), BCCMD_TRANS_ERROR_CONNECTION, BCCMD_TRANS_ERRTYPE_ABORT);
}

// ========================================================================================
// Disable shallow sleep and set clock to max
// ========================================================================================
bool bccmd_spi::disable_sleep_and_speed_up()
{
	bool success = false;
	BCCMDTRANS_FUNCTION_DEBUG_SENTRY_R(bool, success);
	
    Debug("disable_sleep_and_speed_up\n");
	if (slutid_force_fast_clock_ != 0)
	{
		// Disable shallow sleep if this key is present.
		// by writing a non-zero value to the address.
		if (!spi_write(slutid_force_fast_clock_, 0xffff))
		{
			raise_spi_error(II("Failed to write to disable Shallow Sleep"));
		}
		else
		{
			// Speed up the spi clock.
			pttrans_stream_setvar(mPtStream, SPICLOCK, pttrans_stream_getvar(mPtStream, SPIMAXCLOCK));

			uint16 data;

			// B-61606
            if (pttrans_stream_xap_helper(mPtStream).GetChipType() == CHIP_TYPE_ELV)
			{
				std::string real_max_clock  = pttrans_stream_getvar(mPtStream, SPICLOCK);
				StopWatch loop_timer;
				do
				{
					pttrans_stream_setvar(mPtStream, SPICLOCK, pttrans_stream_getvar(mPtStream, SPIMAXCLOCK));
					(void) spi_read(slutid_force_fast_clock_, data);
				}
				while(loop_timer.duration() < 500 && strcmp(pttrans_stream_getvar(mPtStream, SPICLOCK), real_max_clock.c_str()) != 0);
			}

			// Read a value until the SPI becomes reliable, initially ignoring errors
			(void) spi_read(slutid_force_fast_clock_, data);
			(void) spi_read(slutid_force_fast_clock_, data);
			bool spi_success = spi_read(slutid_force_fast_clock_, data);

			// If still unreliable then give it a few more tries
			int i = 0;
			while (i < 3 && !spi_success)
			{
				spi_success = spi_read(slutid_force_fast_clock_, data);
				i++;
			}

			// If still not reliable, raise error				
			if (!spi_success)
			{
				raise_spi_error(II("Failed to stabilise reads after disabling Shallow Sleep"));
			}
			
			success = spi_success;
		}
	}
	else
	{
		raise_error(II("SPI communications suboptimal.\n")
			II("This firmware build does not support shallow sleep suppression.\n")
			II("SPI communications may be slow"),  
			II("Communication error"), BCCMD_TRANS_ERROR_CONNECTION, BCCMD_TRANS_ERRTYPE_MESSAGE);				
	}
	
	return success;
}

bool bccmd_spi::close()
{
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY;

    if(IsConnected())
        spi_clear_hw_brkps();

    // shut down HQ scraping
    if(mpHQThread != 0)
    {
        delete mpHQThread, mpHQThread = 0;
        // Disable HQ scraping to free up the memory on the target
        set_hq_scraping_enable(false);
    }
    slutid_hq_scraping_list_head_ = 0;

    // let the base class do its OnClose behaviour.
    OnClose();

    // allow the chip to sleep again.
    if (slutid_force_fast_clock_ != 0)
    {
        // enable shallow sleep again, by writing zero to this value. Don't care if it fails
        (void) spi_write(slutid_force_fast_clock_, 0);
        slutid_force_fast_clock_ = 0;
    }

    connected_ = false;

    return true;
}

// ========================================================================================
// Called when the transport is closed.
// ========================================================================================
void bccmd_spi::OnClose()
{
    // do the base class' OnClose behaviour
    bccmd_trans::OnClose();
    // then fix security, because SPI always has access to memory,
	// so security is effectively always disabled when using SPI.
    mSecurityEnabled = SECURITY_DISABLED;
}

// This sets the BCCMD/SPI flag to a new value, and then waits for it to change
bool bccmd_spi::set_flag_wait(uint16 command, uint16 &newcmd)
{
    bool spi_success;
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY_R(bool, spi_success);
	int attempts = 0;
    bool command_changed = false;
    
    // Write down our new command.
	spi_success = spi_write(bccmd_spi_interface_+0, command);

	while ((++attempts <= 5) && (spi_success) && (!command_changed))
	{
		// Trigger the BG interrupt.
		spi_success = spi_write(slutid_spi_user_cmd_, 0);
		if (spi_success)
		{
			// Loop until the BCCMD command has changed
			StopWatch loop_timer;
			do
			{
				spi_success = spi_read(bccmd_spi_interface_+0, newcmd);
				if (spi_success)
				{
					// Check if the command variable has changed.
					if (newcmd != command)
					{
						// If we're recording the SPI transactions then we'll be wanting to play them back
						// again at some point.  To guarantee(?) that we wait long enough for the XAP to
						// finish when playing back wait for twice as long as we actually need.
						if (STRICMP(getvar(SPIDEBUG_REPRODUCIBLE), "ON") == 0)
						{
							HiResClockSleepMilliSec(loop_timer.duration());
						}

						command_changed = true;
					}
					else
					{
						HiResClockSleepMilliSec(1);
					}
				}
			}
			while ((loop_timer.duration() < 100) && (spi_success) && (!command_changed));
		}
	}
	return spi_success;
}

bccmd_trans_error_response bccmd_spi::run_pdu(const BCCMD_PDU& p, BCCMD_PDU& resp)
{
    bccmd_trans_error_response rv;
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY_R(bccmd_trans_error_response, rv);
    uint8* buffer = new uint8[p.size()];
    memcpy(buffer,p.data(),p.size());
    rv = run_pdu ( (BCCMDPDU *)buffer );
    if (rv == BCCMD_TRANS_ERROR_NONE)
    {
        resp = BCCMD_PDU(buffer,p.size());
    }
    delete[] buffer;
    if ((rv == BCCMD_TRANS_ERROR_ABORT) && (get_error_handler()->get_last_error_code() == 0))
    {
		wrap_raise_error("Failed to send or process command",
								  II("Communications Error"),
								  BCCMD_TRANS_ERROR_CONNECTION,
								  BCCMD_TRANS_ERRTYPE_ARI);
    }
    return rv;
}

bccmd_trans_error_response bccmd_spi::send_pdu(const BCCMD_PDU& p)
{
    bccmd_trans_error_response rv;
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY_R(bccmd_trans_error_response, rv);
    uint8* buffer = new uint8[p.size()];
    memcpy(buffer,p.data(),p.size());
    rv = send_pdu ( (BCCMDPDU *)buffer );
    delete[] buffer;
    return rv;
}

bccmd_trans_error_response bccmd_spi::send_pdu(BCCMDPDU *p)
{
	BCCMDTRANS_FUNCTION_DEBUG_SENTRY;
	return process_pdu(p, false);
}

bccmd_trans_error_response bccmd_spi::run_pdu(BCCMDPDU *p)
{
	BCCMDTRANS_FUNCTION_DEBUG_SENTRY;
	return process_pdu(p, true);
}

bccmd_trans_error_response bccmd_spi::process_pdu(BCCMDPDU *p, bool wait_for_result)
{
	bccmd_trans_error_response err = BCCMD_TRANS_ERROR_NONE;
	BCCMDTRANS_FUNCTION_DEBUG_SENTRY_R(bccmd_trans_error_response, err);
	static const int STATE_MACHINE_RESET_RETRIES = 10;

	uint16 buffer_flag;
	uint16 buffer_pointer = 0;

	// If we're not connected, attempt to open the communications.
	if (!IsConnected())
	{
		open(mpHqPduHandler);
		if (!IsConnected())
		{
			err = BCCMD_TRANS_ERROR_ABORT;
			return err;
		}
	}

	// Lock the SPI mutex while processing the BCCMD to ensure we don't get interrupted.
	TransAcquireStreamLock lock(mPtStream);

    // If the chip managed to get left in the middle of an operation, try to return us to a known state.
    Debug("check state machine initial state\n");
	bool spi_success = spi_read(bccmd_spi_interface_+0, buffer_flag);
	for(int attempts = 0; spi_success 
	                        && attempts < STATE_MACHINE_RESET_RETRIES
	                        && buffer_flag != BCCMD_SPI_CMD_IDLE 
	                      	&& buffer_flag != BCCMD_SPI_CMD_ALLOC_FAIL;
	        ++attempts)
	{
		spi_success = set_flag_wait(BCCMD_SPI_CMD_DONE, buffer_flag);
	}
	
	if (!spi_success)
    {
		err = BCCMD_TRANS_ERROR_ABORT;
		return err;
	}
	// state machine must be in IDLE or ALLOC_FAIL to begin a command, or it just won't work.
	else if ((buffer_flag != BCCMD_SPI_CMD_IDLE && buffer_flag != BCCMD_SPI_CMD_ALLOC_FAIL))
	{
        raise_comms_error(II("Chip command interface initial state incorrect (BCCMD SPI)"));
        err = BCCMD_TRANS_ERROR_ABORT;
        return err;
	}

    if(STRICMP(getvar(SPIDEBUG_REPRODUCIBLE), "ON") != 0)
	{
		// Check if PtTransport can do the BCCMD, if we're connected to a Babel then we'll try
		// and do the BCCMD on there.  Other options will just fail.
		if (pttrans_stream_bccmd_cmd(mPtStream, (uint16*)p, p->pdulen, wait_for_result) == PTTRANS_SUCCESS)
		{
            err = BCCMD_TRANS_ERROR_NONE;
            return err;
		}

		if(pttrans_get_last_error(NULL, NULL) != PTTRANS_ERROR_PLUGIN_FN)
		{
			// pttrans_bccmd_cmd did something and returned failure, this could mean it timed out and its handling of the bccmd_spi state machine on errors
			// isn't awesome, so try to pick up/clean up where it left off if we can.
			Debug("attempting to recover state machine after babel BCCMD fail\n");
			spi_success = spi_read(bccmd_spi_interface_+0, buffer_flag);
			for(int attempts = 0; spi_success && attempts < STATE_MACHINE_RESET_RETRIES
			                      && buffer_flag != BCCMD_SPI_CMD_IDLE 
			                      && buffer_flag != BCCMD_SPI_CMD_ALLOC_FAIL 
			                      && buffer_flag != BCCMD_SPI_CMD_PENDING 
			                      && buffer_flag != BCCMD_SPI_CMD_RESP;
			        ++attempts)
			{
				spi_success = set_flag_wait(BCCMD_SPI_CMD_DONE, buffer_flag);
			}
			if(!spi_success)
			{
				Debug("spi failure in set_flag_wait.\n");
			}
			else
			{
				Debug("state machine recover ended with state = %d\n", buffer_flag);
			}
		}
	}

    if(buffer_flag == BCCMD_SPI_CMD_IDLE || buffer_flag == BCCMD_SPI_CMD_ALLOC_FAIL)
	{
        Debug("Buffer flag is \"idle\" or \"alloc fail\"\n");
		err = allocate_mem(p, &buffer_pointer);
		if (err != BCCMD_TRANS_ERROR_NONE)
		{
			return err;
		}

		// Copy the data to the CHIP and make the CHIP do the work
		spi_success = (pttrans_stream_write_block_verify(mPtStream, buffer_pointer, p->pdulen, (uint16*)p) == PTTRANS_SUCCESS);
		
        if (!spi_success)
        {
            Debug("spi failure\n");
        }
        else if (wait_for_result)
        {
            Debug("Wait for result\n");
            spi_success = set_flag_wait(BCCMD_SPI_CMD_CMD, buffer_flag);
        }
        else
		{
            Debug("send command without wait\n");

            // Write down our new command.
	        spi_success = spi_write(bccmd_spi_interface_+0, BCCMD_SPI_CMD_CMD) && spi_write(slutid_spi_user_cmd_, 0);
            if (spi_success)
            {
                err = BCCMD_TRANS_ERROR_NONE;
                return err;
            }
		}
	}

    StopWatch timer;
    if (spi_success && buffer_flag == BCCMD_SPI_CMD_PENDING)
    {
	    Debug("Wait for the chip to finish\n");
    }
	while (spi_success && buffer_flag == BCCMD_SPI_CMD_PENDING && timer.duration() < 1000)
	{
		spi_success = spi_read(bccmd_spi_interface_+0, buffer_flag);
	}

	if (spi_success)
	{
		if (buffer_flag == BCCMD_SPI_CMD_IDLE)
		{
            Debug("WARNING: Chip returned to idle state when asked to process command (BCCMD SPI)\n");
            err = BCCMD_TRANS_ERROR_RETRY;
			return err;
		}
		else if (buffer_flag != BCCMD_SPI_CMD_RESP)
		{
            Debug("WARNING: Chip did not respond within timeout period (BCCMD SPI)\n");
            err = BCCMD_TRANS_ERROR_RETRY;
			return err;
		}
		else
		{
        	Debug("Chip has responded\n");
            get_error_handler()->clear_errors();

			// If we're recording the SPI transactions then we'll be wanting to play them back
			// again at some point.  To guarantee(?) that we wait long enough for the XAP to
			// finish when playing back wait for twice as long as we actually need.
			if (STRICMP(getvar(SPIDEBUG_REPRODUCIBLE), "ON") == 0)
			{
				HiResClockSleepMilliSec(timer.duration());
			}

			// if we're picking up an incomplete pttrans BCCMD then we didn't get the buffer pointer at the start.
			if(!buffer_pointer) 
			{
				spi_success = spi_read(bccmd_spi_interface_+2, buffer_pointer);
			}

			if (spi_success)
			{
				// Read the value back from the pdu.
				spi_success = (pttrans_stream_read_block_verify(mPtStream, buffer_pointer, p->pdulen, (uint16*)p) == PTTRANS_SUCCESS);

				// Tell the chip that we are done now
				spi_success = spi_success && set_flag_wait(BCCMD_SPI_CMD_DONE, buffer_flag);
				if (spi_success)
				{
					if (buffer_flag != BCCMD_SPI_CMD_IDLE)
					{
                        Debug("WARNING: Chip command interface did not return to idle (BCCMD SPI)\n");
						err = BCCMD_TRANS_ERROR_RETRY;
					}
				}
			}
		}
	}
	if (!spi_success)
	{
		err = BCCMD_TRANS_ERROR_ABORT;
	}
	return err;	
}

bccmd_trans_error_response bccmd_spi::allocate_mem(BCCMDPDU *p, uint16 *ptr)
{
    bccmd_trans_error_response err = BCCMD_TRANS_ERROR_NONE;
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY_R(bccmd_trans_error_response, err);
    bool reproducible = (STRICMP(getvar(SPIDEBUG_REPRODUCIBLE), "ON") == 0);
    bool spi_success = true;
    uint16 block_len = p->pdulen;
	uint16 buffer_flag = 0;

    // If we're recording the SPI transactions then we'll be wanting to play them back
    // again at some point.  To guarantee(?) we're always allocating the same block in
    // the XAP try and allocate a 512 block rather than what we actually want.
    if (reproducible) 
    {
        block_len = 512;
    }

    StopWatch loop_timer;
    do
    {
        // Try to allocate the block on chip.
        bool spi_success = spi_write(bccmd_spi_interface_+1, block_len);
        if (spi_success)
        {
            Debug("written block len ok\n");

            spi_success = set_flag_wait(BCCMD_SPI_CMD_ALLOC_REQ, buffer_flag);
            if (spi_success)
            {
                Debug("set flag wait = true\n");
                if (buffer_flag != BCCMD_SPI_CMD_ALLOC_OK)
                {
                    // If we're in reproducible mode we'll try to allocate a smaller block 
                    // as there may not be any 512 word blocks available on the chip.
                    if (reproducible)
                    {
                        block_len /= 2;
                        if (block_len <= p->pdulen)
                        {
                            block_len = p->pdulen;
                        }
                    }
                }
            }
        }
    } 
    while ((spi_success) && (buffer_flag != BCCMD_SPI_CMD_ALLOC_OK) && (loop_timer.duration() < 500));

    if ((spi_success) && (buffer_flag == BCCMD_SPI_CMD_ALLOC_OK))
    {
        Debug("allocated ok, read the value\n");
        spi_success = spi_read(bccmd_spi_interface_+2, *ptr);
    }
    else if (buffer_flag == BCCMD_SPI_CMD_ALLOC_FAIL)
    {
		wrap_raise_error("Chip could not allocate memory (BCCMD SPI)",
								  II("Communications Error"),
								  BCCMD_TRANS_ERROR_CONNECTION,
								  BCCMD_TRANS_ERRTYPE_ARI);
        err = BCCMD_TRANS_ERROR_ABORT;
    }
    else
    {
        Debug("WARNING: Chip did not respond to allocate memory request (BCCMD SPI)\n");
        err = BCCMD_TRANS_ERROR_RETRY;
    }

    return err;
}


bool bccmd_spi::reset_bc(reset_t reset_type)
{
    bool success = true;
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY_R(bool, success);
    // Lock the SPI mutex.  This will be released on function return.
    TransAcquireStreamLock lock(mPtStream);
    bccmd_trans_error_response err;

    // Stop polling for HQ PDUs - HQ scraping defaults to disabled after reset
    bool was_hq_scraping = hq_scraping();
    disable_hq_scraping();

    if (reset_type == RESET_WARM)
    {
		BCCMD_WARM_RESET_PDU p;
		p.set_req_type(BCCMDPDU_SETREQ);

        int retries = 0;    
		do
	    {
		    err = send_pdu(p);
            retries++;
	    }
	    while ((err == BCCMD_TRANS_ERROR_RETRY) && (retries < 3));

        // Fail if still in error state
   		success = (err == BCCMD_TRANS_ERROR_NONE);
        if (!success)
        {
			raise_error(II("Warm reset failed\n"), II("Error"), BCCMD_TRANS_ERROR_OPERATION, BCCMD_TRANS_ERRTYPE_ABORT);
        }
    }
    else if (reset_type == RESET_COLD)
    {
		if(pttrans_stream_xap_reset_and_go(mPtStream) != PTTRANS_SUCCESS)
		{
			raise_error(II("Cold reset failed\n"), II("Error"), BCCMD_TRANS_ERROR_OPERATION, BCCMD_TRANS_ERRTYPE_ABORT);
			success = false;
		}
    }

    if (success)
    {
        HiResClockSleepMilliSec(1000);

        success = disable_sleep_and_speed_up();

        if (success)
        {
            escape_auto_baud_and_protocol_detection();
        }
    }
    else if (was_hq_scraping)
    {
        // we think we didn't reset, so try to resume hq scraping.
        enable_hq_scraping();
    }

    return success;
}

bool bccmd_spi::reset_and_close(reset_t reset_type)
{
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY;
    bool retVal = reset_bc(reset_type);
    if(retVal == false)
    {
        // reset failed, but close anyway. Forces re-open for next bccmd,
        // which is probably more robust that leaving open after a failed
        // cold reset
        close();
        return retVal;
    }

	return close();
}

bool bccmd_spi::get_firmware_id(int *const id, uint16 *nameBuffer, const size_t sNameBuffer)
{
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY;
    uint16 val;

	ASSERT(id != NULL);
	ASSERT((nameBuffer != NULL) || (sNameBuffer == 0));
 ASSERT(sNameBuffer < USHRT_MAX);
	if (!IsConnected())
		return false;
	if ((slutid_id_string_location_ == 0) || (slutid_id_integer_location_ == 0))
		return false;
	// copy out the string.
	// Lock the SPI mutex.  This will be released on function return.
    TransAcquireStreamLock lock(mPtStream);
	uint16 i = 0;
	while (i < sNameBuffer)
	{
        if (pttrans_stream_read_verify(mPtStream, slutid_id_string_location_ + i, &val) != PTTRANS_SUCCESS)
            return false;
		nameBuffer[i] = (uint16)val;
		if (nameBuffer[i] == 0)
			break;
		i++;
  ASSERT(i < USHRT_MAX);
	}
	// copy the value
    if (pttrans_stream_read_verify(mPtStream, slutid_id_integer_location_, &val) == PTTRANS_FAILURE)
			return false;
	*id = val;
	return true;
}



int bccmd_spi::GetNumOperationAttempts() 
{
	return 6;
}


bool bccmd_spi::IsConnected() const 
{
	return connected_;
}


bool bccmd_spi::read_mem(GenericPointer addr, uint16 len, uint16 * data)
{
	bool retval = false;
	BCCMDTRANS_FUNCTION_DEBUG_SENTRY_R(bool, retval);

    // Can only read from raw data here
    if (addr.Mem() == Mem_RawData)
	{
		// Lock the SPI mutex.  This will be released on function return.
		TransAcquireStreamLock lock(mPtStream);

		if (!data)
		{
			data = (uint16*)malloc(sizeof(uint16) * len);
		}

		retval = (pttrans_stream_read_block_verify(mPtStream, (uint16)addr, len, data) == PTTRANS_SUCCESS);

		// Start chip regardless
		if (pttrans_stream_xap_stopped(mPtStream) != PTTRANS_XAP_RUNNING) 
		{
			pttrans_stream_xap_go(mPtStream);
		}
	}

	return retval;
}

//==============================================================================
// Get or create a xap helper object.
// This is an overridden version for pttransport that returns the one pttransport
// has already made.
//==============================================================================
bool                                                   // returns whether an object was successfully obtained.
bccmd_spi::get_xap_helper(CXapHelper *&apXapHelper,    // [out] this is set to point to the object.
                          bool force_reread)           // ignored for this implementation.
{
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY;
    (void) force_reread;  // we get our xaphelper from pttransport, which doesn't support force rereading.
    try
    {
        apXapHelper = &pttrans_stream_xap_helper(mPtStream);
    }
    catch(const std::runtime_error &)
    {
        apXapHelper = NULL;
    }
    return apXapHelper != NULL && apXapHelper->IsReadyForUse();
}

//==============================================================================
// Set XAP hardware breakpoint. Separate method added (instead of
// just using the general SPI write routine) as we need to stop the
// XAP for the write to take effect (and obviously restart it when done).
//==============================================================================
bool bccmd_spi::spi_set_hw_brkp(uint32 brkp_addr)
{
    bool success;
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY_R(bool, success);
    // Lock the SPI mutex.  This will be released on function return.
    TransAcquireStreamLock lock(mPtStream);
    success = spi_stop();
    if(pttrans_stream_xap_helper(mPtStream).IsBc7OrLater() && success)
        success = spi_write(XAP_BRK_INDEX, hw_bps_enabled);

    if (success)
    {
        // Set h/w breakpoint register
        success = spi_write (XAP_BRK_REGH, (uint16)(brkp_addr >> 16)) && spi_write (XAP_BRK_REGL, (uint16)(brkp_addr & 0x0000FFFF));
        if (success)
        {
            // Arrange that an interrupt fires when the h/w breakpoint
            // is reached (another agent must also set the B flag).
            int result = pttrans_stream_xap_go_irq_on_breakpoint(mPtStream);
            success = result == PTTRANS_SUCCESS;
        }
    }
    if(success)
        ++hw_bps_enabled;

    return success;
}

//==============================================================================
// Read XAP hardware breakpoint.
//==============================================================================
bool bccmd_spi::spi_get_hw_brkp(uint32 &hw_brkp)
{
    bool spi_success;
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY_R(bool, spi_success);
    // Lock the SPI mutex.  This will be released on function return.
    TransAcquireStreamLock lock(mPtStream);
 
    spi_success = spi_stop();
    if (spi_success)
    {
        // Get contents of h/w breakpoint registers
        uint16 spi_value;
        spi_success = spi_read (XAP_BRK_REGH, spi_value);
        if (spi_success)
        {
            hw_brkp = spi_value << 16;

            spi_success = spi_read (XAP_BRK_REGL, spi_value);
            hw_brkp |= (spi_value & 0xFFFF);
        }
    }
 
    if (spi_success)
    {
        spi_success = (pttrans_stream_xap_go_irq_on_breakpoint(mPtStream) == PTTRANS_SUCCESS);
    }

    return spi_success;
}

//==============================================================================
// Clear XAP hardware breakpoint. Separate method added (instead of
// just using the general SPI write routine) as we need to stop the
// XAP for the write to take effect (and obviously restart it when done).
//==============================================================================
bool bccmd_spi::spi_clear_hw_brkps()
{
    bool success;
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY_R(bool, success);
    // Lock the SPI mutex.  This will be released on function return.
    TransAcquireStreamLock lock(mPtStream);

    success = spi_stop();
    while(success && hw_bps_enabled > 0)
    {
        --hw_bps_enabled;
        if(pttrans_stream_xap_helper(mPtStream).IsBc7OrLater())
            success = spi_write(XAP_BRK_INDEX, hw_bps_enabled);

        if (success)
        {
            success = spi_write (XAP_BRK_REGH, 0x00FF) && spi_write (XAP_BRK_REGL, 0xFFFF);
        }
    }
    if (success)
    {
        int result = pttrans_stream_xap_go_irq_on_breakpoint(mPtStream);
        success = result == PTTRANS_SUCCESS;
    }
    return success;
}

bool bccmd_spi::spi_clear_hw_brkp()
{
    bool success;
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY_R(bool, success);
    // Lock the SPI mutex.  This will be released on function return.
    TransAcquireStreamLock lock(mPtStream);

    success = spi_stop();
    if(success && hw_bps_enabled > 0)
    {
        --hw_bps_enabled;
        if(pttrans_stream_xap_helper(mPtStream).IsBc7OrLater())
            success = spi_write(XAP_BRK_INDEX, hw_bps_enabled);
        if (success)
        {
            // "Clear" h/w breakpoint register
            success = spi_write (XAP_BRK_REGH, 0x00FF) && spi_write (XAP_BRK_REGL, 0xFFFF);
        }
    }
    if (success)
    {
        int result = pttrans_stream_xap_go_irq_on_breakpoint(mPtStream);
        success = result == PTTRANS_SUCCESS;
    }
    return success;
}


// Enable / Disable HQ logging in the firmware (for HQ scraping over SPI)
bool bccmd_spi::set_hq_scraping_enable(const bool enabled)
{
    bool result(false);
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY_R(bool, result);

    BCCMD_HQ_SCRAPING_ENABLE_PDU pdu;

    pdu.set_req_type(BCCMDPDU_SETREQ);
    pdu.set_seq_no(0x00); // irrelevant
    if(enabled)
    {
        pdu.set_data(1);
    }
    else
    {
        pdu.set_data(0);
    }

    BCCMDPDU* pduData = (BCCMDPDU*)pdu.data();

    result = run_pdu_report_errs(pduData);
    
    if(result)
    {
        BCCMD_HQ_SCRAPING_ENABLE_PDU lpdu((uint8*)pduData, pduData->pdulen * sizeof(uint16));

        if(lpdu.get_status() != BCCMDPDU_STAT_OK)
        {
            result = false;
        }
    }

    return result;
}

void  bccmd_spi::setHqScrapeInterval(const uint16 interval)
{
    BCCMDTRANS_FUNCTION_DEBUG_SENTRY;
    if( mpHQThread )
    {
        mpHQThread->SetInterval(interval); 
    }
}

bool bccmd_spi::spi_stop()
{
	BCCMDTRANS_FUNCTION_DEBUG_SENTRY;
	int tries = 0;
	int stopped;
	do {
		do 	{
			stopped = (pttrans_stream_xap_stop(mPtStream) == PTTRANS_SUCCESS);
		} while (!stopped && (++tries < 5));
		if (stopped)
			return true;
    } while (raise_error(II("Chip may be in a Deep Sleep mode, which may prevent the Stop command from being processed. "),
		II("Communication error"), BCCMD_TRANS_ERROR_CONNECTION, BCCMD_TRANS_ERRTYPE_ARI) == BCCMD_TRANS_ERROR_RETRY);
	return false;
}

//=========================================================================
// Read verify wrapper for PtTrans
//=========================================================================
bool bccmd_spi::spi_read(uint16 addr, uint16 &data)
{
	BCCMDTRANS_FUNCTION_DEBUG_SENTRY;
	return (pttrans_stream_read_verify(mPtStream, addr, &data) == PTTRANS_SUCCESS);
}

//=========================================================================
// Write verify wrapper for PtTrans
//=========================================================================
bool bccmd_spi::spi_write(uint16 addr, uint16 data)
{
	BCCMDTRANS_FUNCTION_DEBUG_SENTRY;
	return (pttrans_stream_write_verify(mPtStream, addr, data) == PTTRANS_SUCCESS);
}

const char *bccmd_spi::getvar(const char *var)
{
	return pttrans_stream_getvar(mPtStream, var);
}

const uint16 HQScraper::mMAX_SLEEP_TIME_MS = 100;

HQScraper::HQScraper(bccmd_spi * const bccmdSpi, 
                        BlueCoreDeviceController_newStyle::CallBackProvider* hqPduHandler,
                        uint16 const hqListHeadAddr)
: mpBccmdSpi(bccmdSpi), mpPduHandler(hqPduHandler), mHqListHeadAddr(hqListHeadAddr)
{
    // default polling interval to 1 second (good enough for fault reports)
    mPollMultiplier = 10;
    mPollRemainder = 0;
}

HQScraper::~HQScraper()
{
    // Ensure that the thread stops before destruction of members
    // accessed by ThreadFunc.
    Kill(1000);
}

int HQScraper::ThreadFunc()
{
    // Poll for HQ PDUs
    uint16 listhead(0);
    
    while(KeepGoing())
    {
        // Check the list head pointer for HQ PDU
        do
        {
            int readRes = pttrans_stream_read(mpBccmdSpi->PtStream(), mHqListHeadAddr, &listhead);
            if (readRes != PTTRANS_ERROR_NO_ERROR || (listhead > 0 && !ScrapeHq()))
            {
                break;
            }
        } while (listhead > 0 && KeepGoing());

        // Interval is broken up to allow the thread to exit sooner if stopped.
        CriticalSection::Lock here(mIntervalGuard);
        for(uint16 sleepCount = 0; sleepCount < mPollMultiplier && KeepGoing(); ++sleepCount)
        {
            HiResClockSleepMilliSec(mMAX_SLEEP_TIME_MS);
        }
        if(mPollRemainder != 0 && KeepGoing())
        {
            HiResClockSleepMilliSec(mPollRemainder);
        }
    }

    return 0;
}

inline void HQScraper::SetInterval(uint16 const interval)
{
    CriticalSection::Lock here(mIntervalGuard);
    mPollMultiplier = interval / mMAX_SLEEP_TIME_MS;
    mPollRemainder = interval % mMAX_SLEEP_TIME_MS;
}

inline uint16 HQScraper::GetInterval()
{
    CriticalSection::Lock here(mIntervalGuard);
    return ((mPollMultiplier * mMAX_SLEEP_TIME_MS) + mPollRemainder);
}

// Pull and HQ PDU from the queue - return true if a valid HQ PDU is obtained
bool HQScraper::ScrapeHq()
{
    bool result(false);
    uint16 hqPduLen;

    BCCMD_HQ_SCRAPING_LEN_PDU pdu;
    pdu.set_req_type(BCCMDPDU_GETREQ);
    pdu.set_seq_no(0x00); // irrelevant

    BCCMDPDU* pduData = (BCCMDPDU*)pdu.data();
    result = mpBccmdSpi->run_pdu_report_errs(pduData);    
    if(result)
    {
        BCCMD_HQ_SCRAPING_LEN_PDU lpdu((uint8*)pduData, pduData->pdulen * sizeof(uint16));
        if(lpdu.get_status() != BCCMDPDU_STAT_OK)
        {
            return false;
        }

        hqPduLen = lpdu.get_data();
        if(hqPduLen == 0)
        {
            return false;
        }
        else
        {
            BCCMD_HQ_SCRAPING_PDU pdu;
            pdu.set_req_type(BCCMDPDU_GETREQ);
            pdu.set_seq_no(0x00); // irrelevant
            pdu.set_hq_len(hqPduLen);

            BCCMDPDU* pduData = (BCCMDPDU*)pdu.data();
            result = mpBccmdSpi->run_pdu_report_errs(pduData);    
            if(result)
            {
                BCCMD_HQ_SCRAPING_PDU lpdu((uint8*)pduData, pduData->pdulen * sizeof(uint16));
                if(lpdu.get_status() != BCCMDPDU_STAT_OK)
                {
                    return false;
                }

                uint16* pData = new uint16[lpdu.get_hq_len()];
                lpdu.get_hq_data(pData, lpdu.get_hq_len());
                if(pData[1] == 0) // Length of HQPDU
                {
                    delete[] pData;
                    return false;
                }
                delete[] pData;

                // Pass on to the HQ handler
                mpPduHandler->onPDU(lpdu.get_hq());
            }
        }
    }

    return result;
}

//=========================================================================
// Forward debug from bccmd_trans base class to the spi log.
//=========================================================================
void bccmd_spi::Debug(const char *aFmt, ...)
{
    va_list ap;
    va_start(ap, aFmt);
    pttrans_stream_debugfv(mPtStream, PTTRANS_DEBUG_TYPE_NORMAL, aFmt, ap);
    va_end(ap);
}

//=========================================================================
// Forward debug from bccmd_trans base class to the spi log.
//=========================================================================
void bccmd_spi::DebugFull(const char *aFmt, ...)
{
    va_list ap;
    va_start(ap, aFmt);
    pttrans_stream_debugfv(mPtStream, PTTRANS_DEBUG_TYPE_FULL, aFmt, ap);
    va_end(ap);
}
