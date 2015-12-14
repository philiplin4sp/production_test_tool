///////////////////////////////////////////////////////////////////////
//
//  FILE   :  aclscheduler.h
//
//  Copyright CSR 2006
//
//  AUTHOR :  Jonathan Burcham
//
//  CLASS  :  ACLScheduler
//
//  PURPOSE:  Distribute bandwidth across the different acl channels evenly.
//
//  $Id: //depot/bc/main/devHost/btcli/aclscheduler.h $
//
///////////////////////////////////////////////////////////////////////

#ifndef ACL_SCHEDULER_H
#define ACL_SCHEDULER_H

#include "aclengine_impl.h"
#include "acl_channel_list.h"
#include "common/types.h"

#include <vector>
#include <deque>
#include <list>

#include "csrhci/bluecontroller.h"
#include "hcipacker/hcidatapdu.h"
#include "thread/signal_box.h"

class AclEngine::Implementation::Scheduler
{
public:
    Scheduler(BlueCoreDeviceController_newStyle *deviceController, AclEngine::Implementation::ChannelList *mChannelList);
    bool add(ExtendedHCIACLPDU_Helper helper);
    bool nocp(const HCI_EV_NUMBER_COMPLETED_PKTS_T_PDU& pdu);
    bool setBufferSizes(const HCI_READ_BUFFER_SIZE_RET_T_PDU& pdu);
    void sentRbs();
    void disconnectFreedTokens(uint32 tokensfreed);
    void kick();
private:
    int32 tokenPool;
    uint16 aclPacketNumber;
    uint16 aclPacketSize;
    uint32 maxTokensPerChannel;
    BlueCoreDeviceController_newStyle *deviceController;
    CriticalSection a; // Use this to lock any data structures in the scheduler
    ChannelList *mChannelList;
    enum RBS_State {
        RBS_INVALID,
        AWAITING_REPLY,
        RBS_VALID
    } rbsState;
    std::deque<ExtendedHCIACLPDU_Helper> preinit_pdus;
    bool add_i(ExtendedHCIACLPDU_Helper helper);
    void sendPdus();
    CountedPointer<AclEngine::Implementation::ChannelList::Channel> setupChannel(ExtendedHCIACLPDU_Helper helper);
};

#endif /*ACL_SCHEDULER_H*/
