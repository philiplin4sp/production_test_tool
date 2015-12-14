// Copyright (C) Cambridge Silicon Radio Ltd 2000-2009

// flashid.h

// Class for reading the identity of a flash image (on chip or off).
// Uses an interface on an abstract base class to read locations.
//
//  1.5 06:jan:2001     ckl:        Created

// $Id: //depot/bc/bluesuite_2_4/devHost/spi/flashdll/flashid.h#1 $


// abstract class - provides a location reader.

#ifndef FLASHID_H_INCLUDED
#define FLASHID_H_INCLUDED

#include "abstract/memory_reader.h"
#include "chiputil/sluttable.h"
#include <cstdio>

class CXapHelper;

const int FlashIDBuildNameLen = 200;

enum FlashIDState
{
    FlashIDStateUnread,
    FlashIDStateRead,
    FlashIDStateFailed
};

class FlashIDSolver
{
public:

    FlashIDSolver(IMemoryReaderGP *Reader, CXapHelper *Helper = NULL)  { FlashIDSolver::Reader = Reader; FlashIDSolver::Helper = Helper;}
    void SetReader(IMemoryReaderGP *Reader) { FlashIDSolver::Reader = Reader; }
    void SetXapHelper(CXapHelper *Helper) { FlashIDSolver::Helper = Helper; }
    const IMemoryReaderGP *GetReader() { return Reader; }
    const CXapHelper *GetHelper() { return Helper; }

    FlashIDState Solve(uint32 *loaderId, char *loaderName,
                       uint32 *stackId,  char *stackName,
                       const size_t nameLen,
                       const GenericPointer *slutPtr = NULL);
    
private:
    IMemoryReaderGP *Reader;
    CXapHelper *Helper;
    bool GetFirmwareIdFromSlut(const SlutTable & slut_table, char *BuildName, size_t NameLen, uint32 *BuildNum);

};

#endif // FLASHID_H_INCLUDED
