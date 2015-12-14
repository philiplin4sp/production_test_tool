////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) Cambridge Silicon Radio Ltd 2004-2009
//
//  FILE:    sluttable.cpp
//
//  AUTHOR:  ckl
//
//  PURPOSE: Class tha
//
//  USES:    hi_res_timer.h
//
//  CLASS:   SlutTable, SlutTableEntries
//
////////////////////////////////////////////////////////////////////////////////


#include <map>
#include "assert.h"
#include "slutids.h"
#include "sluttable.h"
#include "chiputil/xaphelper.h"
#include "chiputil/chiphelper.h"
#include "time/hi_res_clock.h"


const int SLUT_MAGIC = 0xD397;
const int SLUT_PTR_MAGIC = 0x793D;

class SlutTable::Entries
{
public:
    typedef std::map<uint16, uint32> SlutMap_T;
    SlutMap_T map_;
    void clear() {map_.clear();};
    void add(uint16 key, uint32 value) {map_.insert(SlutMap_T::value_type(key, value));};
    bool key_exists(int16 key) { return map_.find(key) != map_.end();};
    uint32 val(int16 key) { return map_[key]; }
};


SlutTable::SlutTable(IMemoryReaderGP *reader, CXapHelper *xap_helper)
{
    assert(reader);
    readergp_ = reader;
    xap_helper_ = xap_helper;
    entries_ = NULL;
}

SlutTable::~SlutTable()
{
    delete entries_;
}


bool SlutTable::read_slut(SlutType type, bool force_reread)
{
    bool result = false;
    assert(readergp_ != NULL);
    if (readergp_ == NULL) return false;

    if(entries_ && this->type == type && !force_reread)
    {
        // we've read the slut of this type and we're not being forced to do it again, so we're done.
        return true;
    }

    invalidate();
    this->type = type;

    struct SlutLocation
    {
        bool (SlutTable::* read_function)(GenericPointer, bool);
        GenericPointer gp;
    };


    static const size_t MAX_SLUT_POSSIBILITIES = 4;

    /* Define SlutLocation constants for the various candidate addresses of SLUTs */
    static const SlutLocation raw_prebc7 = { &SlutTable::read_slut_bc, GenericPointer(Mem_RawData, 0x100) };
    static const SlutLocation raw_bc7plus = { &SlutTable::read_slut_bc, GenericPointer(Mem_RawData, 0x80) };
    static const SlutLocation loader_prebc7 = { &SlutTable::read_slut_bc, GenericPointer(Mem_Flash, 0x100) };
    static const SlutLocation loader_bc7plus = { &SlutTable::read_slut_bc_loader, GenericPointer(Mem_Flash, 0x80) };
    static const SlutLocation stack_prebc7 = { &SlutTable::read_slut_bc, GenericPointer(Mem_Flash, 0x4100) };
    static const SlutLocation stack_bc7plus_monolithic = { &SlutTable::read_slut_bc_stack, GenericPointer(Mem_Flash, 0x80) };
    static const SlutLocation stack_bc7plus = { &SlutTable::read_slut_bc, GenericPointer(Mem_Flash, 0x4080) };

    size_t num_slut_possibilities = 0;
    const SlutLocation *slut_possibilities[MAX_SLUT_POSSIBILITIES];

    switch(type)
    {
    case SlutType_Raw:
        num_slut_possibilities = 2;
        slut_possibilities[0] = &raw_prebc7;
        slut_possibilities[1] = &raw_bc7plus;
        if(xap_helper_ && xap_helper_->IsReadyForUse())
        {
            switch(xap_helper_->GetChipFamily())
            {
            case CHIP_FAMILY_BLUECORE:
                num_slut_possibilities = 1;
                slut_possibilities[0] = xap_helper_->IsPreBc7() ? &raw_prebc7 : &raw_bc7plus;
                break;
            default:
            {
                break;
            }
            }
        }
        else
        {
        }
        break;
    case SlutType_FlashLoader:

        num_slut_possibilities = 2;
        slut_possibilities[0] = &loader_prebc7;
        slut_possibilities[1] = &loader_bc7plus;
        if(xap_helper_ && xap_helper_->IsReadyForUse())
        {
            if(xap_helper_->CanHaveLoader())
            {
                switch(xap_helper_->GetChipFamily())
                {
                case CHIP_FAMILY_BLUECORE:
                    num_slut_possibilities = 1;
                    slut_possibilities[0] = xap_helper_->IsPreBc7() ? &loader_prebc7 : &loader_bc7plus;
                    break;
                default:
                    // Not a chip type we recognise. Read the full set of slut possibilities for this slut type.
                    break;
                }
            }
            else
            {
                // this chip cannot contain a loader slut.
                num_slut_possibilities = 0;
            }
        }
        break;
    case SlutType_FlashStack:
        num_slut_possibilities = 3;
        slut_possibilities[0] = &stack_prebc7;
        slut_possibilities[1] = &stack_bc7plus_monolithic;
        slut_possibilities[2] = &stack_bc7plus;
        if(xap_helper_ && xap_helper_->IsReadyForUse())
        {
            switch(xap_helper_->GetChipFamily())
            {
            case CHIP_FAMILY_BLUECORE:
                if (xap_helper_->IsPreBc7())
                {
                    num_slut_possibilities = 1;
                    slut_possibilities[0] = &stack_prebc7;
                }
                else
                {
                    if (xap_helper_->CanHaveLoader())
                    {
                        num_slut_possibilities = 2;
                        slut_possibilities[0] = &stack_bc7plus;
                        slut_possibilities[1] = &stack_bc7plus_monolithic;
                    }
                    else
                    {
                        num_slut_possibilities = 1;
                        slut_possibilities[0] = &stack_bc7plus_monolithic;
                    }
                }
                break;
            default:
                // Not a chip type we recognise. Read the full set of slut possibilities for this slut type.
                break;
            }
        }
        break;
    case SlutType_Rom:
        break;
    default:
        // a slut type we don't recognise. in release mode don't read anything. in debug assert as this is probably an internal error.
        assert(false);
    }

    // go through the slut possibilities and try to read them.
    for(size_t i = 0; i < num_slut_possibilities && !result; ++i)
    {
        // if this is the only slut then retry because we expect it to be there.
        // if not then don't retry to go through the possibilities quicker.
#       define CALL_MEMBER_FN(fn) (this->*(fn))
        if (CALL_MEMBER_FN(slut_possibilities[i]->read_function)(slut_possibilities[i]->gp, num_slut_possibilities == 1))
        {
            result = true;
        }
    }
    return result;
}



bool SlutTable::read_slut_bc(GenericPointer slut_location, bool retry)
{
    GenericPointer slut_ptr;
    uint16 data_buf[IMemoryReaderGP::MAX_BLOCK_SIZE];
    uint32 location;
    uint16 slut_key;
    uint16 slut_val;
    bool stack = false;

    if(!read_slut_header(slut_location, data_buf, retry))
    {
        return false;
    }

    // If we don't find the magic value then give up.
    if (data_buf[0] != SLUT_MAGIC)
        return false;

    // Check if we're looking in the stack.
    if (slut_location.Mem() == Mem_Flash && (int)slut_location >= 0x4000)
        stack = true;

    // If we're looking in the stack part of flash then offset the
    // 16-bit location so that it is also in the stack.
    location = data_buf[1];
    if (stack) location += 0x4000;

    // Create a pointer to the actual SLUT.
    slut_ptr = slut_location = GenericPointer(slut_location.Mem(), location);

    unifiSlut = false;
    entries_ = new Entries;
    assert(entries_);

    do 
    {
        if (!readergp_->read_mem(slut_ptr, IMemoryReaderGP::MAX_BLOCK_SIZE, data_buf))
        {
            invalidate();
            return false;
        }
        int i;
        for (i = 0; i < IMemoryReaderGP::MAX_BLOCK_SIZE; i+= 2)
        {
            slut_key = data_buf[i];
            slut_val = data_buf[i + 1];

            // If the value is a pointer add an offset.
            if (slut_key == bluecore::SLUTID_ID_STRING ||
                slut_key == bluecore::SLUTID_ID_INTEGER ||
                slut_key == bluecore::SLUTID_ID_STRING_FULL)
            {
                if (stack) slut_val += 0x4000;
            }

            if (slut_key == bluecore::SLUTID_NONE)
                break;
            entries_->add(slut_key, slut_val);
        }
        slut_ptr += i;
    } while ((slut_key != bluecore::SLUTID_NONE) && ((int)slut_ptr - (int)slut_location < 256));
    return true;
}


//**********************************************************
// read a slut and then check that it's a loader or stack slut
// if expect_stack is true and it's not a stack slut then reject, and vice versa.
//**********************************************************
bool SlutTable::read_slut_bc_check_type(GenericPointer slut_location, bool retry, bool expect_stack)
{
    bool result = read_slut_bc(slut_location, retry);
    if (result)
    {
        result = (entry_exists(bluecore::SLUTID_BCCMD_INTERFACE) == expect_stack);
        if (!result)
        {
            invalidate();
        }
    }
    return result;
}

//**********************************************************
// read a memory location looking for the magic numbers that indicate a slut header.
// if retry is true then we expect it to be there so try it a few times.
//**********************************************************
bool SlutTable::read_slut_header(GenericPointer slut_location, uint16 *data_buf, bool retry)
{
    int tries = retry ? 3 : 1;
    while(tries > 0 &&
          !(readergp_->read_mem(slut_location, 3, data_buf) &&
             (data_buf[0] == SLUT_MAGIC || data_buf[0] == SLUT_PTR_MAGIC)))
    {
        HiResClockSleepMilliSec(100);
        --tries;
    }
    return tries > 0;
}

bool SlutTable::entry_exists(uint16 key) const
{
    return entries_ ? entries_->key_exists(key) : false;
}

uint32 SlutTable::getValue(uint16 key) const
{
    assert(entries_ != NULL);
    return entries_->val(key);
}

void SlutTable::invalidate()
{
    delete entries_;
    entries_ = NULL;
}
