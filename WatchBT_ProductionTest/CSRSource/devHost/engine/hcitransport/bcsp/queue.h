
/****************************************************************************

	Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
	Queue.h  -  defines base queue

MODIFICATION HISTORY
	1.2  27-apr-00 npm  - added boilerplating

*/

#ifndef _QUEUE_H_
#define _QUEUE_H_

#ifdef	RCS_STRINGS
static  char    queue_h_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/queue.h#1 $";
#endif

#include "common/types.h"

struct _BCSPStack ;

/*
STRUCTURE
	QueueNode

FUNCTION
	Provides base functionality for building a queue
	
FIELDS
	next - the next node in the queue

IMPLEMENTATION NODE
	This implementation assumes the following compiler behaviour:
	struct _derived
	{
		struct _base baseObj ;
	} derivedObj;


	ASSERT(&derivedObj == &derivedObj.baseObj) ;
*/


typedef struct _QueueNode
{
	struct _QueueNode * next ;
} QueueNode ;

/*
STRUCTURE
	Queue

FUNCTION
	Provides base functionality for building a queue.  The queue can
	have blocking synchronous operations performed upon it.
	
FIELDS
	head - the head node in the queue
	tail - the tail node in the queue
	waitmask -	a bitmask of task-ids indicating which tasks are waiting 
				on this queue.

IMPLEMENTATION NODE
	This implementation assumes the following compiler behaviour:
	struct _derived
	{
		struct _base baseObj ;
	} derivedObj;


	ASSERT(&derivedObj == &derivedObj.baseObj) ;
*/



typedef struct _Queue
{
	QueueNode * head ;
	QueueNode * tail ;
	uint16 waitMask ; //contains bit set for each task that is blocked on this queue
} Queue ;

extern void addNodeToQueue(struct _BCSPStack * stack,Queue* queue,QueueNode * node) ;
extern QueueNode * SYNCBLKgetNodeFromQueue(struct _BCSPStack * stack,Queue * queue) ;
extern QueueNode * ASYNCBLKgetNodeFromQueue(struct _BCSPStack * stack,Queue * queue) ;
extern QueueNode * peekAtNodeInQueue(Queue * queue) ;
extern QueueNode * SYNCBLKpeekAtNodeInQueue(struct _BCSPStack * stack,Queue * queue) ;
extern void userAddNodeToQueue(Queue * queue,QueueNode * node) ;
extern QueueNode * schedGetNodeFromQueue(Queue * queue) ;




#endif
