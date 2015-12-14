/**********************************************************************
 *
 *  bccmdtransdebugsentrywithreturn.h
 *
 *  Copyright (C) Cambridge Silicon Radio Ltd 
 *
 *  CBccmdTransDebugSentryWithReturn is an extension to CBccmdTransDebugSentry that
*     also prints what a function is returning if you use it correctly.
 *
 ***********************************************************************/

#ifndef BCCMDTRANSDEBUGSENTRYWITHRETURN_H
#define BCCMDTRANSDEBUGSENTRYWITHRETURN_H

#include "bccmdtransdebugsentry.h"

//**********************************************
// A class for printing function entries and exits.
// It will also print the value of a variable on exit, which should be what you're returning.
// If you return something other than the variable passed to the constructor
// then this class will incorrectly state the value you're returning.
//**********************************************
template <typename ReturnType> class CBccmdTransDebugSentryWithReturn : public CBccmdTransDebugSentry
{
public:
    // constructor.
    CBccmdTransDebugSentryWithReturn(bccmd_trans *apTrans,      // the stream data object to use for logging
                                     const char *aBlockName,    // the name of the 'block'. usually a function name.
                                     const ReturnType &aRet) :  // a reference to the variable you're returning.
         CBccmdTransDebugSentry(apTrans, aBlockName), mRet(aRet)
    {
    }

    virtual ~CBccmdTransDebugSentryWithReturn()
    {
        CheckPrintExit();
    }

protected:
    const ReturnType &mRet;

    virtual void PrintExit()
    {
        DoPrintExit(mRet);
    }
    void DoPrintExit(int aRet)
    {
        mpTrans->DebugFull(DEBUGSENTRY_OUT_PREFIX " %s returning %d\n", mBlockName, aRet);
    }
    void DoPrintExit(unsigned int aRet)
    {
        mpTrans->DebugFull(DEBUGSENTRY_OUT_PREFIX " %s returning %u\n", mBlockName, aRet);
    }
    void DoPrintExit(const char *aRet)
    {
        mpTrans->DebugFull(DEBUGSENTRY_OUT_PREFIX " %s returning \"%s\"\n", mBlockName, aRet);
    }
    void DoPrintExit(float aRet)
    {
        mpTrans->DebugFull(DEBUGSENTRY_OUT_PREFIX " %s returning %f\n", mBlockName, aRet);
    }
    void DoPrintExit(double aRet)
    {
        mpTrans->DebugFull(DEBUGSENTRY_OUT_PREFIX " %s returning %lf\n", mBlockName, aRet);
    }
};

#define BCCMDTRANS_FUNCTION_DEBUG_SENTRY_R(type, r) CBccmdTransDebugSentryWithReturn<type> function_debug_printer(this, __FUNCTION__, (r))

#endif
