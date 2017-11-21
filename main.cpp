#include "continuation_fcontext.h"
#include <iostream>

int main()
{
    Continuation source_a = callcc(
	[](Continuation &&sink) {
    	    std::cout << "I'm in continuation" << std::endl;
    	    sink = sink.resume();
    	    std::cout << "I'm in continuation" << std::endl;
    	    return std::move(sink);
    	}
    );

    Continuation source_b = callcc(
	[](Continuation &&sink) {
	    sink = sink.resume();
	    std::cout << "I'm in another continuation" << std::endl;
	    sink = sink.resume();
	    std::cout << "I'm in another continuation" << std::endl;
	    return std::move(sink);
	}
    );

    source_a = source_a.resume();
    source_b = source_b.resume();
    source_b = source_b.resume();

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
