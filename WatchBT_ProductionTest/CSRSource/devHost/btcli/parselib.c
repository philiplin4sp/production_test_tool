// Copyright (C) 2000-2006 Cambridge Silicon Radio Ltd.; all rights reserved.
// Generic command line parsing support.
// $Id: //depot/bc/bluesuite_2_4/devHost/btcli/parselib.c#1 $


#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#if !defined(_WINCE) && !defined(_WIN32_WCE)
#include <errno.h>
#include <assert.h>
#define ASSERT assert
#else
#include "wince/wincefuncs.h"
#endif

#include "parselib.h"
#include "commandtable.h"
#include "constants.h"		/* for NOHANDLE */
#include "vars.h"		/* for findVar () */
#include "globals.h"


/*
 * The parse cursor.
 * Normally points at start of current token (or end of line).
 * Any routine which fails to accept a token leaves the cursor unchanged.
 * Any routine which accepts a token moves the cursor to the start of
 * the next token (or end of line).
 */
static const char *parsepos;

#define isspaceornul(x) (isspace (x) || (x) == NUL)
#define iscrornl(x) ((x) == '\r' || (x) == '\n')
#define isvalidinvariable(x) (isalnum (x) || (x) == '_')
#define hexvalue(x) ((x) >= 'a' ? (x) - 'a' + 10 : (x) <= '9' ? (x) - '0' : (x) - 'A' + 10) /* OK, so this assumes ASCII -- sue me */

typedef const char *parse_state;

/*
 * Move cursor to start of next token (or end of line).
 */
static void findNextToken (void)
{
	while (isspace ((unsigned char) *parsepos))
	{
		++parsepos;
	}
}

/*
 * back up the parse state so we can restore it later.
 * i.e:
 *     backupParseState(&state);
 *     if (getKeyword("hello")) {
 *         if (!getKeyword("world")) {
 *             restoreParseState(&state); // if we failed to pop "world", pretend we didn't pop "hello"
 *         }
 *     }
 */
static void backupParseState(parse_state *p_state)
{
	*p_state = parsepos;
}

/*
 * restore a previously backed up parse state.
 */
static restoreParseState(const parse_state *p_state)
{
	parsepos = *p_state;
}

bool getDone (void)
{
	return *parsepos == NUL;
}


/*
 * Return the next token in a malloced buffer.
 * Note strings are not handled, so will be cut short if they contain spaces.
 */
static char *getToken (void)
{
	const char *endptr = parsepos;
	char *token;
	size_t len;

	while (!isspaceornul ((unsigned char) *endptr) && (unsigned char) *endptr != '\n')
	{
		++endptr;
	}

	len = endptr - parsepos;
	token = malloc (len + 1);
	ASSERT (token != NULL);

	memcpy (token, parsepos, len);
	token[len] = NUL;

	return token;
}

/*
 * Return whether the next token is the specific word given
 */
bool getKeyword (const char *word)
{
	char *token = getToken();
	bool found = strcmp(token, word) == 0;

	if(found)
	{
		parsepos += strlen(word);
		findNextToken ();
	}

	free (token);

	return found;
}

/*
 * Try to match current token in dictionary
 * (*result not modified if no match).
 */
static bool getByName (Dictionary dictionary, u32 *result)
{
	char *token = getToken ();
	bool found = lookupByName (dictionary, token, result);

	if (found)
	{
		parsepos += strlen (token);
		findNextToken ();
	}

	free (token);

	return found;
}


bool getCommandIndex (size_t *index)
{
	size_t i, n = getNumCommands ();
	const char *name;
	char *token = getToken ();

	for (i = 0; i < n; ++i)
	{
		name = getCommandAbbNam (i);
		if (strcmp (token, name) == 0)
		{
			break;
		}
		name = getCommandFullName (i);
		if (strcmp (token, name) == 0)
		{
			break;
		}
	}

	free (token);
	if (i == n)
	{
		return false;
	}

	parsepos += strlen (name);
	findNextToken ();

	*index = i;
	return true;
}


/*
 * Try to match current token to hex, binary, octal or decimal value
 * of given maximum size (in bits).
 */
static bool getNumeric (int nbits, u32 *result)
{
	int offset, base;
	const char *endptr;

	ASSERT (nbits <= 32);
	ASSERT (nbits >= 0);

	if (parsepos[0] == '0' && (parsepos[1] == 'x' || parsepos[1] == 'X'))
	{
		offset = 2;
		base = 16;
	}
	else if (parsepos[0] == '0' && (parsepos[1] == 'b' || parsepos[1] == 'B'))
	{
		offset = 2;
		base = 2;
	}
	else if (parsepos[0] == '0')
	{
		offset = 0;
		base = 8;
	}
	else
	{
		offset = 0;
		base = 10;
	}
#ifndef _WIN32_WCE
	errno = 0;
#endif
	*result = strtoul (parsepos + offset, (char **) &endptr, base);
#ifndef _WIN32_WCE
	if (errno != 0) /* Typically ERANGE, but whatever... */
	{
		return false;
	}
#endif
	if (endptr == parsepos + offset || !isspaceornul ((unsigned char) *endptr))
	{
		return false;
	}

	if (nbits < 32 && (*result >> nbits) != 0)
	{
		return false;
	}

	parsepos = endptr;
	findNextToken ();

	return true;
}


char *getVar (void)
{
	char *buf;
	size_t n, ntot;

	if (!isvalidinvariable ((unsigned char) *parsepos))
	{
		return NULL;
	}

	n = 0;
	ntot = 10;
	buf = malloc (ntot);
	ASSERT (buf != NULL);
	while (isvalidinvariable ((unsigned char) *parsepos))
	{
		buf[n++] = *parsepos++;
		if (n == ntot)
		{
			ntot += 10;
			buf = realloc (buf, ntot);
			ASSERT (buf != NULL);
		}
	}
	buf[n] = NUL;

	findNextToken ();

	return buf;
}


char *getRestOfLine (void)
{
	char *buf, *bend;

	buf = malloc (strlen (parsepos) + 1);
	ASSERT (buf != NULL);

	strcpy (buf, parsepos);

	if (*buf != NUL)
	{
		bend = buf + strlen (buf) - 1;
		while (iscrornl (*bend))
		{
			*bend = NUL;
			if (bend == buf)
			{
				break;
			}
			--bend;
		}
	}

	parsepos += strlen (parsepos);

	return buf;
}


bool getEquals (void)
{
	if (*parsepos == '=')
	{
		++parsepos;
		while (iscrornl (*parsepos))
		{
			++parsepos;
		}
		return true;
	}
	return false;
}


static size_t getStringInternal (char *buf, size_t n, bool slashEscapes)
{
	/*
	 * If slashEscapes is false, then the only slash escapes
	 * which are recognised are \\, \" and \^; a slash followed
	 * by any other character is not treated as an escape.
	 */

	const char *endptr;
	char c;
	size_t len = 0;
	bool hat = false, slash = false;

	if (*parsepos != '"')
	{
		return BADSIZE;
	}

	endptr = parsepos + 1;
	while ((*endptr != '"' || slash) && *endptr != NUL && n != 0)
	{
		c = *endptr++;
		if (hat)
		{
			if (c == '?')
			{
				c = 127;
			}
			else if (c >= '@' && c <= '_')
			{
				c -= '@'; /* Assumes ASCII... */
			}
			else
			{
				return BADSIZE;
			}
			hat = false;
		}
		else if (slash)
		{
			switch (c)
			{
			case '\\':
			case '"':
			case '^':
				break;
			case 'r':
				if (slashEscapes)
				{
					c = 13;
				}
				else if (n == 1)
				{
					return BADSIZE;
				}
				else
				{
					*buf++ = '\\';
					--n;
					++len;
				}
				break;
			case 'n':
				if (slashEscapes)
				{
					c = 10;
				}
				else if (n == 1)
				{
					return BADSIZE;
				}
				else
				{
					*buf++ = '\\';
					--n;
					++len;
				}
				break;
			case 't':
				if (slashEscapes)
				{
					c = 9;
				}
				else if (n == 1)
				{
					return BADSIZE;
				}
				else
				{
					*buf++ = '\\';
					--n;
					++len;
				}
				break;
			case '0':
				if (slashEscapes)
				{
					c = 0;
				}
				else if (n == 1)
				{
					return BADSIZE;
				}
				else
				{
					*buf++ = '\\';
					--n;
					++len;
				}
				break;
			case 'x':
				if (slashEscapes)
				{
					if (!isxdigit ((unsigned char) endptr[0]) || !isxdigit ((unsigned char) endptr[1]))
					{
						return BADSIZE;
					}
					else
					{
						c = hexvalue (*endptr) << 4;
						++endptr;
						c += hexvalue (*endptr);
						++endptr;
					}
				}
				else if (n == 1)
				{
					return BADSIZE;
				}
				else
				{
					*buf++ = '\\';
					--n;
					++len;
				}
				break;
			default:
				if (slashEscapes)
				{
					return BADSIZE;
				}
				else if (n == 1)
				{
					return BADSIZE;
				}
				else
				{
					*buf++ = '\\';
					--n;
					++len;
				}
				break;
			}
			slash = false;
		}
		else if (c == '^')
		{
			hat = true;
			continue;
		}
		else if (c == '\\')
		{
			slash = true;
			continue;
		}
		*buf++ = c;
		--n;
		++len;
	}

	if (*endptr == NUL || hat || slash)
	{
		return BADSIZE;
	}

	parsepos = endptr + 1;
	findNextToken ();

	return len; /* So NOT NUL-terminated */
}


size_t getString (char *buf, size_t n)
{
	return getStringInternal (buf, n, true);
}


size_t getStringZ (char *buf, size_t n)
{
	size_t len;

	ASSERT (n != 0);

	len = getStringInternal (buf, n - 1, false);

	if (len != BADSIZE)
	{
		buf[len] = NUL;
	}

	return len;
}


size_t getLongHex (u8 *buf, size_t n)
{
	const char *startptr, *endptr;
	u8 o;

	memset (buf, 0, n);

	/* Allow single-digit decimal (else "0" annoyingly gets rejected) */
	if (isdigit ((unsigned char) parsepos[0]) && isspaceornul ((unsigned char) parsepos[1]))
	{
		if (n != 0)
		{
			buf[0] = parsepos[0] - '0';

			parsepos += 2;
			findNextToken ();

			return 1;
		}
		else
		{
			return 0;
		}
	}

	if (parsepos[0] != '0' ||
		(parsepos[1] != 'x' && parsepos[1] != 'X'))
	{
		return 0;
	}

	endptr = parsepos + 2;
	while (isxdigit ((unsigned char) *endptr))
	{
		++endptr;
	}

	if (!isspaceornul ((unsigned char) *endptr))
	{
		return 0;
	}

	if ((size_t) (endptr - parsepos) > n * 2 + 2 || endptr == parsepos + 2)
	{
		return 0;
	}

	startptr = parsepos + 1; /* Points at 'x' */
	parsepos = endptr;
	--endptr;
	while (endptr != startptr)
	{
		o = hexvalue (*endptr);
		--endptr;
		if (endptr != startptr)
		{
			o = o | (hexvalue (*endptr) << 4);
			--endptr;
		}
		*buf++ = o;
	}

	n = (parsepos - startptr) / 2;
	findNextToken ();

	return n;
}


bool get (int nbits, u32 *result, Dictionary dictionary)
{
	ASSERT (nbits <= 32);
	ASSERT (nbits >= 0);

	if (dictionary && getByName (dictionary, result))
	{
		return true;
	}

	if (nbits == 0)
	{
		return false;
	}

	return getNumeric (nbits, result);
}

/*********************************************************
 * try to get a single number or N * M
 *********************************************************/
bool getNumericMultiple(int nbits, u32 *result, u32 *multiplicity)
{
	bool ret = false;
	if (getNumeric(nbits, result))
	{
		parse_state state;
		*multiplicity = 1;
		backupParseState(&state);
		if (getKeyword("*"))
		{
			u32 temp_multiplicity;
			if (getNumeric(32, &temp_multiplicity))
			{
				*multiplicity = temp_multiplicity;
			}
			else
			{
				// put the * keyword back.
				restoreParseState(&state);
			}
		}
		// not getting a multiplicity is not a failure, so still success as long as we got the single number.
		ret = true;
	}
	return ret;
}

static Dictionary last_d =
{
	1,		"last",
	1,		"l",
	0,		NULL
};

bool getba (u32 *lap, u32 *uap, u32 *nap)
{
	u8 ba[6];
	u32 dummy;

	if (get (0, &dummy, last_d))
	{
		*lap = lastlap;
		*uap = lastuap;
		*nap = lastnap;
		return true;
	}
	else if (!getLongHex (ba, sizeof (ba)))
	{
		return false;
	}

	*lap = ba[0] | (ba[1] << 8) | (ba[2] << 16);
	*uap = ba[3];
	*nap = ba[4] | (ba[5] << 8);

	return true;
}


/* Can't be static because of wait[n] () */ Dictionary aclch_d =
{
	0,		"acl0",
	1,		"acl1",
	2,		"acl2",
	3,		"acl3",
	4,		"acl4",
	5,		"acl5",
	6,		"acl6",
	7,		"acl7",
	8,		"acl8",
	9,		"acl9",
	10,             "acl10",
	11,             "acl11",
	12,             "acl12",
	13,             "acl13",
	14,             "acl14",
	15,             "acl15",
	16,             "acl16",
	17,             "acl17",
	0,		"a0",
	1,		"a1",
	2,		"a2",
	3,		"a3",
	4,		"a4",
	5,		"a5",
	6,		"a6",
	7,		"a7",
	8,		"a8",
	9,		"a9",
	10,             "a10",
	11,             "a11",
	12,             "a12",
	13,             "a13",
	14,             "a14",
	15,             "a15",
	16,             "a16",
	17,             "a17",
	0,		NULL
};

/* Can't be static because of wait[n] () */ Dictionary scoch_d =
{
	0,		"sco0",
	1,		"sco1",
	2,		"sco2",
	3,		"sco3",
	4,		"sco4",
	5,		"sco5",
	6,		"sco6",
	7,		"sco7",
	8,		"sco8",
	9,		"sco9",
	0,		"s0",
	1,		"s1",
	2,		"s2",
	3,		"s3",
	4,		"s4",
	5,		"s5",
	6,		"s6",
	7,		"s7",
	8,		"s8",
	9,		"s9",
	0,		NULL
};

static Dictionary bch_d =
{
	0xab,		"ab",
	0xeb,		"pb",
	0,		NULL
};


bool getch (u32 *ch)
{
	u32 dummy;

	if (get (0, &dummy, bch_d))
	{
		*ch = dummy == 0xab ? abch : pbch;
		return true;
	}

	if (get (0, &dummy, aclch_d))
	{
		if (aclch[dummy] == NOHANDLE)
		{
			return false;
		}
		*ch = aclch[dummy];
		return true;
	}

	if (lastaclch != NOHANDLE && get (0, &dummy, last_d))
	{
		*ch = lastaclch;
		return true;
	}

	return get (16, ch, NULL);
}


bool getch_includeNOHANDLE (u32 *ch)
{
	u32 dummy;

	if (get (0, &dummy, bch_d))
	{
		*ch = dummy == 0xab ? abch : pbch;
		return true;
	}

	if (get (0, &dummy, aclch_d))
	{
		*ch = aclch[dummy];
		return true;
	}

	if (lastaclch != NOHANDLE && get (0, &dummy, last_d))
	{
		*ch = lastaclch;
		return true;
	}

	return get (16, ch, NULL);
}



/* Try to match current token to connection handle; return last SCO connection handle or ACL connection handle if requested and valid */
bool getsch (u32 *ch)
{
	u32 dummy;

	if (get (0, &dummy, bch_d))
	{
		*ch = dummy == 0xab ? abch : pbch;
		return true;
	}

	if (get (0, &dummy, scoch_d))
	{
		if (scoch[dummy] == NOHANDLE)
		{
			return false;
		}
		*ch = scoch[dummy];
		return true;
	}

	if (get (0, &dummy, aclch_d))
	{
		if (aclch[dummy] == NOHANDLE)
		{
			return false;
		}
		*ch = aclch[dummy];
		return true;
	}

	if ((lastscoch != NOHANDLE || lastaclch != NOHANDLE) && get (0, &dummy, last_d))
	{
		if (lastscoch != NOHANDLE)
		{
			*ch = lastscoch;
		}
		else
		{
			*ch = lastaclch;
		}
		return true;
	}

	return get (16, ch, NULL);
}


void startParse (const char *s)
{
	parsepos = s;

	findNextToken ();
}


bool substituteVars (char *sout, const char *sin, size_t n)
{
	char c, *var;
	const char *value;
	size_t varlen;

	ASSERT (n != 0);
	while ((c = *sin++) != NUL && n > 1)
	{
		if (c != '$')
		{
			--n;
			*sout++ = c;
			continue;
		}

		if (*sin == '$')
		{
			--n;
			*sout++ = c;
			++sin;
			continue;
		}
		if (isspace ((unsigned char) *sin))
		{
			++sin;
			continue;
		}

		for (varlen = 0; isvalidinvariable ((unsigned char) sin[varlen]); ++varlen)
			;

		if (varlen == 0)
		{
			continue;
		}

		var = malloc (varlen + 1);
		ASSERT (var != NULL);
		memcpy (var, sin, varlen);
		sin += varlen;
		var[varlen] = NUL;
		value = findVar (var);
		free (var);

		if (value != NULL)
		{
			while ((c = *value++) != NUL && n > 1)
			{
				--n;
				*sout++ = c;
			}
		}
	}

	if (c != NUL)
	{
		return false;
	}

	*sout = NUL;
	return true;
}

void new_dynstr(struct dynstr *str, int start_length) 
{
	ASSERT(NULL != str);

	str->capacity = BUFFERSIZE;
	str->data = malloc(BUFFERSIZE);
	str->data[0] = NUL;
	str->length = start_length;

	ASSERT(NULL != str->data);
}

void delete_dynstr(struct dynstr *str)
{
	ASSERT(NULL != str);
    ASSERT(NULL != str->data);

	free(str->data);
	str->data = NULL;
}

static struct dynstr *check_and_resize(struct dynstr *buf)
{
	if (buf->length == buf->capacity)
	{
		/* resize string if capacity outgrown */
		buf->capacity += 100;
		buf->data = realloc (buf->data, buf->capacity);

		ASSERT (buf->data != NULL);
	}

	return buf;
}

struct dynstr *fgetsDynamic(struct dynstr *buf, FILE *stream)
{
	int c;
	bool eofonly = true;
	ASSERT (NULL != buf);
	ASSERT (NULL != buf->data);

	/* may need to be reimplemented using fgets */
	while ((c = fgetc(stream)) != EOF)
	{
		eofonly = false; /* there is data to read */
		buf = check_and_resize(buf);

		buf->data[buf->length++] = c;

		if ('\n' == c)
		{
			break;
		}
	}
    
        check_and_resize(buf);
	buf->data[buf->length] = NUL;

	if (eofonly) 
	{
		/* 
		 * no data was read; the first character found in the stream in this 
		 * instance was EOF.
		 */
		return NULL;
	}
	return buf;
}

size_t appendToDynamic (struct dynstr *buf, char *chars, int count)
{
	int c;
	ASSERT (NULL != buf);
	ASSERT (NULL != buf->data);

	/* may need to be reimplemented using fgets */
	while (0 != (c = *chars++) && 0 != count--)
	{
            buf = check_and_resize(buf);
            buf->data[buf->length++] = c;
	}
    
        check_and_resize(buf);
	buf->data[buf->length] = NUL;
        return buf->length;
}

static size_t getStringDynamicInternal (struct dynstr *buf, bool slashEscapes)
{
	/*
	 * If slashEscapes is false, then the only slash escapes
	 * which are recognised are \\, \" and \^; a slash followed
	 * by any other character is not treated as an escape.
	 */

	const char *endptr;
	char c;
	size_t len = 0;
	bool hat = false, slash = false;

	if (*parsepos != '"')
	{
		return BADSIZE;
	}

	endptr = parsepos + 1;
	while ((*endptr != '"' || slash) && *endptr != NUL)
	{
		c = *endptr++;
		if (hat)
		{
			if (c == '?')
			{
				c = 127;
			}
			else if (c >= '@' && c <= '_')
			{
				c -= '@'; /* Assumes ASCII... */
			}
			else
			{
				return BADSIZE;
			}
			hat = false;
		}
		else if (slash)
		{
			switch (c)
			{
			case '\\':
			case '"':
			case '^':
				break;
			case 'r':
				if (slashEscapes)
				{
					c = 13;
				}
				else
				{
					buf = check_and_resize (buf);
					buf->data[buf->length++] = '\\';
					++len;
				}
				break;
			case 'n':
				if (slashEscapes)
				{
					c = 10;
				}
				else
				{
					buf = check_and_resize (buf);
					buf->data[buf->length++] = '\\';
					++len;
				}
				break;
			case 't':
				if (slashEscapes)
				{
					c = 9;
				}
				else
				{
					buf = check_and_resize (buf);
					buf->data[buf->length++] = '\\';
					++len;
				}
				break;
			case '0':
				if (slashEscapes)
				{
					c = 0;
				}
				else
				{
					buf = check_and_resize (buf);
					buf->data[buf->length++] = '\\';
					++len;
				}
				break;
			case 'x':
				if (slashEscapes)
				{
					if (!isxdigit ((unsigned char) endptr[0]) || !isxdigit ((unsigned char) endptr[1]))
					{
						return BADSIZE;
					}
					else
					{
						c = hexvalue (*endptr) << 4;
						++endptr;
						c += hexvalue (*endptr);
						++endptr;
					}
				}
				else
				{
					buf = check_and_resize (buf);
					buf->data[buf->length++] = '\\';
					++len;
				}
				break;
			default:
				if (slashEscapes)
				{
					return BADSIZE;
				}
				else
				{
					buf = check_and_resize (buf);
					buf->data[buf->length++] = '\\';
					++len;
				}
				break;
			}
			slash = false;
		}
		else if (c == '^')
		{
			hat = true;
			continue;
		}
		else if (c == '\\')
		{
			slash = true;
			continue;
		}
		buf = check_and_resize (buf);
		buf->data[buf->length++] = c;
		++len;
	}

	if (*endptr == NUL || hat || slash)
	{
		return BADSIZE;
	}

	parsepos = endptr + 1;
	findNextToken ();

	return len; /* So NOT NUL-terminated */
}

size_t getStringDynamicZ (struct dynstr *buf)
{
	size_t len;

	ASSERT (NULL != buf);
	ASSERT (NULL != buf->data);

	len = getStringDynamicInternal (buf, false);

	if (len != BADSIZE)
	{
		if (buf->capacity <= len)
		{
			buf->data = realloc (buf->data, buf->capacity = len + 100);
		}
		buf->data[len] = NUL;
	}

	return len;
}

size_t getStringDynamic (struct dynstr *buf)
{
	return getStringDynamicInternal (buf, true);
}

bool substituteVarsDynamic (struct dynstr *sout, const char *sin)
{
	char c, *var;
	const char *value;
	size_t varlen;

	while ((c = *sin++) != NUL)
	{
		if (c != '$')
		{
			sout = check_and_resize(sout);
			sout->data[sout->length++] = c;
			continue;
		}

		if (*sin == '$')
		{
			sout = check_and_resize(sout);
			sout->data[sout->length++] = c;
			++sin;
			continue;
		}
		if (isspace ((unsigned char) *sin))
		{
			++sin;
			continue;
		}

		for (varlen = 0; isvalidinvariable ((unsigned char) sin[varlen]); ++varlen)
			;

		if (varlen == 0)
		{
			continue;
		}

		var = malloc (varlen + 1);
		ASSERT (var != NULL);
		memcpy (var, sin, varlen);
		sin += varlen;
		var[varlen] = NUL;
		value = findVar (var);
		free (var);

		if (value != NULL)
		{
			while ((c = *value++) != NUL)
			{
				sout = check_and_resize(sout);
				sout->data[sout->length++] = c;
			}
		}
	}

	if (c != NUL)
	{
		return false;
	}
    check_and_resize(sout);
	sout->data[sout->length] = NUL;
	return true;
}

bool get_uint8_list_Dynamic(struct dynstr *list)
{
    uint32 list_element;
    uint8 list_element8;
    uint8 *list_uint8;

    ASSERT(NULL != list);
    ASSERT(NULL != list->data);

    while (!getDone())
    {
        if (!get(8, &list_element, NULL))
        {
            return false;
        }

        check_and_resize(list);
        list_uint8 = (uint8 *) list->data;
        list_element8 = (uint8) list_element;
        list_uint8[list->length++] = list_element8;
    }

    return true;
}
