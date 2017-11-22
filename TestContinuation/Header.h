#ifndef HEADER_HPP
#define HEADER_HPP

/* compile without boost */
#ifdef USE_BOOST
#include "continuation_fcontext.h"
/* compile with boost */
#else
#include <boost/context/all.hpp>
#include <boost/thread/thread.hpp>
typedef boost::context::continuation Continuation;
#define callcc boost::context::callcc
boost::thread_group tg;
#endif

#endif
