////////////////////////////////////////////////////////////////////////////////
//
//  FILE:       xtrabccmdradiotestpdu.cpp
//
//  CLASS:      None
//
//  PURPOSE:    This file is included in CVS for completeness.
//
////////////////////////////////////////////////////////////////////////////////

#include "bcflayout.h"

//  class RADIOTEST_CFG_HOPPING_SEQUENCE_PDU : public BCCMD_RADIOTEST_PDU
enum {
	OFFSET_START(RADIOTEST_CFG_HOPPING_SEQUENCE)
	OFFSET_uint16(RADIOTEST_CFG_HOPPING_SEQUENCE_req_type),
	OFFSET_uint16(RADIOTEST_CFG_HOPPING_SEQUENCE_wordlength),
	OFFSET_uint16(RADIOTEST_CFG_HOPPING_SEQUENCE_seq_no),
	OFFSET_uint16(RADIOTEST_CFG_HOPPING_SEQUENCE_varid),
	OFFSET_uint16(RADIOTEST_CFG_HOPPING_SEQUENCE_status),
	OFFSET_uint16(RADIOTEST_CFG_HOPPING_SEQUENCE_radiotest_field),
	OFFSET_uint16(RADIOTEST_CFG_HOPPING_SEQUENCE_channel0),
	OFFSET_uint16(RADIOTEST_CFG_HOPPING_SEQUENCE_channel1),
	OFFSET_uint16(RADIOTEST_CFG_HOPPING_SEQUENCE_channel2),
	OFFSET_uint16(RADIOTEST_CFG_HOPPING_SEQUENCE_channel3),
	OFFSET_uint16(RADIOTEST_CFG_HOPPING_SEQUENCE_channel4),
	RADIOTEST_CFG_HOPPING_SEQUENCE_pduSize
} ;

//  class RADIOTEST_CFG_PIO_CHANNEL_MASK_PDU : public BCCMD_RADIOTEST_PDU
enum {
	OFFSET_START(RADIOTEST_CFG_PIO_CHANNEL_MASK)
	OFFSET_uint16(RADIOTEST_CFG_PIO_CHANNEL_MASK_req_type),
	OFFSET_uint16(RADIOTEST_CFG_PIO_CHANNEL_MASK_wordlength),
	OFFSET_uint16(RADIOTEST_CFG_PIO_CHANNEL_MASK_seq_no),
	OFFSET_uint16(RADIOTEST_CFG_PIO_CHANNEL_MASK_varid),
	OFFSET_uint16(RADIOTEST_CFG_PIO_CHANNEL_MASK_status),
	OFFSET_uint16(RADIOTEST_CFG_PIO_CHANNEL_MASK_radiotest_field),
	OFFSET_uint16(RADIOTEST_CFG_PIO_CHANNEL_MASK_channel0),
	OFFSET_uint16(RADIOTEST_CFG_PIO_CHANNEL_MASK_channel1),
	OFFSET_uint16(RADIOTEST_CFG_PIO_CHANNEL_MASK_channel2),
	OFFSET_uint16(RADIOTEST_CFG_PIO_CHANNEL_MASK_channel3),
	OFFSET_uint16(RADIOTEST_CFG_PIO_CHANNEL_MASK_channel4),
	RADIOTEST_CFG_PIO_CHANNEL_MASK_pduSize
} ;

//  class RADIOTEST_PCM_TONE_STEREO_PDU : public BCCMD_RADIOTEST_PDU
enum {
	OFFSET_START(RADIOTEST_PCM_TONE_STEREO)
	OFFSET_uint16(RADIOTEST_PCM_TONE_STEREO_req_type),
	OFFSET_uint16(RADIOTEST_PCM_TONE_STEREO_wordlength),
	OFFSET_uint16(RADIOTEST_PCM_TONE_STEREO_seq_no),
	OFFSET_uint16(RADIOTEST_PCM_TONE_STEREO_varid),
	OFFSET_uint16(RADIOTEST_PCM_TONE_STEREO_status),
	OFFSET_uint16(RADIOTEST_PCM_TONE_STEREO_radiotest_field),
	OFFSET_uint16(RADIOTEST_PCM_TONE_STEREO_params0),
	OFFSET_uint16(RADIOTEST_PCM_TONE_STEREO_params1),
	OFFSET_uint16(RADIOTEST_PCM_TONE_STEREO_params2),
	OFFSET_uint16(RADIOTEST_PCM_TONE_STEREO_params3),
	RADIOTEST_PCM_TONE_STEREO_pduSize
} ;

//  class RADIOTEST_PCM_TONE_INTERFACE_PDU : public BCCMD_RADIOTEST_PDU
enum {
	OFFSET_START(RADIOTEST_PCM_TONE_INTERFACE)
	OFFSET_uint16(RADIOTEST_PCM_TONE_INTERFACE_req_type),
	OFFSET_uint16(RADIOTEST_PCM_TONE_INTERFACE_wordlength),
	OFFSET_uint16(RADIOTEST_PCM_TONE_INTERFACE_seq_no),
	OFFSET_uint16(RADIOTEST_PCM_TONE_INTERFACE_varid),
	OFFSET_uint16(RADIOTEST_PCM_TONE_INTERFACE_status),
	OFFSET_uint16(RADIOTEST_PCM_TONE_INTERFACE_radiotest_field),
	OFFSET_uint16(RADIOTEST_PCM_TONE_INTERFACE_params0),
	OFFSET_uint16(RADIOTEST_PCM_TONE_INTERFACE_params1),
	OFFSET_uint16(RADIOTEST_PCM_TONE_INTERFACE_params2),
	OFFSET_uint16(RADIOTEST_PCM_TONE_INTERFACE_params3),
	RADIOTEST_PCM_TONE_INTERFACE_pduSize
} ;
#include "xtrabccmdradiotestpdu.h"

RADIOTEST_CFG_HOPPING_SEQUENCE_PDU::RADIOTEST_CFG_HOPPING_SEQUENCE_PDU()
: BCCMD_RADIOTEST_PDU ()
{
    set_radiotest ( RADIOTEST_CFG_HOPPING_SEQUENCE );
}

RADIOTEST_CFG_HOPPING_SEQUENCE_PDU::RADIOTEST_CFG_HOPPING_SEQUENCE_PDU( const PDU& a )
: BCCMD_RADIOTEST_PDU ( a ) {}

RADIOTEST_CFG_HOPPING_SEQUENCE_PDU::RADIOTEST_CFG_HOPPING_SEQUENCE_PDU( const uint8 * buffer , uint32 len )
: BCCMD_RADIOTEST_PDU ( buffer , len ) {}

void RADIOTEST_CFG_HOPPING_SEQUENCE_PDU::set_channels ( const uint16 channels[5] )
{
    if ( size() < RADIOTEST_CFG_HOPPING_SEQUENCE_pduSize )
    {
        resize ( RADIOTEST_CFG_HOPPING_SEQUENCE_pduSize );
        set_length();
    }
    set_uint16 ( RADIOTEST_CFG_HOPPING_SEQUENCE_channel0 , channels[0] );
    set_uint16 ( RADIOTEST_CFG_HOPPING_SEQUENCE_channel1 , channels[1] );
    set_uint16 ( RADIOTEST_CFG_HOPPING_SEQUENCE_channel2 , channels[2] );
    set_uint16 ( RADIOTEST_CFG_HOPPING_SEQUENCE_channel3 , channels[3] );
    set_uint16 ( RADIOTEST_CFG_HOPPING_SEQUENCE_channel4 , channels[4] );
}

void RADIOTEST_CFG_HOPPING_SEQUENCE_PDU::get_channels ( uint16 channels[5] ) const
{
    channels[0] = get_uint16 ( RADIOTEST_CFG_HOPPING_SEQUENCE_channel0 );
    channels[1] = get_uint16 ( RADIOTEST_CFG_HOPPING_SEQUENCE_channel1 );
    channels[2] = get_uint16 ( RADIOTEST_CFG_HOPPING_SEQUENCE_channel2 );
    channels[3] = get_uint16 ( RADIOTEST_CFG_HOPPING_SEQUENCE_channel3 );
    channels[4] = get_uint16 ( RADIOTEST_CFG_HOPPING_SEQUENCE_channel4 );
}


///////////////////////////////////////////////////////////////////////////////


RADIOTEST_CFG_PIO_CHANNEL_MASK_PDU::RADIOTEST_CFG_PIO_CHANNEL_MASK_PDU()
: BCCMD_RADIOTEST_PDU ()
{
    set_radiotest ( RADIOTEST_CFG_PIO_CHANNEL_MASK );
}

RADIOTEST_CFG_PIO_CHANNEL_MASK_PDU::RADIOTEST_CFG_PIO_CHANNEL_MASK_PDU( const PDU& a )
: BCCMD_RADIOTEST_PDU ( a ) {}

RADIOTEST_CFG_PIO_CHANNEL_MASK_PDU::RADIOTEST_CFG_PIO_CHANNEL_MASK_PDU( const uint8 * buffer , uint32 len )
: BCCMD_RADIOTEST_PDU ( buffer , len ) {}

void RADIOTEST_CFG_PIO_CHANNEL_MASK_PDU::set_channels ( const uint16 channels[5] )
{
    if ( size() < RADIOTEST_CFG_PIO_CHANNEL_MASK_pduSize )
    {
        resize ( RADIOTEST_CFG_PIO_CHANNEL_MASK_pduSize );
        set_length();
    }
    set_uint16 ( RADIOTEST_CFG_PIO_CHANNEL_MASK_channel0 , channels[0] );
    set_uint16 ( RADIOTEST_CFG_PIO_CHANNEL_MASK_channel1 , channels[1] );
    set_uint16 ( RADIOTEST_CFG_PIO_CHANNEL_MASK_channel2 , channels[2] );
    set_uint16 ( RADIOTEST_CFG_PIO_CHANNEL_MASK_channel3 , channels[3] );
    set_uint16 ( RADIOTEST_CFG_PIO_CHANNEL_MASK_channel4 , channels[4] );
}

void RADIOTEST_CFG_PIO_CHANNEL_MASK_PDU::get_channels ( uint16 channels[5] ) const
{
    channels[0] = get_uint16 ( RADIOTEST_CFG_PIO_CHANNEL_MASK_channel0 );
    channels[1] = get_uint16 ( RADIOTEST_CFG_PIO_CHANNEL_MASK_channel1 );
    channels[2] = get_uint16 ( RADIOTEST_CFG_PIO_CHANNEL_MASK_channel2 );
    channels[3] = get_uint16 ( RADIOTEST_CFG_PIO_CHANNEL_MASK_channel3 );
    channels[4] = get_uint16 ( RADIOTEST_CFG_PIO_CHANNEL_MASK_channel4 );
}


///////////////////////////////////////////////////////////////////////////////


RADIOTEST_PCM_TONE_STEREO_PDU::RADIOTEST_PCM_TONE_STEREO_PDU()
: BCCMD_RADIOTEST_PDU ()
{
    set_radiotest ( RADIOTEST_PCM_TONE_STEREO );
}

RADIOTEST_PCM_TONE_STEREO_PDU::RADIOTEST_PCM_TONE_STEREO_PDU( const PDU& a )
: BCCMD_RADIOTEST_PDU ( a ) {}

RADIOTEST_PCM_TONE_STEREO_PDU::RADIOTEST_PCM_TONE_STEREO_PDU( const uint8 * buffer , uint32 len )
: BCCMD_RADIOTEST_PDU ( buffer , len ) {}

void RADIOTEST_PCM_TONE_STEREO_PDU::set_params ( const uint16 params[4] )
{
    if ( size() < RADIOTEST_PCM_TONE_STEREO_pduSize )
    {
        resize ( RADIOTEST_PCM_TONE_STEREO_pduSize );
        set_length();
    }
    set_uint16 ( RADIOTEST_PCM_TONE_STEREO_params0 , params[0] );
    set_uint16 ( RADIOTEST_PCM_TONE_STEREO_params1 , params[1] );
    set_uint16 ( RADIOTEST_PCM_TONE_STEREO_params2 , params[2] );
    set_uint16 ( RADIOTEST_PCM_TONE_STEREO_params3 , params[3] );
}

void RADIOTEST_PCM_TONE_STEREO_PDU::get_params ( uint16 params[4] ) const
{
    params[0] = get_uint16 ( RADIOTEST_PCM_TONE_STEREO_params0 );
    params[1] = get_uint16 ( RADIOTEST_PCM_TONE_STEREO_params1 );
    params[2] = get_uint16 ( RADIOTEST_PCM_TONE_STEREO_params2 );
    params[3] = get_uint16 ( RADIOTEST_PCM_TONE_STEREO_params3 );
}

///////////////////////////////////////////////////////////////////////////////


RADIOTEST_PCM_TONE_INTERFACE_PDU::RADIOTEST_PCM_TONE_INTERFACE_PDU()
: BCCMD_RADIOTEST_PDU ()
{
    set_radiotest ( RADIOTEST_PCM_TONE_INTERFACE );
}

RADIOTEST_PCM_TONE_INTERFACE_PDU::RADIOTEST_PCM_TONE_INTERFACE_PDU( const PDU& a )
: BCCMD_RADIOTEST_PDU ( a ) {}

RADIOTEST_PCM_TONE_INTERFACE_PDU::RADIOTEST_PCM_TONE_INTERFACE_PDU( const uint8 * buffer , uint32 len )
: BCCMD_RADIOTEST_PDU ( buffer , len ) {}

void RADIOTEST_PCM_TONE_INTERFACE_PDU::set_params ( const uint16 params[4] )
{
    if ( size() < RADIOTEST_PCM_TONE_INTERFACE_pduSize )
    {
        resize ( RADIOTEST_PCM_TONE_INTERFACE_pduSize );
        set_length();
    }
    set_uint16 ( RADIOTEST_PCM_TONE_INTERFACE_params0 , params[0] );
    set_uint16 ( RADIOTEST_PCM_TONE_INTERFACE_params1 , params[1] );
    set_uint16 ( RADIOTEST_PCM_TONE_INTERFACE_params2 , params[2] );
    set_uint16 ( RADIOTEST_PCM_TONE_INTERFACE_params3 , params[3] );
}

void RADIOTEST_PCM_TONE_INTERFACE_PDU::get_params ( uint16 params[4] ) const
{
    params[0] = get_uint16 ( RADIOTEST_PCM_TONE_INTERFACE_params0 );
    params[1] = get_uint16 ( RADIOTEST_PCM_TONE_INTERFACE_params1 );
    params[2] = get_uint16 ( RADIOTEST_PCM_TONE_INTERFACE_params2 );
    params[3] = get_uint16 ( RADIOTEST_PCM_TONE_INTERFACE_params3 );
}

