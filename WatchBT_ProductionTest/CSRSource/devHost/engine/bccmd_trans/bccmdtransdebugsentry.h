/**********************************************************************
 *
 *  bccmdtransdebugsentry.h
 *
 *  Copyright (C) Cambridge Silicon Radio Ltd 
 *
 *  CBccmdTransDebugSentry is a class that prints function entries and exits.
 *
 ***********************************************************************/

#ifndef BCCMDTRANSDEBUGSENTRY_H
#define BCCMDTRANSDEBUGSENTRY_H

#include <exception>
#include "bccmd_trans.h"
#include "spi/spi_common.h"

//**********************************************
// A class for printing function entries and exits.
// Derived classes should call CheckPrintExit in their destructor.
//**********************************************
#define DEBUGSENTRY_IN_PREFIX SPI_DEBUG_BLOCK_ENTRY
#define DEBUGSENTRY_OUT_PREFIX SPI_DEBUG_BLOCK_EXIT

class CBccmdTransDebugSentry
{
public:
    CBccmdTransDebugSentry(bccmd_trans *apTrans,
                        const char *aBlockName) :
        mBlockName(aBlockName), mpTrans(apTrans), mExitPrinted(false)
    {
        mpTrans->DebugFull(DEBUGSENTRY_IN_PREFIX " %s\n", mBlockName);
    }
    virtual ~CBccmdTransDebugSentry()
    {
        CheckPrintExit();
    }

protected:
    const char *mBlockName;
    bccmd_trans *mpTrans;
    bool mExitPrinted;

    void CheckPrintExit()
    {
        if (!mExitPrinted)
        {
            if(false
#ifndef _WIN32_WCE
               || std::uncaught_exception()
#endif
              )
            {
                mpTrans->DebugFull(DEBUGSENTRY_OUT_PREFIX " %s EXCEPTION\n", mBlockName);
            }
            else
            {
                PrintExit();
            }
            mExitPrinted = true; // make sure we don't print again.
        }
    }

    virtual void PrintExit()
    {
        mpTrans->DebugFull(DEBUGSENTRY_OUT_PREFIX " %s\n", mBlockName);
    }
};

#define BCCMDTRANS_FUNCTION_DEBUG_SENTRY CBccmdTransDebugSentry function_debug_printer(this, __FUNCTION__)

#endif
