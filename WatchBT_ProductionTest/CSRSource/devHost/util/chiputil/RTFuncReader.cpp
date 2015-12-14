/*******************************************************************************

    FILE:       RTFuncReader.cpp
 
                Copyright (C) Cambridge Silicon Radio Ltd 2001-2007. 
                Confidential. http://www.csr.com.
 
    PURPOSE:    RTFuncReader class - Used to read supported radio tests via the 
                SLUT.

*******************************************************************************/

#include "RTFuncReader.h"
#include "slut/slutids.h"
#include "host/bccmd/bccmdradiotestpdu.h"


RTFuncReader::RTFuncReader(IMemoryReaderGP * const zpMemReader)
:   mpMemReader(zpMemReader), mpSlutTable(0), 
    mSlutStatus(RT_SLUT_NOT_READ), mCompleteList(false)
{
}

RTFuncReader::~RTFuncReader()
{
    delete mpSlutTable;
}

RTFuncReader::RTSlutStatus RTFuncReader::Read()
{
    mpSlutTable = new SlutTable(mpMemReader);
    if(mpSlutTable->read_slut() == false)
    {
        mSlutStatus = RT_SLUT_READ_ERROR;
        return mSlutStatus;
    }

    // Check for the existence of the radiotest function list
    if(!mpSlutTable->entry_exists(SLUTID_RADIOTEST_FNS))
    {
        mSlutStatus = RT_SLUTID_NOT_PRESENT;
        return mSlutStatus;
    }
    
    uint16 rtFnsAddr = (uint16)mpSlutTable->getValue(SLUTID_RADIOTEST_FNS);
    if(rtFnsAddr == 0)
    {
        mSlutStatus = RT_SLUTID_NULL;
        return mSlutStatus;
    }

    // Read the radiotests
    if(!ReadFns(rtFnsAddr))
    {
        mSlutStatus = RT_FNS_READ_ERROR;
        return mSlutStatus;
    }
                
    mSlutStatus = RT_SLUT_OK;
    return mSlutStatus;
}

bool RTFuncReader::ReadFns(uint16 zAddr)
{
    // The rt_fn_detail structure is 4 words total on BlueCore
    // The structure contains a function pointer and bit fields,
    // hence the hardcoded sizing here
    static const uint16 RT_STRUCT_SIZE(4);
    
    GenericPointer rtFnsLoc(Mem_RawData, zAddr);
    uint16 rtFncBlock[RT_STRUCT_SIZE];
    uint16 rtFnc;
    bool endOfList(false);
    
    do 
    {
        if (!mpMemReader->read_mem(rtFnsLoc, RT_STRUCT_SIZE, rtFncBlock))
            return false;
            
        // 12 bits used for the radiotest function identifier, which is 
        // the only field we're interested in (and the first field in the
        // structure).
        rtFnc = ((rtFncBlock[0] & 0xFFF0) >> 4);

        // RADIOTEST_END is the sentinel value for old builds.
        // RADIOTEST_COMPLETE_LIST is the sentinel value for builds which
        // should have a complete list of supported radiotests
        if(rtFnc == RADIOTEST_END || rtFnc == RADIOTEST_COMPLETE_LIST)
        {
            endOfList = true;

            if(rtFnc == RADIOTEST_COMPLETE_LIST)
            {
                mCompleteList = true;
            }
        }
        else
        {
            mRadioTests.push_back(rtFnc);
        }

        rtFnsLoc += RT_STRUCT_SIZE;

    } while(endOfList == false);

    return true;
}

RTFuncReader::RTStatus RTFuncReader::Supported(uint16 zRadioTest)
{
    // Default case - it may be supported
    RTStatus status(RT_SUPPORT_UNKNOWN);
    
    if(mSlutStatus == RT_SLUT_OK)
    {
        bool rtFound(false);
        std::vector<uint16>::const_iterator pVal;
        for(pVal = mRadioTests.begin(); pVal != mRadioTests.end() && status != RT_SUPPORTED; ++pVal)
        {
            if(*pVal == zRadioTest)
            {
                status = RT_SUPPORTED;
                break;
            }
        }

        // NOTE: Older builds do not contain a complete list of supported radiotests.
        // For the affected tests, they are effectively unknown, so don't mark as 
        // being NOT_SUPPORTED. Since the fix for B-19858, we should trust the list.
        if(status != RT_SUPPORTED &&
            (mCompleteList == true ||
            (mCompleteList == false && 
            zRadioTest != RADIOTEST_CFG_BIT_ERR &&
            zRadioTest != RADIOTEST_CFG_UAP_LAP &&
            zRadioTest != RADIOTEST_CFG_HOPPING_SEQUENCE &&
            zRadioTest != RADIOTEST_CFG_PIO_CHANNEL_MASK &&
            zRadioTest != RADIOTEST_RADIO_STATUS &&
            zRadioTest != RADIOTEST_RADIO_STATUS_ARRAY))
            )
        {
            status = RT_NOT_SUPPORTED;
        }
    }
    
    return status;
}
    
RTFuncReader::RTSlutStatus RTFuncReader::GetSupported(std::vector<uint16>& zRTList)
{
    if(mCompleteList == false)
    {
        // No point returning a list which is incomplete
        return RF_FNS_INCOMPLETE;
    }

    zRTList = mRadioTests;
    return mSlutStatus;
}
