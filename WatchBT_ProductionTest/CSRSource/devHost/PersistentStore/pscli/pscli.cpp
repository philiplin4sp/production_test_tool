// pscli.cpp
// Command line interface to the persistent store DLL.
/*
MODIFICATION HISTORY
		#51   18:mar:04  sjw   B-671: canned_entry now part of CPSHelper.


RCS IDENTIFIER
  $Id: //depot/bc/bluesuite_2_4/devHost/PersistentStore/pscli/pscli.cpp#1 $

*/

#include <iosfwd>
#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>
#include <ostream>
#include <istream>
#include <iostream>
#include <iomanip>
#include <ios>

using namespace::std;

#include "common/portability.h"
#include "ps/pshelp.h"
#include "csrhci/bccmd_spi.h"
#include "csrhci/bccmd_hci.h"
#include "csrhci/transportconfiguration.h"
#include "pscli.h"
#include "unicode/ichar.h"
#include "unicode/main.h"
#include "thread/system_wide_mutex.h"
#include "pttransport/pttransport.h"
#include "time/hi_res_clock.h"
#include "common/globalversioninfo.h"

#ifndef WIN32
#define stricmp(x,y) strcasecmp(x,y)
#endif

#ifdef UNICODE__ICHAR_WIDE
#define ICHAR_SELECT(a,w) w
#else
#define ICHAR_SELECT(a,w) a
#endif

ichar *name_string = II("pscli - command line interface to the persistent store.");

// Flags turned on/off via command line
typedef enum
{
    BOOL_OPTION_NOBOOTMODE,
    BOOL_OPTION_DEBUG,
    BOOL_OPTION_VERBOSE,
    NUM_BOOL_OPTIONS
}
BoolOptionsEnum;

// Non-boolean options set via the command line
typedef enum
{
    OPTION_NOBOOTMODE,
    OPTION_VERBOSE,
    OPTION_MUL,
    OPTION_USB,
    OPTION_TRANS,
    OPTION_STORE,
    OPTION_SUGGEST,
    NUM_OPTIONS
}
OptionsEnum;

// Global options
static bool gOptions[NUM_BOOL_OPTIONS] = {false, false, false};
static bool gOptionGiven[NUM_OPTIONS] = {false, false, false, false, false, false};

bccmd_trans_error_response CPSCLIErrorHandler::on_error_extra(const ichar *error_string, const ichar * caption, const int error_code, const bccmd_trans_error_type error_type)
{
	if ((error_type == BCCMD_TRANS_ERRTYPE_MESSAGE) || (error_code == BCCMD_TRANS_ERROR_MESSAGE))
	{
		if (gOptions[BOOL_OPTION_VERBOSE])
        {
			ifprintf(stdout, II("%s\n"), error_string);
        }
		return BCCMD_TRANS_ERROR_CONTINUE;
	}
	else
	{
		return BCCMD_TRANS_ERROR_ABORT;
	}
}

// ===========================================================================================
// Output debug information
// ===========================================================================================
void show_debug(const ichar *format, ...)
{
    if (gOptions[BOOL_OPTION_DEBUG])
    {
        iprintf(II("[DEBUG] "));

  	    va_list argptr;
	    va_start(argptr, format);
	    ICHAR_SELECT(vprintf, vwprintf)(format, argptr);
	    va_end(argptr);

        iprintf(II("\n"));
    }
}

// ===========================================================================================
// Display a message followed by the usage information
// ===========================================================================================
void do_usage_msg(const ichar *format, ...)
{
    iprintf(II("========================================================================\n"));
    iprintf(II("== Error: "));

  	va_list argptr;
	va_start(argptr, format);
	ICHAR_SELECT(vprintf, vwprintf)(format, argptr);
	va_end(argptr);

    iprintf(II("\n"));
    iprintf(II("========================================================================\n"));
    do_usage();
}

// ===========================================================================================
// Sets the store
// ===========================================================================================
int set_store(const ichar *store_str)
{
	int store = PS_STORES_DEFAULT;

	if(istricmp(store_str, II("default")) == 0)
    {
		store = PS_STORES_DEFAULT;
    }
	else if(istricmp(store_str, II("imp")) == 0)
    {
		store = PS_STORES_I;
    }
	else if(istricmp(store_str, II("rom")) == 0)
    {
		store = PS_STORES_R;
    }
	else if(istricmp(store_str, II("ram")) == 0)
    {
		store = PS_STORES_T;
    }
	else if(istricmp(store_str, II("fact")) == 0)
    {
		store = PS_STORES_F;
    }
	else
    {
		store = iatoi(store_str);
        if (store == 0)
        {
            do_usage_msg(II("Invalid store \"%s\""), store_str);
        }
    }
    show_debug(II("Store set to \"%s\""), store_str);
    return store;
}

// ===========================================================================================
// Checks that at least the required number of parameters are left for this option/action
// ===========================================================================================
bool check_params
(
    int             current_index,      // The index of the current argument
    const ichar*    str,                // The option/action string that was matched e.g. "-trans"
    int             argc,
    int             num_params_expected // The number of parameters needed for the current command
)
{
    bool valid = true;
	if (current_index + num_params_expected >= argc)
	{
        do_usage_msg(II("Not enough parameters to \"%s\""), str);
    }
    return valid;
}

// ===========================================================================================
// Checks that the parameters for an option are correct
// ===========================================================================================
bool check_option
(
    int             current_index,      // The index of the current argument
    const ichar*    option_str,         // The option string that was matched e.g. "-trans"
    int             argc,
    int             num_params_expected, // The number of parameters needed for the current command
    OptionsEnum     option_type
)
{
    bool valid = true;  // no need to return false - program will exit on usage
	if (check_params(current_index, option_str, argc, num_params_expected))
	{
        if (gOptionGiven[option_type])
        {
            do_usage_msg(II("Option \"%s\" given more than once"), option_str);
        }
        else
        {
            gOptionGiven[option_type] = true;
            show_debug(II("option: %s"), option_str);
        }
    }
    return valid;
}

// ===========================================================================================
// Checks that the parameters for an action are correct
// ===========================================================================================
bool check_action
(
    int             current_index,      // The index of the current argument
    const ichar*    action_str,         // The option string that was matched e.g. "-trans"
    int             argc,
    int             num_params_expected // The number of parameters needed for the current command
)
{
    if (check_params(current_index, action_str, argc, num_params_expected))
    {
        show_debug(II("action: %s"), action_str);
    }
    return true;
}

// ===========================================================================================
// Checks that the string is a valid number
// ===========================================================================================
bool is_valid_number(const ichar *command_str, const ichar *number_str)
{
    size_t i;
    bool is_number = true;

    for (i = 0; i < istrlen(number_str); i++)
    {
        if (!iisdigit(number_str[i]))
        {
            is_number = false;
            break;
        }
    }
    if (!is_number)
    {
        do_usage_msg(II("Parameter to \"%s\" must be an positive integer, got \"%s\""), command_str, number_str);
    }
    return is_number;
}

// ===========================================================================================
// Outputs a success or failure message to the screen
// ===========================================================================================
void show_final_result
(
    int argc,
    ichar* argv[],
	CPSCLIErrorHandler &error_handler
)
{
	iprintf(II("pscli"));
	for (int j=1; j < argc; ++j)
	{
		iprintf(istrchr(argv[j], II(' ')) ? II(" \"%s\"") : II(" %s"), argv[j]);
	}
	iprintf(II(" : concluded with "));

    if (error_handler.get_last_error_code() == 0)
	{
	    iprintf(II("Success\n"));
	}
    else
	{
        iprintf(II("Failure ("));
		iprintf(II("%s"), error_handler.get_last_error_string());     
		iprintf(II(")\n"));
	}
}


// ===========================================================================================
// main
// ===========================================================================================
int imain(int argc, ichar* argv[])
{
	setvbuf( stdout, NULL, _IONBF, 0 );

	CPSHelper helper;
	CPSCLIErrorHandler error_handler;

	std::string trans;
	int mul = -1;
	bool do_suggest = false;    // Allows a "suggest" to not try and connect to the chip
	std::string suggest_argument;
	int i;
	int stores = PS_STORES_DEFAULT;

    iprintf (II("pscli, version %s\n"), II(VERSION_BLUESUITE_STR));

    // If the special build string is set then show the date of the build
    if (strcmp (VERSION_SPECIAL_BUILD, "") != 0)
    {
        iprintf (II("%s\n"), II(VERSION_SPECIAL_BUILD));
	}

    const istring copyright = icoerce(VERSION_COPYRIGHT_START_YEAR("2001"));
    iprintf (II("%s\n"), copyright.c_str());

	struct Jobs
	{
		struct Job
		{
			enum
			{
				enum_trans,
				dump,
				query,
				merge,
				warm_reset,
				cold_reset,
				canned
			} type;
			struct
			{
				union
				{
					const ichar *str;
					uint16 num;
				} arg1;
				union
				{
					const ichar *str;
				} arg2;
			} args;

			Job() : next(NULL) {}
			~Job() { delete next; }

			Job *next;
		} *head, *tail;

		Jobs() : head(NULL), tail(NULL) {}
		~Jobs() { delete head; }
	} jobs;

    // Look for debug only
    for(i = 1; i < argc; ++i)
    {
		if(istricmp(argv[i], II("-DEBUG")) == 0)
		{
		    gOptions[BOOL_OPTION_DEBUG] = true;
		}
    }

    show_debug(II("Processing command line..."));
    for(i = 1; i < argc; ++i)
    {
		if(istricmp(argv[i], II("-MUL")) == 0)
		{
            if (check_option(i, argv[i], argc, 1, OPTION_MUL))
            {
                ++i;
                // Check that at least the first character is a digit
                if (is_valid_number(II("-MUL"), argv[i]))
                {
    		        mul = iatoi(argv[i]);
                }
            }
		}
		else if(istricmp(argv[i], II("-USB")) == 0)
		{
            if (check_option(i, argv[i], argc,  1, OPTION_USB))
            {
                i++;
                if (is_valid_number(II("-USB"), argv[i]))
                {
                    if(gOptionGiven[OPTION_TRANS])
                    {
                        do_usage_msg(II("-USB cannot be used with -TRANS"));
                    }

                    std::ostringstream format;
                    int num = iatoi(argv[i]);
                    format << "SPITRANS=USB SPIPORT=" << num;
    				trans = inarrow(format.str());
                    show_debug(II("usb=%s"), argv[i]);
                }
			}
		}
		else if(istricmp(argv[i], II("-TRANS")) == 0)
		{
            if (check_option(i, argv[i], argc,  1, OPTION_TRANS))
            {
                if(gOptionGiven[OPTION_USB])
                {
                    do_usage_msg(II("-TRANS cannot be used with -USB"));
                }

                i++;
				trans = inarrow(argv[i]);
                show_debug(II("trans=\"%s\""), argv[i]);
			}
		}
		else if(istricmp(argv[i], II("-VERBOSE")) == 0)
		{
            if (check_option(i, argv[i], argc,  0, OPTION_VERBOSE))
            {
			    gOptions[BOOL_OPTION_VERBOSE] = true;
            }
		}
		else if(istricmp(argv[i], II("-HELP")) == 0)
		{
			do_usage();
		}
		else if(istricmp(argv[i], II("-NOBOOTMODE")) == 0)
		{
            if (check_option(i, argv[i], argc,  0, OPTION_NOBOOTMODE))
            {
    		    gOptions[BOOL_OPTION_NOBOOTMODE] = true;
            }
		}
		else if(istricmp(argv[i], II("-STORE")) == 0)
		{
            if (check_option(i, argv[i], argc,  1, OPTION_STORE))
            {
				++i;
                stores = set_store(argv[i]);
			}
		}
		else if(istricmp(argv[i], II("-DEBUG")) == 0)
		{
			// Already processed
		}
		else if(istricmp(argv[i], II("SUGGEST")) == 0)
		{
			if (i + 1 >= argc)
			{
				do_usage_msg(II("Please specify a variable with \"suggest\""));
			}
            ++i;
			do_suggest = true;
			suggest_argument = inarrow(argv[i]);

		}		
		else
		{
			Jobs::Job *new_job = new Jobs::Job;
			if(jobs.tail) jobs.tail->next = new_job;
			else jobs.head = new_job;
			jobs.tail = new_job;

			if(istricmp(argv[i], II("COLD_RESET")) == 0 || istricmp(argv[i], II("-r")) == 0)
			{
				if (check_action(i, argv[i], argc,  0))
				{
					new_job->type = Jobs::Job::cold_reset;
				}
			}
			else if(istricmp(argv[i], II("WARM_RESET")) == 0 || istricmp(argv[i], II("-w")) == 0)
			{
				if (check_action(i, argv[i], argc,  0))
				{
					new_job->type = Jobs::Job::warm_reset;
				}
			}
			else if(istricmp(argv[i], II("MERGE")) == 0 || istricmp(argv[i], II("-m")) == 0)
			{
				if (check_action(i, argv[i], argc,  1))
				{
					new_job->type = Jobs::Job::merge;
					new_job->args.arg1.str = argv[++i];
				}
			}
			else if(istricmp(argv[i], II("DUMP")) == 0 || istricmp(argv[i], II("-d")) == 0)
			{
				if (check_action(i, argv[i], argc,  1))
				{
					new_job->type = Jobs::Job::dump;
					new_job->args.arg1.str = argv[++i];
				}
			}
			else if(istricmp(argv[i], II("QUERY")) == 0 || istricmp(argv[i], II("-q")) == 0)
			{
				if (check_action(i, argv[i], argc,  2))
				{
					new_job->type = Jobs::Job::query;
					new_job->args.arg1.str = argv[++i];
					new_job->args.arg2.str = argv[++i];
				}
			}
			else if(argv[i][0] == II('@'))
			{
    			if (!helper.is_valid_canned(argv[i]))
                {
    				do_usage_msg(II("Unknown canned operation: \"%s\""), argv[i]);
                }
                else
                {
   				    new_job->type = Jobs::Job::canned;
    			    new_job->args.arg1.str = argv[i];
                }
			}
			else
			{
				do_usage_msg(II("Unknown command line argument: \"%s\""), argv[i]);
			}
		}
    }

	if (do_suggest) 
	{
		if (jobs.head)
		{
			do_usage_msg(II("Cannot use \"suggest\" in association with another action or canned operation"));
		}
		else 
		{
			bccmd_trans::trans_suggestion **suggestions = bccmd_trans::suggest_value(trans.c_str(), suggest_argument.c_str());
			if(*suggestions)
			{
				iprintf(II("suggestions for variable \"%hs\":\n"), suggest_argument.c_str());
				for(bccmd_trans::trans_suggestion **s = suggestions; *s; ++s)
				{
					if((*s)->description) iprintf(II("\"%hs\" - %hs\n"), (*s)->value, (*s)->description);
					else iprintf(II("\"%hs\"\n"), (*s)->value);
				}
			}
			else
			{
				iprintf(II("no suggestions for variable \"%hs\".\n"), suggest_argument.c_str());
			}
			bccmd_trans::suggest_value_free(suggestions);
		}
	}

	class PSS {
	public:
		PSS(PsBccmd *p) : p_(p) {}
		PSS &operator=(PsBccmd *p) { p_=p; return *this; }
		~PSS() { delete p_; }
		PsBccmd *operator->() { return p_; }
		operator PsBccmd*() { return p_; }
	private:
		PsBccmd *p_;
	} pss(NULL);

	if(jobs.head)
	{
        bccmd_trans *bccmd_handler = bccmd_trans::create_transport(trans.c_str(), &error_handler, mul);
		if ((bccmd_handler) && (error_handler.get_last_error_code() == 0))
		{
			pss = gOptions[BOOL_OPTION_NOBOOTMODE] ? new PsBccmdRaw(bccmd_handler) : new PsBccmd(bccmd_handler);

			if (pss->open())
			{
				helper.SetServer(pss);
				helper.SetStores(stores);

				for(Jobs::Job *job = jobs.head; job && error_handler.get_last_error_code() == 0; job = job->next)
				{
					switch(job->type)
					{
					case Jobs::Job::dump:
						helper.dump_ps(job->args.arg1.str, CPSHelper::FileFormatCommented);
						break;
					case Jobs::Job::query:
						helper.run_query(job->args.arg1.str, job->args.arg2.str, CPSHelper::FileFormatCommented);
						break;
					case Jobs::Job::merge:
						helper.apply_file(job->args.arg1.str);
						break;
					case Jobs::Job::cold_reset:
						if(pss->reset_bc(RESET_COLD))
						{
							if(gOptions[BOOL_OPTION_VERBOSE]) iprintf(II("cold reset successful\n"));
						}
						else
						{
							// reset_bc will (hopefully) have reported an error, so only show this if verbose.
							if(gOptions[BOOL_OPTION_VERBOSE]) iprintf(II("cold reset failed\n"));
						}
						break;
					case Jobs::Job::warm_reset:
						if(pss->reset_bc(RESET_WARM))
						{
							if(gOptions[BOOL_OPTION_VERBOSE]) iprintf(II("warm reset successful\n"));
						}
						else
						{
							// reset_bc will (hopefully) have reported an error, so only show this if verbose.
							if(gOptions[BOOL_OPTION_VERBOSE]) iprintf(II("warm reset failed\n"));
						}
						break;
					case Jobs::Job::canned:
						helper.apply_canned(job->args.arg1.str);
						break;
					}
				}
			}
			else
			{
				if(gOptions[BOOL_OPTION_VERBOSE]) iprintf(II("open failed.\n"));
			}
		}
		else
		{
			if(gOptions[BOOL_OPTION_VERBOSE]) iprintf(II("create_transport failed.\n"));
		}
	}    
    else if (!do_suggest)
    {
		do_usage_msg(II("No command given"));
    }
	
    // Output a message, indicating success of failure
    show_final_result(argc, argv, error_handler);

	if(pss) pss->close();


	// return non-zero if an error has been reported
    return error_handler.get_last_error_code();
}


// ===========================================================================================
// Output usage information
// ===========================================================================================
void do_usage(void)
{
	iprintf(II("Usage:\n"));
	iprintf(II("pscli\n"));
	iprintf(II("\t[-STORE {default | imp | rom | ram | fact}]\n"));
	iprintf(II("\t[-TRANS \"...\"]\n"));
	iprintf(II("\t[-USB serialnumber]\n"));
	iprintf(II("\t[-MUL n]\n"));
	iprintf(II("\t[-VERBOSE]\n"));
	iprintf(II("\t[-NOBOOTMODE]\n"));
	iprintf(II("\t[-HELP]\n"));
	iprintf(II("\t[ dump <filename> ] [ query <filename> <filename> ] [ merge <filename> ]\n"));
    iprintf(II("\t[ suggest <spivariable> ] [ cold_reset ] [ warm_reset ]\n"));
	iprintf(II("\n"));
	iprintf(II("Actions and canned operations take place in the order they appear\n"));
	iprintf(II("on the command line.  Options are processed first and are\n"));
	iprintf(II("position-agnostic.\n"));
	iprintf(II("\n"));
	iprintf(II("Actions:\n"));
	iprintf(II("========\n"));
	iprintf(II("pscli suggest spiVariable\n"));
	iprintf(II("\tSuggests possible values for the SPI variable that can subsequently\n\tbe used with -TRANS e.g. suggest SPIPORT\n"));
	iprintf(II("pscli dump dumpFilename\n"));
	iprintf(II("\tdumps the contents of chip to dumpFilename.\n"));
	iprintf(II("pscli query dumpFilename queryFilename\n"));
	iprintf(II("\tdumps the keys specified in queryFilename to dumpFilename.\n"));
	iprintf(II("pscli merge mergeFilename\n"));
	iprintf(II("\tmerges the contents of mergeFilename into the contents of the chip.\n"));
	iprintf(II("pscli cold_reset\n"));
	iprintf(II("\tresets the chip, clearing persistent store keys in RAM.\n"));
	iprintf(II("pscli warm_reset\n"));
	iprintf(II("\tresets the chip, preserving persistent store keys in RAM.\n"));
	iprintf(II("\n"));
	iprintf(II("Please use \".psr\" as the extension of all persistent store files.\n"));
	iprintf(II("and \".psq\" as the extension of all query files.\n"));
	iprintf(II("\n"));
	iprintf(II("Options:\n"));
	iprintf(II("========\n"));
	iprintf(II("-TRANS to give a string of space-separated variables defining transport method.\n"));
	iprintf(II("-USB to specify the SPI usb device to use\n\t(equivalent to -TRANS \"SPITRANS=USB SPIPORT=n\")\n"));
	iprintf(II("-MUL to specify a port on an SPI multiplexor (default 0).\n"));
	iprintf(II("-NOBOOTMODE to read keys without translation for bootmode.\n"));
	iprintf(II("-STORE to specify the persistent store in which to perform the operation.\n"));
	iprintf(II("\tdefault specifies all stores in the usual order\n"));
	iprintf(II("\timp specifies just the flash or e2 store (or ram if others unavailable)\n"));
	iprintf(II("\trom specifies the values with the firmware\n"));
	iprintf(II("\tram specified the values in ram\n"));
	iprintf(II("\tfact specified the factory set values\n"));
	iprintf(II("\n"));
	iprintf(II("Canned operations:\n"));
	iprintf(II("==================\n"));
	const CPSHelper::canned_entry *pCanned;
	for(pCanned = canned; pCanned->name; pCanned++)
	{
		iprintf(II("%s\n  %s\n"), pCanned->name, pCanned->description);
	}
    exit(1);
}
