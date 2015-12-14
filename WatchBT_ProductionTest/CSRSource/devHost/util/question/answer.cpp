////////////////////////////////////////////////////////////////////////////////
//
//  FILE:        answer.cpp
//
//  CLASSES:     Answer
//
//  PURPOSE:     Define a class for describing answers to Questions (see
//               question.h)
//
////////////////////////////////////////////////////////////////////////////////

#include "answer.h"

#pragma warning ( disable: 4786 )
#include <algorithm>
#ifndef _WIN32_WCE
#include <iostream>
#endif

const char starter   = '[';
const char ender     = ']';
const char delimiter = '|';

#ifdef _WIN32_WCE
Answer deserialise(const std::string &is)
{
    Answer result;
    if ( is.size() && is[0] == starter )
    {
        const char * c = is.c_str();
        // discard the [
        c++;
        bool done = false;
        result.push_back ( "" );
        Answer::iterator i = result.begin();
        while ( *c && !done )
        {
            switch ( *c )
            {
            case ender:
                done = true;
                break;
            case delimiter:
                result.push_back("");
                i = result.end();
                std::advance(i, -1);
                break;
            default:
                *i += *c;
                break;
            }
			c++;
        }
    }
    return result;
}

std::string serialise(const Answer &a)
{
	std::string os;
	os += starter;
    for ( Answer::const_iterator i = a.begin() ; a.end() != i ; ++i )
    {
        if ( i != a.begin() )
            os += delimiter;
        os += *i;
    }
    os += ender;
    return os;
}

#else

std::istream &operator>>(std::istream &is, Answer &result)
{
    result.clear();
    if ( is  && is.peek() == starter )
    {
        char c;
        // discard the [
        is.get(c);
        bool done = false;
        result.push_back ( "" );
        Answer::iterator i = result.begin();
        while ( is && !done )
        {
            is.get(c);
            switch ( c )
            {
            case ender:
                done = true;
                break;
            case delimiter:
                result.push_back("");
                i = result.end();
                std::advance(i, -1);
                break;
            default:
                *i += c;
                break;
            }
        }
    }
    return is;
}

std::ostream &operator<<(std::ostream &os, const Answer &a)
{
    os << starter;
    for ( Answer::const_iterator i = a.begin() ; a.end() != i ; ++i )
    {
        if ( i != a.begin() )
            os << delimiter;
        os << *i;
    }
    os << ender;
    return os;
}
#endif
