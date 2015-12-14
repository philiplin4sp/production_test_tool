/*
 * MODIFICATION HISTORY
 *      1.1         12:mar:03  pws     Created.
 *      1.2         18:mar:03  pws     Use printlf instead of printf.
 *      1.3         12:jun:03  pws     B-629: Alloc bug in struct_disp.
 *
 * Based on devHost/spi/emucust/struct.c.
 */
/*  system includes  */

#ifdef WIN32
#include <windows.h>
#include <winbase.h>
#endif

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "print.h"
#include "btcli.h"
#include "parselib.h"
#include "struct.h"
#include "dictionary.h"
#include "dictionaries.h"

/* yuk */
extern DictionaryPair *sym_d;

/* for now... */
#define ui_output_ws printlf

/******************************************************************************
Private typedefs, macros and constants.
******************************************************************************/

static int init_depth = 5; /* A reasonable default depth */


/*-------------------- Custom command implementation ------------------------*/

/******************************************************************************
Private variables.
******************************************************************************/

/******************************************************************************
Functions to implement the commands.
******************************************************************************/

#include <stdlib.h>
#include "struct-build.h"

static int get_char_from_file(void *param)
{
    return getc((FILE *) param);
}

static int parse_file(char *name)
{
    FILE *fp;
    int i;

    if ((fp = fopen(name, "r")) == NULL)
    {
        struct_perror(name);
        return 0;
    }

    clean_var();

    while((i = parse_cont(get_char_from_file, (void *) fp)) == 1)
	;


    fclose(fp);

    return (i == -1) ? 0 : 1;
}

static void describe_variable(struct var_info *vp, int disp_named)
{
    switch(vp->type & TYPE_BASIC)
    {
    case TYPE_NAMED:
	if (disp_named)
	    ui_output_ws("%s ", vp->name);
	break;
    case TYPE_STRUCT:
        ui_output_ws("struct ");
	if (vp->name != NULL)
	    ui_output_ws("%s ", vp->name);
        break;
    case TYPE_UNION:
        ui_output_ws("union ");
	if (vp->name != NULL)
	    ui_output_ws("%s ", vp->name);
	break;
    case TYPE_ENUM:
        ui_output_ws("enum ");
	if (vp->name != NULL)
	    ui_output_ws("%s ", vp->name);
	break;
    }
    if (vp->type & TYPE_UNSIGNED)
        ui_output_ws("unsigned ");
    if (vp->type & TYPE_SIGNED)
        ui_output_ws("signed ");
    if (vp->type & TYPE_LONG)
        ui_output_ws("long ");
    if (vp->type & TYPE_SHORT)
        ui_output_ws("short ");
    switch(vp->type & TYPE_BASIC)
    {
    case TYPE_POINTER:
        ui_output_ws("pointer to ");
        break;
    case TYPE_ARRAY:
        ui_output_ws("array [%d] of ", vp->dimension);
        break;
    case TYPE_FUNCTION:
        ui_output_ws("function returning ");
        break;
    case TYPE_VOID:
        ui_output_ws("void");
        break;
    case TYPE_CHAR:
        ui_output_ws("char");
        break;
    case TYPE_INT:
        ui_output_ws("int");
        break;
    case TYPE_FLOAT:
        ui_output_ws("float");
        break;
    case TYPE_DOUBLE:
        ui_output_ws("double");
        break;
    case TYPE_VALUE:
        ui_output_ws("= %d", vp->dimension);
        break;
    }
}

static void dump_variables(struct var_info *vp);

static void describe_variables(struct var_info *vp)
{
    if (vp->comp != NULL)
    {
	if (vp->type == TYPE_STRUCT || vp->type == TYPE_UNION ||
	    vp->type == TYPE_ENUM)
	{
	    if (vp->top_level == 0)
		dump_variables(vp);
	    else
		describe_variable(vp, 0);
	}
	else
	{
	    describe_variable(vp, 0);
	    describe_variables(vp->comp);
	}
    }
    else
	describe_variable(vp, 0);
}

static void dump_variables(struct var_info *vp)
{
    describe_variable(vp, 0);
    
    if (vp->type == TYPE_STRUCT || vp->type == TYPE_UNION  ||
	vp->type == TYPE_ENUM)
    {
	ui_output_ws("{\n");
	for(vp = vp->comp; vp != NULL; vp = vp->next)
	{
	    if (vp->name != NULL)
		ui_output_ws("%s: ", vp->name);
	    describe_variables(vp);
	    ui_output_ws("\n");
	}
	ui_output_ws("}");
    }
    else if (vp->comp != NULL)
	describe_variables(vp->comp);

    ui_output_ws("\n");
}

static void summarise_variable(struct var_info *vp)
{
    describe_variable(vp, 0);
    
    if (vp->type != TYPE_STRUCT && vp->type != TYPE_UNION &&
	vp->type != TYPE_ENUM && vp->comp != NULL)
        summarise_variable(vp->comp);

    ui_output_ws("\n");
}

static char *message_buffer = NULL;
static int message_length = 0;

static int set_message_size(int size)
{
    char *p;

    if (size <= message_length)
        return 1;

    if (message_buffer == NULL)
        message_buffer = (char *) malloc(size);
    else
    {
        p = (char *) realloc(message_buffer, size);
        if (p != NULL)
            message_buffer = p;
        else
        {
            message_buffer[message_length-1] = '.';
            message_buffer[message_length-2] = '.';
            message_buffer[message_length-3] = '.';
            return 0;
        }
    }

    if (message_buffer == NULL)
    {
        struct_perror("set_message_size");
        return 0;
    }

    message_length = size;
    return 1;
}

static int glob_match(char *testbuf, char *match)
{
    /*
     * Match simple globbing pattern "match" against test string
     * "testbuf".  Match only at (i.e. anchor to) the start of the
     * buffer.  Never anchored to end of string.
     *
     * Only the pattern characters * and ? are handled;
     * there is no way to quote them.
     */

    while (*match)
    {
	if (*match == '*')
	{
	    /*
	     * Match any substring including the empty string.
	     *
	     * Backtrack down matches for the remainder of the pattern
	     * after the '*' until we get a match.  This handles
	     * multiple '*'s in the pattern correctly.
	     * (In fact, forward tracking will work unless we have
	     * more complicated patterns, but the following is more
	     * extensible.)
	     */
	    char *startptr = testbuf;

	    /*
	     * Optimise out an initial * if we were searching substrings
	     * anyway.
	     */
	    if (*match == '*')
		match++;
	    startptr += strlen(testbuf) + 1;

	    do
	    {
		/*
		 * Written like this to be standard compatible.
		 * Pointing one off the end of an array is allowed
		 * (as long as you don't dereference it), pointing
		 * off the beginning isn't.
		 *
		 * Note that the first time through we try matching
		 * the remainder against the empty string.
		 */
		startptr--;
		if (glob_match(startptr, match))
		    return 1;
	    } while (startptr >= testbuf);

	    /*
	     * We've now tried all possibilities for the remainder of
	     * the string, so if none worked the match has already failed.
	     */
	    return 0;
	}
	else if (*match == '?')
	{
	    match++;
	    /* Match any single character:  must be exactly one. */
	    if (*testbuf)
		testbuf++;
	    else
		return 0;
	}
	else
	{
	    /* Normal character match. */
	    if (*testbuf++ != *match++)
		return 0;
	}
    }

    /*
     * The tail of the match isn't anchored, so we don't care if
     * there's unmatched stuff left in testbuf.  We've therefore succeded
     * if there's nothing left in the match pattern.
     */
    return !*match;
}

static int variable_addresses(int message_len, struct var_info *vp,
			      u32 base, int depth, int append_name,
			      char *match, int bitlen)
{
    int size, i, l, bits;

    if (depth == 0 || vp == NULL)
        return 0;

    if (append_name && vp->name != NULL)
    {
        i = strlen(vp->name);
        if (set_message_size(message_len + i + 1))
        {
            strcpy(message_buffer + message_len, vp->name);
            message_len += i;
        }
    }

    if (match != NULL && glob_match(message_buffer, match))
	match = NULL;

    switch(vp->type & TYPE_BASIC)
    {
    case TYPE_NAMED:
        return variable_addresses(message_len, vp->comp, base, depth, 0,
				  match, bitlen);
    case TYPE_STRUCT:
        if (set_message_size(message_len + 1))
        {
            if (message_len > 0 && message_buffer[message_len-1] == '*')
            {
                message_buffer[message_len-1] = '-';
                message_buffer[message_len++] = '>';
            }
            else
                message_buffer[message_len++] = '.';
            message_buffer[message_len] = '\0';
        }
        size = 0;
	bits = 0;
        for(vp = vp->comp; vp != NULL; vp = vp->next)
	{
	    if (vp->dimension == 0 || (vp->type & TYPE_COMPLEX) != 0)
	    {
		if (bits > 0)
		{
		    size++;
		    bits = 0;
		}
		size += variable_addresses(message_len, vp, base + size,
					   depth, 1, match, -1);
	    }
	    else
	    {
		bits += vp->dimension;
		if (bits > 16)
		{
		    size++;
		    (void) variable_addresses(message_len, vp, base + size,
					      -16, 1, match, vp->dimension);	
		    size += (vp->dimension>>4);
		    bits = vp->dimension & 15;
		}
		else 
		{
		    (void) variable_addresses(message_len, vp, base + size,
					      -16-bits, 1, match, vp->dimension);	
		    if (bits == 16)
		    {
			size++;
			bits = 0;
		    }
		}
	    }
	}
	if (bits > 0)
	{
	    bits = 0;
	    size++;
	}
        return size;
    case TYPE_UNION:
        if (set_message_size(message_len + 1))
        {
            if (message_len > 0 && message_buffer[message_len-1] == '*')
            {
                message_buffer[message_len-1] = '-';
                message_buffer[message_len++] = '>';
            }
            else
                message_buffer[message_len++] = '.';
            message_buffer[message_len] = '\0';
        }
        size = 0;
        for(vp = vp->comp; vp != NULL; vp = vp->next)
        {
            i = variable_addresses(message_len, vp, base, depth, 1, match, -1);
            if (i > size)
                size = i;
        }
        return size;
    case TYPE_ENUM:
	if (match == NULL)
	{
	    ui_output_ws("%.*s: addr = 0x%04x: type = enum %s", message_len,
			 message_buffer, (unsigned int)base,
			 (vp->name == NULL) ? "(unnamed)" : vp->name);
	    if (depth < 0 && bitlen > 0)
		ui_output_ws(": startbit = %d: bitlen = %d",
			     32 + depth, bitlen);
	    ui_output_ws("\n");
	}
        return 1;
    case TYPE_POINTER:
        if (vp->comp->type == TYPE_FUNCTION)
        {
	    if (match == NULL)
	    {
		ui_output_ws("%.*s: addr = 0x%04x: type = function pointer",
			     message_len, message_buffer,
			     (unsigned int)base);
		ui_output_ws("\n");
	    }
            return 2;
        }
        else
        {
	    if (match == NULL)
#if 1
	    {
		ui_output_ws("%.*s: addr = 0x%04x: type = pointer to ",
			     message_len, message_buffer, (unsigned int)base);
		if (vp->comp->type == TYPE_NAMED)
		    describe_variable(vp->comp->comp, 1);
		else
		    describe_variable(vp->comp, 0);
		ui_output_ws("\n");
	    }
#else
		ui_output_ws("%.*s at %04x = H'%04x\n", message_len,
			     message_buffer, base, mem);
	    if (mem != 0 && mem != 0xffff)
	    {
		if (set_message_size(message_len + 1))
		{
		    message_buffer[message_len++] = '*';
		    message_buffer[message_len] = '\0';
		}
		variable_addresses(message_len, vp->comp, (xapadr) mem,
				   depth-1, 0, match, -1);
	    }
#endif
            return 1;
        }
    case TYPE_ARRAY:
        if (set_message_size(message_len + 12))
            l = 0;
        else
            l = -1;
        size = 0;
        for(i = 0; i < vp->dimension; i++)
        {
            if (l != -1)
            {
                l = sprintf(message_buffer + message_len, "[%d", i);
                message_buffer[message_len + l] = ']';
                message_buffer[message_len + l + 1] = '\0';
            }
            size += variable_addresses(message_len + l + 1, vp->comp,
                                       base + size, depth, 0, match, -1);
        }
        return size;
    case TYPE_FUNCTION:
	if (match == NULL)
	    ui_output_ws("%.*s: addr = 0x%04x: type = function\n", message_len,
			 message_buffer, (unsigned int)base);
        return 0;
    case TYPE_VOID:
	if (match == NULL)
	    ui_output_ws("%.*s: addr = 0x%04x: type = void\n", message_len,
			 message_buffer, (unsigned int)base);
        return 0;
    case TYPE_CHAR:
	if (match == NULL)
	{
	    ui_output_ws("%.*s: addr = 0x%04x: type = char", message_len,
			 message_buffer, (unsigned int)base);
	    if (depth < 0 && bitlen > 0)
		ui_output_ws(": startbit = %d: bitlen = %d",
			     32 + depth, bitlen);
	    ui_output_ws("\n");
	}
	return 1;
    case TYPE_INT:
	if (match == NULL)
	{
	    ui_output_ws("%.*s: addr = 0x%04x", message_len, message_buffer,
			 (unsigned int)base);
	}
        if (vp->type & TYPE_LONG)
        {
	    if (match == NULL)
	    {
		ui_output_ws(": type =");
		if (vp->type & TYPE_UNSIGNED)
		    ui_output_ws(" unsigned");
		ui_output_ws(" long");
		if (depth < 0 && bitlen > 0)
		    ui_output_ws(": startbit = %d: bitlen = %d",
				 48 + depth, bitlen);
		ui_output_ws("\n");
	    }
            return 2;
        }
        else if (vp->type & TYPE_SHORT)
        {
	    if (match == NULL)
	    {
		ui_output_ws(": type =");
		if (vp->type & TYPE_UNSIGNED)
		    ui_output_ws(" unsigned");
		ui_output_ws(" short");
		if (depth < 0 && bitlen > 0)
		    ui_output_ws(": startbit = %d: bitlen = %d",
				 32 + depth, bitlen);
		ui_output_ws("\n");
	    }
            return 1;
        }
        else
        {
	    if (match == NULL)
	    {
		ui_output_ws(": type =");
		if (vp->type & TYPE_UNSIGNED)
		    ui_output_ws(" unsigned");
		ui_output_ws(" int");
		if (depth < 0 && bitlen > 0)
		    ui_output_ws(": startbit = %d: bitlen = %d",
				 32 + depth, bitlen);
		ui_output_ws("\n");
	    }
            return 1;
        }
    case TYPE_FLOAT:
	if (match == NULL)
	    ui_output_ws("%.*s: addr = 0x%04x: type = float\n", message_len,
			 message_buffer, (unsigned int)base);
        return 2;
    case TYPE_DOUBLE:
	if (match == NULL)
	    ui_output_ws("%.*s: addr = 0x%04x: type = double\n", message_len,
			 message_buffer, (unsigned int)base);
        return 4;
    }

    return 0;
}

#define STRINGBUFLEN 1024

static void struct_disp_failed(char *match, char *var)
{
    /* No arguments - print syntax message and exit */
    printSyntax(" \"location[/match]\" [\"type-specifier\"] [\"*count\"]");
    printSyntaxContin(" type-specifier: typedef-type | struct|enum|union type");
    /*
     * Original syntax message was:
     * Syntax:    struct_disp location[/match] [type specifier] [*count]
     * type_specifier is the name of the type as specified in the
     * structure specification file. Note the difference between just
     * specifying 'type', which will look for a typedef or symbol name,
     * and specifying 'struct type' or 'union type' which will look for
     * a structure or union definition with the specified name. If the
     * optional match is specified, then only those parts of the
     * structure containing that string will be printed.
     */
    if (match)
	free(match);
    if (var)
	free(var);
}

void struct_disp(void)
{
    struct var_info *vp;
    int l, count = 1, i, done_type, done_count, struct_type = TYPE_NAMED;
    int  bad;
    u32 adr;
    char *match = NULL, *var = NULL, *struct_name = NULL, *match2;
    char stringbuf[STRINGBUFLEN];
    size_t gsret;

    if ((gsret = getStringZ(stringbuf, STRINGBUFLEN)) == 0 ||
	gsret == BADSIZE)
    {
	struct_disp_failed(match, var);
	return;
    }

    if ((match = strchr(stringbuf, '/')) != NULL)
    {
	*match++ = 0;
	/* No anchor at start of match; search for substring. */
	match2 = (char *)malloc(strlen(match) + 2);
	if (!match2)
	    return;
	*match2 = '*';
	strcpy(match2+1, match);
	match = match2;
    }
    else if ((match = strchr(stringbuf, '.')))
    {
	/* Anchor at start, look for entire name as match. */
	match2 = (char *)malloc(strlen(stringbuf) + 1);
	if (!match2)
	    return;
	strcpy(match2, stringbuf);
	/* Insert NULL to normalise variable name */
	*match = 0;
	match = match2;
    }

    var = (char *)malloc(strlen(stringbuf)+1);
    if (!var)
    {
	if (match)
	    free(match);
	return;
    }
    strcpy(var, stringbuf);

    /*
     * (A comment; remember these?)
     * All remaining arguments are optional.  Make sure we don't
     * have too many.  The "*count" must be last.
     */
    done_type = done_count = 0;
    while (!getDone())
    {
	if (done_count ||
	    (gsret = getStringZ(stringbuf, STRINGBUFLEN)) == 0 ||
	    gsret == BADSIZE)
	{
	    struct_disp_failed(match, var);
	    return;
	}
	if (*stringbuf == '*')
	{
	    char *eptr;
	    done_count = 1;
	    count = (int) strtol(stringbuf+1, &eptr, 0);
	    if (*eptr || count <= 0 || count >0x100)
	    {
		struct_disp_failed(match, var);
		return;
	    }
	}
	else if (done_type)
	{
	    struct_disp_failed(match, var);
	    return;
	}
	else
	{
	    char *ptr = stringbuf;
	    if (!strncmp(stringbuf, "struct", 6))
		struct_type = TYPE_STRUCT;
	    else if (!strncmp(stringbuf, "union", 5))
		struct_type = TYPE_UNION;
	    else if (!strncmp(stringbuf, "enum", 4))
		struct_type = TYPE_ENUM;
	    else
		struct_type = TYPE_NAMED;

	    if (struct_type != TYPE_NAMED)
	    {
		/* allow "struct thingy" as well as "struct" "thingy" */
		while (isalpha((unsigned int)*ptr))
		    ptr++;
		while (isspace((unsigned int)*ptr))
		    ptr++;
		if (!*ptr &&
		    ((gsret == getStringZ(ptr = stringbuf, STRINGBUFLEN))
		     == 0 || gsret == BADSIZE))
		{
		    struct_disp_failed(match, var);
		    return;
		}
	    }
	    struct_name = (char *)malloc(strlen(ptr)+1);
	    if (!struct_name)
	    {
		if (match)
		    free(match);
		free(var);
		return;
	    }
	    strcpy(struct_name, ptr);
	}
    }

    if (!struct_name)
    {
	if ((vp = find_var(TYPE_NAMED, var)) == NULL)
	{
	    if (*var == '$')
	    {
		vp = find_var(TYPE_NAMED, var+1);
	    }
	    if (vp == NULL)
	    {
		ui_output_ws("Can't automatically determine the type for %s\n",
			     var);
		if (match)
		    free(match);
		free(var);
		return;
	    }
	}
	ui_output_ws("Using type/variable declaration named %s\n", vp->name); 
    }
    else
	vp = find_var(struct_type, struct_name);

    bad = 0;
    if (vp == NULL)
    {
        ui_output_ws("Type not found.\n");
	bad = 1;
    }
    else if (!sym_d || !lookupByName(sym_d, var, &adr))
    {
	char *eptr;
	adr = strtoul(var, &eptr, 0);
	if (*eptr || eptr == var)
	{
	    ui_output_ws("Can't parse memory location.\n");
	    if (!sym_d)
		ui_output_ws("(Symbol table is not loaded.)");
	    bad = 1;
	}
    }

    if (bad)
    {
	if (match)
	    free(match);
	free(var);
	if (struct_name)
	    free(struct_name);
        return;
    }

#if 0
    if (!IS_DMEM(adr))
    {
	ui_output_ws("%08x is not in the data address space.\n", adr);
	return;
    }
#endif

    l = strlen(var);
    if (count > 1)
        l += 6;
    if (!set_message_size(l))
    {
	ui_output_ws("Error setting up for struct_disp (set_message_size).\n");
	return;
    }

    strcpy(message_buffer, var);
    for(i = 0; i < count; i++)
    {
	if (count > 1)
	    sprintf(message_buffer+strlen(var), "[%d]", i);
	adr += variable_addresses(strlen(message_buffer), vp, adr, init_depth, 0, match, -1);
    }

    return;
}


void struct_load(void)
{
    char stringbuf[STRINGBUFLEN];
    if (getDone() || getStringZ(stringbuf, STRINGBUFLEN) == BADSIZE ||
	!getDone())
    {
	/* No arguments - print syntax message and exit */
	printSyntax(" \"filename\"");
	return;
    }

    if (!parse_file(stringbuf))
    {
	ui_output_ws("Load failed.\n");
	return;
    }

    ui_output_ws("File loaded.\n");

    if (!sym_d)
    {
	int len = strlen(stringbuf);
	if (len > 3 && !strcmp(stringbuf+len-4, ".str"))
	{
	    FILE *testfile;
	    strcpy(stringbuf+len-4, ".sym");
	    if ((testfile = fopen(stringbuf, "r")))
	    {
		fclose(testfile);
		ui_output_ws("Attempting to load corresponding symbol table...\n");
		if (loadSymbolData(NULL, stringbuf))
		    ui_output_ws("...File loaded.\n");
	    }
	}
	if (!sym_d)
	    ui_output_ws("No symbol table found.  You may want to load it.\n");
    }
}


void struct_match(void)
{
    struct var_info *vp;
	int all;
    char stringbuf[STRINGBUFLEN];

    if (getDone() || getStringZ(stringbuf, STRINGBUFLEN) == BADSIZE ||
	!getDone())
    {
	/* No arguments - print syntax message and exit */
	printSyntax("\"name-to-match\"");
	return;
    }

    all = !strcmp(stringbuf, "_all_");

    for(vp = top_head; vp != NULL; vp = vp->next)
	if (all || strstr(vp->name, stringbuf) != NULL)
	{
	    switch(vp->type)
	    {
	    case TYPE_STRUCT:
		ui_output_ws("struct %s\n",vp->name);
		break;
	    case TYPE_ENUM:
		ui_output_ws("enum %s\n",vp->name);
		break;
	    case TYPE_UNION:
		ui_output_ws("union %s\n",vp->name);
		break;
	    default:
		ui_output_ws("%s: ", vp->name);
		summarise_variable(vp);
		ui_output_ws("\n");
		break;
	    }
	}

    return;
}

void struct_whatis(void)
{
    struct var_info *vp = NULL;
    int struct_type = -1, bad = 0;
    char stringbuf[STRINGBUFLEN];

    if (getStringZ(stringbuf, STRINGBUFLEN) == BADSIZE)
    {
	bad = 1;
    }
    else
    {
	char *ptr = stringbuf;
	if (!strncmp(stringbuf, "struct", 6))
	    struct_type = TYPE_STRUCT;
	else if (!strncmp(stringbuf, "union", 5))
	    struct_type = TYPE_UNION;
	else if (!strncmp(stringbuf, "enum", 4))
	    struct_type = TYPE_ENUM;
	else
	    struct_type = TYPE_NAMED;

	if (struct_type != TYPE_NAMED)
	{
	    /* allow "enum foo" as well as "enum" "foo" */
	    while (isalpha((unsigned int)*ptr))
		ptr++;
	    while (isspace((unsigned int)*ptr))
		ptr++;

	    if (!*ptr && getStringZ(ptr = stringbuf, STRINGBUFLEN) == BADSIZE)
		bad = 1;
	}

	if (!bad)
	    vp = find_var(struct_type, ptr);
    }

    if (bad)
    {
	/* No arguments - print syntax message and exit */
	printSyntax(" type-specifier");
	printSyntaxContin("type-specifier: typedef-name | struct|enum|union typename");
	return;
    }

    if (vp == NULL)
    {
        ui_output_ws("Type not found.\n");
        return;
    }

    dump_variables(vp);

    return;
}

void struct_depth(void)
{
    u32 depth;

    if (!get(16, &depth, NULL) || depth < 0)
    {
	printSyntax(" depth-of-pointers-to-dereference\n");
	return;
    }

    if (depth > 0)
        init_depth = depth;

    return;
}
