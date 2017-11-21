#include "fcontext.h"
#include <cstdlib>
#include <stdexcept>
#include <cassert>
#include <iostream>

class SimpleStackAllocator
{
    public:
	static std::size_t const size = 128;

	void *allocate() const
	{
	    void *sp = std::malloc(size);
	    if (!sp)
	    {
		throw std::bad_alloc();
	    }
	    return static_cast<char*>(sp) + size;
	}

	void deallocate(void *sp, std::size_t size) const
	{
	    assert(sp != nullptr);
	    void *p = static_cast<char *>(sp) - size;
	    std::free(p);
	}
};

int value;

void f1(transfer_t t)
{
    value = 3;
    jump_fcontext( t.fctx, 0);
}

void f2(transfer_t t)
{
    std::cout << "f1: entered" << std::endl;
    SimpleStackAllocator alloc;
    void * sp = alloc.allocate();
    fcontext_t ctx = make_fcontext(sp, SimpleStackAllocator::size, f1);
    jump_fcontext(ctx, 0);
    jump_fcontext(t.fctx, 0);
}

void stacked()
{
    value = 0;
    SimpleStackAllocator alloc;
    void *sp = alloc.allocate();
    fcontext_t ctx = make_fcontext(sp, SimpleStackAllocator::size, f2);
    jump_fcontext(ctx, 0);
    assert(value == 3);
    alloc.deallocate(sp, SimpleStackAllocator::size);
}

int main()
{
    stacked();
}
