///////////////////////////////////////////////////////////////////////
//
//	File	: VersionInfo.cpp
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
//		1.1		25:Jun:01	at	First stand-alone version.
//		1.2		03:Jul:01	ckl	Extra constructor - uses current module.
//		1.3		03:Jul:01	ckl	Fixed bug in string length.
//		1.4		24:Jul:01	at	Fixed bug in previous bug fix.
//		1.5		19:Nov:01	at	Added Unicode support.
//		1.6		13:Dec:01	ckl	Uses istring instead of CString.
//		1.7		20:Dec:01	at	Fixed bugs in istring changes.
//		1.8		11:feb:02	at	Removed ichar dependency.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/engine/versioninfo/VersionInfo.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "VersionInfo.h"

// Visual C++ debugging code
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#ifdef DEBUG_NEW
#define new DEBUG_NEW
#endif
#endif

// Instantiate for both narrow and wide strings
template class VersionInfo<std::string>;
template class VersionInfo<std::wstring>;

// Maximum filename length
static const size_t max_path_len = 5000;

// Components of block names
static const char block_fixed_file_info[] = "\\";
static const char block_var_file_info[] = "\\VarFileInfo\\Translation";
static const char block_string_file_info[] = "\\StringFileInfo\\%04X%04X\\%s";

// String names
static const char string_company_name[] = "CompanyName";
static const char string_file_description[] = "FileDescription";
static const char string_file_version[] = "FileVersion";
static const char string_internal_name[] = "InternalName";
static const char string_legal_copyright[] = "LegalCopyright";
static const char string_original_filename[] = "OriginalFilename";
static const char string_product_name[] = "ProductName";
static const char string_product_version[] = "ProductVersion";
static const char string_comments[] = "Comments";
static const char string_legal_trademarks[] = "LegalTrademarks";
static const char string_private_build[] = "PrivateBuild";
static const char string_special_build[] = "SpecialBuild";

// Constructors
template<class string>
VersionInfo<string>::VersionInfo()
{
	string::value_type path[max_path_len];
	int path_len = GetModuleFileNameI(NULL, path, max_path_len);
	Init(string(path, path_len));
}

template<class string>
VersionInfo<string>::VersionInfo(const string &path)
{
	Init(string(path));
}

// Initialise for the named executable
template<class string>
void VersionInfo<string>::Init(const string &path)
{
	// Read the version information
	initialised = false;
	DWORD zero;
	versionInfoSize = GetFileVersionInfoSizeI(path.c_str(), &zero);
	if (versionInfoSize)
	{
		versionInfo = new BYTE[versionInfoSize];
		if (GetFileVersionInfoI(path.c_str(), 0, versionInfoSize, versionInfo))
		{
			initialised = true;
			size_t entries;
			const DWORD *info = GetVarFileInfo(&entries);
			if (info && entries) language = *info;
			else initialised = false;
		}
	}
    else
        //  don't allow this to go into the destructor uninitialised!
        versionInfo = 0;
}

// Destructor
template<class string>
VersionInfo<string>::~VersionInfo()
{
	if (versionInfo) delete[] versionInfo;
}

// Accessor functions
template<class string>
void *VersionInfo<string>::GetValue(const char *block, size_t *length) const
{
	// Attempt to query the requested block
	LPVOID buffer;
	size_t bufferLength;
	if (!initialised
		|| !VerQueryValueI(versionInfo, Coerce(block).c_str(), &buffer,(PUINT) &bufferLength)
		|| (bufferLength == 0))
	{
		buffer = 0;
	}

	// Return the results
	if (length) *length = buffer ? bufferLength : 0;
	return buffer;
}

template<class string>
const VS_FIXEDFILEINFO *VersionInfo<string>::GetFixedFileInfo() const
{
	return static_cast<VS_FIXEDFILEINFO *>(GetValue(block_fixed_file_info, 0));
}

template<class string>
const DWORD *VersionInfo<string>::GetVarFileInfo(size_t *entries) const
{
	size_t length;
	DWORD *buffer = static_cast<DWORD *>(GetValue(block_var_file_info, &length));
	if (entries) *entries = buffer ? length / sizeof(DWORD) : 0;
	return buffer;
}

template<class string>
void VersionInfo<string>::GetVarFileInfo(std::vector<DWORD> &value) const
{
	// Obtain a pointer to the buffer
	size_t entries;
	const DWORD *info = GetVarFileInfo(&entries);

	// Copy the values to an array
	value.reserve(entries);
	for (size_t index = 0; index < entries; ++index)
	{
		value.push_back(info[index]);
	}
}

template<class string>
string VersionInfo<string>::GetStringFileInfo(DWORD language, const char *name) const
{
	std::vector<char> block(strlen(block_string_file_info) + strlen(name) + 1);
	sprintf(&block[0], block_string_file_info, language & 0xffff, language >> 16, name);
	size_t length;
	const string::value_type *value = (const string::value_type *) GetValue(&block[0], &length);
	return value ? string(value, length - 1) : string();
}

template <class string>
string VersionInfo<string>::GetCompanyName() const
{
	return GetStringFileInfo(language, string_company_name);
}

template <class string>
string VersionInfo<string>::GetFileDescription() const
{
	return GetStringFileInfo(language, string_file_description);
}

template <class string>
string VersionInfo<string>::GetFileVersion() const
{
	return GetStringFileInfo(language, string_file_version);
}

template <class string>
string VersionInfo<string>::GetInternalName() const
{
	return GetStringFileInfo(language, string_internal_name);
}

template <class string>
string VersionInfo<string>::GetLegalCopyright() const
{
	return GetStringFileInfo(language, string_legal_copyright);
}

template <class string>
string VersionInfo<string>::GetOriginalFilename() const
{
	return GetStringFileInfo(language, string_original_filename);
}

template <class string>
string VersionInfo<string>::GetProductName() const
{
	return GetStringFileInfo(language, string_product_name);
}

template <class string>
string VersionInfo<string>::GetProductVersion() const
{
	return GetStringFileInfo(language, string_product_version);
}

template <class string>
string VersionInfo<string>::GetComments() const
{
	return GetStringFileInfo(language, string_comments);
}

template <class string>
string VersionInfo<string>::GetLegalTrademarks() const
{
	return GetStringFileInfo(language, string_legal_trademarks);
}

template <class string>
string VersionInfo<string>::GetPrivateBuild() const
{
	return GetStringFileInfo(language, string_private_build);
}

template <class string>
string VersionInfo<string>::GetSpecialBuild() const
{
	return GetStringFileInfo(language, string_special_build);
}

// Language selection
template<class string>
void VersionInfo<string>::SetLanguage(DWORD language)
{
	VersionInfo::language = language;
}

template<class string>
DWORD VersionInfo<string>::GetLanguage() const
{
	return language;
}

// Coerce narrow character string
template<>
std::string VersionInfo<std::string>::Coerce(const char *s)
{
	return s;
}

template<>
std::wstring VersionInfo<std::wstring>::Coerce(const char *s)
{
    size_t need = mbstowcs(0, s, strlen(s));
    std::vector<wchar_t> buff(need);
    mbstowcs(&buff[0], s, strlen(s));
    return std::wstring(&buff[0], need);
}

// Narrow or wide API functions, with corrected const-ness
template<>
DWORD VersionInfo<std::string>::GetModuleFileNameI(HMODULE module, char *path, DWORD size)
{
	return ::GetModuleFileNameA(module, path, size);
}

template<>
DWORD VersionInfo<std::wstring>::GetModuleFileNameI(HMODULE module, wchar_t *path, DWORD size)
{
	return ::GetModuleFileNameW(module, path, size);
}

template<>
DWORD VersionInfo<std::string>::GetFileVersionInfoSizeI(const char *path, LPDWORD handle)
{
	return ::GetFileVersionInfoSizeA(const_cast<char *>(path), handle);
}

template<>
DWORD VersionInfo<std::wstring>::GetFileVersionInfoSizeI(const wchar_t *path, LPDWORD handle)
{
	return ::GetFileVersionInfoSizeW(const_cast<wchar_t *>(path), handle);
}

template<>
BOOL VersionInfo<std::string>::GetFileVersionInfoI(const char *path, DWORD handle, DWORD len, LPVOID data)
{
	return ::GetFileVersionInfoA(const_cast<char *>(path), handle, len, data);
}

template<>
BOOL VersionInfo<std::wstring>::GetFileVersionInfoI(const wchar_t *path, DWORD handle, DWORD len, LPVOID data)
{
	return ::GetFileVersionInfoW(const_cast<wchar_t *>(path), handle, len, data);
}

template<>
BOOL VersionInfo<std::string>::VerQueryValueI(const LPVOID block, const char *sub_block, LPVOID *buffer, PUINT len)
{
	return ::VerQueryValueA(block, const_cast<char *>(sub_block), buffer, len);
}

template<>
BOOL VersionInfo<std::wstring>::VerQueryValueI(const LPVOID block, const wchar_t *sub_block, LPVOID *buffer, PUINT len)
{
	return ::VerQueryValueW(block, const_cast<wchar_t *>(sub_block), buffer, len);
}
