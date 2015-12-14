////////////////////////////////////////////////////////////////////////////////
//
//  FILE:        answer.h
//
//  CLASSES:     Answer
//
//  PURPOSE:     Define a class for describing answers to Questions (see
//               question.h)
//
////////////////////////////////////////////////////////////////////////////////

#ifndef UTIL_ANSWER_H
#define UTIL_ANSWER_H

#ifndef _WIN32_WCE
#include <iosfwd>
#endif
#include <string>
#include <list>

//  the answer is a list of keys.
typedef std::list<std::string> Answer;

#ifdef _WIN32_WCE
Answer deserialise(const std::string &);
std::string serialise(const Answer &);
#else
std::istream &operator>>(std::istream &, Answer &);
std::ostream &operator<<(std::ostream &, const Answer &);
#endif

#endif
