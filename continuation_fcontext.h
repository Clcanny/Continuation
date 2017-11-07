#ifndef Continuation_FCONTEXT_H
#define Continuation_FCONTEXT_H

#include <cstddef>
#include <utility>
#include <type_traits>
#include <cassert>
#include <tuple>

#include <boost/context/detail/invoke.hpp>
#include <boost/context/detail/exchange.hpp>

#include "fcontext.h"

/* POD */
struct StackContext
{
    void *sp{ nullptr };
    std::size_t size{ 0 };
};

class StackAllocator
{
    private:
	std::size_t size;

    public:
	StackAllocator(std::size_t s = 100) :
	    size(s)
	{}

	StackContext allocate()
	{
	    void * vp = std::malloc(size);
            if (!vp) {
                throw std::bad_alloc();
            }
            StackContext sctx;
            sctx.size = size;
            sctx.sp = static_cast<char *>(vp) + sctx.size;
            return sctx;
	}

	void deallocate(StackContext &sctx) noexcept
	{
	    assert(sctx.sp != nullptr);
	    void *vp = static_cast<char *>(sctx.sp) - sctx.size;
            std::free(vp);
	}
};

struct forced_unwind
{
    fcontext_t fctx{ nullptr };

    forced_unwind() = default;

    forced_unwind(fcontext_t f) :
	fctx(f)
    {}
};

transfer_t context_unwind(transfer_t t);

class Continuation
{
    private:
	template <typename Fn >
	friend class ControlRecord;

	template <typename Fn >
    	friend transfer_t context_ontop(transfer_t);

    	template <typename Fn >
    	friend Continuation callcc(StackAllocator, Fn &&);

    private:
    	fcontext_t  fctx{ nullptr };

    private:
	Continuation(fcontext_t f) noexcept :
	    fctx{ f }
	{}

    public:
        Continuation() noexcept = default;
	Continuation(Continuation const& other) noexcept = delete;
	Continuation &operator=(Continuation const& other) noexcept = delete;

        Continuation(Continuation &&other) noexcept
	{
            std::swap(fctx, other.fctx);
        }
    
        Continuation & operator=(Continuation && other) noexcept
	{
            if (this != & other)
	    {
                Continuation tmp = std::move( other);
                swap(tmp);
            }
            return * this;
        }
    
        ~Continuation()
	{
	    if (fctx != nullptr)
	    {
		/* you shouldn't arrvie here */
		ontop_fcontext(
		    boost::context::detail::exchange(fctx, nullptr),
		    nullptr,
		    context_unwind
		);
	    }
	}   

	Continuation resume()
    	{
    	    assert(fctx != nullptr);
    	    return jump_fcontext(
    	    	boost::context::detail::exchange(fctx, nullptr),
    	    	nullptr).fctx;
    	}

	void swap(Continuation & other) noexcept
	{
    	    std::swap(fctx, other.fctx);
    	}
};

template <typename Fn>
class ControlRecord
{
    private:
	StackContext sctx;
	StackAllocator salloc;
	typename std::decay<Fn>::type fn;

    private:
	static void destory(ControlRecord *cr) noexcept
	{
	    StackAllocator salloc = cr->salloc;
	    StackContext sctx = cr->sctx;
	    /* deallocate record */
	    cr->~ControlRecord();
	    salloc.deallocate(sctx);
	}

    public:
	ControlRecord(StackContext s, StackAllocator const &sa, Fn &&f) noexcept :
	    sctx(s), salloc(sa), fn(std::forward<Fn>(f))
	{}

	ControlRecord(ControlRecord const &) = delete;
	ControlRecord &operator=(ControlRecord const &) = delete;

	void deallocate() noexcept
	{
	    destory(this);
	}

	fcontext_t run(fcontext_t fctx)
	{
	    Continuation continuation{ fctx };
	    continuation = boost::context::detail::invoke(fn, std::move(continuation));
	    return boost::context::detail::exchange(continuation.fctx, nullptr);
	}
};

transfer_t context_unwind(transfer_t t)
{
    throw forced_unwind(t.fctx);
    return { nullptr, nullptr };
}

template <typename Fn>
transfer_t context_exit(transfer_t t) noexcept
{
    ControlRecord<Fn> *cr = static_cast<ControlRecord<Fn> *>(t.data);
    /* destroy context stack */
    cr->deallocate();
    return { nullptr, nullptr };
}

template <typename Fn>
void context_entry(transfer_t t) noexcept
{
    assert(t.fctx != nullptr);
    assert(t.data != nullptr);

    /* transfer control structure to the context-stack */
    ControlRecord<Fn> *cr = static_cast<ControlRecord<Fn> *>(t.data);

    try
    {
	/* jump back to create_context() */
	t = jump_fcontext(t.fctx, nullptr);
	/* start executing */
	t.fctx = cr->run(t.fctx);
    }
    catch (forced_unwind const& e)
    {
	t = { e.fctx, nullptr };
    }
    assert(t.fctx != nullptr);
    ontop_fcontext(t.fctx, cr, &context_exit<Fn>);

    /* context already terminated */
    assert(false);
}

template <typename Fn>
transfer_t context_ontop(transfer_t t)
{
    auto p = static_cast<std::tuple<Fn> *>(t.data);
    assert(p != nullptr);

    typename std::decay<Fn>::type fn = std::get<0>(*p);

    t.data = nullptr;
    Continuation continuation{ t.fctx };
    /* execute function, pass Continuation via reference */
    continuation = fn(std::move(continuation));

    return { boost::context::detail::exchange(continuation.fctx, nullptr), nullptr };
}

template <typename Fn>
fcontext_t create_context(StackAllocator salloc, Fn &&fn)
{
    auto sctx = salloc.allocate();

    /* reserve space for control structure */
    void *storage = reinterpret_cast<void *>(
	(reinterpret_cast<uintptr_t>(sctx.sp) - static_cast<uintptr_t>(sizeof(ControlRecord<Fn>)))
	& ~static_cast<uintptr_t>(0xff)
    );

    /* placment new for control structure on context stack */
    ControlRecord<Fn> * record = new (storage) ControlRecord<Fn>{ sctx, salloc, std::forward<Fn>(fn) };

    /* 64byte gab between control structure and stack top */
    /* should be 16byte aligned */
    void *stack_top = reinterpret_cast<void *>(
	reinterpret_cast<uintptr_t>(storage) - static_cast<uintptr_t>(64)
    );
    void *stack_bottom = reinterpret_cast<void *>(
        reinterpret_cast<uintptr_t>(sctx.sp) - static_cast<uintptr_t>(sctx.size)
    );

    /* create fast-context */
    const std::size_t size = reinterpret_cast<uintptr_t>(stack_top) - reinterpret_cast<uintptr_t>(stack_bottom);
    const fcontext_t fctx = make_fcontext(stack_top, size, &context_entry<Fn>);
    assert(fctx != nullptr);

    /* transfer control structure to context-stack */
    return jump_fcontext(fctx, record).fctx;
}

template <typename Fn>
Continuation callcc(Fn &&fn)
{
    return callcc(StackAllocator(), std::forward<Fn>(fn));
}

template <typename Fn>
Continuation callcc(StackAllocator salloc, Fn &&fn)
{
    return Continuation { create_context<Fn>(salloc, std::forward<Fn>(fn)) }.resume();
}

#endif
