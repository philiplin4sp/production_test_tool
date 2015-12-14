////////////////////////////////////////////////////////////////////////////////
//
//  FILE     : display_pdu.cpp
//
//  AUTHOR   : Adam Hughes
//
//  Copyright CSR 2003.  All rights reserved.
//
//  PURPOSE  : 1) to replace WinPrimDebug and cope with wire formate PDUs
//             2) to display the innards of a PDU in a useful (debug) way.
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#pragma warning (disable:4786)
#endif
#include "display_pdu.h"
#include "time/time_stamp.h"
#include "gen_display_pdu.h"
#include <map>
#include <string.h>

#ifndef USE_DISPLAY_PDU
std::string display_pdu ( const PDU& pdu  )
{ return ""; }
std::string display_pdu ( const HCIEventPDU & event )
{ return ""; }
std::string display_pdu ( const HCICommandPDU & event )
{ return ""; }
std::string display_pdu_no_time ( const PDU& pdu )
{ return ""; }
#else

#include <cstdio>


static std::string (*g_vmPduCallback) (int indent, const VM_PDU &pdu) = 0;

void SetVmPduCallback(std::string (*_vmPduCallback)(int indent, const VM_PDU &pdu))
{
	g_vmPduCallback = _vmPduCallback;
}


////////////////////////////////////////////////////////////////////////////////
//  file static functions
////////////////////////////////////////////////////////////////////////////////

static a_string body ( const PDU& pdu , PDU_displayer * policy );
static a_string display_hciCommand_pdu ( int indent , const PDU& , PDU_displayer * policy );
static a_string display_hciACL_pdu ( int indent , const PDU& , PDU_displayer * policy );
static a_string display_hciSCO_pdu ( int indent , const PDU& , PDU_displayer * policy );
static a_string display_debug_pdu ( int indent , const PDU& , PDU_displayer * policy );
static a_string display_vm_pdu ( int indent , const PDU& , PDU_displayer * policy );

////////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION : display_pdu
//
//  ARGUMENTS: const PDU& a pdu to be displayed.
//
//  RETURNS  : std::string displaying the content of the pdu or a message
//             explaining why there is a problem.  If policy class not,
//             provided the defult is to think it is rfcli.
//
//  PURPOSE  : 1) to replace WinPrimDebug and cope with wire format PDUs
//             2) to display the innards of a PDU in a useful (debug) way.
//
////////////////////////////////////////////////////////////////////////////////

std::string display_pdu_no_time ( const PDU& pdu )
{
    RfcliPDUDisplayer a;
    return display_pdu_no_time(pdu,&a);
}

std::string display_pdu ( const HCICommandPDU& pdu )
{
    RfcliPDUDisplayer a;
    return display_pdu(pdu,&a);
}

std::string display_pdu ( const HCIEventPDU& pdu )
{
    RfcliPDUDisplayer a;
    return display_pdu(pdu,&a);
}

std::string display_pdu ( const PDU& pdu, const TimeStamp &when )
{
    RfcliPDUDisplayer a;
    return display_pdu( pdu, when, &a );
}

std::string display_pdu ( const PDU& pdu )
{
    RfcliPDUDisplayer a;
    return display_pdu( pdu, TimeStamp(), &a );
}

std::string display_pdu ( const PDU& pdu , PDU_displayer * policy )
{
    return display_pdu( pdu, TimeStamp(), policy );
}

std::string display_pdu_no_time ( const PDU& pdu , PDU_displayer * policy )
{
	std::string rv("");
	
	a_string b = body(pdu,policy);
	if (!b.empty())
	{
		rv = (policy->prefix() + b + policy->postfix()).get_string();
	}
	
	return rv;
}

std::string display_pdu ( const PDU& pdu, const TimeStamp &when , PDU_displayer * policy )
{
	std::string rv("");
		
	a_string b = body(pdu,policy);
	if (!b.empty())
	{
		rv = (policy->prefix_time(when) + b + policy->postfix()).get_string();
	}
	
	return rv;
}

std::string display_pdu ( const HCICommandPDU& pdu , PDU_displayer * policy )
{
    // bad, but needed.
    if ( pdu.channel() == PDU::hciCommand )
        return (policy->prefix_time(TimeStamp()) + display_cmd_pdu ( 0 , pdu , policy ) + policy->postfix()).get_string();
    else
        return display_pdu ( PDU(pdu), policy );
}

std::string display_pdu ( const HCIEventPDU& pdu , PDU_displayer * policy )
{
    // bad, but needed.
    if ( pdu.channel() == PDU::hciCommand )
        return (policy->prefix_time(TimeStamp()) + display_evt_pdu ( 0 , pdu , policy ) + policy->postfix()).get_string();
    else
        return display_pdu ( PDU(pdu) , policy );
}

////////////////////////////////////////////////////////////////////////////////
//  big switch statement that does most of the work.
////////////////////////////////////////////////////////////////////////////////

a_string body ( const PDU& pdu , PDU_displayer * policy )
{
    a_string rString;
    switch ( pdu.channel() )
    {
    case PDU::bccmd:
        rString += display_bccmd_pdu ( 0 , pdu , policy );
        break;
    case PDU::hq:
        rString += display_hq_pdu ( 0 , pdu , policy );
        break;
    case PDU::dm:
        rString += display_dm_pdu ( 0 , pdu , policy );
        break;
    case PDU::hciCommand:
        rString += display_hciCommand_pdu ( 0 , pdu , policy );
        break;
    case PDU::hciACL:
        rString += display_hciACL_pdu ( 0 , pdu , policy );
        break;
    case PDU::hciSCO:
        rString += display_hciSCO_pdu ( 0 , pdu , policy );
        break;
    case PDU::l2cap:
        {
        uint16 type = L2CAP_PDU (pdu).get_type();
        if ( type >= L2CA_PRIM_BASE && type <= L2CA_PRIM_MAX)
            rString += display_l2cap_pdu ( 0 , pdu , policy );
        else if ( type < TCP_PRIM_BASE )
            rString += display_bnep_pdu ( 0 , pdu , policy );
        else if ( type < UDP_PRIM_BASE )
            rString += display_tcp_pdu ( 0 , pdu , policy );
        else
            rString += display_udp_pdu ( 0 , pdu , policy );
        }
        break;
    case PDU::rfcomm:
        rString += display_rfcomm_pdu ( 0 , pdu , policy );
        break;
    case PDU::sdp:
        {
            uint16 type = SDP_PDU( pdu ).get_type();
            if (type >= ATT_PRIM_BASE)
                rString += display_att_pdu( 0 , pdu , policy );
            else
                rString += display_sdp_pdu ( 0 , pdu , policy );
            break;
        }
    case PDU::debug:
        rString += display_debug_pdu ( 0 , pdu , policy );
        break;
    case PDU::vm:
        {
            a_string vmString = display_vm_pdu ( 0 , pdu , policy );
            //  if the vm pdu is supressed,
            //  (ie it is a part message and is being aggregated)
            //  then don't print it.
            if ( vmString.get_string().empty() )
                return "";
            else
                rString += vmString;
        }
        break;
    default:
        //  pdu is not on a recognised channel
        char ch[5];
        sprintf ( ch , "0x%02x" , pdu.channel() );
        rString += "Channel ";
        rString += ch;
        rString += " not recognised.  Cannot interpret PDU.\n";
        rString += policy->display_uint8Array ( 1 , "Content" , pdu , 0 , pdu.size() ).get_string();
    }
    return rString;
}

////////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION : display_hciCommand_pdu
//
//  ARGUMENTS: const PDU& a pdu to be displayed.
//
//  RETURNS  : std::string displaying the content of the pdu or a message
//             explaining why there is a problem.
//
////////////////////////////////////////////////////////////////////////////////

a_string display_hciCommand_pdu ( int indent , const PDU& pdu , PDU_displayer * policy )
{
    //  We need to guess what it is.
    HCIEventPDU evt (pdu);
    HCICommandPDU cmd (pdu);
    enum { neither = 0, command = 1, event = 2, either = 3};
    int guess = 0;
    if ( evt.get_parameter_length() + 2u == evt.size() )
        guess |= event;
    if ( cmd.get_parameter_length() + 3u == cmd.size() )
        guess |= command;

    a_string rv;
    switch ( guess )
    {
    case event:
        rv = display_evt_pdu ( indent , evt , policy );
        break;
    case command:
        rv = display_cmd_pdu ( indent , evt , policy );
        break;
    case either:
        rv = policy->display_text ( indent , "Uncertain HCI command/event.  Could be:\n" )
           + display_evt_pdu ( indent + 1 , evt , policy )
           + policy->display_text ( indent , "or :" )
           + display_cmd_pdu ( indent + 1 , cmd , policy );
        break;
    case neither:
        rv = policy->display_text ( indent , "Bad HCI packet:\n" )
           + policy->display_default ( indent, "Data" , pdu );
        break;
    default:
        rv = policy->display_text ( indent , "Internal problem.\n" )
           + policy->display_default  ( indent, "Data", pdu );
        break;
    }
    return rv;
}

////////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION : display_hciACL_pdu
//
//  ARGUMENTS: const PDU& a pdu to be displayed.
//
//  RETURNS  : std::string displaying the content of the pdu or a message
//             explaining why there is a problem.
//
////////////////////////////////////////////////////////////////////////////////

a_string display_hciACL_pdu ( int indent , const PDU& pdu , PDU_displayer * policy )
{
    HCIACLPDU a(pdu);
    uint8 * buffer = new uint8[a.get_length()];
    a.get_data ( buffer , a.get_length() );
    a_string rv = policy->display_prim_start ( 0 , "ACL" )
        + policy->display_uint16 ( 0 , "handle" , a.get_handle() )
        + policy->display_uint8 ( 0 , "packet_boundary" , uint8(a.get_pbf() >> 12) )
        + policy->display_uint8 ( 0 , "broadcast_type" , uint8(a.get_bct() >> 14) )
        + policy->display_uint16 ( 0 , "length" , a.get_length() )
        + policy->display_uint8Array ( 0 , "data" , buffer , a.get_length() )
        + policy->display_prim_end ( 0 , "ACL" );
    delete[] buffer;
    return rv;
}

////////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION : display_hciSCO_pdu
//
//  ARGUMENTS: const PDU& a pdu to be displayed.
//
//  RETURNS  : std::string displaying the content of the pdu or a message
//             explaining why there is a problem.
//
////////////////////////////////////////////////////////////////////////////////

a_string display_hciSCO_pdu ( int indent , const PDU& pdu , PDU_displayer * policy )
{
    HCISCOPDU s(pdu);
    uint8 buffer[256]; // size < 255
    s.get_data ( buffer , s.get_length() );
    return policy->display_prim_start ( 0 , "SCO" )
           + policy->display_uint16 ( 0 , "handle" , s.get_handle() )
           + policy->display_uint8 ( 0 , "length" , s.get_length() )
           + policy->display_uint8Array ( 0 , "data" , buffer , s.get_length() )
           + policy->display_prim_end ( 0 , "SCO" );
}

////////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION : display_debug_pdu
//
//  ARGUMENTS: const PDU& a pdu to be displayed.
//
//  RETURNS  : std::string displaying the content of the pdu or a message
//             explaining why there is a problem.
//
////////////////////////////////////////////////////////////////////////////////

a_string display_debug_pdu ( int indent , const PDU& pdu , PDU_displayer * policy )
{
    return a_string(pdu.size() ? (char*)pdu.data() : "Empty pdu.")+"\n";
}

////////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION : display_vm_pdu
//
//  ARGUMENTS: const PDU& a pdu to be displayed.
//
//  RETURNS  : std::string displaying the content of the pdu or a message
//             explaining why there is a problem.
//
////////////////////////////////////////////////////////////////////////////////

static std::map<uint8,std::string> vm_put_char;

a_string display_vm_pdu ( int indent , const PDU& pdu , PDU_displayer * policy )
{
    a_string default_rv = "VM channel pdu. ";
    uint16 len = pdu.size() / 2;
    default_rv += policy->display_uint16 ( 0 , "Length" , len ).get_string();
    if ( len )
    {
        VM_PDU v(pdu);
        default_rv += policy->display_uint16 ( indent , "Claimed Length" , v[0] ).get_string();
        if ( v[0] == len && v[0] > 1 )
        {
            uint16 type = v[1];
            if ( type >= 0x80 && type < 0x83 )
            {
                //  vm control channels
                if ( type == 0x80 )
                {
                    if ( len == 4 )
                    {
                        default_rv.erase();
                        switch ( v[3] )
                        {
                        case 0:
                        case '\n':
                            {
                            //  overwrite existing content
                            char label[40];
                            sprintf ( label , "VM%u: " , v[2] );
                            default_rv = policy->display_text( indent , ( label + vm_put_char[v[2]] + "\n" ).c_str() ).get_string();
                            vm_put_char[v[2]].erase();
                            }
                            break;
                        default:
                            vm_put_char[v[2]] += char ( v[3] );
                            break;
                        }
                    }
                }
                else
                {
                    //  can't deal yet.
                }

            }
            else if ( type >= 0x100 && type < 0x300 )
            {
				std::string callbackResult;
                if ( g_vmPduCallback )
				{
					callbackResult = g_vmPduCallback(indent, v);
				}
				
				int stringLength = callbackResult.length();
				if (stringLength == 0)
                {
					//  byte stream channels.
					uint32 byte_len = v.get_stream_size();
					//  overwrite existing content
					callbackResult = ( policy->display_text( indent , "VM byte stream\n" )
                                     + policy->display_uint8( indent + 1, "channel" , v.get_stream_id() )
                                     + policy->display_text( indent + 1, ":\n")
							         + policy->display_uint8Array ( indent + 1 , "data", pdu , 4 , byte_len ) ).get_string();
				}

				default_rv = callbackResult;
            }
            else
            {
				std::string callbackResult;
                if ( g_vmPduCallback )
				{
					callbackResult = g_vmPduCallback(indent, v);
				}

				int stringLength = callbackResult.length();
				if (stringLength == 0)
                {
                    //  Application specific packet, or unknown type.
                    uint16* data = new uint16[len];
                    v.get_packet(data);
                    callbackResult = policy->display_uint16Array ( 0 , "VM channel pdu", data , len ).get_string();
                    delete[] data;
                }

				default_rv = callbackResult;
            }
        }
    }
    return default_rv;
}

#define L2CA_RES_TABLE_X(a, b) case a: return display_uint16_symbolic(indent, name, a, #a)
#define L2CA_RES_TABLE_SEP ;

a_string PDU_displayer::display_l2ca_conn_result_t ( int indent , const char * name , uint16 value)
{
    switch (value)
    {
        L2CA_CONRES_TABLE;

        default:
            return display_uint16_symbolic(indent, name, value, "UNKNOWN_L2CAP_CONNECT_ERROR");
    }
}

a_string PDU_displayer::display_l2ca_move_result_t ( int indent , const char * name , uint16 value)
{
    switch (value)
    {
        L2CA_MOVERES_TABLE;

        default:
            return display_uint16_symbolic(indent, name, value, "UNKNOWN_L2CAP_MOVE_ERROR");
    }
}

a_string PDU_displayer::display_l2ca_disc_result_t ( int indent , const char * name , uint16 value)
{
    switch (value)
    {
        L2CA_DISCRES_TABLE;

        default:
            return display_uint16_symbolic(indent, name, value, "UNKNOWN_L2CAP_DISCONNECT_ERROR");
    }
}

a_string PDU_displayer::display_l2ca_conf_result_t ( int indent , const char * name , uint16 value)
{
    switch (value)
    {
        L2CA_CONFRES_TABLE;

        default:
            return display_uint16_symbolic(indent, name, value, "UNKNOWN_L2CAP_CONFIG_ERROR");
    }
}

a_string PDU_displayer::display_l2ca_info_result_t ( int indent , const char * name , uint16 value)
{
    switch (value)
    {
        L2CA_INFORES_TABLE;

        default:
            return display_uint16_symbolic(indent, name, value, "UNKNOWN_L2CAP_INFO_ERROR");
    }
}

a_string PDU_displayer::display_l2ca_data_result_t ( int indent , const char * name , uint16 value)
{
    switch (value)
    {
        L2CA_DATARES_TABLE;

        default:
            return display_uint16_symbolic(indent, name, value, "UNKNOWN_L2CAP_DATA_ERROR");
    }
}

a_string PDU_displayer::display_l2ca_misc_result_t ( int indent , const char * name , uint16 value)
{
    switch (value)
    {
        L2CA_MISCRES_TABLE;

        default:
            return display_uint16_symbolic(indent, name, value, "UNKNOWN_L2CAP_MISC_ERROR");
    }
}

#undef L2CA_RES_TABLE_X
#undef L2CA_RES_TABLE_SEP

#define RFC_RES_TABLE_X(a, b) case a: return display_uint16_symbolic(indent, name, a, #a)
#define RFC_RES_TABLE_SEP ;

a_string PDU_displayer::display_RFC_RESPONSE_T ( int indent , const char * name , uint16 value)
{
    switch (value)
    {
        RFC_RES_TABLE;

        default:
            return display_uint16_symbolic(indent, name, value, "UNKNOWN_RFCOMM_ERROR");
    }
}

#undef RFC_RES_TABLE_X
#undef RFC_RES_TABLE_SEP

#define SDC_RES_TABLE_X(a, b) case a: return display_uint16_symbolic(indent, name, a, #a)
#define SDC_RES_TABLE_SEP ;

a_string PDU_displayer::display_SDC_RESPONSE_T ( int indent , const char * name , uint16 value)
{
    switch (value)
    {
        SDC_RES_TABLE;

        default:
            return display_uint16_symbolic(indent, name, value, "UNKNOWN_SDC_ERROR");
    }
}

#undef SDC_RES_TABLE_X
#undef SDC_RES_TABLE_SEP

#define SDS_RES_TABLE_X(a, b) case a: return display_uint16_symbolic(indent, name, a, #a)
#define SDS_RES_TABLE_SEP ;

a_string PDU_displayer::display_SDS_RESPONSE_T ( int indent , const char * name , uint16 value)
{
    switch (value)
    {
        SDS_RES_TABLE;

        default:
            return display_uint16_symbolic(indent, name, value, "UNKNOWN_SDS_ERROR");
    }
}

#undef SDS_RES_TABLE_X
#undef SDS_RES_TABLE_SEP

/* Determine if value is within the bounds of the RFCOMM error space */
static bool is_rfcomm_error(uint16 value)
{
    return value == RFC_SUCCESS || (value >= RFCOMM_ERRORCODE_BASE && value < RFCOMM_ERRORCODE_BASE + BLUESTACK_ERRORCODE_BLOCK);
}

/* RFC_CONNECT_CFM can also display l2ca_conn_result errors */
a_string PDU_displayer::display_RFC_CONNECT_CFM_RESPONSE_T ( int indent , const char * name , uint16 value)
{
    if (is_rfcomm_error(value))
        return display_RFC_RESPONSE_T(indent, name, value);
   
    return display_l2ca_conn_result_t(indent, name, value);
}

/* RFC_DATAWRITE_CFM can also display l2ca_data_result errors */
a_string PDU_displayer::display_RFC_DATAWRITE_CFM_RESPONSE_T ( int indent , const char * name , uint16 value)
{
    if (is_rfcomm_error(value))
        return display_RFC_RESPONSE_T(indent, name, value);

    return display_l2ca_data_result_t(indent, name, value);
}

/* RFC_MOVE_L2CAP_CHANNEL_CFM can also display l2ca_move_result errors */
a_string PDU_displayer::display_RFC_MOVE_L2CAP_CHANNEL_RESPONSE_T ( int indent , const char * name , uint16 value)
{
    if (is_rfcomm_error(value))
        return display_RFC_RESPONSE_T(indent, name, value);

    return display_l2ca_move_result_t(indent, name, value);
}

a_string PDU_displayer::display_DM_SM_TRUST_T (int indent, const char * name, uint16 value)
{
    return display_uint16(indent, name, value);
}

a_string PDU_displayer::display_DM_SM_KEY_TYPE_T (int indent, const char * name, uint16 value)
{
    return display_uint16(indent, name, value);
}

a_string PDU_displayer::display_DM_SM_PERMANENT_ADDRESS_T (int indent, const char * name, uint16 value)
{
    return display_uint16(indent, name, value);
}

a_string PDU_displayer::display_CryptoBlob (uint8 length, const uint16 *array )
{
    // Maximum number of uint16s. Currently set for 128bit keys.
    const uint8 length_max = 8;

    if (length == 0 || length > length_max)
        return "";

    // Each uint16 has 4 hex digits. There will be "0x" at the start and "\0" at the end.
    char str[3 + length_max * 4];

    // This will have to be extended if length_max increases */
    char format[] = "0x%04x%04x%04x%04x%04x%04x%04x%04x";
    uint16 a[length_max];

    // Copy input into fixed size array, zeroing unused elements
    memset(a, 0, (length_max - length)*sizeof(uint16));
    memcpy(a + length_max - length, array, length*sizeof(uint16));

    // Truncate format string to correct length
    format[2 + length * 4] = 0;

    // Write string. We'll need more a[]s if length_max increases
    sprintf ( str , format , a[7], a[6], a[5], a[4], a[3], a[2], a[1], a[0] );

    return a_string(str);
}

////////////////////////////////////////////////////////////////////////////////
//  Rfcli style utility functions
////////////////////////////////////////////////////////////////////////////////

a_string RfcliPDUDisplayer::display_bool   ( int indent , const char * name , bool   value )
{
    return display_indent ( indent ) + name + " = " + ( value ? "TRUE" : "FALSE" ) + "\n";
}

a_string RfcliPDUDisplayer::display_uint8  ( int indent , const char * name , uint8  value )
{
    char str[16];
    sprintf ( str , "0x%02x (%u)" , value, value );
    return display_indent ( indent ) + name + " = " + str + "\n";
}

a_string RfcliPDUDisplayer::display_int8   ( int indent , const char * name , int8   value )
{
    char str[16];
    sprintf ( str , "0x%02x (%d)" , (value & 0xFF), value );
    return display_indent ( indent ) + name + " = " + str + "\n";
}

a_string RfcliPDUDisplayer::display_uint16 ( int indent , const char * name , uint16 value )
{
    char str[16];
    sprintf ( str , "0x%04x (%u)" , value, value );
    return display_indent ( indent ) + name + " = " + str + "\n";
}

a_string RfcliPDUDisplayer::display_int16  ( int indent , const char * name , int16  value )
{
    char str[16];
    sprintf ( str , "0x%04x (%d)" , (value & 0xFFFF), value );
    return display_indent ( indent ) + name + " = " + str + "\n";
}

a_string RfcliPDUDisplayer::display_uint24 ( int indent , const char * name , uint24 value )
{
    char str[32];
    sprintf ( str , "0x%06lx (%lu)" , value, value );
    return display_indent ( indent ) + name + " = " + str + "\n";
}

a_string RfcliPDUDisplayer::display_uint32 ( int indent , const char * name , uint32 value )
{
    char str[32];
    sprintf ( str , "0x%08lx (%lu)" , value, value );
    return display_indent ( indent ) + name + " = " + str + "\n";
}

a_string RfcliPDUDisplayer::display_int32 ( int indent , const char * name , int32 value )
{
    char str[32];
    sprintf ( str , "0x%08lx (%ld)" , (value & 0xFFFFFFFF), value );
    return display_indent ( indent ) + name + " = " + str + "\n";
}

a_string RfcliPDUDisplayer::display_uint8Array ( int indent , const char * name , const uint8* data , size_t size )
{
    a_string sIndent = display_indent ( indent + 1 );
    a_string rv = display_indent(indent) + name + " =\n";
    char str[16];
    const unsigned int wrap_width = 8;
    char printable[wrap_width+1];
    memset(printable,0,sizeof(printable));
    rv += sIndent;
    size_t i;
    for ( i = 0 ; i < size ; )
    {
        sprintf ( str , "0x%02x" , data[i] );
        rv += str;
        if ( ' ' <= data[i] && data[i] <= '~' )
            printable[i%wrap_width] = data[i];
        else
            printable[i%wrap_width] = '.';
        i++;
        if ( i < size )
        {
            // only delimit up to the penultimate datum.
            if ( i % wrap_width )
                rv += ", ";
            else
            {
                rv += ", ";
                rv += printable;
                rv += "\n" + sIndent;
                memset(printable,0,sizeof(printable));
            }
        }
    }
    rv += "  ";
    for ( i %= wrap_width ; i && i < wrap_width ; ++i )
    {
        rv += "      ";
    }
    rv += printable;
    rv += "\n";
    return rv;
}

a_string RfcliPDUDisplayer::display_uint8Array ( int indent , const char * name , const PDU& pdu , size_t startdex , size_t size )
{
    return display_uint8Array ( indent , name , pdu.data() + startdex , size );
}

a_string RfcliPDUDisplayer::display_uint16Array ( int indent , const char * name , const uint16* data , size_t count )
{
    a_string sIndent = display_indent ( indent + 1 );
    a_string rv = display_indent(indent) + name + " =\n";
    char str[16];
    const int wrap_width = 4;
    rv += sIndent;
    for ( size_t i = 0 ; i < count ; )
    {
        sprintf ( str , "0x%04x" , data[i] );
        rv += str;
        i++;
        if ( i < count )
        {
            // only delimit up to the penultimate datum.
            if ( i % wrap_width )
                rv += ", ";
            else
                rv += ",\n" + sIndent;
        }
    }
    rv += "\n";
    return rv;
}

a_string RfcliPDUDisplayer::display_uint24Array ( int indent , const char * name , const uint24* data , size_t count )
{
    a_string sIndent = display_indent ( indent + 1 );
    a_string rv = display_indent(indent) + name + " =\n";
    char str[16];
    const int wrap_width = 4;
    rv += sIndent;
    for ( size_t i = 0 ; i < count ; )
    {
        sprintf ( str , "0x%06lx" , data[i] );
        rv += str;
        i++;
        if ( i < count )
        {
            // only delimit up to the penultimate datum.
            if ( i % wrap_width )
                rv += ", ";
            else
                rv += ",\n" + sIndent;
        }
    }
    rv += "\n";
    return rv;
}

a_string RfcliPDUDisplayer::display_uint32Array ( int indent , const char * name , const uint32* data , size_t count )
{
    a_string sIndent = display_indent ( indent + 1 );
    a_string rv = display_indent(indent) + name + " =\n";
    char str[16];
    const int wrap_width = 4;
    rv += sIndent;
    for ( size_t i = 0 ; i < count ; )
    {
        sprintf ( str , "0x%08lx" , data[i] );
        rv += str;
        i++;
        if ( i < count )
        {
            // only delimit up to the penultimate datum.
            if ( i % wrap_width )
                rv += ", ";
            else
                rv += ",\n" + sIndent;
        }
    }
    rv += "\n";
    return rv;
}

a_string RfcliPDUDisplayer::display_BluetoothDeviceAddress ( int indent , const char * name , const BluetoothDeviceAddress& value )
{
    return display_indent ( indent ) + name + " =\n"
         + display_uint24 ( indent + 1 , "lap" , value.get_lap() )
         + display_uint8  ( indent + 1 , "uap" , value.get_uap() )
         + display_uint16 ( indent + 1 , "nap" , value.get_nap() );
}

a_string RfcliPDUDisplayer::display_BluetoothDeviceAddressWithType ( int indent , const char * name , const BluetoothDeviceAddressWithType& value )
{
     char str[16];
     sprintf ( str , "0x%02x (%u)" , value.get_type(), value.get_type() );

    return display_indent ( indent ) + name + " =\n"
        + display_indent  ( indent + 1) + "type = " + str + " (" + value.get_type_text() + ")\n"
        + display_uint24 ( indent + 1 , "lap" , value.get_addr().get_lap() )
        + display_uint8  ( indent + 1 , "uap" , value.get_addr().get_uap() )
        + display_uint16 ( indent + 1 , "nap" , value.get_addr().get_nap() );
}

a_string RfcliPDUDisplayer::display_EtherNetAddress ( int indent , const char * name , const EtherNetAddress& value )
{
    return display_uint16Array ( indent , name, value.get_data() , 3 );
}

a_string RfcliPDUDisplayer::display_IPAddress ( int indent , const char * name , const IPAddress& value )
{
    return display_uint16Array ( indent , name , value.get_data() , 8 );
}

a_string RfcliPDUDisplayer::display_HCIEventMask ( int indent , const char * name , const HCIEventMask& value )
{
    const uint8 * data = value.get_data();
    char str[32];
    sprintf ( str , "0x%02x%02x%02x%02x%02x%02x%02x%02x" , data[7] , data[6] , data[5] , data[4] , data[3] , data[2] , data[1] , data[0] );
    return display_indent(indent) + name + " = " + str + "\n";
}

a_string RfcliPDUDisplayer::display_ULPEventMask ( int indent , const char * name , const ULPEventMask& value )
{
    const uint8 * data = value.get_data();
    char str[32];
    sprintf ( str , "0x%02x%02x%02x%02x%02x%02x%02x%02x" , data[7] , data[6] , data[5] , data[4] , data[3] , data[2] , data[1] , data[0] );
    return display_indent(indent) + name + " = " + str + "\n";
}

a_string RfcliPDUDisplayer::display_DMHCIEventMask ( int indent , const char * name , const HCIEventMask& value )
{
    const uint8 * data = value.get_data();
    char str[32];
    sprintf ( str , "0x%02x%02x%02x%02x%02x%02x%02x%02x" , data[5] , data[4] , data[7] , data[6] , data[1] , data[0] , data[3] , data[2] );
    return display_indent(indent) + name + " = " + str + "\n";
}

a_string RfcliPDUDisplayer::display_CryptoBlob ( int indent , const char * name , uint8 length, const uint16 *array )
{
    return display_indent(indent) + name + " = "
            + PDU_displayer::display_CryptoBlob(length, array) + "\n";
}

a_string RfcliPDUDisplayer::display_InquiryResult ( int indent , const char * name , const InquiryResult& value )
{
    return display_indent(indent) + name + " =\n"
         + display_BluetoothDeviceAddress ( indent + 1 , "bd_addr" , value.get_bd_addr() )
         + display_uint8 ( indent + 1 , "Page_Scan_Repetition_Mode" , value.get_page_scan_repetition_mode() )
         + display_uint8 ( indent + 1 , "Page_Scan_Period_Mode" , value.get_page_scan_period_mode() )
         + display_uint8 ( indent + 1 , "Page_Scan_Mode(1.2:Rsvd)" , value.get_page_scan_mode() )
         + display_uint24 ( indent + 1 , "Class_of_Device" , value.get_class_of_device() )
         + display_uint16 ( indent + 1 , "Clock_Offset" , value.get_clock_offset() );
}

a_string RfcliPDUDisplayer::display_InquiryResultWithRSSI ( int indent , const char * name , const InquiryResultWithRSSI& value )
{
    return display_indent(indent) + name + " =\n"
         + display_BluetoothDeviceAddress ( indent + 1 , "bd_addr" , value.get_bd_addr() )
         + display_uint8 ( indent + 1 , "Page_Scan_Repetition_Mode" , value.get_page_scan_repetition_mode() )
         + display_uint8 ( indent + 1 , "Page_Scan_Period_Mode" , value.get_page_scan_period_mode() )
         + display_uint24 ( indent + 1 , "Class_of_Device" , value.get_class_of_device() )
         + display_uint16 ( indent + 1 , "Clock_Offset" , value.get_clock_offset() )
         + display_uint8 ( indent + 1 , "RSSI" , value.get_rssi() );
}

a_string RfcliPDUDisplayer::display_ncp_elementArray ( int indent , const char * name , const PDU_displayer::ncp_element*data , size_t count )
{
    a_string rv;
    a_string header = display_indent( indent ) + name + "=\n";
    for ( size_t i = 0 ; i < count ; ++i )
        rv += header
           +  display_uint16 ( indent + 1 , "handle" , data[i].handle )
           +  display_uint16 ( indent + 1 , "packets" , data[i].pkts );
    return rv;
}

a_string RfcliPDUDisplayer::display_link_key_bd_addrArray ( int indent , const char * name , const PDU_displayer::lb_element* data , size_t count )
{
    a_string rv;
    a_string header = display_indent( indent ) + name + " =\n";
    for ( size_t i = 0 ; i < count ; ++i )
    {
        const uint8 *link_key_data = data[i].key.get_data();
        if (link_key_data)
        {
            rv += header
               +  display_BluetoothDeviceAddress ( indent + 1 , "bd_addr" , data[i].bd_addr )        
               +  display_uint8Array ( indent + 1 ,"link_key",  link_key_data, 16 );
        }
        else
        {
            rv += "Link Key Missing\n";
        }
    }
    return rv;
}

a_string RfcliPDUDisplayer::display_lp_powerstate ( int indent , const char * name , const LP_POWERSTATE_T& value )
{
    a_string rv = display_indent(indent) + name + " =\n";
	rv += display_uint8(indent + 1 , "mode" , value.mode )
	   +  display_uint16(indent + 1, "min_interval" , value.min_interval )
	   +  display_uint16(indent + 1, "max_interval" , value.max_interval )
	   +  display_uint16(indent + 1, "attempt" , value.attempt )
	   +  display_uint16(indent + 1, "timeout" , value.timeout )
	   +  display_uint16(indent + 1, "duration" , value.duration );
	return rv;
}

a_string RfcliPDUDisplayer::display_indent ( int indent )
{
    a_string rv;
    if ( indent )
        rv.append ( indent * 4 , ' ' );
    return rv;
}

a_string RfcliPDUDisplayer::display_text   ( int indent , const char * text )
{
    return display_indent ( indent ) + text ;
}

a_string RfcliPDUDisplayer::display_prim_start   ( int indent , const char * text )
{
    return display_indent ( indent ) + text + "\n";
}

a_string RfcliPDUDisplayer::display_prim_end   ( int indent , const char * text )
{
    return "";
}

a_string RfcliPDUDisplayer::display_name    ( int indent , const char * name , const unsigned char * string )
{
    return display_indent ( indent ) + name + " = " + (const char*)string + "\n";
}

a_string RfcliPDUDisplayer::display_data    ( int indent , const char * name , const a_string& string )
{
    return display_indent ( indent ) + name + " =\n" + string;
}

a_string RfcliPDUDisplayer::prefix_time( const TimeStamp& when )
{
    a_string x ("---- ");
    x += when.hms() + " ------------------\n";
    return x;
}

a_string RfcliPDUDisplayer::prefix()
{
    return "------------------------------------\n";
}

a_string RfcliPDUDisplayer::postfix()
{
    return "------------------------------------\n";
}

a_string RfcliPDUDisplayer::display_unknown_type ( uint16 aType , const char * aChannel , const PDU& pdu )
{
    a_string rString;
    char type[7];
    sprintf ( type , "0x%04x" , aType );
    rString += aChannel;
    rString += " pdu (type ";
    rString += type ;
    rString += ") is not recognised.\n";
    rString += display_default( 0 , "Content", pdu );
    return rString;
}

a_string RfcliPDUDisplayer::display_ULP_SUB_EVENTS_T ( int indent , const char * name , uint8* value )
{
	return "Not Implemented\n";
}

a_string RfcliPDUDisplayer::display_STREAM_BUFFER_SIZES_T ( int indent , const char * name , STREAM_BUFFER_SIZES_T streams)
{
        return display_indent(indent) + name + " =\n"
            + display_uint16(indent + 1, "buffer_size_sink", streams.buffer_size_sink)
            + display_uint16(indent + 1, "buffer_size_source", streams.buffer_size_source);
}

a_string RfcliPDUDisplayer::display_DM_SM_SERVICE_T ( int indent , const char * name , DM_SM_SERVICE_T service )
{
    return display_indent(indent) + name + " =\n"
        + display_uint16(indent + 1, "protocol_id", service.protocol_id)
        + display_uint16(indent + 1, "channel", service.channel);
}

a_string RfcliPDUDisplayer::display_rs_table ( int indent , DM_LP_WRITE_ROLESWITCH_POLICY_REQ_T_PDU &prim )
{
    if (prim.get_length() == 0)
        return display_text(indent, "rs_table = NULL\n");

    a_string rv = display_text(indent, "rs_table =\n")
                + display_text(indent+1, "       Slv    Mst    USMst  All    RS_IN  NORS_OUT\\n");

    for ( int index = 0 ; index != prim.get_length() ; ++index )
    {
        char str[60];

        sprintf (str, "%3d)   %1d      %1d      %1d      %1d      %1d      %1d\\n" , index, prim.get_slaves(index),
                prim.get_masters(index), prim.get_unsniffed_masters(index), prim.get_connections(index),
                prim.get_roleswitch_incoming(index), prim.get_forbid_roleswitch_outgoing(index) );

        rv += display_text (indent+1, str );
    }

    return rv;
}

a_string RfcliPDUDisplayer::display_uint16_symbolic( int indent , const char * name , uint16 value, const char * symbol)
{
    char value_str[7];

    sprintf ( value_str , "0x%04x" , value );

    return display_indent ( indent ) + name + " = " + value_str + " (" + symbol + ")\n";
}

a_string RfcliPDUDisplayer::display_DM_SM_UKEY_T (int indent, const char * name, DM_SM_KEY_TYPE_T type, DM_SM_RFCLI_UKEY_T u)
{
    a_string rv = display_text(indent, name) + " =\n";

    switch (type)
    {
        case DM_SM_KEY_ENC_BREDR:
            rv += display_text(indent + 1, "enc_bredr =\n")
                + display_CryptoBlob(indent + 2, "link_key", 8, u.enc_bredr.link_key);
            break;

        case DM_SM_KEY_ENC_CENTRAL:
            rv += display_text(indent + 1, "enc_central =\n")
                + display_CryptoBlob(indent + 2, "ediv", 1, &u.enc_central.ediv)
                + display_CryptoBlob(indent + 2, "rand", 4, u.enc_central.rand)
                + display_CryptoBlob(indent + 2, "ltk", 8, u.enc_central.ltk);
            break;

        case DM_SM_KEY_DIV:
            rv += display_text(indent + 1, "div =\n")
                + display_CryptoBlob(indent + 2, "div", 1, &u.div);
            break;

        case DM_SM_KEY_SIGN:
            rv += display_text(indent + 1, "sign =\n")
                + display_CryptoBlob(indent + 2, "csrk", 8, u.sign.csrk);
            break;

        case DM_SM_KEY_ID:
            rv += display_text(indent + 1, "id =\n")
                + display_CryptoBlob(indent + 2, "irk", 8, u.id.irk);
            break;

        default:
            rv = "";
            break;
    }

    return rv;
}

a_string RfcliPDUDisplayer::display_DM_SM_UKEY_T_array (int indent, const char * name, uint16 keys_present, const DM_SM_RFCLI_UKEY_T *array)
{
    uint8 i;
    a_string rv;

    for (i = 0; keys_present != 0 && i != DM_SM_MAX_NUM_KEYS;
                ++i, keys_present >>= DM_SM_NUM_KEY_BITS)
        rv += display_DM_SM_UKEY_T(
            indent,
            name,
            (DM_SM_KEY_TYPE_T)(keys_present & ((1 << DM_SM_NUM_KEY_BITS) - 1)),
            array[i]);

    return rv;
}

a_string RfcliPDUDisplayer::display_SMKeyState( int indent , const char * name , const SMKeyState& value )
{
    return display_indent(indent) + name + " =\n"
         + display_CryptoBlob(indent + 1, "ir", 8, value.get_ir())
         + display_CryptoBlob(indent + 1, "er", 8, value.get_er());
}

////////////////////////////////////////////////////////////////////////////////
//  TCL array writing utility functions
////////////////////////////////////////////////////////////////////////////////

a_string TCLArrayDisplayer::display_bool   ( int indent , const char * name , bool   value )
{
    return display_indent(indent) + name + " " + ( value ? "TRUE" : "FALSE" );
}

a_string TCLArrayDisplayer::display_uint8  ( int indent , const char * name , uint8  value )
{
    char str[16];
    sprintf ( str , "0x%02x" , value);
    return display_indent ( indent ) + name + " " + str;
}

a_string TCLArrayDisplayer::display_int8   ( int indent , const char * name , int8   value )
{
    char str[16];
    sprintf ( str , "0x%02x" , (value & 0xFF) );
    return display_indent ( indent ) + name + " " + str;
}

a_string TCLArrayDisplayer::display_uint16 ( int indent , const char * name , uint16 value )
{
    char str[16];
    sprintf ( str , "0x%04x" , value);
    return display_indent ( indent ) + name + " " + str;
}

a_string TCLArrayDisplayer::display_int16  ( int indent , const char * name , int16  value )
{
    char str[16];
    sprintf ( str , "0x%04x" , (value & 0xFFFF) );
    return display_indent ( indent ) + name + " " + str;
}

a_string TCLArrayDisplayer::display_uint24 ( int indent , const char * name , uint24 value )
{
    char str[32];
    sprintf ( str , "0x%06lx" , value);
    return display_indent ( indent ) + name + " " + str;
}

a_string TCLArrayDisplayer::display_uint32 ( int indent , const char * name , uint32 value )
{
    char str[32];
    sprintf ( str , "0x%08lx" , value);
    return display_indent ( indent ) + name + " " + str;
}

a_string TCLArrayDisplayer::display_int32 (int indent , const char * name, int32 value)
{
    char str[32];
    sprintf ( str , "0x%08lx", (value & 0xFFFFFFFF) );
    return display_indent (indent ) + name + " " + str;
}

a_string TCLArrayDisplayer::display_uint8Array ( int indent , const char * name , const uint8* data , size_t size )
{
    a_string rv = display_indent ( indent ) + name + " {";
    char str[16];
    for ( size_t i = 0 ; i != size ; ++i )
    {
        sprintf ( str , "0x%02x" , data[i] );
        if ( i ) 
            rv += " ";
        rv += str;
    }
    return rv + "}";
}

a_string TCLArrayDisplayer::display_uint8Array ( int indent , const char * name , const PDU& pdu , size_t startdex , size_t size )
{
    return display_uint8Array ( indent , name , pdu.data() + startdex , size );
}

a_string TCLArrayDisplayer::display_uint16Array ( int indent , const char * name , const uint16* data , size_t count )
{
    a_string rv = display_indent ( indent ) + name + " {";
    char str[16];
    for ( size_t i = 0 ; i != count ; ++i )
    {
        sprintf ( str , "0x%04x" , data[i] );
        if ( i ) 
            rv += " ";
        rv += str;
    }
    return rv + "}";
}

a_string TCLArrayDisplayer::display_uint24Array ( int indent , const char * name , const uint24* data , size_t count )
{
    a_string rv = display_indent ( indent ) + name + " {";
    char str[16];
    for ( size_t i = 0 ; i != count ; ++i)
    {
        sprintf ( str , "0x%06lx" , data[i] );
        if ( i ) 
            rv += " ";
        rv += str;
    }
    rv += "}";
    return rv;
}

a_string TCLArrayDisplayer::display_uint32Array ( int indent , const char * name , const uint32* data , size_t count )
{
    a_string rv = display_indent ( indent ) + name + " {";
    char str[16];
    for ( size_t i = 0 ; i < count ; ++i )
    {
        sprintf ( str , "0x%08lx" , data[i] );
        if ( i ) 
            rv += " ";
        rv += str;
    }
    rv += "}";
    return rv;
}

a_string TCLArrayDisplayer::display_BluetoothDeviceAddress ( int indent , const char * name , const BluetoothDeviceAddress& value )
{
    char str[16];
    sprintf ( str , "0x%04x%02x%06lx" , value.get_nap() , value.get_uap() , value.get_lap() );
    return display_indent ( indent ) + name + " " + str;
}

a_string TCLArrayDisplayer::display_BluetoothDeviceAddressWithType ( int indent , const char * name , const BluetoothDeviceAddressWithType& value )
{
    return display_indent ( indent ) + name + " {"
        + display_uint8 ( indent + 1 , "type" , value.get_type() )
        + display_BluetoothDeviceAddress ( indent + 1 , "bd_addr", value.get_addr() )
        + " }";
}

a_string TCLArrayDisplayer::display_EtherNetAddress ( int indent , const char * name , const EtherNetAddress& value )
{
    return  display_uint16Array ( indent , name , value.get_data() , 3 );
}

a_string TCLArrayDisplayer::display_IPAddress ( int indent , const char * name , const IPAddress& value )
{
    return display_uint16Array ( indent , name , value.get_data() , 8 );
}

a_string TCLArrayDisplayer::display_HCIEventMask ( int indent , const char * name , const HCIEventMask& value )
{
    const uint8 * data = value.get_data();
    char str[32];
    sprintf ( str , "0x%02x%02x%02x%02x%02x%02x%02x%02x" , data[7] , data[6] , data[5] , data[4] , data[3] , data[2] , data[1] , data[0] );
    return display_indent(indent) + name + " " + str;
}

a_string TCLArrayDisplayer::display_ULPEventMask ( int indent , const char * name , const ULPEventMask& value )
{
    const uint8 * data = value.get_data();
    char str[32];
    sprintf ( str , "0x%02x%02x%02x%02x%02x%02x%02x%02x" , data[7] , data[6] , data[5] , data[4] , data[3] , data[2] , data[1] , data[0] );
    return display_indent(indent) + name + " " + str;
}

a_string TCLArrayDisplayer::display_DMHCIEventMask ( int indent , const char * name , const HCIEventMask& value )
{
    const uint8 * data = value.get_data();
    char str[32];
    sprintf ( str , "0x%02x%02x%02x%02x%02x%02x%02x%02x" , data[5] , data[4] , data[7] , data[6] , data[1] , data[0] , data[3] , data[2] );
    return display_indent(indent) + name + " " + str;
}

a_string TCLArrayDisplayer::display_CryptoBlob ( int indent , const char * name , uint8 length, const uint16 *array )
{
    return display_indent(indent) + name + " "
            + PDU_displayer::display_CryptoBlob(length, array);
}

a_string TCLArrayDisplayer::display_InquiryResult ( int indent , const char * name , const InquiryResult& value )
{
    return display_indent( indent ) + name + " {"
         + display_BluetoothDeviceAddress ( indent + 1 , "bd_addr" , value.get_bd_addr() )
         + display_uint8 ( indent + 1 , "Page_Scan_Repetition_Mode" , value.get_page_scan_repetition_mode() )
         + display_uint8 ( indent + 1 , "Page_Scan_Period_Mode" , value.get_page_scan_period_mode() )
         + display_uint8 ( indent + 1 , "Page_Scan_Mode(1.2:Rsvd)" , value.get_page_scan_mode() )
         + display_uint24 ( indent + 1 , "Class_of_Device" , value.get_class_of_device() )
         + display_uint16 ( indent + 1 , "Clock_Offset" , value.get_clock_offset() )
         + " }";
}

a_string TCLArrayDisplayer::display_InquiryResultWithRSSI ( int indent , const char * name , const InquiryResultWithRSSI& value )
{
    return display_indent( indent ) + name + " {"
         + display_BluetoothDeviceAddress ( indent + 1 , "bd_addr" , value.get_bd_addr() )
         + display_uint8 ( indent + 1 , "Page_Scan_Repetition_Mode" , value.get_page_scan_repetition_mode() )
         + display_uint8 ( indent + 1 , "Page_Scan_Period_Mode" , value.get_page_scan_period_mode() )
         + display_uint24 ( indent + 1 , "Class_of_Device" , value.get_class_of_device() )
         + display_uint16 ( indent + 1 , "Clock_Offset" , value.get_clock_offset() )
         + display_uint8 ( indent + 1 , "RSSI" , value.get_rssi() )
         + " }";
}

a_string TCLArrayDisplayer::display_ncp_elementArray ( int indent , const char * name , const PDU_displayer::ncp_element*data , size_t count )
{
    a_string rv = display_indent( indent ) + name + " {";
    for ( size_t i = 0 ; i < count ; ++i )
        rv += "{"
           +  display_uint16 ( indent + 1 , "handle" , data[i].handle )
           +  display_uint16 ( indent + 1 , "packets" , data[i].pkts )
           +  "}";
    return rv + "}";
}

a_string TCLArrayDisplayer::display_link_key_bd_addrArray ( int indent , const char * name , const PDU_displayer::lb_element* data , size_t count )
{
    a_string rv = display_indent( indent ) + name + " {";
    for ( size_t i = 0 ; i < count ; ++i )
    {
        const uint8 *link_key_data = data[i].key.get_data();
        if (link_key_data)
        {
            rv += "{"
               +  display_BluetoothDeviceAddress ( indent , "bd_addr" , data[i].bd_addr )
               +  display_uint8Array ( indent , "link_key" , data[i].key.get_data() , 16 )
               +  "}";
        }
        else
        {
            rv += "Link Key missing\n";
        }
    }
    return rv + "}";
}

a_string TCLArrayDisplayer::display_lp_powerstate ( int indent , const char * name , const LP_POWERSTATE_T& value )
{
    a_string rv = display_indent(indent) + name + " {";
	rv += display_uint8(indent + 1 , "mode" , value.mode )
	   +  display_uint16(indent + 1, "min_interval" , value.min_interval )
	   +  display_uint16(indent + 1, "max_interval" , value.max_interval )
	   +  display_uint16(indent + 1, "attempt" , value.attempt )
	   +  display_uint16(indent + 1, "timeout" , value.timeout )
	   +  display_uint16(indent + 1, "duration" , value.duration );
	return rv + "}";
}

a_string TCLArrayDisplayer::display_indent ( int indent )
{
    return " ";
}

a_string TCLArrayDisplayer::display_text   ( int indent , const char * text )
{
    return display_indent ( indent ) + text ;
}

a_string TCLArrayDisplayer::display_prim_start   ( int indent , const char * text )
{
    return display_indent ( indent ) + text + " {";
}

a_string TCLArrayDisplayer::display_prim_end   ( int indent , const char * text )
{
    return " }";
}

a_string TCLArrayDisplayer::display_name    ( int indent , const char * name , const unsigned char * string )
{
    return display_indent ( indent ) + name + " \"" + (const char*)string + "\"";
}

a_string TCLArrayDisplayer::display_data    ( int indent , const char * name , const a_string& string )
{
    return display_indent ( indent ) + name + " {" + string + " }";
}

a_string TCLArrayDisplayer::prefix_time( const TimeStamp& when )
{
    return a_string("time ") + when.hms().c_str();
}

a_string TCLArrayDisplayer::prefix()
{
    return "";
}

a_string TCLArrayDisplayer::postfix()
{
    return "";
}

a_string TCLArrayDisplayer::display_unknown_type ( uint16 aType , const char * aChannel , const PDU& pdu )
{
    return display_indent(0) 
         + display_text(0,"channel") + " " + display_text(0,aChannel)
         + display_uint16(0,"type",aType)
         + display_default(0,"pdu",pdu );
}

a_string TCLArrayDisplayer::display_ULP_SUB_EVENTS_T ( int indent , const char * name , uint8* value )
{
	return "Not Implemented\n";
}

a_string TCLArrayDisplayer::display_STREAM_BUFFER_SIZES_T ( int indent , const char * name , STREAM_BUFFER_SIZES_T streams)
{
    return display_indent(indent) + name + " {"
         + display_uint16(indent + 1, "buffer_size_sink", streams.buffer_size_sink)
         + display_uint16(indent + 1, "buffer_size_source", streams.buffer_size_source)
         + "}";
}

a_string TCLArrayDisplayer::display_DM_SM_SERVICE_T ( int indent , const char * name , DM_SM_SERVICE_T service )
{
    return display_indent(indent) + name + " {"
         + display_uint16(indent + 1, "protocol_id", service.protocol_id)
         + display_uint16(indent + 1, "channel", service.channel)
         + "}";
}

a_string TCLArrayDisplayer::display_rs_table ( int indent , DM_LP_WRITE_ROLESWITCH_POLICY_REQ_T_PDU &prim )
{
    a_string rv = display_text(indent, "rs_table { ");

    for ( int index = 0 ; index != prim.get_length() ; ++index )
    {
        char str[60];

        sprintf (str, "{ %1d %1d %1d %1d %1d %1d } ", prim.get_slaves(index), prim.get_masters(index),
                prim.get_unsniffed_masters(index), prim.get_connections(index), prim.get_roleswitch_incoming(index),
                prim.get_forbid_roleswitch_outgoing(index) );
    }

    return rv + "}";
}

a_string TCLArrayDisplayer::display_uint16_symbolic( int indent , const char * name , uint16 value, const char * symbol)
{
    char value_str[7];

    sprintf ( value_str , "0x%04x" , value );

    return display_indent ( indent ) + name + " " + symbol;
}

a_string TCLArrayDisplayer::display_DM_SM_UKEY_T (int indent, const char * name, DM_SM_KEY_TYPE_T type, DM_SM_RFCLI_UKEY_T u)
{
    a_string rv;

    switch (type)
    {
        case DM_SM_KEY_ENC_BREDR:
            rv = display_CryptoBlob(indent + 2, "link_key", 8, u.enc_bredr.link_key);
            break;

        case DM_SM_KEY_ENC_CENTRAL:
            rv = display_CryptoBlob(indent + 2, "ediv", 1, &u.enc_central.ediv)
                + display_CryptoBlob(indent + 2, "rand", 4, u.enc_central.rand)
                + display_CryptoBlob(indent + 2, "ltk", 8, u.enc_central.ltk);
            break;

        case DM_SM_KEY_DIV:
            rv = display_CryptoBlob(indent + 2, "div", 1, &u.div);
            break;

        case DM_SM_KEY_SIGN:
            rv = display_CryptoBlob(indent + 2, "csrk", 8, u.sign.csrk);
            break;

        case DM_SM_KEY_ID:
            rv = display_CryptoBlob(indent + 2, "irk", 8, u.id.irk);
            break;

        default:
            rv = "";
            break;
    }

    return rv;
}

a_string TCLArrayDisplayer::display_DM_SM_UKEY_T_array (int indent, const char * name, uint16 keys_present, const DM_SM_RFCLI_UKEY_T *array)
{
    uint8 i;
    a_string rv;

    for (i = 0; keys_present != 0 && i != DM_SM_MAX_NUM_KEYS;
                ++i, keys_present >>= DM_SM_NUM_KEY_BITS)
        rv += display_DM_SM_UKEY_T(
            indent,
            name,
            (DM_SM_KEY_TYPE_T)(keys_present & ((1 << DM_SM_NUM_KEY_BITS) - 1)),
            array[i]);

    return rv;
}

a_string TCLArrayDisplayer::display_SMKeyState( int indent , const char * name , const SMKeyState& value )
{
    return display_indent(indent) + name + " {"
         + display_CryptoBlob(indent + 1, "ir", 8, value.get_ir())
         + display_CryptoBlob(indent + 1, "er", 8, value.get_er())
         + "}";
}

///////////////////////////////////////////////////////////////////////////////
//  a_string definitions.
///////////////////////////////////////////////////////////////////////////////

a_string::a_string ( const char * s ) :
internal ( s ? s : "" ) {}

a_string& a_string::operator+ ( const a_string& b )
{ internal += b.internal; return *this; }

a_string& a_string::operator+ ( const char * a )
{ internal.append ( a ); return *this; }

a_string& a_string::operator+ ( const std::string& a )
{ internal += a; return *this; }

void a_string::operator= ( const a_string& a )
{ internal = a.internal; }

void a_string::operator= ( const char * a )
{ internal = a; }

void a_string::operator= ( const std::string& a )
{ internal = a; }

void a_string::operator+= ( const a_string& a )
{ internal += a.internal; }

void a_string::operator+= ( const char * a )
{ internal.append ( a ); }

void a_string::operator+= ( const uint8 * a )
{ internal.append ( (const char*) a ); }

void a_string::operator+= ( const std::string& a )
{ internal += a; }

a_string& a_string::append ( int count , const char a )
{ internal.append ( count , a ); return *this; }

const std::string& a_string::get_string() const
{ return internal; }

a_string operator+ ( const char * a , const a_string& b )
{
    return a_string(a) + b;
}

#endif//USE_DISPAY_PDU
