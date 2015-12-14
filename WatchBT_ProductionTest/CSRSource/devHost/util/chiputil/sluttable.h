#ifndef CHIPUTIL__SLUTTABLE_H_INCLUDED
#define CHIPUTIL__SLUTTABLE_H_INCLUDED

/**********************************************************************
*
*  FILE   :  sluttable.h
*
*            Copyright (C) Cambridge Silicon Radio Ltd 2004-2009
*
*  PURPOSE:  SlutTable class: Used to read a SLUT table and provide 
*            access to the keys.
*
*  $Id: $
*
***********************************************************************/

#include "abstract/memory_reader.h"
#include "common/types.h"

enum SlutType
{
    SlutType_Raw = 0,
    SlutType_FlashLoader,
    SlutType_FlashStack,
    SlutType_Rom
};

class CXapHelper;
class ChipHelper;

class SlutTable
{
public:
    // This class reads a slut table from an IMemoryReader and provides access to the keys.
    // optionally a XapHelper can be passed in to help to find the slut.
    SlutTable(IMemoryReaderGP *reader, CXapHelper *xap_helper = 0);
    ~SlutTable();
    bool read_slut(SlutType type = SlutType_Raw, bool force_reread = false);
    bool entry_exists(uint16 key) const;
    uint32 getValue(uint16 key) const;
    SlutType type;
    bool unifiSlut;     // The SLUT was found in a place that suggests it was unifi.
private:
    bool read_slut_bc(GenericPointer slut_location, bool retry);
    bool read_slut_header(GenericPointer slut_location, uint16 *data_buf, bool retry);
    bool read_slut_bc_loader(GenericPointer slut_location, bool retry)
    {
        return read_slut_bc_check_type(slut_location, retry, false);
    }
    bool read_slut_bc_stack(GenericPointer slut_location, bool retry)
    {
        return read_slut_bc_check_type(slut_location, retry, true);
    }
    bool read_slut_bc_check_type(GenericPointer slut_location, bool retry, bool expect_stack);
    void invalidate();
    IMemoryReaderGP * readergp_;
    CXapHelper * xap_helper_;
    class Entries;
    Entries * entries_;
};

#endif // CHIPUTIL__SLUTTABLE_H_INCLUDED
