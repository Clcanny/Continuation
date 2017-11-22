#include "Header.h"
#include <iostream>

int main()
{
    int a = 0;
    Continuation source = callcc(
	[&](Continuation &&sink) {
	    int b = 1;
	    while (true)
	    {
		sink = sink.resume();
		int next = a + b;
		a = b;
		b = next;
	    }
	    return std::move(sink);
	}
    );

    for (int i = 0; i < 10; i++)
    {
	std::cout << a << std::endl;
	source = source.resume();
    }
}
