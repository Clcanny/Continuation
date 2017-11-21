#include "continuation_fcontext.h"
#include <iostream>

int main()
{
    Continuation source_c = callcc(
	    [](Continuation &&sink) {
	    sink = sink.resume();
	    std::cout << "Depth: 1" << std::endl;
	    Continuation source_d = callcc(
		    [](Continuation &&sink) {
			sink = sink.resume();
			std::cout << "Depth: 2" << std::endl;
			return std::move(sink);
		}
	    );
	    source_d = source_d.resume();
	    return std::move(sink);
	}
    );
    source_c = source_c.resume();
}
