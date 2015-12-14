/******************************************************************************
Project:        boot_prog_test.dll
File Name:      boot_prog_test.h
Description:    Downloads a test program and returns its output.
*/

// $Id:  $
//
//////////////////////////////////////////////////////////////////////

#if !defined(TEST_BOOT_PROG_H_INCLUDED)
#define TEST_BOOT_PROG_H_INCLUDED


#include "common/types.h"
#include "boot_prog.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum BootProgTest_Error
{
    BootProgTest_Success              = 0,
    BootProgTest_NoConnection         = 1,
    BootProgTest_CouldntOpenTransport = 2,
    BootProgTest_CouldntStopProcessor = 3,
    BootProgTest_CouldntDownloadProg  = 4,
    BootProgTest_Timeout              = 5
};

// Download test program to chip and retrieve result.
BootProgTest_Error BootProgTest(const boot_prog_test *bootdef,
                                uint32 *result, uint16 *params,
                                bool reset, int port, int mul = -1);

// Broadcast test program to all available chips and retrieve their results.
// 'results' should be at least 16 elements long.
BootProgTest_Error BootProgTestBroadcast(const boot_prog_test *bootdef,
                                         uint32 *results, uint16 *params,
                                         bool reset, int port);

#endif // TEST_BOOT_PROG_H_INCLUDED


