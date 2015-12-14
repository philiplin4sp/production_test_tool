////////////////////////////////////////////////////////////////////////////////
//
//  FILE    :    name_store.h
//
//  Copyright CSR 2001-2006
//
//  CLASS   :    name_store
//
//  PURPOSE :    to define a string type which is dually char
//               and wchar_t, both at the same time in terms of typing,
//               but not data.  Effectively it contains two strings one of
//               which is alway empty.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef HEADER_NAME_STORE_H_
#define HEADER_NAME_STORE_H_
#include <stdlib.h>

// name_store asserts that every pointer passed in or out is not null.
class name_store
{
public:
    name_store ( const char * const aString );
    name_store ( const wchar_t * const aString );
    virtual ~name_store ();
    name_store ( const name_store & );

    const char * getA () const;
    const wchar_t * getW () const;
    void get ( const char *& aString ) const;
    void get ( const wchar_t *& aString ) const;

    bool isA() const { return ( mStringA != 0 ); }
    bool isW() const { return ( mStringW != 0 ); }
private:
    name_store & operator= ( const name_store & );
    char * mStringA;
    wchar_t * mStringW;
};

#endif//HEADER_NAME_STORE_H_
