#ifndef H4_QUEUES_H__
#define H4_QUEUES_H__

#include "transportimplementation.h"
#include "common/types.h"
#include "thread/critical_section.h"
#include <queue>

//#define H4_MULTIQUEUE /* Have separate and prioritised queues for each pkttype */

// The H4Queues object encapsulates the Q(s) of packets that we are
// trying to transmit.  It is assumed thqt many threads will call 
// 'addNewPacket' to add packets to the list of packets that need
// sending.  Only one thread will remove packets from the Q(s).
// This thread will call 'getNextTxPacket' to see if there are any 
// packets to be sent.  If this function returns 'true' then there
// is something to send.  This thread can now use the value stored
// in 'nextTxPacket' as often as it wants.  When it has sent the 
// packet, it will call  'unlinkNextTxPacket', which will cause 
// the packet to be destroyed.
//
// There are two slightly different versions of this code.  One that 
// holds one queue of all packets, which means that packets are sent
// in FIFO order.  The other version has a Q for each packet type,
// and the packets are sent in priority order (SCO, then CMD/EVT and
// then ACL).

class H4Queues
{
public:
	H4Queues();
	~H4Queues();

	void addNewPacket(const PDU &aPacket, uint8 h4HeaderByte, bool needCallback = true);

	bool getNextTxPacket();
	const PDU &nextTxPacket() const;
	uint8 nextTxPacketHeaderByte() const;
	bool nextTxPacketNeedsCallback() const;

	void unlinkNextTxPacket();

	bool packetsToSend();

protected:
	struct H4_QUEUE
	{
		struct QueueItem
		{
			PDU pdu;
			uint8 header;
			bool callback;

			QueueItem(const PDU &p, uint8 h, bool c)
				: pdu(p), header(h), callback(c) {}
		};

		// The list of PDU's
		std::queue<QueueItem> list;

#ifdef H4_MULTIQUEUE
		PDU::bc_channel pktType;
#endif
		// Destructor
		~H4_QUEUE();
	};

#ifdef H4_MULTIQUEUE
	enum
	{
		H4_NUM_ORDERED_QUEUES = 3,
		H4_NUM_QUEUES = H4_NUM_ORDERED_QUEUES + 1
	};
	enum H4_QUEUENUM    /* Highest-priority is lowest-numbered */
	{
		H4_QUEUE_PRI1 = PDU::hciSCO,
		H4_QUEUE_PRI2 = PDU::hciACL,
		H4_QUEUE_PRI3 = PDU::hciCommand,
		H4_QUEUE_PRIX = PDU::bcsp_channel_count
	};
	H4_QUEUE queue[H4_NUM_QUEUES];
	int txActiveQueue;
#else
	H4_QUEUE queue;
#endif

	CriticalSection cs;
	uint32 packetCount;
};

#endif /* def H4_QUEUES_H__ */
