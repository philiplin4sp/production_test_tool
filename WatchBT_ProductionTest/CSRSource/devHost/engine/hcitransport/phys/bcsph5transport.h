///////////////////////////////////////////////////////////////////////
//
//  FILE   :  BCSPH5Transport.h
//
//  Copyright CSR 2002-2006
//
//  AUTHOR :  Mark MArshall
//
//  CLASS  :  BCSPH5Transport
//
//  PURPOSE:  Transparent class inheriting top API from the
//            HCITransport class, and owning a H5Stack which does
//            all the work.
//
//            Start-up sequence:
//              stack = new BCSPH5Transport(H5Configuration);
//              stack->start();
//
//  $Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/phys/bcsph5transport.h#1 $
//
//  MODIFICATION HISTORY:
//
///////////////////////////////////////////////////////////////////////

#ifndef BCSPH5_TRANSPORT_H__
#define BCSPH5_TRANSPORT_H__

#include "transportimplementation.h"
#include "uart.h"
#include "bcsph5stack.h"


// The BCSPH5Transport uses multiple inheritence.  The second class that we derive
// from 'CallBackInterface' is only used to provide a Java-'interface' like
// functionality.
class BCSPH5Transport : public BlueCoreTransportImplementation, public BCSPH5Stack::CallBackInterface
{
public:
    BCSPH5Transport(const BCSPConfiguration& , UARTPtr , const CallBacks& );
    virtual ~BCSPH5Transport();

	// Control the H5Stack
    virtual bool start(void);
    virtual bool ready(uint32 timeout);
    virtual void stop();

	// Hook to enable debug of UART
    virtual void setDataWatch(DataWatcher *w);

	/////////////////////////////////////////////////////////////////////////
	// NAME
	//  SleepControl  -  Control when the transport lets the BlueCore sleep
	//
	//virtual bool SleepControl(enum SleepType type);

	/////////////////////////////////////////////////////////////////////////
	// NAME
	//  PingPeriod  -  Control when the transport pings the BlueCore
	//
	//virtual bool PingPeriod(int millisecs);

	// These are the callback functions from the H5Stack.
	// H5Stack::CallBackInterface
protected:
	virtual void sentCommand(bool);
	virtual void sentACLData(bool);
	virtual void sentSCOData(bool);
	virtual void sentEvent  (bool);
	virtual void sentOther  (bool);

	virtual void recvCommand(const PDU &aPacket);
	virtual void recvACLData(const PDU &aPacket);
	virtual void recvSCOData(const PDU &aPacket);
	virtual void recvEvent  (const PDU &aPacket);
	virtual void recvOther  (const PDU &aPacket);

	virtual void linkFailed(FailureMode mode);

private:

	// The actual stack object
    BCSPH5Stack *mStack;

	// The function that gets called to make us do some work
    virtual void sendpdu(const PDU &aPacket);
};

#endif /* BCSPH5_TRANSPORT_H__ */
