// $Id: //depot/bc/bluesuite_2_4/devHost/common/ce_platform_mappings.h#1 $
// Maps EVC-generated platform names to usable defines.
/*
   $Log: ce_platform_mappings.h,v $
   Revision 1.2  2001/06/05 17:07:38  cl01
   Checked flags.

   Revision 1.1  2001/05/22 15:19:50  cl01
   Reworking for CE/Win32 merge


*/

#ifndef CE_PLATFORM_MAPPINGS_INCLUDED
#define CE_PLATFORM_MAPPINGS_INCLUDED

#ifdef WIN32_PLATFORM_HPC2000
#define _HPC
#endif

#ifdef WIN32_PLATFORM_HPCPRO
#define _HPCPRO
#endif

#ifdef WIN32_PLATFORM_PSPC
#define _PPC
#endif

#endif // CE_PLATFORM_MAPPINGS_INCLUDED
