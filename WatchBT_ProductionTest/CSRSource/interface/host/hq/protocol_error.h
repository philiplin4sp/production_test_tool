#ifndef _PROTOCOL_ERROR_H
#define _PROTOCOL_ERROR_H
/**************************************************************

FILE
    protocol_error.h - define ids for protocol-errors

DESCRIPTION
  A protocol error is a type of fault.  It is used to report
  on errors detected during transactions between the local
  entity and the peer.  The intention is to give customers
  some clue as to why teh chip may not be behaving quite as
  they expect.  For example, the rfcomm layer might emit
  a protocol_error with layer=rfcomm entity=remote and
  error=payload_too_large to indicate that the other end
  has messed up and sent a payload that is too large.

MODIFICATION HISTORY

1.1 21:may:02 npm - created
1.2 22:may:02 pws H15.105: lint changes (dedosify).
1.3 14:aug:03 cjo B-848: Bill's carriage-return characters wreak lint.

------------------------------------------------------------------
--- This modification history is now closed. Do not update it. ---
------------------------------------------------------------------

*/




/*
  The protocol_entity is used to point the finger
  at whoever it is that messed up.  Hopefully this will
  almost exclusively be 'remote_entity'.
*/

typedef enum {
     LOCAL_ENTITY,
     REMOTE_ENTITY,
     UNKNOWN_ENTITY
}
protocol_entity ;

/*
  The protocol_layer indicates at which layer the error was detected.
*/

typedef enum
{
     LM_LAYER,
     L2CAP_LAYER,
     SDP_LAYER,
     DM_LAYER,
     RFCOMM_LAYER,
     BNEP_LAYER,
     TCSBIN_LAYER

}
protocol_layer ;

/*
  Now we get to the interesting stuff - the errors.
  These are maintained as separate lists for neatness.
  Please remember to add new errors at the bottom so as not
  to invalidate old error codes.
*/

typedef enum
{

/*
   lm errors go here
*/
     LM_LAYER_BASE=0,
/*
   l2cap errors go here
*/
     L2CAP_LAYER_BASE=0,
     /*
       L2_CORRUPT_PACKET indicates that an l2cap packet was received where the
       length in the header did not match the actual payload.  This may occur due
       to heavy radio interference.
       info0 = payload length
       info1 = length reported in the header
     */
     L2_CORRUPT_PACKET,

/*
   rfcomm errors go here
*/
     RFCOMM_LAYER_BASE=0,
/*
   sdp errors go here
*/
     SDP_LAYER_BASE=0

} protocol_error;


/*
  Now here are some macros to simplify life for us
*/

#define RFCOMM_ERR(ERROR) hq_protocol_error(RFCOMM_LAYER,REMOTE_ENTITY,ERROR)
#define L2CAP_ERR(ERROR) hq_protocol_error(L2CAP_LAYER,REMOTE_ENTITY,ERROR)
#define L2CAP_ERR_X(ERROR,i0,i1,i2) hq_protocol_error_with_info(L2CAP_LAYER,REMOTE_ENTITY,ERROR,i0,i1,i2)
#define SDP_ERR(ERROR) hq_protocol_error(SDP_LAYER,REMOTE_ENTITY,ERROR)

/*
  include this here for hq_protocol_error
*/

#include "hqpdu.h"

#endif
