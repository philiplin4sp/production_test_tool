//==============================================================================================
// Runs PSTool operations in a background thread so they can be cancelled
//==============================================================================================

#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include "stdafx.h"
#include "thread/thread.h"
#include "ps/pshelp.h"

enum TServerOperationEnum
{
    SERVER_OPERATION_CONNECT,
    SERVER_OPERATION_RESET,
    SERVER_OPERATION_DUMP,
    SERVER_OPERATION_MERGE,
    SERVER_OPERATION_QUERY
};

class CPSToolDlg;

class CServerThread : public Threadable
{
public:
    CServerThread(TServerOperationEnum operation, CPSHelper *helper) {m_Successful = false; m_server_operation = operation; m_helper = helper;}
    void SetHelper(CPSHelper *helper) {m_helper = helper;}
    void SetFilename1(LPCTSTR filename) {m_filename1 = filename;}
    void SetFilename2(LPCTSTR filename) {m_filename2 = filename;}
    int ThreadFunc();
    bool SuccessfulOperation() {return m_Successful;}

private:
    CPSHelper *m_helper;
    CString m_filename1;
    CString m_filename2;
    bool m_Successful;
    TServerOperationEnum m_server_operation;
};

#endif