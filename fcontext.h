#ifndef FCONTEXT_H
#define FCONTEXT_H

#include <cstddef>

typedef void* fcontext_t;

struct transfer_t
{
    fcontext_t fctx;
    void *data;
};

extern "C" transfer_t
jump_fcontext(fcontext_t const to, void *vp);

extern "C" fcontext_t
make_fcontext(void *sp, std::size_t size, void (* fn)(transfer_t));

extern "C" transfer_t
ontop_fcontext(fcontext_t const to, void *vp, transfer_t (* fn)(transfer_t));

#endif
