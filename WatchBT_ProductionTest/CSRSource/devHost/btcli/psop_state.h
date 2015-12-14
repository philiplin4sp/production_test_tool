#ifndef PSOP_STATE_H
#define PSOP_STATE_H

#include "typedefs.h"
#include "globals.h"

#ifdef __cplusplus
extern "C" {
#endif

/*********************************
* keeps track of ps operations.
* psop_add adds a new ps operation.
* psop_get gets the ps operation associated with a bccmd sequence number.
* psop_update updates a ps operation. the old sequence number must be supplied so
*   that we can find and clean up the old data.
*
* expected use will be:
* PSOPDATA data;
* data.seqno = bccmdseqno;
* data.state = first state;
* psop_add(&data);
*
* when a reply is received:
* PSOPDATA data;
* psop_get(reply_seqno, &data);
* send next bccmd
* data.seqno = bccmdseqno;
* data.state = next state;
* psop_update(reply_seqno, &data);
*
* if the new state is NONE then the storage is also freed and no further cleanup is required.
*********************************/

// add a new operation.
void psop_add(const PSOPDATA *);
// get the operation state associated with a bccmd sequence number.
void psop_get(u16 seqno, PSOPDATA *);
// update an operation. must supply the old bccmd sequence number for cleanup purposes.
void psop_update(u16 old_seqno, const PSOPDATA *);
// clear all transactions.
void psop_clear_all();

#ifdef __cplusplus
}	// extern "C"
#endif

#endif

