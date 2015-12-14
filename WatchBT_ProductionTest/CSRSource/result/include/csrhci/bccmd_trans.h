///////////////////////////////////////////////////////////////////////////////
//
//  FILE:       bccmd_trans.h
//
//  Copyright Cambridge Silicon Radio 2003-2006
//
//  PURPOSE:    Declare the API for the BCCMD Transport library:
//                  the bccmd_trans class.
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/bccmd_trans/bccmd_trans.h#1 $
//
///////////////////////////////////////////////////////////////////////////////

#ifndef BCCMD_TRANS_INCLUDED
#define BCCMD_TRANS_INCLUDED

#include "unicode/ichar.h"
#include "common/types.h"
#include "abstract/memory_reader.h"

#ifdef WIN32
#include "windows.h"
#endif

#define BCCMD_TRANS_API 

#ifndef ASSERT
#include "assert.h"
#define ASSERT assert
#endif

#include "hcipacker/hcipacker.h"
#include "csrhci/bluecontroller.h"
class SlutTable;
class CXapHelper;
#include <memory>

enum bccmd_trans_error_code {
	BCCMD_TRANS_ERROR_MESSAGE,
	BCCMD_TRANS_ERROR_KEY_UNKNOWN,
	BCCMD_TRANS_ERROR_CONNECTION,
	BCCMD_TRANS_ERROR_OPERATION,
	BCCMD_TRANS_ERROR_ACCESS,
	BCCMD_TRANS_ERROR_PERMISSIONS,
	BCCMD_TRANS_ERROR_LAST
} ;

enum bccmd_trans_error_response{
	BCCMD_TRANS_ERROR_CONTINUE = 0,
	BCCMD_TRANS_ERROR_NONE = 0,
	BCCMD_TRANS_ERROR_ABORT = 1,  // Die, and kill that which called you.
	BCCMD_TRANS_ERROR_RETRY = 2,	 // Retry the operation that gerenated the error
	BCCMD_TRANS_ERROR_IGNORE = 4	 // Fail, and notify the operation that spawned you.
};

#ifndef WIN32
#define MB_OK 0
#define MB_ABORTRETRYIGNORE 1
#define MB_OKCANCEL 2
#endif
enum bccmd_trans_error_type{
	BCCMD_TRANS_ERRTYPE_MESSAGE = -1,
	BCCMD_TRANS_ERRTYPE_ABORT = MB_OK,  // This is not going to work.  Abort.
	BCCMD_TRANS_ERRTYPE_ARI = MB_ABORTRETRYIGNORE, // A retry might work Abort, Retry, Fail.
	BCCMD_TRANS_ERRTYPE_OK_CANCEL = MB_OKCANCEL // A retry won't work. Abort, Continue.
};

enum reset_t
{
	RESET_NONE,
	RESET_WARM,
	RESET_COLD
};



class BCCMD_TRANS_API bccmd_trans_exception
{
public:
	bccmd_trans_exception(const ichar *error_str, const ichar *caption_str, const int code,const bccmd_trans_error_type error_type);
	~bccmd_trans_exception();
	const ichar *error_str();
	const ichar *caption_str();
	int m_code; 
	bccmd_trans_error_type m_type;
private:
	ichar * error_str_;
	ichar * caption_str_;
};

//*******************************************
// The base error handler class for bccmd_trans
//
// The base class handles storing or errors and accumulating an overall rtesponse.
// Derived classes should override on_error_extra to do any extra behaviour, like showing
// messages and errors to the user and querying them about what to do next.
//*******************************************
class BCCMD_TRANS_API bccmd_trans_error_handler
{
protected:
	void store_last_error(const ichar *error_string, const int error_code, bccmd_trans_error_type error_type);
	void add_to_overall_response(bccmd_trans_error_response resp);
	ichar *last_error_string_;
	int last_error_code_;
	bccmd_trans_error_type last_error_type_;
	bccmd_trans_error_response last_response_;
	bccmd_trans_error_response overall_response_;

	//***************************************
	// the default implementation of on_error_extra.
	// returns ABORT for errors and CONTINUE for messages without asking the user.
	// derived classes should override this function to add extra behaviour on errors and messages
	//***************************************
	virtual bccmd_trans_error_response on_error_extra(const ichar *error_string, const ichar * caption, const int error_code, const bccmd_trans_error_type error_type)
	{
		return error_type == BCCMD_TRANS_ERRTYPE_MESSAGE || error_code == BCCMD_TRANS_ERROR_MESSAGE ?
		       BCCMD_TRANS_ERROR_CONTINUE :
		       BCCMD_TRANS_ERROR_ABORT;
	}

public:
	bccmd_trans_error_handler();
	virtual ~bccmd_trans_error_handler();
	__inline bccmd_trans_error_response on_error(const ichar *error_string, const ichar * caption, const int error_code, const bccmd_trans_error_type error_type);
	const ichar* get_last_error_string() const;
	const int get_last_error_code() const;
	bccmd_trans_error_type get_last_error_type() const;
	bccmd_trans_error_response get_last_response() const;
	bccmd_trans_error_response get_overall_response() const;
	void clear_errors();
};

//**************************************************
// raise an error. returns the behaviour to follow.
//
// this method handles storing the error and updating the response.
// It calls on_error_extra to perform any actions the derived class wants.
//**************************************************
bccmd_trans_error_response bccmd_trans_error_handler::on_error(const ichar *error_string, const ichar * caption, const int error_code, const bccmd_trans_error_type error_type)
{
    bool stored = false;
    // if it's not a message store the error
    if (error_type != BCCMD_TRANS_ERRTYPE_MESSAGE)
    {
        stored = true;
        store_last_error(error_string, error_code, error_type);
        // if on_error_extra doesn't return (throws or terminates the thread) assume it meant this.
        last_response_ = BCCMD_TRANS_ERROR_ABORT;
    }
    bccmd_trans_error_response resp = on_error_extra(error_string, caption, error_code, error_type);
    // if we stored the error then also store the response.
    if (stored)
    {
        last_response_ = resp;
    }
    // always add to the overall response.
    add_to_overall_response(resp);

    return resp;
}

#ifdef WIN32
class BCCMD_TRANS_API bccmd_trans_win_mbox_error_handler: public bccmd_trans_error_handler
{
public:
	bccmd_trans_win_mbox_error_handler(HWND parent_hwnd);
	HWND parent_hwnd_;
	virtual bccmd_trans_error_response on_error_extra(const ichar *error_string, const ichar * caption, const int error_code, const bccmd_trans_error_type error_type);
};
#endif

class BCCMD_TRANS_API bccmd_trans_throw_error_handler: public bccmd_trans_error_handler
{
public:
	virtual bccmd_trans_error_response on_error_extra(const ichar *error_string, const ichar * caption, const int error_code, const bccmd_trans_error_type error_type);
};




class BCCMD_TRANS_API bccmd_trans: public IMemoryReaderGP
{
public:
	static bccmd_trans *create_transport(const char *trans, bccmd_trans_error_handler *error_handler, int mul = -1);

	struct trans_suggestion
	{
		char *value;
		char *description;
	};

	static trans_suggestion *suggest_var(const char *trans);
	static void suggest_var_free(trans_suggestion *mem);
	static trans_suggestion **suggest_value(const char *trans, const char *var);
	static void suggest_value_free(trans_suggestion **mem);

	bccmd_trans(bccmd_trans_error_handler *handler);
	virtual ~bccmd_trans();
	virtual bool reset_bc(reset_t reset_type) = 0; 
	virtual bool reset_and_close(reset_t reset_type) = 0;
	virtual bool get_firmware_id(int *const id, uint16 * nameBuffer, const size_t sNameBuffer);
	virtual bool read_mem(GenericPointer addr, uint16 len, uint16 * data);
	virtual bool get_slut(SlutTable *&apSlut, bool force_reread);

	BCCMD_PDU run_pdu_report_errs(const BCCMD_PDU& p);
	bool run_pdu_report_errs(BCCMDPDU*p);
	virtual int GetNumOperationAttempts() = 0;

	virtual bool open(BlueCoreDeviceController_newStyle::CallBackProvider* zpHQPDUHandler = 0) = 0;
	virtual bool close() = 0;
	virtual bool IsConnected() const = 0;

	virtual const char *getvar(const char *var) { return ""; }

	bccmd_trans_error_response raise_error(const ichar *error_string, const ichar * caption, const int error_code, const bccmd_trans_error_type error_type);
	virtual bccmd_trans_error_handler *get_error_handler(void);
	void set_error_handler(bccmd_trans_error_handler *error_handler);

    typedef enum
    {
        SECURITY_UNKNOWN,
        SECURITY_ENABLED,
        SECURITY_DISABLED,
    } SecurityStateEnum;
    SecurityStateEnum detectSecurity();

    // HQ Scraping support (bccmd_spi)
    virtual bool supportsHQ() const = 0;
    virtual void setHqScrapeInterval(const uint16 interval) = 0;

    virtual void Debug(const char *aFmt, ...) {}
    virtual void DebugFull(const char *aFmt, ...) {}

protected:
	virtual bccmd_trans_error_response run_pdu(const BCCMD_PDU& p, BCCMD_PDU& resp) = 0;
	virtual bccmd_trans_error_response send_pdu(const BCCMD_PDU& p) = 0;

    virtual bool get_xap_helper(CXapHelper *&apXapHelper, bool force_reread);

    // should be called by the derived class when it closes a connection.
    void OnClose();

    SecurityStateEnum mSecurityEnabled;

private:
    bccmd_trans_error_response do_request(const BCCMD_PDU&p, BCCMD_PDU &resp, bccmd_trans_error_handler *error_handler);
    bool check_status(BCCMD_PDU &resp);

    bccmd_trans_error_handler *error_handler_;
    bccmd_trans_error_handler m_default_error_handler;

    // The following is used to save a first error message until retries have failed
    const ichar *m_error_message;

    std::auto_ptr<CXapHelper> mp_xap_helper;
    std::auto_ptr<SlutTable> mp_slut;
};

#endif // BCCMD_TRANS_INCLUDED

