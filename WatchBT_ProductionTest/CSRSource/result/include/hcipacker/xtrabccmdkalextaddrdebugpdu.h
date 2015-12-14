////////////////////////////////////////////////////////////////////////////////
//
//  File:       xtrabccmdkalextaddrdebugpdu.h
//
//  Class:      ...
//
//  Purpose:    PDUs for debugging the Kalimba over BCCMD.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef XTRABCCMDKALEXTADDRDEBUGPDU_H
#define XTRABCCMDKALEXTADDRDEBUGPDU_H

#include "hcipacker/gen_bccmd.h"

class BCCMD_KALEXTADDRDEBUG_SIMPLE_PDU : public BCCMD_KALEXTADDRDEBUG_PDU
{
    public:
        BCCMD_KALEXTADDRDEBUG_SIMPLE_PDU();
        uint16 get_u16(void) const;
        void set_u16(uint16 value);
};

class BCCMD_KALEXTADDRDEBUG_READDM_PDU : public BCCMD_KALEXTADDRDEBUG_PDU
{
    public:
        BCCMD_KALEXTADDRDEBUG_READDM_PDU();
        void set_start(uint32 value);
        uint16 get_num_u16(void) const;
        void set_num_u16(uint16 value);
        void get_data(uint16 *copyto, uint16 len) const;
};

class BCCMD_KALEXTADDRDEBUG_WRITEDM_PDU : public BCCMD_KALEXTADDRDEBUG_PDU
{
    public:
        BCCMD_KALEXTADDRDEBUG_WRITEDM_PDU();
        void set_start(uint32 value);
        uint16 get_num_u16(void) const;
        void set_num_u16(uint16 value);
        void set_data(const uint16 *copyto, uint16 len);
};

#endif
