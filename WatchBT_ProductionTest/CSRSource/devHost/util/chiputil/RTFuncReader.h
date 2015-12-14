/*******************************************************************************

    FILE:       RTFuncReader.h
 
                Copyright (C) Cambridge Silicon Radio Ltd 2001-2007. 
                Confidential. http://www.csr.com.
 
    PURPOSE:    RTFuncReader class - Used to read supported radio tests via the 
                SLUT.

*******************************************************************************/

#ifndef _RT_FUNC_READER_H_
#define _RT_FUNC_READER_H_

#include "common/types.h"
#include "chiputil/sluttable.h"
#include <vector>


class RTFuncReader
{
public:
    RTFuncReader(IMemoryReaderGP * const zpMemReader);
    ~RTFuncReader();
    
    typedef enum
    {
        RT_SLUT_OK = 0,         // Means the supported radiotests have been read successfully
        RT_SLUT_NOT_READ,
        RT_SLUT_READ_ERROR,
        RT_SLUTID_NOT_PRESENT,  // For firmware builds without CL 54508, which don't expose the radiotests
        RT_SLUTID_NULL,         // For firmware builds with CL 54508, but where the radiotests are not exposed
        RT_FNS_READ_ERROR,
        RF_FNS_INCOMPLETE
    }RTSlutStatus;

    // Reads the slut and the radiotest functions if available
    // Returns the status of the SLUT / radiotest entries
    RTSlutStatus Read();
    
    typedef enum
    {
        RT_NOT_SUPPORTED = 0,
        RT_SUPPORT_UNKNOWN, // I.e. Radiotests not listed in SLUT, might be supported though
        RT_SUPPORTED
    }RTStatus;

    // Returns the status for a particular radiotest
    RTStatus Supported(uint16 zRadioTest);

    // Get all supported radiotests.
    // Returns the status of the SLUT / radiotest entries
    RTSlutStatus GetSupported(std::vector<uint16>& zRTList);

private:
    RTFuncReader();
    RTFuncReader& operator=(const RTFuncReader& zRhs);
    RTFuncReader(const RTFuncReader& zRhs);
    
    // Read the radiotests, starting at address zAddr
    bool ReadFns(uint16 zAddr);
    
    SlutTable* mpSlutTable;
    IMemoryReaderGP * const mpMemReader;
    std::vector<uint16> mRadioTests;
    RTSlutStatus mSlutStatus;
    bool mCompleteList;
};

#endif //_RT_FUNC_READER_H_
