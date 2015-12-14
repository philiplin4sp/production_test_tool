/****************************************************************************
 **
 **    XapHelper.cpp  -  Common Xap definitions
 ** 
 ****************************************************************************/

#include "xaphelper.h"

// Mapping from values read from ff9a to chip type
static const VersionMappingStruct gDigitalMapping[] = 
{
    {RAW_DIGITAL_CHIP_VERSION_KAL, CHIP_TYPE_KAL},
    {RAW_DIGITAL_CHIP_VERSION_NNK, CHIP_TYPE_NNK},
    {RAW_DIGITAL_CHIP_VERSION_PDW, CHIP_TYPE_PDW},
    {RAW_DIGITAL_CHIP_VERSION_CYT, CHIP_TYPE_CYT},
    {RAW_DIGITAL_CHIP_VERSION_ODJ, CHIP_TYPE_ODJ},
    {RAW_DIGITAL_CHIP_VERSION_PUG, CHIP_TYPE_PUG},
    {RAW_DIGITAL_CHIP_VERSION_SLY, CHIP_TYPE_SLY},
    {RAW_DIGITAL_CHIP_VERSION_ZBD, CHIP_TYPE_ZBD},
    {RAW_DIGITAL_CHIP_VERSION_ELV, CHIP_TYPE_ELV},
    {RAW_DIGITAL_CHIP_VERSION_JPL, CHIP_TYPE_JPL},
    {RAW_DIGITAL_CHIP_VERSION_JJK, CHIP_TYPE_JJK},
    {RAW_DIGITAL_CHIP_VERSION_PHP, CHIP_TYPE_PHP},
    {RAW_DIGITAL_CHIP_VERSION_HRB, CHIP_TYPE_HRB},
    {RAW_DIGITAL_CHIP_VERSION_FRG, CHIP_TYPE_FRG},
    {RAW_DIGITAL_CHIP_VERSION_SGL, CHIP_TYPE_SGL},
    {RAW_DIGITAL_CHIP_VERSION_MTK, CHIP_TYPE_MTK},
    {RAW_DIGITAL_CHIP_VERSION_AKR, CHIP_TYPE_AKR},
    {RAW_DIGITAL_CHIP_VERSION_GBO, CHIP_TYPE_GBO},

    {RAW_DIGITAL_CHIP_VERSION_INVALID, CHIP_TYPE_UNKNOWN}
};

// Mapping from values read from fe81 to chip type
static const VersionMappingStruct gVersionMapping[] = 
{
    {RAW_CHIP_VERSION_BFT, CHIP_TYPE_BFT},
    {RAW_CHIP_VERSION_DSH, CHIP_TYPE_DSH},
    {RAW_CHIP_VERSION_JEM, CHIP_TYPE_JEM},
    {RAW_CHIP_VERSION_CIN, CHIP_TYPE_CIN},
    {RAW_CHIP_VERSION_ANA, CHIP_TYPE_ANA},
    {RAW_CHIP_VERSION_LPC, CHIP_TYPE_LPC},
    {RAW_CHIP_VERSION_OXY, CHIP_TYPE_OXY},
    {RAW_CHIP_VERSION_GDN, CHIP_TYPE_GDN},
    {RAW_CHIP_VERSION_GEM, CHIP_TYPE_GEM},
    {RAW_CHIP_VERSION_NUT, CHIP_TYPE_NUT},
    {RAW_CHIP_VERSION_ROB, CHIP_TYPE_ROB},
    {RAW_CHIP_VERSION_LEO, CHIP_TYPE_LEO},
    {RAW_CHIP_VERSION_PUR, CHIP_TYPE_PUR},
    {RAW_CHIP_VERSION_VGA, CHIP_TYPE_VGA},
    {RAW_CHIP_VERSION_DAL, CHIP_TYPE_DAL},

    {RAW_CHIP_VERSION_INVALID, CHIP_TYPE_UNKNOWN}
};

// Mapping from chip type to display name
static const DisplayNameMappingStruct gDisplayNameMapping[] =
{
    {CHIP_TYPE_KAL, "BC3 MM (kal)"},
    {CHIP_TYPE_NNK, "BC3 ROM (nnk)"},
    {CHIP_TYPE_PDW, "BC3 Audio Flash (pdw)"},
    {CHIP_TYPE_CYT, "BC4-EXT (cyt)"},
    {CHIP_TYPE_ODJ, "BC4-ROM (odj)"},
    {CHIP_TYPE_PUG, "BC4 Headset (pug)"},
    {CHIP_TYPE_SLY, "BC4 Headset (sly)"},
    {CHIP_TYPE_ZBD, "BC5-ROM (zbd)"},
    {CHIP_TYPE_JJK, "BC4 Audio Flash (jjk)"},
    {CHIP_TYPE_ELV, "BC5-MM (elv)"},
    {CHIP_TYPE_JPL, "BC5-FM-ROM (jpl)"},
    {CHIP_TYPE_PHP, "(php)"},
    {CHIP_TYPE_HRB, "(hrb)"},
    {CHIP_TYPE_FRG, "BC6 Headset (frg)"},
    {CHIP_TYPE_SGL, "BC6-ROM (sgl)"},
    {CHIP_TYPE_MTK, "(mtk)"},
    {CHIP_TYPE_AKR, "(akr)"},
    {CHIP_TYPE_GBO, "CSR6140 (gbo)"},
    {CHIP_TYPE_BFT, "UF1050/1052 (bft)"},
    {CHIP_TYPE_DSH, "CSR1302 (dsh)"},
    {CHIP_TYPE_JEM, "CSR7820/7888 (jem)"},
    {CHIP_TYPE_CIN, "CSR60xx (cin)"},
    {CHIP_TYPE_ANA, "(ana)"},
    {CHIP_TYPE_LPC, "(lpc)"},   // LPC 'chip type' should never be seen
    {CHIP_TYPE_OXY, "CSR9830 (oxy)"},
    {CHIP_TYPE_GDN, "CSR8670 (gdn)"},
    {CHIP_TYPE_GEM, "gem"},
    {CHIP_TYPE_NUT, "CSR8820 (nut)"},
    {CHIP_TYPE_ROB, "(rob)"},
    {CHIP_TYPE_LEO, "(leo)"},
    {CHIP_TYPE_PUR, "(pur)"},
    {CHIP_TYPE_VGA, "(vga)"},
    {CHIP_TYPE_DAL, "dal"},
    
    {CHIP_TYPE_UNKNOWN, "Unknown Device"}
};

/**************************************************************************************************************
 * Constructor 
 **************************************************************************************************************/
CXapHelper::CXapHelper()
{
    Init();
}

/**************************************************************************************************************
 * Constructor that sets chip versions
 **************************************************************************************************************/
CXapHelper::CXapHelper(uint16 ff9a, uint16 fe81) 
{
    Init();
    SetFF9A(ff9a);
    SetFE81(fe81);
}

/**************************************************************************************************************
 * Common constructor initialisation
 **************************************************************************************************************/
void CXapHelper::Init()
{
    ClearVersion();
    mAllowUnknownChipType = false;
}

/**************************************************************************************************************
 * Looks through a mapping structure to convert a version value read from the chip into a chip_type enumeration  
 **************************************************************************************************************/
void CXapHelper::SetChipType
(
    uint16 chip_version,   // the masked version read from the chip
    uint16 invalid_value,  // the value that indicates the end of the array
    const VersionMappingStruct *versionStruct   // the mapping structure
)
{
    // Ignore chip versions of 0
    if (chip_version == 0)
    {
        return;
    }
    // Look through the mapping array to set the chip type
    bool found = false;
    for (unsigned int i = 0; ((versionStruct[i].chip_version != invalid_value) && (!found)); i++)
    {
        if (versionStruct[i].chip_version == chip_version)
        {
            found = true;
            mChipType = versionStruct[i].chip_type;
        }
    }

    if (!found)
    {
        // Unknown chip type
        mChipType = CHIP_TYPE_UNKNOWN;
        assert(mAllowUnknownChipType);
    }
}

/**************************************************************************************************************
 * Save value read from FF9A
 **************************************************************************************************************/
void CXapHelper::SetFF9A(uint16 ff9a)
{
    mFF9A = ff9a;
    SetChipType(DIGITAL_CHIP_VERSION(mFF9A), RAW_DIGITAL_CHIP_VERSION_INVALID, gDigitalMapping);
}

/**************************************************************************************************************
 * Save value read from FE81
 **************************************************************************************************************/
void CXapHelper::SetFE81(uint16 fe81)
{
    // If non-zero FF9a then fe81 must be 0
    if ((mFF9A != 0xffff) && (mFF9A != 0))
    {
        assert(mAllowUnknownChipType || (fe81 == 0));
    }
    mFE81 = fe81;
    SetChipType(CHIP_VERSION_FE81(mFE81), RAW_CHIP_VERSION_INVALID, gVersionMapping);
}

/**************************************************************************************************************
 * On pre BC7 chip FF9A returns non zero
 **************************************************************************************************************/
bool CXapHelper::IsPreBc7() const
{
    assert(IsReadyFor(true));
    return (DIGITAL_CHIP_VERSION(mFF9A) != 0);
}

/**************************************************************************************************************
 * Indicates if BC7+
 **************************************************************************************************************/
bool CXapHelper::IsBc7OrLater() const
{
    return (!IsPreBc7());
}

/**************************************************************************************************************
 * Returns the name to display to the user
 **************************************************************************************************************/
const char *CXapHelper::GetDisplayName() const
{
    const char *ChipDisplayName = "Unknown device"; 

    assert(IsReadyForUse());
    
    bool found = false;
    for (unsigned int i = 0; ((gDisplayNameMapping[i].chip_version != CHIP_TYPE_UNKNOWN) && (!found)); i++)
    {
        if (gDisplayNameMapping[i].chip_version == mChipType)
        {
            found = true;
            ChipDisplayName = gDisplayNameMapping[i].name_to_display;
        }
    }

    // Unknown chip type
    assert(mAllowUnknownChipType || found);
    
    return ChipDisplayName;
}

/**************************************************************************************************************
 * Certain chips require a Xap restart after reset
 **************************************************************************************************************/
bool CXapHelper::RequiresStartAfterReset() const
{
    assert(IsReadyForUse());
    return (mChipType >= CHIP_TYPE_CIN);
}

/**************************************************************************************************************
 * Some chips require an extra check on chip start because they can stop again quickly. B-81267, B-93433
 **************************************************************************************************************/
bool CXapHelper::RequiresPcCheckOnStart() const
{
    assert(IsReadyForUse());
    return IsBc7OrLater();
}

/**************************************************************************************************************
 * Some chips require the regulator enable registers to be latched before and after reset or they will power down.
 **************************************************************************************************************/
bool CXapHelper::RequiresRegulatorEnablesOnReset() const
{
    assert (IsReadyForUse());
    return mChipType == CHIP_TYPE_GDN;
}

/**************************************************************************************************************
 * Get RAM base
 **************************************************************************************************************/
uint16 CXapHelper::GetRamBase() const
{
    uint16 ram_base;

    uint16 digital_chip_version = DIGITAL_CHIP_VERSION(mFF9A);
    if (digital_chip_version > 0)
    {
        if (digital_chip_version >= RAW_DIGITAL_CHIP_VERSION_HRB)
        {
            ram_base = 0x9000;
        }
        else
        {
            ram_base = 0xa000;
        }
    }
    else
    {
        // BC7 plus
        ram_base = 0x8000;
    }
    return ram_base;
}


/**************************************************************************************************************
 * Get program base
 **************************************************************************************************************/
uint32 CXapHelper::GetProgBase() const
{
    return (mChipType < CHIP_TYPE_ELV) ? 0x140000 : 0x540000;
}

/**************************************************************************************************************
 * Maps RAM addresses specified in data space to program space addresses.
 **************************************************************************************************************/
uint32 CXapHelper::MapRamToProg(uint16 aDataAddr) const
{
    assert(IsReadyForUse());
    
    uint16 rambase = GetRamBase();
    uint32 progbase = GetProgBase();

    return progbase + aDataAddr - rambase;
}

/**************************************************************************************************************
 * Maps program space addresses to RAM addresses specified in data space.
 **************************************************************************************************************/
uint16 CXapHelper::MapProgToRam(uint32 aProgAddr) const
{
    assert(IsReadyForUse());

    uint16 rambase = GetRamBase();
    uint32 progbase = GetProgBase();

    return (uint16)((aProgAddr - progbase) + rambase);
}

/**************************************************************************************************************
 * Returns the chip type identified
 **************************************************************************************************************/
ChipTypeEnum CXapHelper::GetChipType() const 
{
    assert(IsReadyForUse());
    return mChipType;
}

/**************************************************************************************************************
 * Used to "assert" that when the class is used, it has been setup correctly for a given function.
 **************************************************************************************************************/
bool CXapHelper::IsReadyFor(bool bc7_query_only) const
{
#   define CHIP_TYPE_KNOWN (mChipType != CHIP_TYPE_UNKNOWN)
#   define FF9A_KNOWN (mFF9A != RAW_UNKNOWN_CHIP_VALUE)
#   define FE81_KNOWN (mFE81 != RAW_UNKNOWN_CHIP_VALUE)
#   define FE81_NEEDED (DIGITAL_CHIP_VERSION(mFF9A) == 0)

    bool ret = false;

    if (mAllowUnknownChipType)
    {
        ret = true;
    }
    else if(bc7_query_only)
    {
        // either we're totally ready or we're half way through identifying a BC7+
        ret = IsReadyForUse()
            || (FF9A_KNOWN && FE81_NEEDED && !FE81_KNOWN && !CHIP_TYPE_KNOWN);
    }
    else
    {
        // must be something we recognise, and we need to know FF9A,
        //   and if FF9A is zero we need to know FE81
        ret = CHIP_TYPE_KNOWN && FF9A_KNOWN && (FE81_KNOWN || !FE81_NEEDED);
    }
    return ret;
}

/**************************************************************************************************************
 * Used to "assert" that when the class is used, it has been setup correctly. Can also be used 
 * to check externally that the class is ready.
 **************************************************************************************************************/
bool CXapHelper::IsReadyForUse() const
{
    return IsReadyFor(false);
}

/**************************************************************************************************************
 * Reset the class to initial values.
 **************************************************************************************************************/
void CXapHelper::ClearVersion()
{
    mFF9A = RAW_UNKNOWN_CHIP_VALUE;
    mFE81 = RAW_UNKNOWN_CHIP_VALUE;
    mChipType = CHIP_TYPE_UNKNOWN;
}

/**************************************************************************************************************
 * Is the chip bluecore or unifi or something else.
 **************************************************************************************************************/
ChipFamilyEnum CXapHelper::GetChipFamily(void) const
{
    ChipFamilyEnum family = CHIP_FAMILY_UNKNOWN;

    assert(IsReadyForUse());

    switch(mChipType)
    {
    case CHIP_TYPE_BFT:
    case CHIP_TYPE_DSH:
    case CHIP_TYPE_CIN:
    case CHIP_TYPE_ANA:
        family = CHIP_FAMILY_UNIFI;
        break;
    case CHIP_TYPE_UNKNOWN:
        // already set to 'unknown' family

        assert(mAllowUnknownChipType); 
        break;
    default: // Default for BlueCore since there more of these
        family = CHIP_FAMILY_BLUECORE;
        break;
    }
    return family;
}

/**************************************************************************************************************
 * If this chip can have a Loader return true.
 **************************************************************************************************************/
bool CXapHelper::CanHaveLoader() const
{
    bool canHaveLoader = false;

    assert(IsReadyForUse());

    switch(GetChipFamily())
    {
    case CHIP_FAMILY_BLUECORE:
        if(IsPreBc7() || CHIP_TYPE_GDN == mChipType)
        {
            canHaveLoader = true;
        }
        break;
    case CHIP_FAMILY_UNIFI:
        canHaveLoader = true;
        break;
    default:
        break;
    }

    return canHaveLoader;
}

/**************************************************************************************************************
 * Checks to see if the processor select value is valid for this chip type
 **************************************************************************************************************/
bool CXapHelper::IsValidProcSelect(uint16 aProcSelect) const
{
    bool is_valid = false;
    switch (GetChipFamily())
    {
        case CHIP_FAMILY_BLUECORE:
        {
            is_valid = (aProcSelect == 2);
            break;
        }
        case CHIP_FAMILY_UNIFI:
        {
            is_valid = (aProcSelect <= 3);
            break;
        }
        default:
        {
            break;
        }
    }
    return is_valid;
}

/**************************************************************************************************************
 * Formats the values of the two ID registers in to one 32 bit value
 **************************************************************************************************************/
uint32 CXapHelper::GetChipId() const
{
    uint32 ret = 0;

    ret = GetFF9A();
    if (ret == 0)
    {
        ret = GetFE81();
        ret <<= 16;
    }

    return ret;
}

