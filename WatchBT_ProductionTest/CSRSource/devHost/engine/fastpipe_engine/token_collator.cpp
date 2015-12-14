#include "token_collator.h"
#include "token_util.h"
#include <algorithm>
#include <cassert>

void TokenCollator::addFragment(const uint8 * const fragment, const size_t length)
{
    assert(this);
    assert(fragment);
    std::copy(fragment, fragment + length, std::back_inserter(tokenFragments));
}

bool TokenCollator::doIsTokenAvailable() const
{
    return tokenFragments.size() >= TOKEN_SIZE;
}

bool TokenCollator::isTokenAvailable() const
{
    assert(this);
    return doIsTokenAvailable();
}

void TokenCollator::getToken(uint8 &pipe_id, uint16 &credits) const
{
    assert(this);
    assert(doIsTokenAvailable());
    pipe_id = tokenFragments[PIPE_OFFSET];
    credits = composeCredits(tokenFragments[CREDIT_OFFSET],
                             tokenFragments[CREDIT_OFFSET + 1]);
}

void TokenCollator::removeFirstToken()
{
    assert(this);
    assert(doIsTokenAvailable());
    tokenFragments.erase(tokenFragments.begin(), tokenFragments.begin() + TOKEN_SIZE);
}
