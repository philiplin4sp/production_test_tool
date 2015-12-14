/****************************************************************************

	Copyright (C) Cambridge Silicon Radio April 2000-2006

FILE
	TransferRequest.h  -  defines transfer request

MODIFICATION HISTORY
	1.4  27-apr-00 npm  - added boilerplating

*/

#ifndef _TRANSFERREQUEST_H_
#define _TRANSFERREQUEST_H_

#ifdef	RCS_STRINGS
static  char    transferrequest_h_id[]
  = "$Id: //depot/bc/bluesuite_2_4/devHost/engine/hcitransport/bcsp/transferrequest.h#1 $";
#endif



#include "queue.h"

/*
STRUCTURE
	BCTransferStatus

FUNCTION
	encodes the status of a transfer request

*/

typedef enum
{
	transferComplete=0,
	transferWaiting,
	transferBufferTooSmall,
	transferCancelled
} BCTransferStatus ;

/*
STRUCTURE
	BCTransferRequest

FUNCTION
	marshalls all the information necessary to conduct a transfer 

FIELDS
	node - base node to allow transfer requests to be queued.
	data - pointer to data 
	callback - pointer to callback function called when transfer completed
	callbackParam - the user-defined parameter to be passed to the callback func
	length - the number of bytes to be transfered
	channel - the channel on which to perform the transfer
*/

typedef struct _BCTransferRequest
{
	QueueNode node ;
	uint8 * data ;
	void (*callback)(struct _BCTransferRequest*,BCTransferStatus,void *) ;
	void * callbackParam ;
	BCTransferStatus status ;
	uint16 length ;
	uint8 channel ;
} BCTransferRequest ;

/*
STRUCTURE
	BCTransferRequestQueue

FUNCTION
	Acts as a queue for transfer requests

FIELDS
	queue - base queue which actually handles functionality for us
*/

typedef struct _BCTransferRequestQueue
{
	Queue queue ;
} BCTransferRequestQueue ;




extern BCTransferRequest * SYNCBLKgetTransferRequestFromQueue(struct _BCSPStack * stack,BCTransferRequestQueue * queue) ;
extern void addTransferRequestToQueue(struct _BCSPStack * stack,BCTransferRequestQueue * queue,BCTransferRequest * req) ;
extern BCTransferRequest * SYNCBLKpeekAtHeadOfTransferRequestQueue(struct _BCSPStack *stack,BCTransferRequestQueue * reqQueue) ;
extern BCTransferRequest * peekAtHeadOfTransferRequestQueue(BCTransferRequestQueue * reqQueue) ;
extern void doCallback(struct _BCTransferRequest * req,BCTransferStatus status) ;
extern BCTransferRequest * ASYNCBLKgetTransferRequestFromQueue(struct _BCSPStack * stack,BCTransferRequestQueue * queue) ;

#endif

