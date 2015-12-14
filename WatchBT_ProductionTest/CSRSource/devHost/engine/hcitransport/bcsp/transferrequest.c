/****************************************************************************

  Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
        TransferRequest.c  -  defines transfer request functionality

MODIFICATION HISTORY
	1.4  27-apr-00 npm  - added boilerplating

*/


#ifdef	RCS_STRINGS
static const char transferrequest_c_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/transferrequest.c#1 $";
#endif

#include "transferrequest.h"


/****************************************************************************
NAME
	SYNCBLKgetTransferRequestFromQueue

FUNCTION
	wrapper function to retrieve a transfer-request from a queue
	Blocks immediately if no requests available

IMPLEMENTATION NOTE
	just uses underlying queue functionality

****************************************************************************/

BCTransferRequest * SYNCBLKgetTransferRequestFromQueue(struct _BCSPStack * stack,BCTransferRequestQueue * queue) 
{
	return (BCTransferRequest*) SYNCBLKgetNodeFromQueue(stack,&queue->queue) ;
}


BCTransferRequest * ASYNCBLKgetTransferRequestFromQueue(struct _BCSPStack * stack,BCTransferRequestQueue * queue) 
{
	return (BCTransferRequest*) ASYNCBLKgetNodeFromQueue(stack,&queue->queue) ;
}


/****************************************************************************
NAME
	addTransferRequestToQueue

FUNCTION
	wrapper function to add a transfer-request to a queue

IMPLEMENTATION NOTE
	just uses underlying queue functionality

****************************************************************************/

void addTransferRequestToQueue(struct _BCSPStack * stack,BCTransferRequestQueue * queue,BCTransferRequest * req) 
{
	addNodeToQueue(stack,&queue->queue,(QueueNode*) req) ;
}

/****************************************************************************
NAME
	SYNCBLKpeekAtHeadOfTransferRequestQueue

FUNCTION
	wrapper function to return the request at the head of the queue without
	actually removing it.  Blocks immediately if the queue is empty

IMPLEMENTATION NOTE
	just uses underlying queue functionality

****************************************************************************/


BCTransferRequest * SYNCBLKpeekAtHeadOfTransferRequestQueue(struct _BCSPStack * stack,BCTransferRequestQueue * reqQueue)
{
	return (BCTransferRequest*) SYNCBLKpeekAtNodeInQueue(stack,&reqQueue->queue) ;
}

BCTransferRequest * peekAtHeadOfTransferRequestQueue(BCTransferRequestQueue * reqQueue)
{
	return (BCTransferRequest*) peekAtNodeInQueue(&reqQueue->queue) ;
}

/****************************************************************************
NAME
	doCallback

FUNCTION
	does the callback for a transfer request

IMPLEMENTATION NOTE
	it's possible a client might be polling on the transfer status so we
	need to be careful about the order in which we do things

****************************************************************************/

void doCallback(BCTransferRequest * req,BCTransferStatus status)
{
	if (!req) return ; //that was easy ;-)
	
	if (req->callback)
	{
		//we must save these in case a polling client sees xfer complete go before we callback
		void * param ;
		void (*callback)(BCTransferRequest*,BCTransferStatus,void*) ;
		param = req->callbackParam ;
		callback = req->callback ;
		req->status = status ; 
		callback(req,status,param) ;
	}
}
