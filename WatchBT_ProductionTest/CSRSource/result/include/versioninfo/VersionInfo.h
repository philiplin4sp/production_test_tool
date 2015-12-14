///////////////////////////////////////////////////////////////////////
//
//	File	: VersionInfo.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: VersionInfo
//
//	Purpose	: Provide access to the VS_VERSION_INFO information of an
//			  executable file image. The constructor must be passed
//			  the full relative filename of the executable file; this
//			  does not have to be the file currently executing.
//
//			  This class is implemented as a template instantiated for
//			  std::string and std::wstring only.
//
//			  The following member functions provide access to the
//			  top level entries:
//				GetValue			- The root block containing binary
//									  version information and flags.
//				GetFixedFileInfo	- Translation table of languages
//									  and code pages.
//				GetVarFileInfo		- Language specific string table
//									  entries.
//
//			  The following member functions provide access to the
//			  individual entries of the string file info, returning
//			  empty strings for failure:
//				GetCompanyName		- Company that produced the file,
//									  for example "Cambridge Silicon
//									  Radio Limited".
//				GetFileDescription	- File description to be presented
//									  to users.
//				GetFileVersion		- File version of the file.
//				GetInternalName		- The internal name of the file or
//									  module, or the original filename.
//				GetLegalCopyright	- The full text of all copyright
//									  notices that apply to the file.
//				GetOriginalFilename	- The original filename of the
//									  file, excluding path.
//				GetProductName		- The name of the product with
//									  which the file is distributed.
//				GetProductVersion	- The version of the product with
//									  which the file is distributed.
//				GetComments			- Additional information that
//									  could be displayed for
//									  diagnostic purposes.
//				GetLegalTrademarks	- The full text of all trademarks
//									  and registered trademarks that
//									  apply to the file.
//				GetPrivateBuild		- Information about a private
//									  version of the file. This is not
//									  normally present.
//				GetSpecialBuild		- How this file differs from the
//									  standard version. This is not
//									  normally present.
//
//			  By default the first language within the file is used,
//			  but this can be changed or checked using the following
//			  member functions based on the values returned by
//			  GetVarFileInfo:
//				SetLanguage			- Specify the language for
//									  subsequent Get operations. The
//									  value is not checked for
//									  validity.
//				GetLanguage			- Read the currently selected
//									  language.
//
//	Modification history:
//	
//		1.1		21:Sep:00	at	File created.
//		1.2		28:Sep:00	at	Changed loop incremenent for
//								consistency.
//		1.3		24:Oct:00	at	Corrected debugging macros.
//		1.4		27:Oct:00	at	Corrected termination of strings under
//								Windows 98.
//		1.5		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//
//		Moved from DFUWizard project and restarted revision numbering
//
//		1.1		25:jun:01	at	First stand-alone version.
//		1.2		03:jul:01	ckl	extra constructor - uses current module.
//		1.3		13:dec:01	ckl	Uses string rather than CString.
//		1.4		20:dec:01	at	Updated revision history.
//		1.5		11:feb:02	at	Removed ichar dependency.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/engine/versioninfo/VersionInfo.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header files once
#ifndef VERSIONINFO_H
#define VERSIONINFO_H

#if _MSC_VER > 1000
#pragma once
#endif

// Include library header files
#include <windows.h>
#include <vector>
#include <string>

// VersionInfo class
template<class string>
class VersionInfo
{
public:

	// Constructors
	VersionInfo();
	VersionInfo(const string &path);

	// Destructor
	virtual ~VersionInfo();

	// Accessor functions
	void *GetValue(const char *block, size_t *length) const;
	const VS_FIXEDFILEINFO *GetFixedFileInfo() const;
	const DWORD *GetVarFileInfo(size_t *entries) const;
	void GetVarFileInfo(std::vector<DWORD> &value) const;
	string GetStringFileInfo(DWORD language, const char *name) const;
	string GetCompanyName() const;
	string GetFileDescription() const;
	string GetFileVersion() const;
	string GetInternalName() const;
	string GetLegalCopyright() const;
	string GetOriginalFilename() const;
	string GetProductName() const;
	string GetProductVersion() const;
	string GetComments() const;
	string GetLegalTrademarks() const;
	string GetPrivateBuild() const;
	string GetSpecialBuild() const;

	// Language selection
	void SetLanguage(DWORD language);
	DWORD GetLanguage() const;

private:

	// Was initialisation successful
	bool initialised;

	// Raw version information
	DWORD versionInfoSize;
	void *versionInfo;

	// Current language and code page
	DWORD language;

	// Initialise for the named executable
	void Init(const string &path);

	// Coerce narrow character string
	static string Coerce(const char *s);

	// Narrow or wide API functions, with corrected const-ness
	static DWORD GetModuleFileNameI(HMODULE module, typename string::value_type *path,
	                                DWORD size);
	static DWORD GetFileVersionInfoSizeI(const typename string::value_type *path,
	                                     LPDWORD handle);
	static BOOL GetFileVersionInfoI(const typename string::value_type *path,
	                                DWORD handle, DWORD len, LPVOID data);
	static BOOL VerQueryValueI(const LPVOID block,
	                           const typename string::value_type *sub_block,
							   LPVOID *buffer, PUINT len);
};

#endif
