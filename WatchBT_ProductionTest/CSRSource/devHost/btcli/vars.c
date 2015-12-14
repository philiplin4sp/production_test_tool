// Copyright (C) 2000-2006 Cambridge Silicon Radio Ltd.; all rights reserved.
// Generic variable support.
// $Id: //depot/bc/bluesuite_2_4/devHost/btcli/vars.c#1 $


#include <stdlib.h>
#include <string.h>

#if !defined(_WINCE) && !defined(_WIN32_WCE)
#include <assert.h>
#define ASSERT assert
#else
#include "wince/wincefuncs.h"
#endif

#include "vars.h"
#include "print.h"


/* Implement the variable list as a linked list */
typedef struct VariableNode
{
	struct VariableNode *next;	/* NULL if last in list */
	const char *name;
	const char *value;
} VariableNode;

/* The variable list is empty when the head pointer is NULL */
static VariableNode *vars = NULL;


void printVar (const char *name)
{
	VariableNode *vn = vars;

	while (vn != NULL)
	{
		if (name == NULL || strcmp (vn->name, name) == 0)
		{
			printlf ("%s=%s\n", vn->name, vn->value);
		}
		vn = vn->next;
	}
}


void deleteVar (const char *name)
{
	VariableNode *vn = vars, *parent = NULL;

	while (vn != NULL)
	{
		if (strcmp (vn->name, name) == 0)
		{
			if (parent == NULL)
			{
				vars = vn->next;
			}
			else
			{
				parent->next = vn->next;
			}
			free ((void *) vn->name);
			free ((void *) vn->value);
			free (vn);
			break;
		}
		parent = vn;
		vn = vn->next;
	}
}


const char *findVar (const char *name)
{
	VariableNode *vn = vars;

	while (vn != NULL)
	{
		if (strcmp (vn->name, name) == 0)
		{
			return vn->value;
		}
		vn = vn->next;
	}

	return getenv (name);
}


/* Duplicate a string, if possible, else return NULL */
static char *sstrdup (const char *s)
{
	size_t len = strlen (s) + 1;
	char *s2;

	s2 = malloc (len);
	if (s2 != NULL)
	{
		memcpy (s2, s, len);
	}

	return s2;
}


void addVar (const char *name, const char *value)
{
	VariableNode *vn = vars;

	while (vn != NULL)
	{
		if (strcmp (vn->name, name) == 0)
		{
			free ((void *) vn->value);
			value = sstrdup (value);
			ASSERT (value != 0);
			vn->value = value;
			return;
		}
		vn = vn->next;
	}
	vn = malloc (sizeof (VariableNode));
	ASSERT (vn != NULL);
	name = sstrdup (name);
	ASSERT (name != NULL);
	value = sstrdup (value);
	ASSERT (value != NULL);
	vn->name = name;
	vn->value = value;
	vn->next = vars;
	vars = vn;
}
