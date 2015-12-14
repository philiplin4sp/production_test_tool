/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

/*
 * The definitions of GUIDs visible to the users go here
 */

/*
 * GUID definition are required to be outside of header inclusion pragma to
 * avoid error during precompiled headers.
 */
#ifndef _CDC_API_H_
#define _CDC_API_H_

#include "devioctl.h"

#define IOCTL_ACM_SEND_ENCAPSULATED_COMMAND \
    CTL_CODE(FILE_DEVICE_SERIAL_PORT, 128, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_ACM_GET_ENCAPSULATED_RESPONSE \
    CTL_CODE(FILE_DEVICE_SERIAL_PORT, 129, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_ACM_WAIT_FOR_ENCAPSULATED_RESPONSE \
    CTL_CODE(FILE_DEVICE_SERIAL_PORT, 130, METHOD_BUFFERED, FILE_ANY_ACCESS)

#endif

