///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) Cambridge Silicon Radio April 2000-2006
//
//  FILE    :   bcspimplementation_core.cpp  -  wrapper for BCSP stack
//
///////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <cstdlib>
#include <cstring>
#include "../bcsp/bcspstack.h"
#include "bcspimplementation.h"


BCSPImplementation::~BCSPImplementation()
{
    // Stop the stack
    Kill();
    BCSPshutdownStack(mStack) ;

    //free up bcsp packet headers...
    for (unsigned int c = 0; c < NUM_PACKETS; c++)
        if (Packets[c])
            delete Packets[c];

    //free the stack
    free(mStack);
}


BCSPImplementation::BCSPImplementation ( UARTPtr aFile )
:
    mFile ( aFile ),
    mLockCS ( 0 ),
    mXMITBusy ( false ),
    mRCVBusy ( false ),
    mXMITBytesAvailable ( 0 ),
    mRCVBytesAvailable ( 0 ),
    mStackMustDie ( false ),
    mStackDead ( mSignals ),
    userOnLinkFailSync ( 0 ),
    userOnLinkFailRetries ( 0 )
{
    memset(Packets,0,sizeof(Packets)) ;
    //create the initial stack state
    createBCSPStack() ;
    mBCSPenv.allocMem = allocMem;
    mBCSPenv.freeMem = freeMem;
    mBCSPenv.enterCS = enterCS;
    mBCSPenv.leaveCS = leaveCS;
    mBCSPenv.signal = signal;
    mBCSPenv.onLinkFailureSync = onLinkFailSync;
    mBCSPenv.onLinkFailureRetries = onLinkFailRetries;
    mBCSPenv.clockMask = 0xFFFFFFFF;
    // call back to reverse BCSPaddPacket below
    mBCSPenv.releasePacket = releasePacket; 
    mBCSPenv.envState = this;
    setEnvironment(mStack,&mBCSPenv) ;
}

bool BCSPImplementation::Start()
{
    mStackMustDie = false;
    return mFile->open() && Threadable::Start();
}
    
void BCSPImplementation::setDataWatch ( DataWatcher * w )
{
    mFile->setDataWatch ( w );
}

int BCSPImplementation::ThreadFunc()
{
    if (!mStack->configuration->useLinkEstablishmentProtocol)
        mStack->isUnchoked = true;
    mStackDead.unset();
    runStack();
    mStackDead.set(); //we've exited
    return 0;
}

void BCSPImplementation::createBCSPStack()
{
    mStack = (BCSPStack * ) calloc(1,sizeof(BCSPStack)) ;
    initialiseStack(mStack) ;
    BTRACE0(WINERR,"WINERR debug enabled\n") ;
    //add a few packets...
    for (unsigned int c = 0; c < NUM_PACKETS; c++)
    {
        Packet * pkt = new Packet;
        BCSPaddPacket(mStack, pkt);
        // keep track of them so that we can delete them later.
        Packets[c] = pkt;
    }
}

void BCSPImplementation::runStack()
{
    //here's where we actually run round in a loop calling the scheduler...
    while(!mStackMustDie)
    {
        uint32 timeNow = ms_clock() ;
        uint32 wakeupTime = scheduler(mStack,timeNow) ;
        //now see if we can do anything with the uart...
        //try sending ...
        if(!mXMITBusy) 
        {
            //copy stuff out of stack output buffer 
            mXMITBytesAvailable = numBytesInTransmitBuffer(mStack) ;
            if (mXMITBytesAvailable)
            {
                readFromTransmitBuffer(mStack,mXMITBuffer, mXMITBytesAvailable) ;
                if (mXMITBytesAvailable) 
                {
                    BTRACE0(IO,"-->") ;
                    for (unsigned int c = 0 ; c < mXMITBytesAvailable ; c++)
                        PLAINBTRACE1(IO," %02x",mXMITBuffer[c]) ;
                    PLAINBTRACE0(IO,"\n") ;
                }
            }
        }

        //now startup I/O  - non-zero indicates that the read or write has returned immediately
        if (!startIO() && isStackIdle(mStack))
                wait(wakeupTime) ;

        //now try loading the stack receive buffer
        if (mRCVBytesAvailable &&(mRCVBytesAvailable <= numFreeSlotsInReceiveBuffer(mStack)))
        {
            if (mRCVBytesAvailable) 
            {
                BTRACE0(IO,"<--") ;
                for (unsigned int c = 0 ; c < mRCVBytesAvailable ; c++) PLAINBTRACE1(IO," %02x",mRCVBuffer[c]) ;
                PLAINBTRACE0(IO,"\n") ;
            }
            writeToReceiveBuffer(mStack,mRCVBuffer,(uint8) mRCVBytesAvailable) ;
            mRCVBytesAvailable = 0 ;
        }
    }
    //we'll return if we get the stackmustdie signal
} 

uint8 BCSPImplementation::startIO()
{
    uint8 retval = 0 ;
    int  err ;
    if ((!mXMITBusy) && (mXMITBytesAvailable))
    {
        mXMITBusy = true ;
        mLastXmitTime = ms_clock() ;
        if ( mFile->write(mXMITBuffer,mXMITBytesAvailable,&mXMITbytesWritten) )
        {
            //function succeeded immediately
            mXMITBusy = false ;
            retval =1 ;
        }
        else if ( ( err = mFile->LastError() ) )
        {
            mXMITBusy = false ;
            BTRACE1(WINERR,"WriteFile error %d\n",err) ;
            BTRACE1(WINERR,"bytesavail = %d",mXMITBytesAvailable);
            BTRACE1(WINERR,"byteswritten = %d",mXMITbytesWritten);
        }
    }

    if ((!mRCVBusy)  && (!mRCVBytesAvailable))
    {        
        //Now try to read some chars from the uart.
        uint16 bytesToGet = numFreeSlotsInReceiveBuffer(mStack) ;
        mRCVBusy = true ;
        if ( mFile->read( mRCVBuffer , bytesToGet , &mRCVBytesAvailable ) )
        {
            mRCVBusy = false ;
            retval =1 ;
        }
        else if ( (err = mFile->LastError()) )
        {
            mRCVBusy = false ;
            BTRACE1(WINERR,"ReadFile error %d\n",err) ;
        }
    }

    return retval ; //indicate that we need to wait for some io to complete or not
}

#define WRITEFILE_PERIOD 500

void BCSPImplementation::wait(uint32 wakeupTime)
{
    //this function just puts the stack thread into idle until some
    //event (or timeout) wakes us up.
    int32 timeout = wakeupTime - ms_clock();
    if (timeout <= 0)
        return;

    // There appears to be a bug in the WriteFile
    // implementation under Win98.  Occasionally, the WriteFile never
    // sets the completion event.  Close down and then reopen the com port 
    // if the writefile hasn't completed after a certain period.
    if (mXMITBusy && (ms_clock() - mLastXmitTime > WRITEFILE_PERIOD)) 
    {
        BTRACE3(WINERR,"Cancelling IO at time %d (%d ms from %d)\n", ms_clock(), WRITEFILE_PERIOD, mLastXmitTime);
        mLastXmitTime = ms_clock() ;
        mXMITBusy = false;
        mRCVBusy = false;
        mFile->reopen();
        return;
    }
    //on with the real code...

    switch ( mFile->wait ( timeout , &mRCVBytesAvailable ) )
    {
    case UARTAbstraction::UART_XMITDONE : //This is the transmit-finished - all we need to do is reset the event
        mXMITBusy = false ; 
        BTRACE0(USER0,"TX DONE\n") ;
        break ; //allow xmit to go ahead 
    case UARTAbstraction::UART_RCVDONE: //This is a receive from the uart 
        mRCVBusy = false ;
        BTRACE0(USER0,"RX DONE\n") ;
        break ;
    case UARTAbstraction::CLIENT_TXREQ:
        BTRACE0(USER0,"TX REQUEST\n") ;
        break ;//write req - no action necessary
    case UARTAbstraction::CLIENT_DIEREQ: //the user has requested that we shutdown
        BTRACE0(USER0,"DIE REQUEST\n") ;
        break ;
    case UARTAbstraction::timedOut:
        BTRACE0(USER0,"WAIT TIMEOUT\n") ;
        break;
    case UARTAbstraction::UART_ERROR:
        onLinkFailUart();
        BTRACE0(USER0,"UART ERROR\n") ;
        break;
    default :
        BTRACE0(USER0,"BAD WAIT VALUE\n") ;
        break ;//error!
    }
}

bool BCSPImplementation::connect(uint32 timeout)
{
    //wait for timeout or until we have link-establishment
    int32 timeoutTime = ms_clock() + timeout ;
    do
    {
        if (BCSPLinkEstablished(mStack))
            return true ;
        sleep(10) ;
    } while ( (int32)(timeoutTime-ms_clock()) > 0) ; 
    return false ;
}

void BCSPImplementation::Stop()
{
    // kill the BCSP stack thread
    if ( IsActive() )
    {
        Threadable::Stop();
        mStackMustDie = true;
        mFile->abortWait();
        mStackDead.wait();
    }
}

void BCSPImplementation::onLinkFailSync( void * aThis )
{
    // alert the user
    BCSPImplementation * lThis = (BCSPImplementation *) aThis;
    if (lThis->userOnLinkFailSync)
        lThis->userOnLinkFailSync ( lThis->userOnLinkFailParamSync ) ;
}

void BCSPImplementation::onLinkFailRetries( void * aThis )
{
    // alert the user
    BCSPImplementation * lThis = (BCSPImplementation *) aThis;
    if (lThis->userOnLinkFailRetries)
        lThis->userOnLinkFailRetries ( lThis->userOnLinkFailParamRetries ) ;
}

void BCSPImplementation::onLinkFailUart()
{
    // alert the user
    if (userOnLinkFailUart)
        userOnLinkFailUart ( userOnLinkFailParamUart ) ;
}

void BCSPImplementation::setOnLinkFailSync(void (*fn)(void*),void * param)
{
    userOnLinkFailSync = fn ;
    userOnLinkFailParamSync = param ;
}

void BCSPImplementation::setOnLinkFailRetries(void (*fn)(void*),void * param)
{
    userOnLinkFailRetries = fn ;
    userOnLinkFailParamRetries = param ;
}

void BCSPImplementation::setOnLinkFailUart(void (*fn)(void*),void * param)
{
    userOnLinkFailUart = fn ;
    userOnLinkFailParamUart = param ;
}

#ifdef SIMPLE_MEMORY_MARKERS
static uint16 gMemAllocCount = 0;
#endif

#define MEM_MAGIC_LONG 0x7fa569cc

void * BCSPImplementation::allocMem ( void * envState , uint32 size ) 
{
#ifdef SIMPLE_MEMORY_MARKERS
    uint8 *mem = new uint8[size+4];
    mem[0] = 0xFF;
    mem[3] = 0xA5;
    *((uint16*)(mem+1)) = gMemAllocCount;
    return mem + 4;
#else
    uint8 *mem = new uint8[size + sizeof(long)];
    *((unsigned long *)mem) = MEM_MAGIC_LONG;
    return mem + sizeof(long);
#endif
}

void BCSPImplementation::freeMem ( void * envState , void * tofree ) 
{
#ifdef SIMPLE_MEMORY_MARKERS
    if ( ((uint8 *)tofree)[-4] == 0xFF && ((uint8 *)tofree)[-1] == 0xA5 )
        delete[] (((uint8 *)tofree) - 4);
#else
    uint8 *mem = ((uint8 *)tofree)  - sizeof(long);
    if (*((unsigned long *)mem) == MEM_MAGIC_LONG)
        delete[] mem;
#endif
    else
#ifndef _WIN32_WCE
        abort();
#else
        exit(-1);
#endif
}

void BCSPImplementation::enterCS ( void * envState )
{
    ((BCSPImplementation*)envState)->mLockCS =
        new CriticalSection::Lock ( ((BCSPImplementation*)envState)->cs );
}

void BCSPImplementation::leaveCS ( void * envState )
{
    CriticalSection::Lock *lock = ((BCSPImplementation*)envState)->mLockCS;
    ((BCSPImplementation*)envState)->mLockCS = 0;
    delete lock;
}

void BCSPImplementation::signal ( void * envState )
{
    ((BCSPImplementation*)envState)->mFile->reqXmit();
}

void BCSPImplementation::releasePacket ( void * envState , Packet *pkt )
{
    // if we are going to delete it, chop it out of our list of things
    // to delete...
    for (unsigned int c = 0; c < NUM_PACKETS; c++)
        if (((BCSPImplementation *)envState)->Packets[c] == pkt)
        {
            ((BCSPImplementation *)envState)->Packets[c] = 0;
            break;
        }
    delete pkt;
}
