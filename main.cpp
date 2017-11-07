#include "continuation_fcontext.h"
#include <iostream>

int main()
{
    Continuation source = callcc(
	[](Continuation &&sink) {
    	    std::cout << "I'm in continuation" << std::endl;
    	    sink = sink.resume();
    	    std::cout << "I'm in continuation" << std::endl;
    	    return std::move(sink);
    	}
    );

    source = source.resume();
}
