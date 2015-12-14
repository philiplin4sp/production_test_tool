////////////////////////////////////////////////////////////////////////////////
//
//  File:       xtrabccmdkaldebugpdu.h
//
//  Class:      ...
//
//  Purpose:    PDUs for debugging the Kalimba over BCCMD.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef XTRABCCMDVMDEBUGPDU_H
#define XTRABCCMDVMDEBUGPDU_H

#include "hcipacker/gen_bccmd.h"

class BCCMD_KALDEBUG_SIMPLE_PDU : public BCCMD_KALDEBUG_PDU
{
    public:
        BCCMD_KALDEBUG_SIMPLE_PDU();
        BCCMD_KALDEBUG_SIMPLE_PDU(const PDU&);
        BCCMD_KALDEBUG_SIMPLE_PDU(const uint8 *buffer, uint32 len);
        uint16 get_u16(void) const;
        void set_u16(uint16 value);
};

class BCCMD_KALDEBUG_CONTEXT_PDU : public BCCMD_KALDEBUG_PDU
{
    public:
        BCCMD_KALDEBUG_CONTEXT_PDU();
        BCCMD_KALDEBUG_CONTEXT_PDU(const PDU&);
        BCCMD_KALDEBUG_CONTEXT_PDU(const uint8 *buffer, uint32 len);
        uint16 get_state(void) const;
        void set_state(uint16 value);
        void get_reg(uint16 *copyto, uint16 len) const;
        void set_reg(const uint16 *copyto, uint16 len);
        static const int num_words_in_regs;
};

class BCCMD_KALDEBUG_WRITEREG_PDU : public BCCMD_KALDEBUG_PDU
{
    public:
        BCCMD_KALDEBUG_WRITEREG_PDU();
        BCCMD_KALDEBUG_WRITEREG_PDU(const PDU&);
        BCCMD_KALDEBUG_WRITEREG_PDU(const uint8 *buffer, uint32 len);
        uint16 get_reg(void) const;
        void set_reg(uint16 value);
        uint16 get_lsw(void) const;
        void set_lsw(uint16 value);
        uint16 get_msb(void) const;
        void set_msb(uint16 value);
};

class BCCMD_KALDEBUG_READDM_PDU : public BCCMD_KALDEBUG_PDU
{
    public:
        BCCMD_KALDEBUG_READDM_PDU();
        BCCMD_KALDEBUG_READDM_PDU(const PDU&);
        BCCMD_KALDEBUG_READDM_PDU(const uint8 *buffer, uint32 len);
        uint16 get_start_u24(void) const;
        void set_start_u24(uint16 value);
        uint16 get_num_u16(void) const;
        void set_num_u16(uint16 value);
        void get_data(uint16 *copyto, uint16 len) const;
        void set_data(const uint16 *copyto, uint16 len);
};

class BCCMD_KALDEBUG_WRITEDM_PDU : public BCCMD_KALDEBUG_PDU
{
    public:
        BCCMD_KALDEBUG_WRITEDM_PDU();
        BCCMD_KALDEBUG_WRITEDM_PDU(const PDU&);
        BCCMD_KALDEBUG_WRITEDM_PDU(const uint8 *buffer, uint32 len);
        uint16 get_start_u24(void) const;
        void set_start_u24(uint16 value);
        uint16 get_num_u16(void) const;
        void set_num_u16(uint16 value);
        void get_data(uint16 *copyto, uint16 len) const;
        void set_data(const uint16 *copyto, uint16 len);
};

class BCCMD_KALDEBUG_READPM_PDU : public BCCMD_KALDEBUG_PDU
{
    public:
        BCCMD_KALDEBUG_READPM_PDU();
        BCCMD_KALDEBUG_READPM_PDU(const PDU&);
        BCCMD_KALDEBUG_READPM_PDU(const uint8 *buffer, uint32 len);
        uint16 get_start_u32(void) const;
        void set_start_u32(uint16 value);
        uint16 get_num_u16(void) const;
        void set_num_u16(uint16 value);
        void get_data(uint16 *copyto, uint16 len) const;
        void set_data(const uint16 *copyto, uint16 len);
};

class BCCMD_KALDEBUG_WRITEPM_PDU : public BCCMD_KALDEBUG_PDU
{
    public:
        BCCMD_KALDEBUG_WRITEPM_PDU();
        BCCMD_KALDEBUG_WRITEPM_PDU(const PDU&);
        BCCMD_KALDEBUG_WRITEPM_PDU(const uint8 *buffer, uint32 len);
        uint16 get_start_u32(void) const;
        void set_start_u32(uint16 value);
        uint16 get_num_u16(void) const;
        void set_num_u16(uint16 value);
        void get_data(uint16 *copyto, uint16 len) const;
        void set_data(const uint16 *copyto, uint16 len);
};

class BCCMD_KALDEBUG_SETDATABREAKPOINT_PDU : public BCCMD_KALDEBUG_PDU
{
    public:
        BCCMD_KALDEBUG_SETDATABREAKPOINT_PDU();
        BCCMD_KALDEBUG_SETDATABREAKPOINT_PDU(const PDU&);
        BCCMD_KALDEBUG_SETDATABREAKPOINT_PDU(const uint8 *buffer, uint32 len);
        uint16 get_addr(void) const;
        void set_addr(uint16 value);
        uint16 get_read(void) const;
        void set_read(uint16 value);
        uint16 get_write(void) const;
        void set_write(uint16 value);
};

class BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT_PDU : public BCCMD_KALDEBUG_PDU
{
    public:
        BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT_PDU();
        BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT_PDU(const PDU&);
        BCCMD_KALDEBUG_SETNUMBEREDPMBREAKPOINT_PDU(const uint8 *buffer, uint32 len);
        uint16 get_addr(void)const;
        void set_addr(uint16 value);
        uint16 get_brk_point_number(void)const;
        void set_brk_point_number(uint16 value);
};

#endif

