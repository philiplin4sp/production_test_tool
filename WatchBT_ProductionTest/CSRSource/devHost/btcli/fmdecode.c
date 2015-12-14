/*
 * Copyright (C) 2000-2006 Cambridge Silicon Radio Ltd.; all rights reserved.
 * Decoding of FM/RDS BCCMD/HQ message data
 *
 * MODIFICATION HISTORY
 */

/* This file supports BOTH the receive and transmit parts of the FM-API */

#include "fmdecode.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#if !defined(_WINCE) && !defined(_WIN32_WCE)
#include <stddef.h>
#include <assert.h>
#define ASSERT assert
#else
#include "wince/wincefuncs.h"
#endif

#include "btcli.h"
#include "print.h"
#include "dictionaries.h"
#include "assertn.h"
#include "host/bccmd/bccmdpdu.h"
#include "host/fm/fm_api.h"
#include "globals.h"

void printLeftRightStatus(u32 content, u32 mask_l, u32 mask_r, u32 mask_if)
{
    u32 mask;

    mask = content & (mask_l | mask_r);

    if (mask == (mask_l | mask_r))
        printlf("L+R");
    if (mask == mask_l)
        printlf("L");
    if (mask == mask_r)
        printlf("R");
    if ((content & mask_if) != 0)
    {
        if (mask != 0)
            printlf("+");
        printlf("interface");
    }
    else if (mask == 0)
        printlf("no");
    printlf(" mute");
}

/* Decode and print FM force mute status */
void decodeFMForceMute(u32 content)
{
    printlf(": pcm1: ");
    printLeftRightStatus(content,
                         FM_API_MISC_FORCE_MUTE_PCM1_LEFT_MASK,
                         FM_API_MISC_FORCE_MUTE_PCM1_RIGHT_MASK,
                         FM_API_MISC_FORCE_MUTE_PCM1_INTERF_MASK);
    printlf(" pcm2: ");
    printLeftRightStatus(content,
                         FM_API_MISC_FORCE_MUTE_PCM2_LEFT_MASK,
                         FM_API_MISC_FORCE_MUTE_PCM2_RIGHT_MASK,
                         FM_API_MISC_FORCE_MUTE_PCM2_INTERF_MASK);
    printlf(" codec: ");
    printLeftRightStatus(content,
                         FM_API_MISC_FORCE_MUTE_CODEC_LEFT_MASK,
                         FM_API_MISC_FORCE_MUTE_CODEC_RIGHT_MASK,
                         0);
}

/* Decode and print the contents of the audio routing register. */
void decodeFMRegAudioRouting(u32 content)
{
    printlf(": ara:");
    printByValue(miscFmAudioRoutingModeAra_d, (content & 0x3), 2, "");
    printlf(" arb:");
    printByValue(miscFmAudioRoutingModeArb_d, (content & 0xc) >> 2, 2, "");
    printlf(" arc:");
    printByValue(miscFmAudioRoutingModeArc_d, (content & 0x10) >> 4, 1, "");
    printlf(" arh:0x%1x", ((unsigned) content & 0x20) >> 5);
    printlf(" vsa:");
    printByValue(miscFmAudioRoutingModeVsa_d, (content & 0xc0) >> 6, 2, "");
    printlf(" vsb:");
    printByValue(miscFmAudioRoutingModeVsb_d, (content & 0x300) >> 8, 2, "");
    printlf(" vsc:");
    printByValue(miscFmAudioRoutingModeVsc_d, (content & 0xc00) >> 10, 2, "");
}

/* Decode and print the contents of the MISC register. */
void decodeFMRegMisc(u32 content)
{
    u8 page = (uint8) FM_API_MISC_MODE_UNPACK(content);
    /* XXX: if read_page is set to >=0x10, this will be unable to decode
     * a read value, and will in fact mis-decode it. However, if you're
     * using HCI, there's no need to do this. */
    printlf("misc ");
    printlf("mode:");
    printByValue(miscRegisterMode_d, page, 4, NULL);
    switch (page)
    {
    case FM_API_MISC_MODE_AUDIO_ROUTING:
        decodeFMRegAudioRouting(content);
	break;
    case FM_API_MISC_MODE_FM_CONTROL:
	if (content & 0x01)
	    printlf(": auto HILO");
	else
	    printlf(": use HLM");
	/* XXX duplicated in decodeFMReg() */
	if (content & 0x02)
	    printlf(": discard E");
	else
	    printlf(": store E");
        if (content & 0x04)
            printlf(": TR disabled");
        else
            printlf(": TR enabled");
        break;
    case FM_API_MISC_MODE_RDS_CONTROL:
	/* XXX duplicated in decodeFMReg() */
	content & 0x01 ? printlf(": flush") : printlf(": do nothing");
	if (content & 0x02)
	    printlf(": force re-sync");
	break;
    case FM_API_MISC_MODE_AUDIO_GAIN:
	printlf(": gain_pcm1: %u gain_pcm2: %u gain_codec: %u ",
                ((unsigned) content & 0x7), (((unsigned) content >> 3) & 0x7), (((unsigned) content >> 6) & 0x1f));
        break;
    case FM_API_MISC_MODE_FORCE_MUTE:
        decodeFMForceMute(content);
	break;
    case FM_API_MISC_MODE_READ_PAGE:
        printlf(": read_page: ");
        printByValue(miscRegisterMode_d, content & 0xFFFu, 12, NULL);
        break;
    }
}

static void decodePower(uint32 content)
{
    // Probably worth creating a dictionary for this.
	printlf("power");
	if((content & 0x03) == 0)
		printlf(": all off");
	if(content & 0x01)
		printlf(": FM");
	if(content & 0x02)
		printlf(": RDS");
}

static void decodeFirmVer(uint32 content)
{
    printlf("firm ver 0x%x", content);
}

static void decodeAsicVer(uint32 content)
{
    printlf("ASIC ver 0x%x", content);
}

static void decodeBitField(Dictionary dict, uint32 content)
{
    int index = 0;
    const char *str0;
    if (content == 0 && (str0 = lookupByValue(dict, content)) != NULL)
    {
        printlf(" : %s", str0);
    }
    else
    {
        while (dict[index].name != NULL)
        {
            if (dict[index].value & content)
                printlf(": %s ", dict[index].name);
            ++index;
        }
    }
}

/* Decode and print the contents of an FM register. Return TRUE if the register ID was
recognised, return FALSE (and print no interpretation) if it was not.
*/
bool decodeFMReg (u32 regID, u32 content)
{
	int index = 0;

	/* Always print the register value first. This enables the FM GUI to parse for this value.
	*/
	printlf("\n0x%x ", (unsigned) content);

	switch (regID)
	{
        /* XXX this switch() should use the symbols defined in fm_api.h */
	case 0x00: /* Flag */
		printlf("flag");
		if (content != 0)
		{
			while (fm_flag_vals[index].value != 0)
			{
				if (fm_flag_vals[index].value & content)
					printlf(" : %s", fm_flag_vals[index].name);
				index++;
			}
		}
		break;

	case 0x02: /* int_mask */
		printlf("int mask");
		if (content != 0)
		{
			while (fm_int_mask_vals[index].value != 0)
			{
				if (fm_int_mask_vals[index].value & content)
					printlf(" : %s", fm_int_mask_vals[index].name);
				index++;
			}
		}
		break;

	case 0x04: /*  status */
		printlf("status \n");
		switch (content & 0x07)
		{
		case 0:
			printlf("Radio not powered or has been reset");
			break;

		case 1:
			printlf("Not tuned to a valid frequency");
			break;

		case 2:
			printlf("Tuning in progress");
			break;

		case 3:
			printlf("Tuned to valid frequency, RDS off");
			break;

		case 4:
			printlf("Tuned to valid frequency, no RDS present");
			break;

		case 5:
			printlf("Tuned to valid frequency, RDS not synced");
			break;
					
		case 6:
			printlf("Tuned to valid frequency, RDS being received");
			break;

		default:
			break;
		}
		if (content & 0x08)
			printlf(": RDA");
		if (content & 0x10)
			printlf(": STR");
		if (content & 0x20)
			printlf(": IFR");
		if (content & 0x40)
			printlf(": RSR");
		break;

	case 0x05: /* rssi_lvl */
		printlf("RSSI %ddBm", (signed char)content);
		break;

	case 0x06: /* if_count */
		printlf("IF %dkHz", (int16)content);
		break;

	case 0x08: /* rssi_limit */
		printlf("RSSI_limit %ddBm", (signed char)content);
		break;

	case 0x09: /* if_limit */
		printlf("IF_limit %ukHz", (unsigned) content);
		break;

	case 0x0a: /* search_lvl */
		printlf("search level %ddBm", (signed char)content);
		break;

	case 0x0b: /* search_dir */
		printlf("search dir: ");
		if(content == 0)
			printlf("increasing");
		else if (content == 1)
			printlf("decreasing");
		else
			printlf("??");
		break;

	case 0x0c: /* freq */
		printlf("freq %.1fMHz", (content + 60000.0)/1000.0);
		break;

	case 0x0e: /* af_freq */
		printlf("AF freq %.1fMHz", (content + 60000.0)/1000.0);
		break;

	case 0x10: /* tuner_mode */
		printlf("tuner mode: ");
		switch (content & 0x03)
		{
		case 0:
			printlf("none in progress");
			break;

		case 1:
			printlf("preset mode");
			break;

		case 2:
			printlf("search mode");
			break;

		case 3:
			printlf("af jump mode");
			break;
		}
		if(content & 0x10)
			printlf(": use FDD");
		else
			printlf(": use SEARCH_DIR");
		if(content & 0x20)
			printlf(": FDD down");
		else
			printlf(": FDD up");
		if(content & 0x40)
			printlf(": SKP");
		if(content & 0x80)
			printlf(": TX-ANT");
		break;

	case 0x11: /* mute_state */
		printlf("mute state ");
		if(content == 0)
			printlf(": no mute");
		if(content & 0x01)
			printlf(": MSS");
		if(content & 0x02)
			printlf(": MSA");
		if(content & 0x04)
			printlf(": MSL");
		if(content & 0x08)
			printlf(": MSR");
		break;

	case 0x12: /* most_mode */
		printlf("most mode: ");
		!(content & 0x01) ? printlf("mono") : printlf("stereo");
		break;
	
	case 0x13: /* most_blend */
		printlf("most blend: ");
		!(content & 0x01) ? printlf("soft") : printlf("switched");
		break;

	case 0x14: /* pause_lvl */
		printlf("pause lvl %luHz", 32 * (ul) content);
		break;

	case 0x15: /* pause_dur */
		printlf("pause dur %ums", (unsigned) content);
		break;
	
	case 0x16: /* misc */
		decodeFMRegMisc(content);
		break;

	case 0x18: /* man_id */
		printlf("man ID 0x%x", (unsigned) content);
		break;

	case 0x1a: /* asic_id */
		printlf("ASIC ID 0x%x", (unsigned) content);
		break;

	case 0x1c: /* firm_ver */
        decodeFirmVer(content);
		break;

	case 0x1e: /* asic_ver */
        decodeAsicVer(content);
		break;

	case 0x1f: /* band */
		printlf("band: ");
		switch(content)
		{
		    case 0:	printlf("Europe/US"); break;
		    case 1:	printlf("Japan"); break;
		    default:	printlf("??"); break;
		}
		break;

	case 0x20: /* hilo_get */
		printlf("hilo : HLM: FLO = FREQ");
		!(content & 0x01) ? printlf("-FIF") : printlf("+FIF");
		printlf(", AHL: ");
		(content & 0x02) ? printlf("auto HILO") : printlf("use HLM");
		break;

	case 0x21: /* demph_mode */
		printlf("demph mode ");
		!(content & 0x01) ? printlf("50us") : printlf("75us");
		break;

	case 0x22: /* rds_system */
		printlf("RDS system");
		if(content & 0x01)
			printlf(": RBDS");
		else
			printlf(": RDS");
		/* XXX duplicated in decodeFMRegMisc() */
		if(content & 0x02)
			printlf(": discard E");
		else
			printlf(": store E");
		break;
	
	case 0x23: /* rds_mem */
		printlf("RDS mem %u blocks", (unsigned) content);
		break;

	case 0x24: /* power */
        decodePower((uint8) content);
		break;

	case 0x25: /* rds_cntr */
		/* XXX duplicated in decodeFMRegMisc() */
		printlf("RDS cntr: ");
		content & 0x01 ? printlf("flush") : printlf("do nothing");
		if (content & 0x02)
			printlf(": force re-sync");
		break;

	case 0x26: /* rds_blk_b */
		printlf("B block 0x%x", (unsigned) content);
		break;

	case 0x28: /* rds_msk_b */
		printlf("B block mask 0x%x", (unsigned) content);
		break;

	case 0x2a: /* rds_pi */
		printlf("PI 0x%x", (unsigned) content);
		break;

	case 0x2c: /* rds_pi_mask */
		printlf("PI mask 0x%x", (unsigned) content);
		break;

	case 0x30: /* fm_audio_routing */
		printlf("fm_audio_routing");
                decodeFMRegAudioRouting(content);
		break;

	case 0x32: /* fm_audio_gain */
		printlf("fm_audio_gain: gain_pcm1: %u gain_pcm2: %u gain_codec_l: %u gain_codec_r: %u",
                        ((unsigned) content & 0x7), (((unsigned) content >> 3) & 0x7),
                        (((unsigned) content >> 6) & 0x1f), (((unsigned) content >> 11) & 0x1f));
		break;

        case 0x34: /* fm_force_mute */
                decodeFMForceMute(content);
                break;

        case 0x36: /* pcm_intermediate_state */
		printlf("PCM1: %u, PCM2: %u", (unsigned) content & 0x0f, ((unsigned) content >> 4) & 0x0f);
                break;


        case 0x38: /* fm_params */
		printlf("fm_params: FLO = FREQ");
		!(content & 0x01) ? printlf("-FIF") : printlf("+FIF");
                if (content & 0x02)
                    printlf(": tone rejection in use");
                break;

        case 0x3a: /* snr */
                printlf("SNR %.2fdB", content/4.0);
                break;
        case 0x3c: /* rssi_end */
            printlf("RSSI end freq %.1fMHz", (content + 60000.0)/1000.0);
            break;
            
	case 0x3e: /*  audio_io */
		printlf("Audio outputs: ");
        
		if (content & 0x01)
		{
			printlf("Analogue ENABLED ");
		}
        else
		{
			printlf("Analogue DISABLED ");
		}
		if (content & 0x02)
		{
			printlf("Digital ENABLED ");
		}
        else
		{
			printlf("Digital DISABLED ");
		}
		break;
    case FM_API_SUBADDR_DEMOD_IF:
        printlf("Demod IF %.1f Hz ",((float)(content * 2000 /1.024)-2000000.0)); 
		break;
        
	default:
		return false;
		break;
	}

	return true;
}


/* Decode and print an RDS data block starting at the given index. 
   Note that the output is parsed by the FM/RDS GUI so the 
   format is significant.*/

void decodeRDSBlock (u32 val, u32 stat)
{
	printlf("(");
	switch(stat & 0x07)
	{
	case 0:
		printlf("A_");
		break;

	case 1:
		printlf("B_");
		break;

	case 2:
		printlf("C_");
		break;

	case 3:
		printlf("C'");
		break;

	case 4:
		printlf("D_");
		break;

	case 5:
		printlf("E_");
		break;

	case 6:
		printlf("XE");
		break;

	case 7:
		printlf("IV");
		break;
	}
	switch(stat & 0x18)
	{
	case 0:
		printlf("0");
		break;

	case 0x08:
		printlf("2");
		break;

	case 0x10:
		printlf("5");
		break;

	case 0x18:
		printlf("*");
	break;
	}
	if(stat & 0x20)
		printlf("-)");
	else
		printlf("|)");
	printlf(",0x%02x,0x%04x ", (unsigned) stat, (unsigned) val);
}

/* FM TX */

/* Decode and print the contents of an FM register. Return TRUE if the register ID was
recognised, return FALSE (and print no interpretation) if it was not.
*/
bool decodeFMTXReg (u32 regID, u32 content)
{
    int index = 0;

    /* Always print the register value first. This enables the FM GUI to parse for this value.
    */
    printlf("\n0x%x ", (unsigned) content);

    switch (regID)
    {
    case FMTX_API_SUBADDR_POWER: 
        printlf("flag");
        printlf("power");
        if((content & 0x03) == 0)
            printlf(": all off");
        if(content & 0x01)
            printlf(": FMTX");
        if(content & 0x02)
            printlf(": RDS");
        break;
    case FMTX_API_SUBADDR_PWRLVL:
        printlf("TX PwrAmp");
        printlf("%d dBm", ((int16)(content & 0xFFFF)));   

        break;
    case FMTX_API_SUBADDR_FREQ:
        printlf("freq %.1fMHz", (content + 60000.0)/1000.0);
        break;
    case FMTX_API_SUBADDR_AUDIO_DEV:
        printlf("Audio dev. %.1fKHz (approximately)", (content & 0xFF) * 750.0 / 1000.0);
        break;
    case FMTX_API_SUBADDR_PILOT_DEV:
        printlf("Pilot dev. %.1fKHz", (content & 0xFF) * 64.0 /1000.0);    
        break;
    case FMTX_API_SUBADDR_FORCE_MONO:
        printlf("Audio");
        if((content & 0x01) == 0)
        {
            printlf(": Stereo");
        }            
        else 
        {
             printlf(": Forced Mono");   
        }    
        break;
    case FMTX_API_SUBADDR_PREEMP:
         printlf("Pre-emp:");
        if((content & 0x01) == 0)
        {
            printlf(": Disabled");
        }            
        else if ((content & 0x02) == 0)
        {
             printlf(": Enabled 50uS");   
        }      
        else
        {
             printlf(": Enabled 75uS");   
        }              
        break;
    case FMTX_API_SUBADDR_MUTE_STATE:
        printlf("Audio");
        if((content & 0x01) == 0)
        {
            printlf(": Unmuted");
        }            
        else 
        {
             printlf(": Muted");   
        }    
        break;
    case FMTX_API_SUBADDR_MPX_LMT:
        printlf("Multiplex limiter");
        if((content & 0x01) == 0)
        {
            printlf(": Enabled");
        }            
        else 
        {
             printlf(": Disabled");   
        }        
        break;
    case FMTX_API_SUBADDR_PI:
        printlf("RDS PI Code 0x%X", (unsigned) content & 0xFFFF);
        break;
    case FMTX_API_SUBADDR_PTY:
        printlf("RDS PTY Code %d", (unsigned) content & 0x3F);
        break;
    case FMTX_API_SUBADDR_AF_FREQ:
        printlf("RDS AF %.1fMHz", (content + 60000.0)/1000.0);
        break;
    case FMTX_API_SUBADDR_DISP_MODE:
        printlf("PSN Display ");   
        switch (content & 0x03)
        {
        case 0:
            printlf("Static");
            break;
        case 1:
            printlf("Scroll");
            break; 
        case 2:
            printlf("Paged");
            break; 
		}
        break;
    case FMTX_API_SUBADDR_DISP_UPD:
        printlf("Display update %ld", (ul) content + 1);
        break;
    case FMTX_API_SUBADDR_RDS_DEV:
        printlf("RDS dev. %.1fKHz", (content & 0xFF) * 66.0 /1000.0);    
        break;
    case FMTX_API_SUBADDR_DRC_MODE:
         printlf("DRC:");
        if((content & 0x01) == 0)
        {
            printlf(": Disabled");
        }            
        else if ((content & 0x02) == 0)
        {
             printlf(": Enabled ");   
        }      
        else
        {
             printlf(": Enabled Soft-Knee");   
        }        
        break;
    case FMTX_API_SUBADDR_ANA_TEST:
        printlf("Audio test: ");
        if ((content & 0x01) == 0)
        {
             printlf(": Not Requested");   
        }      
        else
        {
             printlf(": Requested"); 
			if ((content & 0x02) == 0)
			{
				 printlf(": not complete");   
			}      
			else
			{
				 printlf(": and completed"); 

			}  
        }  
        break;
    case FMTX_API_SUBADDR_ANA_LVL:
            printlf("Audio Levels %d %d", ((unsigned) content & 0xFF00) >> 8, (unsigned) content & 0x00FF);
        break;
    case FMTX_API_SUBADDR_TUNER_MODE:
         printlf("Tuner Mode: ");
        switch (content & FM_API_TUNER_MODE_MASK)
        {
        case FM_API_TUNER_MODE_NONE:
            printlf("Idle");
            break;
        case FM_API_TUNER_MODE_TUNE:
            printlf("Tuning");
            break; 
        case FM_API_TUNER_MODE_CAL:
            printlf("Calibrating");
            break; 
		}
        break;      
    case FMTX_API_SUBADDR_STATUS:
		printlf("Status: ");
        switch (content & 0x03)
        {
        case FM_API_STATUS_TX_RESET:
            printlf("Reset");
            break;
        case FM_API_STATUS_UNTUNED:
            printlf("Untuned");
            break; 
        case FM_API_STATUS_TUNING:
            printlf("Tuning");
            break; 
        case FM_API_STATUS_TUNED:
            printlf("Tuned");
            break; 
		}
        break;
    case FMTX_API_SUBADDR_ANT_CHK:
        printlf("Antenna Check:");
        if((content & 0x01) == 0)
        {
            printlf(": Pending");
        }            
        else if ((content & 0x02) == 0)
        {
             printlf(": Completed FAILED");   
        }      
        else
        {
             printlf(": Completed PASSED");   
        }              
        break;
    case FMTX_API_SUBADDR_AUDIO_RANGE:
        printlf("Audio Range: %d ", (unsigned) content & 0x1F);            
        break;
    case FMTX_API_SUBADDR_REF_ERR:
        printlf("REF. Error : %.2fKHz ", ((unsigned) content & 0xFFFF) / 16.0);
        break;
    case FMTX_API_SUBADDR_AUDIO_IO:
         printlf("Audio input: ");
        if((content & 0x01) == 0)
        {
            printlf("Analogue");
        }            
        else 
        {
             printlf("Digital");   
        }            
        break;
    case FMTX_API_SUBADDR_DRC_CONFIG:
        printlf("DRC Config: ");
		switch(content & 0x3){
		case 0:
			printlf("Ratio: 2"); 
			 break;
		case 1:
			printlf("Ratio: 8/3"); 
			 break;
		case 2:
			printlf("Ratio: 4"); 
			 break;
		default:
			printlf("Ratio: undefined"); 
        }            
        printlf(" Attack: %2.1f ms",((float)((content & 0x3C)>>2)+1)/2.0); 
        printlf(" Release: %d ms",((((unsigned) content & 0x3C0)>>6)+1)*100);  
        break;
	case FMTX_API_SUBADDR_FLAG:
        printlf("flag");
        if (content != 0)
        {
            while (fmtx_flag_vals[index].value != 0)
            {
                if (fmtx_flag_vals[index].value & content)
                    printlf(" : %s", fmtx_flag_vals[index].name);
                index++;
            }
        }
        break;

	case FMTX_API_SUBADDR_INT_MASK:
        printlf("int mask");
        if (content != 0)
        {
            while (fmtx_int_mask_vals[index].value != 0)
            {
                if (fmtx_int_mask_vals[index].value & content)
                    printlf(" : %s", fmtx_int_mask_vals[index].name);
                index++;
            }
        }
        break;
    case FMTX_API_SUBADDR_AUD_THRES_LOW:
         printlf("Low audio threshold: ");
         printlf(" %ld%%  ", (((ul) content & 0x1FFF)*100)/0x1FFF);    
        break;
    case FMTX_API_SUBADDR_AUD_THRES_HIGH:
         printlf("High audio threshold: ");
         printlf(" %ld%%  ", (((ul) content & 0x1FFF)*100)/0x1FFF);              
        break;
    case FMTX_API_SUBADDR_AUD_DUR_LOW:
		if(content == 0x00)
        {
			printlf("Low audio duration: (Disabled) "); 
        }            
        else 
        {
			printlf("Low audio duration: (Enabled) %.1fs ", (content & 0xFF) * 0.1); 
        }            
        break;
    case FMTX_API_SUBADDR_AUD_DUR_HIGH:
		if(content == 0x00)
        {
			printlf("High audio duration: (Disabled) "); 
        }            
        else 
        {
			printlf("High audio duration: (Enabled) %.1fs ", (content & 0xFF) * 0.1); 
        }      
         break;
    case FMTX_API_SUBADDR_AUD_LVL_L:
        printlf("Audio Level Left %ld %% (0x%x) ",(((ul) content & 0x1FFF)*100)/(0x1fff), (unsigned) content & 0x1FFF);
		break;
    case FMTX_API_SUBADDR_AUD_LVL_R:
        printlf("Audio Level Right %ld %% (0x%x)",(((ul) content & 0x1FFF)*100)/(0x1fff), (unsigned) content & 0x1FFF);
        break;
    case FMTX_API_SUBADDR_DRC_CMP_THRESH:
        printlf("DRC cmp thresh (0x%x) ", (unsigned) content & 0x0FFF); 
		break;
    case FMTX_API_SUBADDR_DRC_CMP_BOOST:
        printlf("DRC cmp boost (0x%x) ", (unsigned) content & 0x07FF); 
		break;
    case FMTX_API_SUBADDR_DRC_CMP_LIMIT:
        printlf("DRC cmp limit (0x%x) ", (unsigned) content & 0x0FFF); 
		break;
    case FMTX_API_SUBADDR_RF_MUTE:
		printlf("Transmitter RF:");
        if((content & 0x01) == 0)
        {
            printlf(" Unmuted");
        }            
        else 
        {
             printlf(" Muted");   
        }        
        break;
		break;
	default:
        return false;
        break;
    }

    return true;
}


void decodeRDSTEXT(const uint32* payload)
{
	printlf("Received FMTX Text Payload:");
	printlf("%x %x %x %x", (unsigned) payload[0], (unsigned) payload[1], (unsigned) payload[2], (unsigned) payload[3]);
}

