#include "critical_section.h"

struct critical_section
{
    CriticalSection cs;
    explicit critical_section(bool recursive) : cs(recursive) {}
};

struct critical_section *create_critical_section()
{
    return new critical_section(false);
}

struct critical_section *create_recursive_critical_section()
{
    return new critical_section(true);
}

void destroy_critical_section(struct critical_section *cs)
{
    delete cs;
}

void lock_critical_section(struct critical_section *cs)
{
    cs->cs.Enter();
}

void unlock_critical_section(struct critical_section *cs)
{
    cs->cs.Leave();
}
