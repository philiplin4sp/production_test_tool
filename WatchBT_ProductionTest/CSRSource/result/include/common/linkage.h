/* $Id: //depot/bc/bluesuite_2_4/devHost/common/linkage.h#1 $ */
/*
MODIFICATION HISTORY
25:apr:01	ckl	removed BCSP_API - now defined in BCSPStack.h 
			and StackCOnfiguration.h
*/

#ifndef LINKAGE_H

#define LINKAGE_H

#if !defined(WIN32) || defined(STATIC_LINKING)
#define DLL_API
#elif defined(_USRDLL)
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#ifdef __cplusplus
#define LINKAGE "C"
#else 
#define LINKAGE 
#endif

#ifdef WIN32
#define THREADRETURN unsigned int __stdcall
#else
#define THREADRETURN void *
#endif

#define EXPORTC  extern LINKAGE DLL_API 
#endif
