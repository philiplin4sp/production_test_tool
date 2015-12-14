/* Common decode functions for FM/RDS data */
/* This file supports BOTH the receive and transmit parts of the FM-API */


#ifndef FMDECODE_H
#define FMDECODE_H

#include "btcli.h"

/* Decode and print the contents of an FM register message*/
bool decodeFMReg (u32 regID, u32 content);

/* Decode and print the contents of an RDS register read or HQ message*/
void decodeRDSBlock (u32 val, u32 stat);

/* Decode and print the contents of the Misc register */
void decodeFMRegMisc(u32 content);

/* Decode and print the contents of an FMTX register message*/
extern bool decodeFMTXReg(u32 regID, u32 content);

/* Decode and print the contents of an FMTX TEXT message*/
extern void decodeRDSTEXT(const uint32* payload);


#endif /*FMDECODE_H*/
