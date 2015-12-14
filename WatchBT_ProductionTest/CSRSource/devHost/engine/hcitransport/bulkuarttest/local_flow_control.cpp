#include <cstdio>
#include <cassert>

#include "local_flow_control.h"

#define MOSUL_BUFFER_SIZE 16000

LocalFlowControl::LocalFlowControl() :
    credits(MOSUL_BUFFER_SIZE)
{
}

void LocalFlowControl::doAddCredits(const credit_t credits)
{
    assert(this);
    CriticalSection::Lock lock(cs);
    this->credits += credits;
    if (this->credits >= 200) signal.set();
}

credit_t LocalFlowControl::doUseCredits()
{
    assert(this);
    CriticalSection::Lock lock(cs);
    while (credits < 200)
    {
         CriticalSection::AntiLock al(cs); 
         signal.wait(1000);
    }

    signal.unset();
    const credit_t c(200); 
    this->credits -= 200; 
    return c;
}

