//==============================================================================================
//
// Runs PSTool operations in a background thread so they can be cancelled
//
//==============================================================================================

#include "Serverthread.h"

// =================================================================================
// Run the operation in a background thread
// =================================================================================
int CServerThread::ThreadFunc()
{
    m_Successful = true; // handle operation that return (void)

    switch (m_server_operation)
    {
        case SERVER_OPERATION_CONNECT:
        {
            m_Successful = m_helper->GetServer()->open();
            break;
        }
        case SERVER_OPERATION_RESET:
        {
            m_Successful = m_helper->ResetBC(RESET_WARM);
            break;
        }
        case SERVER_OPERATION_MERGE:
        {
            m_helper->apply_file(m_filename1);
            break;
        }
        case SERVER_OPERATION_DUMP:
        {
            ASSERT(m_filename1 != "");
            m_helper->dump_ps(m_filename1, CPSHelper::FileFormatCommented);
            break;
        }
        case SERVER_OPERATION_QUERY:
        {
            ASSERT(m_filename1 != "");
            ASSERT(m_filename2 != "");
			m_helper->run_query(m_filename1, m_filename2, CPSHelper::FileFormatCommented);			
            break;
        }
        default:
        {
            // Unknown operation
            ASSERT(false);
        }
    }
    return 0;
}
