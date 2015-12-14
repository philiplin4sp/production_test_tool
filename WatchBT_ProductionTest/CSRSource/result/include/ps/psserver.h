// psserver.h 
// interface definition that all servers of persistent store will implement.
//
// 01:nov:2000    ckl   Created
// 16:jan:2001    ckl   Removed MAX_PS_VALUE_LEN
// 23:feb:01  Chris Lowe 	Now supports firmware versions.

#ifndef  PSSERVER_H_INCLUDED
#define  PSSERVER_H_INCLUDED

#include "pshelp_types.h"

#include "csrhci/bccmd_trans.h"
#include "assert.h"

// pure abstract base class defining the interface for the persistent store 
// This function uses data stored in spi format.

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PSHELP_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PSHELP_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#if defined(PSHELP_LIB) || !defined(WIN32)
#define PSHELP_API
#else
#ifdef PSHELP_EXPORTS
#define PSHELP_API __declspec(dllexport)
#else
#define PSHELP_API __declspec(dllimport)
#endif
#endif



enum ps_error_code {
	PS_ERROR_KEY_UNKNOWN = BCCMD_TRANS_ERROR_LAST
};

class PSHELP_API PsServer: public IMemoryReaderGP
{
public:
	// construction
	PsServer() : stores_(0), bootmode_(NoBootmode()), known_key_list_(NULL), keys_known_(false) {m_cancelled = false;}
	// destruction
	virtual ~PsServer();
	virtual bool psbc_size(pskey key, uint16 * const len, bool raw = false) = 0;
	// TRUE if the data item was found in the ps, else FALSE.  
	virtual bool psbc_set(pskey key, const uint16 * const data, const uint16 len, bool raw = false ) = 0;
	virtual bool psbc_get(pskey key, uint16 * const data, uint16 maxlen,
		uint16 * const len, bool raw = false ) = 0;
	virtual bool psbc_next(pskey key, pskey *next_key, bool raw = false) = 0;
    virtual bool psbc_next_all(pskey key, pskey *next_key) = 0;
	virtual bool psbc_clr(pskey key, bool raw = false ) = 0;
	virtual bool psbc_clrs(pskey key, bool raw = false ) = 0;
	virtual bool psbc_known_key(pskey key, bool raw = false ) = 0;
	virtual bool psbc_factory_set() = 0;
	virtual bool psbc_factory_restore(bool all ) = 0;
	virtual bool psbc_clrb(pskey key) = 0;
	virtual bool supports_stores() = 0;
	bool supports_bootmodes() { return NumBootmodes() > 0; }
	bool inquire_known_keys_from_mem();
	bool inquire_known_keys_from_psnextall();
	virtual bool inquire_known_keys() { return inquire_known_keys_from_mem(); }
	void copy_known_keys(PsServer *);
	virtual bccmd_trans_error_handler* get_error_handler() = 0;
	virtual bool reset_bc(reset_t reset_type) = 0; 
	virtual bool open() = 0;
	virtual bool close() = 0;
	virtual void set_error_handler(bccmd_trans_error_handler *error_handler) = 0;
	virtual bool get_firmware_id(int *const id, uint16 * nameBuffer, const size_t sNameBuffer) = 0;
	virtual bool read_mem(GenericPointer addr, uint16 len, uint16 * data) = 0;
	virtual bool get_slut(SlutTable *&apSlut, bool force_reread) = 0;

	virtual bool key_exists(pskey key);
	virtual bool keys_known() {return keys_known_;}
	pskey key_next(pskey key);

	virtual bool is_open() = 0;

    virtual void clear_cancel() {m_cancelled = false;}
    virtual void do_cancel() {m_cancelled = true;}
    virtual bool is_cancelled() {return m_cancelled;}
	bool get_connected() { return is_open() || open(); }

	virtual uint16 GetStores() {return stores_;};
	virtual void SetStores(uint16 stores) {stores_ = stores;};

	virtual uint16 NumBootmodes() = 0;
	virtual uint16 GetBootmode() {return bootmode_; }
	virtual void SetBootmode(uint16 bootmode)
	{
		if(bootmode >= 0 && bootmode < NumBootmodes())
			bootmode_ = bootmode;
		else
			bootmode_ = NoBootmode();
	}
	static uint16 NoBootmode() { return 0xFFFF; }

	virtual const char *getvar(const char *var) { return ""; }

protected:
	uint16 stores_; // the stores to be accessed.  By default, this is 0, indicating the default store.
	uint16 bootmode_;	// the bootmode to be accessed. By default, this is NoBootmode().

	class KeySet;
	KeySet * known_key_list_;
	bool keys_known_;
    bool m_cancelled;
};



class PSHELP_API PsBccmd: public PsServer
{
public:

	// construction
	PsBccmd(bccmd_trans *trans) : PsServer(), trans_(trans) {}
	// destruction
	virtual ~PsBccmd() {delete trans_; trans_ = 0;};
//	virtual int check_ids(const ichar *filename = "pstool.ids");
	bool psbc_size(pskey key, uint16 * const len, bool raw = false);
	// TRUE if the data item was found in the ps, else FALSE.  
	bool psbc_set(pskey key, const uint16 * const data, const uint16 len, bool raw = false );
	bool psbc_get(pskey key, uint16 * const data, uint16 maxlen,
		uint16 * const len, bool raw = false );
	bool psbc_next(pskey key, pskey *next_key, bool raw = false);
    bool psbc_next_all(pskey key, pskey *next_key);
	bool psbc_clr(pskey key, bool raw = false );
	bool psbc_clrs(pskey key, bool raw = false );
	bool psbc_known_key(pskey key, bool raw = false );
	bool psbc_factory_set(); 
	bool psbc_factory_restore(bool all );
	bool psbc_clrb(pskey key) { return false; }
	bool supports_stores();
	uint16 NumBootmodes();
	bool inquire_known_keys();
	bccmd_trans_error_handler* get_error_handler() {assert(trans_); return trans_->get_error_handler();}
	bool reset_bc(reset_t reset_type) {assert(trans_); return trans_->reset_bc(reset_type);}
	bool open()
	{
		assert(trans_);
		if(trans_->open())
		{
			if(inquire_known_keys()) return true;
			trans_->close();
		}
		return false;
	}
	bool close() {assert(trans_); return trans_->close();};
	void set_error_handler(bccmd_trans_error_handler *error_handler) {assert(trans_); trans_->set_error_handler(error_handler);}
	bool get_firmware_id(int *const id, uint16 * nameBuffer, const size_t sNameBuffer) {assert(trans_); return trans_->get_firmware_id(id, nameBuffer, sNameBuffer);}
	virtual bool read_mem(GenericPointer addr, uint16 len, uint16 * data) {assert(trans_); if (addr.Mem() != Mem_RawData) return false; else return trans_->read_mem(addr, len, data);}
	virtual bool get_slut(SlutTable *&apSlut, bool force_reread) { assert(trans_); return trans_->get_slut(apSlut, force_reread); }

	bool is_open();

	bccmd_trans *GetTrans() {return trans_;}
	void SetTrans(bccmd_trans *trans) {trans_ = trans;}

	const char *getvar(const char *var);
protected:
	bccmd_trans *trans_;
};

class PSHELP_API PsBccmdRaw: public PsBccmd
{
public:
    // construction
    PsBccmdRaw(bccmd_trans *trans) : PsBccmd(trans) {}
    bool psbc_size(pskey key, uint16 * const len, bool raw = false) { return PsBccmd::psbc_size(key, len, true); }
    // TRUE if the data item was found in the ps, else FALSE.  
    bool psbc_set(pskey key, const uint16 * const data, const uint16 len, bool raw = false ) { return PsBccmd::psbc_set(key, data, len, true); }
    bool psbc_get(pskey key, uint16 * const data, uint16 maxlen,
        uint16 * const len, bool raw = false ) { return PsBccmd::psbc_get(key, data, maxlen, len, true); }
    bool psbc_next(pskey key, pskey *next_key, bool raw = false) { return PsBccmd::psbc_next(key, next_key, true); }
    bool psbc_clr(pskey key, bool raw = false ) { return PsBccmd::psbc_clr(key, true); }
    bool psbc_clrs(pskey key, bool raw = false ) { return PsBccmd::psbc_clrs(key, true); }
    bool psbc_known_key(pskey key, bool raw = false ) { return psbc_known_key(key, true); }
};

#endif // PSSERVER_H_INCLUDED

