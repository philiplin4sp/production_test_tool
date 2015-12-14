/****************************************************************************
FILE
        bccmd_spi_common.h  -  data shared between host and chip

DESCRIPTION
        Constants and data structures shared between host and chip for the
        bccmd_spi interface.

        The structure C definition is probably not of use to the host as
        there is no way to guarantee the host and chip compilers will lay out
        its data in the same way, however it is defined here as it is the
        host's primary access to the interface.

MODIFICATION HISTORY
        1.1  21:jan:02   pws    Moved to interface/host/bccmd.

OLD MODIFICATION HISTORY
        1.1   7:feb:01   cjo    Created - cut from bccmd_spi.h.
        1.2   7:feb:01   mm     removed include of "bccmd_spi.h"
        ------------------------------------------------------------------
        --- This modification history is now closed. Do not update it. ---
        ------------------------------------------------------------------
*/

#ifndef __BCCMD_SPI_COMMON_H__
#define __BCCMD_SPI_COMMON_H__


/* A block of data viewed by the host and chip code.  This provides the
gateway through which commands and responses are conveyed.

This is used directly as a C definition by the code on the chip.  The host
code must work with this layout.  In practice this means it is unlikely that
the host will be able to use this typedef directly. */

typedef struct {
        uint16          cmd;          /* State variable and access cntl. */

/* States of the "cmd" field control the feeding of commands to bccmd (over
SPI) via the buffer pointed by bccmd_spi_buffer.buffer, and the returning the
results to the caller in the same buffer.   Values for "cmd" are below. */

#define BCCMD_SPI_CMD_IDLE       (0)  /* British Rail porter. */
#define BCCMD_SPI_CMD_ALLOC_REQ  (1)  /* Host wants to write a command. */
#define BCCMD_SPI_CMD_ALLOC_OK   (2)  /* Chip has allocated a buffer. */
#define BCCMD_SPI_CMD_ALLOC_FAIL (3)  /* Chip cannot provide the buffer. */
#define BCCMD_SPI_CMD_CMD        (4)  /* Host has written a command. */
#define BCCMD_SPI_CMD_PENDING    (5)  /* Chip is executing command. */
#define BCCMD_SPI_CMD_RESP       (6)  /* Chip has written the result. */
#define BCCMD_SPI_CMD_DONE       (7)  /* Host has finished with the buffer. */

        uint16          size;         /* Requested size of *buffer. */
        uint16*         buffer;       /* bccmd command/response buffer. */
} BCCMD_SPI_INTERFACE;


/* The following rules surround use of the structure.

When "cmd" is IDLE:
      - the host may write to "size", (then)
      - the host may change "cmd" to ALLOC_REQ.

When "cmd" is ALLOC_REQ:
      - the chip may (allocate a buffer of size "size" uint16s and) write the
                buffer's address into "buffer", (then)
      - the chip may change "cmd" to ALLOC_OK or ALLOC_FAIL.

When "cmd" is ALLOC_OK:
      - the host may write to the buffer pointed to by "buffer", knowing it
                is at least "size" uint16s long, (then)
      - the host may change "cmd" to CMD.

When "cmd" is ALLOC_FAIL:
      - the host may write to "size", (then)
      - the host may change "cmd" to ALLOC_REQ.

When "cmd" is CMD:
      - the chip may change "cmd" to PENDING.

When "cmd" is PENDING:
      - the chip may write to the buffer pointed to by "buffer", knowing it
                is at least "size" uint16s long, (then)
      - the chip may change "cmd" to RESP.

When "cmd" is RESP:
      - the host may change "cmd" to DONE.

When "cmd" is DONE:
      - the chip may (free the memory pointed to by "buffer" and) set
                "buffer" to NULL.

When "cmd" is DONE:
      - the chip may change "cmd" to IDLE.
*/


#endif  /* __BCCMD_SPI_COMMON_H__ */
