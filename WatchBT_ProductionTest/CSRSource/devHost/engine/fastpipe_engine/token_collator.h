/*
 * Copyright CSR 2008
 *
 * $Id: //depot/bc/bluesuite_2_4/devHost/engine/fastpipe_engine/token_collator.h#1 $
 *
 * Purpose: Inputs token fragments received, combining them with previously
 *          received token fragments. Outputs complete tokens received. 
 *
 */

#ifndef TOKEN_COLLATOR_H
#define TOKEN_COLLATOR_H

#include "common/types.h"
#include <deque>

class TokenCollator
{
public:
    void addFragment(const uint8 *fragment, size_t length);
    bool isTokenAvailable() const ;
    void getToken(uint8 &pipe_id, uint16 &credits) const;
    void removeFirstToken();
    
private:
    bool doIsTokenAvailable() const;
    std::deque<uint8> tokenFragments;
};

#endif
