///////////////////////////////////////////////////////////////////////////////
//
//  FILE:       bccmd_spi.h
//
//  Copyright Cambridge Silicon Radio 2003-2006
//
//  PURPOSE:    Declare the underlying spi implementation of the
//              BCCMD Transport library:
//                  the bccmd_spi class.
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/bccmd_trans/bccmd_spi.h#1 $
//
///////////////////////////////////////////////////////////////////////////////

#ifndef BCCMD_SPI_INCLUDED
#define BCCMD_SPI_INCLUDED

#include "bccmd_trans.h"
#include "pttransport/pttransport.h"
#include "thread/thread.h"

class HQScraper;

class BCCMD_TRANS_API bccmd_spi : public bccmd_trans
{
public:
	bccmd_spi(bccmd_trans_error_handler *handler, const char *trans = NULL,
              int port = -1, int multispi = -1);
	virtual ~bccmd_spi();
	virtual bool reset_bc(reset_t reset_type); 
	virtual bool reset_and_close(reset_t reset_type);
	virtual bool get_firmware_id(int *const id, uint16 *nameBuffer, const size_t sNameBuffer);
	virtual bool read_mem(GenericPointer addr, uint16 len, uint16 * data);
	virtual bool open(BlueCoreDeviceController_newStyle::CallBackProvider* hqPduHandler = 0);
	virtual bool close();
	void OnClose();
	virtual int GetNumOperationAttempts();
	virtual bool IsConnected() const;

	virtual const char *getvar(const char *var);

    // Added for benefit of native execution; breaks the
    // layering somewhat, but expediency wins.
    bool spi_set_hw_brkp(uint32 brkp_addr);
    bool spi_get_hw_brkp(uint32 &hw_brkp);
    bool spi_clear_hw_brkp(void);
    bool spi_clear_hw_brkps(void);

    // HQ Scraping support
    virtual bool supportsHQ() const { return mSupportsHQ; };
    virtual void setHqScrapeInterval(const uint16 interval);

    virtual void Debug(const char *aFmt, ...);
    virtual void DebugFull(const char *aFmt, ...);

    pttrans_stream_t PtStream() { return mPtStream; }

protected:
    virtual bool get_xap_helper(CXapHelper *&apXapHelper, bool force_reread);

private:
	bccmd_trans_error_response run_pdu(const BCCMD_PDU& p, BCCMD_PDU& resp);
	bccmd_trans_error_response send_pdu(const BCCMD_PDU& p);
	bccmd_trans_error_response run_pdu(BCCMDPDU *p);
	bccmd_trans_error_response send_pdu(BCCMDPDU *p);
	bccmd_trans_error_response process_pdu(BCCMDPDU *p, bool wait_for_result);
    bccmd_trans_error_response allocate_mem(BCCMDPDU *p, uint16 *ptr);
	// HANDLE runEvent;
	bool set_flag_wait(uint16 command, uint16 &newcmd);
    bool set_hq_scraping_enable(const bool enabled);
	bool spi_stop();
	bool spi_read(uint16 addr, uint16 &data);
	bool spi_write(uint16 addr, uint16 data);
	bool disable_sleep_and_speed_up();  
    void raise_spi_error(const ichar *error_message);
    void raise_comms_error(const ichar *error_message);
    bccmd_trans_error_response wrap_raise_error(const char *error_string, const ichar * caption, const int error_code, const bccmd_trans_error_type error_type);
    bool read_slut();
    bool read_slut_slowly();
    bool retry_slut();
    void output_error_to_spi_log(const char *default_message);
    void output_hex_value_to_spi_log(const char *message, uint16 value);
    bool setup_access_to_chip();
    void escape_auto_baud_and_protocol_detection();
    bool hq_scraping();
    void enable_hq_scraping();
    void disable_hq_scraping();

    pttrans_stream_t mPtStream;
/*
struct bccmd_spi_interface_struct {
        bccmd_spi_cmd   cmd;
        uint16          size;
        uint16*         buffer;
} bccmd_spi_interface;
*/
	// This is the address of the 'bccmd_spi_interface_struct' structure
	uint16 bccmd_spi_interface_;
	uint16 slutid_id_string_location_;
	uint16 slutid_id_integer_location_;
	uint16 slutid_spi_user_cmd_;
	uint16 slutid_force_fast_clock_;
	uint16 slutid_uart_auto_baud_rate_;
	uint16 slutid_uart_auto_protocol_;

	bool connected_;

    // HQ scraping support
	uint16 slutid_hq_scraping_list_head_;
    bool mSupportsHQ;
    HQScraper * mpHQThread;
    BlueCoreDeviceController_newStyle::CallBackProvider* mpHqPduHandler;
    
    uint16 hw_bps_enabled;
};

// Thread object will be used to poll for HQ PDUs
class HQScraper : public Threadable
{
public:
    HQScraper(bccmd_spi * const bccmdSpi, BlueCoreDeviceController_newStyle::CallBackProvider* hqPduHandler,
                        uint16 hqListHeadAddr);
    virtual ~HQScraper();
    void SetInterval(uint16 const interval);
private:
    // Maximum time to sleep without checking for thread stop
    static const uint16 mMAX_SLEEP_TIME_MS;

    virtual int ThreadFunc();
    bool ScrapeHq();
    uint16 GetInterval();

    bccmd_spi * const mpBccmdSpi;
    BlueCoreDeviceController_newStyle::CallBackProvider* const mpPduHandler;

    // Poll interval is in milliseconds, stored as a multiple of the mMAX_SLEEP_TIME_MS
    // value and a remainder. This allows for fine poll interval control, and prevents
    // long waits for thread stop (due to many smaller sleeps whilst checking for thread stop).
    uint16 mPollMultiplier; // multiples of mMAX_SLEEP_TIME_MS
    uint16 mPollRemainder;  // remaining poll interval time to be added to 
                            // (mMAX_SLEEP_TIME_MS * mPollMultiplier), in milliseconds

    CriticalSection mIntervalGuard;
    uint16 mHqListHeadAddr;
};

#endif // BCCMD_SPI_INCLUDED
