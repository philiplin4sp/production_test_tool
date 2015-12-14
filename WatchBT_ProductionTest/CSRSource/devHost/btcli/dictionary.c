// Copyright (C) 2000-2006 Cambridge Silicon Radio Ltd.; all rights reserved.
// Generic dictionary support.
// $Id: //depot/bc/bluesuite_2_4/devHost/btcli/dictionary.c#1 $


#include <string.h>

#include "dictionary.h"
#include "print.h"


const char *lookupByValue (const Dictionary dictionary, u32 value)
{
	size_t i;
	const char *name;

	for (i = 0; ; ++i)
	{
		name = dictionary[i].name;
		if (name == NULL)
		{
			return NULL;
		}
		if (dictionary[i].value == value)
		{
			break;
		}
	}

	return name;
}

u32 getMax (const Dictionary dictionary)
{
	size_t i;
	u32 maxVal = 0;

	for (i = 0; ; ++i)
	{
		if (dictionary[i].name)
		{
			if (dictionary[i].value > maxVal)
			{
				maxVal = dictionary[i].value;
			}
		}
		else
		{
			break;
		}
	}

	return maxVal;
}



bool lookupByName (const Dictionary dictionary, const char *name, u32 *value)
{
	size_t i;
	const char *dictname;

	for (i = 0; ; ++i)
	{
		dictname = dictionary[i].name;
		if (dictname == NULL)
		{
			return false;
		}
		if (strcmp (name, dictname) == 0)
		{
			*value = dictionary[i].value;
			break;
		}
	}

	return true;
}


bool printMatchByName (const Dictionary dictionary, const char *name)
{
	bool found = false;
	size_t i;
	const char *dictname;

	for (i = 0; ; ++i)
	{
		dictname = dictionary[i].name;
		if (dictname == NULL)
		{
			break;
		}
		if (strstr (dictname, name) != NULL)
		{
			found = true;
			printlf ("%s (0x%04lx)\n", dictname, (ul) dictionary[i].value);
		}
	}

	return found;
}
