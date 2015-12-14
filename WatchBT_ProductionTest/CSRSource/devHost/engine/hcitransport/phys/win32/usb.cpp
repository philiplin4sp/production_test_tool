///////////////////////////////////////////////////////////////////////////////
//
//  FILE    :   usb.cpp
//
//  Copyright CSR 2001-2006
//
//  PURPOSE :   to implement the make function for USB.
//
///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "../csr_usb.h"
#include "../wid_usb.h"

template <class T> USBAbstraction * make_helper ( const T* aName , const USBConfiguration &aThis )
{
    switch ( aName[4] )
    {
    //  \\.\csrI
    case 'c':
    case 'C':
        return new USBStack ( aThis );
        break;
    //  \\.\btwusbI
    case 'b':
    case 'B':
        return new USBWidStack ( aThis );
        break;
    default:
		// Are we using the very long name?
		// \\?\usb#vid_0a12&pid_0001#6&25761c1&0&1#{f12d3cf8-b11d-457e-8641-be2af2d6d204}
		if (aName[0] == '\\' && aName[1] == '\\' && aName[2] == '?' && aName[3] == '\\')
			return new USBStack ( aThis );
        return 0;
    }
}

template USBAbstraction * make_helper<char> ( const char* , const USBConfiguration& );
template USBAbstraction * make_helper<wchar_t> ( const wchar_t* , const USBConfiguration& );

bool USBConfiguration::testOpen() const
{
    HANDLE lComport = INVALID_HANDLE_VALUE;
    if ( nameIsWide() )
        lComport = CreateFileW( getNameW() ,
		    GENERIC_READ|GENERIC_WRITE,
		    0, //exclusive access
		    0, //security
		    OPEN_EXISTING,
		    FILE_FLAG_OVERLAPPED,
		    0
		    ) ;
    else
        lComport = CreateFileA( getNameA() ,
		    GENERIC_READ|GENERIC_WRITE,
		    0, //exclusive access
		    0, //security
		    OPEN_EXISTING,
		    FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL,
		    0
		    ) ;
    DWORD i;
    if ( lComport != INVALID_HANDLE_VALUE )
        ::CloseHandle ( lComport );
    else
        i = ::GetLastError();
    return ( lComport != INVALID_HANDLE_VALUE );
}

