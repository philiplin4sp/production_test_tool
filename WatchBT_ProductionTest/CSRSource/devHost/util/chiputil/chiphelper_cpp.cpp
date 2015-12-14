/**********************************************************************
*
*  FILE   :  chiphelper_cpp.cpp
*
*            Copyright (C) Cambridge Silicon Radio Ltd 2007-2009
*
*  PURPOSE:  ChipHelper C++ wrapper.
*
*  $Id: $
*
***********************************************************************/


#include "chiphelper_private.h"


ChipHelper::ChipHelper() : 
    m_desc(ChipHelper_Null())
{
}

ChipHelper::ChipHelper(ChipDescript *desc) : 
    m_desc(desc)
{
}

ChipHelper::ChipHelper(CsrUint16 ver) :
    m_desc(ChipHelper_GetVersionAny(0x0000, ver))
{
}

void ChipHelper::ClearVersion()
{
    m_desc = ChipHelper_Null();
}

void ChipHelper::GetVersionSdio(CsrUint8 sdio_ver)
{
    m_desc = ChipHelper_GetVersionSdio(sdio_ver);
}

void ChipHelper::GetVersionUniFi(CsrUint16 ver)
{
    m_desc = ChipHelper_GetVersionAny(0x0000, ver);
}

void ChipHelper::GetVersionAny(CsrUint16 from_FF9A, CsrUint16 from_FE81)
{
    m_desc = ChipHelper_GetVersionAny(from_FF9A, from_FE81);
}

void ChipHelper::GetVersionBlueCore(chip_helper_bluecore_age bc_age, CsrUint16 version)
{
    m_desc = ChipHelper_GetVersionBlueCore(bc_age, version);
}


#define CHIP_HELPER_DEF0_CPP_DEF(ret_type, name, info)          \
ret_type ChipHelper:: name() const                              \
{                                                               \
    return ChipHelper_ ## name(m_desc);                         \
}

#define CHIP_HELPER_DEF1_CPP_DEF(ret_type, name, type1, name1)      \
ret_type ChipHelper:: name(type1 name1) const                       \
{                                                                   \
    return ChipHelper_ ## name(m_desc, name1);                      \
}

CHIP_HELPER_LIST(CPP_DEF)

CsrInt32 ChipHelper::DecodeWindow(chip_helper_window_index window,
                             chip_helper_window_type type,
                             CsrUint32 offset,
                             CsrUint16 &page, CsrUint16 &addr, CsrUint32 &len) const
{
    return ChipHelper_DecodeWindow(m_desc, window, type, offset, &page, &addr, &len);
}
