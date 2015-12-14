#ifndef  PSSTORE_H_INCLUDED
#define  PSSTORE_H_INCLUDED

#include "psserver.h"

class PSHELP_API PsCache : public PsServer
{
public:
	PsCache();
	virtual ~PsCache();

	void SetServer(PsServer *server)
	{
		server_ = server;
		set_error_handler(server_->get_error_handler());
	}
	PsServer *GetServer() const { return server_; }

	void UseCache(bool use_cache);

	void UseServer();
	void ReleaseServer();

	struct CallbackResult
	{
		pskey key;
		uint16 store;
		uint16 bootmode;
		bool raw;
	};

	typedef void (*Callback)(void *context, const CallbackResult &);

	bool get(pskey key, Callback callback, void *context, bool raw = false);

	virtual bool psbc_size(pskey key, uint16 * const len, bool raw = false);
	// TRUE if the data item was found in the ps, else FALSE.  
	virtual bool psbc_set(pskey key, const uint16 * const data, const uint16 len, bool raw = false );
	virtual bool psbc_get(pskey key, uint16 * const data, uint16 maxlen,
		uint16 * const len, bool raw = false );
	virtual bool psbc_known_key(pskey key, bool raw = false );
	virtual bool psbc_next(pskey key, pskey *next_key, bool raw = false);
    virtual bool psbc_next_all(pskey key, pskey *next_key);
	virtual bool psbc_clr(pskey key, bool raw = false );
	virtual bool psbc_clrs(pskey key, bool raw = false );
	virtual bool psbc_clrb(pskey key);
	virtual bool supports_stores() { return true; }
	virtual uint16 NumBootmodes();
	virtual bool open();
	virtual bool close();
	virtual bool psbc_factory_set();
	virtual bool psbc_factory_restore(bool all);
	virtual void set_error_handler(bccmd_trans_error_handler *error_handler);
	virtual bccmd_trans_error_handler* get_error_handler();
	virtual bool reset_bc(reset_t reset_type);
	virtual bool get_firmware_id(int *const id, uint16 * nameBuffer, const size_t sNameBuffer);
	virtual bool read_mem(GenericPointer addr, uint16 len, uint16 * data);
	virtual bool get_slut(SlutTable *&apSlut, bool force_reread);

	virtual bool key_exists(pskey key);
	virtual bool keys_known();

    virtual void do_cancel() {m_cancelled = true;}
    virtual bool is_cancelled() {return m_cancelled;}

	virtual bool inquire_known_keys();

	virtual const char *getvar(const char *var) { return server_ ? server_->getvar(var) : ""; }

	bool is_open() { return server_ && server_->is_open(); }

	void delete_child_on_destruct(bool delete_server) {delete_server_ = delete_server; }

	void ClearCache(pskey key, uint16 store);
	void ClearCache();

protected:
	bool delete_server_;
	PsServer *server_;

	void PauseThread();
	void ResumeThread();

	class Impl;
	friend class Impl;
	Impl *impl_;
};

#endif
