///////////////////////////////////////////////////////////////////////
//
//  FILE   :  h4_queues.cpp
//
//  AUTHOR :  Mark Marshall
//
//  CLASS  :  H4Queues
//
//  PURPOSE:  Generic portion of H4 implementation of HCITransport
//
///////////////////////////////////////////////////////////////////////

#include "h4dsstack.h"
#include "h4_queues.h"
#include <cassert>

H4Queues::H4Queues() : packetCount(0)
{
#ifdef H4_MULTIQUEUE
    queue[0].pktType = H4_QUEUE_PRI1;
    queue[1].pktType = H4_QUEUE_PRI2;
    queue[2].pktType = H4_QUEUE_PRI3;
    queue[3].pktType = H4_QUEUE_PRIX;

	txActiveQueue = -1;
#endif
}

H4Queues::~H4Queues()
{
}

// Always called when the Q is un-locked
H4Queues::H4_QUEUE::~H4_QUEUE()
{
    while (!list.empty())
		list.pop();
}

void H4Queues::addNewPacket(const PDU &pdu, uint8 h4HeaderByte, bool needCallback)
{
#ifdef H4_MULTIQUEUE
	int queuenum;
	// Search for an ordered Q of this type, if none is found then 
	// queuenum is left at the last unordered Q.
    for (queuenum = 0; queuenum < H4_NUM_ORDERED_QUEUES; ++queuenum)
		if (queue[queuenum].pktType == pdu.channel())
			break;
#endif

    CriticalSection::Lock here(cs);

#ifdef H4_MULTIQUEUE
	queue[queuenum].list.push(H4_QUEUE::QueueItem(pdu, h4HeaderByte, needCallback));
#else
	queue.list.push(H4_QUEUE::QueueItem(pdu, h4HeaderByte, needCallback));
#endif

	++packetCount;
}

bool H4Queues::getNextTxPacket()
{
	CriticalSection::Lock here(cs);

#ifdef H4_MULTIQUEUE
	assert(txActiveQueue == -1);
	for (int queueNum = 0; queueNum < H4_NUMQUEUES; ++queueNum)
	{
		if (!queue[queueNum].list.empty())
		{
			assert(packetCount != 0);
			txActiveQueue = queueNum;
			return true;
		}
	}
#else
	if (!queue.list.empty())
	{
		assert(packetCount != 0);
		return true;
	}
#endif

	assert(packetCount == 0);
	return false;
}

const PDU &H4Queues::nextTxPacket() const
{
	assert(packetCount != 0);
#ifdef H4_MULTIQUEUE
	assert(txActiveQueue != -1);
	return queue[txActiveQueue].list.front().pdu;
#else
	return queue.list.front().pdu;
#endif
}

uint8 H4Queues::nextTxPacketHeaderByte() const
{
	assert(packetCount != 0);
#ifdef H4_MULTIQUEUE
	assert(txActiveQueue != -1);
	return queue[txActiveQueue].list.front().header;
#else
	return queue.list.front().header;
#endif
}

bool H4Queues::nextTxPacketNeedsCallback() const
{
	assert(packetCount != 0);
#ifdef H4_MULTIQUEUE
	assert(txActiveQueue != -1);
	return queue[txActiveQueue].list.front().callback;
#else
	return queue.list.front().callback;
#endif
}

void H4Queues::unlinkNextTxPacket()
{
	CriticalSection::Lock here(cs);

#ifdef H4_MULTIQUEUE
	assert(txActiveQueue != -1);
	queue[txActiveQueue].list.pop();
	txActiveQueue = -1;
#else
	queue.list.pop();
#endif

	--packetCount;
}

bool H4Queues::packetsToSend()
{
    CriticalSection::Lock here(cs);
	return packetCount != 0;
}
