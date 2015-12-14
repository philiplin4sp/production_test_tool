///////////////////////////////////////////////////////////////////////////////
//
//  FILE:       bccmd_hci.h
//
//  Copyright Cambridge Silicon Radio 2003-2006
//
//  PURPOSE:    Declare the underlying hci implementation of the
//              BCCMD Transport library:
//                  the bccmd_hci class.
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/bccmd_trans/bccmd_hci.h#1 $
//
///////////////////////////////////////////////////////////////////////////////

#ifndef BCCMD_HCI_INCLUDED
#define BCCMD_HCI_INCLUDED

#ifndef NDEBUG
#define PRINT_DATA
#endif

#ifdef PRINT_DATA
#define dwfilename "C:/data.txt"
#endif

#include "bccmd_trans.h"
#include "csrhci/bluecontroller.h"
#include "thread/signal_box.h"
#include "thread/critical_section.h"

// This class is exported from the PsSpiBccmd.dll
class BCCMD_TRANS_API bccmd_hci_base : public bccmd_trans
#ifdef PRINT_DATA
, public DataWatcher
#endif
{
public:
	bccmd_hci_base(bccmd_trans_error_handler *handler);
	virtual ~bccmd_hci_base();
	bool requires_permissions();
	virtual bool reset_bc(reset_t reset_type); 
	virtual bool reset_and_close(reset_t reset_type);
	virtual int GetNumOperationAttempts();
	virtual bool IsConnected() const = 0;

    virtual bool supportsHQ() const { return true; };
    virtual void setHqScrapeInterval(const uint16 interval) { };

protected:
    bccmd_trans_error_response run_pdu (const BCCMD_PDU& p, BCCMD_PDU& resp);
	bccmd_trans_error_response send_pdu(const BCCMD_PDU& p);
	bool transport_error_condition_;
    SignalBox pdu_completed;
    SignalBox::Item transport_died;
    CriticalSection prot_sb;
    virtual BlueCoreDeviceController_newStyle* dev_con() = 0;

	void clear_transport_error();
private:
#ifdef PRINT_DATA
    void onSend (const uint8*, size_t);
    void onRecv (const uint8*, size_t);
#endif
};

class BCCMD_TRANS_API bccmd_hci : public bccmd_hci_base , public BlueCoreDeviceController_newStyle::CallBackProvider
{
public:
    bccmd_hci(bccmd_trans_error_handler *handler, TransportConfiguration *trans);
    virtual ~bccmd_hci();
    virtual bool open(BlueCoreDeviceController_newStyle::CallBackProvider* hqPduHandler = 0);
	virtual bool close();
	virtual bool IsConnected() const;

private:
    virtual BlueCoreDeviceController_newStyle* dev_con();
	void onPDU ( const PDU& );
	void onTransportFailure( FailureMode f );
	TransportConfiguration *trans_;
	BlueCoreDeviceController_newStyle *dev_con_;

public:
	static unsigned long USBDeviceIterate(unsigned long prev = 0xFFFFFFFF);
	static size_t USBDeviceName(unsigned long dev, char *buf, size_t buflen);
	static size_t USBDeviceName(unsigned long dev, wchar_t *buf, size_t buflen);
};

class BCCMD_TRANS_API bccmd_hci_ref : public bccmd_hci_base
{
public:
    bccmd_hci_ref(bccmd_trans_error_handler *handler, BlueCoreDeviceController_newStyle *dev_con_);
    ~bccmd_hci_ref(void);
    void set_dev_con(BlueCoreDeviceController_newStyle *dev_con_);
    virtual bool open(BlueCoreDeviceController_newStyle::CallBackProvider* hqPduHandler = 0);
    virtual bool close();
	virtual bool IsConnected() const;
private:
	BlueCoreDeviceController_newStyle *dev_con_;
    virtual BlueCoreDeviceController_newStyle* dev_con();
};

#endif // BCCMD_HCI_INCLUDED


