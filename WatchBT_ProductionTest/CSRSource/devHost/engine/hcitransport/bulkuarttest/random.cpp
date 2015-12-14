#include <cassert>

#include "random.h"

Random::Random(uint8 seed) :
    impl(new Impl(seed))
{
}

uint8 Random::operator()()
{
    assert(this);
    return impl->getNext();
}

Random::Impl::Impl(uint8 seed) :
    x(seed)
{
}

uint8 Random::Impl::getNext()
{
    assert(this);
    // Use Coveyou algorithm
    // Make sure interim calculations don't overflow x
    uint32 big_x = x;
    x = (uint8) ((big_x * (big_x + 1)) & 0xFF);
    return x;
}

/*uint32 random_iterator(uint32 seed)
{   
    return seed * (seed + 1);
} */  
