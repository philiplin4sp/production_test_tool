///////////////////////////////////////////////////////////////////////////////
//
//  FILE    :   usb.h
//
//  Copyright CSR 2001-2006
//
//  CLASS   :   USBAbstraction
//
//  PURPOSE :   to provide a usb abstraction class to hide the csr and widcomm
//              implementations
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CSR_USB_ABSTRACTION_H_
#define _CSR_USB_ABSTRACTION_H_

#include "csrhci/transportapi.h"
#include "csrhci/physicaltransportconfiguration.h"
#include "common/nocopy.h"
#include "common/countedpointer.h"

class USBAbstraction : private NoCopy , public Watched , public Counter
{
public:
    USBAbstraction ( const USBConfiguration & aConfig )
    :   mConfig ( aConfig ),
        onFailureFunc(0),
        onFailureContext(0)
    {}
    virtual ~USBAbstraction () {}

    virtual bool open_connection () = 0;
    virtual void close_connection (void) = 0;

    virtual bool is_open (void) = 0;

	virtual bool size_forbidden(size_t) { return false; }
    virtual int send_cmd (void *data, size_t length) = 0;
    virtual int send_acl (void *data, size_t length) = 0;
    virtual int send_sco (void *buffer, size_t length) = 0;

    virtual void set_evt_callback (void (*func)(void *, size_t, void *), void *context) = 0;
    virtual void set_acl_callback (void (*func)(void *, size_t, void *), void *context) = 0;
    virtual void set_sco_callback (void (*func)(void *, size_t, void *), void *context) = 0;
    void set_fail_callback(void (*func)(FailureMode, void *), void *context)
    { onFailureFunc = func; onFailureContext = context; }
    void onFailure ( FailureMode f )
    {
        if(onFailureFunc)
            onFailureFunc(f,onFailureContext);
    }

    virtual bool set_sco_bandwidth(int bandwidth)
    {return false;}

protected:
    const USBConfiguration mConfig;
    void (*onFailureFunc) (FailureMode, void*);
    void *onFailureContext;
};

typedef CountedPointer<USBAbstraction> USBPtr;

#endif
