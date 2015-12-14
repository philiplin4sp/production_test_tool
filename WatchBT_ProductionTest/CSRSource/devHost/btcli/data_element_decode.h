/******************************************************************************

    file data_element_decode.h  --  SDP data element decoding engine

    Copyright (C) 2002-2006 Cambridge Silicon Radio Ltd.; all rights reserved

    $Log: bnep.c,v $

******************************************************************************/


#if 1
#include "types.h"
#else
typedef unsigned char bool;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
#define FALSE 0
#define TRUE 1
#define NULL 0
#endif


/******************************************************************************

    #defines for data element types, and special tokens

******************************************************************************/

#define DATA_ELEMENT_TYPE_NIL 0
#define DATA_ELEMENT_TYPE_UINT 1
#define DATA_ELEMENT_TYPE_INT 2
#define DATA_ELEMENT_TYPE_UUID 3
#define DATA_ELEMENT_TYPE_TEXT 4
#define DATA_ELEMENT_TYPE_BOOL 5
#define DATA_ELEMENT_TYPE_SEQ 6
#define DATA_ELEMENT_TYPE_START 0x40
#define DATA_ELEMENT_TYPE_SEPARATOR 0x50
#define DATA_ELEMENT_TYPE_END 0x60
#define DATA_ELEMENT_TYPE_SEQ_START (DATA_ELEMENT_TYPE_SEQ | DATA_ELEMENT_TYPE_START)
#define DATA_ELEMENT_TYPE_SEQ_SEPARATOR (DATA_ELEMENT_TYPE_SEQ | DATA_ELEMENT_TYPE_SEPARATOR)
#define DATA_ELEMENT_TYPE_SEQ_END (DATA_ELEMENT_TYPE_SEQ | DATA_ELEMENT_TYPE_END)
#define DATA_ELEMENT_TYPE_ALT 7
#define DATA_ELEMENT_TYPE_ALT_START (DATA_ELEMENT_TYPE_ALT | DATA_ELEMENT_TYPE_START)
#define DATA_ELEMENT_TYPE_ALT_SEPARATOR (DATA_ELEMENT_TYPE_ALT | DATA_ELEMENT_TYPE_SEPARATOR)
#define DATA_ELEMENT_TYPE_ALT_END (DATA_ELEMENT_TYPE_ALT | DATA_ELEMENT_TYPE_END)
#define DATA_ELEMENT_TYPE_URL 8
#define DATA_ELEMENT_ERROR_TRUNCATED 0xf0
#define DATA_ELEMENT_ERROR_INVALID 0xf1
#define DATA_ELEMENT_ERROR_INTERNAL 0xf2


/******************************************************************************

    function data_element_decode  --  decode a data element

The data element is specified as a pointer to a pointer and a length.
These pointers are updated to point to the end of the data element
on success; on failure they are updated to point at the failure.

The handler is called for each data element.  If the pointer passed
is NULL, or the type is *_NIL or *_SEQ_* or *_ALT_* or *_ERROR_*
then both the pointer and the length passed should be ignored.
Otherwise, the len octets at buf are guaranteed to be valid.

This function returns TRUE iff no errors were detected.

******************************************************************************/

bool data_element_decode (const uint8 **buf, uint32 *len,
                          void (*handle_element) (unsigned type,
                                                const uint8 *buf, uint32 len));
