///////////////////////////////////////////////////////////////////////
//
//  FILE   :  H4DSTransport.h
//
//  Copyright CSR 2003-2006
//
//  AUTHOR :  Mark Marshall
//
//  CLASS  :  H4DSTransport
//
//  PURPOSE:  transparent class inheriting top API from the
//            BlueCoreTransport class, and owning a H4Stack which does
//            all the work.
//
//            BlueCoreTransport also provides BCSP-specific private channels
//            L2CAP etc which are tunneled through HCI vendor specific
//            commands and puts the debug channel messages to a file
//            in the debug build version.
//
//  MODIFICATION HISTORY:
//
//  $Id: $
//
///////////////////////////////////////////////////////////////////////

#ifndef H4DSTRANSPORT_H__
#define H4DSTRANSPORT_H__

#include "transportimplementation.h"
#include "h4dsstack.h"

class H4DSTransport : public BlueCoreTransportImplementation, public H4DSStack::CallBackInterface
{
public:
    H4DSTransport(const H4DSConfiguration &, UARTPtr, const CallBacks &);
    virtual ~H4DSTransport();

    virtual bool start(void);
    virtual bool ready(uint32 timeout);
    virtual void stop();

    virtual void setDataWatch(DataWatcher *w);

private:
    virtual void sendpdu(const PDU &aPacket);

	// These are the callback fundtion from H4DSStack::CallBackInterface
	virtual void sentPacket(bool);
	virtual void recvPacket(const PDU &aPacket);
	virtual void syncLost(FailureMode mode);

    H4DSStack *mStack;
};

#endif // H4DSTRANSPORT_H__
