#include "continuation_fcontext.h"
#include <iostream>

int main()
{
    Continuation source_a = callcc(
	[&](Continuation &&sink) {
	    sink = sink.resume();
	    std::cout << "I'm in continuation a" << std::endl;
	}
    );

    Continuation source_b = callcc(
	[&](Continuation &&sink) {
	    source_a = std::move(source_a);
	}
    );
}
