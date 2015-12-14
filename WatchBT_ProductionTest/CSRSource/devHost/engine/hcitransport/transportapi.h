//////////////////////////////////////////////////////////////////////////////
//
//  FILE   :    transportapi.h
//
//  Copyright CSR 2001-2006
//
//  PURPOSE:    A place to hold things which leaked from the physical
//              transport abstraction layer.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __TRANSPORTAPI_H__
#define __TRANSPORTAPI_H__

//  For H4plus and H5 there is explicit out of band signalling
//  that controls when the BlueCore sleeps.  There are three values that
//  can be given for the sleep control parameter:
//
//  NEVER (default) The BlueCore is never allowed to go to sleep.
//  ALWAYS          The BlueCore is set to sleep whenever possible.  It
//                  is woken up if we need to talk to it, but then it 
//                  is put back to sleep,
//  NOW             The BlueCore is set to sleep now, but if we need to
//                  talk to it again, we wake it up,  It then remains 
//                  awake until further processing.

enum SleepType
{
    SLEEP_TYPE_NEVER,
    SLEEP_TYPE_ALWAYS,
    SLEEP_TYPE_NOW
};

enum FailureMode
{
    no_failure,
    bcsp_sync_recvd,
    bcsp_retry_limit,
    h4_sync_loss,
    uart_failure,
    usb_handle_loss,
    sdio_transport_fail
};

#endif//__TRANSPORTAPI_H__
