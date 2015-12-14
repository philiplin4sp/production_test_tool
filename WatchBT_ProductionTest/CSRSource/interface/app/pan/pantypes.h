#ifndef PANTYPES_H
#define PANTYPES_H

#ifdef BLUELAB

/* In the VM */

#include <csrtypes.h>
#include "app/bluestack/types.h"
#include "app/bluestack/bluetooth.h"
#include <stream.h>

#else /* !BLUELAB */
#ifdef BTCHIP

/* On chip, in the firmware */

#include "bt.h"
#include "app/bluestack/types.h"
#include "app/bluestack/bluetooth.h"
#include "mblk.h"
#include "buffer.h"

typedef BUFFER *BUFFER_HANDLE;

#else /* !BTCHIP (&& !BLUELAB) */

/* Off-chip */

#include "common/types.h"

typedef uint16 BUFFER_HANDLE;

#endif /* (!)BTCHIP (&& !BLUELAB) */

/* Not in the VM */

#define BUFFER_HANDLE_NONE ((BUFFER_HANDLE) 0)

#endif /* (!)BLUELAB */

#endif /* PANTYPES_H */
