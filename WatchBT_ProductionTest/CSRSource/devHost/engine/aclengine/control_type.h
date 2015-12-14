#ifndef CONTROL_TYPE_H
#define CONTROL_TYPE_H

enum ControlType 
{
    C_START,
#ifdef SEND_START_ACK_ENABLED
    START_ACK,
#endif
    START_NAK,
    CANCEL_SEND, 
    CANCEL_RECEIVE,
    FINISH,
    FINISH_ACK
};

#endif // CONTROL_TYPE_H
