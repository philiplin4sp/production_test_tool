/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        Queue.c  -  queue base functionality

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating

*/


#ifdef	RCS_STRINGS
static  char    queue_c_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/queue.c#1 $";
#endif

#include "bcspstack.h"
#include "queue.h"
#include "scheduler.h"



/****************************************************************************
NAME
	addNodeToQueue

FUNCTION
	adds a node to the tail of the queue - this may unblock waiting tasks

IMPLEMENTATION NOTE
	

****************************************************************************/

void addNodeToQueue(struct _BCSPStack * stack,Queue * queue,QueueNode * node)
{
	node->next = null ;
	if (!queue->head)
	{
		queue->head = queue->tail = node ;
		unblock(stack,&queue->waitMask) ; 
	}
	else
	{
		queue->tail->next = node ;
		queue->tail = node ;
	}
}

/****************************************************************************
NAME
	SYNCBLKgetNodeFromQueue

FUNCTION
	attempt to obtain a node from the head of the queue - if none is 
	available, block immediately

IMPLEMENTATION NOTE
	

****************************************************************************/

QueueNode * SYNCBLKgetNodeFromQueue(struct _BCSPStack * stack,Queue * queue)
{
	QueueNode * node = null ;
	if (queue->head)
	{
		node = queue->head ;
		if (queue->tail ==queue->head) queue->tail = null ;
		queue->head = queue->head->next ;
	}
	else immediateBlock(stack,&queue->waitMask) ; //this causes longjmp back to scheduler
	return node ;
}

/****************************************************************************
NAME
	ASYNCBLKgetNodeFromQueue

FUNCTION
	attempt to obtain a node from the head of the queue - if none is 
	available mark this task as blocked but return so we can try
	something else.

IMPLEMENTATION NOTE
	

****************************************************************************/

QueueNode * ASYNCBLKgetNodeFromQueue(struct _BCSPStack * stack,Queue * queue)
{
	QueueNode * node = null ;
	if (queue->head)
	{
		node = queue->head ;
		if (queue->tail ==queue->head) queue->tail = null ;
		queue->head = queue->head->next ;
	}
	else deferredBlock(stack,&queue->waitMask) ;
	return node ;
}

/****************************************************************************
NAME
	peekAtNodeInQueue

FUNCTION
	return the head node in the queue without actually removing it.

IMPLEMENTATION NOTE
	

****************************************************************************/

QueueNode * peekAtNodeInQueue(Queue * queue) 
{
	return queue->head ;
}

/****************************************************************************
NAME
	SYNCBLKpeekAtNodeInQueue

FUNCTION
	return the head node in the queue without actually removing it.
	If there is no head node, block immediately.

IMPLEMENTATION NOTE
	

****************************************************************************/

QueueNode * SYNCBLKpeekAtNodeInQueue(struct _BCSPStack * stack,Queue * queue) 
{
	if (!queue->head) immediateBlock(stack,&queue->waitMask) ;
	return queue->head ;
}


/****************************************************************************
NAME
	userAddNodeToQueue

FUNCTION
	used by user functions to add a node to the queue.  This mustn't
	do any unblocking since we're in an indeterminate state with the 
	stack waitmask.  This is only called for a couple of transfer request
	queues which are protected by critical sections, so the actual queue
	manipulation is safe.

IMPLEMENTATION NOTE
	

****************************************************************************/


void userAddNodeToQueue(Queue * queue,QueueNode * node)
{
	node->next = null ;
	if (!queue->head)
	{
		queue->head = queue->tail = node ;
	}
	else
	{
		queue->tail->next = node ;
		queue->tail = node ;
	}
}


/****************************************************************************
NAME
	schedGetNodeFromQueue

FUNCTION
	used by scheduler to remove a node from a queue.  Since the scheduler
	runs outside of the context of any task, clearly we can't block.

IMPLEMENTATION NOTE
	

****************************************************************************/

QueueNode * schedGetNodeFromQueue(Queue * queue)
{
	QueueNode * node = null ;
	if (queue->head)
	{
		node = queue->head ;
		if (queue->tail ==queue->head) queue->tail = null ;
		queue->head = queue->head->next ;
	}
	return node ;
}
