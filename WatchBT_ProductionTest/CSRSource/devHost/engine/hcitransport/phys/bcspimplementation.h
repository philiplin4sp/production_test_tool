/****************************************************************************

  Copyright (C) Cambridge Silicon Radio Ltd 2000-2009

FILE
    bcspimplementation.h  -  wrapper around bcsp stack

PURPOSE
    A container for the BCSP stack, which keeps tabs on all the data
    and interfaces necessary for it to run.

****************************************************************************/



#ifndef _BCSPIMPLEMENTATION_H_
#define _BCSPIMPLEMENTATION_H_

#include "common/linkage.h"

#include "../bcsp/bcspstack.h"
#include "uart.h"
#include "thread/thread.h"
#include "thread/signal_box.h"
#include "thread/critical_section.h"

class BCSPImplementation : public Threadable
{
public : //  which is of course totally platform independant... yeah right.
    BCSPImplementation( UARTPtr aFile ) ;
    ~BCSPImplementation() ;
    BCSPStack * getBCSPStack() {return mStack;}
    bool connect(uint32 timeout) ;
    bool Start() ;
    void Stop() ;

    void setOnLinkFailSync(void (*fn)(void*),void* param) ;
    void setOnLinkFailRetries(void (*fn)(void*),void* param) ;
    void setOnLinkFailUart(void (*fn)(void*),void* param) ;
    static void onLinkFailSync ( void * envState ) ;
    static void onLinkFailRetries ( void * envState ) ;
    void onLinkFailUart() ;

    void setDataWatch ( DataWatcher * w );
private: 
    //  a com port
    UARTPtr mFile;
    //  The Rest (TM).
    int ThreadFunc();
    BCSPStack * mStack ;
    BCSPEnvironment mBCSPenv ;
    static void * allocMem ( void * envState , uint32 size ) ;
    static void freeMem ( void * envState , void * tofree ) ;
    CriticalSection cs;
    CriticalSection::Lock * mLockCS;
    static void enterCS ( void * envState ) ;
    static void leaveCS ( void * envState ) ;
    static void signal ( void * envState ) ;
    static void releasePacket(void *envState, Packet *pkt);

    void sleep ( uint32 duration );
    // a millisecond clock (of unknown granularity)
    uint32 ms_clock();

    bool mXMITBusy ;
    bool mRCVBusy ;
    uint32 mXMITBytesAvailable ;
    uint32 mRCVBytesAvailable ;
    uint8 mXMITBuffer[BYTE_BUFFERSIZE] ;
    uint8 mRCVBuffer[BYTE_BUFFERSIZE] ;
    uint32 mXMITbytesWritten ;
    uint32 mLastXmitTime ;

    bool mStackMustDie ;
    SignalBox mSignals;
    SignalBox::Item mStackDead ;
    uint8 startIO() ;

    void wait( uint32 wakeupTime ) ;

    void createBCSPStack() ;
    void runStack();

    void (* userOnLinkFailSync)(void*) ;
    void * userOnLinkFailParamSync ;
    void (* userOnLinkFailRetries)(void*) ;
    void * userOnLinkFailParamRetries ;
    void (* userOnLinkFailUart)(void*) ;
    void * userOnLinkFailParamUart ;

    enum { NUM_PACKETS = 10 };
    Packet * Packets[NUM_PACKETS];
} ;

#endif
