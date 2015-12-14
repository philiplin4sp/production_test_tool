/******************************************************************************

    file data_element_decode.c  --  SDP data element decoding engine

    Copyright (C) 2002-2006 Cambridge Silicon Radio Ltd.; all rights reserved

    $Log: bnep.c,v $

******************************************************************************/


#include "data_element_decode.h"


/******************************************************************************

    #defines for data element sizes

******************************************************************************/

#define DATA_ELEMENT_SIZE_1 0 /* Or 0 if TYPE_NIL */
#define DATA_ELEMENT_SIZE_2 1
#define DATA_ELEMENT_SIZE_4 2
#define DATA_ELEMENT_SIZE_8 3
#define DATA_ELEMENT_SIZE_16 4
#define DATA_ELEMENT_SIZE_A8 5
#define DATA_ELEMENT_SIZE_A16 6
#define DATA_ELEMENT_SIZE_A32 7


/******************************************************************************

    macro DATA_ELEMENT_TYPE  --  extract the data element type from descriptor
    macro DATA_ELEMENT_SIZE  --  extract the data element size from descriptor

******************************************************************************/

#define DATA_ELEMENT_TYPE(x) ((x) >> 3)
#define DATA_ELEMENT_SIZE(x) ((x) & 7)


/******************************************************************************

    variable data_element_errno  --  last error encountered

******************************************************************************/

static unsigned data_element_errno;


/******************************************************************************

    function data_element_get_size  --  get a data element size

On entry, *buf should point at the data element descriptor.

On exit, the size is returned and *buf and *len are updated,
except if an error occurs, in which case DATA_ELEMENT_A_BAD_SIZE
is returned and *buf and *len are preserved.

******************************************************************************/

static uint32 data_element_get_size (unsigned des, const uint8 **buf, uint32 *len, void (*handle_element) (unsigned type, const uint8 *buf, uint32 len))
{
    uint32 size;
#define DATA_ELEMENT_A_BAD_SIZE ((uint32) -1)

    switch (des)
    {
    case DATA_ELEMENT_SIZE_A8:
        if (*len < 1)
        {
            data_element_errno = DATA_ELEMENT_ERROR_TRUNCATED;
            handle_element (DATA_ELEMENT_ERROR_TRUNCATED, NULL, 0);
            return DATA_ELEMENT_A_BAD_SIZE;
        }
        ++*buf;
        --*len;
        *len -= 1;
        size = *(*buf)++;
        return size;

    case DATA_ELEMENT_SIZE_A16:
        if (*len < 2)
        {
            data_element_errno = DATA_ELEMENT_ERROR_TRUNCATED;
            handle_element (DATA_ELEMENT_ERROR_TRUNCATED, NULL, 1);
            return DATA_ELEMENT_A_BAD_SIZE;
        }
        ++*buf;
        --*len;
        *len -= 2;
        size = *(*buf)++;
        size <<= 8;
        size |= *(*buf)++;
        return size;

    case DATA_ELEMENT_SIZE_A32:
        if (*len < 4)
        {
            data_element_errno = DATA_ELEMENT_ERROR_TRUNCATED;
            handle_element (DATA_ELEMENT_ERROR_TRUNCATED, NULL, 2);
            return DATA_ELEMENT_A_BAD_SIZE;
        }
        ++*buf;
        --*len;
        *len -= 4;
        size = *(*buf)++;
        size <<= 8;
        size |= *(*buf)++;
        size <<= 8;
        size |= *(*buf)++;
        size <<= 8;
        size |= *(*buf)++;
        return size;
    }

    data_element_errno = DATA_ELEMENT_ERROR_INTERNAL;
    handle_element (DATA_ELEMENT_ERROR_INTERNAL, NULL, 0);
    return DATA_ELEMENT_A_BAD_SIZE;
}


/******************************************************************************

    function data_element_decode_internal  --  decode a data element

Internal function used so that it can be wrapped around errno handling.

******************************************************************************/

static void data_element_decode_internal (const uint8 **buf, uint32 *len, void (*handle_element) (unsigned type, const uint8 *buf, uint32 len))
{
    unsigned des, det;
    uint32 size;

    if (*len == 0)
    {
        data_element_errno = DATA_ELEMENT_ERROR_TRUNCATED;
        handle_element (DATA_ELEMENT_ERROR_TRUNCATED, NULL, 3);
    }
    else
    {
        det = DATA_ELEMENT_TYPE (**buf);
        des = DATA_ELEMENT_SIZE (**buf);
        switch (det)
        {
        case DATA_ELEMENT_TYPE_NIL:
            if (des != DATA_ELEMENT_SIZE_1)
            {
                data_element_errno = DATA_ELEMENT_ERROR_INVALID;
                handle_element (DATA_ELEMENT_ERROR_INVALID, NULL, 0);
            }
            else
            {
                ++*buf;
                --*len;
                handle_element (det, NULL, 0);
            }
            break;

        case DATA_ELEMENT_TYPE_UUID:
            if (des != DATA_ELEMENT_SIZE_2 &&
                des != DATA_ELEMENT_SIZE_4 &&
                des != DATA_ELEMENT_SIZE_16)
            {
                data_element_errno = DATA_ELEMENT_ERROR_INVALID;
                handle_element (DATA_ELEMENT_ERROR_INVALID, NULL, 0);
                break;
            }
            /* Deliberate fall-through */
        case DATA_ELEMENT_TYPE_UINT:
        case DATA_ELEMENT_TYPE_INT:
            if (des > DATA_ELEMENT_SIZE_16)
            {
                data_element_errno = DATA_ELEMENT_ERROR_INVALID;
                handle_element (DATA_ELEMENT_ERROR_INVALID, NULL, 0);
            }
            else
            {
                size = 1 << des;
                if (*len < 1 + size)
                {
                    data_element_errno = DATA_ELEMENT_ERROR_TRUNCATED;
                    handle_element (DATA_ELEMENT_ERROR_TRUNCATED, NULL, 4);
                }
                else
                {
                    ++*buf;
                    --*len;
                    handle_element (det, *buf, size);
                    *buf += size;
                    *len -= size;
                }
            }
            break;

        case DATA_ELEMENT_TYPE_TEXT:
        case DATA_ELEMENT_TYPE_URL:
            if (des < DATA_ELEMENT_SIZE_A8)
            {
                data_element_errno = DATA_ELEMENT_ERROR_INVALID;
                handle_element (DATA_ELEMENT_ERROR_INVALID, NULL, 0);
            }
            else
            {
                uint32 save_len = *len;

                if ((size = data_element_get_size (des, buf, len, handle_element)) != DATA_ELEMENT_A_BAD_SIZE)
                {
                    if (*len < size)
                    {
                        *buf -= save_len - *len;
                        *len = save_len;
                        data_element_errno = DATA_ELEMENT_ERROR_TRUNCATED;
                        handle_element (DATA_ELEMENT_ERROR_TRUNCATED, NULL, 5);
                    }
                    else
                    {
                        handle_element (det, *buf, size);
                        *buf += size;
                        *len -= size;
                    }
                }
            }
            break;

        case DATA_ELEMENT_TYPE_BOOL:
            if (des != DATA_ELEMENT_SIZE_1)
            {
                data_element_errno = DATA_ELEMENT_ERROR_INVALID;
                handle_element (DATA_ELEMENT_ERROR_INVALID, NULL, 0);
            }
            else
            {
                if (*len < 2)
                {
                    data_element_errno = DATA_ELEMENT_ERROR_TRUNCATED;
                    handle_element (DATA_ELEMENT_ERROR_TRUNCATED, NULL, 6);
                }
                else
                {
                    ++*buf;
                    --*len;
                    handle_element (det, *buf, 1);
                    ++*buf;
                    --*len;
                }
            }
            break;

        case DATA_ELEMENT_TYPE_SEQ:
        case DATA_ELEMENT_TYPE_ALT:
            if (des < DATA_ELEMENT_SIZE_A8)
            {
                data_element_errno = DATA_ELEMENT_ERROR_INVALID;
                handle_element (DATA_ELEMENT_ERROR_INVALID, NULL, 0);
            }
            else
            {
                uint32 save_len = *len;

                if ((size = data_element_get_size (des, buf, len, handle_element)) != DATA_ELEMENT_A_BAD_SIZE)
                {
                    if (*len < size)
                    {
                        *buf -= save_len - *len;
                        *len = save_len;
                        data_element_errno = DATA_ELEMENT_ERROR_TRUNCATED;
                        handle_element (DATA_ELEMENT_ERROR_TRUNCATED, NULL, 7);
                    }
                    else
                    {
                        save_len = *len;
                        handle_element (det | DATA_ELEMENT_TYPE_START, NULL, 0);
                        while (save_len - *len != size)
                        {
                            data_element_decode (buf, len, handle_element);
                            if (data_element_errno)
                            {
                                break;
                            }
                            if (save_len - *len != size)
                            {
                                handle_element (det | DATA_ELEMENT_TYPE_SEPARATOR, NULL, 0);
                            }
                        }
                        handle_element (det | DATA_ELEMENT_TYPE_END, NULL, 0);
                    }
                }
            }
            break;

        default:
            data_element_errno = DATA_ELEMENT_ERROR_INVALID;
            handle_element (DATA_ELEMENT_ERROR_INVALID, NULL, 0);
            break;
        }
    }
}


/******************************************************************************

    function data_element_decode  --  decode a data element

******************************************************************************/

bool data_element_decode (const uint8 **buf, uint32 *len, void (*handle_element) (unsigned type, const uint8 *buf, uint32 len))
{
    data_element_errno = 0;

    data_element_decode_internal (buf, len, handle_element);

    return data_element_errno == 0;
}
