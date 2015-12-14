///////////////////////////////////////////////////////////////////////////////
//
//  FILE    :    name_store.cpp
//
//  Copyright CSR 2001-2006
//
//  CLASS   :    name_store
//
//  PURPOSE :    to implement an string type which is dually char and 
//               wchar_t, both at the same time in terms of typing, but not
//               data.  The class contains two strings one of which is always
//               empty.
//
///////////////////////////////////////////////////////////////////////////////

#include "name_store.h"
#include <string.h>
#include <wchar.h>
#include <assert.h>

static char * clone ( const char * aString )
{
    if ( aString )
        return strcpy ( new char   [strlen (aString) + 1] , aString );
    else
        return 0;
}

static wchar_t * clone ( const wchar_t * aString )
{
    if ( aString )
        return wcscpy ( new wchar_t[wcslen (aString) + 1] , aString );
    else
        return 0;
}

name_store::name_store ( const char * const aString )
: mStringA ( clone (aString) ) ,
  mStringW ( 0 )
{
    assert ( aString );
}

name_store::name_store ( const wchar_t * const aString )
: mStringA ( 0 ) ,
  mStringW ( clone (aString) )
{
    assert ( aString );
}

name_store::~name_store ()
{
    delete[] mStringA;
    delete[] mStringW;
}

name_store::name_store ( const name_store & aFrom )
: mStringA ( clone (aFrom.mStringA) ) ,
  mStringW ( clone (aFrom.mStringW) )
{
}

const char * name_store::getA () const
{
    assert ( mStringA );
    return mStringA;
}

const wchar_t * name_store::getW () const
{
    assert ( mStringW );
    return mStringW;
}

void name_store::get ( const char *& aString ) const
{
    aString = getA();
}

void name_store::get ( const wchar_t *& aString ) const
{
    aString = getW();
}

/*
distring & distring::operator= ( const distring & aFrom )
{
    abort();
    return *this;
}
*/
