#include "raw_checker.h"
#include "defines.h"
#include <algorithm>

RawChecker::RawChecker() :
    mreceive_random(RANDOM_SEED),
    msend_random(RANDOM_SEED),
    mbytes_received(0),
    mbytes_sent(0)
{
}

void RawChecker::onSend ( const uint8* const data, const uint32 length)
{
    uint8 expected[length];
    std::generate(&expected[0], &expected[length], msend_random);
    mbytes_sent += length;
    if (!std::equal(&expected[0], &expected[length], data))
    {
        printf("Data sent through the uart class incorrectly after %d bytes (packet end)\n");
    }
}

void RawChecker::onRecv ( const uint8* const data, const uint32 length)
{
    uint8 expected[length];
    std::generate(&expected[0], &expected[length], mreceive_random);
    mbytes_received += length;
    if (!std::equal(&expected[0], &expected[length], data))
    {
        printf("Data received incorrectly by uart class after %d bytes received (packet end)\n", mbytes_received);
    }
}

