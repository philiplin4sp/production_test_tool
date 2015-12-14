///////////////////////////////////////////////////////////////////////
//
//  FILE   :  aclscheduler.cpp
//
//  Copyright CSR 2006
//
//  AUTHOR :  Jonathan Burcham
//
//  CLASS  :  ACLScheduler
//
//  PURPOSE:  Distribute bandwidth across the different acl channels evenly.
//
//  $Id: //depot/bc/main/devHost/btcli/aclscheduler.cpp $
//
///////////////////////////////////////////////////////////////////////

#include "aclscheduler.h"
#include "acl_channel_list.h"
#include "acl_channel.h"
#include "time/stop_watch.h"
#include <math.h>
#include <cassert>
#include <algorithm>

AclEngine::Implementation::Scheduler::Scheduler(BlueCoreDeviceController_newStyle *deviceController, ChannelList *lChannelList) :
    tokenPool(0),
    aclPacketNumber(0),
    aclPacketSize(0),
    mChannelList(lChannelList),
    rbsState(RBS_INVALID)
{
    this->deviceController = deviceController;
}

CountedPointer<AclEngine::Implementation::ChannelList::Channel> AclEngine::Implementation::Scheduler::setupChannel(ExtendedHCIACLPDU_Helper helper)
{
    // Find out if we must create a new broadcast channel
    switch (helper.get_bct())
    {
    case HCIACLPDU::act:
        if (mChannelList->isActChValid())
        {
            if (mChannelList->getActCh() != helper.get_handle())
            {
                return false;
            }
        }
        else
        {
            mChannelList->add_act_channel(helper.get_handle());
        }
        break;
    case HCIACLPDU::pic:
        if (mChannelList->isPicChValid())
        {
            if (mChannelList->getPicCh() != helper.get_handle())
            {
                return false;
            }
        }
        else
        {
            mChannelList->add_pic_channel(helper.get_handle());
        }
        
        break;
    default:
        break;
    }

    return mChannelList->get_channel(helper.get_handle());
}


bool AclEngine::Implementation::Scheduler::setBufferSizes(const HCI_READ_BUFFER_SIZE_RET_T_PDU& pdu)
{
    CriticalSection::Lock lock(a);
    if (RBS_VALID != rbsState)
    {
        tokenPool = pdu.get_total_acl_data_pkts();
    }
    rbsState = RBS_VALID;
    mChannelList->setBufferSizes(pdu);
    sendPdus();
    return true;
}

bool AclEngine::Implementation::Scheduler::add(ExtendedHCIACLPDU_Helper helper)
{
    CriticalSection::Lock lock(a);
    CountedPointer<AclEngine::Implementation::ChannelList::Channel> channel(setupChannel(helper));

    if (channel)
    {
        switch (rbsState)
        {
        case RBS_INVALID:
            deviceController->send(HCI_READ_BUFFER_SIZE_T_PDU());
            rbsState = AWAITING_REPLY;
            // fall through
        case AWAITING_REPLY:
            channel->sender.add(helper);
            break;
        case RBS_VALID:
            channel->sender.add(helper);
            sendPdus();
            break;
        }
        return true;
    }
    else
    {
        return false;
    }
}

bool AclEngine::Implementation::Scheduler::nocp(const HCI_EV_NUMBER_COMPLETED_PKTS_T_PDU& pdu)
{
    CriticalSection::Lock lock(a);
    bool ok(true);
    uint8 numch(pdu.get_num_handles());

    for (uint8 i = 0; i < numch; ++i)
    {
        uint16 aclch, numPackets;
        pdu.get_num_completed_pkts(i, aclch, numPackets);

        CountedPointer<AclEngine::Implementation::ChannelList::Channel> channel(mChannelList->get_channel(aclch));

        if (channel)
        {
            // Only accept tokens when the connection is recognised as being open.
            tokenPool +=  numPackets;
            // Even if an error occurs, keep going, but report error.
            ok &= channel->sender.nocp(numPackets);
        }
        else
        {
            // Else keep a note that something untoward happened
            ok = false;
        }
    }
    sendPdus();
    return ok;
}

void AclEngine::Implementation::Scheduler::sentRbs()
{
    CriticalSection::Lock lock(a);
    switch (rbsState)
    {
    case RBS_INVALID:
        rbsState = AWAITING_REPLY;
        break;
    case AWAITING_REPLY:
    case RBS_VALID:
        break;
    } 
}

void AclEngine::Implementation::Scheduler::disconnectFreedTokens(uint32 tokensFreed)
{
    CriticalSection::Lock lock(a);
    tokenPool += tokensFreed;
}

void AclEngine::Implementation::Scheduler::kick()
{
    CriticalSection::Lock lock(a);
    switch (rbsState)
    {
    case RBS_INVALID:
        deviceController->send(HCI_READ_BUFFER_SIZE_T_PDU());
        rbsState = AWAITING_REPLY;
        break;
    case AWAITING_REPLY:
        // Nothing can be done currently. Scheduler will be be kicked once reply has been received.
        break;
    case RBS_VALID:
        sendPdus();
        break;
    }
}


uint32 totalTime(0);
uint32 bytesdone(0);

void AclEngine::Implementation::Scheduler::sendPdus()
{
    HCIACLPDU pdu(null_pdu);
    while (tokenPool > 0 && mChannelList->getPdu(pdu))
    {
        deviceController->send(pdu, false);
        --tokenPool;
    }
}



