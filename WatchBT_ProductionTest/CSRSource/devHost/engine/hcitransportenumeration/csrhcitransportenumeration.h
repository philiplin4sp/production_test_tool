// Enumerate available transports

#ifndef CSRHCITRANSPORTENUMERATION_H
#define CSRHCITRANSPORTENUMERATION_H

#include "question/question.h"
#include "question/answer.h"
#include <utility>
#include <vector>
#include "csrhci/transportconfiguration.h"

//  some people might like a "none" option for their transport.  but not by default.
Question enumerateTransports( bool allow_none = false ); // Question really is a pointer
//  this will enumerate as we go.
extern const Question whatTransport;

std::pair<bool,TransportConfigurationPtr> createTransportConfig ( const Answer &path );

#endif
