#ifndef RANDOM_H
#define RANDOM_H

#include "common/types.h"
#include "common/countedpointer.h"

class Random
{
public:
    Random(uint8 seed);
    class Impl : public Counter
    {
    public:
        Impl(uint8 seed);
        uint8 getNext();
    private:
        uint8 x;
    };

    uint8 operator()();
private:
    CountedPointer<Impl> impl;
};


//uint32 random_iterator(uint32 seed);

#endif
