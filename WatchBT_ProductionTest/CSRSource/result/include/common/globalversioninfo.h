/*
   Company-wide version information

   Restrict text characters to ASCII subset. Otherwise conversion from resource text codepage to
   console codepage will be required.
*/
#ifndef STATICVERSIONINFO_H
#define STATICVERSIONINFO_H

#include "common/dynamicversioninfo.h"

/* Company-wide strings */
#define VERSION_SHORT_COMPANY_NAME        							"CSR plc."
#define VERSION_LONG_COMPANY_NAME         							"Cambridge Silicon Radio Ltd."
#define VERSION_COPYRIGHT_NO_YEAR									"Copyright (C) " VERSION_YEAR ", " VERSION_LONG_COMPANY_NAME
#define VERSION_COPYRIGHT_START_YEAR(startyear)   					"Copyright (C) " startyear "-" VERSION_YEAR ", " VERSION_LONG_COMPANY_NAME

/* Product names */
#define VERSION_PRODUCT_NAME_BLUESUITE								"BlueSuite"

#endif /*STATICVERSIONINFO_H*/
