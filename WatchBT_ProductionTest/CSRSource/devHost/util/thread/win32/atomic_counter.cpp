#include "thread/atomic_counter.h"

#include <windows.h>

class AtomicCounter::Impl
{
public:
	Impl(long init_val) : counter(init_val) {}

	LONG counter;
};

AtomicCounter::AtomicCounter(long init_val) : impl_(new Impl(init_val)) {}

AtomicCounter::AtomicCounter(const AtomicCounter &orig) : impl_(new Impl(orig.impl_->counter)) {}

AtomicCounter::~AtomicCounter()
{
	delete impl_;
}

long AtomicCounter::inc()
{
	return InterlockedIncrement(&impl_->counter);
}

long AtomicCounter::dec()
{
	return InterlockedDecrement(&impl_->counter);
}

void AtomicCounter::assign(long v)
{
	impl_->counter = v;
}

long AtomicCounter::read()
{
	return impl_->counter;
}
