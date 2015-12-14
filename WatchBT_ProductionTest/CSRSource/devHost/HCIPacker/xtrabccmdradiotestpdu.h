////////////////////////////////////////////////////////////////////////////////
//
//  FILE:       xtrabccmdradiotestpdu.h
//
//  CLASS:      None
//
//  PURPOSE:    This file is included in CVS for completeness.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __XTRABCCMDRADIOTESTPDU_H__
#define __XTRABCCMDRADIOTESTPDU_H__

#include "hcipacker/gen_bccmd.h"

class RADIOTEST_CFG_HOPPING_SEQUENCE_PDU : public BCCMD_RADIOTEST_PDU
{
public:
    RADIOTEST_CFG_HOPPING_SEQUENCE_PDU() ;
    RADIOTEST_CFG_HOPPING_SEQUENCE_PDU( const PDU& );
    RADIOTEST_CFG_HOPPING_SEQUENCE_PDU( const uint8 * buffer , uint32 len ) ;
    void set_channels ( const uint16 channels[5] );
    void get_channels ( uint16 channels[5] ) const;
};

class RADIOTEST_CFG_PIO_CHANNEL_MASK_PDU : public BCCMD_RADIOTEST_PDU
{
public:
    RADIOTEST_CFG_PIO_CHANNEL_MASK_PDU() ;
    RADIOTEST_CFG_PIO_CHANNEL_MASK_PDU( const PDU& );
    RADIOTEST_CFG_PIO_CHANNEL_MASK_PDU( const uint8 * buffer , uint32 len ) ;
    void set_channels ( const uint16 channels[5] );
    void get_channels ( uint16 channels[5] ) const;
};

class RADIOTEST_PCM_TONE_STEREO_PDU : public BCCMD_RADIOTEST_PDU
{
public:
    RADIOTEST_PCM_TONE_STEREO_PDU() ;
    RADIOTEST_PCM_TONE_STEREO_PDU( const PDU& );
    RADIOTEST_PCM_TONE_STEREO_PDU( const uint8 * buffer , uint32 len ) ;
    void set_params ( const uint16 params[4] );
    void get_params ( uint16 params[4] ) const;
};

class RADIOTEST_PCM_TONE_INTERFACE_PDU : public BCCMD_RADIOTEST_PDU
{
public:
    RADIOTEST_PCM_TONE_INTERFACE_PDU() ;
    RADIOTEST_PCM_TONE_INTERFACE_PDU( const PDU& );
    RADIOTEST_PCM_TONE_INTERFACE_PDU( const uint8 * buffer , uint32 len ) ;
    void set_params ( const uint16 params[4] );
    void get_params ( uint16 params[4] ) const;
};
#endif /* __XTRABCCMDRADIOTESTPDU_H__ */
