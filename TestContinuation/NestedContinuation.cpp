#include "Header.h"
#include <iostream>

int main()
{
    Continuation source_a = callcc(
	[](Continuation &&sink) {
	    sink = sink.resume();
	    std::cout << "Depth: 1" << std::endl;
	    Continuation source_b = callcc(
		[](Continuation &&sink) {
		    sink = sink.resume();
		    std::cout << "Depth: 2" << std::endl;
		    return std::move(sink);
		}
	    );
	    source_b = source_b.resume();
	    return std::move(sink);
	}
    );
    source_a = source_a.resume();

}
