#include "fcontext.h"

class SimpleStackAllocator

void stacked()
{
    int value1 = 0;
    StackAllocator sa;
    void *sp = sa.allocate();
}
