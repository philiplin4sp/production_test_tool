/*
 * bccmdbc3psupdu.h
 *
 * Old BCCMD PDU for testing bc3 psu, battery charger,
 * and associated functionality. This is not supported in
 * newer firmware releases. There are now separate BCCMDs for
 * each piece of battery charger functionality.
 */

#ifndef BCCMDBC3PSUPDU_H
#define BCCMDBC3PSUPDU_H

typedef enum {
     BCCMDPDU_BC3PSU_PSU_TRIM,
     BCCMDPDU_BC3PSU_BUCK_REG,
     BCCMDPDU_BC3PSU_MIC_EN,
     BCCMDPDU_BC3PSU_LED0,
     BCCMDPDU_BC3PSU_LED1,
     BCCMDPDU_BC3PSU_PSU_CURRENT_TRIM
} BCCMDPDU_BC3PSU_FUNCTION;

typedef struct {
    uint16 function;
    uint16 data ;
} BCCMDPDU_BC3PSU;

#endif
