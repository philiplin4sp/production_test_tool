/*
 * MODIFICATION HISTORY
 *      1.1         12:mar:03  pws     Created: straight copy from spi/emucust.
 *      1.2         12:mar:03  pws     Remove strdup().
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <setjmp.h>

#if !defined(_WINCE) && !defined(_WIN32_WCE)
#include <ctype.h>
#include <errno.h>
#else
#include "wince/wincefuncs.h"
#endif

#ifndef sfree
#define sfree		free
#endif

#ifndef smalloc
#define smalloc		malloc
#endif

#define snew(x)		((x *) smalloc(sizeof(x)))

#include "struct-build.h"

int struct_error_log(char *fmt, ...);

#ifndef struct_error_log
int struct_error_log(char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    (void) vfprintf(stderr, fmt, ap);
    va_end(ap);

	return 0;
}
#endif   

static jmp_buf parse_abort;

void struct_perror(const char *message)
{
    char *err;
    if ((err = strerror(errno)) == NULL)
	err = "Unknown error";
    
    struct_error_log("%s: %s\n", message, err);
}

struct var_info *top_head = NULL;
static struct var_info *top_tail = NULL, *top_clean = NULL;

static int parse_variable(int (*)(void *), void *, struct var_info *, int);

void clean_var(void)
{
    struct var_info *vp, *vp2;

    for(vp = top_clean; vp != NULL; vp = vp2)
    {
	if (vp->name != NULL)
            sfree(vp->name);
        vp2 = vp->next_clean;
        sfree(vp);
    }

    top_clean = top_head = top_tail = NULL;
}

static struct var_info *new_var_info(void)
{
    struct var_info *vp;

    if ((vp = snew(struct var_info)) == NULL)
    {
        struct_perror("new_var_info");
        longjmp(parse_abort, 1);
    }

    vp->next_clean = top_clean;
    top_clean = vp;

    vp->name = NULL;
    vp->type = 0;
    vp->dimension = 0;
    vp->top_level = 0;
    vp->comp = NULL;
    vp->next = NULL;

    return vp;
}

#define TOKEN_BUFFER_SIZE 256

#define TOKEN_STRUCT   257
#define TOKEN_UNION    258
#define TOKEN_ARRAY    259
#define TOKEN_POINTER  260
#define TOKEN_FUNCTION 261
#define TOKEN_NAMED    262
#define TOKEN_VOID     263
#define TOKEN_CHAR     264
#define TOKEN_SHORT    265
#define TOKEN_INT      266
#define TOKEN_LONG     267
#define TOKEN_FLOAT    268
#define TOKEN_DOUBLE   269
#define TOKEN_SIGNED   270
#define TOKEN_UNSIGNED 271
#define TOKEN_INTEGER  272 /* Whole number 0.. */
#define TOKEN_ENUM     273
#define TOKEN_UNKNOWN  274

static int ungot_token = EOF;
static int token = EOF;
static int ungot_char = EOF;

static char token_buffer[TOKEN_BUFFER_SIZE];

static void unget_token()
{
    ungot_token = token;
}

static int get_char(int (*nextchar)(void *), void *param)
{
    int c;

    if (ungot_char != EOF)
    {
	c = ungot_char;
	ungot_char = EOF;
	return c;
    }

    return nextchar(param);
}

static void unget_char(int c)
{
    ungot_char = c;
}

static int get_token(int (*nextchar)(void *), void *param)
{
    int c, i;

    if (ungot_token != EOF)
    {
        token = ungot_token;
        ungot_token = EOF;
        return token;
    }


    do {
        c = get_char(nextchar, param);
    } while(isspace(c));
    if (c == EOF)
    {
        token = EOF;
        return token;
    }
    for(i = 0; i < TOKEN_BUFFER_SIZE-1 && c != EOF && !isspace(c);)
    {
        token_buffer[i++] = c;
        if (c == '[' || c == ']' || c == '{' || c == '}')
            break;
        c = get_char(nextchar, param);
        if (c == '[' || c == ']' || c == '{' || c == '}')
        {
            unget_char(c);
            break;
        }
    }
    if (i == 1 && ispunct((unsigned char)token_buffer[0]))
    {
        token = token_buffer[0];
        return token;
    }
    token_buffer[i] = '\0';

    if (!strcmp(token_buffer, "typedef"))
        token = TOKEN_NAMED;
    else if (!strcmp(token_buffer, "char"))
        token = TOKEN_CHAR;
    else if (!strcmp(token_buffer, "void"))
        token = TOKEN_VOID;
    else if (!strcmp(token_buffer, "short"))
        token = TOKEN_SHORT;
    else if (!strcmp(token_buffer, "int"))
        token = TOKEN_INT;
    else if (!strcmp(token_buffer, "long"))
        token = TOKEN_LONG;
    else if (!strcmp(token_buffer, "float"))
        token = TOKEN_FLOAT;
    else if (!strcmp(token_buffer, "double"))
        token = TOKEN_DOUBLE;
    else if (!strcmp(token_buffer, "signed"))
        token = TOKEN_SIGNED;
    else if (!strcmp(token_buffer, "unsigned"))
        token = TOKEN_UNSIGNED;
    else if (!strcmp(token_buffer, "struct"))
        token = TOKEN_STRUCT;
    else if (!strcmp(token_buffer, "union"))
        token = TOKEN_UNION;
    else if (!strcmp(token_buffer, "array"))
        token = TOKEN_ARRAY;
    else if (!strcmp(token_buffer, "pointer"))
        token = TOKEN_POINTER;
    else if (!strcmp(token_buffer, "function"))
        token = TOKEN_FUNCTION;
    else if (!strcmp(token_buffer, "enum"))
        token = TOKEN_ENUM;
    else if (strspn(token_buffer, "0123456789") == strlen(token_buffer) ||
	     ((*token_buffer == '-' || *token_buffer == '+') &&
	      strspn(token_buffer+1, "0123456789") == strlen(token_buffer)-1))
        token = TOKEN_INTEGER;
    else
        token = TOKEN_UNKNOWN;

    return token;
}

static char *xstrdup(const char *s)
{
    char *p;

    p = (char *)malloc(strlen(s)+1);
    if (p == NULL)
    {
        struct_perror("xstrdup");
        longjmp(parse_abort, 1);
    }

    strcpy(p, s);

    return p;
}

static void unexpected_token(char *state)
{
    struct_error_log("%s: ", state);

    if (token == EOF)
        struct_error_log("Unexpected end of file");
    else
    {
        struct_error_log("Unexpected token %d = ", token);
        if (token < 256)
        {
            if (isascii(token) && isprint(token))
                struct_error_log("'%c'", token);
            else
                struct_error_log("'\\%03o'", token);
        }
        else
            struct_error_log("\"%s\"", token_buffer);
    }

    struct_error_log(".\n");
    longjmp(parse_abort, 1);
}
        

static int parse_type(int (*nextchar)(void *), void *param)
{
    int type = 0;

    for(;;)
    {
        switch(get_token(nextchar, param))
        {
        case TOKEN_VOID:
            return type | TYPE_VOID;
        case TOKEN_CHAR:
            return type | TYPE_CHAR;
        case TOKEN_SHORT:
            type |= TYPE_SHORT;
            break;
        case TOKEN_INT:
            return type | TYPE_INT;
        case TOKEN_LONG:
            type |= TYPE_LONG;
            break;
        case TOKEN_FLOAT:
            return TYPE_FLOAT;
        case TOKEN_DOUBLE:
            return TYPE_DOUBLE;
        case TOKEN_SIGNED:
            type |= TYPE_SIGNED;
            break;
        case TOKEN_UNSIGNED:
            type |= TYPE_UNSIGNED;
            break;
        case TOKEN_NAMED:
            return TYPE_NAMED;
        case TOKEN_STRUCT:
            return TYPE_STRUCT;
        case TOKEN_UNION:
            return TYPE_UNION;
        case TOKEN_ARRAY:
            return TYPE_ARRAY;
        case TOKEN_POINTER:
            return TYPE_POINTER;
        case TOKEN_FUNCTION:
            return TYPE_FUNCTION;
        case TOKEN_ENUM:
            return TYPE_ENUM;
	case TOKEN_INTEGER:
	    return TYPE_VALUE;
        default:
            if (type == 0)
                unexpected_token("parse_type, expecting type");
            unget_token();
            return type |= TYPE_INT;
        }
    }
}

static int parse_array(int (*nextchar)(void *), void *param)
{
    int i;

    if (get_token(nextchar, param) != '[')
        unexpected_token("parse_array, expecting '['");
    switch(get_token(nextchar, param))
    {
    case ']':
        return 0;
    case TOKEN_INTEGER:
        i = atoi(token_buffer);
        if (get_token(nextchar, param) != ']')
            unexpected_token("parse_array, expecting ']'");
        return i;
    default:
        unexpected_token("parse_array, expecting array length");
        return 0;
    }
}

static void delete_var(struct var_info *var)
{
	struct var_info **vpp;

	for(vpp = &top_clean; *vpp != var; vpp = &((*vpp)->next_clean))
		;

	*vpp = var->next_clean;
	sfree(var);
}

struct var_info *find_var(int type, char *name)
{
    struct var_info *vp;

    for(vp = top_head; vp != NULL; vp = vp->next)
        if ((vp->type & TYPE_COMPOUND) == type && vp->name != NULL &&
	    !strcmp(vp->name, name))
            return vp;

    return NULL;
}

int find_enum_symbol(char *symbol, long *value)
{
    struct var_info *vp, *vp2;

    for(vp = top_head; vp != NULL; vp = vp->next)
        if ((vp->type & TYPE_COMPOUND) == TYPE_ENUM)
	    for(vp2 = vp->comp; vp2 != NULL; vp2 = vp2->next)
	    {
		if ((vp2->type & TYPE_BASIC) == TYPE_VALUE &&
		    !strcmp(vp2->name, symbol))
		{
		    *value = vp2->dimension;
		    return 1;
		}
	    }

    return 0;
}

static struct var_info *find_or_create(int type, char *name)
{
    struct var_info *vp;

    if ((vp = find_var(type, name)) != NULL)
        return vp;

    vp = new_var_info();
    vp->type = type;
    vp->name = xstrdup(name);
    vp->top_level = 1;


    if (top_head == NULL)
	top_head = vp;
    else
	top_tail->next = vp;

    top_tail = vp;

    return vp;
}

static struct var_info *new_parse_variable(int (*nextchar)(void *),
					   void *param, int named)
{
    struct var_info *vp;

    vp = new_var_info();
    if (parse_variable(nextchar, param, vp, named))
        return vp;

    delete_var(vp);
    return NULL;
}

static void parse_compound(int (*nextchar)(void *), void *param,
			   struct var_info *vp)
{
    struct var_info *tail;

    if (get_token(nextchar, param) != '{')
    {
        unget_token();
        return;
    }

    if (vp->comp != NULL)
    {
        struct_error_log("Multiple definitions of %s.\n", vp->name);
        longjmp(parse_abort, 1);
    }

    vp->comp = tail = new_parse_variable(nextchar, param, 1);

    while(tail != NULL)
    {
        tail->next = new_parse_variable(nextchar, param, 1);
        tail = tail->next;
    }
}
        
static int parse_variable(int (*nextchar)(void *), void *param,
			  struct var_info *vp, int named)
{
    int type;

    if (named)
    {
        switch(get_token(nextchar, param))
        {
        case '}':
        case EOF:
            return 0;
	case TOKEN_STRUCT:
	case TOKEN_UNION:
	case TOKEN_ENUM:
	    vp->name = NULL;
	    unget_token();
	    break;
	case ':':
	    vp->name = xstrdup("$$padding$$");
	    unget_token();
	    break;
        case TOKEN_ARRAY:
        case TOKEN_POINTER:
        case TOKEN_FUNCTION:
        case TOKEN_UNKNOWN:
	    vp->name = xstrdup(token_buffer);
            break;
        default:
            unexpected_token("parse_variable, expecting type");
            return 0;
        }
    }

    switch(get_token(nextchar, param))
    {
    case ':':
	if (get_token(nextchar, param) != TOKEN_INTEGER)
	{
	    unexpected_token("parse_variable, expecting bit-field length");
	    return 0;
	}
	vp->dimension = atoi(token_buffer);
	break;
    default:
	unget_token();
	break;
    }

    switch((vp->type = parse_type(nextchar, param)) & TYPE_BASIC)
    {
    case TYPE_VOID:
    case TYPE_CHAR:
    case TYPE_INT:
    case TYPE_FLOAT:
    case TYPE_DOUBLE:
        break;
    case TYPE_VALUE:
        vp->dimension = atoi(token_buffer);
	break;
    case TYPE_ARRAY:
        vp->dimension = parse_array(nextchar, param);
        /* FALLTHRU */
    case TYPE_POINTER:
    case TYPE_FUNCTION:
        vp->comp = new_parse_variable(nextchar, param, 0);
        break;
    case TYPE_NAMED:
        get_token(nextchar, param);
        vp->comp = find_or_create(vp->type, token_buffer);
        break;
    case TYPE_STRUCT:
    case TYPE_ENUM:
    case TYPE_UNION:
        type = vp->type;
        vp->type = TYPE_NAMED;
        if (get_token(nextchar, param) == '{')
        {
            vp->comp = new_var_info();
            unget_token();
        }
        else
            vp->comp = find_or_create(type, token_buffer);
        vp->comp->type = type;
        parse_compound(nextchar, param, vp->comp);
        break;
    }

    return 1;
}
        
int parse_cont(int (*nextchar)(void *), void *param)
{
    struct var_info *vp, *vp2;

    if (setjmp(parse_abort))
    {
        clean_var();
        return -1;
    }

    if ((vp = new_parse_variable(nextchar, param, 1)) == NULL)
	return 0;

    if (vp->name == NULL)
    {
	delete_var(vp);
	return 1;
    }

    vp->top_level = 1;
	
    if (top_head == NULL)
    {
	top_head = top_tail = vp;
    }
    else
    {
        if ((vp2 = find_var(vp->type & TYPE_COMPOUND, vp->name)) != NULL)
        {
            sfree(vp2->name);
            vp->next = vp2->next;
	    vp->next_clean = vp2->next_clean;
            memcpy(vp2, vp, sizeof(struct var_info));
            delete_var(vp);
        }
        else
        {
            top_tail->next = vp;
            top_tail = vp;
        }
    }

    return top_tail != NULL;
}

unsigned long variable_sizeof(struct var_info *vp)
{
    unsigned long size, i;
    int bits;

    if (vp == NULL)
        return 0;
    
    switch(vp->type & TYPE_BASIC)
    {
    case TYPE_NAMED:
        return variable_sizeof(vp->comp);
    case TYPE_STRUCT:
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
		size += variable_sizeof(vp);
	    }
	    else
	    {
		bits += vp->dimension;
		if (bits > 16)
		{
		    size += 1 + (vp->dimension>>4);
		    bits = vp->dimension & 15;
		}
		else if (bits == 16)
		{
		    size++;
		    bits = 0;
		}
	    }
	}
	if (bits > 0)
	    size++;
        return size;
    case TYPE_UNION:
        size = 0;
        for(vp = vp->comp; vp != NULL; vp = vp->next)
        {
            i = variable_sizeof(vp);
            if (i > size)
                size = i;
        }
        return size;
    case TYPE_ENUM:
        return 1;
    case TYPE_POINTER:
        if (vp->comp->type == TYPE_FUNCTION)
            return 2;
        else
            return 1;
    case TYPE_ARRAY:
        return variable_sizeof(vp->comp) * vp->dimension;
    case TYPE_FUNCTION:
        return 0;
    case TYPE_VOID:
        return 0;
    case TYPE_CHAR:
	return 1;
    case TYPE_INT:
        if (vp->type & TYPE_LONG)
            return 2;
        else
            return 1;
    case TYPE_FLOAT:
        return 2;
    case TYPE_DOUBLE:
        return 4;
    }

    return 0;
}   

