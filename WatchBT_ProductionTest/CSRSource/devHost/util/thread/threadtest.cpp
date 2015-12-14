// threadtest.cpp : Defines the entry point for the console application.
//

#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <stdio.h>

#include "thread.h"
#include "critical_section.h"

class TestThread : public Threadable
{
    const char* name;

    virtual int ThreadFunc();

    static CriticalSection critSec;

public:

    TestThread(const char* _name) : Threadable()
    {
        name = _name;
    }
};

CriticalSection TestThread::critSec;

int TestThread::ThreadFunc()
{
    for(int i=0; i<10; i++)
    {
        if(!KeepGoing()) return *name + ('A'-'a');

        printf("TestThread(%s) %i\n", name, i);
        if((i+*name)%4 == 3)
        {
            printf("TestThread(%s) %i LOCKING\n", name, i);
            {
                CriticalSection::Lock l1(critSec);
                printf("TestThread(%s) %i LOCKED\n", name, i);
                for(int j=0; j<10; j++)
                {
                    if(!KeepGoing())
                    break;

                    Sleep(100);
                }
            }
            printf("TestThread(%s) %i UN-LOCK\n", name, i);
            if(!KeepGoing()) return *name + ('a'-'A');
        }
        else
        {
            Sleep(200);
        }
    }
    return *name;
}

int main(int argc, char* argv[])
{
    TestThread a("A");
    TestThread b("B");
    TestThread c("C");

    printf("Hello World!\n");

    a.Start();
    b.Start();

    Sleep(600);

    c.Start();
    Sleep(2700);
    printf("TestThread(%s) KILL\n", "C");
    c.Kill(1500);

    while(a.IsActive() || b.IsActive())
    {
        //printf("Main Thread\n");
        Sleep(200);
    }

    printf("Threads A & B have ended! (%c,%c,%c)", a.GetExitCode(), b.GetExitCode(), c.GetExitCode());

    return 0;
}

