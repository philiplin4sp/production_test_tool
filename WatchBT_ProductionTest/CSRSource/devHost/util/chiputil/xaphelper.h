/****************************************************************************
 **
 **    XapHelper.h  -  Common Xap definitions
 ** 
 **    NOTE: Included in XAP embedded code. Code needs to be compatible with XAP C compiler
 ****************************************************************************/

#ifndef XAPHELPER_H
#define XAPHELPER_H


#define RAW_INVALID_VALUE   (-1) /* This needs to be a valid int16 value for XAP compiler */

#ifndef __XAP__
/* These are required by WIN32 but break bootprog builds (XAP compiler) */
#include <assert.h>
#include "common/types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Macros to extract version information bits
#define DIGITAL_CHIP_VERSION(reg_ff9a) (((reg_ff9a) >> 8) & 0x1F)
#define CHIP_VERSION_FE81(reg_fe81) ((reg_fe81) & 0xFF)

// Value to indicate uninitialised
#define RAW_UNKNOWN_CHIP_VALUE 0xffff

typedef enum
{
    RAW_DIGITAL_CHIP_VERSION_KAL        = 0x05,
    RAW_DIGITAL_CHIP_VERSION_NNK        = 0x06,
    RAW_DIGITAL_CHIP_VERSION_PDW        = 0x07,
    RAW_DIGITAL_CHIP_VERSION_CYT        = 0x08,
    RAW_DIGITAL_CHIP_VERSION_ODJ        = 0x09,
    RAW_DIGITAL_CHIP_VERSION_PUG        = 0x0C,
    RAW_DIGITAL_CHIP_VERSION_SLY        = 0x0D,
    RAW_DIGITAL_CHIP_VERSION_ZBD        = 0x0E,
    RAW_DIGITAL_CHIP_VERSION_JJK        = 0x0F,
    RAW_DIGITAL_CHIP_VERSION_ELV        = 0x10,
    RAW_DIGITAL_CHIP_VERSION_JPL        = 0x11,
    RAW_DIGITAL_CHIP_VERSION_PHP        = 0x12,
    RAW_DIGITAL_CHIP_VERSION_HRB        = 0x13,
    RAW_DIGITAL_CHIP_VERSION_FRG        = 0x14,
    RAW_DIGITAL_CHIP_VERSION_SGL        = 0x15,
    RAW_DIGITAL_CHIP_VERSION_MTK        = 0x16,
    RAW_DIGITAL_CHIP_VERSION_AKR        = 0x17,
    RAW_DIGITAL_CHIP_VERSION_GBO        = 0x18,
    RAW_DIGITAL_CHIP_VERSION_LAST       = RAW_DIGITAL_CHIP_VERSION_GBO,

    RAW_DIGITAL_CHIP_VERSION_INVALID    = RAW_INVALID_VALUE
} DigitalChipVersionEnum;

typedef enum
{
    RAW_CHIP_VERSION_BFT    = 0x01,
    RAW_CHIP_VERSION_DSH    = 0x20,
    RAW_CHIP_VERSION_JEM    = 0x21,
    RAW_CHIP_VERSION_CIN    = 0x22,
    RAW_CHIP_VERSION_ANA    = 0x23,
    RAW_CHIP_VERSION_LPC    = 0x24,
    RAW_CHIP_VERSION_OXY    = 0x27,
    RAW_CHIP_VERSION_GDN    = 0x28,
    RAW_CHIP_VERSION_GEM    = 0x29,
    RAW_CHIP_VERSION_NUT    = 0x2A,
    RAW_CHIP_VERSION_ROB    = 0x2B,
    RAW_CHIP_VERSION_LEO    = 0x2D,
    RAW_CHIP_VERSION_PUR    = 0x30,
    RAW_CHIP_VERSION_VGA    = 0x31,
    RAW_CHIP_VERSION_DAL    = 0x32,

    RAW_CHIP_VERSION_INVALID = RAW_INVALID_VALUE
} ChipVersionEnum;

typedef enum
{
    CHIP_FAMILY_UNKNOWN,
    CHIP_FAMILY_BLUECORE,
    CHIP_FAMILY_UNIFI
} ChipFamilyEnum;

typedef enum
{
    // DigitalChipVersionEnum chips
    CHIP_TYPE_KAL = RAW_DIGITAL_CHIP_VERSION_KAL,
    CHIP_TYPE_NNK = RAW_DIGITAL_CHIP_VERSION_NNK,
    CHIP_TYPE_PDW = RAW_DIGITAL_CHIP_VERSION_PDW,
    CHIP_TYPE_CYT = RAW_DIGITAL_CHIP_VERSION_CYT,
    CHIP_TYPE_ODJ = RAW_DIGITAL_CHIP_VERSION_ODJ,
    CHIP_TYPE_PUG = RAW_DIGITAL_CHIP_VERSION_PUG,
    CHIP_TYPE_SLY = RAW_DIGITAL_CHIP_VERSION_SLY,
    CHIP_TYPE_ZBD = RAW_DIGITAL_CHIP_VERSION_ZBD,
    CHIP_TYPE_JJK = RAW_DIGITAL_CHIP_VERSION_JJK,
    CHIP_TYPE_ELV = RAW_DIGITAL_CHIP_VERSION_ELV,
    CHIP_TYPE_JPL = RAW_DIGITAL_CHIP_VERSION_JPL,
    CHIP_TYPE_PHP = RAW_DIGITAL_CHIP_VERSION_PHP,
    CHIP_TYPE_HRB = RAW_DIGITAL_CHIP_VERSION_HRB,
    CHIP_TYPE_FRG = RAW_DIGITAL_CHIP_VERSION_FRG,
    CHIP_TYPE_SGL = RAW_DIGITAL_CHIP_VERSION_SGL,
    CHIP_TYPE_MTK = RAW_DIGITAL_CHIP_VERSION_MTK,
    CHIP_TYPE_AKR = RAW_DIGITAL_CHIP_VERSION_AKR,
    CHIP_TYPE_GBO = RAW_DIGITAL_CHIP_VERSION_GBO,
   
     // ChipVersionEnum chips
    CHIP_TYPE_BFT = RAW_DIGITAL_CHIP_VERSION_LAST + RAW_CHIP_VERSION_BFT,
    CHIP_TYPE_DSH = RAW_DIGITAL_CHIP_VERSION_LAST + RAW_CHIP_VERSION_DSH,
    CHIP_TYPE_JEM = RAW_DIGITAL_CHIP_VERSION_LAST + RAW_CHIP_VERSION_JEM,
    CHIP_TYPE_CIN = RAW_DIGITAL_CHIP_VERSION_LAST + RAW_CHIP_VERSION_CIN,
    CHIP_TYPE_ANA = RAW_DIGITAL_CHIP_VERSION_LAST + RAW_CHIP_VERSION_ANA,
    CHIP_TYPE_LPC = RAW_DIGITAL_CHIP_VERSION_LAST + RAW_CHIP_VERSION_LPC,
    CHIP_TYPE_OXY = RAW_DIGITAL_CHIP_VERSION_LAST + RAW_CHIP_VERSION_OXY,
    CHIP_TYPE_GDN = RAW_DIGITAL_CHIP_VERSION_LAST + RAW_CHIP_VERSION_GDN,
    CHIP_TYPE_GEM = RAW_DIGITAL_CHIP_VERSION_LAST + RAW_CHIP_VERSION_GEM,
    CHIP_TYPE_NUT = RAW_DIGITAL_CHIP_VERSION_LAST + RAW_CHIP_VERSION_NUT,
    CHIP_TYPE_ROB = RAW_DIGITAL_CHIP_VERSION_LAST + RAW_CHIP_VERSION_ROB,
    CHIP_TYPE_LEO = RAW_DIGITAL_CHIP_VERSION_LAST + RAW_CHIP_VERSION_LEO,
    CHIP_TYPE_PUR = RAW_DIGITAL_CHIP_VERSION_LAST + RAW_CHIP_VERSION_PUR,
    CHIP_TYPE_VGA = RAW_DIGITAL_CHIP_VERSION_LAST + RAW_CHIP_VERSION_VGA,
    CHIP_TYPE_DAL = RAW_DIGITAL_CHIP_VERSION_LAST + RAW_CHIP_VERSION_DAL,

    // =================================================================================
    // Entries added here must also be reflected in the lookup table in the source file
    // =================================================================================
    CHIP_TYPE_UNKNOWN
}
ChipTypeEnum;


typedef struct
{
    uint16              chip_version;
    ChipTypeEnum        chip_type;
}
VersionMappingStruct;

typedef struct
{
    uint16      chip_version;
    const char  *name_to_display;
}
DisplayNameMappingStruct;

#ifdef __cplusplus
/* Close the extern "C" */
}

// ============================================================================
// If the class is passed the values read from FF9A and FE81 it identifies
// the type of chip (GetChipType) and returns attributes and functionality
// required for the current chip
// ============================================================================
class CXapHelper
{
public:
    CXapHelper(); 
    CXapHelper(uint16 ff9a, uint16 fe81); 

    void SetFF9A(uint16 ff9a);
    void SetFE81(uint16 fe81);
    uint16 GetFF9A() const {assert(IsReadyForUse()); return mFF9A;}
    uint16 GetFE81() const {assert(IsReadyForUse()); return mFE81;}
    uint32 GetChipId() const;

    void ClearVersion();

    ChipTypeEnum GetChipType() const;
    const char *GetDisplayName() const;     // Returns a string to display to the user to identify the chip
    ChipFamilyEnum GetChipFamily(void) const;

    bool IsReadyForUse() const;             // Indicates if class has been initialised
    void AllowUnknownChipType() {mAllowUnknownChipType = true;}      // Prevents an assert on an unknown chip type

    // ========================================================================
    // Generation information
    // ========================================================================
    bool IsBc7OrLater() const;
    bool IsPreBc7() const;

    // ========================================================================
    // Conditional functionality
    // ========================================================================
    bool RequiresStartAfterReset() const;
    bool RequiresPcCheckOnStart() const;
    bool RequiresRegulatorEnablesOnReset() const;

    // ========================================================================
    // Chip Properties
    // ========================================================================
    uint16 MapProgToRam(uint32 aProgAddr) const;  // Maps RAM addresses specified in data space to program space addresses.   
    uint32 MapRamToProg(uint16 aDataAddr) const;  // Maps program space addresses to RAM addresses specified in data space.

    bool CanHaveLoader() const;

    bool IsValidProcSelect(uint16 aProcSelect) const;
    
private:
    void Init();
    void SetChipType(uint16 chip_version, uint16 invalid_value, const VersionMappingStruct *versionStruct);
    uint16 GetRamBase() const;
    uint32 GetProgBase() const;

    uint16 mFF9A;    // chip version read from 0xff9a
    uint16 mFE81;    // chip version read from 0xfe81. n/a for pre-BC7
    
    bool   mAllowUnknownChipType;  // if true, will not assert on unknown chips

    ChipTypeEnum mChipType;

    bool IsReadyFor(bool) const;
};

#endif // __cplusplus

#endif
